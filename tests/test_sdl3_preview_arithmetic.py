"""Compare the generated SDL preview helper with the locked DOS word ops."""
from __future__ import annotations

import os
from pathlib import Path
import random
import re
import shutil
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "port"))
from game_abi import adapt_preview_word_arithmetic  # noqa: E402

sys.path.insert(0, str(ROOT / "tools"))
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402

sys.path.insert(0, str(ROOT / "build" / "python"))
try:
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
    from unicorn.x86_const import (
        UC_X86_REG_AX, UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_EFLAGS,
        UC_X86_REG_IP,
    )
except ImportError:  # pragma: no cover - required workstation dependency
    Uc = None


SOURCE_PATH = ROOT / "src" / "obj_seg003.c"
CODE_START = 0xCDA4
LOCKED_CODE = bytes.fromhex("2b 06 9c 09 d1 f8")  # SUB AX,[camera_pos_z]; SAR AX,1
CAMERA_POS_Z_DS_OFFSET = 0x099C

# Nine explicit assignments in draw_track_preview are adapted. The y delta
# from camera_pos_y appears twice with obj_height, once for hill geometry and
# once for the temporary vector.
ANCHORS = (
    ("transformed.pos.x = (trackctrpos2[cx] - camera_pos_x) >> 1;",
     "transformed.pos.x = port_preview_coord_half(trackctrpos2[cx], camera_pos_x);"),
    ("transformed.pos.y = (-camera_pos_y) >> 1;",
     "transformed.pos.y = port_preview_coord_half(0, camera_pos_y);"),
    ("transformed.pos.z = (row_ctr_zs[cz] - camera_pos_z) >> 1;",
     "transformed.pos.z = port_preview_coord_half(row_ctr_zs[cz], camera_pos_z);"),
    ("transformed.pos.x = (trackctrpos2[tile_col] - camera_pos_x) >> 1;",
     "transformed.pos.x = port_preview_coord_half(trackctrpos2[tile_col], camera_pos_x);"),
    ("transformed.pos.y = (obj_height - camera_pos_y) >> 1;",
     "transformed.pos.y = port_preview_coord_half(obj_height, camera_pos_y);"),
    ("transformed.pos.z = (row_ctr_zs[row_idx] - camera_pos_z) >> 1;",
     "transformed.pos.z = port_preview_coord_half(row_ctr_zs[row_idx], camera_pos_z);"),
    ("vector.x = (obj_x_pos - camera_pos_x) >> 1;",
     "vector.x = port_preview_coord_half(obj_x_pos, camera_pos_x);"),
    ("vector.y = (obj_height - camera_pos_y) >> 1;",
     "vector.y = port_preview_coord_half(obj_height, camera_pos_y);"),
    ("vector.z = (obj_z - camera_pos_z) >> 1;",
     "vector.z = port_preview_coord_half(obj_z, camera_pos_z);"),
)


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    if configured.is_file():
        return str(configured)
    return shutil.which("gcc")


def extract_function(source: str, signature: str) -> str:
    match = re.search(re.escape(signature) + r"\s*\{", source)
    if match is None:
        raise AssertionError(f"Missing function signature {signature!r}")
    start = match.start()
    brace = match.end() - 1

    # Skip comments and quoted text so brace counting follows C structure.
    depth = 1
    pos = brace + 1
    state = "code"
    while pos < len(source) and depth:
        char = source[pos]
        following = source[pos + 1] if pos + 1 < len(source) else ""
        if state == "code":
            if char == "/" and following == "*":
                state = "block-comment"
                pos += 1
            elif char == "/" and following == "/":
                state = "line-comment"
                pos += 1
            elif char == '"':
                state = "string"
            elif char == "'":
                state = "character"
            elif char == "{":
                depth += 1
            elif char == "}":
                depth -= 1
        elif state == "block-comment" and char == "*" and following == "/":
            state = "code"
            pos += 1
        elif state == "line-comment" and char == "\n":
            state = "code"
        elif state in {"string", "character"}:
            if char == "\\":
                pos += 1
            elif ((state == "string" and char == '"') or
                  (state == "character" and char == "'")):
                state = "code"
        pos += 1
    if depth:
        raise AssertionError(f"Unterminated function body {signature!r}")
    return source[start:pos]


def locked_image() -> bytes:
    _packed, unpacked, _report, _transforms = verify(write=False)
    return MZ.parse(unpacked).load_image(unpacked)


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value if value < 0x8000 else value - 0x10000


def frozen_sub_sar(machine, code_address: int, data_segment: int,
                   coordinate: int, camera: int) -> int:
    machine.mem_write((data_segment << 4) + CAMERA_POS_Z_DS_OFFSET,
                      (camera & 0xFFFF).to_bytes(2, "little"))
    machine.reg_write(UC_X86_REG_CS, code_address >> 4)
    machine.reg_write(UC_X86_REG_IP, code_address & 0xF)
    machine.reg_write(UC_X86_REG_DS, data_segment)
    machine.reg_write(UC_X86_REG_AX, coordinate & 0xFFFF)
    machine.reg_write(UC_X86_REG_EFLAGS, 0x0202)
    machine.emu_start(code_address, code_address + len(LOCKED_CODE), count=2)
    return signed_word(machine.reg_read(UC_X86_REG_AX))


def input_cases() -> list[tuple[int, int]]:
    edges = (-32768, -32767, -2, -1, 0, 1, 2, 32766, 32767)
    cases = [(coordinate, camera) for coordinate in edges for camera in edges]
    cases.extend(((30208, -2800), (29184, -2800), (512, -2800),
                  (-32500, 400), (0, -1), (1, -32768)))
    generator = random.Random(0x5D17)
    cases.extend((generator.randint(-32768, 32767),
                  generator.randint(-32768, 32767)) for _ in range(192))
    return cases


class Sdl3PreviewArithmeticTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Uc is None:
            raise AssertionError("Unicorn is required for locked preview instruction checks")
        cls.compiler = host_compiler()
        if cls.compiler is None:
            raise AssertionError("A host C compiler is required for preview adapter checks")
        cls.original = SOURCE_PATH.read_text(encoding="latin-1")
        cls.adapted = adapt_preview_word_arithmetic(cls.original)
        cls.original_preview = extract_function(cls.original, "void draw_track_preview(void)")
        cls.adapted_preview = extract_function(cls.adapted, "void draw_track_preview(void)")
        cls.helper = extract_function(
            cls.adapted, "static int16_t port_preview_coord_half(int16_t coordinate, int16_t camera)")

    def test_adapter_replaces_exactly_nine_preview_assignments_only(self):
        for old, new in ANCHORS:
            with self.subTest(anchor=old):
                self.assertEqual(self.original_preview.count(old), 1)
                self.assertEqual(self.adapted_preview.count(new), 1)
                self.assertNotIn(old, self.adapted_preview)
        self.assertEqual(len(ANCHORS), 9)

        # Removing the generated helper and restoring only draw_track_preview
        # must reproduce the input byte-for-byte, proving all other source
        # functions and declarations are untouched by this adapter.
        helper_marker = "/* PORT_BUILD: locked preview SUB AX followed by SAR AX,1. */"
        helper_start = self.adapted.index(helper_marker)
        preview_start = self.adapted.index("void draw_track_preview(void)", helper_start)
        without_helper = self.adapted[:helper_start] + self.adapted[preview_start:]
        self.assertNotEqual(without_helper, self.adapted)
        restored = without_helper.replace(self.adapted_preview, self.original_preview, 1)
        self.assertEqual(restored, self.original)

        with self.assertRaises(ValueError):
            adapt_preview_word_arithmetic(self.original.replace(ANCHORS[0][0], "", 1))

    def test_generated_helper_matches_locked_sub_ax_sar_ax(self):
        image = locked_image()
        self.assertEqual(image[CODE_START:CODE_START + len(LOCKED_CODE)], LOCKED_CODE)

        code_segment, entry_ip, data_segment = 0x1000, 0x0100, 0x3000
        code_address = (code_segment << 4) + entry_ip
        machine = Uc(UC_ARCH_X86, UC_MODE_16)
        machine.mem_map(0, 0x100000)
        machine.mem_write(code_address, image[CODE_START:CODE_START + len(LOCKED_CODE)])
        cases = input_cases()
        expected = [(coordinate, camera,
                     frozen_sub_sar(machine, code_address, data_segment,
                                    coordinate, camera))
                    for coordinate, camera in cases]

        rows = ",\n".join(
            f"    {{{coordinate}, {camera}, {result}}}"
            for coordinate, camera, result in expected)
        probe = (
            "#include <stdint.h>\n" + self.helper + "\n"
            "struct PreviewCase { int16_t coordinate, camera, expected; };\n"
            f"static const struct PreviewCase cases[] = {{\n{rows}\n}};\n"
            "int main(void) {\n"
            "    unsigned i;\n"
            "    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)\n"
            "        if (port_preview_coord_half(cases[i].coordinate, cases[i].camera) "
            "!= cases[i].expected) return (int)(i + 1);\n"
            "    return 0;\n}\n"
        )

        with tempfile.TemporaryDirectory(prefix="stunts-preview-arithmetic-") as directory:
            root = Path(directory)
            cfile = root / "preview_helper_probe.c"
            executable = root / "preview_helper_probe.exe"
            cfile.write_text(probe, encoding="ascii")
            environment = os.environ.copy()
            environment["PATH"] = (str(Path(self.compiler).resolve().parent) + os.pathsep
                                   + environment.get("PATH", ""))
            result = subprocess.run(
                [self.compiler, "-std=c11", "-O0", "-Wall", "-Wextra", "-Werror",
                 str(cfile), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
                timeout=30, env=environment,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT,
                                    capture_output=True, text=True, check=False,
                                    timeout=30, env=environment)
            self.assertEqual(result.returncode, 0,
                             f"compiled helper diverged at case {result.returncode - 1}: "
                             + result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
