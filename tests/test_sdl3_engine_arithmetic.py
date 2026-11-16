"""Engine delta: execute the original divide window including its MSC helpers."""
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT/'port'), str(ROOT/'tools'), str(ROOT/'build/python')]
from build import adapt_gameplay_word_arithmetic
from tests.test_sdl3_word_arithmetic import extract_function, host_compiler
from tools.porting.diffharness.oracle import OracleImage, SymbolMap
from tools.porting.diffharness.emulator import REGS, linear
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_CODE


def original_delta(image, delta, mass):
    machine = Uc(UC_ARCH_X86, UC_MODE_16)
    machine.mem_map(0, 0x100000)
    machine.mem_write(0x10100, image.relocated(0x1010))
    routine = SymbolMap().resolve('doengine')
    cs, ip = routine.far_at(0x1010)
    # load32330: push zero high word / mass; sign-extend delta; multiply25;
    # call unsigned divider at load32358; SAR AX,1 at32363.
    start, stop = 32330, 32365
    for reg, value in dict(cs=cs, ip=ip+start-routine.start, ds=0x8000,
                           ss=0x8000, sp=0xe000, bp=0xe100).items():
        machine.reg_write(REGS[reg], value)
    machine.mem_write(0x80000+0xe100+12, struct.pack('<H', 0x1000))
    machine.mem_write(0x81002, struct.pack('<H', mass))
    machine.mem_write(0x80000+0xe100-8, struct.pack('<h', delta))
    hit = []
    def hook(uc, address, size, data):
        if address == 0x10100+stop:
            hit.append(True); uc.emu_stop()
    machine.hook_add(UC_HOOK_CODE, hook)
    machine.emu_start(0x10100+start, 0, count=10000)
    if not hit: raise AssertionError('original divide window did not finish')
    return machine.reg_read(REGS['ax']) & 65535


class EngineArithmeticTests(unittest.TestCase):
    def test_negative_delta_matches_original_unsigned_divider(self):
        from tools.porting.project_headers import expand_project_headers
        source = expand_project_headers((ROOT/'game/carsim.c').read_text(), ROOT)
        adapted = adapt_gameplay_word_arithmetic(source)
        helper = '\n'.join(extract_function(adapted, name) for name in
                           ('port_game_s16_from_u16', 'port_game_engine_delta'))
        code = '#include <stdint.h>\n#include <stdio.h>\n'+helper+'''
int main(void) { int d; unsigned m; while(scanf("%d %u",&d,&m)==2)
    printf("%u\\n",(uint16_t)port_game_engine_delta(d,m)); return 0; }
'''
        vectors = [(d, m) for d in (-32768,-500,-43,-2,-1,0,1,43,500,32767)
                   for m in (1,15,1000,32767,32768,65535)]
        image = OracleImage.load(ROOT/'assets')
        expected = [original_delta(image, *v) for v in vectors]
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder); (path/'probe.c').write_text(code)
            env = os.environ.copy()
            env['PATH'] = str(Path(host_compiler()).parent)+os.pathsep+env.get('PATH','')
            subprocess.run([host_compiler(), '-std=c11', '-O2', str(path/'probe.c'),
                            '-o', str(path/'probe.exe')], check=True, env=env)
            run = subprocess.run([str(path/'probe.exe')], input=''.join(f'{d} {m}\n' for d,m in vectors),
                                 text=True, capture_output=True, check=True)
        self.assertEqual([int(s) for s in run.stdout.split()], expected)
        self.assertEqual(expected[vectors.index((-1,15))], 2183)


if __name__ == '__main__': unittest.main()
