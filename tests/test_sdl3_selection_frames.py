"""Full indexed selection screens from independently verified DOS replays.

Animated car poses are matched by the complete frame, not host time or a
capture ordinal. This verifies rendering; it does not claim timer equivalence.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
import zlib

from tests.sdl3_screen_regression import read_port_fbr
from tests.test_sdl3_file_ui import newest_build_input

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build/sdl3"
FIXTURES = ROOT / "tests/fixtures/sdl3/original_replay"
LABELS = ("car-countach", "track-default", "opponent-clock")
RUN_MS = 34_000


def references():
    rows = json.loads((FIXTURES / "manifest.json").read_text())
    result = {}
    for row in rows:
        payload = zlib.decompress((FIXTURES / row["payload"]).read_bytes())
        if len(payload) != 64768:
            raise AssertionError(f"{row['label']}: invalid reference extent")
        for value, key in ((payload, "payload_sha256"),
                           (payload[:64000], "indices_sha256"),
                           (payload[64000:], "palette_rgb6_sha256")):
            if hashlib.sha256(value).hexdigest() != row[key]:
                raise AssertionError(f"{row['label']}: {key} differs")
        if row["label"] in LABELS:
            result[row["label"]] = payload
    if set(result) != set(LABELS):
        raise AssertionError("missing original selection references")
    return result


def write_input(path):
    events = []
    def key(seconds, scan):
        events.append({"visible_tick": int(seconds * 1e9),
                       "channel": "dos.keyboard.scancodes",
                       "payload": [scan, scan | 128]})
    def mouse(seconds, x, y, buttons=0):
        events.append({"visible_tick": int(seconds * 1e9),
                       "channel": "dos.mouse.normalized",
                       "payload": {"u": x / 319, "v": y / 199,
                                   "buttons": buttons}})
    key(5, 28)
    key(7, 0x4b)
    key(7.3, 28)
    mouse(7.55, 279, 137)
    # Hold long enough for a complete rotation; then visit the static views.
    # Escape activates the highlighted car button in the original menu.
    # Move from Next to Done before pressing it.
    mouse(24.8, 279, 115)
    key(25, 1)
    mouse(26, 218, 100, 1)
    mouse(26.15, 218, 100)
    mouse(26.5, 191, 139)
    key(29, 1)
    mouse(30, 40, 144, 1)
    mouse(30.15, 40, 144)
    mouse(30.5, 191, 139)
    path.write_text(json.dumps({"format": "portforge-exact-input-script-v1",
                                "anchor_tick": 0, "events": events}) + "\n")


class SelectionFrameTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.expected = references()
        exe, report = BUILD / "stunts.exe", BUILD / "build-report.json"
        newest_ns, newest = newest_build_input()
        if not exe.is_file() or not report.is_file():
            raise AssertionError("build the SDL3 game before selection verification")
        if min(exe.stat().st_mtime_ns, report.stat().st_mtime_ns) < newest_ns:
            raise AssertionError(f"SDL3 build predates {newest.relative_to(ROOT)}")
        cls.temporary = tempfile.TemporaryDirectory(prefix="stunts-selection-")
        folder = Path(cls.temporary.name)
        cls.captures = folder / "frames"
        script, trace = folder / "input.json", folder / "trace.jsonl"
        write_input(script)
        env = os.environ.copy()
        env.pop("STUNTS_ASSET_ROOT", None)
        env.update(SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
        env["PATH"] = os.pathsep.join((r"C:\msys64\mingw32\bin",
                                      r"C:\tools\sdl3-3.4.16-i686\bin",
                                      env.get("PATH", "")))
        try:
            run = subprocess.run([
                str(exe), f"--trace={trace}", f"--capture-dir={cls.captures}",
                f"--input-script={script}",
                f"--test-startup-seed={ROOT / 'tests/fixtures/sdl3/startup-seed.json'}",
                "--test-auto-protection", f"--run-ms={RUN_MS}",
            ], cwd=folder, env=env, capture_output=True, text=True, timeout=60)
            if run.returncode:
                raise AssertionError(f"selection run failed: {run.stdout}\n{run.stderr}")
            events = [json.loads(line) for line in trace.read_text().splitlines()]
            stops = [e for e in events if e.get("event_type") == "host_stop"]
            if len(stops) != 1 or stops[0].get("reason") != "requested run duration reached":
                raise AssertionError(f"selection run did not stop cleanly: {stops}")
            cls.actual = {}
            for path in cls.captures.glob("*.fbr"):
                pixels, palette = read_port_fbr(path)
                payload = pixels + palette
                cls.actual.setdefault(hashlib.sha256(payload).hexdigest(), payload)
            if not cls.actual:
                raise AssertionError("selection run produced no frame captures")
        except Exception:
            cls.temporary.cleanup()
            raise

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def assert_exact(self, label):
        expected = self.expected[label]
        digest = hashlib.sha256(expected).hexdigest()
        if digest not in self.actual:
            nearest = min((sum(a != b for a, b in zip(value[:64000], expected[:64000])),
                           sum(a != b for a, b in zip(value[64000:], expected[64000:])))
                          for value in self.actual.values())
            self.fail(f"{label}: nearest complete frame differs at {nearest[0]} indexed "
                      f"pixels and {nearest[1]} palette bytes")

    def test_car_countach_matches_original(self):
        self.assert_exact("car-countach")

    def test_default_track_matches_original(self):
        self.assert_exact("track-default")

    def test_opponent_clock_matches_original(self):
        self.assert_exact("opponent-clock")


if __name__ == "__main__":
    unittest.main()
