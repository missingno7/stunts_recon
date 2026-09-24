"""Resolve reviewed far code targets using owned publics or pristine call anchors."""
from common import ROOT, read_json, require, sha


def resolve_code_symbols(names, image, relocations):
    from library import bind_library
    layout=read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256']==sha(image), 'Code symbols belong to another oracle')
    owners=read_json(ROOT/'layout/manifest.json')['owners']; result={}
    for name in names:
        require(name in layout['symbols'], 'Unknown far code symbol: '+name)
        symbol=layout['symbols'][name]
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
            # entry, complete raw or exact active-C ownership, and two
            # independent relocated call sites.
            target=symbol['mapped_target']
            inventory=read_json(ROOT/'recovery/restunts-inventory.json')
            matches=[f for f in inventory['functions'] if f.get('stable_id')==target['stable_id']
                     and f.get('name')==target['name']
                     and f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED']
            require(len(matches)==1, 'Far code alias lacks unique verified mapped target')
            function=matches[0]
            require(target['start']==function['start'] and target['end']==function['end']
                    and target['sha256']==function['sha256']
                    and sha(image[target['start']:target['end']])==target['sha256'],
                    'Far code target extent/hash changed')
            require(any((o['kind']=='UNRESOLVED_RAW' and o['start']<=target['start']
                         and target['end']<=o['end']) or
                        (o['kind']=='MATCHING_C' and o.get('name')==function['name']
                         and o['start']==target['start'] and o['end']==target['end'])
                        for o in owners),
                    'Mapped target lacks a complete raw or exact active-C owner')
            address=target['start']
            require(len(symbol['anchors'])>=2 and len({a['caller_task'] for a in symbol['anchors']})>=2,
                    'Raw code target needs independent callers')
            for anchor in symbol['anchors']:
                callers=[f for f in inventory['functions'] if f.get('stable_id')==anchor['caller_task']
                         and f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
                         and f['start']<=anchor['site'] and anchor['site']+5<=f['end']]
                require(len(callers)==1 and not target['start']<=anchor['site']<target['end'],
                        'Far code anchor lacks independent verified caller')
        frame=symbol['frame_load_address']
        require(symbol['anchors'], 'Code frame needs independent evidence')
        for anchor in symbol['anchors']:
            at=anchor['site']; raw=bytes.fromhex(anchor['hex'])
            require(len(raw)==5 and raw[0]==0x9a and image[at:at+5]==raw, 'Code frame anchor changed')
            require(anchor['relocation'] in relocations and anchor['relocation']['load_offset']==at+3,
                    'Code anchor lacks segment relocation')
            require(int.from_bytes(raw[3:],'little')*16==frame and
                    int.from_bytes(raw[1:3],'little')+frame==address, 'Code frame/public conflict')
        result[name]={'kind':'far-code','frame_load_address':frame,'load_address':address}
    return result


def resolve_recipe_symbols(recipe, image, relocations):
    names={f['target'] for f in recipe['expected_fixups']}
    if not names:return None
    mode=recipe.get('binding',{}).get('mode')
    if mode=='external-far-call-v1':
        return resolve_code_symbols(names,image,relocations)
    if mode=='external-far-call-dgroup-offset16-v1':
        code={f['target'] for f in recipe['expected_fixups'] if f['loc']=='pointer32'}
        data={f['target'] for f in recipe['expected_fixups'] if f['loc']=='offset16'}
        require(code and data and not code.intersection(data) and code|data==names,
                'Mixed binding needs distinct reviewed code and data targets')
        from data_symbols import resolve_symbols
        return {**resolve_code_symbols(code,image,relocations),
                **resolve_symbols(data,image,relocations)}
    from data_symbols import resolve_symbols
    return resolve_symbols(names,image,relocations)
