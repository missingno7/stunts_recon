"""Check the test-only Port Forge seed injection at its source call boundary."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class StartupSeedTests(unittest.TestCase):
    def test_reference_random_wait_and_prng_state_are_injected_exactly(self) -> None:
        compiler = os.environ.get("CC") or shutil.which("gcc")
        if compiler is None:
            for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                              Path(r"C:\msys64\mingw64\bin\gcc.exe")):
                if candidate.is_file():
                    compiler = str(candidate)
                    break
        self.assertIsNotNone(compiler, "GCC is required for the SDL3 host checks")
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-seed-") as directory:
            executable = Path(directory) / "startup-seed-probe.exe"
            result = subprocess.run(
                [str(compiler), "-std=c11", "-Wall", "-Wextra", "-Werror",
                 str(ROOT / "tests" / "sdl3_startup_seed_probe.c"),
                 str(ROOT / "port" / "test_seed.c"),
                 str(ROOT / "port" / "random.c"), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable), str(ROOT / "tests" / "fixtures" / "sdl3" /
                                      "startup-seed.json")],
                cwd=ROOT, capture_output=True, text=True, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("reads=6078 timer=340 rand=9479 Kevin=0x3d", result.stdout)


if __name__ == "__main__":
    unittest.main()
