"""reallink: diagnostic single-LINK rebuild of the Stunts MCGA executable.

DIAGNOSTIC ONLY (integ29: `validate.py --image`).  Nothing here grants
ownership or acceptance; it is not yet a gate.

Pipeline
  1. Units: the image is cut into link units along the reviewed object/module
     map (layout/link-objects.json) and the canonical manifest.
     A unit whose every byte is owned by accepted contributions is built
     from sources (pinned MSC 5.10 / MASM 5.10, per-object recipe flags).
     Every other unit is an explicit RAW DEBT OMF object: original bytes as
     LEDATA plus one base16 FIXUPP per original MZ relocation site (so LINK
     emits the relocation obligation), PUBDEFs for the symbols accepted
     objects need, and nothing else.  Raw debt is never reconstruction.
  2. Processing order (--order oracle, default): topological order over (a)
     address order inside each output segment, (b) first-appearance order of
     code segments, (c) DGROUP _DATA address order, (d) the oracle's
     per-EXEPACK-bank relocation order.  THIS ORDER IS ORACLE-DERIVED, NOT
     INDEPENDENT: the report records `order_basis`.  --order library tests the
     library-search hypothesis instead (explicit seg000-seg009, a runtime
     library, a game library; LINK chooses the module order itself).
  3. One run of pinned LINK 3.65 (/DOSSEG /NOI /MAP), then pinned EXEPACK.
  4. Comparison against layout/oracle.lock.json / build/oracle/*.

All outputs go under build/reallink/.
"""
import argparse
import hashlib
import json
import re
import shutil
import struct
import subprocess
import sys
import uuid
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json, require  # noqa: E402
from compiler import verify_toolchain, toolchain_path, check_inline_asm  # noqa: E402
from preprocessor import prepare  # noqa: E402
from assembler import prepare_asm  # noqa: E402
from object_flags import recipe_flags  # noqa: E402
from omf import OmfReader  # noqa: E402
from mz import MZ  # noqa: E402

OUT = ROOT / 'build' / 'reallink'
DGROUP_BASE = 178032
IMAGE_INIT_END = 199994          # _edata: start of the WORD-aligned _BSS (EXEPACK pads to 200000)
# The last initialized DGROUP byte is 199992: [199993] is the zero WORD-alignment gap before
# _BSS, which LINK does not write (EXEPACK's final fill run of the packed oracle covers 7 zeros
# from 199993, integ29).  The linked image is compared zero-padded to _edata.
INIT_DATA_END = 199993
BSS_END = 222352
# Link recipe (integ32): the MZ SP of 8000 is LINK /ST:8000 over the pinned CRT0 STACK
# segment (2,048 bytes, paragraph, combine stack, in DGROUP).  L6-comdef fixtures
# MAP_STACK_SUM_ST: with /ST:n the linked STACK and SP are n even with an explicit CRT
# STACK; without /ST, STACK contributions add.  No synthetic stack contribution.
LINK_STACK = 8000
LINK_OPTIONS = ['/DOSSEG', '/NOI', '/NOD', '/MAP', '/CP:1', '/ST:%d' % LINK_STACK]
CRT0_MEMBER = ('toolchain/msc510/MLIBCR.LIB', 'dos' + chr(92) + 'crt0.asm')
# absolute runtime markers (pinned CRT sources; acceptance doc: __AHSHIFT = 12, __acrtused = 9876h)
ABSOLUTES = {'__AHSHIFT': 12, '__AHINCR': 4096, '__acrtused': 0x9876}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def log(*args):
    print(*args, flush=True)


# --------------------------------------------------------------------------- OMF writer
def rec(kind, body):
    head = bytes([kind]) + struct.pack('<H', len(body) + 1) + body
    return head + bytes([(-sum(head)) & 255])


def nm(s):
    b = s.encode('ascii')
    assert len(b) < 256
    return bytes([len(b)]) + b


def idx(i):
    assert 0 < i < 0x8000
    return bytes([i]) if i < 128 else bytes([0x80 | (i >> 8), i & 255])


class RawObject:
    """Explicit raw-debt OMF module (one segment contribution + optional group)."""

    ALIGN = {'byte': 1, 'word': 2, 'para': 3, 'page': 4}

    def __init__(self, module, segment, klass, align, data, length=None, group=None,
                 combine=2):
        self.module, self.segment, self.klass, self.align = module, segment, klass, align
        self.data = bytes(data)
        self.length = len(self.data) if length is None else length
        self.group, self.combine = group, combine
        self.publics = []        # (name, offset)
        self.absolutes = []      # (name, value)
        self.externs = []        # names
        self.fixups = []         # (offset, extern_name) base16, in emission order
        self.start = None        # (offset) entry point within segment
        self.extra_segdefs = []  # [(name, class, align, length, group)] zero-length declarations

    def ext(self, name):
        if name not in self.externs:
            self.externs.append(name)
        return self.externs.index(name) + 1

    def build(self, cuts=None):
        names = ['']
        def lname(s):
            if s not in names:
                names.append(s)
            return names.index(s) + 1
        segs = [(self.segment, self.klass, self.align, self.length, self.group, self.combine)]
        segs += [(n, c, a, l, g, 2) for n, c, a, l, g in self.extra_segdefs]
        for s in segs:
            lname(s[0]); lname(s[1])
        groups = []
        for s in segs:
            if s[4] and s[4] not in groups:
                groups.append(s[4]); lname(s[4])
        out = [rec(0x80, nm(self.module))]
        out.append(rec(0x96, b''.join(nm(n) for n in names)))
        for name, klass, align, length, group, combine in segs:
            assert length <= 0xFFFF, (self.module, length)
            acbp = (self.ALIGN[align] << 5) | (combine << 2)
            out.append(rec(0x98, bytes([acbp]) + struct.pack('<H', length) +
                           idx(lname(name)) + idx(lname(klass)) + idx(1)))
        for g in groups:
            body = idx(lname(g))
            for i, s in enumerate(segs, 1):
                if s[4] == g:
                    body += b'\xff' + idx(i)
            out.append(rec(0x9A, body))
        # fixups reference externs: declare all first
        for off, name in self.fixups:
            self.ext(name)
        # records are kept short: LINK 3.65 dies ('invalid psp address') on long PUBDEFs
        body = b''
        for n in self.externs:
            body += nm(n) + b'\x00'
            if len(body) > 800:
                out.append(rec(0x8C, body)); body = b''
        if body:
            out.append(rec(0x8C, body))

        def pubdefs(head, items):
            body = head
            for name, off in items:
                body += nm(name) + struct.pack('<H', off) + b'\x00'
                if len(body) > 800:
                    out.append(rec(0x90, body)); body = head
            if len(body) > len(head):
                out.append(rec(0x90, body))
        if self.publics:
            gi = (groups.index(self.group) + 1) if self.group else 0
            pubdefs((idx(gi) if gi else b'\x00') + idx(1), self.publics)
        if self.absolutes:
            pubdefs(b'\x00\x00\x00\x00', self.absolutes)   # group 0, segment 0, frame 0
        # data in chunks; cuts = sorted chunk starts chosen by caller
        size = len(self.data)
        starts = sorted(set([0] + [c for c in (cuts or []) if 0 < c < size]))
        chunks = []
        for i, s in enumerate(starts):
            e = starts[i + 1] if i + 1 < len(starts) else size
            fx_at = {off for off, _ in self.fixups}
            while e - s > 1024:
                cut = s + 1024
                if cut - 1 in fx_at:
                    cut -= 1
                chunks.append((s, cut)); s = cut
            if e > s:
                chunks.append((s, e))
        data = bytearray(self.data)
        for off, _ in self.fixups:
            data[off:off + 2] = b'\x00\x00'
        placed = set()
        for s, e in chunks:
            out.append(rec(0xA0, idx(1) + struct.pack('<H', s) + bytes(data[s:e])))
            fx = [(off, n) for off, n in self.fixups if s <= off and off + 2 <= e]
            # keep caller order
            body = b''
            for off, name in fx:
                local = off - s
                locat = 0x80 | 0x40 | (2 << 2) | (local >> 8)   # M=1 segment-relative, loc=base
                body += bytes([locat, local & 255])
                body += bytes([0x56]) + idx(self.ext(name))     # F5 (target frame), T2 ext, P=1
                placed.add(off)
                if len(body) > 900:
                    out.append(rec(0x9C, body)); body = b''
            if body:
                out.append(rec(0x9C, body))
        missing = [off for off, _ in self.fixups if off not in placed]
        assert not missing, (self.module, 'fixup crosses chunk', missing[:5])
        if self.start is not None:
            body = bytes([0xC1, 0x00]) + idx(1) + idx(1) + struct.pack('<H', self.start)
            out.append(rec(0x8A, body))
        else:
            out.append(rec(0x8A, b'\x00'))
        return b''.join(out)


# --------------------------------------------------------------------------- DOS tools
def run_dos(tool, args, cwd, timeout=300):
    lock = read_json(ROOT / 'layout/toolchain.json')
    tc = tool.parent
    argv = [lock['runner']['path'], '-e', '-v5.00', str(tool), *args]
    env = {'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.', 'MSDOS_TEMP': '.'}
    r = subprocess.run(argv, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       timeout=timeout, creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    return r.returncode, r.stdout.decode('ascii', 'replace'), argv


def compile_c(recipe, base, workname=None):
    """Canonical recipe compile, only the DOS basename differs (-> BASE_TEXT)."""
    profile = recipe['profile']
    config, _ = verify_toolchain(profile)
    source = (ROOT / recipe['source']).read_bytes()
    expanded, _closure = prepare(source, profile)
    text = expanded.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    check_inline_asm(text, config)
    flags = recipe_flags(recipe) or config['flags']
    staged = text.replace('\n', '\r\n').encode('ascii')
    key = sha(staged + json.dumps([profile, flags, base]).encode())[:16]
    cache = OUT / 'cache' / f'{base}_{key}.OBJ'
    if cache.exists():
        return cache.read_bytes(), {'cached': True, 'flags': flags, 'profile': profile,
                                    'source': recipe['source'], 'invocation_kind': 'C'}
    work = OUT / 'compile' / (workname or base)
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    (work / f'{base}.C').write_bytes(staged)
    tc = (ROOT / config['directory']).resolve()
    rc, out, argv = run_dos(tc / config['executable'], ['/c', *flags, f'{base}.C'], work, 120)
    (work / 'compiler.log').write_text(out)
    obj = work / f'{base}.OBJ'
    if rc or not obj.exists():
        raise RuntimeError(f'compile failed {recipe["source"]}: {out[-400:]}')
    cache.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(obj, cache)
    return obj.read_bytes(), {'cached': False, 'flags': flags, 'profile': profile,
                              'source': recipe['source'], 'invocation_kind': 'C',
                              'command': argv, 'working_directory': str(work)}


SEGLINE = re.compile(r'^(\s*)([A-Za-z_$@?][\w$@?]*)(\s+segment\b)(.*)$', re.I)


def adapt_asm(text, link_segment, force_byte=False):
    """Diagnostic link adaptation of a reconstruction ASM source.

    Only the CODE segment is renamed to the link unit's output segment and
    its class set to 'CODE' (reconstruction ASM uses _TEXT / seg012 with class
    'STUNTSC', which a real link would merge into the runtime _TEXT or move
    behind all CODE segments).  integ37: the zero-length reconstruction DSEG
    (class 'STUNTSD' or 'DATA') is linked as class 'BEGDATA', like the link
    prelude, so /DOSSEG keeps it at the DGROUP base before chksum's NULL.
    Returns (text, list_of_changes)."""
    changes = []
    code_names = set()
    lines = text.split('\n')
    for i, line in enumerate(lines):
        m = SEGLINE.match(line)
        if not m:
            continue
        name, rest = m.group(2), m.group(4)
        cls = re.search(r"'([^']*)'", rest)
        if name.upper() == 'DSEG' and cls and cls.group(1).upper() in ('DATA', 'STUNTSD'):
            new = f"{m.group(1)}{name}{m.group(3)} byte public 'BEGDATA'"
            changes.append((line.strip(), new.strip()))
            lines[i] = new
            continue
        if cls and cls.group(1).upper() in ('CODE', 'STUNTSC'):
            code_names.add(name.upper())
            new_rest = rest.replace(cls.group(0), "'CODE'")
            if force_byte:
                new_rest = re.sub(r'\bword\b', 'byte', new_rest, flags=re.I)
            new = f'{m.group(1)}{link_segment}{m.group(3)}{new_rest}'
            if new != line:
                changes.append((line.strip(), new.strip()))
            lines[i] = new
    if not code_names:
        return '\n'.join(lines), changes
    pat = re.compile(r'\b(' + '|'.join(re.escape(n) for n in code_names) + r')\b', re.I)
    for i, line in enumerate(lines):
        if SEGLINE.match(line):
            continue
        code = line.split(';', 1)
        if re.match(r'^\s*(assume|[\w$@?]+\s+ends\b)', code[0], re.I) or re.search(r'\bseg\s', code[0], re.I):
            new = pat.sub(link_segment, code[0]) + ((';' + code[1]) if len(code) > 1 else '')
            if new != line:
                changes.append((line.strip(), new.strip()))
                lines[i] = new
    return '\n'.join(lines), changes


def assemble(recipe, base, link_segment, force_byte=False):
    config, _ = verify_toolchain(recipe['profile'])
    source = (ROOT / recipe['source']).read_bytes()
    expanded, include_closure = prepare_asm(source)
    require(recipe.get('include_closure', []) == include_closure,
            'real-link ASM include closure differs from tracked include files')
    text = expanded.decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    text, changes = adapt_asm(text, link_segment, force_byte)
    staged = text.replace('\n', '\r\n').encode('ascii')
    key = sha(staged + json.dumps(config['flags']).encode())[:16]
    cache = OUT / 'cache' / f'{base}_{key}.OBJ'
    info = {'flags': config['flags'], 'profile': recipe['profile'], 'adaptations': changes,
            'source': recipe['source'], 'include_closure': include_closure,
            'invocation_kind': 'ASM'}
    if cache.exists():
        return cache.read_bytes(), dict(info, cached=True)
    work = OUT / 'compile' / base
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    (work / f'{base}.ASM').write_bytes(staged)
    tc = (ROOT / config['directory']).resolve()
    rc, out, argv = run_dos(tc / config['executable'], [*config['flags'], f'{base},{base}.OBJ,{base}.LST;'], work, 120)
    (work / 'assembler.log').write_text(out)
    obj = work / f'{base}.OBJ'
    if rc or not obj.exists():
        raise RuntimeError(f'assemble failed {recipe["source"]}: {out[-600:]}')
    require((ROOT/recipe['source']).read_bytes() == source and
            prepare_asm(source)[1] == include_closure,
            'real-link ASM source/include closure changed during assembly')
    cache.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(obj, cache)
    info.update(command=argv, working_directory=str(work), cached=False)
    return obj.read_bytes(), info


def library_member(owner):
    from omf import OmfReader as R
    lib = toolchain_path(owner['library'])
    data = lib.read_bytes()
    assert sha(data) == owner['library_sha256'], 'pinned library changed'
    hits = [b for n, b in R().split_library(data) if n == owner['module'] and sha(b) == owner['module_sha256']]
    assert len(hits) == 1, owner['module']
    return hits[0]


def data_member_object(owner, communal_unit_accepted=False):
    """integ37: the link input of a data-only pinned runtime member: the
    hash-pinned member itself.  A member whose COMDEF communals stay part of
    the raw communal unit (omf_policy.communals, `_file.c`) is linked with
    exactly its COMDEF record re-declared as an EXTDEF of the same names in the
    same external-index order; every other record is the member's own.  LINK
    then resolves those names to the raw communal unit, which keeps their
    storage as explicit raw debt.  integ39: once the whole communal unit is
    accepted the member is linked unchanged, COMDEFs included."""
    blob = library_member(owner)
    info = {'library': owner['library'], 'module': owner['module'], 'module_sha256': sha(blob)}
    names = owner.get('omf_policy', {}).get('communals')
    if not names or communal_unit_accepted:
        return blob, info
    out, at, replaced = bytearray(), 0, 0
    while at < len(blob):
        kind, length = blob[at], struct.unpack_from('<H', blob, at + 1)[0]
        end = at + 3 + length
        if kind == 0xB0:
            body = blob[at + 3:end - 1]
            parsed, pos = [], 0
            while pos < len(body):
                n = body[pos]; name = body[pos + 1:pos + 1 + n].decode('latin1'); pos += 1 + n
                pos += 1                                   # type index 0
                data_type = body[pos]; pos += 1
                assert data_type == 0x62, 'only near communals are re-declared'
                v = body[pos]; pos += 1 + {0x81: 2, 0x84: 3, 0x88: 4}.get(v, 0)
                parsed.append(name)
            assert parsed == list(names), (parsed, names)
            out += rec(0x8C, b''.join(nm(n) + b'\x00' for n in parsed))
            replaced += 1
        else:
            out += blob[at:end]
        at = end
    assert replaced == 1, 'expected exactly one COMDEF record'
    info.update({'comdef_as_extdef': list(names), 'link_object_sha256': sha(bytes(out))})
    return bytes(out), info


# --------------------------------------------------------------------------- inputs
class Ctx:
    pass


def load_inputs():
    c = Ctx()
    c.oracle = read_json(ROOT / 'layout/oracle.lock.json')
    c.image = (ROOT / 'build/oracle/load-image.bin').read_bytes()
    assert sha(c.image) == c.oracle['load_image']['sha256'], 'oracle image changed'
    c.relocs = [r['load_offset'] for r in c.oracle['unpacked_mz']['relocations']]
    c.reloc_index = {site: i for i, site in enumerate(c.relocs)}
    c.manifest = read_json(ROOT / 'layout/manifest.json')
    c.objmap = read_json(ROOT / 'layout/link-objects.json')
    c.code_symbols = read_json(ROOT / 'layout/code-symbols.json')['symbols']
    c.data_symbols = read_json(ROOT / 'layout/data-symbols.json')['symbols']
    c.recipes = {}
    for o in c.manifest['owners']:
        if 'recipe' in o:
            c.recipes[o['recipe']] = read_json(ROOT / o['recipe'])
    return c


def w16(img, at):
    return struct.unpack_from('<H', img, at)[0]


def link_segment_name(objseg):
    """Output segment name used in the link for an objmap segment."""
    if objseg.startswith('seg010'):
        return '_TEXT'
    m = re.match(r'seg(\d\d\d)', objseg)
    return f'S{m.group(1)}_TEXT'


# --------------------------------------------------------------------------- units
class Unit:
    def __init__(self, uid, kind, segment=None, start=None, end=None):
        self.id, self.kind, self.segment = uid, kind, segment
        self.start, self.end = start, end          # code/primary extent in load image
        self.data = []                             # [(segname, start, end)] secondary DGROUP extents
        self.objs = []                             # [(filename, bytes, info)]
        self.raw = None                            # RawObject for raw units
        self.accepted = kind not in ('raw-code', 'raw-data', 'bss', 'stack', 'prelude')
        self.key = start if start is not None else 0
        self.pieces = []                           # accepted owners composing this unit
        self.notes = []
        self.shims = {}                            # segment -> RawObject (alias PUBDEFs, name-binding debt)

    def extents(self):
        out = []
        if self.start is not None:
            out.append((self.segment, self.start, self.end))
        out += self.data
        return out

    def __repr__(self):
        return f'<{self.id} {self.kind} {self.segment} {self.start}-{self.end}>'


def owners_in(c, lo, hi):
    return [o for o in c.manifest['owners'] if o['start'] < hi and lo < o['end']]


# integ36: runtime DGROUP model.  An accepted pinned runtime member whose
# DGROUP storage rows are all modelled is linked as its own hash-pinned OBJ;
# every storage row it declares (initialized DGROUP segments) is carved out of
# the raw DGROUP debt, so LINK places those contributions itself.  A member
# stays raw bytes when a COMMON segment's complete overlay is not supplied by
# linked members (LINK sizes a common by its largest contribution).
# integ37: BEGDATA is linked.  /DOSSEG puts class BEGDATA first in DGROUP, so
# chksum.asm's paragraph-aligned NULL is the DGROUP base, where it is in the
# image.  The zero-length reconstruction DSEG (prelude and every ASM module's
# DSEG) is itself linked as class BEGDATA (adapt_asm), first by appearance, so
# it stays at the DGROUP base in front of NULL.
RUNTIME_UNMODELLED_CLASSES = ()
RUNTIME_BSS_PLACEMENT = 'link-runtime-member-v1'      # runtime_binding.RUNTIME_BSS_PLACEMENT
# LINK /DOSSEG defines these itself (start of class BSS / class STACK).
LINK_DOSSEG_SYMBOLS = ('_edata', '_end')
RUNTIME_COMMONS = {'PAD': (199973, 199992), 'EPAD': (199992, 199993)}


def runtime_link_plan(c):
    plan = {}
    from runtime_binding import runtime_owners
    owners = [o for o in runtime_owners(c.manifest) if o.get('binding')]
    commons = defaultdict(list)
    for o in owners:
        decl = {s['name']: s for s in o['binding']['declarations']['segments']}
        rows, reason = [], None
        for name, row in sorted(o['binding'].get('storage', {}).items(), key=lambda t: t[1]['start']):
            seg = decl.get(name)
            if seg is None:
                reason = 'storage row %s without SEGDEF' % name
                break
            if seg['class'] in ('STACK',) or name == '_BSS':
                continue                      # STACK: /ST; _BSS: bss_owners rows
            if seg['class'] in RUNTIME_UNMODELLED_CLASSES:
                reason = '%s class %s segment is not linked by the runtime DGROUP model' % (name, seg['class'])
                break
            if seg['combine'] == 'common':
                commons[name].append((o['id'], row['start'], row['end']))
            if row['end'] > row['start']:
                rows.append((name, row['start'], row['end']))
        plan[o['id']] = {'linked': reason is None, 'data': rows, 'reason': reason}
    changed = True
    while changed:
        changed = False
        for name, rows in commons.items():
            # The complete pinned MSG COMMON overlay (runtime_binding.verify_runtime_common).
            full = RUNTIME_COMMONS.get(name, (min(r[1] for r in rows), max(r[2] for r in rows)))
            linked = [r for r in rows if plan[r[0]]['linked']]
            if linked and not any((r[1], r[2]) == full for r in linked):
                for r in linked:
                    plan[r[0]].update(linked=False, reason='COMMON %s [%d,%d) is not wholly supplied by linked members'
                                      % (name, full[0], full[1]))
                changed = True
    return plan


def carve(pieces, holes):
    """Subtract [s,e) holes from [s,e) pieces."""
    out = []
    for s, e in pieces:
        cuts = sorted((max(a, s), min(b, e)) for a, b in holes if a < e and s < b)
        at = s
        for a, b in cuts:
            if at < a:
                out.append((at, a))
            at = max(at, b)
        if at < e:
            out.append((at, e))
    return out


def build_units(c, args):
    units = []
    c.runtime_plan = runtime_link_plan(c) if args.runtime in ('members', 'libraries') else {}
    objs = sorted(c.objmap['objects'], key=lambda o: o['start'])
    code_objs = [o for o in objs if o['kind'] in ('C', 'ASM', 'RUNTIME')]
    next_start = {id(o): (code_objs[i + 1]['start'] if i + 1 < len(code_objs) else 176576)
                  for i, o in enumerate(code_objs)}

    for o in code_objs:
        seg = link_segment_name(o['segment'])
        lo, hi, limit = o['start'], o['end'], next_start[id(o)]
        own = [w for w in owners_in(c, lo, limit) if w['kind'] != 'LINK_FILL']
        accepted = [w for w in own if w['kind'] != 'UNRESOLVED_RAW']
        raw = [w for w in own if w['kind'] == 'UNRESOLVED_RAW']
        # raw bytes allowed only as trailing zero fill after the accepted union
        acc_end = max((w['end'] for w in accepted), default=lo)
        def is_fill(w):
            s, e = max(w['start'], lo), min(w['end'], limit)
            return s >= acc_end and not any(c.image[s:e])
        raw_real = [w for w in raw if not is_fill(w)]
        contained = all(w['start'] >= lo and w['end'] <= limit for w in accepted)
        if o['kind'] == 'ASM':
            # each accepted ASM contribution is a complete assembled module: own unit;
            # raw remainder pieces become raw units
            for w in sorted(own, key=lambda w: w['start']):
                if w['kind'] == 'UNRESOLVED_RAW':
                    s, e = max(w['start'], lo), min(w['end'], hi)
                    if e <= s:
                        continue
                    u = Unit(f'raw_{o["id"]}_{s}', 'raw-code', seg, s, e)
                    u.notes.append(f'raw remainder of objmap module {o["id"]}')
                    units.append(u)
                else:
                    if w['start'] < lo:
                        continue   # already emitted with the previous module
                    assert w['kind'] in ('MATCHING_ASM', 'MATCHING_C'), w['id']
                    kind = 'asm' if w['kind'] == 'MATCHING_ASM' else 'c'
                    if kind == 'c' and w['start'] & 1:
                        u = Unit(f'raw_oddc_{w["id"]}', 'raw-code', seg, w['start'], w['end'])
                        u.notes.append('accepted C contribution starts at an odd address inside an ASM '
                                       'segment: a separately compiled MSC object is WORD aligned, so it '
                                       'is linked as raw debt (not a standalone original object)')
                        units.append(u)
                        continue
                    u = Unit(f'{kind}_{w["id"] or w["name"]}', kind, seg, w['start'], w['end'])
                    u.pieces = [w]
                    if kind == 'c':
                        u.notes.append('C contribution inside an ASM segment: compiled into the shared segment by DOS basename')
                    units.append(u)
            continue
        if o['kind'] == 'RUNTIME':
            lib = [w for w in accepted if w['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY'
                   and w['start'] == lo]
            data_segs = []
            if lib:
                segs_ = lib[0].get('binding', {}).get('declarations', {}).get('segments') or []
                data_segs = [x['name'] for x in segs_ if x['class'] != 'CODE' and x['length']]
            if args.runtime in ('members', 'libraries') and lib and data_segs:
                plan = c.runtime_plan.get(lib[0]['id'])
                if args.runtime in ('members', 'libraries') and plan and plan['linked'] and not raw_real and contained:
                    # integ36 runtime DGROUP model: the hash-pinned member OBJ itself,
                    # or, in library mode, the member loaded by normal LINK search;
                    # its DGROUP contributions are carved out of raw DGROUP debt.
                    u = Unit(f'rt_{lib[0]["id"]}', 'rt', seg, lo, max(w['end'] for w in accepted))
                    u.pieces = accepted
                    u.data = list(plan['data'])
                    u.notes.append(('normal LINK library member with its DGROUP segments ' if args.runtime == 'libraries'
                                    else 'pinned member OBJ with its DGROUP segments ') +
                                   ','.join('%s[%d,%d)' % d for d in u.data))
                    units.append(u)
                    continue
                u = Unit(f'raw_{o["id"]}', 'raw-code', seg, lo, hi)
                u.notes.append('pinned member carries DGROUP data ' + ','.join(data_segs) +
                               ': linked as raw (%s)' % ((plan or {}).get('reason') or 'no runtime DGROUP plan'))
                units.append(u)
                continue
            if args.runtime in ('members', 'libraries') and lib and not raw_real and contained:
                u = Unit(f'rt_{lib[0]["id"]}', 'rt', seg, lo, max(w['end'] for w in accepted))
                u.pieces = accepted
            else:
                u = Unit(f'raw_{o["id"]}', 'raw-code', seg, lo, hi)
                if lib:
                    u.notes.append('accepted pinned runtime member linked as raw bytes (--runtime raw)')
            units.append(u)
            continue
        # C objects
        if accepted and not raw_real and contained:
            u = Unit(f'c_{o["id"]}', 'c', seg, lo, acc_end)
            u.pieces = sorted(accepted, key=lambda w: w['start'])
            if len(u.pieces) > 1:
                u.notes.append('object assembled from several accepted C contributions')
            units.append(u)
        elif accepted and args.partial == 'split':
            for w in sorted(own, key=lambda w: w['start']):
                s, e = max(w['start'], lo), min(w['end'], hi)
                if e <= s:
                    continue
                if w['kind'] == 'UNRESOLVED_RAW':
                    u = Unit(f'raw_{o["id"]}_{s}', 'raw-code', seg, s, e)
                else:
                    u = Unit(f'c_{w["id"]}', 'c', seg, w['start'], w['end']); u.pieces = [w]
                    u.notes.append(f'piece of partially accepted object {o["id"]} (--partial split)')
                units.append(u)
        else:
            u = Unit(f'raw_{o["id"]}', 'raw-code', seg, lo, hi)
            if accepted:
                u.notes.append('partially accepted C object linked as raw debt: ' +
                               ', '.join(str(w['id'] or w['name']) for w in accepted))
            units.append(u)
    # integ37: hash-pinned data-only runtime members (no code): the member OBJ
    # itself, its DGROUP contributions carved out of raw DGROUP debt.
    from runtime_binding import runtime_data_members
    for member in runtime_data_members(c.manifest):
        plan = c.runtime_plan.get(member['id']) if args.runtime in ('members', 'libraries') else None
        if plan and plan['linked'] and plan['data']:
            u = Unit(f'rtd_{member["id"]}', 'rt-data', None, None, None)
            u.pieces = [member]
            u.data = list(plan['data'])
            u.key = min(a for _, a, _ in u.data)
            u.notes.append('pinned data-only member OBJ with its DGROUP segments ' +
                           ','.join('%s[%d,%d)' % d for d in u.data))
            units.append(u)
    # integ35: the objmap object of every code unit (host of its raw _BSS placeholder)
    for u in units:
        if u.start is None:
            continue                    # integ37: data-only runtime members have no code object
        for o in code_objs:
            if o['start'] <= u.start < next_start[id(o)]:
                u.object = o['id']
                break
    # far data modules
    for o in objs:
        if o['kind'] == 'DATA' and 'far data' in o['segment']:
            own = [w for w in owners_in(c, o['start'], o['end'])]
            assert len(own) == 1 and own[0]['kind'] == 'MATCHING_C_DATA', o['id']
            u = Unit(f'far_{own[0]["id"]}', 'far-data', None, o['start'], o['end'])
            u.pieces = own
            units.append(u)
    # DGROUP initialized data
    byid = {}
    for u in units:
        for p in u.pieces:
            byid[p['id']] = u
    c.owned_dgroup_fill = []
    carved = [(a, b) for u in units if u.kind in ('rt', 'rt-data') for _, a, b in u.data]
    for w in owners_in(c, DGROUP_BASE, INIT_DATA_END):
        s, e = max(w['start'], DGROUP_BASE), min(w['end'], INIT_DATA_END)
        if w['kind'] == 'LINK_FILL':
            # integ33: owned DGROUP word-alignment fill; LINK re-creates it
            # from the following contribution's WORD alignment (link_fill).
            c.owned_dgroup_fill.append(s)
            continue
        if w['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY_DATA':
            # integ36: owned pinned runtime data is linked by its member OBJ.
            parent = byid.get(w['parent'])
            if parent is None or parent.kind not in ('rt', 'rt-data') or (w['segment'], w['start'], w['end']) not in parent.data:
                for a, b in carve([(s, e)], carved):
                    u = Unit(f'rawdata_{a}', 'raw-data', '_DATA', a, b)
                    u.notes.append(f'owned runtime data of non-linked member {w["parent"]}')
                    units.append(u)
                c.runtime_problems = getattr(c, 'runtime_problems', []) + [
                    'owned runtime data %s is not linked from its pinned member' % w['id']]
            continue
        if w['kind'] in ('MATCHING_C_DATA', 'MATCHING_ASM_DATA') and w.get('parent'):
            # integ31: a whole ASM module's own _DATA is placed like a C TU's.
            parent = byid.get(w['parent'])
            if parent is None or parent.kind != ('asm' if w['kind'] == 'MATCHING_ASM_DATA' else 'c'):
                u = Unit(f'rawdata_{s}', 'raw-data', '_DATA', s, e)
                u.notes.append(f'accepted data of non-linked parent {w["parent"]}')
                units.append(u)
            else:
                parent.data.append((w['segment'], s, e))
        elif w['kind'] == 'MATCHING_C_DATA':
            u = Unit(f'dmod_{w["id"]}', 'data-module', '_DATA', s, e)
            u.pieces = [w]
            units.append(u)
        else:
            assert w['kind'] == 'UNRESOLVED_RAW', w
            for a, b in carve([(s, e)], carved):
                units.append(Unit(f'rawdata_{a}', 'raw-data', '_DATA', a, b))
    # integ32: accepted BSS storage (bss_owners) belongs to its object's own _BSS
    # (link-module-order-v1) or to its communals (link-communal-v1); integ35: raw
    # BSS debt is one placeholder per object plus the communal unit
    # (add_special_units, place_bss_placeholders).
    c.bss_problems = []
    for w in c.manifest.get('bss_owners', []):
        if w['kind'] in ('UNRESOLVED_RAW', 'LINK_FILL', 'LINK_COMMUNAL'):
            continue                # integ39: fill re-created by LINK; communals allocated by LINK
        parent = byid.get(w.get('parent'))
        if parent is None:
            c.bss_problems.append(f'BSS owner {w["id"]}: parent {w.get("parent")} is not linked from source')
            continue
        parent.data.append((w['segment'], w['start'], w['end']))
        parent.bss_rows = getattr(parent, 'bss_rows', []) + [w]
    return units


# DGROUP tail after the last _DATA contribution (evidence: acceptance doc / runtime
# owners: _cflush.asm XP far-pointer table 4 B before crt0dat's HDR; nmsghdr HDR,
# crt0msg MSG, PAD, EPAD are class MSG).  Raw debt, segment-level model only.
DGROUP_TAIL = [(199722, 199726, 'XP', 'DATA'), (199726, INIT_DATA_END, 'MSG', 'MSG')]
RAW_DATA_CLASS = {'_DATA': 'DATA', 'XP': 'DATA', 'MSG': 'MSG'}


def model_dgroup_tail(units, enabled=True):
    if not enabled:
        return units
    out = []
    for u in units:
        if u.kind != 'raw-data' or u.end <= DGROUP_TAIL[0][0]:
            out.append(u); continue
        pos = u.start
        for s, e, seg, cls in DGROUP_TAIL:
            if e <= pos or u.end <= s:
                continue
            if pos < s:
                out.append(Unit(f'rawdata_{pos}', 'raw-data', '_DATA', pos, s)); pos = s
            v = Unit(f'rawdata_{seg}_{pos}', 'raw-data', seg, pos, min(e, u.end))
            v.notes.append(f'DGROUP tail modelled as segment {seg} class {cls}')
            out.append(v); pos = min(e, u.end)
    return out


def split_raw_data(c, units):
    """Split raw DGROUP pieces between contiguous relocation runs so each run can
    take its own place in the processing order (raw debt freedom, labelled)."""
    out = []
    for u in units:
        if u.kind != 'raw-data':
            out.append(u); continue
        sites = sorted(s for s in c.relocs if u.start <= s < u.end)
        if len(sites) < 2:
            out.append(u); continue
        by_index = sorted(sites, key=lambda s: c.reloc_index[s])
        runs = [[by_index[0]]]
        for s in by_index[1:]:
            if c.reloc_index[s] == c.reloc_index[runs[-1][-1]] + 1:
                runs[-1].append(s)
            else:
                runs.append([s])
        if len(runs) == 1:
            out.append(u); continue
        runs.sort(key=lambda r: min(r))
        if not all(max(runs[i]) < min(runs[i + 1]) for i in range(len(runs) - 1)):
            u.notes.append('interleaved relocation runs inside raw data piece; not split')
            out.append(u); continue
        bounds = [u.start] + [min(r) for r in runs[1:]] + [u.end]
        for i in range(len(runs)):
            v = Unit(f'rawdata_{bounds[i]}', 'raw-data', '_DATA', bounds[i], bounds[i + 1])
            v.notes.append(f'split of raw data {u.start}-{u.end} at relocation run')
            out.append(v)
    return out


def image_game_order(c, game, report=None, all_units=None, carry_forward=True):
    """Game library member order from image facts only (integ31, diagnostic):
    chains keep their within-segment address order; a member's anchor is its
    own accepted _DATA start or else the lowest unowned DGROUP address in the
    game library data region that only its code addresses (DS memory
    operands; immediates are not trusted).  A member without an anchor
    follows its chain predecessor (`image`); the variant `image-back` lets C
    members without one precede their chain successor instead.  Never reads
    the MZ relocation table."""
    import sys as _sys
    loc = str(ROOT / 'build/python')
    if loc not in _sys.path:
        _sys.path.insert(0, loc)
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    md.detail = True
    lo, hi = 191860, 199722            # after the explicit objects' _DATA, before the DGROUP tail
    refs = defaultdict(set)
    units = [u for u, _ in game]
    scan = all_units if all_units is not None else units
    owned = [(s, e) for u in scan for seg, s, e in u.data if seg == '_DATA']
    for u in scan:
        if u.start is None or u.segment in (None, '_DATA') or u.kind == 'raw-data':
            continue
        for ins in md.disasm(c.image[u.start:u.end], u.start):
            if ins.disp_size != 2 or ins.disp_offset <= 0:
                continue
            mem = [op for op in ins.operands if op.type == capstone.x86.X86_OP_MEM]
            if any(ins.reg_name(op.mem.segment) in ('cs', 'es', 'ss') or
                   ins.reg_name(op.mem.base) == 'bp' for op in mem):
                continue
            v = DGROUP_BASE + w16(c.image, ins.address + ins.disp_offset)
            if lo <= v < hi and not any(s <= v < e for s, e in owned):
                refs[v].add(u.id)
    anchor = {}
    for u in units:
        own = [s for seg, s, e in u.data if seg == '_DATA']
        if u.segment == '_DATA' and u.start is not None:
            own.append(u.start)
        excl = [v for v, us in refs.items() if us == {u.id}]
        if own or excl:
            anchor[u.id] = min(own) if own else min(excl)
    chains = defaultdict(list)
    for u in units:
        chains['C' if u.kind == 'c' else (u.segment or u.kind)].append(u)
    key = {}
    for name, chain in chains.items():
        chain.sort(key=lambda u: (u.start if u.start is not None else 1 << 30))
        if name == 'C' and not carry_forward:
            nxt = 1 << 30
            for u in reversed(chain):
                nxt = min(nxt, anchor.get(u.id, nxt))
                key[u.id] = (nxt, 0, u.start or 0)
        else:
            prev = -1
            for u in chain:
                prev = max(prev, anchor.get(u.id, prev))
                key[u.id] = (prev, 1, u.start or 0)
    ordered = sorted(game, key=lambda t: key[t[0].id])
    if report is not None:
        report.update({'anchored': len(anchor), 'members': len(units),
                       'order': [u.id for u, _ in ordered]})
    return ordered


def drop_alignment_fill(c, units):
    """A single zero byte at an odd address directly before an accepted
    word-aligned _DATA contribution is LINK alignment fill, not an object:
    LINK re-creates it from the next contribution's WORD alignment."""
    starts = {s for u in units if u.kind in ('c', 'asm', 'data-module', 'rt', 'rt-data') for seg, s, e in u.extents() if seg == '_DATA'}
    # integ31: likewise the zero byte at an odd address right after an
    # odd-length accepted _DATA (asm012_133660's 5 bytes) and before raw
    # data: the raw debt piece then starts at the next word, WORD aligned.
    ends = {e for u in units if u.kind in ('c', 'asm', 'data-module', 'rt', 'rt-data') for seg, s, e in u.extents() if seg == '_DATA'}
    out, fills = [], list(getattr(c, 'owned_dgroup_fill', []))
    for u in units:
        if (u.kind == 'raw-data' and u.end - u.start == 1 and u.start % 2 == 1 and
                c.image[u.start] == 0 and u.end in starts):
            fills.append(u.start)
            continue
        if (u.kind == 'raw-data' and u.segment == '_DATA' and u.start % 2 == 1 and
                u.end - u.start > 1 and c.image[u.start] == 0 and u.start in ends):
            fills.append(u.start)
            u.start += 1
            u.key = u.start
            u.after_fill = True
            u.notes.append('leading zero byte is LINK word-alignment fill after an odd-length accepted _DATA')
        out.append(u)
    return out, sorted(fills)


# --------------------------------------------------------------------------- accepted objects
def build_accepted(c, units, args):
    """Compile / assemble / extract every accepted unit; returns report rows."""
    rows = []
    n = 0
    for u in units:
        if not u.accepted:
            continue
        for p in u.pieces:
            n += 1
            recipe = c.recipes.get(p.get('recipe')) if p.get('recipe') else None
            if u.kind in ('c', 'far-data', 'data-module'):
                if u.kind == 'c':
                    base = 'S' + u.segment[1:4]            # S000..S037 -> S000_TEXT
                elif u.kind == 'far-data':
                    base = 'F' + str(p['start'] % 1000).zfill(3)
                else:
                    base = 'DMOD'
                data, info = compile_c(recipe, base, f'{u.id}_{n}')
            elif u.kind == 'asm':
                force = bool(p['start'] & 1)
                data, info = assemble(recipe, f'A{n:03d}', u.segment, force_byte=force)
                if force:
                    info['forced_byte_alignment'] = True
            elif u.kind == 'rt':
                data, info = library_member(p), {'library': p['library'], 'module': p['module']}
            elif u.kind == 'rt-data':
                data, info = data_member_object(p, communal_unit_accepted(c.manifest))
            else:
                raise AssertionError(u.kind)
            # integ39: COMDEF-bearing accepted objects (communal unit) are read with
            # their communals; the strict checks are communal_unit's.
            obj = OmfReader(communals=True).read(data)
            u.objs.append({'bytes': data, 'obj': obj, 'piece': p, 'info': info})
            rows.append({'unit': u.id, 'kind': u.kind, 'piece': p.get('id') or p.get('name'), 'info': info,
                         'segments': {s['name']: [s['class'], s['alignment'], s['length']]
                                      for s in obj.segment_defs}})
    return rows


def placements(u, entry):
    """Load address of each nonempty segment contribution of one accepted OBJ."""
    obj, p = entry['obj'], entry['piece']
    place, problems = {}, []
    for s in obj.segment_defs:
        if not s['length']:
            continue
        if s['class'] in ('CODE',) or (u.kind == 'far-data' and s['class'] == 'FAR_DATA'):
            place[s['name']] = p['start']
        elif s['name'] == '_BSS':
            d = [x for x in u.data if x[0] == '_BSS']
            if d:
                place['_BSS'] = d[0][1]
            elif u.kind == 'rt' and getattr(u, 'bss_placeholder', None):
                place['_BSS'] = u.bss_placeholder['start']     # raw-owned: placement checked, not granted
            else:
                problems.append(f'{u.id}: emitted _BSS {s["length"]} B without an accepted BSS owner')
        elif u.kind == 'rt' and s['class'] == 'STACK':
            continue                              # CRT0 STACK: LINK /ST sets the linked stack
        elif u.kind in ('rt', 'rt-data') and s['name'] != '_BSS' and s['name'] != '_DATA':
            d = [x for x in u.data if x[0] == s['name']]
            if d:
                place[s['name']] = d[0][1]
            else:
                problems.append(f'{u.id}: emitted {s["name"]} ({s["class"]}) {s["length"]} B without a storage row')
        elif s['name'] == '_DATA':
            if u.kind == 'data-module':
                place['_DATA'] = p['start']
            else:
                d = [x for x in u.data if x[0] == '_DATA']
                if d:
                    place['_DATA'] = d[0][1]
                else:
                    problems.append(f'{u.id}: emitted _DATA {s["length"]} B without an accepted data interval')
        else:
            problems.append(f'{u.id}: emitted {s["name"]} ({s["class"]}) {s["length"]} B not modelled')
    return place, problems


# --------------------------------------------------------------------------- segments / frames
def segment_layout(units):
    segs = {}
    for u in units:
        if u.kind in ('c', 'asm', 'rt', 'raw-code'):
            segs.setdefault(u.segment, []).append(u)
    order = sorted(segs, key=lambda s: min(x.start for x in segs[s]))
    starts = {s: min(x.start for x in segs[s]) for s in order}
    starts['_TEXT'] = 117842     # LINK /DOSSEG reserves 16 bytes at the start of _TEXT
    frames = {s: starts[s] // 16 for s in order}
    return order, starts, frames, segs


# --------------------------------------------------------------------------- symbols
def derive_symbols(c, units):
    """Addresses implied by original operands at accepted fixup sites (diagnostic)."""
    derived = defaultdict(set)
    problems = []
    seg_order, seg_starts, frames, _ = segment_layout(units)
    for u in units:
        for e in u.objs:
            place, probs = placements(u, e)
            problems += probs
            obj = e['obj']
            for f in obj.linker_fixups:
                if f['target_kind'] != 'external' or f['segment'] not in place:
                    continue
                site = place[f['segment']] + f['offset']
                enc = bytes.fromhex(f['encoded_addend'])
                disp = f['displacement'] or 0
                name = f['target']
                if f['self_relative'] and f['width'] == 2:
                    rel = (w16(c.image, site) - struct.unpack_from('<H', enc)[0] - disp) & 0xFFFF
                    rel = rel - 0x10000 if rel & 0x8000 else rel
                    derived[name].add(('near', site + 2 + rel, frames[u.segment]))
                elif f['loc'] == 'pointer32':
                    off = (w16(c.image, site) - struct.unpack_from('<H', enc)[0] - disp) & 0xFFFF
                    seg = w16(c.image, site + 2)
                    derived[name].add(('far', seg * 16 + off, seg))
                elif f['loc'] in ('offset16', 'loader-offset16'):
                    val = (w16(c.image, site) - struct.unpack_from('<H', enc)[0] - disp) & 0xFFFF
                    fk, fr = f['frame_kind'], f['frame']
                    if fk == 'group' or (fk == 'segment' and str(fr).upper() in ('DSEG', '_DATA')):
                        derived[name].add(('dgroup', DGROUP_BASE + val, None))
                    elif fk in ('location', 'segment') and (fk == 'location' or fr in frames):
                        base = frames[u.segment if fk == 'location' else fr] * 16
                        derived[name].add(('code-ofs', base + val, base // 16))
                    elif fk == 'target':
                        derived[name].add(('target-ofs', val, None))
                    else:
                        derived[name].add(('?ofs:' + str(fr), val, None))
                elif f['self_relative'] and f['width'] == 2:
                    rel = (w16(c.image, site) - struct.unpack_from('<H', enc)[0] - disp) & 0xFFFF
                    rel = rel - 0x10000 if rel & 0x8000 else rel
                    derived[name].add(('near', site + 2 + rel, frames[u.segment]))
                elif f['loc'] == 'base16':
                    derived[name].add(('frame', None, w16(c.image, site)))
    return derived, problems


def resolve_symbols(c, units):
    defined = {}
    locals_ = set()
    for u in units:
        for e in u.objs:
            obj = e['obj']
            lp = {p['name'] for p in obj.local_publics}
            for p in obj.publics:
                if p['name'] in lp:
                    continue
                if p['name'] in defined and p['name'] not in ('__acrtused', '__acrtmsg'):
                    defined.setdefault('$dups', []).append(p['name'])
                defined.setdefault(p['name'], u.id)
    # integ39: a name any linked object declares as a COMDEF is allocated by LINK.
    communal = {cm['name'] for u in units for e in u.objs for cm in getattr(e['obj'], 'communals', [])}
    needed = set()
    for u in units:
        for e in u.objs:
            obj = e['obj']
            for name, scope in zip(obj.externals, obj.external_scopes or ['external'] * len(obj.externals)):
                if scope == 'external' and name not in defined and name not in LINK_DOSSEG_SYMBOLS \
                        and name not in communal:
                    needed.add(name)
    derived, problems = derive_symbols(c, units)
    resolved, conflicts, unresolved = {}, [], []
    for name in sorted(needed):
        cand = None
        if name in c.code_symbols:
            t = c.code_symbols[name]
            if 'mapped_target' in t:
                cand = ('code', t['mapped_target']['start'], t['frame_load_address'] // 16, 'code-symbols')
            else:
                a = [x for x in t.get('anchors', []) if x.get('hex', '').startswith('9a')]
                if a:
                    raw = bytes.fromhex(a[0]['hex'])
                    off, seg = struct.unpack_from('<HH', raw, 1)
                    cand = ('code', seg * 16 + off, seg, 'code-symbols(anchor)')
        elif name in c.data_symbols and c.data_symbols[name].get('storage') != 'code_island':
            cand = ('data', c.data_symbols[name]['load_address'], DGROUP_BASE // 16, 'data-symbols')
        elif name in c.data_symbols:
            cand = ('code', c.data_symbols[name]['load_address'], None, 'data-symbols(code_island)')
        elif name in ABSOLUTES:
            cand = ('absolute', ABSOLUTES[name], None, 'pinned runtime absolute')
        d = derived.get(name, set())
        addrs = {a for k, a, _ in d if a is not None and k in ('far', 'dgroup', 'near', 'code-ofs')}
        if cand is None and len(addrs) == 1:
            a = next(iter(addrs))
            kind = next(k for k, x, _ in d if x == a)
            fr = next((f for k, x, f in d if x == a and f is not None), None)
            cand = ('data' if kind == 'dgroup' else 'code', a, fr, 'operand-derived')
        if cand is not None and addrs and addrs != {cand[1]}:
            conflicts.append({'symbol': name, 'table': cand, 'operands': sorted(map(str, d))})
        tofs = {a for k, a, _ in d if k == 'target-ofs'}
        if cand is None and not addrs and len(tofs) == 1:
            cand = ('data', DGROUP_BASE + next(iter(tofs)), DGROUP_BASE // 16,
                    'operand-derived (F5 target frame assumed DGROUP)')
        if cand is None:
            if name in c.data_symbols:
                cand = ('data', c.data_symbols[name]['load_address'], DGROUP_BASE // 16, 'data-symbols(code_island)')
            else:
                unresolved.append({'symbol': name, 'operands': sorted(map(str, d))})
                continue
        resolved[name] = cand
    return defined, resolved, conflicts, unresolved, problems


def _append_public_alias(blob, public, segment_index, offset):
    """Add a link-only alias to an existing same-segment PUBDEF record.

    LINK 3.65 ignores a new PUBDEF record appended to these MSC objects, but
    resolves an additional entry in their existing PUBDEF record. No SEGDEF,
    LEDATA, or FIXUPP is added or changed.
    """
    def take_index(body, at):
        require(at < len(body), 'Truncated OMF index in PUBDEF')
        first = body[at]
        if first & 0x80:
            require(at + 1 < len(body), 'Truncated OMF index in PUBDEF')
            return ((first & 0x7f) << 8) | body[at + 1], at + 2
        return first, at + 1

    pos = 0
    while pos < len(blob):
        require(pos + 3 <= len(blob), 'Truncated link object record')
        kind = blob[pos]
        length = int.from_bytes(blob[pos + 1:pos + 3], 'little')
        end = pos + 3 + length
        require(end <= len(blob), 'Truncated link object payload')
        if kind == 0x90:
            body = blob[pos + 3:end - 1]
            _group, at = take_index(body, 0)
            segment, at = take_index(body, at)
            if segment == segment_index and segment != 0:
                while at < len(body):
                    n = body[at]
                    require(at + 1 + n + 3 <= len(body), 'Truncated PUBDEF entry')
                    existing = body[at + 1:at + 1 + n].decode('ascii')
                    at += 1 + n + 2
                    _, at = take_index(body, at)
                    require(existing != public, 'Duplicate link-only public alias')
                require(0 <= offset <= 0xffff, 'PUBDEF alias offset is out of range')
                entry = bytes([len(public)]) + public.encode('ascii') + struct.pack('<H', offset) + bytes([0])
                return blob[:pos] + rec(kind, body + entry) + blob[end:]
        pos = end
    raise ValueError('Owner OBJ has no PUBDEF for alias segment %d' % segment_index)


def _bind_accepted_code_aliases(units, resolved):
    """Bind reviewed code-symbol aliases to their owning accepted OBJ PUBDEF."""
    bindings = {}
    for name, (kind, address, _frame, source) in sorted(resolved.items()):
        if kind != 'code' or source not in ('code-symbols', 'code-symbols(anchor)'):
            continue
        matches = []
        for unit in units:
            if unit.kind not in ('c', 'asm') or not unit.accepted:
                continue
            for entry in unit.objs:
                for segment in entry['obj'].segment_defs:
                    if segment['class'] != 'CODE' or not segment['length']:
                        continue
                    start = entry['piece']['start']
                    if start <= address < start + segment['length']:
                        matches.append((unit, entry, segment, start))
        if len(matches) != 1:
            continue
        unit, entry, segment, start = matches[0]
        offset = address - start
        entry['bytes'] = _append_public_alias(entry['bytes'], name, segment['index'], offset)
        entry.setdefault('link_public_aliases', []).append(name)
        bindings[name] = {'address': address, 'owner': unit.id, 'segment': segment['name'],
                          'segment_index': segment['index'], 'offset': offset,
                          'source': source, 'record': 'PUBDEF-in-owner-OBJ'}
    return bindings


def containing_unit(units, addr, kinds):
    best = None
    for u in units:
        if u.kind not in kinds:
            continue
        for seg, s, e in u.extents():
            if s <= addr < e or (addr == e and best is None):
                if s <= addr < e:
                    return u, seg, s
                best = (u, seg, s)
    return best if best else (None, None, None)


# --------------------------------------------------------------------------- raw objects
def chunk_cuts(sites_in_order, lo, hi):
    """Finest LEDATA partition consistent with the given FIXUPP order."""
    cuts = []
    n = len(sites_in_order)
    suffix_min = [0] * (n + 1)
    suffix_min[n] = 1 << 30
    for i in range(n - 1, -1, -1):
        suffix_min[i] = min(sites_in_order[i], suffix_min[i + 1])
    pm = -1
    for k in range(n - 1):
        pm = max(pm, sites_in_order[k])
        if pm < suffix_min[k + 1]:
            cuts.append(suffix_min[k + 1] - lo)
    return cuts


def make_raw_objects(c, units, seg_frames, anchors):
    issues = []
    for u in units:
        if u.accepted or u.kind in ('prelude', 'bss', 'stack'):
            continue
        if u.kind == 'raw-code':
            first = u.start == min(x.start for x in units if x.segment == u.segment and x.start is not None)
            align = 'word' if (first or not u.start & 1) else 'byte'
            r = RawObject(u.id[:40], u.segment, 'CODE', align, c.image[u.start:u.end])
        else:
            r = RawObject(u.id[:40], u.segment, RAW_DATA_CLASS[u.segment],
                          'word' if getattr(u, 'after_fill', False) else 'byte', c.image[u.start:u.end],
                          group='DGROUP')
        sites = [s for s in c.relocs if u.start <= s and s + 2 <= u.end]
        sites.sort(key=lambda s: c.reloc_index[s])
        for s in sites:
            w = w16(c.image, s)
            a = anchors.get(w)
            if a is None:
                issues.append(f'{u.id}: relocation site {s} frame {w:#x} has no segment in the link')
                continue
            r.fixups.append((s - u.start, a))
        u.raw = r
        u.cuts = chunk_cuts(sites, u.start, u.end)
    return issues


# --------------------------------------------------------------------------- order
def processing_order(c, units):
    ids = {u.id: u for u in units}
    succ = defaultdict(set)
    dropped = []

    def reaches(a, b):
        stack, seen = [a], {a}
        while stack:
            x = stack.pop()
            if x == b:
                return True
            for y in succ[x]:
                if y not in seen:
                    seen.add(y); stack.append(y)
        return False

    def edge(a, b, why):
        if a == b or b in succ[a]:
            return
        if reaches(b, a):
            dropped.append({'from': a, 'to': b, 'why': why})
            return
        succ[a].add(b)

    # (1) prelude first; bss/stack last
    for u in units:
        if u.kind not in ('prelude',):
            edge('prelude', u.id, 'prelude')
    # (2) address chains inside output segments, (3) segment first appearance
    order, starts, frames, segs = segment_layout(units)
    for s in order:
        chain = sorted(segs[s], key=lambda x: x.start)
        for a, b in zip(chain, chain[1:]):
            edge(a.id, b.id, f'address order in {s}')
    firsts = [min(segs[s], key=lambda x: x.start) for s in order]
    for a, b in zip(firsts, firsts[1:]):
        edge(a.id, b.id, 'segment first appearance')
    far = sorted([u for u in units if u.kind == 'far-data'], key=lambda x: x.start)
    for a, b in zip(far, far[1:]):
        edge(a.id, b.id, 'far data first appearance')
    # (4) DGROUP _DATA address order
    dchain = sorted([(e[1], u) for u in units for e in u.extents() if e[0] == '_DATA'],
                    key=lambda t: t[0])
    for (_, a), (_, b) in zip(dchain, dchain[1:]):
        edge(a.id, b.id, 'DGROUP _DATA address order')
    # (4b) integ36: address order of the other public initialized DGROUP
    # segments (MSG) once linked pinned members and raw pieces share them.
    for name in ('MSG',):
        chain = sorted([(e[1], u) for u in units for e in u.extents() if e[0] == name], key=lambda t: t[0])
        for (_, a), (_, b) in zip(chain, chain[1:]):
            edge(a.id, b.id, 'DGROUP %s address order' % name)
    # keys for data-only units
    last = -1.0
    for i, (_, u) in enumerate(dchain):
        if u.kind in ('raw-data', 'data-module', 'rt-data'):
            u.key = last + 1e-3
            last = u.key
        else:
            last = u.key
    # (5) relocation order per EXEPACK bank
    ext = sorted(((s, e, u.id) for u in units for _, s, e in u.extents()))
    import bisect
    starts_ = [x[0] for x in ext]
    def unit_of(site):
        i = bisect.bisect_right(starts_, site) - 1
        if i >= 0 and ext[i][0] <= site < ext[i][1]:
            return ext[i][2]
        return None
    noncontig = []
    for bank in range(4):
        seq = [unit_of(s) for s in c.relocs if s >> 16 == bank]
        runs = [x for i, x in enumerate(seq) if i == 0 or seq[i - 1] != x]
        seen = set()
        for x in runs:
            if x in seen:
                noncontig.append({'bank': bank, 'unit': x})
            seen.add(x)
        for a, b in zip(runs, runs[1:]):
            if a and b:
                edge(a, b, f'relocation order bank {bank}')
    # Kahn with keys
    import heapq
    indeg = defaultdict(int)
    for a in list(succ):
        for b in succ[a]:
            indeg[b] += 1
    heap = [(ids[x].key, x) for x in ids if indeg[x] == 0]
    heapq.heapify(heap)
    out = []
    while heap:
        _, x = heapq.heappop(heap)
        out.append(ids[x])
        for y in succ[x]:
            indeg[y] -= 1
            if indeg[y] == 0:
                heapq.heappush(heap, (ids[y].key, y))
    assert len(out) == len(units), 'order incomplete'
    return out, dropped, noncontig


# --------------------------------------------------------------------------- link + compare
def merge_raw_runs(c, ordered):
    """Host limit: pinned LINK under MS-DOS Player aborts ('invalid psp address')
    beyond ~216 input objects (probe_count.py).  Consecutive raw units of the same
    output segment that are address-contiguous (gap = zero fill only) are merged
    into one raw OBJ; FIXUPP order and record cuts are preserved, fill bytes
    between them become raw bytes."""
    out = []
    for u in ordered:
        p = out[-1] if out else None
        if (p is not None and not u.accepted and not p.accepted and u.kind == p.kind and
                u.kind in ('raw-code', 'raw-data') and u.segment == p.segment and
                0 <= u.start - p.end <= 1 and not any(c.image[p.end:u.start]) and
                not u.shims and not p.shims):
            m = Unit(p.id if p.id.startswith('merged') else 'merged_' + p.id, p.kind, p.segment, p.start, u.end)
            m.members = getattr(p, 'members', [p.id]) + [u.id]
            r = RawObject(m.id[:40], p.raw.segment, p.raw.klass, p.raw.align, c.image[p.start:u.end],
                          group=p.raw.group)
            shift = u.start - p.start
            r.publics = p.raw.publics + [(n, o + shift) for n, o in u.raw.publics]
            r.absolutes = p.raw.absolutes + u.raw.absolutes
            r.extra_segdefs = p.raw.extra_segdefs + u.raw.extra_segdefs
            r.fixups = p.raw.fixups + [(o + shift, n) for o, n in u.raw.fixups]
            r.start = p.raw.start if p.raw.start is not None else (
                None if u.raw.start is None else u.raw.start + shift)
            m.raw = r
            m.cuts = sorted(set(getattr(p, 'cuts', []) + [shift] + [x + shift for x in getattr(u, 'cuts', [])]))
            m.notes = p.notes + u.notes
            out[-1] = m
        else:
            out.append(u)
    return out


def run_link(c, ordered, args):
    config, _ = verify_toolchain('msc510-medium')
    tc = (ROOT / config['directory']).resolve()
    work = OUT / 'link'
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    names = []
    listing = []
    ordered = merge_raw_runs(c, ordered)
    for u in ordered:
        import os
        kinds = os.environ.get('RL_SHIM_CLASSES')
        only = os.environ.get('RL_SHIM_UNITS')
        blobs = [sh.build() for sh in u.shims.values()
                 if (not kinds or sh.klass in kinds.split(',')) and
                 (not only or u.id in only.split(','))] if not args.no_alias_shims else []
        blobs += [e['bytes'] for e in u.objs] if u.accepted else [u.raw.build(getattr(u, 'cuts', None))]
        for i, b in enumerate(blobs):
            fn = f'M{len(names):03d}.OBJ'
            (work / fn).write_bytes(b)
            names.append(fn[:-4])
            listing.append({'file': fn, 'unit': u.id, 'members': getattr(u, 'members', None),
                            'kind': u.kind, 'start': u.start, 'end': u.end,
                            'data': u.data, 'sha256': sha(b)})
    lines = []
    for i in range(0, len(names), 8):
        chunk = '+'.join(names[i:i + 8])
        lines.append(chunk + ('+' if i + 8 < len(names) else ''))
    opts = ' '.join(args.link_options)
    resp = '\r\n'.join(lines + ['RESULT.EXE', 'RESULT.MAP', f'{opts};']) + '\r\n'
    (work / 'LINK.RSP').write_text(resp)
    rc, out, argv = run_dos(tc / 'LINK.EXE', ['@LINK.RSP'], work, 600)
    (work / 'link.log').write_text(out)
    json.dump(listing, open(work / 'objects.json', 'w'), indent=1)
    result = {'returncode': rc, 'log': out[-4000:], 'command': argv, 'options': opts,
              'object_count': len(names)}
    if not (work / 'RESULT.EXE').exists():
        return result, None
    exe = (work / 'RESULT.EXE').read_bytes()
    rc2, out2, argv2 = run_dos(tc / 'EXEPACK.EXE', ['RESULT.EXE', 'PACKED.EXE'], work, 300)
    (work / 'exepack.log').write_text(out2)
    result['exepack'] = {'returncode': rc2, 'log': out2[-2000:]}
    packed = (work / 'PACKED.EXE').read_bytes() if (work / 'PACKED.EXE').exists() else None
    return result, (exe, packed, (work / 'RESULT.MAP').read_text(errors='replace'))


def bank_partition(sites):
    return [s for b in range(16) for s in sites if s >> 16 == b]


def first_diff(a, b):
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return None if len(a) == len(b) else min(len(a), len(b))


def compare(c, units, exe, packed, mapping):
    rep = {}
    mz = MZ.parse(exe)
    img = mz.load_image(exe)
    orc = c.image[:IMAGE_INIT_END]
    if INIT_DATA_END <= len(img) < IMAGE_INIT_END and not any(c.image[len(img):IMAGE_INIT_END]):
        img = img + bytes(IMAGE_INIT_END - len(img))   # unwritten zero alignment gap before _BSS
    rep['link_header'] = {'cs': mz.cs, 'ip': mz.ip, 'ss': mz.ss, 'sp': mz.sp,
                          'minalloc': mz.minalloc, 'maxalloc': mz.maxalloc,
                          'image_size': len(img), 'relocations': len(mz.relocations)}
    om = c.oracle['unpacked_mz']
    rep['oracle_header'] = {k: om[k] for k in ('cs', 'ip', 'ss', 'sp', 'minalloc', 'maxalloc')}
    rep['oracle_header']['image_size'] = IMAGE_INIT_END
    rep['oracle_header']['relocations'] = len(c.relocs)
    n = min(len(img), len(orc))
    diffs = [i for i in range(n) if img[i] != orc[i]]
    rep['image_equal'] = img == orc
    rep['image_mismatch_bytes'] = len(diffs) + abs(len(img) - len(orc))
    rep['image_first_mismatch'] = diffs[0] if diffs else (None if len(img) == len(orc) else n)
    # mismatch attribution by unit
    ext = sorted(((s, e, u) for u in units for _, s, e in u.extents()), key=lambda t: t[0])
    per = defaultdict(int)
    j = 0
    for d in diffs:
        while j < len(ext) and ext[j][1] <= d:
            j += 1
        k = j
        u = ext[k][2] if k < len(ext) and ext[k][0] <= d < ext[k][1] else None
        per[(u.id, u.kind) if u else ('<fill/none>', '-')] += 1
    runs = []
    for d in diffs:
        if runs and d <= runs[-1][1] + 2:
            runs[-1][1] = d + 1
        else:
            runs.append([d, d + 1])
    rep['mismatch_runs'] = [{'start': a, 'end': b, 'linked': img[a:b].hex(), 'oracle': orc[a:b].hex(),
                             'unit': next((u.id for s0, e0, u in ext if s0 <= a < e0), None)}
                            for a, b in runs[:150]]
    rep['mismatch_by_unit'] = sorted(([k[0], k[1], v] for k, v in per.items()), key=lambda r: -r[2])[:60]
    # relocations
    linked = [r['load_offset'] for r in mz.relocations]
    rep['link_relocation_set_equal'] = set(linked) == set(c.relocs)
    rep['relocations_missing'] = sorted(set(c.relocs) - set(linked))[:50]
    rep['relocations_missing_count'] = len(set(c.relocs) - set(linked))
    rep['relocations_extra'] = sorted(set(linked) - set(c.relocs))[:50]
    rep['relocations_extra_count'] = len(set(linked) - set(c.relocs))
    part = bank_partition(linked)
    rep['bank_partitioned_order_equal'] = part == c.relocs
    fd = first_diff(part, c.relocs)
    rep['bank_partitioned_first_divergence'] = fd
    if fd is not None:
        rep['bank_partitioned_context'] = {'linked': part[max(0, fd - 3):fd + 6],
                                           'oracle': c.relocs[max(0, fd - 3):fd + 6]}
    rep['bank_partitioned_positions_equal'] = sum(1 for a, b in zip(part, c.relocs) if a == b)
    prov = defaultdict(int)
    for site in c.relocs:
        u = next((u for s0, e0, u in ext if s0 <= site < e0), None)
        prov['accepted' if u is not None and u.accepted else 'raw-debt'] += 1
    rep['relocation_provenance'] = dict(prov)
    # packed
    if packed is not None:
        from exepack import unpack
        opk = (ROOT / 'build/oracle/mcga-packed.exe').read_bytes()
        pm = MZ.parse(packed)
        opm = MZ.parse(opk)
        rep['packed'] = {'size': len(packed), 'oracle_size': len(opk),
                         'header': {'cs': pm.cs, 'ip': pm.ip, 'ss': pm.ss, 'sp': pm.sp,
                                    'minalloc': pm.minalloc, 'maxalloc': pm.maxalloc,
                                    'header_size': pm.header_size, 'relocation_offset': pm.relocation_offset},
                         'oracle_header': {'cs': opm.cs, 'ip': opm.ip, 'ss': opm.ss, 'sp': opm.sp,
                                           'minalloc': opm.minalloc, 'maxalloc': opm.maxalloc,
                                           'header_size': opm.header_size,
                                           'relocation_offset': opm.relocation_offset},
                         'equal': packed == opk,
                         'mismatch_bytes': sum(1 for x, y in zip(packed, opk) if x != y) + abs(len(packed) - len(opk)),
                         'first_mismatch': first_diff(packed, opk),
                         'load_first_mismatch': first_diff(pm.load_image(packed), opm.load_image(opk)),
                         'first_30_header_bytes_equal': packed[:30] == opk[:30]}
        try:
            unp, trace = unpack(packed)
            um = MZ.parse(unp)
            uimg = um.load_image(unp)
            rep['packed']['unpacked_image_equal'] = uimg == c.image
            rep['packed']['unpacked_first_mismatch'] = first_diff(uimg, c.image)
            rep['packed']['unpacked_relocation_order_equal'] = [r['load_offset'] for r in um.relocations] == c.relocs
            rep['packed']['unpacked_header'] = {'cs': um.cs, 'ip': um.ip, 'ss': um.ss, 'sp': um.sp,
                                                'minalloc': um.minalloc, 'maxalloc': um.maxalloc}
        except Exception as error:   # noqa
            rep['packed']['unpack_error'] = str(error)
    # map segments
    segs = []
    for m in re.finditer(r'^\s*([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+(\S+)\s+(\S+)', mapping, re.M):
        segs.append({'start': int(m[1], 16), 'stop': int(m[2], 16), 'length': int(m[3], 16),
                     'name': m[4], 'class': m[5]})
    rep['map_segments'] = segs
    pubs = {}
    for m in re.finditer(r'^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+(?:Abs\s+|Res\s+|Imp\s+)?(\S+)\s*$', mapping, re.M):
        pubs[m[3]] = int(m[1], 16) * 16 + int(m[2], 16)
    deltas = []
    for u in units:
        for e in u.objs:
            place, _ = placements(u, e)
            lp = {p['name'] for p in e['obj'].local_publics}
            for p in e['obj'].publics:
                if p['name'] in lp or p['segment'] not in place or p['name'] not in pubs:
                    continue
                exp = place[p['segment']] + p['offset']
                if pubs[p['name']] != exp:
                    deltas.append({'unit': u.id, 'public': p['name'], 'expected': exp,
                                   'linked': pubs[p['name']], 'delta': pubs[p['name']] - exp})
                break
    deltas.sort(key=lambda d: d['expected'])
    rep['misplaced_units'] = deltas
    rep['bss'] = bss_placement(c, units, img, segs, pubs)
    rep['runtime'] = runtime_placement(c, units, img, segs, pubs, mapping)
    return rep


def runtime_placement(c, units, img, segs, pubs, mapping):
    """integ36: where this real link placed every `linked` storage row of each
    linked pinned runtime member (runtime_binding: owned runtime data and the
    zero-length real-link-v1 sections).  Three readings, all of which must
    agree with the row: a MAP public the member defines in that segment; the
    linked value of every own-segment DGROUP offset FIXUPP in the member's
    linked code; the only occurrence in the linked DGROUP of a fixup-free
    contribution's bytes; and, when the member is the only module declaring
    the segment, the MAP segment itself (for a COMMON segment its whole extent)."""
    origin = re.search(r'^\s*([0-9A-F]{4}):0\s+DGROUP\s*$', mapping, re.M)
    dgroup = int(origin[1], 16) * 16 if origin else None
    declarers = defaultdict(set)
    contributors = defaultdict(set)          # integ37: declarers with a nonzero contribution
    for u in units:
        for e in u.objs:
            for sd in e['obj'].segment_defs:
                declarers[sd['name']].add(u.id)
                if sd['length']:
                    contributors[sd['name']].add(u.id)
        if not u.accepted and u.raw is not None:
            declarers[u.raw.segment].add(u.id)
            if u.raw.length:
                contributors[u.raw.segment].add(u.id)
            for n, _, _, ln, _ in u.raw.extra_segdefs:
                declarers[n].add(u.id)
                if ln:
                    contributors[n].add(u.id)
    by_name = defaultdict(list)
    for sg in segs:
        by_name[sg['name']].append(sg)
    rows, problems = [], list(getattr(c, 'runtime_problems', []))
    ids = {u.id: u for u in units}
    checked = set()
    for u in units:
        if u.kind not in ('rt', 'rt-data'):
            continue
        for e in u.objs:
            owner, obj = e['piece'], e['obj']
            checked.add(owner.get('id'))
            storage = owner.get('binding', {}).get('storage', {})
            local = {p['name'] for p in obj.local_publics}
            code = {pubs[p['name']] - p['offset'] for p in obj.publics
                    if p['segment'] == '_TEXT' and p['name'] in pubs and p['name'] not in local}
            for name, row in sorted(storage.items()):
                if row.get('ownership') != 'linked':
                    continue
                found = {}
                for p in obj.publics:
                    if p['segment'] == name and p['name'] in pubs and p['name'] not in local:
                        found.setdefault('map-public', set()).add(pubs[p['name']] - p['offset'])
                if len(code) == 1 and dgroup is not None:
                    base = next(iter(code))
                    for f in obj.linker_fixups:
                        if (f['segment'] == '_TEXT' and f['target_kind'] == 'segment' and f['target'] == name and
                                f['loc'] == 'offset16' and not f['self_relative'] and f['frame_method'] == 1):
                            addend = struct.unpack_from('<H', bytes.fromhex(f['encoded_addend']))[0] + (f['displacement'] or 0)
                            found.setdefault('linked-fixups', set()).add(dgroup + w16(img, base + f['offset']) - addend)
                if (name in obj.segments and row['end'] > row['start'] and dgroup is not None and
                        not any(f['segment'] == name for f in obj.linker_fixups)):
                    # A fixup-free contribution whose bytes occur exactly once in
                    # the linked DGROUP can only have been written there.
                    data = bytes(obj.segment_bytes(name))
                    at = img.find(data, dgroup, len(img))
                    if at >= 0 and img.find(data, at + 1, len(img)) < 0:
                        found['linked-unique-bytes'] = {at}
                sole = declarers.get(name) == {u.id} and len(by_name.get(name, [])) == 1
                if sole:
                    sg = by_name[name][0]
                    found['map-segment'] = {sg['start']}
                    if sg['length'] != row['end'] - row['start']:
                        problems.append('%s %s: MAP segment length %d differs from the row' % (owner['id'], name, sg['length']))
                seg = next((sd for sd in obj.segment_defs if sd['name'] == name), {})
                if (not sole and seg.get('combine') == 'public' and contributors.get(name) == {u.id} and
                        len(by_name.get(name, [])) == 1 and by_name[name][0]['length'] == row['end'] - row['start']):
                    # integ37: the only nonzero contribution to a public segment
                    # (other members declare it empty, e.g. CRT0DAT's XP): the
                    # MAP segment is exactly this member's contribution.
                    found['map-segment-sole-contributor'] = {by_name[name][0]['start']}
                overlay = (seg.get('combine') == 'common' and not sole and len(by_name.get(name, [])) == 1 and
                           all(ids[x].accepted for x in declarers.get(name, ())) and
                           by_name[name][0]['length'] == row['end'] - row['start'] == seg.get('length'))
                if overlay:
                    # integ37: the multi-member MSG COMMON overlay, every declarer a
                    # linked accepted member; this member supplies its complete extent.
                    found['map-common-overlay'] = {by_name[name][0]['start']}
                if seg.get('combine') == 'common' and not sole and not overlay:
                    problems.append('%s %s: linked COMMON segment is not the sole declaration' % (owner['id'], name))
                starts = set().union(*found.values()) if found else set()
                result = {'owner': owner['id'], 'segment': name, 'start': row['start'], 'end': row['end'],
                          'readings': {k: sorted(v) for k, v in found.items()}}
                if not found:
                    problems.append('%s %s: the real link gives no reading of its placement' % (owner['id'], name))
                elif starts != {row['start']}:
                    problems.append('%s %s: real link placed it at %s, row %d' % (owner['id'], name, sorted(starts), row['start']))
                rows.append(result)
    # A linked row of a member that this link did not link from its OBJ is unproven.
    from runtime_binding import runtime_owners
    for o in runtime_owners(getattr(c, 'manifest', {'owners': []})):
        if o['id'] not in checked and any(
                r.get('ownership') == 'linked' for r in o.get('binding', {}).get('storage', {}).values()):
            problems.append('%s: linked storage, but the member is not linked from its pinned OBJ' % o['id'])
    return {'rows': rows, 'problems': problems, 'placed': not problems}


def link_modules(units, image=None):
    """integ34: every OBJ module of the link input with its CODE SEGDEF names,
    its nonempty _BSS and the static owner rows it carries (for the image-derived
    `_BSS` order proof, bss_link.independent_static_order).  integ35: each raw
    `_BSS` placeholder is a module hosted by its object's last module; the raw
    communal unit (c_common, after every `_BSS` by class order) is excluded."""
    out = []

    def raw_module(uid, raw):
        segdefs = [(raw.segment, raw.klass, raw.length)] + [(n, k, ln) for n, k, _, ln, _ in raw.extra_segdefs]
        bss = sum(ln for n, _, ln in segdefs if n == '_BSS')
        return {'id': uid, 'code': [n for n, k, _ in segdefs if k == 'CODE'],
                'code_nonempty': [n for n, k, ln in segdefs if k == 'CODE' and ln],
                'bss': bss, 'bss_align': raw.align, 'owners': []}
    last = {}
    for u in units:
        if u.kind == 'bss':
            continue
        count = len(out)
        for segment, shim in u.shims.items():
            out.append(raw_module('%s:shim:%s' % (u.id, segment), shim))
        if u.accepted:
            rows = [r['id'] for r in getattr(u, 'bss_rows', [])]
            for i, e in enumerate(u.objs):
                obj = e['obj']
                code = [s for s in obj.segment_defs if s['class'] == 'CODE']
                bss = [s for s in obj.segment_defs if s['name'] == '_BSS' and s['length']]
                out.append({'id': '%s#%d' % (u.id, i), 'code': [s['name'] for s in code],
                            'code_nonempty': [s['name'] for s in code if s['length']],
                            'bss': sum(s['length'] for s in bss),
                            'bss_align': bss[0]['alignment'] if bss else None,
                            'owners': rows if bss else []})
                if u.kind == 'rt' and bss:
                    # integ36: a linked pinned member's own _BSS in shared _TEXT is
                    # grounded by the image operands of its own-_BSS FIXUPPs.
                    import bss_link
                    out[-1]['grounded'] = bss_link.grounded_member_bss(obj, u.start, image, DGROUP_BASE)[0] if image is not None else None
        elif u.raw is not None:
            out.append(raw_module(u.id, u.raw))
        if len(out) > count:
            last[u.id] = out[-1]['id']
    # integ35: a raw _BSS placeholder is linked right after its host unit's last module.
    for u in units:
        if u.kind == 'bss' and getattr(u, 'host', None) is not None:
            out.append({'id': u.id, 'code': [], 'code_nonempty': [], 'bss': u.raw.length,
                        'bss_align': u.raw.align, 'owners': [], 'host': last.get(u.host.id),
                        'grounded': getattr(u, 'grounded', None)})
    return out


def communal_unit_accepted(manifest):
    return any(o.get('kind') == 'LINK_COMMUNAL' for o in manifest.get('bss_owners', []))


def communal_placement(c, units, img, segs, pubs):
    """integ39: decide the accepted LINK_COMMUNAL row from this real link
    (communal_unit.check_link): declarers re-derived from the linked objects'
    COMDEF records, MAP publics and c_common, the linked operand of every
    FIXUPP that names a communal.  A COMDEF linked from an accepted source while
    the communal unit is raw is refused (the pinned `_file.c` member links its
    COMDEFs as EXTDEFs then, data_member_object)."""
    import communal_unit
    accepted = [o for o in c.manifest.get('bss_owners', []) if o.get('kind') == 'LINK_COMMUNAL']
    declared, objects = {}, []
    for u in units:
        for e in u.objs:
            piece = e['piece']
            owner = piece.get('id') or piece.get('name')
            place, _ = placements(u, e)
            objects.append({'id': owner, 'obj': e['obj'], 'place': place})
            for cm in getattr(e['obj'], 'communals', []):
                declared.setdefault(cm['name'], []).append((owner, cm['length'], u.accepted))
    if not accepted:
        stray = sorted(declared)
        if not stray:
            return True, []
        return False, [{'id': 'c_common', 'placed': False,
                        'problems': ['COMDEF %s linked without the accepted communal unit' % n for n in stray]}]
    row = accepted[0]
    items = row['communals']
    names = {i['name'] for i in items}
    refs = communal_unit.linked_references(objects, names, img, DGROUP_BASE)
    problems = communal_unit.check_link(items, declared, refs, pubs, segs, c.image, DGROUP_BASE)
    problems += communal_unit.check_names(items, c.manifest)
    result = communal_unit.gate_result(problems)
    return not problems, [{'id': row['id'], 'placement': row.get('placement'), 'communals': len(items),
                           'references': len(refs), 'conditions': result['conditions'],
                           'problems': problems, 'placed': not problems}]


def bss_placement(c, units, img, segs, pubs):
    """integ32: read the real link's placement of every accepted BSS owner."""
    import bss_link
    located, static_rows, communal_rows = {}, [], []
    runtime_rows = []
    for u in units:
        for row in getattr(u, 'bss_rows', []):
            if row.get('placement') == RUNTIME_BSS_PLACEMENT:
                runtime_rows.append(row)
            else:
                (communal_rows if row.get('placement') == bss_link.COMMUNAL else static_rows).append(row)
            for e in u.objs:
                obj = e['obj']
                if not obj.segment_lengths.get('_BSS'):
                    continue
                code = [s['name'] for s in obj.segment_defs if s['class'] == 'CODE' and s['length']]
                place, _ = placements(u, e)
                if len(code) == 1 and code[0] in place:
                    located[row['id']] = (obj, code[0], place[code[0]])
    order = bss_link.independent_static_order(static_rows, link_modules(units, c.image), segment_layout(units)[0],
                                              IMAGE_INIT_END)
    ok_s, static = bss_link.check_static(static_rows, located, img, segs, IMAGE_INIT_END, DGROUP_BASE, order)
    ok_c, communal = communal_placement(c, units, img, segs, pubs)
    ok_r, runtime = bss_link.check_runtime_bss(runtime_rows, located, c.image, img, segs, IMAGE_INIT_END, DGROUP_BASE)
    problems = list(getattr(c, 'bss_problems', []))
    # integ35: every raw placeholder must also land where the partition puts it
    # (a check of the raw debt model, never a placement proof for accepted storage).
    raw = []
    for u in units:
        if u.kind != 'bss':
            continue
        # linked start: the MAP c_common segment, else any public the raw unit defines
        seg = [s for s in segs if s['name'] == 'c_common'] if u.segment == 'c_common' else []
        starts = {seg[0]['start']} if len(seg) == 1 else set()
        starts |= {pubs[n] - off for n, off in u.raw.publics if n in pubs}
        raw.append({'unit': u.id, 'start': u.start, 'end': u.end, 'host': getattr(getattr(u, 'host', None), 'id', None),
                    'grounded': getattr(u, 'grounded', None), 'linked_starts': sorted(starts),
                    'publics': len(u.raw.publics)})
        if (u.segment == 'c_common' and len(seg) != 1) or starts - {u.start}:
            problems.append('raw BSS unit %s linked at %s, partition %d' % (u.id, sorted(starts), u.start))
    # integ39: the owned c_common paragraph fill is re-created by LINK: the BSS
    # sections end (XOE) at its start and c_common starts at its end.
    for w in c.manifest.get('bss_owners', []):
        if w['kind'] != 'LINK_FILL' or w.get('basis') != 'link-communal-paragraph-v1':
            continue
        cc = [s for s in segs if s['name'] == 'c_common']
        xoe = [s for s in segs if s['name'] == 'XOE']
        if len(cc) != 1 or cc[0]['start'] != w['end']:
            problems.append('c_common paragraph fill %s: MAP c_common starts at %s'
                            % (w['id'], [s['start'] for s in cc]))
        if len(xoe) != 1 or xoe[0]['start'] != w['start']:
            problems.append('c_common paragraph fill %s: MAP XOE at %s' % (w['id'], [s['start'] for s in xoe]))
    return {'owners': static + communal + runtime, 'problems': problems, 'raw_placeholders': raw,
            'placed': ok_s and ok_c and ok_r and not problems}


# --------------------------------------------------------------------------- main
def crt0_stack():
    """The pinned CRT0 member's own STACK SEGDEF (name, class, alignment, length,
    combine, DGROUP membership) -- the stack contribution of the real link."""
    lib, member = CRT0_MEMBER
    for name, blob in OmfReader().split_library(toolchain_path(lib).read_bytes()):
        if name == member:
            obj = OmfReader().read(blob)
            rows = [s for s in obj.segment_defs if s['class'] == 'STACK']
            assert len(rows) == 1 and rows[0]['combine'] == 'stack', rows
            grouped = any(rows[0]['name'] in g['segments'] for g in obj.groups if g['name'] == 'DGROUP')
            return dict(rows[0], dgroup=grouped, member=member, library=lib)
    raise AssertionError('pinned CRT0 member missing')


def bss_placeholder_units(c, units):
    """integ35: raw BSS debt as explicit link units (bss_link.check_partition).

    Each `object-bss` placeholder is a raw `_BSS` contribution (WORD aligned, no
    bytes) hosted by its object's code unit: place_bss_placeholders links it
    immediately after that unit, where the object's own `_BSS` would be.  A
    pinned runtime member's placeholder must equal the member's complete `_BSS`
    SEGDEF, placed where the image operands of its own-`_BSS` FIXUPPs put it.
    The `link-word-fill` byte is not linked (LINK re-creates it).  The trailing
    `communal-unit` is LINK's c_common (class BSS, after every `_BSS`)."""
    import bss_link
    out, problems = [], []
    try:
        rows = bss_link.load_partition(c.manifest)
    except ValueError as error:
        c.bss_problems = getattr(c, 'bss_problems', []) + ['BSS partition: %s' % error]
        rows = []
    hosts = defaultdict(list)
    for u in units:
        if getattr(u, 'object', None):
            hosts[u.object].append(u)
    for row in rows:
        if row['kind'] != 'UNRESOLVED_RAW' or row['form'] == bss_link.WORD_FILL:
            continue
        size = row['end'] - row['start']
        if row['form'] == bss_link.COMMUNAL_UNIT:
            u = Unit('bss', 'bss', 'c_common', row['start'], row['end'])
            u.key = 1e9
            # integ39: LINK's c_common is PARAGRAPH aligned; the fill before it is
            # an owned LINK_FILL row that LINK re-creates (not linked).
            u.raw = RawObject('bss_c_common', 'c_common', 'BSS', 'para', b'', length=size, group='DGROUP')
            u.notes.append('raw communal unit [XOE,_end): LINK c_common, one raw unit until the '
                           'communals are reconstructed (%s)' % row['id'])
            out.append(u)
            continue
        host = sorted(hosts.get(row['object'], []), key=lambda h: h.start)
        if host and host[-1].kind == 'rt' and any(e['obj'].segment_lengths.get('_BSS') for e in host[-1].objs):
            # integ36: the pinned member OBJ itself is linked and emits its own
            # _BSS; its raw placeholder is not linked a second time.
            h = host[-1]
            h.bss_placeholder = row
            h.notes.append('own pinned _BSS linked in place of raw placeholder %s' % row['id'])
            continue
        u = Unit('bssraw_' + row['object'], 'bss', '_BSS', row['start'], row['end'])
        u.raw = RawObject(('bssraw_' + row['object'])[:40], '_BSS', 'BSS', 'word', b'', length=size,
                          group='DGROUP')
        u.row = row
        u.host = host[-1] if host else None
        u.key = (u.host.key + 1e-6) if u.host else 1e9 - 1
        u.notes.append('raw _BSS placeholder of %s (%d B, oracle-sized raw debt)' % (row['object'], size))
        if u.host is None:
            problems.append('BSS placeholder %s: object %s is not in the link input' % (row['id'], row['object']))
        elif u.host.segment == '_TEXT':
            try:
                lib, module, blob = pinned_runtime_members(c, [u.host])[u.host.id]
                obj = OmfReader().read(blob)
                start, refs = bss_link.grounded_member_bss(obj, u.host.start, c.image, DGROUP_BASE)
                u.grounded = start
                u.notes.append('pinned %s _BSS %d B grounded at %s by %d own-_BSS operands'
                               % (module, obj.segment_lengths.get('_BSS', 0), start, refs))
                if obj.segment_lengths.get('_BSS', 0) != size or start != row['start']:
                    problems.append('BSS placeholder %s differs from pinned %s _BSS (%s B at %s)'
                                    % (row['id'], module, obj.segment_lengths.get('_BSS'), start))
            except (AssertionError, KeyError, ValueError) as error:
                problems.append('BSS placeholder %s: runtime member not grounded (%s)' % (row['id'], error))
        out.append(u)
    c.bss_problems = getattr(c, 'bss_problems', []) + problems
    return out


def place_bss_placeholders(ordered):
    """Move each hosted raw `_BSS` placeholder right after its host unit."""
    hosted = [u for u in ordered if getattr(u, 'host', None) is not None]
    rest = [u for u in ordered if u not in hosted]
    for p in hosted:
        rest.insert(rest.index(p.host) + 1, p)
    return rest


def add_special_units(units, c=None, stack=True):
    """Prelude, the raw BSS placeholders and communal unit, and the CRT0 STACK.

    integ35: raw BSS debt is placed like raw code (bss_placeholder_units); the
    stack is the pinned CRT0 STACK declaration (LINK /ST sets its size)."""
    pre = Unit('prelude', 'prelude')
    pre.key = -10
    pre.raw = RawObject('prelude', 'DSEG', 'BEGDATA', 'byte', b'', group='DGROUP')
    pre.notes.append('zero-length DGROUP declaration (class BEGDATA, first by appearance) so MASM F0 '
                     'DSEG frames equal the DGROUP base, also in front of a linked chksum NULL')
    out = [pre] + units + (bss_placeholder_units(c, units) if c is not None else [])
    if stack and any(u.kind == 'rt' and any(p.get('module') == CRT0_MEMBER[1] for p in u.pieces) for u in units):
        stack = False                 # integ36: the linked pinned CRT0 OBJ carries its own STACK SEGDEF
    if stack:
        s = crt0_stack()
        st = Unit('stack', 'stack', 'STACK', None, None)
        st.key = 1e9 + 1
        st.raw = RawObject('crt0_stack', s['name'], s['class'], 'para', b'', length=s['length'],
                           combine=5, group='DGROUP' if s['dgroup'] else None)
        st.notes.append('pinned %s STACK SEGDEF (%d B); LINK /ST:%d sets the linked stack'
                        % (s['member'], s['length'], LINK_STACK))
        out.append(st)
    return out


def choose_anchors(c, units):
    order, starts, frames, segs = segment_layout(units)
    anchors, placed = {}, []
    for s in order:
        raws = [u for u in segs[s] if not u.accepted]
        name = None
        if raws:
            name = f'$$F_{s}'
            placed.append((raws[0], name, 0))
        else:
            for u in segs[s]:
                for e in u.objs:
                    lp = {p['name'] for p in e['obj'].local_publics}
                    pubs = [p for p in e['obj'].publics if p['name'] not in lp and p['segment'].endswith('_TEXT') or
                            (p['name'] not in lp and p['segment'] == s)]
                    if pubs:
                        name = pubs[0]['name']; break
                if name:
                    break
        anchors[frames[s]] = name
    rd = [u for u in units if u.kind == 'raw-data']
    anchors[DGROUP_BASE // 16] = '$$F_DGROUP'
    # Once every initialized DGROUP byte is reconstructed, no raw-data unit
    # remains.  The zero-length prelude already owns a DGROUP declaration and
    # can carry the same anchor without creating a synthetic contribution.
    dgroup_host = rd[0] if rd else next(u for u in units if u.kind == 'prelude')
    placed.append((dgroup_host, '$$F_DGROUP', 0))
    for u in units:
        if u.kind == 'far-data':
            e = u.objs[0]
            anchors[u.start // 16] = e['obj'].publics[0]['name']
    return anchors, placed, frames


def odd_data_hypothesis(c, units, apply):
    """Accepted MSC _DATA contributions are WORD aligned; one starting at an odd
    address cannot be placed there by LINK.  Hypothesis (diagnostic only): the
    preceding zero byte is LINK alignment fill / belongs to the object, so the
    preceding raw piece is shortened by that byte and LINK re-creates it."""
    found = []
    for u in units:
        if u.kind not in ('c', 'data-module'):
            continue
        for seg, s, e in u.extents():
            if seg == '_DATA' and s & 1:
                prev = next((v for v in units if v.kind == 'raw-data' and v.end == s), None)
                row = {'unit': u.id, 'data_start': s, 'preceding_byte': c.image[s - 1],
                       'preceding_raw': prev.id if prev else None, 'applied': False}
                if apply and prev is not None and c.image[s - 1] == 0 and prev.end - 1 > prev.start:
                    prev.end -= 1
                    prev.notes.append(f'--pad-hypothesis: last zero byte left to LINK alignment before {u.id} _DATA')
                    row['applied'] = True
                found.append(row)
    return found


def summarize_bytes(units):
    acc = defaultdict(int)
    for u in units:
        for seg, s, e in u.extents():
            if u.kind == 'bss' or seg == '_BSS':
                continue
            acc[('accepted-' + u.kind) if u.accepted else u.kind] += e - s
    return dict(acc)


# --------------------------------------------------------------------------- library-search experiment
def _unit_placements(units, mapping):
    """Linked start of each unit, read from one public it defines (LINK map)."""
    pubs = {}
    for m in re.finditer(r'^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+(?:Abs\s+|Res\s+|Imp\s+)?(\S+)\s*$', mapping, re.M):
        pubs[m[3]] = int(m[1], 16) * 16 + int(m[2], 16)
    placed = {}
    for u in units:
        if u.start is None:
            continue
        cands = []
        for e in u.objs:
            obj = e['obj']
            lp = {p['name'] for p in obj.local_publics}
            place, _ = placements(u, e)
            for p in obj.publics:
                if p['name'] in lp or p['segment'] not in place or place[p['segment']] != u.start:
                    continue
                cands.append((p['name'], p['offset']))
        if u.raw is not None:
            cands += list(u.raw.publics)
        for name, off in cands:
            if name in pubs:
                placed[u.id] = pubs[name] - off
                break
    return placed


def _run_sequence(sites, unit_of):
    seq = []
    for s in sites:
        x = unit_of(s)
        if x is not None and (not seq or seq[-1] != x):
            seq.append(x)
    return seq


PINNED_RUNTIME_LIBRARIES = ('toolchain/msc510/MLIBCR.LIB', 'toolchain/msc510/LIBH.LIB')


PUBLIC_LINE = re.compile(r'^' + chr(92) + 's*([0-9A-F]{4}):([0-9A-F]{4})' + chr(92) + 's+(?:Abs' + chr(92) +
                         's+|Res' + chr(92) + 's+|Imp' + chr(92) + 's+)?(' + chr(92) + 'S+)' + chr(92) + 's*$', re.M)


def library_root_object(names):
    """An EXTDEF-only OMF root to retain every reviewed library contribution.

    LINK still resolves each symbol from the pinned/runtime or game library;
    this zero-segment module supplies no bytes, fixups, or relocation sites.
    """
    unique, seen = [], set()
    for name in names:
        if name not in seen:
            seen.add(name)
            unique.append(name)
    out = [rec(0x80, nm('HISTROOT'))]
    body = b''
    for name in unique:
        entry = nm(name) + b'\x00'
        if body and len(body) + len(entry) > 800:
            out.append(rec(0x8C, body)); body = b''
        body += entry
    if body:
        out.append(rec(0x8C, body))
    out.append(rec(0x8A, b'\x00'))
    return b''.join(out)


def library_public_names(blob):
    """PUBDEF (0x90) names of one OMF module, read record by record (also for
    COMDEF-bearing members that the strict reader refuses)."""
    names, at = set(), 0
    while at + 3 <= len(blob):
        kind = blob[at]
        length = struct.unpack_from('<H', blob, at + 1)[0]
        body = blob[at + 3:at + 2 + length]
        if kind == 0x90:
            i = 0

            def index():
                nonlocal i
                v = body[i]
                if v & 0x80:
                    v = ((v & 0x7F) << 8) | body[i + 1]
                    i += 2
                else:
                    i += 1
                return v
            index()
            segment = index()
            if segment == 0:
                i += 2
            while i < len(body):
                n = body[i]
                names.add(body[i + 1:i + 1 + n].decode('ascii', 'replace'))
                i += 1 + n + 2
                index()
        if kind == 0x8A:
            break
        at += 3 + length
    return names


def pinned_runtime_members(c, units):
    """integ30: the pinned library member of every runtime _TEXT unit.

    Accepted runtime owners name their pinned module identity; a raw runtime
    unit is matched by its complete bytes outside the member's own FIXUPP
    fields (every such unit is exactly one member of MLIBCR.LIB or LIBH.LIB).
    Returns {unit id: (library, module, member bytes)}."""
    members = []
    for lib in PINNED_RUNTIME_LIBRARIES:
        data = toolchain_path(lib).read_bytes()
        for name, blob in OmfReader().split_library(data):
            try:
                obj = OmfReader().read(blob)
            except Exception:       # e.g. COMDEF-bearing data members (no _TEXT code)
                continue
            if '_TEXT' not in obj.segment_lengths or not obj.segment_lengths['_TEXT']:
                continue
            code = obj.segment_bytes('_TEXT')
            mask = bytearray(len(code))
            for f in obj.linker_fixups:
                if f['segment'] == '_TEXT':
                    mask[f['offset']:f['offset'] + f['width']] = bytes([1]) * f['width']
            members.append((lib, name, blob, code, mask, sha(blob)))
    out = {}
    for u in units:
        if u.segment != '_TEXT' or u.start is None:
            continue
        pinned = [p.get('module_sha256') for p in u.pieces if p.get('module_sha256')]
        hits = [m for m in members if m[5] in pinned] if pinned else [
            m for m in members if len(m[3]) <= u.end - u.start + 1 and
            all(m[4][i] or m[3][i] == c.image[u.start + i] for i in range(len(m[3])))]
        # identical code bytes (closeall.c/flushall.c): the reviewed unit map names the member
        named = u.id.split('rt_', 1)[-1].rsplit('_', 1)[0]
        named = chr(92).join(named.split('_', 1)) if named.startswith('dos_') else named
        if len({(m[0], m[1]) for m in hits}) > 1:
            hits = [m for m in hits if m[1] == named]
        names = {(m[0], m[1]) for m in hits}
        assert len(names) == 1, (u.id, sorted(names))
        out[u.id] = hits[0][:3]
    return out


def render_historical_response(name, replacements):
    """Render a checked-in DOS response template with CRLF line endings."""
    template_path = ROOT / 'historical' / name
    text = template_path.read_text(encoding='ascii')
    for token, value in replacements.items():
        text = text.replace(token, value)
    unresolved = re.findall(r'@[A-Z_]+@', text)
    if unresolved:
        raise ValueError(f'unexpanded {name} tokens: {unresolved}')
    return text.replace('\r\n', '\n').replace('\n', '\r\n')


def library_link(c, ordered, args):
    """Library-search hypothesis: explicit seg000-seg009 objects (plus raw
    DGROUP debt, BSS and STACK), then RT.LIB (runtime _TEXT units) and
    GAME.LIB (seg011-seg037, the seg012 modules and the far/DGROUP data
    modules).  LINK chooses which library modules to load and in which order;
    nothing here is taken from the oracle order.

    With --runtime libraries (integ30) the runtime is not rebuilt from units:
    LINK searches the unchanged pinned MLIBCR.LIB and LIBH.LIB themselves, so
    every runtime member carries its real EXTDEFs/PUBDEFs and DGROUP segments
    (the raw DGROUP debt still holds their original data bytes; only code
    placement and relocation order are compared).  Raw game debt that names the
    runtime code frame refers to crt0's `__astart`."""
    import bisect
    pinned = args.runtime == 'libraries'
    members = pinned_runtime_members(c, ordered) if pinned else {}
    if pinned:
        # Names the pinned members define themselves are not re-declared by raw debt.
        defined = set()
        for lib in PINNED_RUNTIME_LIBRARIES:
            for _, blob in OmfReader().split_library(toolchain_path(lib).read_bytes()):
                defined |= library_public_names(blob)
        # A raw far pointer/CALL into the runtime names the pinned public at its
        # exact target (the reference the original object carried); other
        # runtime segment words name crt0's __astart.
        at_address = {}
        by_id = {u.id: u for u in ordered}
        for uid, (lib, module, blob) in members.items():
            try:
                obj = OmfReader().read(blob)
            except Exception:
                continue
            for q in obj.publics:
                if q['segment'] == '_TEXT':
                    at_address.setdefault(by_id[uid].start + q['offset'], q['name'])
        frame = 117840

        def runtime_name(u, o):
            site = u.start + o
            if o >= 2:
                offset = w16(c.image, site - 2)
                name = at_address.get(frame + offset)
                if name and w16(c.image, site) * 16 == frame:
                    return name
            return '__astart'
        for u in ordered:
            if u.raw is not None and u.segment != '_TEXT':
                u.raw.fixups = [(o, runtime_name(u, o) if n == '$$F__TEXT' else n) for o, n in u.raw.fixups]
                u.raw.publics = [(n, o) for n, o in u.raw.publics if n not in defined]
                u.raw.absolutes = [(n, v) for n, v in u.raw.absolutes if n not in defined]
    config, _ = verify_toolchain('msc510-medium')
    tc = (ROOT / config['directory']).resolve()
    work = OUT / 'link-library'
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    explicit_segments = {'S%03d_TEXT' % n for n in range(10)}
    explicit, runtime, game = [], [], []
    for u in ordered:
        blobs = [sh.build() for sh in u.shims.values()]
        blobs += [e['bytes'] for e in u.objs] if u.accepted else [u.raw.build(getattr(u, 'cuts', None))]
        host = getattr(u, 'host', None)
        if host is not None and pinned and host.segment == '_TEXT':
            continue      # integ35: the pinned library member carries its own real _BSS
        if pinned and u.kind == 'rt-data':
            runtime.append((u, []))     # let LINK search the pinned data-only member
            continue
        if (getattr(args, 'game_input', 'library') == 'explicit' and
                u.kind == 'raw-data' and u.segment == '_DATA'):
            game.append((u, blobs))
        elif u.kind in ('prelude', 'raw-data', 'bss', 'stack') or u.segment in explicit_segments:
            explicit.append((u, blobs))
        elif u.segment == '_TEXT':
            runtime.append((u, [] if pinned else blobs))
        else:
            game.append((u, blobs))
    # prelude first (DSEG at the DGROUP base), then the explicit code objects in
    # address order, raw DGROUP debt, BSS/STACK last (segment declarations only)
    # integ35: a raw _BSS placeholder follows its explicit host object; one hosted
    # by a library member cannot be loaded with it (it defines nothing the member
    # needs), so it is linked with the BSS tail (library experiment, diagnostic).
    rank = {'prelude': 0, 'raw-data': 2, 'bss': 3, 'stack': 4}

    def explicit_key(t):
        u = t[0]
        host = getattr(u, 'host', None)
        if host is not None and host.segment in explicit_segments:
            return (1, host.start + 0.5)
        if u.kind == 'bss':
            return (3, u.start if host is not None else 1 << 30)
        return (rank.get(u.kind, 1), u.start if u.kind != 'raw-data' else 0)
    explicit.sort(key=explicit_key)
    if getattr(args, 'game_order', 'oracle') in ('image', 'image-back'):
        # integ31 hypothesis: image-derived member order (no relocation-table
        # information): the code-segment chains (C objects by segment, ASM
        # modules by address inside S012/S018) merged by each member's DGROUP
        # anchor -- its own accepted _DATA start, else the lowest game-library
        # DGROUP address that only its code references.
        game[:] = image_game_order(c, game, all_units=[u for u, _ in explicit + runtime + game],
                                   carry_forward=args.game_order == 'image')
    if getattr(args, 'game_order', 'oracle') == 'address':
        # Independent hypothesis (integ30): game library members in load-image
        # address order (the within-segment placement order is an image fact;
        # the cross-segment interleaving is not assumed from the oracle).
        game.sort(key=lambda t: (t[0].start if t[0].start is not None else 1 << 30))
    names = []

    object_manifest = []
    def write(group, prefix, group_name):
        out = []
        for u, blobs in group:
            # `blobs` is the exact set passed to LINK or LIB. In pinned-library
            # mode it is intentionally empty for runtime members: LINK searches
            # MLIBCR/LIBH instead of receiving extracted runtime OBJ files.
            shim_names = list(u.shims)
            for i, b in enumerate(blobs):
                if i < len(shim_names):
                    object_kind, piece_id, shim_segment = 'link-shim', None, shim_names[i]
                elif u.accepted:
                    piece = u.objs[i - len(shim_names)]['piece']
                    object_kind = 'accepted'
                    piece_id = piece.get('id') or piece.get('name')
                    shim_segment = None
                else:
                    object_kind = 'raw-debt' if u.kind in ('raw-code', 'raw-data', 'bss') else 'link-scaffold'
                    piece_id, shim_segment = None, None
                # Debt OMF inputs stay conspicuous in both DOS 8.3 filenames and
                # the sidecar inventory; their payload still comes only from the
                # locked image / reviewed BSS partition.
                file_prefix = 'D' if object_kind == 'raw-debt' else ('S' if object_kind == 'link-shim' else prefix)
                fn = '%s%03d.OBJ' % (file_prefix, len(names))
                (work / fn).write_bytes(b)
                names.append(fn)
                out.append(fn)
                object_manifest.append({
                    'file': fn, 'group': group_name, 'unit': u.id, 'unit_kind': u.kind,
                    'object_kind': object_kind, 'piece': piece_id, 'shim_segment': shim_segment,
                    'start': u.start, 'end': u.end, 'sha256': sha(b),
                    'generated_from': (
                        ('reviewed-bss-partition' if u.kind == 'bss' else
                         'locked-oracle-load-image-and-relocations')
                        if object_kind == 'raw-debt' else None),
                })
        return out
    exp = write(explicit, 'E', 'explicit')
    rt = write(runtime, 'R', 'runtime')
    gm = write(game, 'G', 'explicit-game' if getattr(args, 'game_input', 'library') == 'explicit'
               else 'game-library')
    root_names = []
    root_seen = set()

    def root_add(name):
        if name not in root_seen:
            root_seen.add(name)
            root_names.append(name)

    if pinned:
        # The archive members are hash-pinned contributions in the accepted
        # image. Root their reviewed publics so LINK's ordinary library search
        # retains even a member whose entry is reached indirectly.
        runtime_owner_rows = [o for o in c.manifest.get('owners', [])
                              if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY']
        from runtime_binding import runtime_data_members
        runtime_owner_rows.extend(runtime_data_members(c.manifest))
        runtime_members_by_identity = {
            (owner.get('library'), owner.get('module'), owner.get('module_sha256')): owner
            for owner in runtime_owner_rows}
        for lib in PINNED_RUNTIME_LIBRARIES:
            for module, blob in OmfReader().split_library(toolchain_path(lib).read_bytes()):
                owner = runtime_members_by_identity.get((lib, module, sha(blob)))
                if owner is None:
                    continue
                obj = OmfReader(communals=True).read(blob)
                local = {p['name'] for p in obj.local_publics}
                for pub in obj.publics:
                    if pub['name'] not in local:
                        root_add(pub['name'])
        if getattr(args, 'game_input', 'library') == 'library':
            for u, _ in game:
                for entry in u.objs:
                    local = {p['name'] for p in entry['obj'].local_publics}
                    for pub in entry['obj'].publics:
                        if pub['name'] not in local:
                            root_add(pub['name'])
                for shim in u.shims.values():
                    for name, _offset in shim.publics:
                        root_add(name)
        root_blob = library_root_object(root_names)
        root_file = 'ROOT.OBJ'
        (work / root_file).write_bytes(root_blob)
        object_manifest.append({
            'file': root_file, 'group': 'explicit', 'unit': 'link_root', 'unit_kind': 'link-root',
            'object_kind': 'link-root', 'piece': None, 'shim_segment': None,
            'start': None, 'end': None, 'sha256': sha(root_blob), 'generated_from': 'reviewed-publics',
        })
        exp.append(root_file)
    (work / 'objects.json').write_text(json.dumps(object_manifest, indent=1))
    logs = {}
    libraries = ['RT.LIB', 'GAME.LIB']
    if pinned:
        # The unchanged pinned runtime libraries (identity checked by toolchain_path).
        pinned_names = []
        for lib in PINNED_RUNTIME_LIBRARIES:
            data = toolchain_path(lib).read_bytes()
            (work / Path(lib).name).write_bytes(data)
            pinned_names.append(Path(lib).name)
            logs[Path(lib).name] = {'pinned': lib, 'sha256': sha(data)}
        if getattr(args, 'game_input', 'library') == 'explicit':
            libraries = pinned_names
        else:
            libraries = (['GAME.LIB'] + pinned_names if args.library_order == 'game-first'
                         else pinned_names + ['GAME.LIB'])
        if args.library_order == 'combined' and getattr(args, 'game_input', 'library') != 'explicit':
            # Hypothesis: the game modules were appended to a copy of the pinned
            # MLIBCR.LIB (one library, game modules after member 220).
            (work / 'GAME.LIB').write_bytes((work / 'MLIBCR.LIB').read_bytes())
            libraries = ['GAME.LIB'] + pinned_names[1:]
    for lib, files in (('RT.LIB', rt), ('GAME.LIB', gm)):
        if pinned and (lib == 'RT.LIB' or
                       (lib == 'GAME.LIB' and getattr(args, 'game_input', 'library') == 'explicit')):
            continue
        rsp = ['+%s &' % f for f in files[:-1]] + ['+%s' % files[-1]] if files else []
        head = [lib] if (work / lib).exists() else [lib, 'Y']
        if lib == 'GAME.LIB':
            response = render_historical_response('GAME.RSP.in', {
                '@LIBRARY_HEADER@': '\r\n'.join(head),
                '@GAME_MODULES@': '\r\n'.join(rsp),
            })
        else:
            response = '\r\n'.join(head + rsp + ['NUL;']) + '\r\n'
        (work / (lib[:-4] + '.RSP')).write_bytes(response.encode('ascii'))
        rc, out, argv = run_dos(tc / 'LIB.EXE', ['@%s.RSP' % lib[:-4]], work, 300)
        logs[lib] = {'returncode': rc, 'log': out[-1500:], 'modules': len(files), 'command': argv}
    lines = []
    link_objects = exp + gm if getattr(args, 'game_input', 'library') == 'explicit' else exp
    for i in range(0, len(link_objects), 8):
        chunk = '+'.join(x[:-4] for x in link_objects[i:i + 8])
        lines.append(chunk + ('+' if i + 8 < len(link_objects) else ''))
    opts = ' '.join(args.link_options)
    resp = render_historical_response('LINK.RSP.in', {
        '@OBJECT_MODULES@': '\r\n'.join(lines),
        '@OUTPUT_EXE@': 'RESULT.EXE',
        '@MAP_FILE@': 'RESULT.MAP',
        '@LIBRARIES_AND_OPTIONS@': '+'.join(libraries) + ' ' + opts,
    })
    (work / 'LINK.RSP').write_bytes(resp.encode('ascii'))
    rc, out, argv = run_dos(tc / 'LINK.EXE', ['@LINK.RSP'], work, 600)
    (work / 'link.log').write_text(out)
    result = {'mode': 'library', 'runtime': 'pinned libraries' if pinned else 'units',
              'game_library_order': getattr(args, 'game_order', 'oracle'),
              'library_line': '+'.join(libraries), 'libraries': logs, 'link_returncode': rc,
              'link_log': out[-3000:], 'link_command': argv,
              'object_manifest': 'objects.json',
              'link_root': {'file': 'ROOT.OBJ', 'publics': len(root_names),
                            'sha256': sha(root_blob)} if pinned else None,
              'raw_debt_objects': [r['file'] for r in object_manifest if r['object_kind'] == 'raw-debt'],
              'game_input': getattr(args, 'game_input', 'library'),
              'explicit_objects': len(link_objects),
              'runtime_library_modules': len(members) if pinned else len(rt),
              'game_library_modules': 0 if getattr(args, 'game_input', 'library') == 'explicit' else len(gm)}
    if not (work / 'RESULT.EXE').exists():
        return result
    exe = (work / 'RESULT.EXE').read_bytes()
    mapping = (work / 'RESULT.MAP').read_text(errors='replace')
    mz = MZ.parse(exe)
    img = mz.load_image(exe)
    units = [u for u, _ in explicit + runtime + game]
    placed = _unit_placements(units, mapping)
    if pinned:
        # Runtime units: linked start from one code public of their pinned member.
        pubs = {}
        for m in PUBLIC_LINE.finditer(mapping):
            pubs[m[3]] = int(m[1], 16) * 16 + int(m[2], 16)
        for uid, (lib, module, blob) in members.items():
            obj = OmfReader().read(blob)
            local = {q['name'] for q in obj.local_publics}
            code = [q for q in obj.publics if q['segment'] == '_TEXT' and q['name'] not in local
                    and q['name'] in pubs]
            if code:
                placed[uid] = pubs[code[0]['name']] - code[0]['offset']
        result['runtime_members_placed'] = sum(1 for uid in members if uid in placed)
    loaded = [u for u in units if u.id in placed]
    result['units_total'] = len([u for u in units if u.start is not None])
    result['units_placed'] = len(loaded)
    checked_groups = runtime if getattr(args, 'game_input', 'library') == 'explicit' else runtime + game
    result['library_units_not_loaded'] = sorted(u.id for u, _ in checked_groups
                                                if u.id not in placed and u.start is not None)[:80]
    result['image_size'] = len(img)
    result['image_equal'] = img == c.image[:IMAGE_INIT_END]

    def interval_lookup(rows):
        rows = sorted(rows)
        starts = [r[0] for r in rows]

        def f(site):
            i = bisect.bisect_right(starts, site) - 1
            if i >= 0 and rows[i][0] <= site < rows[i][1]:
                return rows[i][2]
            return None
        return f
    linked_unit = interval_lookup([(placed[u.id], placed[u.id] + (u.end - u.start), u.id) for u in loaded])
    oracle_unit = interval_lookup([(u.start, u.end, u.id) for u in loaded])
    by_id = {u.id: u for u in loaded}
    # Each linked relocation mapped back to its original site through its unit's
    # placement, then EXEPACK's stable 64 KiB bank partition by original site.
    mapped = []
    for r in mz.relocations:
        x = linked_unit(r['load_offset'])
        if x is not None:
            mapped.append(r['load_offset'] - placed[x] + by_id[x].start)
    linked_order = bank_partition(mapped)
    oracle_order = [s for s in c.relocs if oracle_unit(s) is not None]
    fd = first_diff(linked_order, oracle_order)
    result['relocations_mapped'] = len(mapped)
    result['relocations_of_loaded_units_in_oracle'] = len(oracle_order)
    result['relocation_order_equal'] = linked_order == oracle_order
    result['relocation_order_first_divergence'] = fd
    result['relocation_order_positions_equal'] = sum(1 for x, y in zip(linked_order, oracle_order) if x == y)
    banks = {}
    for bank in range(4):
        lseq = _run_sequence([s for s in linked_order if s >> 16 == bank], oracle_unit)
        oseq = _run_sequence([s for s in oracle_order if s >> 16 == bank], oracle_unit)
        d = first_diff(lseq, oseq)
        banks[bank] = {'linked_runs': len(lseq), 'oracle_runs': len(oseq), 'equal': lseq == oseq,
                       'first_divergence': d,
                       'context': None if d is None else {'linked': lseq[max(0, d - 3):d + 6],
                                                          'oracle': oseq[max(0, d - 3):d + 6]}}
        if bank in (1, 2):
            banks[bank]['linked_sequence'] = lseq
            banks[bank]['oracle_sequence'] = oseq
    result['relocation_unit_runs_by_bank'] = banks
    result['segment_order'] = [m[4] for m in re.finditer(
        r'^\s*([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+(\S+)\s+(\S+)', mapping, re.M)]
    return result


# --------------------------------------------------------------------------- driver
def stage_inputs(c, stage):
    """integ32: compose candidate (name, source bytes, recipe) triples into the
    manifest exactly as promotion does (promote.apply_ownership), sources written
    under build/reallink/stage/.  Used by the BSS real-link gate and --stage."""
    if not stage:
        return []
    import copy
    import promote as P
    from oracle import verify
    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    work = OUT / 'stage'
    work.mkdir(parents=True, exist_ok=True)
    names = []
    for name, source, recipe in stage:
        recipe = copy.deepcopy(recipe)
        path = work / (name + '-' + uuid.uuid4().hex + Path(recipe['source']).suffix)
        recipe['source'] = path.relative_to(ROOT).as_posix()
        c.manifest = P.apply_ownership(c.manifest, name, recipe, oracle, image)
        path.write_bytes(source)
        c.recipes['recipes/%s.json' % name] = recipe
        names.append(name)
    return names


def run(runtime='members', partial='raw', link_options=None, no_alias_shims=False,
        flat_dgroup=False, order='oracle', tag='default', log=log, library_order='runtime-first',
        game_order='oracle', stage=None, stage_runtime=None, stage_communal=None,
        game_input='library'):
    args = argparse.Namespace(runtime=runtime, partial=partial, library_order=library_order,
                              game_order=game_order,
                              game_input=game_input,
                              link_options=link_options or list(LINK_OPTIONS),
                              no_alias_shims=no_alias_shims, flat_dgroup=flat_dgroup, order=order, tag=tag)
    OUT.mkdir(parents=True, exist_ok=True)
    c = load_inputs()
    staged = stage_inputs(c, stage)
    if stage_communal is not None:
        # integ39: the whole communal unit, composed as batch publication does.
        import communal_unit
        c.manifest = communal_unit.attach(c.manifest, stage_communal)
        staged.append(stage_communal['id'])
    if stage_runtime:
        # integ36: pinned runtime candidates composed as promote_runtime composes them.
        import promote_runtime
        for candidate in stage_runtime:
            c.manifest = promote_runtime.replace_raw_library(c.manifest, candidate)
            staged.append(candidate.get('id') or candidate.get('owner') or 'fill_%d' % candidate['start'])
    units = model_dgroup_tail(split_raw_data(c, build_units(c, args)), not args.flat_dgroup)
    units, fills = drop_alignment_fill(c, units)
    odd = odd_data_hypothesis(c, units, apply=False)
    log('%d units' % len(units))
    rows = build_accepted(c, units, args)
    log('%d accepted objects built' % len(rows))
    units = add_special_units(units, c, stack=not (order == 'library' and runtime == 'libraries'))
    defined, resolved, conflicts, unresolved, problems = resolve_symbols(c, units)
    alias_bindings = _bind_accepted_code_aliases(units, resolved)
    anchors, anchor_pubs, frames = choose_anchors(c, units)
    issues = make_raw_objects(c, units, frames, anchors)
    for u, name, off in anchor_pubs:
        u.raw.publics.append((name, off))
    hidden, aliases = [], []
    placed_syms = 0
    kinds_raw = ('raw-code', 'raw-data', 'bss')
    crt0 = next((u for u in units if u.id.startswith('raw_rt_dos_crt0.asm')), None)
    # integ36: with the pinned CRT0 OBJ linked, its MODEND carries the entry and
    # unresolved absolutes go to the first raw _TEXT unit.
    # integ37: with every runtime _TEXT member linked from its OBJ (chksum.asm
    # included) no raw _TEXT unit remains; the zero-length prelude hosts them.
    absolute_host = crt0 or next((u for u in units if u.kind == 'raw-code' and u.segment == '_TEXT'),
                                 next(u for u in units if u.kind == 'prelude'))
    for name, (kind, addr, frame, src) in sorted(resolved.items()):
        if kind == 'absolute':
            absolute_host.raw.absolutes.append((name, addr))
            continue
        if name in alias_bindings:
            continue
        u, seg, s = containing_unit(units, addr, kinds_raw)
        if u is None:
            au, aseg, as_ = containing_unit(units, addr, ('c', 'asm', 'rt', 'rt-data', 'far-data', 'data-module'))
            if au is None:
                hidden.append({'symbol': name, 'address': addr, 'source': src, 'inside': None})
                continue
            # alias shim: zero-length contribution emitted right before the accepted
            # object, same segment/alignment, PUBDEF at the original address.
            e0 = au.objs[0]['obj']
            if aseg in ('_DATA', '_BSS'):
                sd = next(x for x in e0.segment_defs if x['name'] == aseg)
                segname, klass, grp = aseg, {'_DATA': 'DATA', '_BSS': 'BSS'}[aseg], 'DGROUP'
            elif au.kind == 'far-data':
                sd = next(x for x in e0.segment_defs if x['class'] == 'FAR_DATA')
                segname, klass, grp = sd['name'], 'FAR_DATA', None
            else:
                sd = next((x for x in e0.segment_defs if x['class'] == 'CODE' and x['length']),
                          {'alignment': 'word'})
                segname, klass, grp = au.segment, 'CODE', None
            al = {'paragraph': 'para'}.get(sd['alignment'], sd['alignment'])
            shim = au.shims.get(segname)
            if shim is None:
                shim = au.shims[segname] = RawObject(('alias_' + au.id)[:40], segname, klass, al, b'', group=grp)
            shim.publics.append((name, addr - as_))
            aliases.append({'symbol': name, 'address': addr, 'source': src, 'inside': au.id})
            continue
        if kind == 'code' and u.kind == 'raw-code' and frame is not None and frames.get(u.segment) != frame:
            problems.append('%s: table frame %s != link segment frame %s (%s)'
                            % (name, frame, frames.get(u.segment), u.segment))
        u.raw.publics.append((name, addr - s))
        placed_syms += 1
    if crt0 is not None:
        crt0.raw.start = 0
    ordered, dropped, noncontig = processing_order(c, units)
    ordered = place_bss_placeholders(ordered)
    log('order: %d units, %d dropped order edges' % (len(ordered), len(dropped)))
    adaptations = [r for r in rows if r['info'].get('adaptations')]
    report = {'tag': args.tag, 'args': vars(args), 'staged': staged, 'bytes': summarize_bytes(units),
              'order_basis': ('oracle-derived: address order, segment first appearance, DGROUP order and '
                              'the oracle per-bank relocation order (not independent evidence)'
                              if order == 'oracle' else
                              'library search: LINK loads library modules itself (experiment)'),
              'unit_counts': dict(__import__('collections').Counter(u.kind for u in units)),
              'accepted_objects': rows,
              'asm_link_adaptations': [{'unit': r['unit'], 'piece': r['piece'], 'changes': r['info']['adaptations']}
                                       for r in adaptations],
              'symbols': {'needed_resolved': len(resolved), 'placed_in_raw': placed_syms,
                          'unplaceable': hidden, 'unresolved': unresolved,
                          'alias_shims': aliases,
                          'alias_bindings': list(alias_bindings.values()),
                          'table_vs_operand_conflicts': conflicts},
              'odd_data_starts': odd,
              'dgroup_alignment_fill_bytes': fills,
              'modelling_problems': sorted(set(problems)), 'raw_object_issues': issues,
              'order': [{'unit': u.id, 'kind': u.kind, 'start': u.start, 'end': u.end, 'data': u.data,
                         'notes': u.notes} for u in ordered],
              'dropped_order_edges': dropped, 'noncontiguous_relocation_units': noncontig}
    if order == 'library':
        report['library'] = library_link(c, ordered, args)
    else:
        result, outputs = run_link(c, ordered, args)
        log('LINK rc', result['returncode'])
        report['link'] = result
        if outputs:
            exe, packed, mapping = outputs
            report['compare'] = compare(c, units, exe, packed, mapping)
    path = OUT / ('report-%s.json' % args.tag)
    path.write_text(json.dumps(report, indent=1, default=str))
    report['path'] = path.relative_to(ROOT).as_posix()
    return report


def summary(report):
    """Compact divergence summary (validate.py --image)."""
    cm = report.get('compare', {})
    link = report.get('link', {})
    errors = re.findall(r'error L\d+[^\r\n]*', link.get('log', ''))
    pk = cm.get('packed', {})
    return {'status': 'DIAGNOSTIC_ONLY', 'order_basis': report['order_basis'],
            'report': report.get('path'),
            'link_returncode': link.get('returncode'), 'link_errors': errors[:20],
            'image_equal': cm.get('image_equal'), 'image_mismatch_bytes': cm.get('image_mismatch_bytes'),
            'mismatch_runs': [[r['start'], r['end'], r['unit']] for r in cm.get('mismatch_runs', [])][:40],
            'relocation_set_equal': cm.get('link_relocation_set_equal'),
            'relocations_missing': cm.get('relocations_missing_count'),
            'relocations_extra': cm.get('relocations_extra_count'),
            'bank_order_equal': cm.get('bank_partitioned_order_equal'),
            'bank_order_positions_equal': cm.get('bank_partitioned_positions_equal'),
            'relocation_provenance': cm.get('relocation_provenance'),
            'header': {'linked': cm.get('link_header'), 'oracle': cm.get('oracle_header')},
            'packed_equal': pk.get('equal'), 'packed_mismatch_bytes': pk.get('mismatch_bytes'),
            'bytes': report['bytes'], 'alias_shims': len(report['symbols']['alias_shims']),
            'asm_link_adaptations': len(report['asm_link_adaptations']),
            'odd_data_starts': report['odd_data_starts'],
            'link_options': link.get('options'), 'bss': cm.get('bss'), 'runtime': cm.get('runtime')}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--runtime', choices=['raw', 'members', 'libraries'], default='members',
                    help='raw: runtime _TEXT as raw debt; members: accepted pinned members as extracted OBJs; '
                         'libraries (--order library only): LINK searches the pinned MLIBCR.LIB/LIBH.LIB')
    ap.add_argument('--game-order', choices=['oracle', 'address', 'image', 'image-back'], default='oracle',
                    help='--order library: GAME.LIB member order (oracle-derived processing order, '
                         'independent load-image address order, or the image-derived DGROUP-anchor '
                         'merge of the code-segment chains, integ31)')
    ap.add_argument('--library-order', choices=['runtime-first', 'game-first', 'combined'],
                    default='runtime-first',
                    help='--runtime libraries: library search order on the LINK line')
    ap.add_argument('--game-input', choices=['library', 'explicit'], default='library',
                    help='library: package game units in GAME.LIB; explicit: pass every game unit to LINK')
    ap.add_argument('--partial', choices=['raw', 'split'], default='raw',
                    help='partially accepted C objects: whole raw debt (default) or accepted pieces + raw pieces')
    ap.add_argument('--link-options', nargs='*', default=list(LINK_OPTIONS),
                    help='LINK switches (bash users: MSYS_NO_PATHCONV=1)')
    ap.add_argument('--no-alias-shims', action='store_true',
                    help='do not emit alias PUBDEF shims (leaves name-binding debt unresolved)')
    ap.add_argument('--flat-dgroup', action='store_true',
                    help='do not model the DGROUP tail segments (XP / MSG): everything raw in _DATA')
    ap.add_argument('--order', choices=['oracle', 'library'], default='oracle',
                    help='oracle: oracle-derived processing order (default); library: library-search experiment')
    ap.add_argument('--tag', default='default', help='report name under build/reallink/')
    ap.add_argument('--stage', nargs=3, action='append', metavar=('NAME', 'SOURCE', 'RECIPE'),
                    help='compose a candidate into the manifest as promotion would (diagnostic)')
    a = ap.parse_args()
    stage = [(n, Path(s).read_bytes(), read_json(Path(r))) for n, s, r in (a.stage or [])]
    report = run(a.runtime, a.partial, a.link_options, a.no_alias_shims, a.flat_dgroup, a.order, a.tag,
                 library_order=a.library_order, game_order=a.game_order, stage=stage,
                 game_input=a.game_input)
    if a.order == 'library':
        lib = report['library']
        log(json.dumps({k: v for k, v in lib.items() if k != 'relocation_unit_runs_by_bank'}, indent=1)[:6000])
        for bank, row in lib.get('relocation_unit_runs_by_bank', {}).items():
            log('bank', bank, {k: v for k, v in row.items() if not k.endswith('_sequence')})
    else:
        log(json.dumps(summary(report), indent=1))
    log('report:', report['path'])


if __name__ == '__main__':
    main()
