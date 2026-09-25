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
from multi_contribution import checked_members
from preprocessor import prepare as prepare_source
from assembler import asm_source


def contribution_kind(recipe):
    return 'MATCHING_ASM' if recipe.get('kind', 'c') == 'asm' else 'MATCHING_C'


def checked_asm_function(name, recipe, image):
    """ASM boundaries may be proved by complete emission bytes and both anchors."""
    from function_evidence import current_inventory, reviewed_functions
    inventory = current_inventory(image)
    require(inventory['load_sha256'] == sha(image), 'ASM inventory/oracle identity differs')
    rows = [f for f in inventory['functions'] if f.get('name') == name]
    require(len(rows) == 1, 'ASM name missing/ambiguous in original evidence')
    f = rows[0]
    strong = f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
    emission = (f['status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED'
                and f.get('start_evidence') and f.get('end_evidence')
                and sha(image[f['start']:f['end']]) == f['sha256']
                and (not f.get('bytes_hex') or
                     bytes.fromhex(f['bytes_hex']) == image[f['start']:f['end']]))
    require(strong or emission, 'ASM extent lacks reviewed anchors or complete emission bytes')
    stable = f.get('stable_id', f"asm_load_{f['start']:05x}")
    require(recipe.get('stable_id') == stable and recipe['id'] == name
            and (recipe['start'], recipe['end']) == (f['start'], f['end'])
            and recipe['target'] == {'size': f['size'], 'sha256': f['sha256']}
            and recipe['public'] == '_' + name,
            'ASM recipe differs from independent function evidence')
    if 'original_frame_load_address' in recipe:
        require(recipe['original_frame_load_address'] == f['segment_paragraph']*16,
                'ASM original frame differs from independent segment map')
    require(identity(image[f['start']:f['end']]) == recipe['target'], 'ASM original extent changed')
    reviewed_functions(image)
    return f


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
    kind = contribution_kind(recipe)
    split.append({'id':recipe['stable_id'], 'name':recipe['id'], 'start':start, 'end':end,
                  'kind':kind, 'classification':'GAME_ASM' if kind == 'MATCHING_ASM' else 'GAME_C',
                  'recipe':'recipes/'+recipe['id']+'.json'})
    if end < old['end']:
        split.append({**old, 'start':end, 'id':f"raw_{end:05x}_{old['end']:05x}"})
    at = result['owners'].index(old)
    result['owners'][at:at+1] = split
    return result


def replace_group(manifest, recipe, oracle):
    """Replace one contiguous interval, preserving any accepted members by proof."""
    result = copy.deepcopy(manifest)
    start, end = recipe['start'], recipe['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(overlaps and overlaps[0]['start'] <= start and end <= overlaps[-1]['end'],
            'Group interval is not covered by existing owners')
    accepted = [o for o in overlaps if o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    require(sorted(recipe.get('subsumed_owners', [])) == sorted(o['id'] for o in accepted) and
            len(recipe.get('subsumed_owners', [])) == len(accepted),
            'Group must record exactly its subsumed C owners')
    member_extents = {(m['start'], m['end'], m['name']) for m in recipe['members']}
    for owner in overlaps:
        if owner['kind'] == 'UNRESOLVED_RAW':
            continue
        require(owner['kind'] == contribution_kind(recipe) and start <= owner['start'] and owner['end'] <= end
                and (owner['start'], owner['end'], owner['name']) in member_extents,
                'Group crosses an accepted owner without exact member extent')
        prior = read_json(ROOT/owner['recipe'])
        require(prior['start'] == owner['start'] and prior['end'] == owner['end'] and
                prior['id'] == owner['name'], 'Subsumed owner recipe differs')
        payload, _ = probe(prior, oracle)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
        require(payload == image[owner['start']:owner['end']],
                'Subsumed owner no longer reproduces its exact bytes')
    left = overlaps[0]['start']
    right = overlaps[-1]['end']
    replacement = []
    if left < start:
        require(overlaps[0]['kind'] == 'UNRESOLVED_RAW', 'Group cuts accepted owner at start')
        replacement.append({**overlaps[0], 'end':start,
                            'id':f"raw_{left:05x}_{start:05x}"})
    kind = contribution_kind(recipe)
    replacement.append({'id':recipe['id'], 'name':recipe['id'], 'start':start, 'end':end,
                        'kind':kind, 'classification':'GAME_ASM' if kind == 'MATCHING_ASM' else 'GAME_C',
                        'recipe':'recipes/'+recipe['id']+'.json'})
    if end < right:
        require(overlaps[-1]['kind'] == 'UNRESOLVED_RAW', 'Group cuts accepted owner at end')
        replacement.append({**overlaps[-1], 'start':end,
                            'id':f"raw_{end:05x}_{right:05x}"})
    first = result['owners'].index(overlaps[0])
    result['owners'][first:first+len(overlaps)] = replacement
    return result


def attach_secondary(manifest, recipe, image):
    """Assign complete emitted DGROUP intervals to the owning C recipe."""
    result = copy.deepcopy(manifest)
    specs = recipe.get('secondary_dgroup_segments', {})
    if not specs:
        return result
    parents = [o for o in result['owners'] if o['kind']==contribution_kind(recipe) and
               o.get('name')==recipe['id'] and
               (o['start'],o['end'])==(recipe['start'],recipe['end'])]
    require(len(parents)==1, 'Secondary contribution lacks unique CODE owner')
    parent = parents[0]
    intervals = []
    subsumed=[]
    # The frame/range is reviewed independently of the candidate recipe.
    layout = read_json(ROOT/'layout/data-symbols.json')
    for segment, spec in sorted(specs.items(), key=lambda pair:pair[1]['start']):
        start,end=spec['start'],spec['end']
        require(segment in ('_DATA','CONST','_BSS') and
                type(start)is int and type(end)is int and start<end and
                start==layout['frame_load_address']+spec['dgroup_offset'],
                'Invalid secondary owner interval')
        row={'segment':segment,'start':start,'end':end,'target':spec['target']}
        intervals.append(row)
        if segment=='_BSS':
            require(layout['bss_start']<=start<end<=layout['bss_end'],
                    'BSS ownership outside verified clear range')
            if 'bss_owners' not in result:
                result['bss_owners']=[{'id':'raw_bss','kind':'UNRESOLVED_RAW',
                                       'start':layout['bss_start'],'end':layout['bss_end']}]
            partition=result['bss_owners']
        else:
            require(end<=len(image) and identity(image[start:end])==spec['target'],
                    'Secondary initialized owner differs from oracle')
            partition=result['owners']
        overlaps=[o for o in partition if o['start']<end and start<o['end']]
        prior_kind = 'MATCHING_ASM_DATA' if recipe.get('kind') == 'asm' else 'MATCHING_C_DATA'
        if len(overlaps)==1 and overlaps[0]['kind']==prior_kind:
            prior=overlaps[0]
            require((prior['start'],prior['end'],prior['segment'])==(start,end,segment)
                    and prior['id'] in recipe.get('subsumed_data_owners',[]) and
                    prior['target']==spec['target'],
                    'Secondary accepted owner is not exactly subsumed')
            subsumed.append(prior['id'])
            prior['parent']=parent['id']
        else:
            require(len(overlaps)==1 and overlaps[0]['kind']=='UNRESOLVED_RAW' and
                    overlaps[0]['start']<=start<end<=overlaps[0]['end'],
                    'Secondary interval is not wholly raw-owned')
            old=overlaps[0]
            replacement=[]
            if old['start']<start:
                replacement.append({**old,'end':start,
                                    'id':f"raw_{old['start']:05x}_{start:05x}"})
            kind = 'MATCHING_ASM_DATA' if recipe.get('kind') == 'asm' else 'MATCHING_C_DATA'
            replacement.append({'id':f"{recipe['id']}:{segment}", 'kind':kind,
                                'classification':'GAME_ASM' if kind == 'MATCHING_ASM_DATA' else 'GAME_C',
                                'parent':parent['id'],
                                'segment':segment,'start':start,'end':end,
                                'target':spec['target']})
            if end<old['end']:
                replacement.append({**old,'start':end,
                                    'id':f"raw_{end:05x}_{old['end']:05x}"})
            at=partition.index(old); partition[at:at+1]=replacement
    parent['data_intervals']=intervals
    require(sorted(recipe.get('subsumed_data_owners',[]))==sorted(subsumed),
        'Secondary subsumed owner list differs')
    return result


def checked_function(name, recipe, image):
    from function_evidence import current_inventory
    inventory = current_inventory(image)
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
        kind = recipe.get('kind', 'c')
        require(kind in ('c', 'asm'), 'Unknown contribution kind')
        multi = 'members' in recipe
        if multi:
            require(recipe['id'] == name, 'Multi recipe group ID differs')
            checked_members(recipe, image)
        else:
            (checked_asm_function if kind == 'asm' else checked_function)(name, recipe, image)
        destination = ('asm/'+name+'.ASM' if kind == 'asm' else 'src/'+name+'.c')
        recipe = {**recipe, 'source':destination}
        if kind == 'asm':
            asm_source(source)
            require(recipe['profile'] == 'masm510-game', 'ASM profile differs')
            from compiler import verify_toolchain
            require(recipe.get('assembler_flags') == verify_toolchain(recipe['profile'])[0]['flags'],
                    'ASM recipe flags differ from pinned profile')
            recipe.setdefault('include_closure', [])
        else:
            closure = prepare_source(source, recipe['profile'])[1]
            if 'preprocessor_closure' not in recipe:
                recipe['preprocessor_closure'] = closure
        payload, fast = probe(recipe, oracle, source_override=source)
        active = [o for o in manifest['owners'] if o['kind'] == contribution_kind(recipe) and o.get('name') == name]
        if active:
            require(len(active) == 1 and active[0]['recipe'] == 'recipes/'+name+'.json'
                    and (active[0]['start'],active[0]['end']) == (recipe['start'],recipe['end']),
                    'Existing ownership cannot change')
            staged_manifest = manifest
        else:
            require(not (ROOT/destination).exists() and not (ROOT/'recipes'/(name+'.json')).exists(),
                    'New publication would overwrite existing unowned files')
            staged_manifest = replace_group(manifest, recipe, oracle) if multi else replace_raw(manifest, recipe)
            staged_manifest = attach_secondary(staged_manifest, recipe, image)
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
