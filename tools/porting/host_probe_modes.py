#!/usr/bin/env python3
"""Compile the recovered C TUs through the separate port-only declaration views."""
from __future__ import annotations

import collections
import json
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PORT = ROOT / "tools/porting/port_include"
HOST = ROOT / "tools/porting/host"
sys.path.insert(0, str(ROOT / "tools/porting"))
import host_probe_declarations as declarations  # noqa: E402
from host_probe_declarations import (  # noqa: E402
    TYPE_PREFIX, ALIASES, cdecls, source_parser_text, mask_comments,
    top_level_spans, function_headers, render_type, collect_sources,
    HOST_OVERRIDES, OVERRIDES, call_arities, parse_type_declaration, readj,
)

GCC = Path(r"C:\msys64\mingw64\bin\gcc.exe")
WORK = ROOT / "build/porting/host-probe/compat"
FLAGS = ["-std=gnu11", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wsign-conversion", "-Wpointer-arith", "-Wcast-align", "-Wcast-qual",
         "-Wstrict-prototypes", "-Wmissing-prototypes"]
TIMEOUT = 90
TOTAL_LIMIT = 900
STRICT_CENTRAL = False



def statement_spans(text: str):
    code = mask_comments(text)
    braces = parens = brackets = 0
    start = 0
    spans = []
    i = 0
    while i < len(code):
        c = code[i]
        if c == "#" and (i == 0 or code.rfind("\n", 0, i) >= start):
            nl = code.find("\n", i)
            i = len(code) if nl < 0 else nl + 1
            continue
        if c == "(": parens += 1
        elif c == ")": parens = max(0, parens - 1)
        elif c == "[": brackets += 1
        elif c == "]": brackets = max(0, brackets - 1)
        elif c == "{" and parens == 0 and brackets == 0:
            braces += 1
            start = i + 1
        elif c == "}" and parens == 0 and brackets == 0:
            braces = max(0, braces - 1)
            start = i + 1
        elif c == ";" and parens == 0 and brackets == 0:
            if code[start:i + 1].strip():
                spans.append((start, i + 1))
            start = i + 1
        i += 1
    return spans


def local_names_and_types(source: str, text: str):
    unit = cdecls.Unit(source_parser_text(text), source)
    functions = {}
    data = {}
    for name, rows in unit.decls.items():
        for d in rows:
            if d.get("storage") == "static":
                continue
            if d["type"]["k"] == "fn":
                if d.get("defined"):
                    functions[name] = d["type"]
                else:
                    functions.setdefault(name, d["type"])
            else:
                if d.get("defined"):
                    data[name] = d["type"]
                else:
                    data.setdefault(name, d["type"])
    # The source definition signature is the most reliable host declaration for calls
    # inside its own TU. Do not depend on cdecls recognizing every old dialect variant.
    fn_protos = dict(function_headers(text))
    # Original per-TU prototypes are useful compatibility views where the shared
    # registry type loses a source-local aggregate alias or old declaration shape.
    for name, typ in functions.items():
        fn_protos.setdefault(name, "extern " + render_type(typ, name, target=False) + ";")
    for name in set(fn_protos) & set(HOST_OVERRIDES):
        fn_protos[name] = HOST_OVERRIDES[name]
    for name in OVERRIDES:
        fn_protos.pop(name, None)
    observed = call_arities(text, set(fn_protos))
    defined_functions = {name for name, rows in unit.decls.items()
                         if any(d["type"]["k"] == "fn" and d.get("defined") for d in rows)}
    for name, proto in list(fn_protos.items()):
        if name in HOST_OVERRIDES or name in defined_functions:
            continue
        typ = parse_type_declaration(proto, name)
        if not typ or typ.get("k") != "fn" or typ.get("params") is None:
            continue
        expected = len(typ.get("params") or [])
        arities = observed.get(name, set())
        if name == "call_read_line" or any(n != expected for n in arities):
            linkage = "static " if re.search(r"\bstatic\b", proto) else "extern "
            fn_protos[name] = linkage + render_type(typ["ret"], name + "()", target=False) + ";"
    static_data = set()
    for name, rows in unit.decls.items():
        if any(d.get("storage") == "static" and d.get("defined") and d["type"]["k"] != "fn"
               for d in rows):
            static_data.add(name)
    for name in static_data:
        data.pop(name, None)
    return unit, fn_protos, data, static_data


def source_tag_guards(text: str):
    code = mask_comments(text)
    spans = top_level_spans(text)
    function_ranges = [(a, b) for a, b, kind in spans if kind == "function"]
    out = set()
    for m in re.finditer(r"\b(?:struct|union)\s+([A-Za-z_]\w*)\s*\{", code):
        if any(a <= m.start() < b for a, b in function_ranges):
            continue
        out.add(m.group(1))
    return out


def declaration_names(snippet: str, source: str):
    unit = cdecls.Unit(source_parser_text(snippet), source)
    names = []
    for name, rows in unit.decls.items():
        for d in rows:
            if not d.get("defined") and d.get("storage") != "static":
                names.append((name, d["type"]))
    return names


def local_config(source: str, text: str, owner: str):
    unit, fn_protos, data_types, static_data = local_names_and_types(source, text)
    if STRICT_CENTRAL:
        static_fns = {name: proto for name, proto in fn_protos.items()
                      if re.search(r"\bstatic\b", proto)}
        fn_protos = static_fns
        data_types = {}
    else:
        declaration_evidence = readj(PORT / "declaration-evidence.json")
        function_specs = declaration_evidence.get("function_prototypes", {})
        arity_debt = declaration_evidence.get("function_arity_debt", {})
        observed = call_arities(text, set(fn_protos) | set(function_specs))
        defined_functions = {name for name, rows in unit.decls.items()
                             if any(d["type"]["k"] == "fn" and d.get("defined") for d in rows)}
        for name, debt in arity_debt.items():
            arities = observed.get(name, set())
            if not arities or name in defined_functions:
                continue
            if any(n != debt["prototype_arity"] for n in arities):
                spec = function_specs.get(name, {})
                typ = parse_type_declaration(spec.get("declaration", ""), name)
                if typ and typ.get("k") == "fn":
                    fn_protos[name] = "extern " + render_type(typ["ret"], name + "()", target=False) + ";"
        if source == "src/obj_seg000.c" and "locate_shape_fatal" in fn_protos:
            fn_protos["locate_shape_fatal"] = "extern struct SHAPE2D *locate_shape_fatal();"
    lines = ["/* Generated per-TU PORT_BUILD declaration replacement. */",
             "#ifndef PORT_BUILD", "#define PORT_BUILD 1", "#endif"]
    if STRICT_CENTRAL:
        lines.append("#define STUNTS_PROBE_STRICT_CENTRAL 1")
    lines.append(f"#define STUNTS_TU_{Path(source).stem} 1")
    for name in sorted(fn_protos):
        lines.append(f"#define STUNTS_LOCAL_FN_{name} 1")
    local_data = static_data if STRICT_CENTRAL else set(data_types) | static_data
    for name in sorted(local_data):
        lines.append(f"#define STUNTS_LOCAL_DATA_{name} 1")
    lines += ['#include "stunts_decls.h"', "", "/* Local definitions expose their own host spelling for calls made before the body. */"]
    lines += [p for _, p in sorted(fn_protos.items())]
    lines += ["", "/* Same-TU globals are declared from their definitions before source use. */"]
    if STRICT_CENTRAL:
        lines.append("/* Public externs and non-static definitions come only from stunts_decls.h. */")
    else:
        for name, typ in sorted(data_types.items()):
            decl = render_type(typ, name, target=False)
            masked_source = mask_comments(text)
            if re.search(rf"\b(?:I8S|signed\s+char)\s*(?:\*\s*)*{re.escape(name)}\b", masked_source):
                decl = re.sub(r"\bchar\b", "signed char", decl, count=1)
            lines.append(f"extern {decl};")
    cfg = WORK / "config" / (Path(source).stem + ".h")
    cfg.parent.mkdir(parents=True, exist_ok=True)
    cfg.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return cfg, fn_protos, data_types



def aggregate_spans(text: str, shared_tags: set[str]):
    code = mask_comments(text)
    function_ranges = [(a, b) for a, b, kind in top_level_spans(text) if kind == "function"]
    spans = []
    for match in re.finditer(r"\b(?:typedef\s+)?(?:struct|union)\s+([A-Za-z_]\w*)\s*\{", code):
        tag = match.group(1)
        if tag not in shared_tags or any(a <= match.start() < b for a, b in function_ranges):
            continue
        brace = code.find("{", match.start())
        depth = 1
        pos = brace + 1
        while pos < len(code) and depth:
            if code[pos] == "{": depth += 1
            elif code[pos] == "}": depth -= 1
            pos += 1
        if depth:
            continue
        end = pos + (1 if pos < len(code) and code[pos] == ";" else 0)
        spans.append((match.start(), end))
    # Move the shared register typedef into the header once so the source definition cannot clash.
    for match in re.finditer(r"\btypedef\s+(?:struct|union)\s+MouseRegs\s*\{", code):
        brace = code.find("{", match.start())
        depth, pos = 1, brace + 1
        while pos < len(code) and depth:
            if code[pos] == "{": depth += 1
            elif code[pos] == "}": depth -= 1
            pos += 1
        semi = code.find(";", pos)
        if depth == 0 and semi >= 0:
            spans.append((match.start(), semi + 1))
    return sorted(set(spans))



def legacy_target_widths(text: str):
    """Use MSC target widths for old bare int/long spellings in the host-only overlay."""
    out=[]; i=0; n=len(text); line_start=True
    nl=chr(10); cr=chr(13); tab=chr(9); slash=chr(92)
    while i<n:
        if line_start:
            j=i
            while j<n and text[j] in (" ",tab,cr): j+=1
            if j<n and text[j]=="#":
                end=text.find(nl,j)
                if end<0: out.append(text[i:]); break
                out.append(text[i:end+1]); i=end+1; line_start=True; continue
        c=text[i]
        if c==nl: out.append(c); i+=1; line_start=True; continue
        if c in (" ",tab,cr): out.append(c); i+=1; continue
        line_start=False
        if text.startswith("/*",i):
            end=text.find("*/",i+2); end=n if end<0 else end+2
            chunk=text[i:end]; out.append(chunk); line_start=chunk.endswith(nl); i=end; continue
        if text.startswith("//",i):
            end=text.find(nl,i+2); end=n if end<0 else end
            out.append(text[i:end]); i=end; continue
        if c==chr(34) or c=="'":
            q=c; j=i+1
            while j<n:
                if text[j]==slash: j+=2; continue
                if text[j]==q: j+=1; break
                j+=1
            out.append(text[i:j]); i=j; continue
        if c.isalpha() or c=="_":
            j=i+1
            while j<n and (text[j].isalnum() or text[j]=="_"): j+=1
            word=text[i:j]
            if word in ("unsigned","signed"):
                k=j
                while k<n and text[k].isspace(): k+=1
                if text.startswith("int",k) and (k+3==n or not (text[k+3].isalnum() or text[k+3]=="_")):
                    out.append("U16" if word=="unsigned" else "I16"); i=k+3; continue
                if text.startswith("long",k) and (k+4==n or not (text[k+4].isalnum() or text[k+4]=="_")):
                    out.append("U32" if word=="unsigned" else "I32"); i=k+4; continue
                if word=="unsigned" and not (text.startswith("char",k) or text.startswith("short",k)):
                    out.append("U16"); i=j; continue
                if word=="signed" and not (text.startswith("char",k) or text.startswith("short",k)):
                    out.append("I16"); i=j; continue
            if word=="int": word="I16"
            elif word=="long": word="I32"
            out.append(word); i=j; continue
        out.append(c); i+=1
    return "".join(out)

def transformed_source(source: str, text: str, local_fns, local_data, shared_tags):
    code = mask_comments(text)
    removals = []
    # File-scope extern declarations and prototypes are the declaration blocks replaced by PORT_BUILD.
    for a, b, kind in top_level_spans(text):
        if kind != "stmt":
            continue
        snippet = text[a:b]
        masked = code[a:b].lstrip()
        if not masked or "{" in masked or "=" in masked:
            continue
        rows = declaration_names(snippet, source)
        if not rows:
            continue
        starts_extern = re.match(r"extern\b", masked) is not None
        prototypes = all(t["k"] == "fn" for _, t in rows)
        if starts_extern or prototypes:
            # Every listed name exists in stunts_decls.h. Same-TU definitions are re-declared from
            # their definition signature by local_config().
            removals.append((a, b))

    # Block-scope extern declarations also need to agree with the central API.
    for a, b in statement_spans(text):
        if any(x <= a and b <= y for x, y in removals):
            continue
        masked = code[a:b].lstrip()
        if not re.match(r"extern\b", masked):
            continue
        rows = declaration_names(text[a:b], source)
        if rows:
            removals.append((a, b))

    # The shared header supplies a source-selected complete version of these aggregate tags.
    # Hide only their top-level definitions in this PORT_BUILD overlay.
    removals.extend(aggregate_spans(text, shared_tags))

    out = text
    for a, b in sorted(set(removals), reverse=True):
        out = out[:a] + "\n#ifndef PORT_BUILD\n" + out[a:b] + "\n#endif /* PORT_BUILD */\n" + out[b:]
    if source == "src/obj_seg007.c":
        out = replace_seg007_asm(out, STRICT_CENTRAL)
        out = re.sub(r"typedef\s+unsigned\s+int\s+u16\s*;", "typedef uint16_t u16;", out)
        out = re.sub(r"typedef\s+unsigned\s+long\s+u32\s*;", "typedef uint32_t u32;", out)
        out = re.sub(r"typedef\s+unsigned\s+char\s+u8\s*;", "typedef uint8_t u8;", out)
        out = '#include "seg007_arith.h"\n' + out
    if source == "src/obj_seg003.c":
        out = out.replace("((I16)statemgmtcpy) = ((I16)slow_video_mode_state);",
                          "statemgmtcpy = (I16)slow_video_mode_state; /* PORT_BUILD: replace target lvalue cast. */")
    if source == "src/obj_seg008.c":
        out = re.sub(r"\bU32\s+(?:far\s+)?timer_get_delta_alt\s*\(",
                     "I16 far timer_get_delta_alt(", out)
        # Keep the six-word definition intact; strict mode redirects only its
        # legacy five-argument callers to the API view that splits the final I32.
        mode = "rename:stunts_port_call_read_line_view" if STRICT_CENTRAL else "unprototyped"
        out = rewrite_calls(out, "call_read_line", mode)
    # C does not permit an integer cast as an assignment lvalue. These legacy
    # spellings cast the value-width of globals whose declarations already carry it.
    out = re.sub(r"\(\(\s*((?:I8S|U8|I16S|U16S|I16|U16|I32|U32))\s*\)\s*([A-Za-z_]\w*)\s*\)\s*=(?!=)\s*([^;]+);",
                 lambda m: f"{m.group(2)} = ({m.group(1)})({m.group(3)}); /* PORT_BUILD lvalue cast */", out)
    if source == "src/obj_seg031.c":
        out = re.sub(r"\bvoid\s+(?:far\s+)?nullsub_2\s*\(\s*void\s*\)",
                     "void nullsub_2(void *resource, int16_t selector)", out, count=1)
    if source == "src/obj_seg000.c":
        out = re.sub(r"\bI16\s+main\s*\(\s*I16\s+argc\s*,\s*I8\s*\*\s*argv\[\s*\]\s*\)",
                     "int main(int argc, I8 *argv[])", out, count=1)
        if STRICT_CENTRAL:
            # These source callers keep typed port views while central declarations
            # describe the proven target stack-word contracts.
            out = rewrite_calls(out, "read_file_with_retry", "rename:stunts_port_read_file_with_retry_view")
            out = rewrite_calls(out, "call_read_line", "rename:stunts_port_call_read_line_view")
            out = rewrite_calls(out, "locate_shape_fatal", "rename:stunts_port_locate_shape_fatal_shape_view")
    if source == "src/toupper.c":
        out = re.sub(r"\bI16\s+toupper\s*\(\s*I16\s+ch\s*\)",
                     "int toupper(int ch)", out, count=1)
    if STRICT_CENTRAL:
        if source == "src/obj_seg032_group.c":
            out = rewrite_calls(out, "timer_copy_counter", "rename:stunts_port_timer_copy_counter_split_view")
        if source == "src/obj_seg028.c":
            out = rewrite_calls(out, "audio_init_chunk", "rename:stunts_port_audio_init_chunk_7_view")
        if source == "src/obj_seg007.c":
            out = rewrite_calls(out, "send_audio_stop_event", "rename:stunts_port_send_audio_stop_event_value_view")
        if source == "src/obj_seg003.c":
            out = re.sub(r"\bresbuftext\s*=\s*0\s*;", "resbuftext[0] = 0;", out, count=1)
    out = rewrite_calls(out, "nullsub_2", "pad2")
    if STRICT_CENTRAL:
        out = legacy_target_widths(out)
    if source == "src/obj_seg000.c":
        out = re.sub(r"\bI16\s+main\s*\(\s*I16\s+argc\s*,\s*I8\s*\*\s*argv\[\s*\]\s*\)",
                     "int main(int argc, I8 *argv[])", out, count=1)
    if source == "src/toupper.c":
        out = re.sub(r"\bI16\s+toupper\s*\(\s*I16\s+ch\s*\)",
                     "int toupper(int ch)", out, count=1)
    return out, len(set(removals))


def replace_seg007_asm(text: str, strict_central: bool = False):
    blocks = re.findall(r"\b_asm\s*\{[^{}]*\}", text, flags=re.S)
    if len(blocks) != 2:
        return text
    ax_divide = "stunts_div_u32_u16(100u, (uint16_t)speed)" if not strict_central else "(uint16_t)(100u / (uint16_t)speed)"
    replacements = [
        """/* PORT_BUILD: 8086 DIV, signed IMUL, MUL, DIV, NEG and ADD; preserve AX/DX 16-bit wrap. */
        {
            uint16_t x2_ax = {ax_divide};
            int32_t x2_signed_product = (int32_t)(int16_t)x2_ax * (int32_t)(int16_t)approach;
            approach = (int16_t)(uint16_t)x2_signed_product;
            uint32_t x2_unsigned_product = 127u * (uint16_t)curdist;
            uint16_t x2_quotient = stunts_div_u32_u16(x2_unsigned_product, 6000u);
            volume = (uint16_t)(0u - (uint32_t)x2_quotient + 127u);
        }""",
        """/* PORT_BUILD: 8086 MUL forms DX:AX; DIV returns the low 16-bit quotient. */
        {
            uint32_t x2_product = 6000u * (uint16_t)freq;
            newrate = stunts_div_u32_u16(x2_product, (uint16_t)newrate);
        }""",
    ]
    replacements[0] = replacements[0].replace("{ax_divide}", ax_divide)
    for old, new in zip(blocks, replacements):
        text = text.replace(old, new, 1)
    return text


def rewrite_calls(text: str, name: str, mode: str):
    """Apply a host-only call-site adapter without changing argument evaluation."""
    code = mask_comments(text)
    definitions = []
    for a, b, kind in top_level_spans(text):
        if kind == "function":
            body = code.find("{", a, b)
            if body >= 0:
                definitions.append((a, body))
    edits = []
    for match in re.finditer(rf"\b{re.escape(name)}\s*\(", code):
        open_at = code.find("(", match.start())
        depth = 1
        paren = bracket = brace = commas = 0
        i = open_at + 1
        while i < len(code) and depth:
            c = code[i]
            if c == "(": depth += 1; paren += 1
            elif c == ")":
                depth -= 1
                if paren: paren -= 1
            elif c == "[": bracket += 1
            elif c == "]": bracket = max(0, bracket - 1)
            elif c == "{": brace += 1
            elif c == "}": brace = max(0, brace - 1)
            elif c == "," and depth == 1 and bracket == 0 and brace == 0:
                commas += 1
            i += 1
        if depth or any(a <= match.start() < b for a, b in definitions):
            continue
        args = code[open_at + 1:i - 1].strip()
        line_start = max(code.rfind(";", 0, match.start()), code.rfind("{", 0, match.start()),
                         code.rfind("}", 0, match.start())) + 1
        prefix = code[line_start:match.start()]
        if re.search(r"\b(?:extern|static)\b", prefix) and code[i:].lstrip().startswith(";"):
            continue
        if mode.startswith("pad"):
            desired = int(mode[3:])
            arity = 0 if not args else commas + 1
            if arity < desired:
                edits.append((i - 1, i - 1, ", 0" * (desired - arity)))
        elif mode == "unprototyped":
            edits.append((match.start(), open_at, f"((int16_t (*)()){name})"))
        elif mode.startswith("rename:"):
            edits.append((match.start(), open_at, mode.split(":", 1)[1]))
    for a, b, replacement in sorted(edits, reverse=True):
        text = text[:a] + replacement + text[b:]
    return text


def run_one(source: str, overlay: Path, cfg: Path, index: int):
    obj = WORK / "out" / f"{index:03d}_{Path(source).stem}.o"
    obj.parent.mkdir(parents=True, exist_ok=True)
    common = [str(GCC), *FLAGS, "-DPORT_BUILD=1", "-include", str(HOST / "compat.h"),
              "-include", str(cfg), "-I", str(PORT), "-I", str(HOST / "include"),
              "-I", str(ROOT / "include"), "-I", str(ROOT / "src")]
    runs = []
    for mode in ("syntax", "object"):
        cmd = [*common, "-fsyntax-only", str(overlay)] if mode == "syntax" else [
            *common, "-c", str(overlay), "-o", str(obj)]
        try:
            proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                                  errors="replace", timeout=TIMEOUT, check=False)
            status = "ok" if proc.returncode == 0 else "failed"
            runs.append({"mode": mode, "status": status, "returncode": proc.returncode,
                         "stdout": proc.stdout, "stderr": proc.stderr, "command": cmd})
        except subprocess.TimeoutExpired as exc:
            detail = exc.stderr.decode(errors="replace") if isinstance(exc.stderr, bytes) else (exc.stderr or "timeout")
            runs.append({"mode": mode, "status": "timeout", "returncode": None,
                         "stdout": "", "stderr": detail, "command": cmd})
    error_lines = sorted({line.strip() for run in runs for line in run["stderr"].splitlines()
                          if "error:" in line})
    warning_lines = sorted({line.strip() for run in runs for line in run["stderr"].splitlines()
                            if "warning:" in line})
    return {"source": source, "status": {r["mode"]: r["status"] for r in runs},
            "returncodes": {r["mode"]: r["returncode"] for r in runs},
            "error_count": len(error_lines), "warning_count": len(warning_lines),
            "errors": error_lines, "warnings": warning_lines, "runs": runs}


def run_port_mode(root: Path, gcc: Path, mode: str) -> int:
    global ROOT, PORT, HOST, WORK, GCC, STRICT_CENTRAL
    ROOT = root.resolve()
    PORT = ROOT / "tools/porting/port_include"
    HOST = ROOT / "tools/porting/host"
    WORK = ROOT / "build/porting/host-probe" / mode
    GCC = gcc.resolve()
    STRICT_CENTRAL = mode == "strict-central"
    declarations.ROOT = ROOT
    manifest = readj(ROOT / "layout/manifest.json")
    sources = collect_sources(manifest)
    shared_tags = set(readj(PORT / "declaration-evidence.json").get("shared_struct_tags", []))
    if len(sources) > 128:
        raise SystemExit(f"bounded source inventory exceeded: {len(sources)}")
    WORK.mkdir(parents=True, exist_ok=True)
    (WORK / "overlay/src").mkdir(parents=True, exist_ok=True)
    results = []
    deadline = time.monotonic() + TOTAL_LIMIT
    for i, (source, owner) in enumerate(sources.items(), start=1):
        if time.monotonic() >= deadline:
            raise SystemExit(f"total probe exceeded {TOTAL_LIMIT}s")
        text = (ROOT / source).read_text(encoding="latin-1")
        cfg, local_fns, local_data = local_config(source, text, owner)
        transformed, removed = transformed_source(source, text, local_fns, local_data, shared_tags)
        overlay = WORK / "overlay" / source
        overlay.parent.mkdir(parents=True, exist_ok=True)
        overlay.write_text(transformed, encoding="latin-1")
        item = run_one(source, overlay, cfg, i)
        item["owner"] = owner
        item["removed_declaration_statements"] = removed
        results.append(item)
        print(f"[{i}/{len(sources)}] {source}: syntax={item['status']['syntax']} object={item['status']['object']} errors={item['error_count']} warnings={item['warning_count']} removed={removed}", flush=True)
    syntax_fail = sum(x["status"]["syntax"] != "ok" for x in results)
    object_fail = sum(x["status"]["object"] != "ok" for x in results)
    data = {"compiler": str(GCC), "source_count": len(results), "flags": FLAGS,
            "mode": mode,
            "mode_detail": ("central-only target declarations; TU-local externs/prototypes omitted" if STRICT_CENTRAL else "compatibility views and source-specific adapters over the central target header"),
            "syntax_failures": syntax_fail, "object_failures": object_fail,
            "results": results}
    outjson = WORK / "results.json"
    outjson.write_text(json.dumps(data, indent=2), encoding="utf-8")
    lines = ["# PORT_BUILD host probe: " + mode, "",
             "This port-only probe compiles active C recipe sources with GCC and the shared declarations.",
             ("Strict central-only mode exposes the central target declarations and removes per-TU legacy extern/prototype views." if STRICT_CENTRAL else "Compatibility mode adds host-only per-TU declaration views and call adapters; it preserves the original source files and writes transformed copies under build/porting."),
             "The probe checks syntax and object compilation separately; it does not link or execute code.", "",
             f"Aggregate: syntax failures {syntax_fail}/{len(results)}; object failures {object_fail}/{len(results)}.", "",
             "| Source | Syntax | Object | Errors | Warnings |", "|---|---:|---:|---:|---:|"]
    for x in results:
        lines.append(f"| `{x['source']}` | {x['status']['syntax']} | {x['status']['object']} | {x['error_count']} | {x['warning_count']} |")
    lines += ["", "## Remaining compiler errors", ""]
    for x in results:
        if not x["errors"]:
            continue
        lines.append(f"### `{x['source']}`")
        lines += [f"- {err}" for err in x["errors"]]
        lines.append("")
    (WORK / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"{mode}: syntax failures={syntax_fail}/{len(results)}; object failures={object_fail}/{len(results)}")
    print(f"Report: {WORK / 'report.md'}")
    print(f"Raw: {outjson}")

