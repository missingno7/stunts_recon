"""Opt-in, research-only inspection of objects rejected by strict object_probe."""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

def _project_root() -> Path:
    for parent in Path(__file__).resolve().parents:
        if (parent / "tools/common.py").is_file():
            return parent
    raise RuntimeError("Cannot locate repository tools/common.py")


ROOT = _project_root()
sys.path.insert(0, str(ROOT / "tools"))
from common import identity, json_bytes, read_json, sha, write_json
from diagnostics import diagnose
from object_probe import read_object as read_strict_object
from omf import OmfReader


# Only records the existing generic reader explicitly handles are admitted.
SUPPORTED = {
    0x80: "THEADR", 0x82: "LHEADR", 0x88: "COMENT", 0x96: "LNAMES",
    0x98: "SEGDEF16", 0x9A: "GRPDEF", 0x8C: "EXTDEF", 0xB4: "LEXTDEF",
    0x90: "PUBDEF16", 0xB6: "LPUBDEF", 0xA0: "LEDATA16",
    0xA2: "LIDATA16", 0x9C: "FIXUPP16", 0x8A: "MODEND16",
}
STATIC_SYMBOLS = {0xB4, 0xB6}
SYMBOL_RECORDS = {
    0x8C: "global external symbols (EXTDEF)",
    0xB4: "local external symbols (LEXTDEF)",
    0x90: "global public symbols (PUBDEF16)",
    0xB6: "local public symbols (LPUBDEF)",
}


def _index(body: bytes, at: int) -> tuple[int, int]:
    if at >= len(body):
        raise ValueError("Truncated OMF index")
    if body[at] < 0x80:
        return body[at], at + 1
    if at + 2 > len(body):
        raise ValueError("Truncated two-byte OMF index")
    return ((body[at] & 0x7F) << 8) | body[at + 1], at + 2


def _symbol_rows(kind: int, body: bytes) -> list[dict]:
    """Validate and label symbol-record visibility without merging record classes."""
    rows = []
    at = 0
    if kind in (0x8C, 0xB4):
        while at < len(body):
            size = body[at]
            at += 1
            if at + size > len(body):
                raise ValueError("Truncated OMF external symbol name")
            name = body[at:at + size].decode("latin1")
            at += size
            type_index, at = _index(body, at)
            rows.append({"name": name, "type_index": type_index})
        return rows

    group_index, at = _index(body, at)
    segment_index, at = _index(body, at)
    frame = None
    if group_index == 0 and segment_index == 0:
        if at + 2 > len(body):
            raise ValueError("Truncated OMF PUBDEF frame")
        frame = struct.unpack_from("<H", body, at)[0]
        at += 2
    while at < len(body):
        size = body[at]
        at += 1
        if at + size + 2 > len(body):
            raise ValueError("Truncated OMF public symbol")
        name = body[at:at + size].decode("latin1")
        at += size
        offset = struct.unpack_from("<H", body, at)[0]
        at += 2
        type_index, at = _index(body, at)
        rows.append({"name": name, "offset": offset, "type_index": type_index,
                     "group_index": group_index, "segment_index": segment_index,
                     "frame": frame})
    return rows


def inventory_records(data: bytes) -> list[dict]:
    """Validate framing/checksums and preserve every interpreted record."""
    rows = []
    at = 0
    ended = False
    while at < len(data):
        if ended or at + 3 > len(data):
            raise ValueError("Truncated data or bytes after MODEND")
        kind = data[at]
        length = struct.unpack_from("<H", data, at + 1)[0]
        end = at + 3 + length
        if length < 1 or end > len(data):
            raise ValueError(f"Malformed OMF record header at offset {at}")
        if kind not in SUPPORTED:
            raise ValueError(f"Unsupported OMF record 0x{kind:02X}; inspection will not skip records")
        record = data[at:end]
        if record[-1] != 0 and sum(record) & 0xFF:
            raise ValueError(f"OMF checksum mismatch at offset {at}")
        body = data[at + 3:end - 1]
        if not rows and kind not in (0x80, 0x82):
            raise ValueError("OMF module must begin with THEADR or LHEADR")
        if rows and kind in (0x80, 0x82):
            raise ValueError("Multiple OMF module headers are outside inspection scope")
        visibility = SYMBOL_RECORDS.get(kind)
        symbols = _symbol_rows(kind, body) if kind in SYMBOL_RECORDS else None
        checksum_status = "omitted_zero" if record[-1] == 0 else "checked_valid"
        row = {
            "offset": at, "type": f"0x{kind:02x}", "name": SUPPORTED[kind],
            "length": length, "record_sha256": sha(record),
            "body_sha256": sha(body), "checksum_status": checksum_status,
            "body_hex": body.hex() if visibility else None,
            "symbol_visibility_evidence": visibility,
            "symbols": symbols,
        }
        if kind == 0xA2:
            raise ValueError("LIDATA coverage is outside this inspector's validated subset")
        if kind == 0xA0:
            index, pos = _index(body, 0)
            if pos + 2 > len(body):
                raise ValueError("Truncated LEDATA offset")
            row.update(segment_index=index,
                       data_offset=struct.unpack_from("<H", body, pos)[0],
                       data_size=len(body) - pos - 2)
        rows.append(row)
        ended = kind == 0x8A
        at = end
    if not ended:
        raise ValueError("Missing OMF MODEND")
    return rows


def inspect_bytes(data: bytes, target: bytes, evidence: dict, output_dir: Path,
                  segment: str = "UNIT_TEXT") -> dict:
    output_dir = Path(output_dir).resolve()
    private_root = (ROOT / "build/private").resolve()
    if not output_dir.is_relative_to(private_root):
        raise ValueError("Output directory must be under build/private")
    output_dir.mkdir(parents=True, exist_ok=True)

    rows = inventory_records(data)
    try:
        read_strict_object(data)
    except ValueError as error:
        strict_error = str(error)
    else:
        raise ValueError("Strict object_probe accepted this object; this tool only inspects rejected objects")
    if not any(int(row["type"], 16) in STATIC_SYMBOLS for row in rows):
        raise ValueError("Strict-reader rejection is not accompanied by B4/B6 static-symbol records")
    if "unsupported OMF record" not in strict_error or not any(
            f"{int(row['type'], 16):02x}" in strict_error.lower()
            for row in rows if int(row["type"], 16) in STATIC_SYMBOLS):
        raise ValueError(f"Strict-reader rejection is outside the B4/B6 inspection scope: {strict_error}")

    obj = OmfReader().read(data, label="research-only")
    segment_names = [item["name"] for item in obj.segment_defs]
    if len(segment_names) != len(set(segment_names)):
        raise ValueError("Duplicate SEGDEF names are outside the complete-inspection scope")
    definitions = {item["index"]: item for item in obj.segment_defs}
    intervals = {}
    for row in rows:
        if row["name"] != "LEDATA16":
            continue
        seg_index = row["segment_index"]
        if seg_index not in definitions:
            raise ValueError(f"LEDATA references undefined SEGDEF index {seg_index}")
        limit = definitions[seg_index]["length"]
        start, end = row["data_offset"], row["data_offset"] + row["data_size"]
        if end > limit:
            raise ValueError(f"LEDATA range {start}:{end} exceeds SEGDEF extent {limit}")
        intervals.setdefault(seg_index, []).append((start, end))
    for seg_index, spans in intervals.items():
        ordered = sorted(spans)
        if any(left[1] > right[0] for left, right in zip(ordered, ordered[1:])):
            raise ValueError(f"Overlapping LEDATA ranges in SEGDEF index {seg_index}")
    if segment not in obj.segment_lengths or segment not in obj.segments:
        raise ValueError(f"Requested segment {segment!r} is not fully represented by the generic reader")
    selected = next((item for item in obj.segment_defs if item["name"] == segment), None)
    if selected is None or obj.segment_length(segment) != len(obj.segments[segment]):
        raise ValueError("Selected segment payload does not equal its declared SEGDEF extent")
    spans = sorted(intervals.get(selected["index"], []))
    cursor = 0
    for start, end in spans:
        if start != cursor:
            raise ValueError("Selected segment has a hole or overlapping LEDATA coverage")
        cursor = end
    if cursor != selected["length"]:
        raise ValueError("Selected segment is not fully covered by LEDATA")
    comments = [{key: (value.hex() if isinstance(value, bytes) else value)
                 for key, value in item.items()} for item in obj.comments]
    code = obj.segment_bytes(segment)
    diag_receipt = dict(evidence)
    diag_receipt["work_directory"] = str(output_dir)
    diagnostic = diagnose(target, code, diag_receipt, obj.linker_fixups, segment=segment)
    return {
        "authority": "RESEARCH_ONLY",
        "acceptance_status": "NOT_EVALUATED",
        "promotion_status": "NOT_PROMOTED",
        "bindable_payload_available": False,
        "inspection_scope": "Record framing/checksums, supported record inventory, symbol visibility evidence, and complete initialized selected-segment coverage; not a general OMF semantic validator or linker.",
        "strict_object_probe": {"status": "REJECTED", "error": strict_error},
        "raw_object": identity(data),
        "evidence_identity": identity(json_bytes(evidence)),
        "record_inventory": rows,
        "module_name": obj.name,
        "segment_definitions": obj.segment_defs,
        "segment_lengths": obj.segment_lengths,
        "segments": {name: identity(payload) for name, payload in obj.segments.items()},
        "externals": obj.externals,
        "publics": obj.publics,
        "symbol_views_note": "Generic publics/externals are flattened; record_inventory preserves record type and visibility.",
        "groups": obj.groups,
        "comments": comments,
        "fixups": obj.linker_fixups,
        "target": identity(target),
        "inspected_segment": segment,
        "complete_contribution": identity(code),
        "complete_extent_equal": len(code) == len(target),
        "complete_bytes_equal": code == target,
        "diagnostic": {key: value for key, value in diagnostic.items()
                       if key != "full_diagnostic"},
        "full_diagnostic_path": diagnostic.get("full_diagnostic"),
        "diagnostic_authority": "DIAGNOSTIC_ONLY; complete bytes/fixups remain unaccepted",
    }


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--object", required=True, type=Path, help="Complete object file")
    parser.add_argument("--target", required=True, type=Path, help="Explicit target contribution bytes")
    parser.add_argument("--evidence", required=True, type=Path,
                        help="Explicit JSON receipt/evidence for the frozen diagnostic")
    parser.add_argument("--segment", default="UNIT_TEXT")
    parser.add_argument("--out-dir", required=True, type=Path,
                        help="Output directory beneath build/private")
    args = parser.parse_args(argv)
    output_dir = args.out_dir if args.out_dir.is_absolute() else ROOT / args.out_dir
    report = inspect_bytes(args.object.read_bytes(), args.target.read_bytes(),
                           read_json(args.evidence), output_dir, args.segment)
    report["input_paths"] = {"object": str(args.object.resolve()),
                             "target": str(args.target.resolve()),
                             "evidence": str(args.evidence.resolve())}
    report_path = output_dir / "research-object-inspection.json"
    write_json(report_path, report)
    print(json.dumps({"report": str(report_path), "authority": report["authority"],
                      "strict_status": report["strict_object_probe"]["status"],
                      "raw_object": report["raw_object"],
                      "inspected_segment": report["inspected_segment"],
                      "complete_contribution": report["complete_contribution"],
                      "fixup_count": len(report["fixups"]),
                      "diagnostic": report["diagnostic"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
