"""Frozen track-info data must match the racing consumer's native view."""
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


def initialized_array(source: str, declaration: str) -> str:
    match = re.search(rf"(?m)^{re.escape(declaration)}\s*=\s*\{{.*?^\}};",
                      source, re.S | re.M)
    if match is None:
        raise AssertionError(f"Missing historical {declaration} initializer")
    return match.group(0)


class Sdl3TrackInfoLayoutTests(unittest.TestCase):
    def test_frozen_info_and_track_tables_match_racing_view(self) -> None:
        compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        if not compiler.is_file():
            selected = shutil.which("i686-w64-mingw32-gcc")
            if selected is None:
                self.skipTest("i686 GCC is required for the track-info layout check")
            compiler = Path(selected)
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = str(compiler.parent) + os.pathsep + compiler_env.get("PATH", "")

        historical = (ROOT / "src" / "track_constants_module.c").read_text(
            encoding="latin-1")
        info = initialized_array(historical, "struct track_object_info shapeinfos[120]")
        tracks = initialized_array(historical, "struct track_object trklst[215]")
        camera_data = initialized_array(historical, "unsigned char shapedata42_10[1236]")
        self.assertEqual(len(re.findall(r"^\s*\{", info, re.M)), 120)
        self.assertEqual(len(re.findall(r"^\s*\{", tracks, re.M)), 215)
        self.assertIn("shapedata42_10 + 1044, 114, 25", info)
        self.assertIn("shapedata42_10 + 960, 30, 25", info)

        original_header = (PORT_INCLUDE / "stunts_structs.h").read_text(
            encoding="latin-1")
        derived = adapt_aggregate_views(legacy_target_widths(original_header))
        producer_views = "\n".join(selected_view(derived, "track_constants_module", name)
                                   for name in ("track_object_info", "track_object"))
        consumer_views = "\n".join(selected_view(derived, "obj_seg001_complete", name)
                                   for name in ("VECTOR", "TRKOBJINFO_LINK_BYTES",
                                                "TRKOBJINFO", "TRACKOBJECT"))
        self.assertIn("U16S dataPointer", consumer_views)

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-track-info-") as directory:
            scratch = Path(directory)
            shutil.copyfile(PORT_INCLUDE / "stunts_types.h", scratch / "stunts_types.h")
            (scratch / "stunts_track_views.h").write_text(
                '#include "stunts_types.h"\n'
                'struct SHAPE3D { unsigned char opaque_layout[22]; };\n'
                + producer_views + "\n" + consumer_views + "\n",
                encoding="latin-1")
            producer = scratch / "track_info_producer.c"
            other_camera_arrays = sorted(set(re.findall(r"\bshapedata42(?:_\d+)?\b", info))
                                         - {"shapedata42_10"})
            producer.write_text(
                '#include <stddef.h>\n#include "stunts_track_views.h"\n'
                + "\n".join(f"unsigned char {name}[4096];" for name in other_camera_arrays)
                + "\n" + camera_data + "\n" + info + "\n"
                + 'struct SHAPE3D g_shapes3d[130];\n' + tracks + "\n"
                + 'size_t port_track_producer_stride(void) '
                  '{ return sizeof(shapeinfos[0]); }\n'
                + 'size_t port_track_producer_link_offset(void) '
                  '{ return offsetof(struct track_object_info, opponent1); }\n',
                encoding="latin-1")
            consumer = ROOT / "tests" / "sdl3" / "track_info_layout_consumer.c"
            objects = []
            for source, unit in ((producer, "track_constants_module"),
                                 (consumer, "obj_seg001_complete")):
                output = scratch / (source.stem + ".o")
                result = subprocess.run(
                    [str(compiler), "-std=gnu11", "-O0", "-Werror",
                     f"-DSTUNTS_TU_{unit}=1", "-I", str(scratch),
                     "-c", str(source), "-o", str(output)],
                    cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                    timeout=30, check=False)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(output)
            executable = scratch / "track_info_layout_probe.exe"
            result = subprocess.run(
                [str(compiler), *(str(obj) for obj in objects), "-o", str(executable)],
                cwd=ROOT, env=compiler_env, capture_output=True, text=True,
                timeout=30, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable)], cwd=ROOT, env=compiler_env,
                capture_output=True, text=True, timeout=30, check=False)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
