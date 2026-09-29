#!/usr/bin/env python3
"""Compare an SDL3 M0 JSONL trace with the available Port Forge F5c summary."""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_TRACE = ROOT / "build" / "sdl3" / "runtime-trace.jsonl"
DEFAULT_SUMMARY = ROOT / "docs" / "evidence" / "freeze-campaign" / "F5c-trace_summary.json"
DEFAULT_NORMALIZED = ROOT / "build" / "sdl3" / "runtime-trace.normalized.jsonl"
DEFAULT_REPORT = ROOT / "build" / "sdl3" / "trace-regression.json"

REQUIRED_FIELDS = {
    "header": ("trace_schema", "event_type", "screen_width", "screen_height",
               "indexed_palette_entries", "timer_target_hz", "timer_period_ns"),
    "timer_tick": ("trace_schema", "event_type", "tick_id", "scheduled_tick",
                   "observed_tick", "machine_tick", "timer_target_hz"),
    "simulation_step": ("trace_schema", "event_type", "sim_step_id", "game_frame",
                        "machine_tick"),
    "video_publication": ("trace_schema", "event_type", "frame_id", "sim_step_id",
                          "game_frame", "video_phase", "machine_tick",
                          "publication_reason"),
    "audio_publication": ("trace_schema", "event_type", "audio_publication_id",
                          "audio_frame_count", "machine_tick", "audio_backend"),
    "host_present": ("trace_schema", "event_type", "frame_id", "present_id",
                     "machine_tick"),
    "host_stop": ("trace_schema", "event_type", "machine_tick", "reason"),
}


def read_jsonl(path: Path) -> list[dict]:
    rows = []
    with path.open("r", encoding="utf-8") as stream:
        for number, line in enumerate(stream, 1):
            if not line.strip():
                continue
            row = json.loads(line)
            if not isinstance(row, dict):
                raise ValueError(f"{path}:{number}: expected a JSON object")
            rows.append(row)
    return rows


def run_normalizer(trace: Path, normalizer: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [sys.executable, str(normalizer), str(trace), "--out", str(destination)],
        cwd=ROOT, capture_output=True, text=True, check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "Port Forge trace normalizer failed")


def display_path(path: Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT))
    except ValueError:
        return str(path)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, default=DEFAULT_TRACE)
    parser.add_argument("--summary", type=Path, default=DEFAULT_SUMMARY)
    parser.add_argument("--normalizer", type=Path,
                        default=ROOT / "tools" / "porting" / "normalize_portforge_trace.py")
    parser.add_argument("--normalized", type=Path, default=DEFAULT_NORMALIZED)
    parser.add_argument("--report", type=Path, default=DEFAULT_REPORT)
    args = parser.parse_args(argv)

    failures: list[str] = []
    try:
        run_normalizer(args.trace, args.normalizer, args.normalized)
        rows = read_jsonl(args.normalized)
        reference = json.loads(args.summary.read_text(encoding="utf-8"))
    except (OSError, ValueError, RuntimeError) as exc:
        print(f"trace regression could not run: {exc}", file=sys.stderr)
        return 2

    schema = reference.get("trace_schema")
    if not schema:
        failures.append("F5c summary has no trace_schema")
    counts = Counter(row.get("event_type") for row in rows)
    if counts["header"] != 1:
        failures.append(f"expected exactly one header, found {counts['header']}")
    for row_number, row in enumerate(rows, 1):
        kind = row.get("event_type")
        if row.get("trace_schema") != schema:
            failures.append(f"row {row_number}: trace_schema differs from F5c")
        required = REQUIRED_FIELDS.get(kind)
        if required is None:
            failures.append(f"row {row_number}: unsupported event_type {kind!r}")
        else:
            missing = [field for field in required if field not in row]
            if missing:
                failures.append(f"row {row_number} ({kind}): missing {', '.join(missing)}")

    headers = [row for row in rows if row.get("event_type") == "header"]
    if headers:
        header = headers[0]
        if (header.get("screen_width"), header.get("screen_height"),
                header.get("indexed_palette_entries")) != (320, 200, 256):
            failures.append("header does not describe the 320x200, 256-entry indexed surface")

    timer_rows = sorted((row for row in rows if row.get("event_type") == "timer_tick"),
                        key=lambda row: row.get("tick_id", -1))
    scheduled = [row["scheduled_tick"] for row in timer_rows
                 if isinstance(row.get("scheduled_tick"), int)]
    observed = [row["observed_tick"] for row in timer_rows
                if isinstance(row.get("observed_tick"), int)]
    expected_histogram = reference.get("timer_tick_interval_histogram_ticks", {})
    if not timer_rows or len(scheduled) != len(timer_rows):
        failures.append("timer_tick samples are missing or lack scheduled deadlines")
    if any(right <= left for left, right in zip(scheduled, scheduled[1:])):
        failures.append("scheduled timer deadlines are not strictly increasing")

    expected_samples = sum(int(value) for value in expected_histogram.values())
    expected_ns = (sum(int(interval) * int(amount)
                       for interval, amount in expected_histogram.items()) / expected_samples
                   if expected_samples else 0.0)
    scheduled_mean_ns = (sum(right - left for left, right in zip(scheduled, scheduled[1:])) /
                         (len(scheduled) - 1) if len(scheduled) > 1 else 0.0)
    observed_mean_ns = (sum(right - left for left, right in zip(observed, observed[1:])) /
                        (len(observed) - 1) if len(observed) > 1 else 0.0)
    expected_hz = 1_000_000_000.0 / expected_ns if expected_ns else 0.0
    scheduled_hz = 1_000_000_000.0 / scheduled_mean_ns if scheduled_mean_ns else 0.0
    observed_hz = 1_000_000_000.0 / observed_mean_ns if observed_mean_ns else 0.0
    if not expected_hz or abs(scheduled_hz - expected_hz) > 0.005:
        failures.append(f"scheduled timer rate {scheduled_hz:.6f} Hz differs from "
                        f"F5c {expected_hz:.6f} Hz")

    reference_counts = reference.get("event_counts", {})
    comparison_types = sorted(set(reference_counts) | {
        "simulation_step", "video_publication", "audio_publication", "timer_tick"})
    coverage = []
    for kind in comparison_types:
        baseline_count = reference_counts.get(kind)
        actual_count = counts.get(kind, 0)
        coverage.append({"event_type": kind, "f5c_count": baseline_count,
                         "m0_count": actual_count,
                         "coverage_gap": bool(baseline_count and not actual_count)})

    stop = next((row.get("reason") for row in rows
                 if row.get("event_type") == "host_stop"), None)
    report = {
        "schema": "stunts-sdl3-trace-regression-v1",
        "reference": display_path(args.summary),
        "reference_port_forge_commit": reference.get("source_port_forge_commit"),
        "normalizer": display_path(args.normalizer),
        "normalized_trace": display_path(args.normalized),
        "m0_event_counts": dict(sorted(counts.items())),
        "f5c_event_coverage": coverage,
        "timer": {
            "m0_ticks": len(timer_rows),
            "f5c_interval_histogram_ns": expected_histogram,
            "f5c_mean_scheduled_hz": round(expected_hz, 8),
            "m0_mean_scheduled_hz": round(scheduled_hz, 8),
            "m0_mean_observed_hz": round(observed_hz, 6) if observed_hz else None,
        },
        "startup_stop": stop,
        "failures": failures,
        "passed": not failures,
        "comparison_limit": "F5c-trace_summary.json provides event counts and timer histogram, "
                            "not the raw reference JSONL; event coverage gaps are reported, "
                            "not treated as M0 failures.",
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"passed": report["passed"], "m0_event_counts": report["m0_event_counts"],
                      "timer": report["timer"], "startup_stop": stop,
                      "coverage_gaps": [row["event_type"] for row in coverage
                                        if row["coverage_gap"]],
                      "failures": failures}, indent=2))
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
