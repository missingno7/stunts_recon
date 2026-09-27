#!/usr/bin/env python3
"""Per-function mismatch classifier for candidate C sources (DIAGNOSTIC ONLY).

This tool NEVER grants ownership or acceptance.  It compiles a candidate with the
pinned MSC 5.10 compiler (canonical /AM /O /Gs, or the object's register flag set),
reuses tools/tubench.py for member extraction and reviewed-resolver binding, and
then classifies every selected member into exactly one primary state plus flags:

  ACCEPTED            member interval is owned by an accepted manifest owner
  RECORD_CLOSED_EXACT (hook) exact AND a prefix-proof file marks it closed
  BYTE_EXACT          all bytes equal, every fixup field bound by reviewed resolvers
                      (or by the object's own internal offsets); candidate CODE records
                      covering it contain only exact bytes (diagnostic record closure)
  CODEGEN_EXACT       bytes equal outside fixup fields, fixup shapes equal, every
                      unbound field independently consistent with the target
                      (same-named entry/alias at the target address), none disagrees
  EXACT_OPEN_RECORD   BYTE/CODEGEN exact, but a candidate CODE LEDATA record covering
                      it also holds non-exact or unselected bytes
  BLOCKED_TU_DATA     code exact; TU-owned _DATA/CONST/_BSS field(s) not placed
  BLOCKED_SYMBOL      code exact; external/internal symbol field(s) unknown/disagree
  PROFILE_UNCERTAIN   genuine difference, and object flag profile is PLAUSIBLE/
                      unresolved or the function carries an unresolved S-symptom
  CODEGEN_MISMATCH    genuine instruction/byte differences (sub-flags)
  BOUNDARY_UNCERTAIN  inventory extent unverified or conflicts with objmap
  NO_CANDIDATE        (repository mode) no candidate defines the member

Similarity, alignment and record closure here are diagnostic evidence only.
"""
from __future__ import annotations

import argparse
import bisect
import contextlib
import functools
import hashlib
import io
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import uuid
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[0]
TOOLS = ROOT / 'tools'
PERMUTER = HERE
for _p in (str(TOOLS), str(ROOT / 'build/python'), str(PERMUTER)):
    if _p not in sys.path:
        sys.path.insert(0, _p)

import tubench  # noqa: E402

WORK = ROOT / 'build/classifier/runs'
CACHE = ROOT / 'build/classifier/cache'


def _redirect_tubench():
    tubench.WORKSPACE = WORK
    tubench.MAP_PATH = CACHE / 'tumap.json'
    tubench.MAP_TABLE = CACHE / 'tumap.md'


_redirect_tubench()
import blockdiff  # noqa: E402  (imports permuter/pcore, which redirects tubench.WORKSPACE)
_redirect_tubench()
import diagnostics  # noqa: E402

AUTHORITY = ('DIAGNOSTIC_ONLY: classification never grants ownership, binding or acceptance; '
             'only tools/promote.py + tools/validate.py decide acceptance.')
CANONICAL_FLAGS = ['/AM', '/O', '/Gs']
PROFILE = 'msc510-medium'
ACCEPTED_KINDS = {'MATCHING_C', 'MATCHING_ASM', 'KNOWN_TOOLCHAIN_LIBRARY', 'MATCHING_C_DATA',
                  'MATCHING_ASM_DATA'}
STATES = ['ACCEPTED', 'RECORD_CLOSED_EXACT', 'BYTE_EXACT', 'CODEGEN_EXACT', 'EXACT_OPEN_RECORD',
          'BLOCKED_TU_DATA', 'BLOCKED_SYMBOL', 'PROFILE_UNCERTAIN', 'CODEGEN_MISMATCH',
          'BOUNDARY_UNCERTAIN', 'NO_CANDIDATE']
EXACT_FAMILY = {'RECORD_CLOSED_EXACT', 'BYTE_EXACT', 'CODEGEN_EXACT', 'EXACT_OPEN_RECORD'}
STATE_RANK = {s: i for i, s in enumerate(STATES)}
# Absolute publics whose value is fixed by a pinned runtime member (docs/acceptance.md).
ABSOLUTE_CONSTANTS = {'AHSHIFT': 12}
# Bump when classification semantics change (status_report cache key).
CLASSIFIER_VERSION = '1.2'

# Field categories.  OK = bound by reviewed resolver or internal self-mapping and equal.
FIELD_OK = {'BOUND', 'INTERNAL_CONSISTENT', 'TU_DATA_GROUNDED'}
FIELD_CONSISTENT = {'SYMBOL_CONSISTENT', 'RECEIPT_CONSISTENT', 'DGROUP_BASE_CONSISTENT'}
FIELD_TU = {'TU_DATA_PLACEMENT', 'TU_DATA_UNVERIFIED'}
FIELD_SYMBOL = {'SYMBOL_NAME_DIFFERS', 'SYMBOL_BINDS_ELSEWHERE', 'SYMBOL_UNKNOWN_TARGET',
                'BOUND_VALUE_DIFFERS', 'RECEIPT_DISAGREES', 'INTERNAL_UNMAPPED', 'UNSUPPORTED_SHAPE'}
FIELD_CODEGEN = {'SHAPE_DIFFERS', 'INTERNAL_DIFFERS', 'TU_DATA_CONTENT_DIFFERS', 'OUT_OF_EXTENT',
                 'CALL_TARGET_DIFFERS', 'ADDEND_DIFFERS'}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8'))


# --------------------------------------------------------------- memoised authority
_orig_read_authority = tubench._read_authority
_orig_function_maps = tubench._function_maps
_orig_build_map = tubench.build_tu_map
_orig_symbol_alias = tubench._symbol_alias


@functools.lru_cache(maxsize=1)
def _read_authority_cached():
    return _orig_read_authority()


@functools.lru_cache(maxsize=1)
def _function_maps_cached():
    return _orig_function_maps()


@functools.lru_cache(maxsize=1)
def _build_map_cached():
    CACHE.mkdir(parents=True, exist_ok=True)
    return _orig_build_map()


_alias_memo = {}


def _symbol_alias_cached(symbol_name, code_symbols, data_symbols, image, relocations, resolver_cache,
                         inventory_by_name):
    if symbol_name not in _alias_memo:
        _alias_memo[symbol_name] = _orig_symbol_alias(symbol_name, code_symbols, data_symbols, image,
                                                      relocations, {}, inventory_by_name)
    return _alias_memo[symbol_name]


tubench._read_authority = _read_authority_cached
tubench._function_maps = _function_maps_cached
tubench.build_tu_map = _build_map_cached
tubench._symbol_alias = _symbol_alias_cached


# --------------------------------------------------------------- context
class Context:
    """Read-only authority snapshot shared by all classifications in a process."""

    def __init__(self, register_path=None, objmap_path=None, symbol_receipts=None, record_proof=None,
                 source=None):
        self.oracle, self.image = _read_authority_cached()
        self.relocations = self.oracle[2]['unpacked_mz']['relocations']
        self.reloc_index = {r['load_offset']: i for i, r in enumerate(self.relocations)}
        from function_evidence import current_inventory
        self.inventory = current_inventory(self.image)['functions']
        self.inv_by_start = defaultdict(list)
        self.inv_by_name = defaultdict(list)
        for row in self.inventory:
            if isinstance(row.get('start'), int):
                self.inv_by_start[row['start']].append(row)
            self.inv_by_name[row['name']].append(row)
        self.verified_rows, self.verified_by_name, _ = _function_maps_cached()
        data_layout = read_json(ROOT / 'layout/data-symbols.json')
        self.data_frame = data_layout['frame_load_address']
        self.data_symbols = data_layout['symbols']
        self.data_by_addr = defaultdict(list)
        self.data_extents = []
        for name, row in self.data_symbols.items():
            addr = row.get('load_address')
            if isinstance(addr, int):
                self.data_by_addr[addr].append(name)
                if isinstance(row.get('width'), int) and row['width'] > 0:
                    self.data_extents.append((addr, addr + row['width'], name))
        self.data_extents.sort()
        self.code_symbols = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        self.code_by_addr = defaultdict(list)
        for name, row in self.code_symbols.items():
            target = row.get('mapped_target') or {}
            if isinstance(target.get('start'), int):
                self.code_by_addr[target['start']].append(name)
        self.manifest = read_json(ROOT / 'layout/manifest.json')
        self.owners = sorted((o for o in self.manifest['owners'] if isinstance(o.get('start'), int)),
                             key=lambda o: o['start'])
        self.owner_starts = [o['start'] for o in self.owners]
        self.register_path = Path(register_path) if register_path else ROOT / 'evidence/toolchain-hypotheses.json'
        self.register = read_json(self.register_path)
        self.objmap_path = Path(objmap_path) if objmap_path else ROOT / 'build/workers/objmap/objmap.json'
        self.objmap = read_json(self.objmap_path)
        self.objects = sorted(self.objmap['objects'], key=lambda o: o['start'])
        self.object_starts = [o['start'] for o in self.objects]
        self.receipts = load_symbol_receipts(symbol_receipts) if symbol_receipts else {}
        self.record_proof = load_record_proof(record_proof, source) if record_proof else None
        self.symptoms = unresolved_symptoms(self.register)
        self.flag_rulings = flag_rulings(self.register)

    # -- ownership
    def accepted_bytes(self, start, end):
        """Bytes of [start,end) covered by accepted (non-raw) manifest owners."""
        total = 0
        owners = []
        i = max(0, bisect.bisect_right(self.owner_starts, start) - 1)
        while i < len(self.owners) and self.owners[i]['start'] < end:
            o = self.owners[i]
            lo, hi = max(start, o['start']), min(end, o['end'])
            if hi > lo and o.get('kind') in ACCEPTED_KINDS:
                total += hi - lo
                owners.append({'id': o['id'], 'kind': o['kind']})
            i += 1
        return total, owners

    def object_of(self, start):
        i = bisect.bisect_right(self.object_starts, start) - 1
        if i >= 0 and self.objects[i]['start'] <= start < self.objects[i]['end']:
            return self.objects[i]
        return None


AUTOSYM_OUT = ROOT / 'build/autosym'


def autosym_symbol_receipts(directory=AUTOSYM_OUT):
    """Receipts from an autosym derivation (tools/autosym.py derive --out DIR).

    Code proposals give the mapped entry, data proposals their DGROUP address.
    They stay UNREVIEWED here (at most CODEGEN_EXACT) until merged into layout/."""
    directory = Path(directory)
    result = {}
    code = directory / 'autosym_code_symbols.json'
    data = directory / 'autosym_data_symbols.json'
    for path in (code, data):
        if not path.exists():
            raise ValueError('autosym receipts missing: run tools/autosym.py derive --out ' + str(directory))
    for name, row in read_json(code)['symbols'].items():
        result[name.lstrip('_')] = {'load_address': row['mapped_target']['start'],
                                    'frame_load_address': row['frame_load_address'],
                                    'kind': 'far-code', 'source': 'autosym'}
    for name, row in read_json(data)['symbols'].items():
        result[name.lstrip('_')] = {'load_address': row['load_address'], 'kind': 'dgroup-data',
                                    'source': 'autosym'}
    return result


def load_symbol_receipts(path):
    """Hook for the symbol-closure agent (USER_SPEC item 2).

    Accepts {"symbols": {alias: {load_address, frame_load_address?, kind?}}} or a list of
    {"alias"|"name", "load_address", ...}; `autosym` (or an autosym output directory)
    reads tools/autosym.py receipts.  Receipts are UNREVIEWED here: a field
    consistent only through a receipt can reach CODEGEN_EXACT, never BYTE_EXACT.
    """
    if str(path) == 'autosym':
        return autosym_symbol_receipts()
    if Path(path).is_dir():
        return autosym_symbol_receipts(path)
    doc = read_json(path)
    rows = doc.get('symbols', doc.get('receipts', doc)) if isinstance(doc, dict) else doc
    result = {}
    items = rows.items() if isinstance(rows, dict) else ((r.get('alias') or r.get('name'), r) for r in rows)
    for name, row in items:
        if name and isinstance(row, dict) and isinstance(row.get('load_address'), int):
            result[name.lstrip('_')] = row
    return result


def record_proof_from_source(source, flags='auto'):
    """Derive the hook document with integP's API (tools/prefix_proof.prove_source).

    The proof is re-derived from a fresh compile of the candidate; its
    classifier view carries the RECORD_CLOSED_EXACT intervals.  Diagnostic only."""
    from prefix_proof import prove_source
    raw = Path(source).read_bytes() if isinstance(source, (str, Path)) else source
    return prove_source(raw, flags=flags)['classifier']


def load_record_proof(path, source=None):
    """Hook for the prefix-proof agent (USER_SPEC item 1).

    Accepts {"members": {name: state}} and/or {"closed_intervals": [{start,end,state}]}
    (load-image offsets), a `prefix_proof.py --report` document (its `classifier`
    view), or `auto` to derive it from the candidate source with integP's
    prove_source API.  Only state == RECORD_CLOSED_EXACT upgrades an exact member.
    """
    if str(path) == 'auto':
        if source is None:
            raise ValueError('--record-proof auto needs the candidate source')
        doc = record_proof_from_source(source)
        members = dict(doc.get('members') or {})
        intervals = [(r['start'], r['end']) for r in doc.get('closed_intervals', [])
                     if r.get('state', 'RECORD_CLOSED_EXACT') == 'RECORD_CLOSED_EXACT']
        return {'members': members, 'intervals': intervals, 'path': 'prefix_proof.prove_source'}
    doc = read_json(path)
    if isinstance(doc, dict) and isinstance(doc.get('classifier'), dict):
        doc = doc['classifier']
    members = {k: v for k, v in (doc.get('members') or {}).items()}
    intervals = [(r['start'], r['end']) for r in doc.get('closed_intervals', [])
                 if r.get('state', 'RECORD_CLOSED_EXACT') == 'RECORD_CLOSED_EXACT']
    return {'members': members, 'intervals': intervals, 'path': str(path)}


# --------------------------------------------------------------- toolchain register
def _segments_in(text):
    return {'seg%03d' % int(n) for n in re.findall(r'seg(\d{3})', text or '')}


def flag_rulings(register):
    """segment -> list of dicted TUFLAG entries (status, flags, production_reviewed)."""
    try:
        import object_flags
        reviewed = object_flags.REVIEWED
    except Exception:  # pragma: no cover
        reviewed = {}
    out = defaultdict(list)
    for row in register.get('hypotheses', []):
        hid = row.get('id', '')
        if not hid.startswith('TUFLAG') or 'sweep' in hid:
            continue
        text = ' '.join(str(row.get(k, '')) for k in ('hypothesis', 'reopen_if'))
        m = re.search(r'/AM\s+(/O\w*)\s+/Gs', text)
        opt = m.group(1) if m else None
        if not opt:
            m = re.search(r'RULING:[^/]*?(/O\w*)', row.get('reopen_if', ''))
            opt = m.group(1) if m else None
        flags = ['/AM', opt, '/Gs'] if opt else None
        for seg in _segments_in(row.get('scope', '')):
            prod = hid in reviewed and seg in reviewed[hid].get('segments', ()) and \
                reviewed[hid].get('flags') == flags
            out[seg].append({'id': hid, 'status': row.get('status'), 'flags': flags,
                             'production_reviewed': bool(prod)})
    # Sweep entry: sentence-level labels over segment lists.
    for row in register.get('hypotheses', []):
        if 'sweep' not in row.get('id', ''):
            continue
        for sentence in re.split(r'\.\s+', row.get('evidence', '')):
            label = next((l for l in ('NON_DISCRIMINATING', 'FALSIFIED', 'DISFAVORED', 'PLAUSIBLE')
                          if l in sentence), None)
            if not label:
                continue
            segs = set()
            for m in re.finditer(r'seg(\d{3})((?:\s*,\s*\d{3}\b)*)', sentence):
                segs.add('seg' + m.group(1))
                segs.update('seg' + n for n in re.findall(r'\d{3}', m.group(2)))
            for seg in segs:
                out[seg].append({'id': row['id'], 'status': 'SWEEP_' + label, 'flags': None,
                                 'production_reviewed': False})
    return out


def object_profile(segment, rulings):
    """Return profile status for an object segment from the register snapshot."""
    entries = rulings.get(segment or '', [])
    dedicated = [e for e in entries if not e['status'].startswith('SWEEP_')]
    supported = [e for e in dedicated if e['status'] == 'SUPPORTED' and e['flags']]
    if supported:
        e = supported[0]
        return {'status': 'FLAGGED_PRODUCTION' if e['production_reviewed'] else 'FLAGGED_REGISTER_ONLY',
                'flags': e['flags'], 'entries': [x['id'] for x in entries]}
    if any(e['status'] in ('PLAUSIBLE', 'UNTESTED', 'TOOLING_LIMITED') for e in dedicated):
        return {'status': 'UNCERTAIN', 'flags': CANONICAL_FLAGS, 'entries': [x['id'] for x in entries]}
    if any(e['status'] in ('FALSIFIED', 'DISFAVORED') for e in dedicated):
        return {'status': 'CANONICAL', 'flags': CANONICAL_FLAGS, 'entries': [x['id'] for x in entries]}
    if any(e['status'] == 'SWEEP_PLAUSIBLE' for e in entries):
        return {'status': 'UNCERTAIN', 'flags': CANONICAL_FLAGS, 'entries': [x['id'] for x in entries]}
    return {'status': 'CANONICAL', 'flags': CANONICAL_FLAGS, 'entries': [x['id'] for x in entries]}


def unresolved_symptoms(register):
    """function name -> [symptom ids] for symptoms without a SUPPORTED/EXPLAINED explanation."""
    out = defaultdict(list)
    for s in register.get('symptoms', []):
        status = str(s.get('status', ''))
        expl = s.get('explanations') or {}
        resolved = status in ('EXPLAINED', 'RESOLVED', 'CLOSED') or \
            any(str(v).startswith('SUPPORTED') for v in expl.values())
        if status == 'OPEN':
            resolved = False
        if resolved:
            continue
        for text in s.get('functions', []):
            m = re.match(r'([A-Za-z_]\w*)((?:/\w+)*)', text)
            if not m:
                continue
            base = m.group(1)
            out[base].append(s.get('id'))
            if m.group(2):
                stem = base.rsplit('_', 1)[0]
                for tail in m.group(2).strip('/').split('/'):
                    out[stem + '_' + tail].append(s.get('id'))
    return out


# --------------------------------------------------------------- compile
_last_compile = {}


def sparse_ranges(obj_bytes):
    """segment name -> {'initialized_ranges', 'declared_length'} from SEGDEF/LNAMES/LEDATA."""
    names, segs, ranges = [None], [None], defaultdict(set)
    at = 0
    while at + 3 <= len(obj_bytes):
        kind = obj_bytes[at]
        length = struct.unpack_from('<H', obj_bytes, at + 1)[0]
        body = obj_bytes[at + 3:at + 3 + length - 1]
        if kind == 0x96:
            p = 0
            while p < len(body):
                n = body[p]
                names.append(body[p + 1:p + 1 + n].decode('latin-1'))
                p += 1 + n
        elif kind == 0x98:
            acbp = body[0]
            p = 1 + (3 if (acbp >> 5) == 0 else 0)
            seg_len = struct.unpack_from('<H', body, p)[0]
            p += 2
            idx = body[p]
            if idx & 0x80:
                idx = ((idx & 0x7F) << 8) | body[p + 1]
            segs.append((names[idx], seg_len))
        elif kind == 0xA0:
            p = 0
            idx = body[p]
            if idx & 0x80:
                idx = ((idx & 0x7F) << 8) | body[p + 1]
                p += 2
            else:
                p += 1
            off = struct.unpack_from('<H', body, p)[0]
            ranges[idx].update(range(off, off + len(body) - p - 2))
        at += 3 + length
        if kind in (0x8A, 0x8B):
            break
    policy = {}
    for idx, occupied in ranges.items():
        name, seg_len = segs[idx]
        if occupied == set(range(seg_len)):
            continue
        if occupied == set(range(len(occupied))):
            policy[name] = {'initialized_prefix': len(occupied), 'declared_length': seg_len}
            continue
        rows, start, prev = [], None, None
        for x in sorted(occupied):
            if start is None:
                start = prev = x
            elif x == prev + 1:
                prev = x
            else:
                rows.append([start, prev + 1])
                start = prev = x
        if start is not None:
            rows.append([start, prev + 1])
        policy[name] = {'initialized_ranges': rows, 'declared_length': seg_len}
    return policy


def make_compiler(flags):
    """Return a tubench-compatible compile function using `flags` (pinned toolchain)."""

    def compile_in_worker(source_path: Path):
        import compiler
        from object_probe import read_object
        from preprocessor import prepare
        config, runner = compiler.verify_toolchain(PROFILE)
        source_path = Path(source_path).expanduser().resolve()
        source = source_path.read_bytes()
        expanded, closure = prepare(source, PROFILE)
        text = expanded.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
        if re.search(r'\b(?:_asm|__asm|asm|__emit)\b', text, re.M):
            raise RuntimeError('Inline assembly/raw emission is forbidden')
        run_dir = WORK / 'compiler_runs' / (sha(source)[:12] + '_' + uuid.uuid4().hex[:8])
        run_dir.mkdir(parents=True, exist_ok=False)
        (run_dir / 'UNIT.C').write_bytes(text.replace('\n', '\r\n').encode('ascii'))
        tc = (ROOT / config['directory']).resolve()
        argv = [runner['path'], '-e', '-v5.00', str(tc / config['executable']), '/c'] + list(flags) + ['UNIT.C']
        env = {'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'}
        result = subprocess.run(argv, cwd=run_dir, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                timeout=120, creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
        (run_dir / 'compiler.log').write_bytes(result.stdout)
        obj_path = run_dir / 'UNIT.OBJ'
        if result.returncode or not obj_path.is_file():
            raise RuntimeError(f'Compiler failed ({result.returncode}); see {run_dir / "compiler.log"}')
        obj_bytes = obj_path.read_bytes()
        notes = []
        try:
            obj = read_object(obj_bytes, research_local_symbols=True)
        except ValueError as error:
            if 'Holes or overflow' not in str(error):
                raise
            # DIAGNOSTIC ONLY: re-read with the object's own declared initialized ranges so the
            # comparison can proceed; the hole itself stays an explicit flag (never accepted here).
            policy = sparse_ranges(obj_bytes)
            obj = read_object(obj_bytes, research_local_symbols=True, sparse_zero=policy)
            notes.append({'candidate_object_has_initialized_holes': policy})
        _last_compile.update(obj=obj, obj_bytes=obj_bytes, run_dir=run_dir, flags=list(flags),
                             closure=closure, notes=notes)
        return source, run_dir, obj, obj_bytes

    return compile_in_worker


def code_records(obj_bytes, code_segment_name, segment_defs):
    """Candidate CODE LEDATA extents [(offset, end)] in emission order (A0 records only)."""
    index_by_name = {d['name']: d['index'] for d in segment_defs}
    want = index_by_name.get(code_segment_name)
    out = []
    at = 0
    while at + 3 <= len(obj_bytes):
        kind = obj_bytes[at]
        length = struct.unpack_from('<H', obj_bytes, at + 1)[0]
        body = obj_bytes[at + 3:at + 3 + length - 1]
        if kind in (0xA0, 0xA1):
            p = 0
            seg = body[p]
            if seg & 0x80:
                seg = ((seg & 0x7F) << 8) | body[p + 1]
                p += 2
            else:
                p += 1
            if kind == 0xA0:
                off = struct.unpack_from('<H', body, p)[0]
                p += 2
            else:
                off = struct.unpack_from('<I', body, p)[0]
                p += 4
            if seg == want:
                out.append((off, off + len(body) - p))
        at += 3 + length
        if kind in (0x8A, 0x8B):
            break
    return out


# --------------------------------------------------------------- helpers
def norm(name):
    return (name or '').lstrip('_')


def same_name(a, b):
    a, b = norm(a), norm(b)
    if not a or not b:
        return False
    if a == b:
        return True
    return (len(a) >= 30 or len(b) >= 30) and a[:30] == b[:30]


def le16(data, at):
    return int.from_bytes(data[at:at + 2], 'little')


def addend_of(fix):
    enc = bytes.fromhex(fix.get('encoded_addend') or '')
    return int.from_bytes(enc[:2], 'little') if len(enc) >= 2 else 0


# --------------------------------------------------------------- core classification
def classify_source(source, *, function=None, members=None, interval=None, tu=None, object_id=None,
                    flags=None, ctx=None, keep=False, with_mismatch_detail=True, quiet=True):
    ctx = ctx or Context()
    _redirect_tubench()
    source = Path(source)
    members_arg = None
    if function:
        members_arg = function
    elif members:
        members_arg = members
    if object_id:
        obj_row = next((o for o in ctx.objects if o['id'] == object_id), None)
        if obj_row is None:
            raise SystemExit(f'Unknown objmap object {object_id}')
        interval = (obj_row['start'], obj_row['end'])
    # Profile: pick flags from the register for the object of the first selected member.
    probe_start = interval[0] if interval else None
    if probe_start is None and members_arg:
        first = members_arg.split(',')[0].strip()
        rows = ctx.verified_by_name.get(first) or ctx.inv_by_name.get(first) or []
        probe_start = rows[0]['start'] if rows else None
    if probe_start is None and tu:
        m = next((c for c in _build_map_cached()['closures'] if c['id'] == tu), None)
        probe_start = m['interval']['start'] if m else None
    obj_row = ctx.object_of(probe_start) if probe_start is not None else None
    profile = object_profile(obj_row.get('segment') if obj_row else None, ctx.flag_rulings)
    flags_source = 'register'
    if flags:
        used_flags = list(flags)
        flags_source = 'override'
    else:
        used_flags = profile['flags'] if profile['status'] in ('FLAGGED_PRODUCTION', 'FLAGGED_REGISTER_ONLY') \
            else CANONICAL_FLAGS
    compile_fn = make_compiler(used_flags)
    memo = {}

    def compile_cached(path):
        key = sha(Path(path).read_bytes())
        if key not in memo:
            memo[key] = compile_fn(path)
        return memo[key]

    tubench._compile_in_worker = compile_cached
    try:
        _, _, obj0, _ = compile_cached(source)
    except Exception as error:
        return {'authority': AUTHORITY, 'source': str(source.resolve()), 'status': 'COMPILE_OR_SELECTION_FAILED',
                'error': str(error), 'flags': used_flags, 'members': []}
    # Selected names (tubench semantics) plus every other emitted public that maps to a
    # verified inventory member: the latter are record context only (not reported as selected).
    if members_arg:
        selected = [n.strip() for n in members_arg.split(',') if n.strip()]
    elif interval:
        selected = [r['name'] for r in sorted(ctx.verified_rows, key=lambda r: r['start'])
                    if interval[0] <= r['start'] and r['end'] <= interval[1]]
    else:
        closure = next((c for c in _build_map_cached()['closures'] if c['id'] == tu), None)
        if closure is None:
            raise SystemExit(f'Unknown TU id {tu}')
        selected = [r['name'] for r in closure['members'] if r['name'] in ctx.verified_by_name]
    code_segs = [n for n in obj0.segments if any(p.get('segment') == n for p in obj0.publics)]
    cseg = 'UNIT_TEXT' if 'UNIT_TEXT' in code_segs else (code_segs[0] if code_segs else None)
    pmap = tubench._public_name_map(obj0, cseg) if cseg else defaultdict(list)
    context = []
    for pub in obj0.publics:
        if pub.get('segment') != cseg:
            continue
        row, _ = tubench._resolve_near_member(pub['name'], ctx.verified_by_name, pmap)
        if row and row['name'] not in selected and row['name'] not in context:
            if tubench._choose_public(pmap, row['name']) is not None:
                context.append(row['name'])
    report = None
    for attempt in range(4):
        buf = io.StringIO()
        try:
            with contextlib.redirect_stdout(buf):
                report = tubench.run_workbench(str(source), members_arg=','.join(selected + context))
            break
        except Exception as error:
            if 'authority inputs changed' in str(error) and attempt < 3:
                continue  # a concurrent canonical writer touched layout/evidence; retry the snapshot
            return {'authority': AUTHORITY, 'source': str(source.resolve()), 'status': 'COMPILE_OR_SELECTION_FAILED',
                'error': str(error), 'flags': used_flags, 'members': []}
    obj = _last_compile['obj']
    obj_bytes = _last_compile['obj_bytes']
    run_dir = _last_compile['run_dir']
    result = analyse(report, obj, obj_bytes, ctx, used_flags, flags_source, profile,
                     with_mismatch_detail=with_mismatch_detail)
    result['source'] = str(source.resolve())
    result['source_sha256'] = report['source_sha256']
    result['selection'] = {'requested': {'function': function, 'members': members, 'interval': interval,
                                         'tu': tu, 'object': object_id},
                           'selected': selected, 'record_context_members': context}
    sel = set(selected)
    result['context_members'] = [{'name': e['name'], 'comparison': e.get('comparison'),
                                  'primary_state': e['primary_state']}
                                 for e in result['members'] if e['name'] not in sel]
    result['members'] = [e for e in result['members'] if e['name'] in sel]
    result['summary'] = dict(Counter(e['primary_state'] for e in result['members']))
    result['preprocessor_closure'] = _last_compile.get('closure')
    result['compile_notes'] = _last_compile.get('notes', [])
    if result['compile_notes']:
        for e in result['members']:
            e['flags'].append('candidate_object_has_initialized_holes')
    if not keep:
        shutil.rmtree(run_dir, ignore_errors=True)
    else:
        result['work_directory'] = str(run_dir)
    return result


def _member_maps(ctx, emitted_publics, public_map, code_size):
    """candidate public offset -> (inventory row, emitted size) for all mappable publics."""
    maps = []
    pubs = sorted(emitted_publics, key=lambda p: p['offset'])
    for i, pub in enumerate(pubs):
        nxt = pubs[i + 1]['offset'] if i + 1 < len(pubs) else code_size
        row, _ = tubench._resolve_near_member(pub['name'], ctx.verified_by_name, public_map)
        maps.append({'offset': pub['offset'], 'size': nxt - pub['offset'], 'row': row, 'public': pub['name']})
    return maps


def _map_offset(maps, x):
    """Map a candidate segment offset to a target load offset (or None, reason)."""
    for m in maps:
        if m['offset'] <= x < m['offset'] + m['size']:
            row = m['row']
            if row is None:
                return None, f"inside unmapped public {m['public']}"
            rel = x - m['offset']
            tsize = row['end'] - row['start']
            if rel == 0 or m['size'] == tsize:
                return row['start'] + rel, None
            return None, f"interior of size-mismatched member {row['name']}"
    return None, 'outside emitted publics'


def _known_addresses(target, ctx):
    out = [r['start'] for r in ctx.inv_by_name.get(norm(target), []) if isinstance(r.get('start'), int)]
    for key in (target, '_' + norm(target), norm(target)):
        row = ctx.data_symbols.get(key)
        if row and isinstance(row.get('load_address'), int):
            out.append(row['load_address'])
        crow = ctx.code_symbols.get(key)
        if crow and isinstance((crow.get('mapped_target') or {}).get('start'), int):
            out.append(crow['mapped_target']['start'])
    return sorted(set(out))


def _target_reference(fix, tb, t_at, ctx, seg_hint):
    """(load address referenced by the TARGET field, names at that address)."""
    loc = fix.get('loc')
    target = fix.get('target') or ''
    addend = addend_of(fix)
    if fix.get('self_relative') and len(tb) >= 2:
        dest = t_at + 2 + struct.unpack('<h', tb[:2])[0]
        return dest, [r['name'] for r in ctx.inv_by_start.get(dest, [])]
    if loc == 'pointer32' and len(tb) == 4:
        off, seg = struct.unpack('<HH', tb)
        dest = seg * 16 + off
        return dest, [r['name'] for r in ctx.inv_by_start.get(dest, [])] + ctx.code_by_addr.get(dest, [])
    if loc == 'base16' and len(tb) == 2:
        return le16(tb, 0) * 16, []
    if loc == 'offset16' and len(tb) == 2:
        v = le16(tb, 0)
        frame = seg_hint[target] * 16 if target in seg_hint else ctx.data_frame
        dest = frame + ((v - addend) & 0xFFFF)
        names = list(ctx.data_by_addr.get(dest, [])) + ctx.code_by_addr.get(dest, []) + \
            [r['name'] for r in ctx.inv_by_start.get(dest, [])]
        if not names:
            i = bisect.bisect_right(ctx.data_extents, (dest, 1 << 30, '')) - 1
            for lo, hi, nm in reversed(ctx.data_extents[max(0, i - 64):i + 1]):
                if lo <= dest < hi:
                    names.append(f'{nm}+{dest - lo}')
                    break
        return dest, names
    return None, []


def evaluate_field(fix, ctx, *, t_at, segment_bytes, code_segment, maps, site_row, own_data, obj, seg_hint=None):
    """Categorise one candidate fixup field against the target bytes. Returns dict."""
    image = ctx.image
    seg_hint = seg_hint or {}
    width = fix['width']
    tb = bytes(image[t_at:t_at + width])
    out = {'offset': fix['offset'], 'target_site': t_at, 'loc': fix.get('loc'), 'width': width,
           'target': fix.get('target'), 'target_kind': fix.get('target_kind'),
           'self_relative': bool(fix.get('self_relative')), 'target_bytes': tb.hex()}
    alias = fix.get('alias') or {}
    out['resolver_status'] = alias.get('status')
    loc = fix.get('loc')
    # shape: segment-word relocations must coincide with target relocations
    cand_reloc = (t_at + 2) if (loc == 'pointer32' and width == 4) else (t_at if loc == 'base16' else None)
    if cand_reloc is not None and cand_reloc not in ctx.reloc_index:
        out['category'] = 'SHAPE_DIFFERS'
        out['detail'] = 'candidate segment word has no MZ relocation at the target site'
        return out
    if fix.get('resolved'):
        pv = fix.get('predicted_value')
        if fix.get('self_relative') and isinstance(fix.get('target_public_offset'), int):
            # tubench pairs near-call occurrences by order; recompute at THIS target site.
            pred = struct.pack('<H', (fix['target_public_offset'] - (t_at + 2)) & 0xFFFF)
        elif isinstance(pv, list):
            pred = struct.pack('<HH', pv[0] & 0xFFFF, pv[1] & 0xFFFF)
        else:
            pred = struct.pack('<H', pv & 0xFFFF)
        if pred == tb:
            out['category'] = 'TU_DATA_GROUNDED' if str(alias.get('status', '')).startswith('TU_OWNED_DATA') \
                else 'BOUND'
            return out
        out['detail'] = f'reviewed binding gives {pred.hex()} but target holds {tb.hex()}'
        dest, names = _target_reference(fix, tb, t_at, ctx, seg_hint)
        out['target_address'] = dest
        out['target_names'] = names[:6]
        is_code = fix.get('self_relative') or loc == 'pointer32'
        if is_code and names and not any(same_name(n, fix.get('target')) for n in names):
            out['category'] = 'CALL_TARGET_DIFFERS'
        elif not is_code and any(same_name(n.split('+')[0], fix.get('target')) for n in names):
            # same reviewed object, different element/field offset: a source (index/field) difference
            out['category'] = 'ADDEND_DIFFERS'
        else:
            out['category'] = 'BOUND_VALUE_DIFFERS'
        return out
    tk = fix.get('target_kind')
    target = fix.get('target') or ''
    addend = addend_of(fix)
    # ---- internal references to the candidate's own code segment
    if tk == 'segment' and target == code_segment:
        tload, why = _map_offset(maps, addend)
        if tload is None:
            out['category'] = 'INTERNAL_UNMAPPED'
            out['detail'] = why
            return out
        if fix.get('self_relative'):
            expected = (tload - (t_at + 2)) & 0xFFFF
        else:
            frame = site_row.get('segment_paragraph') if site_row else None
            if not isinstance(frame, int):
                out['category'] = 'INTERNAL_UNMAPPED'
                out['detail'] = 'site frame unknown'
                return out
            expected = (tload - frame * 16) & 0xFFFF
        out['expected'] = f'{expected:04x}'
        out['category'] = 'INTERNAL_CONSISTENT' if struct.pack('<H', expected) == tb[:2] else 'INTERNAL_DIFFERS'
        return out
    # ---- TU-owned data segments
    if tk == 'segment':
        if loc == 'base16' and width == 2 and target in ('_DATA', 'CONST', '_BSS', 'DGROUP'):
            # segment word of a DGROUP member: the paragraph is fixed by the locked DGROUP frame
            ok = le16(tb, 0) * 16 == ctx.data_frame and t_at in ctx.reloc_index
            out['category'] = 'DGROUP_BASE_CONSISTENT' if ok else 'SHAPE_DIFFERS'
            return out
        if loc != 'offset16' or width != 2:
            out['category'] = 'UNSUPPORTED_SHAPE'
            return out
        v = le16(tb, 0)
        implied_base = (v - addend) & 0xFFFF
        out['target_operand'] = v
        out['addend'] = addend
        out['implied_segment_dgroup_offset'] = implied_base
        placement = own_data.get(target, {})
        out['placement_status'] = placement.get('status')
        payload = bytes(obj.segments.get(target, b''))
        if not payload or addend >= len(payload):
            out['category'] = 'TU_DATA_UNVERIFIED'
            out['detail'] = 'no initialized candidate payload at addend (BSS or out of range)'
            return out
        end = payload.find(b'\0', addend)
        if end != -1 and end - addend >= 1 and all(32 <= c < 127 for c in payload[addend:end]):
            item = payload[addend:end + 1]
        else:
            item = payload[addend:addend + min(4, len(payload) - addend)]
        at = ctx.data_frame + v
        orig = bytes(image[at:at + len(item)])
        out['candidate_item'] = item.hex()
        out['target_item'] = orig.hex()
        if orig == item:
            out['category'] = 'TU_DATA_PLACEMENT'
        elif all(32 <= c < 127 for c in item[:-1]) and item.endswith(b'\0'):
            out['category'] = 'TU_DATA_CONTENT_DIFFERS'
        else:
            out['category'] = 'TU_DATA_UNVERIFIED'
            out['detail'] = 'non-string payload differs at implied address (placement or content)'
        return out
    # ---- external symbols
    if tk == 'external':
        receipt = ctx.receipts.get(norm(target))
        if fix.get('self_relative') and not (loc == 'offset16' and width == 2 and t_at >= 1
                                             and image[t_at - 1] == 0xE8):
            out['category'] = 'SHAPE_DIFFERS'
            out['detail'] = 'target has no near CALL opcode before the self-relative field'
            return out
        if norm(target) in ABSOLUTE_CONSTANTS and width == 2:
            value = ABSOLUTE_CONSTANTS[norm(target)]
            out['category'] = 'SYMBOL_CONSISTENT' if le16(tb, 0) == value else 'SYMBOL_BINDS_ELSEWHERE'
            out['detail'] = (f'absolute runtime constant {target}={value}; production binding needs its pinned '
                             'runtime declaration (e.g. dos\\diffhlp.asm)')
            return out
        if loc not in ('offset16', 'pointer32', 'base16'):
            out['category'] = 'UNSUPPORTED_SHAPE'
            return out
        dest, names = _target_reference(fix, tb, t_at, ctx, seg_hint)
        out['target_address'] = dest
        out['target_names'] = names[:6]
        if receipt is not None:
            ok = receipt.get('load_address') == dest or (loc == 'base16' and dest is not None and
                                                       dest <= receipt['load_address'] < dest + 0x10000)
            out['category'] = 'RECEIPT_CONSISTENT' if ok else 'RECEIPT_DISAGREES'
            return out
        if any(same_name(n.split('+')[0], target) for n in names):
            out['category'] = 'SYMBOL_CONSISTENT'
            out['detail'] = 'target address carries the same inventory/alias name; reviewed binding missing'
            return out
        known = _known_addresses(target, ctx)
        if known and loc == 'offset16' and not fix.get('self_relative'):
            v = le16(tb, 0)
            frames = [ctx.data_frame] + ([seg_hint[target] * 16] if target in seg_hint else [])
            if any((a + addend - fr) & 0xFFFF == v for a in known for fr in frames):
                out['category'] = 'SYMBOL_CONSISTENT'
                out['detail'] = ('known address of the candidate symbol + addend reproduces the operand; '
                                 'reviewed resolver did not bind it (e.g. addend outside grounded extent)')
                return out
        if known and loc == 'base16' and dest is not None and any(dest <= a < dest + 0x10000 for a in known):
            out['category'] = 'SYMBOL_CONSISTENT'
            out['detail'] = 'segment word frames the known address of the candidate symbol'
            return out
        is_code = fix.get('self_relative') or loc == 'pointer32'
        inv_known = [r['start'] for r in ctx.inv_by_name.get(norm(target), []) if isinstance(r.get('start'), int)]
        if is_code and names and inv_known and dest not in inv_known:
            out['category'] = 'CALL_TARGET_DIFFERS'
            out['detail'] = f'candidate calls {target} (known at {inv_known[:2]}), target calls {names[:3]}'
        elif names:
            out['category'] = 'SYMBOL_NAME_DIFFERS'
            out['detail'] = f'target address names {names[:3]}'
        elif known and dest not in known:
            out['category'] = 'SYMBOL_BINDS_ELSEWHERE'
            out['detail'] = f'candidate symbol is known at {known[:3]}, target references {dest}'
        else:
            out['category'] = 'SYMBOL_UNKNOWN_TARGET'
            out['detail'] = 'no inventory/alias name at the target address (neutral alias needed)'
        return out
    out['category'] = 'UNSUPPORTED_SHAPE'
    return out


def _block_order(tins, cins):
    """Count target CFG blocks (>=4 insns) found uniquely in the candidate but out of order."""
    leaders = blockdiff._leaders(tins)
    ckeys = [x.key for x in cins]
    positions = []
    for bi, li in enumerate(leaders):
        end = leaders[bi + 1] if bi + 1 < len(leaders) else len(tins)
        seq = [x.key for x in tins[li:end]]
        if len(seq) < 4:
            continue
        hits = [j for j in range(len(ckeys) - len(seq) + 1) if ckeys[j:j + len(seq)] == seq]
        if len(hits) == 1:
            positions.append(hits[0])
    # longest increasing subsequence
    tails = []
    for p in positions:
        k = bisect.bisect_left(tails, p)
        if k == len(tails):
            tails.append(p)
        else:
            tails[k] = p
    return {'unique_blocks_found': len(positions), 'out_of_order_blocks': len(positions) - len(tails)}


def mismatch_detail(tbytes, cbytes, cfix_rel, target_row, member_base):
    """Sub-flags for genuine differences (diagnostic alignment only)."""
    flags = []
    detail = {}
    tdict = {'name': target_row['name'], 'bytes': tbytes, 'reloc_sites': None,
             'segment_offset': target_row.get('segment_offset')}
    try:
        res = blockdiff.compare(tdict, cbytes, cfix_rel, member_start=member_base)
        tins, cins, _ = res['_detail']
        detail.update({k: res[k] for k in ('cost', 'similarity_pct', 'op_classes', 'first_raw_difference',
                                          'differing_bytes_outside_fixups', 'target_insns', 'candidate_insns')})
        detail['frame'] = res['frame']
        if res['frame']['target']['frame'] != res['frame']['candidate']['frame'] or \
                res['frame']['target']['bp_frame'] != res['frame']['candidate']['bp_frame']:
            flags.append('frame')
        if res['frame']['target']['saved'] != res['frame']['candidate']['saved']:
            flags.append('saved_regs')
        first = next((b for b in res['blocks'] if b['cost'] > 0), None)
        detail['first_block_difference'] = first
        bo = _block_order(tins, cins)
        detail['block_order'] = bo
        if bo['out_of_order_blocks']:
            flags.append('block_order')
    except Exception as error:  # diagnostic only
        detail['blockdiff_error'] = str(error)
    if len(tbytes) <= 8000 and len(cbytes) <= 8000:
        try:
            rep = diagnostics.compare_streams(tbytes, cbytes, [dict(f, segment='UNIT_TEXT') for f in cfix_rel])
            classes = sorted({c['class'] for isl in rep['islands'] for c in isl['classifications']})
            detail['diagnostic_classes'] = classes
            if 'STACK_SLOT_ALLOCATION' in classes:
                flags.append('homes')
            if 'REGISTER_ALLOCATION' in classes:
                flags.append('register_allocation')
            if 'BRANCH_TARGET_OR_LAYOUT' in classes or 'CONTROL_FLOW_SHAPE' in classes:
                flags.append('control_flow_layout')
            if 'TEMPORARY_OR_SPILL' in classes:
                flags.append('temporary_or_spill')
        except Exception as error:
            detail['diagnostics_error'] = str(error)
    else:
        detail['diagnostics_skipped'] = 'member larger than 8000 bytes'
    return sorted(set(flags)), detail


def analyse(report, obj, obj_bytes, ctx, used_flags, flags_source, profile, with_mismatch_detail=True):
    image = ctx.image
    code_segment = report['compile']['code_segment']
    segment_bytes = bytes(obj.segments.get(code_segment, b'')) if code_segment else b''
    emitted_publics = sorted((p for p in obj.publics if p.get('segment') == code_segment),
                             key=lambda p: p['offset'])
    public_map = tubench._public_name_map(obj, code_segment) if code_segment else defaultdict(list)
    maps = _member_maps(ctx, emitted_publics, public_map, len(segment_bytes))
    own_data = report.get('own_data_placements', {})
    # full fixup detail list from tubench (all members, resolved + alias)
    all_fix = []
    for m in report['members']:
        all_fix.extend(m['external_fixups'])
    fix_by_offset = {}
    for f in all_fix:
        fix_by_offset[f['offset']] = f
    # every candidate code fixup (tubench only lists fixups inside selected members)
    raw_fixups = [f for f in obj.linker_fixups if f.get('segment') == code_segment]
    records = code_records(obj_bytes, code_segment, getattr(obj, 'segment_defs', [])) if code_segment else []
    members_out = []
    byte_exact_map = {}  # candidate offset range -> exact family?
    for m in report['members']:
        name = m['name']
        row = next((r for r in ctx.verified_by_name.get(name, [])), None)
        start, end = m['target']['start'], m['target']['end']
        tsize = end - start
        base = m['emitted']['offset']
        esize = m['emitted']['size_to_next_public']
        objrow = ctx.object_of(start)
        entry = {'name': name, 'stable_id': m.get('stable_id'),
                 'object': objrow['id'] if objrow else None,
                 'object_kind': objrow['kind'] if objrow else None,
                 'target': {'start': start, 'end': end, 'size': tsize},
                 'emitted': {'offset': base, 'size': esize}, 'flags': [], 'evidence': {}}
        acc, acc_owners = ctx.accepted_bytes(start, end)
        entry['accepted_bytes'] = acc
        # boundary evidence
        boundary = []
        if not objrow:
            boundary.append('outside_objmap_objects')
        elif end > objrow['end']:
            boundary.append('crosses_objmap_object_end')
        if row is None or row.get('status') not in tubench.VERIFIED_STATUSES:
            boundary.append('inventory_extent_unverified')
        objmap_member = None
        if objrow:
            objmap_member = next((x for x in objrow['members'] if x['start'] == start), None)
            if objmap_member is None or objmap_member['end'] != end:
                boundary.append('objmap_member_extent_differs')
            for c in ('confidence_start', 'confidence_end', 'confidence_single_object'):
                if objrow.get(c) not in ('PROVEN',):
                    entry['flags'].append(f'object_{c}_{objrow.get(c)}')
        for w in ctx.objmap.get('seg012_unresolved_windows', []):
            if start < w['hi_site'] and w['lo_site'] < end:
                entry['flags'].append('in_objmap_unresolved_window')
        entry['evidence']['boundary'] = boundary
        # profile / symptoms
        seg_profile = object_profile(objrow.get('segment') if objrow else None, ctx.flag_rulings)
        entry['evidence']['profile'] = seg_profile
        symptoms = ctx.symptoms.get(name, [])
        if symptoms:
            entry['evidence']['unresolved_symptoms'] = symptoms
        profile_uncertain = seg_profile['status'] == 'UNCERTAIN' or bool(symptoms)
        if seg_profile['status'] == 'FLAGGED_REGISTER_ONLY':
            entry['flags'].append('flags_register_only_not_production_reviewed')
        if list(used_flags) != list(seg_profile['flags']):
            entry['flags'].append('compiled_flags_differ_from_object_profile')
            if flags_source == 'override':
                profile_uncertain = True
        if base is None:
            entry['comparison'] = 'NOT_EMITTED'
            entry['primary_state'] = 'ACCEPTED' if acc == tsize else (
                'BOUNDARY_UNCERTAIN' if boundary else 'NO_CANDIDATE')
            entry['flags'].append('candidate_does_not_define_member')
            members_out.append(entry)
            continue
        cand = segment_bytes[base:base + esize]
        tbytes = bytes(image[start:end])
        # candidate fixups inside the member (by candidate offset)
        mfix = [f for f in raw_fixups if base <= f['offset'] < base + esize]
        mask = set()
        fields = []
        seg_hint = {}
        for f in mfix:
            rel = f['offset'] - base
            if f.get('loc') == 'base16' and f.get('target_kind') == 'external' and rel + 2 <= tsize:
                seg_hint[f.get('target')] = le16(image, start + rel)
        for f in mfix:
            rel = f['offset'] - base
            mask.update(range(rel, rel + f['width']))
            if start + rel + f['width'] > len(image):
                fields.append({'offset': f['offset'], 'category': 'OUT_OF_EXTENT', 'target': f.get('target')})
                continue
            detail = dict(f)
            detail.update(fix_by_offset.get(f['offset'], {}))
            fields.append(evaluate_field(detail, ctx, t_at=start + rel, segment_bytes=segment_bytes,
                                         code_segment=code_segment, maps=maps, site_row=row,
                                         own_data=own_data, obj=obj, seg_hint=seg_hint))
        same_size = esize == tsize
        outside = [k for k in range(min(esize, tsize)) if k not in mask and cand[k] != tbytes[k]]
        extent_conflict = None
        if esize > tsize and not outside:
            # Candidate public extent runs past the inventory end: compare the extra bytes with the
            # image that FOLLOWS the target (object tail/unmapped bytes or the next row).
            ext = bytes(image[start:start + esize])
            ext_out = [k for k in range(tsize, esize) if k not in mask and cand[k] != ext[k]]
            ext_rel = sorted(r - start for r in ctx.reloc_index if start <= r < start + esize)
            c_rel = sorted([f['offset'] - base + 2 for f in mfix if f.get('loc') == 'pointer32' and f['width'] == 4] +
                           [f['offset'] - base for f in mfix if f.get('loc') == 'base16'])
            following = [x['name'] for x in (objrow or {}).get('members', [])
                         if end <= x['start'] < start + esize]
            extent_conflict = {'candidate_extent': [start, start + esize], 'inventory_extent': [start, end],
                               'extra_bytes_equal_image': not ext_out and ext_rel == c_rel,
                               'objmap_members_covered_by_extension': following}
            entry['evidence']['extent_conflict'] = extent_conflict
        extent_exact = bool(extent_conflict and extent_conflict['extra_bytes_equal_image'])
        t_relocs = sorted(r - start for r in ctx.reloc_index if start <= r < end)
        c_relocs = sorted([f['offset'] - base + 2 for f in mfix if f.get('loc') == 'pointer32' and f['width'] == 4] +
                          [f['offset'] - base for f in mfix if f.get('loc') == 'base16'])
        shapes_equal = t_relocs == c_relocs
        if (not same_size or outside) and not extent_exact:
            # After the first genuine difference with a size shift, target sites are no longer
            # aligned: such field comparisons carry no binding evidence.
            first_div = outside[0] if outside else min(esize, tsize)
            if not same_size:
                for f in fields:
                    if f['offset'] - base > first_div and f['category'] not in FIELD_OK:
                        f['category_if_aligned'] = f['category']
                        f['category'] = 'AFTER_DIVERGENCE'
        cats = Counter(f['category'] for f in fields)
        entry['fixups'] = {'count': len(fields), 'categories': dict(cats)}
        entry['evidence']['size'] = {'emitted': esize, 'target': tsize, 'delta': esize - tsize}
        entry['evidence']['outside_fixup_differences'] = len(outside) + abs(esize - tsize)
        entry['evidence']['first_outside_difference'] = (outside[0] if outside else (None if same_size else min(esize, tsize)))
        entry['evidence']['relocation_sites_equal'] = shapes_equal
        entry['evidence']['near_calls_out'] = sum(1 for f in mfix if f.get('self_relative'))
        if entry['evidence']['near_calls_out']:
            entry['flags'].append('has_near_calls_needs_group_or_closure')
        interesting = [f for f in fields if f['category'] not in FIELD_OK]
        entry['field_details'] = interesting[:40]
        entry['field_details_omitted'] = max(0, len(interesting) - 40)
        if extent_exact:
            shapes_equal = True
            entry['evidence']['relocation_sites_equal'] = 'over candidate extent'
        codegen_equal = (same_size or extent_exact) and not outside and shapes_equal and \
            not any(c in FIELD_CODEGEN for c in cats)
        tu_issue = any(c in FIELD_TU for c in cats)
        sym_issue = any(c in FIELD_SYMBOL for c in cats)
        consistent_only = any(c in FIELD_CONSISTENT for c in cats)
        if codegen_equal and not tu_issue and not sym_issue:
            comparison = 'CODEGEN_EXACT' if consistent_only else 'BYTE_EXACT'
            if 'RECEIPT_CONSISTENT' in cats:
                entry['flags'].append('uses_unreviewed_symbol_receipts')
        elif codegen_equal and tu_issue:
            comparison = 'BLOCKED_TU_DATA'
            if sym_issue:
                entry['flags'].append('also_blocked_symbol')
        elif codegen_equal and sym_issue:
            comparison = 'BLOCKED_SYMBOL'
        else:
            comparison = 'CODEGEN_MISMATCH'
            reasons = []
            if not same_size:
                reasons.append('size_delta')
            if outside:
                reasons.append('bytes_outside_fixups')
            if not shapes_equal:
                reasons.append('relocation_shape')
            for c in FIELD_CODEGEN:
                if cats.get(c):
                    reasons.append(c.lower())
            entry['mismatch_reasons'] = reasons
            if with_mismatch_detail:
                cfix_rel = [{'offset': f['offset'] - base, 'width': f['width'], 'loc': f.get('loc'),
                             'self_relative': bool(f.get('self_relative')), 'target': f.get('target'),
                             'target_kind': f.get('target_kind'), 'encoded_addend': f.get('encoded_addend')}
                            for f in mfix]
                sub, det = mismatch_detail(tbytes, cand, cfix_rel, row or {'name': name}, base)
                entry['mismatch_subflags'] = sub + (['size_delta'] if not same_size else [])
                entry['evidence']['mismatch'] = det
        entry['comparison'] = comparison
        exact_family = comparison in ('BYTE_EXACT', 'CODEGEN_EXACT')
        byte_exact_map[base] = (base + esize, exact_family or comparison.startswith('BLOCKED'), exact_family)
        # primary state (record closure refined below)
        if acc == tsize and tsize > 0:
            primary = 'ACCEPTED'
            if not exact_family:
                entry['flags'].append('candidate_not_exact_for_accepted_member')
        elif extent_exact:
            primary = 'BOUNDARY_UNCERTAIN'
            entry['flags'].append('code_exact_over_candidate_extent_' + comparison)
        elif boundary and not exact_family and comparison == 'CODEGEN_MISMATCH':
            primary = 'BOUNDARY_UNCERTAIN'
        elif comparison == 'CODEGEN_MISMATCH' and profile_uncertain:
            primary = 'PROFILE_UNCERTAIN'
        else:
            primary = comparison
        if boundary and primary != 'BOUNDARY_UNCERTAIN':
            entry['flags'].append('boundary_' + '_'.join(boundary))
        if acc and acc < tsize:
            entry['flags'].append('partially_accepted')
        entry['primary_state'] = primary
        members_out.append(entry)
    # ---- diagnostic record closure (candidate CODE LEDATA extents)
    spans = sorted((b, e, ok, ex) for b, (e, ok, ex) in byte_exact_map.items())

    def record_closed(lo, hi):
        pos = lo
        for b, e, ok, ex in spans:
            if e <= pos:
                continue
            if b > pos:
                return False
            if not ex:
                return False
            pos = e
            if pos >= hi:
                return True
        return pos >= hi

    for entry in members_out:
        base = entry['emitted']['offset']
        if base is None:
            continue
        esize = entry['emitted']['size']
        recs = [(a, b) for a, b in records if a < base + esize and base < b]
        closed = [record_closed(a, b) for a, b in recs]
        entry['evidence']['candidate_records'] = [{'offset': a, 'end': b, 'closed_diag': c}
                                                  for (a, b), c in zip(recs, closed)]
        # candidate relocation run order inside each overlapping record (diagnostic only)
        runs = []
        for a, b in recs:
            sites = []
            for f in [f for f in obj.linker_fixups if f.get('segment') == code_segment and a <= f['offset'] < b]:
                if f.get('loc') == 'pointer32' and f['width'] == 4:
                    rel = f['offset'] + 2
                elif f.get('loc') == 'base16':
                    rel = f['offset']
                else:
                    continue
                mm = next((x for x in members_out if x['emitted']['offset'] is not None and
                           x['emitted']['offset'] <= rel < x['emitted']['offset'] + x['emitted']['size']), None)
                if mm is None:
                    sites.append(None)
                else:
                    sites.append(mm['target']['start'] + rel - mm['emitted']['offset'])
            idx = [ctx.reloc_index.get(s) if s is not None else None for s in sites]
            if not idx:
                runs.append('NO_RELOCATIONS')
            elif None in idx:
                runs.append('UNMAPPED_SITES')
            elif all(idx[i + 1] == idx[i] + 1 for i in range(len(idx) - 1)):
                runs.append('CONTIGUOUS_ORACLE_RUN')
            else:
                runs.append('NOT_CONTIGUOUS')
        entry['evidence']['record_relocation_runs'] = runs
        record_state = 'CLOSED_DIAG' if closed and all(closed) else 'OPEN'
        entry['evidence']['record_context'] = record_state
        proof = ctx.record_proof
        proven = False
        if proof:
            st = proof['members'].get(entry['name'])
            proven = st == 'RECORD_CLOSED_EXACT' or any(
                lo <= entry['target']['start'] and entry['target']['end'] <= hi for lo, hi in proof['intervals'])
        comp = entry['comparison']
        if comp in ('BYTE_EXACT', 'CODEGEN_EXACT'):
            if entry['primary_state'] not in ('BYTE_EXACT', 'CODEGEN_EXACT'):
                if record_state == 'OPEN':
                    entry['flags'].append('record_open')
            elif proven:
                entry['primary_state'] = 'RECORD_CLOSED_EXACT'
                entry['flags'].append('record_closed_by_prefix_proof')
            elif record_state == 'OPEN':
                entry['primary_state'] = 'EXACT_OPEN_RECORD'
                entry['flags'].append('exactness_' + comp)
            else:
                entry['flags'].append('record_proof_pending')
        elif comp.startswith('BLOCKED') and record_state == 'OPEN':
            entry['flags'].append('record_open')
    summary = Counter(e['primary_state'] for e in members_out)
    return {'authority': AUTHORITY, 'schema': 'classifier-v1', 'flags': list(used_flags),
            'flags_source': flags_source, 'selection_profile': profile,
            'register': str(ctx.register_path), 'code_segment': code_segment,
            'code_segment_size': len(segment_bytes), 'candidate_code_records': len(records),
            'summary': dict(summary), 'members': members_out,
            'tubench_summary': report['summary']}


def render(result, out=sys.stdout):
    w = out.write
    w(f"# {result['authority']}\n")
    if result.get('status') == 'COMPILE_OR_SELECTION_FAILED':
        w(f"FAILED: {result['error']}\n")
        return
    w(f"source {result['source']}  flags {' '.join(result['flags'])} ({result['flags_source']})\n")
    w('NAME\tTARGET\tEMITTED\tSTATE\tCOMPARISON\tFIELDS\tFLAGS\n')
    for e in result['members']:
        fields = ','.join(f'{k}:{v}' for k, v in sorted((e.get('fixups') or {}).get('categories', {}).items())
                          if k not in FIELD_OK)
        flags = ','.join(e['flags'] + e.get('mismatch_subflags', []))
        w(f"{e['name']}\t{e['target']['size']}\t{e['emitted']['size']}\t{e['primary_state']}\t"
          f"{e.get('comparison')}\t{fields or '-'}\t{flags or '-'}\n")
    w(f"summary {result['summary']}\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('source', help='candidate C source (single function or whole TU)')
    sel = ap.add_mutually_exclusive_group(required=True)
    sel.add_argument('--function')
    sel.add_argument('--members', help='comma-separated inventory names')
    sel.add_argument('--interval', nargs=2, type=lambda v: int(v, 0), metavar=('START', 'END'))
    sel.add_argument('--tu', help='tubench TU closure id')
    sel.add_argument('--object', help='objmap object id (whole object interval)')
    ap.add_argument('--flags', help='override compile flags, e.g. "/AM /Ox /Gs" (diagnostic)')
    ap.add_argument('--register', help='toolchain-hypotheses snapshot (default: evidence/...)')
    ap.add_argument('--symbol-receipts', help='symbol-closure receipts (hook, unreviewed); '
                    '`autosym` reads build/autosym (tools/autosym.py derive)')
    ap.add_argument('--record-proof', help='prefix-proof report or hook file marking RECORD_CLOSED_EXACT; '
                    '`auto` derives it with tools/prefix_proof.prove_source')
    ap.add_argument('--json', help='write full JSON result here')
    ap.add_argument('--keep', action='store_true', help='keep the compiler work directory')
    ap.add_argument('--no-detail', action='store_true', help='skip mismatch alignment detail')
    args = ap.parse_args()
    ctx = Context(register_path=args.register, symbol_receipts=args.symbol_receipts,
                  record_proof=args.record_proof, source=args.source)
    result = classify_source(args.source, function=args.function, members=args.members,
                             interval=tuple(args.interval) if args.interval else None, tu=args.tu,
                             object_id=args.object, flags=args.flags.split() if args.flags else None,
                             ctx=ctx, keep=args.keep, with_mismatch_detail=not args.no_detail)
    render(result)
    if args.json:
        Path(args.json).parent.mkdir(parents=True, exist_ok=True)
        Path(args.json).write_text(json.dumps(result, indent=1, default=str), encoding='utf-8')
        print(f'json: {args.json}')


if __name__ == '__main__':
    main()
