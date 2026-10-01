"""Compare both SDL text paths with their locked 16-bit machine entries."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [
    str(ROOT / "tools"),
    str(ROOT / "tools" / "porting"),
    str(ROOT / "build" / "python"),
    str(ROOT),
]

try:
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import OracleImage, SymbolMap, dgroup_segment
except ImportError as error:  # pragma: no cover - machine-oracle dependency
    RealModeRunner = MemoryRegion = MemoryWrite = RoutineCase = None
    OracleImage = SymbolMap = dgroup_segment = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None


VIDEO_SEGMENT = 0xA000
VIDEO_BYTES = 0x10000
FONT_SEGMENT = 0x5000
FONTDEF_SEG_OFFSET = 0x4E90
SPRITE1_OFFSET = 0x5F20
LINE_TABLE_OFFSET = 0x0600
TEXT_OFFSET = 0xD000
CODE_LOAD_SEGMENT = 0x1000
OP_FONT_DRAW = 0
OP_DRAW_TEXT_AT = 1
OP_SETUP = 2
OP_FONT_OP = 3
OP_FONT_OP2 = 4
RENDER_ENTRY = {
    OP_FONT_DRAW: "font_draw_text",
    OP_DRAW_TEXT_AT: "draw_text_at",
}
FIXED_FRAME = bytes(
    (i * 17 + (i >> 8) * 3 + 11) & 0xFF for i in range(VIDEO_BYTES)
)


def host_compiler() -> Path:
    supplied = os.environ.get("CC")
    candidates = [Path(supplied)] if supplied else []
    candidates.extend((
        Path(r"C:\msys64\mingw32\bin\gcc.exe"),
        Path(r"C:\msys64\mingw64\bin\gcc.exe"),
    ))
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    found = shutil.which("gcc") or shutil.which("i686-w64-mingw32-gcc")
    if found:
        return Path(found).resolve()
    raise unittest.SkipTest("a MinGW GCC compiler is needed for the SDL font probe")


def compiler_environment(compiler: Path) -> dict[str, str]:
    environment = os.environ.copy()
    environment["PATH"] = os.pathsep.join(
        [str(compiler.parent), environment.get("PATH", "")]
    )
    return environment


def compile_host_probe() -> tuple[Path, dict[str, str]]:
    compiler = host_compiler()
    worker = ROOT / "build" / "workers" / "font_oracle" / f"run_{os.getpid()}"
    worker.mkdir(parents=True, exist_ok=True)
    executable = worker / "font_oracle_harness.exe"
    command = [
        str(compiler), "-std=gnu11", "-O2", "-ffunction-sections",
        "-fdata-sections", "-DPORT_BUILD=1",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        str(ROOT / "tests" / "sdl3" / "font_oracle_harness.c"),
        "-Wl,--gc-sections", "-o", str(executable),
    ]
    environment = compiler_environment(compiler)
    build = subprocess.run(
        command, cwd=ROOT, env=environment, capture_output=True, text=True,
        timeout=60, check=False,
    )
    if build.returncode:
        raise AssertionError(
            "font oracle host probe compile failed:\n" + build.stdout + build.stderr
        )
    return executable, environment


def font_default_from_oracle(oracle: OracleImage) -> bytes:
    recipe = __import__("json").loads(
        (ROOT / "recipes" / "fardata_11039.json").read_text(encoding="utf-8")
    )
    start, end = recipe["start"], recipe["end"]
    data = oracle.load_image[start:end]
    if len(data) != 1408:
        raise AssertionError(f"locked built-in FONTDEF has {len(data)} bytes")
    return data


def read_source_default_font() -> bytes:
    import re

    source = (ROOT / "src" / "fardata_11039.c").read_text(encoding="utf-8")
    match = re.search(r"fontdef_default\[1408\]\s*=\s*\{(.*?)\}", source, re.S)
    if match is None:
        raise AssertionError("could not find the built-in font initializer")
    values = re.findall(r"(?<![\w.])(?:0x[\da-fA-F]+|\d+)(?![\w.])", match.group(1))
    return bytes(int(value, 0) for value in values)


def load_runtime_fonts(default_font: bytes) -> dict[str, bytes]:
    fonts = {"builtin-1408": default_font}
    for name in ("FONTDEF.FNT", "FONTN.FNT", "FONTLED.FNT"):
        path = ROOT / "assets" / name
        if not path.is_file():
            raise AssertionError(f"original runtime font asset is missing: {path}")
        fonts[name] = path.read_bytes()
    for name, font in fonts.items():
        if len(font) < 22 + 256 * 2:
            raise AssertionError(f"{name} is shorter than its FONTDEF table")
    return fonts


def frame_seed() -> bytes:
    return FIXED_FRAME


def pack_host_input(operation: int, font: bytes, *, builtin: bool,
                    foreground: int, background: int, x: int, y: int,
                    clip: tuple[int, int, int, int], text: bytes, count: int,
                    initial_frame: bytes) -> bytes:
    if not text or text[-1] != 0 or b"\0" not in text:
        raise ValueError("font test strings must include a terminal NUL")
    if len(initial_frame) != VIDEO_BYTES:
        raise ValueError("font test frame must cover the full 64 KiB video window")
    if len(font) > 4096 or len(text) > 512:
        raise ValueError("font record or text fixture exceeds the host probe")
    fields = (
        operation, 0 if builtin else 1, len(font), foreground, background,
        x & 0xFFFF, y & 0xFFFF, *clip, count, len(text),
    )
    return struct.pack("<13H", *fields) + font + text + initial_frame


class FontOracleHarness:
    def __init__(self, runner: RealModeRunner, symbols: SymbolMap,
                 host: Path, environment: dict[str, str],
                 data_symbols: dict):
        self.runner = runner
        self.symbols = symbols
        self.host = host
        self.environment = environment
        self.dgroup = dgroup_segment(CODE_LOAD_SEGMENT)
        self.fontdefseg_offset = (
            data_symbols["_fontdefseg"]["load_address"] -
            data_symbols["frame_load_address"]
        )
        self.sprite1_offset = (
            data_symbols["_sprite1"]["load_address"] -
            symbols.resolve("font_draw_text").code_base
        )
        if self.fontdefseg_offset != FONTDEF_SEG_OFFSET:
            raise AssertionError("locked fontdefseg offset changed; re-ground this fixture")
        if self.sprite1_offset != SPRITE1_OFFSET:
            raise AssertionError("locked sprite1 offset changed; re-ground this fixture")

    def host_call(self, operation: int, font: bytes, *, builtin: bool = False,
                  foreground: int = 0x17, background: int = 0xE4,
                  x: int = 7, y: int = 9,
                  clip: tuple[int, int, int, int] = (250, 260, 150, 160),
                  text: bytes = b"A\0", count: int = 0xFFFF,
                  initial_frame: bytes | None = None) -> bytes:
        payload = pack_host_input(
            operation, font, builtin=builtin, foreground=foreground,
            background=background, x=x, y=y, clip=clip, text=text,
            count=count, initial_frame=frame_seed() if initial_frame is None
            else initial_frame,
        )
        result = subprocess.run(
            [str(self.host)], input=payload, env=self.environment,
            capture_output=True, timeout=20, check=False,
        )
        if result.returncode:
            raise AssertionError(
                f"font host probe failed ({result.returncode}): {result.stderr.decode(errors='replace')}"
            )
        expected_size = VIDEO_BYTES + len(font) + 2
        if len(result.stdout) != expected_size:
            raise AssertionError(
                f"font host probe returned {len(result.stdout)} bytes, expected {expected_size}"
            )
        return result.stdout

    def machine_setup(self, font: bytes, foreground: int,
                      background: int) -> bytes:
        routine = self.symbols.resolve("font_set_unk")
        code_segment, _entry = routine.far_at(CODE_LOAD_SEGMENT)
        writes = [
            MemoryWrite(FONT_SEGMENT, 0, font),
            MemoryWrite(self.dgroup, self.fontdefseg_offset,
                        struct.pack("<H", FONT_SEGMENT)),
        ]
        case = RoutineCase(
            f"font header colors {foreground:#06x}/{background:#06x}",
            "font_set_unk", args=[foreground, background], call="far",
            memory=writes,
            compare=[MemoryRegion("font", FONT_SEGMENT, 0, len(font))],
        )
        del code_segment  # The routine resolves its own frozen code frame.
        return self.runner.call(case).memory["font"]

    def machine_call(self, operation: int, font: bytes, *,
                     foreground: int = 0x17, background: int = 0xE4,
                     x: int = 7, y: int = 9,
                     clip: tuple[int, int, int, int] = (250, 260, 150, 160),
                     text: bytes = b"A\0", count: int = 0xFFFF,
                     initial_frame: bytes | None = None) -> tuple[bytes, int]:
        prepared_font = self.machine_setup(font, foreground, background)
        routine_name = {
            OP_SETUP: None,
            OP_FONT_OP: "font_op",
            OP_FONT_OP2: "font_op2",
        }.get(operation, RENDER_ENTRY.get(operation))
        if routine_name is None:
            unchanged = frame_seed() if initial_frame is None else initial_frame
            return unchanged + prepared_font + b"\0\0", 0

        routine = self.symbols.resolve(routine_name)
        code_segment, _entry = routine.far_at(CODE_LOAD_SEGMENT)
        descriptor = [0] * 15
        descriptor[1] = VIDEO_SEGMENT
        descriptor[5] = LINE_TABLE_OFFSET
        descriptor[6], descriptor[7], descriptor[8], descriptor[9] = clip
        row_table = b"".join(struct.pack("<H", row * 320)
                             for row in range(200))
        writes = [
            MemoryWrite(FONT_SEGMENT, 0, prepared_font),
            MemoryWrite(self.dgroup, self.fontdefseg_offset,
                        struct.pack("<H", FONT_SEGMENT)),
            MemoryWrite(self.dgroup, TEXT_OFFSET, text),
            MemoryWrite(VIDEO_SEGMENT, 0,
                        frame_seed() if initial_frame is None else initial_frame),
            MemoryWrite(code_segment, self.sprite1_offset,
                        struct.pack("<15H", *descriptor)),
            MemoryWrite(code_segment, LINE_TABLE_OFFSET, row_table),
        ]
        if operation in RENDER_ENTRY:
            args = [TEXT_OFFSET, x & 0xFFFF, y & 0xFFFF]
        elif operation == OP_FONT_OP:
            args = [TEXT_OFFSET, count & 0xFFFF]
        else:
            args = [TEXT_OFFSET]
        case = RoutineCase(
            f"{routine_name} text={text!r} at {x},{y}", routine_name,
            args=args, call="far", memory=writes,
            compare=[MemoryRegion("video", VIDEO_SEGMENT, 0, VIDEO_BYTES),
                     MemoryRegion("font", FONT_SEGMENT, 0, len(font))],
            result_registers=("ax",),
        )
        result = self.runner.call(case)
        return (result.memory["video"] + result.memory["font"] +
                struct.pack("<H", result.registers["ax"])), result.registers["ax"]


@unittest.skipIf(ORACLE_IMPORT_ERROR is not None,
                 f"pinned 16-bit oracle dependencies unavailable: {ORACLE_IMPORT_ERROR}")
class Sdl3FontOracleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        from json import loads

        cls.oracle = OracleImage.load()
        cls.symbols = SymbolMap()
        cls.runner = RealModeRunner(cls.oracle, instruction_budget=500_000)
        cls.host, cls.environment = compile_host_probe()
        cls.default_font = font_default_from_oracle(cls.oracle)
        source_default = read_source_default_font()
        if source_default != cls.default_font:
            raise AssertionError("src/fardata_11039.c differs from the locked built-in FONTDEF")
        data_symbols = loads(
            (ROOT / "layout" / "data-symbols.json").read_text(encoding="utf-8")
        )
        cls.harness = FontOracleHarness(
            cls.runner, cls.symbols, cls.host, cls.environment,
            data_symbols["symbols"] | {"frame_load_address": data_symbols["frame_load_address"]},
        )
        cls.fonts = load_runtime_fonts(cls.default_font)

    def assert_complete_output_matches(self, label: str, expected: bytes,
                                       actual: bytes, font_length: int, *,
                                       include_result: bool = False) -> None:
        if expected == actual:
            return
        frame_end = VIDEO_BYTES
        font_end = frame_end + font_length
        if expected[:frame_end] != actual[:frame_end]:
            index = next(i for i, (a, b) in enumerate(zip(expected, actual)) if a != b)
            self.fail(
                f"{label}: video[{index:#06x}] expected {expected[index]:02x}, "
                f"got {actual[index]:02x}; pixel=({index % 320},{index // 320})"
            )
        if expected[frame_end:font_end] != actual[frame_end:font_end]:
            offset = next(
                i for i, (a, b) in enumerate(zip(expected[frame_end:font_end],
                                                   actual[frame_end:font_end]))
                if a != b
            )
            self.fail(
                f"{label}: font[{offset:#06x}] expected "
                f"{expected[frame_end + offset]:02x}, got {actual[frame_end + offset]:02x}"
            )
        if include_result and expected[font_end:font_end + 2] != actual[font_end:font_end + 2]:
            self.fail(
                f"{label}: result word expected {expected[font_end:font_end + 2].hex()}, "
                f"got {actual[font_end:font_end + 2].hex()}"
            )

    def compare_case(self, label: str, operation: int, font: bytes, *,
                     builtin: bool = False, foreground: int = 0x17,
                     background: int = 0xE4, text: bytes = b"A\0",
                     count: int = 0xFFFF, x: int = 7, y: int = 9,
                     clip: tuple[int, int, int, int] = (250, 260, 150, 160),
                     initial_frame: bytes | None = None) -> bytes:
        host = self.harness.host_call(
            operation, font, builtin=builtin, foreground=foreground,
            background=background, text=text, count=count, x=x, y=y,
            clip=clip,
            initial_frame=initial_frame,
        )
        expected, _result = self.harness.machine_call(
            operation, font, foreground=foreground, background=background,
            text=text, count=count, x=x, y=y, clip=clip,
            initial_frame=initial_frame,
        )
        self.assert_complete_output_matches(
            label, expected, host, len(font),
            include_result=operation in (OP_FONT_OP, OP_FONT_OP2),
        )
        return host

    def test_both_render_entries_match_locked_frames_for_original_font_records(self):
        cases = (
            ("builtin-1408", True, b"Ag\0", 0x17, 0xE4),
            ("FONTDEF.FNT", False, b"Stunts\0", 0x22, 0xE1),
            ("FONTN.FNT", False, b"Game\0", 0xC1, 0x06),
            ("FONTLED.FNT", False, b"012345\0", 0x09, 0xD7),
        )
        for name, builtin, text, foreground, background in cases:
            with self.subTest(font=name, path="font_draw_text"):
                output = self.compare_case(
                    f"{name} transparent", OP_FONT_DRAW, self.fonts[name],
                    builtin=builtin, foreground=foreground, background=background,
                    text=text,
                )
                self.assertNotEqual(output[:VIDEO_BYTES], frame_seed(),
                                    f"{name} transparent entry drew no pixels")
                seed = frame_seed()
                self.assertTrue(
                    any(output[row * 320 + column] != seed[row * 320 + column]
                        for row in range(9, 65) for column in range(7, 220)),
                    f"{name} transparent path did not write beyond its restrictive clip",
                )
            with self.subTest(font=name, path="draw_text_at"):
                output = self.compare_case(
                    f"{name} opaque", OP_DRAW_TEXT_AT, self.fonts[name],
                    builtin=builtin, foreground=background, background=foreground,
                    text=text,
                )
                self.assertNotEqual(output[:VIDEO_BYTES], frame_seed(),
                                    f"{name} opaque entry drew no pixels")
                seed = frame_seed()
                self.assertTrue(
                    any(output[row * 320 + column] != seed[row * 320 + column]
                        for row in range(9, 65) for column in range(7, 220)),
                    f"{name} opaque path did not write beyond its restrictive clip",
                )

    def test_setup_measurement_and_proportional_header_outputs_match(self):
        fontdef = self.fonts["FONTDEF.FNT"]
        for operation, text, count in (
            (OP_FONT_OP, b"A7x\0", 2),
            (OP_FONT_OP2, b"A7x\0", 0),
        ):
            with self.subTest(operation=operation):
                self.compare_case(
                    f"FONTDEF proportional measure {operation}", operation,
                    fontdef, foreground=0xAB07, background=0xCD02,
                    text=text, count=count,
                )

        fontled = self.fonts["FONTLED.FNT"]
        missing_a = struct.unpack_from("<H", fontled, 22 + ord("A") * 2)[0]
        glyph_one = struct.unpack_from("<H", fontled, 22 + ord("1") * 2)[0]
        self.assertEqual(missing_a, 0, "FONTLED fixture must include a missing glyph")
        self.assertNotEqual(glyph_one, 0, "FONTLED fixture must include digit 1")
        measured = self.compare_case(
            "FONTLED counted measurement skips missing glyph", OP_FONT_OP,
            fontled, foreground=0xAB07, background=0xCD02,
            text=b"A12\0", count=1,
        )
        self.assertEqual(struct.unpack_from("<H", measured, VIDEO_BYTES + len(fontled))[0],
                         6, "count=1 must stop after the first drawable FONTLED glyph")
        dirty_header_font = bytearray(self.default_font)
        dirty_header_font[1] = 0xA5
        dirty_header_font[3] = 0x5A
        setup_output = self.compare_case(
            "font_setup_unknown zeroes word high bytes", OP_SETUP,
            bytes(dirty_header_font), foreground=0xAB07, background=0xCD02,
        )
        self.assertEqual(setup_output[VIDEO_BYTES:VIDEO_BYTES + 4],
                         bytes((0x07, 0, 0x02, 0)))

        text = b"Stunts\0"
        rendered = self.compare_case(
            "FONTDEF proportional header mutation", OP_FONT_DRAW, fontdef,
            foreground=0x37, background=0xC2, text=text,
        )
        final_glyph_offset = struct.unpack_from(
            "<H", fontdef, 22 + ord("s") * 2
        )[0]
        final_advance = fontdef[final_glyph_offset]
        self.assertEqual(rendered[VIDEO_BYTES + 12], (final_advance + 7) >> 3)
        self.assertEqual(
            struct.unpack_from("<H", rendered, VIDEO_BYTES + 16)[0],
            final_advance,
        )

    def test_short_and_space_overwrites_ignore_stale_text_tail(self):
        font = self.fonts["FONTDEF.FNT"]
        for operation in (OP_FONT_DRAW, OP_DRAW_TEXT_AT):
            with self.subTest(operation=operation):
                long_result = self.compare_case(
                    f"FONTDEF long text {operation}", operation, font,
                    foreground=0x19, background=0xD3,
                    text=b"StuntsLong\0",
                )
                font_after_long = long_result[VIDEO_BYTES:VIDEO_BYTES + len(font)]
                frame_after_long = long_result[:VIDEO_BYTES]
                # This is a previous longer buffer after its first byte was
                # overwritten by a space and its second byte by the terminator.
                stale_tail = b" \0tuntsLong\0"
                short_result = self.compare_case(
                    f"FONTDEF overwritten space with stale tail {operation}",
                    operation, font_after_long,
                    foreground=0x19, background=0xD3, text=stale_tail,
                    initial_frame=frame_after_long,
                )
                self.assert_complete_output_matches(
                    f"FONTDEF short-buffer stale-tail parity {operation}",
                    self.harness.machine_call(
                        operation, font_after_long, foreground=0x19,
                        background=0xD3, text=stale_tail,
                        initial_frame=frame_after_long,
                    )[0],
                    short_result, len(font),
                )


if __name__ == "__main__":
    unittest.main()
