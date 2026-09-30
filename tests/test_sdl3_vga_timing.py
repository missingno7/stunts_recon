"""Compile and exercise the pure Mode 13h raster status model."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class VgaTimingTests(unittest.TestCase):
    def test_status_bits_follow_raster_phase_without_read_side_effects(self) -> None:
        compiler = os.environ.get("CC") or shutil.which("gcc")
        if compiler is None:
            for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                              Path(r"C:\msys64\mingw64\bin\gcc.exe")):
                if candidate.is_file():
                    compiler = str(candidate)
                    break
        self.assertIsNotNone(compiler, "GCC is required for the SDL3 host checks")
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-vga-") as directory:
            executable = Path(directory) / "vga-timing-probe.exe"
            source = ROOT / "tests" / "sdl3_vga_timing_probe.c"
            result = subprocess.run(
                [str(compiler), "-std=c11", "-Wall", "-Wextra", "-Werror",
                 str(source), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable)], cwd=ROOT, capture_output=True,
                text=True, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
