"""Strict complete adjacent C functions with internal near CALL fixups."""
import copy, struct
from common import ROOT, identity, read_json, require, sha
from binder import bind_contribution
from code_symbols import resolve_recipe_symbols
from function_evidence import reviewed_functions
from secondary_contribution import bind_secondary
def checked_members(recipe, image):
    inv=read_json(ROOT/'evidence/functions.json')
    require(inv['load_sha256']==sha(image),'Inventory/oracle identity differs')
    members=recipe['members']
    require(type(members) is list and len(members)>=2, 'Multi recipe needs members')
    require(len(members)>=2 and recipe['start']==members[0]['start'] and recipe['end']==members[-1]['end'],'Bad multi interval')
    require(identity(image[recipe['start']:recipe['end']])==recipe['target'],'Whole target identity differs')
    for i,m in enumerate(members):
        require(i==0 or members[i-1]['end']==m['start'],'Member gap/overlap')
        rows=[f for f in inv['functions'] if f.get('name')==m['name']]
        require(len(rows)==1,'Missing/ambiguous member')
        f=rows[0]
        verified = f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or (
            f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
            f.get('bytes_hex') and f.get('start_evidence') and f.get('end_evidence') and
            bytes.fromhex(f['bytes_hex'])==image[f['start']:f['end']])
        require(verified,'Unverified member boundary/emission bytes')
        expected_public = f['name'] if f.get('local_symbol') else '_' + f['name']
        require(all(m.get(k)==f.get(k) for k in ('stable_id','start','end')) and
                m['public']==expected_public and
                bool(m.get('local_symbol'))==bool(f.get('local_symbol')) and
                m['target']=={'size':f['size'],'sha256':f['sha256']},
                'Member evidence differs')
        require(identity(image[f['start']:f['end']])==m['target'],'Member bytes differ')
    reviewed_functions(image)
def bind_multi(obj, recipe, image, relocations):
    seg=recipe['object_segment']; length=recipe['end']-recipe['start']
    require(obj.segment_length(seg)==length and len(obj.segment_bytes(seg))==length,'Incomplete CODE extent')
    secondary=recipe.get('secondary_dgroup_segments',{})
    require(all(n==seg or n in secondary or z==0 for n,z in obj.segment_lengths.items()),
            'Unowned data/BSS')
    pubs={m['public']:m['start']-recipe['start'] for m in recipe['members']}
    require(len(pubs)==len(recipe['members']) and len(obj.publics)==len(pubs) and {p['name']:p['offset'] for p in obj.publics if p['segment']==seg}==pubs,'Public offsets differ')
    require({p['name'] for p in obj.local_publics} ==
            {m['public'] for m in recipe['members'] if m.get('local_symbol')},
            'Local helper publics must be complete reviewed members')
    require(recipe['object_declarations']=={'segments':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals},'Full declarations differ')
    require(obj.linker_fixups==recipe['expected_fixups'],'Ordered FIXUPP differs')
    require(set(obj.externals) <= set(pubs) | {'__acrtused'} | {f['target'] for f in obj.linker_fixups}, 'Unexpected external declaration')
    internal=[f for f in obj.linker_fixups if f['target'] in pubs]
    own_data=[f for f in obj.linker_fixups if f['target_kind']=='segment' and
              f['target'] in secondary]
    external=[f for f in obj.linker_fixups if f['segment']==seg and
              f not in internal and f not in own_data]
    require(all(f['self_relative'] for f in internal) and all(not f['self_relative'] for f in external),'Unsupported relative target')
    if external:
        require('external_binding' in recipe,'External binding absent')
        view=copy.copy(obj); view.publics=[{'name':recipe['members'][0]['public'],'segment':seg,'offset':0}]
        view.local_publics=[]; view.local_externals=[]
        view.externals=[n for n in obj.externals if n not in pubs or n==view.publics[0]['name']]
        view.segment_lengths={**obj.segment_lengths,
                              **{name:0 for name in secondary}}
        view.linker_fixups=[]
        for f in external:
            row=dict(f); row['target_index']=view.externals.index(f['target'])+1; view.linker_fixups.append(row)
        sub={'start':recipe['start'],'end':recipe['end'],'object_segment':seg,'public':view.publics[0]['name'],'expected_fixups':view.linker_fixups,'expected_relocations':recipe['expected_relocations'],'binding':{'mode':recipe['external_binding']['mode'],'declarations':{'segments':view.segment_defs,'groups':view.groups,'publics':view.publics,'externals':view.externals}}}
        symbols=resolve_recipe_symbols(sub,image,relocations)
        payload,receipt=bind_contribution(view,sub,symbols)
    else:
        require(recipe['expected_relocations']==[] and 'external_binding' not in recipe,'Unexpected external obligations')
        payload,receipt=obj.segment_bytes(seg),{'mode':'no-external-fixups','generated_relocations':[]}
    linked=bytearray(payload); rows=[]
    linked, data_payloads, secondary_rows, data_relocations=bind_secondary(
        obj,recipe,image,relocations,linked,own_data)
    linked=bytearray(linked)
    for f in internal:
        at=f['offset']
        require(f['segment']==seg and f['loc']=='offset16' and f['width']==2 and f['target_kind']=='external' and f['target_method']==2 and (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==(5,'target',f['target'],0) and 1<=f['target_index']<=len(obj.externals) and obj.externals[f['target_index']-1]==f['target'] and f['displacement']==0 and f['encoded_addend']=='0000' and 1<=at<=length-2 and linked[at-1]==0xe8 and linked[at:at+2]==bytes(2),'Unsupported internal CALL fixup')
        disp=pubs[f['target']]-(at+2)
        require(-32768<=disp<=32767,'Internal displacement overflow')
        struct.pack_into('<h',linked,at,disp)
        rows.append({'offset':at,'target':f['target'],'displacement':disp})
    generated=receipt['generated_relocations']
    require(generated==recipe['expected_relocations'],'Relocation order differs')
    return bytes(linked),{'mode':'multi-function-complete-v2','internal_calls':rows,
        'secondary_dgroup_fixups':secondary_rows,
        'secondary_payloads':{name:raw.hex() for name,raw in data_payloads.items()},
        'secondary_generated_relocations':data_relocations,
        'external':receipt,'generated_relocations':generated}
