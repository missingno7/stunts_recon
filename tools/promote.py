"""Fresh strict source acceptance and serialized, recoverable publication."""
import argparse
import copy
import re
from pathlib import Path
from common import ROOT, read_json, require, identity, sha, json_bytes, write_json, atomic_bytes
from build_exact import build, inputs
from probe_module import probe
from oracle import verify
from mz import MZ
from transaction import exclusive, ensure_consistent, prepare, apply, finish, rollback, recover, invalidate_receipts


def replace_raw(manifest, recipe):
    result = copy.deepcopy(manifest)
    start, end = recipe['start'], recipe['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(len(overlaps) == 1 and overlaps[0]['kind'] == 'UNRESOLVED_RAW', 'Candidate is not wholly raw-owned')
    old = overlaps[0]
    require(old['start'] <= start < end <= old['end'], 'Candidate crosses ownership')
    split = []
    if old['start'] < start:
        split.append({**old, 'end':start, 'id':f"raw_{old['start']:05x}_{start:05x}"})
    split.append({'id':recipe['stable_id'], 'name':recipe['id'], 'start':start, 'end':end,
                  'kind':'MATCHING_C', 'classification':'GAME_C', 'recipe':'recipes/'+recipe['id']+'.json'})
    if end < old['end']:
        split.append({**old, 'start':end, 'id':f"raw_{end:05x}_{old['end']:05x}"})
    at = result['owners'].index(old)
    result['owners'][at:at+1] = split
    return result


def checked_function(name, recipe, image):
    inventory = read_json(ROOT/'evidence/functions.json')
    require(inventory['load_sha256'] == sha(image), 'Function inventory belongs to another oracle')
    found = [f for f in inventory['functions'] if f.get('name') == name]
    require(len(found) == 1, 'Function name missing/ambiguous in original evidence')
    function = found[0]
    require(function['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
            'Strict acceptance needs independently verified complete function boundaries')
    require(all(recipe.get(k) == function.get(k) for k in ('stable_id','start','end'))
            and recipe['id'] == function['name']
            and recipe['target'] == {'size':function['size'], 'sha256':function['sha256']},
            'Recipe differs from independent function evidence')
    require(identity(image[function['start']:function['end']]) == recipe['target'], 'Original extent changed')
    # Explicit reviewed overlays retain their independent CFG and anchor checks.
    from function_evidence import reviewed_functions
    reviewed_functions(image)
    return function


def default_recipe(name, image, relocations):
    existing = ROOT/'recipes'/(name+'.json')
    if existing.exists():
        return read_json(existing)
    functions = read_json(ROOT/'evidence/functions.json')['functions']
    found = [f for f in functions if f.get('name') == name]
    require(len(found) == 1, 'Function missing/ambiguous')
    f = found[0]
    require(f.get('stable_id') and f.get('end') is not None, 'No established extent; search remains available')
    return {'id':name, 'stable_id':f['stable_id'], 'start':f['start'], 'end':f['end'],
            'source':'src/'+name+'.c', 'profile':'msc510-medium', 'object_segment':'UNIT_TEXT',
            'public':'_'+name, 'target':identity(image[f['start']:f['end']]),
            'expected_fixups':[], 'expected_relocations':[r for r in relocations if f['start']-1 <= r['load_offset'] < f['end']],
            'evidence':f['provenance']}


def promote(name, candidate, recipe_path=None, verify_only=False):
    require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name), 'Unsafe function name')
    candidate = Path(candidate).resolve()
    source = candidate.read_bytes()
    recipe_data = Path(recipe_path).read_bytes() if recipe_path else None
    with exclusive():
        ensure_consistent()
        before = inputs()
        manifest = read_json(ROOT/'layout/manifest.json')
        oracle = verify(write=False)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
        import json
        recipe = json.loads(recipe_data) if recipe_data is not None else default_recipe(name, image, oracle[2]['unpacked_mz']['relocations'])
        checked_function(name, recipe, image)
        destination = 'src/'+name+'.c'
        recipe = {**recipe, 'source':destination}
        payload, fast = probe(recipe, oracle, source_override=source)
        active = [o for o in manifest['owners'] if o['kind'] == 'MATCHING_C' and o.get('name') == name]
        if active:
            require(len(active) == 1 and active[0]['recipe'] == 'recipes/'+name+'.json'
                    and (active[0]['start'],active[0]['end']) == (recipe['start'],recipe['end']),
                    'Existing ownership cannot change')
            staged_manifest = manifest
        else:
            require(not (ROOT/destination).exists() and not (ROOT/'recipes'/(name+'.json')).exists(),
                    'New publication would overwrite existing unowned files')
            staged_manifest = replace_raw(manifest, recipe)
        # Recompile every existing contribution, including runtime binding, with
        # only the target source supplied from frozen bytes. No canonical writes.
        staged = build(staged_manifest, {'recipes/'+name+'.json':recipe}, publish=False,
                       source_overrides={destination:source})
        require(inputs() == before and staged['inputs'] == before, 'Canonical inputs changed during acceptance')
        require(candidate.read_bytes() == source, 'Candidate source changed during acceptance')
        if recipe_path:
            require(Path(recipe_path).read_bytes() == recipe_data, 'Candidate recipe changed during acceptance')
        report = {'status':'VERIFIED_ONLY' if verify_only else 'PROMOTED', 'function':name,
                  'source':identity(source), 'bytes':len(payload), 'whole_image':staged['executable'],
                  'relocation_count':staged['relocation_count'], 'fast':fast, 'inputs':before}
        if verify_only:
            write_json(ROOT/'build/acceptance'/name/'report.json', report)
            return report
        changes = {destination:source, 'recipes/'+name+'.json':json_bytes(recipe),
                   'layout/manifest.json':json_bytes(staged_manifest)}
        expected = {**before, **{p:sha(raw) for p,raw in changes.items()}}
        rows = prepare(changes)
        try:
            require(inputs() == before, 'Canonical inputs changed before publication')
            invalidate_receipts()
            apply(rows)
            require(inputs() == expected, 'Unexpected edits during publication')
            artifact = {}
            accepted = build(publish=False, allow_pending=True, artifact=artifact)
            require(inputs() == expected and accepted['inputs'] == expected, 'Unexpected edits during canonical verification')
            require(candidate.read_bytes() == source, 'Candidate source changed before publication completed')
            if recipe_path:
                require(Path(recipe_path).read_bytes() == recipe_data, 'Candidate recipe changed before publication completed')
            finish()
            require(inputs() == expected, 'Inputs changed before acceptance receipt publication')
            atomic_bytes(ROOT/'build/exact/mcga.exe', artifact['executable'])
            write_json(ROOT/'build/exact/acceptance.json', accepted)
            require(inputs() == expected, 'Inputs changed while publishing acceptance receipt')
        except BaseException:
            invalidate_receipts()
            rollback()
            raise
        report['inputs'] = expected
        write_json(ROOT/'build/acceptance'/name/'report.json', report)
        return report


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('function', nargs='?'); p.add_argument('candidate', nargs='?', type=Path)
    p.add_argument('--recipe', type=Path, help='Explicit reviewed binding recipe; optional for no-fixup contributions')
    p.add_argument('--verify-only', action='store_true'); p.add_argument('--recover', action='store_true')
    a = p.parse_args()
    if a.recover:
        if a.function or a.candidate or a.recipe: p.error('--recover takes no candidate')
        recover(); print('Recovery complete; run python tools/validate.py'); return
    if not a.function or not a.candidate: p.error('Supply FUNCTION CANDIDATE.c')
    report = promote(a.function, a.candidate, a.recipe, a.verify_only)
    print(report['status'], report['function'], report['bytes'], 'bytes; fresh HYBRID_EXACT and ordered relocations')


if __name__ == '__main__': main()
