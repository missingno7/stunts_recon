"""Close static-audit candidates with original instruction/service evidence."""
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / 'build/python'), str(ROOT / 'tools/porting')]
from diffharness.oracle import OracleImage, dgroup_segment
from diffharness.emulator import RealModeRunner
from diffharness.model import RoutineCase, MemoryWrite, MemoryRegion
from unicorn.x86_const import (UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX,
                               UC_X86_REG_DX, UC_X86_REG_EFLAGS)
from semantic_audit import functions
from tests.test_sdl3_projection_math import host_compiler


def compile_probe(folder, text):
    cc = host_compiler()
    if not cc:
        raise AssertionError('native GCC required')
    source, exe = Path(folder) / 'probe.c', Path(folder) / 'probe.exe'
    source.write_text(text)
    env = dict(os.environ)
    env['PATH'] = str(Path(cc).parent) + os.pathsep + env.get('PATH', '')
    ran = subprocess.run([cc, '-std=c11', '-O2', str(source), '-o', str(exe)],
                         capture_output=True, text=True, env=env, timeout=30)
    if ran.returncode:
        raise AssertionError(ran.stderr)
    return exe, env


def joystick_probe_body():
    source = (ROOT / 'port/input.c').read_text()
    definitions = functions(source)
    state = source[source.index('typedef struct PortAbsentAxis'):source.index('static uint8_t s_key_down')]
    body = '\n'.join(definitions[n]['body'] for n in ('calibrate_absent_axis',
        'get_joy_flags', 'reset_joystick_selection', 'replay_axis_value'))
    return '#include <stdint.h>\n#include <stdio.h>\nuint8_t joystick_enabled;\n' + state + body


class StaticCandidateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.image = OracleImage.load()

    def test_int86_returns_original_ax_and_register_buffer_including_alias(self):
        body = functions((ROOT / 'port/platform.c').read_text())['int86']['body']
        text = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {uint16_t ax,bx,cx,dx,si,di,cflag;} PortIntRegisters;
static uint16_t mousehorscale=1;
static void port_input_reset_mouse(void){}
static void port_input_mouse_get(int16_t*x,int16_t*y,uint16_t*b){*x=160;*y=100;*b=0;}
static void port_input_mouse_set(int16_t x,int16_t y){}
static void port_input_mouse_set_x_bounds(int16_t x,int16_t y){}
static void port_input_mouse_set_y_bounds(int16_t x,int16_t y){}
static void port_stub_fail(const char*r){abort();}
''' + body + r'''
int main(int argc,char**argv){PortIntRegisters in={0},out={0};in.cflag=0x7777;
 in.ax=(uint16_t)strtoul(argv[1],0,0);int same=atoi(argv[2]);
 int ret=int86(in.ax==0xc201?0x15:0x33,&in,same?&in:&out);
 PortIntRegisters*p=same?&in:&out;
 printf("%d",ret);for(unsigned i=0;i<7;++i)printf(" %u",((uint16_t*)p)[i]);return 0;}
'''
        with tempfile.TemporaryDirectory(prefix='stunts-int86-contract-') as folder:
            exe, env = compile_probe(folder, text)
            for selector in (0, 3, 0xc201):
                for same in (False, True):
                    with self.subTest(selector=selector, alias=same):
                        def interrupt(machine):
                            if selector == 0:
                                machine.reg_write(UC_X86_REG_AX, 0xffff)
                                machine.reg_write(UC_X86_REG_BX, 3)
                            elif selector == 3:
                                machine.reg_write(UC_X86_REG_BX, 0)
                                machine.reg_write(UC_X86_REG_CX, 320)
                                machine.reg_write(UC_X86_REG_DX, 100)
                            else:
                                machine.reg_write(UC_X86_REG_AX, 1)
                                machine.reg_write(UC_X86_REG_BX, 0xaa)
                            machine.reg_write(UC_X86_REG_EFLAGS,
                                              machine.reg_read(UC_X86_REG_EFLAGS) & ~1)
                        out = 0x8000 if same else 0x8020
                        original = RealModeRunner(self.image, interrupt_handlers={
                            0x15 if selector == 0xc201 else 0x33: interrupt}).call(RoutineCase(
                                'MSC int86 scalar/union', '_int86',
                                args=(0x15 if selector == 0xc201 else 0x33, 0x8000, out),
                                memory=(MemoryWrite(dgroup_segment(), 0x8000,
                                                    struct.pack('<7H', selector, 0, 0, 0, 0, 0, 0x7777)),),
                                compare=(MemoryRegion('regs', dgroup_segment(), out, 14),)))
                        ax = original.registers['ax']
                        signed = ax - 65536 if ax & 0x8000 else ax
                        expected = [signed, *struct.unpack('<7H', original.memory['regs'])]
                        ran = subprocess.run([str(exe), str(selector), str(int(same))],
                            capture_output=True, text=True, env=env, timeout=5)
                        self.assertEqual(ran.returncode, 0, ran.stderr)
                        self.assertEqual(list(map(int, ran.stdout.split())), expected)

    def test_absent_joystick_calibration_reset_and_saved_axis_match_original(self):
        text = joystick_probe_body() + r'''
int main(void){for(int i=0;i<80;++i){
 if(i==0||i==7||i==35||i==70)reset_joystick_selection();
 joystick_enabled=i%11==9?2:1;
 int flags=get_joy_flags();int axis=replay_axis_value();
 printf("%u %d %d\n",joystick_enabled,flags,axis);}return 0;}
'''
        ds = dgroup_segment()
        start, end = 0x43a2, 0x43c8
        original_state = self.image.load_image[0x2b770 + start:0x2b770 + end]
        rows = []
        for i in range(80):
            writes = (MemoryWrite(ds, start, original_state), MemoryWrite(ds, 0x4690, b'\1'))
            span = (MemoryRegion('state', ds, start, end - start),)
            if i in (0, 7, 35, 70):
                result = RealModeRunner(self.image).call(RoutineCase('original reset', 133044,
                    memory=writes, compare=span + (MemoryRegion('enabled', ds, 0x4690, 1),)))
                self.assertEqual(result.memory['enabled'], b'\1')
                original_state = result.memory['state']
            enabled = 2 if i % 11 == 9 else 1
            result = RealModeRunner(self.image, io_ports={0x201: 0xff}, writable_ports={0x201}).call(
                RoutineCase('disconnected gameport', 'get_joy_flags',
                    memory=(MemoryWrite(ds, start, original_state),
                            MemoryWrite(ds, 0x4690, bytes([enabled]))), compare=span))
            original_state = result.memory['state']
            axis = RealModeRunner(self.image).call(RoutineCase('original saved analog axis', 133091,
                memory=(MemoryWrite(ds, start, original_state),))).registers['ax'] & 0xff
            rows.append([enabled, result.registers['ax'], axis - 256 if axis & 128 else axis])
        with tempfile.TemporaryDirectory(prefix='stunts-absent-joystick-') as folder:
            exe, env = compile_probe(folder, text)
            ran = subprocess.run([str(exe)], env=env, capture_output=True, text=True, timeout=5)
            self.assertEqual(ran.returncode, 0, ran.stderr)
            self.assertEqual([list(map(int, r.split())) for r in ran.stdout.splitlines()], rows)

    def test_no_device_dosbox_calibration_matches_native(self):
        from diffharness.dos_services import capture_no_joystick
        work = ROOT / 'build/workers/portaudit/round2'
        work.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='dos-nojoy-', dir=work) as folder:
            original = capture_no_joystick(Path(folder) / 'original')
            text = joystick_probe_body() + r'''
int main(void){reset_joystick_selection();for(int i=0;i<80;++i)
 printf("%u\n",(uint16_t)get_joy_flags());return 0;}
'''
            exe, env = compile_probe(folder, text)
            ran = subprocess.run([str(exe)], env=env, capture_output=True, text=True, timeout=5)
            self.assertEqual(ran.returncode, 0, ran.stderr)
            self.assertEqual(list(map(int, ran.stdout.split())), original['flags'])

    def test_replay_playback_prefix_does_not_sample_live_joystick(self):
        original_source = (ROOT / 'game/race.c').read_text()
        body = functions(original_source)['moveeverything']['body']
        prefix = body[:body.index('    if (sigframe != 0 ||')]
        text = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define I16 int16_t
#define far
static int16_t gm_playmode=2,tmr2=10,sigframe,inrepflg;
static struct {int16_t game_recordedframes;} globalgamesettings={20};
static void myupdatesfx(void){abort();}
''' + prefix + '\nrecord_input: abort();\n}\n' + r'''
int main(void){moveeverything(0);printf("%d",tmr2);return 0;}
'''
        data = json.loads((ROOT / 'layout/data-symbols.json').read_text())['symbols']
        def write(name, value, add=0, size=2):
            return MemoryWrite(dgroup_segment(), data[name]['load_address'] - 0x2b770 + add,
                               value.to_bytes(size, 'little'))
        for enabled in (0, 1, 2, 255):
            original = RealModeRunner(self.image).call(RoutineCase('joystick replay bypass', 75416,
                args=(0,), memory=(write('_gm_playmode', 2), write('_tmr2', 10),
                    write('_globalgamesettings', 20, 24), write('_joystick_enabled', enabled, size=1)),
                compare=(MemoryRegion('counter', dgroup_segment(), data['_tmr2']['load_address'] - 0x2b770, 2),)))
            self.assertEqual(original.memory['counter'], b'\x0b\0')
        with tempfile.TemporaryDirectory(prefix='stunts-replay-prefix-') as folder:
            exe, env = compile_probe(folder, text)
            ran = subprocess.run([str(exe)], env=env, capture_output=True, text=True, timeout=5)
            self.assertEqual(ran.returncode, 0, ran.stderr)
            self.assertEqual(int(ran.stdout), 11)

    def test_int86_call_inventory_covers_every_retained_source_call(self):
        from semantic_mapping import int86_call_sites
        calls = int86_call_sites(ROOT)
        self.assertEqual(len(calls), 12)
        self.assertEqual({r['source'] for r in calls}, {'lib/mouse.c'})
        self.assertTrue(all(r['scalar_use'] == 'discarded' for r in calls))

    def test_correspondence_accounts_for_every_row_and_rejects_invented_helpers(self):
        from semantic_mapping import port_correspondence, build_mapping
        layer = json.loads((ROOT / 'tools/porting/port-layer.json').read_text())
        semantic = dict(historical_mapping=build_mapping(ROOT, definitions={}), functions=[])
        mapped = port_correspondence(ROOT, layer, semantic)
        self.assertEqual(set(mapped), {r['id'] for r in layer['rows']})
        for identifier, row in mapped.items():
            self.assertNotEqual(row['classification'], 'unmapped', identifier + ': ' + row['reason'])
            self.assertTrue(row['reason'], identifier)
            if row['classification'] == 'mapped':
                self.assertTrue(row['targets'], identifier)
                for target in row['targets']:
                    size = (ROOT / target['asset']).stat().st_size if target.get('asset') else len(self.image.load_image)
                    self.assertTrue(0 <= target['start'] < target['end'] <= size, identifier)
            else:
                self.assertTrue(row['class4'] and row['class6'], identifier)
        fake = dict(id='native_helper:invented', kind='native_helper', name='invented',
                    portable=[dict(source='port/config.c', name='invented')])
        self.assertEqual(port_correspondence(ROOT, dict(rows=[fake]), semantic)[fake['id']]['classification'], 'unmapped')
        arithmetic = mapped['native_helper:tools/porting/port_include/seg007_arith.h::stunts_div_u32_u16']
        self.assertTrue(any(t.get('public') == '_audio_op_unk2' for t in arithmetic['targets']))

    def test_save_create_cleanup_matches_real_dosbox_filesystem_effects(self):
        from diffharness.dos_services import capture
        from tests.test_sdl3_input import SDL_ROOT
        work = ROOT / 'build/workers/portaudit/round2'
        work.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='dos-cleanup-', dir=work) as folder:
            cases = {r['case']: r for r in capture(Path(folder) / 'original')['cases']}
            text = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
static int failure;
static FILE *audit_fopen(const char*p,const char*m){
 if(failure>=4 && !strcmp(m,"wb")){errno=failure==4?EMFILE:EACCES;return NULL;}
 return fopen(p,m);}
static size_t audit_fwrite(const void*p,size_t s,size_t n,FILE*f){
 if(failure==1)return 0;return fwrite(p,s,n,f);}
#define fopen audit_fopen
#define fwrite audit_fwrite
#include "''' + (ROOT / 'port/file.c').as_posix() + r'''"
#undef fopen
#undef fwrite
int port_memory_extent(const void*p,size_t*s){return 0;}
void *port_memory_alloc(size_t n,const char *s,PortFarPtr *p){abort();return NULL;}
void port_memory_free(void*p){abort();}
void abortmessage(const char *s,...){exit(80);}
int main(int argc,char**argv){failure=atoi(argv[2]);
 if(!port_runtime_set_save_root(argv[1]))return 1;
 printf("%d\n",savefilez(argv[3],"abc",3));return 0;}
'''
            cc = host_compiler()
            self.assertIsNotNone(cc)
            src, exe = Path(folder) / 'native.c', Path(folder) / 'native.exe'
            src.write_text(text)
            env = dict(os.environ)
            env['PATH'] = os.pathsep.join([str(Path(cc).parent), str(SDL_ROOT / 'bin'), env.get('PATH', '')])
            built = subprocess.run([cc, '-std=c11', '-O2', '-ffunction-sections', '-fdata-sections',
                '-I', str(ROOT / 'port'), '-I', str(SDL_ROOT / 'include'), str(src),
                '-Wl,--gc-sections', '-L', str(SDL_ROOT / 'lib'), '-lSDL3', '-o', str(exe)],
                env=env, capture_output=True, text=True, timeout=30)
            self.assertEqual(built.returncode, 0, built.stderr)
            for name, failure, filename in [('readonly', 5, 'AUDIT.TRK'),
                    ('directory', 0, 'AUDIT.TRK'),
                    ('bad-path', 0, 'BAD\\AUDIT.TRK'), ('handles-full', 4, 'AUDIT.TRK'),
                    ('create-injected-5', 5, 'AUDIT.TRK'), ('disk-full-empty', 1, 'AUDIT.TRK'),
                    ('disk-full-existing', 0, 'AUDIT.TRK')]:
                with self.subTest(case=name):
                    save = Path(folder) / ('native-' + name)
                    save.mkdir()
                    target = save / 'AUDIT.TRK'
                    if name == 'directory':
                        target.mkdir()
                    else:
                        target.write_bytes(b'' if name == 'disk-full-empty' else b'OLD')
                    if name == 'readonly':
                        target.chmod(0o444)
                    try:
                        ran = subprocess.run([str(exe), str(save), str(failure), filename],
                            env=env, capture_output=True, text=True, timeout=5)
                        self.assertEqual(ran.returncode, 0, ran.stderr)
                        self.assertEqual(int(ran.stdout), cases[name]['result'])
                        self.assertEqual(target.read_bytes().hex() if target.is_file() else None,
                                         cases[name]['final_hex'])
                        kind = 'directory' if target.is_dir() else 'file' if target.is_file() else 'absent'
                        self.assertEqual(kind, cases[name]['final_kind'])
                    finally:
                        if target.is_file(): target.chmod(0o666)
            # Clear only our fixture's read-only host file before temp cleanup.
            (Path(folder) / 'original/readonly/AUDIT.TRK').chmod(0o666)


if __name__ == '__main__':
    unittest.main()
