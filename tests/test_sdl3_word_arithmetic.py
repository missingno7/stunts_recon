"""Compare the generated physics overlay with locked 16-bit instructions."""
from __future__ import annotations

import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "port"))
from build import adapt_gameplay_word_arithmetic  # noqa: E402

sys.path.insert(0, str(ROOT / "tools"))
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402

sys.path.insert(0, str(ROOT / "build" / "python"))
try:
    from unicorn import Uc, UC_ARCH_X86, UC_HOOK_CODE, UC_MODE_16
    from unicorn.x86_const import (
        UC_X86_REG_AX, UC_X86_REG_BP, UC_X86_REG_CS, UC_X86_REG_DS,
        UC_X86_REG_EFLAGS, UC_X86_REG_IP, UC_X86_REG_SI, UC_X86_REG_SS,
    )
except ImportError:  # pragma: no cover - workstation dependency
    Uc = None


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    if configured.is_file():
        return str(configured)
    return shutil.which("gcc")


def extract_function(source: str, name: str) -> str:
    match = re.search(
        rf"(?m)^(?:I8|I16)\s+{re.escape(name)}\s*\([^;\n]*\)\s*\{{",
        source,
    )
    if match is None:
        raise AssertionError(f"Missing function {name}")
    depth = 1
    end = match.end()
    while depth:
        if end >= len(source):
            raise AssertionError(f"Unterminated function {name}")
        if source[end] == "{":
            depth += 1
        elif source[end] == "}":
            depth -= 1
        end += 1
    return source[match.start():end]


def locked_image() -> bytes:
    packed, unpacked, _report, _transforms = verify(write=False)
    return MZ.parse(unpacked).load_image(unpacked)


def run_locked_window(image: bytes, start: int, end: int, stop: int,
                      *, si: int = 0, distance: int = 0,
                      speed: int = 0) -> tuple[int, int, int]:
    """Execute an immutable instruction window; return AX, flags, word local."""
    if Uc is None:
        raise RuntimeError("Unicorn is required for locked word arithmetic checks")
    machine = Uc(UC_ARCH_X86, UC_MODE_16)
    machine.mem_map(0, 0x100000)
    code_segment, entry_ip = 0x1000, 0x0100
    code_address = (code_segment << 4) + entry_ip
    machine.mem_write(code_address, image[start:end])
    ss, bp, ds, object_offset = 0x2000, 0x8000, 0x3000, 0x1000
    machine.mem_write((ss << 4) + bp + 6, object_offset.to_bytes(2, "little"))
    if start == 21706:
        machine.mem_write((ds << 4) + object_offset + 0x2C,
                          (speed & 0xFFFF).to_bytes(2, "little"))
    else:
        machine.mem_write((ss << 4) + bp - 0x0A,
                          (distance & 0xFFFF).to_bytes(2, "little"))
        machine.mem_write((ds << 4) + object_offset + 0x2C,
                          (speed & 0xFFFF).to_bytes(2, "little"))
    for register, value in (
        (UC_X86_REG_CS, code_segment), (UC_X86_REG_IP, entry_ip),
        (UC_X86_REG_SS, ss), (UC_X86_REG_BP, bp), (UC_X86_REG_DS, ds),
        (UC_X86_REG_SI, si),
    ):
        machine.reg_write(register, value)
    machine.reg_write(UC_X86_REG_EFLAGS, 0x0202)
    stop_address = code_address + (stop - start)
    stopped = False

    def before_instruction(uc, address, _size, _user):
        nonlocal stopped
        if address == stop_address:
            stopped = True
            uc.emu_stop()

    machine.hook_add(UC_HOOK_CODE, before_instruction)
    machine.emu_start(code_address, 0, count=100)
    if not stopped:
        raise AssertionError(f"Locked code did not reach stop at {stop:#x}")
    local_offset = bp - (0x190 if start == 21706 else 0x18)
    local_word = int.from_bytes(
        machine.mem_read((ss << 4) + local_offset, 2), "little")
    return (machine.reg_read(UC_X86_REG_AX) & 0xFFFF,
            machine.reg_read(UC_X86_REG_EFLAGS) & 0xFFFF,
            local_word)


def locked_car_speed(image: bytes, distance: int, speed: int) -> int:
    """Execute locked IMUL/SAR, compare, and car-speed update instructions."""
    if Uc is None:
        raise RuntimeError("Unicorn is required for locked word arithmetic checks")
    start, end, stop = 34129, 34167, 34167
    machine = Uc(UC_ARCH_X86, UC_MODE_16)
    machine.mem_map(0, 0x100000)
    code_segment, entry_ip = 0x1000, 0x0100
    code_address = (code_segment << 4) + entry_ip
    machine.mem_write(code_address, image[start:end])
    ss, bp, ds, object_offset = 0x2000, 0x8000, 0x3000, 0x1000
    machine.mem_write((ss << 4) + bp + 6, object_offset.to_bytes(2, "little"))
    machine.mem_write((ss << 4) + bp - 0x0A,
                      (distance & 0xFFFF).to_bytes(2, "little"))
    speed_at = (ds << 4) + object_offset + 0x2C
    machine.mem_write(speed_at, (speed & 0xFFFF).to_bytes(2, "little"))
    for register, value in (
        (UC_X86_REG_CS, code_segment), (UC_X86_REG_IP, entry_ip),
        (UC_X86_REG_SS, ss), (UC_X86_REG_BP, bp), (UC_X86_REG_DS, ds),
    ):
        machine.reg_write(register, value)
    machine.reg_write(UC_X86_REG_EFLAGS, 0x0202)
    stop_address = code_address + (stop - start)
    stopped = False

    def before_instruction(uc, address, _size, _user):
        nonlocal stopped
        if address == stop_address:
            stopped = True
            uc.emu_stop()

    machine.hook_add(UC_HOOK_CODE, before_instruction)
    machine.emu_start(code_address, 0, count=100)
    if not stopped:
        raise AssertionError("Locked collision window did not finish")
    return int.from_bytes(machine.mem_read(speed_at, 2), "little")


class Sdl3WordArithmeticTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Uc is None:
            raise AssertionError("Unicorn is required for the locked instruction oracle")
        cls.compiler = host_compiler()
        if cls.compiler is None:
            raise AssertionError("A host C compiler is required for the overlay regression")
        cls.image = locked_image()
        cls.original = (ROOT / "src" / "obj_seg001_complete.c").read_text(
            encoding="latin-1")
        cls.overlay = adapt_gameplay_word_arithmetic(cls.original)
        marker = "/* PORT_BUILD: preserve locked 16-bit gameplay arithmetic. */"
        begin = cls.overlay.index(marker)
        finish = cls.overlay.index("extern short rate_frame;", begin)
        cls.helpers = cls.overlay[begin:finish]

    def test_overlay_reproduces_locked_threshold_and_collision_windows(self):
        wall_cases = ((369, 140 << 8), (370, 140 << 8), (468, 140 << 8),
                      (469, 140 << 8), (500, 200 << 8), (767, 140 << 8))
        oracle_rows = []
        for angle, speed in wall_cases:
            _ax, flags, threshold = run_locked_window(
                self.image, 21706, 21736, 21736, si=angle, speed=speed)
            accepted = not (flags & 0x0001 or flags & 0x0040)
            oracle_rows.append((angle, speed, threshold, int(accepted)))

        collision_cases = ((200, 100 << 8), (43, 43 << 8), (42, 42 << 8))
        locked_speeds = [locked_car_speed(self.image, distance, speed)
                         for distance, speed in collision_cases]
        self.assertEqual(locked_speeds, [19968, 0, 2688])

        legacy_body = extract_function(self.original, "car_car_speed_adjust_maybe")
        legacy_body = legacy_body.replace(
            "car_car_speed_adjust_maybe", "legacy_host_car_speed_adjust_maybe", 1)
        adapted_body = extract_function(self.overlay, "car_car_speed_adjust_maybe")
        source = r'''#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define I8 int8_t
#define I16 int16_t
#define I16S int16_t
#define U16S uint16_t
#define U16 uint16_t
#define U8 uint8_t
#define Q8_FRACTION_BITS 8
#define ANGLE_TURN_MASK 0x3ff
#define ANGLE_HALF_TURN 0x200
#define ANGLE_UNITS_PER_TURN 0x400
struct VECTOR { I16S x, y, z; };
struct CARSTATE {
    I8 field_C8;
    U16S car_speed2;
    struct VECTOR car_rotate;
    I16S car_36MwhlAngle;
    U16S car_speed;
};
I16S sinfast(U16S angle) { (void)angle; return 0; }
I16S cosfast(U16S angle)
{
    return angle == 0 ? 256 : (angle == 0x200 ? -256 : 0);
}
I16S mulscl(I16S value, I16S scale)
{
    return (I16S)(((int32_t)value * (int32_t)scale) / 256);
}
I16 polradius2d(I16 x, I16 y)
{
    uint32_t square = (uint32_t)((int32_t)x * x + (int32_t)y * y);
    uint32_t radius = 0;
    while ((radius + 1) * (radius + 1) <= square) ++radius;
    return (I16)radius;
}
'''
        source += self.helpers + "\n" + legacy_body + "\n" + adapted_body + "\n"
        source += "static uint16_t call_collision(uint16_t p, uint16_t o, int adapted) {\n"
        source += "  struct CARSTATE player = {0}, opponent = {0};\n"
        source += "  player.car_speed2 = p; opponent.car_speed2 = o;\n"
        source += "  player.car_rotate.x = 0; opponent.car_rotate.x = 0x200;\n"
        source += "  if (adapted) car_car_speed_adjust_maybe(&player, &opponent);\n"
        source += "  else legacy_host_car_speed_adjust_maybe(&player, &opponent);\n"
        source += "  return player.car_speed2;\n}\n"
        source += "int main(void) {\n"
        source += "  static const struct { int16_t angle; uint16_t speed, threshold; int hit; } walls[] = {\n"
        source += ",\n".join(
            f"    {{{angle}, {speed}, {threshold}, {accepted}}}"
            for angle, speed, threshold, accepted in oracle_rows)
        source += "\n  };\n"
        source += "  size_t n; for (n = 0; n < sizeof(walls)/sizeof(walls[0]); ++n) {\n"
        source += "    uint16_t threshold = (uint16_t)port_game_wall_hit_threshold(walls[n].angle);\n"
        source += "    if (threshold != walls[n].threshold) return 10 + (int)n;\n"
        source += "    if (((uint16_t)walls[n].speed > threshold) != walls[n].hit) return 20 + (int)n;\n"
        source += "  }\n"
        source += "  if (call_collision(100u*256u, 100u*256u, 1) != 19968u) return 30;\n"
        source += "  if (call_collision(43u*256u, 0, 1) != 0u) return 31;\n"
        source += "  if (call_collision(42u*256u, 0, 1) != 2688u) return 32;\n"
        source += "  if (call_collision(100u*256u, 100u*256u, 0) == 19968u) return 33;\n"
        source += "  if (call_collision(43u*256u, 0, 0) == 0u) return 34;\n"
        source += "  return 0;\n}\n"

        with tempfile.TemporaryDirectory(prefix="stunts-word-arithmetic-") as directory:
            root = Path(directory)
            cfile = root / "word_arithmetic_overlay.c"
            executable = root / "word_arithmetic_overlay.exe"
            cfile.write_text(source, encoding="utf-8")
            environment = os.environ.copy()
            environment["PATH"] = (str(Path(self.compiler).resolve().parent) + os.pathsep
                                   + environment.get("PATH", ""))
            result = subprocess.run(
                [self.compiler, "-std=c11", "-O0", "-Wall", "-Wextra",
                 "-Wno-unused-variable", "-Wno-unused-but-set-variable",
                 str(cfile), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
                timeout=30, env=environment,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_build_adapter_requires_every_machine_proven_anchor(self):
        self.assertIn("port_game_wall_hit_threshold(i)", self.overlay)
        self.assertIn("(U16S)activeCarState->car_speed2 > (U16S)threshold", self.overlay)
        self.assertIn("port_game_mul_sar16(0x300, distanceToCar, 2)", self.overlay)
        self.assertIn("(U16S)player->car_speed2 < (U16S)speedPenalty", self.overlay)
        self.assertIn("(U16S)player->car_speed2 - (U16S)speedPenalty", self.overlay)
        with self.assertRaises(RuntimeError):
            adapt_gameplay_word_arithmetic(self.original.replace(
                "speedPenalty = (0x300 * distanceToCar) >> 2;", "", 1))


if __name__ == "__main__":
    unittest.main()
