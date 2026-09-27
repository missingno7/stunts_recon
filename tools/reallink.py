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
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
from compiler import verify_toolchain, toolchain_path, check_inline_asm  # noqa: E402
from preprocessor import prepare  # noqa: E402
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
STACK_SIZE = 8000
# absolute runtime markers (pinned CRT sources; acceptance doc: __AHSHIFT = 12, __acrtused = 9876h)
STACK_IN_DGROUP = True
ABSOLUTES = {'__AHSHIFT': 12, '__acrtused': 0x9876}


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
        return cache.read_bytes(), {'cached': True, 'flags': flags, 'profile': profile}
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
    return obj.read_bytes(), {'cached': False, 'flags': flags, 'profile': profile}


SEGLINE = re.compile(r'^(\s*)([A-Za-z_$@?][\w$@?]*)(\s+segment\b)(.*)$', re.I)


def adapt_asm(text, link_segment, force_byte=False):
    """Diagnostic link adaptation of a reconstruction ASM source.

    Only the CODE segment is renamed to the link unit's output segment and
    its class set to 'CODE' (reconstruction ASM uses _TEXT / seg012 with class
    'STUNTSC', which a real link would merge into the runtime _TEXT or move
    behind all CODE segments).  Returns (text, list_of_changes)."""
    changes = []
    code_names = set()
    lines = text.split('\n')
    for i, line in enumerate(lines):
        m = SEGLINE.match(line)
        if not m:
            continue
        name, rest = m.group(2), m.group(4)
        cls = re.search(r"'([^']*)'", rest)
        if name.upper() == 'DSEG' and cls and cls.group(1).upper() == 'DATA':
            new = f"{m.group(1)}{name}{m.group(3)} byte public 'STUNTSD'"
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
        return text, changes
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
    text = (ROOT / recipe['source']).read_bytes().decode('ascii').replace('\r\n', '\n').replace('\r', '\n')
    text, changes = adapt_asm(text, link_segment, force_byte)
    staged = text.replace('\n', '\r\n').encode('ascii')
    key = sha(staged + json.dumps(config['flags']).encode())[:16]
    cache = OUT / 'cache' / f'{base}_{key}.OBJ'
    info = {'flags': config['flags'], 'profile': recipe['profile'], 'adaptations': changes}
    if cache.exists():
        return cache.read_bytes(), info
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
    cache.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(obj, cache)
    return obj.read_bytes(), info


def library_member(owner):
    from omf import OmfReader as R
    lib = toolchain_path(owner['library'])
    data = lib.read_bytes()
    assert sha(data) == owner['library_sha256'], 'pinned library changed'
    hits = [b for n, b in R().split_library(data) if n == owner['module'] and sha(b) == owner['module_sha256']]
    assert len(hits) == 1, owner['module']
    return hits[0]


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


def build_units(c, args):
    units = []
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
            if args.runtime == 'members' and lib and data_segs:
                u = Unit(f'raw_{o["id"]}', 'raw-code', seg, lo, hi)
                u.notes.append('pinned member carries DGROUP data ' + ','.join(data_segs) +
                               ': needs a runtime DGROUP segment model; linked as raw')
                units.append(u)
                continue
            if args.runtime == 'members' and lib and not raw_real and contained:
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
    for w in owners_in(c, DGROUP_BASE, INIT_DATA_END):
        s, e = max(w['start'], DGROUP_BASE), min(w['end'], INIT_DATA_END)
        if w['kind'] == 'MATCHING_C_DATA' and w.get('parent'):
            parent = byid.get(w['parent'])
            if parent is None or parent.kind != 'c':
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
            units.append(Unit(f'rawdata_{s}', 'raw-data', '_DATA', s, e))
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
            if pos < s:
                out.append(Unit(f'rawdata_{pos}', 'raw-data', '_DATA', pos, s)); pos = s
            v = Unit(f'rawdata_{seg}_{s}', 'raw-data', seg, max(s, pos), min(e, u.end))
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


def drop_alignment_fill(c, units):
    """A single zero byte at an odd address directly before an accepted
    word-aligned _DATA contribution is LINK alignment fill, not an object:
    LINK re-creates it from the next contribution's WORD alignment."""
    starts = {s for u in units if u.kind in ('c', 'data-module') for seg, s, e in u.extents() if seg == '_DATA'}
    out, fills = [], []
    for u in units:
        if (u.kind == 'raw-data' and u.end - u.start == 1 and u.start % 2 == 1 and
                c.image[u.start] == 0 and u.end in starts):
            fills.append(u.start)
            continue
        out.append(u)
    return out, fills


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
            else:
                raise AssertionError(u.kind)
            obj = OmfReader().read(data)
            u.objs.append({'bytes': data, 'obj': obj, 'piece': p, 'info': info})
            rows.append({'unit': u.id, 'piece': p.get('id') or p.get('name'), 'info': info,
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
    needed = set()
    for u in units:
        for e in u.objs:
            obj = e['obj']
            for name, scope in zip(obj.externals, obj.external_scopes or ['external'] * len(obj.externals)):
                if scope == 'external' and name not in defined:
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
            r = RawObject(u.id[:40], u.segment, RAW_DATA_CLASS[u.segment], 'byte', c.image[u.start:u.end],
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
    # keys for data-only units
    last = -1.0
    for i, (_, u) in enumerate(dchain):
        if u.kind in ('raw-data', 'data-module'):
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
    return rep


# --------------------------------------------------------------------------- main
def add_special_units(units):
    pre = Unit('prelude', 'prelude')
    pre.key = -10
    pre.raw = RawObject('prelude', 'DSEG', 'STUNTSD', 'byte', b'', group='DGROUP')
    pre.notes.append('zero-length DGROUP declaration so MASM F0 DSEG frames equal the DGROUP base')
    bss = Unit('bss', 'bss', '_BSS', IMAGE_INIT_END, BSS_END)
    bss.key = 1e9
    bss.raw = RawObject('bss', '_BSS', 'BSS', 'word', b'', length=BSS_END - IMAGE_INIT_END, group='DGROUP')
    stack = Unit('stack', 'stack', 'STACK', None, None)
    stack.key = 1e9 + 1
    stack.raw = RawObject('stack', 'STACK', 'STACK', 'para', b'', length=STACK_SIZE, combine=5,
                          group='DGROUP' if STACK_IN_DGROUP else None)
    return [pre] + units + [bss, stack]


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
    placed.append((rd[0], '$$F_DGROUP', 0))
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
            if u.kind == 'bss':
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


def library_link(c, ordered, args):
    """Library-search hypothesis: explicit seg000-seg009 objects (plus raw
    DGROUP debt, BSS and STACK), then RT.LIB (runtime _TEXT units) and
    GAME.LIB (seg011-seg037, the seg012 modules and the far/DGROUP data
    modules).  LINK chooses which library modules to load and in which order;
    nothing here is taken from the oracle order."""
    import bisect
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
        if u.kind in ('prelude', 'raw-data', 'bss', 'stack') or u.segment in explicit_segments:
            explicit.append((u, blobs))
        elif u.segment == '_TEXT':
            runtime.append((u, blobs))
        else:
            game.append((u, blobs))
    # prelude first (DSEG at the DGROUP base), then the explicit code objects in
    # address order, raw DGROUP debt, BSS/STACK last (segment declarations only)
    rank = {'prelude': 0, 'raw-data': 2, 'bss': 3, 'stack': 4}
    explicit.sort(key=lambda t: (rank.get(t[0].kind, 1), t[0].start if t[0].kind != 'raw-data' else 0))
    names = []

    def write(group, prefix):
        out = []
        for u, blobs in group:
            for b in blobs:
                fn = '%s%03d.OBJ' % (prefix, len(names))
                (work / fn).write_bytes(b)
                names.append(fn)
                out.append(fn)
        return out
    exp = write(explicit, 'E')
    rt = write(runtime, 'R')
    gm = write(game, 'G')
    logs = {}
    for lib, files in (('RT.LIB', rt), ('GAME.LIB', gm)):
        rsp = ['+%s &' % f for f in files[:-1]] + ['+%s' % files[-1], 'NUL;'] if files else []
        (work / (lib[:-4] + '.RSP')).write_bytes(('\r\n'.join([lib, 'Y'] + rsp) + '\r\n').encode('ascii'))
        rc, out, _ = run_dos(tc / 'LIB.EXE', ['@%s.RSP' % lib[:-4]], work, 300)
        logs[lib] = {'returncode': rc, 'log': out[-1500:], 'modules': len(files)}
    lines = []
    for i in range(0, len(exp), 8):
        chunk = '+'.join(x[:-4] for x in exp[i:i + 8])
        lines.append(chunk + ('+' if i + 8 < len(exp) else ''))
    opts = ' '.join(args.link_options)
    resp = '\r\n'.join(lines + ['RESULT.EXE', 'RESULT.MAP', 'RT.LIB+GAME.LIB ' + opts + ';']) + '\r\n'
    (work / 'LINK.RSP').write_bytes(resp.encode('ascii'))
    rc, out, argv = run_dos(tc / 'LINK.EXE', ['@LINK.RSP'], work, 600)
    (work / 'link.log').write_text(out)
    result = {'mode': 'library', 'libraries': logs, 'link_returncode': rc, 'link_log': out[-3000:],
              'explicit_objects': len(exp), 'runtime_library_modules': len(rt), 'game_library_modules': len(gm)}
    if not (work / 'RESULT.EXE').exists():
        return result
    exe = (work / 'RESULT.EXE').read_bytes()
    mapping = (work / 'RESULT.MAP').read_text(errors='replace')
    mz = MZ.parse(exe)
    img = mz.load_image(exe)
    units = [u for u, _ in explicit + runtime + game]
    placed = _unit_placements(units, mapping)
    loaded = [u for u in units if u.id in placed]
    result['units_total'] = len([u for u in units if u.start is not None])
    result['units_placed'] = len(loaded)
    result['library_units_not_loaded'] = sorted(u.id for u, _ in runtime + game
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
        if bank == 1:
            banks[bank]['linked_sequence'] = lseq
            banks[bank]['oracle_sequence'] = oseq
    result['relocation_unit_runs_by_bank'] = banks
    result['segment_order'] = [m[4] for m in re.finditer(
        r'^\s*([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+(\S+)\s+(\S+)', mapping, re.M)]
    return result


# --------------------------------------------------------------------------- driver
def run(runtime='members', partial='raw', link_options=None, no_alias_shims=False,
        flat_dgroup=False, order='oracle', tag='default', log=log):
    args = argparse.Namespace(runtime=runtime, partial=partial,
                              link_options=link_options or ['/DOSSEG', '/NOI', '/NOD', '/MAP', '/CP:1'],
                              no_alias_shims=no_alias_shims, flat_dgroup=flat_dgroup, order=order, tag=tag)
    OUT.mkdir(parents=True, exist_ok=True)
    c = load_inputs()
    units = model_dgroup_tail(split_raw_data(c, build_units(c, args)), not args.flat_dgroup)
    units, fills = drop_alignment_fill(c, units)
    odd = odd_data_hypothesis(c, units, apply=False)
    log('%d units' % len(units))
    rows = build_accepted(c, units, args)
    log('%d accepted objects built' % len(rows))
    units = add_special_units(units)
    defined, resolved, conflicts, unresolved, problems = resolve_symbols(c, units)
    anchors, anchor_pubs, frames = choose_anchors(c, units)
    issues = make_raw_objects(c, units, frames, anchors)
    for u, name, off in anchor_pubs:
        u.raw.publics.append((name, off))
    hidden, aliases = [], []
    placed_syms = 0
    kinds_raw = ('raw-code', 'raw-data', 'bss')
    crt0 = next(u for u in units if u.id.startswith('raw_rt_dos_crt0.asm'))
    for name, (kind, addr, frame, src) in sorted(resolved.items()):
        if kind == 'absolute':
            crt0.raw.absolutes.append((name, addr))
            continue
        u, seg, s = containing_unit(units, addr, kinds_raw)
        if u is None:
            au, aseg, as_ = containing_unit(units, addr, ('c', 'asm', 'rt', 'far-data', 'data-module'))
            if au is None:
                hidden.append({'symbol': name, 'address': addr, 'source': src, 'inside': None})
                continue
            # alias shim: zero-length contribution emitted right before the accepted
            # object, same segment/alignment, PUBDEF at the original address.
            e0 = au.objs[0]['obj']
            if aseg == '_DATA':
                sd = next(x for x in e0.segment_defs if x['name'] == '_DATA')
                segname, klass, grp = '_DATA', 'DATA', 'DGROUP'
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
    crt0.raw.start = 0
    ordered, dropped, noncontig = processing_order(c, units)
    log('order: %d units, %d dropped order edges' % (len(ordered), len(dropped)))
    adaptations = [r for r in rows if r['info'].get('adaptations')]
    report = {'tag': args.tag, 'args': vars(args), 'bytes': summarize_bytes(units),
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
            'odd_data_starts': report['odd_data_starts']}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--runtime', choices=['raw', 'members'], default='members',
                    help='raw: runtime _TEXT as raw debt; members: accepted pinned members as extracted OBJs')
    ap.add_argument('--partial', choices=['raw', 'split'], default='raw',
                    help='partially accepted C objects: whole raw debt (default) or accepted pieces + raw pieces')
    ap.add_argument('--link-options', nargs='*', default=['/DOSSEG', '/NOI', '/NOD', '/MAP', '/CP:1'],
                    help='LINK switches (bash users: MSYS_NO_PATHCONV=1)')
    ap.add_argument('--no-alias-shims', action='store_true',
                    help='do not emit alias PUBDEF shims (leaves name-binding debt unresolved)')
    ap.add_argument('--flat-dgroup', action='store_true',
                    help='do not model the DGROUP tail segments (XP / MSG): everything raw in _DATA')
    ap.add_argument('--order', choices=['oracle', 'library'], default='oracle',
                    help='oracle: oracle-derived processing order (default); library: library-search experiment')
    ap.add_argument('--tag', default='default', help='report name under build/reallink/')
    a = ap.parse_args()
    report = run(a.runtime, a.partial, a.link_options, a.no_alias_shims, a.flat_dgroup, a.order, a.tag)
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
