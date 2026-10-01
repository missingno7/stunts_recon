"""Locked-original parity for race polygon edge and endpoint cases."""
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "build" / "python"))
sys.path.insert(0, str(ROOT))
os.environ["PATH"] = os.pathsep.join(filter(None, [
    r"C:\msys64\mingw64\bin",
    r"C:\msys64\mingw32\bin",
    os.environ.get("PATH", ""),
]))
_dll_handles = []
if os.name == "nt" and hasattr(os, "add_dll_directory"):
    for dll_dir in (ROOT / "build" / "python" / "unicorn" / "lib",
                    Path(r"C:\msys64\mingw64\bin")):
        if dll_dir.is_dir():
            _dll_handles.append(os.add_dll_directory(str(dll_dir)))

from tools.porting.diffharness.emulator import RealModeRunner
from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
from tools.porting.diffharness.oracle import OracleImage, SymbolMap


def host_compiler():
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    return str(configured) if configured.is_file() else shutil.which("gcc")


class Sdl3FlatPolygonTests(unittest.TestCase):
    def test_polygon_edge_cases_match_locked_raster_callbacks(self):
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("a C compiler is needed for the raster harness")

        flat_points = (0, 127, 319, 127, 319, 127, 0, 127)
        cases = (
            ("preRender_default", "default", (197,), "default 197", flat_points),
            ("preRender_patterned", "pattern", (0xFFFF, 197),
             "pattern 0xffff 197", flat_points),
            ("preRender_patterned", "pattern", (0xAA55, 197),
             "pattern 0xaa55 197", flat_points),
            ("preRender_patterned", "pattern", (0x0000, 197),
             "pattern 0x0000 197", flat_points),
            ("preRender_unk", "unknown", (0xFFFF, 29, 31),
             "unknown 0xffff 29 31", flat_points),
            ("preRender_unk", "unknown", (0xAA55, 29, 31),
             "unknown 0xaa55 29 31", flat_points),
            ("preRender_unk", "unknown", (0x0000, 29, 31),
             "unknown 0x0000 29 31", flat_points),
            ("preRender_default_alt", "alt", (39,), "alt 39", (
                197, 36, 196, 37, 196, 38, 195, 39, 195, 42, 195, 45,
                196, 45, 196, 45, 197, 46, 197, 44, 196, 44, 196, 44,
                196, 44, 196, 42, 196, 40, 196, 40, 196, 38, 197, 38,
            )),
            ("preRender_default_alt", "alt", (39,), "alt 39", (
                197, 36, 198, 37, 198, 37, 199, 37, 199, 40, 199, 43,
                198, 44, 198, 45, 197, 46, 197, 44, 198, 44, 198, 42,
                198, 42, 198, 40, 198, 38, 198, 38, 198, 38, 197, 38,
            )),
            ("preRender_default_alt", "alt", (39,), "alt 39", (
                171, 51, 172, 52, 173, 53, 173, 56, 173, 60, 172, 62,
                171, 63, 169, 66, 167, 67, 168, 63, 169, 63, 170, 62,
                170, 60, 171, 59, 171, 57, 171, 55, 170, 55, 170, 55,
            )),
            ("preRender_default_alt", "alt", (39,), "alt 39", (
                105, 20, 103, 22, 108, 24, 105, 26, 102, 24, 107, 22,
            )),
            ("preRender_default", "default", (39,), "default 39", (
                100, 20, 90, 23, 110, 27, 100, 30,
            )),
            ("preRender_default_alt", "alt", (39,), "alt 39", (
                100, 20, 90, 23, 110, 27, 100, 30,
            )),
        )
        with tempfile.TemporaryDirectory(prefix="sdl3-flat-polygon-") as temporary:
            temp = Path(temporary)
            executable = temp / "raster_harness.exe"
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join(filter(None, [
                str(Path(compiler).resolve().parent), environment.get("PATH", "")
            ]))
            build = subprocess.run([
                compiler, "-std=c11", "-O2", "-ffunction-sections",
                "-fdata-sections", "-DPORT_BUILD", "-Iport",
                str(ROOT / "tests" / "sdl3" / "raster_harness.c"),
                "-Wl,--gc-sections", "-o", str(executable),
            ], cwd=ROOT, capture_output=True, text=True, env=environment)
            self.assertEqual(build.returncode, 0,
                             f"raster harness compile failed:\n{build.stderr}")

            runner = RealModeRunner(OracleImage.load(ROOT / "assets"),
                                    instruction_budget=100000)
            symbols = SymbolMap()
            data_symbols = json.loads(
                (ROOT / "layout" / "data-symbols.json").read_text()
            )["symbols"]
            dgroup = 0x3B77

            for mode, operation, attributes, host_prefix, points in cases:
                with self.subTest(mode=mode):
                    routine = symbols.resolve(mode)
                    code_segment, _ = routine.far_at(0x1000)
                    sprite_offset = (data_symbols["_sprite1"]["load_address"] -
                                     routine.code_base)
                    sprite_words = [0] * 15
                    sprite_words[1] = 0xA000
                    sprite_words[5] = 0xC000
                    sprite_words[6:11] = [0, 320, 0, 200, 320]
                    sprite_words[12:15] = [320, 0, 320]
                    point_offset, stack_offset = 0xD000, 0xF000
                    if mode == "preRender_default":
                        call_args = [attributes[0], len(points) // 2, point_offset]
                    elif mode == "preRender_patterned":
                        call_args = [*attributes, len(points) // 2, point_offset]
                    else:
                        call_args = [*attributes, len(points) // 2, point_offset]
                    stack = [0, 0x7000, *call_args]
                    writes = [
                        MemoryWrite(code_segment, sprite_offset,
                                    struct.pack("<15H", *sprite_words)),
                        MemoryWrite(code_segment, 0xC000,
                                    struct.pack("<200H", *(y * 320 for y in range(200)))),
                        MemoryWrite(dgroup, point_offset,
                                    struct.pack("<" + "h" * len(points), *points)),
                        MemoryWrite(0xA000, 0, bytes(65536)),
                        MemoryWrite(dgroup, stack_offset,
                                    struct.pack("<" + "H" * len(stack),
                                                *(value & 0xFFFF for value in stack))),
                    ]
                    oracle_case = RoutineCase(
                        mode, mode, args=call_args, call="far",
                        registers={"ds": dgroup, "ss": dgroup, "sp": stack_offset},
                        memory=writes,
                        compare=(MemoryRegion("vram", 0xA000, 0, 64000),),
                        result_registers=(),
                    )
                    expected = runner.call(oracle_case).memory["vram"]
                    output = temp / f"{operation}.bin"
                    count = len(points) // 2
                    argv = [*host_prefix.split(), str(count),
                            *map(str, points), str(output)]
                    actual_run = subprocess.run(
                        [str(executable), *argv], cwd=ROOT, capture_output=True,
                        text=True, check=False, env=environment)
                    self.assertEqual(actual_run.returncode, 0,
                                     f"{operation} harness failed:\n{actual_run.stderr}")
                    actual = output.read_bytes()
                    differences = [index for index, (got, want) in
                                   enumerate(zip(actual, expected)) if got != want]
                    preview = [(index % 320, index // 320,
                                actual[index], expected[index])
                               for index in differences[:16]]
                    self.assertEqual(actual, expected,
                                     f"{mode} differs from the locked DOS raster; "
                                     f"first pixels: {preview}")
                    if points == flat_points:
                        self.assertEqual(sum(pixel != 0 for pixel in actual), 320)


if __name__ == "__main__":
    unittest.main()
