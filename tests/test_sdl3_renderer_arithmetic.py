"""The original renderer compares a signed word after doubling its cull limit."""
from pathlib import Path
import os
import random
import re
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "port"))
sys.path.insert(0, str(ROOT / "build/python"))
from game_abi import adapt_renderer_word_arithmetic
from tools.porting.diffharness.oracle import OracleImage
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import (UC_X86_REG_AX, UC_X86_REG_CX, UC_X86_REG_CS,
                               UC_X86_REG_EFLAGS)


class RendererArithmeticTests(unittest.TestCase):
    def test_adapter_changes_only_two_proven_renderer_expressions(self):
        source = (ROOT / "src/obj_seg006.c").read_text()
        adapted = adapt_renderer_word_arithmetic(source)
        self.assertEqual(adapted.count("port_renderer_double_word(ts->unk)"), 2)
        helper = re.search(r"/\* PORT_BUILD: locked SHL CX,1[^\n]*\*/\n"
                           r"static int16_t port_renderer_double_word[^}]+}\n\n",
                           adapted).group()
        restored = adapted.replace(helper, "").replace(
            "port_renderer_double_word(ts->unk)", "ts->unk * 2")
        self.assertEqual(restored, source)

    def test_generated_helper_and_signed_compare_match_locked_shl_cmp(self):
        image = OracleImage.load()
        code = bytes.fromhex("d1e13bc8")  # SHL CX,1; CMP CX,AX
        for offset in (86103, 86126):
            self.assertEqual(image.load_image[offset:offset + 4], code)
        source = adapt_renderer_word_arithmetic(
            (ROOT / "src/obj_seg006.c").read_text())
        helper = re.search(r"static int16_t port_renderer_double_word[^}]+}",
                           source).group()
        edges = (-32768, -32767, -16385, -16384, -1, 0, 1, 16383,
                 16384, 30000, 32766, 32767)
        cases = [(limit, distance) for limit in edges
                 for distance in (-32768, -1, 0, 1, 16384, 32767)]
        rng = random.Random(86103)
        cases += [(rng.randrange(-32768, 32768), rng.randrange(-32768, 32768))
                  for _ in range(96)]
        compiler = Path(r"C:/msys64/mingw32/bin/gcc.exe")
        with tempfile.TemporaryDirectory(prefix="renderer-word-") as temporary:
            folder = Path(temporary)
            probe = folder / "probe.c"
            exe = folder / "probe.exe"
            probe.write_text("#include <stdint.h>\n#include <stdio.h>\n" + helper +
                             "\nint main(void){int a,b;while(scanf(\"%d %d\",&a,&b)==2){"
                             "int16_t d=port_renderer_double_word((int16_t)a);"
                             "printf(\"%d %d\\n\",(int)d,d>(int16_t)b);}return 0;}\n")
            env = os.environ.copy()
            env["PATH"] = str(compiler.parent) + os.pathsep + env.get("PATH", "")
            build = subprocess.run([str(compiler), "-std=c11", "-O2", str(probe),
                                    "-o", str(exe)], env=env, capture_output=True,
                                   text=True, timeout=30)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([str(exe)], env=env, capture_output=True, text=True,
                                 input="".join(f"{a} {b}\n" for a, b in cases), timeout=10)
            self.assertEqual(run.returncode, 0, run.stderr)
            actual = [tuple(map(int, row.split())) for row in run.stdout.splitlines()]
        self.assertEqual(len(actual), len(cases))
        machine = Uc(UC_ARCH_X86, UC_MODE_16)
        machine.mem_map(0, 1 << 20)
        machine.mem_write(0x10000, image.relocated(0x1000))
        # seg006 starts at load offset 85344; preserve its real-mode CS base.
        machine.reg_write(UC_X86_REG_CS, 0x1000 + 85344 // 16)
        for (limit, distance), result in zip(cases, actual):
            machine.reg_write(UC_X86_REG_CX, limit & 0xffff)
            machine.reg_write(UC_X86_REG_AX, distance & 0xffff)
            machine.reg_write(UC_X86_REG_EFLAGS, 0x202)
            machine.emu_start(0x10000 + 86103, 0x10000 + 86107)
            word = machine.reg_read(UC_X86_REG_CX) & 0xffff
            signed = word if word < 0x8000 else word - 0x10000
            flags = machine.reg_read(UC_X86_REG_EFLAGS)
            # Signed greater, the inverse of the original JLE: !ZF && SF==OF.
            greater = int(not(flags & 0x40) and bool(flags & 0x80) == bool(flags & 0x800))
            self.assertEqual(result, (signed, greater), (limit, distance))


if __name__ == "__main__":
    unittest.main()
