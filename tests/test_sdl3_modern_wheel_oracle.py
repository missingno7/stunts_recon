"""Differential proof for the procedural wheel and visible degenerate faces."""
from __future__ import annotations

import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest


HERE = Path(__file__).resolve().parent
ROOT = next(parent for parent in HERE.parents
            if (parent / "lib" / "wheel.c").is_file() and
            (parent / "assets").is_dir())
sys.path[:0] = [str(ROOT / "build" / "python"), str(ROOT)]
from tests.test_sdl3_shape_pipeline import (  # noqa: E402
    MENU_FIXTURE,
    _oracle_render,
    _shape_resources,
)
from tools.porting.diffharness.emulator import RealModeRunner  # noqa: E402
from tools.porting.diffharness.model import (  # noqa: E402
    MemoryRegion,
    MemoryWrite,
    RoutineCase,
)
from tools.porting.diffharness.oracle import (  # noqa: E402
    OracleImage,
    SymbolMap,
    dgroup_segment,
)


CASES = (
    ("STANSX.P3S", "car0", (0, 2, 8, 10)),
    ("STAUDI.P3S", "car0", (1, 9)),
    ("STCOUN.P3S", "car0", (0, 7, 8, 15)),
    ("STFGTO.P3S", "car0", (1, 6, 9, 14)),
    ("STLM02.P3S", "car0", (0, 8)),
    ("GAME2.P3S", "truk", (5, 6, 13, 14, 16, 17)),
)
SCREEN_BYTES = 320 * 200
COLORS = (38, 39, 40)


def compiler_path():
    configured = os.environ.get("CC")
    if configured:
        return shutil.which(configured) or (
            configured if Path(configured).is_file() else None)
    mingw = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    return str(mingw) if mingw.is_file() else shutil.which("gcc")


def compile_helper(compiler, output, *sources, include_render=False):
    from tools.porting.project_headers import expand_project_headers
    from tools.porting.host_probe_modes import legacy_target_widths

    prepared_sources = []
    for source in sources:
        if source.parent in (ROOT / "game", ROOT / "lib"):
            prepared = output.parent / ("expanded_" + source.name)
            prepared.write_text(legacy_target_widths(expand_project_headers(
                source.read_text(encoding="latin-1"), ROOT)), encoding="latin-1")
            prepared_sources.append(prepared)
        else:
            prepared_sources.append(source)
    environment = os.environ.copy()
    environment["PATH"] = os.pathsep.join(filter(None, [
        str(Path(compiler).resolve().parent), environment.get("PATH", "")]))
    command = [compiler, "-std=c11", "-O2", "-Wall", "-Wextra",
               "-include", "stdint.h", "-I", str(ROOT / "include"),
               "-I", str(ROOT / "tools" / "porting" / "port_include"),
               "-I", str(ROOT / "port"), "-I", str(HERE / "sdl3"),
               "-DI16=int16_t", "-DI8=int8_t", "-DI8S=int8_t",
               "-DU8=uint8_t", "-DU16=uint16_t", "-DI16S=int16_t",
               "-DU16S=uint16_t", "-DI32=int32_t", "-DU32=uint32_t",
               "-DFAR=", "-DNEAR=", "-DHUGE=", "-Dfar=", "-Dnear=", "-Dhuge="]
    if include_render:
        command += ["-I", str(ROOT / "port" / "render")]
    command += [str(source) for source in prepared_sources] + ["-lm", "-o", str(output)]
    result = subprocess.run(command, cwd=ROOT, env=environment,
                            capture_output=True, text=True, timeout=60)
    if result.returncode:
        raise RuntimeError(f"C test build failed:\n{result.stderr}")


def parse_wheel_pool(pool, count):
    offset = 0
    wheels = []
    for ordinal in range(count):
        depth, material, point_count, primitive, _reserved = struct.unpack_from(
            "<hBBBB", pool, offset)
        points = struct.unpack_from("<" + "h" * (2 * point_count), pool,
                                    offset + 6)
        if primitive == 3:
            wheels.append({"ordinal": ordinal, "depth": depth,
                           "material": material, "points": points})
        offset += 6 + 4 * point_count
    return wheels


def read_captured_faces(path):
    data = path.read_bytes()
    face_count, = struct.unpack_from("<H", data)
    offset = 2
    faces = []
    for index in range(face_count):
        point_count, color = struct.unpack_from("<Hh", data, offset)
        offset += 4
        coords = struct.unpack_from("<" + "h" * (point_count * 2), data,
                                    offset)
        offset += point_count * 4
        faces.append((index, color, tuple(zip(coords[::2], coords[1::2]))))
    if offset != len(data):
        raise AssertionError("C wheel face capture has trailing data")
    return faces


def signed_area2(points):
    return sum(x * points[(i + 1) % len(points)][1] -
               points[(i + 1) % len(points)][0] * y
               for i, (x, y) in enumerate(points))


def make_sprite_writes(routine, data_symbols, dgroup):
    code_segment, _ = routine.far_at(0x1000)
    sprite_offset = data_symbols["_sprite1"]["load_address"] - routine.code_base
    sprite_words = [0] * 15
    sprite_words[1] = 0xA000
    sprite_words[5] = 0xC000
    sprite_words[6:11] = [0, 320, 0, 200, 320]
    sprite_words[12:15] = [320, 0, 320]
    return [
        MemoryWrite(code_segment, sprite_offset,
                    struct.pack("<15H", *sprite_words)),
        MemoryWrite(code_segment, 0xC000,
                    struct.pack("<200H", *(y * 320 for y in range(200)))),
    ]


def call_full_wheel(runner, symbols, data_symbols, points):
    routine = symbols.resolve("solidtire")
    raster = symbols.resolve("drawpolyp")
    dgroup = dgroup_segment(0x1000)
    rect_offset, stack_offset = 0xD000, 0xF000
    stack = [0, 0x7000, rect_offset, 0x2500, *COLORS]
    writes = make_sprite_writes(raster, data_symbols, dgroup)
    writes += [
        MemoryWrite(dgroup, rect_offset, struct.pack("<8h", *points)),
        MemoryWrite(0xA000, 0, bytes(65536)),
        MemoryWrite(dgroup, stack_offset,
                    struct.pack("<" + "H" * len(stack),
                                *(value & 0xFFFF for value in stack))),
    ]
    case = RoutineCase(
        "modern-wheel-full", "solidtire",
        args=(rect_offset, 0x2500, *COLORS), call="far",
        registers={"ds": dgroup, "ss": dgroup, "sp": stack_offset},
        memory=writes,
        compare=(MemoryRegion("vram", 0xA000, 0, SCREEN_BYTES),),
        result_registers=())
    return runner.call(case).memory["vram"]


def call_face(runner, symbols, data_symbols, face):
    index, color, points = face
    operation = ("preRender_wheel_helper4" if index < 16
                 else "drawpolyp")
    routine = symbols.resolve(operation)
    dgroup = dgroup_segment(0x1000)
    point_offset, stack_offset = 0xD000, 0xF000
    flattened = tuple(value for xy in points for value in xy)
    writes = make_sprite_writes(routine, data_symbols, dgroup)
    writes.append(MemoryWrite(0xA000, 0, bytes(65536)))
    if operation == "preRender_wheel_helper4":
        args = (color, 4, *flattened)
    else:
        args = (color, len(points), point_offset)
        writes.append(MemoryWrite(dgroup, point_offset,
                                  struct.pack("<" + "h" * len(flattened),
                                              *flattened)))
    case = RoutineCase(
        f"modern-wheel-face-{index}", operation, args=args, call="far",
        registers={"ds": dgroup, "ss": dgroup, "sp": stack_offset},
        memory=writes,
        compare=(MemoryRegion("vram", 0xA000, 0, SCREEN_BYTES),),
        result_registers=())
    return runner.call(case).memory["vram"]


class ModernWheelOracleTests(unittest.TestCase):
    def test_procedural_wheel_matches_dos_and_keeps_visible_degenerate_faces(self):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("a C compiler is required for the wheel geometry harness")

        with tempfile.TemporaryDirectory(prefix="sdl3-modern-wheel-") as temp_name:
            temp = Path(temp_name)
            geometry_test = temp / "modern_wheel_geometry"
            capture_exe = temp / "legacy_wheel_capture"
            common = (
                ROOT / "lib" / "wheel.c",
                ROOT / "lib" / "whlhelp.c",
                ROOT / "lib" / "whlhelp2.c",
                ROOT / "lib" / "whlhelp3.c",
                ROOT / "port" / "sincos.c",
            )
            compile_helper(compiler, geometry_test,
                           HERE / "sdl3/test_modern_wheel_geometry.c",
                           ROOT / "port" / "render" / "modern_wheel.c",
                           *common, include_render=True)
            compile_helper(compiler, capture_exe,
                           HERE / "sdl3/legacy_wheel_capture.c", *common, include_render=True)
            geometry = subprocess.run([str(geometry_test)], cwd=ROOT,
                                      capture_output=True, text=True, timeout=60)
            self.assertEqual(geometry.returncode, 0, geometry.stderr)

            resources = {(row["archive"], row["name"]): row
                         for row in _shape_resources()}
            image = OracleImage.load(ROOT / "assets")
            symbols = SymbolMap()
            runner = RealModeRunner(image, instruction_budget=1000000)
            data_symbols = json.loads(
                (ROOT / "layout" / "data-symbols.json").read_text())["symbols"]

            for archive, name, expected_zero_area in CASES:
                with self.subTest(shape=f"{archive}:{name}"):
                    row = resources[(archive, name)]
                    scene = _oracle_render(image, symbols, row["chunk"],
                                           MENU_FIXTURE)
                    wheels = parse_wheel_pool(scene["pool"], scene["count"])
                    self.assertTrue(wheels, "fixture must contain a stock wheel")
                    points = wheels[0]["points"]
                    capture = temp / "faces.bin"
                    command = [str(capture_exe), str(capture),
                               *map(str, points), *map(str, COLORS)]
                    captured = subprocess.run(command, cwd=ROOT,
                                              capture_output=True, text=True, timeout=60)
                    self.assertEqual(captured.returncode, 0, captured.stderr)
                    faces = read_captured_faces(capture)
                    self.assertEqual(len(faces), 19)
                    self.assertEqual([len(face[2]) for face in faces],
                                     [4] * 16 + [18, 18, 16])
                    self.assertEqual([face[1] for face in faces],
                                     [COLORS[0]] * 16 + [COLORS[1]] * 2 +
                                     [COLORS[2]])

                    full = call_full_wheel(runner, symbols, data_symbols, points)
                    composed = bytearray(SCREEN_BYTES)
                    degenerate = []
                    visible_degenerate = []
                    for face in faces:
                        frame = call_face(runner, symbols, data_symbols, face)
                        if signed_area2(face[2]) == 0:
                            degenerate.append(face[0])
                            if any(frame):
                                visible_degenerate.append(face[0])
                        for i, value in enumerate(frame):
                            if value:
                                composed[i] = value

                    self.assertEqual(degenerate, list(expected_zero_area))
                    self.assertEqual(visible_degenerate, list(expected_zero_area),
                                     "zero-area DOS faces still paint edge pixels")
                    self.assertEqual(bytes(composed), full,
                                     "ordered face callbacks must recompose exact full DOS wheel")


if __name__ == "__main__":
    unittest.main()
