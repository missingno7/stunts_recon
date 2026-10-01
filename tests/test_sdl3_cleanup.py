"""Compile and exercise SDL3 guest exit-list and debug adapters."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SDL_ROOT = Path(os.environ.get("SDL3_ROOT", r"C:\tools\sdl3-3.4.16-i686"))


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                      Path(r"C:\msys64\mingw64\bin\gcc.exe")):
        if candidate.is_file():
            return str(candidate)
    return shutil.which("gcc")


class SDL3CleanupTests(unittest.TestCase):
    def test_exit_list_order_capacity_persistence_and_guest_handoff(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the SDL3 host checks")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-cleanup-") as directory:
            executable = Path(directory) / "sdl3-cleanup-probe.exe"
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-Wno-unused-parameter",
                str(ROOT / "tests" / "sdl3_cleanup_probe.c"),
                str(ROOT / "port" / "cleanup.c"), "-o", str(executable),
            ]
            environment = os.environ.copy()
            path_parts = [str(Path(compiler).resolve().parent),
                          str(SDL_ROOT / "bin")]
            environment["PATH"] = os.pathsep.join(
                path_parts + [environment.get("PATH", "")])
            result = subprocess.run(command, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

            result = subprocess.run([str(executable)], cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("SDL3 cleanup adapter checks passed", result.stdout)
            self.assertEqual(result.stderr, "cleanup-test:7/ok\n")


if __name__ == "__main__":
    unittest.main()
