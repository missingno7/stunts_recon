"""Empirical MSC 5.10 /O automatic-local BP home predictor.

The observable local order is the C1 local-symbol hash-table order: sum the
ASCII bytes of the identifier's first 31 significant characters and keep the
low four bits, visit buckets 0..15, and visit each collision chain
newest-first.  Within a lexical block, that is equivalent to sorting by
(bucket, reverse declaration order).  Each child block starts after the homes
reserved by its active ancestors.  Exited block locals enter a LIFO free list;
the next block reuses the newest free home with enough capacity, or extends
the frame.  Siblings therefore reuse compatible homes even when local sizes
differ.

This module is a research model, not production tooling.  It predicts homes
only.  It does not model optimizer-elided locals, hidden temporaries, register
selection, spills, parameter ABI offsets, or target-specific object binding.
"""
from __future__ import annotations

import argparse
import difflib
import json
import re
from collections import OrderedDict
from functools import lru_cache
from pathlib import Path


DEFAULT_NAMES = [
    "i", "j", "k", "len", "ptr", "count", "x", "y", "tmp", "src",
    "dst", "value", "index", "out", "ch", "wide", "acc", "temp",
    "result", "left", "right", "pos", "size", "p", "q",
]
_IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def bucket(name: str) -> int:
    """Return MSC's observed 4-bit local-name hash bucket."""
    if not isinstance(name, str) or not _IDENT.fullmatch(name):
        raise ValueError(f"invalid C identifier: {name!r}")
    try:
        raw = name.encode("ascii")[:31]
    except UnicodeEncodeError as exc:
        raise ValueError(f"non-ASCII identifier is outside this model: {name!r}") from exc
    return sum(raw) & 0x0F


def allocated_size(local: dict) -> int:
    """Return the even-byte BP allocation size from a simple C type spelling.

    Callers may provide ``size_bytes`` for structs or other declarations whose
    layout is known externally.  Type spellings accept scalar C names, near or
    far pointers (for example ``int *`` and ``char far *``), and arrays such as
    ``char[80]``.  An explicit ``register`` local still reserves its home.
    """
    explicit = local.get("size_bytes", local.get("size"))
    if explicit is not None:
        size = int(explicit)
    else:
        spelling = str(local.get("type", "int")).strip().lower()
        array = re.search(r"\[(\d+)\]\s*$", spelling)
        count = int(array.group(1)) if array else 1
        if array:
            spelling = spelling[:array.start()].strip()
        if "*" in spelling:
            size = 4 if "far" in spelling else 2
        elif "long" in spelling and "double" not in spelling:
            size = 4
        elif "char" in spelling:
            size = 1
        elif "short" in spelling or "int" in spelling or "enum" in spelling:
            size = 2
        elif "struct_size" in local:
            size = int(local["struct_size"])
        else:
            raise ValueError(f"unknown local type size: {local.get('type')!r}")
        size *= count
    if size <= 0:
        raise ValueError(f"local must occupy positive storage: {local!r}")
    return (size + 1) & ~1


def _path(local: dict) -> tuple[str, ...]:
    value = local.get("block", local.get("scope", ()))
    if value is None:
        return ()
    if isinstance(value, (str, int)):
        return (str(value),)
    return tuple(str(item) for item in value)


def predict(locals_: list[dict]) -> dict:
    """Predict BP offsets for locals.

    Each item has ``name`` and may provide ``type`` or ``size_bytes``,
    ``block`` (a lexical path, default root), ``storage`` (``auto``,
    ``register``, ``static`` or ``parameter``), and ``declaration_index``.
    Input order supplies the declaration index when it is omitted.  The result
    contains per-local rows, exact per-block allocation order, and maximum
    stack bytes implied by this local set.
    """
    records = []
    for index, original in enumerate(locals_):
        row = dict(original)
        if "name" not in row:
            raise ValueError(f"local #{index} is missing name")
        row.setdefault("key", f"{row['name']}@{index}")
        row.setdefault("declaration_index", index)
        row.setdefault("storage", "auto")
        row["bucket"] = bucket(row["name"])
        row["allocated_bytes"] = allocated_size(row) if row["storage"] in ("auto", "register") else 0
        row["block_path"] = _path(row)
        row["bp_offset"] = None
        records.append(row)

    by_path: dict[tuple[str, ...], list[dict]] = OrderedDict()
    children: dict[tuple[str, ...], list[tuple[str, ...]]] = OrderedDict()
    by_path[()] = []
    for row in records:
        if row["storage"] not in ("auto", "register"):
            continue
        path = row["block_path"]
        for depth in range(1, len(path) + 1):
            parent = path[:depth - 1]
            child = path[:depth]
            by_path.setdefault(parent, [])
            by_path.setdefault(child, [])
            child_list = children.setdefault(parent, [])
            if child not in child_list:
                child_list.append(child)
        by_path.setdefault(path, []).append(row)

    orders = []
    cursor = 0
    free_slots = []

    def allocate(row):
        nonlocal cursor
        requested = row["allocated_bytes"]
        found = None
        for index in range(len(free_slots) - 1, -1, -1):
            if free_slots[index]["capacity"] >= requested:
                found = free_slots.pop(index)
                break
        if found is None:
            cursor += requested
            found = {"offset": -cursor, "capacity": requested,
                     "origin": row["key"]}
            reused_from = None
        else:
            reused_from = found["origin"]
        row["bp_offset"] = found["offset"]
        row["slot_capacity_bytes"] = found["capacity"]
        row["reused_from"] = reused_from
        return found

    next_slot_id = 0

    def assign(path: tuple[str, ...], release_at_exit: bool):
        nonlocal next_slot_id
        members = sorted(by_path.get(path, []),
                         key=lambda r: (r["bucket"], -int(r["declaration_index"])))
        allocated = []
        for row in members:
            slot = allocate(row)
            if "slot_id" not in slot:
                slot["slot_id"] = next_slot_id
                next_slot_id += 1
            allocated.append(slot)
        orders.append({"block": list(path), "order": [r["key"] for r in members],
                       "buckets": [r["bucket"] for r in members],
                       "homes": [r["bp_offset"] for r in members],
                       "reused_from": [r.get("reused_from") for r in members]})
        subtree_slots = list(allocated)
        for child in children.get(path, []):
            subtree_slots.extend(assign(child, True))
        if release_at_exit:
            # Inner scopes return homes immediately so sibling blocks can use
            # them.  At containing-scope exit C1 reorders the released family:
            # direct homes first, then descendant homes in lexical preorder.
            # This leaves the deepest/last descendant home on top.
            unique = []
            seen_slots = set()
            for slot in subtree_slots:
                if slot["slot_id"] not in seen_slots:
                    unique.append(slot)
                    seen_slots.add(slot["slot_id"])
            free_slots[:] = [slot for slot in free_slots
                             if slot["slot_id"] not in seen_slots]
            free_slots.extend(unique)
            subtree_slots = unique
        return subtree_slots

    assign((), False)
    return {
        "rule": {"bucket_count": 16,
                 "bucket": "sum(first 31 significant identifier ASCII bytes) & 0x0f",
                 "bucket_visit": "ascending 0..15", "chain_visit": "reverse declaration order",
                 "scope": "active ancestors allocate first; newest freed home large enough is reused; at scope exit, direct homes then descendant homes are returned in lexical preorder"},
        "locals": [{"key": r["key"], "name": r["name"], "type": r.get("type"),
                    "storage": r["storage"], "block": list(r["block_path"]),
                    "bucket": r["bucket"], "allocated_bytes": r["allocated_bytes"],
                    "slot_capacity_bytes": r.get("slot_capacity_bytes"),
                    "reused_from": r.get("reused_from"),
                    "bp_offset": r["bp_offset"]} for r in records],
        "blocks": orders,
        "frame_bytes": cursor,
    }


def suggest_names(desired_home_order, candidates=None, *, allowed_by_local=None):
    """Suggest readable C identifiers that realize ``desired_home_order``.

    ``desired_home_order`` is a sequence of semantic labels (or records with
    ``key``/``name``).  Candidate names may be supplied globally or per label
    through ``allowed_by_local``.  The search keeps bucket numbers nondecreasing
    and, for collisions, emits a declaration order reversed within each bucket
    so MSC's newest-first chains produce the requested homes.
    """
    candidates = list(DEFAULT_NAMES if candidates is None else candidates)
    labels = []
    for index, item in enumerate(desired_home_order):
        if isinstance(item, dict):
            key = str(item.get("key", item.get("name", f"local{index}")))
            label = str(item.get("name", key))
        else:
            key = label = str(item)
        labels.append((key, label))
    if len(labels) == 0:
        return {"assignments": [], "declaration_order": []}
    if len(candidates) < len(labels):
        raise ValueError("not enough candidate identifiers for requested locals")

    unique = []
    seen = set()
    for name in candidates:
        if name not in seen and _IDENT.fullmatch(name):
            bucket(name)
            unique.append(name)
            seen.add(name)

    options = []
    for key, label in labels:
        allowed = (allowed_by_local or {}).get(key, (allowed_by_local or {}).get(label, unique))
        if isinstance(allowed, str):
            allowed = [allowed]
        allowed_set = set(allowed)
        row = [name for name in unique if name in allowed_set]
        if not row:
            raise ValueError(f"no candidate identifier is allowed for {label!r}")
        options.append(row)

    # Dynamic programming over used candidates and the last bucket.  Local
    # counts in recovery work are small; a bit mask also makes uniqueness exact.
    name_index = {name: i for i, name in enumerate(unique)}
    costs = {}
    for index, (_, label) in enumerate(labels):
        for name in options[index]:
            similarity = difflib.SequenceMatcher(None, label.lower(), name.lower()).ratio()
            costs[(index, name)] = (1.0 - similarity) * 20 + name_index[name] * 0.01

    @lru_cache(None)
    def solve(position: int, used_mask: int, last_bucket: int):
        if position == len(labels):
            return 0.0, ()
        best = None
        for name in options[position]:
            bit = 1 << name_index[name]
            value_bucket = bucket(name)
            if used_mask & bit or value_bucket < last_bucket:
                continue
            tail = solve(position + 1, used_mask | bit, value_bucket)
            if tail is None:
                continue
            score = costs[(position, name)] + tail[0]
            candidate = (score, (name,) + tail[1])
            if best is None or candidate < best:
                best = candidate
        return best

    result = solve(0, 0, 0)
    if result is None:
        raise ValueError("no unique candidate assignment realizes this home order")
    chosen = result[1]
    assignments = []
    for (key, label), name in zip(labels, chosen):
        assignments.append({"key": key, "local": label, "identifier": name,
                            "bucket": bucket(name)})
    # Bucket order is already monotone in home order; reverse each collision
    # group when declaring to make the chain traversal match the requested order.
    declaration_order = []
    group = []
    previous_bucket = None
    for assignment in assignments:
        if previous_bucket is not None and assignment["bucket"] != previous_bucket:
            declaration_order.extend(reversed(group))
            group = []
        group.append(assignment)
        previous_bucket = assignment["bucket"]
    declaration_order.extend(reversed(group))
    return {"assignments": assignments,
            "declaration_order": [row["key"] for row in declaration_order],
            "score": result[0],
            "note": "Declare locals in declaration_order; ordinary lexical blocks still determine nesting homes."}


def locals_from_source(source: str, function: str) -> list[dict]:
    """Read simple local declarations from one C function for quick CLI probes.

    Aggregate sizes and complex declarators should use the JSON input form.
    """
    match = re.search(r'\b' + re.escape(function) + r'\s*\([^;{}]*\)\s*\{', source)
    if not match:
        raise ValueError(f'function body not found: {function}')
    start = match.end()
    depth = 1
    end = start
    while depth and end < len(source):
        if source[end] == '{':
            depth += 1
        elif source[end] == '}':
            depth -= 1
        end += 1
    if depth:
        raise ValueError('unterminated function body')
    body = re.sub(r'/\*.*?\*/|//[^\n]*', '', source[start:end-1], flags=re.S)
    declaration = re.compile(
        r'^\s*(?:(register|static)\s+)?'
        r'((?:(?:unsigned|signed|short|long|char|int|void|far|near|const)\s+)+)'
        r'(.+)$', re.S)
    rows = []
    scope = []
    scope_number = 0
    statement = ''
    for char in body + ';':
        if char in '{};':
            found = declaration.match(statement)
            if found:
                for declarator in found.group(3).split(','):
                    declarator = declarator.split('=', 1)[0].strip()
                    name_match = re.fullmatch(r'(\*+\s*)?([A-Za-z_]\w*)(\s*\[\s*\d+\s*\])?', declarator)
                    if not name_match:
                        raise ValueError('Complex declarator requires JSON local specification: ' + declarator)
                    rows.append({'name': name_match.group(2),
                                 'type': found.group(2).strip() + ' '
                                         + ('*' if name_match.group(1) else '')
                                         + (name_match.group(3) or ''),
                                 'storage': found.group(1) or 'auto', 'block': list(scope)})
            statement = ''
            if char == '{':
                scope_number += 1
                scope.append('b' + str(scope_number))
            elif char == '}':
                if scope:
                    scope.pop()
        else:
            statement += char
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", nargs="?", help="JSON file containing {locals:[...]}")
    parser.add_argument("--names", nargs="*", help="Desired home order; emits readable identifier suggestions")
    parser.add_argument("--candidates", nargs="*", help="Identifier pool for --names")
    parser.add_argument("--source", type=Path, help="C source containing a simple function body")
    parser.add_argument("--function", help="Function name with --source")
    args = parser.parse_args()
    if args.names is not None:
        print(json.dumps(suggest_names(args.names, args.candidates), indent=2))
    else:
        if args.source:
            if not args.function:
                parser.error("--source requires --function")
            locals_ = locals_from_source(args.source.read_text(encoding="latin1"), args.function)
        else:
            if args.input is None:
                parser.error("provide a local-spec JSON file, --source/--function, or --names")
            value = json.loads(Path(args.input).read_text(encoding="utf-8"))
            locals_ = value.get("locals", value) if isinstance(value, dict) else value
        print(json.dumps(predict(locals_), indent=2))


if __name__ == "__main__":
    main()
