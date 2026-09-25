"""Read-only pristine packet for a verified mapped function, including large ones.

This is research evidence, not a reviewed overlay, queue admission, original TU,
binding mode, production recipe, or permission to promote.
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[3]/'tools'))
from common import ROOT, read_json, require, sha
from mz import MZ
from oracle import verify
from triage import capabilities
from workflow import fingerprint, load_snapshot, workflow_inputs

sys.path.insert(0, str(ROOT/'build/python'))
from capstone import CS_ARCH_X86, CS_GRP_JUMP, CS_GRP_RET, CS_MODE_16, Cs
from capstone.x86_const import X86_OP_IMM


def _cfg(instructions, start, end):
    by_address = {i.address: i for i in instructions}
    pending, reached, risks, branches = [start], set(), set(), []
    while pending:
        address = pending.pop()
        if address in reached:
            continue
        instruction = by_address.get(address)
        if instruction is None:
            risks.add('flow_to_non_instruction_boundary')
            continue
        reached.add(address)
        following = address + instruction.size
        if instruction.group(CS_GRP_RET):
            continue
        if instruction.group(CS_GRP_JUMP):
            if not instruction.operands or instruction.operands[0].type != X86_OP_IMM:
                risks.add('indirect_jump')
            else:
                target = instruction.operands[0].imm
                branches.append({'from': address, 'to': target, 'mnemonic': instruction.mnemonic})
                if start <= target < end:
                    pending.append(target)
                else:
                    risks.add('jump_outside_interval')
            if instruction.mnemonic == 'jmp':
                continue
        if following < end:
            pending.append(following)
        elif following > end:
            risks.add('fallthrough_past_interval')
        else:
            risks.add('fallthrough_at_interval_end')
    unreachable = [i for i in instructions if i.address not in reached]
    nonpadding = [i for i in unreachable if bytes(i.bytes) != b'\x90']
    return {'entry': start, 'reachable_instructions': len(reached),
            'unreachable_nonpadding': [{'address': i.address, 'bytes': bytes(i.bytes).hex(),
                                        'instruction': (i.mnemonic+' '+i.op_str).strip()} for i in nonpadding],
            'unreachable_nop_bytes': sum(i.size for i in unreachable if bytes(i.bytes) == b'\x90'),
            'direct_branches': branches, 'flow_risks': sorted(risks),
            'external_ingress': 'UNPROVEN: single-entry traversal cannot exclude other entry points'}


def packet(identifier):
    index = read_json(ROOT/'recovery/context-index.json')
    matches = [row for row in index['tasks'] if identifier in (row['id'], row['name'])]
    require(len(matches) == 1, 'Unknown or ambiguous current task')
    row = matches[0]
    raw = (ROOT/row['card']).read_bytes()
    require(sha(raw) == row['card_sha256'], 'Context index/card changed; refresh queue')
    card = json.loads(raw)
    require(card['id'] == row['id'] and card['name'] == row['name'], 'Task/card identity mismatch')
    current = workflow_inputs()
    require(fingerprint(current) == index['workflow_fingerprint'] and
            load_snapshot(card['workflow_snapshot']) == current,
            'Stale research packet; refresh queue')
    evidence = card['evidence']
    require(evidence['status'] == card['boundary_status'] ==
            'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED', 'Unverified boundary')
    start, end = evidence['start'], evidence['end']
    require(evidence['stable_id'] == row['id'] and evidence['name'] == row['name']
            and end-start == evidence['size'] == card['size'], 'Card extent/identity mismatch')
    _, unpacked, oracle_report, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(sha(image) == index['oracle']['sha256'], 'Context index/oracle changed')
    target = image[start:end]
    require(len(target) == evidence['size'] and sha(target) == evidence['sha256'],
            'Card/pristine extent changed')
    actual_relocations = sorted(r['load_offset'] for r in oracle_report['unpacked_mz']['relocations']
                                if start <= r['load_offset'] < end)
    require(actual_relocations == evidence.get('relocation_sites', []),
            'Card/pristine relocation sites differ')
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    decoder.detail = True
    instructions = list(decoder.disasm(target, start))
    require(instructions and instructions[0].address == start
            and instructions[-1].address + instructions[-1].size == end
            and b''.join(bytes(i.bytes) for i in instructions) == target,
            'Pristine linear decode does not cover complete extent')
    rows = [{'load_offset': i.address, 'bytes': bytes(i.bytes).hex(),
             'instruction': (i.mnemonic+' '+i.op_str).strip()} for i in instructions]
    layout = read_json(ROOT/'layout/data-symbols.json')
    frame = layout['frame_load_address']
    known = {s['load_address']-frame for s in layout['symbols'].values()}
    known.update(s['load_address']-frame+f['offset']
                 for s in layout['symbols'].values() for f in s.get('fields', []))
    blockers, risks = capabilities(evidence, rows, known)
    relocations = actual_relocations
    call_rows = [r for r in rows if r['instruction'].split(' ', 1)[0] in ('call', 'lcall')]
    refs = []
    for r in rows:
        ins = r['instruction'].lower()
        if '[' not in ins:
            continue
        kind = ('segment_override' if re.search(r'\b(?:cs|es|ss):\[', ins) else
                'absolute' if re.search(r'(?<!:)\[0x[0-9a-f]+\]', ins) else
                'indexed_or_field')
        refs.append({**r, 'kind': kind})
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    other_entries = [{'name': f['name'], 'start': f['start']}
                     for f in inventory['functions'] if f.get('start') is not None
                     and start < f['start'] < end]
    return {'schema': 1, 'authority': 'RESEARCH_ONLY', 'task': row['name'], 'id': row['id'],
            'card': row['card'], 'card_sha256': row['card_sha256'],
            'oracle_load_sha256': sha(image), 'extent': {'start': start, 'end': end,
              'size': len(target), 'sha256': sha(target), 'distance': evidence.get('distance'),
              'boundary_status': evidence['status'], 'confidence': evidence.get('confidence')},
            'instruction_count': len(rows), 'disassembly': rows,
            'cfg': _cfg(instructions, start, end), 'other_mapped_entries_inside': other_entries,
            'calls': call_rows, 'mz_relocation_sites': relocations, 'memory_operands': refs,
            'researched_capability_blockers': blockers, 'researched_risks': risks,
            'existing_card_blockers': card['capability_blockers'],
            'abi': evidence.get('abi', 'UNESTABLISHED: inspect parameters, return and calling convention'),
            'semantic_source_hints': evidence.get('c_sources', []),
            'source_authority': 'Restunts paths can be callers; Restunts C is not historical source authority',
            'promotion': 'UNREVIEWED: complete contribution, original TU, fixups, binder and native acceptance remain separate'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('function', help='Current stable ID or exact function name')
    parser.add_argument('--summary', action='store_true', help='Compact result; default emits all instructions and memory operands')
    args = parser.parse_args()
    result = packet(args.function)
    if args.summary:
        result['instruction_count'] = len(result.pop('disassembly'))
        operands = result.pop('memory_operands')
        result['memory_operand_count'] = len(operands)
        result['memory_operand_examples'] = operands[:12]
        result['cfg']['direct_branch_count'] = len(result['cfg'].pop('direct_branches'))
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
