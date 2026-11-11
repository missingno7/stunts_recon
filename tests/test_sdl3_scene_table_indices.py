"""DOS track indices spanning adjacent owners resolve to the original rows."""
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile
import unittest

from tests.test_sdl3_scene_layout import (
    ROOT, PORT_INCLUDE, selected_view, scene_array, legacy_target_widths,
)
from port.game_abi import adapt_aggregate_views
from port.render.scene_table_adapter import TRACK_TABLE_LOOKUP, adapt_scene_table_indices


class SceneTableIndexTests(unittest.TestCase):
    def test_every_original_row_and_cross_owner_index(self):
        compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        self.assertTrue(compiler.is_file(), "i686 GCC is required")
        environment = os.environ.copy()
        environment["PATH"] = str(compiler.parent) + os.pathsep + environment.get("PATH", "")
        historical = (ROOT / "src/track_constants_module.c").read_text(encoding="latin-1")
        track = re.search(r"struct track_object trklst\[215\] = \{.*?^\};",
                          historical, re.S | re.M).group(0)
        scene2 = scene_array(historical, "scene2", 19)
        scene3 = scene_array(historical, "scene3", 13)
        header = adapt_aggregate_views(legacy_target_widths(
            (PORT_INCLUDE / "stunts_structs.h").read_text(encoding="latin-1")))
        producer_view = "\n".join(selected_view(header, "track_constants_module", name)
                                  for name in ("scene_shape", "track_object", "track_object_info"))
        consumer_view = selected_view(header, "obj_seg003", "TRACKOBJECT")
        source = (ROOT / "src/obj_seg003.c").read_text(encoding="utf-8")
        adapted = adapt_scene_table_indices(source)
        self.assertEqual(adapted.count("port_track_object_at("), 12)
        self.assertNotIn("&trklst[fence_codes[j2]]", adapted)

        data_layout = json.loads((ROOT / "layout/data-symbols.json").read_text())
        symbols = data_layout["symbols"]
        start = symbols["_trklst"]["load_address"]
        self.assertEqual(symbols["_scene2"]["load_address"], start + 215 * 14)
        self.assertEqual(symbols["_scene3"]["load_address"], start + 234 * 14)
        shape_near = symbols["_g_shapes3d"]["load_address"] - data_layout["frame_load_address"]
        oracle = (ROOT / "build/oracle/load-image.bin").read_bytes()
        expected = []
        for index in range(247):
            info, rotation, shape, low, *flags = struct.unpack_from("<HhHH6B", oracle, start + index * 14)
            def model(word):
                if word == 0:
                    return -1
                self.assertEqual((word - shape_near) % 22, 0)
                return (word - shape_near) // 22
            expected.append((index, rotation, model(shape), model(low), *flags))

        with tempfile.TemporaryDirectory(prefix="stunts-scene-index-") as directory:
            scratch = Path(directory)
            (scratch / "stunts_types.h").write_bytes((PORT_INCLUDE / "stunts_types.h").read_bytes())
            (scratch / "views.h").write_text('#include "stunts_types.h"\n' +
                producer_view + "\n" + consumer_view, encoding="latin-1")
            (scratch / "producer.c").write_text(
                '#include "views.h"\nstruct SHAPE3D { unsigned short marker; };\n'
                'struct SHAPE3D g_shapes3d[130];\n'
                'struct track_object_info shapeinfos[120];\n' +
                track + "\n" + scene2 + "\n" + scene3, encoding="latin-1")
            (scratch / "consumer.c").write_text(
                '#include "views.h"\n#include <stdio.h>\n#include <stdlib.h>\n'
                'struct SHAPE3D { unsigned short marker; };\n'
                'extern struct SHAPE3D g_shapes3d[130];\n'
                'extern struct TRACKOBJECT trklst[215], scene2[19], scene3[13];\n'
                'static void fatal_error(const I8 *s, ...) { (void)s; exit(99); }\n' +
                TRACK_TABLE_LOOKUP + r'''
static int model(struct SHAPE3D *p) { return p ? (int)(p - g_shapes3d) : -1; }
int main(void) {
    for (unsigned i=0; i<247; ++i) {
        struct TRACKOBJECT *p=port_track_object_at(i);
        printf("%u %d %d %d %u %u %u %u %u %u\n", i, p->ss_rotY,
            model(p->ss_shapePtr), model(p->ss_loShapePtr), p->ss_ssOvelay,
            (unsigned char)p->ss_surfaceType, (unsigned char)p->ss_ignoreZBias,
            (unsigned char)p->ss_multiTileFlag, (unsigned char)p->ss_physicalModel,
            (unsigned char)p->scene_unk5);
    }
    return 0;
}
''', encoding="latin-1")
            for unit, define in (("producer", "track_constants_module"), ("consumer", "obj_seg003")):
                result = subprocess.run([str(compiler), "-std=gnu11", "-O2", "-Werror",
                    f"-DSTUNTS_TU_{define}=1", "-I", str(scratch), "-c",
                    str(scratch / f"{unit}.c"), "-o", str(scratch / f"{unit}.o")],
                    env=environment, capture_output=True, text=True, timeout=30)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            executable = scratch / "probe.exe"
            result = subprocess.run([str(compiler), str(scratch / "producer.o"),
                str(scratch / "consumer.o"), "-o", str(executable)],
                env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], env=environment,
                capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            actual = [tuple(map(int, line.split())) for line in result.stdout.splitlines()]
            self.assertEqual(actual, expected)


if __name__ == "__main__":
    unittest.main()
