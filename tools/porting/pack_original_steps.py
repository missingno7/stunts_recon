"""Compact a sealed ORIGINAL race capture. Never reads a port result."""
from __future__ import annotations
import argparse
from collections import Counter
import json
from pathlib import Path
import zlib
from .compare_step_streams import original,digest,boundary,input_identity
from .compare_tick_streams import read_rows

def pack(folder):
    streams=original(folder);receipt=streams.pop('receipt')
    cases=[];seen=set();counts=Counter()
    for row in read_rows(folder/'boundaries.jsonl'):
        if row.get('name') not in ('mat_vec','transmult'):continue
        value=boundary(row);key=digest(input_identity(value));name=row['name']
        if key in seen or counts[name]>=12:continue
        seen.add(key);counts[name]+=1;cases.append(value)
    # These are observations, including known differences, not a whitelist of
    # port results. Tests explicitly name their narrower agreeing claim.
    result=dict(schema='stunts-dosbox-step-fixture-v1',scenario='race',audio=receipt['audio'],
        frames=receipt['requested_frames'],script='inputs/race.json',provenance=receipt,
        input=streams['input'],states=streams['states'],matrix_cases=cases,
        groups={name:{key:dict(count=len(value),sha256=digest(value)) for key,value in groups.items()}
            for name,groups in streams.items() if name in ('boundaries','frames','completed_frames','sound')},sparse_frames=[])
    rows=read_rows(folder/'original.jsonl')
    from .compare_sound_events import targets
    result['engine_targets']=targets(rows)
    # Ordered within each voice/resource/entry, not a claim about cross-entry
    # interleaving. Keep the full differing sound groups above as well.
    from collections import defaultdict
    sequences=defaultdict(list)
    for group,values in streams['sound'].items():
        if not group.startswith('1:'):continue
        for r in values:
            resource=(r['resource'] or {}).get('name','none')
            sequences[f'{resource}:voice={r["voice"]}:entry={r["entry"]:02x}'].append(r)
    result['sound_sequences']={k:dict(count=len(v),sha256=digest(v)) for k,v in sequences.items()}
    with (folder/'original.frames').open('rb') as payloads:
        for frame in (1,5,18,50,120,200):
            choices=[r for r in rows if r['kind']=='frame' and r['game_frame']==frame
                and r['reason']=='race_frame_complete']
            if not choices:continue
            r=choices[-1];payloads.seek(r['payload']*64768);raw=payloads.read(64768)
            result['sparse_frames'].append(dict(game_frame=frame,reason=r['reason'],
                payload_sha256=__import__('hashlib').sha256(raw).hexdigest(),zlib_hex=zlib.compress(raw).hex()))
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('capture',type=Path);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args()
    if a.out.exists():p.error('refusing to overwrite an expectation')
    result=pack(a.capture);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_bytes((json.dumps(result,indent=2)+'\n').encode())
    target=a.out.parent/result['script'];source=a.capture/'input-script.json'
    if target.exists() and target.read_bytes()!=source.read_bytes():raise ValueError('fixture script differs')
    target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(source.read_bytes())
    print(a.out)

if __name__=='__main__':main()
