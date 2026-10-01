"""Exercise the real track-editor save and reload path under SDL3."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "sdl3"
EXECUTABLE = BUILD / "stunts.exe"
BUILD_REPORT = BUILD / "build-report.json"
ASSETS = BUILD / "runtime" / "assets"
STARTUP_SEED = ROOT / "tests" / "fixtures" / "sdl3" / "startup-seed.json"
RUN_MS = 22_000


def tree_sha256(directory: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(item for item in directory.rglob("*") if item.is_file()):
        digest.update(path.relative_to(directory).as_posix().encode("utf-8"))
        digest.update(path.read_bytes())
    return digest.hexdigest()


def newest_build_input() -> tuple[int, Path]:
    """Return the newest source/tool timestamp that can affect the SDL build."""
    candidates = [ROOT / "port" / "build.py",
                  ROOT / "port" / "dependencies.py",
                  ROOT / "port" / "game_abi.py"]
    for directory, suffixes in ((ROOT / "port", {".c", ".h"}),
                                (ROOT / "src", {".c", ".h"}),
                                (ROOT / "include", {".h"})):
        if directory.is_dir():
            candidates.extend(path for path in directory.rglob("*")
                              if path.is_file() and path.suffix.lower() in suffixes)
    existing = [path for path in candidates if path.is_file()]
    newest = max(existing, key=lambda path: path.stat().st_mtime_ns)
    return newest.stat().st_mtime_ns, newest


def write_editor_file_flow(path: Path) -> None:
    """Enter the track editor, save ZZZTEST, then select it from the list."""
    pairs = lambda scan: [scan, scan | 0x80]
    events = [
        (5_000_000_000, [28, 156]),                  # Leave intro.
        (7_000_000_000, list(pairs(0x4D))),          # Main menu: Track.
        (7_300_000_000, [28, 156]),
        (10_000_000_000, list(pairs(0x4D))),         # Track menu: New/Edit.
        (10_300_000_000, [28, 156]),
        (15_000_000_000, list(pairs(0x39))),         # Editor palette.
        (15_200_000_000, list(pairs(0x50))),         # Save row.
        (15_400_000_000, list(pairs(0x50))),
        (15_600_000_000, [28, 156]),
        (16_200_000_000, list(pairs(0x0E)) * 8),     # Clear current basename.
        (16_500_000_000,
         sum((pairs(scan) for scan in (0x2C, 0x2C, 0x2C, 0x14,
                                       0x12, 0x1F, 0x14)), [])),
        (16_800_000_000, [28, 156]),                 # Save the scratch track.
        (18_300_000_000, list(pairs(0x48))),         # Select Load row.
        (18_600_000_000, [28, 156]),
        (19_000_000_000, list(pairs(0x2C))),         # Jump to ZZZTEST.
        (19_300_000_000, [28, 156]),                 # Reload it.
    ]
    payload = {
        "format": "portforge-exact-input-script-v1",
        "anchor_tick": 0,
        "events": [
            {"visible_tick": tick, "channel": "dos.keyboard.scancodes",
             "payload": codes}
            for tick, codes in events
        ],
    }
    path.write_text(json.dumps(payload, separators=(",", ":")) + "\n",
                    encoding="ascii")


class SDL3FileUITests(unittest.TestCase):
    def test_editor_saves_and_reloads_track_in_separate_save_root(self) -> None:
        if not EXECUTABLE.is_file() or not BUILD_REPORT.is_file() or not ASSETS.is_dir():
            self.skipTest("Build the SDL3 game before running UI file-flow checks")
        newest_input_ns, newest_input = newest_build_input()
        built_ns = min(EXECUTABLE.stat().st_mtime_ns,
                       BUILD_REPORT.stat().st_mtime_ns)
        self.assertGreaterEqual(
            built_ns, newest_input_ns,
            "SDL3 executable/report are older than build input "
            f"{newest_input.relative_to(ROOT)}; rebuild before UI acceptance")
        report = json.loads(BUILD_REPORT.read_text(encoding="utf-8"))
        self.assertEqual(report.get("target"), "i686-w64-mingw32")
        self.assertEqual(report.get("game_c_objects"), 38)
        self.assertEqual(report.get("port_objects"), 31)
        self.assertEqual(report.get("function_stub_count"), 0)
        self.assertEqual(report.get("data_stub_count"), 0)
        self.assertTrue(STARTUP_SEED.is_file())

        temp_root = ROOT / "build" / "temp"
        temp_root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-editor-files-",
                                          dir=temp_root) as temporary:
            run_dir = Path(temporary)
            asset_root = run_dir / "assets"
            shutil.copytree(ASSETS, asset_root)
            runtime_asset_hash = tree_sha256(ASSETS)
            source_asset_hash = tree_sha256(ROOT / "assets")
            copied_asset_hash = tree_sha256(asset_root)
            input_path = run_dir / "input.json"
            trace_path = run_dir / "trace.jsonl"
            write_editor_file_flow(input_path)

            environment = os.environ.copy()
            environment["SDL_VIDEODRIVER"] = "dummy"
            environment["SDL_AUDIODRIVER"] = "dummy"
            environment["PATH"] = os.pathsep.join((
                r"C:\msys64\mingw32\bin",
                r"C:\tools\sdl3-3.4.16-i686\bin",
                environment.get("PATH", ""),
            ))
            command = [
                str(EXECUTABLE),
                f"--trace={trace_path}",
                f"--assets={asset_root}",
                f"--input-script={input_path}",
                f"--test-startup-seed={STARTUP_SEED}",
                "--test-auto-protection",
                f"--run-ms={RUN_MS}",
            ]
            try:
                result = subprocess.run(
                    command, cwd=ROOT, env=environment,
                    capture_output=True, text=True, timeout=60, check=False)
            except subprocess.TimeoutExpired as error:
                self.fail(
                    "SDL3 track-editor file flow exceeded its 60-second bound; "
                    f"partial stdout={error.stdout!r}, stderr={error.stderr!r}")

            self.assertEqual(
                result.returncode, 0,
                f"SDL3 track-editor file flow exited {result.returncode}; "
                f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}")
            self.assertNotIn("unresolved host service:", result.stderr)
            self.assertNotIn("PORT fatal stub", result.stderr)
            self.assertNotIn("FILE ERROR", result.stderr)

            trace_events = [
                json.loads(line)
                for line in trace_path.read_text(encoding="utf-8").splitlines()
            ]
            stop_events = [event for event in trace_events
                           if event.get("event_type") == "host_stop"]
            self.assertEqual(len(stop_events), 1, stop_events)
            self.assertEqual(
                stop_events[0].get("reason"), "requested run duration reached")
            input_events = [event for event in trace_events
                            if event.get("event_type") == "input"]
            self.assertTrue(any(event.get("payload") == [44, 172]
                                for event in input_events),
                            "the scripted selector did not jump to ZZZTEST")

            save_root = run_dir / "saves"
            track_path = save_root / "zzztest.trk"
            score_path = save_root / "zzztest.hig"
            self.assertEqual(track_path.stat().st_size, 0x70A)
            self.assertEqual(score_path.stat().st_size, 0x16C)
            self.assertGreater(len(set(track_path.read_bytes())), 1)
            self.assertNotEqual(asset_root, save_root)
            self.assertEqual(tree_sha256(asset_root), copied_asset_hash)
            self.assertEqual(tree_sha256(ASSETS), runtime_asset_hash)
            self.assertEqual(tree_sha256(ROOT / "assets"), source_asset_hash)


if __name__ == "__main__":
    unittest.main()
