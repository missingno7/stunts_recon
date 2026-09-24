"""Run the five predeclared compiler-profile heldouts; research authority only."""

import json
import shutil
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
OUT = ROOT / "build/private/profile-study/heldout"
sys.path.insert(0, str(ROOT / "tools"))

from common import sha  # noqa: E402
from compiler import CompileFailure, compile_source  # noqa: E402
from mz import MZ  # noqa: E402
from oracle import verify as verify_oracle  # noqa: E402


def identity(data):
    return {"size": len(data), "sha256": sha(data)}


def ranges(changes):
    if not changes:
        return []
    result = []
    first = previous = changes[0]
    for item in changes[1:]:
        if item != previous + 1:
            result.append([first, previous + 1])
            first = item
        previous = item
    result.append([first, previous + 1])
    return result


def records(raw):
    result = []
    offset = 0
    while offset + 3 <= len(raw):
        kind = raw[offset]
        length = struct.unpack_from("<H", raw, offset + 1)[0]
        end = offset + 3 + length
        if end > len(raw):
            raise ValueError("truncated OMF record")
        result.append({"offset": offset, "kind": kind, "length": length, "sha256": sha(raw[offset:end])})
        offset = end
    if offset != len(raw):
        raise ValueError("trailing OMF bytes")
    return result


def oracle_from_card(stable_id):
    card = json.loads((ROOT / "recovery/cards" / (stable_id + ".json")).read_text())
    raw = bytes.fromhex("".join(row["bytes"] for row in card["disassembly"]))
    evidence = card["evidence"]
    if identity(raw) != {"size": evidence["size"], "sha256": evidence["sha256"]}:
        # Some cards omit the disassembly. Fall back to the separately locked
        # original load image, then check the card's reviewed interval hash.
        _, unpacked, _, _ = verify_oracle(write=False)
        raw = MZ.parse(unpacked).load_image(unpacked)[evidence["start"]:evidence["end"]]
    if identity(raw) != {"size": evidence["size"], "sha256": evidence["sha256"]}:
        raise ValueError("reviewed target differs from locked pristine interval")
    return raw, evidence


def main():
    plan = json.loads((HERE / "heldout-plan.json").read_text())
    original = json.loads((HERE / "plan.json").read_text())
    if len(plan["conditions"]) != plan["process_ceiling"]:
        raise ValueError("condition count differs from frozen ceiling")
    OUT.mkdir(parents=True, exist_ok=True)
    results = []
    for condition in plan["conditions"]:
        source = (HERE / "sources" / condition["source"]).read_bytes()
        if identity(source) != {k: original["source_identities"][condition["source"]][k] for k in ("size", "sha256")}:
            raise ValueError("frozen source identity changed")
        oracle, evidence = oracle_from_card(condition["card"])
        run_dir = OUT / condition["id"]
        run_dir.mkdir(exist_ok=True)
        prior_path = run_dir / "result.json"
        if prior_path.exists():
            prior = json.loads(prior_path.read_text())
            if (prior["condition"] != condition or prior["source"] != identity(source)
                    or prior["oracle"] != identity(oracle)
                    or prior.get("outcome") != "COMPILED_AND_PARSED"
                    or not (run_dir / "UNIT.OBJ").exists()):
                raise ValueError("incompatible prior heldout receipt")
            results.append(prior)
            continue
        (run_dir / "source.c").write_bytes(source)
        (run_dir / "oracle.bin").write_bytes(oracle)
        result = {"condition": condition, "source": identity(source), "oracle": identity(oracle),
                  "target_interval": [evidence["start"], evidence["end"]],
                  "strict_acceptance": "NOT_EVALUATED"}
        began = time.perf_counter()
        try:
            obj, receipt = compile_source(source, condition["profile"], flags=condition["flags"])
            result["outcome"] = "COMPILED_AND_PARSED"
            result["receipt"] = receipt
            work = Path(receipt["work_directory"])
            for filename in ("UNIT.OBJ", "UNIT.C", "compiler.log"):
                if (work / filename).exists():
                    shutil.copyfile(work / filename, run_dir / filename)
            raw = (run_dir / "UNIT.OBJ").read_bytes()
            code = obj.segments.get("UNIT_TEXT", b"")
            (run_dir / "UNIT_TEXT.bin").write_bytes(code)
            differences = [i for i in range(max(len(code), len(oracle)))
                           if (code[i] if i < len(code) else None) != (oracle[i] if i < len(oracle) else None)]
            result.update({"object": identity(raw), "omf_records": records(raw),
                           "code": identity(code), "code_hex": code.hex(),
                           "segment_defs": obj.segment_defs,
                           "segment_lengths": {name: len(data) for name, data in obj.segments.items()},
                           "publics": obj.publics, "externals": obj.externals,
                           "groups": obj.groups, "ordered_fixups": obj.linker_fixups,
                           "oracle_equal": code == oracle, "oracle_difference_ranges": ranges(differences)})
        except CompileFailure as exc:
            result.update({"outcome": exc.category, "error": str(exc), "receipt": exc.receipt})
            if exc.receipt and exc.receipt.get("work_directory"):
                work = Path(exc.receipt["work_directory"])
                for filename in ("UNIT.OBJ", "UNIT.C", "compiler.log"):
                    if (work / filename).exists():
                        shutil.copyfile(work / filename, run_dir / filename)
        result["desktop_elapsed_seconds"] = time.perf_counter() - began
        (run_dir / "result.json").write_text(json.dumps(result, indent=2) + "\n")
        results.append(result)
        (OUT / "summary.json").write_text(json.dumps({"authority": "RESEARCH_ONLY",
            "plan_sha256": sha((HERE / "heldout-plan.json").read_bytes()),
            "completed": len(results), "results": [{key: row.get(key) for key in
                ("condition", "outcome", "code", "oracle_equal", "oracle_difference_ranges",
                 "desktop_elapsed_seconds", "object", "ordered_fixups", "externals")}
                for row in results]}, indent=2) + "\n")
    for a, b in (("file_huge_msc510_o", "file_huge_msc500_o"),
                 ("file_linear_msc510_o", "file_linear_msc500_o")):
        left = next(row for row in results if row["condition"]["id"] == a)
        right = next(row for row in results if row["condition"]["id"] == b)
        if "code_hex" not in left or "code_hex" not in right:
            continue
        x, y = bytes.fromhex(left["code_hex"]), bytes.fromhex(right["code_hex"])
        changed = [i for i in range(max(len(x), len(y)))
                   if (x[i] if i < len(x) else None) != (y[i] if i < len(y) else None)]
        (OUT / (a + "-vs-" + b + ".json")).write_text(json.dumps({
            "same_source_version_contrast": [a, b], "code_equal": x == y,
            "different_ranges": ranges(changed), "left_code": identity(x), "right_code": identity(y),
            "left_fixups": left["ordered_fixups"], "right_fixups": right["ordered_fixups"]}, indent=2) + "\n")


if __name__ == "__main__":
    main()
