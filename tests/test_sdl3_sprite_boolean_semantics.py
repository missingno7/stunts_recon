"""Compare compiled SDL sprite blitters with the locked DOS entrypoints."""
from __future__ import annotations

from dataclasses import dataclass
import os
from pathlib import Path
import random
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
ASSETS = Path(os.environ.get("STUNTS_ASSETS", ROOT / "assets"))
sys.path.insert(0, str(ROOT / "tools"))
from mz import MZ  # noqa: E402
from oracle import verify  # noqa: E402

sys.path.insert(0, str(ROOT / "tools" / "porting"))
import format_reference  # noqa: E402

sys.path.insert(0, str(ROOT / "build" / "python"))
try:
    from unicorn import Uc, UC_ARCH_X86, UC_HOOK_CODE, UC_MODE_16
    from unicorn.x86_const import (
        UC_X86_REG_BP, UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_EFLAGS,
        UC_X86_REG_ES, UC_X86_REG_IP, UC_X86_REG_SP, UC_X86_REG_SS,
    )
except ImportError:  # pragma: no cover - required workstation dependency
    Uc = None


VIDEO_BYTES = 0x10000
SCREEN_WIDTH = 320
SCREEN_HEIGHT = 200
CODE_BASE = 0x20000
CODE_SEGMENT = CODE_BASE >> 4
SOURCE_SEGMENT = 0x1000
SOURCE_OFFSET = 0x1000
DESTINATION_SEGMENT = 0x3000
STACK_SEGMENT = 0x4000
STACK_POINTER = 0xF000
RETURN_IP = 0x0100
LINE_TABLE_OFFSET = 0x0600

# Operation order is also used by the compiled harness input protocol.
ENTRY_POINTS = {
    "and": (0, 0x23890),
    "and_alt": (1, 0x23BBC),
    "and_alt2": (2, 0x2386C),
    "or_alt": (3, 0x24060),
}


@dataclass(frozen=True)
class BlitCase:
    name: str
    operation: str
    shape: bytes
    x: int
    y: int
    clip: tuple[int, int, int, int]
    destination: bytes
    pitch: int = SCREEN_WIDTH
    surface_height: int = SCREEN_HEIGHT


def host_compiler() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    configured = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    if configured.is_file():
        return str(configured)
    return shutil.which("gcc")


def extract_function(source: str, name: str) -> str:
    match = re.search(
        rf"(?m)^[ \t]*(?:static[ \t]+)?(?:void|int16_t)[ \t]+{re.escape(name)}\s*\(",
        source,
    )
    if match is None:
        raise AssertionError(f"Missing sprite.c definition {name}")
    brace = source.find("{", match.end())
    if brace < 0:
        raise AssertionError(f"Missing body for sprite.c function {name}")
    depth = 1
    pos = brace + 1
    state = "code"
    while pos < len(source) and depth:
        char = source[pos]
        next_char = source[pos + 1] if pos + 1 < len(source) else ""
        if state == "code":
            if char == "/" and next_char == "*":
                state = "block"
                pos += 1
            elif char == "/" and next_char == "/":
                state = "line"
                pos += 1
            elif char == '"':
                state = "string"
            elif char == "'":
                state = "char"
            elif char == "{":
                depth += 1
            elif char == "}":
                depth -= 1
        elif state == "block" and char == "*" and next_char == "/":
            state = "code"
            pos += 1
        elif state == "line" and char == "\n":
            state = "code"
        elif state in {"string", "char"}:
            if char == "\\":
                pos += 1
            elif ((state == "string" and char == '"') or
                  (state == "char" and char == "'")):
                state = "code"
        pos += 1
    if depth:
        raise AssertionError(f"Unterminated sprite.c function {name}")
    return source[match.start():pos]


def locked_image() -> bytes:
    _packed, unpacked, _report, _transforms = verify(write=False)
    return MZ.parse(unpacked).load_image(unpacked)


def make_shape(width: int, height: int, pixels: bytes, *, unknown1: int = 0,
               unknown2: int = 0, pos_x: int = 0, pos_y: int = 0,
               attrs: bytes = b"\0\0\0\0") -> bytes:
    if len(pixels) != width * height or len(attrs) != 4:
        raise ValueError("shape dimensions/pixel bytes or four attributes disagree")
    return struct.pack("<6H", width, height, unknown1 & 0xFFFF,
                       unknown2 & 0xFFFF, pos_x & 0xFFFF, pos_y & 0xFFFF) + attrs + pixels


def patterned_bytes(generator: random.Random, length: int) -> bytes:
    return bytes(generator.getrandbits(8) for _ in range(length))


def make_case(generator: random.Random, name: str, operation: str, *,
              width: int, height: int, x: int, y: int,
              clip: tuple[int, int, int, int], unknown1: int = 0,
              unknown2: int = 0, pixels: bytes | None = None,
              destination: bytes | None = None) -> BlitCase:
    if pixels is None:
        pixels = patterned_bytes(generator, width * height)
    if destination is None:
        destination = patterned_bytes(generator, VIDEO_BYTES)
    return BlitCase(
        name, operation,
        make_shape(width, height, pixels, unknown1=unknown1,
                   unknown2=unknown2, pos_x=unknown1, pos_y=unknown2),
        x, y, clip, destination,
    )


def test_cases() -> list[BlitCase]:
    generator = random.Random(0xB1175)
    cases: list[BlitCase] = []
    # Exercise clipping at each edge, odd/even spans, and empty clip axes.
    edge_specs = (
        (0, 0, (0, 320, 0, 200)),
        (-3, -2, (0, 320, 0, 200)),
        (319, 199, (0, 320, 0, 200)),
        (320, 200, (0, 320, 0, 200)),
        (18, 29, (20, 76, 31, 88)),
        (18, 29, (0, 0, 0, 200)),
    )
    for operation in ENTRY_POINTS:
        for index, (x, y, clip) in enumerate(edge_specs):
            cases.append(make_case(
                generator, f"{operation}-edge-{index}", operation,
                width=5 if index & 1 else 4, height=3 if index & 1 else 4,
                x=x, y=y, clip=clip, unknown1=1, unknown2=2,
            ))

        for index in range(18):
            width = generator.randint(1, 41)
            height = generator.randint(1, 31)
            left = generator.randint(0, 260)
            right = generator.randint(left + 1, SCREEN_WIDTH)
            top = generator.randint(0, 160)
            bottom = generator.randint(top + 1, SCREEN_HEIGHT)
            x = generator.randint(-36, SCREEN_WIDTH + 25)
            y = generator.randint(-28, SCREEN_HEIGHT + 20)
            cases.append(make_case(
                generator, f"{operation}-random-{index}", operation,
                width=width, height=height, x=x, y=y,
                clip=(left, right, top, bottom),
                unknown1=generator.randint(0, 36),
                unknown2=generator.randint(0, 29),
            ))

    # SUB AX,[SI+4] / SUB AX,[SI+6] wrap before the core's signed clip tests.
    # These values turn otherwise offscreen args into narrow visible fragments.
    boundary_specs = (
        (32767, 32766, 0x8001, 0x8002, 8, 6),
        (-32768, -32768, 32700, 32700, 9, 7),
        (32766, -32768, 0x7FFF, 0x8001, 5, 8),
    )
    for operation in ("and_alt2", "or_alt"):
        for index, (x, y, off_x, off_y, width, height) in enumerate(boundary_specs):
            cases.append(make_case(
                generator, f"{operation}-word-wrap-{index}", operation,
                width=width, height=height, x=x, y=y,
                clip=(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT),
                unknown1=off_x, unknown2=off_y,
            ))
    return cases


def compiled_harness(functions: str) -> str:
    prefix = r'''#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

typedef struct PortShape2D {
    uint16_t width, height, unknown1, unknown2, pos_x, pos_y;
    uint8_t attributes[4];
} PortShape2D;
typedef struct PortSprite {
    PortShape2D *sprite_bitmapptr;
    uint16_t *lineofs;
    uint16_t words2[9];
} PortSprite;
static PortSprite s_sprite1;
static PortShape2D s_dummy_shape;
static uint8_t s_destination[65536];
static uint8_t s_shape[65536 + 16];
static size_t s_shape_extent;
static uint16_t s_lines[256];

static uint16_t get_u16(const uint8_t *bytes)
{
    return (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
}
static uint32_t get_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}
static uint8_t *sprite_pixels(const PortSprite *sprite, size_t *extent_out)
{
    if (sprite != &s_sprite1 || sprite->sprite_bitmapptr == NULL)
        return NULL;
    if (extent_out != NULL) *extent_out = sizeof(s_destination);
    return s_destination;
}
static uint8_t *shape_pixels(const PortShape2D *shape, size_t *extent_out)
{
    if (shape == NULL || s_shape_extent < 16u) return NULL;
    if (extent_out != NULL) *extent_out = s_shape_extent - 16u;
    return (uint8_t *)shape + 16u;
}
static void port_guest_unwind(const char *message)
{
    fprintf(stderr, "sprite oracle probe bounds failure: %s\n", message);
    exit(3);
}
'''
    suffix = r'''
static int read_exact(void *destination, size_t bytes)
{
    return fread(destination, 1, bytes, stdin) == bytes;
}
int main(void)
{
    uint8_t count_bytes[4];
    unsigned case_index;
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    if (!read_exact(count_bytes, sizeof(count_bytes))) return 10;
    for (case_index = 0; case_index < get_u32(count_bytes); ++case_index) {
        uint8_t operation;
        uint8_t header[16];
        uint8_t metadata[16];
        uint16_t width, height, active_height, pitch;
        unsigned row;
        PortShape2D *shape = (PortShape2D *)s_shape;
        if (!read_exact(&operation, 1) || !read_exact(header, sizeof(header))) return 11;
        width = get_u16(header);
        height = get_u16(header + 2);
        if (width == 0 || height == 0 || (size_t)width * height > 65520u) return 12;
        memcpy(s_shape, header, sizeof(header));
        s_shape_extent = 16u + (size_t)width * height;
        if (!read_exact(s_shape + 16, s_shape_extent - 16u) ||
            !read_exact(metadata, sizeof(metadata)) ||
            !read_exact(s_destination, sizeof(s_destination))) return 13;
        pitch = get_u16(metadata + 12);
        active_height = get_u16(metadata + 14);
        if (pitch == 0 || active_height == 0 || active_height > 256u ||
            (uint32_t)pitch * active_height > 65536u) return 14;
        s_sprite1.sprite_bitmapptr = &s_dummy_shape;
        s_sprite1.lineofs = s_lines;
        s_sprite1.words2[0] = get_u16(metadata + 4);
        s_sprite1.words2[1] = get_u16(metadata + 6);
        s_sprite1.words2[2] = get_u16(metadata + 8);
        s_sprite1.words2[3] = get_u16(metadata + 10);
        s_sprite1.words2[4] = pitch;
        for (row = 0; row < active_height; ++row)
            s_lines[row] = (uint16_t)(row * pitch);
        switch (operation) {
        case 0: sprite_putimage_and(shape, (int16_t)get_u16(metadata),
                                    (int16_t)get_u16(metadata + 2)); break;
        case 1: sprite_putimage_and_alt(shape, (int16_t)get_u16(metadata),
                                        (int16_t)get_u16(metadata + 2)); break;
        case 2: sprite_putimage_and_alt2(shape, (int16_t)get_u16(metadata),
                                         (int16_t)get_u16(metadata + 2)); break;
        case 3: sprite_putimage_or_alt(shape, (int16_t)get_u16(metadata),
                                       (int16_t)get_u16(metadata + 2)); break;
        default: return 15;
        }
        if (fwrite(s_destination, 1, sizeof(s_destination), stdout) !=
            sizeof(s_destination)) return 16;
    }
    return 0;
}
'''
    return prefix + functions + suffix


def host_input(cases: list[BlitCase]) -> bytes:
    payload = bytearray(struct.pack("<I", len(cases)))
    for case in cases:
        if len(case.destination) != VIDEO_BYTES:
            raise ValueError(f"{case.name}: expected a full 64 KiB initial destination")
        operation_id = ENTRY_POINTS[case.operation][0]
        width, height = struct.unpack_from("<HH", case.shape)
        if len(case.shape) != 16 + width * height:
            raise ValueError(f"{case.name}: malformed shape bytes")
        left, right, top, bottom = case.clip
        payload.extend(bytes((operation_id,)))
        payload.extend(case.shape[:16])
        payload.extend(case.shape[16:])
        payload.extend(struct.pack("<hhHHHHHH", case.x, case.y,
                                   left, right, top, bottom,
                                   case.pitch, case.surface_height))
        payload.extend(case.destination)
    return bytes(payload)


def frozen_draw(image: bytes, machine: Uc, returned: dict[str, bool],
                case: BlitCase) -> bytes:
    _operation, image_offset = ENTRY_POINTS[case.operation]
    entry_ip = image_offset - CODE_BASE
    source_physical = (SOURCE_SEGMENT << 4) + SOURCE_OFFSET
    destination_physical = DESTINATION_SEGMENT << 4
    stack_physical = (STACK_SEGMENT << 4) + STACK_POINTER
    width, height = struct.unpack_from("<HH", case.shape)

    machine.mem_write(source_physical, case.shape)
    machine.mem_write(destination_physical, case.destination)
    # The locked loop addresses the shared line-offset table in its code/data
    # segment (the original scratch probe verified this address convention).
    machine.mem_write(CODE_BASE + LINE_TABLE_OFFSET,
                      b"".join(struct.pack("<H", row * case.pitch)
                               for row in range(case.surface_height)))

    left, right, top, bottom = case.clip
    for offset, value in (
        (0x5F22, DESTINATION_SEGMENT),  # ES segment for the active sprite
        (0x5F2A, LINE_TABLE_OFFSET),
        (0x5F30, top), (0x5F32, bottom),
        (0x5F2C, left), (0x5F2E, right),
        (0x5F34, case.pitch),
    ):
        machine.mem_write(CODE_BASE + offset, struct.pack("<H", value & 0xFFFF))

    stack = bytearray()
    for value in (RETURN_IP, CODE_SEGMENT, SOURCE_OFFSET, SOURCE_SEGMENT,
                  case.x, case.y):
        stack.extend(struct.pack("<H", value & 0xFFFF))
    machine.mem_write(stack_physical, bytes(stack))
    machine.reg_write(UC_X86_REG_CS, CODE_SEGMENT)
    machine.reg_write(UC_X86_REG_IP, entry_ip)
    machine.reg_write(UC_X86_REG_SS, STACK_SEGMENT)
    machine.reg_write(UC_X86_REG_SP, STACK_POINTER)
    machine.reg_write(UC_X86_REG_BP, 0xE000)
    machine.reg_write(UC_X86_REG_DS, 0)
    machine.reg_write(UC_X86_REG_ES, 0)
    machine.reg_write(UC_X86_REG_EFLAGS, 0x0202)
    returned["done"] = False
    machine.emu_start(entry_ip, 0, count=1000000)
    if not returned["done"]:
        raise AssertionError(f"locked {case.operation} did not return: {case.name}")
    return bytes(machine.mem_read(destination_physical, VIDEO_BYTES))


def stock_desert_sce3() -> bytes:
    source = ASSETS / "DESERT.PVS"
    if not source.is_file():
        raise unittest.SkipTest("DESERT.PVS is not provisioned; random sprite cases still run")
    compressed = source.read_bytes()
    decoded, _details = format_reference.decompress(compressed)
    unflipped = format_reference.unflip_pvs(decoded)
    archive, _shapes = format_reference.parse_shape_archive(unflipped, "PVS")
    entry = next(row for row in archive["entries"] if row["name"] == "sce3")
    shape = unflipped[entry["start"]:entry["end"]]
    width, height = struct.unpack_from("<HH", shape)
    if len(shape) != 16 + width * height:
        raise AssertionError("DESERT.PVS sce3 extent does not match its stock bitmap")
    return shape


class Sdl3SpriteBooleanSemanticsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Uc is None:
            raise unittest.SkipTest("Unicorn is required for the locked sprite oracle")
        cls.compiler = host_compiler()
        if cls.compiler is None:
            raise unittest.SkipTest("A host C compiler is required for the sprite comparison")
        cls.image = locked_image()
        source = (ROOT / "port" / "sprite.c").read_text(encoding="utf-8")
        actual_functions = "\n\n".join(extract_function(source, name) for name in (
            "draw_opaque", "compose_boolean", "sprite_putimage_and",
            "sprite_putimage_and_alt", "sprite_header_relative",
            "sprite_putimage_and_alt2",
            "sprite_putimage_or_alt",
        ))
        cls.temporary = tempfile.TemporaryDirectory(prefix="sdl3-sprite-boolean-")
        cls.executable = Path(cls.temporary.name) / "sprite_boolean_harness.exe"
        harness_path = Path(cls.temporary.name) / "sprite_boolean_harness.c"
        harness_path.write_text(compiled_harness(actual_functions), encoding="ascii")
        environment = os.environ.copy()
        environment["PATH"] = (str(Path(cls.compiler).resolve().parent) + os.pathsep
                               + environment.get("PATH", ""))
        result = subprocess.run(
            [cls.compiler, "-std=c11", "-O0", "-Wall", "-Wextra", "-Werror",
             str(harness_path), "-o", str(cls.executable)],
            cwd=ROOT, capture_output=True, text=True, check=False,
            timeout=30, env=environment,
        )
        if result.returncode:
            cls.temporary.cleanup()
            raise AssertionError("actual sprite.c harness compile failed:\n" +
                                 result.stdout + result.stderr)

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "temporary"):
            cls.temporary.cleanup()

    def test_locked_byte_anchors_distinguish_and_copy_and_header_adjust(self):
        anchors = {
            # and_alt2: SUB AX by SHAPE2D x/y then jump to AND core.
            0x2387E: bytes.fromhex("2b 44 04"),
            0x23887: bytes.fromhex("2b 44 06"),
            # and: the shared byte loop is LODSB; AND ES:[DI],AL.
            0x239A2: bytes.fromhex("ac 26 20 05"),
            # and_alt: wrapper jumps into sprite_putimage's copy core.
            0x23BBC: bytes.fromhex(
                "55 8b ec 83 ec 0e 1e 56 57 8e 5e 08 8b 76 06 "
                "8b 46 0a 89 46 fe 8b 46 0c 89 46 fc eb 1c"),
            0x23CCE: bytes.fromhex("f3 a4"),  # REP MOVSB, no boolean op
            # or_alt uses the same header-offset SUB instruction form.
            0x24072: bytes.fromhex("2b 44 04"),
            0x2407B: bytes.fromhex("2b 44 06"),
            0x24197: bytes.fromhex("26 08 05"),  # OR ES:[DI],AL
        }
        for offset, expected in anchors.items():
            with self.subTest(offset=hex(offset)):
                self.assertEqual(self.image[offset:offset + len(expected)], expected)

    def _compare_cases(self, cases: list[BlitCase]) -> None:
        machine = Uc(UC_ARCH_X86, UC_MODE_16)
        machine.mem_map(0, 0x100000)
        machine.mem_write(0, self.image)
        returned = {"done": False}

        def stop_after_far_return(uc, address, _size, _user):
            if address == CODE_BASE + RETURN_IP and uc.reg_read(UC_X86_REG_CS) == CODE_SEGMENT:
                returned["done"] = True
                uc.emu_stop()

        machine.hook_add(UC_HOOK_CODE, stop_after_far_return)
        expected = [frozen_draw(self.image, machine, returned, case) for case in cases]

        result = subprocess.run(
            [str(self.executable)], cwd=ROOT, input=host_input(cases),
            capture_output=True, check=False, timeout=60,
        )
        self.assertEqual(result.returncode, 0, result.stderr.decode(errors="replace"))
        self.assertEqual(len(result.stdout), len(cases) * VIDEO_BYTES)
        for index, (case, oracle_pixels) in enumerate(zip(cases, expected)):
            native_pixels = result.stdout[index * VIDEO_BYTES:(index + 1) * VIDEO_BYTES]
            if native_pixels != oracle_pixels:
                differences = [
                    (offset, oracle_pixels[offset], native_pixels[offset])
                    for offset in range(VIDEO_BYTES)
                    if oracle_pixels[offset] != native_pixels[offset]
                ][:8]
                self.fail(
                    f"{case.operation} diverged from locked code for {case.name}; "
                    f"first (offset, oracle, compiled sprite.c) differences: {differences}"
                )

    def test_all_entries_match_locked_machine_on_random_clips_and_word_boundaries(self):
        cases = test_cases()
        self._compare_cases(cases)

    def test_stock_desert_sce3_sky_panel_matches_opaque_machine_copy(self):
        sky_shape = stock_desert_sce3()
        generator = random.Random(0x5CE3)
        case = make_case(
            generator, "DESERT.PVS:sce3 sky panel at x=0,y=56", "and_alt",
            width=320, height=10, x=0, y=56,
            clip=(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT),
            pixels=sky_shape[16:], destination=patterned_bytes(generator, VIDEO_BYTES),
        )
        # Keep the decoded stock header (including its nonzero offset words).
        case = BlitCase(case.name, case.operation, sky_shape, case.x, case.y,
                        case.clip, case.destination)
        self._compare_cases([case])


if __name__ == "__main__":
    unittest.main()
