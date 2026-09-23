"""Small isolated compiler receipt helper for endurance-001 only."""
import argparse
import json
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
from common import identity, read_json, write_json
from compiler import CompileFailure, compile_source
from diagnostics import diagnose
from mz import MZ
from object_probe import read_object
from oracle import verify


def run(case_id, source_path, prediction_path, case_dir, profile=None, flags=None):
    design = read_json(ROOT / "recovery/corpus/endurance-001/design.json")
    case = next(item for item in design["cases"] if item["id"] == case_id)
    case_dir = Path(case_dir).resolve()
    source_path = Path(source_path).resolve()
    prediction_path = Path(prediction_path).resolve()
    assert case_dir.is_relative_to(ROOT / "recovery/corpus/endurance-001")
    assert source_path.is_relative_to(case_dir) and prediction_path.is_relative_to(case_dir)
    source = source_path.read_bytes()
    prediction = read_json(prediction_path)
    assert prediction.get("prediction") and prediction.get("falsifier"), "Write prediction and falsifier before compiling."

    recipe_path = ROOT / "recipes" / f"{case['name']}.json"
    recipe = read_json(recipe_path) if recipe_path.is_file() else None
    verified = verify(write=False)
    image = MZ.parse(verified[1]).load_image(verified[1])
    if recipe and all(key in recipe for key in ("start", "end", "target")):
        start, end, expected = recipe["start"], recipe["end"], recipe["target"]
    else:
        card = read_json(ROOT / case["card"])
        evidence = card["evidence"]
        start, end = evidence["start"], evidence["end"]
        expected = {"size": evidence["size"], "sha256": evidence["sha256"]}
    target = image[start:end]
    assert identity(target) == {"size": expected["size"], "sha256": expected["sha256"]}, "Pristine target differs from frozen card/recipe."

    trials = case_dir / "trials"
    seq = 1
    while (trials / f"{seq:03d}").exists():
        seq += 1
    out = trials / f"{seq:03d}"
    out.mkdir(parents=True, exist_ok=False)
    (out / "candidate.c").write_bytes(source)
    (out / "target.bin").write_bytes(target)
    write_json(out / "prediction.json", prediction)
    selected_profile = profile or (recipe or {}).get("profile") or "msc510-medium"
    record = {
        "case_id": case_id,
        "function": case["name"],
        "iteration": seq,
        "meaningful_hypothesis_number": prediction.get("meaningful_hypothesis_number"),
        "candidate_path": str((out / "candidate.c").relative_to(ROOT)).replace("\\", "/"),
        "source": identity(source),
        "target": {"start": start, "end": end, **identity(target)},
        "expected_target": expected,
        "profile": selected_profile,
        "flags_override": flags,
        "prediction": prediction,
        "started_utc": datetime.now(timezone.utc).isoformat(),
    }
    try:
        obj, receipt = compile_source(source, selected_profile, flags)
    except CompileFailure as error:
        receipt = error.receipt
        work_value = receipt.get("work_directory")
        work = Path(work_value) if work_value else None
        raw_object_path = work / "UNIT.OBJ" if work else None
        log_path = work / "compiler.log" if work else None
        if raw_object_path and raw_object_path.is_file():
            shutil.copyfile(raw_object_path, out / "object.obj")
            record["raw_object"] = identity(raw_object_path.read_bytes())
        if log_path and log_path.is_file():
            shutil.copyfile(log_path, out / "compiler.log")
        record.update(status="COMPILE_FAILURE", compiler_invoked=bool(error.receipt.get("compiler_stdout_sha256")), category=error.category,
                      error=str(error), receipt=receipt)
        write_json(out / "trial.json", record)
        return record

    work = Path(receipt["work_directory"])
    raw_obj = (work / "UNIT.OBJ").read_bytes()
    shutil.copyfile(work / "UNIT.OBJ", out / "object.obj")
    shutil.copyfile(work / "UNIT.C", out / "staged-source.C")
    shutil.copyfile(work / "compiler.log", out / "compiler.log")
    segment_name = (recipe or {}).get("object_segment", "UNIT_TEXT")
    payload = obj.segment_bytes(segment_name)
    (out / "code.bin").write_bytes(payload)
    segment_ids = {}
    for name, data in obj.segments.items():
        segment_ids[name] = identity(data)
        (out / ("segment-" + name.replace("/", "_").replace("\\", "_") + ".bin")).write_bytes(data)
    diagnostic = diagnose(target, payload, receipt, obj.linker_fixups, segment=segment_name)
    full_path = diagnostic.get("full_diagnostic")
    if full_path:
        source_full = Path(full_path)
        if source_full.is_file():
            shutil.copyfile(source_full, out / "full-diagnostic.json")
            diagnostic["full_diagnostic"] = str((out / "full-diagnostic.json").relative_to(ROOT)).replace("\\", "/")
    write_json(out / "receipt.json", receipt)
    record.update(
        status="COMPILED",
        compiler_invoked=True,
        receipt=receipt,
        object=identity(raw_obj),
        segments=segment_ids,
        segment_lengths=obj.segment_lengths,
        segment_definitions=obj.segment_defs,
        groups=obj.groups,
        publics=obj.publics,
        externals=obj.externals,
        fixups=obj.linker_fixups,
        code=identity(payload),
        emitted_extent_matches=(len(payload) == len(target)),
        unbound_complete_code_matches=(payload == target),
        compact_diagnosis={k: v for k, v in diagnostic.items() if k != "full_diagnostic"},
        full_diagnostic_path=str((out / "full-diagnostic.json").relative_to(ROOT)).replace("\\", "/") if (out / "full-diagnostic.json").exists() else None,
        diagnostic_engine_sha256=(json.loads((out / "full-diagnostic.json").read_text(encoding="utf-8")).get("engine_sha256") if (out / "full-diagnostic.json").exists() else None),
        completed_utc=datetime.now(timezone.utc).isoformat(),
    )
    write_json(out / "trial.json", record)
    return record


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--case", required=True)
    parser.add_argument("--source", required=True)
    parser.add_argument("--prediction", required=True)
    parser.add_argument("--case-dir", required=True)
    parser.add_argument("--profile")
    parser.add_argument("--flags", help="Research-only exact compiler flag list, space-separated")
    args = parser.parse_args()
    print(json.dumps(run(args.case, args.source, args.prediction, args.case_dir, args.profile,
                         args.flags.split() if args.flags else None), indent=2, sort_keys=True))
