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
                self.assertNotIn("modern scene capture failed", process.stderr.lower())
                self.assertNotIn("modern renderer failed", process.stderr.lower())
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
                            # A following simulation frame proves this render
                            # completed before the publication stop boundary.
                            if int(e["game_frame"])+1 in states:
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

    def test_camera_switches_material_latch_and_replay_preserve_live_state(self):
        assert_build_ready()
        from tests.test_sdl3_diagnostics import compiler_path, make_test_environment
        environment = make_test_environment(compiler_path())
        with tempfile.TemporaryDirectory(prefix="stunts-camera-equivalence-") as directory:
            root = Path(directory)
            # F2/F3/F4 expose the car and its brake-light material latch. Return
            # to cockpit, then enter replay and seek/cycle cameras there.
            keys = [(1500,[28,156]),(3000,[28,156]),(4500,[28,156]),(6000,[72]),
                    (8000,[60,188]),(10000,[61,189]),(12000,[62,190]),
                    (14000,[59,187]),
                    (16000,[1,129]),(17500,[60,188]),(18500,[62,190]),
                    (19500,[28]),(20500,[156])]
            script = root / "input.json"
            script.write_text(json.dumps({"format":"portforge-exact-input-script-v1",
                "anchor_tick":0,"events":[{"visible_tick":ms*1000000,
                "channel":"dos.keyboard.scancodes","payload":codes} for ms,codes in keys]}))
            results = []
            for renderer in ("faithful","modern"):
                run = root / renderer
                run.mkdir()
                config = run / "config.json"
                config.write_text('{"manual_word_check":false}')
                result = subprocess.run([str(EXECUTABLE),f"--renderer={renderer}",
                    "--window=1680x720","--hidden",f"--config={config}",f"--assets={ASSETS}",
                    f"--trace={run / 'trace.jsonl'}",f"--capture-dir={run}",
                    f"--input-script={script}",f"--test-startup-seed={STARTUP_SEED}",
                    "--test-auto-protection","--run-ms=22000"],
                    cwd=ROOT,env=environment,capture_output=True,text=True,timeout=60)
                self.assertEqual(result.returncode,0,result.stderr)
                self.assertNotIn("failed",result.stderr.lower())
                events = read_trace(run / "trace.jsonl")
                snapshots = {int(e["sim_step_id"]):e["bytes_hex"] for e in events
                             if e.get("event_type")=="state_snapshot"}
                states = {int(e["game_frame"]):snapshots[int(e["sim_step_id"])]
                          for e in events if e.get("event_type")=="simulation_step"
                          and e.get("runtime_mode")=="live" and e.get("game_inputmode")==1}
                self.assertGreaterEqual(len(states),180)
                self.assertTrue(any(e.get("game_replay_mode")==1 for e in events))
                frames = {}
                for e in events:
                    if (e.get("event_type")!="video_publication" or e.get("game_mode")!=0
                        or e.get("game_inputmode")!=1 or e.get("publication_reason")!="sprite_blit_to_video"):
                        continue
                    tick=int(e["machine_tick"])
                    # Inputs are tied to the PIT timeline, while render starts
                    # between simulation ticks. Compare stable view intervals.
                    if any(abs(tick-ms*1000000)<250000000 for ms,_ in keys): continue
                    file=run / f"frame-{int(e['frame_id']):06d}.fbr"
                    if file.is_file(): frames[int(e["game_frame"])]=file.read_bytes()[20:]
                results.append((states,frames))
            states,frames=results[1];reference_states,reference_frames=results[0]
            common=states.keys()&reference_states.keys()
            self.assertGreaterEqual(len(common),180)
            for frame in common:self.assertEqual(states[frame],reference_states[frame],f"state {frame}")
            common=frames.keys()&reference_frames.keys()
            self.assertGreaterEqual(len(common),120)
            for frame in common:self.assertEqual(frames[frame],reference_frames[frame],f"indexed frame {frame}")
