#!/usr/bin/env python3
"""Build a source-evidenced global-state dictionary for the porting notes.

Generated model data and compiler-probe scratch stay under build/porting (apart
from normal build/search reports made by search.py). --no-probes uses the
bounded probe cache in that directory; otherwise the project compiler path is
invoked for isolated type-layout probes. This tool never changes source,
recipes, ownership, or the byte oracle.
"""
from __future__ import annotations

import argparse
import ast
import json
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / "build/porting"
DOCS = WORK
PROBES = WORK / "typeprobes"
REPORTS = WORK / "typeprobe-reports.json"


def read_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def line_no(text: str, pos: int) -> int:
    return text.count("\n", 0, pos) + 1


def mask_noncode(text: str) -> str:
    """Blank comments and literals while preserving offsets and newlines."""
    out = list(text)
    i = 0
    state = "code"
    while i < len(text):
        c = text[i]
        n = text[i + 1] if i + 1 < len(text) else ""
        if state == "code":
            if c == "/" and n == "*":
                out[i] = out[i + 1] = " "
                i += 2
                state = "comment"
                continue
            if c == "/" and n == "/":
                out[i] = out[i + 1] = " "
                i += 2
                state = "linecomment"
                continue
            if c in ("'", '"'):
                quote = c
                out[i] = " "
                state = "string" if quote == '"' else "char"
                i += 1
                continue
        elif state == "comment":
            if c == "*" and n == "/":
                out[i] = out[i + 1] = " "
                i += 2
                state = "code"
                continue
            if c not in "\r\n":
                out[i] = " "
        elif state == "linecomment":
            if c in "\r\n":
                state = "code"
            else:
                out[i] = " "
        else:
            if c == "\\" and i + 1 < len(text):
                out[i] = " "
                if text[i + 1] not in "\r\n":
                    out[i + 1] = " "
                i += 2
                continue
            if (state == "string" and c == '"') or (state == "char" and c == "'"):
                out[i] = " "
                state = "code"
            elif c not in "\r\n":
                out[i] = " "
        i += 1
    return "".join(out)


def mask_preprocessor_lines(text: str) -> str:
    """Blank C preprocessor directives without changing source offsets."""
    out = list(text)
    offset = 0
    for line in text.splitlines(keepends=True):
        if line.lstrip().startswith("#"):
            for i in range(offset, offset + len(line)):
                if out[i] not in "\r\n":
                    out[i] = " "
        offset += len(line)
    return "".join(out)


def c_functions(text: str):
    """Return top-level C function ranges; retain all source offsets."""
    code = mask_noncode(text)
    ranges = []
    depth = 0
    stmt_start = 0
    active = None
    active_depth = 0
    for i, ch in enumerate(code):
        if ch == "{" and depth == 0:
            header = code[stmt_start:i].strip()
            match = re.search(r"([A-Za-z_]\w*)\s*\([^;{}]*\)\s*$", header, re.S)
            # Aggregate types in parameter lists are normal C function
            # signatures; only exclude control constructs and typedef bodies.
            excluded = re.search(r"\b(if|for|while|switch|sizeof|typedef)\b", header)
            if match and not excluded and "=" not in header:
                active = (match.group(1), stmt_start)
                active_depth = 1
            else:
                active = None
            depth = 1
        elif ch == "{" and depth:
            depth += 1
            if active:
                active_depth += 1
        elif ch == "}" and depth:
            depth -= 1
            if active:
                active_depth -= 1
                if active_depth == 0:
                    ranges.append((active[0], active[1], i + 1))
                    active = None
            if depth == 0:
                stmt_start = i + 1
        elif ch == ";" and depth == 0:
            stmt_start = i + 1
    by_line = {}
    for name, start, end in ranges:
        first = line_no(text, start)
        last = line_no(text, end)
        for n in range(first, last + 1):
            by_line[n] = name
    return ranges, by_line, code


def asm_functions(text: str):
    rows = []
    current = None
    for no, line in enumerate(text.splitlines(), 1):
        m = re.search(r"^\s*([A-Za-z_?$@][\w?$@]*)\s+PROC\b", line, re.I)
        if m:
            current = m.group(1).lstrip("_")
        if current:
            rows.append((no, current))
        if re.search(r"^\s*[A-Za-z_?$@][\w?$@]*\s+ENDP\b", line, re.I):
            current = None
    return dict(rows)


def top_level_declarations(text: str, code: str, function_ranges):
    """Map identifier-bearing file-scope semicolon statements to source lines."""
    in_fn = [False] * (len(code) + 1)
    for _, a, b in function_ranges:
        in_fn[a:b] = [True] * (b - a)
    decls = []
    depth = 0
    start = 0
    code = mask_preprocessor_lines(code)
    for i, ch in enumerate(code):
        if ch == "{" and depth == 0:
            # A file-scope initializer/aggregate starts a nested declarator.
            depth = 1
        elif ch == "{" and depth:
            depth += 1
        elif ch == "}" and depth:
            depth -= 1
            if depth == 0:
                before = code[start:i + 1]
                # Keep a file-scope aggregate initializer attached to its
                # declarator; discard type bodies and function bodies.
                if in_fn[i] or not ("=" in before and not re.search(r"\b(struct|union)\s+\w+\s*\{", before)):
                    start = i + 1
        elif ch == ";" and depth == 0 and not in_fn[i]:
            frag = mask_preprocessor_lines(text[start:i + 1]).strip()
            clean_frag = code[start:i + 1]
            if frag.strip() and not re.search(r"\b(struct|union|enum)\s+\w+\s*\{", clean_frag):
                decls.append((start, i + 1, frag.strip()))
            start = i + 1
    return decls


def type_definitions(text: str, code: str, source: str):
    """Extract each tag definition and its top-level member declarations."""
    pat = re.compile(r"\b(typedef\s+)?(struct|union)\s+([A-Za-z_]\w*)\s*\{")
    defs = []
    pack = 0
    line_offsets = [0]
    for m in re.finditer("\n", text):
        line_offsets.append(m.end())
    pos = 0
    while True:
        m = pat.search(code, pos)
        if not m:
            break
        depth = 1
        i = m.end()
        while i < len(code) and depth:
            if code[i] == "{":
                depth += 1
            elif code[i] == "}":
                depth -= 1
            i += 1
        close = code.find(";", i)
        if close < 0:
            pos = i
            continue
        start = m.start()
        # Keep a preceding typedef token on the same declaration line.
        if m.group(1):
            start = m.start(1)
        raw = text[start:close + 1]
        current_pack = 0
        for pm in re.finditer(r"#\s*pragma\s+pack\s*\(([^)]*)\)", text[:start], re.I):
            arg = pm.group(1).strip()
            current_pack = 1 if arg == "1" else 0
        body = text[m.end():i - 1]
        body_code = code[m.end():i - 1]
        fields = []
        field_start = 0
        field_depth = 0
        for j, ch in enumerate(body_code):
            if ch == "{": field_depth += 1
            elif ch == "}": field_depth -= 1
            elif ch == ";" and field_depth == 0:
                decl = body[field_start:j + 1].strip()
                if decl:
                    fields.append({"declaration": decl, "line": line_no(text, m.end() + field_start)})
                field_start = j + 1
        defs.append({"tag": m.group(3), "kind": m.group(2), "source": source,
                     "line": line_no(text, start), "code_start": start,
                     "raw": raw, "pack": current_pack, "fields": fields})
        pos = close + 1
    return defs


def type_probe_source(source_text: str, source_path: str, defs, target_index: int):
    """Emit one accepted type declaration plus a public sizeof sentinel pair."""
    code = mask_noncode(source_text)
    target = defs[target_index]
    target_func = target.get("containing_function")
    prior = [d for d in defs if d["code_start"] < target["code_start"]
             and (d.get("containing_function") is None or d.get("containing_function") == target_func)]
    latest_by_tag = {}
    for d in prior:
        latest_by_tag[d["tag"]] = d
    selected = sorted(list(latest_by_tag.values()) + [target], key=lambda d: d["code_start"])
    pieces = ["/* Isolated P2b MSC ABI probe; all layouts copied from accepted source. */\n"]
    # Preserve simple typedef aliases (for example the C6 audio u8/u16/u32 names).
    typedefs = {}
    for m in re.finditer(r"\btypedef\b[^;{}]*;", code, re.S):
        if m.start() >= target["code_start"]:
            continue
        stmt = source_text[m.start():m.end()].strip()
        if not re.search(r"\b(struct|union)\s+\w+\s*\{", stmt):
            if "\n" not in stmt or len(stmt) < 300:
                alias = re.findall(r"[A-Za-z_]\w*", stmt.rstrip(";"))[-1]
                typedefs[alias] = (m.start(), stmt)
    defs_and_typedefs = [(d["code_start"], "def", d) for d in selected]
    defs_and_typedefs += [(p, "typedef", t) for p, t in typedefs.values()]
    for _, kind, row in sorted(defs_and_typedefs, key=lambda x: x[0]):
        if kind == "typedef":
            pieces.append(row + "\n")
        else:
            if row["pack"]:
                pieces.append("#pragma pack(1)\n")
            pieces.append(row["raw"] + "\n")
            if row["pack"]:
                pieces.append("#pragma pack()\n")
    # Initializers force MSC to emit initialized _DATA bytes rather than
    # tentative definitions as COMDEF, which the scratch object probe does
    # not allocate without a linker rule.
    pieces.append(f"unsigned char p2bb000[sizeof({target['kind']} {target['tag']})] = {{0}};\n")
    pieces.append("unsigned char p2be000[1] = {0};\n")
    return "".join(pieces)


def load_inputs():
    manifest = read_json(ROOT / "layout/manifest.json")
    registry = read_json(ROOT / "layout/names-registry.json")
    data_symbols = read_json(ROOT / "layout/data-symbols.json")
    communal_units = [row for row in manifest.get("bss_owners", [])
                      if row.get("kind") == "LINK_COMMUNAL" and row.get("id") == "c_common"]
    if len(communal_units) != 1:
        raise RuntimeError("layout/manifest.json must contain the accepted c_common LINK_COMMUNAL unit")
    communal_doc = {"schema": communal_units[0].get("schema"),
                    "start": communal_units[0].get("start"),
                    "end": communal_units[0].get("end"),
                    "communals": communal_units[0].get("communals", [])}
    owner_by_id = {o["id"]: o for o in manifest["owners"]}
    source_owner = {}
    active = []
    for owner in manifest["owners"]:
        if owner.get("kind") not in ("MATCHING_C", "MATCHING_C_DATA", "MATCHING_ASM", "MATCHING_ASM_DATA"):
            continue
        recipe_path = owner.get("recipe")
        source_path = None
        if recipe_path and (ROOT / recipe_path).is_file():
            source_path = read_json(ROOT / recipe_path).get("source")
        if not source_path or not (ROOT / source_path).is_file():
            continue
        lang = "c" if owner["kind"] in ("MATCHING_C", "MATCHING_C_DATA") else "asm"
        row = {"id": owner["id"], "kind": owner["kind"], "source": source_path,
               "recipe": recipe_path, "language": lang}
        active.append(row)
        source_owner[source_path] = row
    symbol_storage = defaultdict(list)
    for original, meta in data_symbols.get("symbols", {}).items():
        symbol_storage[meta.get("load_address")].append({"original_public": original, **meta})
    communal_by_address = {c["address"]: c for c in communal_doc["communals"]}
    intervals = []
    for owner in manifest["owners"]:
        for item in owner.get("data_intervals", []):
            intervals.append((item["start"], item["end"], owner))
        if owner.get("kind") in ("MATCHING_C_DATA", "MATCHING_ASM_DATA") and owner.get("start") is not None:
            intervals.append((owner["start"], owner["end"], owner))
        if owner.get("kind") == "KNOWN_TOOLCHAIN_LIBRARY_DATA" and owner.get("start") is not None:
            intervals.append((owner["start"], owner["end"], owner))
    return manifest, registry, data_symbols, communal_doc, owner_by_id, source_owner, active, symbol_storage, communal_by_address, intervals


def collect_probes(active, generate=True):
    result_path = REPORTS
    if not generate:
        if not result_path.is_file():
            raise RuntimeError(f"--no-probes requires the cache at {result_path.relative_to(ROOT)}")
        cached = read_json(result_path)
        return cached.get("variants", {})
    variants = {}
    generated = 0
    searched = 0
    jobs = []
    for owner in active:
        if owner["language"] != "c":
            continue
        source_path = ROOT / owner["source"]
        text = source_path.read_text(encoding="utf-8", errors="replace")
        defs = type_definitions(text, mask_noncode(text), owner["source"])
        if not defs:
            continue
        _, by_line, _ = c_functions(text)
        for d in defs:
            d["containing_function"] = by_line.get(d["line"])
            key = f"{owner['source']}:{d['line']}:{d['kind']} {d['tag']}"
            jobs.append((key, owner, source_path, text, defs, d))
    if len(jobs) > 160:
        raise RuntimeError(f"type-probe cap exceeded ({len(jobs)} > 160)")
    for job_no, (key, owner, source_path, text, defs, target) in enumerate(jobs):
        if not generate:
            continue
        probe_path = PROBES / (source_path.stem + f"_typeprobe_{job_no:03d}.c")
        probe_path.parent.mkdir(parents=True, exist_ok=True)
        target_index = defs.index(target)
        probe_path.write_text(type_probe_source(text, owner["source"], defs, target_index), encoding="utf-8")
        generated += 1
        try:
            proc = subprocess.run([sys.executable, str(ROOT / "tools/search.py"), str(probe_path.relative_to(ROOT))],
                                  cwd=ROOT, text=True, capture_output=True, timeout=60)
            output = json.loads(proc.stdout) if proc.returncode == 0 else {}
            report_ref = output.get("report")
            report = read_json(ROOT / report_ref) if report_ref and (ROOT / report_ref).is_file() else {}
            comp = report.get("compiler", {})
            observed = report.get("observed_output") or {}
            obj_path = observed.get("path")
            obj = read_json(ROOT / obj_path) if obj_path and (ROOT / obj_path).is_file() else {}
            publics = obj.get("publics", [])
            offsets = {}
            for pub in publics:
                name = pub.get("name", "").lstrip("_")
                if name in ("p2bb000", "p2be000"):
                    offsets[name] = pub.get("offset")
            sizes = {}
            a = offsets.get("p2bb000")
            b = offsets.get("p2be000")
            if isinstance(a, int) and isinstance(b, int) and b >= a:
                sizes[f"{target['kind']} {target['tag']}"] = b - a
            variants[key] = {"status": comp.get("status", "UNKNOWN"),
                             "candidate": str(probe_path.relative_to(ROOT)),
                             "report": report_ref,
                             "size_bytes": sizes,
                             "source": owner["source"], "line": target["line"],
                             "tag": f"{target['kind']} {target['tag']}",
                             "tags": [f"{target['kind']} {target['tag']}"],
                             "logged_error_lines": comp.get("logged_error_lines", []),
                             "message": comp.get("message")}
            searched += 1
            if not sizes:
                variants[key]["message"] = variants[key].get("message") or "compiled, but public byte sentinels were not found"
            print(f"probe {owner['id']} {target['tag']}:{target['line']}: {comp.get('status')} size={next(iter(sizes.values()), '?')}", flush=True)
        except subprocess.TimeoutExpired:
            variants[key] = {"status": "TIMEOUT", "candidate": str(probe_path.relative_to(ROOT)),
                             "source": owner["source"], "line": target["line"], "tag": f"{target['kind']} {target['tag']}",
                             "tags": [f"{target['kind']} {target['tag']}"]}
            print(f"probe {owner['id']}: timeout", file=sys.stderr)
        except Exception as exc:
            variants[key] = {"status": "ERROR", "candidate": str(probe_path.relative_to(ROOT)),
                             "source": owner["source"], "line": target["line"], "tag": f"{target['kind']} {target['tag']}",
                             "tags": [f"{target['kind']} {target['tag']}"], "message": str(exc)}
            print(f"probe {owner['id']}: {exc}", file=sys.stderr)
    write_json(result_path, {"schema": "p2b-typeprobe-results-v1", "jobs": generated, "completed": searched, "variants": variants})
    return variants


def member_probe_source(source_text, source_path, definitions, target, field_index):
    """Emit an isolated prefix type for a canonical compiler member-offset probe."""
    code = mask_noncode(source_text)
    target_func = target.get("containing_function")
    prior = [d for d in definitions if d["code_start"] < target["code_start"]
             and (d.get("containing_function") is None or d.get("containing_function") == target_func)]
    latest = {}
    for definition in prior:
        latest[definition["tag"]] = definition
    selected = sorted(latest.values(), key=lambda d: d["code_start"])
    pieces = ["/* Isolated canonical MSC member-prefix probe. */\n"]
    typedefs = {}
    for match in re.finditer(r"\btypedef\b[^;{}]*;", code, re.S):
        if match.start() >= target["code_start"]:
            continue
        statement = source_text[match.start():match.end()].strip()
        if not re.search(r"\b(struct|union)\s+\w+\s*\{", statement) and ("\n" not in statement or len(statement) < 300):
            alias = re.findall(r"[A-Za-z_]\w*", statement.rstrip(";"))[-1]
            typedefs[alias] = (match.start(), statement)
    entries = [(d["code_start"], "definition", d) for d in selected]
    entries += [(offset, "typedef", statement) for offset, statement in typedefs.values()]
    for _, kind, row in sorted(entries, key=lambda item: item[0]):
        if kind == "typedef":
            pieces.append(row + "\n")
        else:
            if row["pack"]:
                pieces.append("#pragma pack(1)\n")
            pieces.append(row["raw"] + "\n")
            if row["pack"]:
                pieces.append("#pragma pack()\n")
    if target["pack"]:
        pieces.append("#pragma pack(1)\n")
    prefix_fields = "".join(field["declaration"] + "\n" for field in target["fields"][:field_index])
    pieces.append("struct StateModelPrefix {\n" + prefix_fields + "};\n")
    if target["pack"]:
        pieces.append("#pragma pack()\n")
    pieces.append("unsigned char p2bb000[sizeof(struct StateModelPrefix)]={0};\n")
    pieces.append("unsigned char p2be000[1]={0};\n")
    return "".join(pieces)


def embedded_fields(definition):
    """List by-value aggregate members whose offsets can be probed."""
    found = []
    for field_index, field in enumerate(definition["fields"]):
        statement = re.sub(r"/\*.*?\*/|//[^\n]*", " ", field["declaration"], flags=re.S).strip()
        match = re.search(r"\b(?:struct|union)\s+([A-Za-z_]\w*)\s+(.+);", statement, re.S)
        if not match or "*" in match.group(2):
            continue
        inner_tag = match.group(1)
        declarators, start, depth = [], 0, 0
        for index, char in enumerate(match.group(2)):
            if char == "[": depth += 1
            elif char == "]": depth -= 1
            elif char == "," and depth == 0:
                declarators.append(match.group(2)[start:index].strip())
                start = index + 1
        declarators.append(match.group(2)[start:].strip())
        for declarator in declarators:
            if "*" in declarator:
                continue
            variable = re.match(r"([A-Za-z_]\w*)\s*((?:\[[^]]*\]\s*)*)$", declarator)
            if not variable:
                continue
            dimensions = [int(value) for value in re.findall(r"\[(\d+)\]", variable.group(2))]
            count = 1
            for value in dimensions:
                count *= value
            found.append({"field_index": field_index, "field": variable.group(1), "inner_tag": inner_tag,
                          "array_count": count, "declaration": field["declaration"], "line": field["line"]})
    return found


def public_delta(report_path):
    """Return the byte extent bracketed by the probe's two public arrays."""
    if not report_path:
        return None
    report = read_json(ROOT / report_path)
    observed = report.get("observed_output") or {}
    obj_path = observed.get("path")
    if not obj_path:
        return None
    obj = read_json(ROOT / obj_path)
    publics = {item.get("name", "").lstrip("_"): item.get("offset") for item in obj.get("publics", [])}
    start, end = publics.get("p2bb000"), publics.get("p2be000")
    return end - start if isinstance(start, int) and isinstance(end, int) and end >= start else None


def collect_member_offsets(generate=True):
    """Measure nested aggregate offsets from the accepted obj_seg003 declarations."""
    result_path = WORK / "member-offset-results.json"
    if not generate:
        if not result_path.is_file():
            raise RuntimeError(f"--no-probes requires the cache at {result_path.relative_to(ROOT)}")
        return read_json(result_path)

    source_path = "src/obj_seg003.c"
    source_text = (ROOT / source_path).read_text(encoding="utf-8", errors="replace")
    definitions = type_definitions(source_text, mask_noncode(source_text), source_path)
    by_line = c_functions(source_text)[1]
    for definition in definitions:
        definition["containing_function"] = by_line.get(definition["line"])
    targets = []
    for parent_tag in ("GAMESTATE", "CARSTATE", "SIMD", "TRANSFORMEDSHAPE3D"):
        definition = next((item for item in definitions
                           if item["tag"] == parent_tag and item.get("containing_function") is None), None)
        if definition:
            targets.extend((definition, field) for field in embedded_fields(definition))
    if len(targets) > 64:
        raise RuntimeError(f"member-offset probe cap exceeded ({len(targets)} > 64)")
    results = []
    for definition, field in targets:
        if field["field_index"] == 0:
            result = {"offset_bytes": 0, "status": "C-first-member-offset", "report": None, "candidate": None}
        else:
            candidate = PROBES / "members" / f"{definition['tag']}_{field['field']}.c"
            candidate.parent.mkdir(parents=True, exist_ok=True)
            candidate.write_text(member_probe_source(source_text, source_path, definitions, definition,
                                                       field["field_index"]), encoding="utf-8")
            try:
                process = subprocess.run([sys.executable, str(ROOT / "tools/search.py"),
                                          str(candidate.relative_to(ROOT))], cwd=ROOT, text=True,
                                         capture_output=True, timeout=60)
                output = json.loads(process.stdout) if process.returncode == 0 else {}
                report_ref = output.get("report")
                report = read_json(ROOT / report_ref) if report_ref else {}
                result = {"offset_bytes": public_delta(report_ref),
                          "status": (report.get("compiler") or {}).get("status", "NO_REPORT"),
                          "report": report_ref, "candidate": str(candidate.relative_to(ROOT)),
                          "message": (report.get("compiler") or {}).get("message")}
            except subprocess.TimeoutExpired:
                result = {"offset_bytes": None, "status": "TIMEOUT",
                          "candidate": str(candidate.relative_to(ROOT))}
        results.append({"parent_tag": definition["tag"], "parent_source": source_path,
                        "parent_line": definition["line"], **field, **result})
        print(f"member offset {definition['tag']}.{field['field']}: {result['status']} "
              f"offset={result.get('offset_bytes', '?')}", flush=True)
    output = {"schema": "state-model-member-offset-results-v1", "source": source_path, "results": results}
    write_json(result_path, output)
    return output


SUBSYSTEM_RULES = {
    "simulation": ("game", "car", "speed", "crash", "collision", "wheel", "physics", "state", "rpm", "gear"),
    "camera": ("camera", "cam", "view", "look"),
    "track_replay": ("track", "trk", "tile", "road", "hill", "row", "column", "replay", "rpl", "race", "lap"),
    "render_video": ("render", "shape", "sprite", "poly", "pixel", "video", "screen", "rect", "font", "palette", "draw", "projection", "sphere", "transform", "window", "matrix", "mat"),
    "input": ("input", "key", "kb", "mouse", "joy", "button", "cursor"),
    "audio": ("audio", "sound", "snd", "voice", "engine", "pitch", "music", "sample", "speaker"),
    "menu_ui": ("menu", "score", "hiscore", "intro", "dialog", "text", "popup", "dashboard", "dash", "hud"),
    "resource_io": ("file", "resource", "res", "alloc", "heap", "memory", "chunk", "load", "unload"),
    "timer_platform": ("timer", "tick", "irq", "bios", "dos", "clock", "interrupt", "callback"),
}


def subsystem_for(name, functions, owner_id):
    score = Counter()
    text = (name + " " + owner_id + " " + " ".join(functions)).lower()
    for group, words in SUBSYSTEM_RULES.items():
        score[group] = sum(text.count(w) for w in words)
    best = score.most_common()
    n = name.lower()
    if any(k in n for k in ("audio", "sound", "snd", "voice", "music", "g_player_sound", "op_eng_sound")):
        return "audio"
    if n == "replay_state_cache":
        return "audio"
    if any(k in n for k in ("mouse", "joy", "keyboard", "key_status", "input_device", "input_status", "line_input")):
        return "input"
    if n.startswith("camera") or n.startswith("cam_") or "camera_" in n:
        return "camera"
    if any(k in n for k in ("menu", "hiscore", "highscore", "score_entry", "selection", "dialog", "popup", "textbounds", "cursor_flash")):
        return "menu_ui"
    if any(k in n for k in ("projection_", "projected_", "transformed_", "sprite", "shape", "poly", "pixel", "video_", "screen_", "font", "palette", "rect", "draw_")):
        return "render_video"
    if any(k in n for k in ("replay", "rpl", "tdreplay", "replayshapes")):
        return "track_replay"
    if n == "randomseeds":
        return "simulation"
    if not best or best[0][1] == 0:
        return "core_or_unclassified"
    if len(best) > 1 and best[0][1] == best[1][1]:
        return "shared/" + "+".join(sorted(g for g, n in best if n == best[0][1]))
    return best[0][0]


def type_of_decl(stmt: str, name: str):
    """Best-effort C declarator type spelling without initializer."""
    raw = re.sub(r"/\*.*?\*/|//[^\n]*", " ", stmt, flags=re.S).strip().rstrip(";")
    raw = re.sub(r"\b(extern|static|register)\b", "", raw)
    # Restrict to simple comma-separated file-scope declarators.
    chunks = []
    start = 0
    paren = bracket = brace = 0
    for i, ch in enumerate(raw):
        if ch == "(": paren += 1
        elif ch == ")": paren -= 1
        elif ch == "[": bracket += 1
        elif ch == "]": bracket -= 1
        elif ch == "{": brace += 1
        elif ch == "}": brace -= 1
        elif ch == "," and paren == bracket == brace == 0:
            chunks.append(raw[start:i].strip()); start = i + 1
    chunks.append(raw[start:].strip())
    base = ""
    name_chunk = None
    for ix, chunk in enumerate(chunks):
        no_init = chunk.split("=", 1)[0].strip()
        if re.search(r"\b" + re.escape(name) + r"\b", no_init):
            name_chunk = no_init
            if ix == 0:
                loc = re.search(r"\b" + re.escape(name) + r"\b", no_init)
                base = no_init[:loc.start()].strip()
            else:
                first = chunks[0].split("=", 1)[0].strip()
                first_name = re.search(r"\b([A-Za-z_]\w*)\s*(?:\[[^]]*\])?\s*$", first)
                if first_name:
                    base = first[:first_name.start()].strip()
                else:
                    base = first
                loc = re.search(r"\b" + re.escape(name) + r"\b", no_init)
                base += " " + no_init[:loc.start()].strip()
            break
    if name_chunk is None:
        return None
    # If declaration contains a function pointer, keep the literal type but don't size it.
    array = re.search(r"\b" + re.escape(name) + r"\b\s*((?:\[[^]]*\]\s*)+)", name_chunk)
    dims = [x.strip() for x in re.findall(r"\[([^]]*)\]", array.group(1))] if array else []
    inferred_size = None
    if dims and all(not x for x in dims) and re.search(r"\b(?:char|u8|s8)\b", base):
        literal = re.search(r"\b" + re.escape(name) + r"\s*\[\s*\]\s*=\s*(\"(?:\\.|[^\"])*\")", raw)
        if literal:
            try:
                inferred_size = len(ast.literal_eval(literal.group(1)).encode("latin1")) + 1
            except Exception:
                inferred_size = None
    tail = name_chunk[name_chunk.find(name) + len(name):].split("=", 1)[0].strip()
    size_base = base + (" " + tail if "*" in tail or re.search(r"\b(?:near|far)\b", tail) else "")
    return {"spelling": re.sub(r"\s+", " ", (base + " " + tail).strip()),
            "base": re.sub(r"\s+", " ", size_base).strip(), "dims": dims,
            "inferred_size_bytes": inferred_size}


def parse_type_size(base, dims, struct_sizes, typedef_sizes):
    t = re.sub(r"\b(const|volatile|extern|static|register|near|far)\b", " ", base or "")
    t = re.sub(r"\s+", " ", t).strip()
    if "*" in t:
        ptr_size = 4 if re.search(r"\bfar\b", base or "") else 2
        size = ptr_size
    elif t in ("char", "signed char", "unsigned char", "u8", "s8"):
        size = 1
    elif t in ("short", "unsigned short", "signed short", "int", "unsigned", "unsigned int", "signed", "signed int", "u16", "s16"):
        size = typedef_sizes.get(t, 2)
    elif t in ("long", "unsigned long", "signed long", "u32", "s32"):
        size = typedef_sizes.get(t, 4)
    elif t in ("float",): size = 4
    elif t in ("double",): size = 8
    elif re.fullmatch(r"(?:struct|union)\s+[A-Za-z_]\w*", t):
        size = struct_sizes.get(t)
    else:
        size = typedef_sizes.get(t)
    if size is None:
        return None
    count = 1
    for dim in dims:
        if not dim or not dim.isdigit():
            return None
        count *= int(dim)
    return size * count


def source_alias_data(sources, registered_names, source_identifiers=None):
    """Read explicit accepted-source comments that map a binding name to a TU spelling."""
    result = defaultdict(list)
    for path, text in sources.items():
        if not path.lower().endswith(".c"):
            continue
        live_code = mask_noncode(text)
        for comment in re.finditer(r"/\*.*?\*/|//[^\n]*", text, re.S):
            body = comment.group(0)
            for new_name, old_name in re.findall(r"\b([A-Za-z_]\w*)\s*=\s*([A-Za-z_]\w*)\b", body):
                if new_name in registered_names and re.search(r"\b" + re.escape(old_name) + r"\b", live_code):
                    result[new_name].append({"source": path, "line": line_no(text, comment.start()), "spelling": old_name})
    for name, spelling in (source_identifiers or {}).items():
        if name not in registered_names or not spelling:
            continue
        for path, text in sources.items():
            if not path.lower().endswith(".c"):
                continue
            live_code = mask_noncode(text)
            match = re.search(r"\b" + re.escape(spelling) + r"\b", live_code)
            if match:
                result[name].append({"source": path, "line": line_no(text, match.start()), "spelling": spelling,
                                     "kind": "registry_source_identifier"})
                break
    return result


def data_declaration(name, sources, func_lines, declaration_cache, source_aliases=None):
    hits = []
    spellings = [name, "_" + name.lstrip("_")]
    for row in (source_aliases or {}).get(name, []):
        spellings.append(row["spelling"])
    spellings = list(dict.fromkeys(spellings))
    for path, text in sources.items():
        if path not in func_lines:
            continue
        ranges, by_line, code = func_lines[path]
        decls = declaration_cache.get(path, [])
        for start, end, stmt in decls:
            clean_stmt = mask_noncode(stmt)
            matched = next((sp for sp in spellings if re.search(r"\b" + re.escape(sp) + r"\b", clean_stmt)), None)
            if matched:
                if re.search(r"\b(?:struct|union)\s+\w+\s*\{", clean_stmt):
                    continue
                info = type_of_decl(stmt, matched)
                if info:
                    hits.append({"path": path, "line": line_no(text, start), "declaration": stmt,
                                 "type": info["spelling"], "base": info["base"], "dims": info["dims"],
                                 "inferred_size_bytes": info.get("inferred_size_bytes")})
        # File-scope static objects declared inside a function body retain
        # static storage and appear in the address registry; capture these too.
        lines = text.splitlines()
        for no, line in enumerate(lines, 1):
            if not by_line.get(no):
                continue
            code_line = mask_noncode(line)
            if re.match(r"\s*static\b[^;{}=]*\([^;{}]*\)\s*(?:\{|$)", code_line):
                continue
            matched = next((sp for sp in spellings if re.search(r"\b" + re.escape(sp) + r"\b", code_line)), None)
            if not matched or not re.search(r"\bstatic\b", code_line):
                continue
            stmt = line
            j = no
            while ";" not in mask_noncode(stmt) and j < min(len(lines), no + 8):
                stmt += "\n" + lines[j]
                j += 1
            info = type_of_decl(stmt, matched)
            if info:
                hits.append({"path": path, "line": no, "declaration": stmt.strip(),
                             "type": info["spelling"], "base": info["base"], "dims": info["dims"],
                             "inferred_size_bytes": info.get("inferred_size_bytes")})
    # Prefer a defining tentative/initialized declaration over an extern.
    hits.sort(key=lambda h: ("extern" in h["declaration"], "static" not in h["declaration"], len(h["declaration"]), h["path"], h["line"]))
    return hits


def build_declaration_index(sources, func_lines, declaration_cache, registered_names, source_aliases):
    """Index accepted file-scope and function-local static declarations in one pass."""
    alias_to_names = defaultdict(set)
    for name in registered_names:
        for alias in (name, "_" + name.lstrip("_")):
            alias_to_names[alias].add(name)
        for row in source_aliases.get(name, []):
            alias_to_names[row["spelling"]].add(name)
    token_pat = re.compile(r"[A-Za-z_][A-Za-z_0-9]*")
    index = defaultdict(list)
    for path, text in sources.items():
        if path not in func_lines:
            continue
        _, by_line, _ = func_lines[path]
        def add_stmt(stmt, line):
            if re.search(r"\b(?:struct|union)\s+\w+\s*\{", mask_noncode(stmt)):
                return
            clean = mask_noncode(stmt)
            for token in {m.group(0) for m in token_pat.finditer(clean)}:
                for name in alias_to_names.get(token, ()):
                    info = type_of_decl(stmt, token)
                    if info:
                        index[name].append({"path": path, "line": line, "declaration": stmt.strip(),
                                            "type": info["spelling"], "base": info["base"], "dims": info["dims"],
                                            "inferred_size_bytes": info.get("inferred_size_bytes")})
        for start, end, stmt in declaration_cache.get(path, []):
            add_stmt(stmt, line_no(text, start))
        lines = text.splitlines()
        for no, line in enumerate(lines, 1):
            if not by_line.get(no) or not re.search(r"\bstatic\b", mask_noncode(line)):
                continue
            code_line = mask_noncode(line)
            if re.match(r"\s*static\b[^;{}=]*\([^;{}]*\)\s*(?:\{|$)", code_line):
                continue
            stmt = line
            j = no
            while ";" not in mask_noncode(stmt) and j < min(len(lines), no + 8):
                stmt += "\n" + lines[j]
                j += 1
            add_stmt(stmt, no)
    for name, hits in index.items():
        hits.sort(key=lambda h: ("extern" in h["declaration"], "static" not in h["declaration"], len(h["declaration"]), h["path"], h["line"]))
    return index


def build_type_use_index(sources, c_funcs):
    """Index tagged type mentions by TU and enclosing function with source lines."""
    result = defaultdict(list)
    pat = re.compile(r"\b(struct|union)\s+([A-Za-z_]\w*)\b")
    for path, (ranges, by_line, code) in c_funcs.items():
        for no, line in enumerate(code.splitlines(), 1):
            fn = by_line.get(no)
            if not fn:
                continue
            for m in pat.finditer(line):
                result[(path, m.group(2))].append({"function": fn, "source": path, "line": no})
    return result


def struct_typedef_aliases(sources):
    """Map accepted typedef spellings back to their tagged aggregate layout."""
    aliases = {}
    for path, text in sources.items():
        if not path.lower().endswith(".c"):
            continue
        code = mask_noncode(text)
        for d in type_definitions(text, code, path):
            if not d["raw"].lstrip().startswith("typedef"):
                continue
            tail = d["raw"][d["raw"].rfind("}") + 1:]
            m = re.search(r"\b([A-Za-z_]\w*)\s*;", tail)
            if m:
                aliases[m.group(1)] = d["tag"]
        for m in re.finditer(r"\btypedef\s+(?:struct|union)\s+([A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*;", code):
            aliases[m.group(2)] = m.group(1)
    return aliases


def masm_operand_count(value):
    """Count MASM data items in a db/dw/dd operand list (strings count by bytes)."""
    value = value.split(";", 1)[0].strip()
    if not value:
        return 0
    items, start, depth, quote = [], 0, 0, None
    for i, ch in enumerate(value):
        if quote:
            if ch == quote:
                quote = None
        elif ch in "\"'":
            quote = ch
        elif ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        elif ch == "," and depth == 0:
            items.append(value[start:i].strip()); start = i + 1
    items.append(value[start:].strip())
    count = 0
    for item in items:
        if not item:
            continue
        dup = re.match(r"(?:([\w?]+)\s+)?(\d+)\s+dup\s*\((.*)\)$", item, re.I)
        if dup:
            n = int(dup.group(2))
            inside = masm_operand_count(dup.group(3))
            count += n * max(inside, 1)
        elif len(item) >= 2 and item[0] in "\"'" and item[-1] == item[0]:
            count += len(item[1:-1])
        else:
            count += 1
    return count


def asm_data_declaration(name, sources, alias_row=None):
    """Find accepted MASM data extents and EXTRN storage widths."""
    spellings = [name, "_" + name.lstrip("_")]
    if alias_row and alias_row.get("public_symbol"):
        spellings.append(alias_row["public_symbol"])
    spellings = {s.lower() for s in spellings}
    ext_width = {"byte": (1, "byte"), "word": (2, "word"), "dword": (4, "dword"), "fword": (6, "fword"), "qword": (8, "qword")}
    hits = []
    equ_defs = []
    for path, text in sources.items():
        if not path.lower().endswith(".asm"):
            continue
        lines = text.splitlines()
        for no, line in enumerate(lines, 1):
            clean = line.split(";", 1)[0].strip()
            ext = re.match(r"extrn\s+([\w?$@]+)\s*:\s*(byte|word|dword|fword|qword)\b", clean, re.I)
            if ext and ext.group(1).lower() in spellings:
                b, w = ext_width[ext.group(2).lower()]
                hits.append({"path": path, "line": no, "kind": "external_width", "width": b,
                             "storage_type": w, "declaration": line.strip()})
                continue
            equ = re.match(r"([\w?$@]+)\s+equ\s+(.+)$", clean, re.I)
            if equ and equ.group(1).lower() in spellings:
                equ_defs.append({"path": path, "line": no, "kind": "equ_alias", "expression": equ.group(2).strip(),
                                 "declaration": line.strip()})
                continue
            label = re.match(r"([\w?$@]+)\s+(db|dw|dd)\s*(.*)$", clean, re.I)
            if not label or label.group(1).lower() not in spellings:
                continue
            directive = label.group(2).lower()
            unit = {"db": 1, "dw": 2, "dd": 4}[directive]
            count = masm_operand_count(label.group(3))
            last_no = no
            for cont_no in range(no + 1, len(lines) + 1):
                nxt = lines[cont_no - 1].split(";", 1)[0].strip()
                if not nxt:
                    continue
                continuation = re.match(r"(db|dw|dd)\s*(.*)$", nxt, re.I)
                if continuation:
                    cont_unit = {"db": 1, "dw": 2, "dd": 4}[continuation.group(1).lower()]
                    if cont_unit != unit:
                        break
                    count += masm_operand_count(continuation.group(2))
                    last_no = cont_no
                    continue
                break
            hits.append({"path": path, "line": no, "kind": "definition", "width": unit,
                         "storage_type": {1: "byte", 2: "word", 4: "dword"}[unit],
                         "size_bytes": count * unit, "last_line": last_no,
                         "declaration": line.strip()})
    definitions = [x for x in hits if x["kind"] == "definition"]
    if not definitions and equ_defs:
        widths = [x for x in hits if x["kind"] == "external_width"]
        for equ in equ_defs:
            width = next(iter(widths), None)
            if width:
                equ.update({"kind": "alias_definition", "width": width["width"],
                            "storage_type": width["storage_type"], "size_bytes": width["width"],
                            "width_path": width["path"], "width_line": width["line"],
                            "width_declaration": width["declaration"]})
            definitions.append(equ)
    return definitions if definitions else hits


def build_reference_index(sources, source_functions, registered_names, asm_aliases, source_aliases, pointer_names=None):
    """Scan accepted function bodies once and index direct global references."""
    c_names, asm_names = defaultdict(set), defaultdict(set)
    names = sorted(set(registered_names))
    pointer_names = set(pointer_names or ())
    for name in names:
        candidates = [name, "_" + name.lstrip("_")]
        proposal = asm_aliases.get(name)
        if proposal and proposal.get("public_symbol"):
            candidates.append(proposal["public_symbol"])
        candidates.extend(x["spelling"] for x in source_aliases.get(name, []))
        for alias in set(candidates):
            c_names[alias].add(name)
            asm_names[alias.lower()].add(name)
    index = {name: {"readers": {}, "writers": {}, "escaped": {}, "refs": []} for name in names}
    token_pat = re.compile(r"[A-Za-z_?$@][\w?$@]*")
    asm_rw = {"inc", "dec", "add", "sub", "and", "or", "xor", "not", "neg", "shl", "shr", "rcl", "rcr", "xchg"}
    for path, text in sources.items():
        info = source_functions.get(path)
        if not info:
            continue
        is_asm = path.lower().endswith(".asm")
        if is_asm:
            by_line = info
            lines = text.splitlines()
        else:
            _, by_line, code = info
            lines = code.splitlines()
        for no, line in enumerate(lines, 1):
            fn = by_line.get(no)
            if not fn:
                continue
            clean = line.split(";", 1)[0] if is_asm else line
            tokens = list(token_pat.finditer(clean))
            for ti, match in enumerate(tokens):
                token = match.group(0)
                targets = asm_names.get(token.lower(), set()) if is_asm else c_names.get(token, set())
                if not targets:
                    continue
                row = {"function": fn, "source": path, "line": no}
                before, after = clean[:match.start()], clean[match.end():]
                if is_asm:
                    mnem = re.search(r"\b([a-z]+)\s*(.*)$", clean, re.I)
                    mnemonic = mnem.group(1).lower() if mnem else ""
                    operands = mnem.group(2).split(",", 1) if mnem else [""]
                    first_is_target = bool(operands and re.search(r"\b" + re.escape(token) + r"\b", operands[0], re.I))
                    reads = not first_is_target or mnemonic in asm_rw
                    writes = first_is_target and (mnemonic in asm_rw or (mnemonic == "mov" and len(operands) > 1))
                    address = mnemonic in ("lea", "offset")
                else:
                    # Dot member assignments write the owning aggregate; arrow dereferences
                    # read a global pointer but write the pointed object, not the pointer slot.
                    member_chain = r"(?:\s*(?:\.\s*[A-Za-z_]\w*|\[[^\]]*\]))*"
                    arrow_chain = r"(?:\s*(?:->\s*[A-Za-z_]\w*|\[[^\]]*\]))*"
                    direct_lhs = bool(re.match(r"\s*(?:\[[^\]]*\]\s*)*(?:\+\+|--|(?:\+|-|\*|/|%|&|\||\^)?=(?!=))", after))
                    member_lhs = bool(re.match(member_chain + r"\s*(?:\+\+|--|(?:\+|-|\*|/|%|&|\||\^)?=(?!=))", after))
                    arrow_deref = bool(re.match(arrow_chain, after) and after.lstrip().startswith("->"))
                    writes = direct_lhs or (member_lhs and not arrow_deref)
                    reads = not (direct_lhs or (member_lhs and not arrow_deref))
                    reads = reads or (writes and bool(re.match(r"\s*(?:\+\+|--|[+*/%&|^+-]=)", after)))
                    address = bool(re.search(r"&\s*$", before))
                    increment = bool(re.search(r"(?:\+\+|--)\s*$", before))
                for name in targets:
                    bucket = index[name]
                    target_reads, target_writes = reads, writes
                    if not is_asm and name in pointer_names:
                        simple_pointer_store = bool(re.match(r"\s*(?:\+|-|\*|/|%|&|\||\^)?=(?!=)", after))
                        pointer_increment = increment and not bool(re.search(r"\[[^]]*\]\s*$", before))
                        target_writes = simple_pointer_store or pointer_increment
                        target_reads = not target_writes
                    if not is_asm and increment:
                        target_writes = True
                        target_reads = True
                    if not is_asm and arrow_deref:
                        target_writes = False
                        target_reads = True
                    if target_reads:
                        bucket["readers"][(fn, path)] = row
                    if target_writes:
                        bucket["writers"][(fn, path)] = row
                    if address:
                        bucket["escaped"][(fn, path)] = row
                    access = "address" if address else ("read/write" if target_reads and target_writes else ("write" if target_writes else "read"))
                    bucket["refs"].append({**row, "spelling": token, "access": access})
    for name in names:
        for key in ("readers", "writers", "escaped"):
            index[name][key] = sorted(index[name][key].values(), key=lambda x: (x["source"], x["line"], x["function"]))
    return index


def scan_references(name, sources, source_functions, asm_alias=None, source_alias_rows=None):
    aliases = [name, "_" + name.lstrip("_")]
    if asm_alias:
        aliases.append(asm_alias)
    aliases.extend(x["spelling"] for x in (source_alias_rows or []))
    aliases = list(dict.fromkeys(aliases))
    readers, writers, escaped, refs = {}, {}, {}, []
    for path, text in sources.items():
        lang = "asm" if path.lower().endswith(".asm") else "c"
        function_info = source_functions.get(path)
        if not function_info:
            continue
        if lang == "c":
            _, by_line, code = function_info
            lines = code.splitlines()
            for no, clean in enumerate(lines, 1):
                fn = by_line.get(no)
                if not fn:
                    continue
                for alias in aliases:
                    pat = re.compile(r"\b" + re.escape(alias) + r"\b")
                    for m in pat.finditer(clean):
                        before, after = clean[:m.start()], clean[m.end():]
                        lhs = bool(re.match(r"\s*(?:\[[^]]*\]\s*)*(?:\+\+|--|(?:\+|-|\*|/|%|&|\||\^)?=(?!=))", after))
                        inc = bool(re.search(r"(?:\+\+|--)\s*$", before))
                        addr = bool(re.search(r"&\s*$", before))
                        access = "write" if lhs or inc else "read"
                        row = {"function": fn, "source": path, "line": no}
                        if inc:
                            readers[(fn, path)] = row
                        if addr:
                            escaped[(fn, path)] = row
                        elif access == "write":
                            writers[(fn, path)] = row
                        else:
                            readers[(fn, path)] = row
                        refs.append({**row, "spelling": alias, "access": "address" if addr else access})
        else:
            by_line = function_info
            lines = text.splitlines()
            for no, orig in enumerate(lines, 1):
                fn = by_line.get(no)
                if not fn:
                    continue
                clean = re.sub(r";.*$", "", orig)
                for alias in aliases:
                    pat = re.compile(re.escape(alias), re.I)
                    match = pat.search(clean)
                    if not match:
                        continue
                    left, right = clean[:match.start()], clean[match.end():]
                    # MASM destination operand is the first comma-separated operand.
                    mnemonic = re.search(r"\b(mov|inc|dec|add|sub|and|or|xor|not|neg|shl|shr|rcl|rcr|xchg|stos|lods|cmp|test)\b\s*(.*)$", clean, re.I)
                    access = "read"
                    if mnemonic:
                        operands = mnemonic.group(2).split(",", 1)
                        if mnemonic.group(1).lower() in ("inc", "dec", "not", "neg", "add", "sub", "and", "or", "xor", "shl", "shr", "rcl", "rcr", "xchg"):
                            access = "write"
                        elif len(operands) > 1 and re.search(re.escape(alias), operands[0], re.I):
                            access = "write"
                    row = {"function": fn, "source": path, "line": no}
                    (writers if access == "write" else readers)[(fn, path)] = row
                    refs.append({**row, "spelling": alias, "access": access})
    sortref = lambda d: sorted(d.values(), key=lambda x: (x["source"], x["line"], x["function"]))
    return sortref(readers), sortref(writers), sortref(escaped), refs


def owner_for_address(address, communal, intervals, active):
    if communal:
        ids = communal.get("declarers", [])
        return ids
    rows = []
    for start, end, owner in intervals:
        if start <= address < end:
            rows.append(owner.get("id"))
    return sorted(set(rows))


def asm_alias_data(registry, data_symbols, sources):
    """Ground accepted MASM data aliases from tracked address and source data.

    The canonical symbol map supplies an address-qualified public spelling;
    the accepted ASM source must still contain its data declaration. This
    replaces the earlier worker-proposal input without weakening that anchor.
    """
    result = {}
    symbols_by_address = defaultdict(list)
    for public, metadata in data_symbols.get("symbols", {}).items():
        address = metadata.get("load_address")
        if isinstance(address, int):
            symbols_by_address[address].append((public, metadata))
    for address_text, entry in registry.get("names", {}).items():
        if entry.get("kind") != "data":
            continue
        try:
            address = int(address_text)
        except (TypeError, ValueError):
            continue
        alias = entry.get("name")
        if not alias:
            continue
        for public, metadata in symbols_by_address.get(address, []):
            candidate = {"public_symbol": public}
            if not asm_data_declaration(alias, sources, candidate):
                continue
            result[alias] = {"public_symbol": public, "evidence_file": "layout/data-symbols.json",
                             "rationale": "The canonical symbol map ties this public to the alias address, and the accepted ASM contribution contains its declaration.",
                             "address": address, "references": [], "assigned_occurrences": [],
                             "storage": metadata.get("storage")}
            break
    return result

def enum_unit_lifetime_replay(name, typ, owner_subsystem, readers, writers, address, evidence):
    n = name.lower()
    funcs = [x["function"].lower() for x in readers + writers]
    allfunc = " ".join(funcs)
    unit = "raw integer/byte representation; physical unit not established"
    unit_basis = []
    if "*" in (typ or ""):
        if re.search(r"\bhuge\b", typ, re.I):
            unit = "16-bit huge pointer (segment:offset address); pointee storage is separately owned"
        elif re.search(r"\bfar\b", typ, re.I):
            unit = "16-bit far pointer (segment:offset address); pointee storage is separately owned"
        else:
            unit = "16-bit near offset pointer; pointee storage is separately owned"
    for dim in ("x", "y", "z"):
        if n.endswith("_" + dim) and ("camera" in n or "camera" in allfunc):
            unit = "camera-space coordinate; scale not established"
            break
    if any(k in n for k in ("projection_half_width", "projection_half_height", "projection_left_origin", "projection_top_origin", "projection_center_x", "projection_center_y")):
        unit = "screen-space projection extent/origin in integer pixels"
        unit_basis = [{"citation": "asm/projection_vector_window.ASM:11-27"}]
    elif any(k in n for k in ("projection_x_angle", "projection_y_angle")):
        unit = "projection angle parameter; source uses turn-based trig, but scale for this stored value is not established"
        unit_basis = [{"citation": "asm/projection_vector_window.ASM:11-27"}, {"citation": "asm/sincos.ASM:273-332"}]
    elif any(k in n for k in ("projection_x_scale", "projection_y_scale")):
        unit = "projection scale coefficient; fixed-point denominator not established"
        unit_basis = [{"citation": "asm/projection_vector_window.ASM:11-27"}]
    elif "speed" in n and "car" in n:
        unit = "speed fields in CARSTATE use unsigned Q8 mph where documented; this object's specific scale is not established"
        unit_basis = [{"citation": "src/obj_seg003.c:101-107"}]
    elif (any(k in n for k in ("mouse_x", "mouse_y", "mousex", "mousey", "mouse_api_y", "cursorx", "msecoordx", "pos_x", "pos_y"))
          and owner_subsystem in ("input", "render_video", "menu_ui")):
        unit = "screen-space integer pixel coordinate"
        unit_basis = [{"citation": "src/seg017_mouse_whole.c:40-68"}]
    elif "angle" in n or "rot" in n:
        unit = "integer angle/rotation parameter; where passed to fast trig, a turn is 0x400 units; field-specific scale not established"
        unit_basis = [{"citation": "asm/sincos.ASM:273-332"}]
    elif any(k in n for k in ("tick", "timer", "clock", "countdown")):
        unit = "timer/tick count or control flag; tick duration must be taken from timer source"
        unit_basis = [{"citation": "asm/timer_counter_deadline_helpers.ASM:57-84"}]
    if owner_subsystem in ("simulation", "track_replay") and any(k in n for k in ("pos", "coord", "world", "track", "row", "col", "center")):
        unit = "simulation/track coordinate or index; field-specific coordinate frame/scale not established"
        unit_basis = [{"citation": "src/obj_seg001_complete.c:740-745"}, {"citation": "src/obj_seg001_complete.c:2699-2711"}]
    if owner_subsystem == "render_video" and any(k in n for k in ("x", "y", "left", "right", "top", "bottom", "width", "height")):
        unit = "screen/raster coordinate or extent; field-specific pixel/fixed-point scale not established"
        unit_basis = [{"citation": "src/obj_seg006.c:408-415"}]
    if "*" in (typ or ""):
        if re.search(r"\bhuge\b", typ, re.I):
            unit = "16-bit huge pointer (segment:offset address); pointee storage is separately owned"
        elif re.search(r"\bfar\b", typ, re.I):
            unit = "16-bit far pointer (segment:offset address); pointee storage is separately owned"
        else:
            unit = "16-bit near offset pointer; pointee storage is separately owned"

    if "*" in (typ or ""):
        lifetime = "process-local address/handle; pointee lifetime follows its owning resource or state allocation"
    elif "timer" in n or "tick" in n or owner_subsystem == "timer_platform":
        lifetime = "interrupt/timer lifetime"
    elif owner_subsystem == "simulation":
        lifetime = "race state; updated or consumed by simulation routines"
    elif owner_subsystem == "track_replay":
        lifetime = "track/race or replay lifetime; exact reset point not established"
    elif owner_subsystem == "render_video":
        lifetime = "frame/render/resource lifetime; exact reset point not established"
    elif owner_subsystem in ("audio", "input", "menu_ui"):
        lifetime = "session/device/UI lifetime; exact reset point not established"
    elif not writers:
        lifetime = "persistent initialized data"
    else:
        lifetime = "not established from accepted access sites"

    replay_class = "unknown"
    preserve = None
    reason = "The accepted access/declaration evidence does not establish whether this value is part of a replay snapshot."
    if "*" in (typ or ""):
        replay_class, preserve = "process_local_pointer_or_handle", False
        reason = "The 16-bit pointer value is process/address-space state. Preserve its deterministic pointee contents separately when they are replay input or simulation state."
    elif re.search(r"\b(struct\s+)?(GAMESTATE|CARSTATE)\b", typ or "") or n in ("state", "core", "gamestate"):
        replay_class, preserve = "simulation_snapshot", True
        reason = "Simulation state and checkpoint structures are read and written by game-state update/restore routines."
    elif n in ("g_tdreplay16buf", "replay_steer_flag", "replay_axis_magnitude"):
        replay_class, preserve = "normalized_replay_input_or_cursor", True
        reason = "Accepted replay code consumes/stores this ordered steering/input representation; preserve its bytes and frame association exactly."
    elif n in ("replay_state_cache", "replay_control", "replay_file", "g_replaybarcpytgl", "replaybar_toggle", "replayrst", "replayshapes"):
        replay_class, preserve = "replay_presentation_or_selection_state", False
        reason = "This is playback UI/control, a file/resource identifier, or a presentation asset reference; the replay input bytes and simulation snapshot are authoritative instead."
    elif re.search(r"\bstruct\s+GAMESTATE_SNAPSHOT\b", typ or "") or n == "race_stats":
        replay_class, preserve = "derived_race_summary", False
        reason = "This summary is materialized from fields in GAMESTATE at race end; replay determinism comes from the source state and input, not this derived summary copy."
    elif n == "randomseeds":
        replay_class, preserve = "simulation_random_generator_state", True
        reason = "The accepted replay checkpoint stores the six-byte seed in GAMESTATE and restore reinitializes the random generator from it."
    elif n == "globalgamesettings":
        replay_class, preserve = "deterministic_race_configuration", True
        reason = "Player/opponent car, material, transmission and track settings choose simulation inputs; replay must load the same configuration."
    elif n == "gmconfigbackup":
        replay_class, preserve = "menu_configuration_backup", False
        reason = "This is the configuration copy restored after the menu flow; it is setup/UI state, not per-frame simulation state."
    elif n in {"hillconsts", "collision_point_set_a", "collision_point_set_b", "collision_point_set_c",
                "collision_rotation_offsets", "speed_recovery_divisors", "collision_point_x_signs",
                "collision_point_y_signs", "trklst", "road_elem_ctrz", "ophys_7"}:
        replay_class, preserve = "deterministic_track_or_vehicle_simulation_input", True
        reason = "Accepted simulation/track routines read this table or value to place cars, resolve collision/grip, or interpret the selected track-object geometry; preserve or reproduce the exact backing input for replay."
    elif owner_subsystem == "timer_platform":
        replay_class, preserve = "process_timer_or_interrupt_state", False
        reason = "This is live timer/callback/interrupt machinery. Replay determinism follows the frame-indexed input and GAMESTATE; process timer state is rebuilt by startup/runtime registration."
    elif owner_subsystem == "resource_io":
        replay_class, preserve = "resource_loader_or_handle_state", False
        reason = "This value belongs to resource lookup/loading or a loaded presentation asset; reproduce the same resource inputs, but do not copy the process loader state into a replay snapshot."
    elif "audio" in owner_subsystem:
        replay_class, preserve = "audio_presentation_or_driver_state", False
        reason = "Audio voices, profiles, chunks, and playback cursors are presentation/driver state derived from simulation and loaded audio resources."
    elif n in ("fallback_rotation_cache", "plane_rotation_cache"):
        replay_class, preserve = "recomputed_transform_cache", False
        reason = "plnrotop writes these matrices as transform workspaces from the current rotation inputs before consuming them; they are recomputed derived math state, not replay checkpoint input."
    elif n == "pln_rot_output":
        replay_class, preserve = "derived_simulation_frame_scratch", False
        reason = "plnrotop derives this vector from current plane/rotation inputs, then update_player_state consumes it in wheel-coordinate calculations; the checkpointed CARSTATE and deterministic track/config inputs are authoritative."
    elif n == "g_penaltytm":
        replay_class, preserve = "derived_penalty_increment_and_display", False
        reason = "player_op computes this increment and adds it immediately to core.game_penalty; the latter is the checkpointed simulation total, while g_penaltytm is also read for display."
    elif n in {"flagsdown", "byte_3b8f2", "byte_3fe00"}:
        replay_class, preserve = "physical_input_or_dialog_state", False
        reason = "This is live keyboard/joystick/dialog state; replay uses the normalized per-frame input bytes instead of preserving device polling state."
    elif n in {"last_tacho", "steering_zone", "steering_dot_x", "steering_dot_y", "last_steering_step",
                "half_scale_flag", "current_num_paints", "obj_flags", "vertex_count", "g_vector_bitix",
                "primitive_type_table", "inverse_power_of_two_table", "function_key_scan_codes",
                "sphere_scanline_profiles", "track_prev_vec", "scene2", "scene3", "hill_offs",
                "fence_offsets", "fence_codes", "fence_off_1", "fence_off_2", "fence_off_3", "fence_off_4",
                "paint_cycle", "detthrlevel", "cur_shps", "sky_hgt_world", "g_skybox_sky_clr",
                "g_skyboxwat_clr", "prevcamrot", "scene_1ht", "scene_2ht", "scene_3ht", "scene_4ht",
                "scene_idx", "maxscnh", "spdneedlegaugeclr", "backlightovr8", "exwd", "car_wheel_offsets",
                "secondveccar", "veccar", "car_dvecs", "player_engine_profile", "opponent_engine_profile",
                "one_through_fourteen_table", "intro_text_color_a", "intro_text_color_b", "intro_text_color_c",
                "intro_text_color_d", "intro_text_color_e", "intro_text_color_f", "intro_text_style_a",
                "intro_text_style_b", "intro_text_style_c", "intro_text_style_d", "intro_text_style_e",
                "intro_text_style_f", "text_cursor_outline_color", "line_edit_x", "line_edit_y",
                "g_hovercolor_idle", "g_animphase", "timeraud", "slomodiv8", "skybox_loaded", "scenery_names",
                "textrespfxchr", "word_3f88e", "g_audchnkvalue", "vidflg4_is1"}:
        replay_class, preserve = "presentation_or_derived_workspace", False
        reason = "Accepted readers/writers place this value in rendering, menu, resource, or audio setup. It is recomputed or reloaded outside the authoritative GAMESTATE/input checkpoint."
    elif funcs and all(fn in {"update_frame", "draw_track_preview", "draw_clip", "skybox_op_helper2", "skybox_op",
                              "load_skybox", "unload_skybox", "load_tracks_menu_shapes", "setup_car_shapes",
                              "shape3d_free_car_shapes", "shape3d_load_car_shapes", "trans_op", "select_rot",
                              "calc_sincos80", "vector_op_unk2", "mouse_timer_sprite_unknown", "read_line",
                              "read_line_helper", "read_line_helper2", "do_joystick_resource_text",
                              "do_key_resource_text", "do_mou_resource_text", "do_pau_restext"} for fn in funcs):
        replay_class, preserve = "presentation_or_derived_workspace", False
        reason = "Every indexed access is in accepted drawing, transformation, menu, or resource-presentation code; the value is not authoritative replay input or checkpoint state."
    elif owner_subsystem in ("simulation", "track_replay") and writers:
        replay_class, preserve = "simulation_or_track_runtime_state", True
        reason = "State is mutated by simulation/track flow and can influence subsequent race state."
    elif owner_subsystem in ("render_video", "camera", "menu_ui", "audio", "input"):
        replay_class, preserve = "presentation_or_device_state", False
        reason = "This value is owned by presentation/device/UI flow, outside the authoritative simulation snapshot."
    elif not writers and ("const" in (typ or "") or owner_subsystem == "pinned_runtime"):
        replay_class, preserve = "immutable_table_or_configuration", False
        reason = "Initialized read-only table/configuration is build/resource input, not mutable replay state."
    if owner_subsystem == "input" and any(k in n for k in ("replay", "inputmode", "input_byte")):
        replay_class, preserve = "normalized_replay_input_or_cursor", True
        reason = "The per-frame normalized input representation, rather than physical-device polling state, drives replay."
    if n == "g_tdreplay16buf":
        replay_class, preserve = "process_local_pointer_to_replay_input", False
        reason = "The far pointer slot is process-local, but the ordered bytes it addresses are authoritative replay input."

    return {"value": unit, "confidence": "exact-declaration/access-evidence" if unit != "raw integer/byte representation; physical unit not established" else "unknown",
            "evidence": unit_basis}, {"value": lifetime, "confidence": "subsystem/access-flow inference" if lifetime != "not established from accepted access sites" else "unknown",
            "evidence": evidence[:4]}, {"class": replay_class, "preserve_bit_exactly": preserve,
            "reason": reason, "evidence": evidence[:6]}


def build_model(run_probes=True):
    (manifest, registry, data_symbols, communal_doc, owner_by_id, source_owner,
     active, symbol_storage, communal_by_address, intervals) = load_inputs()
    extent_evidence = read_json(ROOT / "tools/porting/object-extent-evidence.json")
    sources = {}
    source_functions = {}
    c_funcs = {}
    declaration_cache = {}
    for owner in active:
        path = owner["source"]
        if path in sources:
            continue
        text = (ROOT / path).read_text(encoding="utf-8", errors="replace")
        sources[path] = text
        if owner["language"] == "c":
            ranges, by_line, code = c_functions(text)
            c_funcs[path] = (ranges, by_line, code)
            declaration_cache[path] = top_level_declarations(text, code, ranges)
            source_functions[path] = c_funcs[path]
        else:
            source_functions[path] = asm_functions(text)
    variants = collect_probes(active, generate=run_probes)
    struct_size = {}
    for probe_key, row in variants.items():
        source = row.get("source")
        type_tag = row.get("tag")
        type_line = row.get("line")
        for _, size in row.get("size_bytes", {}).items():
            struct_size[(source, type_tag, type_line)] = size

    # Fold tentative communal declarations into the exact linker-proven object extents.
    communal_by_name = {}
    for c in communal_doc["communals"]:
        name = c["name"].lstrip("_")
        communal_by_name[name] = c
    aliases = asm_alias_data(registry, data_symbols, sources)
    registry_names = {v["name"] for v in registry["names"].values() if v.get("kind") == "data"}
    source_identifiers = {}
    for entry in registry["names"].values():
        if entry.get("kind") == "data" and entry.get("source_identifier"):
            source_identifiers[entry["name"]] = entry["source_identifier"]
    source_aliases = source_alias_data(sources, registry_names, source_identifiers)
    declaration_index = build_declaration_index(sources, c_funcs, declaration_cache, registry_names, source_aliases)
    pointer_names = {name for name, hits in declaration_index.items()
                     if any("*" in (hit.get("base") or "") and not hit.get("dims") for hit in hits)}
    pointer_names.update(c["name"].lstrip("_") for c in communal_doc["communals"]
                         if "*" in (c.get("size_basis", {}).get("declaration") or ""))
    reference_index = build_reference_index(sources, source_functions,
        registry_names, aliases, source_aliases, pointer_names)
    type_use_index = build_type_use_index(sources, c_funcs)

    data_rows = []
    registry_data = [(int(a), v) for a, v in registry["names"].items() if v.get("kind") == "data"]
    for address, entry in sorted(registry_data):
        name = entry["name"]
        communal = communal_by_address.get(address)
        owner_ids = owner_for_address(address, communal, intervals, active)
        owner_ids = [i for i in owner_ids if i in owner_by_id]
        proposal = aliases.get(name)
        asm_alias = proposal.get("public_symbol") if proposal else None
        decls = declaration_index.get(name, [])
        decl = decls[0] if decls else None
        asm_decls = asm_data_declaration(name, sources, proposal)
        asm_decl = asm_decls[0] if asm_decls else None
        # ASM labels retain their actual accepted public spellings; registry spellings are binding aliases.
        evidence = [{"kind": "image_address", "citation": f"MCGA load-image address 0x{address:05X} ({address})"}]
        if communal:
            size = communal.get("size")
            communal_declaration = communal.get("size_basis", {}).get("declaration")
            if decl:
                declaration = decl["declaration"]
                type_text = decl["type"]
                if decl.get("dims") == [""] and "*" not in decl.get("base", ""):
                    element = parse_type_size(decl["base"], [], {}, {})
                    if element and size is not None and size % element == 0:
                        type_text = f"{decl['base']}[{size // element}]"
                evidence.append({"kind": "accepted_c_declaration", "citation": f"{decl['path']}:{decl['line']}", "text": decl["declaration"]})
                # The registry contains a typed GAMESTATE_SNAPSHOT view named
                # race_stats at the same load address as a four-byte common
                # symbol. Keep the typed view's accepted extent instead of
                # inheriting the unrelated common row's byte count.
                if name == "race_stats" and re.search(r"\bGAMESTATE_SNAPSHOT\b", type_text):
                    snapshot_sizes = [n for (src, tag, type_line), n in struct_size.items()
                                      if src == decl["path"] and tag == "struct GAMESTATE_SNAPSHOT"
                                      and type_line <= decl["line"]]
                    if snapshot_sizes:
                        size = snapshot_sizes[-1]
                        snapshot_probe = next((v for v in variants.values()
                                               if v.get("source") == decl["path"]
                                               and v.get("tag") == "struct GAMESTATE_SNAPSHOT"
                                               and v.get("line", 0) <= decl["line"]
                                               and v.get("size_bytes")), {})
                        race_write = next(((src, line_no(text, match.start()))
                                           for src, text in sources.items()
                                           for match in re.finditer(r"\brace_stats\s*=", mask_noncode(text))), None)
                        overlay_cites = [f"{decl['path']}:{decl['line']}"]
                        if race_write:
                            overlay_cites.append(f"{race_write[0]}:{race_write[1]}")
                        evidence.append({"kind": "registry_typed_overlay", "citation": "layout/names-registry.json; " + "; ".join(overlay_cites),
                                         "text": f"race_stats is the accepted {size}-byte GAMESTATE_SNAPSHOT view at this address; the LINK communal record at the same address names _total_game and has a separate four-byte declaration."})
                        evidence.append({"kind": "compiler_type_size_probe", "citation": snapshot_probe.get("report", "accepted GAMESTATE_SNAPSHOT sizeof probe"),
                                         "candidate": snapshot_probe.get("candidate"), "size_bytes": size,
                                         "method": "isolated canonical MSC sizeof probe"})
            else:
                declaration = communal_declaration
                info = None
                for candidate_name in dict.fromkeys((communal.get("name", name), name, "_" + name.lstrip("_"))):
                    info = type_of_decl(declaration or "", candidate_name)
                    if info:
                        break
                type_text = info["spelling"] if info else (declaration or "communal declaration type unresolved")
            evidence.append({"kind": "communal_declaration", "citation": f"{communal.get('declarers', ['?'])[0]} tentative declaration; exact linker declaration: {communal_declaration}"})
        elif decl:
            declaration = decl["declaration"]
            type_text = decl["type"]
            sizes = {}
            for (src, type_tag, type_line), type_bytes in struct_size.items():
                if src == decl["path"] and type_line <= decl["line"]:
                    sizes[type_tag] = type_bytes
            size = decl.get("inferred_size_bytes")
            if size is None:
                size = parse_type_size(decl["base"], decl["dims"], sizes, {})
            evidence.append({"kind": "c_declaration", "citation": f"{decl['path']}:{decl['line']}", "text": decl["declaration"]})
            if address == 192414 and any("library_ctype" in x for x in owner_ids):
                owner_interval = next((o for a, b, o in intervals if a == address and "library_ctype" in o.get("id", "")), None)
                if owner_interval:
                    size = owner_interval["end"] - owner_interval["start"]
                    type_text = f"unsigned char[{size}]"
                    evidence.append({"kind": "pinned_runtime_extent", "citation": "layout/manifest.json: library_ctype_192414:_DATA [192414,192671)",
                                     "text": f"Pinned runtime DATA contribution extent; {size} bytes."})
        elif asm_decl and asm_decl.get("kind") in ("definition", "alias_definition"):
            declaration = asm_decl["declaration"]
            type_text = (f"MASM {asm_decl['storage_type']} alias of {asm_decl.get('expression')}"
                         if asm_decl.get("kind") == "alias_definition" else f"MASM {asm_decl['storage_type']} data object")
            size = asm_decl.get("size_bytes")
            evidence.append({"kind": "accepted_asm_data_definition", "citation": f"{asm_decl['path']}:{asm_decl['line']}",
                             "through_line": asm_decl.get("last_line"), "text": asm_decl["declaration"]})
            if asm_decl.get("width_path"):
                evidence.append({"kind": "accepted_asm_external_width", "citation": f"{asm_decl['width_path']}:{asm_decl['width_line']}",
                                 "text": asm_decl["width_declaration"]})
        elif proposal:
            declaration = asm_decl.get("declaration") if asm_decl else None
            if asm_decl and asm_decl.get("kind") == "external_width":
                type_text = f"MASM external {asm_decl['storage_type']} (object extent unresolved)"
            else:
                type_text = "ASM-owned data; storage width/aggregate type not declared in accepted source"
            size = asm_decl.get("size_bytes") if asm_decl and asm_decl.get("kind") == "definition" else None
            if proposal.get("evidence_file"):
                evidence.append({"kind": "semantic_alias", "citation": proposal["evidence_file"], "text": proposal.get("rationale")})
            if asm_decl:
                evidence.append({"kind": "accepted_asm_symbol", "citation": f"{asm_decl['path']}:{asm_decl['line']}",
                                 "text": asm_decl["declaration"]})
        else:
            declaration = None
            type_text = "unresolved from accepted source"
            size = None
        # The linker interval identifies the storage-owning contribution, not necessarily the logical consumer.
        owner_sources = [source_owner.get(owner_by_id[o].get("parent", o)) or source_owner.get(o) for o in owner_ids]
        owner_sources = [x for x in owner_sources if x]
        owner_id = owner_ids[0] if owner_ids else "unmapped"
        access = reference_index.get(name, {"readers": [], "writers": [], "escaped": [], "refs": []})
        readers, writers, escaped, refs = access["readers"], access["writers"], access["escaped"], access["refs"]
        if name == "audio_frmarr":
            # The byte-array identifier is read to form a struct pointer, but
            # the buffer payload accesses occur through AUDIO_CAR_FRAME *.
            # Close that narrow pointer alias using the accepted assignment
            # and consumer bodies instead of reporting pointer formation as a
            # content read.
            audio_text = mask_noncode(sources.get("src/obj_seg001_complete.c", ""))
            audio_write = re.search(r"audioRecord\s*->\s*player_offsets\s*\[\s*0\s*\]\s*=", audio_text)
            audio_read = re.search(r"record\s*->\s*player_rpm", audio_text)
            readers = ([{"function": "apply_audio_frame", "source": "src/obj_seg001_complete.c",
                         "line": line_no(audio_text, audio_read.start())}] if audio_read else [])
            writers = ([{"function": "audio_carstate", "source": "src/obj_seg001_complete.c",
                         "line": line_no(audio_text, audio_write.start())}] if audio_write else [])
            refs = ([{"function": "audio_carstate", "source": "src/obj_seg001_complete.c",
                      "line": line_no(audio_text, audio_write.start()), "access": "indirect buffer write through AUDIO_CAR_FRAME *"}] if audio_write else [])
            refs += ([{"function": "apply_audio_frame", "source": "src/obj_seg001_complete.c",
                       "line": line_no(audio_text, audio_read.start()), "access": "indirect buffer read through AUDIO_CAR_FRAME *"}] if audio_read else [])
        evidence.extend({"kind": "access", "citation": f"{x['source']}:{x['line']}", "function": x["function"], "access": x["access"]}
                        for x in refs[:24])
        owner_subsystem = subsystem_for(name, [x["function"] for x in readers + writers], owner_id)
        if any(owner_by_id[o].get("kind") == "KNOWN_TOOLCHAIN_LIBRARY_DATA" for o in owner_ids):
            owner_subsystem = "pinned_runtime"
        # Linker-allocated storage declarations can be recoverable even when no data word is emitted in a C TU.
        if communal:
            owner_subsystem = subsystem_for(name, [x["function"] for x in readers + writers], " ".join(communal.get("declarers", [])))
        evidence.extend({"kind": row.get("kind", "accepted_source_alias"),
                         "citation": f"{row['source']}:{row['line']}", "text": f"{name} is bound to source spelling {row['spelling']}"}
                        for row in source_aliases.get(name, []))
        unit, lifetime, replay = enum_unit_lifetime_replay(name, type_text, owner_subsystem,
                                                            readers, writers, address, evidence)
        pointee_replay = None
        if name == "g_tdreplay16buf":
            pointee_replay = {"class": "ordered_replay_input_bytes", "preserve_bit_exactly": True,
                              "reason": "The frame-indexed bytes are read by update_gamestate and the pointer slot is only their process-local address.",
                              "evidence": [{"citation": "src/obj_seg001_complete.c:1758-1770"}]}
        elif name == "cvxs_a":
            pointee_replay = {"class": "GAMESTATE_replay_checkpoint_array", "preserve_bit_exactly": True,
                              "reason": "Each pointed checkpoint is assigned a whole GAMESTATE and restored as a whole GAMESTATE.",
                              "evidence": [{"citation": "src/obj_seg001_complete.c:1734-1738"},
                                           {"citation": "src/obj_seg001_complete.c:1758-1770"}]}
        # Semantic aliases from accepted public-name proposals make the uncertainty inspectable.
        size_basis = ("accepted C typed view and canonical compiler sizeof probe" if name == "race_stats" else
            ("LINK communal-unit declared type" if communal else (
            "pinned runtime DATA extent" if address == 192414 and size is not None else
            "accepted C declaration" if decl and size is not None else
            "accepted ASM data definition" if asm_decl and asm_decl.get("kind") in ("definition", "alias_definition") and size is not None else
            "unresolved")))
        if not communal and decl and size is None:
            size_basis = "accepted C declaration; sizeof type not resolved by current probe"
        # If no direct type source exists, preserve the address interval as a diagnostic lower bound only.
        following = [a for a, _ in registry_data if a > address]
        span = min(following) - address if following else None
        primary_aggregate = re.search(r"\b(struct|union)\s+([A-Za-z_]\w*)\b", type_text or "")
        primary_tag = primary_aggregate.group(2) if primary_aggregate else None
        alternate_type_views = []
        if size is not None:
            seen_views = set()
            for candidate in decls:
                cm = re.search(r"\b(struct|union)\s+([A-Za-z_]\w*)\b", candidate.get("type", ""))
                if not cm or "*" in (candidate.get("type") or "") or cm.group(2) == primary_tag:
                    continue
                view_kind, view_tag = cm.group(1), cm.group(2)
                view_key = (view_kind, view_tag, candidate["path"], candidate["line"])
                if view_key in seen_views:
                    continue
                seen_views.add(view_key)
                tag_name = f"{view_kind} {view_tag}"
                measured = [(type_line, n) for (src, ttag, type_line), n in struct_size.items()
                            if src == candidate["path"] and ttag == tag_name and type_line <= candidate["line"]]
                if not measured:
                    continue
                type_line, view_element_size = max(measured, key=lambda x: x[0])
                dims = candidate.get("dims", [])
                if dims and all(d.isdigit() for d in dims):
                    view_count = 1
                    for dim in dims:
                        view_count *= int(dim)
                elif dims and all(not d for d in dims):
                    view_count = size // view_element_size if view_element_size and size % view_element_size == 0 else None
                else:
                    view_count = 1
                if view_count is None or view_element_size * view_count != size:
                    continue
                probe_key = f"{candidate['path']}:{type_line}:{tag_name}"
                type_probe = variants.get(probe_key, {})
                alternate_type_views.append({
                    "tag": view_tag, "kind": view_kind, "address": address, "size_bytes": size,
                    "element_count": view_count, "view_of": name,
                    "evidence": [
                        {"kind": "registered_object_address", "citation": f"MCGA load-image address 0x{address:05X} ({address})"},
                        {"kind": "alternate_accepted_c_declaration", "citation": f"{candidate['path']}:{candidate['line']}",
                         "text": candidate["declaration"]},
                        {"kind": "compiler_type_size_probe", "citation": type_probe.get("report"),
                         "candidate": type_probe.get("candidate"), "size_bytes": view_element_size,
                         "method": "isolated canonical MSC sizeof probe"},
                        {"kind": "backing_object_extent", "citation": f"{decl['path']}:{decl['line']}" if decl else "communal/source object extent",
                         "text": f"The alternate TU declaration views the same {size}-byte registered object."}
                    ]})
        data_rows.append({
            "name": name, "address": address, "address_hex": f"0x{address:05X}",
            "storage": ("pinned_runtime_data" if any(owner_by_id[o].get("kind") == "KNOWN_TOOLCHAIN_LIBRARY_DATA" for o in owner_ids)
                        else ("communal" if communal else (symbol_storage[address][0].get("storage") if symbol_storage.get(address) else "initialized_or_bss_unresolved"))),
            "type": type_text, "size_bytes": size, "size_basis": size_basis,
            "span_to_next_registered_address_bytes": span,
            "owner_subsystem": owner_subsystem, "owner_objects": owner_ids,
            "declaring_translation_units": sorted({x["source"] for x in owner_sources}
                | ({decl["path"]} if decl else set())
                | ({asm_decl["path"]} if asm_decl else set())),
            "readers": readers, "writers": writers, "address_escapes": escaped,
            "alternate_type_views": alternate_type_views,
            "units": unit, "lifetime": lifetime, "replay_determinism": replay,
            "pointee_replay_determinism": pointee_replay,
            "declaration": declaration, "evidence": evidence,
            "coverage": "complete" if size is not None and type_text else "incomplete: type or size unresolved"
        })

    # Preserve each accepted TU's version of a struct tag; same-named tags need not be layout-identical.
    type_rows = []
    all_tags = defaultdict(list)
    for owner in active:
        if owner["language"] != "c":
            continue
        text = sources[owner["source"]]
        defs = type_definitions(text, mask_noncode(text), owner["source"])
        _, by_line, code = c_funcs[owner["source"]]
        for d in defs:
            key = f"{d['kind']} {d['tag']}"
            all_tags[d["tag"]].append((owner, d, struct_size.get((owner["source"], key, d["line"])) ))
    # Start with registered global/static/communal instances, then add embedded
    # members only where an isolated canonical MSC prefix probe measured offsets.
    instances_by_tag = defaultdict(list)
    type_size_by_tag = {}
    for tag, records in all_tags.items():
        measured = [size for _, _, size in records if size is not None]
        if measured:
            type_size_by_tag[tag] = measured[0]
    typedef_aliases = struct_typedef_aliases(sources)
    for row in data_rows:
        row_type = row["type"] or ""
        if "*" in row_type:
            continue
        match = re.search(r"\b(struct|union)\s+([A-Za-z_]\w*)\b", row_type)
        tag = match.group(2) if match else typedef_aliases.get(row_type.split("[", 1)[0].strip())
        if tag:
            dims = re.findall(r"\[([^]]*)\]", row_type)
            element_count = 1
            for dim in dims:
                if dim.strip().isdigit():
                    element_count *= int(dim.strip())
                else:
                    element_count = None
                    break
            instances_by_tag[tag].append({"name": row["name"], "address": row["address"],
                "size_bytes": row["size_bytes"], "element_count": element_count, "owner_subsystem": row["owner_subsystem"],
                "readers": row["readers"], "writers": row["writers"],
                "replay_determinism": row["replay_determinism"], "evidence": row["evidence"][:8]})
        for view in row.get("alternate_type_views", []):
            instances_by_tag[view["tag"]].append({
                "name": f"{row['name']} view as {view['kind']} {view['tag']}",
                "address": view["address"], "size_bytes": view["size_bytes"],
                "element_count": view["element_count"], "owner_subsystem": row["owner_subsystem"],
                "readers": row["readers"], "writers": row["writers"],
                "replay_determinism": row["replay_determinism"], "view_of": row["name"],
                "evidence": view["evidence"]})
    # audio_frmarr is declared as a byte buffer, then indexed through the
    # accepted AUDIO_CAR_FRAME type. Record the typed view without pretending
    # the declared backing object is a struct array.
    audio_buffer = next((r for r in data_rows if r["name"] == "audio_frmarr"), None)
    audio_frame_record = next((d for owner, d, size in all_tags.get("AUDIO_CAR_FRAME", [])
                               if owner["source"] == "src/obj_seg001_complete.c" and size is not None), None)
    audio_frame_size = type_size_by_tag.get("AUDIO_CAR_FRAME")
    if audio_buffer and audio_frame_record and audio_frame_size and audio_buffer["size_bytes"] % audio_frame_size == 0:
        audio_probe = next((v for v in variants.values()
                            if v.get("source") == "src/obj_seg001_complete.c"
                            and v.get("tag") == "struct AUDIO_CAR_FRAME"), {})
        audio_decl_ev = next((e for e in audio_buffer["evidence"] if e.get("kind") in ("accepted_c_declaration", "accepted_c_declaration")), {})
        audio_access = next((x for x in audio_buffer["readers"] + audio_buffer["writers"]
                             if x.get("function") == "audio_carstate"), {})
        audio_source_text = sources.get("src/obj_seg001_complete.c", "")
        audio_cast = re.search(r"\(\s*struct\s+AUDIO_CAR_FRAME\s*\*\s*\)\s*audio_frmarr", mask_noncode(audio_source_text))
        audio_decl_citation = audio_decl_ev.get("citation", "src/obj_seg001_complete.c:unknown")
        audio_cast_citation = (f"src/obj_seg001_complete.c:{line_no(audio_source_text, audio_cast.start())}" if audio_cast else
                               (f"{audio_access['source']}:{audio_access['line']}" if audio_access else
                                 "src/obj_seg001_complete.c:unknown")
                               )
        instances_by_tag["AUDIO_CAR_FRAME"].append({
            "name": "audio_frmarr cast-view as AUDIO_CAR_FRAME",
            "address": audio_buffer["address"],
            "size_bytes": audio_buffer["size_bytes"],
            "element_count": audio_buffer["size_bytes"] // audio_frame_size,
            "owner_subsystem": audio_buffer["owner_subsystem"],
            "readers": audio_buffer["readers"], "writers": audio_buffer["writers"],
            "replay_determinism": audio_buffer["replay_determinism"],
            "view_of": "audio_frmarr",
            "evidence": [
                {"kind": "accepted_byte_buffer_declaration", "citation": audio_decl_citation,
                 "text": f"The accepted declaration keeps audio_frmarr byte-backed, total {audio_buffer['size_bytes']} bytes."},
                {"kind": "accepted_struct_cast_use", "citation": audio_cast_citation,
                 "text": "The code casts audio_frmarr to struct AUDIO_CAR_FRAME * and indexes by sndposrecord."},
                {"kind": "compiler_size_probe", "citation": audio_probe.get("report", "accepted AUDIO_CAR_FRAME sizeof probe"),
                 "candidate": audio_probe.get("candidate"), "size_bytes": audio_frame_size,
                 "method": "isolated canonical MSC sizeof probe"}
            ]})
    member_offset_results = collect_member_offsets(generate=run_probes).get("results", [])
    for member in member_offset_results:
        if member.get("offset_bytes") is None or member.get("status") != "COMPILED" and member.get("status") != "C-first-member-offset":
            continue
        child_tag = member["inner_tag"]
        child_size = type_size_by_tag.get(child_tag)
        if child_size is None:
            continue
        for parent in instances_by_tag.get(member["parent_tag"], []):
            parent_count = parent.get("element_count") or 1
            parent_stride = type_size_by_tag.get(member["parent_tag"])
            if parent_count > 1 and parent_stride is None:
                continue
            offset = member["offset_bytes"]
            count = member.get("array_count", 1)
            cite = {"kind": "accepted_member_declaration", "citation": f"{member['parent_source']}:{member['line']}",
                    "text": member["declaration"].strip()}
            if member.get("report"):
                offset_ev = {"kind": "compiler_prefix_size_probe", "citation": member["report"],
                             "candidate": member.get("candidate"),
                             "method": "MSC /O sizeof of exact accepted aggregate prefix before this embedded member"}
            else:
                offset_ev = {"kind": "C_first_member_offset", "citation": f"{member['parent_source']}:{member['parent_line']}",
                             "method": "First declared member begins at offset zero in a C aggregate"}
            parent_probe = next((v for v in variants.values()
                                 if v.get("source") == member["parent_source"]
                                 and v.get("tag") == f"struct {member['parent_tag']}"
                                 and v.get("size_bytes")), {})
            stride_ev = ({"kind": "parent_compiler_size_probe", "citation": parent_probe.get("report"),
                          "candidate": parent_probe.get("candidate"), "size_bytes": parent_stride,
                          "method": "canonical MSC sizeof(parent tag) used as array element stride"}
                         if parent_count > 1 else None)
            for parent_index in range(parent_count):
                indexed_parent = (f"{parent['name']}[{parent_index}]" if parent_count > 1 else parent["name"])
                child_name = f"{indexed_parent}.{member['field']}"
                child = {"name": child_name,
                         "address": parent["address"] + parent_index * (parent_stride or 0) + offset,
                         "size_bytes": child_size * count, "element_count": count,
                         "container": indexed_parent, "field": member["field"],
                         "owner_subsystem": parent["owner_subsystem"], "readers": parent["readers"], "writers": parent["writers"],
                         "replay_determinism": parent["replay_determinism"],
                         "evidence": [cite, offset_ev] + ([stride_ev] if stride_ev else [])}
                instances_by_tag[child_tag].append(child)
    for tag, records in sorted(all_tags.items()):
        variants_json = []
        aggregate_readers, aggregate_writers = {}, {}
        instances = instances_by_tag.get(tag, [])
        for inst in instances:
            for x in inst.get("readers", []): aggregate_readers[(x["function"], x["source"])] = x
            for x in inst.get("writers", []): aggregate_writers[(x["function"], x["source"])] = x
        nested_instance_evidence = []
        if tag == "CARSTATE":
            nested_instance_evidence.extend(e for i in instances for e in i.get("evidence", []))
        type_units = "Field-specific. Known physical scales and coordinate transforms are listed in field_semantics; otherwise retain declared integer/byte representation without assigning a unit."
        type_lifetime = "Per instance: simulation aggregates last through a race/checkpoint; resource/configuration instances follow their resource/session; frame workspaces are per frame. See instance owner/access evidence."
        if tag in ("GAMESTATE", "CARSTATE"):
            type_replay = {"class": "simulation_snapshot", "preserve_bit_exactly": True,
                           "reason": "The accepted game code copies/restores whole GAMESTATE values and updates both embedded CARSTATE records as simulation proceeds.",
                           "evidence": [{"kind": "state_copy_restore", "citation": "src/obj_seg001_complete.c:1734-1738"},
                                        {"kind": "checkpoint_write", "citation": "src/obj_seg001_complete.c:1758-1770"},
                                        {"kind": "car_update", "citation": "src/obj_seg001_complete.c:933-1060"}]}
        elif tag == "SIMD":
            type_replay = {"class": "deterministic_race_configuration", "preserve_bit_exactly": True,
                           "reason": "SIMD tuning inputs are read by initialization, speed, grip and collision routines; replay needs the same selected tuning configuration.",
                           "evidence": [{"kind": "simulation_use", "citation": "src/obj_seg001_complete.c:1511-1523"},
                                        {"kind": "simulation_use", "citation": "src/obj_seg001_complete.c:2173-2197"},
                                        {"kind": "simulation_use", "citation": "src/obj_seg001_complete.c:2362-2367"},
                                        {"kind": "simulation_use", "citation": "src/obj_seg001_complete.c:3341-3355"}]}
        elif instances:
            instance_values = [x.get("replay_determinism", {}).get("preserve_bit_exactly") for x in instances]
            if instance_values and all(v is True for v in instance_values):
                instance_classes = {x.get("replay_determinism", {}).get("class") for x in instances}
                exact_class = next(iter(instance_classes)) if len(instance_classes) == 1 else "simulation_or_deterministic_configuration_instance"
                type_replay = {"class": exact_class, "preserve_bit_exactly": True,
                               "reason": "Every indexed global instance inherits an exact simulation snapshot/configuration classification from its accepted owner.",
                               "evidence": [e for i in instances for e in i.get("evidence", [])[:2]]}
            elif instance_values and all(v is False for v in instance_values):
                instance_classes = {x.get("replay_determinism", {}).get("class") for x in instances}
                exact_class = next(iter(instance_classes)) if len(instance_classes) == 1 else "presentation_or_device_state"
                type_replay = {"class": exact_class, "preserve_bit_exactly": False,
                               "reason": "Every indexed global instance is classified as non-authoritative replay state in the symbol inventory.",
                               "evidence": [e for i in instances for e in i.get("evidence", [])[:2]]}
            else:
                type_replay = {"class": "instance-dependent-or-unclassified", "preserve_bit_exactly": None,
                               "reason": "Instances use different or incomplete replay classifications; preserve a particular instance when its owning field enters simulation state.",
                               "evidence": [e for i in instances for e in i.get("evidence", [])[:2]]}
        else:
            type_replay = {"class": "no_registered_global_or_embedded_instance", "preserve_bit_exactly": False,
                           "reason": "No instance of this accepted TU layout is present in the names registry or measured inside a registered aggregate. It is therefore a local, parameter, or resource/pointee layout rather than registered global replay state; preserve it only when an owning runtime resource requires it.",
                           "evidence": [{"kind": "type_definition", "citation": f"{owner['source']}:{d['line']}"}
                                        for owner, d, _ in records]}
        for owner, d, size in records:
            decl_refs = []
            # Type mentions show call/type context, not an inferred read or write.
            use_lines = type_use_index.get((owner["source"], tag), [])[:40]
            variant_evidence = [{"kind": "type_definition", "citation": f"{owner['source']}:{d['line']}"}]
            probe_key = f"{owner['source']}:{d['line']}:{d['kind']} {tag}"
            probe = variants.get(probe_key, {})
            if size is not None:
                variant_evidence.append({"kind": "compiler_size_probe", "citation": probe.get("report"), "candidate": probe.get("candidate"),
                                         "method": "MSC /O public byte arrays bracket sizeof(tag); byte-offset delta"})
            variants_json.append({"translation_unit": owner["id"], "source": owner["source"], "line": d["line"],
                                  "size_bytes": size, "size_status": "compiler-measured" if size is not None else "unresolved",
                                  "packing": "pack(1)" if d["pack"] else "default MSC 5.10 packing",
                                  "fields": d["fields"], "type_uses": use_lines,
                                  "readers": sorted(aggregate_readers.values(), key=lambda x: (x["source"], x["line"])),
                                  "writers": sorted(aggregate_writers.values(), key=lambda x: (x["source"], x["line"])),
                                  "evidence": variant_evidence})
            type_replay["evidence"].extend(variant_evidence[:1])
        field_semantics = []
        if tag == "CARSTATE":
            field_semantics = [
                {"field": "car_speed", "unit": "unsigned Q8 mph", "meaning": "revolution-coupled speed field", "evidence": [{"citation": "src/obj_seg003.c:101-107"}]},
                {"field": "car_speed2", "unit": "unsigned Q8 mph", "meaning": "actual translational speed; can diverge from car_speed during jumps", "evidence": [{"citation": "src/obj_seg003.c:101-107"}]},
                {"field": "car_posWorld1", "unit": "world coordinates at 64 times short map/vector coordinates", "meaning": "car world position", "evidence": [{"citation": "src/obj_seg001_complete.c:740-745"}, {"citation": "src/obj_seg001_complete.c:1692-1695"}]},
                {"field": "car_posWorld2", "unit": "world coordinates at 64 times short map/vector coordinates", "meaning": "prior/secondary car position used in simulation", "evidence": [{"citation": "src/obj_seg001_complete.c:2699-2711"}]},
                {"field": "car_36MwhlAngle", "unit": "integer angle consumed by 0x400-turn fast-trig convention", "meaning": "normalized heading difference used as the car's angular skid/impact vector; also passed to cosfast", "evidence": [{"citation": "src/obj_seg001_complete.c:2444-2460"}, {"citation": "src/obj_seg001_complete.c:2527-2537"}, {"citation": "asm/sincos.ASM:273-332"}]}
            ]
        elif tag == "GAMESTATE":
            field_semantics = [
                {"field": "game_frame", "unit": "simulation frame index", "meaning": "indexes g_tdreplay16buf and advances once per state update", "evidence": [{"citation": "src/obj_seg001_complete.c:1758-1770"}]},
                {"field": "game_frames_per_sec", "unit": "frames per second / scheduling rate", "meaning": "rate counter used by game loop timing", "evidence": [{"citation": "src/obj_seg001_complete.c:1770-1778"}]},
                {"field": "game_inputmode", "unit": "enumerated byte flag (0 waiting, 1 active, 2 suppressed during intro)", "meaning": "simulation input mode", "evidence": [{"citation": "src/obj_seg003.c:189"}, {"citation": "src/obj_seg001_complete.c:1758-1763"}]}
            ]
        elif tag == "VECTORLONG":
            field_semantics = [
                {"field": "lx, ly, lz", "unit": "world-coordinate integer at 64 times the short VECTOR/map coordinate", "meaning": "long car world position; conversions shift right by six when entering short simulation/render coordinates", "evidence": [{"citation": "src/obj_seg001_complete.c:740-745"}, {"citation": "src/obj_seg001_complete.c:1692-1695"}]}
            ]
        elif tag == "VECTOR":
            field_semantics = [
                {"field": "x, y, z", "unit": "context-dependent signed 16-bit coordinate triplet; short world/map coordinates in car-relative calculations, other call sites use model or view space", "meaning": "coordinate frame follows the owning instance and transform path", "evidence": [{"citation": "src/obj_seg001_complete.c:740-745"}, {"citation": "src/obj_seg006.c:8"}]}
            ]
        elif tag == "POINT2D":
            field_semantics = [
                {"field": "px, py", "unit": "context-dependent signed integer pair; projected raster points are screen pixels, SIMD points are car/dashboard setup coordinates", "meaning": "do not assign one universal frame to all POINT2D instances", "evidence": [{"citation": "src/obj_seg006.c:260-280"}, {"citation": "src/obj_seg003.c:204-231"}]}
            ]
        elif tag == "RECTANGLE":
            field_semantics = [
                {"field": "left, right, top, bottom", "unit": "screen-space integer pixel edges for renderer clipping rectangles", "meaning": "right/bottom comparisons use ptx+1 and pty+1 in transformed-shape bounds", "evidence": [{"citation": "src/obj_seg006.c:408-415"}, {"citation": "src/obj_seg006.c:8"}]}
            ]
        elif tag == "MouseRegs":
            field_semantics = [
                {"field": "ax, bx, cx, dx", "unit": "DOS mouse interrupt register values; cx is horizontal coordinate (with mousehorscale mode adjustment), dx is vertical coordinate", "meaning": "BIOS/driver packet, not replay-frame simulation state", "evidence": [{"citation": "src/seg017_mouse_whole.c:1-18"}, {"citation": "src/seg017_mouse_whole.c:53-68"}]}
            ]
        elif tag == "SIMD":
            field_semantics = [
                {"field": "idle_rpm, downshift_rpm, upshift_rpm, max_rpm", "unit": "engine-rate thresholds compared with CARSTATE.car_currpm; RPM calibration is not independently established", "meaning": "initialize current RPM from idle_rpm and select shifts/clamp using the thresholds", "evidence": [{"citation": "src/obj_seg001_complete.c:1511"}, {"citation": "src/obj_seg001_complete.c:2173-2179"}, {"citation": "src/obj_seg001_complete.c:2228-2229"}]},
                {"field": "car_mass", "unit": "simulation mass divisor; physical mass unit and scale are not established", "meaning": "divides the scaled speedDelta term in the engine acceleration path", "evidence": [{"citation": "src/obj_seg001_complete.c:2266"}]},
                {"field": "gear_ratios[7]", "unit": "per-gear unsigned integer ratio; fixed-point denominator is not established", "meaning": "copied into CARSTATE and indexed by current gear", "evidence": [{"citation": "src/obj_seg001_complete.c:1519"}, {"citation": "src/obj_seg001_complete.c:2196-2197"}]},
                {"field": "grip, sliding[5]", "unit": "integer grip/sliding coefficients; denominator is not established", "meaning": "combine tire and surface response during simulation", "evidence": [{"citation": "src/obj_seg001_complete.c:2362-2367"}]},
                {"field": "collide_points[2]", "unit": "short car collision coordinates expanded by << 6 at world-coordinate use sites", "meaning": "car collision profile points used by car/car and car/track collision tests", "evidence": [{"citation": "src/obj_seg001_complete.c:1387"}, {"citation": "src/obj_seg001_complete.c:1429-1477"}]},
                {"field": "wheel_coords[4]", "unit": "short VECTOR car-local wheel coordinate triplets; exact asset scale is not established", "meaning": "wheel placement input for vehicle simulation/render setup", "evidence": [{"citation": "src/obj_seg001_complete.c:1028"}, {"citation": "src/obj_seg001_complete.c:1386"}]},
                {"field": "spdcenter, spdpoints; revcenter, revpoints", "unit": "integer dashboard raster coordinates / extents", "meaning": "position speedometer and tachometer lines/sprites", "evidence": [{"citation": "src/obj_seg005.c:1600-1617"}]},
                {"field": "knob_points[7]", "unit": "gear-selector/dashboard layout coordinates; pixel scale not established", "meaning": "per-gear knob position copied to CARSTATE and updated on shifts", "evidence": [{"citation": "src/obj_seg001_complete.c:1521-1523"}, {"citation": "src/obj_seg001_complete.c:2186-2216"}]},
                {"field": "aero_resistance", "unit": "integer quadratic drag coefficient; fixed-point scale is not established", "meaning": "builds the per-car aerodynamic resistance table with an i-squared term and right shift 9", "evidence": [{"citation": "src/obj_seg001_complete.c:3348-3355"}]}
            ]
        elif tag == "AUDIO_CAR_FRAME":
            field_semantics = [
                {"field": "player_offsets[6], opponent_offsets[6]", "unit": "short-vector coordinate deltas for audio spatialization; source coordinate frame follows the target/current VECTOR inputs", "meaning": "capture past/current target deltas and pass them to the audio service", "evidence": [{"citation": "src/obj_seg001_complete.c:2738-2743"}, {"citation": "src/obj_seg001_complete.c:2832-2840"}]},
                {"field": "player_rpm, opponent_rpm", "unit": "copied CARSTATE engine-rate values", "meaning": "per-frame audio pitch inputs; derived presentation state", "evidence": [{"citation": "src/obj_seg001_complete.c:2744"}, {"citation": "src/obj_seg001_complete.c:2753"}]}
            ]
        instance_subsystems = set()
        for instance in instances:
            value = instance.get("owner_subsystem")
            if not value:
                continue
            if value.startswith("shared/"):
                instance_subsystems.update(x for x in value[len("shared/"):].split("+") if x)
            else:
                instance_subsystems.add(value)
        instance_subsystems = sorted(instance_subsystems)
        type_owner = (instance_subsystems[0] if len(instance_subsystems) == 1 else
                      ("shared/" + "+".join(instance_subsystems) if instance_subsystems else
                       subsystem_for(tag, [x["function"] for x in list(aggregate_readers.values()) + list(aggregate_writers.values())], " ".join(r[0]["id"] for r in records))))
        type_rows.append({"tag": tag, "variants": variants_json, "instances": instances,
                          "owner_subsystem": type_owner,
                          "address": [{"name": x["name"], "address": x["address"], "address_hex": f"0x{x['address']:05X}",
                                       "size_bytes": x["size_bytes"], "element_count": x.get("element_count"),
                                       "evidence": x.get("evidence", [])} for x in instances],
                          "readers": sorted(aggregate_readers.values(), key=lambda x: (x["source"], x["line"])),
                          "writers": sorted(aggregate_writers.values(), key=lambda x: (x["source"], x["line"])),
                          "units": type_units, "field_semantics": field_semantics, "lifetime": type_lifetime,
                          "replay_determinism": type_replay,
                          "evidence": [{"kind": "type_definition", "citation": f"{r[1]['source']}:{r[1]['line']}"} for r in records]})

    # Global data declarations with exact semantic scales grounded in accepted source comments/code.
    scale_ledger = [
        {"fact": "sinfast selects one of four quadrants from the high byte, indexes the 256-entry quarter-wave table from the low byte, and cosfast adds 0x100; one complete turn is 0x400 units. Table peak 16384 is Q14 amplitude.",
         "evidence": ["asm/sincos.ASM:273-332", "asm/sincos.ASM:6-262", "asm/graphics_resource_runtime.ASM:3103-3121"]},
        {"fact": "CARSTATE.car_speed and car_speed2 are unsigned Q8 mph (raw value = mph * 256); car_speed is rev-coupled and car_speed2 is actual translational speed, so they diverge during jumps.",
         "evidence": ["src/obj_seg003.c:101-107", "src/obj_seg003.c:101-107"]},
        {"fact": "CARSTATE car_posWorld1/car_posWorld2 coordinates are 64 times short VECTOR/map coordinates at conversions that shift by six or shift short deltas left by six.",
         "evidence": ["src/obj_seg001_complete.c:740-745", "src/obj_seg001_complete.c:1692-1695", "src/obj_seg001_complete.c:2699-2711"]},
        {"fact": "Projection center and half extents are stored in integer screen coordinates (160,100); transformed-shape clip rectangles compare projected coordinates as pixel edges.",
         "evidence": ["asm/projection_vector_window.ASM:11-27", "src/obj_seg006.c:408-415"]},
        {"fact": "Mouse x coordinates are converted by mousehorscale, which is set when the selected width is 320; mouse y coordinates are passed directly. The mouse API limits are pixel-coordinate bounds.",
         "evidence": ["src/seg017_mouse_whole.c:40-43", "src/seg017_mouse_whole.c:49-68"]},
        {"fact": "MouseRegs is a DOS interrupt register packet; it is device/presentation input and not recorded simulation state.",
         "evidence": ["src/seg017_mouse_whole.c:1-18", "src/seg017_mouse_whole.c:62-68"]}
    ]
    subsystem_notes = [
        {"subsystem": "simulation", "summary": "GAMESTATE checkpoint state, embedded player/opponent CARSTATE, deterministic SIMD car setup, and random-generator state.",
         "evidence": ["src/obj_seg003.c:90-235", "src/obj_seg001_complete.c:1758-1770", "src/obj_seg001_complete.c:933-1060", "src/obj_seg001_complete.c:1758-1770"]},
        {"subsystem": "camera_render", "summary": "Camera viewpoint/mode values feed world-to-view transforms; projection globals and per-frame transformed-shape workspaces feed clipping and raster coordinates.",
         "evidence": ["src/obj_seg003.c:936-971", "asm/projection_vector_window.ASM:11-27", "src/obj_seg006.c:260-280", "src/obj_seg006.c:408-415"]},
        {"subsystem": "menu_ui", "summary": "Menu configuration, score/name text, dialog selection and cursor/rectangle state live outside the per-frame simulation snapshot; persistent race configuration must still be reloaded identically.",
         "evidence": ["src/obj_seg000.c:550-555", "src/obj_seg000.c:611-612", "src/obj_seg008.c:1766-1795"]},
        {"subsystem": "input", "summary": "Keyboard/joystick/mouse polling and status stacks are device/UI state; replay consumes the frame-indexed byte stream through core.game_frame.",
         "evidence": ["src/obj_seg008.c:770-817", "src/seg017_mouse_whole.c:40-68", "src/obj_seg001_complete.c:1758-1770"]},
        {"subsystem": "audio", "summary": "Audio driver, chunks, voices, timers, and derived car-frame audio records are presentation/resource state; simulation car position and RPM are sampled to drive sound.",
         "evidence": ["src/obj_seg027.c:16-80", "src/obj_seg007.c:7-49", "src/obj_seg001_complete.c:2661-2809"]}
    ]
    supplemental_object_extents = []
    for row in extent_evidence.get("objects", []):
        source_view = row.get("source_view", "")
        if not source_view.startswith("game_camera_buttons_"):
            continue
        name = source_view.split("[", 1)[0]
        supplemental_object_extents.append({
            "name": name,
            "address": row["address"],
            "address_hex": f"0x{row['address']:05X}",
            "type": "int16_t[9]",
            "size_bytes": row["allocated_extent_bytes"],
            "size_basis": row["allocation_basis"],
            "owner_object": "obj_seg005",
            "replay_determinism": "presentation/menu state",
            "evidence": row["evidence"],
            "target_payload_hex": row.get("target_payload_hex"),
            "target_values": row.get("target_values")
        })
    model = {
        "schema": "stunts-global-state-model-v1",
        "generated_by": "tools/porting/state_model.py",
        "authority": {"names_registry": "layout/names-registry.json; reconstruction binding names, not original recovered names",
                      "accepted_sources": [x["source"] for x in active],
                      "communal_extents": "layout/manifest.json:bss_owners[id=c_common].communals",
                      "address_authority": "MCGA load-image address evidence, keyed to layout/data-symbols.json"},
        "access_analysis": {"scope": "Function-level direct C/ASM name/member accesses plus explicit closure for audio_frmarr cast to AUDIO_CAR_FRAME*.",
                            "pointer_alias_limit": "Arbitrary writes through escaped pointers are not attributed transitively; address_escapes lists such sites separately and must be reviewed before treating reader/writer sets as exhaustive."},
        "coverage": {"registry_data_symbols": len(data_rows), "complete_data_symbol_type_and_size": sum(r["coverage"] == "complete" for r in data_rows),
                     "unresolved_data_symbols": sum(r["coverage"] != "complete" for r in data_rows),
                     "symbols_with_unknown_unit": sum(r["units"]["confidence"] == "unknown" for r in data_rows),
                     "symbols_with_unknown_replay_class": sum(r["replay_determinism"]["preserve_bit_exactly"] is None for r in data_rows),
                     "symbols_marked_bit_exact_replay_state": sum(r["replay_determinism"]["preserve_bit_exactly"] is True for r in data_rows),
                     "symbols_marked_presentation_or_process_local": sum(r["replay_determinism"]["preserve_bit_exactly"] is False for r in data_rows),
                     "struct_tags": len(type_rows), "struct_variants": sum(len(r["variants"]) for r in type_rows),
                     "struct_variants_with_compiler_size": sum(v["size_bytes"] is not None for r in type_rows for v in r["variants"]),
                     "struct_types_with_global_or_embedded_addresses": sum(bool(r["instances"]) for r in type_rows),
                     "struct_types_with_instance_dependent_replay_class": sum(r["replay_determinism"]["preserve_bit_exactly"] is None for r in type_rows),
                     "struct_types_without_registered_instances": sum(not r["instances"] for r in type_rows),
                     "active_source_files": len(sources),
                     "supplemental_target_object_extents": len(supplemental_object_extents)},
        "state_policy": {"replay_determinism": "Treat the accepted GAMESTATE/CARSTATE image, random seed, ordered replay input bytes, and simulation-affecting track state as authoritative. Presentation state may be regenerated unless exact playback appearance is an explicit product requirement. Unknown rows remain unknown.",
                         "reconstructed_names": "All registry spellings are binding aliases. Do not treat them as recovered historical identifiers.",
                         "process_pointers": "Near/far/huge pointer slots are process-local addresses and are not copied bit-exactly into a portable replay; preserve deterministic pointee contents separately where the pointee is replay input or a GAMESTATE checkpoint.",
                         "unknown_units": "A raw integer/byte with no proven fixed-point scale or coordinate frame is reported as unknown, not guessed.",
                         "object_extents": "Lookup gaps and maximum observed accesses are diagnostics only. Supplemental object extents require accepted declarations paired with exact locked contributions or an exact LINK communal declaration.",
                         "scale_ledger": scale_ledger, "subsystem_notes": subsystem_notes},
        "data_symbols": data_rows,
        "supplemental_target_object_extents": supplemental_object_extents,
        "object_extent_evidence": extent_evidence,
        "struct_types": type_rows
    }
    return model


def md_cell(value):
    if value is None: return "?"
    if isinstance(value, (list, tuple)): return "; ".join(str(x) for x in value) if value else "—"
    return str(value).replace("|", "\\|").replace("\n", " ")


def validate_model(model):
    """Reject inconsistent inventory counts before publishing build output."""
    if not isinstance(model, dict) or model.get("schema") != "stunts-global-state-model-v1":
        raise ValueError("unsupported global-state model schema")
    coverage = model.get("coverage", {})
    data_rows = model.get("data_symbols", [])
    supplemental = model.get("supplemental_target_object_extents", [])
    type_rows = model.get("struct_types", [])
    if not isinstance(data_rows, list) or not isinstance(type_rows, list) or not isinstance(supplemental, list):
        raise ValueError("state model inventories must be arrays")
    addresses = [row.get("address") for row in data_rows]
    names = [row.get("name") for row in data_rows]
    if len(addresses) != len(set(addresses)) or len(names) != len(set(names)):
        raise ValueError("state model has duplicate registered data symbols")
    variants = [variant for row in type_rows for variant in row.get("variants", [])]
    measured = sum(variant.get("size_bytes") is not None for variant in variants)
    expected = {
        "registry_data_symbols": len(data_rows),
        "complete_data_symbol_type_and_size": sum(row.get("coverage") == "complete" for row in data_rows),
        "unresolved_data_symbols": sum(row.get("coverage") != "complete" for row in data_rows),
        "symbols_with_unknown_unit": sum(row.get("units", {}).get("confidence") == "unknown" for row in data_rows),
        "symbols_with_unknown_replay_class": sum(row.get("replay_determinism", {}).get("preserve_bit_exactly") is None for row in data_rows),
        "symbols_marked_bit_exact_replay_state": sum(row.get("replay_determinism", {}).get("preserve_bit_exactly") is True for row in data_rows),
        "symbols_marked_presentation_or_process_local": sum(row.get("replay_determinism", {}).get("preserve_bit_exactly") is False for row in data_rows),
        "struct_tags": len(type_rows),
        "struct_variants": len(variants),
        "struct_variants_with_compiler_size": measured,
        "struct_types_with_global_or_embedded_addresses": sum(bool(row.get("instances")) for row in type_rows),
        "struct_types_with_instance_dependent_replay_class": sum(row.get("replay_determinism", {}).get("preserve_bit_exactly") is None for row in type_rows),
        "struct_types_without_registered_instances": sum(not row.get("instances") for row in type_rows),
        "supplemental_target_object_extents": len(supplemental),
    }
    for key, value in expected.items():
        if coverage.get(key) != value:
            raise ValueError(f"state model coverage mismatch for {key}: {coverage.get(key)!r} != {value!r}")
    if any(not row.get("name") or not row.get("evidence") or row.get("size_bytes") is None for row in supplemental):
        raise ValueError("supplemental object extents require a name, size, and evidence")
    return model


def write_markdown(model, output_dir=DOCS):
    out = ["# Global state dictionary", "",
           "This generated dictionary enumerates every `kind=data` address in `layout/names-registry.json` and every tagged `struct`/`union` definition in the accepted C contributions. Registry identifiers are link/address-constrained reconstruction aliases, not recovered historical names. The accepted declarations, the loaded-image address, and compiler probes are kept separate as evidence.", "",
           f"Coverage: **{model['coverage']['registry_data_symbols']} data symbols** ({model['coverage']['complete_data_symbol_type_and_size']} with grounded type and byte size; {model['coverage']['unresolved_data_symbols']} with a missing type or size), **{model['coverage']['struct_tags']} struct tags** across {model['coverage']['struct_variants']} accepted-TU layouts ({model['coverage']['struct_variants_with_compiler_size']} compiler-sized). Units remain unclassified for {model['coverage']['symbols_with_unknown_unit']} symbols; replay class remains unknown for {model['coverage']['symbols_with_unknown_replay_class']} symbols. {model['coverage']['struct_types_with_instance_dependent_replay_class']} aggregate tags combine instances with different replay classes; see their per-instance rows. See [`state-model.json`](state-model.json) for complete evidence records.", "",
           "## Deterministic replay boundary", "",
           "The simulation checkpoint is a complete 0x460-byte `GAMESTATE`, including both 0xD0-byte `CARSTATE`s, the six-byte random seed and opaque fields. `update_gamestate` writes the full aggregate into the replay checkpoint array; `restore_gamestate` assigns a full checkpoint back and initializes the random generator from its seed. Replay advances by `core.game_frame` through `g_tdreplay16buf`. Preserve the aggregate bytes, seed, replay bytes and frame order exactly. The pointer slots that address these buffers are process-local and are not portable replay data. Keyboard, joystick, mouse, raster, menu and audio-driver state stay in the presentation/device side of the boundary.", "",
           "| Evidence-backed state fact | Porting consequence | Evidence |", "|---|---|---|"]
    keyfacts = [
        ("`GAMESTATE` has 0x460 bytes: three 24-element long arrays, timing/frame fields, two embedded `CARSTATE`s, seed bytes, replay flags and opaque spans.", "Keep the whole aggregate in checkpoints, including currently unexplained fields.", "src/obj_seg003.c:155-202; src/obj_seg001_complete.c:1758-1770"),
        ("Each embedded `CARSTATE` has 0xD0 bytes with two long world-position vectors, wheel-result arrays, RPM/speed/gearing and flags.", "Keep player and opponent state as separate complete records.", "src/obj_seg003.c:90-153; compiler-sized `struct CARSTATE` probe in `struct_types`"),
        ("`car_speed`/`car_speed2` are Q8 mph; `speed2` is actual movement speed and can diverge during jumps.", "Keep the unsigned 16-bit raw values and convert only at API boundaries.", "src/obj_seg003.c:101-107"),
        ("Fast trig uses 0x400 input units per turn, 0x100 per quadrant and a peak table value of 16384 (Q14).", "Keep angle and multiply-scale conventions at the legacy boundary.", "asm/sincos.ASM:273-332; asm/sincos.ASM:6-262; asm/graphics_resource_runtime.ASM:3103-3121"),
        ("Car world-position longs are converted to short vectors with shifts by six.", "Apply the proven x64 conversion when entering short map/vector coordinates.", "src/obj_seg001_complete.c:740-745; src/obj_seg001_complete.c:1692-1695"),
        ("`SIMD` stores per-car setup values including ratios, RPM thresholds, grip, wheel locations and dashboard-point data.", "Treat loaded tuning as deterministic race configuration; do not conflate with mutable `CARSTATE`.", "src/obj_seg003.c:204-235; src/obj_seg005.c:200-231"),
    ]
    for fact, effect, ev in keyfacts:
        out.append(f"| {fact} | {effect} | `{ev}` |")
    supplemental = model.get("supplemental_target_object_extents", [])
    if supplemental:
        out += ["", "## Supplemental target object extents", "",
                "These accepted source objects have locked contribution boundaries but no names-registry entries. They are supplemental extent facts, not recovered original identifiers.",
                "", "| Name | Address | Type | Bytes | Owner | Evidence |", "|---|---:|---|---:|---|---|"]
        for row in supplemental:
            cites = row.get("evidence", [])
            out.append("| " + " | ".join(map(md_cell, [row["name"], row["address_hex"], row["type"],
                row["size_bytes"], row["owner_object"], cites])) + " |")
    out += ["", "## State by subsystem", "",
            "These summaries group state by its observed owner and use flow. The complete address-by-address inventory follows; shared rows keep a `shared/...` subsystem label.", "",
            "| Subsystem | State boundary | Evidence |", "|---|---|---|"]
    for note in model["state_policy"]["subsystem_notes"]:
        out.append("| " + " | ".join(map(md_cell, [note["subsystem"], note["summary"], note["evidence"]])) + " |")
    out += ["", "## Global/static/communal symbol inventory", "",
            "`Address` is the loaded-image byte address from the registry; `size` is the C/ASM declaration extent or accepted LINK communal/runtime extent. Reader/writer function names come from accepted C/ASM body analysis; each cited access line is retained in JSON. Arbitrary writes through escaped pointers are not guessed: those sites stay in `address_escapes`. C source aliases are cited. A `?` means the physical unit or replay role remains unknown. `span` to the next symbol is diagnostic only and never used as object size.", "",
            "| Name | Address | Storage | Type | Size | Owner subsystem / owner object | Units | Lifetime | Replay class / exact? | Readers | Writers | Evidence |",
            "|---|---:|---|---|---:|---|---|---|---|---|---|---|"]
    for r in model["data_symbols"]:
        ev = []
        for e in r["evidence"][:3]:
            ev.append(e.get("citation", "?"))
        readers = [x["function"] for x in r["readers"]]
        writers = [x["function"] for x in r["writers"]]
        out.append("| " + " | ".join(map(md_cell, [r["name"], r["address_hex"], r["storage"], r["type"], r["size_bytes"],
            f"{r['owner_subsystem']} / {','.join(r['owner_objects'])}", r["units"]["value"], r["lifetime"]["value"],
            f"{r['replay_determinism']['class']} / {r['replay_determinism']['preserve_bit_exactly']}",
            readers, writers, ev])) + " |")
    out += ["", "## Accepted C struct/union types", "",
            "Each row is an accepted translation-unit layout. Repeated tags remain separate because declarations differ between TUs; the 1014-byte `GAMESTATE` view in `src/obj_seg004.c:2` stays distinct from the 1120-byte live checkpoint. The JSON stores every member declaration and evidence line; `size_bytes` is measured by an isolated canonical MSC compile of `sizeof(tag)` bracket arrays, where the probe completed successfully.", "",
            "| Tag | Accepted TU variant | Size | Instances at addresses | Owner subsystem | Readers | Writers | Units | Lifetime | Replay class / exact? | Evidence |", "|---|---|---:|---|---|---|---|---|---|---|---|"]
    for t in model["struct_types"]:
        for v in t["variants"]:
            inst = [f"{x['name']}@0x{x['address']:05X}" for x in t["instances"]]
            cite = f"{v['source']}:{v['line']}"
            if v["evidence"] and len(v["evidence"]) > 1:
                cite += "; compiler size probe " + (v["evidence"][1].get("citation") or "unknown report")
            out.append("| " + " | ".join(map(md_cell, [t["tag"], v["translation_unit"], v["size_bytes"], inst,
                t["owner_subsystem"], [x["function"] for x in t["readers"]], [x["function"] for x in t["writers"]],
                t["units"], t["lifetime"], f"{t['replay_determinism']['class']} / {t['replay_determinism']['preserve_bit_exactly']}", cite])) + " |")
    out += ["", "### Evidence-backed field units and coordinate frames", "",
            "Only fields with a source-grounded unit or transform interpretation are listed here; all declared member names/types remain in JSON, and unnamed fields stay opaque.", "",
            "| Tag | Field(s) | Unit / meaning | Evidence |", "|---|---|---|---|"]
    for t in model["struct_types"]:
        for field in t["field_semantics"]:
            refs = [e.get("citation", "?") for e in field.get("evidence", [])]
            out.append("| " + " | ".join(map(md_cell, [t["tag"], field["field"],
                field["unit"] + "; " + field["meaning"], refs])) + " |")
    out += ["", "## Coordinate, scale and lifetime rules", "",
            "- The scale ledger in JSON records only evidence-backed shared rules. Units absent from code comments/conversions are left as raw integer/byte or unknown.",
            "- `GAMESTATE` and `CARSTATE` rows are race simulation state and replay-checkpoint state; fields named only by offsets are retained as opaque bytes/words.",
            "- Replay payload bytes must retain their exact order. Physical keyboard, mouse and joystick status is sampled into normalized frame input; presentation polling must not overwrite recorded input during playback.",
            "- Camera, geometry, sprite, viewport, palette, menu, font and audio state is tagged as presentation state when the accepted call/access flow supports it. Raw data stays preserved in legacy resources, but it is not treated as authoritative simulation snapshot state.",
            "- Rows with `coverage=incomplete` and `preserve_bit_exactly=null` identify concrete evidence gaps in `state-model.json`; they are not silently assigned C types, units or replay semantics.", ""]
    output_dir.mkdir(parents=True, exist_ok=True)
    (output_dir / "state-model.md").write_text("\n".join(out), encoding="utf-8")


def emit_model(model, output_dir=DOCS):
    """Validate and emit both complete reports below build/."""
    validate_model(model)
    output_dir = Path(output_dir).resolve()
    build_root = (ROOT / "build").resolve()
    try:
        output_dir.relative_to(build_root)
    except ValueError as error:
        raise ValueError("state-model outputs must stay under build/") from error
    output_dir.mkdir(parents=True, exist_ok=True)
    write_markdown(model, output_dir)
    write_json(output_dir / "state-model.json", model)
    return output_dir / "state-model.md", output_dir / "state-model.json"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--no-probes", action="store_true",
                    help="reuse complete type-layout and member-offset reports under build/porting")
    args = ap.parse_args()
    model = build_model(run_probes=not args.no_probes)
    markdown_path, json_path = emit_model(model)
    print(json.dumps(model["coverage"], indent=2))
    print(f"wrote {markdown_path.relative_to(ROOT)}")
    print(f"wrote {json_path.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
