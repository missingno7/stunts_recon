"""One-off reviewed pristine overlay for init_carstate_from_simd.

The packet and exact scratch object are research evidence; this checks both
again and installs only bounded original instruction/flow evidence. Production
acceptance remains the native grinder and whole-image path.
"""

import json
import sys
from pathlib import Path

from research_packet import packet

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
from common import read_json, sha  # noqa: E402
from compiler import compile_source  # noqa: E402
from function_evidence import reviewed_functions  # noqa: E402
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402


def main():
    p = packet("init_carstate_from_simd")
    assert p["id"] == "load_06898" and p["extent"]["size"] == 618
    assert not (p["cfg"]["flow_risks"] or p["cfg"]["unreachable_nonpadding"]
                or p["calls"] or p["mz_relocation_sites"]
                or p["researched_capability_blockers"] or p["other_mapped_entries_inside"])
    start, end = p["extent"]["start"], p["extent"]["end"]
    recipe = read_json(ROOT / "recipes/init_carstate_from_simd.json")
    assert recipe["stable_id"] == p["id"] and (recipe["start"], recipe["end"]) == (start, end)
    assert recipe["expected_fixups"] == recipe["expected_relocations"] == []
    assert recipe["evidence"]["path"] == "src/restunts/asmorig/seg001.asm"
    source = (ROOT / recipe["source"]).read_bytes()
    assert sha(source) == "10591b41ce0161332efc41d8f615a45070aef95e9f0c2c6e8497ed29f02d90cf"
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    target = image[start:end]
    obj, _ = compile_source(source, recipe["profile"])
    assert obj.segments["UNIT_TEXT"] == target
    assert obj.publics == [{"name": "_init_carstate_from_simd", "offset": 0, "segment": "UNIT_TEXT"}]
    assert not obj.linker_fixups
    assert all(obj.segment_lengths.get(name, 0) == 0 for name in ("_DATA", "CONST", "_BSS"))

    rows = p["disassembly"]
    by = {r["load_offset"]: bytes.fromhex(r["bytes"]) for r in rows}
    pending, reached = [start], set()
    while pending:
        at = pending.pop()
        if at in reached:
            continue
        assert at in by
        reached.add(at)
        raw = by[at]
        following = at + len(raw)
        if raw[0] in (0xcb, 0xca, 0xc3, 0xc2):
            continue
        if raw[0] == 0xeb or 0x70 <= raw[0] <= 0x7f:
            pending.append(following + int.from_bytes(raw[1:], "little", signed=True))
            if raw[0] == 0xeb:
                continue
        elif raw[0] == 0xe9:
            pending.append(following + int.from_bytes(raw[1:], "little", signed=True))
            continue
        assert raw[0] not in (0xff, 0xea)
        pending.append(following)
    padding = sorted(set(by) - reached)
    assert len(padding) == 1 and all(by[at] == b"\x90" for at in padding)
    overlay = {
        "name": p["task"], "stable_id": p["id"], "start": start, "end": end,
        "size": p["extent"]["size"], "sha256": p["extent"]["sha256"],
        "relocation_sites": [], "disassembly": rows,
        "reachable_instruction_offsets": sorted(reached), "padding_offsets": padding,
        "boundary_anchors": [
            {"site": start - 4, "hex": image[start - 4:start + 4].hex()},
            {"site": end - 4, "hex": image[end - 4:end + 4].hex()},
        ],
        "abi": {"arguments": "near CARSTATE/SIMD pointers, byte transmission, three signed longs, short angle",
                "return": "void, far RET",
                "fields": "CARSTATE stores through byte 0xCF; four wheel-vector array elements"},
        "evidence_class": "pristine named far entry, complete mapped interval, direct CFG, no calls or relocations",
        "boundary_proof": "Original seg001 named far proc entry and next mapped boundary; verified 618-byte interval, anchored neighboring bytes and complete direct CFG with one unreachable NOP; fresh exact single-public compiler object independently checked",
    }
    path = ROOT / "layout/function-evidence.json"
    doc = read_json(path)
    assert doc["oracle_sha256"] == p["oracle_load_sha256"]
    assert not any(f["name"] == p["task"] or f.get("stable_id") == p["id"] for f in doc["functions"])
    doc["functions"].append(overlay)
    path.write_bytes((json.dumps(doc, indent=2) + "\n").encode())
    assert p["task"] in reviewed_functions(image)
    print(f"Installed {p['task']} overlay: {len(rows)} instructions, {len(padding)} NOP; fresh complete object exact")


if __name__ == "__main__":
    main()
