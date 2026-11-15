"""Build Stunts' observer in a private DOSBox-X copy, reusing pinned VS objects.

The installed SimAnt acceptance tree supplies keyboard/mouse scripting and the
normal-core hook only. Its ideal-machine patches are deliberately NOT applied.
No installed file is written. See README.md for prerequisites and provenance.
"""
from pathlib import Path
import argparse
import difflib
import hashlib
import json
import os
import re
import shutil
import subprocess
import time

HERE=Path(__file__).resolve().parent
BASE=Path('C:/tools/dosbox-x-2026.08.31-simant-acceptance-observation-src')
ROOT=HERE.parents[2]

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def build(out,rebuild=False):
    out=out.resolve();out.mkdir(parents=True,exist_ok=rebuild)
    pins=json.loads((HERE/'source-pins.json').read_text())
    for relative,expected in pins.items():
        if sha(BASE/relative)!=expected:raise ValueError('Pinned DOSBox source differs: '+relative)
    source=out/'source'
    # A real private source copy; cached installed objects are read-only inputs.
    for name in ('src','include'):
        shutil.copytree(BASE/name,source/name,dirs_exist_ok=rebuild)
    changes={
        'src/dosbox.cpp':[
            ('void DOSBOX_AcceptanceRecordIO(uint32_t port, uint32_t value,', 'static bool stunts_enabled=false;\nstatic void StuntsIO(uint32_t,uint32_t,bool);\nvoid DOSBOX_AcceptanceRecordIO(uint32_t port, uint32_t value,'),
            ('                               uint32_t width, bool output) {', '                               uint32_t width, bool output) {\n    StuntsIO(port,value,output);'),
            ('DOSBOX_AcceptanceIOWatchArmed = any_open;', 'DOSBOX_AcceptanceIOWatchArmed = any_open || stunts_enabled;'),
            ('void DOSBOX_AcceptanceOnInstruction(void) {', '#include "stunts_observer.inc"\nvoid DOSBOX_AcceptanceOnInstruction(void) {\n    StuntsInstruction();'),
            ('DOSBOX_AcceptanceExecArmed = still_armed;', 'DOSBOX_AcceptanceExecArmed = still_armed || stunts_enabled;'),
            ('void DOSBOX_AcceptanceStart(void) {', 'void DOSBOX_AcceptanceStart(void) {\n    StuntsInit();')],
        'src/ints/mouse.cpp':[
            ('void Mouse_AcceptanceMoveMickeys(int32_t dx, int32_t dy) {',
             'void Mouse_StuntsAcceptancePosition(uint16_t x,uint16_t y,uint8_t buttons) {\n'
             '    // DOS input channel, independent of PS/2 AUX host capture.\n'
             '    mouse.x=float((std::max)(mouse.min_x,(std::min)(mouse.max_x,int16_t(x))));\n'
             '    mouse.y=float((std::max)(mouse.min_y,(std::min)(mouse.max_y,int16_t(y))));\n'
             '    mouse.buttons=buttons&7;\n}\n'
             'void Mouse_AcceptanceMoveMickeys(int32_t dx, int32_t dy) {')],
        'src/cpu/core_normal.cpp':[
            ('#include "acceptance.h"','#include "acceptance.h"\nextern bool StuntsBoundaryYield();'),
            ('\t\t\tDOSBOX_AcceptanceOnInstruction();','\t\t\tDOSBOX_AcceptanceOnInstruction();\n\t\tif (StuntsBoundaryYield()) return CBRET_NONE;')],
    }
    inputs={}
    for relative,replacements in changes.items():
        path=source/relative;old=path.read_text(encoding='utf-8');text=old
        for before,after in replacements:
            if text.count(before)!=1:raise ValueError('Nonunique patch anchor: '+before)
            text=text.replace(before,after)
        patch=''.join(difflib.unified_diff(old.splitlines(True),text.splitlines(True),fromfile=relative,tofile=relative))
        expected=HERE/(Path(relative).name+'.patch')
        if expected.exists() and expected.read_text(encoding='utf-8')!=patch:
            raise ValueError('Readable patch differs from generator: '+str(expected))
        if not expected.exists():raise ValueError('Missing readable patch: '+str(expected))
        path.write_text(text,encoding='utf-8')
        inputs[str(BASE/relative)]=sha(BASE/relative)
    shutil.copyfile(HERE/'stunts_observer.inc',source/'src/stunts_observer.inc')
    shutil.copyfile(HERE/'stunts_boundary.inc',source/'src/stunts_boundary.inc')
    obj=BASE/'obj/dosbox-x/x64/Release SDL2';logs=obj/'dosbox-x.tlog'
    clines=(logs/'CL.command.1.tlog').read_text(encoding='utf-16').splitlines()
    jobs={}
    for i,line in enumerate(clines[:-1]):
        if not line.startswith('^') or Path(line[1:]).name.lower() not in ('dosbox.cpp','mouse.cpp','core_normal.cpp'):continue
        stem=Path(line[1:]).stem.lower()
        relative={'dosbox':'src/dosbox.cpp','mouse':'src/ints/mouse.cpp','core_normal':'src/cpu/core_normal.cpp'}[stem]
        args=clines[i+1]
        args=re.sub(r'/Fo"[^"]*"',lambda m:'/Fo"'+str(out/(stem+'.obj'))+'"',args,flags=re.I)
        args=re.sub(r'/Fd"[^"]*"',lambda m:'/Fd"'+str(out/(stem+'.pdb'))+'"',args,flags=re.I)
        args=args[:args.upper().rfind(' C:\\TOOLS\\')]
        args='/std:c++17 /I"'+str(source/'include')+'" '+args+' "'+str(source/relative)+'"'
        (out/(stem+'.rsp')).write_text(args);jobs[stem]=relative
    if set(jobs)!={'dosbox','mouse','core_normal'}:raise ValueError('Missing installed compiler recipe')
    link=(logs/'link.command.1.tlog').read_text(encoding='utf-16').splitlines()
    start=max(i for i,l in enumerate(link) if l.startswith('^'));link='\n'.join(link[start+1:])
    for option,name in [('OUT','dosbox-x.exe'),('PDB','dosbox-x.pdb'),('IMPLIB','dosbox-x.lib'),('LTCGOUT','dosbox-x.iobj')]:
        link=re.sub('/'+option+r':"[^"]*"',lambda m:'/'+option+':"'+str(out/name)+'"',link,flags=re.I)
    link=re.sub(r'/NATVIS:\S+','',link,flags=re.I)
    for stem in jobs:
        link=re.sub(r'"[^"\n]*\\'+stem+r'\.OBJ"',lambda m:'"'+str(out/(stem+'.obj'))+'"',link,flags=re.I)
    (out/'link.rsp').write_text(link)
    vc=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    (out/'environment.cmd').write_text('@echo off\ncall "'+str(vc)+'" >nul\nset\n')
    env=dict(os.environ)
    for line in subprocess.check_output(['cmd','/c',str(out/'environment.cmd')],text=True).splitlines():
        if '=' in line:k,v=line.split('=',1);env[k]=v
    compiler=Path(env['VCToolsInstallDir'])/'bin/Hostx64/x64/cl.exe'
    started=time.perf_counter()
    commands=[(stem,[str(compiler),'@'+stem+'.rsp'],300) for stem in jobs]
    commands.append(('link',[str(compiler.with_name('link.exe')),'@link.rsp'],600))
    for name,command,timeout in commands:
        with (out/(name+'.log')).open('w') as log:
            run=subprocess.run(command,cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT,timeout=timeout)
        if run.returncode:raise RuntimeError((out/(name+'.log')).read_text()[-6000:])
    inputs.update({str(p):sha(p) for p in obj.glob('*.obj')})
    inputs.update({str(p):sha(p) for p in HERE.glob('*') if p.is_file()})
    receipt=dict(schema='stunts-dosbox-observer-build-v1',inputs=inputs,
        executable=str(out/'dosbox-x.exe'),sha256=sha(out/'dosbox-x.exe'),seconds=time.perf_counter()-started,
        clock='unmodified DOSBox-X PIT/PIC; tick counted at original Stunts INT8 entry',
        compiled_units=sorted(jobs.values()))
    (out/'observer-build.json').write_text(json.dumps(receipt,indent=2)+'\n')
    print(json.dumps({k:v for k,v in receipt.items() if k!='inputs'},indent=2))

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True)
    p.add_argument('--rebuild',action='store_true',help='reuse only this private build and its LTCG cache')
    a=p.parse_args();build(a.out,a.rebuild)
