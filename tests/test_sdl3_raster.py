"""Focused source-backed regression for the SDL3 race raster helpers."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]


def host_compiler():
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    return str(configured) if configured.is_file() else shutil.which("gcc")


class Sdl3RasterTests(unittest.TestCase):
    def test_raster_contracts(self):
        compiler = host_compiler()
        if compiler is None:
            self.skipTest("a C compiler is needed for the raster harness")
        with tempfile.TemporaryDirectory(prefix="sdl3-raster-") as temporary:
            executable = Path(temporary) / "raster_harness.exe"
            command = [
                compiler, "-std=c11", "-O2", "-ffunction-sections",
                "-fdata-sections", "-DPORT_BUILD", "-Iport",
                str(ROOT / "tests" / "sdl3" / "raster_harness.c"),
                "-Wl,--gc-sections", "-o", str(executable),
            ]
            environment = os.environ.copy()
            environment["PATH"] = (
                str(Path(compiler).resolve().parent) + os.pathsep +
                environment.get("PATH", "")
            )
            build = subprocess.run(command, cwd=ROOT, capture_output=True,
                                   text=True, check=False, env=environment)
            self.assertEqual(build.returncode, 0,
                             f"raster harness compile failed:\n{build.stdout}\n"
                             f"{build.stderr}")
            run = subprocess.run([str(executable)], cwd=ROOT,
                                 capture_output=True, text=True, check=False)
            self.assertEqual(run.returncode, 0,
                             f"raster harness failed ({run.returncode}):\n"
                             f"{run.stdout}\n{run.stderr}")


if __name__ == "__main__":
    unittest.main()
