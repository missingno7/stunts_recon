"""Resolve reviewed far code targets using owned publics or pristine call anchors."""
from common import ROOT, identity, read_json, require, sha


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
    if not owner['start'] <= function['start'] < owner['end']:
        return False
    recipe = read_json(ROOT/owner['recipe'])
    if 'module_proof' in recipe and recipe.get('id') == owner['name']:
        # A whole grounded module owns each verified entry it declares; an
        # entry row may run one clipped linker-fill byte past the module end.
        clip = owner['end'] + (1 if recipe['module_proof']['end_boundary'].get('kind') ==
                               'zero-fill-after-return' else 0)
        return (owner['start'] <= function['start'] < owner['end'] and
                function['end'] <= clip and
                any((m.get('name'), m.get('start')) == (function['name'], function['start'])
                    for m in recipe['members']))
    if recipe.get('prefix_of_object') and recipe.get('id') == owner['name']:
        # A record-closed prefix owns the verified entry of each listed member,
        # including the member cut at the prefix end (its remainder stays raw).
        return any((m['name'], m['start'], m['end']) ==
                   (function['name'], function['start'], function['end'])
                   for m in recipe['prefix_members'])
    if not (owner['start'] <= function['start'] and function['end'] <= owner['end']):
        return False
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


def _module_entry_target(name, target, symbol, owners, image):
    """An entry of an accepted whole ASM module (integ25).

    A PARTIAL_UNMAPPED inventory row (start known, end unmapped) can still
    be a far-code target when an accepted `asm-module-extent-v1` owner lists
    it as a module entry: that module proof re-grounds the entry from its
    original relocated far CALLs (asm_module.checked_module) and the module's
    own PUBDEF places it.  The alias spelling is the inventory C spelling;
    the alias's own anchors are checked by the caller as usual."""
    from asm_module import checked_module
    proof = symbol['module_entry']
    require(set(proof) == {'owner'} and name == '_' + target['name'] and
            set(target) == {'name', 'start'},
            'Module-entry alias shape differs')
    found = [o for o in owners if o['id'] == proof['owner'] and o['kind'] == 'MATCHING_ASM' and
             o.get('module_form') == 'asm-module' and 'recipe' in o]
    require(len(found) == 1, 'Module-entry alias lacks its accepted ASM module owner')
    recipe = read_json(ROOT/found[0]['recipe'])
    require((recipe['start'], recipe['end']) == (found[0]['start'], found[0]['end']),
            'Module-entry owner recipe differs')
    checked_module(recipe, image)
    entries = [m for m in recipe['members'] if m['name'] == target['name']]
    publics = [p for p in recipe['object_declarations']['publics'] if p['name'] == name]
    require(len(entries) == 1 and len(publics) == 1 and
            entries[0]['start'] == target['start'] == recipe['start'] + publics[0]['offset'] and
            symbol['frame_load_address'] == recipe['original_frame_load_address'],
            'Module-entry alias differs from the accepted module public')
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
        require(symbol.get('anchors') or symbol.get('pointer_anchors') or symbol.get('table_anchors'),
                'Code symbol lacks reviewed call/pointer evidence: '+name)
        if name == '_main':
            # CRT0's pinned _main EXTDEF is checked when its complete member
            # binds. The original relocated CALL independently identifies the
            # reviewed game entry even while CRT0 remains raw-owned.
            from function_evidence import reviewed_functions
            target=symbol['mapped_target']
            entry=reviewed_functions(image)['ported_stuntsmain_']
            require(target=={'name':'ported_stuntsmain_', 'stable_id':'load_00000',
                             'start':0, 'end':1434, 'sha256':entry['sha256']} and
                    entry['bytes_hex']==image[:1434].hex() and
                    symbol['anchors']==[{'caller_module':'dos\\crt0.asm',
                        'site':118009, 'hex':'9a00000000',
                        'relocation':{'segment':4096,'offset':52476,'load_offset':118012}}],
                    'CRT0 main entry proof differs')
            address=0
        elif 'owner' in symbol:
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
            # MASM 5.10 keeps 31 significant characters: a reviewed truncated
            # public names the same longer inventory entry.
            truncated = ((len('_' + target['name']) > 31 and
                          name == ('_' + target['name'])[:31] and
                          symbol.get('masm_truncated_public') is True) or
                         # MSC 5.10 C externals keep 31 identifier characters
                         # plus the underscore (asm_module.c_public).
                         (len('_' + target['name']) > 32 and
                          name == ('_' + target['name'])[:32] and
                          symbol.get('c_truncated_public') is True))
            require(name == '_' + target['name'] or symbol.get('reviewed_alias') == name or
                    truncated, 'Far code alias name lacks inventory or explicit review')
            if inventory is None:
                from function_evidence import current_inventory
                inventory=current_inventory(image)
            if 'entry_proof' in symbol:
                address=_reference_entry_target(name,target,symbol['entry_proof'],
                                                inventory,owners,image)
                function=None
            elif 'module_entry' in symbol:
                address=_module_entry_target(name,target,symbol,owners,image)
                function=None
            else:
                function=None
            def neighbour_bounded(f):
                # Emission-verified extent whose start and end coincide with
                # instruction-verified neighbours (reviewed per alias).
                return (symbol.get('boundary_proof')=='verified-neighbours-v1' and
                        any(g['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                            g.get('end')==f['start'] for g in inventory['functions']) and
                        any(g['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and
                            g.get('start')==f['end'] for g in inventory['functions']))
            matches=[f for f in inventory['functions'] if
                     (target.get('stable_id') is None or
                      f.get('stable_id')==target.get('stable_id'))
                     and f.get('name')==target['name']
                     and (f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                          (f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED'
                           and ((f.get('start_evidence') and f.get('end_evidence')) or
                                neighbour_bounded(f))))]
            if function is None and 'entry_proof' not in symbol and 'module_entry' not in symbol:
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
            anchors=(symbol.get('anchors',[])+symbol.get('pointer_anchors',[])+
                     symbol.get('table_anchors',[]))
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
        require(symbol.get('anchors') or symbol.get('pointer_anchors') or symbol.get('table_anchors'),
                'Code frame needs independent evidence')
        for anchor in symbol.get('anchors',[]):
            at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
            # A relocated far CALL (9A) or far JMP (EA) names the same entry.
            require(len(raw)==5 and raw[0] in (0x9a,0xea) and image[at:at+5]==raw, 'Code frame anchor changed')
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
        table_sites=_checked_table_anchors(name,symbol,frame,address,image,relocations)
        result[name]={'kind':'far-code','frame_load_address':frame,'load_address':address,
                      **({'table_sites':table_sites} if table_sites else {})}
    return result


def _checked_table_anchors(name, symbol, frame, address, image, relocations):
    """integ30: entries of a reviewed far-pointer table (function-evidence island
    `far_pointer_table32` inside an instruction-verified procedure).  Each entry is
    the original relocated segment:offset of this alias's mapped entry."""
    rows=symbol.get('table_anchors',[])
    if not rows:
        return []
    from function_evidence import reviewed_functions
    islands=[(f['name'],island) for f in reviewed_functions(image).values()
             for island in f.get('data_islands',[]) if island.get('kind')=='far_pointer_table32']
    sites=[]
    for anchor in rows:
        at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
        inside=[n for n,island in islands if island['load_offset']<=at and
                at+4<=island['load_offset']+len(bytes.fromhex(island['hex'])) and
                (at-island['load_offset'])%4==0]
        require(set(anchor)=={'caller_task','site','hex','relocation'} and
                len(raw)==4 and image[at:at+4]==raw and len(inside)==1 and
                anchor['relocation'] in relocations and anchor['relocation']['load_offset']==at+2 and
                int.from_bytes(raw[2:4],'little')*16==frame and
                int.from_bytes(raw[0:2],'little')+frame==address,
                'Code table anchor differs from its reviewed far-pointer table: '+name)
        sites.append(at)
    return sites


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
    require(symbol.get('anchors', []) == [] and len(symbol.get('pointer_anchors', [])) == 1,
            'Callback alias needs exactly one reviewed independent pointer witness')
    anchor = symbol['pointer_anchors'][0]
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


def _far_data(names, image, relocations):
    from data_only import resolve_far_data_symbols
    return resolve_far_data_symbols(names, image, relocations)


def _c_dgroup_base_fixups(recipe):
    """Every `_DATA` segment fixup is a zero-addend base16 with the DGROUP group
    frame, and `_DATA` is a zero-length DATA SEGDEF listed in DGROUP."""
    declarations=(recipe.get('binding',{}).get('declarations') or
                  recipe.get('object_declarations') or {})
    data=[d for d in declarations.get('segments',[]) if d.get('name')=='_DATA']
    groups=[g for g in declarations.get('groups',[]) if g.get('name')=='DGROUP']
    if len(data)!=1 or len(groups)!=1:
        return False
    definition,group=data[0],groups[0]
    return (definition.get('length')==0 and definition.get('class')=='DATA' and
            definition.get('index') in group.get('segment_indices',[]) and
            all(f['loc']=='base16' and f['width']==2 and not f['self_relative'] and
                f['target_method']==0 and f['target_index']==definition['index'] and
                (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
                (1,'group','DGROUP',group['index']) and
                f['displacement']==0 and f['encoded_addend']=='0000'
                for f in recipe['expected_fixups']
                if f['target_kind']=='segment' and f['target']=='_DATA'))


def resolve_recipe_symbols(recipe, image, relocations):
    names={f['target'] for f in recipe['expected_fixups']}
    if not names:return None
    mode=recipe.get('binding',{}).get('mode')
    island_names={n for n,s in read_json(ROOT/'layout/data-symbols.json')['symbols'].items()
                  if s.get('storage')=='code_island'}
    cs_named=recipe.get('kind')=='asm' and any(
        (n if n.startswith('_') else '_'+n) in island_names for n in names)
    # A C object reloading DS from DGROUP (zero-length _DATA base16) composes
    # per fixup whatever its external shapes are (integ26).
    c_data_base=(recipe.get('kind','c')=='c' and
                 any(f['target_kind']=='segment' and f['target']=='_DATA'
                     for f in recipe['expected_fixups']))
    if c_data_base or (cs_named and mode=='asm-external-dgroup-offset16-v1') or (
            mode in ('external-far-call-dgroup-offset16-v1',
                'external-far-call-code-pointer-dgroup-offset16-v1')) or (
            mode=='asm-external-far-call-self-base16-v1') or (
            recipe.get('kind')=='asm' and mode not in
            ('asm-local-code-offset16-v1','asm-external-far-call-self-base16-v1') and
            any((f['target_kind']=='segment' and
                 f['target'] in (recipe['object_segment'],'DSEG')) or
                (f['target_kind']=='external' and f['self_relative'])
                for f in recipe['expected_fixups'])):
        from data_symbols import (resolve_symbols, check_folded_recipe,
                                  checked_dseg_base, checked_dgroup_layout)
        fixes=recipe['expected_fixups']; seg=recipe['object_segment']
        local={f['target'] for f in fixes if f['target_kind']=='segment'}
        data_base='_DATA' in local and recipe.get('kind','c')=='c' and _c_dgroup_base_fixups(recipe)
        require(local <= {seg,'DSEG'} | ({'_DATA'} if data_base else set()),
                'Unreviewed composed segment target')
        code={f['target'] for f in fixes if f['target_kind']=='external' and
              f['loc'] in ('pointer32','base16','loader-offset16')}
        near={f['target'] for f in fixes if f['target_kind']=='external' and
              f['self_relative'] and f['loc']=='offset16'}
        code_offsets=set(recipe.get('reviewed_code_offsets',[]))
        require(all(f['target_kind']=='external' and f['loc']=='offset16' and
                    not f['self_relative']
                    for f in fixes if f['target'] in code_offsets) and
                code_offsets <= {f['target'] for f in fixes},
                'Unreviewed external CODE offset declaration')
        data={f['target'] for f in fixes if f['target_kind']=='external' and
              f['loc']=='offset16' and not f['self_relative'] and
              f['target'] not in code_offsets}
        absolute=(code|data)&{'__AHSHIFT'}
        code-=absolute; data-=absolute
        # Per-fixup composition: reviewed CS-resident data (code islands) binds
        # under the CS data rule, never as DGROUP data.
        islands=read_json(ROOT/'layout/data-symbols.json')['symbols']
        cs_data={n for n in data|code if islands.get(n if n.startswith('_') else '_'+n,{})
                 .get('storage')=='code_island'}
        # A CS island datum may be named by a MOV AX offset / MOV DX base pair.
        require(all(f['loc'] in ('offset16','base16') for f in fixes if f['target'] in cs_data),
                'CS data fixup form differs')
        data-=cs_data; code-=cs_data
        # A public of an accepted FAR_DATA module is named by an offset16 and
        # its segment word (base16), like a far code pointer pair.
        from data_only import far_data_publics
        far_data=(code|data)&set(far_data_publics())
        require(all(f['loc'] in ('offset16','base16') and not f['self_relative']
                    for f in fixes if f['target'] in far_data),
                'Far-data fixup form differs')
        code-=far_data; data-=far_data
        require(not code.intersection(data|near|code_offsets) and
                not data.intersection(near|code_offsets) and
                code|data|cs_data|far_data|near|code_offsets|absolute|local==names,
                'Composed fixups need distinct grounded code/data targets')
        from data_symbols import resolve_cs_symbols
        result={**(resolve_code_symbols(code,image,relocations) if code else {}),
                **(resolve_symbols(data,image,relocations) if data else {}),
                **(resolve_cs_symbols(cs_data,image,relocations) if cs_data else {}),
                **(_far_data(far_data,image,relocations) if far_data else {}),
                **(resolve_near_code_symbols(near|code_offsets,recipe,image)
                   if near or code_offsets else {})}
        if seg in local:
            result[seg]={'kind':'local-text','frame_load_address':
                         recipe['original_frame_load_address']}
        if data_base:
            # MSC `_loadds` / `/Au` DS reload: `mov ax,DGROUP` is a base16
            # FIXUPP to the object's zero-length _DATA with the DGROUP group
            # frame; its value is the DGROUP paragraph, grounded by the
            # startup evidence and its ordered MZ relocation (checked_dgroup_layout).
            result['_DATA']={'kind':'local-dgroup-base','frame_load_address':
                             checked_dgroup_layout(image,relocations)['frame_load_address']}
        if absolute:
            from runtime_absolute import ahshift
            result['__AHSHIFT']=ahshift(image,relocations)
        if any(f['target'] in data and f['displacement'] for f in fixes):
            layout=checked_dgroup_layout(image,relocations)
            for f in fixes:
                if f['target'] in data and f['displacement']:
                    width=layout['symbols'][f['target']].get('width')
                    require(type(width) is int and width>0,
                            'Composed data displacement lacks object extent')
                    result[f['target']]['width']=width
        if 'DSEG' in local or any(f['frame_method']==0 and f['frame']=='DSEG'
                                   for f in fixes):
            result['DSEG']={'kind':'local-dseg-base',
                            'frame_load_address':checked_dseg_base(image,relocations)}
            layout=checked_dgroup_layout(image,relocations)
            for f in fixes:
                if f['target'] in data and f['frame_method']==0 and f['frame']=='DSEG':
                    symbol=result[f['target']]
                    symbol['dseg_frame_load_address']=result['DSEG']['frame_load_address']
                    symbol['dseg_frame_proof']='pinned-dgroup-base'
                    if f['displacement']:
                        width=layout['symbols'][f['target']].get('width')
                        require(type(width) is int and width>0,
                                'Composed DSEG addend lacks object extent')
                        symbol['width']=width
        check_folded_recipe(recipe,result,image)
        return result
    if mode == 'asm-local-code-offset16-v1':
        require(names == {recipe['object_segment']}, 'Local code FIXUPP target differs')
        return None
    if mode == 'asm-external-near-transfer-v1':
        return resolve_near_code_symbols(names, recipe, image)
    if mode == 'asm-external-far-call-self-base16-v1':
        require({f['target'] for f in recipe['expected_fixups']
                 if f['target_kind']=='segment'} == {recipe['object_segment']},
                'ASM self-base target differs')
        return resolve_code_symbols(names-{recipe['object_segment']},image,relocations)
    if mode == 'asm-external-dgroup-offset16-v1':
        from data_symbols import resolve_symbols, check_folded_recipe, checked_dseg_base, checked_dgroup_layout
        names |= {f['frame'] for f in recipe['expected_fixups'] if f['frame_method']==2}
        result=resolve_symbols(names,image,relocations)
        if any(f['frame_method']==0 for f in recipe['expected_fixups']):
            layout=checked_dgroup_layout(image,relocations)
            owned=recipe.get('secondary_dgroup_segments',{}).get('DSEG')
            if owned is None:
                base=checked_dseg_base(image,relocations)
                proof='pinned-dgroup-base'
            else:
                base=owned['start']
                require(type(base) is int and
                        base == layout['frame_load_address']+owned['dgroup_offset'] and
                        type(owned['end']) is int and
                        layout['frame_load_address'] <= base < owned['end'] <= layout['bss_start'],
                        'Owned DSEG lacks bounded DGROUP placement proposal')
                proof='secondary-own-data'
            for fix in recipe['expected_fixups']:
                if fix['frame_method']==0:
                    result[fix['target']]['dseg_frame_load_address']=base
                    result[fix['target']]['dseg_frame_proof']=proof
                    if fix['displacement']:
                        width=layout['symbols'][fix['target']].get('width')
                        require(type(width) is int and width > 0,
                                'DSEG displacement lacks reviewed object extent')
                        result[fix['target']]['width']=width
        # integ38: a DGROUP-framed external with an addend (the high word of a
        # dword, `_timer_callback_counter+2`) stays inside its reviewed object
        # extent; the binder checks 0 <= displacement < width.
        for fix in recipe['expected_fixups']:
            if (fix['frame_method']!=0 and fix['target_kind']=='external' and
                    fix['displacement'] and fix['target'] in result):
                width=checked_dgroup_layout(image,relocations)['symbols'][fix['target']].get('width')
                require(type(width) is int and width > 0,
                        'DGROUP displacement lacks reviewed object extent')
                result[fix['target']]['width']=width
        check_folded_recipe(recipe,result,image)
        return result
    if mode in ('external-far-call-v1','asm-external-far-call-v1'):
        return resolve_code_symbols(names,image,relocations)
    if mode == 'external-far-call-code-pointer-v1':
        require(recipe['id']=='remove_frame_callback' and
                names=={'_timer_get_counter_unk','_timer_remove_callback','_frame_callback'},
                'Unreviewed code-pointer-only candidate')
        return {**resolve_code_symbols(names-{'_frame_callback'},image,relocations),
                '_frame_callback':resolve_callback_pointer(image,relocations)}
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


def resolve_near_code_symbols(names, recipe, image):
    """Resolve verified entries or pinned labels in the caller's physical segment."""
    from function_evidence import current_inventory
    import re
    inventory = current_inventory(image)
    frame = recipe['original_frame_load_address']
    require(type(frame) is int and frame % 16 == 0 and
            frame <= recipe['start'] < recipe['end'] <= frame+65536,
            'Near caller lacks a grounded physical segment')
    callers = [f for f in inventory['functions'] if
               f.get('name') == recipe['id'] and f.get('start') == recipe['start'] and
               f.get('segment_paragraph', -1)*16 == frame]
    require(len(callers) == 1, 'Near caller lacks verified segment membership')
    owners = read_json(ROOT/'layout/manifest.json')['owners']
    reviewed=recipe.get('reviewed_near_targets',{})
    require(set(reviewed)<=names, 'Unused reviewed near target')
    result = {}
    for name in names:
        matches = [f for f in inventory['functions'] if name == '_'+f['name'] and
                   f['status'] in ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                                   'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED') and
                   f.get('start_evidence') and f.get('end_evidence') and
                   f.get('segment_paragraph', -1)*16 == frame and
                   sha(image[f['start']:f['end']]) == f['sha256'] and
                   any(_complete_target_owner(owner, f) for owner in owners)]
        if len(matches)==1:
            require(name not in reviewed, 'Inventory entry must not be reclassified as label')
            address=matches[0]['start']
        else:
            require(not matches and name in reviewed,
                    'Near target lacks unique verified same-segment entry or reviewed label: '+name)
            proof=reviewed[name]
            label=re.fullmatch(r'_loc_([0-9A-Fa-f]+)',name)
            require(label is not None and set(proof)=={'source_line'},
                    'Near label proof shape differs')
            address=int(label.group(1),16)-0x10000
            path='src/restunts/asmorig/seg012.asm'
            reference=ROOT/'build/references/restunts'/path
            pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
            require(identity(reference.read_bytes())==pinned,
                    'Near label reference source differs')
            lines=reference.read_text(encoding='latin1').splitlines()
            line=proof['source_line']
            require(type(line) is int and 1<=line<=len(lines) and
                    lines[line-1].strip().lower()==(name[1:]+':').lower(),
                    'Near label lacks pinned source declaration')
            # The containing extent may also be emission-verified with both
            # boundary evidences (as for entries above); the label must still be
            # an original instruction boundary of it (integ30, L3-asm).
            containing=[f for f in inventory['functions'] if
                        f.get('start',10**9)<=address<f.get('end',-1) and
                        (f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                         (f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
                          f.get('start_evidence') and f.get('end_evidence'))) and
                        f.get('segment_paragraph',-1)*16==frame and
                        sha(image[f['start']:f['end']])==f['sha256'] and
                        any(_complete_target_owner(owner,f) for owner in owners)]
            require(len(containing)==1, 'Near label lacks unique verified containing extent')
            import sys
            decoder_path=str(ROOT/'build/python')
            if decoder_path not in sys.path:sys.path.insert(0,decoder_path)
            import capstone
            decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16)
            function=containing[0]
            require(address in {ins.address for ins in decoder.disasm(
                    image[function['start']:function['end']],function['start'])},
                    'Near label is not an original instruction boundary')
        require(frame<=address<frame+65536,'Near target crosses physical segment')
        result[name] = {'kind':'near-code','frame_load_address':frame,
                        'load_address':address}
    return result
