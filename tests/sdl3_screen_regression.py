"""Self-contained capture and exact comparison for the SDL3 startup screens."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import zlib


ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures" / "sdl3"
EXECUTABLE = ROOT / "build" / "sdl3" / "stunts.exe"
ASSETS = ROOT / "build" / "sdl3" / "runtime" / "assets"
STARTUP_SEED = FIXTURES / "startup-seed.json"
SCREEN_NAMES = ("splash", "title", "menu")
FRAME_BYTES = 320 * 200
PALETTE_BYTES = 256 * 3
FBR_BYTES = 20 + FRAME_BYTES + PALETTE_BYTES
RUN_MS = 9000


def read_reference(name: str) -> tuple[bytes, bytes]:
    """Read the immutable PortForge indexed pixels and RGB6 palette."""
    metadata_path = FIXTURES / f"{name}.json"
    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    payload = zlib.decompress((FIXTURES / metadata["payload"]).read_bytes())
    if len(payload) != FRAME_BYTES + PALETTE_BYTES:
        raise AssertionError(f"{name}: wrong uncompressed reference size")
    checks = (
        (payload, "payload_sha256"),
        (payload[:FRAME_BYTES], "indices_sha256"),
        (payload[FRAME_BYTES:], "palette_rgb6_sha256"),
    )
    for value, key in checks:
        if hashlib.sha256(value).hexdigest() != metadata[key]:
            raise AssertionError(f"{name}: reference {key} mismatch")
    return payload[:FRAME_BYTES], payload[FRAME_BYTES:]


def read_port_fbr(path: Path) -> tuple[bytes, bytes]:
    data = path.read_bytes()
    if len(data) != FBR_BYTES or data[:8] != b"STFBR1\0\0":
        raise AssertionError(f"{path}: malformed port framebuffer capture")
    width = int.from_bytes(data[8:10], "little")
    height = int.from_bytes(data[10:12], "little")
    palette_size = int.from_bytes(data[12:14], "little")
    if (width, height, palette_size) != (320, 200, PALETTE_BYTES):
        raise AssertionError(f"{path}: unexpected framebuffer or palette dimensions")
    return data[20:20 + FRAME_BYTES], data[20 + FRAME_BYTES:]


def write_startup_input(path: Path) -> None:
    """Let both intro screens render, then enter the menu and align its cursor."""
    script = {
        "format": "portforge-exact-input-script-v1",
        "anchor_tick": 0,
        "events": [
            # The seeded intro draws the title after about four seconds.
            # Dismiss its second wait so the already-published title remains
            # in the captures, then let the menu poll the aligned pointer.
            {"visible_tick": 5_000_000_000,
             "channel": "dos.keyboard.scancodes", "payload": [28, 156]},
            {"visible_tick": 5_200_000_000,
             "channel": "dos.mouse.normalized",
             # The immutable menu fixture's visible cursor origin is
             # (206,189), so use that logical point for this screen match.
             "payload": {"u": 206 / 319, "v": 189 / 199, "buttons": 0}},
        ],
    }
    path.write_text(json.dumps(script, separators=(",", ":")) + "\n",
                    encoding="ascii")


def capture_startup_screens(capture_dir: Path) -> list[str]:
    """Run the built game with deterministic startup and collect its FBRs."""
    if not EXECUTABLE.is_file():
        raise AssertionError(
            f"SDL3 executable is missing: {EXECUTABLE}; run `python port/build.py build` first")
    if not ASSETS.is_dir():
        raise AssertionError(f"SDL3 runtime assets are missing: {ASSETS}")
    if not STARTUP_SEED.is_file():
        raise AssertionError(f"startup seed fixture is missing: {STARTUP_SEED}")

    capture_dir.mkdir(parents=True, exist_ok=True)
    run_dir = capture_dir.parent
    trace_path = run_dir / "trace.jsonl"
    input_path = run_dir / "input.json"
    write_startup_input(input_path)

    environment = os.environ.copy()
    environment.pop("STUNTS_ASSET_ROOT", None)
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
        f"--capture-dir={capture_dir}",
        f"--input-script={input_path}",
        f"--test-startup-seed={STARTUP_SEED}",
        "--test-auto-protection",
        f"--run-ms={RUN_MS}",
    ]
    try:
        # Exercise the executable's own asset lookup from an unrelated working
        # directory, as an Explorer launch or shortcut would do.
        result = subprocess.run(command, cwd=run_dir, env=environment,
                                capture_output=True, text=True, timeout=30,
                                check=False)
    except subprocess.TimeoutExpired as error:
        raise AssertionError(
            "SDL3 startup run failed to stop within 30 seconds; "
            f"partial stdout={error.stdout!r}, stderr={error.stderr!r}") from error

    captures = sorted(capture_dir.glob("*.fbr"))
    if result.returncode != 0:
        raise AssertionError(
            f"SDL3 startup run exited {result.returncode}; stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}; captures={len(captures)}")
    if not trace_path.is_file():
        raise AssertionError("SDL3 startup run did not create its runtime trace")
    try:
        trace_events = [json.loads(line) for line in
                        trace_path.read_text(encoding="utf-8").splitlines()]
    except (OSError, json.JSONDecodeError) as error:
        raise AssertionError(f"SDL3 startup trace is malformed: {error}") from error
    stop_events = [event for event in trace_events
                   if event.get("event_type") == "host_stop"]
    if len(stop_events) != 1 or stop_events[0].get("reason") != \
            "requested run duration reached":
        reasons = [event.get("reason") for event in stop_events]
        raise AssertionError(
            "SDL3 guest did not stop cleanly at the requested run duration; "
            f"host_stop reasons={reasons}; stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}")
    if stop_events[0].get("host_ns", 0) < (RUN_MS - 100) * 1_000_000:
        raise AssertionError(
            "SDL3 guest stopped before the timed startup boundary: "
            f"host_ns={stop_events[0].get('host_ns')}")
    if not captures:
        trace_tail = ""
        if trace_path.is_file():
            trace_tail = "\n".join(trace_path.read_text(encoding="utf-8").splitlines()[-12:])
        raise AssertionError(
            "SDL3 startup completed without framebuffer captures; "
            f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}\ntrace tail:\n{trace_tail}")
    return [result.stdout, result.stderr]


def find_exact_screens(capture_dir: Path) -> tuple[dict[str, Path], dict[str, str]]:
    """Match captures by full indexed image and palette, never by ordinal."""
    references = {name: read_reference(name) for name in SCREEN_NAMES}
    matches: dict[str, Path] = {}
    nearest: dict[str, tuple[int, int, Path]] = {}
    malformed: list[str] = []
    for path in sorted(capture_dir.glob("*.fbr")):
        try:
            pixels, palette = read_port_fbr(path)
        except AssertionError as error:
            malformed.append(str(error))
            continue
        for name, (expected_pixels, expected_palette) in references.items():
            pixel_diff = sum(a != b for a, b in zip(pixels, expected_pixels))
            palette_diff = sum(a != b for a, b in zip(palette, expected_palette))
            score = (pixel_diff, palette_diff)
            prior = nearest.get(name)
            if prior is None or score < (prior[0], prior[1]):
                nearest[name] = (pixel_diff, palette_diff, path)
            if pixel_diff == 0 and palette_diff == 0:
                matches[name] = path

    diagnostics = {}
    for name in SCREEN_NAMES:
        if name in matches:
            diagnostics[name] = f"exact at {matches[name].name}"
        elif name in nearest:
            pixel_diff, palette_diff, path = nearest[name]
            diagnostics[name] = (f"nearest {path.name}: {pixel_diff} indexed pixels, "
                                 f"{palette_diff} palette bytes differ")
        else:
            diagnostics[name] = "no valid captures"
    if malformed:
        diagnostics["malformed"] = "; ".join(malformed)
    return matches, diagnostics


def fresh_capture_directory() -> tuple[tempfile.TemporaryDirectory[str], Path]:
    temporary = tempfile.TemporaryDirectory(prefix="stunts-sdl3-screens-")
    capture_dir = Path(temporary.name) / "captures"
    return temporary, capture_dir
