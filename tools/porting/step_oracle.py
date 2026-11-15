"""Run a bounded race with original-derived boundary observations by game step.

The original and port execute the same semantic input script. Capture requests
are not expectations; only sealed DOSBox-X output may become a reference.
"""
from __future__ import annotations
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT))

def race_script(path):
    events=[]
    def key(phase,at,scans):events.append(dict(at=at,phase=phase,channel='dos.keyboard.scancodes',payload=scans))
    key(1,0,[28,156]);key(2,0,[28,156]);key(2,1,[28,156]);key(3,0,[28,156])
    for i,scan in enumerate([25,19,18,47,23,18,17,23,49,34,28]):key(4,i,[scan,scan|128])
    key(5,0,[72])
    for at,scans in [(80,[200,75]),(100,[203,77]),(120,[205,72,30]),(121,[158]),
                     (150,[44]),(151,[172]),(160,[200,80]),(180,[208])]:key(6,at,scans)
    script=dict(format='stunts-step-input-v1',anchor_tick=0,events=events)
    path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes((json.dumps(script,indent=2)+'\n').encode())
    return script

def run_port(folder,script,audio,frames,timeout=120,*,capture_frames=False):
    folder=folder.resolve();folder.mkdir(parents=True,exist_ok=False)
    shutil.copytree(ROOT/'build/sdl3/runtime/assets',folder/'assets')
    (folder/'config.json').write_text(json.dumps(dict(manual_word_check=True,renderer='faithful',frame_rate=20,detail_level=0,drawing_path='dirty_rects')))
    env=os.environ.copy();env.update(SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy')
    env['PATH']=os.pathsep.join(['C:/msys64/mingw32/bin','C:/tools/sdl3-3.4.16-i686/bin',env.get('PATH','')])
    command=[str(ROOT/'build/sdl3/stunts.exe'),f'--assets={folder/"assets"}',f'--config={folder/"config.json"}',
        f'--diagnostics-dir={folder/"diagnostics"}',f'--trace={folder/"trace.jsonl"}',
        f'--boundary-trace={folder/"boundaries.jsonl"}',f'--sound-requests={folder/"sound.jsonl"}',
        f'--input-script={script.resolve()}',f'--test-startup-seed={ROOT/"tests/fixtures/sdl3/startup-seed.json"}',
        '--seed=1','--clock=manual','--run-ticks=3000',f'--boundary-frames={frames}',f'--audio={audio}','--audio-output=none']
    if capture_frames:command.append(f'--capture-dir={folder/"frames"}')
    with (folder/'run.log').open('w') as log:
        result=subprocess.run(command,cwd=folder,env=env,stdout=log,stderr=subprocess.STDOUT,timeout=timeout)
    if result.returncode:raise RuntimeError(f'port exited {result.returncode}: {folder/"run.log"}')
    rows=[json.loads(line) for line in (folder/'trace.jsonl').read_text().splitlines()]
    if not any(r.get('reason')=='requested game frame reached' for r in rows):
        raise RuntimeError(f'port did not reach game frame {frames}: {folder}')
    return folder

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out',type=Path,required=True);p.add_argument('--audio',choices=['ad15','pc15'],default='ad15')
    p.add_argument('--frames',type=int,default=200);p.add_argument('--dosbox',type=Path)
    p.add_argument('--port-only',action='store_true')
    p.add_argument('--capture-frames',action='store_true',help='diagnostic full port FBRs; never reference input')
    a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=False)
    script=out/'input.json';race_script(script)
    if not a.port_only:
        if not a.dosbox:p.error('--dosbox required for original capture')
        from tools.porting.original_tick_oracle import capture
        capture(argparse.Namespace(output=out/'original',script=script,dosbox=a.dosbox,ticks=3000,audio=a.audio,
            cycles=30000,timeout=180,frames=a.frames,boundaries=True))
    run_port(out/'port',script,a.audio,a.frames,capture_frames=a.capture_frames)
    print(out)

if __name__=='__main__':main()
