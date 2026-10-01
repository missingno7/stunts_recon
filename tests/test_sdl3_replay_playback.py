"""Bounded real-time SDL3 replay play, pause, seek, and resume regression."""
from __future__ import annotations

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_driving import (
    ASSETS, BUILD_REPORT, EXECUTABLE, ESCAPE_NS, ROOT, STARTUP_SEED,
    assert_build_ready, decode_player_snapshot, write_driving_input,
)


RUN_MS = 25_500
REWIND_DOWN_NS = 15_700_000_000
REWIND_UP_NS = 18_200_000_000
FIRST_PLAY_DOWN_NS = 18_500_000_000
FIRST_PLAY_UP_NS = 18_650_000_000
PAUSE_DOWN_NS = 21_000_000_000
PAUSE_UP_NS = 21_150_000_000
SEEK_DOWN_NS = 21_800_000_000
SEEK_UP_NS = 22_400_000_000
RESUME_DOWN_NS = 22_700_000_000
RESUME_UP_NS = 22_850_000_000


def mouse_event(tick: int, x: int, y: int, buttons: int) -> dict[str, object]:
    return {
        "visible_tick": tick,
        "channel": "dos.mouse.normalized",
        "payload": {"u": x / 319, "v": y / 199, "buttons": buttons},
    }


def write_replay_input(path: Path) -> None:
    """Record a race, rewind to its start, and exercise replay toolbar actions."""
    write_driving_input(path)
    script = json.loads(path.read_text(encoding="ascii"))
    # The established driving script seeks while paused. Replace that seek
    # section with explicit replay-toolbar clicks so this test exercises live
    # playback as well as the pause and seek paths.
    events = [event for event in script["events"]
              if int(event["visible_tick"]) <= ESCAPE_NS]
    events.extend([
        # Toolbar index 1 rewinds the recorded race to frame zero.
        mouse_event(REWIND_DOWN_NS, 130, 185, 1),
        mouse_event(REWIND_UP_NS, 130, 185, 0),
        mouse_event(REWIND_UP_NS + 100_000_000, 0, 0, 0),
        # Toolbar index 3 starts playback. Move away after the click to keep
        # the pointer from continuously selecting a replay control.
        mouse_event(FIRST_PLAY_DOWN_NS, 253, 164, 1),
        mouse_event(FIRST_PLAY_UP_NS, 253, 164, 0),
        mouse_event(FIRST_PLAY_UP_NS + 100_000_000, 0, 0, 0),
        # Toolbar index 4 pauses playback and index 1 seeks backward.
        mouse_event(PAUSE_DOWN_NS, 211, 164, 1),
        mouse_event(PAUSE_UP_NS, 211, 164, 0),
        mouse_event(PAUSE_UP_NS + 100_000_000, 0, 0, 0),
        mouse_event(SEEK_DOWN_NS, 130, 185, 1),
        mouse_event(SEEK_UP_NS, 130, 185, 0),
        mouse_event(SEEK_UP_NS + 100_000_000, 0, 0, 0),
        # Start playback again after the backward seek.
        mouse_event(RESUME_DOWN_NS, 253, 164, 1),
        mouse_event(RESUME_UP_NS, 253, 164, 0),
        mouse_event(RESUME_UP_NS + 100_000_000, 0, 0, 0),
    ])
    script["events"] = sorted(events, key=lambda event: int(event["visible_tick"]))
    path.write_text(json.dumps(script, separators=(",", ":")) + "\n",
                    encoding="ascii")


def run_trace(executable: Path, trace_path: Path,
              input_path: Path) -> tuple[
                  str, list[dict[str, object]], list[dict[str, object]],
                  list[dict[str, object]], dict[int, dict[str, int]],
                  list[dict[str, object]], list[dict[str, object]], int]:
    """Run the game and retain only trace rows needed by this regression."""
    environment = os.environ.copy()
    environment["SDL_VIDEODRIVER"] = "dummy"
    environment["SDL_AUDIODRIVER"] = "dummy"
    environment["PATH"] = os.pathsep.join((
        r"C:\msys64\mingw32\bin",
        r"C:\tools\sdl3-3.4.16-i686\bin",
        environment.get("PATH", ""),
    ))
    command = [
        str(executable),
        f"--trace={trace_path}",
        f"--assets={ASSETS}",
        f"--input-script={input_path}",
        f"--test-startup-seed={STARTUP_SEED}",
        "--test-auto-protection",
        f"--run-ms={RUN_MS}",
    ]
    try:
        result = subprocess.run(
            command, cwd=ROOT, env=environment, capture_output=True,
            text=True, timeout=60, check=False)
    except subprocess.TimeoutExpired as error:
        raise AssertionError(
            "SDL3 replay playback exceeded its 60-second bound; "
            f"partial stdout={error.stdout!r}, stderr={error.stderr!r}") from error

    inputs: list[dict[str, object]] = []
    steps: list[dict[str, object]] = []
    paused_publications: list[dict[str, object]] = []
    host_presents: list[dict[str, object]] = []
    snapshots: dict[int, dict[str, int]] = {}
    stop_events: list[dict[str, object]] = []
    try:
        with trace_path.open(encoding="utf-8") as source:
            for line in source:
                event = json.loads(line)
                kind = event.get("event_type")
                if kind == "input":
                    inputs.append(event)
                elif kind == "simulation_step":
                    steps.append(event)
                elif kind == "state_snapshot":
                    snapshots[int(event["sim_step_id"])] = decode_player_snapshot(event)
                elif kind == "video_publication" and (
                    event.get("game_mode") == 2 and
                    event.get("game_replay_mode") == 1
                ):
                    paused_publications.append(event)
                elif kind == "host_stop":
                    stop_events.append(event)
                elif kind == "host_present":
                    host_presents.append(event)
    except (OSError, KeyError, TypeError, ValueError, json.JSONDecodeError) as error:
        raise AssertionError(f"SDL3 replay trace is malformed: {error}") from error

    return (result.stderr, inputs, steps, paused_publications, snapshots,
            stop_events, host_presents, result.returncode)


def find_mouse_input(inputs: list[dict[str, object]], x: int, y: int,
                     buttons: int, min_tick: int,
                     max_tick: int) -> dict[str, object] | None:
    u = x / 319
    v = y / 199
    for event in inputs:
        payload = event.get("payload")
        if (event.get("channel") == "dos.mouse" and isinstance(payload, dict) and
                payload.get("buttons") == buttons and
                min_tick <= int(event.get("machine_tick", -1)) < max_tick and
                abs(float(payload.get("u", -2)) - u) < 0.01 and
                abs(float(payload.get("v", -2)) - v) < 0.01):
            return event
    return None


class Sdl3ReplayPlaybackTests(unittest.TestCase):
    def test_recorded_replay_plays_pauses_seeks_and_resumes(self) -> None:
        assert_build_ready()
        newest_runtime_input = max(
            (ROOT / "port" / "input.c").stat().st_mtime_ns,
            (ROOT / "port" / "sdl_host.c").stat().st_mtime_ns,
            (ROOT / "port" / "main.c").stat().st_mtime_ns,
        )
        self.assertGreaterEqual(BUILD_REPORT.stat().st_mtime_ns,
                                newest_runtime_input,
                                "SDL3 build predates replay input/runtime changes")
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-replay-") as directory:
            run_dir = Path(directory)
            trace_path = run_dir / "trace.jsonl"
            input_path = run_dir / "input.json"
            write_replay_input(input_path)
            (stderr, inputs, steps, paused_publications, snapshots,
             stop_events, host_presents, returncode) = run_trace(
                EXECUTABLE, trace_path, input_path)

        self.assertEqual(returncode, 0, f"SDL3 exited with stderr:\n{stderr}")
        self.assertIn("PORT test startup seed applied", stderr)
        self.assertNotIn("unresolved host service:", stderr)
        self.assertNotIn("PORT fatal stub", stderr)
        self.assertEqual(len(stop_events), 1, stop_events)
        self.assertEqual(stop_events[0].get("reason"),
                         "requested run duration reached", stop_events[0])

        def required_mouse(x: int, y: int, buttons: int, expected_tick: int,
                           description: str) -> dict[str, object]:
            event = find_mouse_input(
                inputs, x, y, buttons,
                min_tick=expected_tick - 250_000_000,
                max_tick=expected_tick + 1_000_000_000)
            self.assertIsNotNone(event, f"{description} was not traced")
            return event or {}

        first_play = required_mouse(
            253, 164, 1, FIRST_PLAY_DOWN_NS, "replay Play click")
        pause = required_mouse(211, 164, 1, PAUSE_DOWN_NS, "replay Pause click")
        seek = required_mouse(130, 185, 1, SEEK_DOWN_NS, "replay rewind click")
        resume = required_mouse(
            253, 164, 1, RESUME_DOWN_NS, "replay resume click")
        first_play_tick = int(first_play["machine_tick"])
        pause_tick = int(pause["machine_tick"])
        seek_tick = int(seek["machine_tick"])
        resume_tick = int(resume["machine_tick"])

        def replay_steps(begin: int, end: int) -> list[tuple[dict[str, object],
                                                               dict[str, int]]]:
            selected = []
            for event in steps:
                if (event.get("game_mode") == 2 and
                        event.get("game_replay_mode") == 0 and
                        begin <= int(event.get("machine_tick", -1)) < end):
                    state = snapshots.get(int(event.get("sim_step_id", -1)))
                    if state is not None:
                        selected.append((event, state))
            return selected

        first_playback = replay_steps(first_play_tick, pause_tick)
        self.assertGreaterEqual(
            len(first_playback), 35,
            f"real-time replay produced only {len(first_playback)} frames")
        first_play_frames = [state["frame"] for _, state in first_playback]
        self.assertLessEqual(first_play_frames[0], 2,
                             f"rewind did not return to the start: {first_play_frames[:4]}")
        self.assertGreaterEqual(first_play_frames[-1], 40,
                                f"replay did not advance: {first_play_frames[-5:]}")
        self.assertTrue(
            all(after == before + 1 for before, after in
                zip(first_play_frames, first_play_frames[1:])),
            f"recorded replay frames were not sequential: {first_play_frames[:12]} …")
        first_play_times = [int(event["host_ns"]) for event, _ in first_playback]
        self.assertGreaterEqual(first_play_times[-1] - first_play_times[0],
                                2_000_000_000,
                                "replay playback did not sustain real-time advancement")

        paused_interval_steps = [
            event for event in steps
            if event.get("game_mode") == 2 and
            pause_tick <= int(event.get("machine_tick", -1)) < seek_tick
        ]
        self.assertFalse(
            paused_interval_steps,
            "game state advanced while the replay was paused before seeking: "
            f"{paused_interval_steps[:3]}")
        paused_rows = [
            event for event in paused_publications
            if pause_tick <= int(event.get("machine_tick", -1)) < seek_tick
        ]
        self.assertGreaterEqual(len(paused_rows), 2,
                                "paused replay did not continue publishing its UI")
        paused_frames = {int(event["game_frame"]) for event in paused_rows}
        self.assertEqual(len(paused_frames), 1,
                         f"game frame changed during pause: {paused_frames}")
        presented_paused_rows = [
            event for event in host_presents
            if pause_tick <= int(event.get("machine_tick", -1)) < seek_tick
        ]
        self.assertGreaterEqual(len(presented_paused_rows), 1,
                                "the paused replay UI was never presented")
        published_paused_ids = {int(event["frame_id"]) for event in paused_rows}
        self.assertTrue(any(int(event["frame_id"]) in published_paused_ids
                            for event in presented_paused_rows),
                        "no paused guest image was displayed")
        # SDL retains the last displayed image. Window exposure is tested by
        # the native video probe; repeated uploads of static pixels waste CPU.

        sought = [
            snapshots[int(event["sim_step_id"])]
            for event in steps
            if event.get("game_mode") == 2 and
            event.get("game_replay_mode") == 1 and
            seek_tick <= int(event.get("machine_tick", -1)) < resume_tick and
            int(event.get("sim_step_id", -1)) in snapshots
        ]
        self.assertGreaterEqual(len(sought), 20,
                                "replay rewind did not rebuild the earlier states")
        self.assertLess(max(state["frame"] for state in sought),
                        next(iter(paused_frames)),
                        "rewind did not move behind the paused frame")

        resumed = replay_steps(resume_tick, RUN_MS * 1_000_000)
        self.assertGreaterEqual(len(resumed), 30,
                                f"replay did not resume: {len(resumed)} frames")
        resumed_frames = [state["frame"] for _, state in resumed]
        self.assertTrue(
            all(after == before + 1 for before, after in
                zip(resumed_frames, resumed_frames[1:])),
            f"resumed replay frames were not sequential: {resumed_frames[:12]} …")
        resumed_times = [int(event["host_ns"]) for event, _ in resumed]
        self.assertGreaterEqual(resumed_times[-1] - resumed_times[0],
                                1_500_000_000,
                                "resumed playback did not sustain real-time advancement")


if __name__ == "__main__":
    unittest.main()
