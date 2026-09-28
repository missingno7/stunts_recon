"""Strict complete DGROUP contributions emitted by one C translation unit."""
import struct
import copy
from common import ROOT, identity, read_json, require
from data_symbols import checked_dgroup_layout


def _data_fixups(obj, recipe, name, raw, specs, image, relocations):
    """Apply supported complete OMF data fixups from reviewed targets."""
    from code_symbols import resolve_code_symbols
    from data_symbols import resolve_symbols
    from binder import _checked_data_addend
    fixes=[f for f in obj.linker_fixups if f['segment']==name]
    if not fixes:
        return raw, [], []
    names={f['target'] for f in fixes if f['target_kind']=='external'}
    declared=recipe.get('secondary_external_targets',{})
    code=set(declared.get('code',[])); data=set(declared.get('data',[]))
    far=set(declared.get('far_data',[]))
    require(code.isdisjoint(data) and far.isdisjoint(code|data) and code|data|far==names,
            'Secondary data fixups lack exact reviewed target partition')
    symbols={**(resolve_code_symbols(code,image,relocations) if code else {}),
             **(resolve_symbols(data,image,relocations) if data else {})}
    if far:
        from data_only import far_data_segment_targets
        far_targets=far_data_segment_targets(far,image,relocations)
    output=bytearray(raw); occupied=set(); rows=[]; generated=[]
    frame=checked_dgroup_layout(image,relocations)['frame_load_address']
    for fix in fixes:
        at=fix['offset']; width=fix['width']; loc=fix['loc']
        require(not fix['self_relative'] and
                (fix['displacement']==0 or
                 (recipe.get('kind')=='asm' and fix['target_kind']=='segment')) and
                type(at)is int and 0<=at<=len(raw)-width and
                not occupied.intersection(range(at,at+width)),
                'Unsupported or overlapping secondary data fixup')
        occupied.update(range(at,at+width))
        encoded=bytes.fromhex(fix['encoded_addend'])
        require(len(encoded)==width and raw[at:at+width]==encoded,
                'Secondary data encoded addend differs')
        if fix['target_kind']=='external' and fix['target'] in far:
            # integ33: `dw seg X` naming a public of an accepted far-data module.
            # MASM frames the EXTDEF itself (F2); LINK writes the paragraph of
            # X's FAR_DATA segment and an MZ relocation.  The word must be one
            # of that module's own reviewed placement anchors (the pinned
            # `dw seg` declaration of this very alias, rechecked here), so the
            # binding is grounded independently of the candidate.
            from data_only import _checked_far_anchor
            target=far_targets[fix['target']]
            site=specs[name]['start']+at
            anchors=[a for a in target['anchors'] if a.get('kind')=='data-segment-word' and
                     (a.get('relocation') or {}).get('load_offset')==site]
            here={p['name'] for p in obj.publics if p['segment']==name and p['offset']==at}
            require(loc=='base16' and width==2 and encoded==bytes(2) and
                    fix['displacement']==0 and fix['target_method']==2 and
                    1<=fix['target_index']<=len(obj.externals) and
                    obj.externals[fix['target_index']-1]==fix['target'] and
                    (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                    (2,'external',fix['target'],fix['target_index']) and
                    len(anchors)==1 and anchors[0].get('alias') in here and
                    target['start']%16==0,
                    'Unsupported far-data segment word')
            _checked_far_anchor(anchors[0],target['start'],image,relocations)
            base,offset=target['start'],0
        elif fix['target_kind']=='external':
            require(fix['target_method']==2 and
                    1<=fix['target_index']<=len(obj.externals) and
                    obj.externals[fix['target_index']-1]==fix['target'] and
                    (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                    (5,'target',fix['target'],0),
                    'Unsupported secondary external datum/frame')
            target=symbols[fix['target']]
            base,address=target['frame_load_address'],target['load_address']
            require(type(base)is int and type(address)is int and
                    base%16==0 and 0<=address-base<=65535,
                    'Secondary external target frame/address invalid')
            addend=(0 if target.get('kind')=='far-code' else
                    _checked_data_addend(target,encoded[:2]))
            if target.get('kind')=='far-code':
                require(encoded[:2]==b'\0\0','Code pointer addend unsupported')
            else:
                require(target['group']=='DGROUP','Secondary data target not DGROUP')
            offset=address-base+addend
        elif (fix['target_kind']=='segment' and recipe.get('kind')=='asm' and
              fix['target']==recipe['object_segment']):
            # integ31: a whole ASM module's data naming its own code (the
            # `dw offset proc` / `dw seg proc` far callback of asm012_133660):
            # the module's own segment frame (F0) at its original, reviewed
            # code frame; the displacement stays inside the module and the
            # segment word's MZ relocation is an ordered data obligation.
            definition,=[d for d in obj.segment_defs if d['name']==fix['target']]
            code_frame=recipe.get('original_frame_load_address')
            length=recipe['end']-recipe['start']
            require('module_proof' in recipe and fix['target_method']==0 and
                    fix['target_index']==definition['index'] and
                    (fix['frame_method'],fix['frame_kind'],fix['frame'],fix['frame_index'])==
                    (0,'segment',fix['target'],definition['index']) and
                    type(code_frame) is int and code_frame%16==0 and
                    code_frame<=recipe['start'] and recipe['end']<=code_frame+65536 and
                    loc in ('offset16','base16') and encoded==bytes(width) and
                    type(fix['displacement']) is int and 0<=fix['displacement']<length,
                    'Unsupported own-code data fixup')
            base=code_frame
            offset=recipe['start']-code_frame+fix['displacement']
        elif fix['target_kind']=='segment':
            target_name=fix['target']
            require(target_name in specs and
                    (fix['frame_method'],fix['frame_kind'],fix['frame'])==
                    (1,'group','DGROUP'),
                    'Unsupported secondary segment target/frame')
            definition,=[d for d in obj.segment_defs if d['name']==target_name]
            require(fix['target_method']==0 and
                    fix['target_index']==definition['index'],
                    'Secondary segment datum differs')
            base=frame; address=specs[target_name]['start']
            addend=_own_addend(fix,encoded[:2],recipe)
            require(addend<specs[target_name]['end']-address,
                    'Secondary segment addend escapes complete object')
            offset=address-base+addend
        elif fix['target_kind']=='group':
            require(fix['target']=='DGROUP' and loc=='base16' and
                    fix['target_method']==1,
                    'Unsupported secondary group base fixup')
            base=frame;offset=0
        else:
            require(False,'Unsupported secondary data FIXUPP target kind')
        require(0<=offset<=65535,'Secondary data offset overflow')
        if loc=='offset16' or loc=='loader-offset16':
            require(width==2,'Invalid secondary offset width')
            struct.pack_into('<H',output,at,offset)
            value=offset; relocated=None
        elif loc=='base16':
            require(width==2 and encoded==bytes(2),'Invalid secondary base addend')
            value=base//16;struct.pack_into('<H',output,at,value)
            relocated=at
        elif loc=='pointer32':
            require(width==4 and encoded[2:]==bytes(2),'Invalid secondary far pointer addend')
            struct.pack_into('<HH',output,at,offset,base//16)
            value=[offset,base//16];relocated=at+2
        else:
            require(False,'Unsupported secondary data fixup location')
        if relocated is not None:
            site=specs[name]['start']+relocated
            generated.append({'segment':(site//65536)*4096,
                              'offset':site%65536,'load_offset':site})
        rows.append({'segment':name,'offset':at,'loc':loc,
                     'target':fix['target'],'linked_value':value})
    return bytes(output), rows, generated


def _own_addend(fix, encoded, recipe):
    """The in-segment offset of an own-data FIXUPP.  MSC stores it in the
    LEDATA word (displacement 0); MASM 5.10 stores it as the FIXUPP target
    displacement over a zero word (integ31, ASM module _DATA)."""
    addend = int.from_bytes(encoded, 'little')
    if fix['displacement']:
        require(recipe.get('kind') == 'asm' and addend == 0 and
                type(fix['displacement']) is int and 0 < fix['displacement'] < 65536,
                'Own-data displacement form differs')
        addend = fix['displacement']
    return addend


def check_data_publics(obj, specs, code):
    """Global data defined by the TU lies inside its complete placed segment
    (integ26). A public spelled like a reviewed DGROUP alias must be placed at
    that alias's address: one binding name never names two addresses."""
    layout = read_json(ROOT/'layout/data-symbols.json')
    for public in obj.publics:
        if public['segment'] == code or public['segment'] == '?0':
            continue
        require(public['segment'] in specs, 'Data public in an unowned segment')
        spec = specs[public['segment']]
        require(type(public['offset']) is int and
                0 <= public['offset'] < spec['end'] - spec['start'],
                'Data public outside its complete placed segment')
        alias = layout['symbols'].get(public['name'])
        require(alias is None or
                alias['load_address'] == spec['start'] + public['offset'],
                'Data public conflicts with the reviewed alias address: ' + public['name'])


def bind_secondary(obj, recipe, image, relocations, code_payload, code_fixups):
    specs = recipe.get('secondary_dgroup_segments', {})
    code = recipe['object_segment']
    nonempty = {name for name, size in obj.segment_lengths.items()
                if size and name != code}
    require(set(specs) == nonempty, 'Unowned data/BSS or unused secondary specification')
    if not specs:
        return bytes(code_payload), {}, [], []
    layout = checked_dgroup_layout(image,relocations)
    frame = layout['frame_load_address']
    groups = [g for g in obj.groups if g['name'] == 'DGROUP']
    require(len(groups) == 1, 'Missing/ambiguous secondary DGROUP')
    definitions = {row['name']: row for row in obj.segment_defs}
    code_bytes = bytearray(code_payload)
    contribution = {}
    proof = []
    generated = []
    occupied = set()
    extents = []
    relocation_sites = {row['load_offset'] for row in relocations}
    for name, spec in specs.items():
        require(name in ('_DATA', 'CONST', '_BSS', 'DSEG') and name in groups[0]['segments']
                and name in definitions, 'Unsupported secondary segment')
        start, end = spec['start'], spec['end']
        size = obj.segment_length(name)
        require(type(start) is int and type(end) is int and
                start == frame + spec['dgroup_offset'] and end-start == size and
                0 <= spec['dgroup_offset'] < 65536 and
                spec['dgroup_offset']+size <= 65536,
                'Secondary placement or full SEGDEF length differs')
        require(all(end <= left or right <= start for left, right in extents),
                'Secondary placements overlap')
        extents.append((start, end))
        if name == '_BSS':
            require(layout['bss_start'] <= start < end <= layout['bss_end'] and
                    name not in obj.segments and
                    image[start:min(end,len(image))] == bytes(max(0,min(end,len(image))-start)),
                    'Secondary BSS is not zero-initialized inside verified BSS')
            raw = bytes(size)
        else:
            require(end <= layout['bss_start'] and end <= len(image),
                    'Initialized secondary escapes image/DGROUP')
            raw = obj.segment_bytes(name)
            require(len(raw) == size, 'Incomplete initialized secondary bytes')
        bound, data_rows, data_relocations=_data_fixups(
            obj,recipe,name,raw,specs,image,relocations)
        require(identity(bound)==spec['target'] and
                (name=='_BSS' or bound==image[start:end]),
                'Secondary complete bytes/fixups differ from oracle')
        expected=[r for r in relocations if start<=r['load_offset']<end]
        require(data_relocations==expected==spec.get('expected_relocations',[]),
                'Secondary data ordered relocations differ')
        proof.extend(data_rows)
        generated.extend(data_relocations)
        own = [f for f in code_fixups if f['target_kind'] == 'segment' and
               f['target'] == name]
        require(own, 'Secondary placement lacks a code reference')
        for fix in own:
            require((fix['segment'], fix['loc'], fix['width'], fix['self_relative'],
                     fix['target_method'], fix['target_index'], fix['frame_method'],
                     fix['frame_kind'], fix['frame'], fix['frame_index']) ==
                    (code, 'offset16', 2, False, 0, definitions[name]['index'],
                     1, 'group', 'DGROUP', groups[0]['index']) and
                    type(fix['displacement']) is int,
                    'Unsupported own-data CODE fixup')
            at = fix['offset']
            encoded = bytes.fromhex(fix['encoded_addend'])
            addend = _own_addend(fix, encoded, recipe)
            require(len(encoded) == 2 and 0 <= at <= len(code_bytes)-2 and
                    at not in occupied and at+1 not in occupied and
                    code_bytes[at:at+2] == encoded and 0 <= addend < size,
                    'Own-data code fixup or addend escapes contribution')
            absolute = recipe['start']+at
            require(absolute not in relocation_sites and absolute+1 not in relocation_sites,
                    'Own-data offset unexpectedly relocated')
            occupied.update((at,at+1))
            value = spec['dgroup_offset']+addend
            require(value <= 65535 and image[absolute:absolute+2] == value.to_bytes(2,'little'),
                    'Own-data code reference disagrees with oracle placement')
            struct.pack_into('<H',code_bytes,at,value)
            proof.append({'segment':name,'code_offset':at,'addend':addend,'value':value})
        contribution[name] = bound
    check_data_publics(obj, specs, code)
    require(all(f['segment'] in specs or f['segment']==code for f in obj.linker_fixups),
            'Fixup in unowned segment')
    used={f['target'] for f in obj.linker_fixups if f['segment'] in specs and
          f['target_kind']=='external'}
    declared=recipe.get('secondary_external_targets',{})
    require(set(declared.get('code',[]))|set(declared.get('data',[]))|
            set(declared.get('far_data',[]))==used,
            'Unused secondary external declarations')
    return bytes(code_bytes), contribution, proof, generated


def bind_single_secondary(obj, recipe, image, relocations):
    """Reuse the exact single-C external binder while owning every SEGDEF."""
    from binder import bind_contribution
    from code_symbols import resolve_recipe_symbols
    code=recipe['object_segment']
    specs=recipe.get('secondary_dgroup_segments',{})
    require(recipe['object_declarations']==
            {'segments':obj.segment_defs,'groups':obj.groups,
             'publics':obj.publics,'externals':obj.externals},
            'Complete secondary C declarations differ')
    require(obj.linker_fixups==recipe['expected_fixups'],
            'Complete ordered secondary C FIXUPPs differ')
    own=[f for f in obj.linker_fixups if f['segment']==code and
         f['target_kind']=='segment' and f['target'] in specs]
    external=[f for f in obj.linker_fixups if f['segment']==code and f not in own]
    view=copy.copy(obj)
    view.segment_lengths={**obj.segment_lengths,**{name:0 for name in specs}}
    # Global data the TU defines in its own placed segments (integ31, seg030)
    # is checked by bind_secondary/check_data_publics; the CODE binder sees
    # only the code public.
    view.publics=[p for p in obj.publics if p['segment'] not in specs]
    view.segment_defs=[{**d,'length':0} if d['name'] in specs else d
                       for d in obj.segment_defs]
    view.linker_fixups=external
    sub={**recipe,'expected_fixups':external}
    if external:
        require('binding' in recipe,'External binding missing')
        sub['binding']={**recipe['binding'],'declarations':
                        {'segments':view.segment_defs,'groups':view.groups,
                         'publics':view.publics,'externals':view.externals}}
    else:
        sub.pop('binding',None)
    symbols=resolve_recipe_symbols(sub,image,relocations)
    payload,receipt=bind_contribution(view,sub,symbols)
    linked,segments,proof,data_relocs=bind_secondary(
        obj,recipe,image,relocations,payload,own)
    generated=receipt['generated_relocations']
    require(generated==recipe['expected_relocations'],
            'Secondary C relocation order differs')
    return linked, {'mode':'single-with-secondary-v1',
                    'external':receipt,'secondary_dgroup_fixups':proof,
                    'secondary_payloads':{n:b.hex() for n,b in segments.items()},
                    'secondary_generated_relocations':data_relocs,
                    'generated_relocations':generated}
