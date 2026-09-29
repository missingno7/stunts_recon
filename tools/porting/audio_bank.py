#!/usr/bin/env python3
"""Read-only parser for Stunts 1.1 audio resource banks (.VCE/.KMS/.SFX).

The layout follows the game's audioresource_find/read_audio_event routines.
It preserves unknown voice-record bytes and does not synthesize or patch data.
Run from the repository root, e.g.:
    python tools/porting/audio_bank.py --verify-all --json build/porting/audio-bank-validation.json
"""
from __future__ import annotations

import argparse
import collections
import json
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_ASSETS = ROOT / "assets"


class BankError(ValueError):
    pass


def u16(buf: bytes, pos: int) -> int:
    if pos < 0 or pos + 2 > len(buf):
        raise BankError(f"u16 out of bounds at {pos:#x}")
    return struct.unpack_from("<H", buf, pos)[0]


def u32(buf: bytes, pos: int) -> int:
    if pos < 0 or pos + 4 > len(buf):
        raise BankError(f"u32 out of bounds at {pos:#x}")
    return struct.unpack_from("<I", buf, pos)[0]


def fourcc(raw: bytes) -> str:
    return raw.decode("ascii", "replace")


@dataclass(frozen=True)
class Chunk:
    name: str
    index: int
    relative: int
    start: int
    end: int


@dataclass(frozen=True)
class Archive:
    start: int
    declared_size: int
    end: int
    count: int
    data_base: int
    chunks: tuple[Chunk, ...]


def parse_archive(
    buf: bytes, start: int = 0, limit: int | None = None, *, use_limit_as_end: bool = False
) -> Archive:
    """Parse one resource directory; offsets are relative to 6 + 8*count."""
    if limit is None:
        limit = len(buf)
    if start < 0 or limit > len(buf) or start + 6 > limit:
        raise BankError(f"archive header out of bounds at {start:#x}")
    declared = u32(buf, start)
    count = u16(buf, start + 4)
    data_base = start + 6 + 8 * count
    declared_end = start + declared
    end = limit if use_limit_as_end else declared_end
    if data_base > limit:
        raise BankError(f"directory ({data_base:#x}) exceeds bound {limit:#x}")
    if declared < 6 + 8 * count:
        raise BankError(f"declared size {declared:#x} is smaller than directory")
    if declared_end > limit:
        raise BankError(f"declared end {declared_end:#x} exceeds bound {limit:#x}")

    raw_entries: list[tuple[str, int, int]] = []
    for i in range(count):
        name = fourcc(buf[start + 6 + 4 * i : start + 10 + 4 * i])
        rel = u32(buf, start + 6 + 4 * count + 4 * i)
        pos = data_base + rel
        if pos < data_base or pos > end:
            raise BankError(f"{name} offset {rel:#x} outside archive end {end:#x}")
        raw_entries.append((name, rel, pos))

    unique_starts = sorted({pos for _, _, pos in raw_entries})
    next_start: dict[int, int] = {}
    for i, pos in enumerate(unique_starts):
        next_start[pos] = unique_starts[i + 1] if i + 1 < len(unique_starts) else end
    chunks = tuple(
        Chunk(name, i, rel, pos, next_start[pos])
        for i, (name, rel, pos) in enumerate(raw_entries)
    )
    return Archive(start, declared, end, count, data_base, chunks)


SPECIAL_ONE_BYTE = {0xDC, 0xDD, 0xDE, 0xE0, 0xE1, 0xE2, 0xE4, 0xE9, 0xEA}
SPECIAL_NO_DATA = {0xD9, 0xDA, 0xDB, 0xE3}
COMMAND_NAMES = {
    0xD9: "return-from-subtrack",
    0xDA: "stop-track",
    0xDB: "restart-track",
    0xDC: "select-sample/program",
    0xDD: "music-rate",
    0xDE: "voice-event",
    0xDF: "voice-parameter-event",
    0xE0: "max-voices",
    0xE1: "set-note",
    0xE2: "loop-begin",
    0xE3: "loop-end",
    0xE4: "set-velocity",
    0xE5: "set-word-parameter",
    0xE6: "call-subtrack",
    0xE7: "text/meta",
    0xE8: "raw-driver-bytes",
    0xE9: "set-channel",
    0xEA: "set-audio-value",
}


def read_vlq(buf: bytes, pos: int, end: int, *, max_bytes: int = 5) -> tuple[int, int]:
    value = 0
    for _ in range(max_bytes):
        if pos >= end:
            raise BankError("truncated variable-length integer")
        b = buf[pos]
        pos += 1
        value = (value << 7) | (b & 0x7F)
        if not (b & 0x80):
            return value, pos
    raise BankError("variable-length integer exceeds five bytes")


def parse_events(buf: bytes, start: int, end: int) -> list[dict[str, object]]:
    """Decode the stream consumed by src/obj_seg028.c:read_audio_event."""
    events: list[dict[str, object]] = []
    pos = start
    while pos < end:
        event_start = pos
        delta, pos = read_vlq(buf, pos, end)
        delta_bytes = pos - event_start
        if pos >= end:
            raise BankError("missing event command")
        command = buf[pos]
        pos += 1
        event: dict[str, object] = {
            "offset": event_start,
            "delta": delta,
            "delta_bytes": delta_bytes,
            "command": command,
            "name": COMMAND_NAMES.get(command, "channel-event" if command < 0xD9 else "unknown-special"),
        }
        if command in (0xE7, 0xE8):
            if pos >= end:
                raise BankError(f"truncated {command:#04x} length")
            length = buf[pos]
            pos += 1
            if pos + length > end:
                raise BankError(f"truncated {command:#04x} payload ({length} bytes)")
            payload = buf[pos : pos + length]
            pos += length
            event["payload"] = payload.hex()
            if command == 0xE7:
                event["text"] = payload.split(b"\0", 1)[0].decode("ascii", "replace")
        elif command == 0xE6:
            if pos + 5 > end:
                raise BankError("truncated E6 call-subtrack")
            event["param"] = buf[pos]
            target = buf[pos + 1 : pos + 5]
            event["target_raw"] = target.hex()
            event["target_fourcc"] = fourcc(target)
            event["target_value"] = int.from_bytes(target, "little")
            pos += 5
        elif command in SPECIAL_ONE_BYTE:
            if pos >= end:
                raise BankError(f"truncated {command:#04x} parameter")
            event["param"] = buf[pos]
            pos += 1
        elif command == 0xDF:
            if pos + 2 > end:
                raise BankError("truncated DF event")
            event["param"] = buf[pos]
            event["value"] = buf[pos + 1]
            pos += 2
        elif command == 0xE5:
            if pos + 2 > end:
                raise BankError("truncated E5 word parameter")
            event["value"] = u16(buf, pos)
            pos += 2
        elif command in SPECIAL_NO_DATA:
            pass
        elif command < 0xD9:
            if command > 0x80:
                if pos >= end:
                    raise BankError("truncated channel-event parameter")
                event["param"] = buf[pos]
                pos += 1
            value, pos = read_vlq(buf, pos, end)
            event["value"] = value
        else:
            # The game reader leaves unrecognized EBh..FFh commands payload-free.
            event["supported_by_game_reader"] = False
        event["length"] = pos - event_start
        if command == 0xE8:
            # process_audio_chunk_event passes AudioEvent.length - 4 to +39.
            # Keep that caller-visible count separate from the stored payload.
            event["driver_send_length"] = event["length"] - 4
            payload_length = len(bytes.fromhex(str(event["payload"])))
            event["driver_send_payload_delta"] = event["driver_send_length"] - payload_length
            event["driver_dropped_tail_bytes"] = max(0, payload_length - event["driver_send_length"])
            event["driver_read_past_payload_bytes"] = max(0, event["driver_send_length"] - payload_length)
        events.append(event)
    return events


def parse_header_chunk(buf: bytes, chunk: Chunk) -> dict[str, object]:
    """Decode the HDR1 tables patched by audio_map_song_tracks/instruments."""
    if chunk.start + 4 > chunk.end:
        raise BankError("short HDR1 chunk")
    declared = u32(buf, chunk.start)
    if declared < 7 or chunk.start + declared > chunk.end:
        raise BankError(f"HDR1 size {declared:#x} outside chunk span")
    p = chunk.start + 4
    fmt = buf[p : p + 2]
    p += 2
    instrument_count = buf[p]
    p += 1
    if p + 4 * instrument_count + 1 > chunk.start + declared:
        raise BankError("HDR1 instrument names truncated")
    instruments = [fourcc(buf[p + 4 * i : p + 4 * i + 4]) for i in range(instrument_count)]
    p += 4 * instrument_count
    track_count = buf[p]
    p += 1
    if p + 5 * track_count > chunk.start + declared:
        raise BankError("HDR1 track table truncated")
    tracks = []
    for i in range(track_count):
        q = p + 5 * i
        tracks.append({"name": fourcc(buf[q : q + 4]), "flag": buf[q + 4]})
    return {
        "declared_span": declared,
        "format_bytes": fmt.hex(),
        "instrument_names": instruments,
        "tracks": tracks,
    }


def parse_song_archive(buf: bytes, archive: Archive, label: str) -> dict[str, object]:
    chunks = {c.name.upper(): c for c in archive.chunks}
    hdr = chunks.get("HDR1")
    header = parse_header_chunk(buf, hdr) if hdr else None
    parsed_chunks: list[dict[str, object]] = []
    for chunk in archive.chunks:
        row: dict[str, object] = {
            "name": chunk.name,
            "offset": chunk.start - archive.start,
            "relative_data_offset": chunk.relative,
            "span": chunk.end - chunk.start,
        }
        if chunk.name.upper() == "HDR1":
            row["kind"] = "header"
            row["header"] = header
        elif chunk.start + 4 <= chunk.end:
            total_span = u32(buf, chunk.start)
            if total_span < 4 or chunk.start + total_span > chunk.end:
                raise BankError(
                    f"{label}/{chunk.name}: stream size {total_span:#x} exceeds chunk bound "
                    f"{chunk.end - chunk.start:#x}"
                )
            events = parse_events(buf, chunk.start + 4, chunk.start + total_span)
            hist = collections.Counter(int(e["command"]) for e in events)
            row.update(
                kind="event-track",
                stream_span=total_span,
                event_count=len(events),
                command_counts={f"0x{k:02X}": v for k, v in sorted(hist.items())},
                events=events,
                trailing_bytes=(chunk.end - chunk.start - total_span),
            )
        else:
            row["kind"] = "opaque"
        parsed_chunks.append(row)
    present = {c.name.upper() for c in archive.chunks}
    track_refs = [] if header is None else [str(t["name"]).upper() for t in header["tracks"]]
    missing_refs = [name for name in track_refs if name not in present]
    if missing_refs:
        raise BankError(f"{label}: HDR1 track references absent chunks {missing_refs}")
    return {
        "label": label,
        "archive_start": archive.start,
        "declared_size": archive.declared_size,
        "chunk_count": archive.count,
        "header": header,
        "chunks": parsed_chunks,
    }


def parse_vce(buf: bytes, archive: Archive, label: str) -> dict[str, object]:
    records = []
    for chunk in archive.chunks:
        if chunk.start + 4 > chunk.end:
            raise BankError(f"{label}/{chunk.name}: short voice record")
        size = u16(buf, chunk.start)
        revision = u16(buf, chunk.start + 2)
        if size < 0x3C or chunk.start + size > chunk.end:
            raise BankError(
                f"{label}/{chunk.name}: record size {size:#x} exceeds directory span "
                f"{chunk.end - chunk.start:#x}"
            )
        # These offsets are read in the game-side envelope/sample state machine.
        def byte_at(offset: int) -> int | None:
            return buf[chunk.start + offset] if offset < size else None
        def word_at(offset: int) -> int | None:
            return u16(buf, chunk.start + offset) if offset + 2 <= size else None
        fields = {
            "sample_kind_byte_05": byte_at(0x05),
            "voice_mask": u16(buf, chunk.start + 0x0C),
            "event_note_bias": buf[chunk.start + 0x10],
            "ad15_flag_0a": byte_at(0x0A),
            "ad15_note_adjust_11": byte_at(0x11),
            "mt15_state_byte_12": byte_at(0x12),
            "mt15_td15_event_value_gate_15": byte_at(0x15),
            "ad15_parameter_bytes_16_18_hex": buf[chunk.start + 0x16 : chunk.start + 0x19].hex(),
            "sample_rate_seed_word_1c": word_at(0x1C),
            "voice_state_seed_word_2a": word_at(0x2A),
            "voice_state_seed_word_2c": word_at(0x2C),
            "voice_state_seed_word_30": word_at(0x30),
            "voice_state_seed_byte_34": byte_at(0x34),
            "voice_state_seed_word_36": word_at(0x36),
            "voice_state_seed_word_38": word_at(0x38),
            "game_channel_override_43": byte_at(0x43),
            "driver_bytes_44_46_hex": buf[chunk.start + 0x44 : chunk.start + min(0x47, size)].hex(),
            "level1e": struct.unpack_from("<h", buf, chunk.start + 0x1E)[0],
            "level20": struct.unpack_from("<h", buf, chunk.start + 0x20)[0],
            "level22": struct.unpack_from("<h", buf, chunk.start + 0x22)[0],
            "level24": struct.unpack_from("<h", buf, chunk.start + 0x24)[0],
            "level26": struct.unpack_from("<h", buf, chunk.start + 0x26)[0],
            "loop_mode": buf[chunk.start + 0x28],
            "loop_count": buf[chunk.start + 0x29],
            "limit2e": u16(buf, chunk.start + 0x2E),
            "loop_flags": buf[chunk.start + 0x34],
            "pulse_present": buf[chunk.start + 0x35],
            "pulse_count": buf[chunk.start + 0x3A],
            "pulse_table_hex": buf[chunk.start + 0x3B : chunk.start + 0x43].hex(),
        }
        records.append(
            {
                "name": chunk.name,
                "offset": chunk.start,
                "relative_data_offset": chunk.relative,
                "record_size": size,
                "revision": revision,
                "voice_fields": fields,
                "raw_record": buf[chunk.start : chunk.start + size].hex(),
                "unclassified_gap_after": chunk.end - chunk.start - size,
            }
        )
    return {
        "label": label,
        "declared_size": archive.declared_size,
        "directory_base": archive.data_base,
        "record_count": archive.count,
        "records": records,
        "tail_bytes_after_declared_extent": len(buf) - (archive.start + archive.declared_size),
    }


def parse_file(path: Path) -> dict[str, object]:
    buf = path.read_bytes()
    ext = path.suffix.upper()
    # VCE length words are not reliable final bounds: several banks have a last
    # fixed-size record that crosses the nominal end, though it is present in
    # the raw file. The game never bounds audioresource_find with this field.
    root = parse_archive(buf, use_limit_as_end=(ext == ".VCE"))
    if ext == ".VCE":
        return {"file": path.name, "format": "VCE voice-bank directory", **parse_vce(buf, root, path.name)}
    if ext not in (".KMS", ".SFX"):
        raise BankError(f"unsupported extension: {path.suffix}")
    songs = []
    for top in root.chunks:
        child = parse_archive(buf, top.start, top.end)
        child_result = parse_song_archive(buf, child, f"{path.name}/{top.name}")
        child_result["outer_chunk"] = top.name
        child_result["outer_offset"] = top.start
        songs.append(child_result)
    return {
        "file": path.name,
        "format": "KMS nested song archive" if ext == ".KMS" else "SFX nested event archives",
        "declared_size": root.declared_size,
        "outer_chunk_count": root.count,
        "songs": songs,
    }


def summarize(results: Iterable[dict[str, object]]) -> dict[str, int]:
    totals = collections.Counter()
    for result in results:
        totals["files"] += 1
        if result["format"] == "VCE voice-bank directory":
            totals["voice_records"] += int(result["record_count"])
        else:
            for song in result["songs"]:
                totals["song_archives"] += 1
                for chunk in song["chunks"]:
                    totals["chunks"] += 1
                    totals["events"] += int(chunk.get("event_count", 0))
    return dict(totals)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("files", nargs="*", type=Path, help="input banks; defaults to all audio assets")
    parser.add_argument("--assets", type=Path, default=DEFAULT_ASSETS)
    parser.add_argument("--verify-all", action="store_true", help="validate every .VCE/.KMS/.SFX in assets")
    parser.add_argument("--json", type=Path, default=ROOT / "build/porting/audio-bank-validation.json",
                        help="write the parse tree under build/ (default: build/porting/audio-bank-validation.json)")
    args = parser.parse_args()
    files = args.files
    if args.verify_all or not files:
        files = sorted(
            p for p in args.assets.iterdir() if p.is_file() and p.suffix.upper() in (".VCE", ".KMS", ".SFX")
        )
    results = []
    for path in files:
        try:
            results.append(parse_file(path))
        except (OSError, BankError, struct.error) as exc:
            parser.error(f"{path}: {exc}")
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(results, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"validated": summarize(results), "files": [r["file"] for r in results]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
