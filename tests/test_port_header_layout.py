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
    @unittest.skipUnless(GCC, "GCC is not installed; compile-time layout checks skipped")
    def test_target_aggregate_size_and_offset_assertions_compile(self):
        worker = ROOT / "build" / "workers" / "integ52"
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
