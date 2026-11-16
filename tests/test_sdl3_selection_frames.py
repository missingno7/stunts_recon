"""Full indexed selection screens from independently verified DOS replays.

Each frame is compared at its named scoped key checkpoint. No image search or
IRQ equivalence is used.
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
LABELS = ("car-countach", "car-lancia", "track-default", "opponent-clock")
RUN_MS = 38_000
RUN_TICKS = RUN_MS // 10
TICK_NS = 10000154


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
    # The recovered mouse activation uses held-button repeat timing.
    # Hover to select, then use a queued Enter instead of a 150 ms pulse.
    mouse(26, 218, 100)
    key(26.3, 28)
    mouse(28, 191, 139)
    key(29, 1)
    mouse(30, 40, 144)
    key(30.3, 28)
    # Keep the selected hitbox stable while the guest consumes Enter.
    # A move onto the main-menu Drive hitbox must follow that transition.
    mouse(33, 191, 139)
    path.write_text(json.dumps({"format": "stunts-exact-input-script-v1",
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
        from tools.porting.scenario_oracle import script,run_port
        from tools.porting.compare_scenarios import port_data
        cls.actual={}
        try:
            for name in ('selection','opponent'):
                input_path=folder/(name+'.json');script(name,input_path)
                actual=port_data(run_port(folder/name,input_path,'ad15',1000))
                if actual['terminal']!='scenario checkpoint reached':
                    raise AssertionError(name+' did not reach its semantic terminal')
                cls.actual.update(actual['payloads'])
        except Exception:
            cls.temporary.cleanup()
            raise

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def assert_exact(self, label):
        expected = self.expected[label]
        self.assertIn(label,self.actual)
        self.assertEqual(self.actual[label],expected,label+' named checkpoint indices and palette')

    def test_car_countach_matches_original(self):
        self.assert_exact("car-countach")

    def test_car_lancia_matches_original(self):
        self.assert_exact("car-lancia")

    def test_default_track_matches_original(self):
        self.assert_exact("track-default")

    def test_opponent_clock_matches_original(self):
        self.assert_exact("opponent-clock")


if __name__ == "__main__":
    unittest.main()
