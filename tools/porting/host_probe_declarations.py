"""Source declaration parsing helpers for the non-matching host probe."""
from __future__ import annotations

import collections
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
import cdecls  # noqa: E402

TYPE_PREFIX = """\
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
"""

ALIASES = {
    "I8": "char", "I8S": "signed char", "U8": "unsigned char",
    "I16": "short", "U16": "unsigned short", "I16S": "short", "U16S": "unsigned short",
    # MinGW's stdint int32_t/uint32_t are int/unsigned int (LLP64), matching
    # tools/porting/host/compat.h; target comments preserve the 32-bit intent.
    "I32": "int32_t", "U32": "uint32_t", "FAR": "far", "NEAR": "near", "HUGE": "huge",
}

OVERRIDES = {
    # Independently supported by typeinfer callee/callsite evidence and active definitions.
    "timer_get_delta_alt": "int16_t far timer_get_delta_alt(void);",
    "security_check": "void far security_check(int16_t selection);",
    "sprite_blit_to_video": "int16_t far sprite_blit_to_video(struct SPRITE far *sprite, int16_t mode);",
    # Target callers supply one far pointer (4 bytes) plus one selector word (2 bytes).
    "nullsub_2": "void far nullsub_2(void far *resource, int16_t selector);",
    # Active object definition is the best ABI evidence: one near filename pointer, 16-bit status.
    "file_write_replay": "int16_t far file_write_replay(const char *filename);",
}
HOST_OVERRIDES = {
    # Match the host C library declarations supplied by GCC/MinGW headers.
    "abs": "extern int abs(int);",
    "exit": "extern void exit(int);",
    "rand": "extern int rand(void);",
    "strrchr": "extern char *strrchr(const char *, int);",
    "_itoa": "extern char *_itoa(int, char *, int);",
    "_ultoa": "extern char *_ultoa(unsigned long, char *, int);",
    "itoa": "extern char *itoa(int, char *, int);",
    "strlen": "extern size_t strlen(const char *);",
    "strcmp": "extern int strcmp(const char *, const char *);",
    "strcat": "extern char *strcat(char *, const char *);",
    "strcpy": "extern char *strcpy(char *, const char *);",
    "toupper": "extern int toupper(int);",
    "main": "extern int main(int, char **);",
}


def readj(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def source_parser_text(text: str) -> str:
    # cdecls intentionally reads only one C file. Expand the project's scalar
    # aliases to their host spellings, then give the parser typedefs used by the
    # host shim. This distinguishes bare host int from I16/I16S.
    text = re.sub(r"\btypedef\s+unsigned\s+int\s+u16\s*;", "typedef unsigned short u16;", text)
    text = re.sub(r"\btypedef\s+unsigned\s+long\s+u32\s*;", "typedef unsigned long u32;", text)
    for alias, replacement in sorted(ALIASES.items(), key=lambda kv: -len(kv[0])):
        text = re.sub(rf"\b{alias}\b", replacement, text)
    return TYPE_PREFIX + "\n" + text


def norm_proto(text: str) -> str:
    text = text.replace("struct ?", "void")
    text = re.sub(r"\bfar\b|\bnear\b|\bhuge\b", "", text)
    text = re.sub(r"\bunsigned\s+long\b", "uint32_t", text)
    text = re.sub(r"\blong\b", "int32_t", text)
    text = re.sub(r"\bunsigned\s+int\b", "uint16_t", text)
    text = re.sub(r"\bint\b", "int16_t", text)
    text = re.sub(r"\bunsigned\s+short\b", "uint16_t", text)
    text = re.sub(r"\bshort\b", "int16_t", text)
    text = re.sub(r"\?", "int16_t", text)
    return re.sub(r"\s+", " ", text).strip()


def parse_type_declaration(decl_text: str, expected: str | None = None):
    unit = cdecls.Unit(source_parser_text(decl_text))
    if expected:
        rows = unit.decls.get(expected, [])
        if rows:
            return rows[-1]["type"]
    if len(unit.decls) == 1:
        return next(iter(unit.decls.values()))[-1]["type"]
    return None


def render_type(t, name: str, target: bool) -> str:
    if t is None:
        return f"int16_t {name}"
    k = t["k"]
    if k == "base":
        b = t.get("name", "int")
        u = t.get("unsigned")
        if b == "void": base = "void"
        elif b == "char":
            signed = t.get("signed")
            base = ("uint8_t" if u else "int8_t" if signed else "char") if target else (
                "unsigned char" if u else "signed char" if signed else "char")
        elif b in ("short", "int"):
            base = ("uint16_t" if u else "int16_t") if target else (("unsigned " if u else "") + b)
        elif b == "long":
            base = ("uint32_t" if u else "int32_t") if target else (("unsigned " if u else "") + "long")
        elif b == "float": base = "float"
        elif b == "double": base = "double"
        else: base = b
        return f"{base} {name}".strip()
    if k == "struct":
        return f"{t.get('kw', 'struct')} {t.get('tag', '__unknown')} {name}".strip()
    if k == "ptr":
        inner = "*" + name
        if t["to"]["k"] in ("fn", "arr"):
            inner = "(" + inner + ")"
        return render_type(t["to"], inner, target)
    if k == "arr":
        n = "" if t.get("n") is None else str(t["n"])
        return render_type(t["of"], f"{name}[{n}]", target)
    if k == "fn":
        params = t.get("params")
        if params is None:
            ps = ""
        elif not params:
            ps = "void"
        else:
            ps = ", ".join(render_type(p, "", target).strip() for p in params)
        if t.get("varargs"):
            ps = ps + (", ..." if ps else "...")
        fnname = f"{name}({ps})"
        return render_type(t["ret"], fnname, target)
    return f"int16_t {name}"


def decl_from_model_type(type_text: str, name: str, size_bytes=None) -> str | None:
    s = type_text.strip()
    if s.startswith("MASM "):
        if s == "MASM word data object" and isinstance(size_bytes, int) and size_bytes > 0 and size_bytes % 2 == 0:
            return f"st_near_data_offset {name}[{size_bytes // 2}]"
        return None
    if s == "unsigned": s = "unsigned int"
    s = re.sub(r"\bI8S\b", "signed char", s)
    s = re.sub(r"\bI8\b", "char", s)
    s = re.sub(r"\bU8\b", "unsigned char", s)
    s = re.sub(r"\bI16S?\b", "short", s)
    s = re.sub(r"\bU16S?\b", "unsigned short", s)
    s = re.sub(r"\bI32\b", "long", s)
    s = re.sub(r"\bU32\b", "unsigned long", s)
    s = re.sub(r"\b(?:far|near|huge)\b", "", s)
    # registry/state model stores arrays as "type [N][M]".
    dims = "".join(re.findall(r"\s*(\[[^]]*\])", s))
    dims = "".join(d if re.fullmatch(r"\[\s*\d+\s*\]", d) else "[]" for d in re.findall(r"\[[^]]*\]", dims))
    base = re.sub(r"\s*(?:\[[^]]*\])+$", "", s).strip()
    if not base:
        return None
    if base == "MouseRegs":
        return f"MouseRegs {name}{dims};"
    if not re.search(r"\b(?:char|short|int|long|float|double|void|struct|union|enum)\b|\*", base):
        return None
    base = re.sub(r"\bunsigned\s+long\b", "uint32_t", base)
    base = re.sub(r"\blong\b", "int32_t", base)
    base = re.sub(r"\bunsigned\s+int\b", "uint16_t", base)
    base = re.sub(r"\bint\b", "int16_t", base)
    base = re.sub(r"\s*\*\s*", " *", base).strip()
    return f"{base} {name}{dims};"


def mask_comments(text: str) -> str:
    out = list(text)
    i = 0
    state = "code"
    while i < len(text):
        c = text[i]
        n = text[i + 1] if i + 1 < len(text) else ""
        if state == "code":
            if c == "/" and n == "*": out[i] = out[i+1] = " "; state = "block"; i += 2; continue
            if c == "/" and n == "/": out[i] = out[i+1] = " "; state = "line"; i += 2; continue
            if c == '"': out[i] = " "; state = "string"; i += 1; continue
            if c == "'": out[i] = " "; state = "char"; i += 1; continue
        elif state == "block":
            if c == "*" and n == "/": out[i] = out[i+1] = " "; state = "code"; i += 2; continue
            if c not in "\r\n": out[i] = " "
        elif state == "line":
            if c in "\r\n": state = "code"
            else: out[i] = " "
        else:
            q = '"' if state == "string" else "'"
            if c == "\\" and i + 1 < len(text):
                out[i] = " "
                if text[i+1] not in "\r\n": out[i+1] = " "
                i += 2; continue
            if c == q: out[i] = " "; state = "code"
            elif c not in "\r\n": out[i] = " "
        i += 1
    return "".join(out)


def commentless(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\r\n]*", " ", text, flags=re.S)


def top_level_spans(text: str):
    code = mask_comments(text)
    depth = paren = bracket = 0
    start = 0
    root_is_function = False
    spans = []
    i = 0
    while i < len(code):
        c = code[i]
        line_start = code.rfind("\n", 0, i) + 1
        if c == "#" and not code[line_start:i].strip() and depth == 0:
            nl = code.find("\n", i)
            i = len(code) if nl < 0 else nl + 1
            # Preprocessor lines (especially #define) are not part of the following
            # declaration span that PORT_BUILD may replace.
            start = i
            continue
        if c == "(": paren += 1
        elif c == ")": paren = max(0, paren - 1)
        elif c == "[": bracket += 1
        elif c == "]": bracket = max(0, bracket - 1)
        elif c == "{" and paren == 0 and bracket == 0:
            if depth == 0:
                # Classify the construct from its header, before consuming its body.
                # Testing the full body at '}' misses every ordinary definition.
                signature = code[start:i].strip()
                root_is_function = bool(re.search(
                    r"[A-Za-z_]\w*\s*\([^;{}]*\)\s*$", signature, re.S))
            depth += 1
        elif c == "}" and paren == 0 and bracket == 0:
            depth = max(0, depth - 1)
            if depth == 0 and root_is_function:
                spans.append((start, i + 1, "function"))
                start = i + 1
                root_is_function = False
        elif c == ";" and depth == 0 and paren == 0 and bracket == 0:
            spans.append((start, i + 1, "stmt"))
            start = i + 1
        i += 1
    return spans


def function_headers(text: str):
    code = mask_comments(text)
    out = []
    for a, b, kind in top_level_spans(text):
        if kind != "function":
            continue
        body = code.find("{", a, b)
        if body < 0:
            continue
        sig = code[a:body].strip()
        m = re.search(r"([A-Za-z_]\w*)\s*\([^;{}]*\)\s*$", sig, re.S)
        if m:
            name = m.group(1)
            raw = re.sub(r"(?m)^\s*#.*$", "", sig).strip()
            out.append((name, raw + ";"))
    return out


def call_arities(text: str, names: set[str]):
    """Count argument expressions for calls/declarations, with nested commas ignored."""
    code = mask_comments(text)
    rows = collections.defaultdict(set)
    keywords = {"if", "for", "while", "switch", "sizeof", "return", "_Generic"}
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", code):
        name = m.group(1)
        if name not in names or name in keywords:
            continue
        start = code.find("(", m.start())
        depth = 1
        paren = bracket = brace = 0
        commas = 0
        i = start + 1
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
        if depth:
            continue
        args = code[start + 1:i - 1].strip()
        # C's explicit empty parameter list is commonly written as (void).
        # It is not a one-argument call/prototype for compatibility analysis.
        rows[name].add(0 if not args or args == "void" else commas + 1)
    return rows


def collect_sources(manifest):
    out = {}
    for owner in manifest["owners"]:
        if owner.get("classification") != "GAME_C" or owner.get("kind") not in ("MATCHING_C", "MATCHING_C_DATA"):
            continue
        recipe_path = owner.get("recipe")
        if not recipe_path:
            continue
        recipe = readj(ROOT / recipe_path)
        if recipe.get("kind", "c") == "asm" or not recipe.get("source"):
            continue
        out[recipe["source"]] = owner["id"]
    return dict(sorted(out.items()))

