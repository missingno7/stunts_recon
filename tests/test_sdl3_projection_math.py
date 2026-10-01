"""Focused word-level regression for recovered projection math routines."""
import os
from pathlib import Path
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
    if configured.is_file():
        return str(configured)
    return shutil.which("gcc")


class ProjectionMathTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = host_compiler()
        if compiler is None:
            raise AssertionError("a host C compiler is required for the projection math regression")
        cls.temp = tempfile.TemporaryDirectory(prefix="sdl3-projection-math-")
        executable = Path(cls.temp.name) / "projection_math_harness.exe"
        command = [
            compiler, "-std=c11", "-O2", "-ffunction-sections", "-fdata-sections",
            "-Wno-unused-function", "-Wno-unused-variable",
            "-I", str(ROOT / "port"),
            str(ROOT / "tests" / "sdl3" / "projection_math_harness.c"),
            str(ROOT / "port" / "polang.c"),
            str(ROOT / "port" / "sincos.c"),
            str(ROOT / "port" / "projection.c"),
            "-Wl,--gc-sections", "-o", str(executable),
        ]
        environment = os.environ.copy()
        environment["PATH"] = str(Path(compiler).resolve().parent) + os.pathsep + environment.get("PATH", "")
        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                                check=False, env=environment)
        if result.returncode:
            raise AssertionError(
                f"projection math harness compile failed ({result.returncode}):\n"
                f"command: {command}\n{result.stdout}\n{result.stderr}")
        cls.executable = executable

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temp"):
            cls.temp.cleanup()

    def test_port_math_matches_locked_machine_word_vectors(self):
        result = subprocess.run([str(self.executable)], cwd=ROOT,
                                capture_output=True, text=True, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
