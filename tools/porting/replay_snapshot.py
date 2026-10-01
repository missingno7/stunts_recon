"""Validate a PortForge sparse DOS snapshot and extract its indexed VGA frame.

The framebuffer is guest A000:0000 memory, not a native renderer capture.
PortForge stores an expanded 8-bit palette; shifting by two recovers VGA DAC6.
"""
from __future__ import annotations

from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
import struct
import zlib


MEMORY_BYTES = 1 << 20
PAGE_BYTES = 4096
FRAME_BYTES = 320 * 200
DAC_BYTES = 256 * 3
VGA_OFFSET = 0xA0000


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


@dataclass(frozen=True)
class Snapshot:
    metadata: dict
    memory: bytes
    indices: bytes | None
    dac6: bytes | None

    @property
    def payload(self) -> bytes | None:
        if self.indices is None or self.dac6 is None:
            return None
        return self.indices + self.dac6


def load_snapshot(path: Path) -> Snapshot:
    """Check page and whole-memory hashes before trusting any frame data."""
    path = Path(path)
    meta = json.loads((path / "snapshot.json").read_text(encoding="utf-8"))
    pages = json.loads((path / "memory/pages.json").read_text(encoding="utf-8"))
    sparse = (path / "memory/pages.bin").read_bytes()
    if pages.get("page_size") != PAGE_BYTES or pages.get("total_bytes") != MEMORY_BYTES:
        raise ValueError("unsupported sparse snapshot memory layout")
    memory = bytearray(MEMORY_BYTES)
    seen_indices: set[int] = set()
    ranges: list[tuple[int, int]] = []
    for page in pages["pages"]:
        index, offset = page["index"], page["offset"]
        if not isinstance(index, int) or not 0 <= index < MEMORY_BYTES // PAGE_BYTES:
            raise ValueError("sparse page index out of range")
        if not isinstance(offset, int) or offset < 0 or offset + PAGE_BYTES > len(sparse):
            raise ValueError("sparse page payload out of range")
        if index in seen_indices or any(offset < end and start < offset + PAGE_BYTES for start, end in ranges):
            raise ValueError("duplicate or overlapping sparse page")
        seen_indices.add(index)
        ranges.append((offset, offset + PAGE_BYTES))
        payload = sparse[offset:offset + PAGE_BYTES]
        if sha256(payload) != page["sha256"]:
            raise ValueError(f"sparse page {index} SHA-256 mismatch")
        memory[index * PAGE_BYTES:(index + 1) * PAGE_BYTES] = payload
    if sha256(memory) != meta["fingerprints"]["memory_sha256"]:
        raise ValueError("snapshot whole-memory SHA-256 mismatch")
    mode = meta["devices"]["video_mode"]
    if mode != 0x13:
        return Snapshot(meta, bytes(memory), None, None)
    palette = bytes.fromhex(meta["devices"]["palette_hex"])
    if len(palette) != DAC_BYTES:
        raise ValueError("mode 13h snapshot requires 256 RGB palette entries")
    indices = bytes(memory[VGA_OFFSET:VGA_OFFSET + FRAME_BYTES])
    return Snapshot(meta, bytes(memory), indices, bytes(value >> 2 for value in palette))


def png_preview(snapshot: Snapshot) -> bytes:
    if snapshot.indices is None or snapshot.dac6 is None:
        raise ValueError("snapshot has no mode 13h frame")
    palette = bytes((value << 2) | (value >> 4) for value in snapshot.dac6)
    rgb = bytes(component for index in snapshot.indices
                for component in palette[index * 3:index * 3 + 3])

    def chunk(kind: bytes, payload: bytes) -> bytes:
        return (struct.pack(">I", len(payload)) + kind + payload
                + struct.pack(">I", zlib.crc32(kind + payload)))

    rows = b"".join(b"\0" + rgb[y * 320 * 3:(y + 1) * 320 * 3]
                    for y in range(200))
    return (b"\x89PNG\r\n\x1a\n"
            + chunk(b"IHDR", struct.pack(">IIBBBBB", 320, 200, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(rows, 9)) + chunk(b"IEND", b""))
