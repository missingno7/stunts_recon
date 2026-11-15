"""Separate audio-frame targets from ISR smoothing; never accept unequal pitches.

An engine epoch starts at engine_init. Within the race, compare engine targets
in audio-frame consumption order (not IRQ or render frame), and each driver's
ordered resource/voice/entry stream. Partitioning is diagnostic: it does not
prove the interleaving between different entries. All unequal content remains
a failure, including the initial smoothing ramp and end-of-capture tails.
"""
from __future__ import annotations
import argparse
from collections import defaultdict
import json
from pathlib import Path
from .compare_step_streams import original,port,resync
from .compare_tick_streams import read_rows,first_difference


def targets(rows):
    return [[value&65535 for value in r.get('values_u16',r.get('values_u32',[]))]
        for r in rows if r.get('kind')=='engine_target']


def context(rows):
    start=next((i for i,r in enumerate(rows) if r.get('kind')=='engine_init'),None)
    if start is None:return None
    init=rows[start];ticks=0;milestones=[]
    for r in rows[start:]:
        kind=r['kind']
        if kind=='engine_tick':ticks+=1
        elif kind in ('engine_start','engine_target'):
            milestones.append(dict(kind=kind,raw_ticks_since_init=r['raw_tick']-init['raw_tick'],
                engine_callbacks_since_init=ticks,game_frame=r['game_frame']))
            if kind=='engine_target':break
    return dict(init_raw_tick=init['raw_tick'],milestones=milestones)


def compare(folder,actual_folder):
    expected,actual=original(folder),port(actual_folder)
    arows=read_rows(Path(folder)/'original.jsonl');brows=read_rows(Path(actual_folder)/'sound.jsonl')
    a,b=targets(arows),targets(brows)
    report=dict(engine_targets=dict(original_count=len(a),port_count=len(b),
        agrees=bool(a) and a==b,first_difference=first_difference(a,b)),
        initialization=dict(original=context(arows),port=context(brows)),driver_sequences={})
    def sequences(groups):
        result=defaultdict(list)
        for group,rows in groups.items():
            if not group.startswith('1:'):continue
            for r in rows:
                resource=(r['resource'] or {}).get('name','none')
                result[f'{resource}:voice={r["voice"]}:entry={r["entry"]:02x}'].append(r)
        return result
    left,right=sequences(expected['sound']),sequences(actual['sound'])
    for key in sorted(left.keys()|right.keys()):
        a,b=left[key],right[key]
        report['driver_sequences'][key]=dict(original_count=len(a),port_count=len(b),
            agrees=bool(a) and a==b,first_difference=first_difference(a,b),
            resynchronised=resync(a,b) if a!=b else [])
    report['agrees']=report['engine_targets']['agrees'] and all(r['agrees'] for r in report['driver_sequences'].values())
    return report


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('original',type=Path);p.add_argument('port',type=Path)
    p.add_argument('--out',type=Path,required=True);a=p.parse_args();report=compare(a.original,a.port)
    a.out.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(json.dumps({k:v for k,v in report.items() if k!='driver_sequences'},indent=2))
    print('Differing sequences:',[k for k,v in report['driver_sequences'].items() if not v['agrees']])
    return 0 if report['agrees'] else 1

if __name__=='__main__':raise SystemExit(main())
