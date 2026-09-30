"""Fresh strict source acceptance and serialized, recoverable publication."""
import argparse
import copy
import re
from pathlib import Path
from pathlib import PurePosixPath
from common import ROOT, read_json, require, identity, sha, json_bytes, write_json, atomic_bytes
from build_exact import build, inputs
from probe_module import probe
from oracle import verify
from mz import MZ
from transaction import (exclusive, ensure_consistent, prepare, apply, finish, rollback, recover,
                         invalidate_receipts, lock_free_snapshot, publishing)
from multi_contribution import checked_members
from asm_module import registry_publics
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
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    from pinned_reference import check_reference_identity, reference_line, reference_rows
    require(check_reference_identity(path) == pinned,
            'PreRender table reference source differs from pinned identity')
    interior = reference_rows(path,6841,6967)
    require(reference_line(path,6840).strip()=='preRender_helper2 proc near' and
            reference_line(path,6968).strip()=='preRender_helper2 endp' and
            reference_line(path,6969).strip()=='preRender_helper3 proc near' and
            not any(re.search(r'\b(?:proc|endp)\b', row, re.I) for _, row in interior),
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
            and (recipe['public'] == '_' + name or recipe['public'] in registry_publics(name)),
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
        elif 'prefix_of_object' in prior:
            # Monotonic subsumption: a complete object from the same object
            # start replaces its record-closed prefix. The new contribution is
            # verified on its own; the prefix proof is never consulted for it.
            require(owner['start'] == start and
                    prior['prefix_of_object']['object_start'] == start,
                    'Group does not start at the subsumed prefix object start')
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


def replace_prefix(manifest, recipe, oracle):
    """Own one record-closed prefix of a candidate object (see prefix_proof)."""
    result = copy.deepcopy(manifest)
    start, end = recipe['start'], recipe['end']
    overlaps = [o for o in result['owners'] if o['start'] < end and start < o['end']]
    require(overlaps and overlaps[0]['start'] <= start and end <= overlaps[-1]['end'],
            'Prefix interval is not covered by existing owners')
    accepted = [o for o in overlaps if o['kind'] != 'UNRESOLVED_RAW']
    require(sorted(recipe.get('subsumed_owners', [])) == sorted(o['id'] for o in accepted) and
            len(recipe.get('subsumed_owners', [])) == len(accepted),
            'Prefix must record exactly its subsumed owners')
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    complete = {(m['start'], m['end'], m['name']) for m in recipe['prefix_members'] if m['complete']}
    for owner in accepted:
        require(owner['kind'] == 'MATCHING_C' and start <= owner['start'] and owner['end'] <= end,
                'Prefix crosses an accepted owner')
        prior = read_json(ROOT/owner['recipe'])
        require(prior['start'] == owner['start'] and prior['end'] == owner['end'] and
                prior['id'] == owner['name'], 'Subsumed owner recipe differs')
        if 'prefix_of_object' in prior:
            require(owner['start'] == start and prior['prefix_of_object']['object_start'] == start,
                    'Only a longer prefix of the same object subsumes a prefix owner')
        elif 'members' in prior:
            require(all((m['start'], m['end'], m['name']) in complete for m in prior['members']),
                    'Prefix changes a subsumed accepted group member')
        else:
            require((owner['start'], owner['end'], owner['name']) in complete,
                    'Prefix crosses an accepted owner without exact member extent')
        payload, _ = probe(prior, oracle)
        require(payload == image[owner['start']:owner['end']],
                'Subsumed owner no longer reproduces its exact bytes')
    left, right = overlaps[0]['start'], overlaps[-1]['end']
    replacement = []
    if left < start:
        require(overlaps[0]['kind'] == 'UNRESOLVED_RAW', 'Prefix cuts accepted owner at start')
        replacement.append({**overlaps[0], 'end':start, 'id':f"raw_{left:05x}_{start:05x}"})
    replacement.append({'id':recipe['id'], 'name':recipe['id'], 'start':start, 'end':end,
                        'kind':'MATCHING_C', 'classification':'GAME_C',
                        'recipe':'recipes/'+recipe['id']+'.json',
                        'contribution_form':'prefix_of_object', 'object_start':start,
                        'prefix_records':recipe['prefix_of_object']['records']})
    if end < right:
        require(overlaps[-1]['kind'] == 'UNRESOLVED_RAW', 'Prefix cuts accepted owner at end')
        replacement.append({**overlaps[-1], 'start':end, 'id':f"raw_{end:05x}_{right:05x}"})
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
            require('bss_owners' in result, 'BSS ownership partition missing')
            partition=result['bss_owners']
        else:
            require(end<=len(image) and identity(image[start:end])==spec['target'],
                    'Secondary initialized owner differs from oracle')
            partition=result['owners']
        overlaps=[o for o in partition if o['start']<end and start<o['end']]
        if segment=='_BSS':
            # integ35: raw BSS debt is one placeholder per object; a static claim
            # replaces exactly its own object's whole placeholder (bss_link).
            import bss_link
            objects=read_json(ROOT/'layout/link-objects.json')['objects']
            host=[o['id'] for o in objects if o['start']<=parent['start']<o['end']]
            require(len(overlaps)==1 and (overlaps[0]['start'],overlaps[0]['end'])==(start,end) and
                    ((overlaps[0]['kind']=='UNRESOLVED_RAW' and
                      overlaps[0].get('raw_form')==bss_link.OBJECT_BSS and
                      host==[overlaps[0].get('object')]) or
                     (overlaps[0]['kind']!='UNRESOLVED_RAW' and overlaps[0].get('parent')==parent['id'])),
                    'BSS claim does not replace exactly its own object raw placeholder')
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
        row={'id':f"{recipe['id']}:{segment}", 'kind':kind,
             'classification':'GAME_ASM' if kind == 'MATCHING_ASM_DATA' else 'GAME_C',
             'parent':parent['id'], 'segment':segment,'start':start,'end':end,
             'target':spec['target']}
        if segment=='_BSS':
            # integ32: static storage placed by LINK module order; publication
            # and validation require the real-link proof (tools/bss_link.py).
            import bss_link
            row['placement']=bss_link.STATIC
        replacement.append(row)
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


def check_candidate(name, source, recipe_data, manifest, oracle, image):
    """Fresh individual verification of one candidate against `manifest`.

    Compiles/assembles the candidate in its own fresh probe, checks its complete
    contribution, and derives the ownership change (including fresh probes of
    every subsumed owner).  Writes nothing canonical."""
    import json
    recipe = json.loads(recipe_data) if recipe_data is not None else default_recipe(name, image, oracle[2]['unpacked_mz']['relocations'])
    kind = recipe.get('kind', 'c')
    require(kind in ('c', 'asm'), 'Unknown contribution kind')
    data_only = recipe.get('data_only') is True
    multi = 'members' in recipe
    prefix = 'prefix_of_object' in recipe
    if prefix:
        # Proof inputs only; probe() re-derives the record-closed prefix.
        from prefix_proof import check_recipe_form
        check_recipe_form(recipe)
        require(recipe['id'] == name and kind == 'c', 'Prefix recipe id/kind differs')
    elif data_only:
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
        from assembler import prepare_asm
        _, closure = prepare_asm(source)
        require(recipe['profile'] == 'masm510-game', 'ASM profile differs')
        from compiler import verify_toolchain
        require(recipe.get('assembler_flags') == verify_toolchain(recipe['profile'])[0]['flags'],
                'ASM recipe flags differ from pinned profile')
        if 'include_closure' not in recipe:
            recipe['include_closure'] = closure
        require(recipe['include_closure'] == closure,
                'ASM recipe include closure differs from tracked include files')
    else:
        closure = prepare_source(source, recipe['profile'])[1]
        if 'preprocessor_closure' not in recipe:
            recipe['preprocessor_closure'] = closure
    payload, fast = probe(recipe, oracle, source_override=source)
    staged_manifest = apply_ownership(manifest, name, recipe, oracle, image)
    return recipe, destination, payload, fast, staged_manifest


def _cosmetic_compile(recipe, source):
    """Return the full pinned-tool OMF bytes for one accepted recipe/source.

    Cosmetic republication deliberately compares the complete object file,
    including declarations, record ordering, FIXUPPs, and checksums. It does
    not use the bound CODE payload as a proxy for object identity.
    """
    from communal_unit import recipe_declarations
    from object_probe import recipe_sparse_zero
    from assembler import assemble_source
    from compiler import compile_source
    from object_flags import recipe_flags, object_control_flags, same_object

    declared = recipe_declarations(recipe)
    communals = None if declared is None else [name for name, _ in declared]
    if recipe.get('kind', 'c') == 'asm':
        from compiler import verify_toolchain
        require(recipe.get('profile') == 'masm510-game', 'Cosmetic ASM profile differs')
        require(recipe.get('assembler_flags') == verify_toolchain(recipe['profile'])[0]['flags'],
                'Cosmetic ASM flags differ from pinned profile')
        obj, receipt = assemble_source(source, recipe['profile'], communals=communals)
        control = None
    else:
        sparse = recipe_sparse_zero(recipe)
        flags = recipe_flags(recipe)
        obj, receipt = compile_source(source, recipe['profile'], flags,
                                     sparse_zero=sparse, communals=communals)
        control = object_control_flags(recipe)
        if control is not None:
            controlled, controlled_receipt = compile_source(
                source, recipe['profile'], control, sparse_zero=sparse, communals=communals)
            require(same_object(controlled, obj),
                    'Cosmetic object differs under its registered control flags')
            # Keep the full object identity that the pinned production-profile
            # compile emitted; retain the independent control receipt as proof.
            receipt['object_flag_control'] = {'flags': control,
                                              'object': controlled_receipt['object']}
    from pathlib import Path
    raw_path = Path(receipt['work_directory']) / 'UNIT.OBJ'
    raw = raw_path.read_bytes()
    require(identity(raw) == receipt['object'], 'Pinned-tool OBJ receipt identity differs')
    return raw


def _cosmetic_closure(recipe, source):
    """Resolve a source's literal dependencies using the accepted frontend."""
    if recipe.get('kind', 'c') == 'asm':
        from assembler import prepare_asm
        return 'include_closure', prepare_asm(source)[1]
    return 'preprocessor_closure', prepare_source(source, recipe['profile'])[1]


def _cosmetic_closure_update(recipe_path, recipe, old_source, new_source):
    field, old_closure = _cosmetic_closure(recipe, old_source)
    accepted_closure = recipe.get(field, [])
    require(accepted_closure == old_closure,
            f'Accepted {field} differs from its current source: {recipe_path}')
    _, new_closure = _cosmetic_closure(recipe, new_source)
    return ({recipe_path: {field: new_closure}} if new_closure != accepted_closure else {})


def _cosmetic_source_path(value):
    """A canonical source path selects the module-source cosmetic form."""
    if not isinstance(value, str) or ('/' not in value and '\\' not in value):
        return None
    path = PurePosixPath(value.replace('\\', '/'))
    require(not path.is_absolute() and len(path.parts) == 2 and
            path.parts[0] in ('asm', 'src') and
            ((path.parts[0] == 'asm' and path.suffix == '.ASM') or
             (path.parts[0] == 'src' and path.suffix == '.c')),
            'Cosmetic source target must be one canonical asm/*.ASM or src/*.c path')
    return path.as_posix()


def check_cosmetic_source(source_path, candidate, source, before):
    """Prove a complete source-file edit preserves each full recipe object.

    Grouped ASM owners have one active module recipe while their constituent
    function recipes may be inactive. Checking by source path makes the complete
    module the unit of comparison and records the active owners separately.
    Frontend-derived include closure updates may accompany the source change.
    """
    source_path = _cosmetic_source_path(source_path)
    require(source_path is not None, 'Cosmetic source target is not a canonical source path')
    candidate_path = Path(candidate).resolve()
    current_path = (ROOT/source_path).resolve()
    require(current_path.is_relative_to(ROOT.resolve()), 'Cosmetic source path escapes project')
    require(current_path.is_file(), 'Accepted cosmetic source is missing')
    current_source = current_path.read_bytes()
    require(candidate_path.is_file(), 'Cosmetic candidate source is missing')
    require(candidate_path.read_bytes() == source, 'Cosmetic candidate changed before comparison')

    manifest = read_json(ROOT/'layout/manifest.json')
    active_owners = {}
    active_recipe_paths = set()
    recipe_by_owner = {}
    for owner in manifest['owners']:
        recipe_path = owner.get('recipe')
        if not recipe_path:
            continue
        recipe = read_json(ROOT/recipe_path)
        if recipe.get('source') != source_path:
            continue
        active_recipe_paths.add(recipe_path)
        active_owners.setdefault(recipe_path, []).append(owner['id'])
        recipe_by_owner[owner['id']] = recipe_path
    for owner in manifest['owners']:
        if owner.get('kind') not in ('MATCHING_C_DATA', 'MATCHING_ASM_DATA'):
            continue
        recipe_path = recipe_by_owner.get(owner.get('parent'))
        if recipe_path is not None:
            active_owners.setdefault(recipe_path, []).append(owner['id'])

    # Include inactive fragment/group recipes too. Their OMF identities make
    # this safe for checked source fragments that are not themselves owners.
    recipes = {}
    for recipe_file in sorted((ROOT/'recipes').rglob('*.json')):
        recipe = read_json(recipe_file)
        if recipe.get('source') == source_path:
            relative = recipe_file.relative_to(ROOT).as_posix()
            recipes[relative] = recipe
    require(recipes, 'No canonical recipe describes this source file')
    require(active_recipe_paths <= set(recipes),
            'An active owner recipe is absent from the source-file recipe inventory')

    comparisons = []
    recipe_closure_updates = {}
    for recipe_path, recipe in recipes.items():
        update = _cosmetic_closure_update(recipe_path, recipe, current_source, source)
        for path, fields in update.items():
            require(path not in recipe_closure_updates or recipe_closure_updates[path] == fields,
                    'Cosmetic candidates disagree on recipe include closure: '+path)
            recipe_closure_updates[path] = fields
        old_obj = _cosmetic_compile(recipe, current_source)
        new_obj = _cosmetic_compile(recipe, source)
        old_identity, new_identity = identity(old_obj), identity(new_obj)
        require(new_identity == old_identity,
                'Cosmetic source refused: full pinned-tool OBJ identity differs for '+recipe_path)
        comparisons.append({'recipe': recipe_path,
                            'active_owners': sorted(active_owners.get(recipe_path, [])),
                            'object': old_identity})
    require(inputs() == before, 'Canonical inputs changed during cosmetic comparison')
    return {'function': 'source:'+source_path, 'destination': source_path,
            'source_path': source_path, 'kind': Path(source_path).parts[0],
            'profile': 'per-recipe-pinned', 'old_source': identity(current_source),
            'new_source': identity(source), 'candidate': str(candidate_path),
            'active_owners': sorted({name for names in active_owners.values() for name in names}),
            'comparisons': comparisons,
            'recipe_closure_updates': recipe_closure_updates,
            'bytes': len(source)}


def check_cosmetic(name, candidate, source, before):
    """Prove a source-only edit recompiles to the current accepted full OBJ."""
    source_path = _cosmetic_source_path(name)
    if source_path is not None:
        return check_cosmetic_source(source_path, candidate, source, before)
    require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name), 'Unsafe function name')
    manifest = read_json(ROOT/'layout/manifest.json')
    recipe_path = 'recipes/'+name+'.json'
    recipe_file = ROOT/recipe_path
    require(recipe_file.is_file(), 'Cosmetic republication requires the accepted recipe')
    recipe = read_json(recipe_file)
    owner_rows = [o for o in manifest['owners']
                  if o.get('name') == name and o.get('recipe') == recipe_path and
                  o['kind'] in ('MATCHING_C', 'MATCHING_ASM', 'MATCHING_C_DATA', 'MATCHING_ASM_DATA')]
    require(len(owner_rows) == 1, 'Cosmetic target is not one active accepted owner')
    owner = owner_rows[0]
    require((owner['start'], owner['end']) == (recipe['start'], recipe['end']) and
            owner['kind'] == contribution_kind(recipe),
            'Cosmetic recipe no longer describes its accepted owner')
    destination = recipe.get('source')
    require(isinstance(destination, str) and
            ((recipe.get('kind', 'c') == 'asm' and destination.startswith('asm/') and destination.endswith('.ASM')) or
             (recipe.get('kind', 'c') == 'c' and destination.startswith('src/') and destination.endswith('.c'))),
            'Cosmetic recipe source path differs from its contribution kind')
    current_path = ROOT/destination
    require(current_path.is_file(), 'Accepted cosmetic source is missing')
    current_source = current_path.read_bytes()
    candidate_path = Path(candidate).resolve()
    require(candidate_path.is_file(), 'Cosmetic candidate source is missing')
    require(candidate_path.read_bytes() == source, 'Cosmetic candidate changed before comparison')
    old_obj = _cosmetic_compile(recipe, current_source)
    new_obj = _cosmetic_compile(recipe, source)
    old_identity, new_identity = identity(old_obj), identity(new_obj)
    require(new_identity == old_identity,
            'Cosmetic source refused: full pinned-tool OBJ identity differs from accepted source')
    require(inputs() == before, 'Canonical inputs changed during cosmetic comparison')
    recipe_closure_updates = _cosmetic_closure_update(recipe_path, recipe, current_source, source)
    return {'function': name, 'destination': destination, 'recipe': recipe_path,
            'kind': recipe.get('kind', 'c'), 'profile': recipe['profile'],
            'old_source': identity(current_source), 'new_source': identity(source),
            'object': old_identity, 'candidate': str(candidate_path),
            'recipe_closure_updates': recipe_closure_updates,
            'bytes': len(old_obj)}


def cosmetic_recipe_changes(rows):
    """Serialize only frontend-derived dependency closures from cosmetic rows."""
    updates = {}
    for row in rows:
        for recipe_path, fields in row.get('recipe_closure_updates', {}).items():
            require(recipe_path not in updates or updates[recipe_path] == fields,
                    'Cosmetic candidates disagree on recipe closure: '+recipe_path)
            updates[recipe_path] = fields
    changes = {}
    for recipe_path, fields in updates.items():
        recipe = read_json(ROOT/recipe_path)
        changed = dict(recipe)
        for field, value in fields.items():
            require(field in ('include_closure', 'preprocessor_closure'),
                    'Cosmetic closure field is unsupported')
            changed[field] = value
        changes[recipe_path] = json_bytes(changed)
    return changes


def cosmetic_report_id(row):
    """Safe deterministic filename for owner- or source-level reports."""
    return re.sub(r'[^A-Za-z0-9_.-]+', '_', row['function'])


def cosmetic_promote(name, candidate, verify_only=False):
    """Publish only a source whose full OMF is unchanged, with derived closure updates."""
    candidate = Path(candidate).resolve()
    source = candidate.read_bytes()
    if verify_only:
        result = lock_free_snapshot(
            lambda before: check_cosmetic(name, candidate, source, before), inputs)
        require(candidate.read_bytes() == source, 'Cosmetic candidate changed during verification')
        result.update(status='COSMETIC_VERIFIED_ONLY', inputs=inputs())
        write_json(ROOT/'build/acceptance/_cosmetic'/(cosmetic_report_id(result)+'.json'), result)
        return result
    with exclusive():
        ensure_consistent()
        before = inputs()
        result = check_cosmetic(name, candidate, source, before)
        require(candidate.read_bytes() == source, 'Cosmetic candidate changed before publication')
        changes = {} if source == (ROOT/result['destination']).read_bytes() else {result['destination']: source}
        changes.update(cosmetic_recipe_changes([result]))
        if changes:
            expected = {**before, **{path: sha(raw) for path, raw in changes.items()}}
            with publishing():
                rows = prepare(changes)
                try:
                    require(inputs() == before, 'Canonical inputs changed before cosmetic publication')
                    invalidate_receipts()
                    apply(rows)
                    require(inputs() == expected, 'Unexpected edits during cosmetic publication')
                    require(candidate.read_bytes() == source, 'Candidate changed during cosmetic publication')
                    finish()
                    require(inputs() == expected, 'Inputs changed before cosmetic provenance publication')
                except BaseException:
                    rollback()
                    raise
            result['inputs'] = expected
            result['status'] = 'COSMETIC_PUBLISHED'
        else:
            result['inputs'] = before
            result['status'] = 'COSMETIC_NOOP'
        write_json(ROOT/'build/acceptance/_cosmetic'/(cosmetic_report_id(result)+'.json'), result)
        return result


CORRECTION_BASES = ('link-data-alignment', 'link-frame-reassignment',
                    'odd-start-asm-continuation', 'owner-identity-repair')


def _merged_raw(rows):
    """Adjacent UNRESOLVED_RAW rows of one partition collapse into one row."""
    out = []
    for row in rows:
        if (out and row['kind'] == 'UNRESOLVED_RAW' and out[-1]['kind'] == 'UNRESOLVED_RAW' and
                out[-1]['end'] == row['start']):
            last = out[-1]
            out[-1] = {**last, 'end': row['end'], 'id': f"raw_{last['start']:05x}_{row['end']:05x}",
                       'classification': 'UNRESOLVED_MIXED'}
        else:
            out.append(row)
    return out


def correct_ownership(manifest, name, recipe, image):
    """Ownership-correction transaction (integ29): an accepted owner whose extent
    or kind contradicts a link fact is returned to explicit raw ownership, and
    the corrected contribution is then published by the ordinary strict path.

    The correction names the owner and its exact current extent/kind/data
    intervals, a recorded reason and one reviewed basis.  Nothing else changes;
    the corrected recipe is verified freshly like any new publication."""
    spec = recipe.get('ownership_correction')
    require(isinstance(spec, dict) and
            set(spec) == {'schema', 'owner', 'previous', 'reason', 'basis'} and
            spec['schema'] == 'ownership-correction-v1' and spec['basis'] in CORRECTION_BASES and
            isinstance(spec['reason'], str) and len(spec['reason']) >= 20,
            'Ownership correction shape differs')
    result = copy.deepcopy(manifest)
    rows = [o for o in result['owners'] if o.get('recipe') == 'recipes/'+name+'.json' and
            o.get('name') == name and o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    require(len(rows) == 1 and rows[0]['id'] == spec['owner'] and
            (spec['owner'] is not None or spec['basis'] == 'owner-identity-repair'),
            'Corrected owner is not the accepted owner of this recipe name')
    owner = rows[0]
    previous = {'kind': owner['kind'], 'start': owner['start'], 'end': owner['end'],
                'data_intervals': owner.get('data_intervals', [])}
    require(spec['previous'] == previous, 'Correction does not name the current owner extent')
    require(owner.get('contribution_form') is None and not owner.get('reclassified_owners'),
            'Correction of prefix or reclassifying owners is not supported')
    require(not any(o.get('kind') == 'LINK_FILL' and o.get('object') == owner['id']
                    for o in result['owners']),
            'Correction of an owner with LINK fill is not supported')
    new_kind = contribution_kind(recipe)
    if spec['basis'] == 'odd-start-asm-continuation':
        # MSC word-aligns every function start: an accepted C owner that starts
        # at an odd address without a pad byte continues the preceding
        # assembled code; it is reclassified as ASM over the same extent.
        before = [o for o in result['owners'] if o['end'] == owner['start']]
        require(owner['kind'] == 'MATCHING_C' and new_kind == 'MATCHING_ASM' and
                (recipe['start'], recipe['end']) == (owner['start'], owner['end']) and
                owner['start'] % 2 == 1 and image[owner['start']-1] not in (0x00, 0x90) and
                len(before) == 1 and before[0]['kind'] == 'MATCHING_ASM' and
                not owner.get('data_intervals'),
                'Odd-start ASM continuation proof differs')
    elif spec['basis'] == 'owner-identity-repair':
        # An accepted owner row without an owner id: the same contribution is
        # republished over the same extent with its inventory stable id.
        require(owner['id'] is None and new_kind == owner['kind'] and
                (recipe['start'], recipe['end']) == (owner['start'], owner['end']) and
                isinstance(recipe.get('stable_id'), str) and recipe['stable_id'],
                'Owner identity repair differs')
    else:
        require(new_kind == owner['kind'], 'Correction basis cannot change the contribution kind')
    children = {o['id'] for part in ('owners', 'bss_owners') for o in result.get(part, [])
                if owner['id'] is not None and o.get('parent') == owner['id']}
    for partition in ('owners', 'bss_owners'):
        if partition not in result:
            continue
        rows = []
        for o in result[partition]:
            if o is owner or o['id'] in children:
                o = {'id': f"raw_{o['start']:05x}_{o['end']:05x}", 'kind': 'UNRESOLVED_RAW',
                     'classification': 'UNRESOLVED_MIXED', 'start': o['start'], 'end': o['end']}
            rows.append(o)
        result[partition] = _merged_raw(rows)
    record = {'reason': spec['reason'], 'basis': spec['basis'], 'previous': previous,
              'previous_owner': owner['id']}
    if spec['basis'] == 'odd-start-asm-continuation':
        record['reclassified_from'] = {'kind': owner['kind'], 'recipe': owner['recipe']}
    return result, record


def apply_ownership(manifest, name, recipe, oracle, image):
    """The manifest after publishing `recipe` (same rules for single and batch)."""
    destination = recipe['source']
    spec = recipe.get('ownership_correction')
    done = spec is not None and any(
        o.get('name') == name and o['kind'] == contribution_kind(recipe) and
        (o['start'], o['end']) == (recipe['start'], recipe['end']) and
        any(r.get('previous') == spec.get('previous') and r.get('reason') == spec.get('reason')
            for r in o.get('ownership_corrections', []))
        for o in manifest['owners'])
    if spec is not None and not done:
        corrected, record = correct_ownership(manifest, name, recipe, image)
        # The recipe file is the corrected owner's own; a new source file may
        # not overwrite an unowned one (a same-kind correction replaces its own).
        prior = read_json(ROOT/'recipes'/(name+'.json'))
        require(prior['source'] == destination or not (ROOT/destination).exists(),
                'Correction would overwrite an unowned source')
        staged = _publish_ownership(corrected, name, recipe, oracle, image)
        rows = [o for o in staged['owners'] if o.get('recipe') == 'recipes/'+name+'.json' and
                o['kind'] == contribution_kind(recipe)]
        require(len(rows) == 1, 'Corrected owner row missing')
        rows[0]['ownership_corrections'] = [record]
        return staged
    active = [o for o in manifest['owners'] if o['kind'] == contribution_kind(recipe) and o.get('name') == name]
    if active:
        require(len(active) == 1 and active[0]['recipe'] == 'recipes/'+name+'.json'
                and (active[0]['start'],active[0]['end']) == (recipe['start'],recipe['end']),
                'Existing ownership cannot change')
        # integ31 (data ownership ruling): the same whole object may grow or
        # add a secondary DGROUP segment over raw bytes (seg009's _DATA tail,
        # seg028's _DATA).  No segment is dropped; attach_secondary rechecks
        # the exact payload, the listed subsumed children and every cut.
        if recipe.get('secondary_dgroup_segments'):
            current = sorted(({'segment': row['segment'], 'start': row['start'], 'end': row['end'],
                               'target': row['target']}
                              for row in active[0].get('data_intervals', [])),
                             key=lambda row: row['segment'])
            requested = sorted(({'segment': segment, 'start': spec['start'], 'end': spec['end'],
                                 'target': spec['target']}
                                for segment, spec in recipe['secondary_dgroup_segments'].items()),
                               key=lambda row: row['segment'])
            require({row['segment'] for row in current} <= {row['segment'] for row in requested},
                    'Existing secondary segment cannot be dropped')
            if current != requested:
                manifest = attach_secondary(manifest, recipe, image)
        if 'dgroup_word_fill' in recipe:
            manifest = attach_dgroup_word_fill(manifest, recipe, image)
        if 'bss_word_fill' in recipe:
            manifest = attach_bss_word_fill(manifest, recipe)
        return manifest
    require(not (ROOT/destination).exists() and not (ROOT/'recipes'/(name+'.json')).exists(),
            'New publication would overwrite existing unowned files')
    return _publish_ownership(manifest, name, recipe, oracle, image)


def _publish_ownership(manifest, name, recipe, oracle, image):
    data_only = recipe.get('data_only') is True
    staged_manifest = (replace_data_raw(manifest, recipe, oracle) if data_only else
                       replace_prefix(manifest, recipe, oracle) if 'prefix_of_object' in recipe else
                       replace_group(manifest, recipe, oracle) if 'members' in recipe else
                       replace_raw(manifest, recipe))
    if not data_only:
        staged_manifest = attach_secondary(staged_manifest, recipe, image)
    if 'link_fill' in recipe:
        staged_manifest = attach_link_fill(staged_manifest, recipe, image)
    if 'dgroup_word_fill' in recipe:
        staged_manifest = attach_dgroup_word_fill(staged_manifest, recipe, image)
    if 'bss_word_fill' in recipe:
        staged_manifest = attach_bss_word_fill(staged_manifest, recipe)
    return staged_manifest


def attach_dgroup_word_fill(manifest, recipe, image):
    """integ33: own the one-byte DGROUP word-alignment fill LINK leaves after
    this object's odd-length DGROUP segment (link_fill.WORD_BASIS).  The fill
    row is re-derived from both neighbours on every build."""
    from link_fill import WORD_BASIS, word_fill_row, checked_fill
    result = copy.deepcopy(manifest)
    spec = recipe['dgroup_word_fill']
    require(set(spec) == {'segment', 'basis'} and spec['basis'] == WORD_BASIS and
            spec['segment'] in recipe.get('secondary_dgroup_segments', {}),
            'Unsupported recipe DGROUP fill form')
    parents = [o for o in result['owners'] if o.get('name') == recipe['id'] and
               (o['start'], o['end']) == (recipe['start'], recipe['end']) and
               o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    require(len(parents) == 1, 'DGROUP fill lacks its object owner')
    rows = [o for o in result['owners'] if o.get('parent') == parents[0]['id'] and
            o.get('segment') == spec['segment']]
    require(len(rows) == 1, 'DGROUP fill lacks its owned segment row')
    start = rows[0]['end']
    after = [o for o in result['owners'] if o['start'] == start]
    require(len(after) == 1, 'DGROUP fill position is not a single owner')
    fill = word_fill_row(rows[0]['id'], start)
    if after[0].get('kind') == 'LINK_FILL':
        require(after[0] == fill, 'Existing DGROUP fill differs')
    else:
        require(after[0]['kind'] == 'UNRESOLVED_RAW' and after[0]['end'] == start + 1,
                'DGROUP fill is not exactly one raw-owned byte')
        at = result['owners'].index(after[0])
        result['owners'][at] = fill
    checked_fill(fill, result, image, {'recipes/'+recipe['id']+'.json':recipe})
    return result


def attach_bss_word_fill(manifest, recipe):
    """Replace one reviewed raw BSS fill with its derived LINK alignment row."""
    import bss_link
    spec = recipe['bss_word_fill']
    require(set(spec) == {'segment', 'basis'} and spec['segment'] == '_BSS' and
            spec['basis'] == bss_link.BSS_WORD_FILL and
            '_BSS' in recipe.get('secondary_dgroup_segments', {}),
            'Unsupported recipe BSS WORD fill form')
    result = copy.deepcopy(manifest)
    parents = [o for o in result['owners'] if o.get('name') == recipe['id'] and
               (o['start'], o['end']) == (recipe['start'], recipe['end']) and
               o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    require(len(parents) == 1, 'BSS WORD fill lacks its complete object owner')
    bss_rows = [o for o in result['bss_owners'] if o.get('parent') == parents[0]['id'] and
                o.get('segment') == '_BSS']
    require(len(bss_rows) == 1, 'BSS WORD fill lacks its owned _BSS segment')
    start = bss_rows[0]['end']
    candidates = [o for o in result['bss_owners'] if o['start'] == start]
    objects = read_json(ROOT/'layout/link-objects.json')['objects']
    hosts = [o['id'] for o in objects if o['start'] <= parents[0]['start'] < o['end']]
    require(len(hosts) == 1, 'BSS WORD fill object host is ambiguous')
    # An already accepted owner may be republished with a public-name-only
    # source change. Preserve the existing derived row only if every field
    # still describes the same one-byte fill and object host.
    if len(candidates) == 1 and candidates[0].get('kind') == 'LINK_FILL':
        expected = {'id': f'fill_{start:05x}_{start + 1:05x}', 'kind': 'LINK_FILL',
                    'basis': bss_link.BSS_WORD_FILL, 'start': start, 'end': start + 1,
                    'object': hosts[0]}
        require(candidates[0] == expected,
                'Existing BSS WORD fill link ownership differs')
        bss_link.load_partition(result)
        return result
    require(len(candidates) == 1 and candidates[0]['kind'] == 'UNRESOLVED_RAW' and
            candidates[0].get('raw_form') == bss_link.WORD_FILL and
            candidates[0]['end'] == start + 1,
            'BSS WORD fill is not exactly one reviewed raw byte')
    row = {'id':f'fill_{start:05x}_{start + 1:05x}', 'kind':'LINK_FILL',
           'basis':bss_link.BSS_WORD_FILL, 'start':start, 'end':start + 1,
           'object':hosts[0]}
    at = result['bss_owners'].index(candidates[0])
    result['bss_owners'][at] = row
    bss_link.load_partition(result)
    return result


def attach_link_fill(manifest, recipe, image):
    """Own the LINK paragraph fill after a complete object (link_fill ruling)."""
    from link_fill import BASIS, CODE_WORD_BASIS, fill_row, checked_fill
    result = copy.deepcopy(manifest)
    spec = recipe['link_fill']
    if spec.get('basis') == CODE_WORD_BASIS:
        require(set(spec) == {'end', 'basis'} and type(spec['end']) is int and
                spec['end'] == recipe['end'] + 1,
                'Unsupported recipe CODE WORD fill form')
        parent = [o for o in result['owners'] if o.get('name') == recipe['id'] and
                  (o['start'], o['end']) == (recipe['start'], recipe['end']) and
                  o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
        require(len(parent) == 1, 'CODE WORD fill lacks its complete object owner')
        candidate = {'kind':'LINK_FILL', 'basis':CODE_WORD_BASIS,
                     'start':recipe['end'], 'object':parent[0]['id']}
        import promote_runtime
        result = promote_runtime.replace_raw_code_fill(result, candidate)
        row = [o for o in result['owners'] if o.get('kind') == 'LINK_FILL' and
               o.get('start') == recipe['end']]
        require(len(row) == 1, 'CODE WORD fill row missing')
        checked_fill(row[0], result, image,
                     {'recipes/'+recipe['id']+'.json':recipe})
        return result
    require(set(spec) == {'end', 'basis'} and spec['basis'] == BASIS and type(spec['end']) is int,
            'Unsupported recipe LINK fill form')
    rows = [o for o in result['owners'] if o.get('recipe') == 'recipes/'+recipe['id']+'.json' and
            o['end'] == recipe['end'] and o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    require(len(rows) == 1, 'LINK fill lacks its object owner')
    after = [o for o in result['owners'] if o['start'] == recipe['end']]
    require(len(after) == 1 and after[0]['kind'] == 'UNRESOLVED_RAW' and after[0]['end'] >= spec['end'],
            'LINK fill is not raw-owned')
    old = after[0]
    replacement = [fill_row(rows[0]['id'], recipe['end'], spec['end'])]
    if spec['end'] < old['end']:
        replacement.append({**old, 'start': spec['end'], 'id': f"raw_{spec['end']:05x}_{old['end']:05x}"})
    at = result['owners'].index(old)
    result['owners'][at:at+1] = replacement
    checked_fill(replacement[0], result, image)
    return result


def _stage(name, candidate, source, recipe_path, recipe_data, before, verify_only):
    """Complete staged acceptance; reads canonical state, writes nothing canonical."""
    import memo
    with memo.session():
        manifest = read_json(ROOT/'layout/manifest.json')
        oracle = verify(write=False)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
        recipe, destination, payload, fast, staged_manifest = check_candidate(
            name, source, recipe_data, manifest, oracle, image)
    # Recompile every existing contribution, including runtime binding, with
    # only the target source supplied from frozen bytes. No canonical writes.
    staged = build(staged_manifest, {'recipes/'+name+'.json':recipe}, publish=False,
                   source_overrides={destination:source})
    require(inputs() == before and staged['inputs'] == before, 'Canonical inputs changed during acceptance')
    import bss_link
    bss = None
    if bss_link.claims(recipe):
        bss = bss_link.staged_gate([(name, source, recipe)])
        require(inputs() == before, 'Canonical inputs changed during the BSS real-link gate')
    require(candidate.read_bytes() == source, 'Candidate source changed during acceptance')
    if recipe_path:
        require(Path(recipe_path).read_bytes() == recipe_data, 'Candidate recipe changed during acceptance')
    report = {'status':'VERIFIED_ONLY' if verify_only else 'PROMOTED', 'function':name,
              'source':identity(source), 'bytes':len(payload), 'whole_image':staged['executable'],
              'relocation_count':staged['relocation_count'], 'fast':fast, 'inputs':before}
    if bss is not None:
        report['bss_real_link'] = bss
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
    p.add_argument('--cosmetic', action='store_true',
                   help='Republish an owner or canonical source path only when every full pinned-tool OMF OBJ matches')
    p.add_argument('--batch', type=Path, help='Batch file (NAME CANDIDATE [RECIPE] per line; integ39: one '
                   '`COMMUNAL UNIT.json` line makes it an atomic communal transaction): one journaled '
                   'publication; each candidate verified individually, one fresh union whole-image build')
    p.add_argument('--no-independent', action='store_true',
                   help='Batch research runs only: omit the per-candidate DOSBox-X check (never for publication)')
    a = p.parse_args()
    if a.recover:
        if a.function or a.candidate or a.recipe or a.batch or a.cosmetic or a.verify_only:
            p.error('--recover takes no candidate or mode')
        recover(); print('Recovery complete; run python tools/validate.py'); return
    if a.batch:
        if a.function or a.candidate or a.recipe: p.error('--batch takes no single candidate')
        if a.no_independent and not a.verify_only: p.error('Batch publication requires the DOSBox-X check')
        from batch_publish import read_batch, publish_batch
        if a.cosmetic:
            if a.no_independent: p.error('--no-independent does not apply to --cosmetic')
            from batch_publish import cosmetic_batch
            summary = cosmetic_batch(read_batch(a.batch), verify_only=a.verify_only)
            print(summary['status'], len(summary['candidates']), 'cosmetic sources; full OBJ hashes identical')
            return
        summary = publish_batch(read_batch(a.batch), verify_only=a.verify_only,
                                independent=not a.no_independent)
        print(summary['status'], len(summary['published']), 'candidates',
              sum(x['bytes'] for x in summary['published']), 'bytes;', len(summary['dropped']), 'dropped;',
              summary['seconds'], 's')
        if summary.get('communal_unit'):
            # integ39: atomic communal transaction (tools/communal_unit.py)
            print('communal unit', summary['communal_unit'], summary.get('communal_gate', {}).get('status') or
                  summary['dropped'].get('@communal', {}).get('error', '')[:400])
        return
    if not a.function or not a.candidate: p.error('Supply FUNCTION CANDIDATE.c')
    if a.cosmetic:
        if a.recipe: p.error('--cosmetic uses the current accepted recipe; --recipe is not allowed')
        report = cosmetic_promote(a.function, a.candidate, a.verify_only)
        if report.get('source_path'):
            print(report['status'], report['source_path'], len(report['comparisons']),
                  'recipe OBJ identities; source hashes recorded')
        else:
            print(report['status'], report['function'], report['object']['size'],
                  'OBJ bytes; source hashes recorded')
        return
    report = promote(a.function, a.candidate, a.recipe, a.verify_only)
    print(report['status'], report['function'], report['bytes'], 'bytes; fresh HYBRID_EXACT and ordered relocations')
    prefix = report['fast'].get('binding', {}).get('prefix')
    if prefix:
        print('RECORD_CLOSED_EXACT prefix: owned', prefix['owned_records'], 'records /',
              prefix['owned_end_offset'], 'bytes; re-derived', prefix['derived_records'], 'records /',
              prefix['derived_end_offset'], 'bytes of', prefix['candidate_record_count'],
              'candidate records; record states', prefix['record_states'])


if __name__ == '__main__': main()
