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
from transaction import (exclusive, ensure_consistent, prepare, apply, finish, rollback, recover,
                         invalidate_receipts, lock_free_snapshot, publishing)
from multi_contribution import checked_members
from preprocessor import prepare as prepare_source
from assembler import asm_source


def contribution_kind(recipe):
    if recipe.get('data_only'):
        return 'MATCHING_ASM_DATA' if recipe.get('kind') == 'asm' else 'MATCHING_C_DATA'
    return 'MATCHING_ASM' if recipe.get('kind', 'c') == 'asm' else 'MATCHING_C'


def replace_data_raw(manifest, recipe, oracle):
    """A complete data-only module can subsume complete accepted data owners."""
    result = copy.deepcopy(manifest)
    start, end = recipe['start'], recipe['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(overlaps and overlaps[0]['start'] <= start and end <= overlaps[-1]['end'],
            'Data-only interval is not covered')
    accepted = [o for o in overlaps if o['kind'] != 'UNRESOLVED_RAW']
    require(sorted(recipe.get('subsumed_data_owners', [])) == sorted(o['id'] for o in accepted),
            'Data-only subsumed owner list differs')
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    for owner in accepted:
        require(owner['kind'] == contribution_kind(recipe) and
                start <= owner['start'] < owner['end'] <= end and
                identity(image[owner['start']:owner['end']]) == owner['target'],
                'Data-only module crosses an unrelated accepted owner')
        if owner.get('module_form') == 'data-only':
            prior=read_json(ROOT/owner['recipe'])
            payload,_=probe(prior,oracle)
            require(payload==image[owner['start']:owner['end']],
                    'Subsumed data-only module no longer reproduces exactly')
            continue
        parent = [p for p in result['owners'] if p['id'] == owner['parent']]
        require(len(parent) == 1 and parent[0]['kind'] in ('MATCHING_C','MATCHING_ASM'),
                'Subsumed data owner lacks accepted source parent')
        prior = read_json(ROOT/parent[0]['recipe'])
        _, receipt = probe(prior, oracle)
        require(bytes.fromhex(receipt['binding']['secondary_payloads'][owner['segment']]) ==
                image[owner['start']:owner['end']],
                'Subsumed data owner no longer recompiles exactly')
        parent[0]['data_intervals'] = [i for i in parent[0]['data_intervals']
                                      if not (i['start'] == owner['start'] and i['end'] == owner['end'])]
    left, right = overlaps[0]['start'], overlaps[-1]['end']
    replacement=[]
    if left < start:
        require(overlaps[0]['kind']=='UNRESOLVED_RAW','Data-only start cuts accepted owner')
        replacement.append({**overlaps[0], 'end':start, 'id':f'raw_{left:05x}_{start:05x}'})
    kind=contribution_kind(recipe)
    replacement.append({'id':recipe['id'],'name':recipe['id'],'start':start,'end':end,
                        'kind':kind,'classification':'GAME_ASM' if kind=='MATCHING_ASM_DATA' else 'GAME_C',
                        'segment':recipe['object_segment'],'target':recipe['target'],
                        'recipe':'recipes/'+recipe['id']+'.json','module_form':'data-only',
                        **({'far_data':True} if recipe.get('far_data') is True else {})})
    if end < right:
        require(overlaps[-1]['kind']=='UNRESOLVED_RAW','Data-only end cuts accepted owner')
        replacement.append({**overlaps[-1], 'start':end, 'id':f'raw_{end:05x}_{right:05x}'})
    at=result['owners'].index(overlaps[0])
    result['owners'][at:at+len(overlaps)]=replacement
    return result


def _checked_prerender_table_boundary(f, inventory, image):
    """Independent pinned PROC/ENDP boundary for the complete CS dispatch table."""
    if f.get('name') != 'preRender_helper2':
        return False
    require((f['start'], f['end'], f['size'], f['status']) ==
            (137831, 138078, 247, 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED') and
            sha(image[f['start']:f['end']]) == f['sha256'] and
            f['segment_paragraph']*16 == 125472,
            'PreRender table extent/frame differs')
    provenance=f['provenance']
    path='src/restunts/asmorig/seg012.asm'
    require(provenance == {'repository':'restunts',
            'commit':'5c38f258f482e7d28f1ccce73da8992dccbbee89',
            'path':path,'line_start':6840,'line_end':6968},
            'PreRender table provenance differs')
    source=ROOT/'build/references/restunts'/path
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    require(identity(source.read_bytes()) == pinned,
            'PreRender table reference source differs from pinned identity')
    lines=source.read_text(encoding='latin1').splitlines()
    require(lines[6839].strip()=='preRender_helper2 proc near' and
            lines[6967].strip()=='preRender_helper2 endp' and
            lines[6968].strip()=='preRender_helper3 proc near' and
            not any(' proc ' in row.lower() or ' endp' in row.lower()
                    for row in lines[6840:6967]),
            'PreRender table source procedure boundary differs')
    require(any(other.get('name')=='preRender_helper3' and
                other.get('start')==f['end'] for other in inventory['functions']) and
            any(other.get('name')=='preRender_helper' and
                other.get('end')==f['start'] for other in inventory['functions']),
            'PreRender table neighboring mapped boundaries differ')
    return True


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
    # An emission-verified extent may instead be bounded on both sides by
    # instruction-verified neighbours (reviewed per recipe).
    bounded = (recipe.get('boundary_proof') == 'verified-neighbours-v1' and
               f['status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
               sha(image[f['start']:f['end']]) == f['sha256'] and
               any(g['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                   g.get('end') == f['start'] for g in inventory['functions']) and
               any(g['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                   g.get('start') == f['end'] for g in inventory['functions']))
    require(strong or emission or bounded or _checked_prerender_table_boundary(f, inventory, image),
            'ASM extent lacks reviewed anchors or complete emission bytes')
    stable = f.get('stable_id') or f"asm_load_{f['start']:05x}"
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
    require(isinstance(recipe.get('stable_id'), str) and recipe['stable_id'],
            'Owner identity needs a stable id')
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
    member_extents = {(m['start'], m.get('end'), m['name']) for m in recipe['members']}
    module = 'module_proof' in recipe
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    if module:
        from asm_module import checked_module, checked_cross_kind_link
        inventory = checked_module(recipe, image)
        entries = {m['start'] for m in recipe['members']}
    reclassified = []
    for owner in overlaps:
        if owner['kind'] == 'UNRESOLVED_RAW':
            continue
        require(start <= owner['start'] and owner['end'] <= end, 'Group crosses an accepted owner')
        if owner['kind'] != contribution_kind(recipe):
            # Only a grounded whole ASM module may reclassify an accepted C
            # owner inside it, with a rechecked same-module link.
            require(module and owner['kind'] == 'MATCHING_C' and
                    contribution_kind(recipe) == 'MATCHING_ASM',
                    'Group crosses an accepted owner')
            link = checked_cross_kind_link(owner, recipe, image, inventory)
            reclassified.append({'id': owner['id'], 'name': owner['name'],
                                 'from': owner['kind'], 'recipe': owner['recipe'],
                                 'link': link['kind']})
        prior = read_json(ROOT/owner['recipe'])
        require(prior['start'] == owner['start'] and prior['end'] == owner['end'] and
                prior['id'] == owner['name'], 'Subsumed owner recipe differs')
        if module:
            # A whole module keeps each subsumed owner on grounded entries.
            ends = {f['end'] for f in inventory['functions']
                    if f.get('status') in ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                                           'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')}
            require(owner['start'] in entries and
                    (owner['end'] in entries or owner['end'] == end or owner['end'] in ends),
                    'Subsumed owner does not start/end at module entries')
        elif 'members' in prior:
            # A previously accepted group can be enlarged only by retaining
            # its entire ordered member interval and every member identity.
            prior_members=prior['members']
            members=recipe['members']
            matching=[i for i in range(len(members)-len(prior_members)+1)
                      if members[i:i+len(prior_members)]==prior_members]
            require(len(matching)==1 and
                    prior_members[0]['start']==owner['start'] and
                    prior_members[-1]['end']==owner['end'],
                    'Group changes a subsumed accepted group member')
        else:
            require((owner['start'], owner['end'], owner['name']) in member_extents,
                    'Group crosses an accepted owner without exact member extent')
        payload, _ = probe(prior, oracle)
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
    row = {'id':recipe['id'], 'name':recipe['id'], 'start':start, 'end':end,
           'kind':kind, 'classification':'GAME_ASM' if kind == 'MATCHING_ASM' else 'GAME_C',
           'recipe':'recipes/'+recipe['id']+'.json'}
    if module:
        row['module_form'] = 'asm-module'
    if reclassified:
        # Reclassification evidence; the retired C source stays unowned.
        row['reclassified_owners'] = reclassified
    replacement.append(row)
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
        require(overlaps and overlaps[0]['start']<=start and end<=overlaps[-1]['end'],
                'Secondary interval is not wholly covered')
        accepted=[o for o in overlaps if o['kind']!='UNRESOLVED_RAW']
        for prior in accepted:
            require(prior['kind']==prior_kind and prior['segment']==segment and
                    start<=prior['start']<prior['end']<=end and
                    prior['id'] in recipe.get('subsumed_data_owners',[]) and
                    identity(image[prior['start']:prior['end']])==prior['target'],
                    'Secondary accepted owner is not wholly subsumed')
            if (prior.get('parent') == parent['id'] and
                    (prior['start'],prior['end'],prior['target']) ==
                    (start,end,spec['target'])):
                subsumed.append(prior['id'])
                continue
            if prior.get('module_form') == 'data-only':
                old_recipe=read_json(ROOT/prior['recipe'])
                payload,_=probe(old_recipe)
                require(payload==image[prior['start']:prior['end']],
                        'Subsumed data-only module no longer reproduces exactly')
                subsumed.append(prior['id'])
                continue
            old_parent=[o for o in result['owners'] if o['id']==prior['parent']]
            if not old_parent:
                require(prior['parent'] in recipe.get('subsumed_owners', []),
                        'Secondary subsumed owner has no CODE parent')
                old_parent=[o for o in read_json(ROOT/'layout/manifest.json')['owners']
                            if o['id']==prior['parent']]
                removed_parent=True
            else:
                removed_parent=False
            require(len(old_parent)==1 and old_parent[0]['kind'] in ('MATCHING_C','MATCHING_ASM'),
                    'Secondary subsumed CODE parent differs')
            prior_recipe=read_json(ROOT/old_parent[0]['recipe'])
            _, prior_receipt=probe(prior_recipe)
            require(bytes.fromhex(prior_receipt['binding']['secondary_payloads'][segment])==
                    image[prior['start']:prior['end']],
                    'Subsumed secondary owner no longer recompiles exactly')
            if not removed_parent:
                old_parent[0]['data_intervals']=[i for i in old_parent[0]['data_intervals']
                                                 if not (i['start']==prior['start'] and i['end']==prior['end'])]
            subsumed.append(prior['id'])
        replacement=[]
        if overlaps[0]['start']<start:
            require(overlaps[0]['kind']=='UNRESOLVED_RAW','Secondary start cuts accepted owner')
            replacement.append({**overlaps[0],'end':start,
                                'id':f"raw_{overlaps[0]['start']:05x}_{start:05x}"})
        kind = 'MATCHING_ASM_DATA' if recipe.get('kind') == 'asm' else 'MATCHING_C_DATA'
        replacement.append({'id':f"{recipe['id']}:{segment}", 'kind':kind,
                            'classification':'GAME_ASM' if kind == 'MATCHING_ASM_DATA' else 'GAME_C',
                            'parent':parent['id'], 'segment':segment,'start':start,'end':end,
                            'target':spec['target']})
        if end<overlaps[-1]['end']:
            require(overlaps[-1]['kind']=='UNRESOLVED_RAW','Secondary end cuts accepted owner')
            replacement.append({**overlaps[-1],'start':end,
                                'id':f"raw_{end:05x}_{overlaps[-1]['end']:05x}"})
        at=partition.index(overlaps[0]); partition[at:at+len(overlaps)]=replacement
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


def _stage(name, candidate, source, recipe_path, recipe_data, before, verify_only):
    """Complete staged acceptance; reads canonical state, writes nothing canonical."""
    manifest = read_json(ROOT/'layout/manifest.json')
    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    import json
    recipe = json.loads(recipe_data) if recipe_data is not None else default_recipe(name, image, oracle[2]['unpacked_mz']['relocations'])
    kind = recipe.get('kind', 'c')
    require(kind in ('c', 'asm'), 'Unknown contribution kind')
    data_only = recipe.get('data_only') is True
    multi = 'members' in recipe
    if data_only:
        require(not multi and recipe['id'] == name and
                recipe['target'] == identity(image[recipe['start']:recipe['end']]) and
                (recipe['object_segment'] == '_DATA' or recipe.get('far_data') is True),
                'Data-only module identity/extent differs from oracle')
        if kind == 'asm':
            require(recipe.get('positive_asm_evidence'),
                    'ASM data classification lacks positive evidence')
    elif multi:
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
        staged_manifest = (replace_data_raw(manifest, recipe, oracle) if data_only else
                           replace_group(manifest, recipe, oracle) if multi else
                           replace_raw(manifest, recipe))
        if not data_only:
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
    return report, destination, recipe, staged_manifest


def promote(name, candidate, recipe_path=None, verify_only=False):
    require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name), 'Unsafe function name')
    candidate = Path(candidate).resolve()
    source = candidate.read_bytes()
    recipe_data = Path(recipe_path).read_bytes() if recipe_path else None
    if verify_only:
        # No exclusive writer lock: a consistent snapshot of canonical inputs
        # (seqlock generation + input identities) is enough for a read-only check.
        report = lock_free_snapshot(
            lambda before: _stage(name, candidate, source, recipe_path, recipe_data, before, True)[0],
            inputs)
        write_json(ROOT/'build/acceptance'/name/'report.json', report)
        return report
    with exclusive():
        ensure_consistent()
        before = inputs()
        report, destination, recipe, staged_manifest = _stage(
            name, candidate, source, recipe_path, recipe_data, before, False)
        changes = {destination:source, 'recipes/'+name+'.json':json_bytes(recipe),
                   'layout/manifest.json':json_bytes(staged_manifest)}
        expected = {**before, **{p:sha(raw) for p,raw in changes.items()}}
        with publishing():
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
