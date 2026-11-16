"""Compare original/port observations at semantic boundaries, never IRQ epochs.

No image search or state-content alignment is used for acceptance. A race group
is (active, input GAMESTATE frame, simulation/render scope), preserving every call
within it. Content resynchronisation is an explicitly separate diagnostic.
"""
from __future__ import annotations
import argparse
from collections import Counter,defaultdict
import hashlib
import json
from pathlib import Path
from .compare_tick_streams import read_rows,fnv,state_projection,SCALARS,first_difference

def digest(value):
    return hashlib.sha256(json.dumps(value,sort_keys=True,separators=(',',':')).encode()).hexdigest()

def boundary(row):
    return dict(name=row['name'],args=row['args'],result=row['result'],regions=[
        {k:v for k,v in region.items() if k!='before' or region['read']} for region in row['regions']])

def input_identity(row):
    return dict(name=row['name'],args=row['args'],regions=[
        {k:v for k,v in region.items() if k not in ('before','after') or k=='before' and region['read']}
        for region in row['regions']])

def group_key(scope,frame,active):return f'{active}:{scope}:{frame}'

def boundary_groups(rows):
    groups=defaultdict(list)
    for r in rows:
        if r.get('kind')=='boundary':
            groups[group_key(r['scope'],r['frame'],r.get('active',0))].append(boundary(r))
    return dict(groups)

def sealed_original(folder):
    folder=Path(folder);receipt=json.loads((folder/'receipt.json').read_text())
    if receipt.get('schema')!='stunts-dosbox-tick-receipt-v1' or receipt['returncode']:
        raise ValueError('expected sealed original DOSBox-X capture')
    locked=Path(__file__).resolve().parents[2]/'layout/oracle.lock.json'
    if (receipt['oracle_lock_sha256']!=hashlib.sha256(locked.read_bytes()).hexdigest() or
        receipt['executable_sha256']!=json.loads(locked.read_text())['unpacked']['sha256']):
        raise ValueError('capture is not the locked original executable')
    for path,field in [('original.exe','executable_sha256'),('original.jsonl','capture_sha256'),
        ('original.frames','frames_sha256'),('bindings.json','bindings_sha256'),
        ('boundary-bindings.json','boundary_bindings_sha256'),('boundaries.jsonl','boundary_capture_sha256'),
        ('input-script.json','script_sha256')]:
        with (folder/path).open('rb') as stream:
            checksum=hashlib.file_digest(stream,'sha256').hexdigest()
        if checksum!=receipt[field]:
            raise ValueError('unsealed original capture: '+path)
    rows=read_rows(folder/'original.jsonl')
    if rows[-1].get('kind')!='end':raise ValueError('original capture has no terminal record')
    return rows,receipt

def original(folder):
    folder=Path(folder);rows,receipt=sealed_original(folder)
    if rows[-1].get('terminal')!='frame-limit':raise ValueError('original failed to reach frame limit')
    states={};frames=defaultdict(list);completed=defaultdict(list);sounds=defaultdict(list);cache={}
    active=0
    with (folder/'original.frames').open('rb') as payloads:
        for r in rows:
            if r['kind']=='state':
                value=state_projection(bytes.fromhex(r['bytes_hex']));active=value['game_inputmode']
                if active:
                    key=str(value['game_frame'])
                    if key in states:raise ValueError('duplicate active frame; replay requires epoch contract')
                    states[key]=value
            elif r['kind']=='frame':
                index=r['payload']
                if index not in cache:
                    payloads.seek(index*64768);raw=payloads.read(64768)
                    if len(raw)!=64768:raise ValueError('truncated frame')
                    cache[index]=dict(indices=fnv(raw[:64000]),palette=fnv(raw[64000:]))
                target=completed if r['reason']=='race_frame_complete' else frames
                target[f'{active}:{r["game_frame"]}'].append(dict(reason=
                    'palette' if r['reason']=='video_set_palette' else r['reason'],**cache[index]))
            elif r['kind']=='driver':
                entry=r['entry']
                sounds[group_key(r['scope'],r['game_frame'],r['game_inputmode'])+':'+r['source']].append(
                    dict(driver=r['driver'],entry=entry,arguments=[r['stack_u16'][i] for i in SCALARS[entry]],
                    result=r['return_ax']&255 if entry==0 else r['return_ax'] if entry in (0x36,0x3c) else None,
                    voice=r['voice'],chunk=r['chunk'],resource=r['resource'],payload=r['payload_hex']))
    return dict(boundaries=boundary_groups(read_rows(folder/'boundaries.jsonl')),states=states,
        frames=dict(frames),completed_frames=dict(completed),sound=dict(sounds),receipt=receipt,
        input=[r['scancode'] for r in rows if r['kind']=='scan'])

def port(folder):
    folder=Path(folder);rows=read_rows(folder/'trace.jsonl');states={};frames=defaultdict(list);completed=defaultdict(list);sounds=defaultdict(list)
    for r in rows:
        kind=r.get('event_type')
        if kind=='state_snapshot':
            value=state_projection(bytes.fromhex(r['bytes_hex']))
            if value['game_inputmode']:
                key=str(value['game_frame'])
                if key in states:raise ValueError('duplicate active frame; replay requires epoch contract')
                states[key]=value
        elif kind=='frame_complete':
            completed[f'{r["game_inputmode"]}:{r["game_frame"]}'].append(dict(reason=r['publication_reason'],
                indices=r['indices_fnv1a64'],palette=r['palette_fnv1a64']))
        elif kind=='video_publication':
            # Host-only transition-row publications are outside the named
            # original routine-completion scope, as in the earlier comparator.
            from .compare_tick_streams import VIDEO_REASONS
            if r['publication_reason'] in VIDEO_REASONS:
                frames[f'{r["game_inputmode"]}:{r["game_frame"]}'].append(dict(reason=r['publication_reason'],
                    indices=r['indices_fnv1a64'],palette=r['palette_fnv1a64']))
    for r in read_rows(folder/'sound.jsonl'):
        if r['kind']=='driver':
            sounds[group_key(r['scope'],r['game_frame'],r['game_inputmode'])+':'+r['source']].append(
                dict(driver=r['driver'],entry=r['entry'],arguments=r['arguments_u16'],result=r['return_u16'],
                    voice=r['voice'],chunk=r['chunk'],resource=r['resource'],payload=r['payload_hex']))
    return dict(boundaries=boundary_groups(read_rows(folder/'boundaries.jsonl')),states=states,
        frames=dict(frames),completed_frames=dict(completed),sound=dict(sounds),
        input=[scan for r in rows if r.get('event_type')=='input' and r.get('channel')=='dos.keyboard.scancodes'
            for scan in r['payload']])

def resync(a,b,limit=8):
    """Ordered diff with bounded context; only exact group equality is a pass."""
    from difflib import SequenceMatcher
    matcher=SequenceMatcher(a=[digest(r) for r in a],b=[digest(r) for r in b],autojunk=False)
    return [dict(original_index=i,port_index=j,original_count=ii-i,port_count=jj-j,
        original=a[i:min(ii,i+2)],port=b[j:min(jj,j+2)])
        for tag,i,ii,j,jj in matcher.get_opcodes() if tag!='equal'][:limit]

def compare(expected,actual):
    report={}
    for stream in ('boundaries','states','frames','completed_frames','sound'):
        a,b=expected[stream],actual[stream];keys=sorted(a.keys()|b.keys(),key=lambda k:tuple(
            int(s) if s.isdigit() else s for s in k.split(':')))
        good=[k for k in keys if k in a and k in b and a[k]==b[k]]
        differences=[]
        for k in keys:
            if k in good:continue
            left=a.get(k,[]);right=b.get(k,[])
            detail=dict(group=k,original_count=len(left),port_count=len(right))
            if stream=='states':detail.update(original=left,port=right)
            elif len(differences)<8:
                detail.update(first_difference=first_difference(left,right),resynchronised=resync(left,right))
            differences.append(detail)
        report[stream]=dict(original_groups=len(a),port_groups=len(b),agreeing_groups=len(good),
            agrees=bool(a) and not differences,differences=differences,agreeing=good)
    report['input']=dict(agrees=expected['input']==actual['input'],original_count=len(expected['input']),
        port_count=len(actual['input']),first_difference=first_difference(expected['input'],actual['input']))
    # Compare observed outputs on identical original inputs independently of
    # caller scheduling. This is finite captured-case evidence, not a domain proof.
    known=defaultdict(set);totals=Counter();coverage=defaultdict(Counter);examples={}
    for values in expected['boundaries'].values():
        for r in values:known[digest(input_identity(r))].add(digest(r));totals[r['name']]+=1
    for values in actual['boundaries'].values():
        for r in values:
            key=digest(input_identity(r));name=r['name']
            status='agrees' if digest(r) in known.get(key,set()) else 'differs' if key in known else 'unmatched_inputs'
            coverage[name][status]+=1
            if status!='agrees' and (name,status) not in examples:examples[name,status]=r
    report['routines']={name:dict(original_calls=totals[name],**coverage[name],examples={
        status:examples[name,status] for status in ('differs','unmatched_inputs') if (name,status) in examples})
        for name in sorted(totals.keys()|coverage.keys())}
    original_drivers=defaultdict(list);port_drivers=defaultdict(list)
    for groups,target in ((expected['sound'],original_drivers),(actual['sound'],port_drivers)):
        for values in groups.values():
            for r in values:target[f'port_audio_driver_{r["entry"]:02x}'].append(r)
    driver_report={}
    for name in sorted(original_drivers.keys()|port_drivers.keys()):
        known=defaultdict(set)
        for r in original_drivers[name]:known[digest({k:v for k,v in r.items() if k!='result'})].add(r['result'])
        counts=Counter()
        for r in port_drivers[name]:
            key=digest({k:v for k,v in r.items() if k!='result'})
            counts['agrees' if r['result'] in known.get(key,set()) else 'differs' if key in known else 'unmatched_inputs']+=1
        entry=int(name.rsplit('_',1)[1],16)
        ordered=[]
        for groups in (expected['sound'],actual['sound']):
            ordered.append({key:items for key,values in groups.items()
                if (items:=[r for r in values if r['entry']==entry])})
        driver_report[name]=dict(original_calls=len(original_drivers[name]),**counts,
            ordered_step_stream_agrees=ordered[0]==ordered[1],
            claim_limit='typed arguments/resource/defined return only; ordered step streams compared separately; driver private memory not captured')
    report['driver_entries']=driver_report
    return report

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('original',type=Path);p.add_argument('port',type=Path)
    p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    report=compare(original(a.original),port(a.port));a.out.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(json.dumps({k:{x:v for x,v in r.items() if x not in ('differences','agreeing','first_difference')}
        for k,r in report.items() if k not in ('routines','driver_entries')},indent=2))
    return 0 if all(report[k]['agrees'] for k in ('boundaries','states','frames','completed_frames','sound','input')) else 1

if __name__=='__main__':raise SystemExit(main())
