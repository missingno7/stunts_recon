"""Compile the port's derived audio views for all three audio translation units."""
from __future__ import annotations

from pathlib import Path
import os
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


class Sdl3AudioLayoutTests(unittest.TestCase):
    def test_derived_host_views_agree_across_audio_units(self) -> None:
        compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
        if not compiler.is_file():
            candidate = shutil.which("i686-w64-mingw32-gcc")
            if candidate is None:
                self.skipTest("GCC with an i686 target is required for the SDL3 ABI check")
            compiler = Path(candidate)
        compiler_env = os.environ.copy()
        compiler_env["PATH"] = str(compiler.parent) + os.pathsep + compiler_env.get("PATH", "")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-audio-abi-") as directory:
            include_dir = Path(directory)
            shutil.copyfile(PORT_INCLUDE / "stunts_types.h", include_dir / "stunts_types.h")
            original = (PORT_INCLUDE / "stunts_structs.h").read_text(encoding="latin-1")
            derived = adapt_aggregate_views(legacy_target_widths(original))
            wanted = {"AUDIOCHUNK", "AUDIOVOICE", "AudioChunk", "AudioVoice",
                      "AudioPayload", "AudioTimer"}

            source = ROOT / "tests" / "sdl3" / "audio_layout_probe.c"
            for unit in ("obj_seg027", "obj_seg028", "obj_seg007"):
                # Each derived view is repeated for every accepted TU. Compile
                # its exact selected blocks, without parsing the whole header
                # (including hundreds of unrelated aggregates) three times.
                blocks = []
                found = set()
                for match in re.finditer(
                    rf"#if defined\(STUNTS_TU_{unit}\)\n(.*?)\n#endif",
                    derived, re.S,
                ):
                    block = match.group(0)
                    declaration = re.search(r"\bstruct (\w+)\s*\{", block)
                    if declaration and declaration.group(1) in wanted:
                        found.add(declaration.group(1))
                        blocks.append(block)
                self.assertEqual(found, wanted, f"{unit}: missing audio views")
                (include_dir / "stunts_audio_views.h").write_text(
                    '#include "stunts_types.h"\n' + "\n".join(blocks),
                    encoding="latin-1",
                )
                result = subprocess.run(
                    [str(compiler), "-std=gnu11", "-Werror", "-fsyntax-only",
                     f"-DSTUNTS_TU_{unit}=1", "-I", str(include_dir), str(source)],
                    cwd=ROOT, capture_output=True, text=True, check=False,
                    timeout=30, env=compiler_env,
                )
                self.assertEqual(result.returncode, 0,
                                 f"{unit}: {result.stdout}{result.stderr}")


if __name__ == "__main__":
    unittest.main()
