#!/usr/bin/env python3
"""Normalize Stunts Port Forge event JSONL while preserving raw observations.

The input is one JSON object per line. The tool streams rows, accepts any input
path, and does not infer simulation-step IDs from sampled game-frame counters.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys
from typing import TextIO


TRACE_SCHEMA = "stunts-runtime-trace-v1"
PHASE_NAMES = {0: "frame_start", 1: "vertical_retrace_start"}


def _mode_pair(row: dict) -> str | None:
    if "game_inputmode" not in row or "game_replay_mode" not in row:
        return None
    return f"input{row['game_inputmode']}_replay{row['game_replay_mode']}"


def _tick(row: dict) -> int | None:
    for key in ("machine_tick", "observed_tick", "actual_tick"):
        value = row.get(key)
        if isinstance(value, int):
            return value
    return None


def normalize_stream(source: Path, output: TextIO) -> int:
    """Write normalized JSONL from *source* to *output*; return row count."""
    timer_previous: tuple[int, int | None] | None = None
    video_previous: dict[str, tuple[int, str | None]] = {}
    row_count = 0

    with source.open("r", encoding="utf-8") as stream:
        for line_number, line in enumerate(stream, 1):
            if not line.strip():
                continue
            try:
                row = json.loads(line)
            except json.JSONDecodeError as exc:
                raise ValueError(f"{source}:{line_number}: invalid JSON: {exc.msg}") from exc
            if not isinstance(row, dict):
                raise ValueError(f"{source}:{line_number}: expected a JSON object")

            schema = row.setdefault("trace_schema", TRACE_SCHEMA)
            if schema != TRACE_SCHEMA:
                raise ValueError(
                    f"{source}:{line_number}: unsupported trace_schema {schema!r}"
                )
            kind = row.get("event_type")
            if not isinstance(kind, str) or not kind:
                raise ValueError(f"{source}:{line_number}: event_type is required")

            mode = _mode_pair(row)
            if mode is not None and not row.get("game_mode_pair_raw"):
                row["game_mode_pair_raw"] = mode
            if not isinstance(row.get("rate_target_hz"), int) and \
                    isinstance(row.get("globalgamesettings_game_framespersec"), int):
                row["rate_target_hz"] = row["globalgamesettings_game_framespersec"]

            tick = _tick(row)
            if tick is not None and not isinstance(row.get("machine_tick"), int):
                row["machine_tick"] = tick

            if kind == "timer_tick":
                if "tick_id" not in row:
                    event_id = row.get("event_id", row.get("sequence"))
                    if event_id is not None:
                        row["tick_id"] = event_id
                observed = row.get("observed_tick")
                if not isinstance(observed, int):
                    observed = tick
                frame = row.get("game_frame")
                if isinstance(observed, int):
                    if timer_previous is not None:
                        previous_tick, previous_frame = timer_previous
                        interval = observed - previous_tick
                        if interval >= 0:
                            row["timer_interval_ticks"] = interval
                        if isinstance(frame, int) and isinstance(previous_frame, int):
                            delta = (frame - previous_frame) & 0xFFFF
                            if delta < 0x8000:
                                row["game_frame_delta_since_prev_timer"] = delta
                            else:
                                row["game_frame_counter_discontinuity"] = {
                                    "previous": previous_frame,
                                    "current": frame,
                                    "delta_mod16": delta,
                                }
                    timer_previous = (observed, frame if isinstance(frame, int) else None)

            elif kind == "video_publication":
                if "frame_id" not in row:
                    event_id = row.get("event_id", row.get("sequence"))
                    if event_id is not None:
                        row["frame_id"] = event_id
                phase_code = row.get("phase_code")
                if not row.get("video_phase") and isinstance(phase_code, int):
                    row["video_phase"] = PHASE_NAMES.get(phase_code, f"phase_{phase_code}")
                phase = str(row.get("video_phase", phase_code if phase_code is not None else "unknown"))
                frame = row.get("game_frame")
                if isinstance(frame, int):
                    previous = video_previous.get(phase)
                    if previous is not None:
                        previous_frame, previous_mode = previous
                        delta = (frame - previous_frame) & 0xFFFF
                        if delta < 0x8000:
                            row["game_frame_delta_since_prev_same_phase"] = delta
                        else:
                            row["game_frame_counter_discontinuity"] = {
                                "previous": previous_frame,
                                "current": frame,
                                "delta_mod16": delta,
                                "mode_before": previous_mode,
                                "mode_after": mode,
                            }
                    video_previous[phase] = (frame, mode)

            output.write(json.dumps(row, sort_keys=True, separators=(",", ":"),
                                    ensure_ascii=False) + "\n")
            row_count += 1

    return row_count


def normalize_file(source: Path, destination: Path) -> int:
    """Normalize to a caller-selected file path, preventing in-place overwrite."""
    source = source.resolve()
    destination = destination.resolve()
    if source == destination:
        raise ValueError("input and output paths must differ")
    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open("w", encoding="utf-8", newline="\n") as output:
        return normalize_stream(source, output)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path, help="input Stunts Port Forge event JSONL")
    parser.add_argument("--out", type=Path, help="normalized JSONL output (default: stdout)")
    args = parser.parse_args(argv)

    try:
        if args.out is None:
            count = normalize_stream(args.trace, sys.stdout)
        else:
            count = normalize_file(args.trace, args.out)
    except (OSError, ValueError) as exc:
        parser.error(str(exc))
    print(f"normalized {count} trace rows", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
