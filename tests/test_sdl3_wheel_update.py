"""Live wheel inputs must preserve DOS spans without native global adjacency."""
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from tests.test_sdl3_shape_pipeline import FrozenMachine, OracleImage, SymbolMap, ROOT
from tools.porting.diffharness.oracle import DGROUP_PARAGRAPH

sys.path.insert(0, str(ROOT / "port"))
from game_abi import adapt_wheel_update_inputs
from host_probe_modes import legacy_target_widths


PROLOGUE = r'''#include "stunts_types.h"
struct VECTOR { I16S x, y, z; };
extern int16_t sinfast(int16_t), cosfast(int16_t), mulscl(int16_t, int16_t);
/* Intentionally break the DOS adjacency; a correct host adapter cannot need it. */
struct VECTOR pos_pt __attribute__((aligned(4))), ancv2 __attribute__((aligned(4)));
struct VECTOR ctrmesh __attribute__((aligned(4))), g_op_carvector2 __attribute__((aligned(4)));
struct VECTOR pts_set[6] __attribute__((aligned(32)));
struct VECTOR secondveccar[6] __attribute__((aligned(32)));
struct VECTOR veccar[6] __attribute__((aligned(32)));
struct VECTOR car_dvecs[6] __attribute__((aligned(32)));
struct VECTOR veco[6] __attribute__((aligned(32)));
struct VECTOR secondoveh[6] __attribute__((aligned(32)));
struct VECTOR opponent_pointc[6] __attribute__((aligned(32)));
struct VECTOR veh_od[6] __attribute__((aligned(32)));
'''


def wheel_cases():
    source = [value for i in range(24)
              for value in (110 + i * 13, -170 + i * 7, 720 - i * 19)]
    origin = [301, -302, 503, 1101, -1102, 1303]
    for player in (1, 0):
        angles = (-240, -1, 0, 1, 240) if player else (-65, -1, 0, 1, 65)
        for angle in angles:
            for base in ([63, 64, -63, -64], [-32768, 32767, -127, 127]):
                for cached in (0x7777, angle):
                    yield [player, angle, cached, *source, *origin, *base]


def oracle_wheel(image, symbols, data, packet):
    machine = FrozenMachine(image, symbols)
    def offset(name):
        return data[name]["load_address"] - DGROUP_PARAGRAPH * 16
    source = offset("_pts_set" if packet[0] else "_veco")
    origin = offset("_pos_pt" if packet[0] else "_ctrmesh")
    def write(at, words):
        machine.write_ds(at, struct.pack("<" + "h" * len(words), *words))
    write(source, packet[3:75]); write(origin, packet[75:81])
    write(0xC000, [0x5555] * 72)
    write(0xC100, packet[81:85]); write(0xC120, [0x1111] * 4 + [packet[2]])
    machine.call("sub_204AE", (0xC000, machine.dgroup, packet[1],
                               0xC100, 0xC120, source, origin))
    return machine.read_ds(0xC000, 144) + machine.read_ds(0xC120, 10)


class Sdl3WheelUpdateTests(unittest.TestCase):
    def test_split_globals_and_packed_inputs_match_frozen_wheels(self):
        gcc = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        if not gcc.is_file():
            self.skipTest("the pinned i686 compiler is required")
        data = json.loads((ROOT / "layout/data-symbols.json").read_text())["symbols"]
        for names in (("_pts_set", "_secondveccar", "_veccar", "_car_dvecs"),
                      ("_veco", "_secondoveh", "_opponent_pointc", "_veh_od")):
            starts = [data[name]["load_address"] for name in names]
            self.assertEqual([b - a for a, b in zip(starts, starts[1:])], [36] * 3)
        for first, second in (("_pos_pt", "_ancv2"), ("_ctrmesh", "_g_op_carvector2")):
            self.assertEqual(data[second]["load_address"] - data[first]["load_address"], 6)

        source = (ROOT / "src/obj_seg004.c").read_text(encoding="latin-1")
        start = source.index("void wheel_update(struct VECTOR far *out, I16 angle,")
        end = source.index("\n}\n\nstatic U8  arrowConn0", start) + 2
        body = source[start:end]
        wrapper = (ROOT / "tests/sdl3/wheel_update_probe.c").read_text()
        wrapper = wrapper.replace("#include <stdint.h>\n\nstruct VECTOR { int16_t x, y, z; };\n", "")
        environment = os.environ.copy()
        environment["PATH"] = str(gcc.parent) + os.pathsep + environment.get("PATH", "")
        cases = list(wheel_cases())
        image, symbols = OracleImage.load(ROOT / "assets"), SymbolMap()
        expected = [oracle_wheel(image, symbols, data, case) for case in cases]
        self.assertEqual(len(cases), 40)
        with tempfile.TemporaryDirectory(prefix="stunts-wheel-update-") as temporary:
            work = Path(temporary)
            packets = work / "input.bin"
            packets.write_bytes(b"".join(struct.pack("<85h", *case) for case in cases))
            for adapted in (False, True):
                with self.subTest(adapted=adapted):
                    cfile, exe, output = work / "probe.c", work / "probe.exe", work / "out.bin"
                    transformed = adapt_wheel_update_inputs(body) if adapted else body
                    cfile.write_text(PROLOGUE + legacy_target_widths(transformed) + wrapper)
                    compiled = subprocess.run([
                        str(gcc), "-std=gnu11", "-O0", "-I", str(ROOT / "port"),
                        "-I", str(ROOT / "tools/porting/port_include"),
                        "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
                        str(cfile), str(ROOT / "port/sincos.c"), "-o", str(exe),
                    ], env=environment, capture_output=True, text=True, timeout=60)
                    self.assertEqual(compiled.returncode, 0, compiled.stdout + compiled.stderr)
                    ran = subprocess.run([str(exe), str(packets), str(output)],
                                         env=environment, capture_output=True, text=True, timeout=15)
                    self.assertEqual(ran.returncode, 0, ran.stdout + ran.stderr)
                    results = output.read_bytes()
                    self.assertEqual(len(results), 40 * 332)
                    mismatches = 0
                    for i, want in enumerate(expected):
                        record = results[i * 332:(i + 1) * 332]
                        globals_result, packed_result = record[:154], record[178:]
                        addresses = struct.unpack("<6I", record[154:178])
                        self.assertNotEqual(addresses[1] - addresses[0], 36)
                        self.assertNotEqual(addresses[5] - addresses[4], 6)
                        if adapted:
                            self.assertEqual(globals_result, want, f"global input case {cases[i][:3]}")
                            self.assertEqual(packed_result, want, f"packed input case {cases[i][:3]}")
                        else:
                            mismatches += globals_result != want
                    if not adapted:
                        self.assertEqual(mismatches, 40, "the old adjacency assumption must fail")

    def test_adapter_rejects_missing_function_or_division_anchors(self):
        with self.assertRaises(ValueError):
            adapt_wheel_update_inputs("")
        source = (ROOT / "src/obj_seg004.c").read_text(encoding="latin-1")
        with self.assertRaises(ValueError):
            adapt_wheel_update_inputs(source.replace("y = base[j] / 64;", "y = 0;"))


if __name__ == "__main__":
    unittest.main()
