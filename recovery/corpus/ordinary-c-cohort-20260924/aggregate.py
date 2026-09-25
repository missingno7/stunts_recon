"""Read-only accounting for this research cohort's archived compiler batches.

Run from the repository root. Does not change production attempts or ledger.
"""

import hashlib
import json
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
COHORT = "ordinary-c-cohort-20260924"
TASKS = ("rect_is_adjacent", "mat_multiply", "heapsort_by_order", "file_get_shape2d", "mat_mul_vector", "init_carstate_from_simd")
TASK_PATHS = {"rect_is_adjacent": ("rect_is_adjacent", "load_169d0"),
              "mat_multiply": ("mat_multiply", "load_229f2"),
              "heapsort_by_order": ("heapsort_by_order", "load_26be8"),
              "file_get_shape2d": ("file_get_shape2d", "load_2265b"),
              "mat_mul_vector": ("mat_mul_vector", "load_228ee"),
              "init_carstate_from_simd": ("init_carstate_from_simd", "load_06898")}
sys.path.insert(0, str(ROOT / "tools"))
from object_probe import read_object  # noqa: E402


def digest(value):
    raw = json.dumps(value, sort_keys=True, separators=(",", ":")).encode()
    return hashlib.sha256(raw).hexdigest()


def named_fixup(value):
    # OMF indices and record positions are retained in raw receipts but are
    # metadata, not a new target-level output when symbol/role/order agree.
    return {key: item for key, item in value.items()
            if key not in ("frame_index", "target_index", "record_offset", "record_index")}


def semantic_key(outcome, profile, archive_dir):
    declarations = [{key: value for key, value in entry.items()
                     if key not in ("index", "overlay_index", "frame_index")}
                    for entry in outcome["declarations"]]
    objects = [entry["path"] for entry in outcome["archived_compiler_artifacts"]
               if entry["path"].endswith(".obj")]
    if len(objects) != 1:
        raise ValueError("missing single archived complete object")
    parsed = read_object((archive_dir / objects[0]).read_bytes())
    segment_payloads = {name: {"size": len(payload), "sha256": hashlib.sha256(payload).hexdigest()}
                        for name, payload in parsed.segments.items()}
    return digest({"profile": profile, "code": outcome["code"],
                   "segments": segment_payloads,
                   "declarations": declarations, "publics": outcome["publics"],
                   "externals": outcome["externals"],
                   "ordered_named_fixups": [named_fixup(item) for item in outcome["fixups"]]})


def collect():
    rows = defaultdict(list)
    for task in TASKS:
        for task_path in TASK_PATHS[task]:
          parent = ROOT / "build/private/research-batches" / task_path
          for plan_path in sorted(parent.glob("*/plan.json")):
            plan = json.loads(plan_path.read_text())
            manifest = plan["manifest"]
            if manifest.get("cohort_id") != COHORT:
                continue
            result_path = plan_path.with_name("results.json")
            if not result_path.exists():
                continue
            result = json.loads(result_path.read_text())
            if result["task"] != task_path or result["authority"] != "RESEARCH_ONLY":
                raise ValueError("task/authority mismatch: " + str(result_path))
            rows[task].append((int(manifest["round"]), str(result_path.relative_to(ROOT)), plan, result))
    report = {"schema": 1, "authority": "READ_ONLY_RECEIPT_ACCOUNTING",
              "cohort_id": COHORT, "tasks": {}, "shared_hypothesis_compiler_processes": 0,
              "verification_recompiles": "NOT_IN_RESEARCH_BATCH_RECEIPTS"}
    for task in TASKS:
        batches = sorted(rows[task], key=lambda item: (item[0], item[1]))
        seen = {}
        hypotheses = processes = successes = failures = 0
        precompiler_rejections = postcompiler_object_rejections = other_failures = 0
        batch_rows = []
        class_rows = []
        for round_number, path, plan, result in batches:
            if result["compiler_processes"] != sum(x.get("compiler_processes", 0)
                                                    for x in result["outcomes"]):
                raise ValueError("per-batch process count mismatch: " + path)
            hypotheses += len(result["outcomes"])
            processes += result["compiler_processes"]
            batch_rows.append({"round": round_number, "path": path,
                               "complete": result["complete"],
                               "hypotheses": len(result["outcomes"]),
                               "compiler_processes": result["compiler_processes"]})
            for outcome in result["outcomes"]:
                if outcome["status"] != "COMPILED":
                    failures += 1
                    if outcome.get("category") == "UNSUPPORTED_SOURCE" and not outcome["compiler_processes"]:
                        precompiler_rejections += 1
                    elif outcome.get("category") == "UNSUPPORTED_OBJECT" and outcome["compiler_processes"]:
                        postcompiler_object_rejections += 1
                    else:
                        other_failures += 1
                    continue
                successes += 1
                key = semantic_key(outcome, plan["profile"], ROOT / Path(path).parent)
                if key in seen:
                    continue
                seen[key] = (round_number, outcome["id"], path)
                class_rows.append({"semantic_key": key, "first_round": round_number,
                                   "first_hypothesis": outcome["id"], "receipt": path,
                                   "code": outcome["code"], "extent_equal": outcome["extent_equal"],
                                   "literal_code_equal": outcome["literal_code_equal"],
                                   "ordered_fixups": [named_fixup(x) for x in outcome["fixups"]]})
        report["tasks"][task] = {"rounds": len(set(x["round"] for x in batch_rows)),
                                 "batches": batch_rows, "hypotheses": hypotheses,
                                 "hypothesis_compiler_processes": processes,
                                 "non_compiled_outcomes": failures,
                                 "precompiler_rejections": precompiler_rejections,
                                 "postcompiler_object_rejections": postcompiler_object_rejections,
                                 "other_failures": other_failures, "compiled": successes,
                                 "effective_output_classes": len(seen),
                                 "duplicate_effective_outputs": successes-len(seen),
                                 "classes": class_rows}
        report["shared_hypothesis_compiler_processes"] += processes
    return report


if __name__ == "__main__":
    print(json.dumps(collect(), indent=2))
