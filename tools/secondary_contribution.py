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
    require(code.isdisjoint(data) and code|data==names,
            'Secondary data fixups lack exact reviewed target partition')
    symbols={**(resolve_code_symbols(code,image,relocations) if code else {}),
             **(resolve_symbols(data,image,relocations) if data else {})}
    output=bytearray(raw); occupied=set(); rows=[]; generated=[]
    frame=checked_dgroup_layout(image,relocations)['frame_load_address']
    for fix in fixes:
        at=fix['offset']; width=fix['width']; loc=fix['loc']
        require(not fix['self_relative'] and fix['displacement']==0 and
                type(at)is int and 0<=at<=len(raw)-width and
                not occupied.intersection(range(at,at+width)),
                'Unsupported or overlapping secondary data fixup')
        occupied.update(range(at,at+width))
        encoded=bytes.fromhex(fix['encoded_addend'])
        require(len(encoded)==width and raw[at:at+width]==encoded,
                'Secondary data encoded addend differs')
        if fix['target_kind']=='external':
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
            addend=int.from_bytes(encoded[:2],'little')
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
        require(name in ('_DATA', 'CONST', '_BSS') and name in groups[0]['segments']
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
                     fix['frame_kind'], fix['frame'], fix['frame_index'], fix['displacement']) ==
                    (code, 'offset16', 2, False, 0, definitions[name]['index'],
                     1, 'group', 'DGROUP', groups[0]['index'], 0),
                    'Unsupported own-data CODE fixup')
            at = fix['offset']
            encoded = bytes.fromhex(fix['encoded_addend'])
            addend = int.from_bytes(encoded, 'little')
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
    require(all(f['segment'] in specs or f['segment']==code for f in obj.linker_fixups),
            'Fixup in unowned segment')
    used={f['target'] for f in obj.linker_fixups if f['segment'] in specs and
          f['target_kind']=='external'}
    declared=recipe.get('secondary_external_targets',{})
    require(set(declared.get('code',[]))|set(declared.get('data',[]))==used,
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
    own=[f for f in obj.linker_fixups if f['target_kind']=='segment' and
         f['target'] in specs]
    external=[f for f in obj.linker_fixups if f['segment']==code and f not in own]
    view=copy.copy(obj)
    view.segment_lengths={**obj.segment_lengths,**{name:0 for name in specs}}
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
