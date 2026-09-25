"""Resolve reviewed far code targets using owned publics or pristine call anchors."""
from common import ROOT, read_json, require, sha


def _complete_target_owner(owner, function):
    if owner['kind'] == 'UNRESOLVED_RAW':
        return owner['start'] <= function['start'] and function['end'] <= owner['end']
    if owner['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY':
        # A mapped function can become one exact public within a complete pinned
        # runtime member. The reference inventory sometimes extends a function
        # over LINK fill or into the next runtime segment; the OMF public and
        # member extent are the independent runtime facts used here.
        return (owner['start'] <= function['start'] < owner['end'] and
                any(p['segment'] == owner['segment'] and owner['start']+p['offset'] == function['start']
                    for p in owner['publics']))
    if owner['kind'] not in ('MATCHING_C','MATCHING_ASM'):
        return False
    if (owner['start'], owner['end'], owner.get('name')) == (
            function['start'], function['end'], function['name']):
        return True
    if not (owner['start'] <= function['start'] and function['end'] <= owner['end']):
        return False
    recipe = read_json(ROOT/owner['recipe'])
    return recipe.get('id') == owner['name'] and any(
        (member.get('name'), member.get('start'), member.get('end'),
         member.get('target', {}).get('sha256')) ==
        (function['name'], function['start'], function['end'], function['sha256'])
        for member in recipe.get('members', []))


def _reference_entry_target(name, target, proof, inventory, owners, image):
    """Ground an entry whose containing ASM/data region has no complete CFG."""
    from common import identity
    require(name=='_sprite_make_wnd' and target==
            {'name':'sprite_make_wnd','start':150540} and
            proof=={'kind':'pinned-entry-after-verified-predecessor-v1',
                    'source_path':'src/restunts/asmorig/seg012.asm',
                    'source_line':14155,'predecessor':'draw_patterned_lines',
                    'entry_hex':'558bec83ec081e5657'},
            'Unreviewed partial code entry proof')
    entry,=[f for f in inventory['functions'] if f.get('name')==target['name'] and
            f.get('start')==target['start'] and f['status']=='PARTIAL_UNMAPPED']
    predecessor,=[f for f in inventory['functions'] if f.get('name')==proof['predecessor'] and
                  f.get('end')==target['start'] and
                  f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED']
    require(sha(image[predecessor['start']:predecessor['end']])==predecessor['sha256'],
            'Partial entry predecessor bytes differ')
    reference=ROOT/'build/references/restunts'/proof['source_path']
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][proof['source_path']]
    require(identity(reference.read_bytes())==pinned and
            reference.read_text(encoding='latin1').splitlines()[proof['source_line']-1].strip()==
            'sprite_make_wnd proc far',
            'Partial entry lacks pinned source label')
    raw=bytes.fromhex(proof['entry_hex'])
    require(image[target['start']:target['start']+len(raw)]==raw and
            any(o['kind']=='UNRESOLVED_RAW' and o['start']<=target['start'] and
                target['start']+len(raw)<=o['end'] for o in owners),
            'Partial entry bytes are not raw-owned')
    return target['start']


def resolve_code_symbols(names, image, relocations):
    from library import bind_library
    layout=read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256']==sha(image), 'Code symbols belong to another oracle')
    owners=read_json(ROOT/'layout/manifest.json')['owners']; result={}
    inventory=None
    for name in names:
        require(name in layout['symbols'], 'Unknown far code symbol: '+name)
        symbol=layout['symbols'][name]
        require(symbol.get('anchors') or symbol.get('pointer_anchors'),
                'Code symbol lacks reviewed call/pointer evidence: '+name)
        if 'owner' in symbol:
            found=[o for o in owners if o['id']==symbol['owner'] and o['kind']=='KNOWN_TOOLCHAIN_LIBRARY']
            require(len(found)==1, 'Code symbol lacks pinned active library owner')
            owner=found[0]; bind_library(owner,image,relocations)
            public=[p for p in owner['publics'] if p['name']==name]
            require(len(public)==1, 'Code public missing/ambiguous')
            address=owner['start']+public[0]['offset']
        else:
            # The source symbol name is a reviewed binding alias, not a claim
            # about the original PUBDEF spelling. Require an exact mapped code
            # entry, complete raw or exact active-C ownership, and an
            # original relocated call to that independently mapped entry.
            target=symbol['mapped_target']
            require(name == '_' + target['name'] or symbol.get('reviewed_alias') == name,
                    'Far code alias name lacks inventory or explicit review')
            if inventory is None:
                from function_evidence import current_inventory
                inventory=current_inventory(image)
            if 'entry_proof' in symbol:
                address=_reference_entry_target(name,target,symbol['entry_proof'],
                                                inventory,owners,image)
                function=None
            else:
                function=None
            matches=[f for f in inventory['functions'] if
                     (target.get('stable_id') is None or
                      f.get('stable_id')==target.get('stable_id'))
                     and f.get('name')==target['name']
                     and (f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                          (f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED'
                           and f.get('start_evidence')
                           and f.get('end_evidence')))]
            if function is None and 'entry_proof' not in symbol:
                require(len(matches)==1, 'Far code alias lacks unique verified mapped target')
                function=matches[0]
                if function['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and function.get('bytes_hex'):
                    require(bytes.fromhex(function['bytes_hex'])==image[function['start']:function['end']],
                            'Far code emission bytes changed')
                require(target['start']==function['start'] and target['end']==function['end']
                        and target['sha256']==function['sha256']
                        and sha(image[target['start']:target['end']])==target['sha256'],
                        'Far code target extent/hash changed')
                require(any(_complete_target_owner(o, function) for o in owners),
                        'Mapped target lacks a complete raw or exact active-C owner')
                address=target['start']
            anchors=symbol.get('anchors',[])+symbol.get('pointer_anchors',[])
            require(anchors, 'Raw code target needs a relocated verified caller')
            if 'distinct_verified_callers' in symbol:
                require(symbol['distinct_verified_callers']==len({a['caller_task'] for a in anchors}),
                        'Code alias corroboration count differs')
            for anchor in anchors:
                callers=[f for f in inventory['functions'] if f.get('stable_id')==anchor['caller_task']
                         and f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
                         and f['start']<=anchor['site'] and
                         anchor['site']+len(bytes.fromhex(anchor['hex']))<=f['end']]
                require(len(callers)==1 and not target['start']<=anchor['site']<target.get('end',target['start']+1),
                        'Far code anchor lacks independent verified caller')
        frame=symbol['frame_load_address']
        require(symbol.get('anchors') or symbol.get('pointer_anchors'),
                'Code frame needs independent evidence')
        for anchor in symbol.get('anchors',[]):
            at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
            require(len(raw)==5 and raw[0]==0x9a and image[at:at+5]==raw, 'Code frame anchor changed')
            require(anchor['relocation'] in relocations and anchor['relocation']['load_offset']==at+3,
                    'Code anchor lacks segment relocation')
            require(int.from_bytes(raw[3:],'little')*16==frame and
                    int.from_bytes(raw[1:3],'little')+frame==address, 'Code frame/public conflict')
        for anchor in symbol.get('pointer_anchors',[]):
            at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
            require(len(raw)==6 and raw[0]==0xb8 and raw[3]==0xba and
                    image[at:at+6]==raw, 'Code pointer MOV pair changed')
            require(anchor['relocation'] in relocations and
                    anchor['relocation']['load_offset']==at+4,
                    'Code pointer base lacks MZ relocation')
            require(int.from_bytes(raw[4:6],'little')*16==frame and
                    int.from_bytes(raw[1:3],'little')+frame==address,
                    'Code pointer pair disagrees with mapped target')
        result[name]={'kind':'far-code','frame_load_address':frame,'load_address':address}
    return result


def resolve_callback_pointer(image, relocations):
    """Resolve one code-pointer alias from an independent pristine MOV pair.

    This alias does not claim an original PUBDEF name or TU. The source target
    is mapped independently of the candidate set_frame_callback operand.
    """
    layout = read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256'] == sha(image), 'Callback alias belongs to another oracle')
    symbol = layout['symbols']['_frame_callback']
    target = symbol['mapped_target']
    inventory = read_json(ROOT/'evidence/functions.json')
    matches = [f for f in inventory['functions']
               if f.get('stable_id') == target['stable_id'] and f.get('name') == target['name']
               and f.get('start') == target['start'] and f.get('end') == target['end']
               and f.get('sha256') == target['sha256']
               and f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
    require(len(matches) == 1 and sha(image[target['start']:target['end']]) == target['sha256'],
            'Callback pointer lacks unique verified mapped target')
    owners = read_json(ROOT/'layout/manifest.json')['owners']
    require(any(_complete_target_owner(o, matches[0]) for o in owners),
            'Callback target lacks complete raw or exact C owner')
    anchor = symbol['pointer_anchor']
    callers = [f for f in inventory['functions'] if f.get('stable_id') == anchor['caller_task']
               and f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
               and f.get('start', 10**9) <= anchor['site']
               and anchor['site'] + 6 <= f.get('end', -1)
               and f['name'] != 'set_frame_callback']
    require(len(callers) == 1, 'Callback pointer anchor is not an independent mapped caller')
    at = anchor['site']; raw = bytes.fromhex(anchor['hex'])
    require(len(raw) == 6 and raw[0] == 0xb8 and raw[3] == 0xba
            and image[at:at+6] == raw, 'Callback MOV offset/segment pair changed')
    require(anchor['relocation'] in relocations and
            anchor['relocation']['load_offset'] == at+4,
            'Callback pointer segment lacks reviewed MZ relocation')
    frame = symbol['frame_load_address']
    require(type(frame) is int and frame % 16 == 0 and
            int.from_bytes(raw[4:6], 'little')*16 == frame and
            frame + int.from_bytes(raw[1:3], 'little') == target['start'],
            'Callback pointer address/frame differs from mapped target')
    return {'kind': 'far-code', 'frame_load_address': frame,
            'load_address': target['start']}


def resolve_recipe_symbols(recipe, image, relocations):
    names={f['target'] for f in recipe['expected_fixups']}
    if not names:return None
    mode=recipe.get('binding',{}).get('mode')
    if mode in ('external-far-call-v1','asm-external-far-call-v1'):
        return resolve_code_symbols(names,image,relocations)
    if mode == 'asm-external-cs-offset16-v1':
        from data_symbols import resolve_cs_symbols
        return resolve_cs_symbols(names,image,relocations)
    if mode in ('external-far-call-dgroup-offset16-v1',
                'external-far-call-code-pointer-dgroup-offset16-v1'):
        absolute=names & {'__AHSHIFT'}
        code={f['target'] for f in recipe['expected_fixups'] if f['loc']=='pointer32' or
              (mode.endswith('code-pointer-dgroup-offset16-v1') and
               f['loc'] in ('base16','loader-offset16'))} - absolute
        data={f['target'] for f in recipe['expected_fixups'] if f['loc']=='offset16'} - absolute
        require(code and data and not code.intersection(data) and code|data|absolute==names,
                'Mixed binding needs distinct reviewed code/data/absolute targets')
        from data_symbols import resolve_symbols, check_folded_recipe
        result = {**resolve_code_symbols(code,image,relocations),
                  **resolve_symbols(data,image,relocations)}
        if absolute:
            from runtime_absolute import ahshift
            result['__AHSHIFT']=ahshift(image,relocations)
        check_folded_recipe(recipe, result, image)
        return result
    if mode=='external-far-call-cs-pointer-v1':
        code={f['target'] for f in recipe['expected_fixups'] if f['loc']=='pointer32'}
        data={f['target'] for f in recipe['expected_fixups'] if f['loc'] in ('offset16','base16')}
        require(code and data and not code.intersection(data) and code|data==names,
                'CS pointer binding needs distinct far code and CS data targets')
        from data_symbols import resolve_cs_symbols
        return {**resolve_code_symbols(code,image,relocations),
                **resolve_cs_symbols(data,image,relocations)}
    if mode=='external-frame-callback-v1':
        require(names == {'_byte_442E4', '_word_46468', '_timer_reg_callback',
                          '_frame_callback'}, 'Callback binding external set changed')
        from data_symbols import resolve_symbols
        return {**resolve_symbols({'_byte_442E4', '_word_46468'}, image, relocations),
                **resolve_code_symbols({'_timer_reg_callback'}, image, relocations),
                '_frame_callback': resolve_callback_pointer(image, relocations)}
    from data_symbols import resolve_symbols, check_folded_recipe
    result = resolve_symbols(names,image,relocations)
    check_folded_recipe(recipe, result, image)
    return result
