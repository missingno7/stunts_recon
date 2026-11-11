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
Plans are arrays of old/new rows. Function rows use address/owner. Globals use
kind=global (address/owner optional for declaration-only aliases); structure
fields use kind=field and struct, with optional asm_old/asm_new spellings.
File rows use kind=c-unit or file, old/new repository source paths. Assembly
module rows use kind=asm-module, old/new owner IDs, source/new_source paths.
Moves update full path references in recipes/manifest/tools/tests/layout/evidence;
the oracle lock and port side are immutable to this tool. Inventory member and
mapped_target names stay historical facts. Reports list old-name port occurrences.
"""
from __future__ import annotations

import argparse
import base64
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time

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
SOURCE_DIRS = ('src', 'asm', 'include')
STATE_DIRS = (*SOURCE_DIRS, 'recipes', 'layout', 'tools', 'tests', 'evidence')
MOVE_KINDS = {'file', 'c-unit', 'asm-module'}

def row_id(row):
    return (row.get('kind'), row.get('struct'), row['old'])

def symbol_rows(rows):
    return [r for r in rows if r.get('kind') not in MOVE_KINDS | {'field'}]

def file_moves(rows):
    return {r.get('source', r['old']): r.get('new_source', r['new'])
            for r in rows if r.get('kind') in MOVE_KINDS}



def symbol_map(rows):
    mapping = {}
    for row in symbol_rows(rows):
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


def renamed_owners(value, mapping, field=None):
    if isinstance(value, dict):
        return {mapping.get(k, k): renamed_owners(v, mapping, k) for k, v in value.items()}
    if isinstance(value, list):
        return [renamed_owners(v, mapping, field) for v in value]
    if isinstance(value, str):
        if field in ('id', 'owner', 'owner_id', 'code_owner', 'source_owner', 'owners', 'declaring_owners', 'declaring_owner', 'parent') and value in mapping:
            return mapping.get(value, value)
        for old, new in mapping.items():
            if value == old + ':_BSS':
                return new + ':_BSS'
    return value


def canonical_files(root):
    return {p.relative_to(root).as_posix(): p.read_bytes()
            for folder in STATE_DIRS
            for p in sorted((root / folder).rglob('*'))
            if p.is_file() and '__pycache__' not in p.parts}


def make_changes(files, rows):
    mapping = symbol_map(rows)
    omf = {k: v for k, v in mapping.items() if k.startswith('_')}
    moves = file_moves(rows)
    path_spellings = dict(moves)
    for old, new in moves.items():
        path_spellings[old.replace('/', '\\')] = new.replace('/', '\\')
        path_spellings[old.replace('/', '\\\\')] = new.replace('/', '\\\\')
        if Path(old).name != Path(new).name:
            require(old not in files or sum(Path(n).name.casefold() == Path(old).name.casefold() for n in files if n.startswith(('src/', 'asm/'))) == 1,
                    'Ambiguous source basename in path references: ' + Path(old).name)
            path_spellings[Path(old).name] = Path(new).name
    owners = {r['old']: r['new'] for r in rows if r.get('kind') == 'asm-module'}
    fields = [r for r in rows if r.get('kind') == 'field']
    changes = {}
    for name, raw in files.items():
        path = Path(name)
        if name.startswith(('port/', 'tools/porting/', 'docs/porting/')):
            continue
        if path.parts[0] in SOURCE_DIRS and path.suffix.lower() in ('.c', '.h', '.asm', '.inc'):
            asm = path.suffix.lower() in ('.asm', '.inc')
            # C _foo is a different identifier from foo. Underscore decoration
            # belongs to the assembly/OMF namespace, never to C token matching.
            code_map = mapping if asm else {r['old']: r['new'] for r in symbol_rows(rows)}
            text = replace_tokens(raw.decode('latin1'), code_map, asm, labels=mapping)
            result = replace_fields(text, fields, asm).encode('latin1')
        elif path.suffix == '.json' and name != 'layout/oracle.lock.json' and (omf or moves or owners):
            # Only decorated OMF names globally: plain names often identify
            # immutable inventory facts (recipe members and mapped_target).
            document = json.loads(raw)
            updated = renamed(document, {**omf, **moves})
            updated = renamed_owners(updated, owners)
            if name == 'layout/code-symbols.json':
                for old, new in omf.items():
                    if old in document['symbols'] and new in updated['symbols']:
                        binding = updated['symbols'][new]
                        if 'mapped_target' in binding:
                            # The reviewed plan authorizes a new spelling of
                            # the independently anchored target, not new facts.
                            binding['reviewed_alias'] = new
            if name == 'layout/manifest.json':
                # This is a current-source contract, not a historical quote:
                # communal size proofs require the literal declaring source.
                for owner in updated.get('bss_owners', []):
                    if owner.get('kind') != 'LINK_COMMUNAL':
                        continue
                    for item in owner.get('communals', []):
                        basis = item.get('size_basis', {})
                        if isinstance(basis.get('declaration'), str):
                            basis['declaration'] = replace_tokens(basis['declaration'],
                                {r['old']: r['new'] for r in symbol_rows(rows)})
            if name == 'layout/names-registry.json':
                for row in symbol_rows(rows):
                    if row.get('address') is None:
                        continue
                    address = str(int(str(row['address']), 0))
                    # Data declarations can be interior or unemitted aliases.
                    # Rename existing canonical rows; never invent a public
                    # registry row from a source-only data-plan address.
                    if row.get('kind') in ('global', 'data') and address not in updated['names']:
                        continue
                    entry = updated['names'].setdefault(address, {
                        'inventory_name': row['old'], 'kind': 'code' if row.get('kind') not in ('data', 'global') else 'data',
                        'basis': 'Explicit rename plan; pinned-object and exact-image proof required'})
                    entry['name'] = row['new']
            result = json_bytes(updated) if updated != document else raw
        else:
            result = raw
        # Explicit source paths may occur in recipes, tools, tests and evidence.
        # Replace complete path spellings, including paths embedded in prose.
        if name != 'layout/oracle.lock.json' and moves:
            text = result.decode('latin1')
            for old, new in path_spellings.items():
                text = re.sub(r'(?<![A-Za-z_0-9.])' + re.escape(old) + r'(?![A-Za-z_0-9.])', lambda _: new, text, flags=re.I)
            result = text.encode('latin1')
        if result != raw:
            changes[name] = result
    for old, new in moves.items():
        if old not in files:
            continue
        changes[new] = changes.pop(old, files[old])
        changes[old] = None
    return changes


from functools import lru_cache


@lru_cache(maxsize=96)
def field_sites(text):
    """Locate C members through syntax and scoped types, never a bare-token swap.

    pycparser is a read-only locator. The historical compiler is still the proof.
    Unsupported syntax/macros refuse the row rather than guessing a receiver.
    Offsets refer to the original text, including comments and CRLFs.
    """
    try:
        from pycparser import c_ast as A, c_parser
    except ImportError as error:
        raise ValueError('Scoped field renames require the local pycparser package') from error
    masked = ''.join((''.join('\n' if c == '\n' else ' ' for c in t)
                      if t.startswith(('/*', '//')) else t) for t in C_TOKEN.findall(text))
    masked = re.sub(r'(?m)^[ \t]*#(?:[^\n]*\\\n)*[^\n]*',
                    lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), masked)
    masked = masked.replace('\r', ' ')
    masked = re.sub(r'\b(?:FAR|NEAR|HUGE|far|near|huge|cdecl|pascal|interrupt|__cdecl|_cdecl)\b',
                    lambda m: ' ' * len(m[0]), masked)
    # Locator-only MSC spelling expansion. Keep a character map so no source
    # formatting, literals or historical compiler inputs are rewritten here.
    widths = {'I8': 'char', 'I8S': 'signed char', 'U8': 'unsigned char',
              'I16': 'int', 'I16S': 'short', 'U16': 'unsigned int',
              'U16S': 'unsigned short', 'I32': 'long', 'U32': 'unsigned long'}
    masked = re.sub(r'\b(?:_loadds|_saveregs|_export)\b', lambda m: ' ' * len(m[0]), masked)
    masked = re.sub(r'\b_?asm\s*\{[^{}]*\}',
                    lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), masked)
    positions, parts, cursor = [], [], 0
    for m in re.finditer(r'\b(?:' + '|'.join(widths) + r')\b', masked):
        parts.extend((masked[cursor:m.start()], widths[m[0]]))
        positions.extend(range(cursor, m.start()))
        positions.extend([m.start()] * len(widths[m[0]]))
        cursor = m.end()
    parts.append(masked[cursor:]); positions.extend(range(cursor, len(masked)))
    located = ''.join(parts)
    # C89 implicit-int prototypes are accepted by MSC but not pycparser.
    for m in reversed(list(re.finditer(r'(?m)^([A-Za-z_]\w*)[ \t]*\([^;{}]*\);', located))):
        located = located[:m.start()] + 'int ' + located[m.start():]
        positions[m.start():m.start()] = [positions[m.start()]] * 4
    try:
        ast = c_parser.CParser().parse('\n' + located)
    except Exception as error:
        return None, 'Scoped C field parser cannot locate members: ' + str(error), None
    offsets = [0]
    for m in re.finditer('\n', located):
        offsets.append(m.end())
    sites, declared, unresolved = {}, set(), set()
    structs, aliases, scopes = {}, {}, [{}]

    def put(tag, field, coord):
        if coord is None or coord.line < 2:
            return
        at = positions[offsets[coord.line - 2] + coord.column - 1]
        require(text[at:at + len(field)] == field, 'Field locator coordinate differs: ' + field)
        sites.setdefault((tag, field), set()).add(at)

    def resolve(t):
        while isinstance(t, A.TypeDecl):
            t = t.type
        if isinstance(t, A.IdentifierType) and len(t.names) == 1 and t.names[0] in aliases:
            return resolve(aliases[t.names[0]])
        if isinstance(t, (A.Struct, A.Union)) and t.name in structs:
            return structs[t.name]
        return t

    def lookup(name):
        return next((s[name] for s in reversed(scopes) if name in s), None)

    def expr(n):
        if isinstance(n, A.ID):
            return resolve(lookup(n.name))
        if isinstance(n, A.Cast):
            return resolve(n.to_type.type)
        if isinstance(n, (A.ArrayRef, A.UnaryOp)):
            base = expr(n.name if isinstance(n, A.ArrayRef) else n.expr)
            if isinstance(n, A.UnaryOp) and n.op == '&':
                return A.PtrDecl([], base)
            return resolve(base.type) if isinstance(base, (A.PtrDecl, A.ArrayDecl)) else base
        if isinstance(n, A.StructRef):
            base = expr(n.name)
            if n.type == '->' and isinstance(base, A.PtrDecl):
                base = resolve(base.type)
            if isinstance(base, (A.Struct, A.Union)):
                field = next((d for d in base.decls or [] if d.name == n.field.name), None)
                return resolve(field.type) if field else None
        if isinstance(n, A.FuncCall):
            base = expr(n.name)
            if isinstance(base, A.PtrDecl):
                base = resolve(base.type)
            return resolve(base.type) if isinstance(base, A.FuncDecl) else None
        if isinstance(n, A.BinaryOp):
            left, right = expr(n.left), expr(n.right)
            return left if isinstance(left, (A.PtrDecl, A.ArrayDecl)) else right
        if isinstance(n, A.TernaryOp):
            return expr(n.iftrue)
        return None

    class Locate(A.NodeVisitor):
        def visit_Struct(self, n):
            if n.decls is not None and n.name:
                structs[n.name] = n
                for d in n.decls:
                    declared.add((n.name, d.name))
                    typ = d.type
                    while hasattr(typ, 'type') and not isinstance(typ, A.TypeDecl):
                        typ = typ.type
                    put(n.name, d.name, typ.coord)
                    self.visit(d.type)
        visit_Union = visit_Struct

        def visit_Typedef(self, n):
            aliases[n.name] = n.type
            self.visit(n.type)
            base = resolve(n.type)
            if isinstance(base, (A.Struct, A.Union)) and not base.name:
                base.name = n.name
                self.visit(base)

        def visit_Decl(self, n):
            if n.name:
                scopes[-1][n.name] = n.type
            self.generic_visit(n)

        def visit_FuncDef(self, n):
            scopes[0][n.decl.name] = n.decl.type
            scopes.append({})
            self.visit(n.decl)
            self.visit(n.body)
            scopes.pop()

        def visit_Compound(self, n):
            scopes.append({})
            self.generic_visit(n)
            scopes.pop()

        def visit_StructRef(self, n):
            base = expr(n.name)
            if n.type == '->' and isinstance(base, A.PtrDecl):
                base = resolve(base.type)
            if isinstance(base, (A.Struct, A.Union)) and base.name:
                put(base.name, n.field.name, n.field.coord)
            else:
                unresolved.add(n.field.name)
            self.visit(n.name)
    Locate().visit(ast)
    return sites, declared, unresolved


def replace_fields(text, rows, asm=False):
    if not rows:
        return text
    relevant = [r for r in rows if re.search(r'\b' + re.escape(r['struct']) + r'\b', text)
                and re.search(r'\b' + re.escape(r['old']) + r'\b', text)]
    if not relevant:
        return text
    if asm:
        # MASM fields have a structure-local spelling, without C decoration.
        out, tag = [], None
        maps = {r['struct']: {x.get('asm_old', x['old']): x.get('asm_new', x['new'])
                             for x in relevant if x['struct'] == r['struct']} for r in relevant}
        for line in text.splitlines(keepends=True):
            m = re.match(r'^\s*(\w+)\s+struc\b', line, re.I)
            if m:
                tag = m[1]
            if tag in maps:
                line = replace_tokens(line, maps[tag], asm=True)
            # Resolve TYPE.member tokens, preserving literals and comments.
            tokens, previous, rewritten = ASM_TOKEN.findall(line), [], []
            for token in tokens:
                if len(previous) >= 2 and previous[-1] == '.' and previous[-2] in maps:
                    token = maps[previous[-2]].get(token, token)
                rewritten.append(token)
                if token.strip():
                    previous.append(token)
            line = ''.join(rewritten)
            out.append(line)
            if re.match(r'^\s*\w+\s+ends\b', line, re.I):
                tag = None
        return ''.join(out)
    sites, declared, unresolved = field_sites(text)
    require(sites is not None, declared)
    edits = {}
    for r in relevant:
        key = (r['struct'], r['old'])
        if key not in declared:
            continue
        require(not any(re.search(r'\b' + re.escape(r['old']) + r'\b', m[0])
                        for m in re.finditer(r'\b_?asm\s*\{[^{}]*\}', text)),
                'Field occurs in inline assembly requiring an explicit scoped operand: ' + r['old'])
        require(r['old'] not in unresolved, 'Unresolved C member receiver: ' + r['old'])
        require(not re.search(r'(?m)^\s*#.*(?:\.|->)\s*' + re.escape(r['old']) + r'\b', text),
                'Field occurs in an opaque preprocessor directive: ' + r['old'])
        # A destination already in this structure would merge distinct members.
        require(r['new'] == r['old'] or (r['struct'], r['new']) not in declared,
                'Destination field already declared in ' + r['struct'] + ': ' + r['new'])
        for at in sites.get(key, ()):
            require(at not in edits or edits[at] == (r['old'], r['new']), 'Conflicting field rows')
            edits[at] = (r['old'], r['new'])
    for at, (old, new) in sorted(edits.items(), reverse=True):
        text = text[:at] + new + text[at + len(old):]
    return text


def port_occurrences(root, rows):
    names = {r['old'] for r in rows}
    file_names = {n for old in file_moves(rows) for n in (old, Path(old).name)}
    names.update(file_names)
    alternatives = [('(?i:' + re.escape(n) + ')' if n in file_names else re.escape(n))
                    for n in sorted(names, key=lambda n: (-len(n), n))]
    pattern = re.compile(r'(?<![A-Za-z_0-9])_?(' + '|'.join(alternatives) +
                         r')(?![A-Za-z_0-9])') if names else None
    canonical_files = {n.casefold(): n for n in file_names}
    found = []
    for folder in ('port', 'tools/porting', 'docs/porting'):
        for path in sorted((root / folder).rglob('*')):
            if not path.is_file() or '__pycache__' in path.parts:
                continue
            raw = path.read_bytes()
            if b'\0' in raw:
                continue
            for line, text in enumerate(raw.decode('utf-8-sig', errors='replace').splitlines(), 1):
                searchable = re.sub(r'\\[bBAZ]', ' ', text)
                hits = sorted({canonical_files.get(n.casefold(), n) for n in pattern.findall(searchable)}) if pattern else []
                if hits:
                    found.append({'file': path.relative_to(root).as_posix(), 'line': line,
                                  'text': text, 'names': hits})
    return found


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
    for path, raw in files.items():
        if path.split('/')[0] not in SOURCE_DIRS or Path(path).suffix.lower() not in ('.c', '.h', '.asm', '.inc'):
            continue
        tokens = ASM_TOKEN if Path(path).suffix.lower() in ('.asm', '.inc') else C_TOKEN
        present.update(t for t in tokens.findall(raw.decode('latin1')) if WORDS.fullmatch(t))
    registry = json.loads(files['layout/names-registry.json'])['names']
    recipes = active_recipes(files)
    publics = set()
    publics.update(w for w in present if w.startswith('_'))
    names, truncs = {}, {}
    for row in symbol_rows(rows):
        names.setdefault(row['new'], []).append(row['old'])
        truncs.setdefault(('_' + row['new'])[:31], []).append(row['old'])
    good, refused = [], []
    for row in rows:
        old, new = row['old'], row['new']
        reason = None
        if row.get('kind') in MOVE_KINDS:
            try:
                moves = file_moves([row])
                source, destination = next(iter(moves.items()))
                for path in (source, destination):
                    publication_path(path)
                require(source in files and destination not in files, 'Source absent or destination exists')
                require(list(file_moves(rows).values()).count(destination) == 1, 'Duplicate move destination')
                require(sum(r.get('source', r['old']) == source for r in rows if r.get('kind') in MOVE_KINDS) == 1,
                        'Duplicate move source')
                require(source.split('/')[0] in SOURCE_DIRS[:2] and
                        destination.split('/')[0] == source.split('/')[0] and
                        Path(source).suffix.lower() == Path(destination).suffix.lower(),
                        'Move must preserve the C/ASM source kind and directory')
                require(any(x['recipe']['source'] == source for x in recipes.values()),
                        'Source has no active rebuildable recipe')
                if row.get('kind') == 'asm-module':
                    require(IDENT.fullmatch(old) and IDENT.fullmatch(new) and
                            any(old in x['owners'] for x in recipes.values()) and new not in present and
                            new not in {o for x in recipes.values() for o in x['owners']},
                            'Invalid or colliding assembly owner')
                good.append(row)
            except ValueError as error:
                refused.append({**row, 'status': 'refused', 'reason': str(error)})
            continue
        if row.get('kind') == 'field':
            try:
                require(IDENT.fullmatch(old) and IDENT.fullmatch(new) and old != new,
                        'invalid or unchanged field identifier')
                require(new not in C_RESERVED, 'reserved C word')
                require(isinstance(row.get('struct'), str) and IDENT.fullmatch(row['struct']),
                        'Field requires a structure name')
                changed = make_changes(files, [row])
                require(any(n.startswith(SOURCE_DIRS) for n in changed),
                        'Structure member absent from reconstruction sources')
                good.append(row)
            except ValueError as error:
                refused.append({**row, 'status': 'refused', 'reason': str(error)})
            continue
        if sum(r['old'] == old for r in symbol_rows(rows)) > 1:
            reason = 'ambiguous old name in symbol rows'
        elif not IDENT.fullmatch(old) or not IDENT.fullmatch(new) or old == new:
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
            address = str(int(str(row['address']), 0)) if row.get('address') is not None else None
            entry = registry.get(address)
            if entry and entry['name'] != old:
                reason = 'registry address has a different canonical name: ' + entry['name']
            # The source-bearing owner is the proof unit, never a runtime blob.
            defining = [r for r in recipes.values()
                        if row.get('owner') in r['owners'] or r['recipe'].get('id') == row.get('owner')]
            if row.get('kind') in ('global', 'data') and not row.get('owner'):
                defining = [r for r in recipes.values() if re.search(
                    r'(?<![\w$@?])_?' + re.escape(old) + r'(?![\w$@?])',
                    files[r['recipe']['source']].decode('latin1'))]
                if defining:
                    row = {**row, 'grounding': 'complete-declarations-and-historical-link'}
            if not defining:
                reason = 'owner has no rebuildable defining source (pinned runtime or unknown owner)'
            elif not any(re.search(r'(?<![\w$@?])_?' + re.escape(old) + r'(?![\w$@?])',
                                  files[r['recipe']['source']].decode('latin1')) for r in defining):
                reason = 'old symbol is absent from the defining owner'
            elif row.get('kind') not in ('global', 'data') and (address is None or int(address) not in set().union(*(definition_addresses(r, files, old) for r in defining))):
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
            if row.get('kind') in ('global', 'data') and address is not None:
                grounded = set().union(*(definition_addresses(r, files, old) for r in defining))
                if grounded and int(address) not in grounded:
                    reason = 'plan address differs from the independently bound global'
            near_declarers = []
            for key, item in recipes.items():
                recipe = item['recipe']
                original_near = {f['target'] for f in recipe.get('expected_fixups', [])
                                 if f.get('target_kind') == 'external' and f.get('self_relative')
                                 and f.get('loc') == 'offset16'} | set(recipe.get('reviewed_code_offsets', []))
                declarations = recipe.get('object_declarations') or recipe.get('binding', {}).get('declarations') or {}
                own_publics = {p['name'] for p in declarations.get('publics', [])} | {m['public'] for m in recipe.get('members', []) if m.get('public')}
                original_near -= own_publics
                if '_' + old in original_near and '_' + old not in recipe.get('reviewed_near_targets', {}):
                    near_declarers.append(key)
            if near_declarers:
                reason = ('existing near-entry proof requires the original inventory spelling; '
                          'the current resolver cannot bind a renamed entry: ' + ', '.join(near_declarers))
            bindings = json.loads(files.get('layout/code-symbols.json', b'{"symbols":{}}'))['symbols']
            if any(any(field in bindings.get(public, {}) for field in ('module_entry', 'entry_proof'))
                   for public in ('_' + old, ('_' + old)[:31], ('_' + old)[:32])):
                reason = 'existing module/partial-entry binding proof requires the original emitted spelling'
        (refused if reason else good).append({**row, 'status': 'refused', 'reason': reason} if reason else row)
    return good, refused


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


def object_view(obj, mapping=None):
    """Complete extents and ordered semantic fixups; declaration multisets.

    Only external table indexes are replaceable. Segment/group indexes and
    fixup methods, widths, addends, locations and order remain obligations.
    COMDEF order is deferred to the mandatory whole-image and BSS LINK gates.
    CodeView segments remain byte-exact (including their diagnostic records).
    """
    mapping = mapping or {}
    fields = ('segments', 'segment_lengths', 'segment_defs', 'groups', 'publics',
              'linker_fixups', 'local_publics', 'local_externals', 'communals', 'comments')
    view = renamed({key: getattr(obj, key, None) for key in fields}, mapping)
    sort = lambda values: sorted(values, key=lambda v: json.dumps(v, sort_keys=True))
    for key in ('publics', 'local_publics', 'local_externals', 'communals'):
        view[key] = sort(view[key] or [])
    view['externals'] = sort([(mapping.get(n, n), scope) for n, scope in
                              zip(obj.externals, obj.external_scopes)])
    require(len(obj.externals) == len(obj.external_scopes), 'Incomplete external scope table')
    for f in view['linker_fixups'] or []:
        for prefix in ('target', 'frame'):
            if f.get(prefix + '_kind') == 'external':
                f.pop(prefix + '_index', None)
    return view


def declaration_records(raw, mapping):
    """Retain PUBDEF group/frame/type and EXTDEF type data the reader omits."""
    from omf import OmfReader
    result = []
    for kind, body in OmfReader.records(raw):
        if kind not in (0x90, 0xB6, 0x8C, 0xB4):
            continue
        at, group, segment, frame = 0, None, None, None
        if kind in (0x90, 0xB6):
            group, at = OmfReader._index(body, at)
            segment, at = OmfReader._index(body, at)
            if segment == 0:
                frame = int.from_bytes(body[at:at+2], 'little'); at += 2
        while at < len(body):
            size = body[at]
            name = body[at+1:at+1+size].decode('ascii'); at += size + 1
            offset = None
            if kind in (0x90, 0xB6):
                offset = int.from_bytes(body[at:at+2], 'little'); at += 2
            type_index, at = OmfReader._index(body, at)
            result.append((kind, group, segment, frame, mapping.get(name, name), offset, type_index))
    return sorted(result, key=repr)


def compare_objects(before, after, mapping):
    # These objects have passed the strict production decoder, with no unknown
    # OMF records or unsupported fixups. Decoded bytes and fixups replace record
    # encoding equality, not proof coverage. Non-semantic records stay exact.
    diff = first_difference(object_view(before[0], mapping), object_view(after[0]), 'object')
    declaration_diff = first_difference(declaration_records(before[1], mapping),
                                        declaration_records(after[1], {}), 'declarations')
    from omf import OmfReader
    debug_names = lambda obj: [d['name'] for d in getattr(obj, 'debug_segments', [])]
    debug = lambda compiled: {n: OmfReader(communals=True).read(compiled[1]).segments.get(n, b'')
                              for n in debug_names(compiled[0])}
    debug_diff = first_difference(debug(before), debug(after), 'object.debug_bytes')
    represented = {0x8C, 0xB4, 0x90, 0xB6, 0xB0, 0x9C, 0xA0, 0xA2}
    remainder = lambda raw: [(kind, body) for kind, body in OmfReader.records(raw) if kind not in represented]
    record_diff = first_difference(remainder(before[1]), remainder(after[1]), 'records')
    if diff or declaration_diff or record_diff or debug_diff:
        return {'semantic': diff or declaration_diff or debug_diff,
                'omf': record_diff or declaration_diff or diff or debug_diff}
    return None


def refresh_expectations(recipe, obj):
    """Record the fresh emitted order/indexes only after semantic equivalence."""
    import copy
    recipe = copy.deepcopy(recipe)
    declarations = {'segments': obj.segment_defs, 'groups': obj.groups,
                    'publics': obj.publics, 'externals': obj.externals}
    if 'object_declarations' in recipe:
        recipe['object_declarations'] = declarations
    if 'declarations' in recipe.get('binding', {}):
        recipe['binding']['declarations'] = declarations
    if 'expected_fixups' in recipe:
        recipe['expected_fixups'] = obj.linker_fixups
    if 'communal_declarations' in recipe:
        recipe['communal_declarations'] = [{'name': c['name'], 'size': c['length']} for c in obj.communals]
    return recipe


class ExactnessFailure(ValueError):
    """A successfully executed proof found a changed executable or placement."""


class BaselineProofFailure(ValueError):
    """The unrenamed tree could not establish a historical baseline."""


def proof_failure_category(error):
    category = getattr(error, 'details', {}).get('category')
    if isinstance(error, RegistryFailure) or category == 'BINDING_REVIEW_REQUIRED':
        return 'refused'
    if isinstance(error, ExactnessFailure) or category in ('BYTE_MISMATCH', 'EXTENT_MISMATCH'):
        return 'name-sensitive'
    return 'proof-error'


def exact_bss_gate():
    from bss_link import canonical_gate
    try:
        return canonical_gate()
    except ValueError as error:
        if str(error).startswith('BSS storage is not placed by the real link:'):
            raise ExactnessFailure(str(error)) from error
        raise


def historical_proof():
    """Run histbuild's real library-search LINK and EXEPACK, recording members.

    Resolve actual library publics against LINK's map, including data-only and
    zero-byte members, rather than using truncated not-loaded diagnostics.
    """
    import histbuild
    import reallink
    from omf import OmfReader
    report = histbuild.build(tag='rename')
    if report['status'] != 'BUILT_EXACT':
        raise ExactnessFailure('Historical LINK/EXEPACK is ' + report['status'] + '; ' + report['report'])
    link_dir = (histbuild.ROOT / report['report']).parent / 'reallink/link-library'
    mapping = (link_dir / 'RESULT.MAP').read_text(errors='replace')
    public_names = {m[3] for m in reallink.PUBLIC_LINE.finditer(mapping)}
    loaded, providers = [], {}
    for path in (link_dir / 'GAME.LIB', link_dir / 'LIBH.LIB'):
        # Libraries here are the actual search inputs, built by histbuild.
        for ordinal, (module_name, module_raw) in enumerate(OmfReader(communals=True).split_library(path.read_bytes())):
            obj = OmfReader(communals=True).read(module_raw)
            local = {p['name'] for p in obj.local_publics}
            hits = {p['name'] for p in obj.publics if p['name'] in public_names and p['name'] not in local}
            for name in hits:
                require(name not in providers, 'LINK map cannot uniquely identify library provider: ' + name)
                providers[name] = (path.name, ordinal, module_name)
            if hits:
                # The worker receipt crosses JSON before canonical publication.
                # Keep the exact identities stable across that round trip.
                loaded.append([path.name, ordinal, module_name])
    explicit = json.loads((link_dir / 'objects.json').read_bytes())
    # Explicit objects are always loaded; all physical members remain present.
    # GAME.LIB member names are generated from order, not the source basename.
    return {'status': report['status'], 'report': report['report'],
            'loaded_library_modules': sorted(loaded),
            'explicit_objects': report['link']['library_report']['explicit_objects'],
            'object_count': len(explicit)}


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
    communals = None if declared is None else True
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
        self.historical_baseline = None
        self.whole_cache = {}

    def stage(self, changes):
        for name in self.current.keys() | changes.keys():
            raw = changes[name] if name in changes else self.files.get(name)
            if raw is None:
                (self.root / name).unlink(missing_ok=True)
            else:
                atomic_bytes(self.root / name, raw)
        self.current = changes

    def trial(self, rows, whole=False):
        started = time.monotonic()
        cache_key = hashlib.sha256(json_bytes(rows)).hexdigest()
        if whole and cache_key in self.whole_cache:
            changes, ordinal, previous = self.whole_cache[cache_key]
            self.stage(dict(changes))
            result = {**previous, 'reused_from': ordinal,
                      'seconds': round(time.monotonic() - started, 3)}
            self.trials.append(result)
            atomic_bytes(self.root / f'build/rename-trials/{len(self.trials):04d}.json', json_bytes(result))
            print(f"Trial {len(self.trials)}: reused frozen-input whole proof from trial {ordinal}", flush=True)
            return result
        changes = make_changes(self.files, rows)
        affected = [key for key, r in self.recipes.items()
                    if key in changes or r['recipe']['source'] in changes or
                    any(c['path'] in changes for c in r['recipe'].get('preprocessor_closure', []) +
                        r['recipe'].get('include_closure', []) if 'path' in c)]
        if any(key not in self.baselines for key in affected):
            self.stage({})
        failures, communal_reorders = [], []
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
            source = changes.get(recipe['source'], self.files.get(recipe['source']))
            field, closure = _cosmetic_closure(recipe, source)
            if recipe.get(field, []) != closure:
                recipe[field] = closure
                changes[key] = json_bytes(recipe)
        self.stage(dict(changes))
        for key in affected:
            if isinstance(self.baselines[key], dict):
                continue
            recipe = json.loads(changes.get(key, self.files[key]))
            source = changes.get(recipe['source'], self.files.get(recipe['source']))
            fingerprint = (key, hashlib.sha256(source + json_bytes(recipe) +
                           b''.join(changes[n] or b'' for n in sorted(changes) if n.startswith('include/'))).hexdigest())
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
                else:
                    before_communals = renamed(getattr(self.baselines[key][0], 'communals', []), symbol_map(rows))
                    if before_communals != getattr(after[0], 'communals', []):
                        communal_reorders.append({'recipe': key, 'before': before_communals,
                                                  'after': after[0].communals,
                                                  'requires': 'whole-image/BSS-link and historical BUILT_EXACT'})
                    updated = refresh_expectations(recipe, after[0])
                    if updated != recipe:
                        changes[key] = json_bytes(updated)
        self.stage(dict(changes))
        image_result = None
        if whole and not failures:
            try:
                if self.image:
                    image_result = self.image()
                else:
                    if self.historical_baseline is None:
                        candidate = dict(self.current)
                        self.stage({})
                        try:
                            from oracle import verify
                            verify(write=True)
                            self.historical_baseline = historical_proof()
                        except Exception as error:
                            raise BaselineProofFailure('Unrenamed historical baseline failed: ' + str(error)) from error
                        finally:
                            self.stage(candidate)
                    from build_exact import build
                    from bss_link import canonical_gate
                    from oracle import verify
                    verify(write=True)
                    image_result = build(publish=False)
                    if image_result['status'] != 'HYBRID_EXACT':
                        raise ExactnessFailure('Image is not exact')
                    image_result = {'status': image_result['status'],
                                    'relocation_count': image_result['relocation_count'],
                                    'bss_real_link': exact_bss_gate()}
                    historical = historical_proof()
                    if historical['loaded_library_modules'] != self.historical_baseline['loaded_library_modules'] or historical['explicit_objects'] != self.historical_baseline['explicit_objects']:
                        raise ExactnessFailure('Historical link loaded a different set of modules')
                    image_result['historical'] = historical
                    image_result['historical_baseline'] = self.historical_baseline
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
            except Exception as error:
                failures.append({'owner': getattr(error, 'owner_id', None),
                                 'recipe': getattr(error, 'owner_recipe', None),
                                 'record': 'whole-image/BSS-link', 'error': str(error),
                                 'phase': 'registry' if isinstance(error, RegistryFailure) else 'image/BSS',
                                 'proof_category': getattr(error, 'details', {}).get('category'),
                                 'category': proof_failure_category(error)})
        result = {'names': [r['old'] for r in rows], 'affected_recipes': affected,
                  'failures': failures, 'seconds': round(time.monotonic() - started, 3),
                  'whole_image': image_result, 'communal_reorders': communal_reorders}
        self.trials.append(result)
        if whole and all(f['category'] != 'proof-error' for f in failures):
            self.whole_cache[cache_key] = (dict(self.current), len(self.trials), result)
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
                               'with_names': [r['old'] for r in accepted],
                               'with_rows': [row_id(r) for r in accepted]}]
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
            relative.split('/')[0] in STATE_DIRS and not relative.startswith(('tools/porting/', 'tests/../')) and
            relative == path.relative_to(ROOT.resolve()).as_posix() and
            path != (ROOT / 'layout/oracle.lock.json').resolve(),
            'Invalid rename publication path: ' + relative)
    return path


def prepare_publication(changes):
    """Journal moves/deletions and all reconstruction path references together."""
    import transaction
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
    reverse = [{**r, 'old': r['new'], 'new': r['old'],
                **({'source': r['new_source'], 'new_source': r['source']} if 'source' in r and 'new_source' in r else {})} for r in rows]
    require(not make_changes(files, rows), 'Plan is absent or partially applied')
    require(bool(make_changes(files, reverse)), 'Applied symbols are absent')
    registry = json.loads(files['layout/names-registry.json'])['names']
    for row in symbol_rows(rows):
        if row.get('address') is None:
            continue
        address = str(int(str(row['address']), 0))
        if row.get('kind') in ('global', 'data') and address not in registry:
            continue
        require(registry.get(address, {}).get('name') == row['new'],
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
        clean, failed = ([], [{**r, 'status': 'batch-aborted',
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
    require(isinstance(rows, list) and all(isinstance(r, dict) and {'old', 'new'} <= r.keys()
                                         for r in rows), 'Plan must be a list of old/new rows with kind-specific grounding')
    require(len({row_id(r) for r in rows}) == len(rows), 'Duplicate old name in plan')
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
    namespace = set()  # C/ASM namespace is collected by preflight, excluding prose and the port.
    if args.check:
        check_applied(rows, expected)
        good, refused = rows, []
    else:
        good, refused = preflight(rows, expected, namespace)
    work_root = args.report.resolve().parent if args.report else ROOT / 'build/workers/renamefix'
    work_root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='run-', dir=work_root))
    stage = work / 'tree'
    for name, raw in {**tracked, **expected}.items():
        if Path(name).parts[0] in (*SOURCE_DIRS, 'recipes', 'layout', 'tools', 'tests', 'evidence', 'historical'):
            path = stage / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(raw)
    env = dict(os.environ, STUNTS_BUILD_WORKERS='1', STUNTS_ASSET_DIR=str(ROOT / 'assets'),
               NUKED_OPL3_ROOT=os.environ.get('NUKED_OPL3_ROOT', str(ROOT / 'build/dependencies/nuked-opl3')),
               GIT_DIR=str(ROOT / '.git'), GIT_WORK_TREE=str(stage),
               STUNTS_TOOLCACHE=os.environ.get('STUNTS_TOOLCACHE', str(ROOT / 'build/toolcache')),
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
    reported = {row_id(r) for r in report['results']}
    report['results'].extend({**r, 'status': 'batch-aborted', 'reason': 'Another name was refused'}
                             for r in rows if row_id(r) not in reported)
    report['results'].sort(key=lambda r: next(i for i, x in enumerate(rows) if row_id(x) == row_id(r)))
    current = canonical_files(ROOT)
    changed = [n for n in current.keys() | expected.keys() if current.get(n) != expected.get(n)]
    require(not changed, 'Canonical files changed during proof; discard this run: ' + ', '.join(sorted(changed)))
    require(all((ROOT / name).read_bytes() == raw for name, raw in tracked.items()),
            'Tracked inputs changed during proof')
    clean = [r for r in report['results'] if r['status'] == 'clean']
    if not args.dry_run and not args.check and clean:
        require(args.bisect or len(clean) == len(rows), 'Cannot publish partial batch without --bisect')
        changes = {name: (stage / name).read_bytes() if (stage / name).exists() else None
                   for name in report['changes']}
        def verify():
            from build_exact import build
            from bss_link import canonical_gate
            from oracle import verify as verify_oracle
            os.environ['STUNTS_BUILD_WORKERS'] = '1'
            verify_oracle(write=True)
            require(build(publish=False, allow_pending=True)['status'] == 'HYBRID_EXACT', 'Canonical image differs')
            exact_bss_gate()
            canonical_historical = historical_proof()
            require(canonical_historical['loaded_library_modules'] == report['final']['whole_image']['historical']['loaded_library_modules'],
                    'Canonical historical link changed loaded modules')
        publish(changes, expected, verify)
    report['port_occurrences'] = port_occurrences(ROOT, clean)
    report['file_rename_policy'] = ('Repository source paths and owners change; compile_source and assemble_source stage fixed UNIT.C/UNIT.ASM DOS names. '
                                    'The compiler-derived code segment name and THEADR therefore remain unchanged. '
                                    'The historical link additionally stages deterministic short Snnn.C/Annn.ASM names derived from build order. '
                                    'No object segment is relabelled or patched; before/after full objects and historical LINK/EXEPACK must prove equality.')
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
