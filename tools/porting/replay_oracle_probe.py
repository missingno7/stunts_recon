#!/usr/bin/env python3
"""Read archived ArtifactV2 metadata. Live original capture uses DOSBox-X.

Only source-root inputs are read. Snapshots, logs, receipts, and optional frames
are written under --output-dir. A boundary count is exclusive: --to 800 means
the guest state immediately after boundary ordinal 799.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import subprocess

from replay_snapshot import load_snapshot, png_preview, sha256


SCHEMA_V13 = "pf-canonical-rm-v13"
SCHEMA_V14 = "pf-canonical-rm-v14"
POINT = "stunts.global.task"
VERIFICATION = re.compile(
    r"verification: (\d+) canonical comparisons, .*?\((\d+) full audits, (\d+) mismatches\)")
CANONICAL = re.compile(r"ArtifactV2 replay verified:.*canonical ([0-9a-f]{64})")


def choose_runner(schema: str, source_root: Path, output_dir: Path,
                  v13_runner: Path | None = None, v14_runner: Path | None = None,
                  build_v13: bool = False, compiler: Path | None = None) -> tuple[Path, Path]:
    """Return executable and matching canonical-projection config directory."""
    # Compatibility reader for archived receipts and its synthetic tests. This
    # no longer builds or invokes an interpreter. Live references use DOSBox-X.
    if schema not in (SCHEMA_V13,SCHEMA_V14):
        raise ValueError(f"unsupported canonical schema {schema}")
    if build_v13:
        raise ValueError("legacy interpreter builds retired; use original_tick_oracle.py")
    runner=v13_runner if schema==SCHEMA_V13 else v14_runner
    if runner is None:
        raise ValueError("v13 replay needs an explicit archived runner" if schema==SCHEMA_V13 else
                         "live replay probing retired; use original_tick_oracle.py")
    configs=[p for p in source_root.rglob('canonical-projections-v1.json')
             if ('history' in p.parts)==(schema==SCHEMA_V13)]
    if len(configs)!=1:raise ValueError('ambiguous archived projection configuration')
    if not runner.is_file():raise FileNotFoundError(runner)
    return runner.resolve(),configs[0].resolve()


def resolve_replay(source_root: Path, name: str) -> Path:
    path = Path(name)
    if path.is_file():
        return path.resolve()
    corpus = source_root / "artifacts/replays-v2"
    if not name.endswith(".pfreplay.json"):
        name += ".pfreplay.json"
    path = corpus / name
    if not path.is_file():
        raise FileNotFoundError(f"ArtifactV2 replay missing: {path}")
    return path.resolve()


def resolve_bound_base(source_root: Path, replay_file: Path, replay: dict) -> Path:
    binding = replay["environment"]["base_snapshot"]
    name = binding["file"]
    if Path(name).name != name:
        raise ValueError("base snapshot binding must be a filename")
    candidates = (replay_file.parent / name,
                  source_root / "artifacts/replays-v2" / name,
                  source_root / "artifacts" / name)
    for path in candidates:
        if path.is_dir():
            return path.resolve()
    raise FileNotFoundError(f"bound base snapshot missing: {name}")


def probe(**kwargs):
    raise RuntimeError("Legacy interpreter capture retired; use tools/porting/original_tick_oracle.py with DOSBox-X")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--replay", required=True, help="corpus basename or ArtifactV2 path")
    parser.add_argument("--to", type=int, required=True, help="exclusive semantic boundary")
    parser.add_argument("--label", required=True)
    parser.add_argument("--v13-runner", type=Path)
    parser.add_argument("--v14-runner", type=Path)
    parser.add_argument("--build-v13", action="store_true")
    parser.add_argument("--compiler", type=Path, help="C++ compiler for --build-v13")
    args = parser.parse_args(argv)
    try:
        receipt = probe(source_root=args.source_root, output_dir=args.output_dir,
                        replay_name=args.replay, to_exclusive=args.to, label=args.label,
                        v13_runner=args.v13_runner, v14_runner=args.v14_runner,
                        build_v13=args.build_v13, compiler=args.compiler)
    except (ValueError, FileNotFoundError, RuntimeError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f"{exc}\n")
    print(receipt)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
