"""Sustained seeded SDL3 driving smoke test."""
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
RUN_MS = 18_000
THROTTLE_DOWN_NS = 6_000_000_000
THROTTLE_UP_NS = 11_000_000_000
STEER_LEFT_DOWN_NS = 11_300_000_000
STEER_LEFT_UP_NS = 12_100_000_000
STEER_RIGHT_DOWN_NS = 12_300_000_000
STEER_RIGHT_UP_NS = 13_100_000_000
BRAKE_DOWN_NS = 13_300_000_000
BRAKE_UP_NS = 14_300_000_000
ESCAPE_NS = 15_300_000_000
REPLAY_DOWN_NS = 15_700_000_000
REPLAY_DOWN_UP_NS = 15_900_000_000
REPLAY_LEFT_NS = 16_100_000_000
REPLAY_LEFT_UP_NS = 16_300_000_000
REPLAY_SEEK_DOWN_NS = 16_500_000_000
REPLAY_SEEK_UP_NS = 17_500_000_000
GAMESTATE_BYTES = 1120
GAME_TRAVEL_OFFSET = 316
GAME_FRAME_OFFSET = 320
PLAYER_STATE_OFFSET = 338
PLAYER_SPEED_OFFSET = PLAYER_STATE_OFFSET + 42
PLAYER_SPEED2_OFFSET = PLAYER_STATE_OFFSET + 44


def write_driving_input(path: Path) -> None:
    """Drive the seeded race, enter replay, and seek through recorded frames."""
    script = {
        "format": "portforge-exact-input-script-v1",
        "anchor_tick": 0,
        "events": [
            {"visible_tick": tick, "channel": "dos.keyboard.scancodes",
             "payload": [28, 156]}
            for tick in (1_500_000_000, 3_000_000_000, 4_500_000_000)
        ] + [
            {"visible_tick": THROTTLE_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [72]},
            {"visible_tick": THROTTLE_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [200]},
            {"visible_tick": STEER_LEFT_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [75]},
            {"visible_tick": STEER_LEFT_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [203]},
            {"visible_tick": STEER_RIGHT_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [77]},
            {"visible_tick": STEER_RIGHT_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [205]},
            {"visible_tick": BRAKE_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [80]},
            {"visible_tick": BRAKE_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [208]},
            {"visible_tick": ESCAPE_NS,
             "channel": "dos.keyboard.scancodes", "payload": [1]},
            {"visible_tick": REPLAY_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [80]},
            {"visible_tick": REPLAY_DOWN_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [208]},
            {"visible_tick": REPLAY_LEFT_NS,
             "channel": "dos.keyboard.scancodes", "payload": [75]},
            {"visible_tick": REPLAY_LEFT_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [203]},
            {"visible_tick": REPLAY_SEEK_DOWN_NS,
             "channel": "dos.keyboard.scancodes", "payload": [28]},
            {"visible_tick": REPLAY_SEEK_UP_NS,
             "channel": "dos.keyboard.scancodes", "payload": [156]},
        ],
    }
    path.write_text(json.dumps(script, separators=(",", ":")) + "\n",
                    encoding="ascii")


def assert_build_ready() -> None:
    if not EXECUTABLE.is_file() or not BUILD_REPORT.is_file():
        raise AssertionError(
            "SDL3 build is missing; run `python port/build.py build` first")
    if not ASSETS.is_dir() or not STARTUP_SEED.is_file():
        raise AssertionError("SDL3 runtime assets or startup seed fixture are missing")

    report = json.loads(BUILD_REPORT.read_text(encoding="utf-8"))
    if (report.get("target") != "i686-w64-mingw32" or
            report.get("game_c_objects") != 38 or
            report.get("port_objects") != 32 or
            report.get("compile", {}).get("object_passes") != 38):
        raise AssertionError(
            f"SDL3 build report is not the expected complete host build: {report}")

    # A complete report is only useful when it postdates the code paths this
    # smoke test exercises. The report is written after the executable links.
    required_inputs = (
        ROOT / "src" / "track_constants_module.c",
        ROOT / "src" / "obj_seg003.c",
        ROOT / "src" / "obj_seg004.c",
        ROOT / "src" / "obj_seg005.c",
        ROOT / "src" / "obj_seg001_complete.c",
        ROOT / "port" / "main.c",
        ROOT / "port" / "diagnostics.c",
        ROOT / "port" / "config.c",
        ROOT / "port" / "game_abi.py",
        ROOT / "port" / "sprite.c",
        ROOT / "port" / "sprite_aux.c",
        ROOT / "port" / "timer.c",
        ROOT / "port" / "input_script.c",
        ROOT / "port" / "trace.c",
        ROOT / "port" / "trace_hooks.c",
        ROOT / "port" / "build.py",
        ROOT / "port" / "dependencies.py",
        ROOT / "port" / "audio.c",
        ROOT / "port" / "audio_backend.h",
        ROOT / "port" / "ad15_driver.c",
        ROOT / "port" / "ad15_driver.h",
        ROOT / "port" / "port_opl3.c",
        ROOT / "port" / "port_opl3.h",
        ROOT / "layout" / "manifest.json",
        ROOT / "tools" / "porting" / "host_probe_modes.py",
        ROOT / "tools" / "porting" / "host_probe_declarations.py",
    )
    missing = [str(path) for path in required_inputs if not path.is_file()]
    if missing:
        raise AssertionError(f"SDL3 build inputs are missing: {missing}")
    newest_input_ns = max(path.stat().st_mtime_ns for path in required_inputs)
    if (BUILD_REPORT.stat().st_mtime_ns < newest_input_ns or
            EXECUTABLE.stat().st_mtime_ns < newest_input_ns):
        raise AssertionError(
            "SDL3 executable predates a gameplay/timer input; "
            "run `python port/build.py build` before this test")


def read_trace(path: Path) -> list[dict[str, object]]:
    try:
        return [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines()]
    except (OSError, json.JSONDecodeError) as error:
        raise AssertionError(f"SDL3 driving trace is missing or malformed: {error}") from error


def decode_player_snapshot(event: dict[str, object]) -> dict[str, int]:
    if event.get("state_type") != "GAMESTATE" or event.get("state_size") != GAMESTATE_BYTES:
        raise AssertionError(f"unexpected gameplay state record: {event}")
    try:
        state = bytes.fromhex(str(event["bytes_hex"]))
    except (KeyError, ValueError) as error:
        raise AssertionError(f"malformed GAMESTATE bytes: {error}") from error
    if len(state) != GAMESTATE_BYTES:
        raise AssertionError(f"GAMESTATE has {len(state)} bytes, expected {GAMESTATE_BYTES}")
    return {
        "travel": struct.unpack_from("<i", state, GAME_TRAVEL_OFFSET)[0],
        "frame": struct.unpack_from("<H", state, GAME_FRAME_OFFSET)[0],
        "speed": struct.unpack_from("<H", state, PLAYER_SPEED_OFFSET)[0],
        "speed2": struct.unpack_from("<H", state, PLAYER_SPEED2_OFFSET)[0],
        "steering": struct.unpack_from("<h", state, PLAYER_STATE_OFFSET + 0x20)[0],
        "braking": state[PLAYER_STATE_OFFSET + 0xBC],
        "accelerating": state[PLAYER_STATE_OFFSET + 0xBD],
        "inputmode": state[1013],
    }


class Sdl3DrivingTests(unittest.TestCase):
    def test_keypad_controls_drive_and_exit_live_race(self) -> None:
        assert_build_ready()
        environment = os.environ.copy()
        environment["SDL_VIDEODRIVER"] = "dummy"
        environment["SDL_AUDIODRIVER"] = "dummy"
        environment["PATH"] = os.pathsep.join((
            r"C:\msys64\mingw32\bin",
            r"C:\tools\sdl3-3.4.16-i686\bin",
            environment.get("PATH", ""),
        ))

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-driving-") as temp:
            run_dir = Path(temp)
            trace_path = run_dir / "trace.jsonl"
            input_path = run_dir / "input.json"
            write_driving_input(input_path)
            command = [
                str(EXECUTABLE),
                f"--trace={trace_path}",
                f"--assets={ASSETS}",
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
                    "SDL3 driving run exceeded its 60-second bound; "
                    f"partial stdout={error.stdout!r}, stderr={error.stderr!r}")

            self.assertEqual(
                result.returncode, 0,
                f"SDL3 driving run exited {result.returncode}; "
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

            inputs = [event for event in events if event.get("event_type") == "input"]
            self.assertGreaterEqual(
                sum(event.get("payload") == [28, 156] for event in inputs), 3,
                "script did not enter the seeded race three times")
            throttle_down = next(
                (event for event in inputs if event.get("payload") == [72]), None)
            throttle_up = next(
                (event for event in inputs if event.get("payload") == [200]), None)
            steering_left_down = next(
                (event for event in inputs if event.get("payload") == [75]), None)
            steering_left_up = next(
                (event for event in inputs if event.get("payload") == [203]), None)
            steering_right_down = next(
                (event for event in inputs if event.get("payload") == [77]), None)
            steering_right_up = next(
                (event for event in inputs if event.get("payload") == [205]), None)
            brake_down = next(
                (event for event in inputs if event.get("payload") == [80]), None)
            brake_up = next(
                (event for event in inputs if event.get("payload") == [208]), None)
            escape = next(
                (event for event in inputs if event.get("payload") == [1]), None)
            replay_down = next(
                (event for event in inputs if event.get("payload") == [80]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            replay_down_up = next(
                (event for event in inputs if event.get("payload") == [208]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            replay_left = next(
                (event for event in inputs if event.get("payload") == [75]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            replay_left_up = next(
                (event for event in inputs if event.get("payload") == [203]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            replay_seek_down = next(
                (event for event in inputs if event.get("payload") == [28]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            replay_seek_up = next(
                (event for event in inputs if event.get("payload") == [156]
                 and int(event.get("machine_tick", -1)) > ESCAPE_NS), None)
            self.assertIsNotNone(throttle_down, "keypad 8 down was not traced")
            self.assertIsNotNone(throttle_up, "keypad 8 break was not traced")
            for name, event in (
                ("keypad 4 down", steering_left_down),
                ("keypad 4 break", steering_left_up),
                ("keypad 6 down", steering_right_down),
                ("keypad 6 break", steering_right_up),
                ("keypad 2 down", brake_down),
                ("keypad 2 break", brake_up),
                ("Escape", escape),
                ("replay-button down", replay_down),
                ("replay-button down break", replay_down_up),
                ("replay-button left", replay_left),
                ("replay-button left break", replay_left_up),
                ("replay seek Enter", replay_seek_down),
                ("replay seek Enter break", replay_seek_up),
            ):
                self.assertIsNotNone(event, f"{name} was not traced")
            down_tick = int(throttle_down["machine_tick"])
            up_tick = int(throttle_up["machine_tick"])
            self.assertLess(down_tick, up_tick)

            snapshots = {
                int(event["sim_step_id"]): decode_player_snapshot(event)
                for event in events
                if event.get("event_type") == "state_snapshot"
            }
            steps = [event for event in events
                     if event.get("event_type") == "simulation_step"]
            held = [
                event for event in steps
                if event.get("runtime_mode") == "live"
                and event.get("game_mode") == 0
                and event.get("game_inputmode") == 1
                and down_tick <= int(event.get("machine_tick", -1)) < up_tick
                and int(event.get("sim_step_id", -1)) in snapshots
            ]
            self.assertGreaterEqual(
                len(held), 20,
                "held throttle did not produce sustained live simulation steps; "
                f"observed {len(held)} in the five-second key-down window")
            step_times = [int(event["host_ns"]) for event in held]
            self.assertGreaterEqual(
                step_times[-1] - step_times[0], 3_500_000_000,
                "live steps did not span the held-throttle interval: "
                f"{step_times[0]} .. {step_times[-1]}")
            largest_gap = max(after - before
                              for before, after in zip(step_times, step_times[1:]))
            self.assertLessEqual(
                largest_gap, 500_000_000,
                f"held-throttle simulation stalled for {largest_gap / 1e9:.2f}s")

            states = [snapshots[int(event["sim_step_id"])] for event in held]
            frames = [state["frame"] for state in states]
            self.assertTrue(
                all(after > before for before, after in zip(frames, frames[1:])),
                f"game frame did not advance on consecutive live steps: {frames[:12]} … {frames[-4:]}")
            self.assertTrue(
                all(state["inputmode"] == 1 for state in states),
                "GAMESTATE disagreed with the trace's live input mode")
            self.assertGreater(
                states[-1]["speed2"], states[0]["speed2"],
                f"player speed did not rise under held throttle: "
                f"{states[0]['speed2']} -> {states[-1]['speed2']}")
            self.assertGreater(
                states[-1]["travel"], states[0]["travel"],
                f"player travel did not grow under held throttle: "
                f"{states[0]['travel']} -> {states[-1]['travel']}")

            def states_during(down: dict[str, object],
                              up: dict[str, object]) -> list[dict[str, int]]:
                begin = int(down["machine_tick"])
                end = int(up["machine_tick"])
                selected = [
                    snapshots[int(event["sim_step_id"])]
                    for event in steps
                    if event.get("event_type") == "simulation_step"
                    and event.get("runtime_mode") == "live"
                    and event.get("game_mode") == 0
                    and event.get("game_inputmode") == 1
                    and begin <= int(event.get("machine_tick", -1)) < end
                    and int(event.get("sim_step_id", -1)) in snapshots
                ]
                self.assertGreaterEqual(
                    len(selected), 5,
                    f"held key window {begin}..{end} produced only {len(selected)} live steps")
                return selected

            left_states = states_during(steering_left_down, steering_left_up)
            self.assertLess(
                min(state["steering"] for state in left_states), -10,
                "keypad 4 did not steer the player left")
            right_states = states_during(steering_right_down, steering_right_up)
            self.assertGreater(
                max(state["steering"] for state in right_states), 10,
                "keypad 6 did not steer the player right")
            brake_states = states_during(brake_down, brake_up)
            self.assertTrue(
                any(state["braking"] != 0 for state in brake_states),
                "keypad 2 did not set the player's braking state")
            self.assertLess(
                brake_states[-1]["speed2"], brake_states[0]["speed2"],
                "player speed did not fall while the brake was held")

            escape_tick = int(escape["machine_tick"])
            post_escape_live = [
                event for event in steps
                if event.get("event_type") == "simulation_step"
                and event.get("runtime_mode") == "live"
                and int(event.get("machine_tick", -1)) > escape_tick + 500_000_000
            ]
            self.assertFalse(
                post_escape_live,
                "Escape did not leave the live race within 500 ms: "
                f"{post_escape_live[:3]}")

            publications = [event for event in events
                            if event.get("event_type") == "video_publication"]
            replay_publications = [
                event for event in publications
                if int(event.get("machine_tick", -1)) > escape_tick
                and event.get("game_mode") == 2
                and event.get("game_replay_mode") == 1
            ]
            self.assertGreaterEqual(
                len(replay_publications), 2,
                "Escape stopped live physics without publishing the replay UI; "
                f"post-Escape replay publications: {replay_publications[:3]}")
            first_replay_tick = int(replay_publications[0]["machine_tick"])
            self.assertLessEqual(
                first_replay_tick - escape_tick, 500_000_000,
                "Escape did not enter replay presentation within 500 ms: "
                f"Escape={escape_tick}, first replay={first_replay_tick}")
            self.assertIs(
                publications[-1], replay_publications[-1],
                "the final published UI frame did not remain in replay mode")

            replay_seek_up_tick = int(replay_seek_up["machine_tick"])
            replay_states = [
                snapshots[int(event["sim_step_id"])]
                for event in steps
                if event.get("game_mode") == 2
                and event.get("game_replay_mode") == 1
                and int(event.get("machine_tick", -1)) >= replay_seek_up_tick
                and int(event.get("sim_step_id", -1)) in snapshots
            ]
            replay_frames = [state["frame"] for state in replay_states]
            self.assertGreaterEqual(
                len(replay_states), 20,
                "the replay seek did not advance recorded GAMESTATE snapshots")
            self.assertGreaterEqual(
                max(replay_frames) - min(replay_frames), 50,
                "the replay seek did not move substantially through recorded frames: "
                f"{replay_frames[:5]} … {replay_frames[-5:]}")


if __name__ == "__main__":
    unittest.main()
