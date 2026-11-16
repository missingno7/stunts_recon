"""Small storage helpers for transient DOSBox-X and port capture runs."""
from __future__ import annotations

from pathlib import Path


def byte_size(path: Path) -> int:
    """Return the bytes currently stored by one file or run directory."""
    path = Path(path)
    if path.is_file():
        return path.stat().st_size
    if not path.exists():
        return 0
    total = 0
    for child in path.rglob("*"):
        if child.is_file():
            try:
                total += child.stat().st_size
            except OSError:
                pass
    return total


def discard_raw_frames(capture: Path) -> int:
    """Delete only the uncompressed DOSBox frame dump(s) from a packed run."""
    capture = Path(capture)
    removed = 0
    for candidate in (capture / "original.frames",
                      capture / "original" / "original.frames"):
        try:
            size = candidate.stat().st_size
            candidate.unlink()
            removed += size
        except FileNotFoundError:
            pass
    return removed


def discard_raw_file(path: Path, *, within: Path) -> int:
    """Delete one raw payload only when it belongs to the supplied run tree."""
    path = Path(path).resolve()
    root = Path(within).resolve()
    try:
        path.relative_to(root)
    except ValueError:
        return 0
    try:
        size = path.stat().st_size
        path.unlink()
        return size
    except FileNotFoundError:
        return 0


def print_size(path: Path, label: str = "capture") -> None:
    print(f"{label} size: {byte_size(path)} bytes")
