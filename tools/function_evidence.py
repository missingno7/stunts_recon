"""Reviewed bounded instruction/flow overlays for functions outside the small importer."""
from common import ROOT, read_json, require, sha


def reviewed_functions(image):
    path=ROOT/'layout/function-evidence.json'
    if not path.exists(): return {}
    document=read_json(path)
    require(document['oracle_sha256']==sha(image),'Selected function evidence oracle changed')
    result={}
    for f in document['functions']:
        start,end=f['start'],f['end'];code=image[start:end]
        require(len(code)==f['size'] and sha(code)==f['sha256'],'Selected function extent changed')
        rows=f['disassembly'];at=start
        for row in rows:
            raw=bytes.fromhex(row['bytes'])
            require(row['load_offset']==at and image[at:at+len(raw)]==raw,'Selected instruction evidence gap/mismatch')
            at+=len(raw)
        require(at==end,'Selected instruction evidence incomplete')
        for anchor in f['boundary_anchors']:
            raw=bytes.fromhex(anchor['hex']);site=anchor['site']
            require(image[site:site+len(raw)]==raw,'Selected boundary anchor changed')
        # Independently traverse direct 8086 control flow in this bounded subset.
        by={r['load_offset']:bytes.fromhex(r['bytes']) for r in rows}
        pending=[start];seen=set()
        while pending:
            at=pending.pop()
            if at in seen:continue
            require(at in by,'Selected branch/fallthrough not an instruction boundary')
            seen.add(at);raw=by[at];nxt=at+len(raw)
            if raw[0] in (0xcb,0xca,0xc3,0xc2):continue
            if raw[0]==0xeb or 0x70<=raw[0]<=0x7f:
                pending.append(nxt+int.from_bytes(raw[1:],'little',signed=True))
                if raw[0]==0xeb:continue
            elif raw[0]==0xe9:
                pending.append(nxt+int.from_bytes(raw[1:],'little',signed=True));continue
            require(raw[0] not in (0xff,0xea) or (raw[0]==0xff and (raw[1]>>3)&7==6),
                    'Unreviewed indirect control flow in selected function')
            pending.append(nxt)
        padding=set(f['padding_offsets'])
        require(set(by)-seen==padding and all(by[p]==b'\x90' for p in padding),'Unowned/unexpected unreachable bytes')
        require(sorted(seen)==f['reachable_instruction_offsets'],'Reachability evidence changed')
        result[f['name']]=f
    return result


def apply_reviewed(inventory, image):
    reviewed=reviewed_functions(image)
    for index,f in enumerate(inventory['functions']):
        if f['name'] in reviewed:
            overlay=reviewed[f['name']]
            inventory['functions'][index]={**f,**overlay,'status':'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                'confidence':'reviewed_pristine_entry_extent_cfg','prior_mapping_issues':f.get('issues',[]),'issues':[]}
    return reviewed
