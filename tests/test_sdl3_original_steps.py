"""Finite port-layer agreements anchored only to sealed original observations."""
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import shutil
import tempfile
import unittest
import zlib
from tests.test_sdl3_driving import ROOT,assert_build_ready
from tests.test_sdl3_input import SDL_ROOT,host_compiler
from tools.porting.compare_step_streams import port,digest,boundary,resync
from tools.porting.step_oracle import run_port

FIXTURES=ROOT/'tools/porting/step_oracle_cases'
WORK=ROOT/'build/tests/test_sdl3_original_steps'
shutil.rmtree(WORK,ignore_errors=True)
WORK.mkdir(parents=True,exist_ok=True)

SERVICE_PROBE=r'''
#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {uint16_t ax,bx,cx,dx,si,di,cflag;} Registers;
uint16_t mousehorscale=1;
int int86(int,void*,void*);
void port_guest_check_stop(void){}
void port_guest_notify_activity(void){}
void port_guest_unwind(const char *s){fprintf(stderr,"%s\n",s);abort();}
void port_stub_fail(const char *s){port_guest_unwind(s);}
void port_trace_input_keyboard(uint64_t a,uint64_t b,uint64_t c,const uint8_t *p,size_t n){}
void port_trace_input_mouse(uint64_t a,uint64_t b,uint64_t c,double u,double v,uint16_t buttons){}
int main(void){
 unsigned interrupt,selector,cx,dx;Registers regs;
 assert(SDL_Init(SDL_INIT_EVENTS));port_input_init();
 while(scanf("%u %u %u %u",&interrupt,&selector,&cx,&dx)==4){
   regs=(Registers){.ax=selector,.cx=cx,.dx=dx};
   int86(interrupt,&regs,&regs);
   uint16_t response[3]={0};
   if(interrupt==0x33 && selector==3){response[0]=regs.bx;response[1]=regs.cx;response[2]=regs.dx;}
   else if(selector==0 || interrupt==0x15){response[0]=regs.ax;response[1]=regs.bx;}
   for(unsigned i=0;i<3;++i)printf("%02x%02x",response[i]&255,response[i]>>8);
   puts("");
 }
 /* Reset must reset old bounds and buttons as well as center coordinates. */
 port_input_mouse_set_x_bounds(1,3);port_input_mouse_set_y_bounds(2,4);
 port_input_mouse_set_buttons(7);regs=(Registers){0};int86(0x33,&regs,&regs);
 regs.ax=3;int86(0x33,&regs,&regs);assert(regs.bx==0 && regs.cx==320 && regs.dx==100);
 port_input_mouse_set(319,199);regs.ax=3;int86(0x33,&regs,&regs);assert(regs.cx==638 && regs.dx==199);
 kb_init_interrupt();port_input_apply_dos_scancode(205);port_input_apply_dos_scancode(72);port_input_apply_dos_scancode(30);
 printf("keys %u %u\n",(uint16_t)normkeyi(),(uint16_t)0);
 assert(normkeyi()==0);
 /* Test text stays a producer; it does not widen the game's pending queue. */
 kb_init_interrupt();
 port_input_enable_test_auto_protection(1);assert(port_input_type_test_text("ab"));
 assert(testkey()=='a');assert(testkey()=='b');assert(testkey()=='\r');assert(testkey()==0);
 port_input_shutdown();SDL_Quit();return 0;
}
'''

class OriginalStepTests(unittest.TestCase):
    def test_captured_matrix_arguments_extend_the_machine_diffharness(self):
        from tests import test_matrix_machine_semantics as matrix
        compiler=matrix.compiler_path();env=matrix.compiler_environment(compiler)
        WORK.mkdir(parents=True,exist_ok=True)
        fixture=json.loads((FIXTURES/'race-ad15.json').read_text())
        cases=[];records=[]
        for row in fixture['matrix_cases']:
            starts=[0,8,24] if row['name']=='mat_vec' else [0,16,32];words=[0x5aa5]*64
            for i,r in enumerate(row['regions']):
                if r['alias']>=0:starts[i]=starts[r['alias']]+r['offset']//2
                if r['read']:
                    values=struct.unpack('<'+'H'*(len(r['before'])//4),bytes.fromhex(r['before']))
                    words[starts[i]:starts[i]+len(values)]=values
            cases.append((0 if row['name']=='mat_vec' else 1,*starts,words));records.append(row)
        self.assertEqual(len(cases),24)
        with tempfile.TemporaryDirectory(dir=WORK) as tmp:
            exe=Path(tmp)/'matrix.exe'
            result=subprocess.run([str(compiler),'-std=gnu11','-O0','-ffunction-sections','-fdata-sections',
                '-DPORT_BUILD=1','-include',str(ROOT/'tools/porting/host/compat.h'),'-I',str(ROOT/'port'),
                '-I',str(ROOT/'tools/porting/port_include'),'-I',str(ROOT/'tools/porting/host/include'),
                '-I',str(SDL_ROOT/'include'),str(ROOT/'port/matrix.c'),str(ROOT/'tests/matrix_machine_probe.c'),
                '-Wl,--gc-sections','-o',str(exe)],env=env,capture_output=True,text=True,timeout=45)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            observed=matrix.host_words(exe,env,cases);runner=matrix.RealModeRunner(matrix.OracleImage.load())
            for case,row,host in zip(cases,records,observed):
                mode,first,second,output,words=case
                expected=matrix.machine_words(runner,row['name'],mode,first,second,output,words)
                self.assertEqual(host,expected,'all 64 surrounding words, original routine versus production C')
                for start,region in zip((first,second,output),row['regions']):
                    data=struct.pack('<64H',*expected)[2*start:2*start+len(region['after'])//2]
                    self.assertEqual(data.hex(),region['after'],'isolated original versus captured original call')

    def test_all_active_race_calls_states_completed_frames_and_sound_targets(self):
        assert_build_ready();WORK.mkdir(parents=True,exist_ok=True)
        for audio in ('ad15','pc15'):
            with self.subTest(audio=audio),tempfile.TemporaryDirectory(dir=WORK) as tmp:
                fixture=json.loads((FIXTURES/f'completed-race-{audio}.json').read_text())
                script=FIXTURES/fixture['script'];seal=fixture['provenance']
                self.assertEqual(hashlib.sha256(script.read_bytes()).hexdigest(),seal['script_sha256'])
                self.assertEqual(hashlib.sha256((ROOT/'layout/oracle.lock.json').read_bytes()).hexdigest(),seal['oracle_lock_sha256'])
                self.assertEqual(hashlib.sha256((ROOT/f'assets/{audio.upper()}.DRV').read_bytes()).hexdigest(),seal['driver_sha256'])
                folder=run_port(Path(tmp)/'run',script,audio,fixture['frames']);actual=port(folder)
                self.assertEqual(actual['input'],fixture['input'])
                self.assertEqual(actual['states'],fixture['states'])
                self.assertEqual(len(actual['states']),200)
                expected={k:v for k,v in fixture['groups']['boundaries'].items() if k.startswith('1:')}
                observed={k:dict(count=len(v),sha256=digest(v)) for k,v in actual['boundaries'].items() if k.startswith('1:')}
                self.assertEqual(len(expected),400)
                self.assertEqual(observed,expected,'ordered calls, scalar returns, bounded reads/writes and aliases')
                self.assertFalse(any(k.startswith('0:1:') for k in actual['boundaries']),
                    'initial paint must precede the first update; no injected inactive simulation')
                # All completed frames, including the dashboard restoration at
                # frame 1. Intermediate frame-1 blits remain diagnostic only.
                completed={k:dict(count=len(v),sha256=digest(v)) for k,v in actual['completed_frames'].items() if k.startswith('1:')}
                self.assertEqual(len(completed),200)
                self.assertEqual(completed,{k:v for k,v in fixture['groups']['completed_frames'].items() if k.startswith('1:')})
                keys=[f'1:{frame}' for frame in range(2,201)]
                for key in keys:
                    self.assertEqual(dict(count=len(actual['frames'][key]),sha256=digest(actual['frames'][key])),
                        fixture['groups']['frames'][key],f'frame group {key}')
                from tools.porting.compare_tick_streams import read_rows
                from tools.porting.compare_sound_events import targets
                self.assertEqual(targets(read_rows(folder/'sound.jsonl')),fixture['engine_targets'])
                self.assertEqual(len(fixture['engine_targets']),200)
                from collections import defaultdict
                sequences=defaultdict(list)
                for group,values in actual['sound'].items():
                    if not group.startswith('1:'):continue
                    for r in values:
                        resource=(r['resource'] or {}).get('name','none')
                        sequences[f'{resource}:voice={r["voice"]}:entry={r["entry"]:02x}'].append(r)
                # The 24h smoothing ramp is an explicit open difference, not
                # accepted through content search or timing relabeling.
                pitch_entries=set(fixture['engine_pitch_entries'])
                self.assertTrue(pitch_entries)
                observed_sound={k:dict(count=len(v),sha256=digest(v)) for k,v in sequences.items() if k not in pitch_entries}
                expected_sound={k:v for k,v in fixture['sound_sequences'].items() if k not in pitch_entries}
                self.assertTrue(expected_sound)
                self.assertEqual(observed_sound,expected_sound,'ordered per voice/resource/entry; no cross-entry interleaving claim')
                for sparse in fixture['sparse_frames']:
                    payload=zlib.decompress(bytes.fromhex(sparse['zlib_hex']))
                    self.assertEqual(len(payload),64768)
                    self.assertEqual(hashlib.sha256(payload).hexdigest(),sparse['payload_sha256'])

    def test_mouse_reset_bios_result_and_keyboard_overwrite_against_original(self):
        WORK.mkdir(parents=True,exist_ok=True);fixture=json.loads((FIXTURES/'input-services.json').read_text())
        compiler=host_compiler();self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory(dir=WORK) as tmp:
            folder=Path(tmp);source=folder/'services.c';exe=folder/'services.exe';source.write_text(SERVICE_PROBE)
            # Compile the unchanged production INT adapter with its register
            # declaration; unrelated public audio dispatch roots need no stubs.
            platform=(ROOT/'port/platform.c').read_text()
            selected=platform[platform.index('typedef struct PortIntRegisters'):platform.index('\nint16_t criterr(')]
            unit=folder/'int86.c';unit.write_text('#include "port_runtime.h"\n#include <string.h>\n#include <stdio.h>\n'+selected)
            env=os.environ.copy();env['PATH']=os.pathsep.join((str(Path(compiler).parent),str(SDL_ROOT/'bin'),env.get('PATH','')))
            result=subprocess.run([compiler,'-std=gnu11','-O0','-ffunction-sections','-fdata-sections',
                '-Wno-unused-parameter','-I',str(ROOT/'port'),'-I',str(SDL_ROOT/'include'),str(source),
                str(ROOT/'port/input.c'),str(unit),'-Wl,--gc-sections',
                '-L',str(SDL_ROOT/'lib'),'-lSDL3','-o',str(exe)],env=env,capture_output=True,text=True,timeout=45)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            commands=''.join(f"{0x33 if r['name']=='int33' else 0x15} {' '.join(map(str,r['args']))}\n" for r in fixture['service_calls'])
            result=subprocess.run([str(exe)],input=commands,env=env,capture_output=True,text=True,timeout=5)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            expected=[r['regions'][0]['after'] for r in fixture['service_calls']]
            expected.append(f"keys {fixture['keyboard']['pending_result']} {fixture['keyboard']['next_result']}")
            self.assertEqual(result.stdout.splitlines(),expected)

    def test_output_only_initial_bytes_are_not_an_input_contract(self):
        left=dict(name='mat_vec',args=[None],result=None,regions=[dict(name='output',read=0,alias=-1,offset=0,before='ffff',after='1234')])
        right=json.loads(json.dumps(left));right['regions'][0]['before']='0000'
        self.assertEqual(boundary(left),boundary(right))
        right['regions'][0]['after']='5678'
        self.assertNotEqual(boundary(left),boundary(right))
        differences=resync([boundary(left),dict(name='next')],[boundary(right),dict(name='next')])
        self.assertEqual(len(differences),1)
        self.assertEqual(differences[0]['original_index'],0)

if __name__=='__main__':unittest.main()
