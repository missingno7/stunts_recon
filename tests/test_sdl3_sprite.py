"""Source-backed regression for the interlaced SDL sprite translation."""
import hashlib
import json
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest
import zlib


ROOT = Path(__file__).resolve().parents[1]
ASSETS = Path(os.environ.get("STUNTS_ASSETS", ROOT / "assets"))
sys_path = ROOT / "tools" / "porting"
import sys
sys.path.insert(0, str(sys_path))
import format_reference  # noqa: E402


def load_pf_fixture(name):
    directory = ROOT / "tests" / "fixtures" / "sdl3"
    metadata = json.loads((directory / f"{name}.json").read_text(encoding="utf-8"))
    payload = zlib.decompress((directory / metadata["payload"]).read_bytes())
    if hashlib.sha256(payload).hexdigest() != metadata["payload_sha256"]:
        raise AssertionError(f"{name} PF fixture payload digest mismatch")
    return payload[:64000], payload[64000:]


def host_compiler():
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    return str(configured) if configured.is_file() else shutil.which("gcc")


@unittest.skipUnless(ASSETS.is_dir(), "original game assets are not provisioned")
class SpriteUnk3AssetTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = host_compiler()
        if compiler is None:
            raise unittest.SkipTest("a C compiler is needed for the sprite harness")
        compressed = (ASSETS / "SDTITL.PVS").read_bytes()
        cls.archive_bytes, _ = format_reference.decompress(compressed)
        cls.archive_bytes = format_reference.unflip_pvs(cls.archive_bytes)
        cls.archive, _ = format_reference.parse_shape_archive(
            cls.archive_bytes, "PVS")
        cls.shapes = {}
        for entry in cls.archive["entries"]:
            if entry["name"] in ("prod", "titl"):
                cls.shapes[entry["name"]] = cls.archive_bytes[
                    entry["start"]:entry["end"]]
        if set(cls.shapes) != {"prod", "titl"}:
            raise AssertionError("SDTITL.PVS must contain prod and titl shapes")
        main_bytes, _ = format_reference.decompress((ASSETS / "SDMAIN.PVS").read_bytes())
        main_archive, _ = format_reference.parse_shape_archive(main_bytes, "PVS")
        palette_entry = next(e for e in main_archive["entries"] if e["name"] == "!pal")
        cls.asset_palette = main_bytes[palette_entry["start"] + 16:
                                       palette_entry["start"] + 16 + 768]
        cls.temp = tempfile.TemporaryDirectory(prefix="sdl3-sprite-unk3-")
        cls.temp_path = Path(cls.temp.name)
        cls.exe = cls.temp_path / "sprite_unk3_harness.exe"
        command = [
            compiler, "-std=c11", "-O2", "-ffunction-sections",
            "-fdata-sections", "-Wno-unused-function", "-Wno-unused-variable",
            str(ROOT / "tests" / "sdl3" / "sprite_unk3_harness.c"),
            str(ROOT / "port" / "sincos.c"),
            "-Wl,--gc-sections", "-o", str(cls.exe),
        ]
        environment = os.environ.copy()
        environment["PATH"] = str(Path(compiler).resolve().parent) + os.pathsep + environment.get("PATH", "")
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True, check=False, env=environment)
        if result.returncode:
            raise AssertionError(f"sprite harness compile failed ({result.returncode}):\n"
                                 f"command: {command}\n"
                                 f"{result.stdout}\n{result.stderr}")

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temp"):
            cls.temp.cleanup()

    def test_four_phase_blit_copies_real_title_shapes_exactly(self):
        """Exercise prod (the former 811-pixel fault) and full-screen titl."""
        for name, shape_bytes in self.shapes.items():
            with self.subTest(shape=name):
                shape_file = self.temp_path / f"{name}.shape"
                output_file = self.temp_path / f"{name}.pixels"
                shape_file.write_bytes(shape_bytes)
                result = subprocess.run([str(self.exe), str(shape_file),
                                         str(output_file)],
                                        cwd=ROOT, capture_output=True,
                                        text=True, check=False)
                self.assertEqual(result.returncode, 0, result.stderr)

                width = int.from_bytes(shape_bytes[0:2], "little")
                height = int.from_bytes(shape_bytes[2:4], "little")
                x = int.from_bytes(shape_bytes[8:10], "little")
                y = int.from_bytes(shape_bytes[10:12], "little")
                source = shape_bytes[16:16 + width * height]
                expected = bytearray(320 * 200)
                for row in range(height):
                    start = (y + row) * 320 + x
                    expected[start:start + width] = source[row * width:(row + 1) * width]
                self.assertEqual(output_file.read_bytes(), expected)
                screen_name = "splash" if name == "prod" else "title"
                reference_pixels, reference_palette = load_pf_fixture(screen_name)
                self.assertEqual(bytes(expected), reference_pixels)
                self.assertEqual(self.asset_palette, reference_palette)


if __name__ == "__main__":
    unittest.main()
