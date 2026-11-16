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
EXECUTABLE = Path(os.environ.get("STUNTS_TEST_EXECUTABLE",
                                 ROOT / "build" / "sdl3" / "stunts.exe"))
ASSETS = ROOT / "build" / "sdl3" / "runtime" / "assets"
STARTUP_SEED = FIXTURES / "startup-seed.json"
SCREEN_NAMES = ("splash", "title", "menu")
FRAME_BYTES = 320 * 200
PALETTE_BYTES = 256 * 3
FBR_BYTES = 20 + FRAME_BYTES + PALETTE_BYTES
RUN_MS = 9000


def read_reference(name: str) -> tuple[bytes, bytes]:
    """Read the sealed original DOSBox-X indexed pixels and RGB6 palette."""
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
        "format": "stunts-exact-input-script-v1",
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
    """Capture the named original checkpoints on the manual clock."""
    import struct
    from tools.porting.scenario_oracle import script,run_port
    from tools.porting.compare_scenarios import port_data
    capture_dir.mkdir(parents=True,exist_ok=True)
    input_path=capture_dir.parent/'input.json'
    script('startup',input_path)
    folder=run_port(capture_dir.parent/'run',input_path,'ad15',1000)
    actual=port_data(folder)
    if actual['terminal']!='scenario checkpoint reached':
        raise AssertionError('startup did not reach its semantic terminal')
    for name in SCREEN_NAMES:
        payload=actual['payloads'][name]
        (capture_dir/('checkpoint-'+name+'.fbr')).write_bytes(
            b'STFBR1\0\0'+struct.pack('<HHHHI',320,200,768,0,0)+payload)
    return [(folder/'run.log').read_text()]


def find_exact_screens(capture_dir: Path) -> tuple[dict[str, Path], dict[str, str]]:
    """Compare only named checkpoints; never search for a matching image."""
    matches={};diagnostics={}
    for name in SCREEN_NAMES:
        path=capture_dir/('checkpoint-'+name+'.fbr')
        actual=read_port_fbr(path);expected=read_reference(name)
        differences=[sum(a!=b for a,b in zip(x,y)) for x,y in zip(actual,expected)]
        diagnostics[name]=f'{differences[0]} indices, {differences[1]} palette bytes differ at named checkpoint'
        if actual==expected:matches[name]=path
    return matches,diagnostics


def fresh_capture_directory() -> tuple[tempfile.TemporaryDirectory[str], Path]:
    temporary = tempfile.TemporaryDirectory(prefix="stunts-sdl3-screens-")
    capture_dir = Path(temporary.name) / "captures"
    return temporary, capture_dir
