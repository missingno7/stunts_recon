"""Research-only shared-base equations for candidate OMF contributions.

Public and private DATA segment placements are solved separately. A consistent
base is never a byte, extent, fixup, TU, or historical LINK acceptance claim.
"""
from collections import defaultdict
import sys

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from object_probe import read_object
from oracle import verify

sys.path.insert(0, str(ROOT/'build/python'))
from capstone import Cs, CS_ARCH_X86, CS_MODE_16


def solve_shared_base(equations):
    """Return a common base, a minimal conflicting pair, or an abstention."""
    if len(equations) < 2:
        return {'state': 'INSUFFICIENT_INDEPENDENT_ANCHORS', 'witness': []}
    frames = {row.get('frame_paragraph') for row in equations}
    if None in frames:
        return {'state': 'UNKNOWN_FRAME', 'witness': []}
    if len(frames) != 1:
        first = equations[0]
        other = next(row for row in equations[1:]
                     if row['frame_paragraph'] != first['frame_paragraph'])
        return {'state': 'INCONSISTENT_FRAME', 'witness': [first['symbol'], other['symbol']]}
    first = equations[0]
    for other in equations[1:]:
        if other['inferred_base'] != first['inferred_base']:
            return {'state': 'INCONSISTENT_BASE',
                    'witness': [first['symbol'], other['symbol']],
                    'base_difference': other['inferred_base']-first['inferred_base']}
    return {'state': 'CONSISTENT_BASE', 'base': first['inferred_base'],
            'frame_paragraph': first['frame_paragraph'], 'witness': []}


def resolve_public(name, by_name):
    exact = by_name.get(name, [])
    if len(exact) == 1:
        return exact[0], 'EXACT'
    if exact:
        return None
    if name.startswith('_') and not name.startswith('__'):
        alias = by_name.get(name[1:], [])
        if len(alias) == 1:
            return alias[0], 'SINGLE_LEADING_UNDERSCORE_ALIAS'
    return None


def public_rows(census, inventory):
    by_name = defaultdict(list)
    for function in inventory['functions']:
        if function['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
            by_name[function['name']].append(function)
    rows = []
    for candidate in census['objects']:
        if len(candidate['publics']) < 2:
            continue
        grouped = defaultdict(list)
        unresolved = []
        for public in candidate['publics']:
            result = resolve_public(public['name'], by_name)
            if result is None:
                unresolved.append(public['name'])
                continue
            function, match = result
            grouped[public['segment']].append({
                'symbol': public['name'], 'symbol_match': match,
                'candidate_offset': public['offset'],
                'oracle_start': function['start'], 'oracle_end': function['end'],
                'frame_paragraph': function.get('segment_paragraph'),
                'inferred_base': function['start']-public['offset']})
        for segment in sorted({public['segment'] for public in candidate['publics']}):
            equations = grouped[segment]
            solved = solve_shared_base(equations)
            candidate_size = candidate['segment_lengths'].get(segment)
            span = None
            if solved['state'] == 'CONSISTENT_BASE' and candidate_size is not None:
                span = [solved['base'], solved['base']+candidate_size]
            oracle_span = ([min(row['oracle_start'] for row in equations),
                            max(row['oracle_end'] for row in equations)] if equations else None)
            rows.append({'task_id': candidate['task_id'], 'object_sha256': candidate['object_sha256'],
                         'candidate_segment': segment, 'publics_total': sum(
                             public['segment'] == segment for public in candidate['publics']),
                         'matched_publics': equations, 'unresolved_publics': unresolved,
                         'placement': solved, 'candidate_segment_bytes': candidate_size,
                         'candidate_span_if_placed': span,
                         'oracle_anchored_function_span': oracle_span,
                         'extent_equal_to_anchored_span': (span == oracle_span if span and oracle_span else None),
                         'fixup_modes': candidate['fixup_modes'],
                         'acceptance': 'NOT_EVALUATED'})
    return rows


def private_rows(census, inventory, image, relocations, anchors, dgroup):
    by_name = defaultdict(list)
    for function in inventory['functions']:
        if function['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
            by_name[function['name']].append(function)
    require(dgroup['frame_load_address'] % 16 == 0, 'DGROUP frame not paragraph aligned')
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    rows = []
    for candidate in census['objects']:
        for segment, size in candidate['segment_lengths'].items():
            if segment == 'UNIT_TEXT' or size <= 0:
                continue
            declared = [anchor for anchor in anchors['anchors']
                        if anchor['task_id'] == candidate['task_id']
                        and anchor['candidate_segment'] == segment]
            row = {'task_id': candidate['task_id'], 'object_sha256': candidate['object_sha256'],
                   'candidate_segment': segment, 'candidate_segment_bytes': size,
                   'placement': {'state': 'INSUFFICIENT_INDEPENDENT_ANCHORS'},
                   'content_state': 'NOT_TESTED', 'acceptance': 'NOT_EVALUATED'}
            if not declared:
                rows.append(row)
                continue
            path = ROOT/candidate['archive_paths'][0]
            data = path.read_bytes()
            require(sha(data) == candidate['object_sha256'], 'Candidate object identity drift')
            obj = read_object(data, research_local_symbols=True)
            payload = obj.segment_bytes(segment)
            require(len(payload) == size, 'Private initialized segment incomplete')
            equations = []
            for anchor in declared:
                functions = by_name.get(anchor['oracle_function'], [])
                require(len(functions) == 1, 'Private-data reference function not uniquely mapped')
                function = functions[0]
                site = anchor['oracle_instruction_site']
                require(function['start'] <= site and site+3 <= function['end'],
                        'Private-data reference outside mapped function')
                instruction = next((ins for ins in decoder.disasm(
                    image[function['start']:function['end']], function['start'])
                    if ins.address == site), None)
                require(instruction is not None and instruction.size == 3
                        and instruction.mnemonic == 'mov'
                        and instruction.op_str.startswith('ax, ')
                        and image[site:site+1].hex() == anchor['expected_opcode_hex'],
                        'Private-data reference instruction changed')
                require(not any(site+1 <= fix < site+3 for fix in relocations),
                        'Private-data reference has unresolved MZ relocation')
                source = (ROOT/anchor['source_path']).read_text(encoding='latin1').splitlines()
                require(anchor['source_symbol_hypothesis'] in source[anchor['source_line']-1],
                        'Source association changed')
                group_offset = int.from_bytes(image[site+1:site+3], 'little')
                oracle_address = dgroup['frame_load_address'] + group_offset
                equations.append({'symbol': anchor['source_symbol_hypothesis'],
                                  'candidate_offset': anchor['candidate_offset'],
                                  'oracle_address': oracle_address,
                                  'frame_paragraph': dgroup['frame_load_address']//16,
                                  'inferred_base': oracle_address-anchor['candidate_offset'],
                                  'reference_site': site, 'reference_bytes': image[site:site+3].hex()})
            solved = solve_shared_base(equations)
            row['placement'] = solved
            row['equations'] = equations
            if len({equation['inferred_base'] for equation in equations}) == 1:
                base = equations[0]['inferred_base']
                require(0 <= base and base+size <= len(image), 'Private DATA placement outside oracle')
                observed = image[base:base+size]
                mismatches = [i for i, (a, b) in enumerate(zip(payload, observed)) if a != b]
                matched_prefix = next((i for i, (a, b) in enumerate(zip(payload, observed))
                                       if a != b), size)
                row.update(candidate_span_if_placed=[base, base+size],
                           matching_prefix_bytes=matched_prefix,
                           candidate_sha256=sha(payload), oracle_span_sha256=sha(observed),
                           content_state='EXACT' if not mismatches else 'DIFFERS',
                           first_mismatch_offset=mismatches[0] if mismatches else None,
                           candidate_byte_at_first_mismatch=payload[mismatches[0]] if mismatches else None,
                           oracle_byte_at_first_mismatch=observed[mismatches[0]] if mismatches else None)
            rows.append(row)
    return rows


def run():
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    census = read_json(ROOT/'recovery/candidate-omf-census.json')
    anchors = read_json(ROOT/'recovery/shared-base-anchors.json')
    dgroup = read_json(ROOT/'layout/data-symbols.json')
    _, unpacked, oracle, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(inventory['load_sha256'] == sha(image) == anchors['oracle_load_sha256']
            == dgroup['oracle_sha256'], 'Shared-base input/oracle identity drift')
    require(census['authority'] == 'CANDIDATE_OBJECT_RESEARCH_ONLY', 'Candidate census authority changed')
    relocations = {item['load_offset'] for item in oracle['unpacked_mz']['relocations']}
    publics = public_rows(census, inventory)
    private = private_rows(census, inventory, image, relocations, anchors, dgroup)
    report = {'schema': 1, 'authority': 'RESEARCH_ONLY_PLACEMENT_DIAGNOSTIC',
              'oracle_load_sha256': sha(image),
              'input_sha256': {name: sha((ROOT/name).read_bytes()) for name in (
                  'recovery/restunts-inventory.json', 'recovery/candidate-omf-census.json',
                  'recovery/shared-base-anchors.json', 'layout/data-symbols.json')},
              'candidate_objects': len(census['objects']),
              'multi_public_objects': len({row['object_sha256'] for row in publics}),
              'private_initialized_data_objects': len({row['object_sha256'] for row in private}),
              'public_placements': publics, 'private_data_placements': private,
              'limitation': 'Placement equations do not establish original TU/data ownership, complete extent, ordered fixup binding, historical LINK behavior, or strict acceptance.'}
    write_json(ROOT/'recovery/shared-base-census.json', report)
    print({'multi_public_objects': report['multi_public_objects'],
           'private_initialized_data_objects': report['private_initialized_data_objects'],
           'public_states': [row['placement']['state'] for row in publics],
           'private_states': [(row['placement']['state'], row['content_state']) for row in private]})
    return report


if __name__ == '__main__':
    run()
