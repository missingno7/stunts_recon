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
        island_by={}
        for island in islands:
            raw=bytes.fromhex(island['hex']);island_start=island['load_offset']
            require(raw and start<=island_start and island_start+len(raw)<=end and
                    island_start not in island_by and
                    island.get('kind') in ('jump_table16','lookup_table8') and
                    image[island_start:island_start+len(raw)]==raw and
                    (not island.get('sha256') or sha(raw)==island['sha256']),
                    'Unsupported or changed reviewed code data island')
            island_by[island_start]=(raw,island)
        if islands:
            require(f.get('bytes_hex')==code.hex() and f.get('boundary_anchors') and
                    f.get('boundary_proof'), 'Reviewed embedded data emission/boundary proof missing')
            require(f.get('verification_kind') in ('embedded_data_dispatch_v1',
                    'embedded_jump_table16_mat_rot_zxy_v1') or
                    (f.get('verification_kind')=='embedded_jump_table16' and
                     f['name']=='sin_fast' and (start,end)==(141022,141100) and
                     len(islands)==1 and islands[0]['load_offset']==141042 and
                     len(bytes.fromhex(islands[0]['hex']))==8 and
                     islands[0]['indirect_jump_site']==141037 and
                     f.get('frame_load_address')==125472),
                    'Unsupported reviewed code data island')
        for row in rows:
            while at in island_by: at+=len(island_by[at][0])
            raw=bytes.fromhex(row['bytes'])
            require(row['load_offset']==at and image[at:at+len(raw)]==raw,'Selected instruction evidence gap/mismatch')
            at+=len(raw)
        while at in island_by: at+=len(island_by[at][0])
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
        while at in island_by: at+=len(island_by[at][0])
        require(at==end,'Selected instruction evidence incomplete')
        for anchor in f['boundary_anchors']:
            raw=bytes.fromhex(anchor['hex']);site=anchor['site']
            require(image[site:site+len(raw)]==raw,'Selected boundary anchor changed')
        # Independently traverse direct 8086 control flow in this bounded subset.
        by={r['load_offset']:bytes.fromhex(r['bytes']) for r in rows}
        table_targets=[]
        if f.get('verification_kind')=='embedded_jump_table16':
            table_targets=[f['frame_load_address']+int.from_bytes(bytes.fromhex(islands[0]['hex'])[i:i+2],'little')
                           for i in range(0,8,2)]
            require(len(set(table_targets))==4 and all(t in by for t in table_targets)
                    and by[141037]==bytes.fromhex('2effa7d23c') and
                    f['frame_load_address']+int.from_bytes(by[141037][-2:],'little')==141042,
                    'Reviewed sine indirect branches differ')
        if f.get('verification_kind')=='embedded_jump_table16_mat_rot_zxy_v1':
            require(f['name']=='mat_rot_zxy' and (start,end)==(90618,91002) and
                    f['frame_load_address']==85344 and len(islands)==1 and
                    islands[0]['kind']=='jump_table16' and
                    islands[0]['load_offset']==90978 and
                    islands[0]['indirect_jump_site']==90770 and
                    by.get(90770)==bytes.fromhex('2effa70216') and
                    by.get(90767)==bytes.fromhex('03c0') and
                    by.get(90769)==bytes.fromhex('93') and
                    by.get(90759)==bytes.fromhex('3d0700') and
                    by.get(90762)==bytes.fromhex('7603') and
                    f['frame_load_address']+int.from_bytes(by[90770][-2:],'little')==90978 and
                    len(bytes.fromhex(islands[0]['hex']))==16,
                    'Reviewed rotation switch bounds or dispatch changed')
            proof, = f['table_proofs']
            table_raw=bytes.fromhex(islands[0]['hex'])
            table_targets=[f['frame_load_address']+int.from_bytes(table_raw[i:i+2],'little')
                           for i in range(0,16,2)]
            require(proof['jump_site']==90770 and proof['jump_bytes']==by[90770].hex() and
                    proof['island_load_offset']==90978 and
                    proof['island_hex']==table_raw.hex() and
                    proof['frame_load_address']==85344 and
                    proof['valid_indices']==list(range(8)) and
                    proof['targets']==table_targets and
                    proof['entry_offsets']==[t-85344 for t in table_targets] and
                    proof['index_guard']['range_check_site']==90757 and
                    proof['index_guard']['bytes']==image[90757:90767].hex() and
                    all(by.get(row['load_offset'])==bytes.fromhex(row['bytes'])
                        for row in proof['flag_and_guard_instructions']) and
                    {row['load_offset'] for row in proof['flag_and_guard_instructions']}==
                    {90626,90628,90635,90652,90659,90678,90685,90757,90759,90762,90770} and
                    all(target in by for target in table_targets),
                    'Reviewed rotation switch targets or bounded index proof changed')
        table_targets_by_site={}
        if f.get('verification_kind')=='embedded_data_dispatch_v1':
            # These are the reviewed bounds, not caller-supplied claims of
            # arbitrary table reachability. Every entry must be represented.
            expected={
                'polarAngle': [(125522,'33ff'),(125534,'83cf08'),
                               (125543,'83cf04'),(125556,'83cf02')],
                'file_load_resource': [(104915,'3d0800'),(104918,'7603'),
                                       (104923,'03c0'),(104925,'93')],
                'sub_35C4E': []}
            require(f['name'] in expected and
                    len(f.get('dispatch_tables',[])) ==
                    {'polarAngle':2,'file_load_resource':1,'sub_35C4E':0}[f['name']] and
                    (f['name']!='sub_35C4E' or
                     (len(islands)==1 and islands[0]['kind']=='lookup_table8' and
                      island_by[islands[0]['load_offset']][0]==bytes(range(256)))),
                    'Unreviewed embedded dispatch/lookup structure')
        for table in f.get('dispatch_tables',[]):
            site=table['site']; island=island_by.get(table['island_load_offset'])
            require(island is not None and island[1]['kind']=='jump_table16' and
                    site in by and by[site]==bytes.fromhex(table['instruction_hex']) and
                    table['entry_width']==2 and
                    table['frame_load_address']==f['frame_load_address'] and
                    table['frame_load_address']+table['base_displacement']==table['island_load_offset'] and
                    int.from_bytes(by[site][-2:],'little')==table['base_displacement'],
                    'Reviewed indirect-jump table does not match instruction/island')
            require([(p['site'],p['hex']) for p in table['index_proof']]==expected[f['name']] and
                    table['byte_index_values']==list(range(0,len(island[0]),2)) and
                    all(image[p['site']:p['site']+len(bytes.fromhex(p['hex']))]
                   ==bytes.fromhex(p['hex']) for p in table['index_proof']),
                    'Reviewed table index proof changed')
            raw_table=island[0];targets=[]
            for byte_index in table['byte_index_values']:
                require(byte_index%2==0 and byte_index+2<=len(raw_table),
                        'Reviewed table index exceeds complete embedded table')
                targets.append(table['frame_load_address']+
                               int.from_bytes(raw_table[byte_index:byte_index+2],'little'))
            require(targets==table['targets'] and all(target in by for target in targets),
                    'Reviewed computed-jump targets differ or leave mapped code')
            table_targets_by_site[site]=targets
        require(set(table_targets_by_site)=={s for s,raw in by.items()
                if raw[0] in (0x2e,0x26,0x36,0x3e) and len(raw)>2 and
                raw[1]==0xff and ((raw[2]>>3)&7) in (4,5)} if
                f.get('verification_kind')=='embedded_data_dispatch_v1' else True,
                'Reviewed indirect-jump coverage incomplete')
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
            if at in table_targets_by_site:
                pending.extend(table_targets_by_site[at]);continue
            if f.get('verification_kind')=='embedded_jump_table16' and at==141037:
                pending.extend(table_targets)
                continue
            if f.get('verification_kind')=='embedded_jump_table16_mat_rot_zxy_v1' and at==90770:
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
