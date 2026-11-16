"""Scoped original/port UI scenarios: keyed dialog visits, never host-time input."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT))
from tools.porting.original_tick_oracle import capture
from tools.porting.capture_storage import byte_size


class Script:
    def __init__(self):self.events=[];self.settings={}
    def event(self,phase,at,channel,payload,visit=1):
        self.events.append(dict(at=at,phase=phase,visit=visit,channel=channel,payload=payload))
    def key(self,phase,at,scan,visit=1,*,held=False):
        self.event(phase,at,'dos.keyboard.scancodes',[scan] if held else [scan,scan|128],visit)
    def mouse(self,phase,at,x,y,visit=1):
        self.event(phase,at,'dos.mouse.normalized',dict(u=x/320,v=y/200,buttons=0),visit)
    def point(self,phase,at,label,visit=1,stop=False,optional=False):
        self.event(phase,at,'oracle.checkpoint',dict(label=label,stop=int(stop),optional=optional),visit)
    def intro(self):
        self.key(18,0,28);self.mouse(2,0,160,20)
    def answer(self):
        self.key(3,0,28)
        for i,scan in enumerate([25,19,18,47,23,18,17,23,49,34,28]):self.key(4,i,scan)
    def text(self,visit,scans):
        for i,scan in enumerate([14]*8+scans+[28]):self.key(4,i,scan,visit)
    def write(self,path):
        path.write_bytes((json.dumps(dict(format='stunts-step-input-v2',anchor_tick=0,**self.settings,events=self.events),indent=2)+'\n').encode())


def script(name,path):
    s=Script()
    if name=='startup':
        s.point(18,0,'splash');s.point(18,0,'title',2);s.key(18,0,28,2)
        s.point(2,0,'menu',stop=True)
    elif name=='opponent':
        s.intro();s.mouse(2,0,40,144);s.key(2,0,28)
        s.point(8,0,'opponent-clock');s.mouse(8,0,160,20)
        for i,scan in enumerate([77,28,77,77,28,77,28]):s.key(8,i,scan)
        s.point(8,2,'opponent-selected')
        s.point(7,0,'opponent-car');s.key(7,0,28)
        s.point(2,0,'opponent-done',2,stop=True)
    elif name in ('selection','manual-crash','highscore'):
        s.intro();s.key(2,0,75);s.key(2,1,28)
        s.point(7,0,'car-countach')
        if name=='selection':
            for i,scan in enumerate([80,28,28,28]):s.key(7,i,scan)
            s.point(7,4,'car-lancia');s.key(7,4,72);s.key(7,5,28)
            s.key(2,0,77,2);s.key(2,1,28,2)
            s.point(9,0,'track-default',stop=True)
        else:
            for i,scan in enumerate([80,80,80,28,72,72,72,28]):s.key(7,i,scan)
            s.key(2,0,28,2);s.answer();s.key(5,0,72,held=True)
            for at,scan in [(40,30),(41,158),(90,30),(91,158),(150,30),(151,158)]:s.key(6,at,scan,held=True)
            s.point(6,100,'race-driving')
            if name=='highscore':
                s.settings['evaluation_finish_frames']=200
                s.point(4,0,'highscore-entry',2)
                for i,scan in enumerate([71]+[83]*20+[20,23,46,37,49,23,49,18,28]):s.key(4,i,scan,2)
                s.point(16,4294967295,'highscore-saved',stop=True)
            else:s.point(14,0,'race-result',stop=True)
    elif name in ('editor','file-error'):
        s.intro();s.key(2,0,77);s.key(2,1,28)
        s.point(9,0,'track-default');s.key(9,0,28)
        s.point(11,0,'load-track');s.key(11,0,32);s.key(11,1,28)
        s.point(9,1,'track-loaded');s.key(9,1,77);s.key(9,2,28)
        s.point(10,0,'editor-loaded');s.mouse(10,0,160,20)
        # Palette: select a road tile, place it, remove it, then save.
        for i,scan in enumerate([57,71,77,28,77,28,28]):s.key(10,i,scan)
        s.point(10,6,'piece-placed');s.point(10,7,'piece-removed')
        s.key(10,7,57)
        s.key(10,8,71)
        # Select the save command from the palette using its fixed row.
        for at in range(9,18):s.key(10,at,80)
        s.key(10,18,28)
        s.point(4,0,'save-dialog')
        s.text(1,[48,30,32,53,33,23,38,18] if name=='file-error' else [44,44,44,49,23,49,18])
        if name=='file-error':
            s.point(13,0,'save-error',7,stop=True)
            s.point(10,19,'accepted-bad-path',stop=True,optional=True)
        else:s.point(10,19,'editor-saved',stop=True)
    elif name=='replay':
        s.intro();s.key(2,0,28);s.answer();s.key(5,0,72,held=True)
        s.key(6,200,200,held=True);s.key(6,200,1)
        s.point(22,0,'replay-paused');s.mouse(22,0,170,164);s.key(22,0,28)
        s.point(22,0,'replay-seek',2);s.key(22,0,1,2)
        s.point(13,0,'pause-menu',4);s.mouse(13,0,0,0,4)
        for i,scan in enumerate([80,80,80,80,28]):s.key(13,i,scan,4)
        s.point(4,0,'replay-save-dialog',2);s.text(2,[31,10,19,18,25,38,30,21])
        s.point(24,4294967295,'replay-saved',stop=True)
    else:raise ValueError(name)
    s.write(path);return s


def run_port(folder,script,audio,ticks=4000):
    folder.mkdir(parents=True,exist_ok=False)
    shutil.copytree(ROOT/'build/sdl3/runtime/assets',folder/'assets',ignore=shutil.ignore_patterns('diagnostics'))
    (folder/'config.json').write_text(json.dumps(dict(manual_word_check=True,renderer='faithful',frame_rate=20,detail_level=0,drawing_path='dirty_rects')))
    env=os.environ.copy();env.update(SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy')
    env['PATH']=os.pathsep.join(['C:/msys64/mingw32/bin','C:/tools/sdl3-3.4.16-i686/bin',env.get('PATH','')])
    executable=Path(os.environ.get('STUNTS_TEST_EXECUTABLE',ROOT/'build/sdl3/stunts.exe'))
    command=[str(executable.resolve()),f'--assets={folder/"assets"}',f'--config={folder/"config.json"}',
        f'--diagnostics-dir={folder/"diagnostics"}',f'--trace={folder/"trace.jsonl"}',
        f'--boundary-trace={folder/"boundaries.jsonl"}',f'--sound-requests={folder/"sound.jsonl"}',
        f'--capture-dir={folder/"frames"}',f'--input-script={script}',
        f'--test-startup-seed={ROOT/"tests/fixtures/sdl3/startup-seed.json"}',
        '--seed=1','--clock=manual',f'--run-ticks={ticks}',f'--audio={audio}','--audio-output=none']
    with (folder/'run.log').open('w') as log:
        result=subprocess.run(command,cwd=folder,env=env,stdout=log,stderr=subprocess.STDOUT,timeout=120)
    (folder/'run-result.json').write_text(json.dumps(dict(returncode=result.returncode,command=command),indent=2)+'\n')
    if result.returncode:raise RuntimeError(f'port failed ({result.returncode}): {folder/"run.log"}')
    return folder


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('scenario',choices=['startup','opponent','selection','editor','file-error','manual-crash','replay','highscore'])
    p.add_argument('--out',type=Path,help='run directory; the default is replaced on each run')
    p.add_argument('--dosbox',type=Path);p.add_argument('--script',type=Path);p.add_argument('--original-only',action='store_true');p.add_argument('--port-only',action='store_true')
    p.add_argument('--ticks',type=int,default=4000);a=p.parse_args()
    if a.out is None:
        out=ROOT/'build'/'port-captures'/'scenario'
        shutil.rmtree(out,ignore_errors=True)
        out.mkdir(parents=True,exist_ok=True)
    else:
        out=a.out.resolve();out.mkdir(parents=True,exist_ok=False)
    path=out/'input.json'
    if a.script:shutil.copyfile(a.script,path)
    else:script(a.scenario,path)
    if not a.port_only:
        if not a.dosbox:p.error('--dosbox is required')
        capture(argparse.Namespace(output=out/'original',script=path,dosbox=a.dosbox,ticks=a.ticks,audio=a.audio,
            cycles=30000,timeout=180,frames=0,boundaries=True,graphics=False))
    if not a.original_only:run_port(out/'port',path,a.audio,a.ticks)
    print(f"scenario run size: {byte_size(out)} bytes")
    print(out)

if __name__=='__main__':main()
