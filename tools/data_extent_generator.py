"""Conservative array extents derived from verified original instructions.

Only a proved guard on the exact scaled index grants a range.  Unknown index
flows and interior independently anchored symbols fail closed.
"""
import argparse
import hashlib
import json
import sys

from common import ROOT, read_json, require


def _audio_buffer_extent(image, layout, inventory):
    """Corroborate the ring buffer from its complete index write set and source span."""
    from common import identity, sha
    import re
    name = '_unk_44F4C'
    symbol = layout['symbols'].get(name)
    if symbol is None:
        return None
    frame = layout['frame_load_address']; base = frame + 0x97dc
    require(symbol['load_address'] == base and symbol['storage'] == 'bss',
            'Audio buffer base differs')
    functions = {f['name']: f for f in inventory['functions']}
    for function, start, end in [('audio_carstate', 0x88a4, 0x8cd8),
                                  ('setup_player_cars', 0x13702, 0x139b4),
                                  ('frame_callback', 0x12596, 0x12698)]:
        f = functions[function]
        require((f['start'], f['end']) == (start, end) and
                sha(image[f['start']:f['end']]) == f['sha256'] and
                f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                'Audio index caller identity differs')
    anchors = [(0x8a82,'b82200'), (0x8a85,'f72e7492'),
               (0x8a89,'05dc97'), (0x8a8f,'8bd8'), (0x8a97,'894706'),
               (0x8cbb,'ff067492'), (0x8cbf,'833e749228'),
               (0x8cc4,'7506'), (0x8cc6,'c70674920000'),
               (0x13876,'c70674920000')]
    require(all(image[at:at+len(bytes.fromhex(raw))] == bytes.fromhex(raw)
                for at, raw in anchors) and
            0x8cc4 + 2 + int.from_bytes(image[0x8cc5:0x8cc6], 'little', signed=True) == 0x8ccc,
            'Audio ring stride/access/wrap changed')
    # The complete original image contains seven direct references to this
    # index. All are in verified functions. Its only writes are zero, increment,
    # and the guarded zero immediately after the increment reaches 40.
    sites = [at for at in range(len(image)-1) if image[at:at+2] == b'\x74\x92']
    expected = [0x88be,0x8a87,0x8cbd,0x8cc1,0x8cc8,0x125c8,0x13878]
    require(sites == expected, 'Audio ring index has an unreviewed reference/write')
    references = [(0x88bd,'a17492'), (0x8a85,'f72e7492'),
                  (0x8cbb,'ff067492'), (0x8cbf,'833e749228'),
                  (0x8cc6,'c70674920000'), (0x125c7,'a17492'),
                  (0x13876,'c70674920000')]
    require(all(image[at:at+len(bytes.fromhex(raw))] == bytes.fromhex(raw)
                for at,raw in references),
            'Audio ring index has an unguarded write or altered read')
    writes = [{'site':0x8cbb,'kind':'increment_then_wrap'},
              {'site':0x8cc6,'kind':'constant_zero'},
              {'site':0x13876,'kind':'constant_zero'}]
    require({w['site'] for w in writes} == {0x8cbb,0x8cc6,0x13876} and
            image[0x8cbf:0x8cc4] == bytes.fromhex('833e749228'),
            'Audio ring index has an unguarded write')
    path = 'src/restunts/asmorig/dseg.asm'
    source = ROOT/'build/references/restunts'/path
    pinned = read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    require(identity(source.read_bytes()) == pinned, 'Audio buffer reference source differs')
    lines = source.read_text(encoding='latin1').splitlines()
    require(re.fullmatch(r'unk_44F4C\s+db\s+0', lines[36686].strip(), re.I) and
            re.fullmatch(r'dastbmp_y2\s+dw\s+0', lines[38046].strip(), re.I) and
            all(re.fullmatch(r'db\s+0', row.strip(), re.I) for row in lines[36687:38046]),
            'Audio buffer reference label span differs')
    span = 38047-36687
    require(0x22*0x28 == span == 1360 and
            symbol.get('width') in (None, span) and base+span <= layout['bss_end'],
            'Audio buffer code and reference extents disagree')
    for other_name, other in layout['symbols'].items():
        if other_name == name or other['storage'] == 'code_island':
            continue
        other_base = other['load_address']; other_end = other_base + other.get('width', 1)
        require(not (base < other_end and other_base < base+span),
                'Audio buffer overlaps independently anchored object')
    return {'load_address': base, 'storage': 'bss', 'width': span,
            'extent_proof': {'kind': 'corroborated-ring-buffer-v1',
                             'stride': 34, 'wrap_count': 40,
                             'index_displacement': 0x9274,
                             'reference_path': path,
                             'reference_lines': [36687,38047],
                             'anchors': [{'site':at,'hex':raw} for at,raw in anchors],
                             'index_references': [{'site':at,'hex':raw} for at,raw in references],
                             'all_index_reference_sites': sites,
                             'writes': writes}}


def _decoder():
    location = str(ROOT / 'build/python')
    if location not in sys.path:
        sys.path.insert(0, location)
    import capstone
    require(capstone.__version__ == '5.0.3', 'Pinned instruction decoder differs')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    return decoder, capstone


def _memory_key(op, ins, capstone):
    if op.type != capstone.x86.X86_OP_MEM:
        return None
    return (ins.reg_name(op.mem.segment), ins.reg_name(op.mem.base),
            ins.reg_name(op.mem.index), op.mem.disp)


def _edges(insns, capstone):
    by_address = {ins.address: ins for ins in insns}
    edges = {}
    for ins in insns:
        following = ins.address + ins.size
        if ins.mnemonic in ('ret', 'retf', 'iret'):
            destinations = []
        elif ins.mnemonic.startswith('j') and ins.operands and ins.operands[0].type == capstone.x86.X86_OP_IMM:
            target = ins.operands[0].imm
            destinations = [target] if ins.mnemonic == 'jmp' else [following, target]
        else:
            destinations = [following]
        edges[ins.address] = [at for at in destinations if at in by_address]
    return edges


def _reaches(edges, source, target, blocked=None):
    todo, seen = [source], set()
    while todo:
        at = todo.pop()
        if at == target:
            return True
        if at == blocked or at in seen:
            continue
        seen.add(at)
        todo.extend(edges.get(at, ()))
    return False


def _guard(insns, edges, scale_at, access_at, index_key, capstone):
    """Return inclusive bounds from guards whose bad edge bypasses access."""
    lower, upper, anchors = None, None, []
    positions = {ins.address: n for n, ins in enumerate(insns)}
    index = positions[scale_at]
    for n in range(max(0, index-24), index):
        cmp_ins = insns[n]
        if cmp_ins.mnemonic != 'cmp' or len(cmp_ins.operands) != 2:
            continue
        left, right = cmp_ins.operands
        if right.type != capstone.x86.X86_OP_IMM or _memory_key(left, cmp_ins, capstone) != index_key:
            continue
        if n+1 >= index:
            continue
        jump = insns[n+1]
        if jump.mnemonic not in ('jl', 'jnge', 'jb', 'jnae', 'jle', 'jng', 'jbe', 'jna',
                                 'jg', 'jnle', 'ja', 'jnbe', 'jge', 'jnl', 'jae', 'jnb'):
            continue
        if not jump.operands or jump.operands[0].type != capstone.x86.X86_OP_IMM:
            continue
        bad = jump.operands[0].imm
        if _reaches(edges, bad, access_at) or not _reaches(edges, jump.address+jump.size, access_at):
            continue
        if (_reaches(edges, insns[0].address, scale_at, blocked=jump.address) or
                _reaches(edges, insns[0].address, jump.address, blocked=cmp_ins.address)):
            continue
        value = right.imm
        if jump.mnemonic in ('jl', 'jnge', 'jb', 'jnae', 'jle', 'jng', 'jbe', 'jna'):
            first = value+(jump.mnemonic in ('jle', 'jng', 'jbe', 'jna'))
            lower = first if lower is None else max(lower, first)
        else:
            last = value-(jump.mnemonic in ('jge', 'jnl', 'jae', 'jnb'))
            upper = last if upper is None else min(upper, last)
        anchors.extend(({'site': cmp_ins.address, 'hex': cmp_ins.bytes.hex()},
                        {'site': jump.address, 'hex': jump.bytes.hex()}))
    if lower is None and upper is not None:
        # Simple counted loops: a dominating immediate initialization and
        # positive increments are the only writes to the local index.
        candidates = []
        for n, ins in enumerate(insns[:index]):
            if (ins.mnemonic == 'mov' and len(ins.operands) == 2 and
                    _memory_key(ins.operands[0], ins, capstone) == index_key and
                    ins.operands[1].type == capstone.x86.X86_OP_IMM and
                    not _reaches(edges, insns[0].address, scale_at, blocked=ins.address)):
                candidates.append((n, ins))
        if len(candidates) == 1 and index_key[1] == 'bp':
            init_n, init = candidates[0]
            writes = []
            for ins in insns[init_n+1:]:
                if (ins.mnemonic in ('mov', 'inc', 'dec', 'add', 'sub', 'and', 'or', 'xor') and
                        ins.operands and _memory_key(ins.operands[0], ins, capstone) == index_key):
                    writes.append(ins)
            if (writes and all(ins.mnemonic == 'inc' for ins in writes) and
                    all(ins.address > access_at for ins in writes) and
                    not any(ins.mnemonic.startswith('call') or ins.mnemonic == 'lcall'
                            for ins in insns[init_n+1:positions[access_at]])):
                lower = init.operands[1].imm
                anchors.insert(0, {'site': init.address, 'hex': init.bytes.hex()})
                anchors.extend({'site': ins.address, 'hex': ins.bytes.hex()} for ins in writes)
    if lower is None or upper is None or lower < 0 or upper < lower:
        return None
    # A call or a store to the index between a guard and the scale invalidates
    # this deliberately narrow straight-line proof.
    first = min(a['site'] for a in anchors)
    for ins in insns[positions[first]:index]:
        if ins.mnemonic.startswith('j') and not any(a['site'] == ins.address for a in anchors):
            return None
        if ins.mnemonic.startswith('call') or ins.mnemonic in ('lcall', 'int'):
            return None
        if (ins.mnemonic not in ('cmp',) and ins.operands and
                _memory_key(ins.operands[0], ins, capstone) == index_key and
                not (ins.mnemonic == 'mov' and any(a['site'] == ins.address for a in anchors))):
            return None
    return lower, upper, anchors


def _scaled_accesses(insns, edges, capstone):
    """Recognize MSC's constant-in-AX IMUL/MUL and register shift idioms."""
    for n, scale in enumerate(insns):
        stride = None
        index_key = None
        reg = 'ax'
        start = n
        if (scale.mnemonic in ('imul', 'mul') and len(scale.operands) == 1 and n and
                insns[n-1].mnemonic == 'mov' and len(insns[n-1].operands) == 2 and
                insns[n-1].operands[0].type == capstone.x86.X86_OP_REG and
                insns[n-1].reg_name(insns[n-1].operands[0].reg) == 'ax' and
                insns[n-1].operands[1].type == capstone.x86.X86_OP_IMM):
            stride = insns[n-1].operands[1].imm
            index_key = _memory_key(scale.operands[0], scale, capstone)
            start = n-1
        elif (scale.mnemonic == 'shl' and len(scale.operands) == 2 and
                scale.operands[1].type == capstone.x86.X86_OP_IMM and n and
                insns[n-1].mnemonic == 'mov' and len(insns[n-1].operands) == 2 and
                insns[n-1].operands[0].type == capstone.x86.X86_OP_REG):
            reg = scale.reg_name(scale.operands[0].reg)
            if reg == insns[n-1].reg_name(insns[n-1].operands[0].reg):
                stride = 1 << scale.operands[1].imm
                index_key = _memory_key(insns[n-1].operands[1], insns[n-1], capstone)
                start = n-1
        if not index_key or stride is None or stride <= 1 or stride > 4096:
            continue
        index_at = insns[start].address
        scale_anchors = [{'site': ins.address, 'hex': ins.bytes.hex()} for ins in insns[start:n+1]]
        # Follow only simple register transfers to an indexed DS operand.
        current = reg
        for access in insns[n+1:n+9]:
            if access.mnemonic.startswith('j') or access.mnemonic.startswith('call') or access.mnemonic in ('lcall', 'ret', 'retf'):
                break
            if (access.mnemonic == 'mov' and len(access.operands) == 2 and
                    access.operands[0].type == capstone.x86.X86_OP_REG and
                    access.operands[1].type == capstone.x86.X86_OP_REG and
                    access.reg_name(access.operands[1].reg) == current):
                current = access.reg_name(access.operands[0].reg)
                scale_anchors.append({'site': access.address, 'hex': access.bytes.hex()})
                continue
            for op in access.operands:
                key = _memory_key(op, access, capstone)
                if not key or key[1] != current or key[2] or key[0] in ('cs', 'es', 'ss'):
                    continue
                bound = _guard(insns, edges, index_at, access.address, index_key, capstone)
                if bound:
                    lo, hi, guards = bound
                    yield {'source_base': key[3] & 0xffff, 'stride': stride,
                           'index_bound': [lo, hi],
                           'anchors': guards + scale_anchors +
                           [{'site': access.address, 'hex': access.bytes.hex()}],
                           'access': access.address}
            # A changed scaled register makes later accesses unrelated.
            if (access.operands and access.operands[0].type == capstone.x86.X86_OP_REG and
                    access.reg_name(access.operands[0].reg) == current):
                break


def derive(image, relocations, layout=None, inventory=None):
    """Return accepted generated entries and rejected/conflicting observations."""
    layout = layout or read_json(ROOT/'layout/data-symbols.json')
    inventory = inventory or read_json(ROOT/'evidence/functions.json')
    require(hashlib.sha256(image).hexdigest() == layout['oracle_sha256'], 'Extent oracle differs')
    decoder, capstone = _decoder()
    relocated = {r['load_offset'] for r in relocations}
    witnesses = []
    indexed_sites = []
    for function in inventory['functions']:
        if function.get('status') != 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
            continue
        start, end = function['start'], function['end']
        body = image[start:end]
        require(hashlib.sha256(body).hexdigest() == function['sha256'], 'Extent caller differs')
        insns = list(decoder.disasm(body, start))
        if sum(ins.size for ins in insns) != len(body):
            continue
        edges = _edges(insns, capstone)
        for ins in insns:
            for op in ins.operands:
                key = _memory_key(op, ins, capstone)
                if key and key[1] in ('bx', 'si', 'di') and key[0] not in ('cs', 'es', 'ss') and ins.disp_size == 2:
                    if not any(ins.address+ins.disp_offset+k in relocated for k in (0, 1)):
                        indexed_sites.append((ins.address, key[3] & 0xffff))
        for observation in _scaled_accesses(insns, edges, capstone):
            access = insns[next(n for n, ins in enumerate(insns) if ins.address == observation['access'])]
            if access.disp_size != 2 or any(access.address+access.disp_offset+k in relocated for k in (0, 1)):
                continue
            observation['function'] = function['name']
            observation['start'] = start
            observation['end'] = end
            observation['sha256'] = function['sha256']
            witnesses.append(observation)
    frame = layout['frame_load_address']
    by_base = {}
    for witness in witnesses:
        lo, hi = witness['index_bound']
        first = frame + witness['source_base'] + lo*witness['stride']
        if first >= frame+65536:
            continue
        aliases = [(name, symbol) for name, symbol in layout['symbols'].items()
                   if symbol['storage'] != 'code_island' and
                   0 <= first-symbol['load_address'] < witness['stride']]
        if not aliases:
            continue
        aliases.sort(key=lambda pair: pair[1]['load_address'])
        base = aliases[0][1]['load_address']
        field = first-base
        if field >= witness['stride']:
            continue
        witness['field_offset'] = field
        by_base.setdefault(base, []).append(witness)
    derived, rejected = {}, []
    for base, rows in sorted(by_base.items()):
        strides = {w['stride'] for w in rows}
        origins = {w['source_base']-w['field_offset'] for w in rows}
        if len(strides) != 1 or len(origins) != 1:
            rejected.append({'base': base, 'reason': 'conflicting bounds or strides'})
            continue
        stride = strides.pop()
        lo = min(w['index_bound'][0] for w in rows)
        hi = max(w['index_bound'][1] for w in rows)
        if base != frame + origins.pop() + lo*stride:
            rejected.append({'base': base, 'reason': 'unproved leading elements'})
            continue
        end = base+(hi-lo+1)*stride
        names = [name for name, symbol in layout['symbols'].items() if symbol['load_address'] == base and symbol['storage'] != 'code_island']
        if not names:
            rejected.append({'base': base, 'reason': 'no independently anchored base alias'})
            continue
        name = next((n for n in names if layout['symbols'][n].get('width') == end-base), names[0])
        symbol = layout['symbols'][name]
        cap = layout['bss_end'] if symbol['storage'] == 'bss' else layout['bss_start']
        if end > cap or base < (layout['bss_start'] if symbol['storage'] == 'bss' else frame):
            rejected.append({'base': base, 'reason': 'storage boundary'})
            continue
        conflicts = [(n, s) for n, s in layout['symbols'].items() if n != name and s['storage'] != 'code_island' and
                     ((s.get('width') and base < s['load_address']+s['width'] and s['load_address'] < end) or
                      (base < s['load_address'] < end))]
        if conflicts:
            rejected.append({'base': base, 'reason': 'independent interior anchor', 'symbols': [n for n, _ in conflicts]})
            continue
        if symbol.get('width') is not None and symbol['width'] != end-base:
            rejected.append({'base': base, 'reason': 'reviewed width conflict'})
            continue
        source_base = rows[0]['source_base']-rows[0]['field_offset']
        # An unbounded use of a different static base does not itself prove
        # that it reaches this slice.  Any indexed displacement *inside* the
        # slice, however, must have its own bounded witness.
        indexed = {at for at, disp in indexed_sites if base-frame <= disp < end-frame}
        proved = {w['access'] for w in rows}
        if indexed-proved:
            rejected.append({'base': base, 'reason': 'unbounded indexed access',
                             'sites': sorted(indexed-proved)})
            continue
        rows.sort(key=lambda w: (w['start'], w['access']))
        derived[name] = {'load_address': base, 'storage': symbol['storage'], 'width': end-base,
                         'extent_proof': {'kind': 'generated-indexed-v1',
                                          'source_base': source_base,
                                          'stride': stride,
                                          'index_bound': [lo, hi],
                                          'field_offsets': sorted({w['field_offset'] for w in rows}),
                                          'anchors': [{'function': w['function'], 'start': w['start'],
                                                       'access': w['access']} for w in rows]}}
    if '_unk_44F4C' in layout['symbols']:
        derived['_unk_44F4C'] = _audio_buffer_extent(image, layout, inventory)
    return derived, rejected, witnesses


def main():
    from oracle import verify
    from mz import MZ
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', help='Write detailed scan to this path (under build/)')
    args = parser.parse_args()
    _, unpacked, report, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    derived, rejected, witnesses = derive(image, report['unpacked_mz']['relocations'])
    result = {'derived': derived, 'rejected': rejected, 'bounded_accesses': witnesses}
    if args.output:
        path = (ROOT / args.output).resolve()
        require(path.is_relative_to((ROOT/'build').resolve()), 'Output must be under build/')
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({'derived': len(derived), 'rejected': len(rejected),
                      'bounded_accesses': len(witnesses), 'names': sorted(derived)}, indent=2))


if __name__ == '__main__':
    main()
