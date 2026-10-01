"""Renderer and aspect must leave authoritative live GAMESTATE bytes unchanged."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_driving import (
    ROOT, EXECUTABLE, ASSETS, STARTUP_SEED, assert_build_ready, read_trace,
)


class RendererEquivalenceTests(unittest.TestCase):
    def test_live_state_and_faithful_framebuffer_across_renderer_aspects(self):
        assert_build_ready()
        environment = os.environ.copy()
        environment.update(SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
        environment["PATH"] = os.pathsep.join((
            r"C:\msys64\mingw32\bin", r"C:\tools\sdl3-3.4.16-i686\bin",
            environment.get("PATH", "")))
        with tempfile.TemporaryDirectory(prefix="stunts-render-equivalence-") as directory:
            root = Path(directory)
            script = root / "input.json"
            script.write_text(json.dumps({
                "format": "portforge-exact-input-script-v1", "anchor_tick": 0,
                "events": [{"visible_tick": tick, "channel": "dos.keyboard.scancodes",
                            "payload": [28, 156]}
                           for tick in (1500000000, 3000000000, 4500000000)] +
                          [{"visible_tick": 6000000000, "channel": "dos.keyboard.scancodes",
                            "payload": [72]}],
            }), encoding="ascii")
            results = []
            for renderer, size in (("faithful", "960x720"), ("modern", "960x720"),
                                   ("modern", "1280x720"), ("modern", "1680x720")):
                run = root / f"{renderer}-{size}"
                run.mkdir()
                config = run / "config.json"
                config.write_text('{"manual_word_check":false}', encoding="ascii")
                process = subprocess.run([
                    str(EXECUTABLE), f"--renderer={renderer}", f"--window={size}",
                    "--hidden", f"--config={config}", f"--assets={ASSETS}",
                    f"--trace={run / 'trace.jsonl'}", f"--capture-dir={run}",
                    f"--input-script={script}", f"--test-startup-seed={STARTUP_SEED}",
                    "--test-auto-protection", "--stop-after-sim-steps=160", "--run-ms=20000",
                ], cwd=ROOT, env=environment, capture_output=True, text=True, timeout=60)
                self.assertEqual(process.returncode, 0, process.stderr)
                self.assertNotIn("contract failure", process.stderr.lower())
                self.assertNotIn("modern frame rejected", process.stderr.lower())
                self.assertNotIn("modern scene rejected", process.stderr.lower())
                events = read_trace(run / "trace.jsonl")
                snapshots = {int(e["sim_step_id"]): e["bytes_hex"] for e in events
                             if e.get("event_type") == "state_snapshot"}
                states = {int(e["game_frame"]): snapshots[int(e["sim_step_id"])]
                          for e in events if e.get("event_type") == "simulation_step"
                          and e.get("runtime_mode") == "live"
                          and e.get("game_inputmode") == 1}
                self.assertGreaterEqual(len(states), 100)
                frames = {}
                for e in events:
                    if (e.get("event_type") == "video_publication"
                            and e.get("game_inputmode") == 1
                            and e.get("publication_reason") == "sprite_blit_to_video"):
                        file = run / f"frame-{int(e['frame_id']):06d}.fbr"
                        if file.is_file():
                            frames[int(e["game_frame"])] = file.read_bytes()
                self.assertGreaterEqual(len(frames), 90)
                results.append((states, frames))
            reference_states, reference_frames = results[0]
            for states, frames in results[1:]:
                common = reference_states.keys() & states.keys()
                self.assertGreaterEqual(len(common), 100)
                for frame in common:
                    self.assertEqual(states[frame], reference_states[frame], f"GAMESTATE frame {frame}")
                common = reference_frames.keys() & frames.keys()
                self.assertGreaterEqual(len(common), 90)
                for frame in common:
                    # FBR's 20-byte header includes the asynchronous publication ID.
                    # Compare every indexed pixel and all 768 RGB6 palette bytes.
                    self.assertEqual(frames[frame][20:], reference_frames[frame][20:],
                                     f"faithful framebuffer frame {frame}")
