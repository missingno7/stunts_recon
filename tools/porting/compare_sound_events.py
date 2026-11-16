"""Compare engine pitch targets, and all other ordered active sound requests.

An engine epoch starts at engine_init. Within the race, compare engine targets
in audio-frame consumption order (not IRQ or render frame), and each driver's
ordered resource/voice/entry stream. Partitioning is diagnostic: it does not
prove the interleaving between different entries. Engine smoothing prehistory
depends on original loading CPU time; only engine pitch entry 24h is represented
by its exact target records. Other pitch entries and all other content compare.
"""
from __future__ import annotations
import argparse
from collections import defaultdict
import json
from pathlib import Path
from .compare_step_streams import resync,digest
from .compare_tick_streams import read_rows,first_difference,SCALARS
from .compare_step_streams import sealed_original


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


def scoped_rows(rows,through_frame):
    if through_frame is None:return rows
    result=[]
    for row in rows:
        # A chronological prefix, never a filter which re-admits later seek
        # epochs merely because their frame number has returned to zero.
        if row.get('game_inputmode')==1 and row.get('game_frame',0)>through_frame:break
        result.append(row)
    return result


def driver_sequences(rows,is_original,through_frame=None):
    result=defaultdict(list)
    for r in scoped_rows(rows,through_frame):
        if r.get('kind')!='driver' or r['game_inputmode']!=1:continue
        entry=r['entry'];resource=(r['resource'] or {}).get('name','none')
        result[f'{resource}:voice={r["voice"]}:entry={entry:02x}'].append(
            dict(driver=r['driver'],entry=entry,arguments=[r['stack_u16'][i] for i in SCALARS[entry]] if is_original else r['arguments_u16'],
                result=(r['return_ax']&255 if entry==0 else r['return_ax'] if entry in (0x36,0x3c) else None) if is_original else r['return_u16'],
                voice=r['voice'],chunk=r['chunk'],resource=r['resource'],payload=r['payload_hex']))
    return result


def compact_sound(rows,is_original,through_frame,pitch_entries=None):
    sequences=driver_sequences(rows,is_original,through_frame)
    if pitch_entries is None:
        continuous={k[:-2]+'24' for k,values in sequences.items() if k.endswith(':entry=09')
            and any(r['arguments'][1]==65535 for r in values)}
        pitch_entries=sorted(k for k in sequences if is_engine_pitch(k,continuous))
    return dict(through_frame=through_frame,engine_pitch_entries=sorted(pitch_entries),
        engine_targets=targets(scoped_rows(rows,through_frame)),
        driver_sequences={k:dict(count=len(v),sha256=digest(v)) for k,v in sequences.items() if k not in pitch_entries})


def compare(folder,actual_folder,through_frame=None):
    arows,_=sealed_original(folder);brows=read_rows(Path(actual_folder)/'sound.jsonl')
    left,right=driver_sequences(arows,True,through_frame),driver_sequences(brows,False,through_frame)
    a,b=targets(scoped_rows(arows,through_frame)),targets(scoped_rows(brows,through_frame))
    report=dict(engine_targets=dict(original_count=len(a),port_count=len(b),
        agrees=bool(a) and a==b,first_difference=first_difference(a,b)),
        initialization=dict(original=context(arows),port=context(brows)),driver_sequences={})
    continuous=set()
    for key,items in left.items():
        if key.endswith(':entry=09') and any(r['arguments'][1]==65535 for r in items):
            continuous.add(key[:-2]+'24')
    for key in sorted(left.keys()|right.keys()):
        a,b=left[key],right[key]
        engine_pitch=is_engine_pitch(key,continuous)
        report['driver_sequences'][key]=dict(original_count=len(a),port_count=len(b),
            agrees=bool(a) and bool(b) and (report['engine_targets']['agrees'] if engine_pitch else a==b),
            comparison_rule='engine-targets' if engine_pitch else 'ordered-requests',
            raw_requests_agree=a==b,first_difference=first_difference(a,b),
            resynchronised=resync(a,b) if a!=b else [])
    report['comparison_rule']='exact engine targets plus ordered per-driver/resource/voice/entry content; raw cross-entry ISR interleaving remains diagnostic'
    report['through_frame']=through_frame
    report['agrees']=report['engine_targets']['agrees'] and all(r['agrees'] for r in report['driver_sequences'].values())
    return report


def is_engine_pitch(key,continuous=None):
    resource,_,entry=key.partition(':voice=')
    return resource.startswith(('ADENG','PCENG')) and resource.endswith('.VCE') and entry.endswith(':entry=24') and (continuous is None or key in continuous)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('original',type=Path);p.add_argument('port',type=Path)
    p.add_argument('--out',type=Path,required=True);p.add_argument('--through-frame',type=int)
    a=p.parse_args();report=compare(a.original,a.port,a.through_frame)
    a.out.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(json.dumps({k:v for k,v in report.items() if k!='driver_sequences'},indent=2))
    print('Differing sequences:',[k for k,v in report['driver_sequences'].items() if not v['agrees']])
    return 0 if report['agrees'] else 1

if __name__=='__main__':raise SystemExit(main())
