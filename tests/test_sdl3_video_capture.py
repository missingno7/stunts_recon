"""F12 debug captures preserve indexed pixels and their publication identity."""
import json
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from tests.test_sdl3_diagnostics import compiler_path, make_test_environment, SDL_ROOT, ROOT


class Sdl3VideoCaptureTests(unittest.TestCase):
    def test_f12_capture_and_normal_key_routing(self):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("requires i686 GCC and SDL3")
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-video-capture-") as temporary:
            folder = Path(temporary)
            exe = folder / "capture-probe.exe"
            result = subprocess.run([
                compiler, "-std=gnu11", "-Wall", "-Wextra", "-Werror",
                "-I", str(ROOT / "port"), "-I", str(SDL_ROOT / "include"),
                str(ROOT / "tests/sdl3/video_capture_probe.c"),
                *(str(ROOT / "port" / file) for file in
                  ("video.c", "sdl_host.c", "trace.c", "diagnostics.c")),
                "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(exe),
            ], env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe), str(folder / "diagnostics")],
                                    env=environment, capture_output=True, text=True, timeout=20)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            session, = (folder / "diagnostics").glob("stunts-*")
            capture, = session.glob("capture-*")
            frame, = capture.glob("*.fbr")
            data = frame.read_bytes()
            self.assertEqual(data[:8], b"STFBR1\0\0")
            width, height, palette_length, _, frame_id = struct.unpack_from("<HHHHI", data, 8)
            self.assertEqual((width, height, palette_length), (320, 200, 768))
            expected = bytes((i * 7 + i // 320) & 255 for i in range(64000))
            palette = bytes(i & 63 for i in range(768))
            self.assertEqual(data[20:64020], expected)
            self.assertEqual(data[64020:], palette)
            bmp = (capture / "screenshot.bmp").read_bytes()
            self.assertEqual(bmp[:2], b"BM")
            self.assertEqual(struct.unpack_from("<I", bmp, 2)[0], len(bmp))
            offset, = struct.unpack_from("<I", bmp, 10)
            pixels = b"".join(bmp[offset + row * 320:offset + (row + 1) * 320]
                              for row in range(199, -1, -1))
            self.assertEqual(pixels, expected)
            for i in range(256):
                color = bytes(((v << 2) | (v >> 4)) for v in palette[i * 3:i * 3 + 3][::-1])
                self.assertEqual(bmp[54 + i * 4:54 + i * 4 + 4], color + b"\0")
            rows = list(map(json.loads, (session / "trace.jsonl").read_text().splitlines()))
            marker, = (row for row in rows if row["event_type"] == "debug_capture")
            publication, = (row for row in rows if row["event_type"] == "video_publication"
                             and row["frame_id"] == frame_id)
            self.assertEqual(marker["frame_id"], frame_id)
            self.assertEqual(Path(marker["directory"]), capture)
            self.assertEqual(publication["game_frame"], 37)
            self.assertEqual(publication["renderer_y_rotation"], 255)
            presents = [row for row in rows if row["event_type"] == "host_present"]
            self.assertEqual(len(presents), 7,
                             "only image changes or explicit window invalidations should present")
            self.assertEqual([row["frame_id"] for row in presents], [2, 4, 5, 5, 5, 5, 5])


if __name__ == "__main__":
    unittest.main()
