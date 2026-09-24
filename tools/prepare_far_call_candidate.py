"""Prepare a reviewed direct-flow far-CALL candidate from fresh complete OMF.

This is a narrow supervisor preparation path for the already verified
external-far-call-v1 binder. It writes a recipe and CFG overlay only when fresh
compiler output binds byte-for-byte to the immutable original contribution.
FAST, whole-image acceptance and promotion still belong to grind.py.
"""
import argparse
import sys

from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from common import ROOT, identity, project_path, read_json, require, sha, write_json
from compiler import compile_source
from mz import MZ
from oracle import verify
from workflow import state

sys.path.insert(0, str(ROOT/'build/python'))
from capstone import CS_ARCH_X86, CS_GRP_CALL, CS_GRP_JUMP, CS_GRP_RET, CS_MODE_16, Cs


def straight_line_overlay(function, image, fixups, binding_mode='external-far-call-v1'):
    start, end = function['start'], function['end']
    raw = image[start:end]
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    decoder.detail = True
    instructions = list(decoder.disasm(raw, start))
    require(instructions and b''.join(bytes(i.bytes) for i in instructions) == raw,
            'Incomplete pristine instruction decode')
    returns = [i for i in instructions if bytes(i.bytes)[0] == 0xcb]
    require(len(returns) == 1, 'Requires one ordinary far RET')
    ret = returns[0]
    require(all(bytes(i.bytes) == b'\x90' for i in instructions if i.address > ret.address),
            'Unowned bytes after far RET')
    require(all(not i.group(CS_GRP_CALL) or bytes(i.bytes)[0] == 0x9a
                for i in instructions), 'Near or indirect call needs manual CFG review')
    require(all(not i.group(CS_GRP_JUMP) or bytes(i.bytes)[0] == 0xeb
                or 0x70 <= bytes(i.bytes)[0] <= 0x7f
                or bytes(i.bytes)[0] == 0xe9 for i in instructions),
            'Indirect or unsupported branch needs manual CFG review')
    by = {i.address: bytes(i.bytes) for i in instructions}
    pending = [start]
    reached = set()
    while pending:
        at = pending.pop()
        if at in reached:
            continue
        require(at in by, 'Pristine branch/fallthrough leaves instruction boundaries')
        reached.add(at)
        ins = by[at]
        nxt = at + len(ins)
        if ins[0] == 0xcb:
            continue
        if ins[0] == 0xeb or 0x70 <= ins[0] <= 0x7f or ins[0] == 0xe9:
            pending.append(nxt + int.from_bytes(ins[1:], 'little', signed=True))
            if ins[0] in (0xeb, 0xe9):
                continue
        require(not (ins[0] == 0xff and (ins[1] >> 3) & 7 != 6),
                'Indirect control flow needs manual CFG review')
        pending.append(nxt)
    far_calls = [i for i in instructions if bytes(i.bytes)[0] == 0x9a]
    require(len(far_calls) == len(fixups) and all(i.size == 5 for i in far_calls),
            'Every compiler fixup must correspond to a pristine far CALL')
    require(sorted(i.address-start+1 for i in far_calls) == sorted(f['offset'] for f in fixups),
            'Pristine far CALL sites differ from compiler fixups')
    padding = [i.address for i in instructions if i.address not in reached]
    require(all(by[p] == b'\x90' for p in padding), 'Unowned unreachable instruction bytes')
    require(ret.address in reached, 'Far RET is unreachable')
    return {'name': function['name'], 'stable_id': function['stable_id'],
            'start': start, 'end': end, 'size': function['size'], 'sha256': function['sha256'],
            'relocation_sites': function['relocation_sites'],
            'disassembly': [{'load_offset': i.address, 'bytes': bytes(i.bytes).hex(),
                             'instruction': (i.mnemonic+' '+i.op_str).strip()}
                            for i in instructions],
            'reachable_instruction_offsets': sorted(reached),
            'padding_offsets': padding,
            'boundary_anchors': [
                {'site': start-4, 'hex': image[start-4:start+4].hex()},
                {'site': end-4, 'hex': image[end-4:end+4].hex()}],
            'binding_review': binding_mode,
            'evidence_class': 'pristine mapped interval, bounded direct CFG, relocated far CALL and reviewed address aliases',
            'boundary_proof': 'Existing uniquely decoded source-label interval; original adjacent bytes and complete pristine CFG rechecked by function_evidence.py'}


def run(stable_id, source_arg, binding_mode='external-far-call-v1'):
    require(binding_mode in ('external-far-call-v1','external-far-call-dgroup-offset16-v1',
                             'external-frame-callback-v1'),
            'Unsupported preparation binding mode')
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    found = [f for f in inventory['functions'] if f.get('stable_id') == stable_id
             and f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    require(len(found) == 1, 'Requires one verified current function extent')
    function = found[0]
    require(not state(function['name'], [function['start'], function['end']])['blocked'],
            'Function interval is blocked')
    source = project_path(source_arg)
    require(source.is_file() and source.is_relative_to(ROOT/'recovery/candidates'),
            'Candidate C must exist under recovery/candidates')
    recipe_path = ROOT/'recipes'/(function['name']+'.json')
    require(not recipe_path.exists(), 'Recipe already exists; manual review required')
    owners = read_json(ROOT/'layout/manifest.json')['owners']
    require(any(o['kind'] == 'UNRESOLVED_RAW' and o['start'] <= function['start']
                and function['end'] <= o['end'] for o in owners), 'Target is not wholly raw-owned')
    _, unpacked, oracle_report, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(sha(image[function['start']:function['end']]) == function['sha256'],
            'Original target extent changed')
    relocations = oracle_report['unpacked_mz']['relocations']
    original_relocations = [r for r in relocations
                            if function['start'] <= r['load_offset'] < function['end']]
    require(original_relocations and sorted(r['load_offset'] for r in original_relocations)
            == function['relocation_sites'], 'Original MZ relocation sites changed')
    obj, compiler_receipt = compile_source(source.read_bytes(), 'msc510-medium')
    public = '_'+function['name']
    declarations = {'segments': obj.segment_defs, 'groups': obj.groups,
                    'publics': obj.publics, 'externals': obj.externals}
    recipe = {'id': function['name'], 'stable_id': stable_id,
              'start': function['start'], 'end': function['end'],
              'source': source.relative_to(ROOT).as_posix(), 'profile': 'msc510-medium',
              'object_segment': 'UNIT_TEXT', 'public': public,
              'target': identity(image[function['start']:function['end']]),
              'expected_fixups': obj.linker_fixups,
              'expected_relocations': original_relocations,
              'binding': {'mode': binding_mode, 'declarations': declarations},
              'evidence': function['provenance']}
    symbols = resolve_recipe_symbols(recipe, image, relocations)
    bound, binding_receipt = bind_contribution(obj, recipe, symbols)
    require(bound == image[function['start']:function['end']],
            'Fresh fully bound contribution differs from immutable original')
    require(function['start'] >= 4 and function['end']+4 <= len(image),
            'Boundary anchor outside pristine image')
    overlay = straight_line_overlay(function, image,
                                    [f for f in obj.linker_fixups if f['loc']=='pointer32'],
                                    binding_mode)
    evidence_path = ROOT/'layout/function-evidence.json'
    evidence = read_json(evidence_path)
    require(evidence['oracle_sha256'] == sha(image), 'Reviewed function evidence oracle changed')
    require(all(row['name'] != function['name'] for row in evidence['functions']),
            'Existing function overlay needs manual review')
    updated = {**evidence, 'functions': [*evidence['functions'], overlay]}
    try:
        write_json(evidence_path, updated)
        write_json(recipe_path, recipe)
        from function_evidence import reviewed_functions
        require(function['name'] in reviewed_functions(image), 'New CFG overlay failed its independent checker')
    except Exception:
        write_json(evidence_path, evidence)
        recipe_path.unlink(missing_ok=True)
        raise
    print({'prepared': function['name'], 'start': function['start'], 'end': function['end'],
           'fixups': len(obj.linker_fixups), 'source': recipe['source'],
           'compiler_object': compiler_receipt['object'], 'binding': binding_receipt['mode'],
           'strict_acceptance': 'NOT_EVALUATED; run factory refresh and grind.py attempt'})
    return recipe


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stable_id')
    parser.add_argument('--source', required=True)
    parser.add_argument('--binding', choices=('external-far-call-v1','external-far-call-dgroup-offset16-v1',
                                               'external-frame-callback-v1'),
                        default='external-far-call-v1')
    args = parser.parse_args()
    run(args.stable_id, args.source, args.binding)
