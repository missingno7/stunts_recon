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

This module is a research model, not production tooling. It predicts BP homes
and the observed SI/DI assignment for explicit one-word ``register`` objects.
It derives save/restore order from those assignments plus any caller-supplied
compiler-generated SI/DI uses. It does not infer hidden temporaries, spills,
parameter ABI offsets, or target-specific object binding.
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


REGISTER_SEQUENCE = ("si", "di")


def _is_explicit_register(row: dict) -> bool:
    return row.get("storage") == "register" or bool(row.get("register", False))


def _register_type_eligible(row: dict) -> tuple[bool, str]:
    """Classify the observed MSC 5.10 register-local candidate types.

    The tested allocator places one-word integer and near-pointer objects in
    SI/DI.  Byte scalars, longs, and far pointers keep their homes in memory.
    An explicit ``register_candidate`` field can describe a future observed
    scalar spelling without changing the conservative defaults.
    """
    if row.get("register_candidate") is False:
        return False, "candidate_disabled"
    spelling = str(row.get("type", "int")).strip().lower()
    if re.search(r"\[\s*\d*\s*\]", spelling):
        return False, "array"
    if "*" in spelling:
        try:
            size = allocated_size(row)
        except (TypeError, ValueError):
            size = 4 if "far" in spelling else 2
        return (size == 2, "near_pointer" if size == 2 else "far_pointer_or_wide_pointer")
    if re.search(r"\b(char|long|float|double|struct|union|void)\b", spelling):
        return False, "non_word_scalar_or_aggregate"
    if not re.search(r"\b(short|int|enum)\b", spelling) and row.get("register_candidate") is not True:
        return False, "unmodeled_type"
    try:
        size = allocated_size(row)
    except (TypeError, ValueError):
        size = int(row.get("size_bytes", row.get("size", 0)))
    return (size == 2, "word_integer" if size == 2 else "wide_integer")


def _register_assignment(locals_: list[dict], other_register_uses=None) -> dict:
    """Predict SI/DI for explicit register objects using lexical lifetimes.

    Register candidates are visited in source declaration order, independent
    of the identifier-hash order used for BP homes. Function parameters marked
    register enter before locals. A nested scope inherits the active registers;
    its assignments are released at scope exit, allowing sibling reuse.
    """
    rows = []
    for index, original in enumerate(locals_):
        row = dict(original)
        row.setdefault("key", f"{row.get('name', 'local')}@{index}")
        row.setdefault("declaration_index", index)
        row["_input_order"] = index
        row["block_path"] = _path(row)
        row["explicit_register"] = _is_explicit_register(row)
        row["assigned_register"] = None
        if not row["explicit_register"]:
            row["register_reason"] = "not_explicit_register"
            row["type_eligible"] = False
            row["register_candidate"] = False
        else:
            eligible, reason = _register_type_eligible(row)
            row["type_eligible"] = eligible
            initialized = bool(row.get("initialized", row.get("initializer") is not None))
            referenced = bool(row.get("referenced", row.get("used", True)))
            row["register_candidate"] = eligible and (initialized or referenced)
            row["register_reason"] = reason
            if eligible and not initialized and not referenced:
                row["register_reason"] = "unreferenced_uninitialized"
        rows.append(row)

    assignments = []

    def try_assign(row, active):
        if not row["register_candidate"]:
            return active
        available = next((reg for reg in REGISTER_SEQUENCE if reg not in active), None)
        if available is None:
            row["register_reason"] = "no_free_register"
            return active
        row["assigned_register"] = available
        row["register_reason"] = "assigned"
        return set(active) | {available}

    # Explicit register parameters are present before any body declaration.
    active = set()
    params = sorted((r for r in rows if r.get("storage") == "parameter"),
                    key=lambda r: (int(r["declaration_index"]), r["_input_order"]))
    for row in params:
        active = try_assign(row, active)

    candidate_rows = [r for r in rows if r.get("storage") != "parameter"]
    path_rows = {}
    child_paths = {}
    for row in candidate_rows:
        path = row["block_path"]
        path_rows.setdefault(path, []).append(row)
        for depth in range(1, len(path) + 1):
            parent, child = path[:depth - 1], path[:depth]
            children = child_paths.setdefault(parent, [])
            if child not in children:
                children.append(child)

    def subtree_first(path):
        indexes = [int(r["declaration_index"]) for r in candidate_rows
                   if r["block_path"][:len(path)] == path]
        return min(indexes) if indexes else 10**12

    def walk(path, inherited):
        state = set(inherited)
        events = []
        for row in path_rows.get(path, []):
            events.append((int(row["declaration_index"]), row["_input_order"], "local", row))
        for child in child_paths.get(path, []):
            events.append((subtree_first(child), -1, "child", child))
        for _, _, kind, item in sorted(events, key=lambda e: (e[0], e[1])):
            if kind == "local":
                state = try_assign(item, state)
            else:
                # Child register assignments are scoped and freed on return.
                walk(item, state)

    walk((), active)
    extra = set()
    for register in (other_register_uses or []):
        value = str(register).strip().lower()
        if value not in REGISTER_SEQUENCE:
            raise ValueError(f"unsupported non-local register use: {register!r}; use SI or DI")
        extra.add(value)
    assigned = {r["assigned_register"] for r in rows if r["assigned_register"]}
    used_registers = assigned | extra
    push_sequence = [reg for reg in reversed(REGISTER_SEQUENCE) if reg in used_registers]
    pop_sequence = [reg for reg in REGISTER_SEQUENCE if reg in used_registers]
    for row in rows:
        if row["explicit_register"]:
            assignments.append({"key": row["key"], "name": row.get("name"),
                                "type": row.get("type"), "storage": row.get("storage"),
                                "block": list(row["block_path"]),
                                "declaration_index": int(row["declaration_index"]),
                                "eligible": row["type_eligible"],
                                "candidate": row["register_candidate"],
                                "register": row["assigned_register"],
                                "reason": row["register_reason"]})
    return {
        "register_rule": {
            "candidate_order": "explicit register parameters first, then source declaration order among active lexical scopes",
            "register_order": ["SI", "DI"],
            "eligible_types": "one-word integer scalars and near pointers; not char, long, aggregate, array, or far pointer",
            "dead_declaration": "an initialized candidate remains assigned even if its value is later dead; an uninitialized unreferenced declaration does not consume a register",
            "scope": "active outer assignments remain reserved; child assignments are released at block exit and siblings can reuse them",
            "home_interaction": "explicit register locals still receive normal BP homes from the independent local-name hash rule",
        },
        "register_assignments": assignments,
        "assigned_registers": [reg for reg in REGISTER_SEQUENCE if reg in assigned],
        "other_register_uses": [reg for reg in REGISTER_SEQUENCE if reg in extra],
        "used_registers": [reg for reg in REGISTER_SEQUENCE if reg in used_registers],
        "prologue_push_sequence": push_sequence,
        "epilogue_pop_sequence": pop_sequence,
        "push_sequence_scope": "complete only when other_register_uses includes compiler-generated SI/DI uses outside explicit register objects",
    }


def predict(locals_: list[dict], *, other_register_uses=None) -> dict:
    """Predict BP offsets for locals.

    Each item has ``name`` and may provide ``type`` or ``size_bytes``,
    ``block`` (a lexical path, default root), ``storage`` (``auto``,
    ``register``, ``static`` or ``parameter``), and ``declaration_index``.
    Input order supplies the declaration index when it is omitted.  The result
    contains per-local rows, exact per-block allocation order, maximum stack
    bytes, explicit register assignments, and the save/restore sequence implied
    by those assignments plus ``other_register_uses``.
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
    result = {
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
    result.update(_register_assignment(locals_, other_register_uses))
    return result


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


def suggest_registers(desired_assignments, candidates=None, *, allowed_by_local=None,
                      occupied_registers=None):
    """Suggest readable names and declaration order for desired SI/DI locals.

    ``desired_assignments`` may be ``{"si": "index", "di": "source"}`` or
    a list of records with ``register``, ``key``/``name`` and optional ``type``.
    For candidates active in one lexical block, declaring the SI item before
    the DI item realizes the requested mapping.  Names affect BP-home hash
    order only; the returned ``bp_home_order`` makes that independent order
    visible for later ``suggest_names``/``predict`` work.
    """
    if isinstance(desired_assignments, dict):
        requested = []
        for reg, item in desired_assignments.items():
            if isinstance(item, dict):
                row = dict(item)
                row.setdefault("register", reg)
            else:
                row = {"register": reg, "name": str(item), "key": str(item)}
            requested.append(row)
    else:
        requested = [dict(row) for row in desired_assignments]
    order = {"si": 0, "di": 1}
    for row in requested:
        reg = str(row.get("register", "")).lower()
        if reg not in order:
            raise ValueError(f"desired register must be SI or DI: {reg!r}")
        row["register"] = reg
        row.setdefault("key", row.get("name", f"local{len(requested)}"))
        row.setdefault("name", row["key"])
        row.setdefault("type", "int")
    requested.sort(key=lambda row: order[row["register"]])
    if len({row["register"] for row in requested}) != len(requested):
        raise ValueError("a single active scope can assign at most one desired local to each register")
    occupied = {str(reg).lower() for reg in (occupied_registers or [])}
    if occupied - set(order):
        raise ValueError("occupied_registers accepts only SI and DI")
    active = set(occupied)
    for row in requested:
        available = next((reg for reg in REGISTER_SEQUENCE if reg not in active), None)
        if available != row["register"]:
            context = f" with active {', '.join(sorted(occupied))}" if occupied else ""
            raise ValueError(f"requested {row['register'].upper()} assignment is not reachable{context}; "
                             "declarations take the first free register")
        active.add(available)

    pool = list(DEFAULT_NAMES if candidates is None else candidates)
    unique = []
    for name in pool:
        if name not in unique and _IDENT.fullmatch(name):
            bucket(name)
            unique.append(name)
    if len(unique) < len(requested):
        raise ValueError("not enough candidate identifiers for requested register assignments")

    options = []
    name_index = {name: i for i, name in enumerate(unique)}
    costs = {}
    for index, row in enumerate(requested):
        key, label = str(row["key"]), str(row["name"])
        allowed = (allowed_by_local or {}).get(key, (allowed_by_local or {}).get(label, unique))
        if isinstance(allowed, str):
            allowed = [allowed]
        values = [name for name in unique if name in set(allowed)]
        if not values:
            raise ValueError(f"no candidate identifier is allowed for {label!r}")
        options.append(values)
        for name in values:
            similarity = difflib.SequenceMatcher(None, label.lower(), name.lower()).ratio()
            costs[index, name] = (1.0 - similarity) * 20 + name_index[name] * 0.01

    @lru_cache(None)
    def choose(position, used_mask):
        if position == len(requested):
            return 0.0, ()
        best = None
        for name in options[position]:
            bit = 1 << name_index[name]
            if used_mask & bit:
                continue
            tail = choose(position + 1, used_mask | bit)
            candidate = (costs[position, name] + tail[0], (name,) + tail[1])
            if best is None or candidate < best:
                best = candidate
        return best

    score, names = choose(0, 0) if requested else (0.0, ())
    assignments = []
    for row, name in zip(requested, names):
        assignments.append({"key": str(row["key"]), "local": str(row["name"]),
                            "identifier": name, "type": row["type"],
                            "register": row["register"].upper(), "bucket": bucket(name)})
    declaration_order = [row["key"] for row in assignments]
    home_rows = sorted(assignments, key=lambda row: (row["bucket"],
                                                     -declaration_order.index(row["key"])))
    return {"assignments": assignments, "declaration_order": declaration_order,
            "bp_home_order": [row["key"] for row in home_rows], "score": score,
            "occupied_registers": [reg for reg in REGISTER_SEQUENCE if reg in occupied],
            "note": "Declare SI-assigned locals before DI-assigned locals within one active scope; names choose BP-home buckets, not register identity."}


def locals_from_source(source: str, function: str) -> list[dict]:
    """Read simple local declarations from one C function for quick CLI probes.

    Simple register parameters and local declarators are included. Aggregate
    sizes and complex declarators should use the JSON input form.
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
    rows = []
    header = match.group(0)
    params = header[header.find('(') + 1:header.rfind(')')]
    if params.strip() and params.strip() != 'void':
        parameter_pattern = re.compile(
            r'^\s*(register\s+)?(.+?)\s+(\*+\s*)?([A-Za-z_]\w*)'
            r'(\s*\[\s*\d+\s*\])?\s*$')
        for parameter in params.split(','):
            found_param = parameter_pattern.match(parameter)
            if not found_param:
                continue
            param_name = found_param.group(4)
            rows.append({'name': param_name,
                         'type': found_param.group(2).strip() + ' '
                                 + (found_param.group(3) or '').replace(' ', '')
                                 + (found_param.group(5) or ''),
                         'storage': 'parameter', 'register': bool(found_param.group(1)),
                         'block': [], 'initialized': False,
                         'referenced': len(re.findall(r'\b' + re.escape(param_name) + r'\b', body)) > 0})
    declaration = re.compile(
        r'^\s*(?:(register|static)\s+)?'
        r'((?:(?:unsigned|signed|short|long|char|int|void|far|near|const)\s+)+'
        r'|(?:struct|union|enum)\s+[A-Za-z_]\w*(?:\s+(?:far|near|const))*)\s*'
        r'(.+)$', re.S)
    scope = []
    scope_number = 0
    statement = ''
    for char in body + ';':
        if char in '{};':
            found = declaration.match(statement)
            if found:
                for declarator in found.group(3).split(','):
                    initialized = '=' in declarator
                    declarator = declarator.split('=', 1)[0].strip()
                    if not declarator:
                        continue
                    name_match = re.fullmatch(r'(\*+\s*)?([A-Za-z_]\w*)(\s*\[\s*\d+\s*\])?', declarator)
                    if not name_match:
                        raise ValueError('Complex declarator requires JSON local specification: ' + declarator)
                    name = name_match.group(2)
                    rows.append({'name': name,
                                 'type': found.group(2).strip() + ' '
                                         + ('*' if name_match.group(1) else '')
                                         + (name_match.group(3) or ''),
                                 'storage': found.group(1) or 'auto', 'block': list(scope),
                                 'initialized': initialized,
                                 'referenced': len(re.findall(r'\b' + re.escape(name) + r'\b', body)) > 1})
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
    parser.add_argument("--registers", nargs="+",
                        help="Suggest SI/DI locals, e.g. si:index di:source (bare labels map to SI then DI)")
    parser.add_argument("--occupied-registers", nargs="*", choices=REGISTER_SEQUENCE,
                        help="Already active register locals from enclosing scopes")
    parser.add_argument("--other-register-uses", nargs="*", choices=REGISTER_SEQUENCE,
                        help="Known SI/DI uses from generated code outside explicit register locals")
    args = parser.parse_args()
    if args.registers is not None:
        assignments = {}
        bare_labels = []
        for token in args.registers:
            if ':' in token:
                reg, label = token.split(':', 1)
                assignments[reg] = label
            else:
                bare_labels.append(token)
        occupied = {str(reg).lower() for reg in (args.occupied_registers or [])}
        for label in bare_labels:
            reg = next((item for item in REGISTER_SEQUENCE
                        if item not in assignments and item not in occupied), None)
            if reg is None:
                parser.error("no free register for bare label; use explicit SI/DI assignment syntax")
            assignments[reg] = label
        print(json.dumps(suggest_registers(assignments, args.candidates,
                                           occupied_registers=args.occupied_registers), indent=2))
    elif args.names is not None:
        print(json.dumps(suggest_names(args.names, args.candidates), indent=2))
    else:
        if args.source:
            if not args.function:
                parser.error("--source requires --function")
            locals_ = locals_from_source(args.source.read_text(encoding="latin1"), args.function)
            other_register_uses = args.other_register_uses or []
        else:
            if args.input is None:
                parser.error("provide a local-spec JSON file, --source/--function, or --names")
            value = json.loads(Path(args.input).read_text(encoding="utf-8"))
            locals_ = value.get("locals", value) if isinstance(value, dict) else value
            other_register_uses = args.other_register_uses or (
                value.get("other_register_uses", []) if isinstance(value, dict) else [])
        print(json.dumps(predict(locals_, other_register_uses=other_register_uses), indent=2))


if __name__ == "__main__":
    main()
