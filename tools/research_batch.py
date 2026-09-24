"""Archive a bounded, research-only compiler batch and compare unique outcomes.

The manifest names a current recovery card and independent C hypotheses. This
tool does not bind objects, spend grinder attempts, or authorize promotion.
"""
import argparse
import json
import re
import shutil
import uuid
from pathlib import Path

from common import ROOT, identity, project_path, read_json, require, sha, write_json
from compiler import CompileFailure, compile_source
from diagnostics import diagnose
from mz import MZ
from oracle import verify
from workflow import fingerprint, load_snapshot, workflow_inputs


def _spans(diagnosis):
    covered = set()
    for anchor in diagnosis.get('exact_anchors', []):
        start, end = anchor['target']['bytes']
        covered.update(range(start, end))
    return covered


def _ranges(offsets):
    result = []
    for value in sorted(offsets):
        if result and result[-1][1] == value:
            result[-1][1] += 1
        else:
            result.append([value, value+1])
    return result


def _difference(left, right):
    common = min(len(left), len(right))
    changed = [i for i in range(common) if left[i] != right[i]]
    prefix = 0
    while prefix < common and left[prefix] == right[prefix]:
        prefix += 1
    suffix = 0
    while suffix < common - prefix and left[-1 - suffix] == right[-1 - suffix]:
        suffix += 1
    return {'lengths': [len(left), len(right)], 'changed_common_ranges': _ranges(changed),
            'equal_prefix_bytes': prefix, 'equal_suffix_bytes': suffix,
            'length_delta': len(right) - len(left)}


def _archive_compiler_artifacts(receipt, destination, name):
    work = receipt.get('work_directory')
    if not work:
        return []
    archived = []
    for source_name, suffix in [('UNIT.OBJ', '.obj'), ('compiler.log', '-compiler.log')]:
        source = Path(work)/source_name
        if source.is_file():
            target = destination/(name+suffix)
            shutil.copyfile(source, target)
            archived.append({'path': target.name, 'identity': identity(target.read_bytes())})
    return archived


def run(manifest_path):
    manifest_path = project_path(manifest_path)
    manifest = read_json(manifest_path)
    task = manifest['task']
    require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', task), 'Invalid task name')
    require(isinstance(manifest.get('question'), str) and manifest['question'].strip(),
            'A discriminating research question is required')
    rows = manifest['hypotheses']
    ceiling = manifest['compiler_process_ceiling']
    require(isinstance(ceiling, int) and 2 <= len(rows) <= ceiling <= 32,
            'Batch must contain 2-32 hypotheses within its hard compiler ceiling')
    names = [r['id'] for r in rows]
    require(len(set(names)) == len(names) and all(re.fullmatch(r'[A-Za-z0-9_-]+', n) for n in names),
            'Hypothesis IDs must be unique and path-safe')
    for row in rows:
        for field in ('hypothesis', 'prediction', 'falsifier'):
            require(isinstance(row.get(field), str) and row[field].strip(),
                    f'{row["id"]}: missing {field}')
    index = read_json(ROOT/'recovery/context-index.json')
    matches = [row for row in index['tasks'] if task in (row['id'], row['name'])]
    require(len(matches) == 1, 'Unknown/ambiguous task; refresh queue and use stable ID or exact name')
    card_path = ROOT/matches[0]['card']
    card_bytes = card_path.read_bytes()
    require(sha(card_bytes) == matches[0]['card_sha256'], 'Context index/card changed; refresh queue')
    card = json.loads(card_bytes)
    require(card['name'] == matches[0]['name'], 'Context index/card name mismatch')
    require(fingerprint(workflow_inputs()) == index['workflow_fingerprint'],
            'Stale context index; refresh queue before research')
    require(load_snapshot(card['workflow_snapshot']) == workflow_inputs(),
            'Stale card; refresh and inspect context before research')
    evidence = card['evidence']
    start, end = evidence['start'], evidence['end']
    if 'oracle_extent' in card:
        require(card['oracle_extent'] == [start, end], 'Card oracle extent conflicts with evidence')
    oracle = verify(write=False)[1]
    target = MZ.parse(oracle).load_image(oracle)[start:end]
    require(len(target) == evidence['size'] == card['size'] and sha(target) == evidence['sha256'],
            'Pristine target differs from card')
    profile = card.get('profile', card.get('profile_hypothesis', 'msc510-medium'))
    segment = read_json(ROOT/card['recipe'])['object_segment'] if card.get('recipe') else 'UNIT_TEXT'
    sources = [(row, project_path(row['source']).read_bytes()) for row in rows]
    root = ROOT/'build/private/research-batches'/task
    destination = root/uuid.uuid4().hex[:12]
    destination.mkdir(parents=True, exist_ok=False)
    # Freeze every hypothesis, prediction, and source before the first compiler call.
    write_json(destination/'plan.json', {'authority': 'RESEARCH_ONLY', 'manifest': manifest,
        'manifest_identity': identity(manifest_path.read_bytes()), 'card': str(card_path.relative_to(ROOT)),
        'card_identity': identity(card_path.read_bytes()), 'target': identity(target),
        'profile': profile, 'segment': segment, 'compiler_process_ceiling': ceiling})
    for row, source in sources:
        (destination/(row['id']+'.c')).write_bytes(source)

    outcomes = []
    unique = {}
    processes = 0
    for row, source in sources:
        name = row['id']
        entry = {'id': name, 'source': identity(source), 'hypothesis': row['hypothesis'],
                 'prediction': row['prediction'], 'falsifier': row['falsifier']}
        try:
            obj, receipt = compile_source(source, profile)
        except CompileFailure as error:
            receipt = error.receipt
            processes += int('command' in receipt)
            entry.update(status='COMPILE_FAILURE', category=error.category, error=str(error),
                         compiler_processes=int('command' in receipt), receipt=receipt)
        else:
            processes += 1
            code = obj.segments.get(segment, b'')
            effective = receipt['effective_code']
            entry.update(status='COMPILED', compiler_processes=1, effective_code=effective,
                         code=identity(code), segment_lengths=obj.segment_lengths,
                         declarations=obj.segment_defs, publics=obj.publics,
                         externals=obj.externals, fixups=obj.linker_fixups,
                         extent_equal=len(code) == len(target),
                         literal_code_equal=code == target,
                         strict_acceptance='NOT_EVALUATED', receipt=receipt)
            if effective in unique:
                entry['same_effective_as'] = unique[effective]['id']
            else:
                (destination/(name+'.bin')).write_bytes(code)
                try:
                    comparison = diagnose(target, code, receipt, obj.linker_fixups, segment=segment)
                    if comparison.get('full_diagnostic'):
                        shutil.copyfile(comparison['full_diagnostic'], destination/(name+'-diagnostic.json'))
                    summary = comparison.get('match_summary', {})
                    entry['diagnosis'] = {'classifications': summary.get('classifications', []),
                        'counts': summary.get('counts', {}), 'exact_anchors': summary.get('anchors', []),
                        'patterns': summary.get('patterns', {}),
                        'error': comparison.get('diagnostic_error')}
                except Exception as error:
                    entry['diagnosis'] = {'error': str(error)}
                unique[effective] = {'id': name, 'code': code, 'entry': entry}
        entry['archived_compiler_artifacts'] = _archive_compiler_artifacts(receipt, destination, name)
        outcomes.append(entry)
        write_json(destination/'results.json', {'authority': 'RESEARCH_ONLY',
            'strict_acceptance': 'NOT_EVALUATED', 'compiler_processes': processes,
            'outcomes': outcomes, 'complete': False})

    representatives = list(unique.values())
    pairs = []
    for i, left in enumerate(representatives):
        for right in representatives[i+1:]:
            a, b = left['entry'], right['entry']
            left_anchors = _spans(a.get('diagnosis', {}))
            right_anchors = _spans(b.get('diagnosis', {}))
            pair = {'left': left['id'], 'right': right['id'],
                'code_delta': _difference(left['code'], right['code']),
                'fixups_equal': a['fixups'] == b['fixups'],
                'declarations_equal': a['declarations'] == b['declarations'],
                'oracle_anchor_ranges_gained': _ranges(right_anchors-left_anchors),
                'oracle_anchor_ranges_lost': _ranges(left_anchors-right_anchors)}
            if pair['fixups_equal']:
                comparison = diagnose(left['code'], right['code'], b['receipt'], b['fixups'], segment=segment)
                if comparison.get('full_diagnostic'):
                    shutil.copyfile(comparison['full_diagnostic'],
                                    destination/(left['id']+'-vs-'+right['id']+'-diagnostic.json'))
                summary = comparison.get('match_summary', {})
                pair['diagnosis'] = {'classifications': summary.get('classifications', []),
                    'counts': summary.get('counts', {}), 'patterns': summary.get('patterns', {}),
                    'error': comparison.get('diagnostic_error')}
            else:
                pair['diagnosis'] = {'skipped': 'Different fixup structures; raw code delta only'}
            pairs.append(pair)
    report = {'authority': 'RESEARCH_ONLY', 'strict_acceptance': 'NOT_EVALUATED',
        'question': manifest['question'], 'task': task, 'target': identity(target),
        'compiler_processes': processes, 'compiler_process_ceiling': ceiling,
        'hypotheses': len(rows), 'unique_effective_outputs': len(unique),
        'outcomes': outcomes, 'candidate_pairs': pairs, 'complete': True,
        'note': 'Literal code equality and diagnostic anchors do not prove binding or whole-image acceptance.'}
    write_json(destination/'results.json', report)
    compact_outputs = []
    for value in representatives:
        row = value['entry']
        compact_outputs.append({'id': row['id'],
            'aliases': [r['id'] for r in outcomes if r.get('effective_code') == row['effective_code']],
            'bytes': row['code']['size'], 'extent_equal': row['extent_equal'],
            'literal_code_equal': row['literal_code_equal'],
            'exact_aligned_instructions': row['diagnosis'].get('counts', {}).get('exact_instructions'),
            'target_anchored_percent': row['diagnosis'].get('counts', {}).get('target_anchored_percent'),
            'mismatch_classes': row['diagnosis'].get('classifications', [])})
    compact_pairs = []
    for pair in pairs[:12]:
        compact_pairs.append({'left': pair['left'], 'right': pair['right'],
            'changed_common_ranges': pair['code_delta']['changed_common_ranges'],
            'length_delta': pair['code_delta']['length_delta'],
            'fixups_equal': pair['fixups_equal'],
            'anchor_bytes_gained': sum(b-a for a,b in pair['oracle_anchor_ranges_gained']),
            'anchor_bytes_lost': sum(b-a for a,b in pair['oracle_anchor_ranges_lost']),
            'candidate_difference_families': [f['kind'] for f in
                pair['diagnosis'].get('patterns', {}).get('families', [])]})
    print(json.dumps({'report': str((destination/'results.json').relative_to(ROOT)),
        'hypotheses': len(rows), 'compiler_processes': processes,
        'unique_effective_outputs': len(unique),
        'outputs': compact_outputs, 'candidate_effects': compact_pairs,
        'omitted_candidate_pairs': max(0, len(pairs)-len(compact_pairs))}, indent=2))
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('manifest', help='JSON plan with question, hard ceiling and independent hypotheses')
    args = parser.parse_args()
    run(args.manifest)


if __name__ == '__main__':
    main()
