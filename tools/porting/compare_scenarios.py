"""Compare scoped checkpoints, ordered input, simulation epochs and saved files.

Epoch increments on a repeated/decreasing GAMESTATE frame, never on matching
contents. UI idle polls are not a fixed-count stream. Comparable routine inputs
retain exact bounded reads/writes/returns; unmatched inputs are explicitly open.
"""
from __future__ import annotations
import argparse
from collections import Counter,defaultdict
import hashlib
import json
from pathlib import Path
from .compare_step_streams import sealed_original,boundary,input_identity,digest
from .compare_tick_streams import read_rows,state_projection,key_word,first_difference,fnv
ROOT=Path(__file__).resolve().parents[2]

def sha(data):return hashlib.sha256(data).hexdigest()


def highscore_fields(data):
    """Fields read by drawhighs/gethighscoretext; unused stack tails stay raw debt.

    Locked HighScoreRecord stride 52; strings 0,17,42, style41, score50.
    This projection never turns the separate whole-file comparison into a pass.
    """
    if len(data)!=364:raise ValueError('high-score table must contain seven DOS records')
    return [dict(name=r[:17].split(b'\0',1)[0].hex(),car=r[17:41].split(b'\0',1)[0].hex(),
        opponent=r[42:50].split(b'\0',1)[0].hex(),style=r[41],score=int.from_bytes(r[50:52],'little'))
        for r in (data[i:i+52] for i in range(0,364,52))]

def states(rows,original):
    result={};previous=None;epoch=0
    for r in rows:
        if (r.get('kind') if original else r.get('event_type'))!=('state' if original else 'state_snapshot'):continue
        value=state_projection(bytes.fromhex(r['bytes_hex']));frame=value['game_frame']
        if previous is not None and frame<=previous:epoch+=1
        previous=frame;result[f'{epoch}:{frame}']=value
    return result


def race_facts(rows,original):
    # Locked GAMESTATE.playerstate +152h; CARSTATE gear BEh, crash C9h,
    # transmission CCh (0 selects the manual-input path in carsim).
    gears=[];transmissions=set();crash=None
    for r in rows:
        if (r.get('kind') if original else r.get('event_type'))!=('state' if original else 'state_snapshot'):continue
        data=bytes.fromhex(r['bytes_hex']);frame=int.from_bytes(data[320:322],'little')
        transmissions.add(data[542]);gear=data[528]
        if not gears or gears[-1][1]!=gear:gears.append([frame,gear])
        if data[539] and crash is None:crash=frame
    return dict(gear_changes=gears,transmissions=sorted(transmissions),first_crash_frame=crash)

def completed_frames(rows,read_frame,original):
    result={};previous=None;epoch=0;active=False
    for r in rows:
        kind=r.get('kind') if original else r.get('event_type')
        if kind==('state' if original else 'state_snapshot'):
            value=state_projection(bytes.fromhex(r['bytes_hex']));frame=value['game_frame']
            if previous is not None and frame<=previous:epoch+=1
            previous=frame;active=bool(value['game_inputmode'])
        elif (kind=='frame' and r.get('reason')=='race_frame_complete' if original else kind=='frame_complete'):
            if active and previous==r['game_frame']:
                key=f'{epoch}:{previous}'
                if key not in result:result[key]=sha(read_frame(r))
    return result

def original_data(folder,include_boundaries=True):
    folder=Path(folder);rows,receipt=sealed_original(folder);points={};payloads={}
    with (folder/'original.frames').open('rb') as frames:
        def read_frame(row):
            frames.seek(row['payload']*64768);data=frames.read(64768)
            if len(data)!=64768:raise ValueError('truncated original frame')
            return data
        for r in rows:
            if r.get('kind')!='frame' or not r['reason'].startswith('checkpoint.'):continue
            label=r['reason'][11:];frames.seek(r['payload']*64768);data=frames.read(64768)
            if len(data)!=64768 or label in points:raise ValueError('bad or duplicate checkpoint')
            points[label]=dict(payload_sha256=sha(data),indices_sha256=sha(data[:64000]),palette_sha256=sha(data[64000:]),game_frame=r['game_frame'])
            payloads[label]=data
        completed=completed_frames(rows,read_frame,True)
    files={};hig_fields={}
    for p in (folder/'assets').rglob('*'):
        if not p.is_file() or p.suffix.upper() not in ('.TRK','.HIG','.RPL'):continue
        relative=p.relative_to(folder/'assets');base=ROOT/'assets'/relative;data=p.read_bytes()
        if not base.exists() or data!=base.read_bytes():
            name=str(relative).replace('\\','/').upper();files[name]=dict(size=len(data),sha256=sha(data))
            if p.suffix.upper()=='.HIG':hig_fields[name]=highscore_fields(data)
    return dict(receipt=receipt,terminal=rows[-1]['terminal'],checkpoints=points,payloads=payloads,
        states=states(rows,True),completed_frames=completed,input=[r['scancode'] for r in rows if r.get('kind')=='scan'],
        keys=[key_word(r['bios_word']) for r in rows if r.get('kind')=='key'],files=files,
        highscore_fields=hig_fields,race_facts=race_facts(rows,True),boundaries=read_rows(folder/'boundaries.jsonl') if include_boundaries else [])

def port_data(folder):
    folder=Path(folder);rows=read_rows(folder/'trace.jsonl');points={};payloads={}
    captures={int(p.stem.split('-')[1]):p for p in (folder/'frames').glob('frame-*.fbr')}
    def read_frame(row):return captures[max(i for i in captures if i<=row['frame_id'])].read_bytes()[20:]
    for r in rows:
        reason=r.get('publication_reason','')
        if not reason.startswith('checkpoint.'):continue
        label=reason[11:];data=read_frame(r)
        if fnv(data[:64000])!=r['indices_fnv1a64'] or fnv(data[64000:])!=r['palette_fnv1a64']:
            raise ValueError('last changed FBR does not represent this publication')
        if len(data)!=64768 or label in points:raise ValueError('bad or duplicate checkpoint')
        points[label]=dict(payload_sha256=sha(data),indices_sha256=sha(data[:64000]),palette_sha256=sha(data[64000:]),game_frame=r['game_frame'])
        payloads[label]=data
    files={str(p.relative_to(folder/'saves')).replace('\\','/').upper():dict(size=p.stat().st_size,sha256=sha(p.read_bytes()))
        for p in (folder/'saves').rglob('*') if p.is_file() and p.suffix.upper() in ('.TRK','.HIG','.RPL')}
    stop=next(r for r in rows if r.get('event_type')=='host_stop')
    return dict(terminal=stop['reason'],checkpoints=points,payloads=payloads,states=states(rows,False),completed_frames=completed_frames(rows,read_frame,False),
        input=[scan for r in rows if r.get('event_type')=='input' and r.get('channel')=='dos.keyboard.scancodes' for scan in r['payload']],
        keys=[key_word(r['bios_word']) for r in rows if r.get('event_type')=='key_consumed'],files=files,
        highscore_fields={name:highscore_fields((folder/'saves'/name).read_bytes()) for name in files if name.endswith('.HIG')},race_facts=race_facts(rows,False),
        boundaries=read_rows(folder/'boundaries.jsonl'))

def compare(expected,actual,script):
    wanted=[e['payload']['label'] for e in script['events'] if e['channel']=='oracle.checkpoint' and not e['payload'].get('optional')]
    report=dict(checkpoints={},routines={},terminal_agrees=expected['terminal']=='scenario-checkpoint' and actual['terminal']=='scenario checkpoint reached')
    for label in wanted:
        a,b=expected['checkpoints'].get(label),actual['checkpoints'].get(label)
        report['checkpoints'][label]=dict(agrees=a is not None and a==b,original=a,port=b)
        if a and b and a!=b:
            x,y=expected['payloads'][label],actual['payloads'][label]
            indices=[i for i in range(64000) if x[i]!=y[i]]
            report['checkpoints'][label].update(indices_differ=len(indices),palette_bytes_differ=sum(u!=v for u,v in zip(x[64000:],y[64000:])),
                first_pixels=[dict(x=i%320,y=i//320,original=x[i],port=y[i]) for i in indices[:8]])
    for stream in ('input','keys','states','completed_frames','files','highscore_fields'):
        a,b=expected[stream],actual[stream]
        report[stream]=dict(agrees=a==b,original_count=len(a),port_count=len(b),
            first_difference=first_difference(list(a.items()) if isinstance(a,dict) else a,list(b.items()) if isinstance(b,dict) else b))
    known=defaultdict(set);totals=Counter();counts=defaultdict(Counter);examples={}
    for r in expected['boundaries']:
        if r.get('kind')!='boundary':continue
        value=boundary(r);known[digest(input_identity(value))].add(digest(value));totals[r['name']]+=1
    for r in actual['boundaries']:
        if r.get('kind')!='boundary':continue
        value=boundary(r);key=digest(input_identity(value));status='agrees' if digest(value) in known.get(key,set()) else 'differs' if key in known else 'unmatched_inputs'
        counts[r['name']][status]+=1
        if status!='agrees' and (r['name'],status) not in examples:examples[r['name'],status]=r
    for name in sorted(totals.keys()|counts.keys()):
        report['routines'][name]=dict(original_calls=totals[name],**counts[name],examples={status:examples[name,status]
            for status in ('differs','unmatched_inputs') if (name,status) in examples})
    report['agrees']=report['terminal_agrees'] and all(r['agrees'] for r in report['checkpoints'].values()) and all(report[s]['agrees'] for s in ('input','keys','states','completed_frames','files')) and not any(r.get('differs') for r in report['routines'].values())
    report['claim']='named UI checkpoints, ordered input, states by monotone epoch/frame, file bytes, finite comparable boundary cases; unmatched calls and private sound state unproven'
    return report

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('capture',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    report=compare(original_data(a.capture/'original'),port_data(a.capture/'port'),json.loads((a.capture/'input.json').read_text()))
    a.out.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(json.dumps({k:v for k,v in report.items() if k not in ('routines','states','keys')},indent=2))
    return 0 if report['agrees'] else 1

if __name__=='__main__':raise SystemExit(main())
