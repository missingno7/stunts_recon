"""Source-backed regression checks for SDL3 shape/resource adapters."""
from pathlib import Path
import hashlib
import json
import os
import shutil
import struct
import subprocess
import tempfile
import unittest
import sys


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "porting"))
sys.path.insert(0, str(ROOT / "build" / "python"))
import format_reference

ASSETS = Path(os.environ.get("STUNTS_ASSETS", ROOT / "assets"))
try:
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import OracleImage
except ImportError as error:
    RealModeRunner = MemoryRegion = MemoryWrite = RoutineCase = OracleImage = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None


def host_compiler():
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    return str(configured) if configured.is_file() else shutil.which("gcc")


def synthetic_pvs_archive(specifications):
    count = len(specifications)
    payload_offset = 6 + count * 8
    archive = bytearray(payload_offset)
    struct.pack_into("<H", archive, 4, count)
    relative = 0
    for index, (width, height, flip, reserved) in enumerate(specifications):
        name = f"S{index:03d}".encode("ascii")
        archive[6 + index * 4:10 + index * 4] = name
        struct.pack_into("<I", archive, 6 + count * 4 + index * 4, relative)
        header = struct.pack("<6H", width, height, 0, 0, 0, 0)
        pixels = bytes((1 + (index * 43 + pixel * 29) % 251
                        for pixel in range(width * height)))
        archive.extend(header + bytes((0, 0, flip << 4, reserved)) + pixels)
        relative += len(header) + 4 + len(pixels)
    struct.pack_into("<I", archive, 0, len(archive))
    return bytes(archive)


def original_pvs_call(runner, archive):
    _, shapes = format_reference.parse_shape_archive(archive, "PVS")
    scratch_size = max((shape["width"] * shape["height"] for shape in shapes),
                       default=1) + 32
    case = RoutineCase(
        "synthetic PVS dispatcher", "file_unflip_shape2d",
        args=[0, 0x6000, 0, 0x8000], call="far",
        memory=[MemoryWrite(0x6000, 0, archive),
                MemoryWrite(0x8000, 0, bytes(scratch_size))],
        compare=[MemoryRegion("archive", 0x6000, 0, len(archive))],
        result_registers=("ax",),
    )
    return runner.call(case)


class Sdl3ResourceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = host_compiler()
        if compiler is None:
            raise unittest.SkipTest("a C compiler is needed for the resource harness")
        cls.temporary = tempfile.TemporaryDirectory(prefix="sdl3-resource-")
        cls.executable = Path(cls.temporary.name) / "resource_harness.exe"
        command = [
            compiler, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
            "-ffunction-sections", "-fdata-sections", "-fwhole-program",
            "-DPORT_BUILD", "-Iport",
            str(ROOT / "tests" / "sdl3" / "resource_harness.c"),
            "-Wl,--gc-sections", "-o", str(cls.executable),
        ]
        environment = os.environ.copy()
        environment["PATH"] = (
            str(Path(compiler).resolve().parent) + os.pathsep +
            environment.get("PATH", "")
        )
        build = subprocess.run(command, cwd=ROOT, capture_output=True,
                               text=True, check=False, env=environment)
        if build.returncode != 0:
            cls.temporary.cleanup()
            raise AssertionError(
                f"resource harness compile failed:\n{build.stdout}\n{build.stderr}"
            )

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temporary"):
            cls.temporary.cleanup()

    def run_harness(self, *arguments):
        result = subprocess.run(
            [str(self.executable), *(str(argument) for argument in arguments)],
            cwd=ROOT, capture_output=True, text=True, check=False,
        )
        self.assertEqual(result.returncode, 0,
                         f"resource harness failed ({result.returncode}):\n"
                         f"{result.stdout}\n{result.stderr}")

    def test_blitters_spans_and_allocation_lifetimes(self):
        self.run_harness()

    def test_incnums_initializer_matches_locked_original_bytes(self):
        symbols = json.loads((ROOT / "layout" / "data-symbols.json").read_text())
        lock = json.loads((ROOT / "layout" / "oracle.lock.json").read_text())
        island = symbols["code_islands"]["incnums"]
        symbol = symbols["symbols"]["_incnums"]
        self.assertEqual(island["start"], symbol["load_address"])
        self.assertEqual(island["end"] - island["start"], symbol["width"])
        self.assertEqual(symbols["oracle_sha256"], lock["load_image"]["sha256"])

        with tempfile.TemporaryDirectory(prefix="sdl3-incnums-") as temp:
            dumped = Path(temp) / "incnums.bin"
            self.run_harness("--incnums-dump", dumped)
            contents = dumped.read_bytes()
        self.assertEqual(len(contents), symbol["width"])
        self.assertEqual(hashlib.sha256(contents).hexdigest(), island["sha256"])

    def test_available_p3s_assets_fit_validated_chunk_spans(self):
        assets = sorted((ROOT / "assets").glob("*.P3S"))
        if not assets:
            self.skipTest("original P3S assets are not present")
        for asset in assets:
            with self.subTest(asset=asset.name):
                self.run_harness(asset)

    def test_bundled_pvs_and_pes_transforms_match_reference(self):
        assets = sorted((ROOT / "assets").glob("*.PVS"))
        assets += sorted((ROOT / "assets").glob("*.PES"))
        if not assets:
            self.skipTest("original PVS/PES assets are not present")
        with tempfile.TemporaryDirectory(prefix="sdl3-unflip-") as temp:
            output = Path(temp) / "transformed.bin"
            for asset in assets:
                with self.subTest(asset=asset.name):
                    kind = asset.suffix[1:].lower()
                    decoded, _ = format_reference.decompress(asset.read_bytes())
                    expected = (format_reference.unflip_pvs(decoded)
                                if kind == "pvs"
                                else format_reference.unflip_pes(decoded))
                    self.run_harness(f"--unflip-{kind}", asset, output)
                    self.assertEqual(output.read_bytes(), expected)

    @unittest.skipUnless(ASSETS.is_dir(), "original game assets are not provisioned")
    def test_synthetic_pvs_handlers_match_locked_dos(self):
        if ORACLE_IMPORT_ERROR is not None:
            self.skipTest(f"locked machine oracle dependency is unavailable: {ORACLE_IMPORT_ERROR}")
        runner = RealModeRunner(OracleImage.load(ASSETS))
        cases = (
            ("flags 1, 2, and 3", synthetic_pvs_archive([
                (3, 4, 1, 0), (2, 5, 2, 0), (3, 5, 3, 0),
            ]), 0),
            ("flag 4 stops after earlier shapes", synthetic_pvs_archive([
                (3, 4, 1, 0), (2, 2, 4, 0), (2, 3, 3, 0),
            ]), 1),
        )
        with tempfile.TemporaryDirectory(prefix="sdl3-pvs-machine-") as temp:
            temp_dir = Path(temp)
            source = temp_dir / "synthetic.pvs"
            output = temp_dir / "transformed.pvs"
            for label, archive, expected_ax in cases:
                with self.subTest(case=label):
                    original = original_pvs_call(runner, archive)
                    self.assertEqual(original.registers["ax"], expected_ax)
                    source.write_bytes(archive)
                    self.run_harness("--unflip-pvs-raw", source, output)
                    self.assertEqual(output.read_bytes(), original.memory["archive"])


if __name__ == "__main__":
    unittest.main()
