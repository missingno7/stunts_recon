"""16-bit OMF reader extracted from PortForge pf_match.py. See docs/upstream-inventory.md."""
from __future__ import annotations
import struct
from pathlib import Path

class MatchError(ValueError):
    pass


class ObjectModule:
    """One compiled unit, as much of it as a byte comparison needs.

    `segments` maps a segment name to its bytes. `publics` and `fixups` carry
    the segment name too, so a caller never has to know the format's index
    numbering. `segment_lengths` maps a segment name to its SEGDEF-declared
    length -- present for a segment with no LEDATA/LIDATA at all (`_BSS`,
    always; any segment this module contributes to but never initializes),
    which `segments` has no entry for and `segment_bytes` would refuse.
    """

    def __init__(self, segments: dict, publics: list, fixups: list,
                 externals: list, name: str = "",
                 segment_lengths: dict | None = None,
                 segment_defs: list | None = None, groups: list | None = None,
                 comments: list | None = None) -> None:
        self.name = name
        self.segments = segments
        self.publics = publics      # [{"name", "segment", "offset"}]
        self.fixups = fixups        # legacy binding view
        self.linker_fixups = []      # full ordered frame/target/addend view
        self.externals = externals  # [name]
        self.segment_lengths = dict(segment_lengths or {})
        # Full linker-facing declarations.  The older maps above remain for
        # callers that only need byte/fixup comparison.
        self.segment_defs = list(segment_defs or [])
        self.groups = list(groups or [])
        self.comments = list(comments or [])

    def segment_bytes(self, segment: str) -> bytes:
        if segment not in self.segments:
            raise MatchError(
                f"the object has no segment {segment!r}; it has "
                f"{', '.join(sorted(self.segments)) or '(none)'}")
        return bytes(self.segments[segment])

    def segment_length(self, segment: str) -> int | None:
        """The SEGDEF-declared length of `segment`, or `None` when this
        module declares no SEGDEF of that name at all."""
        return self.segment_lengths.get(segment)

    def publics_in(self, segment: str) -> list:
        return sorted((p for p in self.publics if p["segment"] == segment),
                      key=lambda p: p["offset"])

    def fixups_in(self, segment: str) -> list:
        return sorted((f for f in self.fixups if f["segment"] == segment),
                      key=lambda f: f["offset"])


class ObjectReader:
    """Base class. A new object format implements `read` and registers below."""

    name = ""

    def read(self, data: bytes, label: str = "") -> ObjectModule:
        raise NotImplementedError

    def read_file(self, path: Path) -> ObjectModule:
        return self.read(path.read_bytes(), path.name)

    def split_library(self, data: bytes) -> list:
        """[(module name, module bytes)] -- only formats that have libraries."""
        raise MatchError(f"the {self.name} reader has no library format")


class OmfReader(ObjectReader):
    """Intel OMF as Borland's tools write it: LEDATA, FIXUPP, PUBDEF, SEGDEF.

    Only the 16-bit record variants are read. A 32-bit variant (the odd record
    type) is REFUSED by name rather than skipped: skipping it would drop data
    or fixups out of a comparison that would then agree about bytes nobody
    looked at.
    """

    name = "omf"

    THEADR, LHEADR = 0x80, 0x82
    COMENT = 0x88
    MODEND16, MODEND32 = 0x8A, 0x8B
    EXTDEF = 0x8C
    PUBDEF16, PUBDEF32 = 0x90, 0x91
    LINNUM16, LINNUM32 = 0x94, 0x95
    LNAMES = 0x96
    SEGDEF16, SEGDEF32 = 0x98, 0x99
    GRPDEF = 0x9A
    FIXUPP16, FIXUPP32 = 0x9C, 0x9D
    LEDATA16, LEDATA32 = 0xA0, 0xA1
    LIDATA16, LIDATA32 = 0xA2, 0xA3
    COMDEF = 0xB0
    LEXTDEF = 0xB4
    LPUBDEF = 0xB6

    #: FIXUPP LOC field -> the width in bytes of the location it patches.
    LOC_WIDTH = {0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2, 9: 4, 11: 6, 13: 4}
    LOC_NAME = {0: "lobyte", 1: "offset16", 2: "base16", 3: "pointer32",
                4: "hibyte", 5: "loader-offset16", 9: "offset32",
                11: "pointer48", 13: "loader-offset32"}
    ALIGNMENT_NAMES = {
        0: "absolute", 1: "byte", 2: "word", 3: "paragraph",
        4: "page", 5: "dword", 6: "unknown6", 7: "unknown7",
    }
    COMBINE_NAMES = {
        0: "private", 1: "reserved1", 2: "public", 3: "reserved3",
        4: "public", 5: "stack", 6: "common", 7: "public",
    }

    REFUSED_32 = {MODEND32, PUBDEF32, SEGDEF32, FIXUPP32, LEDATA32, LIDATA32,
                  LINNUM32}

    @staticmethod
    def records(buf: bytes):
        index = 0
        while index + 3 <= len(buf):
            kind = buf[index]
            length = struct.unpack_from("<H", buf, index + 1)[0]
            body = buf[index + 3:index + 3 + length - 1]  # last byte: checksum
            yield kind, body
            index += 3 + length

    @staticmethod
    def _index(body: bytes, at: int) -> tuple:
        """An OMF index: one byte, or two with the high bit set on the first."""
        if at >= len(body):
            return 0, at + 1
        if body[at] < 0x80:
            return body[at], at + 1
        return ((body[at] & 0x7F) << 8) | body[at + 1], at + 2

    def _expand_iterated_block(self, body: bytes, at: int) -> tuple:
        """One Iterated Data Block of a LIDATA16 record: `(bytes, next_at)`.

        `repeat count` (2 bytes) then `block count` (2 bytes). Block count 0
        means this is a leaf: a 1-byte content length, then that many literal
        bytes, repeated `repeat count` times. A nonzero block count instead
        means `block count` NESTED iterated data blocks follow, each parsed
        the same way; their concatenation is what repeats `repeat count`
        times. Recursive by the format's own definition (TIS OMF, LIDATA).
        """
        repeat_count = struct.unpack_from("<H", body, at)[0]
        at += 2
        block_count = struct.unpack_from("<H", body, at)[0]
        at += 2
        if block_count == 0:
            content_length = body[at]
            at += 1
            content = body[at:at + content_length]
            at += content_length
            return content * repeat_count, at
        unit = bytearray()
        for _ in range(block_count):
            nested, at = self._expand_iterated_block(body, at)
            unit.extend(nested)
        return bytes(unit) * repeat_count, at

    def read(self, data: bytes, label: str = "") -> ObjectModule:
        lnames: list = []
        segment_names: list = []      # 1-based SEGDEF index -> name
        segment_lengths: list = []    # 1-based SEGDEF index -> declared length
        group_names: list = []
        segment_defs: list = []       # linker-facing SEGDEF records
        groups: list = []             # linker-facing GRPDEF records
        externals: list = []
        publics: list = []
        segment_data: dict = {}       # SEGDEF index -> bytearray
        fixups: list = []
        comments: list = []
        module_name = label
        last_segment, last_offset = None, 0
        frame_threads: dict = {}
        target_threads: dict = {}

        for kind, body in self.records(data):
            if kind in self.REFUSED_32:
                raise MatchError(
                    f"OMF record 0x{kind:02X} is a 32-bit variant; this reader "
                    "implements the 16-bit records only, and skipping it would "
                    "drop bytes or fixups out of the comparison")
            if kind in (self.THEADR, self.LHEADR):
                length = body[0] if body else 0
                module_name = body[1:1 + length].decode("latin1")
            elif kind == self.COMENT:
                # Preserve both bytes and the two OMF classification bytes.
                # Default-library and linker directives are version-specific;
                # callers can classify them without the reader discarding data.
                attribute = body[0] if body else None
                comment_class = body[1] if len(body) > 1 else None
                payload = body[2:] if len(body) > 2 else b""
                comments.append({"attribute": attribute,
                                 "class": comment_class,
                                 "data": payload,
                                 "data_hex": payload.hex()})
            elif kind == self.LNAMES:
                at = 0
                while at < len(body):
                    length = body[at]
                    lnames.append(body[at + 1:at + 1 + length].decode("latin1"))
                    at += 1 + length
            elif kind == self.SEGDEF16:
                acbp = body[0]
                at = 1
                alignment_code = (acbp >> 5) & 7
                combine_code = (acbp >> 2) & 7
                big = bool(acbp & 2)
                use_32bit_offset = bool(acbp & 1)
                frame = offset = None
                if (acbp >> 5) == 0:          # absolute segment: frame + offset
                    frame = struct.unpack_from("<H", body, at)[0]
                    offset = body[at + 2]
                    at += 3
                seg_length = struct.unpack_from("<H", body, at)[0]
                at += 2                        # segment length
                name_index, at = self._index(body, at)
                class_index, at = self._index(body, at)
                overlay_index, at = self._index(body, at)
                segment_name = (lnames[name_index - 1]
                                if 0 < name_index <= len(lnames)
                                else f"?{name_index}")
                class_name = (lnames[class_index - 1]
                              if 0 < class_index <= len(lnames)
                              else f"?{class_index}")
                segment_defs.append({
                    "index": len(segment_names) + 1,
                    "name": segment_name,
                    "class": class_name,
                    "length": seg_length,
                    "alignment_code": alignment_code,
                    "alignment": self.ALIGNMENT_NAMES[alignment_code],
                    "combine_code": combine_code,
                    "combine": self.COMBINE_NAMES[combine_code],
                    "big": big,
                    "use_32bit_offset": use_32bit_offset,
                    "frame": frame,
                    "offset": offset,
                    "overlay_index": overlay_index,
                    "acbp": acbp,
                })
                segment_names.append(segment_name)
                segment_lengths.append(seg_length)
            elif kind == self.GRPDEF:
                name_index, at = self._index(body, 0)
                group_name = (lnames[name_index - 1]
                              if 0 < name_index <= len(lnames)
                              else f"?{name_index}")
                segment_indices = []
                while at < len(body):
                    if body[at] != 0xFF:
                        raise MatchError(f"Unsupported GRPDEF component 0x{body[at]:02X}")
                    segment_index, at = self._index(body, at + 1)
                    segment_indices.append(segment_index)
                group_names.append(group_name)
                groups.append({"index": len(group_names), "name": group_name,
                               "segment_indices": segment_indices})
            elif kind in (self.EXTDEF, self.LEXTDEF):
                at = 0
                while at < len(body):
                    length = body[at]
                    externals.append(body[at + 1:at + 1 + length].decode("latin1"))
                    at += 1 + length
                    _, at = self._index(body, at)   # type index
            elif kind in (self.PUBDEF16, self.LPUBDEF):
                at = 0
                group_index, at = self._index(body, at)
                segment_index, at = self._index(body, at)
                if group_index == 0 and segment_index == 0:
                    at += 2                     # explicit frame number
                while at < len(body):
                    length = body[at]
                    name = body[at + 1:at + 1 + length].decode("latin1")
                    at += 1 + length
                    offset = struct.unpack_from("<H", body, at)[0]
                    at += 2
                    _, at = self._index(body, at)  # type index
                    publics.append({"name": name, "segment_index": segment_index,
                                    "offset": offset})
            elif kind == self.LEDATA16:
                segment_index, at = self._index(body, 0)
                offset = struct.unpack_from("<H", body, at)[0]
                payload = body[at + 2:]
                store = segment_data.setdefault(segment_index, bytearray())
                if len(store) < offset + len(payload):
                    store.extend(b"\x00" * (offset + len(payload) - len(store)))
                store[offset:offset + len(payload)] = payload
                last_segment, last_offset = segment_index, offset
            elif kind == self.LIDATA16:
                # An Iterated Data record: one or more Iterated Data Blocks,
                # each `(repeat count, block count, content)` -- a leaf
                # (block count 0) repeats its own CONTENT bytes; a non-leaf
                # repeats the CONCATENATION of its `block count` nested
                # blocks (the TIS OMF spec's own recursive definition, and
                # what TASM's .MODEL macros emit for a zero-filled or
                # repeated-word table inside a compared CODE segment --
                # DSMI's own tables, found via aladdin_forged 2026-09-07).
                # Expanded in full: a comparison that skipped it would
                # compare zeros where the object holds real repeated bytes.
                segment_index, at = self._index(body, 0)
                offset = struct.unpack_from("<H", body, at)[0]
                at += 2
                payload = bytearray()
                while at < len(body):
                    chunk, at = self._expand_iterated_block(body, at)
                    payload.extend(chunk)
                payload = bytes(payload)
                store = segment_data.setdefault(segment_index, bytearray())
                if len(store) < offset + len(payload):
                    store.extend(b"\x00" * (offset + len(payload) - len(store)))
                store[offset:offset + len(payload)] = payload
                last_segment, last_offset = segment_index, offset
            elif kind == self.FIXUPP16:
                at = 0
                while at < len(body):
                    if body[at] & 0x80:
                        locat = (body[at] << 8) | body[at + 1]
                        at += 2
                        loc = (locat >> 10) & 0xF
                        offset = locat & 0x3FF
                        self_relative = not ((body[at - 2] >> 6) & 1)
                        fixdat = body[at]
                        at += 1
                        frame_bit = fixdat >> 7
                        frame_field = (fixdat >> 4) & 7
                        target_bit = (fixdat >> 3) & 1
                        no_displacement = (fixdat >> 2) & 1
                        target_field = fixdat & 3
                        if frame_bit:
                            frame_method, frame_index = frame_threads.get(
                                frame_field & 3, (None, 0))
                        else:
                            frame_method = frame_field
                            frame_index = 0
                            if frame_method in (0, 1, 2):
                                frame_index, at = self._index(body, at)
                            elif frame_method == 3:
                                frame_index = struct.unpack_from("<H", body, at)[0]
                                at += 2
                        if target_bit:
                            thread = target_threads.get(target_field & 3,
                                                        (None, 0))
                            target_method, target_index = thread
                        else:
                            target_method = target_field
                            target_index, at = self._index(body, at)
                        displacement = 0
                        if not no_displacement:
                            displacement = struct.unpack_from("<H", body, at)[0]
                            at += 2
                        if last_segment is None:
                            raise MatchError(
                                "a FIXUPP appears before any LEDATA: the "
                                "position it patches cannot be located")
                        fixups.append({
                            "segment_index": last_segment,
                            "offset": last_offset + offset,
                            "loc": loc,
                            "loc_name": self.LOC_NAME.get(loc, f"loc{loc}"),
                            "width": self.LOC_WIDTH.get(loc, 2),
                            "self_relative": bool(self_relative),
                            "target_method": target_method,
                            "target_index": target_index,
                            "target_displacement": displacement,
                            "frame_method": frame_method,
                            "frame_index": frame_index,
                        })
                    else:
                        thread = body[at]
                        at += 1
                        is_frame = (thread >> 6) & 1
                        method = (thread >> 2) & 7
                        number = thread & 3
                        datum = 0
                        if not (is_frame and method in (4, 5, 6)):
                            datum, at = self._index(body, at)
                        if is_frame:
                            frame_threads[number] = (method, datum)
                        else:
                            target_threads[number] = (method, datum)

        def segment_name(index: int) -> str:
            return (segment_names[index - 1] if 0 < index <= len(segment_names)
                    else f"?{index}")

        def target_of(fixup: dict) -> dict:
            method = fixup["target_method"]
            index = fixup["target_index"]
            if method in (0, 4):
                return {"kind": "segment", "name": segment_name(index)}
            if method in (1, 5):
                return {"kind": "group",
                        "name": (group_names[index - 1]
                                 if 0 < index <= len(group_names)
                                 else f"?{index}")}
            if method in (2, 6):
                return {"kind": "external",
                        "name": (externals[index - 1]
                                 if 0 < index <= len(externals)
                                 else f"?{index}")}
            return {"kind": "absolute", "name": f"frame:{index}"}

        segments = {segment_name(index): bytes(payload)
                    for index, payload in segment_data.items()}
        # SEGDEF order, so a later SEGDEF of a name already seen (a segment a
        # module contributes to more than once) is the one that answers --
        # the same "last one wins" rule `segments` above already has, from
        # `segment_data`'s own insertion order.
        out_segment_lengths = {segment_names[index]: length
                               for index, length in enumerate(segment_lengths)}
        out_publics = [{"name": item["name"],
                        "segment": segment_name(item["segment_index"]),
                        "offset": item["offset"]} for item in publics]
        out_fixups = []
        for fixup in fixups:
            target = target_of(fixup)
            out_fixups.append({
                "segment": segment_name(fixup["segment_index"]),
                "offset": fixup["offset"],
                "width": fixup["width"],
                "loc": fixup["loc_name"],
                "self_relative": fixup["self_relative"],
                "target_kind": target["kind"],
                "target": target["name"],
                "displacement": fixup["target_displacement"],
            })
        for group in groups:
            group["segments"] = [segment_name(i) for i in group["segment_indices"]]
        result = ObjectModule(segments, out_publics, out_fixups, externals,
                            module_name, segment_lengths=out_segment_lengths,
                            segment_defs=segment_defs, groups=groups,
                            comments=comments)
        for raw, normalized in zip(fixups, out_fixups):
            method, index = raw['frame_method'], raw['frame_index']
            if method in (0, 1, 2, 3):
                frame = target_of({'target_method': method, 'target_index': index})
            elif method == 4:
                frame = {'kind': 'location', 'name': normalized['segment']}
            elif method == 5:
                frame = {'kind': 'target', 'name': normalized['target']}
            elif method == 6:
                frame = {'kind': 'none', 'name': None}
            else:
                raise MatchError('Undefined FIXUPP frame thread/method')
            payload = segments.get(normalized['segment'], b'')
            at, width = normalized['offset'], normalized['width']
            result.linker_fixups.append({**normalized, 'frame_method': method,
                'frame_index': index, 'target_method': raw['target_method'],
                'target_index': raw['target_index'],
                'frame_kind': frame['kind'], 'frame': frame['name'],
                'encoded_addend': payload[at:at+width].hex()})
        return result


    def split_library(self, data: bytes) -> list:
        """An OMF library (0xF0) into its modules, in page order."""
        if not data or data[0] != 0xF0:
            raise MatchError("not an OMF library: the first record is not 0xF0")
        page = struct.unpack_from("<H", data, 1)[0] + 3
        at = page
        modules: list = []
        while at < len(data) and data[at] == self.THEADR:
            start = at
            name = ""
            while at < len(data):
                kind = data[at]
                length = struct.unpack_from("<H", data, at + 1)[0]
                if kind == self.THEADR:
                    size = data[at + 3]
                    name = data[at + 4:at + 4 + size].decode("latin1")
                at += 3 + length
                if kind in (self.MODEND16, self.MODEND32):
                    break
            modules.append((name, data[start:at]))
            at = ((at + page - 1) // page) * page
        return modules

