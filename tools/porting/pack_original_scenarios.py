"""Compact sealed ORIGINAL observations into scoped scenario regression cases.

Store every state/completed-frame digest, ordered input and file digests, plus
eight distinct original argument sets per boundary. Original receipts seal the
raw logs; these finite cases do not prove unobserved inputs.
"""
from __future__ import annotations
import argparse
from collections import defaultdict
import json
from pathlib import Path
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT))
from tools.porting.compare_scenarios import original_data
from tools.porting.compare_step_streams import boundary,input_identity,digest
from tools.porting.compare_sound_events import compact_sound
from tools.porting.compare_tick_streams import read_rows
from tools.porting.capture_storage import byte_size,discard_raw_frames


def pack(capture,out,name,audio,replace=False,keep_raw=False):
    data=original_data(capture/'original',include_boundaries=False)
    script=json.loads((capture/'input.json').read_text())
    if digest(script)!=digest(json.loads((capture/'original/input-script.json').read_text())):
        raise ValueError('scenario script is not the sealed original input')
    samples=defaultdict(dict)
    with (capture/'original/boundaries.jsonl').open() as stream:
        for line in stream:
            r=json.loads(line)
            if r.get('kind')!='boundary':continue
            value=boundary(r);key=digest(input_identity(value));sample=samples[r['name']]
            if key in sample:sample[key]['outputs'].add(digest(value))
            elif len(sample)<8:sample[key]=dict(input=input_identity(value),outputs={digest(value)})
    for cases in samples.values():
        for sample in cases.values():sample['outputs']=sorted(sample['outputs'])
    result=dict(schema='stunts-original-scenario-v1',name=name,audio=audio,script=script,
        original_receipt=data['receipt'],checkpoints=data['checkpoints'],
        states={k:digest(v) for k,v in data['states'].items()},completed_frames=data['completed_frames'],
        input=data['input'],keys=data['keys'],files=data['files'],race_facts=data['race_facts'],highscore_fields=data['highscore_fields'],boundaries=dict(samples),
        claim='named checkpoints; observed state/frame hashes; ordered input; whole saved files; at most eight original inputs per routine, no private sound state')
    if name in ('manual-crash','highscore','replay'):
        result['race_sound']=compact_sound(read_rows(capture/'original/original.jsonl'),True,
            200 if name=='replay' else 433)
    out.mkdir(parents=True,exist_ok=True);target=out/(name+'-'+audio+'.json')
    if target.exists() and not replace:raise FileExistsError(target)
    target.write_bytes((json.dumps(result,sort_keys=True,separators=(',',':'))+'\n').encode())
    removed=0 if keep_raw else discard_raw_frames(capture)
    print(target,len(result['states']),len(result['completed_frames']),sum(map(len,samples.values())))
    print(f"packed {target.stat().st_size} bytes; removed {removed} raw frame bytes; "
          f"capture directory now {byte_size(capture)} bytes")


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('capture',type=Path);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--name',required=True);p.add_argument('--audio',choices=['ad15','pc15'],required=True);p.add_argument('--replace',action='store_true')
    p.add_argument('--keep-raw',action='store_true',help='retain uncompressed original frame dumps')
    a=p.parse_args();pack(a.capture,a.out,a.name,a.audio,a.replace,a.keep_raw)

if __name__=='__main__':main()
