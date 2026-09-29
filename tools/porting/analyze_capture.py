#!/usr/bin/env python3
"""Summarize DOSBox-X LOGCPU instructions for the DOSBox-X runtime capture."""
from __future__ import annotations
import argparse
import collections
import csv
import difflib
import hashlib
import json
import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
WORKSPACE = (REPO_ROOT / "build" / "porting" / "runtime-capture").resolve()
MAX_TRACE_BYTES = 512 * 1024 * 1024
MAX_TRACE_LINES = 2_000_000
MAX_MATCHED_EVENTS = 500_000
TARGET_PORTS = {0x40, 0x41, 0x42, 0x43, 0x3DA, 0x3C8, 0x3C9}
ADDR_RE = re.compile(r"^\s*([0-9A-F]{4}:[0-9A-F]{1,8})\s{2}(.*)$", re.I)
REG_RE = re.compile(r"\b(EAX|EDX)\s*:\s*([0-9A-F]{1,8})\b", re.I)
IO_RE = re.compile(r"^(IN|OUT)\s+(AL|AX|EAX|DX|[^,\s]+)\s*,\s*(AL|AX|EAX|DX|[^,\s]+)\s*$", re.I)


def inside_workspace(path: Path) -> Path:
    resolved = path.resolve()
    if not resolved.is_relative_to(WORKSPACE):
        raise ValueError(f"Path must stay under the capture output directory {WORKSPACE}: {resolved}")
    return resolved


def operand_value(text: str, edx: int) -> int:
    if text.upper() == "DX":
        return edx & 0xFFFF
    value = text.strip().upper()
    if value.startswith("0X"):
        value = value[2:]
    if value.endswith("H"):
        value = value[:-1]
    return int(value, 16)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", required=True, type=Path, help="DOSBox-X LOGCPU.TXT")
    parser.add_argument("--before", type=Path, help="INTVEC snapshot before/install state")
    parser.add_argument("--after", type=Path, help="INTVEC snapshot after/install state")
    parser.add_argument("--entry", action="append", default=[], metavar="LABEL=SEG:OFF", help="Count executions at an IVT entry address from the vector snapshot; repeat for game and saved INT 8/9 handlers")
    parser.add_argument("--out", required=True, type=Path, help="JSON summary path under build/porting/runtime-capture")
    args = parser.parse_args()
    entries = {}
    for spec in args.entry:
        if "=" not in spec:
            parser.error(f"entry must be LABEL=SEG:OFF: {spec}")
        label, address_text = spec.split("=", 1)
        if not label or ":" not in address_text:
            parser.error(f"entry must be LABEL=SEG:OFF: {spec}")
        seg_text, off_text = address_text.split(":", 1)
        try:
            entries[label] = (int(seg_text, 16) & 0xFFFF, int(off_text, 16) & 0xFFFF)
        except ValueError:
            parser.error(f"entry address must be hexadecimal SEG:OFF: {spec}")
    trace = inside_workspace(args.trace)
    out = inside_workspace(args.out)
    if not trace.is_file():
        parser.error(f"trace does not exist: {trace}")
    if trace.stat().st_size > MAX_TRACE_BYTES:
        parser.error(f"trace exceeds {MAX_TRACE_BYTES} byte safety limit")
    before = inside_workspace(args.before) if args.before else None
    after = inside_workspace(args.after) if args.after else None
    for p in (before, after):
        if p is not None and (not p.is_file() or p.stat().st_size > 2 * 1024 * 1024):
            parser.error(f"vector snapshot missing or exceeds 2 MiB: {p}")
    out.parent.mkdir(parents=True, exist_ok=True)
    events_path = out.with_suffix(out.suffix + ".events.csv")
    diff_path = out.with_suffix(out.suffix + ".vectors.diff")
    counts = {f"0x{p:04X}": {"read_port_hits": 0, "write_port_hits": 0, "out_values": {}} for p in sorted(TARGET_PORTS)}
    instructions = 0
    io_instructions = 0
    event_count = 0
    entry_hits = collections.Counter()
    digest = hashlib.sha256()
    with trace.open("rb") as raw, events_path.open("w", encoding="utf-8-sig", newline="") as event_file:
        writer = csv.writer(event_file)
        writer.writerow(["trace_line", "cs_ip", "direction", "port", "width_bytes", "out_value", "instruction"])
        for line_no, raw_line in enumerate(raw, 1):
            if line_no > MAX_TRACE_LINES:
                parser.error(f"trace exceeds {MAX_TRACE_LINES} line safety limit")
            digest.update(raw_line)
            line = raw_line.decode("utf-8", errors="replace").rstrip("\r\n")
            address = ADDR_RE.match(line)
            if not address:
                continue
            instructions += 1
            seg_text, ip_text = address.group(1).split(":", 1)
            current_address = (int(seg_text, 16) & 0xFFFF, int(ip_text, 16) & 0xFFFF)
            for label, entry_address in entries.items():
                if current_address == entry_address:
                    entry_hits[label] += 1
            body = address.group(2).split("  ", 1)[0].strip()
            op = IO_RE.match(body)
            if not op:
                continue
            regs = {m.group(1).upper(): int(m.group(2), 16) for m in REG_RE.finditer(line)}
            if "EAX" not in regs or "EDX" not in regs:
                continue
            direction = op.group(1).upper()
            left, right = op.group(2).upper(), op.group(3).upper()
            if direction == "OUT":
                port_token, value_reg = left, right
            else:
                value_reg, port_token = left, right
            try:
                port = operand_value(port_token, regs["EDX"])
            except ValueError:
                continue
            width = {"AL": 1, "AX": 2, "EAX": 4}.get(value_reg, 1)
            if value_reg not in ("AL", "AX", "EAX"):
                continue
            io_instructions += 1
            value = regs["EAX"] & ((1 << (8 * width)) - 1) if direction == "OUT" else None
            for byte_index in range(width):
                touched = (port + byte_index) & 0xFFFF
                key = f"0x{touched:04X}"
                if touched not in TARGET_PORTS:
                    continue
                bucket = counts[key]
                bucket["write_port_hits" if direction == "OUT" else "read_port_hits"] += 1
                out_byte = None
                if direction == "OUT":
                    out_byte = (value >> (8 * byte_index)) & 0xFF
                    values = bucket["out_values"]
                    byte_key = f"0x{out_byte:02X}"
                    values[byte_key] = values.get(byte_key, 0) + 1
                writer.writerow([line_no, address.group(1).upper(), direction, key, width, "" if out_byte is None else f"0x{out_byte:02X}", body])
                event_count += 1
                if event_count > MAX_MATCHED_EVENTS:
                    parser.error(f"matched I/O events exceed {MAX_MATCHED_EVENTS} safety limit")
    vector_diff_written = False
    if before and after:
        left_lines = before.read_text(encoding="utf-8", errors="replace").splitlines()
        right_lines = after.read_text(encoding="utf-8", errors="replace").splitlines()
        diff = difflib.unified_diff(left_lines, right_lines, fromfile=str(before), tofile=str(after), lineterm="")
        diff_path.write_text("\n".join(diff) + "\n", encoding="utf-8")
        vector_diff_written = True
    summary = {
        "trace": str(trace),
        "trace_bytes": trace.stat().st_size,
        "trace_sha256": digest.hexdigest(),
        "trace_lines": line_no if 'line_no' in locals() else 0,
        "instruction_rows": instructions,
        "decoded_io_instructions": io_instructions,
        "target_port_byte_events": event_count,
        "ports": counts,
        "vector_entry_hits": {label: {"address": f"{address[0]:04X}:{address[1]:04X}", "count": entry_hits[label]} for label, address in entries.items()},
        "vector_before": str(before) if before else None,
        "vector_after": str(after) if after else None,
        "vector_diff": str(diff_path) if vector_diff_written else None,
        "limitations": [
            "LOGCPU records instruction order and pre-instruction register state, not cycle timestamps.",
            "IN values are not recovered from pre-instruction state.",
            "Rate calculations need an independent interval or counter source.",
        ],
    }
    out.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"summary": str(out), "events_csv": str(events_path), "vector_diff": str(diff_path) if vector_diff_written else None, "target_port_byte_events": event_count}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

