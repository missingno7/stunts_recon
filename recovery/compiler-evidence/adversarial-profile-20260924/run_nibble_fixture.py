"""Run the two-condition version fixture with raw OMF receipts."""

import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
OUT = ROOT / "build/private/profile-study/nibble-fixture"
sys.path.insert(0, str(ROOT / "tools"))
from common import sha  # noqa: E402
from compiler import compile_source  # noqa: E402


def identity(data):
    return {"size": len(data), "sha256": sha(data)}


def main():
    plan = json.loads((HERE / "nibble-fixture-plan.json").read_text())
    if len(plan["conditions"]) != plan["process_ceiling"]:
        raise ValueError("fixture process ceiling mismatch")
    source = (HERE / plan["source"]).read_bytes()
    OUT.mkdir(parents=True, exist_ok=True)
    results = []
    for condition in plan["conditions"]:
        name = condition["profile"]
        run_dir = OUT / name
        run_dir.mkdir(exist_ok=True)
        receipt_path = run_dir / "result.json"
        if receipt_path.exists():
            prior = json.loads(receipt_path.read_text())
            if prior["condition"] != condition or prior["source"] != identity(source):
                raise ValueError("incompatible previous fixture receipt")
            results.append(prior)
            continue
        obj, receipt = compile_source(source, name, flags=condition["flags"])
        work = Path(receipt["work_directory"])
        for filename in ("UNIT.C", "UNIT.OBJ", "compiler.log"):
            shutil.copyfile(work / filename, run_dir / filename)
        code = obj.segments["UNIT_TEXT"]
        (run_dir / "UNIT_TEXT.bin").write_bytes(code)
        result = {"condition": condition, "source": identity(source), "receipt": receipt,
                  "object": identity((run_dir / "UNIT.OBJ").read_bytes()),
                  "code": identity(code), "code_hex": code.hex(),
                  "segment_defs": obj.segment_defs, "publics": obj.publics,
                  "externals": obj.externals, "groups": obj.groups,
                  "ordered_fixups": obj.linker_fixups, "strict_acceptance": "NOT_APPLICABLE"}
        receipt_path.write_text(json.dumps(result, indent=2) + "\n")
        results.append(result)
    (OUT / "summary.json").write_text(json.dumps({"plan_sha256": sha((HERE / "nibble-fixture-plan.json").read_bytes()),
        "source": identity(source), "results": [{"condition": x["condition"], "code": x["code"],
        "publics": x["publics"], "ordered_fixups": x["ordered_fixups"]} for x in results]}, indent=2) + "\n")


if __name__ == "__main__":
    main()
