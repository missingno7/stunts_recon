"""Bounded lexical scope inference for semantic span candidates.

This intentionally proves only literal, simple bounds. Unknown writes erase a
local's prior value; control-flow/alias facts are not guessed.
"""
from __future__ import annotations

import re


def _mask_literals(code: str) -> str:
    """Mask comments and C string/character contents without moving offsets."""
    out = list(code)
    i = 0
    while i < len(code):
        if code.startswith("//", i):
            end = code.find("\n", i)
            if end < 0:
                end = len(code)
            for j in range(i, end):
                out[j] = " "
            i = end
            continue
        if code.startswith("/*", i):
            end = code.find("*/", i + 2)
            end = len(code) if end < 0 else end + 2
            for j in range(i, end):
                if out[j] != "\n":
                    out[j] = " "
            i = end
            continue
        if code[i] in ('"', "'"):
            quote = code[i]
            out[i] = " "
            i += 1
            while i < len(code):
                char = code[i]
                out[i] = " " if char != "\n" else "\n"
                if char == "\\":
                    i += 1
                    if i < len(code):
                        out[i] = " " if code[i] != "\n" else "\n"
                elif char == quote:
                    i += 1
                    break
                i += 1
            continue
        i += 1
    return "".join(out)


def _matching(code: str, start: int, left: str, right: str) -> int | None:
    depth = 0
    for i in range(start, len(code)):
        if code[i] == left:
            depth += 1
        elif code[i] == right:
            depth -= 1
            if depth == 0:
                return i
    return None


def _body_end(code: str, start: int) -> int:
    while start < len(code) and code[start].isspace():
        start += 1
    if start < len(code) and code[start] == "{":
        close = _matching(code, start, "{", "}")
        return len(code) if close is None else close + 1
    # A single C statement body. Braced subexpressions and nested parentheses
    # are skipped so the first top-level semicolon closes the controlled body.
    paren = bracket = brace = 0
    for i in range(start, len(code)):
        char = code[i]
        if char == "(": paren += 1
        elif char == ")": paren = max(0, paren - 1)
        elif char == "[": bracket += 1
        elif char == "]": bracket = max(0, bracket - 1)
        elif char == "{": brace += 1
        elif char == "}":
            if brace == 0:
                return i
            brace -= 1
        elif char == ";" and paren == 0 and bracket == 0 and brace == 0:
            return i + 1
    return len(code)


def lexical_scopes(code: str):
    """Return brace ranges and simple for/while body ranges, all half-open."""
    masked = _mask_literals(code)
    blocks = []
    stack = []
    for i, char in enumerate(masked):
        if char == "{":
            stack.append(i)
        elif char == "}" and stack:
            begin = stack.pop()
            blocks.append((begin + 1, i))

    loops = []
    for match in re.finditer(r"\b(for|while)\s*\(", masked):
        open_paren = masked.find("(", match.start())
        close_paren = _matching(masked, open_paren, "(", ")")
        if close_paren is None:
            continue
        header = masked[open_paren + 1:close_paren]
        body_start = close_paren + 1
        while body_start < len(masked) and masked[body_start].isspace():
            body_start += 1
        loops.append({
            "kind": match.group(1), "start": match.start(), "header_start": open_paren + 1,
            "header_end": close_paren, "body_start": body_start,
            "body_end": _body_end(masked, body_start), "header": header,
        })
    return masked, blocks, loops


def _loop_fact(loop: dict, env: dict[str, int], eval_max):
    if loop["kind"] == "for":
        pieces = loop["header"].split(";")
        if len(pieces) != 3:
            return None
        init, condition = pieces[0].strip(), pieces[1].strip()
        init_match = re.search(
            r"(?:(?:I8S?|U8|I16S?|U16S?|I32|U32|int|short|long)\s+)?"
            r"([A-Za-z_]\w*)\s*=\s*(.+)$", init)
        cond_match = re.fullmatch(r"([A-Za-z_]\w*)\s*<\s*(.+)", condition)
        if not init_match or not cond_match or init_match.group(1) != cond_match.group(1):
            return None
        lo = eval_max(init_match.group(2), env, {})
        hi = eval_max(cond_match.group(2), env, {})
        if lo is None or hi is None or hi <= lo:
            return None
        return init_match.group(1), hi - 1
    cond_match = re.fullmatch(r"\s*([A-Za-z_]\w*)\s*<\s*(.+?)\s*", loop["header"])
    if not cond_match:
        return None
    hi = eval_max(cond_match.group(2), env, {})
    return (cond_match.group(1), hi - 1) if hi is not None else None


def scope_env_at(code: str, offset: int, macros: dict[str, int], eval_max):
    """Infer only facts in the access's enclosing blocks/loops, in source order.

    This removes the old whole-function maximum leak. Assignments with an
    unmodelled RHS invalidate the binding, so e.g. link = table[link] cannot
    leave a stale link=capacity fact behind for the next access.
    """
    masked, blocks, loops = lexical_scopes(code)
    containing_blocks = [scope for scope in blocks if scope[0] <= offset < scope[1]]
    # A preceding assignment is usable only when it is in the function's
    # root scope or a lexical block that also contains the queried access.
    allowed_block_starts = {scope[0] for scope in containing_blocks}
    active_loops = [loop for loop in loops if loop["body_start"] <= offset < loop["body_end"]]
    header_ranges = [(loop["header_start"], loop["header_end"]) for loop in loops]
    statements = []
    for loop in active_loops:
        fact = _loop_fact(loop, {}, eval_max)
        # Re-evaluate at the actual event position below, after earlier
        # assignments and enclosing loop facts have been applied.
        statements.append((loop["start"], "loop", loop))

    assign_pattern = re.compile(r"(?<![=!<>+\-*/%&|^])\b([A-Za-z_]\w*)\s*=\s*([^;\n]+);")
    for assign in assign_pattern.finditer(masked, 0, offset):
        if any(begin <= assign.start() < end for begin, end in header_ranges):
            continue
        containing = [scope for scope in blocks if scope[0] <= assign.start() < scope[1]]
        nearest = max(containing, key=lambda scope: scope[0], default=None)
        if nearest is not None and nearest[0] not in allowed_block_starts:
            continue
        statements.append((assign.start(), "assign", assign))

    env: dict[str, int] = dict(macros)
    # Parent loop facts are naturally applied before nested-loop facts.
    statements.sort(key=lambda row: (row[0], row[1] == "assign"))
    for _, kind, item in statements:
        if kind == "loop":
            fact = _loop_fact(item, env, eval_max)
            if fact:
                env[fact[0]] = fact[1]
            continue
        lhs, rhs = item.group(1), item.group(2).strip()
        value = eval_max(rhs, env, macros)
        if value is None:
            env.pop(lhs, None)
        else:
            env[lhs] = value
    return env


def pointer_max_index_scoped(code_body: str, param: str, macros: dict[str, int], eval_max):
    hits = []
    for match in re.finditer(r"\b" + re.escape(param) + r"\s*\[([^\]]+)\]", code_body):
        env = scope_env_at(code_body, match.start(), macros, eval_max)
        value = eval_max(match.group(1), env, macros)
        if value is not None:
            hits.append(value)
    return max(hits) if hits else None


def global_subscript_index_scoped(code_body: str, global_name: str, offset: int,
                                  index_expr: str, macros: dict[str, int], eval_max):
    return eval_max(index_expr, scope_env_at(code_body, offset, macros, eval_max), macros)
