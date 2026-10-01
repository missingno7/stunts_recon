"""Presentation correction never changes the logical MCGA coordinate grid."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_projection_math import ROOT, host_compiler


class ViewportTests(unittest.TestCase):
    def test_corrected_rectangles_and_inverse_mapping(self):
        compiler = host_compiler()
        self.assertIsNotNone(compiler, "host compiler required")
        environment = os.environ.copy()
        environment["PATH"] = str(Path(compiler).parent) + os.pathsep + environment.get("PATH", "")
        with tempfile.TemporaryDirectory(prefix="stunts-viewport-") as directory:
            exe = Path(directory) / "viewport.exe"
            result = subprocess.run([
                compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-I", str(ROOT / "port"), str(ROOT / "tests/sdl3/viewport_probe.c"),
                "-o", str(exe),
            ], env=environment, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe)], env=environment, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
