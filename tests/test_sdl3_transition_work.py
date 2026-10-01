"""Check the sprite_1_unk3 work formula against the frozen real-mode routine."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
ASSETS = Path(os.environ.get("STUNTS_ASSET_DIR", ROOT / "assets"))


def _host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                      Path(r"C:\msys64\mingw64\bin\gcc.exe")):
        if candidate.is_file():
            return str(candidate)
    return shutil.which("gcc")


@unittest.skipUnless(ASSETS.is_dir(), "original game assets are not provisioned")
class SpriteUnk3TimingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from tools.porting.diffharness.emulator import RealModeRunner
        from tools.porting.diffharness.oracle import OracleImage
        compiler = _host_compiler()
        if compiler is None:
            raise unittest.SkipTest("a host GCC compiler is needed for the work probe")
        cls.temp = tempfile.TemporaryDirectory(prefix="sdl3-transition-work-")
        cls.probe = Path(cls.temp.name) / "transition_work_probe.exe"
        command = [compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
                   str(ROOT / "tests" / "sdl3" / "transition_work_probe.c"),
                   "-o", str(cls.probe)]
        environment = os.environ.copy()
        environment["PATH"] = (str(Path(compiler).resolve().parent) +
                               os.pathsep + environment.get("PATH", ""))
        result = subprocess.run(command, cwd=ROOT, env=environment,
                                capture_output=True, text=True, check=False)
        if result.returncode:
            raise AssertionError(f"transition work probe compile failed:\n"
                                 f"{result.stdout}\n{result.stderr}")
        cls.runner = RealModeRunner(OracleImage.load(ASSETS))

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temp"):
            cls.temp.cleanup()

    def _production_header_count(self, width: int, height: int,
                                 phase: int) -> int:
        result = subprocess.run([str(self.probe), str(width), str(height),
                                 str(phase)], cwd=ROOT, capture_output=True,
                                text=True, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)
        return int(result.stdout.strip())

    def _frozen_count(self, width: int, height: int, phase: int) -> int:
        from tools.porting.diffharness.examples import _sprite_machine_state
        from tools.porting.diffharness.model import MemoryWrite, RoutineCase

        payload_height = max(height, 12)
        pixels = bytes((i * 13 + 7) & 0xFF for i in range(width * payload_height))
        shape = struct.pack("<6H4s", width, height, 0, 0, 61, 0,
                            bytes(4)) + pixels
        memory = _sprite_machine_state(source_width=width,
                                       source_height=payload_height)
        memory.append(MemoryWrite(0x6000, 0x0100, shape))
        case = RoutineCase("sprite_1_unk3 timing", "sprite_1_unk3",
                           args=[0x0100, 0x6000, phase], call="far",
                           memory=memory)
        # RealModeRunner's hook counts the synthetic return sentinel too.
        return self.runner.call(case).instructions - 1

    def test_production_header_work_matches_frozen_oracle_across_dimensions_and_phases(self):
        for width, height in ((0, 0), (1, 1), (5, 5), (13, 6),
                              (22, 12), (39, 48), (320, 200)):
            for phase in range(4):
                with self.subTest(width=width, height=height, phase=phase):
                    actual = self._frozen_count(width, height, phase)
                    self.assertEqual(self._production_header_count(
                        width, height, phase), actual)


if __name__ == "__main__":
    unittest.main()
