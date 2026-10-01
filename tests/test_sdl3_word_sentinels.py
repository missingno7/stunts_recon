"""Prove SDL3 keeps DOS 16-bit unsigned sentinel comparisons on the host."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
import sys
sys.path.insert(0, str(ROOT / "port"))
sys.path.insert(0, str(ROOT / "tools"))
from game_abi import adapt_word_sentinels  # noqa: E402
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    if configured.is_file():
        return str(configured)
    return shutil.which("gcc")


def locked_image() -> bytes:
    _packed, unpacked, _report, _transforms = verify(write=False)
    return MZ.parse(unpacked).load_image(unpacked)


class Sdl3WordSentinelTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sources = {
            name: (ROOT / "src" / name).read_text(encoding="latin-1")
            for name in ("obj_seg003.c", "obj_seg004.c")
        }
        cls.adapted = {
            name: adapt_word_sentinels(source, name)
            for name, source in cls.sources.items()
        }

    def test_frozen_comparisons_match_three_verified_cmp_and_branch_anchors(self):
        image = locked_image()
        # The immediate FFFF compares against the 16-bit word, then follows
        # the equality or inequality branch that implements the sentinel test.
        anchors = {
            0xDC7D: bytes.fromhex("83 7e d4 ff 75 4e"),
            0x10CED: bytes.fromhex("26 83 3f ff 74 0e"),
            0x10FC6: bytes.fromhex("26 83 3f ff 74 0e"),
        }
        for offset, expected in anchors.items():
            with self.subTest(offset=hex(offset)):
                self.assertEqual(image[offset:offset + len(expected)], expected)

    def test_adapter_has_only_the_three_counted_source_anchors(self):
        intro_name = "obj_seg003.c"
        track_name = "obj_seg004.c"
        intro_expr = "carHeadingData == 0xffff"
        track_expr = "g_td01_track_filecpy[prev_path] == 0xffff"
        self.assertEqual(self.sources[intro_name].count(intro_expr), 1)
        self.assertEqual(self.sources[track_name].count(track_expr), 2)
        self.assertEqual(self.adapted[intro_name].count("(uint16_t)carHeadingData == 0xffffu"), 1)
        self.assertEqual(
            self.adapted[track_name].count(
                "(uint16_t)g_td01_track_filecpy[prev_path] == 0xffffu"),
            2,
        )
        self.assertEqual(
            self.adapted[intro_name].replace(
                "(uint16_t)carHeadingData == 0xffffu", intro_expr),
            self.sources[intro_name],
        )
        self.assertEqual(
            self.adapted[track_name].replace(
                "(uint16_t)g_td01_track_filecpy[prev_path] == 0xffffu", track_expr),
            self.sources[track_name],
        )
        self.assertEqual(adapt_word_sentinels("int untouched;\n", "other.c"),
                         "int untouched;\n")

    def test_missing_machine_proven_source_anchors_are_rejected(self):
        intro_name = "obj_seg003.c"
        track_name = "obj_seg004.c"
        with self.assertRaisesRegex(ValueError, "Expected 1"):
            adapt_word_sentinels(
                self.sources[intro_name].replace("carHeadingData == 0xffff", "", 1),
                intro_name,
            )
        with self.assertRaisesRegex(ValueError, "Expected 2"):
            adapt_word_sentinels(
                self.sources[track_name].replace(
                    "g_td01_track_filecpy[prev_path] == 0xffff", "", 1),
                track_name,
            )

    def test_compiled_adapted_expressions_follow_dos_word_equality(self):
        compiler = host_compiler()
        if compiler is None:
            self.fail("A host C compiler is required for the sentinel regression")
        intro_expr = "(uint16_t)carHeadingData == 0xffffu"
        track_expr = "(uint16_t)g_td01_track_filecpy[prev_path] == 0xffffu"
        source = f'''#include <stdint.h>
static int intro_legacy(int16_t carHeadingData)
{{ return carHeadingData == 0xffff; }}
static int intro_adapted(int16_t carHeadingData)
{{ return {intro_expr}; }}
static int track_legacy(int16_t *g_td01_track_filecpy, int16_t prev_path)
{{ return g_td01_track_filecpy[prev_path] == 0xffff; }}
static int track_adapted(int16_t *g_td01_track_filecpy, int16_t prev_path)
{{ return {track_expr}; }}
int main(void)
{{
    int16_t missing = -1;
    int16_t present = 7;
    int16_t table[2] = {{ -1, 7 }};
    if (intro_legacy(missing) != 0 || intro_adapted(missing) != 1) return 1;
    if (intro_adapted(present) != 0) return 2;
    if (track_legacy(table, 0) != 0 || track_adapted(table, 0) != 1) return 3;
    if (track_adapted(table, 1) != 0) return 4;
    return 0;
}}
'''
        with tempfile.TemporaryDirectory(prefix="sdl3-word-sentinels-") as temporary:
            folder = Path(temporary)
            cfile = folder / "word_sentinels.c"
            executable = folder / "word_sentinels.exe"
            cfile.write_text(source, encoding="utf-8")
            environment = os.environ.copy()
            environment["PATH"] = (str(Path(compiler).resolve().parent) + os.pathsep
                                   + environment.get("PATH", ""))
            build = subprocess.run(
                [compiler, "-std=c11", "-O0", "-Wall", "-Wextra",
                 "-Wno-type-limits", str(cfile), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
                timeout=30, env=environment,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            run = subprocess.run([str(executable)], cwd=ROOT, capture_output=True,
                                 text=True, check=False, timeout=10)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)


if __name__ == "__main__":
    unittest.main()
