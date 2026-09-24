"""Current local archive census of complete candidate compiler OMF objects.

Research only: candidate FIXUPP records are not original linker records and
cannot authorize a binder mode or reopen a task. Duplicate archive copies are
collapsed by object SHA-256 within each current supervisor task.
"""
import collections
import json

from common import ROOT, read_json, sha, write_json
from mz import MZ
from object_probe import read_object
from oracle import verify


def mode(fixup):
    return ('{loc}:self={self_relative}:frame={frame_method}/{frame_kind}:'
            'target={target_method}/{target_kind}'.format(**fixup))


def effective_output_sha(obj):
    """Research identity for complete emitted contributions, not raw OMF records.

    Module names, record packing, and checksums may differ without changing
    compiler output. Retain segment declarations, initialized bytes, public
    order, externals, and ordered fixups so a collapse cannot hide obligations.
    """
    payload = {'segment_defs': obj.segment_defs,
               'segment_lengths': obj.segment_lengths,
               'segments': {name: bytes(data).hex() for name, data in obj.segments.items()},
               'publics': obj.publics, 'externals': obj.externals,
               'fixups': obj.linker_fixups}
    return sha(json.dumps(payload, sort_keys=True, separators=(',', ':')).encode())


def compare_nonfixup(obj, function, image):
    """Research comparison; masked fields remain unresolved binding obligations."""
    if not function or function['status'] != 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
        return {'state': 'UNVERIFIED_TARGET'}
    start, end = function['start'], function['end']
    if obj.segment_lengths.get('UNIT_TEXT') != end-start:
        return {'state': 'EXTENT_MISMATCH'}
    code = obj.segment_bytes('UNIT_TEXT')
    if len(code) != end-start:
        return {'state': 'INCOMPLETE_OBJECT_CODE'}
    masked = set()
    for fix in obj.linker_fixups:
        if fix['segment'] != 'UNIT_TEXT':
            return {'state': 'NON_CODE_FIXUP'}
        at, width = fix['offset'], fix['width']
        if not isinstance(at, int) or not isinstance(width, int) or at < 0 or at+width > len(code):
            return {'state': 'INVALID_FIXUP_RANGE'}
        if masked.intersection(range(at, at+width)):
            return {'state': 'OVERLAPPING_FIXUPS'}
        masked.update(range(at, at+width))
    target = image[start:end]
    equal = sum(code[i] == target[i] for i in range(len(code)) if i not in masked)
    total = len(code)-len(masked)
    return {'state': 'COMPARED', 'nonfixup_equal': equal == total,
            'nonfixup_equal_bytes': equal, 'nonfixup_total_bytes': total,
            'unresolved_fixup_bytes': len(masked),
            'single_public_at_zero': obj.publics == [{'name': '_'+function['name'],
                                                       'segment': 'UNIT_TEXT', 'offset': 0}],
            'other_segments_zero': all(name == 'UNIT_TEXT' or size == 0
                                       for name, size in obj.segment_lengths.items())}


def run():
    queue = read_json(ROOT/'recovery/queue.json')
    inventory = {f.get('stable_id'): f for f in
                 read_json(ROOT/'recovery/restunts-inventory.json')['functions']}
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    current = {task['name']: task for task in queue['tasks'] if task['tier'] == 'SUPERVISOR'}
    folders = {key: task for task in current.values() for key in (task['name'], task['id'])}
    roots = (ROOT/'recovery/corpus/endurance-001', ROOT/'build/private/research-batches')
    objects = {}
    errors = []
    file_count = 0
    for root in roots:
        if not root.exists():
            continue
        for path in root.rglob('*'):
            if not path.is_file() or path.suffix.lower() != '.obj':
                continue
            relative = path.relative_to(root)
            if not relative.parts or relative.parts[0] not in folders:
                continue
            task = folders[relative.parts[0]]
            data = path.read_bytes()
            digest = sha(data)
            file_count += 1
            key = (task['id'], digest)
            if key in objects:
                objects[key]['archive_paths'].append(path.relative_to(ROOT).as_posix())
                continue
            try:
                obj = read_object(data, research_local_symbols=True)
            except Exception as exc:
                errors.append({'task_id': task['id'], 'path': path.relative_to(ROOT).as_posix(),
                               'object_sha256': digest, 'error': str(exc)})
                continue
            fixups = [{key: fix[key] for key in ('segment', 'offset', 'loc', 'width',
                      'self_relative', 'frame_method', 'frame_kind', 'target_method',
                      'target_kind', 'target', 'displacement', 'encoded_addend')}
                      for fix in obj.linker_fixups]
            objects[key] = {'task_id': task['id'], 'name': task['name'],
                            'object_sha256': digest,
                            'effective_output_sha256': effective_output_sha(obj),
                            'archive_paths': [path.relative_to(ROOT).as_posix()],
                            'target_extent_bytes': task['size'] if
                                task['boundary_status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' else None,
                            'unit_text_bytes': obj.segment_lengths.get('UNIT_TEXT'),
                            'segment_lengths': obj.segment_lengths,
                            'publics': obj.publics,
                            'externals': obj.externals,
                            'fixups': fixups,
                            'fixup_modes': sorted({mode(fix) for fix in obj.linker_fixups}),
                            'local_symbol_records': obj.local_symbol_records,
                            'oracle_nonfixup_comparison': compare_nonfixup(obj,
                                inventory.get(task['id']), image)}
    families = collections.defaultdict(lambda: {'records': 0, 'object_sha256': set(), 'task_ids': set()})
    for row in objects.values():
        for fixup in row['fixups']:
            family = families[mode(fixup)]
            family['records'] += 1
            family['object_sha256'].add(row['object_sha256'])
            family['task_ids'].add(row['task_id'])
    shape_candidates = [row for row in objects.values()
                        if row['oracle_nonfixup_comparison'].get('nonfixup_equal')
                        and row['oracle_nonfixup_comparison']['single_public_at_zero']
                        and row['oracle_nonfixup_comparison']['other_segments_zero']]
    effective_groups = collections.defaultdict(list)
    for row in objects.values():
        effective_groups[(row['task_id'], row['effective_output_sha256'])].append(row)
    task_groups = collections.defaultdict(list)
    for (task_id, signature), members in effective_groups.items():
        example = members[0]
        task_groups[task_id].append({'effective_output_sha256': signature,
                                    'object_variants': len(members),
                                    'object_sha256': sorted(m['object_sha256'] for m in members),
                                    'unit_text_bytes': example['unit_text_bytes'],
                                    'target_extent_bytes': example['target_extent_bytes'],
                                    'fixup_modes': example['fixup_modes'],
                                    'oracle_nonfixup_comparison': example['oracle_nonfixup_comparison']})
    report = {'schema': 1, 'authority': 'CANDIDATE_OBJECT_RESEARCH_ONLY',
              'archive_roots': [p.relative_to(ROOT).as_posix() for p in roots],
              'current_supervisor_tasks': len(current), 'archive_files': file_count,
              'distinct_task_objects': len(objects),
              'tasks_with_objects': len({row['task_id'] for row in objects.values()}),
              'distinct_effective_outputs': len(effective_groups),
              'effective_output_groups_by_task': {task_id: sorted(groups,
                  key=lambda group: group['effective_output_sha256'])
                  for task_id, groups in sorted(task_groups.items())},
              'complete_nonfixup_shape_objects': len(shape_candidates),
              'complete_nonfixup_shape_tasks': sorted({row['task_id'] for row in shape_candidates}),
              'parser_errors': errors,
              'mode_families': {name: {'records_in_distinct_task_objects': value['records'],
                                     'distinct_object_payloads': len(value['object_sha256']),
                                     'task_ids': sorted(value['task_ids'])}
                                for name, value in sorted(families.items())},
              'objects': sorted(objects.values(), key=lambda r: (r['task_id'], r['object_sha256'])),
              'limitation': 'Only locally archived candidate compiler objects are counted. MZ relocations do not disclose OMF modes; candidate mode frequency is not a population estimate.'}
    write_json(ROOT/'recovery/candidate-omf-census.json', report)
    print({key: report[key] for key in ('current_supervisor_tasks', 'archive_files',
          'distinct_task_objects', 'distinct_effective_outputs',
          'tasks_with_objects', 'mode_families', 'parser_errors')})
    return report


if __name__ == '__main__':
    run()
