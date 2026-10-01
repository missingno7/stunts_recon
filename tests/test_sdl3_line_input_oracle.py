"""Differentially prove the translated line-input helpers against the lock."""
from __future__ import annotations

import json
import os
from pathlib import Path
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
    str(ROOT / "tests"),
]

try:
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import (
        OracleImage, RoutineAddress, SymbolMap, dgroup_segment,
    )
    from test_sdl3_font_oracle import (
        VIDEO_BYTES, compiler_environment, font_default_from_oracle,
        frame_seed, host_compiler, load_runtime_fonts, read_source_default_font,
    )
except ImportError as error:  # pragma: no cover - pinned toolchain dependencies
    RealModeRunner = MemoryRegion = MemoryWrite = RoutineCase = None
    OracleImage = RoutineAddress = SymbolMap = dgroup_segment = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None


CODE_LOAD_SEGMENT = 0x1000
VIDEO_SEGMENT = 0xA000
FONTDEF_SEG_OFFSET = 0x4E90
LINE_RECT_POINTER_OFFSET = 0x4E8E
TEXT_OFFSET = 0xD000
TEXT_CAPACITY = 512
SPRITE_LINE_TABLE_OFFSET = 0x0600
FONT_SEGMENTS = {0: None, 1: 0x5000, 2: 0x5100, 3: 0x5200}
FONT_NAMES = ("builtin-1408", "FONTDEF.FNT", "FONTN.FNT", "FONTLED.FNT")
ACTION_CURSOR = 0
ACTION_CLEAR_TAIL = 1
ACTION_BLINK_TWICE = 2


def fresh_transformed_line_input(compiler: Path) -> Path:
    """Regenerate the PORT_BUILD overlay from canonical C before compiling."""
    script = (
        "import importlib.util, pathlib, sys; "
        "root = pathlib.Path.cwd(); sys.path.insert(0, str(root / 'port')); "
        "spec = importlib.util.spec_from_file_location('line_oracle_port_build', "
        "root / 'port' / 'build.py'); "
        "module = importlib.util.module_from_spec(spec); "
        "sys.modules[spec.name] = module; spec.loader.exec_module(module); "
        "compiler = pathlib.Path(sys.argv[1]); "
        "module.prepare_environment(compiler); module.build_game_objects(compiler)"
    )
    refreshed = subprocess.run(
        [sys.executable, "-c", script, str(compiler)], cwd=ROOT,
        capture_output=True, text=True, timeout=180, check=False,
    )
    if refreshed.returncode:
        raise AssertionError(
            "fresh PORT_BUILD transformation failed:\n" +
            refreshed.stdout + refreshed.stderr
        )
    overlay = ROOT / "build" / "sdl3" / "host-build" / "overlay" / "src" / "obj_seg032_group.c"
    if not overlay.is_file():
        raise AssertionError("fresh host build did not produce obj_seg032_group.c")
    generated = overlay.read_text(encoding="latin-1")
    if "void FAR read_line_helper(void)" not in generated or \
            "void FAR read_line_helper2(void)" not in generated:
        raise AssertionError("fresh transformed source omits a line-input helper")
    if "line_input_screen_rect->width" not in generated or \
            "line_input_screen_rect->height" not in generated:
        raise AssertionError("transformed helpers no longer read the live header view")
    return overlay


def compile_host_probe(overlay: Path) -> tuple[Path, dict[str, str]]:
    compiler = host_compiler()
    worker = ROOT / "build" / "workers" / "line_cursor_oracle" / f"run_{os.getpid()}"
    worker.mkdir(parents=True, exist_ok=True)
    executable = worker / "line_input_oracle_harness.exe"
    translated = worker / "obj_seg032_group_oracle.c"
    helper_object = worker / "obj_seg032_group_oracle.o"
    driver_object = worker / "line_input_oracle_harness.o"
    translated.write_text(
        overlay.read_text(encoding="latin-1") + r'''

/* Probe-only bridge into the translated TU's private editor state. */
void port_line_input_set_state(short x, short y, short cursor_height,
                               short max_width, short cursor_slot, I8 *text)
{
    line_input_text_buffer = text;
    line_edit_x = x;
    line_edit_y = y;
    line_input_cursor_vertical = cursor_height;
    line_text_max_width = max_width;
    line_input_cursor_slot = cursor_slot;
    cursor_flash_state = 1;
}

U16 port_line_input_get_slot(void)
{
    return (U16)line_input_cursor_slot;
}
''',
        encoding="latin-1",
    )
    common = [
        str(compiler), "-std=gnu11", "-O2", "-ffunction-sections",
        "-fdata-sections", "-DPORT_BUILD=1", "-Wno-error=implicit-int",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
    ]
    helper_command = [
        *common,
        "-include", str(ROOT / "build" / "sdl3" / "host-build" / "config" /
                        "obj_seg032_group.h"),
        "-I", str(ROOT / "build" / "sdl3" / "host-build" / "include"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(ROOT / "include"),
        "-I", str(ROOT / "src"),
        "-c", str(translated), "-o", str(helper_object),
    ]
    driver_command = [
        *common,
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(ROOT / "include"),
        "-I", str(ROOT / "src"),
        "-c", str(ROOT / "tests" / "sdl3" / "line_input_oracle_harness.c"),
        "-o", str(driver_object),
    ]
    link_command = [
        str(compiler), str(driver_object), str(helper_object),
        "-Wl,--gc-sections", "-o", str(executable),
    ]
    environment = compiler_environment(compiler)
    for label, command in (("translated line-input TU", helper_command),
                           ("line-input driver", driver_command),
                           ("line-input probe link", link_command)):
        build = subprocess.run(
            command, cwd=ROOT, env=environment, capture_output=True, text=True,
            timeout=90, check=False,
        )
        if build.returncode:
            raise AssertionError(
                f"{label} failed:\n" + build.stdout + build.stderr
            )
    return executable, environment


def pack_host_input(fonts: dict[str, bytes], steps: list[dict[str, int | bytes]]) -> bytes:
    payload = bytearray(b"LIO1")
    for name in FONT_NAMES[1:]:
        font = fonts[name]
        if len(font) > 4096:
            raise ValueError(f"{name} exceeds the host probe's font capacity")
        payload.extend(struct.pack("<H", len(font)))
        payload.extend(font)
    payload.extend(struct.pack("<H", len(steps)))
    for step in steps:
        text = step["text"]
        assert isinstance(text, bytes)
        if not text or text[-1] != 0 or len(text) > TEXT_CAPACITY:
            raise ValueError("line-input fixture text must be bounded and NUL-terminated")
        payload.extend(struct.pack(
            "<12H",
            int(step["font_id"]), int(step["select"]), int(step["action"]),
            int(step["foreground"]), int(step["background"]),
            int(step.get("line_height", 0xFFFF)), int(step["x"]), int(step["y"]),
            int(step["cursor_height"]), int(step["max_width"]),
            int(step["cursor_slot"]), len(text),
        ))
        payload.extend(text)
    return bytes(payload)


def host_call(host: Path, environment: dict[str, str],
              fonts: dict[str, bytes], steps: list[dict[str, int | bytes]]) -> list[bytes]:
    result = subprocess.run(
        [str(host)], input=pack_host_input(fonts, steps), env=environment,
        capture_output=True, timeout=20, check=False,
    )
    if result.returncode:
        raise AssertionError(
            f"line-input host probe failed ({result.returncode}): "
            f"{result.stderr.decode(errors='replace')}"
        )
    outputs: list[bytes] = []
    cursor = 0
    for step in steps:
        font_length = len(fonts[FONT_NAMES[int(step["font_id"]) ]])
        amount = VIDEO_BYTES + font_length + TEXT_CAPACITY + 2
        outputs.append(result.stdout[cursor:cursor + amount])
        cursor += amount
    if cursor != len(result.stdout):
        raise AssertionError(
            f"line-input probe returned {len(result.stdout)} bytes; parsed {cursor}"
        )
    return outputs


def data_offset(names: dict[str, dict], name: str, dgroup_base: int) -> int:
    rows = [(int(address), row) for address, row in names.items()
            if row.get("name") == name]
    if len(rows) != 1:
        raise AssertionError(f"expected one registry binding for {name}, found {len(rows)}")
    offset = rows[0][0] - dgroup_base
    return offset


class LineInputMachine:
    def __init__(self, oracle, runner, symbols, data_symbols, names, fonts):
        self.oracle = oracle
        self.runner = runner
        self.symbols = symbols
        self.dgroup = dgroup_segment(CODE_LOAD_SEGMENT)
        self.dgroup_base = data_symbols["frame_load_address"]
        self.rect_pointer_offset = (
            data_symbols["symbols"]["_line_input_screen_rect"]["load_address"] -
            self.dgroup_base
        )
        self.fontdefseg_offset = (
            data_symbols["symbols"]["_fontdefseg"]["load_address"] - self.dgroup_base
        )
        self.sprite1_offset = (
            data_symbols["symbols"]["_sprite1"]["load_address"] -
            symbols.resolve("font_draw_text").code_base
        )
        self.static_offsets = {
            name: data_offset(names, name, self.dgroup_base)
            for name in (
                "line_input_cursor_vertical", "line_edit_x", "line_edit_y",
                "cursor_flash_state", "line_input_text_buffer",
                "line_text_max_width", "line_input_cursor_slot",
            )
        }
        if self.rect_pointer_offset != LINE_RECT_POINTER_OFFSET or \
                self.fontdefseg_offset != FONTDEF_SEG_OFFSET or \
                self.sprite1_offset != 0x5F20:
            raise AssertionError("locked line-input, font pointer, or sprite offsets changed")
        if self.static_offsets != {
            "line_input_cursor_vertical": 0x72A6,
            "line_edit_x": 0x72A8,
            "line_edit_y": 0x72AA,
            "cursor_flash_state": 0x72AC,
            "line_input_text_buffer": 0x72AE,
            "line_text_max_width": 0x72B0,
            "line_input_cursor_slot": 0x72B2,
        }:
            raise AssertionError(f"line-editor BSS anchors changed: {self.static_offsets}")
        relocated = oracle.relocated(CODE_LOAD_SEGMENT)
        initial_offset, initial_segment = struct.unpack_from(
            "<HH", relocated, self.rect_pointer_offset + self.dgroup_base
        )
        self.builtin_segment = initial_segment
        if initial_offset != 0:
            raise AssertionError("locked initial selected-font far pointer offset is nonzero")
        if self.builtin_segment != CODE_LOAD_SEGMENT + 0x2B1F:
            raise AssertionError("locked startup font pointer no longer selects FONTDEF builtin")

        self.sprite_routine = symbols.resolve("font_draw_text")
        self.sprite_segment, _ = self.sprite_routine.far_at(CODE_LOAD_SEGMENT)
        descriptor = [0] * 15
        descriptor[1] = VIDEO_SEGMENT
        descriptor[5] = SPRITE_LINE_TABLE_OFFSET
        descriptor[6], descriptor[7], descriptor[8], descriptor[9] = (0, 320, 0, 200)
        descriptor[10] = 320  # frozen fill/sprite helpers load the active row pitch here
        self.sprite_descriptor = struct.pack("<15H", *descriptor)
        self.row_table = b"".join(struct.pack("<H", row * 320) for row in range(200))
        self.font_state = {index: bytearray(fonts[name])
                           for index, name in enumerate(FONT_NAMES)}
        self.active_font_id = 0
        self.active_segment = self.builtin_segment
        self.frame = frame_seed()
        self.text = bytes(TEXT_CAPACITY)
        self.cursor_slot = 0

    def _font_segment(self, font_id: int) -> int:
        return self.builtin_segment if font_id == 0 else FONT_SEGMENTS[font_id]

    def _select(self, font_id: int) -> None:
        segment = self._font_segment(font_id)
        record = bytes(self.font_state[font_id])
        case = RoutineCase(
            f"select line font {FONT_NAMES[font_id]}", "font_set_fontdef2",
            args=[0, segment], call="far",
            memory=[MemoryWrite(segment, 0, record)],
            compare=[MemoryRegion("font_pointer", self.dgroup,
                                  self.rect_pointer_offset, 4)],
        )
        result = self.runner.call(case)
        pointer = result.memory["font_pointer"]
        if struct.unpack_from("<HH", pointer) != (0, segment):
            raise AssertionError(
                f"frozen font selection stored unexpected far pointer {pointer.hex()}"
            )
        self.active_font_id = font_id
        self.active_segment = segment

    def _setup_colors(self, foreground: int, background: int) -> None:
        font_id = self.active_font_id
        segment = self.active_segment
        record = bytes(self.font_state[font_id])
        case = RoutineCase(
            f"line font colors {foreground:#04x}/{background:#04x}",
            "font_set_unk", args=[foreground, background], call="far",
            memory=[MemoryWrite(segment, 0, record),
                    MemoryWrite(self.dgroup, self.fontdefseg_offset,
                                struct.pack("<H", segment))],
            compare=[MemoryRegion("font", segment, 0, len(record))],
        )
        self.font_state[font_id] = bytearray(self.runner.call(case).memory["font"])

    def _helper_call(self, step: dict[str, int | bytes]) -> None:
        font_id = self.active_font_id
        segment = self.active_segment
        record = bytes(self.font_state[font_id])
        text_input = step["text"]
        assert isinstance(text_input, bytes)
        text_buffer = text_input[:TEXT_CAPACITY].ljust(TEXT_CAPACITY, b"\0")
        values = {
            "line_input_cursor_vertical": int(step["cursor_height"]),
            "line_edit_x": int(step["x"]),
            "line_edit_y": int(step["y"]),
            "cursor_flash_state": 1,
            "line_input_text_buffer": TEXT_OFFSET,
            "line_text_max_width": int(step["max_width"]),
            "line_input_cursor_slot": int(step["cursor_slot"]),
        }
        writes = [
            MemoryWrite(segment, 0, record),
            MemoryWrite(self.dgroup, self.rect_pointer_offset,
                        struct.pack("<HH", 0, segment)),
            MemoryWrite(self.dgroup, TEXT_OFFSET, text_buffer),
            MemoryWrite(self.dgroup, 0x72A6,
                        struct.pack("<7H", *(
                            values[name] & 0xFFFF for name in (
                                "line_input_cursor_vertical", "line_edit_x",
                                "line_edit_y", "cursor_flash_state",
                                "line_input_text_buffer", "line_text_max_width",
                                "line_input_cursor_slot",
                            )
                        ))),
            MemoryWrite(VIDEO_SEGMENT, 0, self.frame),
            MemoryWrite(self.sprite_segment, self.sprite1_offset,
                        self.sprite_descriptor),
            MemoryWrite(self.sprite_segment, SPRITE_LINE_TABLE_OFFSET,
                        self.row_table),
        ]
        routine = "read_line_helper2" if int(step["action"]) == ACTION_CLEAR_TAIL \
            else "read_line_helper"
        helper_address = self.symbols.resolve(routine)
        code_base = helper_address.segment_paragraph * 16 if hasattr(
            helper_address, "segment_paragraph"
        ) else 0x2A4B0
        resolved = RoutineAddress(routine, helper_address.start, helper_address.end,
                                  code_base, helper_address.stable_id)
        original_resolve = self.runner.symbols.resolve
        try:
            self.runner.symbols.resolve = lambda name: resolved if name == routine else original_resolve(name)
            case = RoutineCase(
                f"{routine} {FONT_NAMES[font_id]} {step['text']!r}", routine,
                args=[], call="far", memory=writes,
                compare=[MemoryRegion("video", VIDEO_SEGMENT, 0, VIDEO_BYTES),
                         MemoryRegion("font", segment, 0, len(record)),
                         MemoryRegion("text", self.dgroup, TEXT_OFFSET,
                                      TEXT_CAPACITY),
                         MemoryRegion("cursor_slot", self.dgroup,
                                      self.static_offsets["line_input_cursor_slot"], 2)],
            )
            result = self.runner.call(case)
        finally:
            self.runner.symbols.resolve = original_resolve
        self.frame = result.memory["video"]
        self.font_state[font_id] = bytearray(result.memory["font"])
        self.text = result.memory["text"]
        self.cursor_slot = struct.unpack("<H", result.memory["cursor_slot"])[0]

    def run_step(self, step: dict[str, int | bytes]) -> bytes:
        font_id = int(step["font_id"])
        if int(step["select"]):
            self._select(font_id)
        elif font_id != self.active_font_id:
            raise AssertionError("fixture changed fonts without a selected-font transition")
        if int(step.get("line_height", 0xFFFF)) != 0xFFFF:
            struct.pack_into("<H", self.font_state[font_id], 18,
                             int(step["line_height"]))
        self._setup_colors(int(step["foreground"]), int(step["background"]))
        if int(step["action"]) == ACTION_BLINK_TWICE:
            original_frame = self.frame
            self._helper_call(step)
            self._helper_call(step)
            if self.frame != original_frame:
                raise AssertionError("two XOR cursor flashes did not restore the frame")
        else:
            self._helper_call(step)
        active_font = bytes(self.font_state[font_id])
        return self.frame + active_font + self.text + struct.pack("<H", self.cursor_slot)


def machine_helpers(runner, symbols) -> None:
    """Add the verified segment-032 labels absent from the review map."""
    functions = json.loads((ROOT / "evidence" / "functions.json").read_text(encoding="utf-8"))["functions"]
    expected = {"read_line_helper": (174070, 174230),
                "read_line_helper2": (174230, 174424)}
    rows = {row["name"]: row for row in functions if row.get("name") in expected}
    if set(rows) != set(expected):
        raise AssertionError("locked evidence does not name both line-input helper extents")
    for name, (start, end) in expected.items():
        row = rows[name]
        if (row["start"], row["end"], row["segment_paragraph"],
                row["segment_offset"]) != (start, end, 0x2A4B, start - 0x2A4B0):
            raise AssertionError(f"frozen {name} address differs from the reviewed extent")
        address = RoutineAddress(name, start, end, 0x2A4B0, row.get("stable_id"))
        original_resolve = symbols.resolve
        # Bind this exact address before any test cases run.
        symbols.resolve = lambda query, n=name, a=address, old=original_resolve: \
            a if query == n else old(query)


@unittest.skipIf(ORACLE_IMPORT_ERROR is not None,
                 f"pinned 16-bit oracle dependencies unavailable: {ORACLE_IMPORT_ERROR}")
class Sdl3LineInputOracleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.oracle = OracleImage.load()
        cls.symbols = SymbolMap()
        cls.runner = RealModeRunner(cls.oracle, instruction_budget=500_000)
        machine_helpers(cls.runner, cls.symbols)
        machine_helpers(cls.runner, cls.runner.symbols)
        cls.default_font = font_default_from_oracle(cls.oracle)
        if read_source_default_font() != cls.default_font:
            raise AssertionError("built-in C font differs from the locked machine image")
        cls.fonts = load_runtime_fonts(cls.default_font)
        cls.data_symbols = json.loads(
            (ROOT / "layout" / "data-symbols.json").read_text(encoding="utf-8")
        )
        cls.names = json.loads(
            (ROOT / "layout" / "names-registry.json").read_text(encoding="utf-8")
        )["names"]
        compiler = host_compiler()
        cls.overlay = fresh_transformed_line_input(compiler)
        cls.host, cls.environment = compile_host_probe(cls.overlay)

    def compare_steps(self, label: str, steps: list[dict[str, int | bytes]]) -> list[bytes]:
        host_outputs = host_call(self.host, self.environment, self.fonts, steps)
        machine = LineInputMachine(
            self.oracle, self.runner, self.symbols, self.data_symbols,
            self.names, self.fonts,
        )
        for index, (step, actual) in enumerate(zip(steps, host_outputs)):
            expected = machine.run_step(step)
            self.assert_complete_match(
                f"{label} step {index}", expected, actual,
                len(self.fonts[FONT_NAMES[int(step["font_id"])]]),
            )
        return host_outputs

    def assert_complete_match(self, label: str, expected: bytes,
                              actual: bytes, font_length: int) -> None:
        if expected == actual:
            return
        frame_end = VIDEO_BYTES
        font_end = frame_end + font_length
        if expected[:frame_end] != actual[:frame_end]:
            offset = next(i for i, (left, right) in enumerate(
                zip(expected[:frame_end], actual[:frame_end])) if left != right)
            self.fail(
                f"{label}: video[{offset:#06x}] expected {expected[offset]:02x}, "
                f"got {actual[offset]:02x} at pixel ({offset % 320},{offset // 320})"
            )
        if expected[frame_end:font_end] != actual[frame_end:font_end]:
            offset = next(i for i, (left, right) in enumerate(
                zip(expected[frame_end:font_end], actual[frame_end:font_end]))
                          if left != right)
            self.fail(
                f"{label}: selected font[{offset:#06x}] expected "
                f"{expected[frame_end + offset]:02x}, got {actual[frame_end + offset]:02x}"
            )
        if expected[font_end:] != actual[font_end:]:
            offset = next(i for i, (left, right) in enumerate(
                zip(expected[font_end:], actual[font_end:])) if left != right)
            self.fail(
                f"{label}: editor state[{offset:#06x}] expected "
                f"{expected[font_end + offset]:02x}, got {actual[font_end + offset]:02x}"
            )

    def test_cursor_font_switching_and_helper2_match_full_locked_state(self):
        steps: list[dict[str, int | bytes]] = [
            # No set_fontdefseg call: the image-start pointer selects the builtin.
            dict(font_id=0, select=0, action=ACTION_CURSOR, foreground=15,
                 background=0, line_height=0xFFFF, x=16, y=170,
                 cursor_height=8, max_width=0, cursor_slot=0, text=b"1\0"),
            # A non-white foreground proves the rectangle reads font[0] dynamically.
            dict(font_id=0, select=0, action=ACTION_CURSOR, foreground=3,
                 background=7, line_height=13, x=34, y=125,
                 cursor_height=13, max_width=0, cursor_slot=1, text=b"12\0"),
            # FONTDEF tail clearing reads its live background and nondefault height.
            dict(font_id=1, select=1, action=ACTION_CLEAR_TAIL, foreground=0x2E,
                 background=7, line_height=11, x=20, y=70,
                 cursor_height=11, max_width=95, cursor_slot=2, text=b"12\0"),
            dict(font_id=2, select=1, action=ACTION_CURSOR, foreground=5,
                 background=12, line_height=9, x=50, y=90,
                 cursor_height=9, max_width=0, cursor_slot=0, text=b"1\0"),
            dict(font_id=3, select=1, action=ACTION_BLINK_TWICE, foreground=9,
                 background=2, line_height=7, x=74, y=110,
                 cursor_height=7, max_width=0, cursor_slot=0, text=b"1\0"),
            # Re-select builtin after three external records and change colors again.
            dict(font_id=0, select=1, action=ACTION_CURSOR, foreground=10,
                 background=6, line_height=12, x=102, y=135,
                 cursor_height=12, max_width=0, cursor_slot=0, text=b"1\0"),
            # Re-select the same external record and mutate its colors at runtime.
            dict(font_id=1, select=1, action=ACTION_CURSOR, foreground=4,
                 background=11, line_height=0xFFFF, x=128, y=80,
                 cursor_height=8, max_width=0, cursor_slot=1, text=b"12\0"),
        ]
        outputs = self.compare_steps("selected font cursor oracle", steps)
        first = outputs[0][:VIDEO_BYTES]
        second = outputs[1][:VIDEO_BYTES]
        seed = frame_seed()
        self.assertNotEqual(first, frame_seed(), "white builtin cursor drew no pixels")
        self.assertEqual(first[170 * 320 + 16], seed[170 * 320 + 16] ^ 15,
                         "the white-on-black cursor must XOR the selected foreground 15")
        self.assertEqual(second[125 * 320 + 42], seed[125 * 320 + 42] ^ 3,
                         "runtime foreground changes must reach the cursor fill operation")
        cleared = outputs[2][:VIDEO_BYTES]
        for row in range(70, 81):
            self.assertEqual(cleared[row * 320 + 114], 7,
                             "helper2 must clear its configured tail to the selected background")
        self.assertEqual(cleared[81 * 320 + 114], seed[81 * 320 + 114],
                         "helper2's clear height must follow the nondefault 11-pixel line height")
        blink_step = outputs[4][:VIDEO_BYTES]
        self.assertEqual(blink_step, outputs[3][:VIDEO_BYTES],
                         "two toggles restore the frame after the preceding font step")

    def test_initial_font_alias_and_dialog_color_match_locked_data(self):
        dlg_address = self.data_symbols["symbols"]["_dlg_colour"]["load_address"]
        self.assertEqual(struct.unpack_from("<H", self.oracle.load_image, dlg_address)[0], 15,
                         "locked dlg_colour data is white palette index 15")
        pointer_address = self.data_symbols["symbols"]["_line_input_screen_rect"]["load_address"]
        pointer_offset, pointer_segment = struct.unpack_from(
            "<HH", self.oracle.load_image, pointer_address
        )
        self.assertEqual((pointer_offset, pointer_segment), (0, 0x2B1F),
                         "locked initial line-header far pointer aliases the builtin font at 0:2B1F")
        builtin = self.oracle.load_image[0x2B1F0:0x2B1F0 + 1408]
        self.assertEqual(builtin[:4], bytes((3, 0, 0, 0)),
                         "startup builtin foreground/background header differs from the locked bytes")
        self.assertEqual(struct.unpack_from("<H", builtin, 18)[0], 8,
                         "startup builtin line-height word differs from the locked bytes")


if __name__ == "__main__":
    unittest.main()
