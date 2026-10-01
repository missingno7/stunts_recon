"""Native-resolution coverage and explicit compositor-layer contracts."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_diagnostics import compiler_path, make_test_environment, SDL_ROOT
from tests.test_sdl3_projection_math import ROOT


class ModernRendererTests(unittest.TestCase):
    def run_probe(self, name, sources, sdl=False):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("requires GCC")
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-render-") as directory:
            exe = Path(directory) / "probe.exe"
            args = [compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                    "-I", str(ROOT / "port"), "-I", str(ROOT / "port/render"),
                    str(ROOT / "tests/sdl3" / name)]
            args += [str(ROOT / "port/render" / source) for source in sources]
            if sdl:
                args += ["-I", str(SDL_ROOT / "include"), "-L", str(SDL_ROOT / "lib"), "-lSDL3"]
            args += ["-lm", "-o", str(exe)]
            result = subprocess.run(args, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe)], env=environment,
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_native_patterns_materials_and_painter_order(self):
        self.run_probe("modern_probe.c", ["renderer_modern.c", "triangulate2d.c", "modern_wheel.c"], True)

    def test_world_and_ui_write_provenance(self):
        self.run_probe("renderer_ui_probe.c", ["renderer_ui.c"])

    def test_concavity_winding_degeneracy_and_rejection(self):
        self.run_probe("triangulate_probe.c", ["triangulate2d.c"])
