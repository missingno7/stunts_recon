"""Native-resolution coverage and explicit compositor-layer contracts."""
import os
import re
from pathlib import Path
import subprocess
import tempfile
import unittest

from tests.test_sdl3_diagnostics import compiler_path, make_test_environment, SDL_ROOT
from tests.test_sdl3_projection_math import ROOT


class ModernRendererTests(unittest.TestCase):
    def run_probe(self, name, sources, sdl=False):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("requires GCC")
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-render-") as directory:
            exe = Path(directory) / "probe.exe"
            args = [compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                    "-I", str(ROOT / "port"), "-I", str(ROOT / "port/render"),
                    str(ROOT / "tests/sdl3" / name)]
            args += [str(ROOT / "port/render" / source) for source in sources]
            if sdl:
                args += ["-I", str(SDL_ROOT / "include"), "-L", str(SDL_ROOT / "lib"), "-lSDL3"]
            args += ["-lm", "-o", str(exe)]
            result = subprocess.run(args, cwd=ROOT, env=environment,
                                    capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe)], env=environment,
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_native_patterns_materials_and_painter_order(self):
        self.run_probe("modern_probe.c", ["renderer_modern.c", "triangulate2d.c", "modern_wheel.c", "renderer_scene.c", "edge_chain_tessellator.c"], True)

    def test_native_panorama_yaw_roll_pitch_and_horizontal_extension(self):
        self.run_probe("sky_probe.c", ["renderer_modern.c", "triangulate2d.c", "modern_wheel.c", "renderer_scene.c", "edge_chain_tessellator.c"], True)

    def test_cockpit_uniform_width_scale_crop_and_world_clearance(self):
        self.run_probe("cockpit_probe.c", ["renderer_modern.c", "triangulate2d.c", "modern_wheel.c", "renderer_scene.c", "edge_chain_tessellator.c"], True)

    def test_modern_scene_and_gpu_failure_never_present_faithful_world(self):
        compiler = compiler_path()
        self.assertIsNotNone(compiler)
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-render-failure-") as directory:
            exe = Path(directory) / "probe.exe"
            result = subprocess.run([compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                "-DPORT_RENDER_INSTRUMENT", "-I", str(ROOT / "port"),
                "-I", str(SDL_ROOT / "include"), str(ROOT / "tests/sdl3/video_failure_probe.c"),
                str(ROOT / "port/video.c"), "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(exe)],
                env=environment, capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(exe)], env=environment, capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_reference_material_override_is_authoritative_after_geometry_binding(self):
        self.run_probe("scene_material_probe.c", ["renderer_camera_scene.c", "renderer_camera.c", "renderer_scene.c"])

    def test_world_and_ui_write_provenance(self):
        self.run_probe("renderer_ui_probe.c", ["renderer_ui.c"])

    def test_concavity_winding_degeneracy_and_rejection(self):
        self.run_probe("triangulate_probe.c", ["triangulate2d.c"])

    def test_camera_near_clip_reference_depth_and_painter_insertion(self):
        self.run_probe("camera_probe.c", ["renderer_camera.c"])

    def test_continuous_edge_chain_crossings_and_collapsed_contours(self):
        self.run_probe("edge_chain_probe.c", ["edge_chain_tessellator.c"])

    def test_all_recovered_heading_tables_preserve_order_and_extend_each_side(self):
        compiler = compiler_path()
        if compiler is None:
            self.skipTest("requires GCC")
        source = (ROOT / "game/render.c").read_text(encoding="latin-1")
        tables = []
        heading_names = ("hzoneNW", "hzoneNE", "hzoneEN", "hzoneES",
                         "hzoneSE", "hzoneSW", "hzoneWS", "hzoneWN")
        for heading in range(8):
            match = re.search(rf"{heading_names[heading]}\[23\]\s*=\s*\{{(.*?)\}};",
                              source, re.S)
            self.assertIsNotNone(match)
            rows = re.findall(r"\{\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\}", match[1])
            self.assertEqual(len(rows), 23)
            tables.append("{" + ",".join("{" + ",".join(row) + "}" for row in rows) + "}")
        environment = make_test_environment(compiler)
        with tempfile.TemporaryDirectory(prefix="stunts-tiles-") as directory:
            temp = Path(directory)
            (temp / "tile_tables.generated.h").write_text(
                "#include <stdint.h>\nstatic const int8_t headingzones[8][23][3]={" +
                ",".join(tables) + "};\n", encoding="ascii")
            exe = temp / "probe.exe"
            result = subprocess.run([compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                "-I", str(temp), "-I", str(ROOT / "port/render"),
                str(ROOT / "tests/sdl3/tile_probe.c"),
                str(ROOT / "port/render/tile_expansion.c"), "-o", str(exe)],
                cwd=ROOT, env=environment, capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(exe)], env=environment,
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_stock_geometry_does_not_support_a_blind_triangle_fan(self):
        from tools.porting.render_geometry_audit import audit
        report = audit()
        self.assertEqual(report["shapes"], 194)
        self.assertEqual(report["primitives"], 4482)
        self.assertEqual(report["primitive_class_counts"], {0: 4226, 1: 161, 2: 1, 3: 94})
        geometry = report["filled_face_geometry"]
        self.assertEqual(geometry["concave_planar_simple"], 8)
        self.assertEqual(geometry["nonplanar"], 773)
        self.assertEqual(geometry["repeated_coordinate_faces"], 34)
        self.assertEqual(geometry["projected_proper_crossing_faces"], 5)
