"""Reviewed bounded instruction/flow overlays for functions outside the small importer."""
import copy
from common import ROOT, read_json, require, sha


def current_inventory(image):
    """Apply reviewed overlays in memory; the checked base inventory stays immutable."""
    inventory=copy.deepcopy(read_json(ROOT/'evidence/functions.json'))
    apply_reviewed(inventory, image)
    return inventory


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
        islands=f.get('data_islands',[])
        if islands:
            require(f.get('verification_kind')=='embedded_jump_table16' and
                    f['name']=='sin_fast' and (start,end)==(141022,141100) and
                    len(islands)==1, 'Unsupported reviewed code data island')
            island=islands[0]; raw=bytes.fromhex(island['hex'])
            require(island['load_offset']==141042 and len(raw)==8 and
                    island['kind']=='jump_table16' and
                    island['indirect_jump_site']==141037 and
                    image[141042:141050]==raw and f.get('frame_load_address')==125472,
                    'Reviewed sine jump table differs')
            require(f.get('bytes_hex')==code.hex() and f.get('start_evidence') and
                    f.get('end_evidence'), 'Reviewed sine emission/boundary proof missing')
        for row in rows:
            raw=bytes.fromhex(row['bytes'])
            if islands and at==islands[0]['load_offset']:
                at+=len(bytes.fromhex(islands[0]['hex']))
            require(row['load_offset']==at and image[at:at+len(raw)]==raw,'Selected instruction evidence gap/mismatch')
            at+=len(raw)
        # Optional, independently sourced raw word-alignment byte immediately after RETF.
        alignment=f.get('alignment_padding',[])
        if alignment:
            require(len(alignment)==1,'Selected alignment padding must be a single reviewed byte')
            pad=alignment[0];raw=bytes.fromhex(pad['hex'])
            require(pad['load_offset']==at and len(raw)==1 and at+len(raw)==end,
                    'Selected alignment byte is not terminal/contiguous')
            require(raw==b'\x00' and pad.get('kind')=='word_alignment' and
                    pad.get('attribution')=='preceding-procedure',
                    'Selected alignment byte is not a reviewed terminal zero pad')
            prov=pad.get('provenance',{})
            require(prov.get('repository')=='restunts' and len(prov.get('commit',''))==40 and
                    prov.get('path','').endswith('.asm') and isinstance(prov.get('line'),int) and
                    'align 2' in prov.get('source_text','') and 'db 0' in prov.get('source_text',''),
                    'Selected alignment padding lacks exact source provenance')
            terminal=bytes.fromhex(rows[-1]['bytes']) if rows else b''
            nonreturn=f.get('terminal_nonreturn_call')
            shared=f.get('terminal_shared_jump')
            shared_ok=False
            if shared and rows and rows[-1]['load_offset']==shared.get('site'):
                branch=bytes.fromhex(shared['hex'])
                target=shared.get('target')
                proof_rows={r['load_offset']:r for r in rows}
                if branch and branch[0] in (0xeb,0xe9) and len(branch) in (2,3):
                    displacement=int.from_bytes(branch[1:], 'little', signed=True)
                    tail_at=target
                    tail_returns=False
                    visited=set()
                    while tail_at in proof_rows and tail_at not in visited:
                        visited.add(tail_at)
                        tail_raw=bytes.fromhex(proof_rows[tail_at]['bytes'])
                        if tail_raw[0] in (0xcb,0xca,0xc3,0xc2):
                            tail_returns=True
                            break
                        if tail_raw[0] in (0xeb,0xe9) or 0x70<=tail_raw[0]<=0x7f:
                            break
                        tail_at+=len(tail_raw)
                    shared_ok=(terminal==branch and at==shared['site']+len(branch)
                               and at+displacement==target and target in proof_rows and tail_returns)
            require(terminal[:1] in (b'\xcb',b'\xca',b'\xc3',b'\xc2') or
                    (f['name']=='fatal_error' and nonreturn==
                     {'site':125512,'hex':'9a7617c51c','source_line':405,'callee':'_abort'}
                     and terminal==bytes.fromhex(nonreturn['hex'])) or shared_ok,
                    'Selected alignment byte lacks terminal return/nonreturn/shared-return-jump proof')
            require(image[at:at+1]==raw,'Selected alignment byte differs from pristine image')
            at+=len(raw)
        require(at==end,'Selected instruction evidence incomplete')
        for anchor in f['boundary_anchors']:
            raw=bytes.fromhex(anchor['hex']);site=anchor['site']
            require(image[site:site+len(raw)]==raw,'Selected boundary anchor changed')
        # Independently traverse direct 8086 control flow in this bounded subset.
        by={r['load_offset']:bytes.fromhex(r['bytes']) for r in rows}
        table_targets=[]
        if islands:
            table_targets=[f['frame_load_address']+int.from_bytes(bytes.fromhex(islands[0]['hex'])[i:i+2],'little')
                           for i in range(0,8,2)]
            require(len(set(table_targets))==4 and all(t in by for t in table_targets)
                    and by[141037]==bytes.fromhex('2effa7d23c') and
                    f['frame_load_address']+int.from_bytes(by[141037][-2:],'little')==141042,
                    'Reviewed sine indirect branches differ')
        pending=[start];seen=set()
        while pending:
            at=pending.pop()
            if at in seen:continue
            require(at in by,'Selected branch/fallthrough not an instruction boundary')
            seen.add(at);raw=by[at];nxt=at+len(raw)
            if f.get('terminal_nonreturn_call') and at==f['terminal_nonreturn_call']['site']:
                require(raw==bytes.fromhex(f['terminal_nonreturn_call']['hex']) and nxt==end-1,
                        'Reviewed nonreturn call moved from terminal position')
                continue
            if raw[0] in (0xcb,0xca,0xc3,0xc2):continue
            if raw[0]==0xeb or 0x70<=raw[0]<=0x7f:
                pending.append(nxt+int.from_bytes(raw[1:],'little',signed=True))
                if raw[0]==0xeb:continue
            elif raw[0]==0xe9:
                pending.append(nxt+int.from_bytes(raw[1:],'little',signed=True));continue
            # FF /2 and /3 are returning indirect calls; only /4 and /5 are indirect jumps.
            if islands and at==141037:
                pending.extend(table_targets)
                continue
            require(raw[0]!=0xea and (raw[0]!=0xff or ((raw[1]>>3)&7) not in (4,5)),
                    'Unreviewed indirect jump in selected function')
            pending.append(nxt)
        padding=set(f['padding_offsets'])
        require(set(by)-seen==padding and all(by[p]==b'\x90' for p in padding),'Unowned/unexpected unreachable bytes')
        require(sorted(seen)==f['reachable_instruction_offsets'],'Reachability evidence changed')
        require(f['name'] not in result,'Duplicate reviewed function name')
        result[f['name']]=f
    return result


def apply_reviewed(inventory, image):
    reviewed=reviewed_functions(image)
    existing={f['name'] for f in inventory['functions']}
    for index,f in enumerate(inventory['functions']):
        if f['name'] in reviewed:
            overlay=reviewed[f['name']]
            status=('BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' if overlay.get('verification_kind')=='embedded_jump_table16'
                    else 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')
            inventory['functions'][index]={**f,**overlay,'bytes_hex':overlay.get('bytes_hex'),
                'status':status,
                'confidence':'reviewed_pristine_entry_extent_cfg','prior_mapping_issues':f.get('issues',[]),'issues':[]}
    for name, overlay in reviewed.items():
        if name in existing: continue
        require(overlay.get('local_symbol') is True and overlay.get('stable_id') ==
                'load_%05x' % overlay['start'] and overlay.get('provenance') and
                overlay.get('segment'),
                'New reviewed entry requires a bounded local-symbol proof')
        inventory['functions'].append({**overlay,'bytes_hex':overlay.get('bytes_hex'),
            'status':'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
            'confidence':'reviewed_pristine_local_entry_extent_cfg',
            'origin':'UNCLASSIFIED; local OMF symbol is candidate evidence'})
    return reviewed
