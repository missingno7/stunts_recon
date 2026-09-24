"""Frozen-body, whole-object TU context comparisons (research only).

The manifest supplies one unchanged target source and ordered context snippets.
Every compiled variant retains complete OMF evidence. Public-bounded windows are
diagnostic: a later private symbol or data contribution can make them incomplete.
This tool cannot create a production recipe or promote a match.
"""
import argparse
import json
import shutil
import uuid
from pathlib import Path

from common import ROOT, identity, project_path, read_json, require, sha, write_json
from compiler import CompileFailure, compile_source
from mz import MZ
from oracle import verify


def public_window(obj, symbol):
    matches = [p for p in obj.publics if p['name'] == symbol]
    require(len(matches) == 1, 'Target public must occur exactly once')
    public = matches[0]
    segment = public['segment']
    code = obj.segment_bytes(segment)
    start = public['offset']
    later = [p['offset'] for p in obj.publics if p['segment'] == segment and p['offset'] > start]
    end = min(later, default=len(code))
    require(0 <= start < end <= len(code), 'Invalid public-bounded target window')
    fixups = []
    for fix in obj.linker_fixups:
        if fix['segment'] == segment and start <= fix['offset'] < end:
            normalized = dict(fix)
            normalized['offset'] -= start
            fixups.append(normalized)
    return {'segment': segment, 'start': start, 'end': end,
            'bytes': code[start:end], 'fixups': fixups,
            'limitation': 'Bounded by the next public or SEGDEF end; private emitted code may be included'}


def protected_window(obj, symbol, oracle_bytes):
    """Research guard for one independently locked exact neighbor window."""
    if not any(p['name'] == symbol for p in obj.publics):
        return {'public': symbol, 'state': 'MISSING_PUBLIC', 'literal_equal': False}
    window = public_window(obj, symbol)
    exact = window['bytes'] == oracle_bytes and not window['fixups']
    return {'public': symbol, 'state': 'EXACT_LITERAL_WINDOW' if exact else 'DIFFERS',
            'literal_equal': window['bytes'] == oracle_bytes,
            'extent': len(window['bytes']), 'oracle_extent': len(oracle_bytes),
            'offset': window['start'], 'segment': window['segment'],
            'bytes': identity(window['bytes']), 'fixups': window['fixups'],
            'limitation': window['limitation']}


def protected_component(obj, lock, oracle_bytes):
    """Check a complete candidate code contribution and declared OMF topology."""
    segment = lock['segment']
    code = obj.segment_bytes(segment) if segment in obj.segment_lengths else b''
    checks = {'code_equal': code == oracle_bytes,
              'declared_extent_equal': obj.segment_lengths.get(segment) == len(oracle_bytes),
              'publics_equal': obj.publics == lock['publics'],
              'fixups_equal': obj.linker_fixups == lock['fixups'],
              'other_segments_zero': all(name == segment or size == 0
                                         for name, size in obj.segment_lengths.items())}
    return {'state': 'EXACT_LITERAL_COMPONENT' if all(checks.values()) else 'DIFFERS',
            'checks': checks, 'segment': segment, 'segment_lengths': obj.segment_lengths,
            'code': identity(code), 'oracle_extent': len(oracle_bytes),
            'publics': obj.publics, 'fixups': obj.linker_fixups,
            'limitation': 'Complete candidate OMF topology; original historical object membership remains unproved'}


def _source(path):
    data = project_path(path).read_bytes()
    require(data and b'\x00' not in data, 'Empty or binary context snippet')
    return data


def _variant_source(target, row, snippets):
    before, after = row.get('before', []), row.get('after', [])
    require(isinstance(before, list) and isinstance(after, list), 'Context order must be lists')
    require(all(name in snippets for name in before + after), 'Unknown context snippet')
    require(len(set(before + after)) == len(before + after), 'Repeated context snippet')
    return b'\n'.join([*(snippets[name] for name in before), target,
                       *(snippets[name] for name in after)]) + b'\n'


def run(manifest_path):
    path = project_path(manifest_path)
    plan = read_json(path)
    require(plan.get('schema') == 1 and plan.get('authority') == 'RESEARCH_ONLY',
            'TU context manifest must declare research-only schema 1')
    require(isinstance(plan.get('question'), str) and plan['question'].strip(), 'Missing question')
    require(isinstance(plan.get('prediction'), str) and plan['prediction'].strip(), 'Missing prediction')
    require(isinstance(plan.get('falsifier'), str) and plan['falsifier'].strip(), 'Missing falsifier')
    variants = plan['variants']
    require(isinstance(variants, list) and 2 <= len(variants) <= 12, 'Expected 2-12 bounded variants')
    labels = [v['id'] for v in variants]
    require(len(set(labels)) == len(labels) and all(x.replace('-', '').replace('_', '').isalnum() for x in labels),
            'Variant IDs must be unique and path-safe')
    target = _source(plan['target_source'])
    require(sha(target) == plan['target_sha256'], 'Target body changed from frozen manifest')
    snippets = {name: _source(p) for name, p in plan.get('snippets', {}).items()}
    sources = {row['id']: _variant_source(target, row, snippets) for row in variants}
    oracle_bytes = None
    if 'oracle' in plan:
        lock = plan['oracle']
        start, end = lock['start'], lock['end']
        whole = verify(write=False)[1]
        oracle_bytes = MZ.parse(whole).load_image(whole)[start:end]
        require(sha(oracle_bytes) == lock['sha256'] and len(oracle_bytes) == end-start,
                'Oracle target identity differs from manifest')
    protections = plan.get('protected_neighbors', [])
    require(isinstance(protections, list) and len(protections) <= 8,
            'Expected at most eight protected neighbors')
    require(len({p['public'] for p in protections}) == len(protections),
            'Repeated protected public')
    protected_oracles = {}
    if protections:
        whole = verify(write=False)[1]
        image = MZ.parse(whole).load_image(whole)
        for protection in protections:
            require(set(protection) == {'public', 'variants', 'oracle'} and
                    isinstance(protection['public'], str) and protection['public'] != plan['target_public'] and
                    isinstance(protection['variants'], list) and protection['variants'] and
                    len(set(protection['variants'])) == len(protection['variants']) and
                    set(protection['variants']) <= set(labels), 'Invalid protected neighbor declaration')
            lock = protection['oracle']
            require(set(lock) == {'start', 'end', 'sha256'} and
                    type(lock['start']) is int and type(lock['end']) is int and
                    0 <= lock['start'] < lock['end'] <= len(image),
                    'Invalid protected neighbor oracle interval')
            data = image[lock['start']:lock['end']]
            require(sha(data) == lock['sha256'], 'Protected neighbor oracle identity differs')
            protected_oracles[protection['public']] = data
    component_lock = plan.get('protected_component')
    component_oracle = None
    if component_lock is not None:
        require(set(component_lock) == {'segment', 'variants', 'oracle', 'publics', 'fixups'} and
                isinstance(component_lock['segment'], str) and
                isinstance(component_lock['variants'], list) and component_lock['variants'] and
                len(set(component_lock['variants'])) == len(component_lock['variants']) and
                set(component_lock['variants']) <= set(labels) and
                isinstance(component_lock['publics'], list) and component_lock['publics'] and
                isinstance(component_lock['fixups'], list), 'Invalid protected component declaration')
        lock = component_lock['oracle']
        whole = verify(write=False)[1]
        image = MZ.parse(whole).load_image(whole)
        require(set(lock) == {'start', 'end', 'sha256'} and
                type(lock['start']) is int and type(lock['end']) is int and
                0 <= lock['start'] < lock['end'] <= len(image),
                'Invalid protected component oracle interval')
        component_oracle = image[lock['start']:lock['end']]
        require(sha(component_oracle) == lock['sha256'],
                'Protected component oracle identity differs')
    root = ROOT/'build/private/tu-context'/uuid.uuid4().hex[:12]
    root = ROOT/'build/private/tu-context'/uuid.uuid4().hex[:12]
    root.mkdir(parents=True, exist_ok=False)
    frozen = {'authority': 'RESEARCH_ONLY', 'manifest': plan,
              'manifest_identity': identity(path.read_bytes()),
              'target_identity': identity(target),
              'snippet_identities': {n: identity(data) for n, data in snippets.items()},
              'oracle_identity': identity(oracle_bytes) if oracle_bytes is not None else None}
    write_json(root/'plan.json', frozen)
    rows = []
    for variant in variants:
        name = variant['id']
        source = sources[name]
        (root/(name+'.c')).write_bytes(source)
        row = {'id': name, 'source_identity': identity(source), 'profile': variant.get('profile', plan['profile']),
               'before': variant.get('before', []), 'after': variant.get('after', [])}
        try:
            obj, receipt = compile_source(source, row['profile'], research_local_symbols=True)
            window = public_window(obj, plan['target_public'])
            row.update(status='COMPILED', receipt=receipt, target_segment=window['segment'],
                       target_offset=window['start'], target_extent=len(window['bytes']),
                       target_bytes=identity(window['bytes']), target_fixups=window['fixups'],
                       target_window_limitation=window['limitation'],
                       oracle_literal_equal=window['bytes'] == oracle_bytes if oracle_bytes is not None else None,
                       all_publics=obj.publics, all_segment_defs=obj.segment_defs,
                       all_segment_lengths=obj.segment_lengths,
                       all_groups=obj.groups, all_externals=obj.externals,
                       local_symbol_records=obj.local_symbol_records,
                       all_fixups=obj.linker_fixups,
                       all_segment_identities={n: identity(bytes(v)) for n, v in obj.segments.items()})
            guards = [protected_window(obj, protection['public'],
                                       protected_oracles[protection['public']])
                      for protection in protections if name in protection['variants']]
            row['protected_neighbors'] = guards
            row['protected_neighbors_all_exact'] = (all(g['state'] == 'EXACT_LITERAL_WINDOW'
                                                         for g in guards) if guards else None)
            row['protected_component'] = (protected_component(obj, component_lock, component_oracle)
                                          if component_lock is not None and name in component_lock['variants']
                                          else None)
            (root/(name+'-target.bin')).write_bytes(window['bytes'])
            (root/(name+'-target.bin')).write_bytes(window['bytes'])
            work = Path(receipt['work_directory'])/'UNIT.OBJ'
            shutil.copyfile(work, root/(name+'.obj'))
        except CompileFailure as error:
            row.update(status='COMPILE_FAILURE', category=error.category,
                       error=str(error), receipt=error.receipt)
        rows.append(row)
        write_json(root/'results.json', {'authority': 'RESEARCH_ONLY', 'complete': False, 'variants': rows})
    baseline = next((r for r in rows if r['id'] == plan.get('baseline', labels[0])), None)
    require(baseline is not None, 'Baseline variant missing')
    for row in rows:
        if row['status'] == baseline['status'] == 'COMPILED':
            row['vs_baseline'] = {
                'bytes_equal': row['target_bytes'] == baseline['target_bytes'],
                'fixups_equal': row['target_fixups'] == baseline['target_fixups'],
                'extent_delta': row['target_extent'] - baseline['target_extent'],
                'offset_delta': row['target_offset'] - baseline['target_offset'],
                'other_publics_equal': [p for p in row['all_publics'] if p['name'] != plan['target_public']]
                                       == [p for p in baseline['all_publics'] if p['name'] != plan['target_public']]}
    groups = {}
    for row in rows:
        if row['status'] == 'COMPILED':
            key = json.dumps([row['target_bytes'], row['target_fixups']], sort_keys=True)
            groups.setdefault(sha(key.encode()), []).append(row['id'])
    report = {'authority': 'RESEARCH_ONLY', 'strict_acceptance': 'NOT_EVALUATED',
              'complete': True, 'question': plan['question'], 'prediction': plan['prediction'],
              'falsifier': plan['falsifier'], 'target_public': plan['target_public'],
              'target_source': identity(target), 'effective_target_groups': list(groups.values()),
              'protected_neighbor_policy': 'Exact literal public windows with no fixups; diagnostic only',
              'protected_neighbors': protections,
              'protected_component': component_lock,
              'variants': rows, 'note': 'Public-bounded target equality is diagnostic; complete TU, binding, neighbors and whole-image acceptance remain separate. OMF local PUBDEF/EXTDEF names appear in the parser public/external lists; local_symbol_records preserves their actual record kinds.'}
    write_json(root/'results.json', report)
    print(json.dumps({'report': str((root/'results.json').relative_to(ROOT)),
                      'effective_target_groups': report['effective_target_groups'],
                      'variants': [{'id': r['id'], 'status': r['status'],
                                    'target_extent': r.get('target_extent'),
                                    'oracle_literal_equal': r.get('oracle_literal_equal'),
                                    'protected_neighbors_all_exact': r.get('protected_neighbors_all_exact'),
                                    'protected_component_state': (r['protected_component']['state']
                                        if r.get('protected_component') else None),
                                    'vs_baseline': r.get('vs_baseline')} for r in rows]}, indent=2))
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('manifest')
    run(parser.parse_args().manifest)


if __name__ == '__main__':
    main()
