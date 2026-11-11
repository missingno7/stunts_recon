"""Prove a public-symbol rename batch, isolate failures, and publish atomically.

Provenance: adapted from gp_recon/tools/link/rename.py (reviewed plan,
before/after complete objects, spelling substitution, rollback), and the token
scanner in duke1_recon/tools/rename.py. Unlike those tools, comments are opaque
except a standalone symbol label. No byte oracle is generated from candidates.

Usage: python tools/rename.py PLAN [--dry-run] [--bisect] [--report FILE]
Without --bisect any rejected name aborts the entire transaction. With it the
maximal greedily compatible subset is re-proven together before publication.
--check checks an already applied plan and freshly builds the exact image.
All trials run in an isolated copy; only publication can touch canonical files.
Historical inventory identities (member.name, mapped_target.name, stable IDs,
inventory_name, evidence and paths) remain evidence, not emitted identifiers.
"""
from __future__ import annotations

import argparse
import base64
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time
import tokenize

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from common import atomic_bytes, json_bytes, require

IDENT = re.compile(r'[A-Za-z_][A-Za-z_0-9]*\Z')
WORDS = re.compile(r'[A-Za-z_$@?][A-Za-z_0-9$@?]*')
# Ordered alternatives: literals and comments must be consumed before tokens.
C_TOKEN = re.compile(r'(?m:^[ \t]*#[ \t]*include[ \t]*<[^>\r\n]*>)|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//(?:\\\r?\n|[^\r\n])*|[0-9][A-Za-z_0-9.]*|[A-Za-z_][A-Za-z_0-9]*|.', re.S)
ASM_TOKEN = re.compile(r'"(?:""|[^"\r\n])*"|\'(?:\'\'|[^\'\r\n])*\'|;[^\r\n]*|[A-Za-z_$@?][A-Za-z_0-9$@?]*|.', re.S)
C_RESERVED = set('''auto break case char const continue default do double else enum extern float
for goto if int long register return short signed sizeof static struct switch typedef union unsigned
void volatile while far near huge pascal cdecl interrupt asm _asm __asm _cdecl __cdecl _far __far
_near __near _huge __huge _pascal __pascal _interrupt __interrupt _loadds _saveregs _export
_cs _ds _es _ss'''.split())
ASM_RESERVED = set('''aaa aad aam aas adc add and arpl bound bsf bsr bt btc btr bts call cbw cdq
clc cld cli clts cmc cmp cmps cmpsb cmpsw cwd cwde daa das dec div enter esc hlt idiv imul in
inc ins insb insw int into iret ja jae jb jbe jc jcxz je jg jge jl jle jmp jna jnae jnb jnbe
jnc jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz lahf lar lds lea leave les lfs
lgdt lgs lidt lldt lmsw lock lods lodsb lodsw loop loope loopne loopnz loopz lsl lss ltr mov
movs movsb movsw movsx movzx mul neg nop not or out outs outsb outsw pop popa popad popf popfd
push pusha pushad pushf pushfd rcl rcr rep repe repne repnz repz ret retf retn rol ror sahf sal
sar sbb scas scasb scasw setc setz sgdt shl shld shr shrd sidt sldt smsw stc std sti stos stosb
stosw str sub test verr verw wait xchg xlat xlatb xor ax bx cx dx si di bp sp al ah bl bh cl ch
dl dh cs ds es ss fs gs eax ebx ecx edx esi edi ebp esp st byte word dword fword qword tbyte
ptr offset seg short near far high low length size type mask width mod shl shr eq ne lt le gt
ge dup this abs all assume at align catstr code comm comment common const data db dd df dq dt
dw else elseif endif end endm endp ends equ err err1 err2 errb errdef errdif errdifi erre erridn
erridni errnb errndef errnz even exitm extrn for forc group if if1 if2 ifb ifdef ifdif ifdifi
ife ifidn ifidni ifnb ifndef include includelib instr irp irpc label lengthof local macro model
name org page para proc public purge radix record rept segment size sizeOf stack struc struct
substr subtitle title type union while use16 use32 uses .8086 .186 .286 .386 fadd fsub fmul
fdiv fld fst fstp finit fninit fwait fnstsw fstsw fild fist fistp fxch fcom fcomp fcompp fchs
fabs fsqrt frndint fldz fld1 fldpi'''.lower().split())
ASM_RESERVED.update('''cmpsd insd iretd jecxz lodsd movsd outsd scasd stosd seto setno setb
setae sete setne setbe seta sets setns setp setnp setl setge setle setg cr0 cr2 cr3 dr0 dr1 dr2
dr3 dr6 dr7 tr6 tr7 f2xm1 faddp fbld fbstp fclex fcos fdecstp fdisi femms feni ffree fiadd
ficom ficomp fidiv fidivr fild fimul fincstp finit fist fistp fisub fisubr fldcw fldenv fldl2e
fldl2t fldlg2 fldln2 fmul fmulp fnclex fndisi fneni fnop fnsave fnstcw fnstenv fnstsw fpatan
fprem fprem1 fptan frstor fsave fscale fsetpm fsin fsincos fstcw fstenv fsubp fsubr fsubrp
ftst fucom fucomp fucompp fxam fxtract fyl2x fyl2xp1 fdivp fdivr fdivrp nothing readonly
private memory endstruc sizestr textequ fortran _based _self _segment _segname'''.split())
CONFIDENCE = {'low': 0, 'medium': 1, 'high': 2}
SOURCE_DIRS = ('src', 'asm', 'include', 'port')


def symbol_map(rows):
    mapping = {}
    for row in rows:
        old, new = row['old'], row['new']
        for a, b in ((old, new), ('_' + old, '_' + new),
                     (('_' + old)[:31], ('_' + new)[:31]),
                     (('_' + old)[:32], ('_' + new)[:32])):
            require(a not in mapping or mapping[a] == b, 'Ambiguous truncated old name: ' + a)
            mapping[a] = b
    return mapping


def label_comment(token, mapping):
    # Only `/* foo */`, `// foo: ...` and `; foo: ...`, never narrative mentions.
    prefix = 2 if token.startswith(('/*', '//')) else 1
    body = token[prefix:-2] if token.startswith('/*') else token[prefix:]
    match = re.fullmatch(r'(\s*)([A-Za-z_][A-Za-z_0-9]*)(\s*|:.*)', body, re.S)
    if match and match[2] in mapping:
        start = prefix + len(match[1])
        return token[:start] + mapping[match[2]] + token[start + len(match[2]):]
    return token


def replace_tokens(text, mapping, asm=False, *, labels=None):
    labels = mapping if labels is None else labels
    # C block comments span lines; do not tokenize each line independently.
    if not asm:
        return ''.join(mapping.get(t, t) if IDENT.fullmatch(t) else
                       label_comment(t, labels) if t.startswith(('/*', '//')) else t
                       for t in C_TOKEN.findall(text))
    out, block_delimiter = [], None
    for line in text.splitlines(keepends=True):
        block = re.match(r'^\s*comment\s+(\S)', line, re.I)
        if block_delimiter is not None:
            out.append(line)
            if block_delimiter in line:
                block_delimiter = None
        elif block:
            out.append(line)
            if block[1] not in line[block.end():]:
                block_delimiter = block[1]
        else:
            out.append(''.join(label_comment(t, labels) if t.startswith(';') else
                               mapping.get(t, t) for t in ASM_TOKEN.findall(line)))
    return ''.join(out)


def renamed(value, mapping):
    """Exact JSON strings/keys only; no substitution inside prose or file paths."""
    if isinstance(value, str):
        return mapping.get(value, value)
    if isinstance(value, list):
        return [renamed(x, mapping) for x in value]
    if isinstance(value, dict):
        keys = [mapping.get(k, k) for k in value]
        require(len(keys) == len(set(keys)), 'JSON key collision')
        return {mapping.get(k, k): renamed(v, mapping) for k, v in value.items()}
    return value


def canonical_files(root):
    return {p.relative_to(root).as_posix(): p.read_bytes()
            for folder in (*SOURCE_DIRS, 'recipes', 'layout')
            for p in sorted((root / folder).rglob('*'))
            if p.is_file() and '__pycache__' not in p.parts}


def make_changes(files, rows):
    mapping = symbol_map(rows)
    omf = {k: v for k, v in mapping.items() if k.startswith('_')}
    changes = {}
    for name, raw in files.items():
        path = Path(name)
        if path.parts[0] in SOURCE_DIRS and path.suffix.lower() in ('.c', '.h', '.asm', '.inc'):
            asm = path.suffix.lower() in ('.asm', '.inc')
            # C _foo is a different identifier from foo. Underscore decoration
            # belongs to the assembly/OMF namespace, never to C token matching.
            code_map = mapping if asm else {r['old']: r['new'] for r in rows}
            result = replace_tokens(raw.decode('latin1'), code_map, asm, labels=mapping).encode('latin1')
        elif path.suffix == '.json' and name != 'layout/oracle.lock.json':
            # Only decorated OMF names globally: plain names often identify
            # immutable inventory facts (recipe members and mapped_target).
            document = json.loads(raw)
            updated = renamed(document, omf)
            if name == 'layout/code-symbols.json':
                for old, new in omf.items():
                    if old in document['symbols'] and new in updated['symbols']:
                        binding = updated['symbols'][new]
                        if 'mapped_target' in binding:
                            # The reviewed plan authorizes a new spelling of
                            # the independently anchored target, not new facts.
                            binding['reviewed_alias'] = new
            if name == 'layout/names-registry.json':
                for row in rows:
                    address = str(int(str(row['address']), 0))
                    entry = updated['names'].setdefault(address, {
                        'inventory_name': row['old'], 'kind': 'code' if row.get('kind') != 'data' else 'data',
                        'basis': 'Explicit rename plan; pinned-object and exact-image proof required'})
                    entry['name'] = row['new']
            result = json_bytes(updated) if updated != document else raw
        else:
            result = raw
        if result != raw:
            changes[name] = result
    return changes


def active_recipes(files):
    manifest = json.loads(files['layout/manifest.json'])
    result = {}
    for owner in manifest['owners']:
        if owner.get('recipe') and owner['kind'].startswith('MATCHING_'):
            key = owner['recipe']
            result.setdefault(key, {'recipe': json.loads(files[key]), 'owners': []})['owners'].append(owner['id'])
    return result


def definition_addresses(item, files, old):
    """Addresses are independently grounded, never taken on trust from a plan."""
    recipe = item['recipe']
    spellings = {old, '_' + old, ('_' + old)[:31], ('_' + old)[:32]}
    found = set()
    for member in recipe.get('members', [recipe]):
        if member.get('public') in spellings and 'start' in member:
            found.add(member['start'])
    for public in recipe.get('object_declarations', {}).get('publics', []):
        if public['name'] in spellings and public['segment'] == recipe.get('object_segment'):
            found.add(recipe['start'] + public['offset'])
    # Alias bindings retain their mapped_target inventory name and address.
    for table in ('layout/code-symbols.json', 'layout/data-symbols.json'):
        for symbol, binding in json.loads(files.get(table, b'{}')).get('symbols', {}).items():
            if symbol in spellings:
                if 'start' in binding.get('mapped_target', {}):
                    found.add(binding['mapped_target']['start'])
                elif 'load_address' in binding:
                    found.add(binding['load_address'])
    return found


def preflight(rows, files, namespace=None):
    """Refusals are independent of the compiler and do not mutate inputs."""
    present = set(namespace or ())
    for raw in files.values():
        present.update(WORDS.findall(raw.decode('latin1')))
    registry = json.loads(files['layout/names-registry.json'])['names']
    opaque = {}
    # Native adapters embed source signatures in Python string literals. The
    # requested token policy forbids rewriting these strings; require explicit
    # adapter work instead of publishing a rename that breaks the shared port.
    for path, raw in files.items():
        if not path.startswith('port/') or not path.endswith('.py'):
            continue
        for token in tokenize.generate_tokens(io.StringIO(raw.decode('utf-8-sig')).readline):
            if token.type != tokenize.STRING:
                continue
            text = re.sub(r'\\[bBAZ]', ' ', token.string)
            for row in rows:
                if re.search(r'(?<![A-Za-z_0-9])' + re.escape(row['old']) + r'(?![A-Za-z_0-9])', text):
                    opaque.setdefault(row['old'], f'{path}:{token.start[0]}')
    recipes = active_recipes(files)
    publics = set()
    for raw in files.values():
        publics.update(w for w in WORDS.findall(raw.decode('latin1')) if w.startswith('_'))
    names, truncs = {}, {}
    for row in rows:
        names.setdefault(row['new'], []).append(row['old'])
        truncs.setdefault(('_' + row['new'])[:31], []).append(row['old'])
    good, refused = [], []
    for row in rows:
        old, new = row['old'], row['new']
        reason = None
        if not IDENT.fullmatch(old) or not IDENT.fullmatch(new) or old == new:
            reason = 'invalid or unchanged identifier'
        elif new in C_RESERVED or new.lower() in ASM_RESERVED:
            reason = 'reserved C/assembler word'
        elif new in present or '_' + new in present or len(names[new]) > 1:
            reason = 'new name already exists'
        elif any(p[:31] == ('_' + new)[:31] for p in publics) or len(truncs[('_' + new)[:31]]) > 1:
            reason = '31-character public truncation collision'
        elif len(new) > 30:
            reason = 'C/ASM public spellings diverge beyond 30 identifier characters'
        elif new in {r['old'] for r in rows}:
            reason = 'rename chains/swaps are not permitted'
        else:
            address = str(int(str(row['address']), 0))
            entry = registry.get(address)
            if entry and entry['name'] != old:
                reason = 'registry address has a different canonical name: ' + entry['name']
            # The source-bearing owner is the proof unit, never a runtime blob.
            defining = [r for r in recipes.values()
                        if row.get('owner') in r['owners'] or r['recipe'].get('id') == row.get('owner')]
            if not defining:
                reason = 'owner has no rebuildable defining source (pinned runtime or unknown owner)'
            elif not any(re.search(r'(?<![\w$@?])_?' + re.escape(old) + r'(?![\w$@?])',
                                  files[r['recipe']['source']].decode('latin1')) for r in defining):
                reason = 'old symbol is absent from the defining owner'
            elif int(address) not in set().union(*(definition_addresses(r, files, old) for r in defining)):
                private = [r for r in defining if r['recipe'].get('kind') == 'asm' and
                           r['recipe']['start'] <= int(address) < r['recipe']['end'] and
                           re.search(r'^\s*' + re.escape(old) + r'\s+proc\b',
                                     files[r['recipe']['source']].decode('ascii'), re.M | re.I)]
                if private and entry and entry.get('object_declared') is False:
                    row = {**row, 'grounding': 'pinned-MASM-listing-required'}
                elif private:
                    reason = 'private PROC lacks a grounded registry inventory target; separate evidence review is required'
                else:
                    reason = 'plan address is not grounded by the defining recipe or symbol binding'
            bindings = json.loads(files.get('layout/code-symbols.json', b'{"symbols":{}}'))['symbols']
            if any(any(field in bindings.get(public, {}) for field in ('module_entry', 'entry_proof'))
                   for public in ('_' + old, ('_' + old)[:31], ('_' + old)[:32])):
                reason = 'existing module/partial-entry binding proof requires the original emitted spelling'
            if old in opaque:
                reason = 'native adapter has an opaque string binding requiring explicit review: ' + opaque[old]
        (refused if reason else good).append({**row, 'status': 'refused', 'reason': reason} if reason else row)
    return good, refused


def object_view(obj):
    # local_symbol_records contains diagnostic file offsets and body hashes.
    # Both change mechanically with permitted name lengths. Local declarations
    # are compared here, and complete ordered LEXTDEF/LPUBDEF bodies below.
    fields = ('segments', 'segment_lengths', 'segment_defs', 'groups', 'publics', 'externals',
              'linker_fixups', 'local_publics', 'local_externals', 'external_scopes', 'communals',
              'comments', 'debug_segments')
    return {key: getattr(obj, key, None) for key in fields}


def first_difference(before, after, path=''):
    """Keep list order and full extents; diagnostics never decide acceptance."""
    if type(before) is not type(after):
        return {'record': path, 'before': repr(before)[:300], 'after': repr(after)[:300]}
    if isinstance(before, dict):
        if before.keys() != after.keys():
            return {'record': path + '.keys', 'before': list(before), 'after': list(after)}
        for key in before:
            diff = first_difference(before[key], after[key], path + '.' + str(key))
            if diff:
                return diff
    elif isinstance(before, (list, tuple, bytes, bytearray)):
        for index, (a, b) in enumerate(zip(before, after)):
            diff = first_difference(a, b, f'{path}[{index}]')
            if diff:
                return diff
        if len(before) != len(after):
            return {'record': path + '.length', 'before': len(before), 'after': len(after)}
    elif before != after:
        return {'record': path, 'before': before, 'after': after}
    return None


def normalized_records(raw, mapping):
    """Normalize only declared names in OMF; every other record byte is kept.

    Length/checksum bytes are derived framing, checked by read_object first.
    EXTDEF/PUBDEF/COMDEF/LNAMES field order and FIXUPP index order are retained.
    In particular data payloads, debug records and COMENT are never text-replaced.
    """
    from omf import OmfReader
    result = []
    for kind, body in OmfReader.records(raw):
        at, parts = 0, []
        def index():
            nonlocal at
            start = at
            _, at = OmfReader._index(body, at)
            parts.append(body[start:at])
        def name():
            nonlocal at
            size = body[at]
            text = body[at + 1:at + 1 + size].decode('ascii')
            parts.append(mapping.get(text, text))
            at += size + 1
        if kind in (0x90, 0xB6):
            index()
            segment, _ = OmfReader._index(body, at)
            index()
            if segment == 0:
                parts.append(body[at:at+2]); at += 2
            while at < len(body):
                name()
                parts.append(body[at:at+2]); at += 2
                index()
        elif kind in (0x8C, 0xB4, 0x96, 0xB0):
            while at < len(body):
                name()
                if kind in (0x8C, 0xB4, 0xB0):
                    index()
                if kind == 0xB0:
                    start = at
                    category = body[at]; at += 1
                    require(category in (0x61, 0x62), 'Unsupported COMDEF')
                    for _ in range(2 if category == 0x61 else 1):
                        _, at = OmfReader._communal_value(body, at)
                    parts.append(body[start:at])
        else:
            parts = [body]
        result.append((f'OMF_{kind:02X}', parts))
    return result


def compare_objects(before, after, mapping):
    # Payload bytes and every semantic declaration/fixup first, then complete
    # ordered records. Never sort, trim or compare just the owned CODE extent.
    diff = first_difference(renamed(object_view(before[0]), mapping), object_view(after[0]), 'object')
    before_records = normalized_records(before[1], mapping)
    after_records = normalized_records(after[1], {})
    record_diff = first_difference(before_records, after_records, 'records')
    if record_diff:
        match = re.match(r'records\[(\d+)\]', record_diff['record'])
        if match:
            index = int(match[1])
            record_diff.update(index=index, ordinal=index + 1,
                               before_kind=before_records[index][0], after_kind=after_records[index][0])
    if diff or record_diff:
        return {'semantic': diff, 'omf': record_diff}
    return None


def compile_object(recipe, source):
    # Adapted from promote._cosmetic_compile: retain its pinned flags,
    # communal/sparse policy and registered flag-control proof. Also retain
    # the receipt so private ASM entries can be grounded by the fresh listing.
    from compiler import compile_source, verify_toolchain
    from assembler import assemble_source
    from object_probe import recipe_sparse_zero
    from communal_unit import recipe_declarations
    from object_flags import recipe_flags, object_control_flags, same_object
    from common import identity
    declared = recipe_declarations(recipe)
    communals = None if declared is None else [n for n, _ in declared]
    if recipe.get('kind', 'c') == 'asm':
        require(recipe['profile'] == 'masm510-game', 'ASM profile differs')
        require(recipe['assembler_flags'] == verify_toolchain(recipe['profile'])[0]['flags'],
                'ASM flags differ from pinned profile')
        obj, receipt = assemble_source(source, recipe['profile'], communals=communals)
    else:
        sparse = recipe_sparse_zero(recipe)
        obj, receipt = compile_source(source, recipe['profile'], recipe_flags(recipe),
                                      sparse_zero=sparse, communals=communals)
        control = object_control_flags(recipe)
        if control is not None:
            controlled, control_receipt = compile_source(source, recipe['profile'], control,
                                                          sparse_zero=sparse, communals=communals)
            require(same_object(controlled, obj), 'Object differs under registered control flags')
            receipt['object_flag_control'] = control_receipt
    raw = (Path(receipt['work_directory']) / 'UNIT.OBJ').read_bytes()
    require(identity(raw) == receipt['object'], 'Full OBJ receipt identity differs')
    return obj, raw, receipt


def check_private_listing(rows, item, compiled, after=False):
    for row in rows:
        if row.get('grounding') != 'pinned-MASM-listing-required' or row['owner'] not in item['owners']:
            continue
        symbol = row['new'] if after else row['old']
        listing = (Path(compiled[2]['work_directory']) / 'UNIT.LST').read_text(encoding='latin1')
        offsets = re.findall(r'^\s*([0-9A-Fa-f]{4,8})\s+' + re.escape(symbol) + r'\s+proc\b',
                             listing, re.M | re.I)
        require(len(offsets) == 1 and item['recipe']['start'] + int(offsets[0], 16) == int(str(row['address']), 0),
                'Private entry address differs from pinned MASM listing: ' + symbol)


class NativeBuildFailure(ValueError):
    """A historical rename cannot yet be represented by the native adapters."""


class RegistryFailure(ValueError):
    """Candidate names do not preserve the repository's grounded registry."""


class ProofEngine:
    """One scratch copy, cached fresh baselines, deterministic subset trials."""
    def __init__(self, root, files, compiler=compile_object, image=None):
        self.root, self.files, self.compiler = root, files, compiler
        self.recipes = active_recipes(files)
        self.baselines, self.cache, self.trials = {}, {}, []
        self.current = {}
        self.image = image

    def stage(self, changes):
        for name in self.current.keys() | changes.keys():
            atomic_bytes(self.root / name, changes.get(name, self.files[name]))
        self.current = changes

    def trial(self, rows, whole=False):
        started = time.monotonic()
        changes = make_changes(self.files, rows)
        affected = [key for key, r in self.recipes.items()
                    if key in changes or r['recipe']['source'] in changes or
                    any(c['path'] in changes for c in r['recipe'].get('preprocessor_closure', []) +
                        r['recipe'].get('include_closure', []) if 'path' in c)]
        if any(key not in self.baselines for key in affected):
            self.stage({})
        failures = []
        for key in affected:
            if key not in self.baselines:
                recipe = self.recipes[key]['recipe']
                try:
                    self.baselines[key] = self.compiler(recipe, self.files[recipe['source']])
                except (ValueError, OSError) as error:
                    self.baselines[key] = {'owner': self.recipes[key]['owners'], 'recipe': key,
                                           'record': 'before-compile', 'error': str(error), 'category': 'proof-error'}
            if isinstance(self.baselines[key], dict):
                failures.append(self.baselines[key])
                continue
            try:
                check_private_listing(rows, self.recipes[key], self.baselines[key])
            except ValueError as error:
                failures.append({'owner': self.recipes[key]['owners'], 'recipe': key,
                                 'record': 'baseline-MASM-listing', 'error': str(error), 'category': 'refused'})
        self.stage(changes)
        # Included source hashes are recomputed by the real frontend, never
        # copied from the previous receipt or inferred from changed tokens.
        from promote import _cosmetic_closure
        for key in affected:
            item = self.recipes[key]
            recipe = json.loads(changes.get(key, self.files[key]))
            source = changes.get(recipe['source'], self.files[recipe['source']])
            field, closure = _cosmetic_closure(recipe, source)
            if recipe.get(field, []) != closure:
                recipe[field] = closure
                changes[key] = json_bytes(recipe)
        self.stage(dict(changes))
        for key in affected:
            if isinstance(self.baselines[key], dict):
                continue
            recipe = json.loads(changes.get(key, self.files[key]))
            source = changes.get(recipe['source'], self.files[recipe['source']])
            fingerprint = (key, hashlib.sha256(source + json_bytes(recipe) +
                           b''.join(changes[n] for n in sorted(changes) if n.startswith('include/'))).hexdigest())
            if fingerprint not in self.cache:
                try:
                    self.cache[fingerprint] = self.compiler(recipe, source)
                except (ValueError, OSError) as error:
                    sensitive = getattr(error, 'category', None) in ('COMPILER_ERROR', 'ASSEMBLER_ERROR')
                    self.cache[fingerprint] = {'owner': self.recipes[key]['owners'], 'recipe': key,
                                               'record': 'after-compile', 'error': str(error),
                                               'category': 'name-sensitive' if sensitive else 'proof-error'}
            after = self.cache[fingerprint]
            if isinstance(after, dict):
                failures.append(after)
            else:
                try:
                    check_private_listing(rows, self.recipes[key], after, after=True)
                except ValueError as error:
                    failures.append({'owner': self.recipes[key]['owners'], 'recipe': key,
                                     'record': 'renamed-MASM-listing', 'error': str(error), 'category': 'refused'})
                diff = compare_objects(self.baselines[key], after, symbol_map(rows))
                if diff:
                    failures.append({'owner': self.recipes[key]['owners'], 'recipe': key,
                                     'category': 'name-sensitive', **diff})
        image_result = None
        if whole and not failures:
            try:
                if self.image:
                    image_result = self.image()
                else:
                    from build_exact import build
                    from bss_link import canonical_gate
                    from oracle import verify
                    verify(write=True)
                    image_result = build(publish=False)
                    require(image_result['status'] == 'HYBRID_EXACT', 'Image is not exact')
                    image_result = {'status': image_result['status'],
                                    'relocation_count': image_result['relocation_count'],
                                    'bss_real_link': canonical_gate()}
                    registry_log = self.root / 'build/rename-registry-check.log'
                    registry_tests = 'tests.test_names_registry_and_far_pointer_tables.NamesRegistryTests.'
                    with registry_log.open('wb') as log:
                        registry = subprocess.run([sys.executable, '-m', 'unittest',
                            registry_tests + 'test_one_name_per_address_and_code_rows_have_grounded_targets',
                            registry_tests + 'test_accepted_objects_no_longer_use_superseded_spellings'],
                            cwd=self.root, stdin=subprocess.DEVNULL, stdout=log, stderr=subprocess.STDOUT)
                    if registry.returncode:
                        raise RegistryFailure('Grounded registry consistency failed; see ' + str(registry_log))
                    image_result['registry_check'] = {'status': 'PASS', 'log': str(registry_log)}
                    if (self.root / 'port/build.py').is_file():
                        log_path = self.root / 'build/rename-native-build.log'
                        with log_path.open('wb') as log:
                            native = subprocess.run([sys.executable, str(self.root / 'port/build.py'), 'build',
                                                     '--assets-source', os.environ['STUNTS_ASSET_DIR']],
                                                    cwd=self.root, stdin=subprocess.DEVNULL,
                                                    stdout=log, stderr=subprocess.STDOUT)
                        if native.returncode:
                            errors = [line for line in log_path.read_text(errors='replace').splitlines()
                                      if 'failed:' in line.lower() or 'error:' in line.lower()]
                            raise NativeBuildFailure('; '.join(errors[:3]) + '; see ' + str(log_path))
                        image_result['native_build'] = {'status': 'PASS', 'log': str(log_path)}
            except Exception as error:
                failures.append({'owner': getattr(error, 'owner_id', None),
                                 'recipe': getattr(error, 'owner_recipe', None),
                                 'record': 'whole-image/BSS-link', 'error': str(error),
                                 'phase': ('native-build' if isinstance(error, NativeBuildFailure) else
                                           'registry' if isinstance(error, RegistryFailure) else 'image/BSS'),
                                 'category': 'refused' if isinstance(error, (NativeBuildFailure, RegistryFailure)) else 'proof-error'})
        result = {'names': [r['old'] for r in rows], 'affected_recipes': affected,
                  'failures': failures, 'seconds': round(time.monotonic() - started, 3),
                  'whole_image': image_result}
        self.trials.append(result)
        atomic_bytes(self.root / f'build/rename-trials/{len(self.trials):04d}.json', json_bytes(result))
        print(f"Trial {len(self.trials)}: {len(rows)} names, {len(affected)} owners, "
              f"{len(failures)} failures, {result['seconds']}s", flush=True)
        for failure in failures:
            if failure.get('record') == 'whole-image/BSS-link':
                print('  Whole-image failure: ' + json.dumps(failure), flush=True)
        return result

    def isolate(self, rows):
        """Bisect failed subsets; retain failure evidence for singleton names."""
        result = self.trial(rows)
        if not result['failures']:
            return list(rows), []
        if len(rows) == 1:
            status = ('refused' if any(f['category'] == 'refused' for f in result['failures']) else
                      'name-sensitive' if all(f['category'] == 'name-sensitive' for f in result['failures']) else 'proof-error')
            return [], [{**rows[0], 'status': status, 'failures': result['failures']}]
        middle = len(rows) // 2
        left, failed_left = self.isolate(rows[:middle])
        right, failed_right = self.isolate(rows[middle:])
        return left + right, failed_left + failed_right

    def compatible(self, pending, accepted=None, whole=False):
        """Preserve good names even when individually clean names interact.

        Grow the retained set in plan order. A rejected name records the other
        accepted names required to reproduce the failure; it is not described
        as a failing singleton. This finds a maximal compatible set, not an
        optimal maximum-cardinality solution.
        """
        accepted = list(accepted or [])
        if not pending:
            return accepted, []
        result = self.trial(accepted + pending, whole=whole)
        if not result['failures']:
            return accepted + pending, []
        if len(pending) == 1:
            status = ('refused' if any(f['category'] == 'refused' for f in result['failures']) else
                      'name-sensitive' if all(f['category'] == 'name-sensitive' for f in result['failures']) else 'proof-error')
            return accepted, [{**pending[0], 'status': status, 'failures': result['failures'],
                               'with_names': [r['old'] for r in accepted]}]
        middle = len(pending) // 2
        kept, left = self.compatible(pending[:middle], accepted, whole)
        kept, right = self.compatible(pending[middle:], kept, whole)
        return kept, left + right


def publish(changes, expected, verify):
    """Use the same durable journal and OS publisher lock as promote.py."""
    import transaction
    with transaction.exclusive():
        transaction.ensure_consistent()
        require(canonical_files(ROOT) == expected, 'Canonical files changed; no publication')
        with transaction.publishing():
            try:
                journal = prepare_publication(changes)
                transaction.apply(journal)
                verify()
                transaction.invalidate_receipts()
                transaction.finish()
            except BaseException:
                rollback_publication()
                raise


def publication_path(relative):
    path = (ROOT / relative).resolve()
    require(path.is_relative_to(ROOT.resolve()) and '\\' not in relative and
            relative.split('/')[0] in (*SOURCE_DIRS, 'recipes', 'layout') and
            relative == path.relative_to(ROOT.resolve()).as_posix() and
            path != (ROOT / 'layout/oracle.lock.json').resolve(),
            'Invalid rename publication path: ' + relative)
    return path


def prepare_publication(changes):
    """The shared journal also covers headers for this publisher.

    transaction.prepare/rollback exclude include/ and port/: ordinary promotion
    cannot publish those sources. Keep its implementation for that normal case.
    A marked rename journal extends the path policy by include/ and port/;
    other publishers see the pending journal and refuse until rename --recover.
    """
    import transaction
    if not any(n.startswith(('include/', 'port/')) for n in changes):
        return transaction.prepare(changes)
    transaction.ensure_consistent()
    rows = []
    encode = lambda raw: None if raw is None else base64.b64encode(raw).decode('ascii')
    for relative, raw in changes.items():
        path = publication_path(relative)
        rows.append({'path': relative, 'before': encode(transaction.file_bytes(path)), 'after': encode(raw)})
    atomic_bytes(transaction.journal_path(), json_bytes({'schema': 'rename-extended-sources-v1', 'files': rows}))
    return rows


def rollback_publication():
    import transaction
    journal = transaction.journal_path()
    if not journal.exists():
        return
    document = json.loads(journal.read_bytes())
    if document.get('schema') not in ('rename-with-headers-v1', 'rename-extended-sources-v1'):
        return transaction.rollback()
    decoded = []
    for row in document['files']:
        path = publication_path(row['path'])
        before = None if row['before'] is None else base64.b64decode(row['before'], validate=True)
        after = None if row['after'] is None else base64.b64decode(row['after'], validate=True)
        current = transaction.file_bytes(path)
        require(current in (before, after), 'Recovery preserves conflicting edit: ' + row['path'])
        decoded.append((path, before, current))
    transaction.invalidate_receipts()
    for path, before, current in reversed(decoded):
        if current != before:
            if before is None:
                path.unlink()
            else:
                atomic_bytes(path, before)
    journal.unlink()


def check_applied(rows, files):
    reverse = [{'old': r['new'], 'new': r['old'], **{k: v for k, v in r.items() if k not in ('old', 'new')}} for r in rows]
    require(not make_changes(files, rows), 'Plan is absent or partially applied')
    require(bool(make_changes(files, reverse)), 'Applied symbols are absent')
    registry = json.loads(files['layout/names-registry.json'])['names']
    for row in rows:
        require(registry.get(str(int(str(row['address']), 0)), {}).get('name') == row['new'],
                'Registry does not contain applied name: ' + row['new'])
        require(sum(r['name'] == row['new'] for r in registry.values()) == 1,
                'Applied name has multiple registry addresses: ' + row['new'])


def worker(request):
    rows, bisect = request['rows'], request['bisect']
    files = canonical_files(ROOT)
    engine = ProofEngine(ROOT, files)
    if request.get('check'):
        check_applied(rows, files)
        clean, failed = rows, []
    elif bisect:
        clean, failed = engine.isolate(rows) if rows else ([], [])
    else:
        trial = engine.trial(rows)
        clean, failed = ([], [{'old': r['old'], 'new': r['new'], 'status': 'batch-aborted',
                              'failures': trial['failures']} for r in rows]) if trial['failures'] else (rows, [])
    # Individual halves passing is insufficient: prove the retained union.
    if clean:
        final = engine.trial([] if request.get('check') else clean, whole=True)
        if final['failures']:
            if bisect and not request.get('check'):
                # Bisection of the failed union is also required: successful
                # disjoint halves can have interactions with each other.
                whole_failure = all(f.get('record') == 'whole-image/BSS-link' for f in final['failures'])
                clean, interactions = engine.compatible(clean, whole=whole_failure)
                failed.extend(interactions)
                final = engine.trial(clean, whole=True) if clean else None
                if final and final['failures'] and not whole_failure:
                    clean, interactions = engine.compatible(clean, whole=True)
                    failed.extend(interactions)
                    final = engine.trial(clean, whole=True) if clean else None
            if final and final['failures']:
                failed.extend({**r, 'status': 'proof-error', 'failures': final['failures']} for r in clean)
                clean = []
    else:
        final = None
    changes = engine.current if clean and not request.get('check') else {}
    output = {'results': [{**r, 'status': 'clean'} for r in clean] + failed,
              'trials': engine.trials, 'final': final, 'changes': sorted(changes)}
    atomic_bytes(Path(request['output']), json_bytes(output))


def tracked_snapshot(root):
    command = subprocess.run(['git', 'ls-files', '-z'], cwd=root, capture_output=True, check=True)
    return {name: (root / name).read_bytes() for name in command.stdout.decode().split('\0') if name}


def run(args):
    started = time.monotonic()
    if args.report:
        require(args.report.resolve().is_relative_to((ROOT / 'build').resolve()) and
                args.report.resolve() != args.plan.resolve(), 'Reports must be separate scratch files under build/')
    rows = json.loads(args.plan.read_text(encoding='utf-8-sig'))
    require(isinstance(rows, list) and all(isinstance(r, dict) and {'old', 'new', 'address', 'owner'} <= r.keys()
                                         for r in rows), 'Plan must be a list of old/new/address/owner rows')
    require(len({r['old'] for r in rows}) == len(rows), 'Duplicate old name in plan')
    if args.only:
        require(set(args.only) <= {r['old'] for r in rows} | {r['new'] for r in rows}, 'Unknown --only name')
        rows = [r for r in rows if r['old'] in args.only or r['new'] in args.only]
    rows = [r for r in rows if CONFIDENCE.get(r.get('confidence', 'high'), -1) >= CONFIDENCE[args.min_confidence]]
    require(rows, 'No selected renames')
    expected = canonical_files(ROOT)
    tracked = tracked_snapshot(ROOT)
    # Untracked tool/tests are copied too, without any git index operation.
    for folder in ('tools', 'tests'):
        for path in (ROOT / folder).rglob('*'):
            if path.is_file() and '__pycache__' not in path.parts:
                tracked[path.relative_to(ROOT).as_posix()] = path.read_bytes()
    namespace = set()
    for raw in tracked.values():
        namespace.update(WORDS.findall(raw.decode('latin1')))
    if args.check:
        check_applied(rows, expected)
        good, refused = rows, []
    else:
        good, refused = preflight(rows, expected, namespace)
    work_root = ROOT / 'build/workers/renametool'
    work_root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='run-', dir=work_root))
    stage = work / 'tree'
    for name, raw in {**tracked, **expected}.items():
        if Path(name).parts[0] in (*SOURCE_DIRS, 'recipes', 'layout', 'tools', 'tests', 'evidence'):
            path = stage / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(raw)
    env = dict(os.environ, STUNTS_BUILD_WORKERS='1', STUNTS_ASSET_DIR=str(ROOT / 'assets'),
               NUKED_OPL3_ROOT=os.environ.get('NUKED_OPL3_ROOT', str(ROOT / 'build/dependencies/nuked-opl3')),
               GIT_DIR=str(ROOT / '.git'), GIT_WORK_TREE=str(stage),
               PYTHONPATH=os.pathsep.join([str(ROOT / 'build/python'), os.environ.get('PYTHONPATH', '')]))
    output = work / 'worker.json'
    request = work / 'request.json'
    atomic_bytes(request, json_bytes({'rows': good if (args.bisect or not refused or args.check) else [],
                                     'bisect': args.bisect, 'check': args.check, 'output': str(output)}))
    with (work / 'proof.log').open('wb') as log:
        result = subprocess.run([sys.executable, str(stage / 'tools/rename.py'), '--_worker', str(request)],
                                cwd=stage, env=env, stdin=subprocess.DEVNULL,
                                stdout=log, stderr=subprocess.STDOUT)
    print('Scratch proof log: ' + str(work / 'proof.log'))
    if result.returncode:
        print('\n'.join((work / 'proof.log').read_text(errors='replace').splitlines()[-10:]))
    require(result.returncode == 0 and output.is_file(), 'Scratch proof failed: ' + str(work / 'proof.log'))
    report = json.loads(output.read_bytes())
    report['results'].extend(refused)
    reported = {r['old'] for r in report['results']}
    report['results'].extend({**r, 'status': 'batch-aborted', 'reason': 'Another name was refused'}
                             for r in rows if r['old'] not in reported)
    report['results'].sort(key=lambda r: next(i for i, x in enumerate(rows) if x['old'] == r['old']))
    current = canonical_files(ROOT)
    changed = [n for n in current.keys() | expected.keys() if current.get(n) != expected.get(n)]
    require(not changed, 'Canonical files changed during proof; discard this run: ' + ', '.join(sorted(changed)))
    require(all((ROOT / name).read_bytes() == raw for name, raw in tracked.items()),
            'Tracked inputs changed during proof')
    clean = [r for r in report['results'] if r['status'] == 'clean']
    if not args.dry_run and not args.check and clean:
        require(args.bisect or len(clean) == len(rows), 'Cannot publish partial batch without --bisect')
        changes = {name: (stage / name).read_bytes() for name in report['changes']}
        def verify():
            from build_exact import build
            from bss_link import canonical_gate
            from oracle import verify as verify_oracle
            os.environ['STUNTS_BUILD_WORKERS'] = '1'
            verify_oracle(write=True)
            require(build(publish=False, allow_pending=True)['status'] == 'HYBRID_EXACT', 'Canonical image differs')
            canonical_gate()
        publish(changes, expected, verify)
    report.update(dry_run=args.dry_run, check=args.check, scratch=str(work),
                  seconds=round(time.monotonic() - started, 3),
                  totals={s: sum(r['status'] == s for r in report['results'])
                          for s in ('clean', 'name-sensitive', 'refused', 'proof-error', 'batch-aborted')})
    destination = args.report or work / 'report.json'
    atomic_bytes(destination, json_bytes(report))
    print(json.dumps({'totals': report['totals'], 'seconds': report['seconds'], 'report': str(destination)}))
    return 0 if len(clean) == len(rows) or args.bisect and not report['totals']['proof-error'] else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('plan', type=Path, nargs='?')
    parser.add_argument('--only', nargs='+')
    parser.add_argument('--min-confidence', choices=tuple(CONFIDENCE), default='low')
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--dry-run', action='store_true')
    parser.add_argument('--bisect', action='store_true', help='isolate failures and re-prove the retained union')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--recover', action='store_true', help='recover the shared publication journal')
    parser.add_argument('--_worker', type=Path, help=argparse.SUPPRESS)
    args = parser.parse_args()
    try:
        if args._worker:
            worker(json.loads(args._worker.read_bytes()))
            return 0
        if args.recover:
            import transaction
            with transaction.exclusive():
                rollback_publication()
                transaction._heal_generation()
            return 0
        require(args.plan is not None, 'A plan is required')
        return run(args)
    except (ValueError, OSError, subprocess.SubprocessError) as error:
        print('Rename refused: ' + str(error), file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
