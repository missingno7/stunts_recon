#!/usr/bin/env python3
"""Block-level, shift-robust comparison of a candidate compile against the original function.

DIAGNOSTIC ONLY.  Similarity numbers never establish acceptance; strict acceptance is
tools/tubench.py (member bytes with resolved fixups) and tools/promote.py --verify-only.

Usage:
  python blockdiff.py SOURCE.c --function NAME [--stub-others] [--all-blocks] [--asm] [--json OUT]

SOURCE may be a standalone candidate or a whole-TU file; the member is located by its
emitted public.  Compilation reuses tools/tubench.py's pinned compile helper (canonical
msc510-medium /AM /O /Gs).  Target bytes come from the locked oracle image.

Scoring: both byte streams are decoded (capstone 5.0.3, 16-bit) into instruction tokens.
Coarse keys (mnemonic + operands with every numeric literal replaced by '#') are aligned by
edit distance (common prefix/suffix trimmed first, difflib anchors for large middles).
Costs: inserted/deleted/substituted instruction = 1.0; aligned pair with identical bytes,
or identical outside candidate FIXUPP bytes = 0; relative branch whose destination maps to
the aligned counterpart of the target's destination = 0 (pure layout shift) else 0.5;
other operand/immediate/displacement difference = 0.5; inserted/deleted NOP pad = 0.25.
Switch jump tables (`jmp cs:[bx+X]`) are decoded as data words, not instructions.
"""
from __future__ import annotations

import argparse
import difflib
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import pcore  # noqa: E402  (sets up sys.path for tools/ and build/python)

import capstone  # noqa: E402

assert capstone.__version__ == '5.0.3', 'pinned decoder differs'
_CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
_CS.detail = True

REL_BRANCH = re.compile(r'^(j\w+|loop\w*|call)$')
TERMINATORS = {'jmp', 'ret', 'retf', 'iret', 'ljmp'}
NUM = re.compile(r'0x[0-9a-fA-F]+|\b\d+\b')
FRAME_PENALTY = 2.0  # search objective only (score_key); not part of the reported cost
PREFIXES = {0x26, 0x2E, 0x36, 0x3E, 0xF0, 0xF2, 0xF3, 0x66, 0x67, 0x64, 0x65}
NOP_INDEL = 0.25  # alignment pads (90) follow from upstream size shifts; weigh them lightly


class Insn:
    __slots__ = ('off', 'size', 'raw', 'mnem', 'ops', 'key', 'dest', 'mask', 'data')

    def __init__(self, off, raw, mnem, ops, data=False):
        self.off, self.raw, self.mnem, self.ops, self.data = off, raw, mnem, ops, data
        self.size = len(raw)
        self.dest = None
        self.mask = frozenset()
        self.key = 'dw' if data else (mnem + ' ' + NUM.sub('#', ops)).strip()

    def text(self):
        return (f'{self.off:04x}: {self.raw.hex():<12} {self.mnem} {self.ops}').rstrip()

    def end(self):
        return self.off + self.size


def _is_rel_branch(ins):
    # Relative branches: E8 (call rel16), E9/EB (jmp), 7x/0F 8x (jcc), E0-E3 (loop/jcxz)
    b = ins.raw
    if not b:
        return False
    op = b[0]
    return op in (0xE8, 0xE9, 0xEB) or 0x70 <= op <= 0x7F or 0xE0 <= op <= 0xE3 or \
        (op == 0x0F and len(b) > 1 and 0x80 <= b[1] <= 0x8F)


def _switch_tables(insns, code, table_base):
    """Find `jmp word ptr cs:[bx + X]` tables; return set of function-relative table starts."""
    starts = set()
    for ins in insns:
        if ins.mnem == 'jmp' and 'cs:[bx' in ins.ops.replace(' ', ''):
            m = re.search(r'\+\s*(0x[0-9a-f]+|\d+)\]', ins.ops)
            if m:
                disp = int(m.group(1), 0)
                rel = table_base(ins, disp)
                if rel is not None and 0 <= rel < len(code):
                    starts.add(rel)
    return starts


def disassemble(code: bytes, fixups=None, *, table_base=None):
    """Decode function bytes into Insn list.

    fixups: list of member-relative fixup dicts (candidate side) or None (target side).
    table_base(ins, disp) -> function-relative offset of a switch table, or None.
    """
    fix_positions = {}
    fix_starts = set()
    for fx in fixups or ():
        fix_starts.add(fx['offset'])
        for k in range(fx['offset'], fx['offset'] + fx.get('width', 2)):
            fix_positions[k] = fx
    tables = set()

    def sweep(table_words):
        out = []
        pos = 0
        n = len(code)
        while pos < n:
            if pos in table_words:
                out.append(Insn(pos, code[pos:pos + 2], 'dw', code[pos:pos + 2][::-1].hex(), data=True))
                out[-1].mask = frozenset(k - pos for k in range(pos, pos + 2) if k in fix_positions)
                pos += 2
                continue
            got = next(_CS.disasm(code[pos:pos + 16], pos, 1), None)
            if got is None:
                out.append(Insn(pos, code[pos:pos + 1], 'db', f'0x{code[pos]:02x}', data=True))
                pos += 1
                continue
            size, mnem, ops = got.size, got.mnemonic, got.op_str
            ins = Insn(pos, code[pos:pos + size], mnem, ops)
            # coarse key: mnemonic + encoding with displacement/immediate fields zeroed, so
            # fixup-bearing fields (encoded as 0 in the candidate) and constants align.
            enc = bytearray(ins.raw)
            for off_, sz_ in ((got.disp_offset, got.disp_size), (got.imm_offset, got.imm_size)):
                if off_ and sz_:
                    enc[off_:off_ + sz_] = b'\0' * sz_
            p0 = next((k for k, b in enumerate(enc) if b not in PREFIXES), 0)
            if enc[p0] in (0x9A, 0xEA) and size >= p0 + 5:         # far call/jmp ptr16:16
                enc[p0 + 1:p0 + 5] = b'\0' * 4
            elif 0xA0 <= enc[p0] <= 0xA3 and size >= p0 + 3:       # mov acc, moffs16
                enc[p0 + 1:p0 + 3] = b'\0' * 2
            ins.key = mnem + ' ' + enc.hex()
            ins.mask = frozenset(k - pos for k in range(pos, pos + size) if k in fix_positions)
            if _is_rel_branch(ins) and not ins.mask:
                try:
                    ins.dest = int(ops.split()[-1], 0)
                except ValueError:
                    ins.dest = None
            out.append(ins)
            pos += size
        return out

    insns = sweep(set())
    if table_base is not None:
        starts = _switch_tables(insns, code, table_base)
        if starts:
            words = set()
            for s in starts:
                p = s
                while p + 2 <= len(code):
                    if fixups is not None:
                        if p not in fix_starts:
                            break
                    else:
                        val = int.from_bytes(code[p:p + 2], 'little')
                        if not table_base.valid_entry(val):
                            break
                    words.add(p)
                    p += 2
                    if p in starts:
                        continue
            tables = words
            insns = sweep(words)
    return insns


def _target_table_base(target):
    segoff = target.get('segment_offset')
    size = len(target['bytes'])

    def base(ins, disp):
        return None if segoff is None else disp - segoff

    base.valid_entry = lambda val: segoff is not None and 0 <= val - segoff < size
    return base


def _candidate_table_base(fixups):
    by_off = {fx['offset']: fx for fx in fixups}

    def base(ins, disp):
        # displacement field is the last 2 bytes of the jmp; its fixup addend is the
        # segment-relative table offset, which the caller converts via member_start.
        fx = by_off.get(ins.off + ins.size - 2)
        if fx is None or not fx.get('encoded_addend'):
            return None
        addend = int.from_bytes(bytes.fromhex(fx['encoded_addend'])[:2], 'little')
        return addend - base.member_start

    base.member_start = 0
    base.valid_entry = lambda val: True
    return base


# ---------------------------------------------------------------- alignment

def _levenshtein(a, b):
    """Return op list [(tag, i, j)] aligning key lists a and b (tags: eq/sub/del/ins)."""
    n, m = len(a), len(b)
    if n == 0:
        return [('ins', None, j) for j in range(m)]
    if m == 0:
        return [('del', i, None) for i in range(n)]
    # full DP with traceback (sizes are bounded by caller)
    prev = list(range(m + 1))
    back = [bytearray(m + 1) for _ in range(n + 1)]  # 0 diag,1 up(del),2 left(ins)
    for j in range(1, m + 1):
        back[0][j] = 2
    for i in range(1, n + 1):
        cur = [i] + [0] * m
        ai = a[i - 1]
        bi = back[i]
        bi[0] = 1
        for j in range(1, m + 1):
            d = prev[j - 1] + (0 if ai == b[j - 1] else 1)
            u = prev[j] + 1
            l = cur[j - 1] + 1
            if d <= u and d <= l:
                cur[j] = d
            elif u <= l:
                cur[j] = u
                bi[j] = 1
            else:
                cur[j] = l
                bi[j] = 2
        prev = cur
    ops = []
    i, j = n, m
    while i > 0 or j > 0:
        t = back[i][j] if (i > 0 and j > 0) else (1 if i > 0 else 2)
        if t == 0:
            ops.append(('eq' if a[i - 1] == b[j - 1] else 'sub', i - 1, j - 1))
            i -= 1
            j -= 1
        elif t == 1:
            ops.append(('del', i - 1, None))
            i -= 1
        else:
            ops.append(('ins', None, j - 1))
            j -= 1
    ops.reverse()
    return ops


def align(ta, ca):
    """Align target/candidate Insn lists by coarse key. Returns ops with absolute indices."""
    a = [x.key for x in ta]
    b = [x.key for x in ca]
    n, m = len(a), len(b)
    p = 0
    while p < n and p < m and a[p] == b[p]:
        p += 1
    s = 0
    while s < n - p and s < m - p and a[n - 1 - s] == b[m - 1 - s]:
        s += 1
    ops = [('eq', i, i) for i in range(p)]
    mid_a, mid_b = a[p:n - s], b[p:m - s]

    def sub_align(xa, xb, oa, ob):
        if len(xa) * len(xb) <= 160000:
            return [(t, None if i is None else i + oa, None if j is None else j + ob)
                    for t, i, j in _levenshtein(xa, xb)]
        out = []
        sm = difflib.SequenceMatcher(None, xa, xb, autojunk=False)
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == 'equal':
                out += [('eq', oa + i1 + k, ob + j1 + k) for k in range(i2 - i1)]
            else:
                ga, gb = xa[i1:i2], xb[j1:j2]
                if len(ga) * len(gb) <= 160000:
                    out += [(t, None if i is None else i + oa + i1, None if j is None else j + ob + j1)
                            for t, i, j in _levenshtein(ga, gb)]
                else:
                    k = min(len(ga), len(gb))
                    out += [('sub', oa + i1 + q, ob + j1 + q) for q in range(k)]
                    out += [('del', oa + i1 + q, None) for q in range(k, len(ga))]
                    out += [('ins', None, ob + j1 + q) for q in range(k, len(gb))]
        return out

    ops += sub_align(mid_a, mid_b, p, p)
    ops += [('eq', n - s + k, m - s + k) for k in range(s)]
    return ops


def _fine(ti, ci, t2c, t_index_by_off, c_index_by_off):
    """Cost and class for a coarse-equal pair."""
    if ti.raw == ci.raw:
        return 0.0, 'same'
    if len(ti.raw) == len(ci.raw) and ci.mask:
        if all(ti.raw[k] == ci.raw[k] for k in range(len(ci.raw)) if k not in ci.mask):
            return 0.0, 'fixup'
    if ti.dest is not None and ci.dest is not None:
        # opcode identical, destination differs: check destination correspondence
        if ti.raw[:1] == ci.raw[:1]:
            tdi = t_index_by_off.get(ti.dest)
            cdi = c_index_by_off.get(ci.dest)
            if tdi is not None and cdi is not None and t2c.get(tdi) == cdi:
                return 0.0, 'shift'
            return 0.5, 'branch'
    return 0.5, 'operand'


def _frame(insns):
    info = {'frame': 0, 'saved': [], 'bp_frame': False}
    k = 0
    if len(insns) > 1 and insns[0].mnem == 'push' and insns[0].ops == 'bp' and \
            insns[1].mnem == 'mov' and insns[1].ops.replace(' ', '') == 'bp,sp':
        info['bp_frame'] = True
        k = 2
        if k < len(insns) and insns[k].mnem == 'sub' and insns[k].ops.startswith('sp,'):
            info['frame'] = int(insns[k].ops.split(',')[1], 0)
            k += 1
        while k < len(insns) and insns[k].mnem == 'dec' and insns[k].ops == 'sp':
            info['frame'] += 1
            k += 1
    while k < len(insns) and insns[k].mnem == 'push' and insns[k].ops in ('si', 'di'):
        info['saved'].append(insns[k].ops)
        k += 1
    return info


def _leaders(insns):
    by_off = {x.off: i for i, x in enumerate(insns)}
    leaders = {0}
    for i, x in enumerate(insns):
        if x.data:
            if x.mnem == 'dw':
                v = int.from_bytes(x.raw, 'little')
            leaders.add(i)
            if i + 1 < len(insns):
                leaders.add(i + 1)
            continue
        if x.dest is not None and x.raw[0] != 0xE8:
            if x.dest in by_off:
                leaders.add(by_off[x.dest])
            if i + 1 < len(insns):
                leaders.add(i + 1)
        elif x.mnem in TERMINATORS:
            if i + 1 < len(insns):
                leaders.add(i + 1)
    return sorted(leaders)


def compare(target, cand_bytes, cand_fixups, member_start=0):
    """Compare target dict (pcore.load_target) with candidate member bytes + fixups."""
    tcode = target['bytes']
    ccode = cand_bytes
    tins = disassemble(tcode, None, table_base=_target_table_base(target))
    cb = _candidate_table_base(cand_fixups)
    cb.member_start = member_start
    cins = disassemble(ccode, cand_fixups, table_base=cb)
    ops = align(tins, cins)
    t2c = {i: j for t, i, j in ops if t in ('eq', 'sub')}
    t_by_off = {x.off: i for i, x in enumerate(tins)}
    c_by_off = {x.off: i for i, x in enumerate(cins)}
    detailed = []
    cost = 0.0
    classes = {}
    for t, i, j in ops:
        if t == 'eq':
            c, cls = _fine(tins[i], cins[j], t2c, t_by_off, c_by_off)
        elif t == 'sub':
            c, cls = 1.0, 'subst'
        elif t == 'del':
            c, cls = (NOP_INDEL, 'missing_nop') if tins[i].raw == b'\x90' else (1.0, 'missing')
        else:
            c, cls = (NOP_INDEL, 'extra_nop') if cins[j].raw == b'\x90' else (1.0, 'extra')
        cost += c
        classes[cls] = classes.get(cls, 0) + 1
        detailed.append((t, i, j, c, cls))

    # exactness outside candidate fixups
    cmask = set()
    for fx in cand_fixups:
        cmask.update(range(fx['offset'], fx['offset'] + fx.get('width', 2)))
    same_len = len(tcode) == len(ccode)
    diff_positions = [k for k in range(min(len(tcode), len(ccode)))
                      if tcode[k] != ccode[k] and k not in cmask]
    exact_outside = same_len and not diff_positions
    raw_exact = tcode == ccode
    # target MZ relocation sites vs candidate segment-part fixup positions
    cand_seg_sites = sorted(fx['offset'] + 2 for fx in cand_fixups
                            if fx.get('loc') == 'pointer32' and fx.get('width', 2) == 4)
    cand_seg_sites += sorted(fx['offset'] for fx in cand_fixups if fx.get('loc') == 'base16')
    cand_seg_sites = sorted(cand_seg_sites)

    # blocks (target CFG leaders)
    leaders = _leaders(tins)
    block_of = {}
    for bi, li in enumerate(leaders):
        end = leaders[bi + 1] if bi + 1 < len(leaders) else len(tins)
        for k in range(li, end):
            block_of[k] = bi
    blocks = [{'index': bi, 't_first': li, 't_last': (leaders[bi + 1] if bi + 1 < len(leaders) else len(tins)) - 1,
               'c_idx': [], 'cost': 0.0, 'first': None} for bi, li in enumerate(leaders)]
    cur = 0
    for t, i, j, c, cls in detailed:
        if i is not None:
            cur = block_of.get(i, cur)
        blk = blocks[cur]
        if j is not None:
            blk['c_idx'].append(j)
        blk['cost'] += c
        if c > 0 and blk['first'] is None:
            blk['first'] = {'class': cls,
                            'target': tins[i].text() if i is not None else None,
                            'candidate': cins[j].text() if j is not None else None}
    block_rows = []
    for blk in blocks:
        t0 = tins[blk['t_first']].off
        t1 = tins[blk['t_last']].end()
        if blk['c_idx']:
            c0 = min(cins[k].off for k in blk['c_idx'])
            c1 = max(cins[k].end() for k in blk['c_idx'])
        else:
            c0 = c1 = None
        csize = (c1 - c0) if c0 is not None else 0
        block_rows.append({'block': blk['index'], 'target': [t0, t1], 'candidate': [c0, c1],
                           'target_size': t1 - t0, 'candidate_size': csize,
                           'size_delta': csize - (t1 - t0), 'cost': round(blk['cost'], 2),
                           'first_difference': blk['first']})
    n = max(len(tins), len(cins), 1)
    tf, cf = _frame(tins), _frame(cins)
    return {
        'function': target['name'],
        'target_size': len(tcode), 'candidate_size': len(ccode), 'size_delta': len(ccode) - len(tcode),
        'target_insns': len(tins), 'candidate_insns': len(cins),
        'frame': {'target': tf, 'candidate': cf, 'same': tf == cf},
        'cost': round(cost, 2), 'similarity_pct': round(100.0 * (1 - cost / n), 2),
        'op_classes': classes,
        'raw_exact': raw_exact, 'exact_outside_candidate_fixups': exact_outside,
        'first_raw_difference': (diff_positions[0] if diff_positions else
                                 (None if same_len else min(len(tcode), len(ccode)))),
        'differing_bytes_outside_fixups': len(diff_positions) + abs(len(tcode) - len(ccode)),
        'target_reloc_sites': target.get('reloc_sites'), 'candidate_segment_fixup_sites': cand_seg_sites,
        'reloc_sites_agree': target.get('reloc_sites') == cand_seg_sites,
        'blocks': block_rows,
        '_detail': (tins, cins, detailed),
        'authority': 'DIAGNOSTIC_ONLY: shift-robust similarity; not strict extent/binding/relocation acceptance.',
    }


def score_key(result):
    """Objective for search: lower is better."""
    if result is None:
        return (float('inf'),)
    # A frame (sub sp,N / saved SI,DI) mismatch is structural evidence, so it adds FRAME_PENALTY.
    frame_pen = 0.0 if result['frame']['same'] else FRAME_PENALTY
    return (0 if result['exact_outside_candidate_fixups'] else 1, result['cost'] + frame_pen,
            abs(result['size_delta']), result['differing_bytes_outside_fixups'])


def compile_and_compare(source: bytes, function: str, target=None, stub_others=False):
    target = target or pcore.load_target(function)
    text = source.decode('ascii')
    if stub_others:
        import csrc
        text = csrc.stub_other_functions(text, function)
    obj, _, _ = pcore.compile_c(text.encode('ascii'))
    seg, start, end, _ = pcore.member_extent(obj, function)
    data, fixups = pcore.member_bytes(obj, function)
    return compare(target, data, fixups, member_start=start)


def render(result, all_blocks=False, asm=False, out=sys.stdout):
    r = result
    w = out.write
    w(f"function {r['function']}: emitted {r['candidate_size']} / target {r['target_size']} bytes "
      f"(delta {r['size_delta']:+d}); insns {r['candidate_insns']}/{r['target_insns']}\n")
    tf, cf = r['frame']['target'], r['frame']['candidate']
    w(f"frame: target sub sp,{tf['frame']} saved {tf['saved']} | candidate sub sp,{cf['frame']} saved {cf['saved']}"
      f"{'' if r['frame']['same'] else '  <-- DIFFERS'}\n")
    w(f"score: cost {r['cost']}  similarity {r['similarity_pct']}%  classes {r['op_classes']}\n")
    w(f"exact: raw={'yes' if r['raw_exact'] else 'no'}  outside-candidate-fixups="
      f"{'YES' if r['exact_outside_candidate_fixups'] else 'no'}  first raw diff "
      f"{'-' if r['first_raw_difference'] is None else hex(r['first_raw_difference'])}  "
      f"differing bytes {r['differing_bytes_outside_fixups']}\n")
    if not r['reloc_sites_agree']:
        w(f"segment relocation sites: target {r['target_reloc_sites']} candidate {r['candidate_segment_fixup_sites']}\n")
    w('blocks (target CFG; cost>0 only unless --all-blocks):\n')
    w('  blk  target        candidate      tsz  csz  delta  cost  first difference\n')
    for b in r['blocks']:
        if not all_blocks and b['cost'] == 0 and b['size_delta'] == 0:
            continue
        t0, t1 = b['target']
        c0, c1 = b['candidate']
        cs = '-' if c0 is None else f'{c0:04x}-{c1:04x}'
        fd = b['first_difference']
        fds = '' if fd is None else f"[{fd['class']}] T:{fd['target'] or '-'} | C:{fd['candidate'] or '-'}"
        w(f"  {b['block']:>3}  {t0:04x}-{t1:04x}  {cs:>13}  {b['target_size']:>4} {b['candidate_size']:>4} "
          f"{b['size_delta']:>+5} {b['cost']:>5}  {fds}\n")
    if asm:
        tins, cins, detailed = r['_detail']
        w('alignment (cost>0 lines marked *):\n')
        for t, i, j, c, cls in detailed:
            tt = tins[i].text() if i is not None else ''
            ct = cins[j].text() if j is not None else ''
            w(f"{'*' if c else ' '} {tt:<44} | {ct:<44} {cls if c else ''}\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('source')
    ap.add_argument('--function', required=True)
    ap.add_argument('--stub-others', action='store_true',
                    help='replace other top-level function bodies with {} before compiling (faster; '
                         'verify once that the member output is unchanged)')
    ap.add_argument('--all-blocks', action='store_true')
    ap.add_argument('--asm', action='store_true', help='print full aligned instruction listing')
    ap.add_argument('--json', help='write JSON result here')
    args = ap.parse_args()
    src = Path(args.source).read_bytes()
    try:
        result = compile_and_compare(src, args.function, stub_others=args.stub_others)
    except pcore.CompileError as error:
        print('COMPILE FAILED:', error)
        print(error.log[-3000:])
        return 2
    render(result, args.all_blocks, args.asm)
    if args.json:
        doc = {k: v for k, v in result.items() if not k.startswith('_')}
        Path(args.json).write_text(json.dumps(doc, indent=1))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
