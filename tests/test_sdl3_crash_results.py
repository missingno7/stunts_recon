"""Seeded SDL3 drive through a crash, DNF results, and clean host stop."""
from __future__ import annotations

import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "sdl3"
EXECUTABLE = BUILD / "stunts.exe"
BUILD_REPORT = BUILD / "build-report.json"
ASSETS = BUILD / "runtime" / "assets"
STARTUP_SEED = ROOT / "tests" / "fixtures" / "sdl3" / "startup-seed.json"

RUN_MS = 40_000
THROTTLE_DOWN_NS = 6_000_000_000
THROTTLE_UP_NS = 35_000_000_000
ESCAPE_NS = 36_000_000_000

GAMESTATE_BYTES = 1120
GAME_TRAVEL_OFFSET = 316
GAME_FRAME_OFFSET = 320
GAME_IMPACT_SPEED_OFFSET = 332
GAME_JUMP_COUNT_OFFSET = 336
PLAYER_STATE_OFFSET = 338
PLAYER_SPEED_OFFSET = PLAYER_STATE_OFFSET + 42
PLAYER_SPEED2_OFFSET = PLAYER_STATE_OFFSET + 44
PLAYER_CRASH_FLAG_OFFSET = PLAYER_STATE_OFFSET + 0xC9
GAME_INPUTMODE_OFFSET = 1013

FBR_HEADER_BYTES = 20
FBR_WIDTH = 320
FBR_HEIGHT = 200
FBR_PALETTE_BYTES = 256 * 3
FBR_PIXELS_BYTES = FBR_WIDTH * FBR_HEIGHT


def write_crash_results_input(path: Path) -> None:
    """Enter the seeded race, hold throttle through the crash, then Escape."""
    events = [
        {"visible_tick": tick, "channel": "dos.keyboard.scancodes",
         "payload": [28, 156]}
        for tick in (1_500_000_000, 3_000_000_000, 4_500_000_000)
    ]
    events.extend([
        {"visible_tick": THROTTLE_DOWN_NS,
         "channel": "dos.keyboard.scancodes", "payload": [72]},
        {"visible_tick": THROTTLE_UP_NS,
         "channel": "dos.keyboard.scancodes", "payload": [200]},
        {"visible_tick": ESCAPE_NS,
         "channel": "dos.keyboard.scancodes", "payload": [1]},
    ])
    path.write_text(json.dumps({
        "format": "portforge-exact-input-script-v1",
        "anchor_tick": 0,
        "events": events,
    }, separators=(",", ":")) + "\n", encoding="ascii")


def assert_build_ready() -> None:
    if not EXECUTABLE.is_file() or not BUILD_REPORT.is_file():
        raise AssertionError(
            "SDL3 build is missing; run `python port/build.py build` first")
    if not ASSETS.is_dir() or not STARTUP_SEED.is_file():
        raise AssertionError("SDL3 runtime assets or startup seed fixture are missing")

    report = json.loads(BUILD_REPORT.read_text(encoding="utf-8"))
    if (report.get("target") != "i686-w64-mingw32" or
            report.get("game_c_objects") != 38 or
            report.get("compile", {}).get("object_passes") != 38 or
            report.get("function_stub_count") != 0 or
            report.get("data_stub_count") != 0):
        raise AssertionError(
            f"SDL3 build report is not the expected complete host build: {report}")

    # Require a complete report and executable newer than gameplay, rendering,
    # file, trace, and audio inputs exercised by this long-running scenario.
    required_inputs = (
        ROOT / "src" / "track_constants_module.c",
        ROOT / "src" / "obj_seg001_complete.c",
        ROOT / "src" / "obj_seg003.c",
        ROOT / "src" / "obj_seg004.c",
        ROOT / "src" / "obj_seg005.c",
        ROOT / "port" / "main.c",
        ROOT / "port" / "game_abi.py",
        ROOT / "port" / "sprite.c",
        ROOT / "port" / "sprite_aux.c",
        ROOT / "port" / "timer.c",
        ROOT / "port" / "input_script.c",
        ROOT / "port" / "trace.c",
        ROOT / "port" / "trace_hooks.c",
        ROOT / "port" / "file.c",
        ROOT / "port" / "build.py",
        ROOT / "port" / "dependencies.py",
        ROOT / "port" / "audio.c",
        ROOT / "port" / "audio_backend.h",
        ROOT / "port" / "port_opl3.c",
        ROOT / "port" / "port_opl3.h",
        ROOT / "layout" / "manifest.json",
        ROOT / "tools" / "porting" / "host_probe_modes.py",
        ROOT / "tools" / "porting" / "host_probe_declarations.py",
    )
    optional_audio_inputs = (
        ROOT / "port" / "ad15_driver.c",
        ROOT / "port" / "ad15_driver.h",
    )
    newest_candidates = required_inputs + tuple(
        path for path in optional_audio_inputs if path.is_file())
    missing = [str(path) for path in required_inputs if not path.is_file()]
    if missing:
        raise AssertionError(f"SDL3 build inputs are missing: {missing}")
    newest_input_ns = max(path.stat().st_mtime_ns for path in newest_candidates)
    if (BUILD_REPORT.stat().st_mtime_ns < newest_input_ns or
            EXECUTABLE.stat().st_mtime_ns < newest_input_ns):
        raise AssertionError(
            "SDL3 executable predates a gameplay/render/audio input; "
            "run `python port/build.py build` before this test")


def read_trace(path: Path) -> list[dict[str, object]]:
    try:
        return [json.loads(line) for line in path.read_text(
            encoding="utf-8").splitlines()]
    except (OSError, json.JSONDecodeError) as error:
        raise AssertionError(f"SDL3 crash/results trace is missing or malformed: {error}") from error


def decode_game_state(event: dict[str, object]) -> dict[str, int]:
    if (event.get("state_type") != "GAMESTATE" or
            event.get("state_size") != GAMESTATE_BYTES):
        raise AssertionError(f"unexpected gameplay state record: {event}")
    try:
        state = bytes.fromhex(str(event["bytes_hex"]))
    except (KeyError, ValueError) as error:
        raise AssertionError(f"malformed GAMESTATE bytes: {error}") from error
    if len(state) != GAMESTATE_BYTES:
        raise AssertionError(
            f"GAMESTATE has {len(state)} bytes, expected {GAMESTATE_BYTES}")
    return {
        "travel": struct.unpack_from("<i", state, GAME_TRAVEL_OFFSET)[0],
        "frame": struct.unpack_from("<H", state, GAME_FRAME_OFFSET)[0],
        "impact_speed": struct.unpack_from(
            "<H", state, GAME_IMPACT_SPEED_OFFSET)[0],
        "jump_count": struct.unpack_from(
            "<h", state, GAME_JUMP_COUNT_OFFSET)[0],
        "speed": struct.unpack_from("<H", state, PLAYER_SPEED_OFFSET)[0],
        "speed2": struct.unpack_from("<H", state, PLAYER_SPEED2_OFFSET)[0],
        "crash": state[PLAYER_CRASH_FLAG_OFFSET],
        "inputmode": state[GAME_INPUTMODE_OFFSET],
    }


def assert_results_frame(capture_dir: Path,
                         expected_frame_id: int) -> None:
    captures = sorted(capture_dir.glob("frame-*.fbr"))
    if not captures:
        raise AssertionError("SDL3 did not capture any indexed video frames")
    frame_path = captures[-1]
    data = frame_path.read_bytes()
    expected_size = FBR_HEADER_BYTES + FBR_PIXELS_BYTES + FBR_PALETTE_BYTES
    if len(data) != expected_size or data[:8] != b"STFBR1\0\0":
        raise AssertionError(
            f"final FBR is malformed: {frame_path.name}, {len(data)} bytes")
    width, height, palette_bytes, frame_id = struct.unpack_from("<HHII", data, 8)
    if (width, height, palette_bytes) != (FBR_WIDTH, FBR_HEIGHT, FBR_PALETTE_BYTES):
        raise AssertionError(
            f"unexpected final FBR geometry/palette: "
            f"{width}x{height}, {palette_bytes} palette bytes")
    if frame_id != expected_frame_id:
        raise AssertionError(
            f"latest captured FBR {frame_id} does not match final published "
            f"frame {expected_frame_id}")

    # The DNF/results screen uses a full-width gray information panel in the
    # lower 72 rows and a bright text table at the top. Checking both regions
    # distinguishes that screen from the road/cockpit and crash frames without
    # pinning every pixel of the rendering.
    pixels = data[FBR_HEADER_BYTES:FBR_HEADER_BYTES + FBR_PIXELS_BYTES]
    palette = data[FBR_HEADER_BYTES + FBR_PIXELS_BYTES:]
    gray_indices = {
        index for index in range(256)
        if palette[index * 3] == palette[index * 3 + 1] == palette[index * 3 + 2]
    }
    bright_indices = {
        index for index in range(256)
        if min(palette[index * 3:index * 3 + 3]) >= 50
    }
    lower_panel = pixels[128 * FBR_WIDTH:]
    gray_panel_pixels = sum(index in gray_indices for index in lower_panel)
    top_text_pixels = sum(index in bright_indices
                          for index in pixels[:64 * FBR_WIDTH])
    if gray_panel_pixels < 20_000 or top_text_pixels < 1_000:
        raise AssertionError(
            "final indexed frame is not the DNF/results screen: "
            f"gray lower-panel pixels={gray_panel_pixels}, "
            f"bright table pixels={top_text_pixels}")


class Sdl3CrashResultsTests(unittest.TestCase):
    def test_seeded_drive_crashes_shows_dnf_and_stops_cleanly(self) -> None:
        assert_build_ready()
        environment = os.environ.copy()
        environment["SDL_VIDEODRIVER"] = "dummy"
        environment["SDL_AUDIODRIVER"] = "dummy"
        environment["PATH"] = os.pathsep.join((
            r"C:\msys64\mingw32\bin",
            r"C:\tools\sdl3-3.4.16-i686\bin",
            environment.get("PATH", ""),
        ))

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-crash-results-") as temp:
            run_dir = Path(temp)
            trace_path = run_dir / "trace.jsonl"
            input_path = run_dir / "input.json"
            capture_dir = run_dir / "captures"
            capture_dir.mkdir()
            write_crash_results_input(input_path)
            command = [
                str(EXECUTABLE),
                f"--trace={trace_path}",
                f"--assets={ASSETS}",
                f"--input-script={input_path}",
                f"--test-startup-seed={STARTUP_SEED}",
                "--test-auto-protection",
                f"--run-ms={RUN_MS}",
                f"--capture-dir={capture_dir}",
            ]
            try:
                result = subprocess.run(
                    command, cwd=run_dir, env=environment,
                    capture_output=True, text=True, timeout=65, check=False)
            except subprocess.TimeoutExpired as error:
                self.fail(
                    "SDL3 crash/results run exceeded its 65-second bound; "
                    f"partial stdout={error.stdout!r}, stderr={error.stderr!r}")

            self.assertEqual(
                result.returncode, 0,
                f"SDL3 crash/results run exited {result.returncode}; "
                f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}")
            self.assertIn("PORT test startup seed applied", result.stderr)
            self.assertNotIn("unresolved host service:", result.stderr)
            self.assertNotIn("PORT fatal stub", result.stderr)

            events = read_trace(trace_path)
            stop_events = [event for event in events
                           if event.get("event_type") == "host_stop"]
            self.assertEqual(len(stop_events), 1, stop_events)
            self.assertEqual(
                stop_events[0].get("reason"), "requested run duration reached",
                f"guest stopped before the bounded run completed: {stop_events[0]}")
            self.assertGreaterEqual(int(stop_events[0]["machine_tick"]),
                                    RUN_MS * 1_000_000)

            inputs = [event for event in events if event.get("event_type") == "input"]
            enter_events = [event for event in inputs
                            if event.get("payload") == [28, 156]]
            self.assertEqual(
                len(enter_events), 3,
                f"expected three race-entry inputs, observed {enter_events}")
            throttle_down = next(
                (event for event in inputs if event.get("payload") == [72]), None)
            throttle_up = next(
                (event for event in inputs if event.get("payload") == [200]), None)
            escape = next(
                (event for event in inputs if event.get("payload") == [1]), None)
            self.assertIsNotNone(throttle_down, "keypad 8 down was not traced")
            self.assertIsNotNone(throttle_up, "keypad 8 break was not traced")
            self.assertIsNotNone(escape, "Escape was not traced")
            down_tick = int(throttle_down["machine_tick"])
            up_tick = int(throttle_up["machine_tick"])
            escape_tick = int(escape["machine_tick"])
            self.assertLess(down_tick, up_tick)
            self.assertLess(up_tick, escape_tick)

            snapshots = {
                int(event["sim_step_id"]): decode_game_state(event)
                for event in events
                if event.get("event_type") == "state_snapshot"
            }
            steps = [event for event in events
                     if event.get("event_type") == "simulation_step"]
            crash_step_ids = [
                step_id for step_id, state in snapshots.items()
                if state["crash"] != 0
            ]
            self.assertTrue(crash_step_ids, "the player never entered the crash state")
            first_crash_step_id = min(crash_step_ids)
            held_steps = [
                event for event in steps
                if event.get("runtime_mode") == "live"
                and event.get("game_mode") == 0
                and event.get("game_inputmode") == 1
                and down_tick <= int(event.get("machine_tick", -1)) < up_tick
                and int(event.get("sim_step_id", -1)) in snapshots
            ]
            self.assertGreaterEqual(
                len(held_steps), 100,
                "held throttle did not produce the long live drive needed to reach "
                f"the obstacle; observed {len(held_steps)} live steps")
            accelerating_steps = [
                event for event in held_steps
                if int(event["sim_step_id"]) < first_crash_step_id
            ]
            self.assertGreaterEqual(
                len(accelerating_steps), 80,
                "the crash began before the test observed a sustained acceleration")
            first_state = snapshots[int(accelerating_steps[0]["sim_step_id"])]
            last_accelerating_state = snapshots[
                int(accelerating_steps[-1]["sim_step_id"])]
            self.assertGreater(
                last_accelerating_state["travel"], first_state["travel"])
            self.assertGreater(
                last_accelerating_state["speed2"], first_state["speed2"])

            crash_states = [
                snapshots[step_id] for step_id in sorted(crash_step_ids)
            ]
            self.assertTrue(crash_states, "the player never entered the crash state")
            crash_state = crash_states[0]
            self.assertGreater(
                crash_state["impact_speed"], 0,
                f"crash state had no impact speed: {crash_state}")
            # The locked mode-1 crash retains the captured impact speed and
            # decelerates during the animation. Modes 2/5 suppress it at once.
            # See test_sdl3_crash_oracle for the original-machine contracts.
            self.assertIn(crash_state["speed2"],
                          (0, crash_state["impact_speed"]),
                          f"crash did not capture or suppress impact speed: {crash_state}")
            # update_car_speed can resynchronize speed2 from the independently
            # braking speed word: the locked 588/0 case becomes 328/328.
            # A global monotonicity assertion would reject original behavior.
            self.assertEqual(crash_states[-1]["speed2"], 0,
                             "crash animation never decelerated to a stop")
            self.assertGreaterEqual(len(crash_states), 5)
            self.assertTrue(all(state["speed"] == state["speed2"] == 0
                                for state in crash_states[-5:]),
                            "crash animation did not remain stationary at its end")
            self.assertLess(
                min(crash_step_ids),
                max(crash_step_ids),
                "crash state did not remain observable in later snapshots")

            publications = [event for event in events
                            if event.get("event_type") == "video_publication"]
            results = [
                event for event in publications
                if event.get("game_replay_mode") == 1
                and int(event.get("machine_tick", -1)) < escape_tick
            ]
            self.assertTrue(
                results,
                "the crashed run never published its DNF/results presentation")
            first_results_tick = int(results[0]["machine_tick"])
            crash_tick = min(
                int(event["machine_tick"]) for event in steps
                if int(event.get("sim_step_id", -1)) in crash_step_ids)
            self.assertGreater(first_results_tick, crash_tick)
            self.assertLessEqual(
                first_results_tick - crash_tick, 8_000_000_000,
                "crash results did not appear within eight seconds of impact")
            self.assertIs(
                publications[-1], results[-1],
                "the final published frame did not remain on the results screen")
            assert_results_frame(capture_dir, int(publications[-1]["frame_id"]))


if __name__ == "__main__":
    unittest.main()
