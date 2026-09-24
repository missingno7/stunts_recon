"""Report mapped intervals with code unreachable from their named entry.

This is a review aid, not a boundary correction or acceptance authority. An
unreachable span may be another entry, inline data, or a decode ambiguity.
"""

import argparse
import hashlib
import json
import sys

from common import ROOT, read_json
from mz import MZ
from oracle import verify

sys.path.insert(0, str(ROOT / 'build/python'))
from capstone import CS_ARCH_X86, CS_GRP_JUMP, CS_GRP_RET, CS_MODE_16, Cs
from capstone.x86_const import X86_OP_IMM


def inspect(function, image, decoder):
    start, end = function['start'], function['end']
    if hashlib.sha256(image[start:end]).hexdigest() != function['sha256']:
        raise ValueError('Inventory extent hash differs from pristine image: ' + function['name'])
    instructions = list(decoder.disasm(image[start:end], start))
    if not instructions or instructions[0].address != start or instructions[-1].address + instructions[-1].size != end:
        return {'kind': 'incomplete_linear_decode'}
    by_address = {instruction.address: instruction for instruction in instructions}
    pending, reached, uncertainties = [start], set(), set()
    while pending:
        address = pending.pop()
        if address in reached:
            continue
        instruction = by_address.get(address)
        if instruction is None:
            uncertainties.add('flow_to_non_instruction_boundary')
            continue
        reached.add(address)
        following = address + instruction.size
        if instruction.group(CS_GRP_RET):
            continue
        if instruction.group(CS_GRP_JUMP):
            if not instruction.operands or instruction.operands[0].type != X86_OP_IMM:
                uncertainties.add('indirect_jump')
                continue
            target = instruction.operands[0].imm
            if start <= target < end:
                pending.append(target)
            else:
                uncertainties.add('jump_outside_interval')
            if instruction.mnemonic == 'jmp':
                continue
        if following < end:
            pending.append(following)
        elif following > end:
            uncertainties.add('fallthrough_past_interval')
        elif not instruction.group(CS_GRP_RET):
            uncertainties.add('fallthrough_at_interval_end')
    unexplained = [instruction for instruction in instructions
                   if instruction.address not in reached and instruction.bytes != b'\x90']
    if not unexplained:
        return None
    first = unexplained[0].address
    return {'kind': 'unreachable_nonpadding_from_named_entry',
            'first_offset': first - start,
            'first_load_address': first,
            'first_bytes': image[first:min(first + 12, end)].hex(),
            'instruction_count': len(unexplained),
            'byte_count': sum(instruction.size for instruction in unexplained),
            'uncertainties': sorted(uncertainties)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--function', help='Exact mapped function name or stable ID')
    args = parser.parse_args()
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    inventory = read_json(ROOT / 'recovery/restunts-inventory.json')
    functions = [function for function in inventory['functions']
                 if function['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
                 and (args.function is None or args.function in (function['name'], function['stable_id']))]
    if args.function and not functions:
        parser.error('No fully mapped function has that name or stable ID')
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    decoder.detail = True
    findings = []
    for function in functions:
        finding = inspect(function, image, decoder)
        if finding:
            findings.append({'name': function['name'], 'stable_id': function['stable_id'],
                             'start': function['start'], 'end': function['end'], **finding})
    print(json.dumps({'schema': 1, 'oracle_sha256': hashlib.sha256(image).hexdigest(),
                      'fully_mapped_scanned': len(functions), 'findings': findings,
                      'scope': 'Single-entry control-flow review aid. Absence of a finding is not boundary proof; external entries and embedded data need review. No boundary, source, or ownership changes.'},
                     indent=2))


if __name__ == '__main__':
    main()
