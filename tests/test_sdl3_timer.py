"""Compile and exercise timer IRQ delivery and recovered deadline behavior."""
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


class SDL3TimerTests(unittest.TestCase):
    def test_irq_dispatch_and_deadlines(self) -> None:
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("GCC is required for the SDL3 host checks")
        if not (SDL_ROOT / "include" / "SDL3" / "SDL.h").is_file():
            self.skipTest("SDL3 headers are not provisioned")
        if not (SDL_ROOT / "lib" / "libSDL3.dll.a").is_file():
            self.skipTest("SDL3 import library is not provisioned")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-timer-") as directory:
            executable = Path(directory) / "sdl3-timer-probe.exe"
            # Retain the existing integration scenarios while moving their raw
            # IRQ injection/inspection to the SDL-free clock core.
            probe = (ROOT / "tests/sdl3_timer_probe.c").read_text()
            for name in ("port_runtime.h", "timer.c"):
                probe = probe.replace(f'"../port/{name}"',
                                      '"' + (ROOT / "port" / name).as_posix() + '"')
            probe = probe.replace("SDL_AddAtomicInt(&s_tick_count, 1)",
                                  "port_clock_advance(&s_clock, 1)")
            probe = probe.replace("SDL_AddAtomicInt(&s_tick_count, (int)count)",
                                  "port_clock_advance(&s_clock, count)")
            probe = probe.replace("SDL_GetAtomicInt(&s_tick_count)",
                                  "atomic_load(&s_clock.raw_tick)")
            probe = probe.replace("SDL_GetAtomicInt(&s_callback_counter)", "s_clock.counter")
            probe = probe.replace("SDL_SetAtomicInt(&s_elapsed_tick_count,",
                                  "atomic_store(&s_clock.elapsed,")
            source = Path(directory) / "probe.c"
            source.write_text(probe)
            command = [
                compiler, "-std=gnu11", "-O0", "-Wall", "-Wextra",
                "-Wpedantic", "-Werror", "-Wno-unused-parameter",
                "-I", str(SDL_ROOT / "include"),
                str(source), str(ROOT / "port/tick_clock.c"),
                "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(executable),
            ]
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join([
                str(Path(compiler).resolve().parent), str(SDL_ROOT / "bin"),
                environment.get("PATH", ""),
            ])
            result = subprocess.run(command, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], cwd=ROOT, env=environment,
                                    capture_output=True, text=True, check=False,
                                    timeout=10)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("SDL3 timer adapter checks passed", result.stdout)


if __name__ == "__main__":
    unittest.main()
