"""Launch the actual drop-in ZIP with game data and no development PATH."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
import zipfile

from tests.test_sdl3_driving import ASSETS, ROOT, STARTUP_SEED, assert_build_ready
from tests.test_sdl3_file_ui import write_editor_file_flow

PACKAGE_FILES = {
    "stunts-sdl3.exe", "SDL3.dll", "SDL3-LICENSE.txt",
    "Nuked-OPL3-LICENSE.txt", "README-SDL3.txt",
}


class Sdl3PackageTests(unittest.TestCase):
    def test_drop_in_launch_and_editor_save_without_development_runtime(self):
        assert_build_ready()
        result = subprocess.run(
            [sys.executable, str(ROOT / "port" / "build.py"), "package", "--no-build"],
            cwd=ROOT, capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        archive = ROOT / "build" / "sdl3" / "stunts-sdl3-win32.zip"
        with tempfile.TemporaryDirectory(prefix="stunts-drop-in-") as temporary:
            run_dir = Path(temporary)
            game_folder = run_dir / "Game folder with spaces"
            shutil.copytree(ASSETS, game_folder)
            # Installing a newer port intentionally replaces its own files.
            # Exercise that upgrade even if the supplied game folder is clean.
            (game_folder / "stunts-sdl3.exe").write_bytes(b"old SDL3 installation")
            originals = {path.name: hashlib.sha256(path.read_bytes()).digest()
                         for path in game_folder.iterdir()
                         if path.is_file() and path.name not in PACKAGE_FILES}
            with zipfile.ZipFile(archive) as bundle:
                self.assertEqual(set(bundle.namelist()), PACKAGE_FILES)
                packaged = {name: hashlib.sha256(bundle.read(name)).digest()
                            for name in PACKAGE_FILES}
                bundle.extractall(game_folder)
            other_cwd = run_dir / "Unrelated working directory"
            other_cwd.mkdir()
            input_path = run_dir / "input.json"
            trace_path = run_dir / "trace.jsonl"
            write_editor_file_flow(input_path)
            environment = os.environ.copy()
            environment.pop("STUNTS_ASSET_ROOT", None)
            environment["PATH"] = str(Path(os.environ["SystemRoot"]) / "System32")
            environment["SDL_VIDEODRIVER"] = "dummy"
            environment["SDL_AUDIODRIVER"] = "dummy"
            result = subprocess.run([
                str(game_folder / "stunts-sdl3.exe"), f"--trace={trace_path}",
                f"--input-script={input_path}", f"--test-startup-seed={STARTUP_SEED}",
                "--test-auto-protection", "--run-ms=22000",
            ], cwd=other_cwd, env=environment, capture_output=True,
                text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("AD15 OPL2 output opened through SDL3", result.stderr)
            self.assertNotIn("FILE ERROR", result.stderr)
            self.assertEqual((game_folder / "saves" / "zzztest.trk").stat().st_size, 0x70A)
            self.assertEqual((game_folder / "saves" / "zzztest.hig").stat().st_size, 0x16C)
            self.assertFalse((run_dir / "saves").exists(), "saves escaped the game folder")
            for name, digest in originals.items():
                self.assertEqual(hashlib.sha256((game_folder / name).read_bytes()).digest(),
                                 digest, f"original game file changed: {name}")
            for name, digest in packaged.items():
                self.assertEqual(hashlib.sha256((game_folder / name).read_bytes()).digest(),
                                 digest, f"installed package file differs: {name}")
            stops = [row for row in map(json.loads, trace_path.read_text().splitlines())
                     if row.get("event_type") == "host_stop"]
            self.assertEqual(len(stops), 1)
            self.assertEqual(stops[0]["reason"], "requested run duration reached")


if __name__ == "__main__":
    unittest.main()
