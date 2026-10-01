#!/usr/bin/env python3
"""Pack verified replay-probe receipts into 64,000-index + 768-DAC fixtures."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import zlib

from replay_snapshot import DAC_BYTES, FRAME_BYTES, sha256


def pack(receipt_file: Path, fixture_dir: Path, label: str,
         description: str, provenance: str,
         parent_corpus_replay: str | None = None) -> dict:
    receipt = json.loads(receipt_file.read_text(encoding="utf-8"))
    if receipt["video_mode"] != 0x13 or receipt["raw"] is None:
        raise ValueError("state-only replay receipt has no indexed frame")
    payload = Path(receipt["raw"]).read_bytes()
    if len(payload) != FRAME_BYTES + DAC_BYTES:
        raise ValueError("indexed payload size mismatch")
    if (sha256(payload[:FRAME_BYTES]) != receipt["indices_sha256"]
            or sha256(payload[FRAME_BYTES:]) != receipt["dac6_sha256"]
            or sha256(payload) != receipt["idxdac6_sha256"]):
        raise ValueError("receipt frame SHA-256 mismatch")
    if provenance not in ("unmodified-corpus", "controlled-menu-branch"):
        raise ValueError("unknown provenance kind")
    if provenance == "controlled-menu-branch" and not parent_corpus_replay:
        raise ValueError("controlled branch requires its parent corpus replay")
    source = Path(receipt["source_replay"])
    row = {
        "label": label,
        "description": description,
        "provenance_kind": provenance,
        "source_replay": source.name,
        "source_file_sha256": receipt["source_replay_file_sha256"],
        "parent_corpus_replay": parent_corpus_replay,
        "source_canonical_schema": receipt["canonical_schema"],
        "source_program_sha256": receipt["program_sha256"],
        "source_base_snapshot_file": Path(receipt["base_snapshot"]).name,
        "source_base_snapshot_sha256": receipt["base_snapshot_declared_sha256"],
        "interpreter_file_sha256": receipt["interpreter_file_sha256"],
        "boundary_exclusive": receipt["to_exclusive"],
        "last_stamp": receipt["last_stamp"],
        "last_recorded_checkpoint": receipt["last_recorded_checkpoint"],
        "canonical_comparisons": receipt["canonical_comparisons"],
        "sparse_mismatches": receipt["sparse_mismatches"],
        "snapshot_memory_sha256": receipt["snapshot_memory_sha256"],
        "snapshot_guest_instructions": receipt["snapshot_guest_instructions"],
        "snapshot_cpu": receipt["cpu"],
        "width": 320,
        "height": 200,
        "indices_sha256": receipt["indices_sha256"],
        "palette_rgb6_sha256": receipt["dac6_sha256"],
        "dac6_sha256": receipt["dac6_sha256"],
        "idxdac6_sha256": receipt["idxdac6_sha256"],
        "payload_sha256": receipt["idxdac6_sha256"],
        "uncompressed_bytes": len(payload),
        "compression": "zlib",
        "payload": f"{label}.idxz",
    }
    if provenance == "controlled-menu-branch":
        coordinates = {"track-default": [0.68, 0.5],
                       "opponent-clock": [0.125, 0.72]}
        if label not in coordinates:
            raise ValueError("branch fixture label has no controlled edit definition")
        row["controlled_edit"] = {
            "channel": "dos.mouse.normalized",
            "occurrences": [264, 270],
            "original_uv": [0.5302083333333333, 0.7541666666666667],
            "replacement_uv": coordinates[label],
            "copied_input_before_occurrence": 450,
            "recorded_frames": 450,
            "checkpoint_every": 20,
        }
    compressed = zlib.compress(payload, 9)
    fixture_dir.mkdir(parents=True, exist_ok=True)
    path = fixture_dir / row["payload"]
    if path.exists():
        raise ValueError(f"refusing to overwrite {path}")
    path.write_bytes(compressed)
    row["compressed_bytes"] = len(compressed)
    return row


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--receipt", type=Path, required=True)
    parser.add_argument("--fixture-dir", type=Path, required=True)
    parser.add_argument("--label", required=True)
    parser.add_argument("--description", required=True)
    parser.add_argument("--provenance", choices=("unmodified-corpus", "controlled-menu-branch"),
                        required=True)
    parser.add_argument("--parent-corpus-replay")
    args = parser.parse_args(argv)
    row = pack(args.receipt, args.fixture_dir, args.label, args.description,
               args.provenance, args.parent_corpus_replay)
    sidecar = args.fixture_dir / f"{args.label}.json"
    if sidecar.exists():
        parser.error(f"refusing to overwrite {sidecar}")
    sidecar.write_text(json.dumps(row, indent=2, sort_keys=True) + "\n",
                       encoding="utf-8")
    print(sidecar)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
