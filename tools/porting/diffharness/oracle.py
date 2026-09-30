"""Load and relocate the immutable unpacked MCGA MZ image."""
from __future__ import annotations

import json
import os
import struct
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
DEFAULT_ASSETS = ROOT / "assets"
DEFAULT_LOAD_SEGMENT = 0x1000
DGROUP_PARAGRAPH = 0x2B87  # locked image's documented 0x3B87 anchor at load 1000h


@dataclass(frozen=True)
class RoutineAddress:
    name: str
    start: int
    end: int
    code_base: int
    stable_id: str | None = None

    def far_at(self, load_segment: int) -> tuple[int, int]:
        if self.code_base & 0x0F:
            raise ValueError(f"code frame {self.code_base:#x} is not paragraph aligned")
        if not 0 <= self.start - self.code_base <= 0xFFFF:
            raise ValueError(f"entry {self.start:#x} is outside its 16-bit code frame")
        base = (load_segment + self.code_base // 16) & 0xFFFF
        return base, self.start - self.code_base


class OracleImage:
    def __init__(self, load_image: bytes, relocations: list[dict], *,
                 asset_dir: Path | None = None):
        self.load_image = load_image
        self.relocations = relocations
        self.asset_dir = asset_dir or DEFAULT_ASSETS

    @classmethod
    def load(cls, asset_dir: Path | None = None) -> "OracleImage":
        """Recreate in memory and verify the complete immutable oracle lock."""
        root = ROOT
        tools = root / "tools"
        if str(tools) not in sys.path:
            sys.path.insert(0, str(tools))
        import oracle as oracle_tool  # tools/oracle.py
        from mz import MZ

        source_assets = asset_dir or Path(os.environ.get("STUNTS_ASSET_DIR", DEFAULT_ASSETS))
        _, unpacked, report, _ = oracle_tool.verify(write=False, asset_dir=source_assets)
        if report != json.loads((root / "layout/oracle.lock.json").read_text(
                encoding="utf-8")):
            raise ValueError("oracle report differs from layout/oracle.lock.json")
        mz = MZ.parse(unpacked)
        return cls(mz.load_image(unpacked), mz.relocations,
                   asset_dir=source_assets)

    def relocated(self, load_segment: int = DEFAULT_LOAD_SEGMENT) -> bytes:
        """Return the MZ load bytes with every ordered segment fixup applied."""
        image = bytearray(self.load_image)
        for fixup in self.relocations:
            at = fixup["load_offset"]
            if at < 0 or at + 2 > len(image):
                raise ValueError(f"MZ relocation outside load image at {at:#x}")
            value = struct.unpack_from("<H", image, at)[0]
            struct.pack_into("<H", image, at, (value + load_segment) & 0xFFFF)
        return bytes(image)


class SymbolMap:
    def __init__(self):
        self.root = ROOT
        self.code = json.loads((ROOT / "layout/code-symbols.json").read_text(
            encoding="utf-8"))["symbols"]
        self.names = json.loads((ROOT / "layout/names-registry.json").read_text(
            encoding="utf-8"))["names"]
        evidence_path = ROOT / "evidence/functions.json"
        self.functions = json.loads(evidence_path.read_text(encoding="utf-8"))["functions"]

    def resolve(self, name_or_address: str | int) -> RoutineAddress:
        if isinstance(name_or_address, int) or str(name_or_address).isdigit():
            start = int(name_or_address)
            rows = [(key, row) for key, row in self.code.items()
                    if row.get("mapped_target", {}).get("start") == start]
            if rows:
                key, row = rows[0]
                target = row["mapped_target"]
                return RoutineAddress(target.get("name", key), start,
                                      target["end"], row["frame_load_address"],
                                      target.get("stable_id"))
            raise KeyError(f"no code-symbol mapping at load offset {start:#x}")

        query = str(name_or_address)
        registry_addresses = [int(address) for address, row in self.names.items()
                              if row.get("name") == query or
                              row.get("inventory_name") == query]
        if registry_addresses:
            try:
                return self.resolve(registry_addresses[0])
            except KeyError:
                pass
        for key, row in self.code.items():
            target = row.get("mapped_target", {})
            labels = {key, key.lstrip("_"), target.get("name"),
                      target.get("stable_id")}
            if query in labels:
                return RoutineAddress(target.get("name", query), target["start"],
                                      target["end"], row["frame_load_address"],
                                      target.get("stable_id"))
        for row in self.functions:
            if query in {row.get("name"), row.get("stable_id")}:
                start = row.get("start")
                found = [(key, entry) for key, entry in self.code.items()
                         if entry.get("mapped_target", {}).get("start") == start]
                if found:
                    key, entry = found[0]
                    target = entry["mapped_target"]
                    return RoutineAddress(target.get("name", query), target["start"],
                                          target["end"], entry["frame_load_address"],
                                          target.get("stable_id", row.get("stable_id")))
        raise KeyError(f"routine {query!r} is not in the reviewed symbol maps")


def dgroup_segment(load_segment: int = DEFAULT_LOAD_SEGMENT) -> int:
    """Rebase the original DGROUP segment anchor for the selected DOS load."""
    return (load_segment + DGROUP_PARAGRAPH) & 0xFFFF
