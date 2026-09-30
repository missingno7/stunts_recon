"""Exact indexed-screen assertions against PortForge references."""
import hashlib
import json
import os
from pathlib import Path
import unittest
import zlib


ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures" / "sdl3"
CAPTURE_DIR = Path(os.environ.get(
    "STUNTS_SDL3_CAPTURE_DIR", ROOT / "build" / "sdl3" / "m1b-title-check"))
MENU_CAPTURE_DIR = Path(os.environ.get(
    "STUNTS_SDL3_MENU_CAPTURE_DIR", ROOT / "build" / "sdl3" / "m1b-menu-aligned"))
CAPTURE_NAMES = {
    "splash": "frame-000004.fbr",
    "title": "frame-000008.fbr",
    "menu": "frame-000016.fbr",
}


def read_reference(name):
    metadata = json.loads((FIXTURES / f"{name}.json").read_text(encoding="utf-8"))
    payload = zlib.decompress((FIXTURES / metadata["payload"]).read_bytes())
    if len(payload) != 64000 + 768:
        raise AssertionError(f"{name}: wrong uncompressed reference size")
    if hashlib.sha256(payload).hexdigest() != metadata["payload_sha256"]:
        raise AssertionError(f"{name}: reference digest mismatch")
    if hashlib.sha256(payload[:64000]).hexdigest() != metadata["indices_sha256"]:
        raise AssertionError(f"{name}: indexed-pixel digest mismatch")
    if hashlib.sha256(payload[64000:]).hexdigest() != metadata["palette_rgb6_sha256"]:
        raise AssertionError(f"{name}: RGB6 palette digest mismatch")
    return payload[:64000], payload[64000:]


def read_port_fbr(path):
    data = path.read_bytes()
    if len(data) != 64788 or data[:8] != b"STFBR1\0\0":
        raise AssertionError(f"{path}: malformed port framebuffer capture")
    width = int.from_bytes(data[8:10], "little")
    height = int.from_bytes(data[10:12], "little")
    palette_size = int.from_bytes(data[12:14], "little")
    if (width, height, palette_size) != (320, 200, 768):
        raise AssertionError(f"{path}: unexpected framebuffer or palette dimensions")
    return data[20:64020], data[64020:64788]


class Sdl3FramebufferTests(unittest.TestCase):
    def assert_screen_matches_portforge(self, name):
        capture_dir = MENU_CAPTURE_DIR if name == "menu" else CAPTURE_DIR
        path = capture_dir / CAPTURE_NAMES[name]
        if not path.is_file():
            self.skipTest(f"port capture is missing: {path}")
        expected_pixels, expected_palette = read_reference(name)
        actual_pixels, actual_palette = read_port_fbr(path)
        self.assertEqual(actual_pixels, expected_pixels)
        self.assertEqual(actual_palette, expected_palette)

    def test_splash_matches_portforge_exactly(self):
        self.assert_screen_matches_portforge("splash")

    def test_title_matches_portforge_exactly(self):
        self.assert_screen_matches_portforge("title")

    def test_menu_matches_portforge_exactly(self):
        self.assert_screen_matches_portforge("menu")


if __name__ == "__main__":
    unittest.main()
