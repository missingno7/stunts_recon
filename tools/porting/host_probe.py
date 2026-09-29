#!/usr/bin/env python3
"""Compile active GAME_C recipe sources with GCC and summarize host-port hazards.

This is an SDL3 preparation probe kept separate from the historical matching build. It does not alter source, recipes, or
acceptance state and never attempts to run or link the game.
"""
from __future__ import annotations

import argparse
import collections
import json
import re
import subprocess
import sys
import time
import shutil
from pathlib import Path

PORTING_DIR = Path(__file__).resolve().parent
ROOT = PORTING_DIR.parents[1]
WORKER = ROOT / "build" / "porting" / "host-probe"
DEFAULT_GCC = Path(r"C:\msys64\mingw64\bin\gcc.exe")
MAX_SOURCES = 128
COMPILE_TIMEOUT_SECONDS = 90
MAX_TOTAL_SECONDS = 900
COMPILER_FLAGS = [
    "-std=gnu11", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
    "-Wsign-conversion", "-Wpointer-arith", "-Wcast-align", "-Wcast-qual",
    "-Wstrict-prototypes", "-Wmissing-prototypes",
]
CATEGORIES = (
    "implicit int widths", "pointer size assumptions", "segment arithmetic",
    "inline asm", "int86/port I/O", "far pointer normalization",
    "signed shift", "struct packing", "other compiler diagnostic",
)


def active_c_sources(root: Path) -> list[dict[str, str]]:
    manifest = json.loads((root / "layout/manifest.json").read_text(encoding="utf-8"))
    found: dict[str, dict[str, str]] = {}
    for owner in manifest["owners"]:
        if owner.get("classification") != "GAME_C" or owner.get("kind") not in ("MATCHING_C", "MATCHING_C_DATA"):
            continue
        recipe_rel = owner.get("recipe")
        if not recipe_rel:
            continue
        recipe = json.loads((root / recipe_rel).read_text(encoding="utf-8"))
        if recipe.get("kind", "c") == "asm":
            continue
        source = recipe.get("source")
        if source:
            found[source] = {"source": source, "owners": ", ".join(sorted(set(
                [*found.get(source, {}).get("owners", "").split(", "), owner["id"]]
            ))), "recipes": recipe_rel}
    return [found[key] for key in sorted(found)]


def source_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""


def source_markers(text: str) -> dict[str, object]:
    comment_hits = []
    platform_hits = []
    port_hits = []
    for line_no, line in enumerate(text.splitlines(), start=1):
        for match in re.finditer(r"PLATFORM\(([^)]+)\)(?:\s*:\s*([^*]+))?", line, re.I):
            platform_hits.append({"tag": match.group(1).strip(), "description": (match.group(2) or "").strip(), "line": line_no})
        for match in re.finditer(r"\bPORT\s*:\s*([^*\r\n]+)", line, re.I):
            port_hits.append({"tag": match.group(1).strip(), "line": line_no})
    code = re.sub(r"/\*.*?\*/|//[^\n]*|\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])*'", " ", text, flags=re.S)
    hits: dict[str, list[str]] = {c: [] for c in CATEGORIES}
    if re.search(r"\b(?:unsigned\s+)?int\b", code):
        hits["implicit int widths"].append("bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit)")
    if re.search(r"\b(?:far|_far|huge|_huge|near|_near)\b", code):
        hits["far pointer normalization"].append("segmented pointer qualifier is erased to a flat host pointer")
    if re.search(r"\b(?:FP_SEG|FP_OFF|MK_FP|segread|segment|segaddr|_segread|FP_SEGMENT)\b", code, re.I):
        hits["segment arithmetic"].append("explicit segment/offset helper or spelling")
    if re.search(r"\b(?:__asm|_asm|asm)\b", code):
        hits["inline asm"].append("inline assembly token")
    if re.search(r"\b(?:int86x?|inp[bw]?|outp[bw]?|_enable|_disable)\b|#\s*pragma\s+intrinsic", text, re.I):
        hits["int86/port I/O"].append("DOS interrupt, port-I/O intrinsic, or intrinsic pragma")
    if re.search(r"(?:>>|<<)", code):
        hits["signed shift"].append("shift expression; inspect signed operands and overflow assumptions")
    if re.search(r"#\s*pragma\s+pack|__attribute__\s*\(\(\s*packed|#\s*pragma\s+align", text, re.I):
        hits["struct packing"].append("explicit packing/alignment directive")
    if re.search(r"\b(?:struct|union)\b", code):
        hits["struct packing"].append("aggregate layout needs host review where it crosses file/API or serialized-data boundaries")
    return {"platform_tags": platform_hits, "port_tags": port_hits, "markers": hits}


def classify_diagnostic(line: str) -> list[str]:
    lower = line.lower()
    result: list[str] = []
    flag_match = re.search(r"\[(-w[^]]+)\]", lower)
    flag = flag_match.group(1) if flag_match else ""
    if flag in {"-wconversion", "-wsign-conversion", "-woverflow", "-wsign-compare",
                "-wchar-subscripts", "-wtype-limits", "-wimplicit-int",
                "-wimplicit-function-declaration", "-wstrict-prototypes",
                "-wbuiltin-declaration-mismatch"}:
        result.append("implicit int widths")
    if flag in {"-wint-to-pointer-cast", "-wpointer-to-int-cast", "-wpointer-sign",
                "-wincompatible-pointer-types", "-wdiscarded-qualifiers", "-wint-conversion"}:
        result.append("pointer size assumptions")
    if re.search(r"(?:^|\W)(?:__asm|_asm|asm)(?:\W|$)|asm statement", lower):
        result.append("inline asm")
    if re.search(r"int86|\binp\b|\boutp\b|intrinsic|port.?io", lower):
        result.append("int86/port I/O")
    if re.search(r"segment|offset|fp_seg|fp_off|mk_fp|far pointer", lower):
        result.append("segment arithmetic")
    if re.search(r"far|huge|near|pointer.*qualifier", lower):
        result.append("far pointer normalization")
    if re.search(r"shift|left shift|right shift|shift-count", lower):
        result.append("signed shift")
    if re.search(r"packed|padding|align|pragma pack|layout", lower):
        result.append("struct packing")
    if not result and re.search(r"implicit (?:int|declaration|function)|conversion|changes value|overflow|narrow|sign-conversion|signedness|conflicting types|different size|incompatible type for argument|return type|incompatible integer", lower):
        result.append("implicit int widths")
    if not result and re.search(r"\b(?:warning|error):", lower):
        result.append("other compiler diagnostic")
    return list(dict.fromkeys(result))


def port_tag_covers(category: str, text: str) -> bool:
    lower = text.lower()
    terms = {
        "implicit int widths": ("int", "width", "16-bit", "16 bit", "scalar", "promotion", "conversion", "signedness"),
        "pointer size assumptions": ("pointer", "segment", "offset", "address", "far"),
        "segment arithmetic": ("segment", "offset", "far-pointer", "far pointer", "16:16"),
        "inline asm": ("asm", "assembly", "instruction"),
        "int86/port I/O": ("port", "interrupt", "bios", "dos", "hardware", "i/o"),
        "far pointer normalization": ("far", "segment", "offset", "pointer"),
        "signed shift": ("shift", "arithmetic right", "signed right"),
        "struct packing": ("pack", "layout", "field offset", "alignment", "record", "structure"),
        "other compiler diagnostic": (),
    }
    return any(term in lower for term in terms.get(category, ()))


def run_one(gcc: Path, source: Path, index: int, out_dir: Path,
            root: Path, compat: Path, include_dir: Path, deadline: float | None = None) -> dict[str, object]:
    obj = out_dir / f"{index:03d}_{source.stem}.o"
    if obj.exists():
        obj.unlink()
    common = [str(gcc), *COMPILER_FLAGS, "-include", str(compat), "-I", str(include_dir),
              "-I", str(root / "include"), "-I", str(root / "src")]
    runs = []
    for mode in ("syntax", "object"):
        cmd = [*common, "-fsyntax-only", str(source)] if mode == "syntax" else [
            *common, "-c", str(source), "-o", str(obj)]
        try:
            remaining = COMPILE_TIMEOUT_SECONDS
            if deadline is not None:
                remaining = min(remaining, deadline - time.monotonic())
                if remaining <= 0:
                    raise TimeoutError("host probe total time limit reached")
            p = subprocess.run(cmd, cwd=root, capture_output=True, text=True,
                               errors="replace", timeout=remaining, check=False)
            stderr = p.stderr
            stdout = p.stdout
            status = "ok" if p.returncode == 0 else "failed"
            rc = p.returncode
        except (subprocess.TimeoutExpired, TimeoutError) as exc:
            detail = getattr(exc, "stderr", None)
            stderr = detail.decode(errors="replace") if isinstance(detail, bytes) else (detail or str(exc))
            stdout = ""
            status, rc = "timeout", None
        runs.append({"mode": mode, "status": status, "returncode": rc,
                     "stdout": stdout, "stderr": stderr, "command": cmd})
    text = source_text(source)
    markers = source_markers(text)
    counts = collections.Counter()
    all_lines: list[str] = []
    category_diags: dict[str, list[str]] = {c: [] for c in CATEGORIES}
    unique_diag: dict[str, tuple[str, str]] = {}
    for run in runs:
        for line in (run["stderr"] or "").splitlines():
            if not re.search(r"\b(?:warning|error):", line):
                continue
            severity = "errors" if "error:" in line else "warnings"
            key = line.strip()
            unique_diag[key] = (severity, run["mode"])
            for category in classify_diagnostic(line):
                row = line.strip()
                if row not in category_diags[category]:
                    category_diags[category].append(row)
    all_lines = list(unique_diag)
    for severity, _mode in unique_diag.values():
        counts[severity] += 1
    for category, values in markers["markers"].items():
        if values:
            counts["hazard_categories"] += 1
    return {
        "source": source.relative_to(root).as_posix(),
        "status": {r["mode"]: r["status"] for r in runs},
        "returncodes": {r["mode"]: r["returncode"] for r in runs},
        "counts": dict(counts),
        "tags": {"PLATFORM": markers["platform_tags"], "PORT": markers["port_tags"]},
        "markers": markers["markers"],
        "diagnostics": category_diags,
        "warning_error_lines": all_lines,
        "runs": runs,
    }


def render_report(results: list[dict[str, object]], gcc: Path, gcc_version: str, gcc_target: str, root: Path) -> str:
    lines = [
        "# Host compiler probe: porting hazard proposal", "",
        "Generated by `tools/porting/host_probe.py`.",
        "The probe compiles each active `GAME_C` C recipe source with GCC for the current MinGW host, once with `-fsyntax-only` and once with `-c`, using warnings and the isolated host compatibility shims. It does not link or run the game.",
        "", f"Compiler: `{gcc}` ({gcc_version}); target `{gcc_target}`", f"Accepted C sources: {len(results)}", "",
        "## Aggregate", "",
    ]
    syntax_fail = sum(x["status"]["syntax"] != "ok" for x in results)
    object_fail = sum(x["status"]["object"] != "ok" for x in results)
    warning_total = sum(x["counts"].get("warnings", 0) for x in results)
    error_total = sum(x["counts"].get("errors", 0) for x in results)
    lines += [f"- Syntax-only failures: {syntax_fail}/{len(results)}; object compile failures: {object_fail}/{len(results)}.",
              f"- Distinct GCC warning diagnostics: {warning_total}; distinct error diagnostics: {error_total} (same line/message across the two passes counted once).",
              "- `I16/U16/I32/U32` are mapped to fixed-width types. Bare `int` remains host 32-bit while original MSC 5.x `int` is 16-bit; flat pointers are host-sized because segmented qualifiers are erased.",
              "- Lexical markers are review requests; compiler diagnostics are reported separately. Errors from missing legacy declarations or incompatible declarations are actionable host-port blockers.", "",
              "## Category coverage", ""]
    for category in CATEGORIES:
        marked = sum(bool(x["markers"].get(category)) for x in results)
        diagnosed = sum(bool(x["diagnostics"].get(category)) for x in results)
        diag_rows = sum(len(x["diagnostics"].get(category, [])) for x in results)
        lines.append(f"- **{category}:** source markers in {marked} files; compiler diagnostics in {diagnosed} files ({diag_rows} distinct diagnostic rows).")
    lines += ["", "## Per-file hazards", ""]
    for item in results:
        lines.append(f"### `{item['source']}`")
        lines.append("")
        lines.append(f"- Compile: syntax `{item['status']['syntax']}`, object `{item['status']['object']}`; distinct diagnostics: {item['counts'].get('warnings',0)} warnings, {item['counts'].get('errors',0)} errors.")
        if item["status"]["syntax"] != "ok" or item["status"]["object"] != "ok":
            error_text = "\n".join(item["warning_error_lines"])
            if re.search(r"_asm|asm statement|unknown pragma.*inline", error_text, re.I):
                lines.append("- BLOCKER CLASS: 3 (GCC does not accept the legacy inline assembly/compiler dialect in this source).")
            else:
                lines.append("- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).")
        platform = item["tags"]["PLATFORM"]
        port = item["tags"]["PORT"]
        tags = []
        platform_grouped = {}
        for item_tag in platform:
            platform_grouped.setdefault(item_tag["tag"], []).append(item_tag["line"])
        for key, tag_lines in platform_grouped.items():
            refs = ",".join(str(n) for n in tag_lines[:5])
            suffix = f" (+{len(tag_lines)-5})" if len(tag_lines) > 5 else ""
            tags.append(f"PLATFORM({key})@{refs}{suffix}")
        port_grouped = {}
        for item_tag in port:
            port_grouped.setdefault(item_tag["tag"], []).append(item_tag["line"])
        for key, tag_lines in port_grouped.items():
            refs = ",".join(str(n) for n in tag_lines[:4])
            suffix = f" (+{len(tag_lines)-4})" if len(tag_lines) > 4 else ""
            tags.append(f"PORT: {key}@{refs}{suffix}")
        lines.append("- Existing tags: " + ("; ".join(tags) if tags else "none"))
        present = False
        active_categories = []
        for category in CATEGORIES:
            findings = list(item["markers"].get(category, []))
            diagnostics = item["diagnostics"].get(category, [])
            if not findings and not diagnostics:
                continue
            present = True
            active_categories.append(category)
            lines.append(f"- **{category}:** " + ("; ".join(findings) if findings else "compiler diagnostic") + (f"; {len(diagnostics)} distinct warning/error lines" if diagnostics else ""))
            shown = set()
            for diag in diagnostics:
                clean = diag.replace(str(root) + "\\", "", 1).replace(str(root) + "/", "", 1)
                if clean in shown:
                    continue
                shown.add(clean)
                lines.append(f"  - `{clean}`")
                if len(shown) >= 3:
                    break
            if len(diagnostics) > 3:
                lines.append(f"  - ? {len(diagnostics)-3} additional categorized diagnostic rows; see `host/results.json`.")
        if not present:
            lines.append("- No screened host-port hazard marker or compiler diagnostic.")
        followups = []
        for category in active_categories:
            if category == "other compiler diagnostic":
                continue
            covered = any(port_tag_covers(category, x["tag"]) for x in port)
            if not covered:
                followups.append(f"add/review `PORT:` coverage for {category}")
        hw_categories = {"segment arithmetic", "int86/port I/O"} & set(active_categories)
        platform_domains = {x["tag"].lower() for x in platform}
        if "int86/port I/O" in hw_categories and not (platform_domains & {"dos", "bios", "hardware", "video", "timer", "input_kb", "input_mouse", "input_joy"}):
            followups.append("add/review `PLATFORM(dos/bios/hardware)` for hardware access")
        if "segment arithmetic" in hw_categories and not (platform_domains & {"dos", "bios", "memory", "file", "video", "audio"}):
            followups.append("add/review `PLATFORM(memory/dos)` for segment-based access")
        lines.append("- Tagging follow-up: " + ("; ".join(followups) if followups else "the screened hazards have matching source PORT tags"))
        lines.append("")
    lines += ["## Category interpretation", "",
              "- **implicit int widths:** audit bare `int`, implicit declarations/returns, narrowing, and sign conversions against the 16-bit MSC ABI.",
              "- **pointer size assumptions:** audit integer/pointer casts, stored pointer widths, pointer serialization, and pointer argument type mismatches against the 16:16 target ABI.",
              "- **segment arithmetic:** replace segment:offset construction or BIOS/DOS memory addressing with explicit host buffers/handles.",
              "- **inline asm:** port instruction blocks to host C/platform APIs or isolated host assembly.",
              "- **int86/port I/O:** move BIOS/DOS interrupt and direct port services behind SDL/platform service boundaries.",
              "- **far pointer normalization:** erasing `far/near/huge` does not preserve segmented bounds, normalization, or pointer width; review each memory/API boundary.",
              "- **signed shift:** make negative right-shift and signed overflow behavior explicit where output depends on it.",
              "- **struct packing:** compare field widths, alignment, and serialized/on-disk/on-wire layouts; add explicit host-side conversion/layout only when required.",
              "- **other compiler diagnostic:** compiler warning/error did not match a porting category; inspect its exact location in the raw results.",
              "", "## Reproduction", "",
              "Run `python tools/porting/host_probe.py --mode legacy` from the repository root. Detailed commands, raw diagnostics, and per-category evidence are in `build/porting/host-probe/legacy/host/results.json`.", ""]
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC,
                        help="GCC executable path (default: C:/msys64/mingw64/bin/gcc.exe)")
    parser.add_argument("--mode", choices=("compat", "strict-central", "legacy"), default="compat",
                        help="compat uses per-TU host views; strict-central audits only the shared declarations; legacy runs the original shim probe")
    args = parser.parse_args()
    root = args.root.resolve()
    if args.gcc.is_file():
        gcc = args.gcc.resolve()
    else:
        located = shutil.which(str(args.gcc))
        gcc = Path(located).resolve() if located else args.gcc.resolve()
    if not gcc.is_file():
        raise SystemExit(f"GCC compiler not found: {gcc}")
    if args.mode != "legacy":
        sys.path.insert(0, str(PORTING_DIR))
        from host_probe_modes import run_port_mode
        return run_port_mode(root, gcc, args.mode)
    output_root = root / "build" / "porting" / "host-probe" / "legacy"
    out_dir = output_root / "host/out"
    include_dir = root / "tools/porting/host/include"
    compat = root / "tools/porting/host/compat.h"
    out_dir.mkdir(parents=True, exist_ok=True)
    (output_root / "host").mkdir(parents=True, exist_ok=True)
    sources = active_c_sources(root)
    if len(sources) > MAX_SOURCES:
        raise SystemExit(f"Refusing to exceed bounded source inventory ({len(sources)} > {MAX_SOURCES}).")
    if not gcc.is_file():
        raise SystemExit(f"GCC compiler not found: {gcc}")
    version_result = subprocess.run([str(gcc), "--version"], capture_output=True, text=True,
                                    errors="replace", timeout=15, check=False)
    gcc_version = version_result.stdout.splitlines()[0] if version_result.stdout else "unknown"
    target_result = subprocess.run([str(gcc), "-dumpmachine"], capture_output=True, text=True,
                                   errors="replace", timeout=15, check=False)
    gcc_target = target_result.stdout.strip() or "unknown"
    results = []
    deadline = time.monotonic() + MAX_TOTAL_SECONDS
    for i, info in enumerate(sources, start=1):
        if time.monotonic() >= deadline:
            raise SystemExit(f"Refusing to exceed {MAX_TOTAL_SECONDS}-second total probe limit.")
        source = (root / info["source"]).resolve()
        item = run_one(gcc, source, i, out_dir, root, compat, include_dir, deadline)
        item["owners"] = info["owners"]
        item["recipes"] = info["recipes"]
        results.append(item)
        print(f"[{i}/{len(sources)}] {item['source']}: syntax={item['status']['syntax']} object={item['status']['object']} warnings={item['counts'].get('warnings',0)} errors={item['counts'].get('errors',0)}", flush=True)
    (output_root / "host").mkdir(parents=True, exist_ok=True)
    (output_root / "host/results.json").write_text(json.dumps({
        "compiler": str(gcc), "compiler_version": gcc_version, "target": gcc_target,
        "flags": COMPILER_FLAGS,
        "source_count": len(results), "results": results,
    }, indent=2), encoding="utf-8")
    report = output_root / "report.md"
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(render_report(results, gcc, gcc_version, gcc_target, root), encoding="utf-8")
    print(f"Compiler: {gcc_version}; target {gcc_target}")
    print(f"Report: {report}")
    print(f"Raw results: {output_root / 'host/results.json'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
