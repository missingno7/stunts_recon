"""typeinfer.py -- program-wide type/prototype evidence from the ORIGINAL executable.

Reads the locked oracle load image, the reviewed function inventory, the DGROUP
data-symbol layout and the manifest; disassembles every mapped function and
collects machine-level evidence:

  globals (per data symbol and byte offset): access widths, dword pairs
    (mov ax,[g] / mov dx,[g+2]), sign extension (cbw / cwd) vs zero extension
    (sub ah,ah), signed vs unsigned jumps after cmp, sar/shr, idiv/div,
    near-pointer dereference (mov bx,[g]; ...[bx]), far pointer loads (les/lds),
    indexed accesses with inferred strides, address-taken immediates.
  functions: callee-side parameter widths/kinds ([bp+6..]), signedness and
    pointer use of parameters; caller-side argument bytes (add sp,N), per-word
    argument kinds (char pushed without extension = prototyped char parameter,
    push ds/ss + offset = far pointer, dx:ax pairs), return value use
    (none / AL / AL+cbw / AL+zx / AX / DX:AX / forwarded).

Then (--compare) it parses the declarations of accepted C sources and the big
candidate TUs (cdecls.py) and reports CONFLICTS between declared types and the
evidence, and between TUs that declare the same address differently.

Outputs (build/namefit/):
  evidence.json   raw aggregated evidence
  decls.json      one inferred declaration per global/function (+ evidence summary)
  decls_draft.h   generated header draft
  conflicts.json  declared-vs-evidence and cross-TU conflicts

    python tools/typeinfer.py            # evidence + decls + conflicts
    python tools/typeinfer.py --show NAME # evidence for one global/function

Diagnostic only; does not touch canonical state.
"""
import argparse
import bisect
import collections
import json
import re
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
OUT = ROOT / 'build' / 'namefit'
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'build/python'))
sys.path.insert(0, str(HERE))

from capstone import Cs, CS_ARCH_X86, CS_MODE_16                      # noqa: E402
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_OP_IMM            # noqa: E402

SIGNED_J = {'jl', 'jge', 'jle', 'jg', 'jnge', 'jnl', 'jng', 'jnle'}
UNSIGNED_J = {'jb', 'jae', 'jbe', 'ja', 'jnae', 'jnb', 'jna', 'jnbe', 'jc', 'jnc'}
REG16 = {'al': 'ax', 'ah': 'ax', 'ax': 'ax', 'bl': 'bx', 'bh': 'bx', 'bx': 'bx', 'cl': 'cx', 'ch': 'cx',
         'cx': 'cx', 'dl': 'dx', 'dh': 'dx', 'dx': 'dx', 'si': 'si', 'di': 'di', 'bp': 'bp', 'sp': 'sp',
         'es': 'es', 'ds': 'ds', 'ss': 'ss', 'cs': 'cs', 'eax': 'ax', 'edx': 'dx', 'ecx': 'cx', 'ebx': 'bx',
         'esi': 'si', 'edi': 'di'}
KNOWN_RESIDUALS = {
    # function name -> short description (supervisor NOTES / docs U1-U8; open or pressure-sensitive)
    'update_frame': 'seg003 CSE temps depend on symbol-table pressure (names vs aggregates)',
    'end_hiscore': 'seg000 dead pop si at 17614 depends on symbol-table pressure',
    'setup_car_shapes': 'seg005 y_pos CSE (class 1/3)',
    'loop_game': 'seg005 temp slot (class 1/3)',
    'audio_driver_timer': 'seg007 C6 residual +8',
    'pad_id': 'U1 S1 SI counter without home',
    'handle_ingame_kb_shortcuts': 'U6 jbe vs jle',
    'enter_hiscore': 'U4 DI vs SI temp (exact in whole seg000)',
    'track_setup': 'frame+homes exact, body class 1',
    'run_car_menu': 'seg000 block placement (exact in whole seg000)',
    'read_line': 'seg032 class 1 (830/832)',
    'draw_2DtrackMap': 'seg009 missing source (-124)',
    'load_intro_resources': 'frame 70 vs 66',
    'sub_2298C': 'U8 ES reuse (accepted)',
    'main': 'ported_stuntsmain_ -16',
}


def log(*a):
    print(*a, file=sys.stderr, flush=True)


# ----------------------------------------------------------------------------- inputs
class Program:
    def __init__(self):
        from oracle import verify
        from mz import MZ
        from context import _inventory
        from common import read_json
        result = verify(write=False)
        mz = MZ.parse(result[1])
        self.image = mz.load_image(result[1])
        self.relocs = {r['load_offset'] for r in mz.relocations}
        inv = _inventory()
        self.functions = sorted((f for f in inv['functions'] if isinstance(f.get('start'), int)
                                 and isinstance(f.get('end'), int) and f['end'] > f['start']),
                                key=lambda f: f['start'])
        self.fn_by_start = {f['start']: f for f in self.functions}
        self.ida_globals = {}
        for g in inv.get('globals', []):
            if isinstance(g, dict) and g.get('name'):
                self.ida_globals.setdefault(g['name'], g.get('directive'))
        ds = read_json(ROOT / 'layout/data-symbols.json')
        self.frame = ds['frame_load_address']
        names = collections.defaultdict(list)
        widths = {}
        for name, s in ds['symbols'].items():
            a = s['load_address']
            names[a].append((name, s))
            if 'width' in s and not s.get('clone_of'):
                widths[a] = max(widths.get(a, 0), s['width'])
        self.sym_names = {a: [n for n, _ in v] for a, v in names.items()}
        self.sym_meta = {a: v for a, v in names.items()}
        self.addrs = sorted(names)
        self.extent = {}
        for i, a in enumerate(self.addrs):
            nxt = self.addrs[i + 1] if i + 1 < len(self.addrs) else a + 2
            gap = nxt - a
            w = widths.get(a)
            self.extent[a] = (w, 'layout') if w else (gap, 'gap')
        self.name_to_addr = {}
        for a, ns in self.sym_names.items():
            for n in ns:
                self.name_to_addr[n[1:] if n.startswith('_') else n] = a
        # code names
        cs = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        self.code_name_to_addr = {}
        for n, s in cs.items():
            t = (s.get('mapped_target') or {}).get('start')
            if isinstance(t, int):
                self.code_name_to_addr[n[1:] if n.startswith('_') else n] = t
        for f in self.functions:
            self.code_name_to_addr.setdefault(f['name'], f['start'])
            self.code_name_to_addr.setdefault(f['name'].lstrip('_'), f['start'])
        # owners
        man = read_json(ROOT / 'layout/manifest.json')
        self.owners = sorted(((o['start'], o['end'], o['kind'], o['id']) for o in man['owners']
                              if isinstance(o.get('start'), int) and isinstance(o.get('end'), int)))
        self.owner_starts = [o[0] for o in self.owners]

    def owner(self, addr):
        i = bisect.bisect_right(self.owner_starts, addr) - 1
        if i >= 0 and self.owners[i][0] <= addr < self.owners[i][1]:
            return self.owners[i]
        return None

    def primary_name(self, a):
        ns = self.sym_meta.get(a, [])
        prim = [n for n, s in ns if not s.get('clone_of')]
        return (prim or [n for n, _ in ns] or [f'_dg_{a - self.frame:04x}'])[0]

    def lookup(self, dsoff):
        """DS offset -> (symbol load address, offset inside) or None."""
        la = self.frame + dsoff
        i = bisect.bisect_right(self.addrs, la) - 1
        if i < 0:
            return None
        a = self.addrs[i]
        w, _ = self.extent[a]
        if la - a < max(w, 1):
            return a, la - a
        return None

    def fn_name(self, start):
        f = self.fn_by_start.get(start)
        return f['name'] if f else f'sub_{start:05X}'


# ----------------------------------------------------------------------------- evidence store
class Ev:
    def __init__(self):
        self.g = collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
        self.p = collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
        self.calls = collections.defaultdict(lambda: {'argbytes': collections.Counter(),
                                                      'argwords': collections.defaultdict(collections.Counter),
                                                      'ret': collections.Counter(), 'callers': set(),
                                                      'kinds': collections.Counter()})
        self.r = collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
        self.gfuncs = collections.defaultdict(set)     # (addr) -> functions touching it
        self.examples = collections.defaultdict(list)

    def add(self, key, cat, val=1, site=None, fn=None):
        kind, a, off = key
        store = self.g if kind == 'g' else self.p if kind == 'p' else self.r
        store[(a, off) if kind == 'g' else (a, off)][cat][val] += 1
        if kind == 'g' and fn is not None:
            self.gfuncs[a].add(fn)
        if site is not None:
            ex = self.examples[(kind, a, off, cat, str(val))]
            if len(ex) < 3:
                ex.append(site)


def tag(src, w, ext=None, const=None, scale=1):
    return {'src': src, 'w': w, 'ext': ext, 'const': const, 'scale': scale}


class FunctionScan:
    def __init__(self, prog, ev, fn):
        self.prog, self.ev, self.fn = prog, ev, fn
        self.start, self.end = fn['start'], fn['end']
        self.far = fn.get('distance') != 'near'
        self.pbase = 6 if self.far else 4
        own = prog.owner(self.start)
        self.origin = own[2] if own else 'UNOWNED'
        self.fname = fn['name']

    def disasm(self):
        """Recursive descent from the entry (jcc/jmp/fallthrough, MSC jump tables); sorted by address."""
        md = Cs(CS_ARCH_X86, CS_MODE_16)
        md.detail = True
        img = self.prog.image
        segbase = (self.fn.get('segment_paragraph') or 0) * 16
        seen = {}
        work = [self.start]
        self.table_bytes = set()
        while work:
            a = work.pop()
            while self.start <= a < self.end and a not in seen and a not in self.table_bytes:
                got = next(md.disasm(img[a:min(a + 16, self.end)], a), None)
                if got is None:
                    break
                seen[a] = got
                mn = got.mnemonic
                if mn in ('ret', 'retf', 'iret', 'hlt'):
                    break
                if mn.startswith('j') or mn.startswith('loop'):
                    op = got.operands[0] if got.operands else None
                    if op is not None and op.type == X86_OP_IMM:
                        work.append(op.imm)
                    elif op is not None and op.type == X86_OP_MEM and got.reg_name(op.mem.segment) == 'cs'                             and segbase:
                        # MSC switch table: preceding `cmp ax, N` bounds the entry count (entries = N+1)
                        prev = [seen[x] for x in sorted(seen) if x < a][-6:]
                        bound = None
                        for q in reversed(prev):
                            if q.mnemonic == 'cmp' and len(q.operands) == 2 and q.operands[1].type == X86_OP_IMM:
                                bound = q.operands[1].imm
                                break
                        t = segbase + (op.mem.disp & 0xFFFF)
                        count = (bound + 1) if bound is not None and 0 <= bound < 256 else 0
                        for e in range(count):
                            tgt = segbase + int.from_bytes(img[t + 2 * e:t + 2 * e + 2], 'little')
                            self.table_bytes.update((t + 2 * e, t + 2 * e + 1))
                            if self.start <= tgt < self.end:
                                work.append(tgt)
                    if mn in ('jmp', 'ljmp'):
                        break
                a = got.address + got.size
        return [seen[a] for a in sorted(seen)]

    # operand helpers
    def mem_key(self, ins, op, regs):
        """Classify a memory operand -> (key, info) ; key=('g',addr,off) or ('p',fn,off) or None."""
        m = op.mem
        seg = ins.reg_name(m.segment) if m.segment else None
        base = ins.reg_name(m.base) if m.base else None
        index = ins.reg_name(m.index) if m.index else None
        disp = m.disp
        if base == 'bp' and seg in (None, 'ss'):
            if index is None and disp >= self.pbase:
                return ('p', self.start, disp - self.pbase), {'direct': True}
            return None, {'local': disp}
        if seg in ('es', 'cs'):
            if seg == 'es' and base in ('bx', 'si', 'di') and regs.get('es') and regs['es'].get('src'):
                return None, {'far_deref': regs['es']['src'], 'off': disp, 'base': base}
            return None, {'seg': seg}
        d16 = disp & 0xFFFF
        if base is None and index is None:
            hit = self.prog.lookup(d16)
            if hit:
                return ('g', hit[0], hit[1]), {'direct': True}
            return None, {'unmapped_ds': d16}
        # based access
        breg = regs.get(REG16.get(base)) if base else None
        if index is None and breg and breg.get('src') and breg.get('ptr'):
            return None, {'near_deref': breg['src'], 'off': disp, 'base': base}
        if d16 >= 0x100 and not (disp < 0 and -disp < 0x800 and self.prog.lookup(d16) is None):
            hit = self.prog.lookup(d16)
            if hit:
                sc = breg.get('scale') if breg and index is None else None
                return ('g', hit[0], hit[1]), {'indexed': True, 'stride': sc}
        if breg and breg.get('src') and index is None:
            return None, {'near_deref': breg['src'], 'off': disp, 'base': base}
        return None, {'based': True}

    def run(self):
        prog, ev = self.prog, self.ev
        insns = self.disasm()
        labels = set()
        for ins in insns:
            if ins.group(1) or ins.mnemonic.startswith('j') or ins.mnemonic.startswith('loop'):
                for op in ins.operands:
                    if op.type == X86_OP_IMM and self.start <= op.imm < self.end:
                        labels.add(op.imm)
        regs = {}
        site = lambda ins: f'{self.fname}+0x{ins.address - self.start:X}'
        n = len(insns)
        for i, ins in enumerate(insns):
            mn = ins.mnemonic
            if ins.address in labels:
                regs = {}
            ops = list(ins.operands)
            # --- memory operand evidence
            memkeys = []
            for oi, op in enumerate(ops):
                if op.type != X86_OP_MEM:
                    continue
                key, info = self.mem_key(ins, op, regs)
                width = op.size
                if mn in ('les', 'lds'):
                    width = 4
                if key:
                    memkeys.append((oi, key, info, width))
                    acc = 'read'
                    if oi == 0 and len(ops) >= 1:
                        if mn in ('mov', 'pop') or mn.startswith('set'):
                            acc = 'write'
                        elif mn in ('cmp', 'test', 'push', 'lcall', 'call', 'ljmp', 'jmp'):
                            acc = 'read'
                        else:
                            acc = 'rmw'
                    ev.add(key, 'width', width, site(ins), self.fname)
                    ev.add(key, 'access', acc)
                    ev.add(key, 'origin', self.origin)
                    if info.get('indexed'):
                        ev.add(key, 'indexed', str(info.get('stride')), site(ins), self.fname)
                    if mn in ('les', 'lds'):
                        ev.add(key, 'far_load', mn, site(ins), self.fname)
                    if mn in ('sar',):
                        ev.add(key, 'sign', 'signed', site(ins))
                    if mn in ('shr',):
                        ev.add(key, 'sign', 'unsigned', site(ins))
                    if mn in ('idiv', 'imul') and oi == 0:
                        ev.add(key, 'sign' if mn == 'idiv' else 'mulsign', 'signed', site(ins))
                    if mn in ('div', 'mul') and oi == 0:
                        ev.add(key, 'sign' if mn == 'div' else 'mulsign', 'unsigned', site(ins))
                    if mn in ('lcall', 'call'):
                        ev.add(key, 'call_through', width, site(ins), self.fname)
                if info.get('near_deref'):
                    src = info['near_deref']
                    ev.add(src, 'near_deref', width, site(ins), self.fname)
                    ev.add(src, 'pointee_off', f"{info['off']}:{width}")
                if info.get('far_deref'):
                    src = info['far_deref']
                    ev.add(src, 'far_deref', width, site(ins), self.fname)
                    ev.add(src, 'pointee_off', f"{info['off']}:{width}")
            # --- dword pairs: [k] then [k+2] in adjacent instruction
            if memkeys and i + 1 < n:
                nxt = insns[i + 1]
                for oi, key, info, width in memkeys:
                    if width != 2 or not info.get('direct'):
                        continue
                    for op2 in nxt.operands:
                        if op2.type == X86_OP_MEM and nxt.mnemonic == mn:
                            k2, inf2 = self.mem_key(nxt, op2, regs)
                            if k2 and inf2.get('direct') and k2[0] == key[0] and k2[1] == key[1] \
                                    and abs(k2[2] - key[2]) == 2:
                                lo = min(k2[2], key[2])
                                ev.add((key[0], key[1], lo), 'dword_pair', mn, site(ins), self.fname)
            # --- compare + jcc signedness
            if mn in ('cmp', 'or', 'test', 'and', 'sub') and i + 1 < n:
                j = insns[i + 1].mnemonic
                sg = 'signed' if j in SIGNED_J else 'unsigned' if j in UNSIGNED_J else None
                if sg and (mn == 'cmp' or (mn == 'or' and sg == 'signed')):
                    srcs = []
                    for op in ops:
                        if op.type == X86_OP_MEM:
                            key, info = self.mem_key(ins, op, regs)
                            if key:
                                srcs.append((key, op.size, None))
                        elif op.type == X86_OP_REG:
                            t = regs.get(REG16.get(ins.reg_name(op.reg)))
                            if t and t.get('src'):
                                srcs.append((t['src'], t['w'], t.get('ext')))
                    zx_present = any(e == 'zx' for _, _, e in srcs)
                    for key, w, ext in srcs:
                        if ext in ('zx', 'sx'):
                            continue       # the extension already told us; compare type is the promoted one
                        if zx_present and sg == 'unsigned' and w == 2:
                            continue       # MSC: unsigned char vs int compares unsigned (E3)
                        ev.add(key, 'sign', sg, site(ins))
            # --- calls
            if mn in ('lcall', 'call') and not any(op.type == X86_OP_MEM or op.type == X86_OP_REG for op in ops):
                self.call(insns, i, regs, site)
                for r in ('ax', 'bx', 'cx', 'dx', 'es'):
                    regs.pop(r, None)
                tgt = self.call_target(insns, i)
                regs['ax'] = tag(('r', tgt, 0), 2)
                regs['dx'] = tag(('r', tgt, 2), 2)
                continue
            if mn in ('lcall', 'call'):
                for r in ('ax', 'bx', 'cx', 'dx', 'es'):
                    regs.pop(r, None)
                continue
            # --- register tag transfer
            self.transfer(ins, ops, regs, site)
        return insns

    def call_target(self, insns, i):
        ins = insns[i]
        if ins.mnemonic == 'lcall' and ins.bytes[0] == 0x9A:
            seg = int.from_bytes(ins.bytes[3:5], 'little')
            off = int.from_bytes(ins.bytes[1:3], 'little')
            return seg * 16 + off
        if ins.mnemonic == 'call':
            for op in ins.operands:
                if op.type == X86_OP_IMM:
                    return op.imm
        return None

    def call(self, insns, i, regs, site):
        ev = self.ev
        tgt = self.call_target(insns, i)
        if tgt is None:
            return
        rec = ev.calls[tgt]
        rec['callers'].add(self.fname)
        near_far = 'far' if insns[i].mnemonic == 'lcall' else (
            'pushcs' if i > 0 and insns[i - 1].mnemonic == 'push' and insns[i - 1].op_str == 'cs' else 'near')
        rec['kinds'][near_far] += 1
        # argument bytes
        j = i + 1
        argbytes = None
        if j < len(insns):
            nx = insns[j]
            if nx.mnemonic == 'add' and nx.op_str.startswith('sp,'):
                argbytes = nx.operands[1].imm
                if argbytes % 2 or argbytes > 64:
                    argbytes = None       # not an argument pop (mis-decoded / stack adjust)
                j += 1
            elif nx.mnemonic == 'inc' and nx.op_str == 'sp' and j + 1 < len(insns) and insns[j + 1].op_str == 'sp':
                argbytes = 2
                j += 2
        # pushes before the call (backward, stop at label/branch/call)
        pushes = []
        k = i - 1
        if near_far == 'pushcs':
            k -= 1
        budget = None if argbytes is None else argbytes // 2
        while k >= 0 and (budget is None or len(pushes) < budget):
            p = insns[k]
            if p.mnemonic in ('lcall', 'call', 'ret', 'retf', 'jmp', 'ljmp') or p.mnemonic.startswith('j'):
                break
            if p.mnemonic == 'push':
                pushes.append(k)
            if p.mnemonic in ('mov',) and p.op_str == 'bp, sp':
                pushes = [x for x in pushes if insns[x].op_str not in ('bp',)]
                break
            if p.mnemonic == 'sub' and p.op_str.startswith('sp,'):
                break
            k -= 1
        if argbytes is None:
            # epilogue follows (L9) or no args: count only pushes above the prologue
            rec['argbytes']['?%d' % (2 * len(pushes))] += 1
        else:
            rec['argbytes'][argbytes] += 1
        words = [self.push_kind(insns, x) for x in pushes]
        for w, kind in enumerate(words):
            rec['argwords'][w][kind] += 1
        # return value use
        use = 'none'
        ax_live, dx_live = True, True
        for m in range(j, min(j + 8, len(insns))):
            q = insns[m]
            s = q.mnemonic + ' ' + q.op_str
            if q.mnemonic in ('retf', 'ret'):
                use = 'forwarded' if ax_live else use
                break
            zero = re.fullmatch(r'(sub|xor) (\w+), \2', s)
            rdn = {q.reg_name(r) for r in q.regs_access()[0]}
            wrn = {q.reg_name(r) for r in q.regs_access()[1]}
            if zero:
                rdn = set()
            if q.mnemonic == 'cwde' and ax_live:
                use = 'char_sx'
                break
            if re.fullmatch(r'(sub|xor) ah, ah', s) or s == 'mov ah, 0':
                use = 'char_zx' if ax_live else use
                break
            if dx_live and rdn & {'dx', 'dl', 'dh', 'edx'}:
                use = 'long'
                break
            if ax_live and rdn & {'ax', 'eax'}:
                use = 'int'
                if q.mnemonic == 'cdq':
                    use = 'int_cwd'           # (long)f(): signed int result widened
                    break
                if wrn & {'dx', 'edx'}:
                    break
                for m2 in range(m + 1, min(m + 4, len(insns))):
                    q2 = insns[m2]
                    r2 = {q2.reg_name(r) for r in q2.regs_access()[0]}
                    w2 = {q2.reg_name(r) for r in q2.regs_access()[1]}
                    if re.fullmatch(r'(sub|xor) (\w+), \2', q2.mnemonic + ' ' + q2.op_str):
                        r2 = set()
                    if r2 & {'dx', 'dl', 'dh'}:
                        use = 'long'
                        break
                    if w2 & {'dx', 'edx'}:
                        break
                break
            if ax_live and 'al' in rdn:
                use = 'char'
                break
            if wrn & {'ax', 'al', 'eax'}:
                ax_live = False
            if wrn & {'dx', 'edx', 'dl'}:
                dx_live = False
            if not ax_live and not dx_live:
                break
            if q.mnemonic.startswith('j') or q.mnemonic in ('lcall', 'call'):
                use = 'none?' if ax_live else use
                break
        rec['ret'][use] += 1

    def push_kind(self, insns, k):
        p = insns[k]
        op = p.operands[0] if p.operands else None
        if op is None:
            return 'push?'
        if op.type == X86_OP_MEM:
            m = op.mem
            if m.base and p.reg_name(m.base) == 'bp':
                return 'mem_param' if m.disp >= self.pbase else 'mem_local'
            return 'mem_global' if not m.base else 'mem_ptr'
        if op.type == X86_OP_IMM:
            return 'imm'
        r = p.reg_name(op.reg)
        if r in ('ds', 'ss', 'cs', 'es'):
            return 'seg_' + r
        # find producer of the pushed register (scan back up to 8).  A low-byte write (mov al,..) keeps
        # scanning: MSC reuses a high byte it already zeroed (sub ah,ah; push ax; mov al,..; push ax).
        lo_written = False
        lo, hi = {'ax': ('al', 'ah'), 'bx': ('bl', 'bh'), 'cx': ('cl', 'ch'), 'dx': ('dl', 'dh')}.get(r, (None, None))
        for q in range(k - 1, max(k - 9, -1), -1):
            ins = insns[q]
            s = ins.op_str
            full = ins.mnemonic + ' ' + s
            if ins.mnemonic.startswith('j') or ins.mnemonic in ('lcall', 'call'):
                if lo_written:
                    return 'char_raw'
                return 'reg_' + ('ret' if ins.mnemonic in ('lcall', 'call') and r in ('ax', 'dx') else r)
            wrn = {ins.reg_name(x) for x in ins.regs_access()[1]}
            if not ({REG16.get(n) for n in wrn} & {r}):
                continue
            if ins.mnemonic == 'cwde' and r == 'ax':
                return 'char_raw' if lo_written else 'char_sx'
            if hi and re.fullmatch(rf'(sub|xor) {hi}, {hi}|mov {hi}, 0', full):
                return 'char_zx'
            if lo and wrn <= {lo} and not lo_written:
                lo_written = True
                continue
            if lo_written:
                if re.fullmatch(rf'(sub|xor) {r}, {r}', full) or (ins.mnemonic == 'mov' and s.startswith(r + ',')
                                                                 and ins.operands[1].type == X86_OP_IMM
                                                                 and ins.operands[1].imm < 256):
                    return 'char_zx'
                return 'char_raw'
            if ins.mnemonic == 'mov' and s.startswith(r + ',') and ins.operands[1].type == X86_OP_IMM:
                return 'imm'
            if ins.mnemonic == 'lea':
                return 'lea'
            if ins.mnemonic == 'mov' and ins.operands[1].type == X86_OP_MEM:
                return 'mem_via_reg'
            return 'reg_expr'
        return 'char_raw' if lo_written else 'reg_' + r

    def transfer(self, ins, ops, regs, site):
        mn = ins.mnemonic
        s = ins.op_str
        ev = self.ev
        rd_ids, wr_ids = ins.regs_access()
        wr = {REG16.get(ins.reg_name(r)) for r in wr_ids}
        if mn == 'cwde':          # cbw
            t = regs.get('ax')
            if t and t.get('src') and t['w'] == 1:
                ev.add(t['src'], 'ext', 'sx', site(ins))
                regs['ax'] = dict(t, w=2, ext='sx')
            else:
                regs.pop('ax', None)
            return
        if mn == 'cdq':           # cwd
            t = regs.get('ax')
            if t and t.get('src') and t['w'] == 2 and not t.get('ext'):
                ev.add(t['src'], 'ext', 'cwd', site(ins))
            regs.pop('dx', None)
            return
        if re.fullmatch(r'(sub|xor) ah, ah', mn + ' ' + s) or (mn == 'mov' and s == 'ah, 0'):
            t = regs.get('ax')
            if t and t.get('src') and t['w'] == 1:
                ev.add(t['src'], 'ext', 'zx', site(ins))
                regs['ax'] = dict(t, w=2, ext='zx')
            else:
                regs.pop('ax', None)
            return
        if mn in ('idiv', 'div'):
            t = regs.get('ax')
            if t and t.get('src') and not t.get('ext'):
                ev.add(t['src'], 'sign', 'signed' if mn == 'idiv' else 'unsigned', site(ins))
            if ops and ops[0].type == X86_OP_REG:
                t2 = regs.get(REG16.get(ins.reg_name(ops[0].reg)))
                if t2 and t2.get('src') and not t2.get('ext'):
                    ev.add(t2['src'], 'sign', 'signed' if mn == 'idiv' else 'unsigned', site(ins))
            regs.pop('ax', None)
            regs.pop('dx', None)
            return
        if mn in ('sar', 'shr') and ops and ops[0].type == X86_OP_REG:
            t = regs.get(REG16.get(ins.reg_name(ops[0].reg)))
            if t and t.get('src') and not t.get('ext'):
                ev.add(t['src'], 'sign', 'signed' if mn == 'sar' else 'unsigned', site(ins))
            for r in wr:
                regs.pop(r, None)
            return
        # scale tracking for index registers
        if mn == 'shl' and ops and ops[0].type == X86_OP_REG and len(ops) == 2 and ops[1].type == X86_OP_IMM:
            r = REG16.get(ins.reg_name(ops[0].reg))
            t = regs.get(r)
            regs[r] = {'src': None, 'w': 2, 'ext': None, 'scale': ((t or {}).get('scale') or 1) * (2 ** ops[1].imm)}
            return
        if mn == 'add' and len(ops) == 2 and ops[0].type == X86_OP_REG and ops[1].type == X86_OP_REG \
                and ops[0].reg == ops[1].reg:
            r = REG16.get(ins.reg_name(ops[0].reg))
            t = regs.get(r)
            regs[r] = {'src': None, 'w': 2, 'ext': None, 'scale': ((t or {}).get('scale') or 1) * 2}
            return
        if mn == 'imul' and len(ops) == 1:
            # ax = ax * op ; if ax held a constant, the other operand is an index scaled by it
            t = regs.get('ax')
            c = t.get('const') if t else None
            other = None
            if ops[0].type == X86_OP_REG:
                other = regs.get(REG16.get(ins.reg_name(ops[0].reg)))
            if c is None and other is not None:
                c = other.get('const')
            regs['ax'] = {'src': None, 'w': 2, 'ext': None, 'scale': c if isinstance(c, int) and c > 0 else None}
            regs.pop('dx', None)
            return
        if mn in ('les', 'lds') and ops and ops[1].type == X86_OP_MEM:
            key, info = self.mem_key(ins, ops[1], regs)
            r = REG16.get(ins.reg_name(ops[0].reg))
            seg = 'es' if mn == 'les' else 'ds'
            src = key or info.get('near_deref') or info.get('far_deref')
            regs[r] = {'src': None, 'w': 2, 'ext': None}
            regs[seg] = {'src': key, 'w': 4, 'ext': None, 'farptr': True} if key else {}
            return
        if mn == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_REG:
            r = REG16.get(ins.reg_name(ops[0].reg))
            rname = ins.reg_name(ops[0].reg)
            if ops[1].type == X86_OP_MEM:
                key, info = self.mem_key(ins, ops[1], regs)
                if rname in ('ah', 'bh', 'ch', 'dh'):
                    regs.pop(r, None)
                    return
                if key:
                    regs[r] = {'src': key, 'w': ops[1].size, 'ext': None,
                               'ptr': ops[1].size == 2 and r in ('bx', 'si', 'di'), 'scale': 1}
                elif info.get('near_deref') or info.get('far_deref'):
                    regs[r] = {'src': None, 'w': ops[1].size, 'ext': None, 'scale': 1}
                else:
                    regs[r] = {'src': None, 'w': ops[1].size, 'ext': None, 'scale': 1}
                return
            if ops[1].type == X86_OP_REG:
                r2 = REG16.get(ins.reg_name(ops[1].reg))
                t = regs.get(r2)
                if rname in ('ah', 'bh', 'ch', 'dh', 'al', 'bl', 'cl', 'dl') and rname != ins.reg_name(ops[1].reg):
                    regs.pop(r, None)
                    return
                if t:
                    nt = dict(t)
                    if r in ('bx', 'si', 'di') and t.get('src') and t['w'] == 2 and not t.get('ext'):
                        nt['ptr'] = True
                    regs[r] = nt
                else:
                    regs.pop(r, None)
                return
            if ops[1].type == X86_OP_IMM:
                regs[r] = {'src': None, 'w': ops[1].size, 'ext': None, 'const': ops[1].imm, 'scale': 1}
                return
        # generic: clobber written registers
        for r in wr:
            if r:
                regs.pop(r, None)


def gather(prog):
    ev = Ev()
    scanned = 0
    for fn in prog.functions:
        FunctionScan(prog, ev, fn).run()
        scanned += 1
    return ev, scanned


# ----------------------------------------------------------------------------- inference
def ctype(width, sign, ptr=None, pointee=None):
    if ptr == 'far':
        return f'{pointee or "void"} far *'
    if ptr == 'near':
        return f'{pointee or "void"} *'
    if width == 1:
        return 'unsigned char' if sign == 'unsigned' else 'char'
    if width == 2:
        return 'unsigned int' if sign == 'unsigned' else 'int'
    if width == 4:
        return 'unsigned long' if sign == 'unsigned' else 'long'
    return 'char'


def pointee_name(c):
    ws = collections.Counter()
    for v, n in c.items():
        off, w = v.split(':')
        ws[int(w)] += n
    offs = {int(v.split(':')[0]) for v in c}
    if len(offs) > 1 or max(offs, default=0) > 0:
        return 'struct ?'
    if ws:
        w = ws.most_common(1)[0][0]
        return {1: 'char', 2: 'int', 4: 'long'}.get(w, 'char')
    return 'void'


def sign_vote(cat):
    s = cat.get('sign', collections.Counter()) + collections.Counter()
    e = cat.get('ext', collections.Counter())
    signed = s['signed'] + e['sx'] + e['cwd']
    unsigned = s['unsigned'] + e['zx']
    if signed and not unsigned:
        return 'signed', signed, unsigned
    if unsigned and not signed:
        return 'unsigned', signed, unsigned
    if signed or unsigned:
        return 'mixed', signed, unsigned
    return None, 0, 0


def infer_leaf(cat):
    """Evidence at one (symbol, offset) -> inferred leaf description."""
    widths = cat.get('width', collections.Counter())
    far = sum(cat.get('far_load', {}).values()) + sum(cat.get('far_deref', {}).values())
    near = sum(cat.get('near_deref', {}).values())
    pair = sum(cat.get('dword_pair', {}).values())
    sign, ns, nu = sign_vote(cat)
    w = None
    if widths:
        w = max(widths, key=lambda x: (widths[x], x))
    if far:
        kind, width = 'farptr', 4
    elif pair and (widths.get(2, 0) <= 2 * pair or not widths.get(1)):
        kind, width = 'dword', 4
    elif near and w == 2:
        kind, width = 'nearptr', 2
    else:
        kind, width = 'scalar', w
    pointee = pointee_name(cat.get('pointee_off', collections.Counter())) if kind in ('farptr', 'nearptr') else None
    return {'kind': kind, 'width': width, 'sign': sign, 'signed_votes': ns, 'unsigned_votes': nu,
            'widths': dict(widths), 'far': far, 'near_deref': near, 'dword_pair': pair,
            'indexed': dict(cat.get('indexed', {})), 'ctype': ctype(width, sign, {'farptr': 'far', 'nearptr': 'near'}.get(kind), pointee)}


def infer(prog, ev):
    globals_ = {}
    per_sym = collections.defaultdict(dict)
    for (a, off), cat in ev.g.items():
        per_sym[a][off] = cat
    for a, offs in sorted(per_sym.items()):
        ext, src = prog.extent[a]
        leaves = {off: infer_leaf(cat) for off, cat in sorted(offs.items())}
        origins = collections.Counter()
        for cat in offs.values():
            origins.update(cat.get('origin', {}))
        strides = collections.Counter()
        for lf in leaves.values():
            for s, n in lf['indexed'].items():
                if s not in ('None', '1'):
                    strides[s] += n
        name = prog.primary_name(a)
        base_leaf = leaves.get(0)
        shape = 'scalar'
        if len(leaves) > 1 or any(lf['indexed'] for lf in leaves.values()) or (ext and base_leaf and ext > (base_leaf['width'] or 1) and src == 'layout'):
            shape = 'aggregate'
        decl = None
        if shape == 'scalar' and base_leaf:
            decl = f"extern {base_leaf['ctype']} {name[1:]};"
        elif base_leaf or leaves:
            lf = base_leaf or next(iter(leaves.values()))
            ws = {x['width'] for x in leaves.values() if x['width']}
            if len(ws) == 1 and lf['width']:
                n_el = (ext // lf['width']) if ext else None
                ct = lf['ctype']
                decl = f"extern {ct} {name[1:]}[{n_el if n_el else ''}];"
            else:
                decl = f"extern struct {{ /* {len(leaves)} leaves */ }} {name[1:]}; /* {ext} bytes */"
        globals_[name] = {
            'address': a, 'ds_offset': a - prog.frame, 'names': prog.sym_names.get(a, []),
            'extent': ext, 'extent_source': src, 'ida_directive': prog.ida_globals.get(name[1:]),
            'shape': shape, 'strides': dict(strides), 'leaves': {str(k): v for k, v in leaves.items()},
            'origins': dict(origins), 'functions': sorted(ev.gfuncs.get(a, ())),
            'n_functions': len(ev.gfuncs.get(a, ())), 'decl': decl}
    # functions
    functions = {}
    params = collections.defaultdict(dict)
    for (fs, off), cat in ev.p.items():
        params[fs][off] = cat
    for f in prog.functions:
        fs = f['start']
        rec = ev.calls.get(fs)
        pr = params.get(fs, {})
        plist = []
        for off, cat in sorted(pr.items()):
            lf = infer_leaf(cat)
            plist.append(dict(lf, offset=off))
        # merge param leaves: dword/far at off consumes off+2
        merged, skip = [], set()
        for p in plist:
            if p['offset'] in skip:
                continue
            if p['kind'] in ('farptr', 'dword') or (p['width'] == 4):
                skip.add(p['offset'] + 2)
            merged.append(p)
        maxoff = max([p['offset'] + max(p['width'] or 2, 2) for p in merged] or [0])
        argbytes = dict(rec['argbytes']) if rec else {}
        ret = dict(rec['ret']) if rec else {}
        argwords = {str(k): dict(v) for k, v in rec['argwords'].items()} if rec else {}
        known = [int(k) for k in argbytes if isinstance(k, int)]
        callsite_bytes = collections.Counter({k: v for k, v in argbytes.items() if isinstance(k, int)})
        # return
        use = collections.Counter(ret)
        use['int'] += use.pop('int_cwd', 0)
        rlo = ev.r.get((fs, 0), {})
        rhi = ev.r.get((fs, 2), {})
        rtype = None
        if use:
            if use['long'] and (rhi.get('far_deref') or rlo.get('far_load')):
                rtype = 'far*'
            elif use['long']:
                rtype = 'long/far*'
            elif rlo.get('near_deref') and not use['char_sx'] and not use['char_zx']:
                rtype = 'near*'
            elif use['char_sx'] and not use['int'] and not use['char_zx']:
                rtype = 'char'
            elif use['char_zx'] and not use['int'] and not use['char_sx']:
                rtype = 'unsigned char'
            elif use['char'] and not use['int']:
                rtype = 'char?'
            elif use['int'] or use['forwarded']:
                rtype = 'int'
            elif use['char_sx'] or use['char_zx'] or use['char']:
                rtype = 'char/int'
            else:
                rtype = 'void?'
        # prototype string
        ps = []
        pos = 0
        pmap = {p['offset']: p for p in merged}
        total = max([maxoff] + known) if (known or merged) else 0
        char_words = set()
        for w, kinds in argwords.items():
            if kinds.get('char_raw'):
                char_words.add(int(w) * 2)
        while pos < total:
            p = pmap.get(pos)
            if p:
                ct = p['ctype']
                if p['kind'] == 'scalar' and p['width'] == 1:
                    ct = 'unsigned char' if p['sign'] == 'unsigned' else 'char'
                elif p['kind'] == 'scalar' and pos in char_words:
                    ct = 'char'
                ps.append(ct)
                pos += 4 if p['kind'] in ('farptr', 'dword') or p['width'] == 4 else 2
            else:
                ps.append('char' if pos in char_words else 'int /*unused*/')
                pos += 2
        rt_c = {'far*': 'void far *', 'near*': 'void *', 'long/far*': 'long', 'char?': 'char', 'char/int': 'int',
                'void?': 'void', None: 'void'}.get(rtype, rtype)
        proto = f"{rt_c} {'far' if f.get('distance') != 'near' else 'near'} {f['name']}({', '.join(ps) or 'void'});"
        own = prog.owner(fs)
        functions[f['name']] = {
            'address': fs, 'end': f['end'], 'distance': f.get('distance'), 'owner': own[3] if own else None,
            'owner_kind': own[2] if own else None,
            'callee_params': merged, 'callee_param_bytes': maxoff,
            'callsite_argbytes': {str(k): v for k, v in argbytes.items()},
            'callsite_argwords': argwords, 'return_use': ret, 'return_type': rtype,
            'callers': sorted(rec['callers']) if rec else [], 'call_kinds': dict(rec['kinds']) if rec else {},
            'prototype': proto}
    return globals_, functions


def write_header(prog, globals_, functions, path):
    lines = ['/* decls_draft.h -- GENERATED by typeinfer.py from the original executable (diagnostic draft).',
             ' * One declaration per global/function inferred from machine evidence; names are the',
             ' * primary layout/data-symbols.json aliases.  Types are evidence summaries, not recovered',
             ' * source: check conflicts.json before using any line. */', '']
    for name, g in sorted(globals_.items(), key=lambda kv: kv[1]['address']):
        if g['decl']:
            ev = []
            lf = g['leaves'].get('0')
            if lf:
                ev.append(f"w{lf['widths']}")
                if lf['sign']:
                    ev.append(f"{lf['sign']} {lf['signed_votes']}/{lf['unsigned_votes']}")
            if g['strides']:
                ev.append(f"strides {g['strides']}")
            lines.append(f"{g['decl']:60} /* DG 0x{g['ds_offset']:04X} {' '.join(ev)} */")
    lines.append('')
    for name, f in sorted(functions.items(), key=lambda kv: kv[1]['address']):
        if f['owner_kind'] == 'KNOWN_TOOLCHAIN_LIBRARY':
            continue
        lines.append(f"extern {f['prototype']:70} /* 0x{f['address']:05X} args {f['callsite_argbytes']} ret {f['return_use']} */")
    Path(path).write_text('\n'.join(lines) + '\n')


# ----------------------------------------------------------------------------- declared types / conflicts
def tu_sources():
    from common import read_json
    man = read_json(ROOT / 'layout/manifest.json')
    out = []
    for o in man['owners']:
        if o['kind'] == 'MATCHING_C' and o.get('recipe'):
            r = read_json(ROOT / o['recipe'])
            if r.get('source') and (ROOT / r['source']).exists():
                out.append({'id': o['id'], 'source': r['source'], 'accepted': True,
                            'start': o.get('start'), 'end': o.get('end')})
    cands = [
        ('cand:s003d_seg003_abbrev', 'build/workers/s003d/seg003_abbrev.c', 40724, 57760),
        ('cand:s003d_seg003_semantic', 'build/workers/s003d/seg003_semantic.c', 40724, 57760),
        ('cand:s005c_obj_seg005', 'build/workers/s005c/obj_seg005.c', None, None),
        ('cand:s008c_seg008', 'build/workers/s008c/seg008.c', None, None),
        ('cand:s027c2_obj_seg027', 'build/workers/s027c2/obj_seg027.c', None, None),
        ('cand:s000c_seg000', 'build/workers/s000c/seg000_s000c.c', 0, 18194),
        ('cand:F-uf_scene_exact', 'build/workers/F-uf/semantic_scene_exact.c', 40724, 57760),
        ('ref:restunts_externs', 'build/references/restunts/src/restunts/c/externs.h', None, None),
    ]
    for cid, path, s, e in cands:
        if (ROOT / path).exists():
            seg = {'s003d': 'seg003', 'F-uf': 'seg003', 's005c': 'seg005', 's008c': 'seg008', 's027c2': 'seg027',
                   's000c': 'seg000'}.get(path.split('/')[2]) if path.startswith('build/workers/') else None
            out.append({'id': cid, 'source': path, 'accepted': False, 'start': s, 'end': e, 'segment': seg})
    return out


def norm(t):
    """Normalized scalar signature for cross-TU comparison."""
    import cdecls
    if t is None:
        return '?'
    k = t['k']
    if k == 'base':
        n = {'short': 'int'}.get(t['name'], t['name'])
        return ('u' if t.get('unsigned') else '') + n
    if k == 'ptr':
        return f"{norm(t['to']) if t['to']['k'] != 'fn' else 'fn'}*{t['dist']}"
    if k == 'arr':
        return f"{norm(t['of'])}[]"
    if k == 'struct':
        return f"struct {t['tag']}"
    if k == 'fn':
        ps = 'unproto' if t['params'] is None else ','.join(param_norm(p) for p in t['params'])
        return f"{norm(t['ret'])} {t['dist']}({ps}{',...' if t.get('varargs') else ''})"
    return '?'


def param_norm(p):
    if p is None:
        return '?'
    if p['k'] == 'ptr':
        return 'ptr' + p['dist']
    return norm(p)


def param_bytes(unit, t):
    if t['params'] is None:
        return None
    total = 0
    for p in t['params']:
        if p is None:
            return None
        s = unit.size(p)
        if s is None:
            return None
        total += max(2, (s + 1) // 2 * 2)
    return total


def compare(prog, globals_, functions, ev):
    import cdecls
    by_addr_g = {g['address']: g for g in globals_.values()}
    fn_by_addr = {f['address']: (n, f) for n, f in functions.items()}
    conflicts = []
    cross = collections.defaultdict(lambda: collections.defaultdict(list))
    tus = tu_sources()
    for tu in tus:
        text = (ROOT / tu['source']).read_text(encoding='latin-1')
        try:
            unit = cdecls.Unit(text, tu['source'])
        except Exception as e:  # pragma: no cover
            conflicts.append({'tu': tu['id'], 'kind': 'parse_error', 'detail': str(e)})
            continue
        tu_fns = set()
        for f in prog.functions:
            if (tu.get('start') is not None and tu['start'] <= f['start'] < tu['end']) or                     (tu.get('segment') and f.get('segment') == tu['segment']):
                tu_fns.add(f['name'])
        for name, decls in unit.decls.items():
            d = decls[0]
            t = d['type']
            if t['k'] == 'fn':
                addr = prog.code_name_to_addr.get(name)
                if addr is None:
                    continue
                cross[('fn', addr)][norm(t)].append(f"{tu['id']}:{d['line']}")
                fr = fn_by_addr.get(addr)
                if not fr:
                    continue
                fname, f = fr
                conflicts.extend(fn_conflicts(unit, tu, name, t, d, f, fname, tu_fns))
                continue
            addr = prog.name_to_addr.get(name)
            if addr is None:
                continue
            cross[('g', addr)][norm(t)].append(f"{tu['id']}:{d['line']}{'(def)' if d['defined'] else ''}")
            g = by_addr_g.get(addr)
            if not g:
                continue
            conflicts.extend(data_conflicts(unit, tu, name, t, d, g, ev, tu_fns))
    # cross-TU: classify each pair of distinct declarations
    for (kind, addr), types in cross.items():
        real = dict(types)
        if len(real) <= 1:
            continue
        keys = sorted(real)
        worst = 'info'
        order = {'info': 0, 'sign': 1, 'proto': 1, 'pointee': 1, 'split': 2, 'shape': 2, 'abi': 3}
        reasons = set()
        for i in range(len(keys)):
            for j in range(i + 1, len(keys)):
                r = cross_class(keys[i], keys[j], kind)
                reasons.add(r)
                if order[r] > order[worst]:
                    worst = r
        acc = {k for k, v in real.items() if any(not x.startswith(('cand:', 'ref:')) for x in v)}
        acc_worst = 'info'
        ak = sorted(acc)
        for i in range(len(ak)):
            for j in range(i + 1, len(ak)):
                r = cross_class(ak[i], ak[j], kind)
                if order[r] > order[acc_worst]:
                    acc_worst = r
        name = prog.primary_name(addr) if kind == 'g' else fn_by_addr.get(addr, (f'sub_{addr:X}',))[0]
        sev = {'abi': 'high', 'shape': 'high', 'split': 'medium', 'sign': 'medium', 'pointee': 'low',
               'proto': 'low', 'info': 'info'}[worst]
        if kind == 'g':
            g = by_addr_g.get(addr)
            lf = (g or {}).get('leaves', {}).get('0') or {}
            evs = {'inferred': lf.get('ctype'), 'widths': lf.get('widths'), 'sign': lf.get('sign'),
                   'shape': (g or {}).get('shape'), 'extent': (g or {}).get('extent')}
        else:
            f = fn_by_addr.get(addr, (None, {}))[1]
            evs = {'prototype': f.get('prototype'), 'argbytes': f.get('callsite_argbytes'),
                   'return_use': f.get('return_use'), 'callee_param_bytes': f.get('callee_param_bytes')}
        conflicts.append({'kind': 'cross_tu', 'what': kind, 'name': name, 'address': addr, 'evidence': evs,
                          'types': {k: v[:8] for k, v in real.items()}, 'class': worst,
                          'class_among_accepted': acc_worst if len(acc) > 1 else None,
                          'among_accepted': len(acc) > 1 and acc_worst not in ('info',),
                          'severity': sev})
    return conflicts, tus



def _split_fn(sig):
    m = re.match(r'^(.*) (far|near)\((.*)\)$', sig)
    if not m:
        raise ValueError(sig)
    return m.group(1), m.group(2), m.group(3)


def _ptr(t):
    m = re.search(r'\*(far|near)(\[\])?$', t)
    return m.group(1) + (m.group(2) or '') if m else None


def cross_class(x, y, kind):
    """abi: different width/argument bytes; split: same bytes, different parameter split;
    shape: array vs scalar / struct vs scalar; pointee: pointers of the same distance to different
    types; sign: only signedness/char-vs-int of same-ABI items; proto: prototyped vs unprototyped,
    void vs value return; info: equivalent spellings."""
    unsign = lambda t: re.sub(r'\bu(char|int|long)', r'\1', t)
    if kind == 'g':
        if unsign(x) == unsign(y):
            return 'sign'
        px, py = _ptr(x), _ptr(y)
        if px and px == py:
            return 'pointee'
        if x.endswith('[]') != y.endswith('[]'):
            return 'shape'
        if x.startswith('struct') and y.startswith('struct'):
            return 'pointee' if x.lower() == y.lower() else 'shape'
        if x.startswith('struct') or y.startswith('struct'):
            return 'shape'
        return 'abi'
    try:
        rx, dx, px = _split_fn(x)
        ry, dy, py = _split_fn(y)
    except ValueError:
        return 'abi'
    w = lambda t: {'char': 2, 'uchar': 2, 'int': 2, 'uint': 2, 'long': 4, 'ulong': 4, 'void': 0}.get(
        t, 4 if t.endswith('far') else 2 if t.endswith('near') else t)
    rank = {'info': 0, 'sign': 1, 'proto': 1, 'pointee': 1, 'split': 2, 'shape': 2, 'abi': 3}
    bad = 'info'
    def worse(a, b):
        return a if rank[a] >= rank[b] else b
    if dx != dy:
        return 'abi'
    if rx != ry:
        if 'void' in (rx, ry):
            bad = 'proto'
        elif w(rx) != w(ry):
            return 'abi'
        elif unsign(rx) == unsign(ry) or ({rx, ry} <= {'char', 'uchar', 'int', 'uint'}):
            bad = 'sign'
        else:
            bad = 'pointee'
    if 'unproto' in (px, py):
        return worse(bad, 'proto')
    pxs = [q for q in px.split(',') if q and q != '...']
    pys = [q for q in py.split(',') if q and q != '...']
    size = lambda q: 4 if q in ('ptrfar', 'long', 'ulong') else 2
    if sum(map(size, pxs)) != sum(map(size, pys)) and not ('...' in px or '...' in py):
        return 'abi'
    if [size(q) for q in pxs] != [size(q) for q in pys]:
        return worse(bad, 'split')
    if pxs != pys:
        return worse(bad, 'sign')
    return bad


def data_conflicts(unit, tu, name, t, d, g, ev, tu_fns):
    import cdecls
    out = []
    size = unit.size(t)
    in_tu = bool(set(g['functions']) & tu_fns) if tu_fns else None
    base = {'tu': tu['id'], 'accepted': tu['accepted'], 'name': name, 'address': g['address'],
            'declared': cdecls.describe(t), 'line': d['line'], 'evidence_in_tu': in_tu}
    sites = lambda off, cat, val: ev.examples.get(('g', g['address'], off, cat, str(val)), [])
    for off_s, lf in g['leaves'].items():
        off = int(off_s)
        indexed_only = lf['indexed'] and sum(lf['widths'].values()) == sum(lf['indexed'].values())
        leaf, path = unit.leaf_at(t, off)
        why = path
        if leaf is None:
            if why in ('beyond array',):
                out.append(dict(base, kind='extent', offset=off, detail=f'access at +{off} beyond declared size {size}',
                                severity='high' if not indexed_only else 'medium'))
            elif why == 'inside scalar':
                outer, _ = unit.leaf_at(t, off - 2) if off >= 2 else (None, None)
                osk = cdecls.scalar_kind(unit, outer) if outer else None
                if osk and osk['width'] == 4 and set(lf['widths']) <= {2}:
                    continue            # high word of a long / segment of a far pointer (pushes, FP_SEG)
                hb, _ = unit.leaf_at(t, off - 1) if off >= 1 else (None, None)
                hsk = cdecls.scalar_kind(unit, hb) if hb else None
                sev = 'low' if hsk and hsk['width'] == 2 and set(lf['widths']) <= {1} else 'medium'
                out.append(dict(base, kind='inside_scalar', offset=off,
                                detail=f'access at +{off} inside declared scalar ({lf["widths"]})', severity=sev))
            continue
        sk = cdecls.scalar_kind(unit, leaf)
        if not sk:
            continue
        ws = lf['widths']
        # widths
        if lf['kind'] == 'farptr' and sk['ptr'] != 'far' and sk['width'] != 4:
            out.append(dict(base, kind='pointer', offset=off, detail=f"les/lds far load; declared {cdecls.describe(leaf)}",
                            severity='high'))
        elif lf['kind'] == 'farptr' and sk['ptr'] is None and sk['width'] == 4:
            out.append(dict(base, kind='pointer', offset=off, detail=f"les/lds far load of a declared long",
                            severity='low'))
        elif lf['near_deref'] and sk['ptr'] is None and sk['width'] == 2:
            out.append(dict(base, kind='pointer', offset=off,
                            detail=f"dereferenced as near pointer ({lf['near_deref']}x); declared {cdecls.describe(leaf)}",
                            severity='medium'))
        elif lf['near_deref'] and sk['ptr'] == 'far':
            out.append(dict(base, kind='pointer', offset=off,
                            detail=f"near deref of declared far pointer", severity='high'))
        w = sk['width']
        if 1 in ws and w >= 2 and not (lf['kind'] in ('farptr',)):
            out.append(dict(base, kind='width', offset=off,
                            detail=f"byte access x{ws[1]} at +{off} to declared {cdecls.describe(leaf)} (w{w})", sites=sites(off, 'width', 1),
                            severity='low' if ws.get(2) else 'medium'))
        if 2 in ws and w == 1:
            out.append(dict(base, kind='width', offset=off,
                            detail=f"word access x{ws[2]} at +{off} to declared {cdecls.describe(leaf)} {''.join(path)}", sites=sites(off, 'width', 2),
                            severity='high'))
        if lf['dword_pair'] and w == 2 and sk['ptr'] is None:
            out.append(dict(base, kind='width', offset=off,
                            detail=f"dword pair (long/far) access x{lf['dword_pair']} to declared {cdecls.describe(leaf)}",
                            severity='medium'))
        # signedness
        if sk['signed'] is not None and lf['sign'] in ('signed', 'unsigned'):
            ev_signed = lf['sign'] == 'signed'
            if ev_signed != sk['signed']:
                out.append(dict(base, kind='sign', offset=off,
                                detail=f"evidence {lf['sign']} ({lf['signed_votes']}s/{lf['unsigned_votes']}u); declared {cdecls.describe(leaf)}",
                                severity='medium' if w == 1 else 'low'))
    return out


def fn_conflicts(unit, tu, name, t, d, f, fname, tu_fns=None):
    import cdecls
    out = []
    in_tu = bool(set(f['callers']) & tu_fns) if tu_fns else None
    base = {'tu': tu['id'], 'accepted': tu['accepted'], 'name': name, 'address': f['address'],
            'declared': cdecls.describe(t), 'line': d['line'], 'what': 'function', 'callers_in_tu': in_tu,
            'defined_in_tu': bool(tu_fns and fname in tu_fns)}
    pb = param_bytes(unit, t)
    known = {int(k): v for k, v in f['callsite_argbytes'].items() if not k.startswith('?')}
    if pb is not None and not t.get('varargs') and known:
        bad = {k: v for k, v in known.items() if k != pb}
        if bad:
            out.append(dict(base, kind='argbytes', detail=f"declared {pb} arg bytes; call sites {known}",
                            severity='high' if not any(k == pb for k in known) else 'medium'))
    if pb is not None and f['callee_param_bytes'] > pb and not t.get('varargs'):
        out.append(dict(base, kind='argbytes', detail=f"callee reads params up to {f['callee_param_bytes']} bytes; declared {pb}",
                        severity='high'))
    # char parameters (prototype char pushed as raw AL)
    if t['params']:
        pos = 0
        for p in t['params']:
            if p is None:
                break
            s = unit.size(p) or 2
            w = f['callsite_argwords'].get(str(pos // 2), {})
            sk = cdecls.scalar_kind(unit, p)
            # Calibrated (cal/charparm*.c): MSC 5.10 pushes char arguments int-extended even under a
            # char prototype, so an extended push says nothing.  A raw AL push (mov al,..; push ax without
            # extension) leaves AH undefined: only a callee reading the parameter as a byte is consistent.
            cp0 = next((q for q in f['callee_params'] if q['offset'] == pos), None)
            if w.get('char_raw') and sk and sk['width'] != 1 and cp0 and cp0['widths'].get(2):
                out.append(dict(base, kind='param_char', detail=f"param at +{pos}: callers push a raw byte (AH undefined) x{w['char_raw']} but callee reads a word; declared {cdecls.describe(p)}",
                                severity='high'))
            elif w.get('char_raw') and sk and sk['width'] != 1:
                out.append(dict(base, kind='param_char', detail=f"param at +{pos}: callers push a raw byte x{w['char_raw']}; declared {cdecls.describe(p)} (char/unsigned char fits)",
                                severity='low'))
            # callee side
            cp = next((q for q in f['callee_params'] if q['offset'] == pos), None)
            if cp and sk:
                if cp['kind'] == 'farptr' and sk['ptr'] != 'far':
                    out.append(dict(base, kind='param_ptr', detail=f"param +{pos} loaded with les/lds; declared {cdecls.describe(p)}", severity='high'))
                if cp['kind'] == 'nearptr' and sk['ptr'] is None:
                    out.append(dict(base, kind='param_ptr', detail=f"param +{pos} dereferenced as near pointer; declared {cdecls.describe(p)}", severity='medium'))
                if cp['widths'].get(1) and sk['width'] == 2 and sk['ptr'] is None:
                    out.append(dict(base, kind='param_width', detail=f"param +{pos} read as byte; declared {cdecls.describe(p)}", severity='low'))
                if sk['signed'] is not None and cp['sign'] in ('signed', 'unsigned') and (cp['sign'] == 'signed') != sk['signed']:
                    out.append(dict(base, kind='param_sign', detail=f"param +{pos} evidence {cp['sign']} ({cp['signed_votes']}s/{cp['unsigned_votes']}u); declared {cdecls.describe(p)}",
                                    severity='low'))
            pos += max(2, (s + 1) // 2 * 2)
    # return
    ru = collections.Counter(f['return_use'])
    rk = cdecls.scalar_kind(unit, t['ret']) if t['ret']['k'] != 'struct' else None
    is_void = t['ret']['k'] == 'base' and t['ret']['name'] == 'void'
    used = ru['int'] + ru['long'] + ru['char_sx'] + ru['char_zx'] + ru['char']
    if is_void and used:
        out.append(dict(base, kind='return', detail=f"declared void; callers use the result {dict(ru)}", severity='high'))
    if rk and rk['width'] == 2 and ru['long']:
        out.append(dict(base, kind='return', detail=f"declared {cdecls.describe(t['ret'])}; callers use DX:AX {dict(ru)}", severity='high'))
    if rk and rk['width'] == 2 and rk['ptr'] is None and ru['char_sx'] and not ru['int']:
        out.append(dict(base, kind='return', detail=f"declared {cdecls.describe(t['ret'])}; callers cbw the result (char return) {dict(ru)}", severity='medium'))
    if rk and rk['width'] == 1 and ru['int'] > (ru['char_sx'] + ru['char_zx'] + ru['char']):
        out.append(dict(base, kind='return', detail=f"declared {cdecls.describe(t['ret'])}; callers use AX as int {dict(ru)}", severity='low'))
    if rk and rk['width'] == 4 and used and not ru['long'] and not ru['forwarded']:
        out.append(dict(base, kind='return', detail=f"declared {cdecls.describe(t['ret'])}; callers only use AX {dict(ru)}", severity='low'))
    return out


def jsonable(x):
    if isinstance(x, dict):
        return {str(k): jsonable(v) for k, v in x.items()}
    if isinstance(x, (list, tuple, set)):
        return [jsonable(v) for v in x]
    return x


def summarize(conflicts):
    c = collections.Counter()
    for x in conflicts:
        c[(x['kind'], x.get('severity'), 'accepted' if x.get('accepted') or x.get('among_accepted') else 'other')] += 1
    return {' / '.join(map(str, k)): v for k, v in sorted(c.items())}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--show', help='print evidence for a global or function name')
    ap.add_argument('--no-compare', action='store_true')
    args = ap.parse_args()
    t0 = time.time()
    prog = Program()
    t1 = time.time()
    ev, scanned = gather(prog)
    t2 = time.time()
    globals_, functions = infer(prog, ev)
    t3 = time.time()
    if args.show:
        key = args.show.lstrip('_')
        for n, g in globals_.items():
            if key in [x.lstrip('_') for x in g['names']]:
                print(json.dumps(jsonable(g), indent=1))
        if key in functions:
            print(json.dumps(jsonable(functions[key]), indent=1))
        return
    evidence = {'globals': {f'{a}:{o}': {c: dict(v) for c, v in cat.items()} for (a, o), cat in ev.g.items()},
                'examples': {'|'.join(map(str, k)): v for k, v in list(ev.examples.items())[:20000]}}
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'evidence.json').write_text(json.dumps(jsonable(evidence)))
    decls = {'schema': 'typenames-decls-v1', 'authority': 'DIAGNOSTIC (machine evidence summary; not recovered source)',
             'globals': globals_, 'functions': functions}
    (OUT / 'decls.json').write_text(json.dumps(jsonable(decls), indent=1))
    write_header(prog, globals_, functions, OUT / 'decls_draft.h')
    t4 = time.time()
    conflicts, tus = ([], []) if args.no_compare else compare(prog, globals_, functions, ev)
    t5 = time.time()
    # link to known residuals
    for c in conflicts:
        g = globals_.get('_' + c['name']) if c.get('what') != 'function' else None
        users = set()
        if c.get('what') == 'function' or c['kind'] == 'cross_tu' and c.get('what') == 'fn':
            users = {c['name']}
        else:
            a = c.get('address')
            gg = next((x for x in globals_.values() if x['address'] == a), None)
            users = set(gg['functions']) if gg else set()
        hits = sorted(u for u in users if u in KNOWN_RESIDUALS)
        if hits:
            c['known_residual_functions'] = hits
    out = {'schema': 'typenames-conflicts-v1', 'tus': tus, 'summary': summarize(conflicts),
           'runtime_seconds': {'load': round(t1 - t0, 2), 'scan': round(t2 - t1, 2), 'infer': round(t3 - t2, 2),
                               'write': round(t4 - t3, 2), 'compare': round(t5 - t4, 2)},
           'conflicts': conflicts}
    (OUT / 'conflicts.json').write_text(json.dumps(jsonable(out), indent=1))
    log(f'functions scanned {scanned}; globals with evidence {len(globals_)}; conflicts {len(conflicts)}')
    log(json.dumps(out['summary'], indent=1))
    log('runtime', out['runtime_seconds'])


if __name__ == '__main__':
    main()
