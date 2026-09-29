from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PORT_INCLUDE = ROOT / "tools" / "porting" / "port_include"
GCC = shutil.which("gcc")
DEFAULT_GCC = Path(r"C:\msys64\mingw64\bin\gcc.exe")
if not GCC and DEFAULT_GCC.is_file():
    GCC = str(DEFAULT_GCC)


class PortHeaderLayoutTests(unittest.TestCase):
    def test_camera_button_arrays_are_nine_words_in_central_header(self):
        header = (PORT_INCLUDE / "stunts_decls.h").read_text(encoding="utf-8")
        for name in ("game_camera_buttons_x1", "game_camera_buttons_x2",
                     "game_camera_buttons_y1", "game_camera_buttons_y2"):
            with self.subTest(name=name):
                self.assertRegex(header, rf"extern int16_t {name}\[9\];")

    def test_f3_machine_contracts_are_present_in_central_header(self):
        header = (PORT_INCLUDE / "stunts_decls.h").read_text(encoding="utf-8")
        self.assertIn("extern I16 far send_audio_stop_event(U16 rate, I16 handle);", header)
        self.assertIn("extern void far *read_file_with_retry(I16 type, U16 near_name_offset, U16 destination_offset, U16 destination_segment);", header)
        self.assertIn("extern I16 far call_read_line(I8 *buffer, I16 x, I16 y, I16 width, I16 limit, I16 flags);", header)
        self.assertIn("extern void far nullsub_2(void far *resource, I16 selector);", header)
        self.assertIn("extern void far * far locate_shape_fatal(void far *data, I8 *name);", header)

    @unittest.skipUnless(GCC, "GCC is not installed; compile-time layout checks skipped")
    def test_target_aggregate_size_and_offset_assertions_compile(self):
        worker = ROOT / "build" / "workers" / "integ55"
        worker.mkdir(parents=True, exist_ok=True)
        source_text = """#include "stunts_structs.h"
_Static_assert(sizeof(stunts_GAMESTATE_1014) == 1014, "GAMESTATE prefix size");
_Static_assert(sizeof(stunts_GAMESTATE_1120) == 1120, "GAMESTATE full size");
_Static_assert(offsetof(stunts_GAMESTATE_1120, game_vec1) == 0x120, "GAMESTATE vector offset");
_Static_assert(offsetof(stunts_GAMESTATE_1120, game_vec3) == 0x12c, "GAMESTATE vector overlay offset");
_Static_assert(sizeof(stunts_SHAPE2D_12) == 12, "SHAPE2D 12-byte view");
_Static_assert(sizeof(stunts_SHAPE2D_14) == 14, "SHAPE2D 14-byte view");
_Static_assert(sizeof(stunts_SHAPE2D_16) == 16, "SHAPE2D 16-byte view");
_Static_assert(sizeof(stunts_AUDIOCHUNK) == 76, "audio chunk target size");
"""
        with tempfile.TemporaryDirectory(prefix="port-layout-", dir=worker) as temp:
            source = Path(temp) / "layout_probe.c"
            source.write_text(source_text, encoding="utf-8")
            result = subprocess.run(
                [str(GCC), "-std=gnu11", "-Wall", "-Wextra", "-Werror", "-fsyntax-only",
                 "-I", str(PORT_INCLUDE), str(source)],
                cwd=ROOT, capture_output=True, text=True, errors="replace", timeout=30, check=False,
            )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
