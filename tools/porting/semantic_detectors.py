#!/usr/bin/env python3
"""Stunts-specific source risks and bounded caller/global span detectors.

Unknown types/provenance remain candidates. Registered contracts also inspect
fresh production overlays; their oracle tests run separately in validation.
"""
from __future__ import annotations

import argparse
import ast
from collections import Counter, defaultdict
import hashlib
from bisect import bisect_right
import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "porting"))
sys.path.insert(0, str(ROOT / "port"))
import cdecls  # noqa: E402
import host_probe_declarations as declarations  # noqa: E402
import host_probe_modes as modes  # noqa: E402
from semantic_bounds import pointer_max_index_scoped, scope_env_at


AUDIT_CLASSES = ("width-signedness", "alias-layout", "service-results",
                 "initial-state", "msc-runtime", "added-glue")


def class_source_signals(text):
    """Candidate sites only; never use these lexical matches as a verdict."""
    code = modes.mask_comments(text)
    patterns = {
        'width-signedness': r'>>|<<|[!=<>]=|\b(?:int|long|char|int8_t|uint8_t|int16_t|uint16_t)\b',
        'alias-layout': r'\[[^\]\n]+\]|\b(?:memcpy|memmove|offsetof|sizeof)\b|&\s*[A-Za-z_]',
        'service-results': r'\b(?:int86|errno|cflag|ferror|fclose|fflush|fread|fwrite|port_fs_\w+|port_audio_driver_\w+)\b',
        'initial-state': r'\b\w*(?:init|reset|startup|palette|mouse|keyboard|environment|handle|directory)\w*\b',
        'msc-runtime': r'\b(?:rand|srand|strcmp|stricmp|strcmpi|strcpy|strcat|strlen|strrchr|itoa|ultoa|sprintf|printf|malloc|calloc|realloc|free|fopen|fseek|ftell|fread|fwrite|fflush|fclose|ctype|port_msc_ctype)\b',
        'added-glue': r'\b[A-Za-z_]\w*\s*\(',
    }
    return {cls: [dict(offset=m.start(), snippet=short_snippet(
        text[max(0, text.rfind('\n', 0, m.start()) + 1):
             text.find('\n', m.end()) if text.find('\n', m.end()) >= 0 else len(text)]))
        for m in re.finditer(pattern, code, re.I)] for cls, pattern in patterns.items()}


def machine_signals(image, start, end):
    """Lossless *linear* instruction evidence, never a CFG/equivalence claim.

    Preserve undecoded tails explicitly. Jump tables/embedded data can decode
    as instructions, so callers must confirm any signal before fixing code.
    """
    from capstone import Cs, CS_ARCH_X86, CS_MODE_16
    from capstone.x86_const import X86_OP_MEM
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    decoder.detail = True
    signals = {key: [] for key in AUDIT_CLASSES}
    decoded = start
    instructions = list(decoder.disasm(image[start:end], start))
    for index, ins in enumerate(instructions):
        decoded = ins.address + ins.size
        fact = dict(address=ins.address, hex=ins.bytes.hex(),
                    instruction=ins.mnemonic + " " + ins.op_str,
                    operand_bytes=[o.size for o in ins.operands])
        if ins.mnemonic in {"cmp", "test", "xor", "and", "or", "cbw", "cwd", "sar", "shr", "shl",
                            "sal", "rcr", "rcl", "div", "idiv", "imul", "mul",
                            "adc", "sbb", "neg", "add", "sub", "inc", "dec"} or (
                ins.mnemonic.startswith('j') and ins.mnemonic != 'jmp'):
            signals['width-signedness'].append(fact)
        memory = [o for o in ins.operands if o.type == X86_OP_MEM]
        if memory:
            signals['alias-layout'].append(dict(fact, memory=[dict(
                segment=ins.reg_name(o.mem.segment) or 'implicit/flow-dependent',
                base=ins.reg_name(o.mem.base), index=ins.reg_name(o.mem.index),
                displacement=o.mem.disp, bytes=o.size, access=o.access) for o in memory]))
        if ins.mnemonic in {'int', 'in', 'out', 'insb', 'insw', 'outsb', 'outsw', 'iret'}:
            window = instructions[index + 1:index + 9]
            signals['service-results'].append(dict(fact, following=[dict(
                address=x.address, instruction=x.mnemonic + ' ' + x.op_str)
                for x in window], limit='linear successor window; register/flag liveness across branches and calls requires review'))
            signals['initial-state'].append(fact)
        if ins.mnemonic in {'call', 'lcall'}:
            signals['added-glue'].append(fact)
    return dict(start=start, end=end, decoded_end=decoded,
                undecoded_tail_hex=image[decoded:end].hex(),
                sha256=hashlib.sha256(image[start:end]).hexdigest(),
                instruction_count=len(instructions), signals=signals,
                authority='locked image; linear disassembly, reachability unproved')


def six_class_audit(root, layer, semantic, overlays=(), image=None):
    """Screen every inventory row in priority order; keep unresolved obligations.

    This extends the existing semantic risk scan with original operands and
    public/member placement. A no-signal result is never a semantic pass.
    Reports are scratch artifacts, not acceptance records.
    """
    from semantic_mapping import public_extent, port_correspondence, int86_call_sites
    from semantic_audit import functions as portable_functions
    from diffharness.oracle import OracleImage, DGROUP_PARAGRAPH
    image = image if image is not None else OracleImage.load().load_image
    root = Path(root)
    manifest = read_json(root / 'layout/manifest.json')
    owners = {r['id']: r for r in manifest['owners']}
    correspondence = port_correspondence(root, layer, semantic, overlays)
    asm = read_json(root / 'docs/porting/asm-migration.json')['routines']
    asm_by_id = {'asm:' + r['owner_id'] + '::' + r['public_symbol']: r for r in asm}
    platform = read_json(root / 'docs/porting/platform-boundary.json')['entries']
    host = {'host:' + r['function'] + '@' + str(r.get('image_load_offset')): r for r in platform}
    historical = semantic['historical_mapping']
    by_address = {r['address']: r for r in historical if r['address'] is not None}
    by_name = defaultdict(list)
    for r in historical:
        for name in {r.get('original_name'), r.get('canonical_name'), r.get('registry_inventory_name')} - {None}:
            by_name[name].append(r)
    source_text, source_functions = {}, {}
    overlay_paths = {r['source']: Path(r['overlay']) for r in overlays}
    cache, entries = {}, []
    data_symbols = read_json(root / 'layout/data-symbols.json')['symbols']
    # These are layout facts only. Extents of unsized labels stay unknown.
    at_address = defaultdict(list)
    for name, spec in data_symbols.items():
        if isinstance(spec.get('load_address'), int):
            at_address[spec['load_address']].append(name)
    alias_groups = [dict(address=a, names=sorted(names)) for a, names in sorted(at_address.items()) if len(names) > 1]
    data_addresses = sorted(at_address)
    runtime_owners = [r for r in manifest['owners'] if r.get('classification') == 'RUNTIME_LIBRARY']

    def enrich_machine(facts):
        for owner in runtime_owners:
            if owner['start'] <= facts['start'] < owner['end']:
                facts['signals']['msc-runtime'].append(dict(owner=owner['id'],
                    start=owner['start'], end=owner['end'],
                    basis='accepted pinned MSC member placement; compare caller observables separately'))
        for fact in facts['signals']['alias-layout']:
            for operand in fact['memory']:
                # Code-island and segment-register data flow are deliberately
                # not guessed from a numeric displacement alone.
                if operand['segment'] not in {'ds', 'implicit/flow-dependent'} or operand['base'] == 'bp':
                    continue
                address = (DGROUP_PARAGRAPH << 4) + (operand['displacement'] & 0xffff)
                at = bisect_right(data_addresses, address) - 1
                candidates = []
                for anchor in data_addresses[max(0, at):at + 2]:
                    for name in at_address[anchor]:
                        spec = data_symbols[name]
                        candidates.append(dict(name=name, address=anchor,
                            width=spec.get('width'), storage=spec.get('storage'),
                            displacement_from_symbol=address - anchor))
                operand['dgroup_candidates'] = candidates
                operand['binding_limit'] = 'DGROUP hypothesis only; prove DS and indexed range at this site before treating a neighbour as an alias'
                facts['signals']['initial-state'].append(dict(address=fact['address'],
                    instruction=fact['instruction'], memory_operand=operand,
                    limit='possible initial-state access; read-before-write and hardware provenance unproved'))

    def priority(row):
        route = ' '.join([row['name'], row.get('category', ''), row.get('subsystem', '')] +
                         [p['source'] for p in row.get('portable', [])]).lower()
        phase = (0 if row.get('execution_coverage', {}).get('classification') == 'never_executed'
                 else 1 if row['kind'] == 'host' else 2 if 'library_' in row['id']
                 else 3 if any(s in route for s in ('audio', 'speaker', 'opl'))
                 else 4 if any(s in route for s in ('sprite', 'render', 'video', 'font', 'graphics')) else 5)
        return phase, row['id']

    for ordinal, row in enumerate(sorted(layer['rows'], key=priority)):
        extents, notes = [], []
        original_data = None
        if row['kind'] == 'semantic_data' and isinstance(row.get('original_address'), int):
            a, size = row['original_address'], row.get('bytes')
            if not isinstance(size, int) or not 0 <= a < a + size <= len(image):
                raise ValueError(f"invalid original data interval {row['id']}")
            original_data = dict(address=a, bytes=size, hex=image[a:a + size].hex(),
                                 authority='locked image data; not decoded as instructions')
        if row['id'] in asm_by_id:
            extent = public_extent(asm_by_id[row['id']], owners)
            if extent:
                extents.append(extent)
        elif row['id'] in host:
            h = host[row['id']]
            if h.get('image_load_offset') and h.get('image_end_exclusive'):
                extents.append(dict(start=int(h['image_load_offset'], 0),
                    end=int(h['image_end_exclusive'], 0), basis='platform-boundary reviewed interval'))
        else:
            candidates = []
            if isinstance(row.get('original_address'), int):
                candidates += [by_address[row['original_address']]] if row['original_address'] in by_address else []
            for name in set([row['name']] + row.get('routines', []) +
                            [p['name'] for p in row.get('portable', [])]):
                candidates.extend(by_name.get(name, []))
            for h in candidates:
                a, b = h['address'], h['end']
                if isinstance(a, int) and isinstance(b, int) and a < b:
                    extent = dict(start=a, end=b, basis='semantic_mapping exact name/address candidate; join is not equivalence')
                    if not any(x['start'] == a and x['end'] == b for x in extents):
                        extents.append(extent)
        mapping = correspondence[row['id']]
        if mapping['classification'] == 'host-only':
            extents = []
            original_data = None
        for target in mapping['targets']:
            if target.get('data'):
                a, size = target['start'], target['end'] - target['start']
                original_data = dict(address=a, bytes=size, hex=image[a:a + size].hex(),
                                     authority=target['basis'])
            elif not any(x['start'] == target['start'] and x['end'] == target['end'] and
                         x.get('asset') == target.get('asset') for x in extents):
                extents.append(target)
        evidence_keys = []
        for ex in extents:
            a, b = ex['start'], ex['end']
            original = (root / ex['asset']).read_bytes() if ex.get('asset') else image
            if not 0 <= a < b <= len(original):
                raise ValueError(f"invalid original interval {row['id']}: {ex}")
            key = (ex.get('asset', '') + ':' if ex.get('asset') else '') + f'{a:x}:{b:x}'
            if key not in cache:
                cache[key] = machine_signals(original, a, b)
                if ex.get('asset'):
                    cache[key]['asset'] = ex['asset']
                    cache[key]['asset_sha256'] = hashlib.sha256(original).hexdigest()
                else:
                    enrich_machine(cache[key])
            evidence_keys.append(dict(key=key, basis=ex['basis']))
        portable, risks = [], []
        source_signals = {c: [] for c in AUDIT_CLASSES}
        for p in row.get('portable', []):
            source = p['source']
            if source not in source_text:
                path = overlay_paths.get(source, root / source)
                source_text[source] = path.read_text(encoding='latin-1')
                source_functions[source] = ({} if path.suffix == '.py' else portable_functions(source_text[source]))
            fn = source_functions[source].get(p['name'])
            text = fn['body'] if fn else row.get('expression', row.get('operation', {}).get('expression', ''))
            if not text:
                notes.append(f"{source}::{p['name']}: no isolated body (macro/data/Python adapter or unresolved join)")
            portable.append(dict(source=source, name=p['name'], signature=p.get('signature'),
                body_sha256=hashlib.sha256(text.encode('latin-1')).hexdigest(),
                source_sha256=hashlib.sha256(source_text[source].encode('latin-1')).hexdigest()))
            risks.extend(scan_function(source, p['name'], text, p.get('line', 1)))
            for cls, signals in class_source_signals(text).items():
                source_signals[cls].extend(dict(s, source=source, function=p['name']) for s in signals)
        checks = {}
        for cls in AUDIT_CLASSES:
            count = sum(len(cache[e['key']]['signals'][cls]) for e in evidence_keys)
            checks[cls] = dict(original_signal_count=count,
                portable_signals=source_signals[cls],
                status='requires-review' if evidence_keys or original_data else 'original-unmapped',
                obligation={
                    'width-signedness': 'Compare operand widths, signed branches, extensions, wrap, shift/divide with host types and promotions.',
                    'alias-layout': 'Resolve effective segment and indexed spans against link/data map, including stack-local aliases; compare host storage and write order.',
                    'service-results': 'Trace each service result register/flag/buffer into consumers across branches/callees, including failures.',
                    'initial-state': 'Establish read-before-write paths from DOS/BIOS/driver startup to first consumer, including PSP/environment/handles/CWD.',
                    'msc-runtime': 'Join pinned runtime public and caller uses to host CRT/adapters; compare defined input domains and failures.',
                    'added-glue': 'Compare native call/state order and generated rewrites with original call graph and observed transitions.',
                }[cls])
            if mapping['classification'] == 'host-only':
                checks[cls]['status'] = 'host-only/state-order-review' if cls in (
                    'initial-state', 'added-glue') else 'host-only/no-direct-original'
                checks[cls]['review'] = mapping['class4' if cls == 'initial-state' else 'class6']
        entries.append(dict(id=row['id'], ordinal=ordinal, phase=priority(row)[0],
            kind=row['kind'], execution_coverage=row.get('execution_coverage'),
            original=evidence_keys, original_data=original_data, portable=portable, source_risks=risks,
            correspondence=mapping,
            checks=checks, notes=notes,
            limit='Screened only; neither absence of a linear signal nor a passing scenario closes these obligations.'))
    return dict(schema='stunts-port-six-class-screen-v1',
        status='SCREENED; semantic review incomplete',
        inputs={p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in (
            'tools/porting/semantic_audit.py', 'tools/porting/semantic_detectors.py',
            'tools/porting/semantic_mapping.py', 'tools/porting/semantic_bounds.py',
            'docs/porting/asm-migration.json', 'docs/porting/platform-boundary.json',
            'layout/manifest.json', 'layout/data-symbols.json', 'layout/names-registry.json')},
        oracle_lock_sha256=hashlib.sha256((root / 'layout/oracle.lock.json').read_bytes()).hexdigest(),
        int86_calls=int86_call_sites(root),
        summary=dict(rows=len(entries), never_executed_first=sum(r['phase'] == 0 for r in entries),
            per_class={c: dict(Counter(r['checks'][c]['status'] for r in entries)) for c in AUDIT_CLASSES},
            correspondence=dict(Counter(r['correspondence']['classification'] for r in entries)),
            original_intervals=len(cache), original_mapped_rows=sum(bool(r['original'] or r['original_data']) for r in entries),
            intervals_with_undecoded_tail=sum(bool(r['undecoded_tail_hex']) for r in cache.values())),
        layout_alias_groups=alias_groups, machine_evidence=cache, rows=entries)


def read_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def short_snippet(text: str, limit: int = 220) -> str:
    text = " ".join(text.strip().split())
    return text if len(text) <= limit else text[:limit - 3] + "..."


def line_at(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def macros_in(text: str) -> dict[str, int]:
    found = {}
    for match in re.finditer(r"(?m)^\s*#\s*define\s+([A-Za-z_]\w*)\s+([^\s/]+)", text):
        token = re.sub(r"[uUlL]+$", "", match.group(2))
        try:
            found[match.group(1)] = int(token, 0)
        except ValueError:
            continue
    return found


def eval_max(expression: str, env: dict[str, int], macros: dict[str, int]):
    """Evaluate the small positive integer expression subset used in loop bounds."""
    expression = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", expression)
    expression = re.sub(r"\(\s*(?:unsigned\s+)?(?:char|short|int|long|I8S?|U8|I16S?|U16S?|I32|U32)\s*\)", "", expression)
    expression = expression.strip()
    try:
        node = ast.parse(expression, mode="eval").body
    except SyntaxError:
        return None

    def visit(item):
        if isinstance(item, ast.Constant) and isinstance(item.value, int):
            return item.value
        if isinstance(item, ast.Name):
            return env.get(item.id, macros.get(item.id))
        if isinstance(item, ast.UnaryOp) and isinstance(item.op, (ast.UAdd, ast.USub)):
            value = visit(item.operand)
            if value is None:
                return None
            return value if isinstance(item.op, ast.UAdd) else -value
        if isinstance(item, ast.BinOp):
            left, right = visit(item.left), visit(item.right)
            if left is None or right is None:
                return None
            try:
                if isinstance(item.op, ast.Add): return left + right
                if isinstance(item.op, ast.Sub): return left - right
                if isinstance(item.op, ast.Mult): return left * right
                if isinstance(item.op, (ast.Div, ast.FloorDiv)) and right: return left // right
                if isinstance(item.op, ast.LShift) and right >= 0: return left << right
                if isinstance(item.op, ast.RShift) and right >= 0: return left >> right
            except (ArithmeticError, OverflowError):
                return None
        return None

    return visit(node)


def function_rows(path: str, text: str):
    code = modes.mask_comments(text)
    unit = cdecls.Unit(declarations.source_parser_text(text), path)
    parsed = {}
    for name, decls in unit.decls.items():
        for row in decls:
            if row.get("defined") and row["type"].get("k") == "fn":
                parsed[name] = row["type"]
    rows = []
    for start, end, kind in modes.top_level_spans(text):
        if kind != "function":
            continue
        brace = code.find("{", start, end)
        if brace < 0:
            continue
        signature = code[start:brace]
        match = re.search(r"([A-Za-z_]\w*)\s*\([^;{}]*\)\s*$", signature, re.S)
        if not match:
            continue
        name = match.group(1)
        rows.append({"name": name, "start": start, "brace": brace, "end": end,
                     "line": line_at(text, start), "signature": text[start:brace],
                     "body": text[brace + 1:end - 1], "code_body": code[brace + 1:end - 1],
                     "type": parsed.get(name, {})})
    return rows


def source_globals(path: str, text: str):
    unit = cdecls.Unit(declarations.source_parser_text(text), path)
    out = {}
    for name, decls in unit.decls.items():
        for row in decls:
            typ = row.get("type", {})
            if row.get("defined") and typ.get("k") != "fn":
                out[name] = typ
    return out


def base_bytes(typ: dict, macros: dict[str, int], aggregate_bytes: dict[str, int]):
    kind = typ.get("k")
    if kind == "arr":
        count = typ.get("n")
        size = base_bytes(typ.get("of", {}), macros, aggregate_bytes)
        return count * size if isinstance(count, int) and size is not None else None
    if kind == "ptr":
        return None
    if kind == "base":
        name = typ.get("name")
        return {"char": 1, "short": 2, "int": 2, "long": 4}.get(name)
    if kind == "struct":
        return aggregate_bytes.get(typ.get("tag"))
    return None


def call_arguments(code: str, open_paren: int):
    depth = 1
    paren = bracket = brace = 0
    start = open_paren + 1
    args = []
    i = start
    while i < len(code):
        char = code[i]
        if char == "(": depth += 1; paren += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                tail = code[start:i].strip()
                if tail: args.append(tail)
                return args, i + 1
            if paren: paren -= 1
        elif char == "[": bracket += 1
        elif char == "]": bracket = max(0, bracket - 1)
        elif char == "{": brace += 1
        elif char == "}": brace = max(0, brace - 1)
        elif char == "," and depth == 1 and bracket == 0 and brace == 0:
            args.append(code[start:i].strip())
            start = i + 1
        i += 1
    return [], len(code)


def pointer_max_index(code_body: str, param: str, macros: dict[str, int]):
    return pointer_max_index_scoped(code_body, param, macros, eval_max)


def pointer_index_expressions(code_body: str, param: str):
    return [match.group(1).strip() for match in re.finditer(
        r"\b" + re.escape(param) + r"\s*\[([^\]]+)\]", code_body)]


def direct_global_binding(argument: str, globals_: dict[str, dict], macros: dict[str, int]):
    cleaned = re.sub(r"\([^()]*\)", " ", argument)
    candidates = [name for name in globals_ if re.search(r"\b" + re.escape(name) + r"\b", cleaned)]
    if not candidates:
        return None, None
    name = max(candidates, key=len)
    indexed = re.search(r"\b" + re.escape(name) + r"\s*\[([^\]]+)\]", cleaned)
    offset = eval_max(indexed.group(1), {}, macros) if indexed else 0
    return name, offset


def direct_global_name(argument: str, globals_: dict[str, dict]):
    return direct_global_binding(argument, globals_, {})[0]


def call_sites(text: str, funcs: list[dict]):
    code = modes.mask_comments(text)
    rows = []
    for caller in funcs:
        body_start, body_end = caller["brace"] + 1, caller["end"] - 1
        body = code[body_start:body_end]
        for match in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", body):
            name = match.group(1)
            if name in {"if", "for", "while", "switch", "sizeof", "return"}:
                continue
            absolute = body_start + match.start()
            args, after = call_arguments(code, code.find("(", absolute))
            rows.append({"name": name, "args": args, "offset": absolute,
                         "line": line_at(text, absolute), "caller": caller["name"],
                         "snippet": short_snippet(text[text.rfind("\n", 0, absolute) + 1:
                                                       (text.find("\n", absolute) if text.find("\n", absolute) >= 0 else len(text))])})
    return rows


def generalized_span_candidates(parsed_by_source: dict):
    """Report interprocedural indexed accesses beyond named caller objects.

    This deliberately conservative subset only claims a candidate. It uses
    parsed array declarations and simple literal loop/offset bounds; complex
    pointer provenance, aggregate sizes, and aliasing remain explicitly unknown.
    """
    callee_defs = {}
    macros_by_source = {}
    for source, item in parsed_by_source.items():
        macros_by_source[source] = item.get("macros", macros_in(item["text"]))
        for function in item["functions"]:
            if function["type"].get("k") == "fn":
                callee_defs.setdefault(function["name"], []).append((source, function))

    access_specs = {}
    aggregate_bytes = {"VECTOR": 6, "POINT2D": 4, "MATRIX": 18}
    for name, definitions in callee_defs.items():
        # Only active definitions have body-derived access facts; ambiguous
        # duplicate definitions stay candidates with their source attached.
        for source, function in definitions:
            params = function["type"].get("params", [])
            names = function["type"].get("pnames") or []
            if params is None:
                continue
            if len(params) != len(names):
                continue
            for index, (param_type, param_name) in enumerate(zip(params, names)):
                if not param_name or param_type.get("k") != "ptr":
                    continue
                expressions = pointer_index_expressions(function["code_body"], param_name)
                if not expressions:
                    continue
                maximum = pointer_max_index(function["code_body"], param_name,
                                            macros_by_source[source])
                element_bytes = base_bytes(param_type.get("to", {}),
                                           macros_by_source[source], aggregate_bytes)
                access_specs.setdefault(name, []).append({
                    "source": source, "function": function, "parameter_index": index,
                    "parameter": param_name, "max_index": maximum,
                    "index_expressions": expressions, "element_bytes": element_bytes,
                    "required_bytes": ((maximum + 1) * element_bytes
                                       if maximum is not None and element_bytes is not None else None),
                })

    results = []
    for caller_source, item in parsed_by_source.items():
        globals_ = item["globals"]
        macros = macros_by_source[caller_source]
        for call in call_sites(item["text"], item["functions"]):
            for access in access_specs.get(call["name"], []):
                pindex = access["parameter_index"]
                if pindex >= len(call["args"]):
                    continue
                argument = call["args"][pindex]
                global_name, element_offset = direct_global_binding(argument, globals_, macros)
                if not global_name:
                    continue
                object_bytes = base_bytes(globals_[global_name], macros,
                                          aggregate_bytes)
                if object_bytes is None or access["required_bytes"] is None:
                    results.append({
                        "class": "caller_pointer_span_extent_unknown",
                        "source": caller_source,
                        "function": call["caller"],
                        "callee": call["name"],
                        "line": call["line"],
                        "snippet": call["snippet"],
                        "argument_global": global_name,
                        "callee_parameter": access["parameter"],
                        "callee_source": access["source"],
                        "callee_index_expressions": access["index_expressions"],
                        "evidence_kind": "parsed_interprocedural_candidate",
                        "status": "candidate_extent_unresolved",
                    })
                    continue
                if element_offset is None:
                    results.append({
                        "class": "caller_pointer_span_offset_unknown",
                        "source": caller_source, "function": call["caller"],
                        "callee": call["name"], "line": call["line"],
                        "snippet": call["snippet"], "argument_global": global_name,
                        "callee_parameter": access["parameter"],
                        "required_bytes": access["required_bytes"],
                        "object_bytes": object_bytes,
                        "evidence_kind": "parsed_interprocedural_candidate",
                        "status": "candidate_offset_unresolved",
                    })
                    continue
                obj_type = globals_[global_name]
                one_element_bytes = (base_bytes(obj_type.get("of", {}), macros, aggregate_bytes)
                                     if obj_type.get("k") == "arr" else object_bytes)
                remaining = object_bytes - (element_offset * (one_element_bytes or 0))
                if access["required_bytes"] > remaining:
                    linked_contract = None
                    if call["name"] == "settires" and access["parameter"] in {"source", "origin"}:
                        linked_contract = "wheel_update.cross_global_inputs"
                    results.append({
                        "class": "caller_pointer_span_exceeds_named_extent",
                        "source": caller_source,
                        "function": call["caller"],
                        "callee": call["name"],
                        "line": call["line"],
                        "snippet": call["snippet"],
                        "argument_global": global_name,
                        "argument_offset_elements": element_offset,
                        "remaining_object_bytes": remaining,
                        "callee_parameter": access["parameter"],
                        "callee_source": access["source"],
                        "callee_index_expressions": access["index_expressions"],
                        "inferred_max_index": access["max_index"],
                        "required_bytes": access["required_bytes"],
                        "evidence_kind": "parsed_interprocedural_candidate",
                        "status": "heuristic_candidate",
                        "registered_contract": linked_contract,
                        "limitations": ["read versus write is not classified",
                                        "simple bound inference only",
                                        "aggregate element size is known only for registered common views"],
                    })

        # Direct named-global accesses with a resolved index past their parsed
        # array extent include the polygon queue-head pattern.
        for function in item["functions"]:
            for global_name, typ in globals_.items():
                if typ.get("k") != "arr" or not isinstance(typ.get("n"), int):
                    continue
                for match in re.finditer(r"\b" + re.escape(global_name) + r"\s*\[([^\]]+)\]",
                                         function["code_body"]):
                    index_expr = match.group(1).strip()
                    absolute = function["brace"] + 1 + match.start()
                    line_start = item["text"].rfind("\n", 0, absolute) + 1
                    line_end = item["text"].find("\n", absolute)
                    if line_end < 0: line_end = len(item["text"])
                    line_text = item["text"][line_start:line_end]
                    if re.match(r"\s*(?:extern|static|register|const)?\s*(?:struct\s+\w+|union\s+\w+|[A-Za-z_]\w*(?:\s*\*)*)\s+"
                                + re.escape(global_name) + r"\s*\[", line_text):
                        continue
                    # Use only preceding source text so a later dynamic write
                    # cannot erase or inflate this access's inferred index.
                    env = scope_env_at(function["code_body"], match.start(), macros, eval_max)
                    index = eval_max(index_expr, env, macros)
                    if index is None:
                        if re.search(r"\+\s*1\b", index_expr):
                            results.append({"class": "named_global_subscript_bound_unresolved",
                                "source": caller_source, "function": function["name"],
                                "line": line_at(item["text"], absolute),
                                "snippet": short_snippet(line_text), "global": global_name,
                                "index_expression": index_expr, "declared_elements": typ["n"],
                                "status": "candidate_bound_unresolved",
                                "evidence_kind": "parsed_local_access_candidate"})
                        continue
                    if index < typ["n"]:
                        continue
                    contract = "polyinfo.one_past_queue_head" if global_name == "poly_link_list" else None
                    results.append({
                        "class": "named_global_subscript_exceeds_extent",
                        "source": caller_source,
                        "function": function["name"],
                        "line": line_at(item["text"], absolute),
                        "snippet": short_snippet(item["text"][line_start:line_end]),
                        "global": global_name,
                        "declared_elements": typ["n"],
                        "index_expression": index_expr,
                        "inferred_index": index,
                        "evidence_kind": "parsed_local_access_candidate",
                        "status": "heuristic_candidate",
                        "registered_contract": contract,
                        "limitations": ["index variable range is a limited constant-propagation result"],
                    })
    return results


def lexical_findings(source: str, text: str, funcs: list[dict]):
    rules = [
        ("signed_shift_or_word_shift", re.compile(r"(?:>>|<<)"), "shift width/signedness is compiler-sensitive"),
        ("word_sentinel_comparison", re.compile(r"(?:==|!=)\s*(?:0[xX]0*[fF]{4}|65535)[uUlL]*\b"), "word sentinel may promote differently on host"),
        ("scalar_narrowing_cast", re.compile(r"\(\s*(?:(?:unsigned|signed)\s+)?(?:int|long|I16S?|U16S?|I32|U32|int16_t|uint16_t|intptr_t|uintptr_t)\s*\)"), "explicit scalar width boundary; operand type still needs review"),
        ("unprototyped_call_cast", re.compile(r"\(\s*\*\s*\)\s*\(\s*\)"), "erased argument contract can hide ABI disagreement"),
        ("fractional_carry_translation", re.compile(r"\b(?:carry|borrow|[xy]_fraction_low|slope)\b"), "translated register carry/fixed-point boundary"),
        ("offset_or_overlay_alias", re.compile(r"\b[A-Za-z_]\w*(?:Offset|offset|dataPointer|cameraOffsetOverride|farptr|nearptr)\b", re.I), "numeric offset or overlay name may be mistaken for a host pointer"),
        ("target_width_type_spelling", re.compile(r"\b(?:unsigned\s+int|signed\s+int|unsigned\s+long|signed\s+long|int|long)\b"), "bare int/long width differs across the DOS and host models"),
    ]
    result = []
    for function in funcs:
        body = function["code_body"]
        base = function["brace"] + 1
        for class_name, pattern, rationale in rules:
            for match in pattern.finditer(body):
                absolute = base + match.start()
                source_line = text.count("\n", 0, absolute) + 1
                line_start = text.rfind("\n", 0, absolute) + 1
                line_end = text.find("\n", absolute)
                if line_end < 0: line_end = len(text)
                result.append({
                    "class": class_name,
                    "source": source,
                    "function": function["name"],
                    "line": source_line,
                    "snippet": short_snippet(text[line_start:line_end]),
                    "evidence_kind": "lexical_signal",
                    "status": "heuristic",
                    "rationale": rationale,
                })
    return result


def scan_function(source, name, text, first_line):
    code = modes.mask_comments(text)
    brace = code.find("{")
    row = {"name": name, "brace": brace, "code_body": code[brace + 1:]}
    findings = lexical_findings(source, text, [row])
    for item in findings:
        item["line"] += first_line - 1
    return findings


def scan_spans(root, rows):
    parsed = {}
    for row in rows:
        source = row["source"]
        text = (root / source).read_text(encoding="latin-1")
        macros = macros_in(modes.expand_project_headers(text, root))
        bounds = "\n".join(f"#define {name} {value}" for name, value in macros.items())
        parsed[source] = {"text": text, "functions": function_rows(source, text),
                          "globals": source_globals(source, bounds + "\n" + text),
                          "macros": macros}
    findings, statuses = build_contract_findings(
        root, read_json(root / "port/semantic-spans.json"), parsed,
        read_json(root / "layout/data-symbols.json"))
    overlays = {row["source"]: Path(row["overlay"]).read_text(encoding="latin-1") for row in rows}
    # Verify the built representation, rather than a Python adapter name.
    wheel = overlays["game/trkscene.c"]
    wheel_body = next(f["body"] for f in function_rows("game/trkscene.c", wheel)
                      if f["name"] == "settires")
    wheel_ok = all(token in wheel_body for token in (
        "port_wheel_source[24]", "source = port_wheel_source;", "origin = port_wheel_origin;",
        "secondveccar", "secondoveh", "car_dvecs", "veh_od", "ancv2", "g_op_carvector2"))
    polygon = overlays["game/polygon.c"]
    polygon_ok = bool(re.search(r"poly_link_list\[POLYINFO_CAPACITY\s*\+\s*1\]", polygon)
                      and re.search(r"#define\s+polyinfo_reset_marker\s+poly_link_list\[POLYINFO_CAPACITY\]", polygon))
    for item in findings:
        ok = wheel_ok if item["class"] == "cross_global_pointer_span" else polygon_ok
        if item["status"] == "contract_drift" or not ok:
            item["status"] = "fail"
            item["message"] = str(item["contract"]) + ": " + "; ".join(
                item.get("issues", []) + ([] if ok else ["fresh production span adapter missing"]))
    return findings + generalized_span_candidates(parsed)


def build_contract_findings(root: Path, contract_data: dict,
                            parsed_by_source: dict, symbols_data: dict):
    findings = []
    contract_status = []
    data_symbols = symbols_data["symbols"]
    vector_bytes = 6
    word_bytes = 2
    for contract in contract_data["contracts"]:
        source = contract["source"]
        text = parsed_by_source[source]["text"]
        code = modes.mask_comments(text)
        funcs = parsed_by_source[source]["functions"]
        globals_ = parsed_by_source[source]["globals"]
        macros = parsed_by_source[source].get("macros", macros_in(text))
        issues = []
        produced = []
        if contract["kind"] == "cross_global_pointer_span":
            function = next((row for row in funcs if row["name"] == contract["function"]), None)
            if function is None:
                issues.append("function definition missing")
            else:
                params = function["type"].get("pnames", [])
                reads = {}
                for name, spec in contract["argument_bindings"].items():
                    if name not in params:
                        issues.append(f"parameter {name} missing from parsed definition")
                        continue
                    max_index = pointer_max_index(function["code_body"], name, macros)
                    required_count = max_index + 1 if max_index is not None else None
                    expected_count = spec["elements_read"]
                    if required_count != expected_count:
                        issues.append(f"{name} inferred span {required_count!r} != registered {expected_count}")
                    reads[name] = {"max_index": max_index, "elements_read": required_count,
                                   "element_bytes": spec["element_bytes"],
                                   "required_bytes": (required_count * spec["element_bytes"]
                                                      if required_count is not None else None)}

                calls = []
                skip_headers = [(row["start"], row["brace"]) for row in funcs]
                skip_headers.extend((a, b) for a, b, kind in modes.top_level_spans(text)
                    if kind == "stmt" and re.search(r"\b" + re.escape(contract["function"]) +
                                                    r"\s*\(", code[a:b]))
                for match in re.finditer(r"\b" + re.escape(contract["function"]) + r"\s*\(", code):
                    if any(start <= match.start() < end for start, end in skip_headers):
                        continue
                    args, _ = call_arguments(code, code.find("(", match.start()))
                    calls.append((match.start(), args))
                expected_source_param = contract["argument_bindings"]["source"]["parameter_index"]
                expected_origin_param = contract["argument_bindings"]["origin"]["parameter_index"]
                for offset, args in calls:
                    if max(expected_source_param, expected_origin_param) >= len(args):
                        issues.append("call arity shorter than registered pointer parameters")
                        continue
                    call_line = line_at(text, offset)
                    line_start = text.rfind("\n", 0, offset) + 1
                    line_end = text.find("\n", offset)
                    if line_end < 0: line_end = len(text)
                    for param_key, arg_index, groups_key in (
                        ("source", expected_source_param, "target_groups"),
                        ("origin", expected_origin_param, "target_origin_pairs"),
                    ):
                        global_name = direct_global_name(args[arg_index], globals_)
                        if not global_name:
                            issues.append(f"call argument {param_key} is not bound to a parsed global")
                            continue
                        declared = base_bytes(globals_[global_name], macros,
                                              {"VECTOR": vector_bytes})
                        required = reads.get(param_key, {}).get("required_bytes")
                        if declared is None or required is None:
                            issues.append(f"could not size {global_name} or infer {param_key} span")
                            continue
                        grouped = next((row for row in contract[groups_key]
                                        if global_name in row), None)
                        if grouped is None:
                            issues.append(f"{global_name} is absent from registered {groups_key}")
                            continue
                        member_facts = []
                        for member in grouped:
                            typ = globals_.get(member)
                            byte_extent = base_bytes(typ, macros, {"VECTOR": vector_bytes}) if typ else None
                            frozen = data_symbols.get("_" + member)
                            if byte_extent is None or not frozen or "load_address" not in frozen:
                                issues.append(f"missing parsed extent/frozen binding for {member}")
                                continue
                            member_facts.append({"name": member, "load_address": frozen["load_address"],
                                                 "extent_bytes": byte_extent,
                                                 "extent_provenance": frozen.get("width_provenance", "source declaration")})
                        contiguous = bool(member_facts) and all(
                            right["load_address"] == left["load_address"] + left["extent_bytes"]
                            for left, right in zip(member_facts, member_facts[1:]))
                        target_total = sum(item["extent_bytes"] for item in member_facts)
                        if not contiguous:
                            issues.append(f"frozen members for {param_key} are not contiguous")
                        if target_total < required:
                            issues.append(f"frozen contiguous {param_key} span {target_total} < required {required}")
                        if declared >= required:
                            issues.append(f"{global_name} no longer demonstrates a cross-object span")
                        produced.append({
                            "class": "cross_global_pointer_span",
                            "source": source,
                            "function": contract["function"],
                            "callsite_line": call_line,
                            "snippet": short_snippet(text[line_start:line_end]),
                            "pointer_parameter": param_key,
                            "argument_global": global_name,
                            "declared_object_bytes": declared,
                            "required_bytes": required,
                            "target_group": member_facts,
                            "target_group_contiguous": contiguous,
                            "target_group_total_bytes": target_total,
                            "evidence_kind": "parsed_source_and_frozen_binding",
                            "status": "contract_covered",
                            "contract": contract["id"],
                            "adapter": contract["adapter"],
                            "behavioral_test": contract["behavioral_test"],
                        })
            if not any("adapt_wheel_update_inputs" in line for line in
                       (root / "port" / "build.py").read_text(encoding="utf-8").splitlines()):
                issues.append("production build no longer invokes wheel adapter")

        elif contract["kind"] == "global_one_past_sentinel":
            global_name = contract["global"]
            function = next((row for row in funcs if row["name"] == contract["function"]), None)
            typ = globals_.get(global_name)
            declared = typ.get("n") if typ and typ.get("k") == "arr" else None
            cap = macros.get(contract["capacity_macro"])
            global_offset = None
            marker_offset = data_symbols.get("_" + contract["marker_global"], {}).get("load_address")
            if contract["capacity_macro"] not in macros or cap != contract["sentinel_index"]:
                issues.append("capacity macro does not match registered sentinel index")
            if declared != contract["declared_elements"]:
                issues.append(f"parsed global extent {declared!r} != registered {contract['declared_elements']}")
            if not function:
                issues.append("sentinel-consuming function definition missing")
            else:
                body = function["code_body"]
                if not re.search(r"\blink\s*=\s*POLYINFO_CAPACITY\s*;", body):
                    issues.append("expected capacity-to-link assignment missing")
                if not re.search(r"\bpoly_link_list\s*\[\s*link\s*\]", body):
                    issues.append("expected array access through link missing")
            start = data_symbols.get("_" + global_name, {}).get("load_address")
            if start is None:
                # Module-private statics have no PUBDEF entry in data-symbols.
                # Their accepted compiler-bound allocation has an exact name
                # binding, not an extent inferred from the next symbol gap.
                bindings = read_json(root / "layout/names-registry.json")["names"]
                starts = [int(at) for at, binding in bindings.items()
                          if binding.get("name") == global_name and binding.get("kind") == "data"]
                if len(starts) == 1:
                    start = starts[0]
            # The array's exact target byte size is independently derivable from
            # the accepted source declaration and two-byte I16 elements.
            global_offset = start + contract["sentinel_index"] * word_bytes if start is not None else None
            matches_target_marker = global_offset is not None and global_offset == marker_offset
            if not matches_target_marker:
                issues.append("frozen sentinel address does not equal the named adjacent marker")
            adapter = (root / "port" / "game_abi.py").read_text(encoding="utf-8")
            adapted = bool(re.search(r"poly_link_list\[POLYINFO_CAPACITY\s*\+\s*1\]", adapter)
                           and re.search(r"#define\s+polyinfo_reset_marker\s+poly_link_list\[POLYINFO_CAPACITY\]", adapter))
            if not adapted:
                issues.append("registered host 401st-element/marker alias adapter is missing")
            produced.append({
                "class": "global_one_past_sentinel",
                "source": source,
                "function": contract["function"],
                "line": function["line"] if function else None,
                "snippet": short_snippet(function["body"]) if function else "",
                "global": global_name,
                "declared_elements": declared,
                "sentinel_index": contract["sentinel_index"],
                "sentinel_byte_address": global_offset,
                "marker_byte_address": marker_offset,
                "frozen_marker_adjacent": matches_target_marker,
                "host_elements": contract["host_elements"],
                "evidence_kind": "parsed_source_and_frozen_binding",
                "status": "contract_covered",
                "contract": contract["id"],
                "adapter": contract["adapter"],
                "behavioral_test": contract["behavioral_test"],
            })
        else:
            issues.append("unknown registered contract kind")

        for finding in produced:
            if issues:
                finding["status"] = "contract_drift"
                finding["issues"] = list(issues)
            findings.append(finding)
        if not produced:
            findings.append({"class": contract["kind"], "source": source,
                             "function": contract.get("function"),
                             "evidence_kind": "registered_contract",
                             "status": "contract_drift", "contract": contract["id"],
                             "issues": issues or ["contract had no observed source occurrence"]})
        contract_status.append({"id": contract["id"],
                                "status": "contract_drift" if issues else "contract_covered",
                                "issues": issues,
                                "behavioral_test": contract["behavioral_test"]})
    return findings, contract_status
