#!/usr/bin/env python3
"""Inventory Stunts port transformations and check the production view contracts.

Reports are derived data. Finite oracle tests and lexical risk signals do not
prove an entire routine equivalent. Historical acceptance remains separate.
"""
from __future__ import annotations

import argparse
import ast
from collections import Counter
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "port"))
sys.path.insert(0, str(ROOT / "tools/porting"))
from host_probe_declarations import (mask_comments, top_level_spans,
                                    function_headers, source_parser_text, cdecls)
from game_abi import host_view_contracts

DEFAULT_WORK = ROOT / "build/porting/semantic-audit"
REGISTRY = ROOT / "port/semantic-contracts.json"


def digest(text):
    return hashlib.sha256(text.encode("latin-1")).hexdigest()


def functions(text):
    """Use the existing declaration scanner, preserving source offsets."""
    code = mask_comments(text)
    result = {}
    for a, b, kind in top_level_spans(text):
        if kind != "function":
            continue
        brace = code.find("{", a, b)
        headers = function_headers(text[a:b])
        if brace >= 0 and len(headers) == 1:
            name, signature = headers[0]
            result[name] = {"signature": signature,
                            "line": text.count("\n", 0, brace) + 1,
                            "start_line": text.count("\n", 0, a) + 1,
                            "body": text[a:b]}
    return result


def declarations(text, source):
    unit = cdecls.Unit(source_parser_text(text), source)
    globals_ = {name for name, rows in unit.decls.items()
                if any(row["type"].get("k") != "fn" for row in rows)}
    return globals_, unit


def normalize(text):
    return re.sub(r"\s+", "", mask_comments(text))


def prepare(work, gcc):
    # A distinct module name avoids tools/build.py and unittest import clashes.
    spec = importlib.util.spec_from_file_location("stunts_sdl_build", ROOT / "port/build.py")
    builder = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(builder)
    _, report = builder.build_game_objects(gcc, work_dir=work, prepare_only=True)
    return report["sources"]


PROTOTYPES = {
    "port_call_read_line": "int16_t (*)(char *, int16_t, int16_t, int16_t, int32_t)",
    "file_read_nofatal": "void *(*)(const char *, void *)",
    "file_read_fatal": "void *(*)(const char *, void *)",
    "file_find": "char *(*)(const char *)",
    "mmgr_op_unk": "void *(*)(void *)",
    "nopsub_37750": "void (*)(uint16_t, void (*)(int16_t))",
    "vector_op_unk2": "int16_t (*)(struct VECTOR *)",
}


def invariant_text(source):
    checks = [
        '_Static_assert((I16)-1 < 0 && (I16S)-1 < 0 && (I32)-1 < 0, "signed DOS scalars");',
        '_Static_assert((U16)-1 > 0 && (U32)-1 > 0, "unsigned DOS scalars");',
        '_Static_assert(sizeof(I8) == 1 && (I8)-1 < 0 && sizeof(I8S) == 1 && (I8S)-1 < 0 && sizeof(U8) == 1 && (U8)-1 > 0, "DOS bytes");',
        '_Static_assert(sizeof(st_near_data_offset) == 2, "near offsets remain words");',
    ]
    for name, expected in PROTOTYPES.items():
        checks.append(f'_Static_assert(__builtin_types_compatible_p(__typeof__(&{name}), '
                      f'{expected}), "native prototype {name}");')
    if source == "src/obj_seg004.c":
        for name in ("pts_set", "secondveccar", "veccar", "car_dvecs", "veco",
                     "secondoveh", "opponent_pointc", "veh_od"):
            checks.append(f'_Static_assert(sizeof({name}) == 6 * 6, "wheel group {name}");')
        for name in ("pos_pt", "ancv2", "ctrmesh", "g_op_carvector2"):
            checks.append(f'_Static_assert(sizeof({name}) == 6, "wheel origin {name}");')
    if source == "src/obj_seg006.c":
        checks.extend([
            '_Static_assert(sizeof(poly_link_list) == 401 * 2, "explicit polygon sentinel extent");',
            '_Static_assert(sizeof(inverse_power_of_two_table) == 32 * 4, "reciprocal dword table");',
        ])
    return "\n".join(checks) + "\n"


def check_contracts(rows, work, gcc):
    env = os.environ.copy()
    env["PATH"] = str(gcc.parent) + os.pathsep + env.get("PATH", "")
    results = []
    for row in rows:
        source = row["source"]
        original = Path(row["overlay"]).read_text(encoding="latin-1")
        generated = work / "checks" / Path(source).name
        generated.parent.mkdir(parents=True, exist_ok=True)
        generated.write_text(original + "\n" + invariant_text(source), encoding="latin-1")
        command = [str(gcc), "-std=gnu11", "-fsyntax-only", "-DPORT_BUILD=1",
                   "-Wno-error=implicit-int", "-Wno-error=incompatible-pointer-types",
                   "-Werror=int-conversion", "-Werror=type-limits",
                   "-include", str(ROOT / "tools/porting/host/compat.h"),
                   "-include", row["config"], "-I", str(work / "include"),
                   "-I", str(ROOT / "port"), "-I", str(ROOT / "tools/porting/host/include"),
                   "-I", str(ROOT / "include"), "-I", str(ROOT / "src"), str(generated)]
        ran = subprocess.run(command, env=env, capture_output=True, text=True,
                             errors="replace", timeout=90)
        errors = [line for line in ran.stderr.splitlines() if "error:" in line]
        (work / "checks" / (Path(source).stem + ".log")).write_text(
            ran.stdout + ran.stderr, encoding="utf-8")
        results.append({"source": source, "status": "pass" if ran.returncode == 0 else "fail",
                        "assertions": original.count("_Static_assert(") +
                                      invariant_text(source).count("_Static_assert("),
                        "errors": errors,
                        "warnings": [line for line in ran.stderr.splitlines() if "warning:" in line]})
    return results


def registry_checks(registry):
    """Reject invented evidence, dangling function claims and duplicate IDs."""
    errors, ids = [], set()
    for contract in registry["contracts"]:
        cid = contract["id"]
        if cid in ids:
            errors.append(f"duplicate contract {cid}")
        ids.add(cid)
        if not contract.get("scope") or not contract.get("observables"):
            errors.append(f"missing claim boundary for {cid}")
        for path in contract.get("tests", []):
            file, _, method = path.partition("::")
            if not (ROOT / file).is_file():
                errors.append(f"missing evidence test {path}")
            elif method and method.rsplit('.', 1)[-1] not in {
                    node.name for node in ast.walk(ast.parse((ROOT / file).read_text(encoding="utf-8")))
                    if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef))}:
                errors.append(f"missing evidence method {path}")
    return errors


def scoped_claims(claims, name, sources):
    """An expression or shared path never becomes a routine isolation proof."""
    return [c for c in claims.get(name, [])
            if set(c.get("source_scope", [])) & set(sources)]


def attach_claims(row, evidence):
    row["evidence"] = [c["id"] for c in evidence]
    row["evidence_claims"] = [{"contract": c["id"], "status": c["status"],
        "scope_level": c["evidence_scope"], "input_domain": c["scope"],
        "observables": c["observables"], "claim_limit": c["claim_limit"],
        "tests": c["tests"]} for c in evidence]
    # Coverage here is scoped. Whole-function coverage can only come from a
    # direct routine test, never from a composite path or instruction window.
    row["coverage"] = sorted({c["status"] + (
        " (" + c["evidence_scope"] + ")" if c["evidence_scope"] not in
        {"direct-routine-cases", "listed-routine-cases", "unverified"} else "")
        for c in evidence})
    row["whole_domain_equivalence"] = "not-established"


def audit(work=DEFAULT_WORK, gcc=Path(r"C:\msys64\mingw32\bin\gcc.exe"), compile_checks=True):
    from semantic_detectors import scan_function, scan_spans
    registry = json.loads(REGISTRY.read_text(encoding="utf-8"))
    work.mkdir(parents=True, exist_ok=True)
    rows = prepare(work, gcc)
    checks = check_contracts(rows, work, gcc) if compile_checks else []
    errors = registry_checks(registry)
    all_globals, original_functions, native_functions = set(), {}, {}
    global_declarations = {}
    parsed_sources = []
    for row in rows:
        source = row["source"]
        original = (ROOT / source).read_text(encoding="latin-1")
        native = Path(row["overlay"]).read_text(encoding="latin-1")
        globals_, unit = declarations(original, source)
        all_globals.update(globals_)
        for name in globals_:
            for declaration in unit.decls[name]:
                if declaration["type"].get("k") != "fn":
                    global_declarations.setdefault(name, []).append({
                        "source": source, "type": declaration["type"],
                        "defined": declaration.get("defined", False)})
        original_functions[source] = functions(original)
        native_functions[source] = functions(native)
        parsed_sources.append({"source": source, "historical_sha256": digest(original),
                               "portable_sha256": digest(native),
                               "declaration_count": len(unit.decls)})
    for path in sorted((ROOT / "port").glob("*")):
        if path.suffix not in {".c", ".h"}:
            continue
        source = path.relative_to(ROOT).as_posix()
        text = path.read_text(encoding="latin-1")
        globals_, unit = declarations(text, source)
        all_globals.update(globals_)
        for name in globals_:
            for declaration in unit.decls[name]:
                if declaration["type"].get("k") != "fn":
                    global_declarations.setdefault(name, []).append({
                        "source": source, "type": declaration["type"],
                        "defined": declaration.get("defined", False)})
        native_functions[source] = functions(text)
        parsed_sources.append({"source": source, "portable_sha256": digest(text)})
    rename = registry.get("renames", {})
    claims = {}
    for item in registry["contracts"]:
        for name in item.get("routines", []):
            claims.setdefault(name, []).append(item)
            if rename.get(name, name) != name:
                claims.setdefault(rename[name], []).append(item)
    inventory, risks = [], []
    matched = set()
    check_by_source = {c["source"]: c for c in checks}
    for source, routines in original_functions.items():
        for name, routine in routines.items():
            native_name = rename.get(name, name)
            counterparts = [(s, fs[native_name]) for s, fs in native_functions.items()
                            if native_name in fs and (s == source or s.startswith("port/"))]
            row = {"id": source + "::" + name, "historical_name": name,
                   "historical_source": source, "historical_line": routine["line"],
                   "historical_signature": routine["signature"],
                   "historical_body_sha256": digest(routine["body"]),
                   "portable_counterparts": [], "evidence": []}
            dependent = sorted(set(re.findall(r"\b[A-Za-z_]\w*\b", routine["body"])) & all_globals)
            row["global_dependency_candidates"] = dependent
            row["dependency_basis"] = "identifier intersection with parsed file-scope declarations; local shadows require review"
            row["aggregate_dependencies"] = sorted(set(re.findall(
                r"\b(?:struct|union)\s+(\w+)", routine["body"])))
            for s, fn in counterparts:
                matched.add((s, native_name))
                row["portable_counterparts"].append({"source": s, "name": native_name,
                    "line": fn["line"], "signature": fn["signature"],
                    "body_sha256": digest(fn["body"]),
                    "global_dependency_candidates": sorted(set(re.findall(
                        r"\b[A-Za-z_]\w*\b", fn["body"])) & all_globals),
                    "aggregate_dependencies": sorted(set(re.findall(
                        r"\b(?:struct|union)\s+(\w+)", fn["body"]))),
                    "body_changed": normalize(routine["body"]) != normalize(fn["body"])})
                risks.extend(scan_function(s, native_name, fn["body"], fn["start_line"]))
            attach_claims(row, scoped_claims(claims, name, [p["source"] for p in row["portable_counterparts"]]))
            if not row["coverage"]:
                row["coverage"] = ["compile-only" if counterparts and
                    check_by_source.get(source, {}).get("status") == "pass" else "unverified/high-risk"]
            # Contract coverage remains scoped; uncovered signals stay visible.
            row["compile_contracts"] = check_by_source.get(source, {}).get("status", "not-run")
            inventory.append(row)
    for source, routines in native_functions.items():
        for name, fn in routines.items():
            if (source, name) not in matched:
                row = {"id": source + "::" + name, "historical_name": None,
                    "portable_counterparts": [{"source": source, "name": name,
                        "line": fn["line"], "signature": fn["signature"],
                        "body_sha256": digest(fn["body"]),
                        "global_dependency_candidates": sorted(set(re.findall(
                            r"\b[A-Za-z_]\w*\b", fn["body"])) & all_globals),
                        "aggregate_dependencies": sorted(set(re.findall(
                            r"\b(?:struct|union)\s+(\w+)", fn["body"]))) }],
                    "dependency_basis": "identifier intersection with parsed file-scope declarations; local shadows require review",
                    "coverage": [], "evidence": []}
                attach_claims(row, scoped_claims(claims, name, [source]))
                if not row["coverage"]:
                    row["coverage"] = ["unverified/high-risk"]
                inventory.append(row)
                risks.extend(scan_function(source, name, fn["body"], fn["start_line"]))
    for check in checks:
        for warning in check["warnings"]:
            match = re.search(r":(\d+):\d+: warning:", warning)
            line = int(match.group(1)) if match else None
            function = "<declaration>"
            in_definition = bool(match and Path(warning[:match.start()]).name == Path(check["source"]).name)
            for name, fn in native_functions.get(check["source"], {}).items() if in_definition else []:
                if line and fn["start_line"] <= line <= fn["start_line"] + fn["body"].count("\n"):
                    function = name
                    break
            risks.append({"class": "compiler_pointer_abi_warning" if "pointer" in warning else
                          "compiler_declaration_warning", "source": check["source"],
                          "function": function, "line": line, "snippet": warning,
                          "evidence_kind": "fresh_production_compiler_diagnostic",
                          "status": "requires-review"})
    from semantic_mapping import build_mapping
    definitions = {}
    for source, fs in native_functions.items():
        for name, fn in fs.items():
            definitions.setdefault(name, []).append({"source": source, "name": name,
                                                   "signature": fn["signature"], "line": fn["line"]})
    historical_mapping = build_mapping(ROOT, definitions, rename)
    by_portable = {p["source"] + "::" + p["name"]: r for r in inventory for p in r["portable_counterparts"]}
    for row in historical_mapping:
        row["coverage_candidates"] = [{"implementation": p["source"] + "::" + p["name"],
            "coverage": by_portable[p["source"] + "::" + p["name"]]["coverage"],
            "evidence": by_portable[p["source"] + "::" + p["name"]]["evidence"]}
            for p in row["portable_names"]]
    span_findings = scan_spans(ROOT, rows)
    risk_by_function = {}
    for finding in risks:
        risk_by_function.setdefault(finding["source"] + "::" + finding["function"], set()).add(finding["class"])
    for row in inventory:
        row["risk_classes"] = sorted(set().union(*(risk_by_function.get(
            p["source"] + "::" + p["name"], set()) for p in row["portable_counterparts"])))
        if row["risk_classes"] and row["coverage"] == ["compile-only"]:
            row["coverage"].append("unverified/high-risk")
    errors.extend(f["message"] for f in span_findings if f.get("status") == "fail")
    errors.extend(e for c in checks for e in c["errors"])
    # Nothing silently disappears if a registry routine is misspelled.
    names = {r.get("historical_name") for r in inventory} | {name for fs in native_functions.values() for name in fs}
    for name in claims:
        if rename.get(name, name) not in names:
            errors.append(f"registry routine has no implementation: {name}")
    report = {"schema": "stunts-port-semantic-audit-v1", "status": "FAIL" if errors else "PASS",
              "claim_boundary": "Static contracts and named finite tests only; risk signals are advisory, not equivalence proofs.",
              "registry_sha256": hashlib.sha256(REGISTRY.read_bytes()).hexdigest(),
              "oracle_lock_sha256": hashlib.sha256((ROOT / "layout/oracle.lock.json").read_bytes()).hexdigest(),
              "ownership_manifest_sha256": hashlib.sha256((ROOT / "layout/manifest.json").read_bytes()).hexdigest(),
              "compiler": {"path": str(gcc), "checks_executed": compile_checks},
              "sources": parsed_sources, "functions": inventory,
              "global_declarations": global_declarations,
              "historical_mapping": historical_mapping,
              "contracts": registry["contracts"], "compiler_contracts": checks,
              "risks": risks, "span_findings": span_findings, "errors": errors,
              "summary": {"historical_c_sources": len(rows),
                          "portable_c_sources": sum(s.endswith('.c') for s in native_functions),
                          "portable_headers": sum(s.endswith('.h') for s in native_functions),
                          "function_rows": len(inventory), "risk_occurrences": len(risks),
                          "historical_functions": len(historical_mapping),
                          "historical_mapping": dict(Counter(r["mapping_status"] for r in historical_mapping)),
                          "risk_classes": dict(Counter(r["class"] for r in risks)),
                          "coverage": dict(Counter(s for r in inventory for s in r["coverage"])),
                          "compiler_assertions": sum(c["assertions"] for c in checks)}}
    (work / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Stunts port semantic audit", "", report["claim_boundary"], "",
             "```json", json.dumps(report["summary"], indent=2), "```", "",
             "The JSON has every function, dependency, risk occurrence and bounded contract. "
             "Evidence entries identify tests; this scan does not rerun those oracle tests.", ""]
    lines += ["- " + error for error in errors]
    (work / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path, default=DEFAULT_WORK)
    parser.add_argument("--gcc", type=Path, default=Path(r"C:\msys64\mingw32\bin\gcc.exe"))
    parser.add_argument("--inventory-only", action="store_true", help="Omit compiler checks; no static verification claim")
    args = parser.parse_args()
    report = audit(args.work, args.gcc, not args.inventory_only)
    print(report["status"], json.dumps(report["summary"], sort_keys=True))
    for error in report["errors"][:15]:
        print(error)
    print("Report:", args.work / "report.json")
    return 0 if report["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
