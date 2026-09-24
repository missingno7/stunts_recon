"""Propose exact-address binding aliases from independent pristine far CALLs.

An alias names an address for candidate compiler fixups. It does not assert an
original PUBDEF spelling, object membership, source authorship, or C ownership.
`--write` adds only mechanically checked entries to the static reviewed registry;
the resolver rechecks every byte, relocation, caller and raw owner when used.
"""
import argparse
import collections
import re
import sys

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from oracle import verify

sys.path.insert(0, str(ROOT/'build/python'))
from capstone import CS_ARCH_X86, CS_MODE_16, Cs


def candidate_aliases(inventory, owners, image, relocations):
    verified = [f for f in inventory['functions']
                if f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    targets = collections.defaultdict(list)
    for f in verified:
        if not re.fullmatch(r'[A-Za-z][A-Za-z0-9_]*', f['name']):
            continue
        if not any((o['kind'] == 'UNRESOLVED_RAW' and o['start'] <= f['start']
                    and f['end'] <= o['end']) or
                   (o['kind'] == 'MATCHING_C' and o.get('name') == f['name']
                    and o['start'] == f['start'] and o['end'] == f['end'])
                   for o in owners):
            continue
        require(sha(image[f['start']:f['end']]) == f['sha256'],
                'Mapped target changed: '+f['name'])
        targets[f['start']].append(f)
    targets = {at: functions[0] for at, functions in targets.items() if len(functions) == 1}
    relocation_by_site = {r['load_offset']: r for r in relocations}
    require(len(relocation_by_site) == len(relocations), 'Duplicate MZ relocation sites')
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    observed = collections.defaultdict(list)
    for caller in verified:
        raw = image[caller['start']:caller['end']]
        if sha(raw) != caller['sha256']:
            raise ValueError('Mapped caller changed: '+caller['name'])
        instructions = list(decoder.disasm(raw, caller['start']))
        if not instructions or b''.join(bytes(i.bytes) for i in instructions) != raw:
            continue
        for instruction in instructions:
            encoded = bytes(instruction.bytes)
            if len(encoded) != 5 or encoded[0] != 0x9a:
                continue
            relocation = relocation_by_site.get(instruction.address+3)
            if relocation is None:
                continue
            frame = int.from_bytes(encoded[3:5], 'little')*16
            address = frame+int.from_bytes(encoded[1:3], 'little')
            if address not in targets or targets[address]['stable_id'] == caller['stable_id']:
                continue
            observed[address].append({'caller_task': caller['stable_id'],
                                      'site': instruction.address, 'hex': encoded.hex(),
                                      'relocation': relocation, 'frame': frame})
    proposals = {}
    for address, calls in sorted(observed.items()):
        by_frame = collections.defaultdict(dict)
        for call in calls:
            by_frame[call['frame']].setdefault(call['caller_task'], call)
        compatible = [(frame, anchors) for frame, anchors in by_frame.items() if len(anchors) >= 2]
        if len(compatible) != 1:
            continue
        frame, anchors = compatible[0]
        function = targets[address]
        alias = '_'+function['name']
        require(alias not in proposals, 'Ambiguous binding alias: '+alias)
        chosen = [dict(anchors[caller]) for caller in sorted(anchors)[:2]]
        for anchor in chosen:
            del anchor['frame']
        proposals[alias] = {'mapped_target': {key: function[key] for key in
                            ('stable_id', 'name', 'start', 'end', 'sha256')},
                            'frame_load_address': frame, 'anchors': chosen}
    return proposals


def run(write=False):
    _, unpacked, oracle_report, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    relocations = oracle_report['unpacked_mz']['relocations']
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    owners = read_json(ROOT/'layout/manifest.json')['owners']
    layout_path = ROOT/'layout/code-symbols.json'
    original = read_json(layout_path)
    require(original['oracle_sha256'] == sha(image), 'Code registry oracle changed')
    proposed = candidate_aliases(inventory, owners, image, relocations)
    additions = {name: value for name, value in proposed.items() if name not in original['symbols']}
    require(all('mapped_target' in original['symbols'][name]
                and proposed[name] == original['symbols'][name]
                for name in set(proposed) & set(original['symbols'])),
            'Existing raw code alias differs from independently derived anchors')
    report = {'schema': 1, 'authority': 'BINDING_ADDRESS_CANDIDATES_ONLY',
              'oracle_sha256': sha(image), 'eligible_mapped_aliases': len(proposed),
              'new_aliases': len(additions), 'existing_symbols': len(original['symbols']),
              'candidates': proposed, 'pending_addition_names': sorted(additions),
              'limitation': 'Two relocated mapped callers prove a far address and frame, not the original PUBDEF spelling, original TU, or target source.'}
    write_json(ROOT/'recovery/code-alias-candidates.json', report)
    if write and additions:
        updated = {**original, 'symbols': {**original['symbols'], **additions}}
        write_json(layout_path, updated)
        try:
            from code_symbols import resolve_code_symbols
            resolved = resolve_code_symbols(set(additions), image, relocations)
            require(len(resolved) == len(additions), 'Code alias verification incomplete')
        except Exception:
            write_json(layout_path, original)
            raise
    print({'eligible_mapped_aliases': len(proposed), 'new_aliases': len(additions),
           'written': bool(write and additions)})
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', action='store_true', help='Write strictly verified aliases to layout/code-symbols.json')
    args = parser.parse_args()
    run(args.write)
