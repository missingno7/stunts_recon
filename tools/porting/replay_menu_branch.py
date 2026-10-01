#!/usr/bin/env python3
"""Re-record the two controlled menu branches used by replay frame fixtures.

Only mouse hold events at occurrences 264 and 270 change. The v14 original
interpreter records a new ArtifactV2 and checks it against its bound base.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess

from replay_oracle_probe import POINT, SCHEMA_V14, choose_runner, resolve_bound_base, sha256


TARGETS = {"track-default": (0.68, 0.50),
           "opponent-clock": (0.125, 0.72)}
SOURCE_NAME = "rec_20260827_013930.pfreplay.json"
ORIGINAL_MOUSE = (0.5302083333333333, 0.7541666666666667)


def make_script(replay: dict, target: str) -> dict:
    if replay["environment"]["base_snapshot"]["canonical_schema"] != SCHEMA_V14:
        raise ValueError("branch source is not canonical v14")
    events = []
    edited = []
    for original in replay["events"]:
        at = original["at"]
        if at["occurrence"] >= 450:
            break
        event = {"occurrence": at["occurrence"], "sequence": len(events),
                 "channel": original["channel"], "payload": original["payload"].copy()}
        if (at["occurrence"] in (264, 270)
                and event["channel"] == "dos.mouse.normalized"):
            payload = event["payload"]
            if (payload["u"], payload["v"]) != ORIGINAL_MOUSE:
                raise ValueError("source mouse event differs from recorded Drive click")
            payload["u"], payload["v"] = TARGETS[target]
            edited.append(at["occurrence"])
        events.append(event)
    if edited != [264, 270]:
        raise ValueError("expected exactly the Drive mouse press and release")
    return {"format": "portforge-dos-input-script-v1", "point_id": POINT,
            "events": events}


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--target", choices=sorted(TARGETS), required=True)
    parser.add_argument("--v14-runner", type=Path)
    args = parser.parse_args(argv)
    source_root = args.source_root.resolve(strict=True)
    output_dir = args.output_dir.resolve()
    source_file = source_root / "artifacts/replays-v2" / SOURCE_NAME
    replay = json.loads(source_file.read_text(encoding="utf-8"))
    script = make_script(replay, args.target)
    exe, config = choose_runner(SCHEMA_V14, source_root, output_dir,
                                v14_runner=args.v14_runner)
    base = resolve_bound_base(source_root, source_file, replay)
    output_dir.mkdir(parents=True, exist_ok=True)
    stem = args.target
    script_file = output_dir / f"{stem}.input.json"
    artifact = output_dir / f"{stem}.pfreplay.json"
    log = output_dir / f"{stem}.record.log"
    if any(path.exists() for path in (script_file, artifact, log)):
        parser.error(f"output {stem} already exists")
    script_file.write_text(json.dumps(script, separators=(",", ":")) + "\n",
                           encoding="utf-8")
    command = [str(exe), "record-rm", str(base), str(source_root / "assets"),
               str(source_root / "game.json"),
               str(source_root / "profiles/replay-boundaries-v1.json"),
               str(source_root / "recovery/execution-plan-oracle.json"),
               POINT, "450", str(artifact), "--input-script", str(script_file),
               "--checkpoint-every", "20", "--canonical-projections", str(config)]
    result = subprocess.run(command, cwd=output_dir, text=True, capture_output=True)
    log.write_text("$ " + subprocess.list2cmdline(command) + "\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    if result.returncode:
        parser.exit(result.returncode, f"recording failed; see {log}\n")
    print(f"{artifact} (source SHA-256 {sha256(source_file.read_bytes())})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
