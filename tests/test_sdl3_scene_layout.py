"""The static scene tables and renderer must use the same host record stride."""
from __future__ import annotations

import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
PORT_INCLUDE = ROOT / "tools" / "porting" / "port_include"
sys.path.insert(0, str(ROOT / "tools" / "porting"))
from host_probe_modes import legacy_target_widths  # noqa: E402
from port.game_abi import adapt_aggregate_views  # noqa: E402


def selected_view(header: str, unit: str, name: str) -> str:
    for match in re.finditer(rf"#if defined\(STUNTS_TU_{unit}\)\n(.*?)\n#endif",
                             header, re.S):
        block = match.group(0)
        if re.search(rf"\bstruct {name}\s*\{{", block):
            return block
    raise AssertionError(f"Missing {unit} view of {name}")


def scene_array(source: str, name: str, count: int) -> str:
    match = re.search(rf"struct scene_shape {name}\[{count}\] = \{{.*?^\}};",
                      source, re.S | re.M)
    if match is None:
        raise AssertionError(f"Missing historical {name} initializer")
    return match.group(0)


class Sdl3SceneLayoutTests(unittest.TestCase):
    def test_scene_table_matches_renderer_view(self) -> None:
        compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        if not compiler.is_file():
            selected = shutil.which("i686-w64-mingw32-gcc")
            if selected is None:
                self.skipTest("i686 GCC is required for the scene layout check")
            compiler = Path(selected)
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = str(compiler.parent) + os.pathsep + compiler_env.get("PATH", "")

        historical = (ROOT / "src" / "track_constants_module.c").read_text(
            encoding="latin-1")
        self.assertIn("unsigned short opaque_first_word;", historical)
        scene2 = scene_array(historical, "scene2", 19)
        scene3 = scene_array(historical, "scene3", 13)
        self.assertEqual(len(re.findall(r"^\s*\{", scene2, re.M)), 19)
        self.assertEqual(len(re.findall(r"^\s*\{", scene3, re.M)), 13)
        self.assertIn("{0, 0, &g_shapes3d[42], &g_shapes3d[42], 0, 0, 1, 0, 255, 0}",
                      scene2)

        original_header = (PORT_INCLUDE / "stunts_structs.h").read_text(
            encoding="latin-1")
        derived = adapt_aggregate_views(legacy_target_widths(original_header))
        producer_view = selected_view(derived, "track_constants_module", "scene_shape")
        consumer_view = selected_view(derived, "obj_seg003", "TRACKOBJECT")
        self.assertIn("void *opaque_first_word;", producer_view)

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-scene-abi-") as directory:
            scratch = Path(directory)
            shutil.copyfile(PORT_INCLUDE / "stunts_types.h", scratch / "stunts_types.h")
            (scratch / "stunts_scene_views.h").write_text(
                '#include "stunts_types.h"\n' + producer_view + "\n" + consumer_view,
                encoding="latin-1",
            )
            producer = scratch / "scene_producer.c"
            producer.write_text(
                '#include <stdint.h>\n#include <stddef.h>\n'
                '#include "stunts_scene_views.h"\n'
                'struct SHAPE3D { uint16_t marker; };\n'
                'struct SHAPE3D g_shapes3d[130];\n'
                + scene2 + "\n" + scene3 + "\n"
                + 'size_t port_scene_producer_stride(void) { return sizeof(scene2[0]); }\n'
                + 'size_t port_scene_producer_shape_offset(void) '
                  '{ return offsetof(struct scene_shape, shape); }\n',
                encoding="latin-1",
            )
            consumer = ROOT / "tests" / "sdl3" / "scene_layout_consumer.c"
            executable = scratch / "scene_layout_probe.exe"
            result = subprocess.run(
                [str(compiler), "-std=gnu11", "-O0", "-Werror",
                 "-DSTUNTS_TU_track_constants_module=1", "-I", str(scratch),
                 "-c", str(producer), "-o", str(scratch / "producer.o")],
                cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                timeout=30, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(compiler), "-std=gnu11", "-O0", "-Werror",
                 "-DSTUNTS_TU_obj_seg003=1", "-I", str(scratch),
                 "-c", str(consumer), "-o", str(scratch / "consumer.o")],
                cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                timeout=30, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(compiler), str(scratch / "producer.o"),
                 str(scratch / "consumer.o"), "-o", str(executable)],
                cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                timeout=30, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable)], cwd=ROOT, env=compiler_env,
                capture_output=True, text=True, timeout=30, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
