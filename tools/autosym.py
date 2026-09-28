"""Automated symbol closure over references in currently raw code (autosym-v1).

Every reference is decoded from the ORIGINAL oracle image and its ordered MZ
relocation table.  No candidate source, object, recipe or search report is an
input: the derivation reads only the immutable oracle, the checked function
inventory (with reviewed overlays), the manifest's ownership partition, the
existing reviewed symbol layouts, and the pinned Restunts reference listings.

For each reference the existing evidence rules decide whether its destination
is independently provable:

* far code (9A CALL / EA JMP, or MOV AX,off / MOV DX,seg pair): an original
  relocated segment word inside an instruction-verified inventory caller, the
  offset plus paragraph landing exactly on a verified inventory entry whose
  complete extent has a complete raw / exact / runtime owner (the
  `code_symbols` rules, reused through its own predicates);
* DGROUP data: an unrelocated 16-bit DS displacement inside a hash-checked,
  instruction-verified procedure that provably keeps DS = DGROUP (compact
  alias), or an offset immediate tied to a pinned `offset LABEL` use line in
  the same procedure with the label placed at that DGROUP offset (reviewed
  alias form); widths only from pinned label spans placed at the alias
  address and not overlapping another object; addends only inside that span.

Proposed names are neutral binding aliases derived from stable ids or DGROUP
offsets.  They are not claims about original PUBDEF names, source names or TUs.
Where the existing resolver rule itself requires the pinned label spelling
(reviewed immediate-anchored aliases), that spelling is used and flagged.
"""
import argparse
import bisect
import hashlib
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path


def _find_root(start):
    for parent in [start, *start.parents]:
        if (parent / 'tools' / 'common.py').is_file() and (parent / 'layout').is_dir():
            return parent
    raise SystemExit('Repository root not found')


REPO = _find_root(Path(__file__).resolve().parent)
for location in (REPO / 'tools', REPO / 'build' / 'python'):
    if str(location) not in sys.path:
        sys.path.insert(0, str(location))

from common import ROOT, identity, read_json, require, sha  # noqa: E402

TOOL_VERSION = 'autosym-v1'
# Every non-reference input of the derivation (pinned reference files are
# identity-checked against layout/references.json when read).
INPUT_FILES = ('layout/oracle.lock.json', 'layout/manifest.json', 'layout/code-symbols.json',
               'layout/data-symbols.json', 'layout/function-evidence.json',
               'layout/references.json', 'evidence/functions.json')
DSEG_PATH = 'src/restunts/asmorig/dseg.asm'
SEGMENT_PREFIXES = {0x26: 'es', 0x2e: 'cs', 0x36: 'ss', 0x3e: 'ds'}
# Reviewed width refusals (integ25 audit of sparse label-span widths).  The
# label span is an IDA reference hypothesis; these spans are not supported by
# the original accesses and are therefore not granted.
WIDTH_REFUSED = {
    '_byte_428D6': 'only indexed accesses byte_428D6[si] with si in 16..23 (seg027 flag6 loops); '
                   'the sole +24 reference is `lea di,[bx+717Eh]` (sub_3945A), the base of a '
                   'separate copy destination, not an element of this array; the 284-byte '
                   'IDA db-run span is unjustified',
}
OTHER_PREFIXES = {0xf0, 0xf2, 0xf3}
CODE_TARGET_STATUSES = ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                        'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')


# ---------------------------------------------------------------- context
def load_context():
    """Immutable facts only; the tool has no candidate input."""
    from oracle import verify
    from mz import MZ
    from function_evidence import current_inventory
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    relocations = oracle[2]['unpacked_mz']['relocations']
    lock = read_json(ROOT / 'layout/oracle.lock.json')
    require(lock['unpacked_mz']['relocations'] == relocations, 'Relocation table differs from lock')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    manifest = read_json(ROOT / 'layout/manifest.json')
    require(manifest['oracle_sha256'] == sha(image), 'Manifest belongs to another oracle')
    data_layout = read_json(ROOT / 'layout/data-symbols.json')
    code_layout = read_json(ROOT / 'layout/code-symbols.json')
    references = read_json(ROOT / 'layout/references.json')['restunts']['evidence_files']
    dseg_source = ROOT / 'build/references/restunts' / DSEG_PATH
    require(identity(dseg_source.read_bytes()) == references[DSEG_PATH],
            'Pinned dseg.asm differs')
    dseg_lines = dseg_source.read_text(encoding='latin1').splitlines()
    from data_symbols import _reference_label_offsets, checked_dgroup_layout
    checked_dgroup_layout(image, relocations)
    placed = _reference_label_offsets(dseg_lines)
    ctx = {
        'image': image, 'relocations': relocations, 'decoder': decoder,
        'capstone': capstone,
        'reloc_index': {r['load_offset']: i for i, r in enumerate(relocations)},
        'inventory': current_inventory(image)['functions'],
        'base_inventory': read_json(ROOT / 'evidence/functions.json')['functions'],
        'owners': manifest['owners'],
        'data_layout': data_layout, 'code_layout': code_layout,
        'frame': data_layout['frame_load_address'],
        'bss_start': data_layout['bss_start'], 'bss_end': data_layout['bss_end'],
        'pinned_files': references, 'dseg_lines': dseg_lines, 'placed': placed,
        'source_cache': {},
    }
    ctx['raw'] = sorted((o['start'], o['end'], o['id']) for o in ctx['owners']
                        if o['kind'] == 'UNRESOLVED_RAW')
    # Pinned dseg labels ordered by DGROUP offset (label -> (offset, line)).
    by_offset = defaultdict(list)
    for label, (offset, line) in placed.items():
        by_offset[offset].append((line, label))
    ctx['label_offsets'] = sorted(by_offset)
    ctx['labels_at'] = {o: sorted(v) for o, v in by_offset.items()}
    ctx['dseg_end_offset'] = _dseg_total_size(dseg_lines)
    ctx['code_frames'] = sorted({f['segment_paragraph'] * 16 for f in ctx['inventory']
                                 if isinstance(f.get('segment_paragraph'), int)})
    return ctx


def _dseg_total_size(lines):
    sizes = {'db': 1, 'dw': 2, 'dd': 4, 'dq': 8}
    total, inside = 0, False
    for line in lines:
        text = line.split(';')[0].strip()
        if not inside:
            inside = text.lower().startswith('dseg segment')
            continue
        if text.lower().startswith('dseg ends'):
            return total
        item = re.fullmatch(r'(?:[A-Za-z_$?@][\w$?@]*\s+)?(db|dw|dd|dq)\s+.*', text, re.I)
        if item:
            total += sizes[item.group(1).lower()]
    raise ValueError('dseg end missing')


def in_raw(ctx, start, end=None):
    end = start + 1 if end is None else end
    for s, e, owner in ctx['raw']:
        if s <= start and end <= e:
            return owner
    return None


def owner_at(ctx, address):
    for o in ctx['owners']:
        if o['kind'] in ('MATCHING_C_DATA', 'MATCHING_ASM_DATA', 'KNOWN_TOOLCHAIN_LIBRARY_DATA'):
            continue
        if o['start'] <= address < o['end']:
            return o
    return None


# --------------------------------------------------------------- decoding
def _prefix_split(raw):
    at, segment = 0, None
    while at < len(raw) and (raw[at] in SEGMENT_PREFIXES or raw[at] in OTHER_PREFIXES):
        if raw[at] in SEGMENT_PREFIXES:
            segment = SEGMENT_PREFIXES[raw[at]]
        at += 1
    return at, segment


def writes_ds(raw):
    at, _ = _prefix_split(raw)
    if at >= len(raw):
        return False
    op = raw[at]
    return (op == 0x1f or op == 0xc5 or
            (op == 0x8e and at + 1 < len(raw) and ((raw[at + 1] >> 3) & 7) == 3))


def decodable(f, image):
    if f.get('status') not in CODE_TARGET_STATUSES:
        return False
    if not isinstance(f.get('start'), int) or not isinstance(f.get('end'), int):
        return False
    if f['status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and not (
            f.get('start_evidence') and f.get('end_evidence')):
        return False
    return sha(image[f['start']:f['end']]) == f['sha256']


def decode_function(ctx, f):
    """Instructions of one verified procedure; overlays use reviewed rows."""
    image, decoder = ctx['image'], ctx['decoder']
    rows = f.get('disassembly')
    out = []
    if rows:
        for row in rows:
            at = row['load_offset']
            raw = bytes.fromhex(row['bytes'])
            ins = list(decoder.disasm(image[at:at + len(raw)], at, 1))
            if len(ins) == 1 and ins[0].size == len(raw):
                out.append(ins[0])
        return out, True
    end = f['end']
    at = f['start']
    for ins in decoder.disasm(image[f['start']:end], f['start']):
        out.append(ins)
        at = ins.address + ins.size
    return out, at == end


def is_instruction_verified(f):
    return f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'


# ----------------------------------------------------------- enumeration
_TRACKED = ('ax', 'bx', 'cx', 'dx', 'si', 'di')


def ds_states(ctx, f, instructions, trace=None):
    """Per-instruction DS value ('DG' / 'X' / None=unreached) by direct-flow dataflow.

    Entry DS = DGROUP is the medium-model far-call ABI (the same premise the
    existing compact resolver applies to every procedure).  DS changes are
    tracked through: MOV DS,r16 from a register loaded by an original
    relocated MOV r16,imm16 whose paragraph is the checked DGROUP frame;
    PUSH/POP through a small abstract stack (ADD/SUB SP, MOV BP,SP / MOV SP,BP,
    PUSH CS; CALL near modelled); MOV [BP+d],DS / MOV DS,[BP+d] spills.  Any
    other DS load (LDS, MOV DS,other) gives 'X'.  Calls and INT preserve DS.
    Joins that disagree give 'X'; unmodelled stack state gives 'X' on POP DS.
    Instructions not reached by direct flow (e.g. unparsed switch tables) are
    None and therefore never DGROUP-proven."""
    capstone = ctx['capstone']
    X86 = capstone.x86
    image, frame, reloc = ctx['image'], ctx['frame'], ctx['reloc_index']
    by_addr = {ins.address: ins for ins in instructions}
    order = [ins.address for ins in instructions]
    tables = {t['site']: t['targets'] for t in f.get('dispatch_tables', []) if 'targets' in t}
    if not any(writes_ds(bytes(i.bytes)) for i in instructions):
        return {a: 'DG' for a in order}

    def successors(ins):
        raw = bytes(ins.bytes)
        pre, _ = _prefix_split(raw)
        op = raw[pre] if pre < len(raw) else None
        a, end = ins.address, ins.address + ins.size
        if op in (0xc3, 0xc2, 0xcb, 0xca, 0xcf):
            return []
        if op in (0xeb, 0xe9):
            return [_rel_target(ins)]
        if op is not None and (0x70 <= op <= 0x7f or op in (0xe0, 0xe1, 0xe2, 0xe3)):
            return [_rel_target(ins), end]
        if a in tables:
            return list(tables[a])  # reviewed overlay dispatch table
        if op == 0xea or (op == 0xff and pre + 1 < len(raw) and ((raw[pre + 1] >> 3) & 7) in (4, 5)):
            return []  # far/indirect jump: successors not modelled
        return [end]

    def _rel_target(ins):
        raw = bytes(ins.bytes)
        pre, _ = _prefix_split(raw)
        op = raw[pre]
        if op in (0xeb,) or 0x70 <= op <= 0x7f or op in (0xe0, 0xe1, 0xe2, 0xe3):
            return ins.address + ins.size + int.from_bytes(raw[pre + 1:pre + 2], 'little', signed=True)
        return ins.address + ins.size + int.from_bytes(raw[pre + 1:pre + 3], 'little', signed=True)

    def transfer(ins, state):
        ds, regs, stack, bpstack, spills = state
        regs = dict(regs)
        spills = dict(spills)
        raw = bytes(ins.bytes)
        pre, _ = _prefix_split(raw)
        op = raw[pre] if pre < len(raw) else None
        modrm = raw[pre + 1] if pre + 1 < len(raw) else None

        def push(v):
            return stack + (v,)

        def pop():
            if not stack or stack[-1] == '?':
                return 'X', ('?',)
            return stack[-1], stack[:-1]
        # Segment/register pushes and pops.
        if op == 0x1e:
            return (ds, regs, push(ds), bpstack, spills)
        if op == 0x1f:
            v, st = pop()
            return (v, regs, st, bpstack, spills)
        if op in (0x06, 0x0e, 0x16):
            # PUSH CS; CALL near is a far call to a same-segment procedure.
            if op == 0x0e and ins.address + 1 in by_addr and image[ins.address + 1] == 0xe8:
                return (ds, regs, stack, bpstack, spills)
            return (ds, regs, push('X'), bpstack, spills)
        if op in (0x07, 0x17):
            v, st = pop()
            return (ds, regs, st, bpstack, spills)
        if op is not None and 0x50 <= op <= 0x57:
            name = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[op - 0x50]
            return (ds, regs, push(regs.get(name, 'X')), bpstack, spills)
        if op is not None and 0x58 <= op <= 0x5f:
            name = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[op - 0x58]
            v, st = pop()
            if name in _TRACKED:
                regs[name] = v
            return (ds, regs, st, bpstack, spills)
        if op in (0x9c,):
            return (ds, regs, push('X'), bpstack, spills)
        if op in (0x9d,):
            v, st = pop()
            return (ds, regs, st, bpstack, spills)
        if op == 0xff and modrm is not None and ((modrm >> 3) & 7) == 6:
            return (ds, regs, push('X'), bpstack, spills)
        if op == 0x8f:
            v, st = pop()
            return (ds, regs, st, bpstack, spills)
        if op == 0xe8:
            if ins.address - 1 >= 0 and image[ins.address - 1] == 0x0e and ins.address - 1 in by_addr:
                return (ds, regs, stack, bpstack, spills)
            return (ds, {}, stack, bpstack, spills)
        if op in (0x9a, 0xcd) or (op == 0xff and modrm is not None and ((modrm >> 3) & 7) in (2, 3)):
            return (ds, {}, stack, bpstack, spills)  # callee preserves DS, not scratch regs
        # SP arithmetic.
        if op in (0x83, 0x81) and modrm is not None and (modrm & 0xc7) == 0xc4:
            sub = ((modrm >> 3) & 7) == 5
            add = ((modrm >> 3) & 7) == 0
            imm = int.from_bytes(raw[pre + 2:pre + (3 if op == 0x83 else 4)], 'little',
                                 signed=(op == 0x83))
            if (add or sub) and imm % 2 == 0 and imm >= 0:
                n = imm // 2
                if add:
                    st = stack[:-n] if n else stack
                    if n > len(stack) or '?' in stack[len(stack) - n:]:
                        st = ('?',)
                    return (ds, regs, st, bpstack, spills)
                return (ds, regs, stack + ('X',) * n, bpstack, spills)
            return (ds, regs, ('?',), bpstack, spills)
        if raw[pre:pre + 2] == b'\x8b\xec':  # mov bp, sp
            return (ds, regs, stack, stack, spills)
        if raw[pre:pre + 2] == b'\x8b\xe5':  # mov sp, bp
            return (ds, regs, bpstack or ('?',), bpstack, spills)
        if op == 0xc9:  # leave
            st = bpstack or ('?',)
            return (ds, regs, st[:-1] if st and st[-1] != '?' else ('?',), bpstack, spills)
        # DS loads.
        if op == 0x8e and modrm is not None and ((modrm >> 3) & 7) == 3:
            if modrm >> 6 == 3:
                src = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[modrm & 7]
                return (regs.get(src, 'X'), regs, stack, bpstack, spills)
            key = _bp_slot(ins, capstone)
            return (spills.get(key, 'X') if key is not None else 'X', regs, stack, bpstack, spills)
        if op == 0xc5:
            return ('X', {k: v for k, v in regs.items() if k != _reg_of(ins)}, stack, bpstack, spills)
        if op == 0x8c and modrm is not None and ((modrm >> 3) & 7) == 3:
            if modrm >> 6 == 3:
                dst = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[modrm & 7]
                regs[dst] = ds
                return (ds, regs, stack, bpstack, spills)
            key = _bp_slot(ins, capstone)
            if key is not None:
                spills[key] = ds
            return (ds, regs, stack, bpstack, spills)
        if op is not None and 0xb8 <= op <= 0xbf and pre == 0:
            name = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[op - 0xb8]
            value = int.from_bytes(raw[1:3], 'little')
            if name in _TRACKED:
                regs[name] = 'DG' if (ins.address + 1 in reloc and value * 16 == frame) else 'X'
            return (ds, regs, stack, bpstack, spills)
        # Generic: written registers lose their DGROUP value; BP-slot writes clear spills.
        try:
            _, written = ins.regs_access()
        except Exception:
            written = []
        for reg in written:
            name = ins.reg_name(reg)
            for full in _TRACKED:
                if name in (full, full[0] + 'l', full[0] + 'h') or (name == 'e' + full):
                    regs.pop(full, None)
            if name in ('sp', 'esp'):
                stack = ('?',)
        for operand in ins.operands:
            if operand.type == X86.X86_OP_MEM and (operand.access & capstone.CS_AC_WRITE):
                key = _bp_slot(ins, capstone)
                if key is not None:
                    spills.pop(key, None)
        return (ds, regs, stack, bpstack, spills)

    def join(a, b):
        if a is None:
            return b
        ds = a[0] if a[0] == b[0] else 'X'
        regs = {k: v for k, v in a[1].items() if b[1].get(k) == v}
        stack = _join_stack(a[2], b[2])
        bpstack = a[3] if a[3] == b[3] else ('?',)
        spills = {k: v for k, v in a[4].items() if b[4].get(k) == v}
        return (ds, regs, stack, bpstack, spills)

    entry = ('DG', {}, (), None, {})
    before = {order[0]: entry} if order and order[0] == f['start'] else {}
    work = list(before)
    guard = 0
    while work and guard < 200000:
        guard += 1
        a = work.pop()
        ins = by_addr[a]
        out = transfer(ins, before[a])
        if trace is not None and before[a][0] == 'DG' and out[0] == 'X':
            trace.append((a, ins.mnemonic, ins.op_str, before[a][2][-4:], out[2][-4:]))
        for s in successors(ins):
            if s not in by_addr:
                continue
            merged = join(before.get(s), out)
            if merged != before.get(s):
                before[s] = merged
                work.append(s)
    return {a: (before[a][0] if a in before else None) for a in order}


def _join_stack(x, y):
    """SP-relative join: keep the common top entries above an unknown bottom."""
    if x == y:
        return x
    if x is None or y is None:
        return ('?',)
    k = 0
    while k < min(len(x), len(y)) and x[-1 - k] == y[-1 - k] and x[-1 - k] != '?':
        k += 1
    return ('?',) + (x[len(x) - k:] if k else ())


def _bp_slot(ins, capstone):
    for operand in ins.operands:
        if operand.type == capstone.x86.X86_OP_MEM and operand.mem.base and \
                ins.reg_name(operand.mem.base) == 'bp' and not operand.mem.index:
            return operand.mem.disp
    return None


def _reg_of(ins):
    return ins.op_str.split(',')[0].strip()


def scan_function(ctx, f):
    """All address-bearing operands of one procedure, from original bytes."""
    capstone = ctx['capstone']
    image, reloc_index = ctx['image'], ctx['reloc_index']
    instructions, complete = decode_function(ctx, f)
    ds_writes = [ins.address for ins in instructions if writes_ds(bytes(ins.bytes))]
    ds_at = ds_states(ctx, f, instructions)
    refs = []
    covered = set()
    for ins in instructions:
        raw = bytes(ins.bytes)
        at = ins.address
        pre, segment = _prefix_split(raw)
        op = raw[pre] if pre < len(raw) else None
        relocs = [a for a in range(at, at + ins.size) if a in reloc_index]
        covered.update(relocs)
        base = {'site': at, 'hex': raw.hex(), 'function': f['name'],
                'caller_task': f.get('stable_id'), 'text': ins.mnemonic + ' ' + ins.op_str}
        if op in (0x9a, 0xea) and pre == 0 and ins.size == 5:
            if at + 3 in reloc_index:
                seg = int.from_bytes(raw[3:5], 'little')
                off = int.from_bytes(raw[1:3], 'little')
                refs.append({**base, 'kind': 'far-call' if op == 0x9a else 'far-jmp',
                             'frame': seg * 16, 'target': seg * 16 + off,
                             'relocation': ctx['relocations'][reloc_index[at + 3]],
                             'relocation_index': reloc_index[at + 3]})
            else:
                refs.append({**base, 'kind': 'far-transfer-unrelocated'})
            continue
        if relocs:
            # MOV AX,imm16 immediately followed by MOV DX,imm16 whose word is
            # relocated: an offset/segment pointer pair.
            if op == 0xba and pre == 0 and relocs == [at + 1] and at - 3 >= 0 and \
                    image[at - 3] == 0xb8:
                pair = image[at - 3:at + 3]
                seg = int.from_bytes(pair[4:6], 'little')
                off = int.from_bytes(pair[1:3], 'little')
                refs.append({**base, 'site': at - 3, 'hex': pair.hex(), 'kind': 'pointer-pair',
                             'frame': seg * 16, 'target': seg * 16 + off,
                             'relocation': ctx['relocations'][reloc_index[at + 1]],
                             'relocation_index': reloc_index[at + 1]})
                continue
            for r in relocs:
                word = int.from_bytes(image[r:r + 2], 'little')
                refs.append({**base, 'kind': 'segment-word', 'relocation_site': r,
                             'paragraph_load_address': word * 16,
                             'relocation': ctx['relocations'][reloc_index[r]],
                             'relocation_index': reloc_index[r]})
            continue
        # Memory displacements.
        if ins.disp_size == 2 and ins.disp_offset > 0:
            for operand in ins.operands:
                if operand.type != capstone.x86.X86_OP_MEM:
                    continue
                seg = ins.reg_name(operand.mem.segment) if operand.mem.segment else None
                basereg = ins.reg_name(operand.mem.base) if operand.mem.base else None
                disp = operand.mem.disp & 0xffff
                if disp != int.from_bytes(raw[ins.disp_offset:ins.disp_offset + 2], 'little'):
                    continue
                if seg is None and basereg == 'bp':
                    break  # stack frame
                if seg == 'ss':
                    break
                kind = {'es': 'es-disp', 'cs': 'cs-disp'}.get(seg, 'dgroup-disp')
                refs.append({**base, 'kind': kind, 'operand_offset': ins.disp_offset,
                             'offset16': disp, 'indexed': bool(basereg or operand.mem.index),
                             'ds_proven': (ds_at.get(at) == 'DG') if kind == 'dgroup-disp' else None,
                             'ds_state': ds_at.get(at),
                             'lea': ins.mnemonic == 'lea'})
                break
        if ins.imm_size == 2 and ins.imm_offset > 0:
            value = int.from_bytes(raw[ins.imm_offset:ins.imm_offset + 2], 'little')
            refs.append({**base, 'kind': 'imm16', 'operand_offset': ins.imm_offset,
                         'offset16': value})
    return {'refs': refs, 'complete': complete, 'ds_writes': ds_writes,
            'covered_relocations': covered, 'instructions': len(instructions)}


def pinned_source(ctx, path):
    if path not in ctx['source_cache']:
        full = ROOT / 'build/references/restunts' / path
        require(path in ctx['pinned_files'] and
                identity(full.read_bytes()) == ctx['pinned_files'][path],
                'Pinned reference source differs: ' + path)
        ctx['source_cache'][path] = full.read_text(encoding='latin1').splitlines()
    return ctx['source_cache'][path]


def proc_span(ctx, f):
    """Pinned PROC/ENDP span of the inventory procedure (1-based lines)."""
    path = (f.get('provenance') or {}).get('path')
    if not path or not re.fullmatch(r'src/restunts/asmorig/seg\d{3}\.asm', path):
        return None
    lines = pinned_source(ctx, path)
    name = f['name']
    starts = [i + 1 for i, line in enumerate(lines)
              if re.match(r'^' + re.escape(name) + r'\s+proc\b', line.strip(), re.I)]
    ends = [i + 1 for i, line in enumerate(lines)
            if re.match(r'^' + re.escape(name) + r'\s+endp\b', line.strip(), re.I)]
    if len(starts) != 1 or len(ends) != 1 or not starts[0] < ends[0]:
        return None
    return path, starts[0], ends[0], lines


# ------------------------------------------------------------- data rules
def label_containing(ctx, dg):
    """Pinned dseg label placed at or before a DGROUP offset, with its span."""
    offsets = ctx['label_offsets']
    i = bisect.bisect_right(offsets, dg) - 1
    if i < 0:
        return None
    base = offsets[i]
    nxt = offsets[i + 1] if i + 1 < len(offsets) else None
    line, label = ctx['labels_at'][base][0]
    next_line = ctx['labels_at'][nxt][0][0] if nxt is not None else None
    return {'label': label, 'offset': base, 'line': line,
            'next_offset': nxt, 'next_line': next_line,
            'span': (nxt - base) if nxt is not None else None,
            'aliases_at_same_offset': [l for _, l in ctx['labels_at'][base][1:]]}


def storage_of(ctx, address):
    if ctx['frame'] <= address < ctx['bss_start'] and address < len(ctx['image']):
        return 'initialized'
    if ctx['bss_start'] <= address < ctx['bss_end']:
        return 'bss'
    return None


def check_width_candidate(ctx, name, address, placement, storage):
    """Reuse the resolver's own span checker; returns (width, provenance) or reason."""
    from data_symbols import _check_reference_width
    if placement['span'] is None:
        return None, 'terminal label span (no following label) unsupported by resolver'
    provenance = 'reference-label-span: dseg.asm:%d-%d' % (placement['line'], placement['next_line'])
    symbol = {'load_address': address, 'width': placement['span'],
              'width_provenance': provenance, 'storage': storage}
    try:
        _check_reference_width(name, symbol, ctx['frame'], {'offsets': ctx['placed']})
    except ValueError as error:
        return None, 'label span rejected by resolver rule: ' + str(error)
    end = address + placement['span']
    limit = ctx['bss_end'] if storage == 'bss' else ctx['bss_start']
    if end > limit:
        return None, 'label span crosses its storage class boundary'
    return (placement['span'], provenance), None


def existing_extents(ctx, data_existing):
    rows = []
    for name, row in data_existing.items():
        width = row.get('width')
        rows.append((row['load_address'], row['load_address'] + (width or 1), name, width))
    return sorted(rows)


# ------------------------------------------------------ existing aliases
def resolve_existing(ctx):
    """Resolve every existing alias through the production resolvers."""
    from code_symbols import resolve_code_symbols
    from data_symbols import resolve_symbols
    image, relocations = ctx['image'], ctx['relocations']
    code, broken = {}, []
    names = sorted(ctx['code_layout']['symbols'])
    try:
        code = resolve_code_symbols(names, image, relocations)
    except ValueError:
        for name in names:
            try:
                code.update(resolve_code_symbols([name], image, relocations))
            except ValueError as error:
                broken.append({'kind': 'code', 'name': name, 'error': str(error)})
    data = {}
    dg_names = sorted(n for n, s in ctx['data_layout']['symbols'].items()
                      if s.get('storage') != 'code_island')
    try:
        data = resolve_symbols(set(dg_names), image, relocations)
    except ValueError:
        for name in dg_names:
            try:
                data.update(resolve_symbols({name}, image, relocations))
            except ValueError as error:
                broken.append({'kind': 'data', 'name': name, 'error': str(error)})
    for name, row in data.items():
        symbol = ctx['data_layout']['symbols'][name]
        row['width'] = symbol.get('width')
        row['storage'] = symbol['storage']
    return code, data, broken


def audit_existing(ctx, code_existing, data_existing):
    """Report existing aliases whose own spelling places them elsewhere."""
    findings = []
    ida = re.compile(r'_?(?:byte|word|dword|unk|off|stru|asc|flt|dbl|seg|a)_([0-9A-Fa-f]{5})$')
    for name, row in sorted(data_existing.items()):
        match = ida.match(name)
        if match:
            implied = int(match.group(1), 16) - 0x10000
            if row['load_address'] not in (implied, implied + 0x10000):
                findings.append({'kind': 'data', 'name': name, 'load_address': row['load_address'],
                                 'dgroup_offset': hex(row['load_address'] - ctx['frame']),
                                 'issue': 'address-derived spelling implies load %d (IDA) or %d (load-address convention)' %
                                          (implied, implied + 0x10000)})
        label = name[1:] if name.startswith('_') else name
        if label in ctx['placed']:
            offset, line = ctx['placed'][label]
            if ctx['frame'] + offset != row['load_address']:
                findings.append({'kind': 'data', 'name': name, 'load_address': row['load_address'],
                                 'dgroup_offset': hex(row['load_address'] - ctx['frame']),
                                 'issue': 'pinned dseg places label %s at %s (line %d)' %
                                          (label, hex(offset), line)})
    rows = existing_extents(ctx, data_existing)
    for i, (start, end, name, width) in enumerate(rows):
        for start2, end2, name2, width2 in rows[i + 1:]:
            if start2 >= end:
                break
            if name2 != name and start2 != start and width:
                findings.append({'kind': 'data-overlap', 'name': name, 'other': name2,
                                 'issue': 'reviewed extent [%s,%s) contains separately '
                                          'aliased %s at %s' % (hex(start - ctx['frame']),
                                                                hex(end - ctx['frame']),
                                                                name2, hex(start2 - ctx['frame']))})
    # Existing aliases at a DGROUP address where no pinned label is placed,
    # inside the pinned span of a label that itself carries an alias: two
    # overlapping object models (blocks a label-span width at the base).
    by_address = defaultdict(list)
    for name, row in data_existing.items():
        by_address[row['load_address']].append(name)
    for address, names in sorted(by_address.items()):
        dg = address - ctx['frame']
        if not 0 <= dg < 0x10000 or dg in ctx['labels_at']:
            continue
        placement = label_containing(ctx, dg)
        if placement is None or placement['span'] is None or \
                not placement['offset'] < dg < placement['offset'] + placement['span']:
            continue
        base = ctx['frame'] + placement['offset']
        if by_address.get(base):
            findings.append({'kind': 'data-interior-alias', 'name': sorted(names)[0],
                             'aliases': sorted(names), 'dgroup_offset': hex(dg),
                             'containing_label': placement['label'],
                             'label_dgroup_offset': hex(placement['offset']),
                             'label_span': placement['span'],
                             'base_aliases': sorted(by_address[base]),
                             'issue': 'alias at unlabelled interior address +%d of pinned label %s '
                                      'whose base is aliased; blocks a label-span width' %
                                      (dg - placement['offset'], placement['label'])})
    code_ida = re.compile(r'_?(?:sub|nopsub|loc|j_sub)_([0-9A-Fa-f]{5})$')
    for name, row in sorted(code_existing.items()):
        match = code_ida.match(name)
        if match:
            implied = int(match.group(1), 16) - 0x10000
            if row['load_address'] not in (implied, implied + 0x10000):
                findings.append({'kind': 'code', 'name': name, 'load_address': row['load_address'],
                                 'issue': 'address-derived spelling implies load %d' % implied})
        symbol = ctx['code_layout']['symbols'][name]
        target = symbol.get('mapped_target')
        if target:
            expected = {'_' + target['name'], target['name']}
            if name not in expected and not symbol.get('masm_truncated_public') and \
                    symbol.get('reviewed_alias') != name:
                findings.append({'kind': 'code', 'name': name,
                                 'issue': 'spelling differs from mapped target %s' % target['name']})
    return findings


# ---------------------------------------------------------- derivation
def code_target_row(ctx, target):
    rows = [f for f in ctx['inventory'] if f.get('start') == target]
    return rows


def target_eligible(ctx, f):
    """The resolver's mapped-target predicate (without the reviewed boundary flag)."""
    from code_symbols import _complete_target_owner
    image = ctx['image']
    if f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
        proof = 'instruction-verified'
    elif f.get('status') == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED':
        if f.get('start_evidence') and f.get('end_evidence'):
            proof = 'emission-verified-start-end-evidence'
        else:
            before = any(g['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                         g.get('end') == f['start'] for g in ctx['inventory'])
            after = any(g['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                        g.get('start') == f['end'] for g in ctx['inventory'])
            if not (before and after):
                return None, 'emission-verified target lacks start/end evidence or verified neighbours'
            proof = 'verified-neighbours-v1'
        if f.get('bytes_hex') and bytes.fromhex(f['bytes_hex']) != image[f['start']:f['end']]:
            return None, 'emission bytes changed'
    else:
        return None, 'target inventory row status %s' % f.get('status')
    if sha(image[f['start']:f['end']]) != f['sha256']:
        return None, 'target extent hash differs'
    owners = [o for o in ctx['owners'] if _complete_target_owner(o, f)]
    if not owners:
        return None, 'target lacks a complete raw/exact/runtime owner'
    return {'proof': proof, 'owner': owners[0]['id'], 'owner_kind': owners[0]['kind']}, None


def harvest_code_anchors(ctx, scans):
    """Every original relocated far CALL/JMP/pointer pair in verified callers."""
    by_target = defaultdict(list)
    for f, scan in scans:
        if not is_instruction_verified(f) or not f.get('stable_id'):
            continue
        for ref in scan['refs']:
            if ref['kind'] in ('far-call', 'far-jmp', 'pointer-pair'):
                by_target[ref['target']].append(ref)
    return by_target


def harvest_dgroup_anchors(ctx, scans, base_only=True):
    """Unrelocated DS displacements in verified procs (base inventory for compact)."""
    base_verified = {(f['name'], f['start']) for f in ctx['base_inventory']
                     if f.get('status') == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'}
    by_offset = defaultdict(list)
    reloc = ctx['reloc_index']
    for f, scan in scans:
        if base_only and (f['name'], f['start']) not in base_verified:
            continue
        if not base_only and not (is_instruction_verified(f) or
                                  (f.get('start_evidence') and f.get('end_evidence'))):
            continue
        for ref in scan['refs']:
            if ref['kind'] != 'dgroup-disp':
                continue
            at = ref['site'] + ref['operand_offset']
            if at - 1 in reloc or at in reloc or at + 1 in reloc:
                continue
            by_offset[ref['offset16']].append({**ref, 'function_row': f,
                                               'function_sha256': f['sha256'],
                                               'function_start': f['start'],
                                               'function_end': f['end']})
    return by_offset


def _anchor_receipt(a):
    return {'site': a['site'], 'hex': a['hex'], 'operand_offset': a['operand_offset'],
            'text': a['text'], 'function': a['function'], 'caller_task': a.get('caller_task'),
            'function_extent': [a['function_start'], a['function_end']],
            'function_sha256': a['function_sha256'], 'ds_state': a.get('ds_state', 'n/a'),
            'operand_relocated': False}


def imm_use_lines(ctx, f, label):
    span = proc_span(ctx, f)
    if span is None:
        return None
    path, first, last, lines = span
    hits = [n for n in range(first + 1, last)
            if re.search(r'\boffset\s+' + re.escape(label) + r'\b', lines[n - 1].split(';')[0], re.I)]
    return path, hits


def derive(ctx, spelling='neutral'):
    image, frame = ctx['image'], ctx['frame']
    code_existing, data_existing, broken = resolve_existing(ctx)
    audit = audit_existing(ctx, code_existing, data_existing)
    code_by_address = defaultdict(list)
    for name, row in code_existing.items():
        code_by_address[row['load_address']].append((name, row))
    data_by_address = defaultdict(list)
    for name, row in data_existing.items():
        data_by_address[row['load_address']].append((name, row))
    extents = existing_extents(ctx, data_existing)

    # Scan every decodable procedure once.
    scans = []
    for f in ctx['inventory']:
        if decodable(f, image):
            scans.append((f, scan_function(ctx, f)))
    code_anchors = harvest_code_anchors(ctx, scans)
    dgroup_anchors = harvest_dgroup_anchors(ctx, scans)
    all_disp = harvest_dgroup_anchors(ctx, scans, base_only=False)
    imm_by_offset = defaultdict(list)
    for f, scan in scans:
        if is_instruction_verified(f) or (f.get('start_evidence') and f.get('end_evidence')):
            for r in scan['refs']:
                if r['kind'] == 'imm16':
                    imm_by_offset[r['offset16']].append({**r, 'function_row': f,
                                                         'function_sha256': f['sha256'],
                                                         'function_start': f['start'],
                                                         'function_end': f['end']})

    # Raw-code references: instructions inside raw ownership.
    references = []
    raw_functions = []
    for f, scan in scans:
        if not any(s < f['end'] and f['start'] < e for s, e, _ in ctx['raw']):
            continue
        raw_functions.append({'name': f['name'], 'start': f['start'], 'end': f['end'],
                              'status': f['status'], 'decode_complete': scan['complete'],
                              'ds_writes': scan['ds_writes']})
        for ref in scan['refs']:
            if in_raw(ctx, ref['site']):
                references.append({**ref, 'function_status': f['status'],
                                   'function_verified': is_instruction_verified(f),
                                   'function_row': f, 'ds_writes': scan['ds_writes']})
    covered = set()
    for f, scan in scans:
        covered |= scan['covered_relocations']
    # Relocated words in raw bytes that no decoded instruction covers.
    for r in ctx['relocations']:
        at = r['load_offset']
        if at in covered or not in_raw(ctx, at, at + 2):
            continue
        references.append({'site': at, 'kind': 'reloc-outside-decoded-code',
                           'relocation': r, 'relocation_index': ctx['reloc_index'][at],
                           'hex': image[at - 2:at + 2].hex() if at >= 2 else image[at:at + 2].hex(),
                           'paragraph_load_address': int.from_bytes(image[at:at + 2], 'little') * 16})

    code_props, data_props, width_additions = {}, {}, {}
    receipts, unprovable = {}, []
    status_rows = []

    # ---- code references
    def classify_code(ref):
        target, ref_frame = ref['target'], ref['frame']
        existing = [(n, r) for n, r in code_by_address.get(target, [])]
        same_frame = [n for n, r in existing if r['frame_load_address'] == ref_frame]
        if same_frame:
            return {'status': 'existing', 'alias': sorted(same_frame)[0]}
        if not ref['function_verified']:
            return {'status': 'not-provable', 'class': 'caller-not-instruction-verified'}
        rows = code_target_row(ctx, target)
        if not rows:
            owner = owner_at(ctx, target)
            return {'status': 'not-provable', 'class': 'target-not-an-inventory-entry',
                    'detail': 'no inventory row starts at %d (owner %s)' %
                              (target, owner['id'] if owner else None)}
        failures = []
        for row in rows:
            ok, why = target_eligible(ctx, row)
            if ok:
                return {'status': 'provable', 'row': row, 'target_proof': ok,
                        'existing_other_frame': sorted(n for n, _ in existing)}
            failures.append('%s: %s' % (row['name'], why))
        return {'status': 'not-provable', 'class': 'target-boundary-or-owner-unverified',
                'detail': '; '.join(failures)}

    for ref in references:
        if ref['kind'] not in ('far-call', 'far-jmp', 'pointer-pair'):
            continue
        result = classify_code(ref)
        if result['status'] == 'provable' and not any(
                a['frame'] == ref['frame'] and
                not result['row']['start'] <= a['site'] < result['row']['end']
                for a in code_anchors.get(ref['target'], [])):
            # integ30: an entry reached only from inside its own extent (the
            # unreferenced stubs of file_decomp_fatal) has no independent anchor.
            result = {'status': 'not-provable', 'class': 'only-self-internal-anchors',
                      'detail': 'every relocated reference lies inside the target extent'}
        if result['status'] == 'provable':
            row = result['row']
            stable = row.get('stable_id') or 'load_%05x' % row['start']
            if spelling == 'label':
                name = '_' + row['name']
                if name in ctx['code_layout']['symbols']:
                    name = '_code_' + stable
            else:
                name = '_code_' + stable
            key = (name, ref['frame'])
            if name in code_props and code_props[name]['frame_load_address'] != ref['frame']:
                name = '%s_f%05x' % (name, ref['frame'])
            if name not in code_props:
                anchors = [a for a in code_anchors.get(ref['target'], [])
                           if a['frame'] == ref['frame'] and
                           not row['start'] <= a['site'] < row['end']]
                calls = [a for a in anchors if a['kind'] in ('far-call', 'far-jmp')]
                pairs = [a for a in anchors if a['kind'] == 'pointer-pair']
                entry = {'frame_load_address': ref['frame'],
                         'mapped_target': {'name': row['name'], 'start': row['start'],
                                           'end': row['end'], 'sha256': row['sha256'],
                                           **({'stable_id': row['stable_id']}
                                              if row.get('stable_id') else {})},
                         'distinct_verified_callers': len({a['caller_task'] for a in anchors})}
                if calls:
                    entry['anchors'] = [{'caller_task': a['caller_task'], 'hex': a['hex'],
                                         'relocation': a['relocation'], 'site': a['site']}
                                        for a in sorted(calls, key=lambda a: a['site'])]
                if pairs:
                    entry['pointer_anchors'] = [{'caller_task': a['caller_task'], 'hex': a['hex'],
                                                 'relocation': a['relocation'], 'site': a['site']}
                                                for a in sorted(pairs, key=lambda a: a['site'])]
                if name != '_' + row['name'] and spelling != 'neutral-all':
                    entry['reviewed_alias'] = name
                if result['target_proof']['proof'] == 'verified-neighbours-v1':
                    entry['boundary_proof'] = 'verified-neighbours-v1'
                entry['proposal_basis'] = ('%s neutral binding alias from original relocated '
                                           'anchors; not an original PUBDEF/source/TU claim; '
                                           'receipt in autosym_receipts.json' % TOOL_VERSION)
                code_props[name] = entry
                receipts[name] = {
                    'kind': 'far-code',
                    'target_proof': {**result['target_proof'], 'inventory_status': row['status'],
                                     'extent': [row['start'], row['end']],
                                     'sha256': row['sha256'],
                                     'image_sha256_check': sha(image[row['start']:row['end']]) == row['sha256']},
                    'anchors': [{'site': a['site'], 'hex': a['hex'], 'kind': a['kind'],
                                 'caller': a['function'], 'caller_task': a['caller_task'],
                                 'relocation_index': a['relocation_index'],
                                 'relocation': a['relocation'],
                                 'segment_word_load_offset': a['relocation']['load_offset'],
                                 'frame': a['frame'], 'target': a['target']}
                                for a in sorted(anchors, key=lambda a: a['site'])],
                    'other_frame_aliases_same_address': result['existing_other_frame']}
            status_rows.append({**_public_ref(ref), 'status': 'proposed', 'alias': name})
        else:
            status_rows.append({**_public_ref(ref), **{k: v for k, v in result.items()
                                                        if k in ('status', 'alias', 'class', 'detail')}})

    # ---- DGROUP references
    def existing_container(address):
        hits = [(s, e, n, w) for s, e, n, w in extents if s <= address < e]
        exact = [h for h in hits if h[0] == address]
        if exact:
            return {'status': 'existing', 'alias': sorted(h[2] for h in exact)[0], 'addend': 0}
        inside = [h for h in hits if h[3] and address - h[0] in
                  set(data_existing[h[2]]['allowed_addends'])]
        if len(inside) == 1:
            return {'status': 'existing+addend', 'alias': inside[0][2],
                    'addend': address - inside[0][0]}
        if len(inside) > 1:
            return {'status': 'not-provable', 'class': 'ambiguous-existing-extents',
                    'detail': ', '.join(h[2] for h in inside)}
        return None

    def use_lines(f, label, offset_only):
        span = proc_span(ctx, f)
        if span is None:
            return None, []
        path, first, last, lines = span
        pattern = (r'\boffset\s+' if offset_only else r'\b') + re.escape(label) + r'\b'
        return path, [n for n in range(first + 1, last)
                      if re.search(pattern, lines[n - 1].split(';')[0], re.I)]

    def reviewed_candidate(dg, label, kind, field=0):
        """Anchor + pinned use line in the same verified procedure."""
        pool = all_disp.get(dg + field, []) if kind == 'disp' else imm_by_offset.get(dg, [])
        for anchor in pool:
            if kind == 'disp' and not anchor['ds_proven']:
                continue
            path, hits = use_lines(anchor['function_row'], label, kind == 'imm')
            if hits:
                return {'function': anchor['function_row'], 'path': path, 'use_line': hits[0],
                        'anchor': anchor, 'field': field}
        return None

    def propose_data(address, want_width):
        """Alias at a pinned-label address; returns ((name, entry, receipt), None) or (None, reason)."""
        dg = address - frame
        storage = storage_of(ctx, address)
        placement = label_containing(ctx, dg)
        require(placement is not None and placement['offset'] == dg, 'internal: label not at address')
        label = placement['label']
        width, why_no_width = None, None
        if want_width:
            width, why_no_width = check_width_candidate(ctx, '_x', address, placement, storage)
            if width is not None:
                end = address + width[0]
                # Interior-alias policy (integ25 ruling): an alias wholly inside
                # the container's placed label span is a sub-object/field name
                # for that same address, not a conflict.  Only a partial overlap
                # (an object starting before the span or running past its end)
                # still refuses the width.
                def partial(s, e):
                    return s < end and address < e and s != address and (s < address or e > end)
                clash = [n for s, e, n, w in extents if partial(s, e)]
                clash += [n for n, p in data_props.items()
                          if p['load_address'] != address and
                          partial(p['load_address'], p['load_address'] + (p.get('width') or 1))]
                if clash:
                    width, why_no_width = None, 'label span overlaps separately aliased ' + ', '.join(sorted(clash)[:6])
        anchors = [a for a in dgroup_anchors.get(dg, []) if a['ds_proven']]
        form, reviewed = None, None
        if anchors:
            form = 'compact-displacement'
        else:
            reviewed = reviewed_candidate(dg, label, 'disp')
            form = 'reviewed-displacement' if reviewed else None
            if reviewed is None:
                reviewed = reviewed_candidate(dg, label, 'imm')
                form = 'reviewed-offset-immediate' if reviewed else None
            if reviewed is None and width is not None:
                for k in range(1, width[0]):
                    reviewed = reviewed_candidate(dg, label, 'disp', k)
                    if reviewed:
                        form = 'reviewed-field-anchor'
                        break
            if reviewed is None:
                return None, ('no DS-proven unrelocated displacement anchor in a base-inventory '
                              'verified procedure, and no displacement/offset-immediate/field anchor '
                              'with a pinned use line of label %s in its procedure' % label)
        if (form == 'compact-displacement' and spelling == 'neutral') or spelling == 'neutral-all':
            name = '_dg_%04x' % dg
        else:
            name = ('_' + label)[:31]
        for table in (ctx['data_layout']['symbols'], data_props):
            other = table.get(name)
            if other is not None and other['load_address'] != address:
                if form == 'compact-displacement':
                    name = '_dg_%04x' % dg
                else:
                    return None, ('resolver-required label spelling %s already names DGROUP %s' %
                                  (name, hex(other['load_address'] - frame)))
        entry = {'load_address': address, 'storage': storage}
        receipt = {'kind': 'dgroup-data', 'form': form, 'dgroup_offset': hex(dg), 'storage': storage,
                   'placement': {'label': label, 'dseg_line': placement['line'],
                                 'dgroup_offset': hex(placement['offset']),
                                 'placed_by': 'cumulative pinned dseg.asm declaration sizes',
                                 'other_labels_same_offset': placement['aliases_at_same_offset']}}
        if form == 'compact-displacement':
            receipt['anchors'] = [_anchor_receipt(a) for a in anchors[:8]]
            receipt['anchor_count'] = len(anchors)
        else:
            f, anchor = reviewed['function'], reviewed['anchor']
            ref_row = {'start': anchor['site'], 'hex': anchor['hex'],
                       'operand_offset': anchor['operand_offset']}
            if reviewed['field']:
                ref_row['field_offset'] = reviewed['field']
            entry.update({'reference_label': label,
                          'reference_declaration_line': placement['line'],
                          'reference_use_path': reviewed['path'],
                          'reference_use_line': reviewed['use_line'],
                          'reference_use_proc': f['name'],
                          'references': [ref_row]})
            if name.startswith('_' + label[:3]) and len(name) < len('_' + label):
                entry['masm_truncated_public'] = True
            receipt['spelling_forced_by_resolver'] = spelling != 'neutral-all'
            receipt['anchors'] = [{**_anchor_receipt(anchor), 'field_offset': reviewed['field'],
                                   'use_line': reviewed['use_line'], 'use_path': reviewed['path'],
                                   'use_text': pinned_source(ctx, reviewed['path'])[reviewed['use_line'] - 1].strip()}]
        if width is not None:
            entry['width'], entry['width_provenance'] = width
            receipt['extent'] = {'width': width[0], 'width_provenance': width[1],
                                 'span_end_label_line': placement['next_line'],
                                 'interior_offsets_accessed': sorted(interior.get(dg, [])),
                                 'overlap_check': 'no existing or proposed alias inside span'}
        elif want_width:
            receipt['extent'] = {'width': None, 'reason': why_no_width}
        return (name, entry, receipt), None

    # Interior-reference demand per pinned label (DS-proven accesses anywhere).
    interior = defaultdict(set)
    for dg, rows in all_disp.items():
        placement = label_containing(ctx, dg)
        if placement and placement['offset'] != dg and any(r['ds_proven'] for r in rows):
            interior[placement['offset']].add(dg - placement['offset'])

    def single_item(placement):
        return placement['span'] is not None and placement['next_line'] - placement['line'] == 1

    data_refs = [r for r in references if r['kind'] == 'dgroup-disp']
    for ref in data_refs:
        dg = ref['offset16']
        address = frame + dg
        if not ref['ds_proven']:
            status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                'class': 'ds-not-proven-dgroup',
                                'detail': 'DS state %s before this instruction (procedure DS writes at %s)' % (ref.get('ds_state') or 'unreached', ref.get('ds_writes'))})
            continue
        storage = storage_of(ctx, address)
        if storage is None:
            status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                'class': 'indexed-negative-displacement' if ref.get('indexed') and dg >= 0x8000 else 'outside-dgroup-storage',
                                'detail': 'DGROUP offset %s outside initialized/BSS' % hex(dg)})
            continue
        found = existing_container(address)
        if found:
            status_rows.append({**_public_ref(ref), **found})
            continue
        placement = label_containing(ctx, dg)
        if placement is None:
            status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                'class': 'no-pinned-label'})
            continue
        base = frame + placement['offset']
        want_width = bool(interior.get(placement['offset'])) or single_item(placement)
        # Proposal for the label base (either the reference itself or its container).
        prop_name = next((n for n, p in data_props.items() if p['load_address'] == base), None)
        if prop_name is None:
            base_existing = [n for n, _ in data_by_address.get(base, [])]
            if base_existing:
                # Existing alias at the label base lacks a width covering this reference.
                name0 = sorted(base_existing)[0]
                wid, why = check_width_candidate(ctx, name0, base, placement, storage_of(ctx, base))
                if wid is not None:
                    end = base + wid[0]
                    clash = [n for s, e, n, w in extents if s < end and base < e and s != base]
                    if clash:
                        wid, why = None, 'span overlaps ' + ', '.join(sorted(clash))
                if wid is not None and name0 in WIDTH_REFUSED:
                    wid, why = None, 'reviewed width refusal: ' + WIDTH_REFUSED[name0]
                if wid is not None and 0 < address - base < wid[0]:
                    width_additions[name0] = {'width': wid[0], 'width_provenance': wid[1]}
                    receipts.setdefault(name0, {'kind': 'width-addition', 'load_address': base,
                                                'placement': placement, 'interior_offsets':
                                                sorted(interior.get(placement['offset'], []))})
                    status_rows.append({**_public_ref(ref), 'status': 'existing+width-addition',
                                        'alias': name0, 'addend': address - base})
                else:
                    status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                        'class': 'interior-of-existing-alias-without-extent',
                                        'detail': '%s at %s; %s' % (name0, hex(placement['offset']), why)})
                continue
            proposal, why = propose_data(base, want_width)
            if proposal is None:
                status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                    'class': 'data-anchor-missing' if 'anchor' in why else 'data-rule',
                                    'detail': 'label %s: %s' % (placement['label'], why)})
                continue
            prop_name, entry, receipt = proposal
            data_props[prop_name] = entry
            receipts[prop_name] = receipt
        entry = data_props[prop_name]
        addend = address - base
        if addend == 0:
            status_rows.append({**_public_ref(ref), 'status': 'proposed', 'alias': prop_name, 'addend': 0})
        elif entry.get('width') and addend < entry['width']:
            status_rows.append({**_public_ref(ref), 'status': 'proposed+addend',
                                'alias': prop_name, 'addend': addend})
        else:
            status_rows.append({**_public_ref(ref), 'status': 'not-provable',
                                'class': 'addend-outside-grounded-extent',
                                'detail': '%s + %d has no reviewed extent (%s)' %
                                          (prop_name, addend, receipts[prop_name].get('extent'))})

    # ---- containers whose interior accesses all hit existing field aliases
    # Interior-alias policy (integ25 ruling): an existing alias at the placed
    # label base without a width gets the label-span width when original
    # DS-proven accesses reach its interior, even where those interior
    # addresses already carry their own (sub-object/field) aliases, which are
    # kept.  A partially overlapping object still refuses the width.
    for dg in sorted(interior):
        base = frame + dg
        placement = label_containing(ctx, dg)
        if placement is None or placement['offset'] != dg or storage_of(ctx, base) is None:
            continue
        base_existing = sorted(n for n, _ in data_by_address.get(base, []))
        if not base_existing or any(data_existing[n].get('width') for n in base_existing):
            continue
        name0 = base_existing[0]
        if name0 in width_additions or name0 in WIDTH_REFUSED:
            continue
        wid, why = check_width_candidate(ctx, name0, base, placement, storage_of(ctx, base))
        if wid is None:
            continue
        end = base + wid[0]
        if any(s < end and base < e and s != base and (s < base or e > end)
               for s, e, n, w in extents):
            continue
        inside = sorted({n for s, e, n, w in extents if base < s < end})
        if not inside:
            continue  # handled by the demand-driven pass above
        width_additions[name0] = {'width': wid[0], 'width_provenance': wid[1]}
        receipts.setdefault(name0, {'kind': 'width-addition', 'load_address': base,
                                    'placement': placement,
                                    'interior_offsets': sorted(interior[dg]),
                                    'interior_field_aliases': inside,
                                    'policy': 'container label span over interior field aliases'})

    # ---- offset immediates tied to pinned `offset LABEL` use lines
    imm_budget = {}
    for ref in sorted((r for r in references if r['kind'] == 'imm16'), key=lambda r: r['site']):
        f = ref['function_row']
        dg = ref['offset16']
        placement = label_containing(ctx, dg)
        if placement is None or placement['offset'] != dg or storage_of(ctx, frame + dg) is None:
            continue
        uses = imm_use_lines(ctx, f, placement['label'])
        if not uses or not uses[1]:
            continue  # a constant, or not provably an address
        # Count at most as many immediates as the procedure has pinned
        # `offset LABEL` uses; surplus equal immediates stay unattributed.
        key = (f['name'], f['start'], placement['label'])
        imm_budget.setdefault(key, len(uses[1]))
        if imm_budget[key] <= 0:
            status_rows.append({**_public_ref(ref), 'kind': 'imm16-unattributed', 'status': 'not-alias',
                                'class': 'immediate-beyond-pinned-offset-use-count',
                                'detail': 'value equals %s placement but all %d pinned uses are attributed' %
                                          (placement['label'], len(uses[1]))})
            continue
        imm_budget[key] -= 1
        address = frame + dg
        found = existing_container(address)
        if found:
            status_rows.append({**_public_ref(ref), 'kind': 'dgroup-offset-imm', **found})
            continue
        prop_name = next((n for n, p in data_props.items() if p['load_address'] == address), None)
        if prop_name is None:
            proposal, why = propose_data(address, bool(interior.get(dg)) or single_item(placement))
            if proposal is None:
                status_rows.append({**_public_ref(ref), 'kind': 'dgroup-offset-imm',
                                    'status': 'not-provable', 'class': 'data-rule', 'detail': why})
                continue
            prop_name, entry, receipt = proposal
            data_props[prop_name] = entry
            receipts[prop_name] = receipt
        status_rows.append({**_public_ref(ref), 'kind': 'dgroup-offset-imm',
                            'status': 'proposed', 'alias': prop_name, 'addend': 0})

    # ---- everything else is explicitly classified
    island_rows = [(s['load_address'], s['load_address'] + s.get('width', 1), n)
                   for n, s in ctx['data_layout']['symbols'].items() if s.get('storage') == 'code_island']
    dgroup_para = frame // 16
    for ref in references:
        kind = ref['kind']
        if kind in ('far-call', 'far-jmp', 'pointer-pair', 'dgroup-disp', 'imm16'):
            continue
        row = _public_ref(ref)
        if kind == 'cs-disp' and re.match(r'2eff[a-f0-9][4-9a-f]', ref['hex']) and \
                ((bytes.fromhex(ref['hex'])[2] >> 3) & 7) in (4, 5):
            row.update({'status': 'not-alias', 'class': 'switch-table-dispatch',
                        'detail': 'CS jump table word list; same-segment offset16 table rule, not a symbol alias'})
        elif kind == 'cs-disp':
            fr = (ref.get('function_row') or {}).get('segment_paragraph')
            address = fr * 16 + ref['offset16'] if isinstance(fr, int) else None
            hit = [n for s, e, n in island_rows if address is not None and s <= address < e]
            if hit:
                row.update(status='existing', alias=hit[0], addend=address - ctx['data_layout']['symbols'][hit[0]]['load_address'])
            else:
                row.update({'status': 'not-provable', 'class': 'cs-island-unreviewed',
                            'detail': 'CS data at %s needs a reviewed pinned code-island listing' %
                                      (address if address is not None else '?')})
        elif kind == 'es-disp' and ref.get('indexed'):
            row.update({'status': 'not-alias', 'class': 'es-relative-field-offset',
                        'detail': 'displacement is a field offset from a runtime far pointer in ES:reg'})
        elif kind == 'es-disp':
            row.update({'status': 'not-provable', 'class': 'es-relative-segment-unknown'})
        elif kind == 'segment-word':
            para = ref['paragraph_load_address']
            what = ('DGROUP base' if para == frame else
                    'code frame' if para in ctx['code_frames'] else 'far/other segment')
            row.update({'status': 'not-alias', 'class': 'segment-base16 (%s)' % what,
                        'detail': 'segment-only reference; bound by module/frame rules, not a symbol alias'})
        elif kind == 'far-transfer-unrelocated':
            row.update({'status': 'not-provable', 'class': 'far-transfer-without-relocation'})
        elif kind == 'reloc-outside-decoded-code':
            para = ref['paragraph_load_address']
            where = 'DGROUP' if frame <= ref['site'] < ctx['bss_start'] else 'code region'
            if where == 'DGROUP':
                off = int.from_bytes(image[ref['site'] - 2:ref['site']], 'little')
                target = para + off
                rows = code_target_row(ctx, target)
                row.update({'status': 'not-provable', 'class': 'pointer32-in-dgroup-data',
                            'target': target,
                            'detail': 'dd far pointer; resolver has no data-table anchor form '
                                      '(target %s inventory entry)' % ('is an' if rows else 'is not an')})
            else:
                row.update({'status': 'not-provable', 'class': 'relocation-in-unverified-code',
                            'detail': 'no instruction-verified procedure decodes this site'})
        status_rows.append(row)

    return {'code_symbols': code_props, 'data_symbols': data_props,
            'width_additions': width_additions, 'receipts': receipts,
            'references': status_rows, 'broken_existing': broken, 'audit': audit,
            'raw_functions': raw_functions, 'existing_code': len(code_existing),
            'existing_data': len(data_existing)}


def _public_ref(ref):
    keep = ('site', 'hex', 'kind', 'function', 'caller_task', 'text', 'target', 'frame',
            'relocation_index', 'operand_offset', 'offset16', 'indexed',
            'paragraph_load_address', 'relocation_site')
    row = {k: ref[k] for k in keep if k in ref}
    if 'offset16' in row:
        row['dgroup_offset_hex'] = hex(row['offset16'])
    return row


# ------------------------------------------------------------- reporting
def coverage(result):
    rows = result['references']
    kinds = Counter(r['kind'] for r in rows)
    table = defaultdict(Counter)
    for r in rows:
        table[r['kind']][r['status']] += 1
    classes = Counter((r['kind'], r.get('class')) for r in rows if r['status'] == 'not-provable')
    bindable_before = sum(1 for r in rows if r['status'] in ('existing', 'existing+addend'))
    newly = sum(1 for r in rows if r['status'] in ('proposed', 'proposed+addend',
                                                   'existing+width-addition'))
    symbolic = sum(1 for r in rows if r['status'] != 'not-alias')
    return {'references_total': len(rows), 'symbol_references': symbolic,
            'bindable_with_existing_aliases': bindable_before,
            'newly_bindable_with_proposals': newly,
            'not_provable': sum(1 for r in rows if r['status'] == 'not-provable'),
            'segment_only_not_alias': sum(1 for r in rows if r['status'] == 'not-alias'),
            'by_kind': {k: dict(v) for k, v in sorted(table.items())},
            'not_provable_classes': {'%s/%s' % k: v for k, v in sorted(classes.items(), key=str)},
            'proposed_code_aliases': len(result['code_symbols']),
            'proposed_data_aliases': len(result['data_symbols']),
            'proposed_width_additions': len(result['width_additions']),
            'raw_verified_functions_scanned': len(result['raw_functions'])}


def write_outputs(ctx, result, out):
    from common import write_json
    out.mkdir(parents=True, exist_ok=True)
    oracle = sha(ctx['image'])
    write_json(out / 'autosym_code_symbols.json',
               {'oracle_sha256': oracle, 'symbols': result['code_symbols']})
    data = ctx['data_layout']
    write_json(out / 'autosym_data_symbols.json',
               {'schema': data['schema'], 'oracle_sha256': oracle,
                'frame_load_address': data['frame_load_address'],
                'frame_relocation': data['frame_relocation'],
                'bss_start': data['bss_start'], 'bss_end': data['bss_end'],
                'symbols': result['data_symbols']})
    write_json(out / 'autosym_data_extent_additions.json',
               {'oracle_sha256': oracle, 'extent_additions': result['width_additions']})
    write_json(out / 'autosym_receipts.json', {'tool': TOOL_VERSION, 'oracle_sha256': oracle,
                                               'receipts': result['receipts']})
    write_json(out / 'autosym_references.json', {'tool': TOOL_VERSION, 'references': result['references']})
    write_json(out / 'autosym_unprovable.json',
               {'tool': TOOL_VERSION,
                'references': [r for r in result['references'] if r['status'] == 'not-provable']})
    write_json(out / 'autosym_audit.json', {'broken_existing': result['broken_existing'],
                                            'findings': result['audit']})
    cov = coverage(result)
    cov['inputs'] = {path: sha((ROOT / path).read_bytes()) for path in INPUT_FILES}
    cov['oracle_sha256'] = oracle
    write_json(out / 'autosym_coverage.json', cov)
    return cov


def merge_layouts(src_layout, proposals, dest_layout):
    """Write merged layout copies (never canonical files)."""
    from common import write_json
    dest_layout = Path(dest_layout)
    # Never the canonical checkout (the one with a .git directory).
    require(not (dest_layout.resolve().parent / '.git').exists(),
            'Refusing to write a canonical layout directory')
    code = read_json(Path(src_layout) / 'code-symbols.json')
    data = read_json(Path(src_layout) / 'data-symbols.json')
    add_code = read_json(Path(proposals) / 'autosym_code_symbols.json')
    add_data = read_json(Path(proposals) / 'autosym_data_symbols.json')
    extents = read_json(Path(proposals) / 'autosym_data_extent_additions.json')
    require(add_code['oracle_sha256'] == code['oracle_sha256'] == data['oracle_sha256'],
            'Proposal oracle differs')
    require(not set(add_code['symbols']) & set(code['symbols']), 'Code alias name collision')
    require(not set(add_data['symbols']) & set(data['symbols']), 'Data alias name collision')
    code['symbols'].update(add_code['symbols'])
    data['symbols'].update(add_data['symbols'])
    for name, extent in extents['extent_additions'].items():
        require(name in data['symbols'] and 'width' not in data['symbols'][name],
                'Extent addition target differs')
        data['symbols'][name].update(extent)
    write_json(dest_layout / 'code-symbols.json', code)
    write_json(dest_layout / 'data-symbols.json', data)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = parser.add_subparsers(dest='command', required=True)
    d = sub.add_parser('derive', help='derive proposals from the oracle and existing layout')
    d.add_argument('--out', type=Path, required=True)
    d.add_argument('--spelling', choices=('neutral', 'neutral-all', 'label'), default='neutral',
                   help='neutral: address/stable-id names wherever the current resolver admits '
                        'them (data reviewed forms keep the resolver-required pinned label); '
                        'neutral-all: address names everywhere (needs resolver_neutral_alias.diff); '
                        'label: inventory/pinned-label spellings')
    m = sub.add_parser('merge', help='merge proposals into COPIES of the layout files')
    m.add_argument('--proposals', type=Path, required=True)
    m.add_argument('--layout', type=Path, default=ROOT / 'layout')
    m.add_argument('--into', type=Path, required=True)
    args = parser.parse_args()
    if args.command == 'derive':
        ctx = load_context()
        result = derive(ctx, args.spelling)
        cov = write_outputs(ctx, result, args.out)
        print(json.dumps(cov, indent=2))
    else:
        merge_layouts(args.layout, args.proposals, args.into)
        print('merged into', args.into)


if __name__ == '__main__':
    main()
