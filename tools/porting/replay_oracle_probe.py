#!/usr/bin/env python3
"""Verify an ArtifactV2 replay prefix with its original DOS interpreter.

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
    port_forge = source_root / "port_forge"
    if schema == SCHEMA_V13:
        tree = port_forge / "build/history/pf-391f1dd"
        if v13_runner and build_v13:
            raise ValueError("choose either --v13-runner or --build-v13")
        if build_v13:
            if compiler is None:
                raise ValueError("--build-v13 requires --compiler")
            output_dir.mkdir(parents=True, exist_ok=True)
            exe = output_dir / "pf_dos_session_v13_391f1dd.exe"
            if exe.exists():
                raise ValueError(f"refusing to overwrite interpreter {exe}")
            compiler_path = Path(compiler).resolve(strict=True)
            environment = os.environ.copy()
            environment["PATH"] = str(compiler_path.parent) + os.pathsep + environment.get("PATH", "")
            subprocess.run([str(compiler_path), "-std=c++17", "-O2", "-Wall", "-Wextra",
                            "-Werror", "-I", str(tree),
                            str(tree / "tools/pf_dos_session.cpp"), "-o", str(exe)],
                           check=True, env=environment)
        elif v13_runner:
            exe = v13_runner
        else:
            raise ValueError("v13 replay needs --v13-runner or --build-v13 --compiler")
    elif schema == SCHEMA_V14:
        tree = port_forge
        exe = v14_runner or port_forge / "build/pf_dos_session.exe"
    else:
        raise ValueError(f"unsupported canonical schema {schema}")
    config = tree / "config/canonical-projections-v1.json"
    if not exe.is_file():
        raise FileNotFoundError(f"original interpreter missing: {exe}")
    if not config.is_file():
        raise FileNotFoundError(f"canonical projection config missing: {config}")
    return exe.resolve(), config.resolve()


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


def probe(*, source_root: Path, output_dir: Path, replay_name: str,
          to_exclusive: int, label: str, v13_runner: Path | None = None,
          v14_runner: Path | None = None, build_v13: bool = False,
          compiler: Path | None = None) -> Path:
    source_root = source_root.resolve(strict=True)
    output_dir = output_dir.resolve()
    replay_file = resolve_replay(source_root, replay_name)
    replay = json.loads(replay_file.read_text(encoding="utf-8"))
    if replay.get("format") != "portforge-replay-v2":
        raise ValueError("only ArtifactV2 replays are supported")
    timeline = replay["timeline"]
    if not 0 < to_exclusive <= len(timeline):
        raise ValueError("--to lies outside replay timeline")
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_-]*", label):
        raise ValueError("--label must contain only letters, digits, hyphen, underscore")
    schema = replay["environment"]["base_snapshot"]["canonical_schema"]
    exe, canonical_config = choose_runner(schema, source_root, output_dir,
                                           v13_runner, v14_runner, build_v13, compiler)
    base = resolve_bound_base(source_root, replay_file, replay)
    stem = f"{label}_b{to_exclusive}"
    output_dir.mkdir(parents=True, exist_ok=True)
    snapshot = output_dir / f"{stem}.pfsnapshot"
    raw = output_dir / f"{stem}.idxdac6"
    preview = output_dir / f"{stem}.png"
    receipt_file = output_dir / f"{stem}.json"
    log = output_dir / f"{stem}.log"
    if any(path.exists() for path in (snapshot, raw, preview, receipt_file, log)):
        raise ValueError(f"output {stem} already exists")
    command = [str(exe), "replay-rm", str(base), str(source_root / "assets"),
               str(source_root / "game.json"),
               str(source_root / "profiles/replay-boundaries-v1.json"),
               str(source_root / "recovery/execution-plan-oracle.json"), POINT,
               str(replay_file), "--to", str(to_exclusive), "--save-snapshot",
               str(snapshot), "--canonical-projections", str(canonical_config)]
    environment = os.environ.copy()
    if build_v13 and compiler is not None:
        environment["PATH"] = str(Path(compiler).resolve().parent) + os.pathsep + environment.get("PATH", "")
    result = subprocess.run(command, cwd=output_dir, text=True, capture_output=True,
                            env=environment)
    output = result.stdout + result.stderr
    log.write_text("$ " + subprocess.list2cmdline(command) + "\n" + output,
                   encoding="utf-8")
    if result.returncode:
        raise RuntimeError(f"replay exited {result.returncode}; see {log}")
    match = VERIFICATION.search(output)
    if not match or int(match.group(3)) or "ArtifactV2 replay verified:" not in output:
        raise RuntimeError(f"canonical verification incomplete; see {log}")
    capture = load_snapshot(snapshot)
    mode = capture.metadata["devices"]["video_mode"]
    if capture.payload is not None:
        raw.write_bytes(capture.payload)
        preview.write_bytes(png_preview(capture))
    checkpoints = [cp for cp in replay["checkpoints"]
                   if cp["boundary_ordinal"] < to_exclusive]
    canonical = CANONICAL.search(output)
    receipt = {
        "source_replay": str(replay_file),
        "source_replay_file_sha256": sha256(replay_file.read_bytes()),
        "base_snapshot": str(base),
        "base_snapshot_declared_sha256": replay["environment"]["base_snapshot"]["sha256"],
        "program_sha256": replay["identity"]["program_sha256"],
        "canonical_schema": schema,
        "interpreter": str(exe),
        "interpreter_file_sha256": sha256(exe.read_bytes()),
        "to_exclusive": to_exclusive,
        "last_stamp": timeline[to_exclusive - 1]["stamp"],
        "last_recorded_checkpoint": checkpoints[-1] if checkpoints else None,
        "verified_canonical_sha256": canonical.group(1) if canonical else None,
        "canonical_comparisons": int(match.group(1)),
        "sparse_full_audits": int(match.group(2)),
        "sparse_mismatches": int(match.group(3)),
        "snapshot_memory_sha256": capture.metadata["fingerprints"]["memory_sha256"],
        "snapshot_guest_instructions": capture.metadata["time"]["guest_instructions"],
        "cpu": {key: capture.metadata["cpu"][key] for key in ("cs", "ip", "ds", "ss")},
        "video_mode": mode,
        "width": 320 if mode == 0x13 else None,
        "height": 200 if mode == 0x13 else None,
        "indices_sha256": sha256(capture.indices) if capture.indices is not None else None,
        "dac6_sha256": sha256(capture.dac6) if capture.dac6 is not None else None,
        "idxdac6_sha256": sha256(capture.payload) if capture.payload is not None else None,
        "raw": str(raw) if capture.payload is not None else None,
        "preview": str(preview) if capture.payload is not None else None,
        "log": str(log),
    }
    receipt_file.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    return receipt_file


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
