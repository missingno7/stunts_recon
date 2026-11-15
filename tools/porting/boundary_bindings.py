"""Ground semantic hooks in locked symbols and accepted recipe code frames."""
from pathlib import Path
import json
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path[:0]=[str(ROOT/'port'),str(ROOT/'tools/porting'),str(ROOT/'tools'),str(ROOT/'build/python')]
from boundary_adapter import CONTRACTS
from diffharness.oracle import OracleImage,SymbolMap,RoutineAddress

def resolve(name):
    symbols=SymbolMap()
    try:return symbols.resolve(name)
    except KeyError:pass
    candidates={int(a) for a,row in symbols.names.items() if row.get('kind')=='code' and row.get('name')==name}
    candidates.update(r['start'] for r in symbols.functions if r.get('name')==name and r.get('start') is not None)
    if len(candidates)!=1:raise ValueError('No exact original entry binding: '+name)
    start=candidates.pop()
    manifest=json.loads((ROOT/'layout/manifest.json').read_text())
    owners=[o for o in manifest['owners'] if o.get('start',1<<30)<=start<o.get('end',0) and o.get('recipe')]
    if len(owners)!=1:raise ValueError('No unique accepted owner for '+name)
    recipe=json.loads((ROOT/owners[0]['recipe']).read_text())
    frame=recipe['original_frame_load_address']
    ends=[r['end'] for r in symbols.functions if r.get('start')==start and r.get('end')]
    if len(set(ends))!=1:raise ValueError('No original function extent for '+name)
    return RoutineAddress(name,start,ends[0],frame)

def configuration(output,script,frames):
    image=OracleImage.load(ROOT/'assets')
    data=json.loads((ROOT/'layout/data-symbols.json').read_text())['symbols']
    lines=[f'BOUNDARY_OUTPUT {output}',f'FRAME_LIMIT {frames}'];bindings=[]
    for c in CONTRACTS:
        r=resolve(c['name']);cs,ip=r.far_at(0x1010)
        mask=sum(1<<i for i,a in enumerate(c['args']) if a is None)
        lines.append(f"BC {c['name']} {(cs<<16)|ip:x} {len(c['args'])} {c['ret']} {mask}")
        for label,expression,size,read in c.get('regions',[]):
            arg=c['params'].index(expression) if expression in c['params'] else -1
            global_name=next((n for n,nbytes,rd in c.get('globals',[]) if nbytes==size and rd==read),None)
            if arg<0 and not global_name:raise ValueError('Missing original global span '+label)
            offset=data[global_name]['load_address'] if arg<0 else 0
            lines.append(f"BR {c['name']} {label} {arg} {size} {read} {offset:x}")
        bindings.append(dict(name=c['name'],start=r.start,end=r.end,frame=r.code_base,
            bytes=image.load_image[r.start:r.start+16].hex(),native_source='port/'+c['source']))
    for name,phase in [('run_intro',1),('select',2),('getprotection',3),('get_pstring',4),('run_game',5)]:
        r=resolve(name);cs,ip=r.far_at(0x1010);lines.append(f'PHASE {(cs<<16)|ip:x} {phase}')
        bindings.append(dict(name=name,start=r.start,end=r.end,frame=r.code_base,phase=phase))
    for name in ['normkey','normkeyi']:
        r=resolve(name);cs,ip=r.far_at(0x1010);lines.append(f'POLL {(cs<<16)|ip:x}')
    r=resolve('moveeverything');cs,ip=r.far_at(0x1010);lines.append(f'INPUTSITE {(cs<<16)|ip:x}')
    for event in script['events']:
        phase,at=event['phase'],event['at'];payload=event['payload']
        if event['channel']=='dos.keyboard.scancodes':lines +=[f'SKEY {phase} {at} {b}' for b in payload]
        elif event['channel']=='dos.mouse.normalized':
            lines.append(f'SMOUSE {phase} {at} {payload["u"]} {payload["v"]} {payload["buttons"]}')
        else:raise ValueError('Unknown semantic input channel')
    return lines,bindings
