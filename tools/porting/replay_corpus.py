"""Single-replay original/port experiment; never launches a corpus sweep.

Community files are passive local data. Expectations are sealed original captures.
"""
from __future__ import annotations
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import time
import re
from itertools import zip_longest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools.porting.capture_storage import byte_size, discard_raw_frames
from tools.porting.scenario_oracle import Script


def metadata(path):
    raw = Path(path).read_bytes()
    if len(raw) < 1828:
        raise ValueError('truncated replay header/embedded track')
    frames = struct.unpack_from('<H', raw, 24)[0]
    if not 1 <= frames <= 12000 or len(raw) != 1828 + frames:
        raise ValueError('invalid replay input extent')
    return dict(name=Path(path).name, sha256=hashlib.sha256(raw).hexdigest(),
                bytes=len(raw), frames=frames, rate=raw[22],
                player_car=raw[:4].decode('ascii', errors='replace'),
                opponent=raw[6], opponent_car=raw[7:11].decode('ascii',errors='replace'),
                track_sha256=hashlib.sha256(raw[26:1828]).hexdigest())


def replay_script(path):
    s = Script()
    s.intro()
    s.mouse(2, 0, 285, 140); s.key(2, 0, 28)
    s.mouse(13, 0, 0, 0, 3)
    for i, scan in enumerate([80, 80, 80, 28]): s.key(13, i, scan, 3)
    # Only CORPUS.RPL has this initial in the isolated asset directory.
    s.key(11, 0, 46); s.key(11, 1, 28)
    s.answer()
    # Loaded replay starts paused. Select Play with the keyboard, then move
    # the pointer away from the toolbar before the next poll.
    s.mouse(22, 0, 170, 164); s.key(22, 0, 28)
    s.mouse(22, 0, 253, 164, 2); s.key(22, 0, 28, 2)
    s.mouse(22, 1, 0, 0, 2)
    s.write(path)


def rows(path):
    opener = gzip.open if str(path).endswith('.gz') else open
    with opener(path, 'rt', encoding='utf-8') as stream:
        for line in stream:
            if line.strip(): yield json.loads(line)


def stream_path(folder,name):
    path=folder/name
    return path if path.exists() else folder/(name+'.gz')


def discard_asset_copy(folder):
    """Only this run's resolved child, never the repository's input assets."""
    root=folder.resolve();target=(root/'assets').resolve()
    if not target.exists():return
    if target.parent!=root or target.name!='assets' or target==ROOT/'assets':
        raise ValueError('asset copy resolves outside the capture directory')
    shutil.rmtree(target)


def pack_port(folder,*,keep_raw=False):
    if not any((folder/name).exists() for name in ('trace.jsonl','sound.jsonl','boundaries.jsonl')):
        return json.loads((folder/'packing.json').read_text())
    start=time.perf_counter()
    if (folder/'assets/CORPUS.RPL').exists():
        identity=dict(replay_sha256=sha(folder/'assets/CORPUS.RPL'))
        script=folder/'input-script.json'
        if not script.exists():script=folder.parent/'input.json'
        if script.exists():identity['script_sha256']=sha(script)
        (folder/'inputs.json').write_text(json.dumps(identity,indent=2)+'\n')
    for name in ('trace.jsonl','sound.jsonl','boundaries.jsonl'):
        source=folder/name
        if not source.exists():continue
        with source.open('rb') as raw,gzip.open(folder/(name+'.gz'),'wb',compresslevel=6) as target:
            shutil.copyfileobj(raw,target)
        if not keep_raw:source.unlink()
    if not keep_raw:discard_asset_copy(folder)
    result=dict(seconds=time.perf_counter()-start,bytes=byte_size(folder))
    (folder/'packing.json').write_text(json.dumps(result,indent=2)+'\n')
    return result


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def pack(capture, info, *, keep_raw=False):
    """Seal before packing. Partial timeouts remain explicitly incomplete evidence."""
    if (capture/'reference.json.gz').exists():raise ValueError('refusing to overwrite an expectation')
    receipt = json.loads((capture/'receipt.json').read_text())
    lock = json.loads((ROOT/'layout/oracle.lock.json').read_text())
    if (receipt['oracle_lock_sha256'] != sha(ROOT/'layout/oracle.lock.json') or
        receipt['executable_sha256'] != lock['unpacked']['sha256']):
        raise ValueError('expectation is not the locked original')
    for name, key in [('original.exe','executable_sha256'), ('original.jsonl','capture_sha256'),
                      ('original.frames','frames_sha256'), ('input-script.json','script_sha256'),
                      ('bindings.json','bindings_sha256'), ('boundaries.jsonl','boundary_capture_sha256'),
                      ('boundary-bindings.json','boundary_bindings_sha256')]:
        if sha(capture/name) != receipt[key]: raise ValueError('unsealed capture: '+name)
    if sha(capture/'assets/CORPUS.RPL') != info['sha256']:
        raise ValueError('captured replay differs')
    start = time.perf_counter(); states=[]; epoch=0; previous=None; last=None
    first_us=None; epoch_us=[]
    # A killed observer can leave one final partial JSON line. Preserve it in
    # the raw archive and reject it for complete captures; never fabricate data.
    with (capture/'original.jsonl').open() as stream, gzip.open(capture/'states.bin.gz','wb') as payload:
        for line in stream:
            try: row=json.loads(line)
            except json.JSONDecodeError:
                if receipt['returncode']==0:raise
                continue
            last=row
            if row['kind']!='state':continue
            raw=bytes.fromhex(row['bytes_hex'])
            if len(raw)!=1120:raise ValueError('bad original state span')
            frame=struct.unpack_from('<H',raw,320)[0]
            if previous is None or frame<=previous:
                epoch+=1;epoch_us.append(row['wall_us'])
            previous=frame
            if first_us is None:first_us=row['wall_us']
            payload.write(raw)
            states.append([epoch,frame,hashlib.sha256(raw).hexdigest()])
    for name in ('original.jsonl','boundaries.jsonl'):
        with (capture/name).open('rb') as source, gzip.open(capture/(name+'.gz'),'wb',compresslevel=6) as target:
            shutil.copyfileobj(source,target)
    complete = (receipt['returncode']==0 and last.get('kind')=='end' and
                last.get('terminal')=='frame-limit' and len(states)==info['frames']*2)
    index=dict(schema='stunts-original-replay-v1',replay=info,provenance=receipt,
               complete=complete,terminal=last.get('terminal','host-timeout'),states=states,
               timing=dict(process_seconds=receipt['seconds'],first_state_seconds=(first_us or 0)/1e6,
                           epoch_first_state_seconds=[v/1e6 for v in epoch_us]))
    with gzip.open(capture/'reference.json.gz','wt',encoding='utf-8') as stream:json.dump(index,stream)
    if not keep_raw:
        discard_raw_frames(capture)
        for name in ('original.jsonl','boundaries.jsonl'):(capture/name).unlink()
        discard_asset_copy(capture)
    index['timing']['packing_seconds']=time.perf_counter()-start
    (capture/'packing.json').write_text(json.dumps(dict(timing=index['timing'],states=len(states),
        complete=complete,bytes=byte_size(capture)),indent=2)+'\n')
    return index


def state_fields():
    """DOS field labels for diagnostics; equality always uses all 1120 bytes."""
    def layout(name):
        if name in ('VECTOR','VECTORLONG'):
            width=2 if name=='VECTOR' else 4
            return [(axis,i*width,width) for i,axis in enumerate(('x','y','z'))],3*width
        filename={'CARSTATE':'car.h','GAMESTATE':'gamestat.h'}[name]
        source=(ROOT/'include'/filename).read_text()
        body=source.split('struct '+name+' {',1)[1].split('};',1)[0]
        result=[];offset=0
        for declaration in body.split(';'):
            match=re.fullmatch(r'\s*(struct\s+\w+|(?:unsigned\s+)?(?:char|short|long))\s+(\w+)(?:\[(\d+)\])?\s*',declaration)
            if not match:continue
            kind,field,count=match.groups();count=int(count or 1)
            if kind.startswith('struct '):children,width=layout(kind.split()[1])
            else:
                width={'char':1,'short':2,'long':4}[kind.split()[-1]];children=[('',0,width)]
            for i in range(count):
                prefix=field+(f'[{i}]' if count>1 else '')
                result.extend((prefix+('.'+child if child else ''),offset+i*width+at,size)
                              for child,at,size in children)
            offset+=count*width
        return result,offset
    fields,size=layout('GAMESTATE')
    if size!=1120:raise ValueError('diagnostic field map is not DOS GAMESTATE')
    return fields


def snapshots(path):
    epoch=0;previous=None
    for row in rows(path):
        if row.get('event_type')!='state_snapshot':continue
        raw=bytes.fromhex(row['bytes_hex']);frame=struct.unpack_from('<H',raw,320)[0]
        if previous is None or frame<=previous:epoch+=1
        previous=frame
        yield (epoch,frame,raw)


def boundary_at(path,epoch,frame):
    current=1;previous=0;result=[]
    for row in rows(path):
        if row.get('kind')!='boundary' or row.get('scope')!=1:continue
        n=row['frame']
        if n<previous:current+=1
        previous=n
        if current==epoch and n==frame:result.append(row)
        if current>epoch or current==epoch and n>frame:break
    return result


def sound_summary(path,original,pitch_entries=None):
    from tools.porting.compare_sound_events import compact_sound
    # Raw requests remain in the sealed archive. The established contract
    # compares targets and ordered resource/voice/entry streams, not PIT epochs.
    sound=[];started=False;continuous=set()
    for r in rows(path):
        if r.get('kind')=='engine_start':started=True
        if r.get('kind')=='driver' and r.get('entry')==9:
            from tools.porting.compare_tick_streams import SCALARS
            args=[r['stack_u16'][i] for i in SCALARS[9]] if original else r['arguments_u16']
            resource=(r.get('resource') or {}).get('name','none')
            if args[1]==65535 and resource.startswith(('ADENG','PCENG')):
                continuous.add(f'{resource}:voice={r["voice"]}:entry=24')
        if started and r.get('kind') in ('engine_target','driver'):sound.append(r)
    return compact_sound(sound,original,None,sorted(continuous) if pitch_entries is None else pitch_entries)


def compare_reference(capture,port):
    from tools.porting.compare_step_streams import boundary,input_identity,digest
    start=time.perf_counter()
    with gzip.open(capture/'reference.json.gz','rt') as stream:reference=json.load(stream)
    if reference.get('schema')!='stunts-original-replay-v1':raise ValueError('unknown reference schema')
    if reference['provenance']['oracle_lock_sha256']!=sha(ROOT/'layout/oracle.lock.json'):
        raise ValueError('reference oracle identity differs')
    actual_replay=(sha(port/'assets/CORPUS.RPL') if (port/'assets/CORPUS.RPL').exists()
        else json.loads((port/'inputs.json').read_text())['replay_sha256'])
    if actual_replay!=reference['replay']['sha256']:
        raise ValueError('port replay differs from original input')
    first=None;count=0;actual_count=0
    with gzip.open(capture/'states.bin.gz','rb') as payload:
        for ordinal,pair in enumerate(zip_longest(reference['states'],snapshots(stream_path(port,'trace.jsonl'))),1):
            expected,actual=pair
            if actual is not None:actual_count+=1
            raw=payload.read(1120) if expected is not None else None
            if expected is not None and hashlib.sha256(raw).hexdigest()!=expected[2]:
                raise ValueError('packed state payload differs from its original index')
            good=expected is not None and actual is not None and expected[:2]==list(actual[:2]) and raw==actual[2]
            if good:count+=1
            elif first is None:
                first=dict(step=ordinal,original=expected[:2] if expected else None,
                           port=list(actual[:2]) if actual else None,fields=[])
                if expected and actual:
                    for name,at,size in state_fields():
                        if raw[at:at+size]!=actual[2][at:at+size]:
                            first['fields'].append(dict(field=name,offset=at,
                                original=raw[at:at+size].hex(),port=actual[2][at:at+size].hex()))
                    first['fields']=first['fields'][:12]
                    a=boundary_at(capture/'boundaries.jsonl.gz',expected[0],expected[1]-1)
                    b=boundary_at(stream_path(port,'boundaries.jsonl'),actual[0],actual[1]-1)
                    a=[boundary(r) for r in a];b=[boundary(r) for r in b]
                    known={digest(input_identity(r)):r for r in a}
                    first['boundary']=dict(original_calls=len(a),port_calls=len(b),
                        differing_outputs=[dict(original=known[digest(input_identity(r))],port=r)
                            for r in b if digest(input_identity(r)) in known and known[digest(input_identity(r))]!=r][:3],
                        first_ordered_difference=next((dict(original=x,port=y) for x,y in zip_longest(a,b) if x!=y),None))
    a=sound_summary(capture/'original.jsonl.gz',True)
    b=sound_summary(stream_path(port,'sound.jsonl'),False,a['engine_pitch_entries'])
    from tools.porting.compare_tick_streams import first_difference
    report=dict(schema='stunts-replay-comparison-v1',complete=reference['complete'],
        state_agrees=reference['complete'] and first is None,
        original_steps=len(reference['states']),port_steps=actual_count,agreeing_steps=count,
        first_difference=first,sound_agrees=a==b,original_sound=a,port_sound=b,
        first_sound_target_difference=first_difference(a['engine_targets'],b['engine_targets']),
        seconds=time.perf_counter()-start)
    report['raw_step_sound']=compare_sound_steps(capture/'original.jsonl.gz',stream_path(port,'sound.jsonl'))
    report['sound_projection_agrees']=report['sound_agrees']
    report['sound_agrees']=report['sound_projection_agrees'] and report['raw_step_sound']['agrees']
    report['seconds']=time.perf_counter()-start
    return report


def compare_sound_steps(original_path,port_path):
    """Raw typed requests by playback simulation frame/scope/source, no IRQ alignment."""
    from tools.porting.compare_tick_streams import SCALARS, first_difference
    from tools.porting.compare_step_streams import digest
    def collect(path,original):
        groups={};started=False
        for r in rows(path):
            if r.get('kind')=='engine_start':started=True
            if not started or r.get('kind')!='driver':continue
            key=f'{r["game_frame"]}:{r["scope"]}:{r["source"]}'
            entry=r['entry']
            value=dict(entry=entry,driver=r['driver'],voice=r['voice'],chunk=r['chunk'],
                       resource=r['resource'],payload=r['payload_hex'])
            value['arguments']=[r['stack_u16'][i] for i in SCALARS[entry]] if original else r['arguments_u16']
            value['result']=(r['return_ax']&255 if entry==0 else r['return_ax']
                if entry in (0x36,0x3c) else None) if original else r['return_u16']
            groups.setdefault(key,[]).append(value)
        return groups
    a=collect(original_path,True);b=collect(port_path,False)
    keys=sorted(a.keys()|b.keys(),key=lambda k:(int(k.split(':')[0]),k))
    bad=[k for k in keys if a.get(k)!=b.get(k)]
    key=bad[0] if bad else None
    return dict(agrees=bool(a) and not bad,original_groups=len(a),port_groups=len(b),
        differing_groups=len(bad),first_group=key,
        first_difference=first_difference(a.get(key,[]),b.get(key,[])) if key else None,
        original={k:dict(count=len(v),sha256=digest(v)) for k,v in a.items()},
        port={k:dict(count=len(v),sha256=digest(v)) for k,v in b.items()})


def run_port(out, script, replay, frames, ticks, timeout=900):
    out.mkdir(parents=True, exist_ok=False)
    shutil.copytree(ROOT/'build/sdl3/runtime/assets', out/'assets',
                    ignore=shutil.ignore_patterns('diagnostics'))
    shutil.copyfile(replay, out/'assets/CORPUS.RPL')
    shutil.copyfile(script,out/'input-script.json')
    (out/'config.json').write_text(json.dumps(dict(manual_word_check=True,
        renderer='faithful', frame_rate=20, detail_level=0, drawing_path='dirty_rects')))
    env = os.environ.copy()
    env.update(SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
    env['PATH'] = os.pathsep.join(['C:/msys64/mingw32/bin',
        'C:/tools/sdl3-3.4.16-i686/bin', env.get('PATH', '')])
    command = [str(ROOT/'build/sdl3/stunts.exe'), f'--assets={out/"assets"}',
        f'--config={out/"config.json"}', f'--diagnostics-dir={out/"diagnostics"}',
        f'--trace={out/"trace.jsonl"}', f'--boundary-trace={out/"boundaries.jsonl"}',
        f'--sound-requests={out/"sound.jsonl"}', f'--input-script={out/"input-script.json"}',
        f'--test-startup-seed={ROOT/"tests/fixtures/sdl3/startup-seed.json"}',
        '--seed=1', '--clock=manual', f'--run-ticks={ticks}',
        f'--stop-after-sim-steps={frames}', '--simulation-boundaries', '--audio=ad15', '--audio-output=none']
    identities=dict(executable_sha256=sha(ROOT/'build/sdl3/stunts.exe'),
                    replay_sha256=sha(replay),script_sha256=sha(script))
    start = time.perf_counter()
    with (out/'run.log').open('w') as log:
        result = subprocess.run(command, cwd=out, env=env, stdout=log,
                                stderr=subprocess.STDOUT, timeout=timeout)
    seconds = time.perf_counter()-start
    summary = dict(seconds=seconds, returncode=result.returncode,
                   states=sum(r.get('event_type')=='state_snapshot' for r in rows(out/'trace.jsonl')),
                   **identities)
    (out/'timing.json').write_text(json.dumps(summary, indent=2)+'\n')
    if result.returncode: raise RuntimeError(f'port failed: {out/"run.log"}')
    if summary['states']!=frames:raise RuntimeError(f'incomplete port playback: {summary}')
    return summary


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('replay', type=Path, nargs='?')
    p.add_argument('--out', type=Path)
    p.add_argument('--inventory', action='store_true', help='inspect passive replay data only')
    p.add_argument('--replay-dir', type=Path, help='local community input directory; never downloaded')
    p.add_argument('--dosbox', type=Path)
    p.add_argument('--port-only', action='store_true', help='menu diagnostic; no expectations')
    p.add_argument('--frames', type=int, help='diagnostic prefix, otherwise complete replay')
    p.add_argument('--timeout', type=float, default=900)
    p.add_argument('--keep-raw', action='store_true')
    p.add_argument('--finish-capture', action='store_true', help='pack/compare an existing single capture; never recapture')
    p.add_argument('--port-output',type=Path,help='native run directory (default: OUT/port)')
    a = p.parse_args()
    if a.inventory:
        paths=[ROOT/'assets/DEFAULT.RPL']
        if a.replay_dir and a.replay_dir.is_dir():paths+=sorted(a.replay_dir.glob('*.RPL'))
        elif a.replay_dir:print(f'Community replay directory absent; skipping: {a.replay_dir}',file=sys.stderr)
        records=[];unsupported=[]
        for path in paths:
            try:records.append(metadata(path))
            except ValueError as error:unsupported.append(dict(name=path.name,error=str(error),bytes=path.stat().st_size))
        print(json.dumps(dict(replays=len(records),input_steps=sum(r['frames'] for r in records),
            tracks=len({r['track_sha256'] for r in records}),cars=sorted({r['player_car'] for r in records}),
            opponents=sum(bool(r['opponent']) for r in records),records=records,
            unsupported=unsupported,total_files=len(paths)),indent=2))
        return
    if a.replay is None or a.out is None:p.error('one replay and --out required')
    if not 0 < a.timeout <= 900: p.error('original timeout must be in (0,900] seconds')
    out = a.out.resolve(); out.mkdir(parents=True, exist_ok=a.finish_capture)
    info = metadata(a.replay); frames = a.frames or info['frames']*2
    (out/'replay.json').write_text(json.dumps(info, indent=2)+'\n')
    script = out/'input.json'; replay_script(script)
    ticks = max(10000, frames*12+10000)
    original_seconds=0
    if not a.port_only and not a.finish_capture:
        if not a.dosbox: p.error('--dosbox required for an original capture')
        from tools.porting.original_tick_oracle import capture
        start = time.perf_counter()
        capture(argparse.Namespace(output=out/'original', script=script, dosbox=a.dosbox,
            ticks=ticks, audio='ad15', cycles=30000, timeout=a.timeout,
            frames=0, steps=frames, boundaries=True, graphics=False, replay=a.replay.resolve(),
            no_rasters=True, simulation_boundaries=True, allow_partial=True))
        original_seconds=time.perf_counter()-start
        print('original seconds:', original_seconds, flush=True)
    if not a.port_only and not (out/'original/reference.json.gz').exists():
        pack(out/'original',info,keep_raw=a.keep_raw)
    port=(a.port_output or out/'port').resolve()
    if not port.exists():
        print(json.dumps(run_port(port, script, a.replay.resolve(), frames, ticks)), flush=True)
    if not a.port_only:
        report=compare_reference(out/'original',port)
        (out/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
        print(json.dumps({k:report[k] for k in ('complete','state_agrees','original_steps','port_steps',
            'agreeing_steps','sound_agrees','seconds','first_sound_target_difference')}))
        pack_port(port,keep_raw=a.keep_raw)
        return 0 if report['state_agrees'] and report['sound_agrees'] else 1


if __name__ == '__main__': raise SystemExit(main())
