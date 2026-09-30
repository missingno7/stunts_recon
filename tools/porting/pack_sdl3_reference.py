#!/usr/bin/env python3
"""Pack PortForge .pfidx captures as compact indexed-frame test fixtures."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib


MAGIC = b"PFIDXFRM"
HEADER_SIZE = 360
PIXEL_BYTES = 320 * 200
PALETTE_BYTES = 256 * 3


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def parse_pfidx(path: Path) -> tuple[dict[str, object], bytes]:
    source = path.read_bytes()
    if len(source) != HEADER_SIZE + PIXEL_BYTES + PALETTE_BYTES:
        raise ValueError(f"{path}: expected a canonical v1 PFIDX file, got {len(source)} bytes")
    if source[:8] != MAGIC:
        raise ValueError(f"{path}: PFIDX magic mismatch")
    version, width, height = struct.unpack_from("<IHH", source, 8)
    ordinal = struct.unpack_from("<Q", source, 16)[0]
    mode = source[24]
    reserved = source[25:32]
    pixel_len, palette_len = struct.unpack_from("<II", source, 32)
    if (version, width, height, mode, pixel_len, palette_len) != (
            1, 320, 200, 0x13, PIXEL_BYTES, PALETTE_BYTES):
        raise ValueError(f"{path}: unsupported PFIDX identity or payload layout")
    if any(reserved):
        raise ValueError(f"{path}: nonzero PFIDX reserved header byte")
    replay_fingerprint = source[40:104].decode("ascii")
    program_sha256 = source[104:168].decode("ascii")
    recorded_indices_hash = source[168:232].decode("ascii")
    recorded_palette_hash = source[232:296].decode("ascii")
    recorded_frame_hash = source[296:360].decode("ascii")
    pixels = source[HEADER_SIZE:HEADER_SIZE + PIXEL_BYTES]
    palette = source[HEADER_SIZE + PIXEL_BYTES:]
    if not (all(len(value) == 64 for value in
                (replay_fingerprint, program_sha256, recorded_indices_hash,
                 recorded_palette_hash, recorded_frame_hash))):
        raise ValueError(f"{path}: malformed PFIDX SHA-256 header field")
    if sha256(pixels) != recorded_indices_hash:
        raise ValueError(f"{path}: indexed-pixel digest does not match PFIDX header")
    if sha256(palette) != recorded_palette_hash:
        raise ValueError(f"{path}: DAC digest does not match PFIDX header")
    return ({
        "source_pfidx": path.name,
        "source_file_sha256": sha256(source),
        "replay_fingerprint": replay_fingerprint,
        "program_sha256": program_sha256,
        "checkpoint_ordinal": ordinal,
        "format_version": version,
        "width": width,
        "height": height,
        "video_mode": mode,
        "indices_sha256": recorded_indices_hash,
        "palette_rgb6_sha256": recorded_palette_hash,
        "portforge_frame_sha256": recorded_frame_hash,
    }, pixels + palette)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True,
                        help="fixture output directory (normally tests/fixtures/sdl3)")
    parser.add_argument("captures", nargs="+", metavar="NAME=PFIDX",
                        help="label and original PortForge indexed capture path")
    args = parser.parse_args(argv)
    args.out.mkdir(parents=True, exist_ok=True)
    for value in args.captures:
        if "=" not in value:
            parser.error(f"capture must use NAME=PFIDX: {value}")
        name, raw_path = value.split("=", 1)
        if not name.replace("-", "").replace("_", "").isalnum():
            parser.error(f"invalid fixture name: {name}")
        source_path = Path(raw_path)
        metadata, payload = parse_pfidx(source_path)
        compressed = zlib.compress(payload, level=9)
        payload_name = f"{name}.idxz"
        (args.out / payload_name).write_bytes(compressed)
        metadata.update({
            "fixture": name,
            "payload": payload_name,
            "uncompressed_bytes": len(payload),
            "payload_sha256": sha256(payload),
            "compressed_bytes": len(compressed),
            "compression": "zlib",
        })
        with (args.out / f"{name}.json").open(
                "w", encoding="utf-8", newline="\n") as stream:
            stream.write(json.dumps(metadata, indent=2, sort_keys=True) + "\n")
        print(f"{name}: PFIDX ordinal {metadata['checkpoint_ordinal']} "
              f"pixels={metadata['indices_sha256']} palette={metadata['palette_rgb6_sha256']} "
              f"{len(payload)}->{len(compressed)} bytes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
