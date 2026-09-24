"""Fresh, evidence-qualified census of every current supervisor task.

Pristine linear decode is research evidence only. It neither reviews a mapped
interval nor changes queue eligibility, source budgets, recipes or ownership.
MZ relocations cannot reveal the missing historical OMF frame/target methods.
"""
import collections
import json
import sys

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from oracle import verify
from workflow import fingerprint, workflow_inputs

sys.path.insert(0, str(ROOT/'build/python'))
from capstone import CS_ARCH_X86, CS_MODE_16, Cs


FAMILY_RESEARCH = {
    'BOUNDARY_OR_EXTENT': ('partial-label mismatch localization', 'available for partial-label rows; no-anchor rows require an independent address', 'high', 'medium for exact opcode aliases; low for the remaining mixed residuals'),
    'MAPPING_OR_CODE_DATA_CLASSIFICATION': ('independent address anchor', 'not available for these no-anchor rows', 'high', 'low'),
    'EMISSION_BYTES_CFG_REVIEW': ('traverse exact source-emission coordinates and prove raw padding/data unreachable', 'exact byte coordinates available; CFG review remains separate', 'medium', 'high for mapping, unknown for source promotion'),
    'SOURCE_OFFSET_TABLE_BYTES_VERIFIED': ('finish each partial function boundary and code/data review after exact table emission', 'complete table words and following label verified in the oracle', 'medium', 'high for table bytes; unknown for complete function'),
    'BOUNDARY_OR_EXTENT_CANDIDATE': ('review external jump target and full contribution', 'pristine decode available', 'medium', 'medium'),
    'INSTRUCTION_EVIDENCE_MISSING': ('review pristine linear decode and CFG/ownership', 'complete research decode available', 'low', 'high for diagnostic unlock; low for production alone'),
    'BINDING_OR_FIXUP_MODE_UNKNOWN': ('compile one complete candidate and compare historical LINK fixups', 'MZ context available; OMF mode requires candidate source', 'medium', 'unknown until OMF modes sampled'),
    'BINDING_OR_FIXUP_CANDIDATE': ('review target public/frame, then complete candidate object', 'direct call target addresses available', 'medium', 'medium for target addressing; low for whole-function unlock'),
    'TU_OR_OBJECT_CONTEXT_CANDIDATE': ('frozen-body predecessor/order probe', 'tu_context.py available where source exists', 'medium', 'medium for research; low for production alone'),
    'DATA_OR_GLOBAL_OWNERSHIP': ('prove address from independent instruction/storage evidence', 'pristine operands available; ownership review manual', 'medium', 'medium'),
    'CODE_SEGMENT_DATA_OWNERSHIP': ('prove same-CODE data contribution and access form', 'bounded existing CS-data evidence', 'high', 'low'),
    'ABI_OR_DECLARATION': ('bounded compiler fixture for observed register/interrupt form', 'research compiler available', 'medium', 'low to medium'),
    'LOCAL_SOURCE_SHAPE_OR_UNTESTED': ('prepare reviewed source and compare complete object', 'research_batch.py available', 'low', 'medium'),
}


def decode_observations(card, image, decoder):
    evidence = card['evidence']
    start, end = evidence.get('start'), evidence.get('end')
    if card['boundary_status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED':
        require(type(start) is int and type(end) is int and 0 <= start < end <= len(image)
                and sha(image[start:end]) == evidence['sha256'],
                'Mapped source-emission bytes changed: '+card['name'])
        return {'state': 'EMISSION_BYTES_VERIFIED_CFG_UNREVIEWED', 'emission_extent_bytes': end-start}
    if card['boundary_status'] != 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
        return {'state': 'UNVERIFIED_INTERVAL'}
    require(type(start) is int and type(end) is int and 0 <= start < end <= len(image),
            'Invalid verified extent')
    payload = image[start:end]
    require(sha(payload) == evidence['sha256'], 'Verified extent drifted: '+card['name'])
    instructions = list(decoder.disasm(payload, start))
    complete = bool(instructions) and instructions[0].address == start and \
        instructions[-1].address + instructions[-1].size == end and \
        b''.join(bytes(i.bytes) for i in instructions) == payload
    calls = collections.Counter()
    jumps = collections.Counter()
    relocation_context = collections.Counter()
    relocations = evidence.get('relocation_sites', [])
    samples = []
    far_targets = []
    near_targets = []
    if complete:
        for ins in instructions:
            raw = bytes(ins.bytes)
            if ins.mnemonic in ('call', 'lcall'):
                mode = ('direct_near_e8' if raw[0] == 0xe8 else
                        'direct_far_9a' if raw[0] == 0x9a else
                        'indirect_ff' if raw[0] == 0xff else 'other_call')
                calls[mode] += 1
                samples.append({'offset': ins.address-start, 'kind': mode, 'bytes': raw.hex()})
                if raw[0] == 0x9a and len(raw) == 5:
                    offset = int.from_bytes(raw[1:3], 'little')
                    paragraph = int.from_bytes(raw[3:5], 'little')
                    far_targets.append({'site': ins.address, 'offset': offset,
                                        'paragraph': paragraph, 'load_target': paragraph*16+offset,
                                        'segment_word_relocated': ins.address+3 in relocations})
                if raw[0] == 0xe8 and len(raw) == 3:
                    displacement = int.from_bytes(raw[1:3], 'little', signed=True)
                    near_targets.append({'site': ins.address,
                                         'load_target': ins.address+ins.size+displacement})
            elif ins.mnemonic == 'jmp':
                mode = ('direct_near' if raw[0] in (0xe9, 0xeb) else
                        'direct_far' if raw[0] == 0xea else 'indirect_or_other')
                jumps[mode] += 1
            for site in relocations:
                if ins.address <= site < ins.address+ins.size:
                    kind = ('call_far_operand' if raw[0] == 0x9a else
                            'jump_far_operand' if raw[0] == 0xea else
                            'other_instruction_operand')
                    relocation_context[kind] += 1
                    samples.append({'offset': site-start, 'kind': 'mz_relocation_'+kind,
                                    'instruction': ins.mnemonic+' '+ins.op_str})
    decoded_rows = ([{'load_offset': ins.address, 'bytes': bytes(ins.bytes).hex(),
                     'instruction': (ins.mnemonic+' '+ins.op_str).strip()} for ins in instructions]
                    if complete and not card.get('disassembly') else [])
    return {'state': 'LINEAR_DECODE_COMPLETE_RESEARCH_ONLY' if complete else 'LINEAR_DECODE_INCOMPLETE',
            'instruction_count': len(instructions), 'calls': dict(calls), 'jumps': dict(jumps),
            'relocation_context': dict(relocation_context), 'relocation_sites': len(relocations),
            'relocation_sites_without_instruction': len(relocations)-sum(relocation_context.values()),
            'far_targets': far_targets, 'near_targets': near_targets,
            'samples': samples[:24], 'sample_count': len(samples),
            'research_disassembly': decoded_rows,
            'card_instruction_evidence': bool(card.get('disassembly'))}


def classify(card, obs):
    categories = []
    if obs['state'] == 'EMISSION_BYTES_VERIFIED_CFG_UNREVIEWED':
        return ['EMISSION_BYTES_CFG_REVIEW']
    if obs['state'] == 'UNVERIFIED_INTERVAL':
        categories.append('BOUNDARY_OR_EXTENT')
        if card['boundary_status'] == 'UNMAPPED_NO_ADDRESS_ANCHOR':
            categories.append('MAPPING_OR_CODE_DATA_CLASSIFICATION')
    else:
        if obs['state'] != 'LINEAR_DECODE_COMPLETE_RESEARCH_ONLY':
            categories.append('MAPPING_OR_CODE_DATA_CLASSIFICATION')
        if not obs['card_instruction_evidence']:
            categories.append('INSTRUCTION_EVIDENCE_MISSING')
        if obs['relocation_sites']:
            categories.append('BINDING_OR_FIXUP_MODE_UNKNOWN')
        if obs['calls'].get('direct_near_e8'):
            categories.append('TU_OR_OBJECT_CONTEXT_CANDIDATE')
        if obs['calls'].get('direct_far_9a') or obs['calls'].get('indirect_ff'):
            categories.append('BINDING_OR_FIXUP_CANDIDATE')
        joined = ' '.join(card['capability_blockers'])
        if 'Unregistered DGROUP' in joined or 'Unregistered absolute' in joined:
            categories.append('DATA_OR_GLOBAL_OWNERSHIP')
        if 'CS-relative storage' in joined:
            categories.append('CODE_SEGMENT_DATA_OWNERSHIP')
        if 'Hardware/interrupt' in joined or 'Explicit SS' in joined:
            categories.append('ABI_OR_DECLARATION')
        if 'External/indirect jump' in joined:
            categories.append('BOUNDARY_OR_EXTENT_CANDIDATE')
        if not categories:
            categories.append('LOCAL_SOURCE_SHAPE_OR_UNTESTED')
    return categories


def primary_category(categories):
    # Select only a gate that follows directly from current evidence. Keep
    # overlapping candidates visible; ambiguity is not resolved by precedence.
    for direct_gate in ('EMISSION_BYTES_CFG_REVIEW', 'BOUNDARY_OR_EXTENT', 'BINDING_OR_FIXUP_MODE_UNKNOWN',
                        'INSTRUCTION_EVIDENCE_MISSING', 'MAPPING_OR_CODE_DATA_CLASSIFICATION'):
        if direct_gate in categories:
            return direct_gate, 'HIGH_FOR_GATE_ONLY'
    if len(categories) == 1:
        return categories[0], 'CANDIDATE_NOT_SOURCE_CAUSE'
    return 'UNKNOWN', 'MULTIPLE_PLAUSIBLE_CAUSES'


def run():
    queue = read_json(ROOT/'recovery/queue.json')
    require(queue['workflow_fingerprint'] == fingerprint(workflow_inputs()),
            'Queue stale; refresh before census')
    _, unpacked, oracle_report, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    table_report = read_json(ROOT/'recovery/table-offset-census.json')
    require(table_report['authority'] == 'RESEARCH_ONLY' and
            table_report['oracle_load_sha256'] == sha(image) and
            table_report['source_inventory_sha256'] == sha((ROOT/'recovery/restunts-inventory.json').read_bytes()),
            'Table-offset census stale; rerun reclassify')
    table_spans = collections.defaultdict(list)
    for span in table_report['tables']:
        if span['state'] == 'EXACT_BRACKETED_TABLE_BYTES':
            table_spans[span['function_id']].append({'label': span['label'],
                'start': span['start'], 'end': span['end'], 'size': span['size']})
    from code_symbols import resolve_code_symbols
    reviewed_code_symbols = resolve_code_symbols(
        set(read_json(ROOT/'layout/code-symbols.json')['symbols']), image,
        oracle_report['unpacked_mz']['relocations'])
    reviewed_targets = {value['load_address'] for value in reviewed_code_symbols.values()}
    decoder = Cs(CS_ARCH_X86, CS_MODE_16)
    rows = []
    for task in queue['tasks']:
        if task['tier'] != 'SUPERVISOR':
            continue
        card = read_json(ROOT/task['card'])
        require(card['id'] == task['id'] and card['name'] == task['name'], 'Card/queue identity mismatch')
        obs = decode_observations(card, image, decoder)
        categories = classify(card, obs)
        verified_tables = table_spans.get(task['id'], [])
        if verified_tables:
            categories.append('SOURCE_OFFSET_TABLE_BYTES_VERIFIED')
        primary, primary_confidence = primary_category(categories)
        far_targets = obs.get('far_targets', [])
        rows.append({'id': task['id'], 'name': task['name'], 'card': task['card'],
                     'boundary_status': task['boundary_status'],
                     'exact_target_bytes': task['size'] if obs['state'] not in ('UNVERIFIED_INTERVAL', 'EMISSION_BYTES_VERIFIED_CFG_UNREVIEWED') else None,
                     'emission_extent_bytes': obs.get('emission_extent_bytes'),
                     'source_table_span_bytes': sum(span['size'] for span in verified_tables),
                     'source_table_spans': verified_tables,
                     'historical_blocker': task['blocker'], 'current_capability_blockers': task['capability_blockers'],
                     'categories': categories, 'primary_category': primary,
                     'primary_confidence': primary_confidence, 'observations': obs,
                     'far_target_address_coverage': {'sites': len(far_targets),
                         'reviewed_sites': sum(f['load_target'] in reviewed_targets for f in far_targets),
                         'all_reviewed': bool(far_targets) and all(f['load_target'] in reviewed_targets for f in far_targets)},
                     'confidence': ('EMISSION_COORDINATES_ONLY_CFG_UNREVIEWED' if obs['state']=='EMISSION_BYTES_VERIFIED_CFG_UNREVIEWED' else
                                    'DIRECT_BYTES_OR_QUEUE' if obs['state']!='UNVERIFIED_INTERVAL' else 'BOUNDARY_UNCERTAIN'),
                     'automatic_reopen': False})
    family = {}
    for category in sorted({c for row in rows for c in row['categories']}):
        members = [row for row in rows if category in row['categories']]
        experiment, testability, cost, confidence = FAMILY_RESEARCH[category]
        family[category] = {'tasks': len(members),
                            'exact_target_bytes': sum(row['exact_target_bytes'] or 0 for row in members),
                            'emission_extent_bytes': sum(row['emission_extent_bytes'] or 0 for row in members),
                            'source_table_span_bytes': sum(row['source_table_span_bytes'] for row in members),
                            'verified_tasks': sum(row['exact_target_bytes'] is not None for row in members),
                            'affected_tasks_upper_bound': len(members),
                            'smallest_discriminating_experiment': experiment,
                            'current_testability': testability,
                            'shared_capability_cost': cost,
                            'unlock_confidence': confidence,
                            'single_change_production_unlock': 'UNKNOWN; overlapping blockers and untested source remain'}
    modes = collections.Counter()
    reloc = collections.Counter()
    primary_counts = collections.Counter(row['primary_category'] for row in rows)
    targets = collections.defaultdict(list)
    for row in rows:
        modes.update(row['observations'].get('calls', {}))
        reloc.update(row['observations'].get('relocation_context', {}))
        for target in row['observations'].get('far_targets', []):
            targets[target['load_target']].append({'caller': row['id'], **target})
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    mapped_starts = collections.defaultdict(list)
    for function in inventory['functions']:
        if function['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
            mapped_starts[function['start']].append(function['name'])
    target_summary = []
    for address, sites in sorted(targets.items()):
        target_summary.append({'load_target': address, 'mapped_function_names': mapped_starts.get(address, []),
                               'calls': len(sites), 'distinct_caller_tasks': len({s['caller'] for s in sites}),
                               'relocated_segment_words': sum(s['segment_word_relocated'] for s in sites),
                               'sites': sites})
    report = {'schema': 1, 'authority': 'RESEARCH_ONLY',
              'queue_fingerprint': queue['workflow_fingerprint'], 'oracle_load_sha256': sha(image),
              'supervisor_tasks': len(rows), 'families_overlapping': family,
              'primary_family_counts': dict(primary_counts),
              'observed_call_instruction_modes': dict(modes),
              'observed_mz_relocation_instruction_contexts': dict(reloc),
              'reviewed_far_code_addresses': sorted(reviewed_targets),
              'far_call_tasks_with_all_target_addresses_reviewed': sum(
                  row['far_target_address_coverage']['all_reviewed'] for row in rows),
              'far_call_targets': target_summary,
              'omf_mode_population': 'UNKNOWN: pristine MZ relocation entries do not carry OMF frame/target methods',
              'tasks': rows,
              'note': 'Research decode and categories do not replace reviewed instruction evidence, establish an original object or reopen a source budget.'}
    write_json(ROOT/'recovery/blocker-census.json', report)
    print(json.dumps({k: report[k] for k in ('supervisor_tasks', 'families_overlapping',
          'observed_call_instruction_modes', 'observed_mz_relocation_instruction_contexts',
          'omf_mode_population')}, indent=2))
    return report


if __name__ == '__main__':
    run()
