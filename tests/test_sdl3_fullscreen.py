"""Exercise borderless fullscreen and shortcut routing through the SDL host."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from tests.test_sdl3_diagnostics import compiler_path, make_test_environment, SDL_ROOT, ROOT


class Sdl3FullscreenTests(unittest.TestCase):
    def test_borderless_toggle_restores_geometry_and_consumes_shortcut(self):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("requires i686 GCC and SDL3")
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-fullscreen-") as temporary:
            exe = Path(temporary) / "fullscreen-probe.exe"
            result = subprocess.run([
                compiler, "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                "-I", str(ROOT / "port"), "-I", str(SDL_ROOT / "include"),
                str(ROOT / "tests/sdl3/fullscreen_probe.c"),
                str(ROOT / "port/sdl_host.c"),
                "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(exe),
            ], env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            # Test both the headless event path and the actual Windows backend.
            drivers = ["dummy", "windows"] if os.name == "nt" else ["dummy"]
            for driver in drivers:
                with self.subTest(driver=driver):
                    environment["SDL_VIDEODRIVER"] = driver
                    result = subprocess.run([str(exe)], env=environment,
                                            capture_output=True, text=True, timeout=20)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
