"""Exercise the polygon-list sentinel in the actual SDL3 translation unit."""
from __future__ import annotations

import os
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
import sys
sys.path.insert(0, str(ROOT / "tools" / "porting"))
from host_probe_modes import legacy_target_widths  # noqa: E402
from port.game_abi import adapt_polygon_storage  # noqa: E402


def polygon_storage_excerpt(overlay: str) -> str:
    """Keep the built storage declarations and both built functions unchanged."""
    parts = []
    for pattern in (
        r"(?m)^#define POLYINFO_CAPACITY 400$",
        r"(?m)^#define POLYINFO_RESET_SENTINEL 0xffff$",
        r"(?m)^#define polyinfo_reset_marker [^\n]+$",
        r"(?m)^U16 polygonnumber;$",
        r"(?m)^static U16 polyinfo_offset;$",
        r"(?m)^static I16 poly_list_insert_result;$",
        r"(?m)^I16 poly_cursor1;$",
        r"(?m)^I16 facenodeiterator;$",
        r"(?m)^I16 polygon_link_3_list_iter;$",
        r"(?m)^I16 poly_link_listit4;$",
        r"(?m)^static I16 poly_link_list\[[^\n;]+\];$",
        r"(?m)^static I8 far\* polyinfoptr;$",
        r"(?m)^static struct POLYINFO far\* poly_info_ptrs\[[^\n;]+\];$",
        r"(?m)^unsigned char transformed_vert_count;$",
    ):
        matches = list(re.finditer(pattern, overlay))
        if len(matches) != 1:
            raise AssertionError(f"Expected one PORT_BUILD declaration: {pattern}")
        parts.append(matches[0].group())
    for name in ("polyinfo_reset", "insert_newest_poly_in_poly_linked_list_40ED6"):
        start = re.search(
            rf"(?m)^(?:extern )?(?:void|U16) {name}\([^;\n]*\)\s*\{{",
            overlay,
        )
        if start is None:
            raise AssertionError(f"Missing production function: {name}")
        depth = 1
        end = start.end()
        while depth:
            if end >= len(overlay):
                raise AssertionError(f"Unterminated production function: {name}")
            if overlay[end] == "{":
                depth += 1
            elif overlay[end] == "}":
                depth -= 1
            end += 1
        parts.append(overlay[start.start():end])
    return "\n".join(parts) + "\n"


class Sdl3PolygonStorageTests(unittest.TestCase):
    def test_historical_head_is_the_word_after_400_links(self) -> None:
        names = json.loads((ROOT / "layout" / "names-registry.json").read_text(
            encoding="utf-8"))["names"]
        symbols = json.loads((ROOT / "layout" / "data-symbols.json").read_text(
            encoding="utf-8"))
        base = symbols["frame_load_address"]
        link = 200406
        marker = symbols["symbols"]["_polyinfo_reset_marker"]["load_address"]
        self.assertEqual(names[str(link)]["name"], "poly_link_list")
        self.assertEqual(names[str(marker)]["name"], "polyinfo_reset_marker")
        self.assertEqual((link - base, marker - base), (0x5766, 0x5A86))
        self.assertEqual(marker - link, 400 * 2)

    def test_list_head_and_polygon_pool_have_separate_storage(self) -> None:
        # Exercise the exact port adapter against the immutable historical TU.
        original = (ROOT / "src" / "obj_seg006.c").read_text(encoding="latin-1")
        overlay = adapt_polygon_storage(legacy_target_widths(original))

        compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        if not compiler.is_file():
            selected = os.environ.get("CC") or shutil.which("gcc")
            resolved = shutil.which(selected) if selected else None
            if resolved is None:
                self.skipTest("GCC is required for the SDL3 host checks")
            compiler = Path(resolved)
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = str(compiler.parent) + os.pathsep + compiler_env.get("PATH", "")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-poly-") as directory:
            executable = Path(directory) / "polygon-storage-probe.exe"
            excerpt = Path(directory) / "polygon_storage_excerpt.inc"
            excerpt.write_text(polygon_storage_excerpt(overlay),
                               encoding="latin-1")
            source = ROOT / "tests" / "sdl3" / "polygon_storage_probe.c"
            result = subprocess.run(
                [str(compiler), "-std=gnu11", "-O0", "-Wall", "-Wextra", "-Werror",
                 "-I", str(directory),
                 str(source), "-o", str(executable)],
                cwd=ROOT, capture_output=True, text=True, check=False,
                timeout=30, env=compiler_env,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run(
                [str(executable)], cwd=ROOT, capture_output=True,
                text=True, check=False,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
