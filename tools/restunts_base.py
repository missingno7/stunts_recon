from pathlib import Path
import sys,re,json,hashlib,collections
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'build/python'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_16
md=Cs(CS_ARCH_X86,CS_MODE_16); md.detail=True
from oracle import verify
verify()
(ROOT/'build/inventory-research').mkdir(parents=True,exist_ok=True)
blob=(ROOT/'build/oracle/load-image.bin').read_bytes()
oracle=json.loads((ROOT/'build/oracle/report.json').read_text())
relocs={r['load_offset'] for r in oracle['unpacked_mz']['relocations']}
base=ROOT/'build/references/restunts/src/restunts/asmorig'
alias={'retn':'ret','sal':'shl','jz':'je','jnz':'jne','jnb':'jae','jnae':'jb','jna':'jbe','jnbe':'ja','jng':'jle','jnle':'jg','jnge':'jl','jnl':'jge','loopnz':'loopne','loopz':'loope','repz':'repe','repnz':'repne'}
regs=set('ax bx cx dx si di bp sp al ah bl bh cl ch dl dh cs ds es ss'.split())
mnems=set('aaa aad aam aas adc add and arpl bound call cbw clc cld cli cmc cmp cmpsb cmpsw cwd daa das dec div enter hlt idiv imul in inc insb insw int into iret ja jae jb jbe jcxz je jg jge jl jle jmp jne jno jnp jns jo jp jpe jpo js lahf lds lea leave les lodsb lodsw loop loope loopne mov movsb movsw mul neg nop not or out outsb outsw pop popa popf push pusha pushf rcl rcr ret retf rol ror sahf sar sbb scasb scasw shl shr stc std sti stosb stosw sub test wait xchg xlat xlatb xor fadd fsub fmul fdiv fld fst fstp fnstsw fstsw'.split())|set(alias)|{'rep','repe','repne','lock'}
def norm(m):
    return alias.get(m,m).replace('lcall','call').replace('ljmp','jmp')
def source_ins(s,line):
    s=s.split(';',1)[0].strip().lower()
    if not s or '=' in s or s.endswith(':'): return None
    tok=s.split()[0]
    if tok in ('db','dw','dd','dq','dt','align','even','org') or re.match(r'^\w+\s+d[bwdqt]\b',s): return {'line':line,'source':s,'unsupported':True}
    if tok not in mnems: return None
    return {'line':line,'source':s,'mnemonic':norm(tok),'regs':re.findall(r'\b(?:ax|bx|cx|dx|si|di|bp|sp|al|ah|bl|bh|cl|ch|dl|dh)\b',s)}
def agree(src, ins):
    if src.get('unsupported'):return False
    m=norm(ins.mnemonic)
    if src['mnemonic'] in ('rep','repe','repne','lock'):
        want=' '.join(src['source'].split()[:2]); got=' '.join((ins.mnemonic+' '+ins.op_str).split()[:2])
        return want==got
    if src['mnemonic']!=m:return False
    # Explicit GPRs must match (implicit registers are not listed by Capstone).
    actual=re.findall(r'\b(?:ax|bx|cx|dx|si|di|bp|sp|al|ah|bl|bh|cl|ch|dl|dh)\b',ins.op_str)
    if sorted(src['regs'])!=sorted(actual):return False
    # Immediate numeric operand, if source states one, is an extra independent check.
    tail=src['source'].split(',')[-1].strip()
    if re.fullmatch(r'-?(?:[0-9][0-9a-f]*h|[0-9]+)',tail):
        value=int(tail[:-1],16) if tail.endswith('h') else int(tail)
        imm=[o.imm for o in ins.operands if o.type==2]
        if not imm or (imm[-1]&65535)!=(value&65535): return False
    return True
funcs=[]; anchors=[]; filemeta=[]
for path in sorted(list(base.glob('seg[0-9]*.asm'))+[base/'dseg.asm']):
    lines=path.read_text(encoding='latin1').splitlines(); current=None
    segmatch=next((re.match(r"^(\w+) segment .*?'([^']+)'",s.strip(),re.I) for s in lines if ' segment ' in s.lower()),None)
    meta={'name':path.stem,'source':path.relative_to(ROOT).as_posix(),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'class':segmatch.group(2) if segmatch else None};filemeta.append(meta)
    for n,line in enumerate(lines,1):
        proc=re.match(r'^\s*(\S+)\s+proc\s+(\w+)',line,re.I)
        if proc:
            current={'name':proc[1],'distance':proc[2],'segment':path.stem,'source':meta['source'],'line_start':n,'items':[],'labels':[]};continue
        if not current:continue
        if re.match(r'^\s*\S+\s+endp\b',line,re.I):
            current['line_end']=n;funcs.append(current);current=None;continue
        label=re.match(r'^\s*(?:loc|locret)_([0-9a-f]+):',line,re.I)
        if label:current['labels'].append({'ida':int(label[1],16),'index':len(current['items']),'line':n})
        item=source_ins(line,n)
        if item:current['items'].append(item)

def decode_match(start,items,end=None):
    if start<0 or start>=len(blob):return None
    code=list(md.disasm(blob[start:min(len(blob),start+15*len(items))],start,count=len(items)))
    if len(code)!=len(items) or any(not agree(s,i) for s,i in zip(items,code)):return None
    finish=code[-1].address+code[-1].size if code else start
    if end is not None and finish!=end:return None
    return code
for f in funcs:
    labels=f['labels']; issues=[]; verified=[]
    if not labels:
        f['status']='UNMAPPED_NO_ADDRESS_ANCHOR';continue
    first=labels[0]; at=first['ida']-65536; pre=f['items'][:first['index']]
    starts=[]
    if not pre:starts=[at]
    else:
        for start in range(max(0,at-15*len(pre)),at):
            if decode_match(start,pre,at) is not None:starts.append(start)
    f['start_candidates']=starts
    if len(starts)==1:f['start']=starts[0]
    else:issues.append('ambiguous_or_mismatching_prefix')
    for i,label in enumerate(labels):
        next_label=labels[i+1] if i+1<len(labels) else None
        ins=f['items'][label['index']:next_label['index'] if next_label else len(f['items'])]
        start=label['ida']-65536; end=next_label['ida']-65536 if next_label else None
        decoded=decode_match(start,ins,end)
        if decoded is None:issues.append('label_interval_mismatch:'+hex(label['ida']))
        else:
            finish=decoded[-1].address+decoded[-1].size if decoded else start
            if not next_label:f['end']=finish
            record={'ida':label['ida'],'load_offset':start,'end':finish,'line':label['line'],'instruction_count':len(ins),'sha256':hashlib.sha256(blob[start:finish]).hexdigest()}
            anchors.append({'function':f['name'],'segment':f['segment'],**record});verified.append(record)
    f['issues']=issues;f['verified_labels']=len(verified);f['label_count']=len(labels)
    if 'start' in f and 'end' in f and f['start']<f['end'] and not issues:
        f['status']='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
        f['size']=f['end']-f['start'];f['sha256']=hashlib.sha256(blob[f['start']:f['end']]).hexdigest();f['relocation_sites']=[r for r in sorted(relocs) if f['start']<=r<f['end']]
        f['bytes_hex']=blob[f['start']:f['end']].hex() if f['size']<=80 else None
    else:f['status']='PARTIAL_UNMAPPED'
# Far calls encode the actual original code segment. Names plus independently mapped target
# starts must agree before using these as segment evidence.
byname={f['name'].lower():f for f in funcs if f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'}
frames=collections.defaultdict(list)
for f in funcs:
    if f['status']!='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':continue
    code=list(md.disasm(blob[f['start']:f['end']],f['start']))
    if len(code)!=len(f['items']):continue
    for src,ins in zip(f['items'],code):
        if ins.bytes[0]!=0x9a or ins.size!=5 or ins.address+3 not in relocs:continue
        symbol=src['source'].split()[-1]
        target=byname.get(symbol)
        if not target:continue
        off=int.from_bytes(ins.bytes[1:3],'little');seg=int.from_bytes(ins.bytes[3:5],'little')
        if seg*16+off==target['start']:
            frames[target['segment']].append({'caller':f['name'],'site':ins.address,'target':target['name'],'segment_paragraph':seg,'offset':off})
for meta in filemeta:
    mapped=[f for f in funcs if f['segment']==meta['name'] and 'start'in f and 'end'in f]
    if mapped:meta.update(observed_min=min(f['start'] for f in mapped),observed_max=max(f['end'] for f in mapped))
    evidence=frames[meta['name']];unique=sorted({r['segment_paragraph'] for r in evidence})
    meta['frame_candidates']=unique;meta['far_call_evidence']=evidence
    if len(unique)==1:
        meta['segment_paragraph']=unique[0]
        for f in funcs:
            if f['segment']==meta['name'] and 'start'in f:
                f['segment_paragraph']=unique[0];f['segment_offset']=f['start']-unique[0]*16
                f['stable_id']='F_%04X_%04X'%(unique[0],f['segment_offset'])
small=[]
for f in funcs:
    if f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' and f['size']<=80 and not f['relocation_sites']:
        code=list(md.disasm(blob[f['start']:f['end']],f['start']))
        row={k:v for k,v in f.items() if k not in ('items','labels')};row['disassembly']=[{'load_offset':i.address,'bytes':i.bytes.hex(),'instruction':i.mnemonic+' '+i.op_str}for i in code]
        row['ordinary_c_hint']=bool(code and code[0].mnemonic=='push' and code[0].op_str=='bp' and code[-1].mnemonic in ('ret','retf'))
        row['external_or_control_flow']=[i.mnemonic+' '+i.op_str for i in code if i.group(1) or i.group(2)]
        small.append(row)
small.sort(key=lambda f:(not f['ordinary_c_hint'],f['size'],f['start']))
report={'algorithm':'loc/locret IDA labels with proposed load=IDA-0x10000; uniquely reverse-decoded prefix and forward-decoded labeled intervals; mnemonic, explicit GPR and literal-immediate verification; segment frames only from original relocated far-call bytes whose named target equals independently mapped start','limitation':'This verifies instruction evidence, not full assembly-byte equality. Symbolic memory displacements not checked; unchanged instruction shape can conceal differing data references. Unsupported emission or any mismatched label interval leaves function unpromoted. Segment observed extents are not complete segment allocation.','load_sha256':hashlib.sha256(blob).hexdigest(),'summary':dict(collections.Counter(f['status'] for f in funcs)),'functions':[{k:v for k,v in f.items() if k not in ('items','labels')} for f in funcs],'segments':filemeta,'anchors':anchors,'small_candidates':small}
out=ROOT/'build/inventory-research/inventory.json';out.write_text(json.dumps(report,indent=2)+'\n')
