"""Deterministic, non-authoritative instruction alignment and local mismatch evidence.

No result of this module is consumed by the binder or acceptance comparisons.
Ranges are half-open, relative to the start of each complete contribution.
"""
import collections
import copy
import json
import sys
from pathlib import Path
from common import ROOT, identity, write_json, sha, json_bytes

SCHEMA = 3
MAX_CELLS = 262144
MAX_RUNS = 2048
BOILERPLATE = {'pop', 'ret', 'retf', 'nop', 'leave'}
SPECIAL_REGISTERS = {'cs','ds','es','ss','fs','gs','sp','esp','bp','ebp'}


def _decoder():
    location = str(ROOT/'build/python')
    if location not in sys.path: sys.path.insert(0, location)
    import capstone
    if capstone.__version__ != '5.0.3':
        raise ValueError('Diagnostic decoder requires Capstone 5.0.3')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    decoder.detail = True
    return capstone, decoder


def _decode(data, start=0):
    cs, decoder = _decoder()
    rows = []
    for ins in decoder.disasm(data, start):
        mnemonic = {b'\x98':'cbw', b'\x99':'cwd'}.get(bytes(ins.bytes), ins.mnemonic)
        operands = []
        for operand in ins.operands:
            item = {'width':operand.size}
            if operand.type == cs.x86.X86_OP_REG:
                item.update(kind='reg', reg=ins.reg_name(operand.reg))
            elif operand.type == cs.x86.X86_OP_IMM:
                item.update(kind='imm', value=operand.imm)
            elif operand.type == cs.x86.X86_OP_MEM:
                mem = operand.mem
                item.update(kind='mem', base=ins.reg_name(mem.base), index=ins.reg_name(mem.index),
                            segment=ins.reg_name(mem.segment), scale=mem.scale, disp=mem.disp)
            else: item.update(kind='unknown', type=operand.type)
            operands.append(item)
        groups = set(ins.groups)
        control = ('return' if cs.CS_GRP_RET in groups else 'jump' if cs.CS_GRP_JUMP in groups
                   else 'call' if cs.CS_GRP_CALL in groups else None)
        fields = []
        for offset, size in [(ins.imm_offset, ins.imm_size), (ins.disp_offset, ins.disp_size)]:
            if offset > 0 and size > 0 and offset+size <= ins.size:
                fields.append([offset, offset+size])
        # Capstone's far-immediate size is unreliable in this version. These
        # unprefixed 16:16 forms have two independently relocatable word halves.
        if ins.size == 5 and ins.bytes[0] in (0x9a, 0xea): fields = [[1, 3], [3, 5]]
        rows.append({'load_offset':ins.address, 'bytes':ins.bytes.hex(),
                     'instruction':mnemonic+' '+ins.op_str, 'mnemonic':mnemonic,
                     'operands':operands, 'control':control, 'operand_fields':fields,
                     'fixup_fields':[], 'implicit_reads':[ins.reg_name(r) for r in ins.regs_read],
                     'implicit_writes':[ins.reg_name(r) for r in ins.regs_write]})
        if mnemonic=='xlatb':
            rows[-1]['implicit_effects_note']='Pinned decoder omits XLAT implicit effects; register-family scope cannot cross this instruction.'
    return rows


def decode(data, start=0):
    """Compatibility surface for reviewed instruction/CFG evidence."""
    return [{k:r[k] for k in ['load_offset','bytes','instruction']} for r in _decode(data,start)]


def _fixups(rows, fixups, segment, bound):
    notes = {'mode':'bound payload' if bound else 'unbound object', 'fields':[], 'ignored':[],
             'rule':'Normalization is alignment-only; affected instructions never count as byte-exact anchors.'}
    if bound:
        notes['rule'] = 'No normalization: every bound byte is compared.'
        return notes
    occupied = set()
    for fix in fixups:
        if fix.get('segment', segment) != segment: continue
        at, width = fix.get('offset'), fix.get('width')
        row = next((r for r in rows if isinstance(at,int) and
                    r['load_offset'] <= at < r['load_offset']+len(bytes.fromhex(r['bytes']))), None)
        valid = row is not None and isinstance(width,int) and width in (1,2,4)
        if valid:
            relative = at-row['load_offset']; span = set(range(relative,relative+width))
            operand_bytes = {b for a,z in row['operand_fields'] for b in range(a,z)}
            valid = span <= operand_bytes and not occupied.intersection(range(at,at+width))
        if not valid:
            notes['ignored'].append({'offset':at,'width':width,'reason':'Not a disjoint decoded operand field; left unnormalized'})
            continue
        occupied.update(range(at,at+width))
        field = {'offset':relative,'width':width,'target':fix.get('target'),'kind':fix.get('loc')}
        row['fixup_fields'].append(field)
        notes['fields'].append({**field,'offset':at})
    return notes


def _decoded_key(row):
    # No instruction address in this key. Near branch destinations are relative
    # to their instruction end, not absolute Capstone display addresses.
    operands = copy.deepcopy(row['operands'])
    if row['control'] in ('jump','call') and len(operands)==1 and operands[0]['kind']=='imm':
        operands[0]['value'] -= row['load_offset']+len(bytes.fromhex(row['bytes']))
        operands[0].pop('width',None)  # short/near encodings share a decoded operation
    return row['mnemonic'], json.dumps(operands,sort_keys=True)


def _pair_level(left, right):
    if right['fixup_fields']:
        a, b = bytes.fromhex(left['bytes']), bytes.fromhex(right['bytes'])
        fields = {n for f in right['fixup_fields'] for n in range(f['offset'],f['offset']+f['width'])}
        if left['mnemonic']==right['mnemonic'] and len(a)==len(b) and all(a[i]==b[i] for i in range(len(a)) if i not in fields):
            return 'FIXUP_NORMALIZED'
        return 'STRUCTURAL' if left['mnemonic']==right['mnemonic'] else 'MISMATCH'
    if left['bytes']==right['bytes']: return 'BYTE_EXACT'
    if _decoded_key(left)==_decoded_key(right): return 'DECODED_EXACT'
    if left['mnemonic']==right['mnemonic'] and [o['kind'] for o in left['operands']]==[o['kind'] for o in right['operands']]:
        return 'STRUCTURAL'
    return 'MISMATCH'


def _tokens(rows):
    # A fixup-bearing instruction cannot seed or extend a byte-exact anchor,
    # even when the unbound zeros happen to equal the target field.
    return [r['bytes'] if not r['fixup_fields'] else ('unbound',i) for i,r in enumerate(rows)]


def _occurrences(tokens, key):
    size = len(key)
    return sum(tuple(tokens[i:i+size])==tuple(key) for i in range(len(tokens)-size+1))


def _nontrivial(rows):
    return any(r['mnemonic'] not in BOILERPLATE and r['instruction'] != 'mov sp, bp' for r in rows)


def _seeds(left, right, lo, hi, ro, rh, notes):
    """Globally unique four-instruction / >=8-byte seeds, monotonic maximum cover."""
    a,b = _tokens(left),_tokens(right)
    ai,bi = collections.defaultdict(list),collections.defaultdict(list)
    for tokens,index in [(a,ai),(b,bi)]:
        for i in range(len(tokens)-3):
            key = tuple(tokens[i:i+4])
            if all(isinstance(t,str) for t in key): index[key].append(i)
    seeds = sorted((ii[0],bi[key][0]) for key,ii in ai.items()
                   if len(ii)==1 and len(bi.get(key,[]))==1 and lo<=ii[0]<=hi-4 and ro<=bi[key][0]<=rh-4
                   and sum(len(bytes.fromhex(t)) for t in key)>=8 and _nontrivial(left[ii[0]:ii[0]+4]))
    runs = []
    for i,j in seeds:
        if runs and i <= runs[-1][0]+runs[-1][2] and i-j==runs[-1][0]-runs[-1][1]:
            x,y,n = runs[-1]; runs[-1]=(x,y,max(n,i+4-x))
        else: runs.append((i,j,4))
    if len(runs)>MAX_RUNS:
        notes.append('Unique-run budget exceeded; middle alignment left conservative.')
        return []
    scores,previous = [],[]
    for k,(i,j,n) in enumerate(runs):
        score,prev=n,-1
        for p,(x,y,size) in enumerate(runs[:k]):
            if x+size<=i and y+size<=j and scores[p]+n>score:
                score,prev=scores[p]+n,p
        scores.append(score);previous.append(prev)
    if not runs:return []
    k=max(range(len(runs)),key=lambda x:(scores[x],-x));chain=[]
    while k>=0:chain.append(runs[k]);k=previous[k]
    return list(reversed(chain))


def _gap(left,right,lo,hi,ro,rh,notes):
    """Bounded edit alignment between strong anchors; deterministic tie order."""
    n,m=hi-lo,rh-ro
    if not n:return [(None,j,'EXTRA') for j in range(ro,rh)]
    if not m:return [(i,None,'MISSING') for i in range(lo,hi)]
    if (n+1)*(m+1)>MAX_CELLS:
        notes.append(f'Alignment cell budget exceeded for target instructions {lo}:{hi}, candidate {ro}:{rh}.')
        return [(i,None,'UNALIGNED') for i in range(lo,hi)]+[(None,j,'UNALIGNED') for j in range(ro,rh)]
    costs=[[0]*(m+1) for _ in range(n+1)];steps=[bytearray(m+1) for _ in range(n+1)]
    for i in range(1,n+1):costs[i][0]=3*i;steps[i][0]=1
    for j in range(1,m+1):costs[0][j]=3*j;steps[0][j]=2
    penalty={'BYTE_EXACT':0,'DECODED_EXACT':1,'FIXUP_NORMALIZED':1,'STRUCTURAL':2,'MISMATCH':5}
    for i in range(1,n+1):
        for j in range(1,m+1):
            kind=_pair_level(left[lo+i-1],right[ro+j-1])
            choices=(costs[i-1][j-1]+penalty[kind],costs[i-1][j]+3,costs[i][j-1]+3)
            step=min(range(3),key=lambda k:(choices[k],k))
            costs[i][j]=choices[step];steps[i][j]=step
    pairs=[];i,j=n,m
    while i or j:
        step=steps[i][j]
        if step==0:
            pairs.append((lo+i-1,ro+j-1,_pair_level(left[lo+i-1],right[ro+j-1])));i-=1;j-=1
        elif step==1:pairs.append((lo+i-1,None,'MISSING'));i-=1
        else:pairs.append((None,ro+j-1,'EXTRA'));j-=1
    return list(reversed(pairs))


def _align(left,right,notes):
    n,m=len(left),len(right);prefix=0
    while prefix<min(n,m) and _pair_level(left[prefix],right[prefix])=='BYTE_EXACT':prefix+=1
    suffix=0
    while suffix<min(n,m)-prefix and _pair_level(left[n-suffix-1],right[m-suffix-1])=='BYTE_EXACT':suffix+=1
    # A duplicated terminal epilogue is not an independent realignment anchor.
    if suffix and (_occurrences(_tokens(left),_tokens(left[n-suffix:]))!=1 or
                   _occurrences(_tokens(right),_tokens(right[m-suffix:]))!=1):
        notes.append('Repeated terminal sequence was not used as a suffix anchor.')
        suffix=0
    seeds=_seeds(left,right,prefix,n-suffix,prefix,m-suffix,notes)
    anchors=([(0,0,prefix)] if prefix else [])+seeds+([(n-suffix,m-suffix,suffix)] if suffix else [])
    pairs=[];x=y=0
    for i,j,length in anchors:
        pairs.extend(_gap(left,right,x,i,y,j,notes))
        pairs.extend((i+k,j+k,'BYTE_EXACT') for k in range(length));x=i+length;y=j+length
    pairs.extend(_gap(left,right,x,n,y,m,notes))
    strong={(i+k,j+k) for i,j,length in anchors for k in range(length)}
    return pairs,strong


def _range(rows,start,end,size):
    a=rows[start]['load_offset'] if start<len(rows) else size
    z=rows[end]['load_offset'] if end<len(rows) else size
    return {'instructions':[start,end],'bytes':[a,z]}


def _classify(pairs,left,right,target_to_candidate):
    result=[];slots=[];regs=[];immediates=[];branches=[];widths=[];unclassified=[];normalized=0
    def add(name,confidence,evidence):result.append({'class':name,'confidence':confidence,'evidence':evidence[:4],
                                                  'omitted_evidence':max(0,len(evidence)-4)})
    for i,j,level in pairs:
        if i is None or j is None:continue
        a,b=left[i],right[j]
        if level=='BYTE_EXACT':continue
        evidence=f"+0x{a['load_offset']:X} {a['instruction']} -> +0x{b['load_offset']:X} {b['instruction']}"
        if level=='FIXUP_NORMALIZED':normalized+=1;continue
        if a['control'] in ('jump','call') and b['control']==a['control'] and a['mnemonic']==b['mnemonic']:
            branches.append((a,b,evidence));continue
        if a['mnemonic'] in ('cbw','cwd','movsx','movzx') or b['mnemonic'] in ('cbw','cwd','movsx','movzx'):
            widths.append(evidence);continue
        if a['mnemonic']!=b['mnemonic'] or len(a['operands'])!=len(b['operands']):
            unclassified.append(evidence);continue
        changes=[]
        for old,new in zip(a['operands'],b['operands']):
            if old==new:continue
            keys={k for k in set(old)|set(new) if old.get(k)!=new.get(k)}
            if keys=={'disp'} and old.get('base')==new.get('base')=='bp':changes.append(('slot',(old['disp'],new['disp'])))
            elif keys<={'reg','base','index'} and keys and not any(old.get(k) in SPECIAL_REGISTERS or new.get(k) in SPECIAL_REGISTERS for k in keys):
                changes.extend(('reg',(old.get(k),new.get(k))) for k in sorted(keys))
            elif keys & {'reg','base','index','segment'} and any(old.get(k) in SPECIAL_REGISTERS or new.get(k) in SPECIAL_REGISTERS for k in keys):
                add('CALL_FRAME_OR_SEGMENT_REGISTER','high',[evidence]);changes.append(('other',None))
            elif keys=={'value'} and old['kind']=='imm':changes.append(('imm',(old['value'],new['value'])))
            elif 'width' in keys:changes.append(('width',None))
            else:changes.append(('other',None))
        kinds={c[0] for c in changes}
        if kinds=={'slot'}:slots.extend((x[1],evidence) for x in changes)
        elif kinds=={'reg'}:regs.extend((x[1],evidence) for x in changes)
        elif kinds=={'imm'}:immediates.append(evidence)
        elif 'width' in kinds:widths.append(evidence)
        else:unclassified.append(evidence)
    for items,name in [(slots,'STACK_SLOT_ALLOCATION'),(regs,'REGISTER_ALLOCATION')]:
        if items:
            forward=collections.defaultdict(set);reverse=collections.defaultdict(set)
            for (a,b),_ in items:forward[a].add(b);reverse[b].add(a)
            consistent=all(len(v)==1 for v in list(forward.values())+list(reverse.values()))
            if consistent:add(name,'high' if len(items)>=2 else 'medium',[e for _,e in items]+(['Observed BP displacements only; local/parameter/object identity not inferred'] if name=='STACK_SLOT_ALLOCATION' else []))
            else:unclassified.extend(e+' (inconsistent substitution)' for _,e in items)
    if immediates:add('IMMEDIATE_VALUE','high',immediates)
    if widths:add('WIDTH_OR_EXTENSION','high',widths)
    for a,b,evidence in branches:
        same_target=False
        if len(a['operands'])==len(b['operands'])==1 and a['operands'][0]['kind']==b['operands'][0]['kind']=='imm':
            same_target=target_to_candidate.get(a['operands'][0]['value'])==b['operands'][0]['value']
        name='BRANCH_ENCODING' if same_target and len(a['bytes'])!=len(b['bytes']) else 'BRANCH_TARGET_OR_LAYOUT'
        add(name,'high' if same_target else 'medium',[evidence, 'Destination follows aligned instruction' if same_target else 'Destination correspondence not proven by alignment'])
    target_rows=[left[i] for i,j,_ in pairs if i is not None];candidate_rows=[right[j] for i,j,_ in pairs if j is not None]
    tc=[r['mnemonic'] for r in target_rows if r['control']];cc=[r['mnemonic'] for r in candidate_rows if r['control']]
    if tc!=cc:add('CONTROL_FLOW_SHAPE','high',[f'Control operations ({len(tc)} -> {len(cc)}), first eight: {tc[:8]} -> {cc[:8]}; no semantic equivalence inferred'])
    if any(r['control']=='return' for r in target_rows+candidate_rows) and [r['bytes'] for r in target_rows]!=[r['bytes'] for r in candidate_rows]:
        add('EPILOGUE_OR_RETURN_LOWERING','medium',['Return-containing regions have different instruction/control sequences'])
    extra=[right[j] for i,j,_ in pairs if i is None];missing=[left[i] for i,j,_ in pairs if j is None]
    if extra:add('EXTRA_INSTRUCTIONS','medium',[f'{len(extra)} candidate instructions have no aligned target instruction'])
    if missing:add('MISSING_INSTRUCTIONS','medium',[f'{len(missing)} target instructions have no aligned candidate instruction'])
    for rows,side in [(extra,'candidate'),(missing,'target')]:
        stores=set();reloads=set()
        for row in rows:
            ops=row['operands']
            if row['mnemonic']=='mov' and len(ops)==2:
                for index,collector in [(0,stores),(1,reloads)]:
                    operand=ops[index]
                    if operand['kind']=='mem' and operand['base']=='bp':collector.add((operand['disp'],operand['width']))
        if stores & reloads:add('TEMPORARY_OR_SPILL','medium',[f'{side} unmatched BP store/reload fields ({len(stores & reloads)}), first four: {sorted(stores & reloads)[:4]}; purpose unproven'])
    if normalized:add('UNRESOLVED_FIXUP_OPERANDS','high',[f'{normalized} operand-normalized instructions; symbols/values still require binding'])
    if unclassified or not result:add('LOCAL_CODEGEN_UNCLASSIFIED','low',unclassified or ['No sufficiently discriminating structural evidence'])
    grouped={}
    for item in result:
        key=(item['class'],item['confidence'])
        if key not in grouped:grouped[key]=item
        else:
            old=grouped[key];evidence=old['evidence']+item['evidence']
            old['omitted_evidence']+=item['omitted_evidence']+max(0,len(evidence)-4)
            old['evidence']=evidence[:4]
    return list(grouped.values())


def _operand_pattern(a,b):
    """Only wholly accounted-for operand changes qualify; mixed differences remain residual."""
    if a['fixup_fields'] or b['fixup_fields'] or a['mnemonic']!=b['mnemonic'] or len(a['operands'])!=len(b['operands']):return None
    if any(a.get(k)!=b.get(k) for k in ('implicit_reads','implicit_writes')):return None
    changes=[]
    for index,(old,new) in enumerate(zip(a['operands'],b['operands'])):
        keys={k for k in set(old)|set(new) if old.get(k)!=new.get(k)}
        if not keys:continue
        if keys=={'disp'} and old.get('kind')=='mem' and old.get('base')==new.get('base')=='bp' and not old.get('index'):
            changes.append(('BP_DISPLACEMENT',index,'disp',old['disp'],new['disp'],old['width']))
        elif keys<= {'reg','base','index'} and not any(old.get(k) in SPECIAL_REGISTERS or new.get(k) in SPECIAL_REGISTERS or not old.get(k) or not new.get(k) for k in keys):
            changes.extend(('REGISTER_ROLE',index,k,old[k],new[k],old['width']) for k in sorted(keys))
        else:return None
    if not changes or len({c[0] for c in changes})!=1:return None
    return changes


def _patterns(left,right,pairs,protected,islands,complete):
    """Cross-island observations. No dataflow, source objects, or repair is inferred."""
    island_for={i:island['id'] for island in islands for i in range(*island['target']['instructions'])}
    candidate_island={j:island['id'] for island in islands for j in range(*island['candidate']['instructions'])}
    mapping={left[i]['load_offset']:(right[j]['load_offset'],k in protected)
             for k,(i,j,_) in enumerate(pairs) if i is not None and j is not None}
    observations=[];eligible={'BP_DISPLACEMENT':[],'REGISTER_ROLE':[]}
    for k,(i,j,level) in enumerate(pairs):
        a=left[i] if i is not None else None;b=right[j] if j is not None else None
        row={'id':'p'+str(k),'target_index':i,'candidate_index':j,
             'target_offset':a['load_offset'] if a else None,'candidate_offset':b['load_offset'] if b else None,
             'islands':sorted({v for v in (island_for.get(i),candidate_island.get(j)) if v is not None}),
             'level':level,'byte_equal':bool(a and b and a['bytes']==b['bytes'])}
        branch=None
        if a and b and not b['fixup_fields'] and a['control'] in ('jump','call') and a['mnemonic']==b['mnemonic'] and len(a['operands'])==len(b['operands'])==1 and a['operands'][0]['kind']==b['operands'][0]['kind']=='imm':
            old,new=a['operands'][0]['value'],b['operands'][0]['value'];destination=mapping.get(old)
            state=('CORRESPONDING_DESTINATION' if destination and destination==(new,True) else
                   'ALIGNED_DESTINATION_UNCERTAIN' if destination and destination[0]==new else
                   'DIFFERENT_ALIGNED_DESTINATION' if destination and destination[1] else 'UNRESOLVED_DESTINATION')
            # Equal relative bytes remain byte-exact; destination evidence is a separate axis.
            same_position=level=='BYTE_EXACT' and a['load_offset']==b['load_offset'] and old==new
            if not same_position and (level!='BYTE_EXACT' or state!='CORRESPONDING_DESTINATION'):
                branch={'state':state,'target_destination':old,'candidate_destination':new,
                        'basis':'destination instruction in protected byte anchor' if destination and destination[1] else 'weak or unavailable destination alignment; no semantic equivalence',
                        'confidence':'high' if state=='CORRESPONDING_DESTINATION' else 'low'}
        if level=='BYTE_EXACT' and not branch:continue
        row['target']=a['instruction'] if a else None;row['candidate']=b['instruction'] if b else None
        if branch:row['branch']=branch
        if b and b['fixup_fields']:
            row['unresolved_fixups']=b['fixup_fields'];row['note']='Symbol identity and full binding obligations remain unresolved, including when raw numbers coincide.'
        pattern=_operand_pattern(a,b) if a and b and not branch else None
        if pattern:
            row['operand_changes']=[{'operand':idx,'field':field,'target':old,'candidate':new,'width':width} for _,idx,field,old,new,width in pattern]
            eligible[pattern[0][0]].append(row)
        observations.append(row)
    if not complete:observations.append({'id':'undecoded','islands':[islands[-1]['id']],'level':'UNDECODED','note':'Incomplete decoding; no families asserted.'})
    families=[];covered=set()
    if complete:
        for kind,sites in eligible.items():
            if len(sites)<2:continue
            # Include unchanged uses of affected locations in the same scope, exposing
            # storage reuse and conflicting mappings rather than silently choosing a majority.
            mapping_rows=[c for site in sites for c in site['operand_changes']]
            touched={c['target'] for c in mapping_rows}|{c['candidate'] for c in mapping_rows}
            checks=[]
            for k,(i,j,_) in enumerate(pairs):
                if i is None or j is None:continue
                a,b=left[i],right[j]
                if a['mnemonic']!=b['mnemonic'] or b['fixup_fields']:continue
                if kind=='REGISTER_ROLE':
                    for register in set(a.get('implicit_reads',[])+a.get('implicit_writes',[])) & touched:
                        checks.append({'site':'p'+str(k),'operand':'implicit','target':register,'candidate':register,
                                       'width':None,'candidate_width':None})
                for idx,(old,new) in enumerate(zip(a['operands'],b['operands'])):
                    fields=['disp'] if kind=='BP_DISPLACEMENT' else ['reg','base','index']
                    for field in fields:
                        if field not in old or field not in new:continue
                        if kind=='BP_DISPLACEMENT' and not (old.get('base')==new.get('base')=='bp' and not old.get('index') and not new.get('index')):continue
                        if old[field] in touched or new[field] in touched:
                            checks.append({'site':'p'+str(k),'operand':idx,'target':old[field],'candidate':new[field],
                                           'width':old['width'],'candidate_width':new['width']})
            forward=collections.defaultdict(set);reverse=collections.defaultdict(set)
            for c in checks:forward[c['target']].add(c['candidate']);reverse[c['candidate']].add(c['target'])
            contradictions=[c for c in checks if len(forward[c['target']])!=1 or len(reverse[c['candidate']])!=1 or c['width']!=c['candidate_width']]
            if kind=='REGISTER_ROLE':
                contradictions.extend({'site':'p'+str(k),'reason':'Opaque implicit register effects in scope'}
                    for k,(i,j,_) in enumerate(pairs) if
                    (i is not None and left[i].get('implicit_effects_note')) or (j is not None and right[j].get('implicit_effects_note')))
            consistent=not contradictions
            family={'id':'f'+str(len(families)+1),'kind':kind,'state':'SUPPORTED_GROUPING' if consistent else 'CONTRADICTED_PATTERN',
                'scope':{'target_instructions':[0,len(left)],'candidate_instructions':[0,len(right)],
                         'frame_or_abi':'No source-object, local/parameter, or BP-stability proof is inferred.'},
                'mapping':[{'target':a,'candidate':b,'width':w} for a,b,w in sorted({(c['target'],c['candidate'],c['width']) for c in mapping_rows})],
                'sites':[s['id'] for s in sites],'islands':sorted({i for s in sites for i in s['islands']}),
                'contradictions':contradictions,'confidence':'medium' if consistent else 'low',
                'basis':'All aligned uses of affected operands checked for one-to-one correspondence and equal access widths; alignment remains heuristic.',
                'limit':'Observed operand pattern, not a source explanation, liveness proof, ABI assignment, or repair. BP offsets are not labelled locals or parameters; register roles retain subregister widths and implicit effects.'}
            families.append(family)
            if consistent:covered.update(family['sites'])
    corresponding=[o['id'] for o in observations if o.get('branch',{}).get('state')=='CORRESPONDING_DESTINATION']
    unresolved=[o['id'] for o in observations if o.get('unresolved_fixups')]
    residual=[o['id'] for o in observations if o['id'] not in covered and o['id'] not in corresponding and o['id'] not in unresolved]
    return {'authority':'OBSERVATIONS_AND_SUPPORTED_GROUPINGS_ONLY','observations':observations,'families':families,
            'corresponding_branches':corresponding,'unresolved_fixups':unresolved,'residuals':residual,
            'coverage':'Every non-byte-exact aligned pair and byte-exact transfer with unproven destination is retained. Families may overlap; counts are not independent repairs.'}


def compare_streams(target,candidate,fixups=(),bound=False,segment='UNIT_TEXT'):
    """Pure analysis; never mutates inputs, never invokes or relaxes binding."""
    target,candidate=bytes(target),bytes(candidate)
    left,right=_decode(target),_decode(candidate)
    normalization=_fixups(right,fixups,segment,bound);notes=[]
    decoded=[sum(len(bytes.fromhex(r['bytes'])) for r in rows) for rows in (left,right)]
    complete=decoded==[len(target),len(candidate)]
    if not complete:notes.append('Decode incomplete: undecoded tails are explicit unresolved byte ranges; no equality claim for them.')
    pairs,strong=_align(left,right,notes)
    # Promote only uniquely supported exact runs. Tiny/repeated local matches
    # remain visible in full alignment, not protected as stable anchors.
    protected=set();at=0
    while at<len(pairs):
        if pairs[at][2]!='BYTE_EXACT':at+=1;continue
        end=at+1
        while end<len(pairs) and pairs[end][2]=='BYTE_EXACT':end+=1
        run=pairs[at:end];i,j,_=run[0];length=len(run)
        anchored=any((x,y) in strong for x,y,_ in run)
        unique=length>=2 and _nontrivial(left[i:i+length]) and _occurrences(_tokens(left),_tokens(left[i:i+length]))==1 and _occurrences(_tokens(right),_tokens(right[j:j+length]))==1
        if anchored or unique:protected.update(range(at,end))
        at=end
    correspondence={left[i]['load_offset']:right[j]['load_offset'] for k,(i,j,level) in enumerate(pairs)
                    if i is not None and j is not None and k in protected}
    blocks=[];at=0;x=y=0
    while at<len(pairs):
        exact=at in protected;end=at+1
        while end<len(pairs) and (end in protected)==exact:end+=1
        run=pairs[at:end];nx=x+sum(i is not None for i,j,_ in run);ny=y+sum(j is not None for i,j,_ in run)
        block={'kind':'ANCHOR' if exact else 'ISLAND','target':_range(left,x,nx,decoded[0]),
               'candidate':_range(right,y,ny,decoded[1])}
        if exact:block['basis']='byte-exact, position-independent, unmasked'
        else:
            block['classifications']=_classify(run,left,right,correspondence)
            block['resumes_exact']=end<len(pairs) and end in protected
        blocks.append(block);x,y=nx,ny;at=end
    if not complete:
        blocks.append({'kind':'ISLAND','target':{'instructions':[len(left),len(left)],'bytes':[decoded[0],len(target)]},
            'candidate':{'instructions':[len(right),len(right)],'bytes':[decoded[1],len(candidate)]},'resumes_exact':False,
            'classifications':[{'class':'LOCAL_CODEGEN_UNCLASSIFIED','confidence':'low','evidence':['Undecoded tail bytes'],'omitted_evidence':0}]})
    anchors=[b for b in blocks if b['kind']=='ANCHOR'];islands=[b for b in blocks if b['kind']=='ISLAND']
    for index,b in enumerate(islands,1):b['id']=index
    counts=collections.Counter(level for _,_,level in pairs)
    exact=sum(a['target']['instructions'][1]-a['target']['instructions'][0] for a in anchors)
    prefix=anchors[0] if blocks and blocks[0]['kind']=='ANCHOR' else None
    suffix=anchors[-1] if complete and blocks and blocks[-1]['kind']=='ANCHOR' else None
    return {'schema':SCHEMA,'authority':'DIAGNOSTIC_ONLY; no acceptance decision',
        'algorithm':'unique 4-instruction >=8-byte monotonic anchors + bounded edit alignment; ties diagonal, deletion, insertion',
        'engine_sha256':sha(Path(__file__).read_bytes()),'target':identity(target),'candidate':identity(candidate),
        'comparison':'bound payload' if bound else 'unbound object','fixup_normalization':normalization,
        'decode_complete':complete,'decoded_bytes':decoded,'instruction_count_note':'Linear decode includes alignment NOPs; not a reachable-CFG instruction count.',
        'counts':{'target_instructions':len(left),'candidate_instructions':len(right),'exact_instructions':exact,
                  'byte_equal_aligned_pairs':counts['BYTE_EXACT'],'decoded_exact_pairs':counts['DECODED_EXACT'],
                  'structural_pairs':counts['STRUCTURAL'],'fixup_normalized_pairs':counts['FIXUP_NORMALIZED'],
                  'anchors':len(anchors),'mismatch_islands':len(islands),
                  'target_anchored_percent':round(100*exact/len(left),2) if left else 0},
        'exact_prefix':prefix,'exact_suffix':suffix,'anchors':anchors,'islands':islands,'notes':notes,
        'patterns':_patterns(left,right,pairs,protected,islands,complete),
        'target_instructions':left,'candidate_instructions':right,
        'alignment':[{'target_index':i,'candidate_index':j,'level':level,'protected':k in protected} for k,(i,j,level) in enumerate(pairs)]}


def compact(report, limit=12):
    """Bounded summary with explicit omissions; full streams exist only in artifact."""
    def bounded(values):return values if len(values)<=limit else values[:limit-1]+values[-1:]
    result={k:report[k] for k in ['schema','authority','engine_sha256','target','candidate','comparison',
        'decode_complete','decoded_bytes','instruction_count_note','counts','exact_prefix','exact_suffix']}
    result['notes']=report['notes'][:limit]
    result['omitted_notes']=report.get('omitted_notes',0)+max(0,len(report['notes'])-limit)
    result['anchors']=bounded(report['anchors']);result['islands']=bounded(report['islands'])
    result['omitted_anchors']=len(report['anchors'])-len(result['anchors'])
    result['omitted_islands']=len(report['islands'])-len(result['islands'])
    result['classifications']=sorted({c['class'] for island in report['islands'] for c in island['classifications']})
    result['fixup_normalization']={**report['fixup_normalization'],'fields':bounded(report['fixup_normalization']['fields']),
        'ignored':bounded(report['fixup_normalization']['ignored']),
        'omitted_fields':report['fixup_normalization'].get('omitted_fields',0)+max(0,len(report['fixup_normalization']['fields'])-limit),
        'omitted_ignored':report['fixup_normalization'].get('omitted_ignored',0)+max(0,len(report['fixup_normalization']['ignored'])-limit)}
    if 'patterns' in report:
        patterns=report['patterns']
        if 'observations' not in patterns:result['patterns']=copy.deepcopy(patterns)  # already compact
        else:
            by_id={o['id']:o for o in patterns['observations']}
            def sites(ids):return [by_id[i] for i in ids[:limit]]
            families=[]
            for family in patterns['families'][:4]:
                item=copy.deepcopy(family)
                for key in ('sites','islands','mapping','contradictions'):
                    item[key+'_count']=len(item[key]);item[key]=item[key][:limit]
                    item['omitted_'+key]=max(0,item[key+'_count']-limit)
                families.append(item)
            result['patterns']={'authority':patterns['authority'],'coverage':patterns['coverage'],
                'families':families,'omitted_families':max(0,len(patterns['families'])-4),
                'observation_count':len(patterns['observations'])}
            for key in ('corresponding_branches','unresolved_fixups','residuals'):
                result['patterns'][key]=sites(patterns[key])
                result['patterns'][key+'_count']=len(patterns[key])
                result['patterns']['omitted_'+key]=max(0,len(patterns[key])-limit)
    return result


def diagnose(target,candidate,receipt,fixups,bound=False,segment='UNIT_TEXT'):
    work=Path(receipt['work_directory'])
    try:
        report=compare_streams(target,candidate,fixups,bound,segment)
        summary=compact(report)
        prefix=report['exact_prefix'];index=prefix['target']['instructions'][1] if prefix else 0
        report['first_differing_instruction']={'index':index,
            'target':report['target_instructions'][index] if index<len(report['target_instructions']) else None,
            'candidate':report['candidate_instructions'][index] if index<len(report['candidate_instructions']) else None}
        # Content addressed: re-analysis cannot overwrite an older receipt's evidence.
        destination=work/('diagnostic-'+sha(json_bytes(report))+'.json')
        write_json(destination,report)
        return {'target':report['target'],'candidate':report['candidate'],'comparison':report['comparison'],
            'decoded_bytes':report['decoded_bytes'],'first_differing_instruction':report['first_differing_instruction'],
            'match_summary':summary,'full_diagnostic':str(destination),'full_diagnostic_identity':identity(destination.read_bytes()),
            'omitted':'Full instructions, pair alignment and any omitted islands/evidence are in full_diagnostic; diagnostic only.'}
    except Exception as error:
        return {'target':identity(target),'candidate':identity(candidate),'diagnostic_error':str(error),
                'authority':'DIAGNOSTIC_UNAVAILABLE; acceptance must still compare complete bytes/fixups'}


def format_summary(summary,name='candidate',acceptance='NOT EVALUATED',islands=False):
    if not summary:return 'MATCH DIAGNOSIS unavailable; inspect the diagnostic error/artifact.'
    if summary.get('patterns') and not islands:return format_patterns(summary,name,acceptance)
    counts=summary['counts'];lines=[f'MATCH DIAGNOSIS: {name} ({summary["comparison"]})',
        f'Target {summary["target"]["size"]} bytes / {counts["target_instructions"]} instructions; candidate {summary["candidate"]["size"]} bytes / {counts["candidate_instructions"]} instructions.',
        f'Protected byte-exact: {counts["exact_instructions"]} instructions ({counts["target_anchored_percent"]}% of target), {counts["anchors"]} anchors; {counts["mismatch_islands"]} islands. Diagnostic metrics only.']
    def span(item):return '+0x%X..+0x%X'%tuple(item['bytes'])
    lines.append('Preserve target: '+', '.join(span(a['target']) for a in summary['anchors']))
    for island in summary['islands']:
        classes=', '.join(c['class']+' ['+c['confidence']+']' for c in island['classifications'])
        lines.append(f'#{island["id"]} target {span(island["target"])} / candidate {span(island["candidate"])}: {classes}')
        evidence=[e for c in island['classifications'] for e in c['evidence']]
        lines.extend('  '+e for e in evidence[:2])
        if len(evidence)>2:lines.append(f'  {len(evidence)-2} further evidence items in structured/full diagnosis.')
        if island['resumes_exact']:lines.append('  Exact stream resumes after this island.')
    suffix=summary['exact_suffix'];lines.append('Exact suffix: '+(span(suffix['target'])+' / '+span(suffix['candidate']) if suffix else 'not independently anchored'))
    norm=summary['fixup_normalization'];lines.append(f'Fixup normalization: {len(norm["fields"])+norm["omitted_fields"]} fields, alignment only; {norm["rule"]}')
    lines.append(f'Omitted: {summary["omitted_anchors"]} anchors, {summary["omitted_islands"]} islands, {summary.get("omitted_notes",0)} notes; use full diagnostic artifact.')
    lines.extend(summary['notes']);lines.append(f'Acceptance: {acceptance}; diagnostic alignment never relaxes byte, extent or fixup checks.')
    return '\n'.join(lines)


def routine_summary(summary):
    """Routine packet: families/residuals first; original islands remain an explicit expansion."""
    if not summary.get('patterns'):return summary
    result={k:summary[k] for k in ('schema','authority','engine_sha256','target','candidate','comparison','decode_complete','counts')}
    result['preserve_byte_ranges']=[{'target':a['target']['bytes'],'candidate':a['candidate']['bytes']} for a in summary['anchors']]
    result['omitted_anchors']=summary['omitted_anchors']
    result['patterns']=copy.deepcopy(summary['patterns'])
    for f in result['patterns']['families']:
        f.pop('limit',None)  # common claim boundary already in authority/coverage and full evidence
    for key in ('corresponding_branches','unresolved_fixups','residuals'):
        result['patterns'][key]=[{k:v for k,v in o.items() if k not in ('target_index','candidate_index','operand_changes','note')} for o in result['patterns'][key]]
    return result


def format_patterns(summary,name,acceptance):
    p=summary['patterns'];c=summary['counts']
    lines=[f'MATCH DIAGNOSIS: {name} ({summary["comparison"]})',f'Strict status: {acceptance}; diagnostics cannot discharge extent/byte/fixup obligations.',
           f'Target/candidate: {summary["target"]["size"]}/{summary["candidate"]["size"]} bytes, {c["target_instructions"]}/{c["candidate_instructions"]} linear instructions.',
           f'Byte preservation evidence: {c["exact_instructions"]} target instructions, {c["anchors"]} anchors, {c["mismatch_islands"]} islands; not source-line boundaries.']
    for f in p['families']:
        mapping=', '.join(str(m['target'])+'->'+str(m['candidate'])+f' ({m["width"]}B)' for m in f['mapping'])
        lines.append(f'{f["id"]} {f["kind"]} {f["state"]} [{f["confidence"]}]: {mapping}; {f["sites_count"]} sites, islands {f["islands"]}, {f["contradictions_count"]} contradictions.')
        if f['contradictions']:lines.append('  Contradictions: '+str(f['contradictions'][:2]))
    def locations(rows):return ', '.join(str(r['target_offset'])+'->'+str(r['candidate_offset']) for r in rows)
    if p['corresponding_branches_count']:
        lines.append(f'Corresponding anchored branch destinations: {p["corresponding_branches_count"]} sites (decimal offsets {locations(p["corresponding_branches"])}); encoding/layout changes remain byte differences.')
    if p['unresolved_fixups_count']:
        symbols=sorted({str(f['target']) for r in p['unresolved_fixups'] for f in r['unresolved_fixups']})
        lines.append(f'Unresolved fixup operands: {p["unresolved_fixups_count"]} sites, symbols {symbols}; full binding still required.')
    lines.append(f'Ungrouped residual observations: {p["residuals_count"]} (not a count of independent fixes).')
    for r in p['residuals']:
        lines.append(f'  {r["id"]} islands {r["islands"]}: {r.get("target_offset")} {r.get("target")} -> {r.get("candidate_offset")} {r.get("candidate")}; '+r.get('branch',{}).get('state',r['level']))
    omitted=sum(p.get('omitted_'+k,0) for k in ('families','corresponding_branches','unresolved_fixups','residuals'))+sum(f['omitted_sites']+f['omitted_mapping']+f['omitted_contradictions']+f['omitted_islands'] for f in p['families'])
    lines.append(f'Observed patterns only; {omitted} omitted entries. Full artifact retains every site/island and contradictions; --islands expands localization. Exact stream resumes at the original anchor boundaries.')
    return '\n'.join(lines)
