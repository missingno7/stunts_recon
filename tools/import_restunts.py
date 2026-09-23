# Derived boundary pass: all physical source lines participate, including labels and
# emissions between procedures. Never infer contiguity from procedure order alone.
exec((Path(__file__).parent/'restunts_base.py').read_text()) if False else None
import runpy
ctx=runpy.run_path(str(__import__('pathlib').Path(__file__).parent/'restunts_base.py'))
globals().update({k:ctx[k] for k in ('ROOT','base','funcs','filemeta','blob','md','relocs','source_ins','decode_match','agree','hashlib','re','json','collections')})
new=0
for meta in filemeta:
    path=ROOT/meta['source'];lines=path.read_text(encoding='latin1').splitlines(); items=[];labels=[];boundaries={}
    for n,line in enumerate(lines,1):
        boundaries[n]=len(items)
        label=re.match(r'^\s*(?:loc|locret)_([0-9a-f]+):',line,re.I)
        if label:labels.append({'ida':int(label[1],16),'index':len(items),'line':n})
        item=source_ins(line,n)
        if item:items.append(item)
    def address(index):
        prev=[l for l in labels if l['index']<=index]
        if prev:
            anchor=prev[-1];subset=items[anchor['index']:index]
            dec=decode_match(anchor['ida']-65536,subset)
            if dec is not None:return (dec[-1].address+dec[-1].size if dec else anchor['ida']-65536),{'method':'forward_from_dense_label','label':anchor,'instructions':len(subset)}
        nxt=next((l for l in labels if l['index']>=index),None)
        if nxt:
            subset=items[index:nxt['index']];end=nxt['ida']-65536
            if len(subset)>25:return None
            possibilities=[]
            for start in range(max(0,end-15*len(subset)),end+1):
                if decode_match(start,subset,end)is not None:possibilities.append(start)
            if len(possibilities)==1:return possibilities[0],{'method':'unique_reverse_to_dense_label','label':nxt,'instructions':len(subset)}
        return None
    for f in funcs:
        if f['segment']!=meta['name']:continue
        a=address(boundaries[f['line_start']]);b=address(boundaries[f['line_end']])
        if not a or not b or a[0]>=b[0]:continue
        subset=items[boundaries[f['line_start']]:boundaries[f['line_end']]]
        dec=decode_match(a[0],subset,b[0])
        if dec is None:continue
        offsets=[i.address for i in dec]+[b[0]]
        if any(offsets[l['index']]!=l['ida']-65536 for l in f['labels']):continue
        if f['status']!='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':new+=1
        f.update(start=a[0],end=b[0],start_evidence=a[1],end_evidence=b[1],status='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',issues=[],size=b[0]-a[0],sha256=hashlib.sha256(blob[a[0]:b[0]]).hexdigest(),relocation_sites=[r for r in sorted(relocs)if a[0]<=r<b[0]],bytes_hex=blob[a[0]:b[0]].hex()if b[0]-a[0]<=80 else None)
byname={f['name'].lower():f for f in funcs if f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'}
frames=collections.defaultdict(list)
for f in funcs:
    if f['status']!='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':continue
    code=list(md.disasm(blob[f['start']:f['end']],f['start']))
    if len(code)!=len(f['items']):continue
    for src,ins in zip(f['items'],code):
        if ins.bytes[0]!=0x9a or ins.size!=5 or ins.address+3 not in relocs:continue
        target=byname.get(src['source'].split()[-1])
        if not target:continue
        off=int.from_bytes(ins.bytes[1:3],'little');seg=int.from_bytes(ins.bytes[3:5],'little')
        if seg*16+off==target['start']:frames[target['segment']].append({'caller':f['name'],'site':ins.address,'target':target['name'],'segment_paragraph':seg,'offset':off})
# Other regions may reference mapped functions, even if caller body contains an unrelated
# edit. Each call's labeled interval itself must decode from pristine bytes and agree.
for f in funcs:
    for n,label in enumerate(f['labels']):
        following=f['labels'][n+1]if n+1<len(f['labels'])else None
        subset=f['items'][label['index']:following['index']if following else len(f['items'])]
        dec=decode_match(label['ida']-65536,subset,following['ida']-65536 if following else None)
        if dec is None:continue
        for src,ins in zip(subset,dec):
            if ins.bytes[0]!=0x9a or ins.size!=5 or ins.address+3 not in relocs:continue
            target=byname.get(src['source'].split()[-1])
            if not target:continue
            off=int.from_bytes(ins.bytes[1:3],'little');seg=int.from_bytes(ins.bytes[3:5],'little')
            if seg*16+off==target['start']:
                row={'caller':f['name'],'site':ins.address,'target':target['name'],'segment_paragraph':seg,'offset':off}
                if row not in frames[target['segment']]:frames[target['segment']].append(row)
for meta in filemeta:
    evidence=frames[meta['name']];unique=sorted({r['segment_paragraph']for r in evidence});meta.update(frame_candidates=unique,far_call_evidence=evidence)
    if len(unique)==1:
        meta['segment_paragraph']=unique[0]
        for f in funcs:
            if f['segment']==meta['name']and'start'in f:
                f['segment_paragraph']=unique[0];f['segment_offset']=f['start']-unique[0]*16;f['stable_id']='F_%04X_%04X'%(unique[0],f['segment_offset'])
small=[]
for f in funcs:
    if f['status']!='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'or f['size']>80 or f['relocation_sites']:continue
    code=list(md.disasm(blob[f['start']:f['end']],f['start']))
    row={k:v for k,v in f.items()if k not in ('items','labels')};row['disassembly']=[{'load_offset':i.address,'bytes':i.bytes.hex(),'instruction':i.mnemonic+' '+i.op_str}for i in code]
    lowlevel=any(i.mnemonic in ('int','cli','sti','iret','in','out','rep movsb','rep movsw')for i in code)
    runtime=f['segment']=='seg010' or f['name'].startswith(('__','_')) or f['name']in('toupper','tolower')
    row['ordinary_c_hint']=not lowlevel and not runtime and bool(code and code[0].mnemonic=='push'and code[0].op_str=='bp'and code[-1].mnemonic in ('ret','retf'))
    row['external_or_control_flow']=[i.mnemonic+' '+i.op_str for i in code if i.group(1)or i.group(2)]
    small.append(row)
small.sort(key=lambda f:(not f['ordinary_c_hint'],f['size'],f['start']))
report=ctx['report'];report.update(functions=[{k:v for k,v in f.items()if k not in ('items','labels')}for f in funcs],segments=filemeta,small_candidates=small,summary=dict(collections.Counter(f['status']for f in funcs)))
report['algorithm']+='; supplemental whole-file source emission stream maps boundaries from adjacent dense labels, checking intervening emissions including explicit data and never assuming procedure adjacency implies byte adjacency'
(ROOT/'build/inventory-research/inventory.json').write_text(json.dumps(report,indent=2)+'\n')
# Attach source facts and verify the independent binary coordinate transform.
from common import read_json, write_json, require, identity
from mz import MZ
references=read_json(ROOT/'layout/references.json')
oracle=read_json(ROOT/'layout/oracle.lock.json')
refroot=ROOT/'build/references/restunts'
for relative, expected in references['restunts']['evidence_files'].items():
    require(identity((refroot/relative).read_bytes())==expected, 'Restunts evidence file changed: '+relative)
reference=(refroot/'src/drvcombiner/assets/game_cracked.exe').read_bytes()
rm=MZ.parse(reference)
reference_image=rm.load_image(reference)
require(len(reference_image)==len(blob), 'Restunts reference image size differs')
differences=[{'offset':i,'oracle':a,'reference':b} for i,(a,b) in enumerate(zip(blob,reference_image)) if a!=b]
require(differences==[{'offset':684,'oracle':0,'reference':1}], 'Unexpected Restunts core byte differences')
require(rm.relocations==oracle['unpacked_mz']['relocations'], 'Restunts core relocation table differs')
binary_anchors=[]
for start in range(0,len(blob),256):
    end=min(start+256,len(blob))
    if start<=684<end: continue
    require(blob[start:end]==reference_image[start:end], 'Binary anchor mismatch')
    binary_anchors.append({'load_start':start,'end':end,'restunts_ida_start':start+0x10000,
                           'restunts_file_start':start+rm.header_size,'sha256':hashlib.sha256(blob[start:end]).hexdigest()})
report['coordinate_proof']={'reference':'src/drvcombiner/assets/game_cracked.exe',
    'reference_identity':identity(reference),'differences':differences,'relocations_equal':True,
    'note':'Reference supplies comparison evidence only; all target bytes come from user assets.',
    'binary_anchors':binary_anchors}
csources=list((refroot/'src/restunts/c').glob('*.c'))
ctext={p:p.read_text(encoding='latin1') for p in csources}
for f in report['functions']:
    f['provenance']={'repository':'restunts','commit':references['restunts']['commit'],'path':f['source'].removeprefix('build/references/restunts/'),'line_start':f['line_start'],'line_end':f['line_end']}
    f['confidence']='verified_instruction_boundaries' if f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' else 'unresolved_boundary'
    if f['confidence']=='verified_instruction_boundaries':f['stable_id']='load_%05x'%f['start']
    else:
        f.pop('stable_id',None)
        f['unresolved_evidence_id']=f['segment']+':line'+str(f['line_start'])
    f['c_sources']=[p.relative_to(refroot).as_posix() for p,t in ctext.items() if re.search(r'\b'+re.escape(f['name'])+r'\s*\(',t)]
    lines=(ROOT/f['source']).read_text(encoding='latin1').splitlines()[f['line_start']-1:f['line_end']]
    f['calls']=[{'target':m[1],'confidence':'source_semantic_only'} for line in lines if (m:=re.search(r'\bcall\s+(?:far ptr |near ptr )?(\w+)',line,re.I))]
    f['origin']='UNCLASSIFIED; assembly representation does not establish historical ASM authorship'
globals_inventory=[]
for seg in report['segments']:
    if seg['class']!='STUNTSD':continue
    for number,line in enumerate((ROOT/seg['source']).read_text(encoding='latin1').splitlines(),1):
        match=re.match(r'^\s*(\w+)\s+(db|dw|dd|dq|dt|struc\w*)\b',line,re.I)
        if match:
            globals_inventory.append({'name':match[1],'directive':match[2],'segment':seg['name'],'path':seg['source'],'line':number,'confidence':'symbol_location_unresolved','commit':references['restunts']['commit']})
report['globals']=globals_inventory
report['reference_commit']=references['restunts']['commit']
write_json(ROOT/'recovery/restunts-inventory.json',report)
print('Imported',len(report['functions']),'procedures;',report['summary'],';',len(binary_anchors),'binary anchors')
