"""Resolve far runtime publics using pinned owners and independent frame anchors."""
from common import ROOT, read_json, require, sha


def resolve_code_symbols(names, image, relocations):
    from library import bind_library
    layout=read_json(ROOT/'layout/code-symbols.json')
    require(layout['oracle_sha256']==sha(image), 'Code symbols belong to another oracle')
    owners=read_json(ROOT/'layout/manifest.json')['owners']; result={}
    for name in names:
        require(name in layout['symbols'], 'Unknown far code symbol: '+name)
        symbol=layout['symbols'][name]
        found=[o for o in owners if o['id']==symbol['owner'] and o['kind']=='KNOWN_TOOLCHAIN_LIBRARY']
        require(len(found)==1, 'Code symbol lacks pinned active library owner')
        owner=found[0]; payload,_=bind_library(owner,image,relocations)
        public=[p for p in owner['publics'] if p['name']==name]
        require(len(public)==1, 'Code public missing/ambiguous')
        address=owner['start']+public[0]['offset'];frame=symbol['frame_load_address']
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
    if recipe.get('binding',{}).get('mode')=='external-far-call-v1':
        return resolve_code_symbols(names,image,relocations)
    from data_symbols import resolve_symbols
    return resolve_symbols(names,image,relocations)
