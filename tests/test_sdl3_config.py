"""Runtime JSON policy, preserved user files, and the live protection boundary."""
from __future__ import annotations

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_diagnostics import compiler_path, make_test_environment, SDL_ROOT
from tests.test_sdl3_driving import (
    ROOT, ASSETS, EXECUTABLE, STARTUP_SEED, assert_build_ready,
    read_trace, decode_player_snapshot, write_driving_input,
)


class Sdl3ConfigTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if os.name != "nt":
            raise unittest.SkipTest("requires the Windows SDL3 runtime")
        compiler = compiler_path()
        if compiler is None or not (SDL_ROOT / "include/SDL3/SDL.h").is_file():
            raise unittest.SkipTest("requires GCC and SDL3 headers")
        cls.temporary = tempfile.TemporaryDirectory(prefix="stunts-config-")
        cls.root = Path(cls.temporary.name)
        cls.probe = cls.root / "config-probe.exe"
        cls.environment = make_test_environment(compiler)
        result = subprocess.run([
            compiler, "-std=gnu11", "-municode", "-Wall", "-Wextra", "-Werror",
            "-I", str(ROOT / "port"), "-I", str(SDL_ROOT / "include"),
            str(ROOT / "tests/sdl3/config_probe.c"), str(ROOT / "port/config.c"),
            "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(cls.probe),
        ], cwd=ROOT, env=cls.environment, capture_output=True, text=True, timeout=30)
        if result.returncode:
            raise AssertionError(result.stdout + result.stderr)

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def probe_file(self, path):
        result = subprocess.run([str(self.probe), str(path)], env=self.environment,
                                capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return tuple(map(int, result.stdout.strip().split()))

    def test_missing_file_creates_defaults_beside_requested_path(self):
        path = self.root / "First launch Žlutý config.json"
        self.assertEqual(self.probe_file(path), (1, 0))
        self.assertEqual(json.loads(path.read_text()), {"manual_word_check": False})
        saved = path.read_bytes()
        self.assertEqual(self.probe_file(path), (1, 0))
        self.assertEqual(path.read_bytes(), saved)

    def test_existing_settings_and_unknown_fields_are_preserved(self):
        path = self.root / "existing.json"
        cases = [
            (b'{"manual_word_check":true}', 1),
            (b'\xef\xbb\xbf{ "manual_word_check" : true, "other": [null, -1.2e+3, '
             b'{"x":"escaped \\\" value", "manual_word_check":false}] }\r\n', 1),
            (b'{"manual_word_\\u0063heck":true}', 1),
            (b'{"manual_wo\\rd_check":true}', 0),
            (b'{"manual_word_check":false}', 0),
            (b'{"nested":{"manual_word_check":true}}', 0),
            (b'{"text":"manual_word_check: true"}', 0),
            (b'{}', 0),
        ]
        for content, expected in cases:
            with self.subTest(content=content):
                path.write_bytes(content)
                self.assertEqual(self.probe_file(path), (1, expected))
                self.assertEqual(path.read_bytes(), content)

    def test_invalid_or_truncated_json_keeps_file_and_defaults(self):
        path = self.root / "invalid.json"
        complete = b'{"manual_word_check":true,"unknown":"backslash\\\\"}'
        invalid = [complete[:index] for index in range(len(complete))] + [
            b'{"manual_word_check":1}', b'{"manual_word_check":"true"}',
            b'{"manual_word_check":true,"manual_word_check":false}',
            b'{"manual_word_check":true}garbage', b'{"manual_word_check":true}\x00',
            b'{"manual_word_check":true,}', b'{"unknown":[01]}',
            b'{"unknown":"\\u00"}', b'{"unknown":"\\',
            b'{"unknown":' + b'[' * 34 + b'0' + b']' * 34 + b'}',
            b' ' * 65537,
        ]
        for content in invalid:
            with self.subTest(content=content[:100]):
                path.write_bytes(content)
                self.assertEqual(self.probe_file(path), (0, 0))
                self.assertEqual(path.read_bytes(), content)

    def test_unavailable_location_does_not_prevent_defaults(self):
        path = self.root / "missing-parent/config.json"
        self.assertEqual(self.probe_file(path), (0, 0))
        self.assertFalse(path.exists())
        self.assertEqual(self.probe_file(self.root), (0, 0))
        self.assertTrue(self.root.is_dir())

    def test_live_game_skips_by_default_and_restores_manual_prompt(self):
        assert_build_ready()
        with tempfile.TemporaryDirectory(prefix="stunts-config-game-") as temporary:
            run_dir = Path(temporary)
            input_path = run_dir / "input.json"
            write_driving_input(input_path)
            for enabled in (False, True):
                with self.subTest(enabled=enabled):
                    config = run_dir / f"config-{enabled}.json"
                    if enabled:
                        config.write_text('{"manual_word_check":true}\n')
                    trace = run_dir / f"trace-{enabled}.jsonl"
                    result = subprocess.run([
                        str(EXECUTABLE), f"--config={config}", f"--assets={ASSETS}",
                        f"--trace={trace}", f"--diagnostics-dir={run_dir / 'diagnostics'}",
                        f"--input-script={input_path}",
                        f"--test-startup-seed={STARTUP_SEED}", "--run-ms=10000",
                    ], cwd=run_dir, env=self.environment,
                        capture_output=True, text=True, timeout=30)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    events = read_trace(trace)
                    states = [decode_player_snapshot(event) for event in events
                              if event.get("event_type") == "state_snapshot"
                              and event.get("state_type") == "GAMESTATE"]
                    live = [state for state in states if state["inputmode"] == 1]
                    if enabled:
                        self.assertFalse(live, "original manual prompt should block the race")
                    else:
                        self.assertTrue(live, "first-launch defaults must reach a live race")
                        self.assertGreater(max(state["frame"] for state in live), 20)
                    self.assertEqual(json.loads(config.read_text()),
                                     {"manual_word_check": enabled})


if __name__ == "__main__":
    unittest.main()
