"""Recorded mouse-device coordinates and complete UI input streams."""
from pathlib import Path
import json
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SDK = Path(os.environ.get("SDL3_ROOT", r"C:/tools/sdl3-3.4.16-i686"))
CC = Path(r"C:/msys64/mingw32/bin/gcc.exe")


class Sdl3InputReplayTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(
            prefix="input-replay-", dir=ROOT / "build" / "workers")
        cls.directory = Path(cls.temp.name)
        cls.executable = cls.directory / "input_script_harness.exe"
        cls.environment = os.environ.copy()
        cls.environment["PATH"] = os.pathsep.join(
            [str(CC.parent), str(SDK / "bin"), cls.environment.get("PATH", "")])
        command = [
            str(CC), "-std=gnu11", "-O2", "-DPORT_BUILD=1", "-include",
            str(ROOT / "tools/porting/host/compat.h"),
            "-I", str(ROOT / "port"),
            "-I", str(ROOT / "tools/porting/port_include"),
            "-I", str(ROOT / "tools/porting/host/include"),
            "-I", str(SDK / "include"),
            str(ROOT / "tests/sdl3/input_script_harness.c"),
            "-L", str(SDK / "lib"), "-lSDL3", "-o", str(cls.executable),
        ]
        result = subprocess.run(command, env=cls.environment, capture_output=True,
                                text=True, timeout=60)
        if result.returncode:
            cls.temp.cleanup()
            raise AssertionError(result.stdout + result.stderr)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_script(self, events):
        path = self.directory / "script.json"
        path.write_text(json.dumps({
            "format": "portforge-exact-input-script-v1", "anchor_tick": 0,
            "events": events}, separators=(",", ":")), encoding="ascii")
        result = subprocess.run([str(self.executable), str(path)],
                                env=self.environment, capture_output=True,
                                text=True, timeout=20)
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout.splitlines()

    def test_recorded_mouse_position_uses_original_device_bounds(self):
        # sdltst2 checkpoint 2699: devices.mouse=(102,130), last applied
        # input seq2978. PortForge normalizes inclusive 0..320/0..200 bounds.
        positions = [(0.31875, 0.6513888889), (0, 0), (1, 1)]
        events = [{"visible_tick": i, "channel": "dos.mouse.normalized",
                   "payload": {"u": u, "v": v, "buttons": i % 2}}
                  for i, (u, v) in enumerate(positions)]
        self.assertEqual(self.run_script(events),
                         ["M 0 102 130 0", "M 1 0 0 1", "M 2 320 200 0"])

    def test_ui_recording_sized_stream_keeps_every_event_and_key_pulse(self):
        # The supplied UI recording has 3039 observations; retaining all of
        # them avoids moving hover positions across hit-test boundaries.
        events = []
        expected = []
        for i in range(3039):
            keyboard = i % 89 == 0
            payload = [80, 208] if keyboard else {
                "u": 0.31875, "v": 0.6513888889, "buttons": i % 2}
            events.append({"visible_tick": i * 10000000,
                           "channel": "dos.keyboard.scancodes" if keyboard
                           else "dos.mouse.normalized", "payload": payload})
            expected.append(f"K {i} 80 208" if keyboard
                            else f"M {i} 102 130 {i % 2}")
        self.assertEqual(self.run_script(events), expected)


if __name__ == "__main__":
    unittest.main()
