"""One-off reviewed pristine extent/CFG overlay for rect_is_adjacent.

Run only after comparing the packet, original ASM entry, and exact research
object. The generic packet remains research-only; this promotes just its
independently checked instruction/flow evidence, not the candidate bytes.
"""

import json
import sys
from pathlib import Path

from research_packet import packet

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
from common import read_json  # noqa: E402
from function_evidence import reviewed_functions  # noqa: E402
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402


def main():
    p = packet("rect_is_adjacent")
    assert p["id"] == "load_169d0" and p["extent"]["size"] == 130
    assert not (p["cfg"]["flow_risks"] or p["cfg"]["unreachable_nonpadding"]
                or p["calls"] or p["mz_relocation_sites"]
                or p["researched_capability_blockers"] or p["other_mapped_entries_inside"])
    start, end = p["extent"]["start"], p["extent"]["end"]
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
    assert len(padding) == 2 and all(by[at] == b"\x90" for at in padding)
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    overlay = {
        "name": p["task"], "stable_id": p["id"], "start": start, "end": end,
        "size": p["extent"]["size"], "sha256": p["extent"]["sha256"],
        "relocation_sites": [], "disassembly": rows,
        "reachable_instruction_offsets": sorted(reached), "padding_offsets": padding,
        "boundary_anchors": [
            {"site": start - 4, "hex": image[start - 4:start + 4].hex()},
            {"site": end - 4, "hex": image[end - 4:end + 4].hex()},
        ],
        "abi": {"arguments": "two near RECTANGLE pointers at BP+6 and BP+8",
                "return": "word AX, far RET",
                "fields": "four word fields at offsets left0/right2/top4/bottom6"},
        "evidence_class": "pristine named far entry, complete mapped interval, direct CFG, no calls or relocations",
        "boundary_proof": "Original seg006 named far proc entry and next mapped entry; unique verified interval, pristine anchored boundary bytes and complete direct CFG with two unreachable NOPs; exact single-public compiler object independently checked",
    }
    path = ROOT / "layout/function-evidence.json"
    doc = read_json(path)
    assert doc["oracle_sha256"] == p["oracle_load_sha256"]
    assert not any(f["name"] == p["task"] or f.get("stable_id") == p["id"] for f in doc["functions"])
    doc["functions"].append(overlay)
    path.write_bytes((json.dumps(doc, indent=2) + "\n").encode())
    assert p["task"] in reviewed_functions(image)
    print(f"Installed independently checked {p['task']} overlay: {len(rows)} instructions, {len(padding)} NOPs")


if __name__ == "__main__":
    main()
