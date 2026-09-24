"""Evidence-qualified Stunts code-frame and candidate TU topology.

This reports anchored frames, address-order runs and direct call edges. The
original game OMF members are unavailable, so same-frame adjacency or calls
are compatible context, never proof of same historical object/TU membership.
"""
import collections
import json

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from oracle import verify
from workflow import fingerprint, workflow_inputs


def run():
    census = read_json(ROOT/'recovery/blocker-census.json')
    require(census['queue_fingerprint'] == fingerprint(workflow_inputs()),
            'Census stale; refresh queue and rerun blocker_census.py')
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(census['oracle_load_sha256'] == sha(image), 'Census oracle differs')
    frames = {s['name']: s.get('segment_paragraph') for s in inventory['segments']}
    functions = [f for f in inventory['functions']
                 if f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
    ]
    by_start = collections.defaultdict(list)
    for f in functions:
        by_start[f['start']].append(f)
    rows = []
    for f in sorted(functions, key=lambda x: (x['start'], x['end'])):
        paragraph = f.get('segment_paragraph', frames.get(f['segment']))
        rows.append({'id': f['stable_id'], 'name': f['name'], 'start': f['start'], 'end': f['end'],
                     'size': f['size'], 'frame_paragraph': paragraph,
                     'frame_evidence': 'IMPORTED_FRAME_ANCHORED_BY_FAR_CALL' if paragraph is not None else 'UNKNOWN',
                     'source_segment_label': f['segment'],
                     'object_membership': 'UNKNOWN'})
    runs = []
    current = []
    for row in rows:
        if current:
            prior = current[-1]
            gap = row['start']-prior['end']
            compatible = row['frame_paragraph'] is not None and \
                prior['frame_paragraph'] == row['frame_paragraph'] and 0 <= gap <= 2 and \
                all(byte == 0x90 for byte in image[prior['end']:row['start']])
            if not compatible:
                if len(current) > 1:
                    runs.append(current)
                current = []
        current.append(row)
    if len(current) > 1:
        runs.append(current)
    components = []
    for i, members in enumerate(runs, 1):
        components.append({'id': f'candidate-run-{i:03d}', 'state': 'CANDIDATE_RANGE',
                           'frame_paragraph': members[0]['frame_paragraph'],
                           'start': members[0]['start'], 'end': members[-1]['end'],
                           'members': [m['id'] for m in members],
                           'gaps': [members[j+1]['start']-members[j]['end']
                                    for j in range(len(members)-1)],
                           'basis': 'Address-contiguous verified intervals in one anchored code frame; intervening bytes only NOP',
                           'limitation': 'May cross original object boundaries or omit private code/data.'})
    edges = []
    for task in census['tasks']:
        for mode in ('far_targets', 'near_targets'):
            for call in task['observations'].get(mode, []):
                target = call['load_target']
                matched = by_start.get(target, [])
                edges.append({'caller_task': task['id'], 'site': call['site'],
                              'kind': 'DIRECT_FAR' if mode == 'far_targets' else 'DIRECT_NEAR',
                              'target_load_address': target,
                              'mapped_target_ids': [f['stable_id'] for f in matched],
                              'target_frame_paragraph': call.get('paragraph'),
                              'relocated_segment_word': call.get('segment_word_relocated'),
                              'object_relation': 'COMPATIBLE_ONLY'})
    # This fixture proves that both bodies can coexist exactly when compiled
    # together. It does not prove the original two functions shared a TU.
    compatible_probes = []
    rectangle = ROOT/'recovery/tu/rectangle-pair.json'
    if rectangle.exists():
        receipt = read_json(rectangle)
        if receipt.get('status') == 'EXACT_CONTIGUOUS_CONTRIBUTION_EXPERIMENT':
            require(sha(image[receipt['start']:receipt['end']]) == receipt['expected']['sha256'],
                    'Rectangle TU probe target changed')
            compatible_probes.append({'state': 'COMPATIBLE_COMPONENT',
                                      'members': ['rect_is_overlapping', 'rect_is_inside'],
                                      'start': receipt['start'], 'end': receipt['end'],
                                      'publics': receipt['publics'],
                                      'evidence': 'recovery/tu/rectangle-pair.json',
                                      'limitation': receipt['limitation']})
    report = {'schema': 1, 'authority': 'RESEARCH_ONLY',
              'oracle_load_sha256': sha(image), 'queue_fingerprint': census['queue_fingerprint'],
              'verified_functions': len(rows), 'anchored_frame_functions': sum(r['frame_paragraph'] is not None for r in rows),
              'functions': rows, 'candidate_runs': components, 'direct_call_edges': edges,
              'compatible_tu_probes': compatible_probes,
              'proven_same_object_pairs': [], 'proven_object_boundaries': [],
              'note': 'Call frame and address order constrain code segment context. Original OMF membership, private contributions and link order remain unproven.'}
    write_json(ROOT/'recovery/build-topology.json', report)
    print(json.dumps({'verified_functions': report['verified_functions'],
                      'anchored_frame_functions': report['anchored_frame_functions'],
                      'candidate_runs': len(components),
                      'candidate_run_functions': sum(len(c['members']) for c in components),
                      'direct_call_edges': len(edges),
                      'mapped_call_edges': sum(bool(e['mapped_target_ids']) for e in edges),
                      'compatible_tu_probes': len(compatible_probes),
                      'proven_same_object_pairs': 0, 'proven_object_boundaries': 0}, indent=2))
    return report


if __name__ == '__main__':
    run()
