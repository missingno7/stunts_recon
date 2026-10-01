"""Runnable renderer/math examples using the checked-in PORT_BUILD routines."""
from __future__ import annotations

from dataclasses import replace
import json
import struct

from .model import CallResult, MemoryRegion, MemoryWrite, MismatchError, RoutineCase
from .oracle import ROOT, SymbolMap


FRAME = MemoryRegion("vram", 0xA000, 0, 0x10000)
LOAD_SEGMENT = 0x1000
SPRITE_SOURCE_SEGMENT = 0x6000
SPRITE_SOURCE_OFFSET = 0x0100
LINE_TABLE_OFFSET = 0xF000
SOURCE_LINE_TABLE_OFFSET = 0xF200


def _frame() -> bytes:
    return bytes((i * 17 + (i >> 8) * 3 + 11) & 0xFF for i in range(FRAME.size))


def _source(width: int, height: int) -> bytes:
    return bytes((i * 29 + (i // width) * 11 + 3) & 0xFF
                 for i in range(width * height))


def _sprite_machine_state(routine_name: str = "sprite_1_unk3", *,
                         source_width: int = 0,
                         source_height: int = 0) -> list[MemoryWrite]:
    symbols = SymbolMap()
    routine = symbols.resolve(routine_name)
    code_segment, _ = routine.far_at(LOAD_SEGMENT)
    data_map = json.loads((ROOT / "layout/data-symbols.json").read_text(
        encoding="utf-8"))["symbols"]["_sprite1"]
    descriptor_offset = data_map["load_address"] - routine.code_base
    words = [0] * 15
    words[0], words[1] = 0, 0xA000
    words[5] = LINE_TABLE_OFFSET
    words[6], words[7], words[8], words[9], words[10] = 0, 320, 0, 200, 320
    words[12], words[14] = 320, 320
    descriptor = struct.pack("<15H", *words)
    line_table = struct.pack("<200H", *(row * 320 for row in range(200)))
    writes = [MemoryWrite(code_segment, descriptor_offset, descriptor),
              MemoryWrite(code_segment, LINE_TABLE_OFFSET, line_table)]
    if source_width and source_height:
        source_map = json.loads((ROOT / "layout/data-symbols.json").read_text(
            encoding="utf-8"))["symbols"]["_sprite2"]
        source_descriptor_offset = source_map["load_address"] - routine.code_base
        source_words = [0] * 15
        source_words[0], source_words[1] = SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT
        source_words[5] = SOURCE_LINE_TABLE_OFFSET
        source_words[7] = source_width
        source_words[9] = source_height
        source_words[10] = source_width
        source_words[12], source_words[14] = source_width, source_width
        source_descriptor = struct.pack("<15H", *source_words)
        source_lines = struct.pack(
            f"<{source_height}H",
            *(SPRITE_SOURCE_OFFSET + 16 + row * source_width
              for row in range(source_height)))
        writes += [MemoryWrite(code_segment, source_descriptor_offset,
                               source_descriptor),
                   MemoryWrite(code_segment, SOURCE_LINE_TABLE_OFFSET,
                               source_lines)]
    return writes


def _shape(width: int, height: int, x: int, y: int, pixels: bytes) -> bytes:
    header = struct.pack("<6H4s", width, height, 0, 0, x, y, bytes(4))
    return header + pixels


def mulscl_case() -> RoutineCase:
    left, right = -12345, 4567
    return RoutineCase(
        "mulscl signed fixed-point multiply", "mulscl",
        args=[left & 0xFFFF, right & 0xFFFF], call="far",
        result_registers=("ax",),
        port_call=lambda lib, _case: CallResult(
            {"ax": lib.call_mulscl(left, right)}, 0, {}, 0, "cdecl"))


def draw_filled_rect_case() -> RoutineCase:
    args = (18, 42, 37, 14, 0x5A)
    initial = _frame()
    writes = _sprite_machine_state()
    writes.append(MemoryWrite(0xA000, 0, initial))
    def port_call(lib, _case):
        return lib.run_draw_filled_rect(initial, args)
    return RoutineCase("draw_filled_rect clipped XOR fill", "draw_filled_rect",
                       args=[value & 0xFFFF for value in args], call="far",
                       memory=writes, compare=(FRAME,), result_registers=(),
                       port_call=port_call)


def sprite_1_unk3_case() -> RoutineCase:
    width, height, x, y, phase = 39, 48, 61, 47, 3
    pixels = _source(width, height)
    initial = _frame()
    source = _shape(width, height, x, y, pixels)
    writes = _sprite_machine_state()
    writes += [MemoryWrite(0xA000, 0, initial),
               MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET, source)]
    def port_call(lib, _case):
        return lib.run_sprite_1_unk3(initial, pixels, width, height, x, y, phase)
    return RoutineCase("sprite_1_unk3 phase-interlaced copy", "sprite_1_unk3",
                       args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT, phase],
                       call="far", memory=writes, compare=(FRAME,),
                       result_registers=(), port_call=port_call)


def sprite_1_unk_case() -> RoutineCase:
    args = (45, 27, 39, 7, 0x6D)
    initial = _frame()
    writes = _sprite_machine_state("sprite_1_unk")
    writes.append(MemoryWrite(0xA000, 0, initial))
    def port_call(lib, _case):
        return lib.run_sprite_1_unk(initial, args)
    return RoutineCase("sprite_1_unk odd-width solid fill", "sprite_1_unk",
                       args=list(args), call="far", memory=writes,
                       compare=(FRAME,), result_registers=(),
                       port_call=port_call)


def icon_combine_case(use_and: bool, width: int = 7) -> RoutineCase:
    height, x, y = 5, 67, 43
    pixels = _source(width, height)
    initial = _frame()
    routine = "putpixel_iconMask" if use_and else "putpixel_iconFillings"
    writes = _sprite_machine_state(routine)
    writes += [MemoryWrite(0xA000, 0, initial),
               MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET,
                           _shape(width, height, x, y, pixels))]
    def port_call(lib, _case):
        return lib.run_icon_combine(initial, pixels, width, height, x, y,
                                    use_and)
    label = "AND mask" if use_and else "OR fill"
    width_label = "single-column" if width == 1 else "odd-width"
    return RoutineCase(f"{routine} {width_label} icon {label}", routine,
                       args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT, x, y],
                       call="far", memory=writes, compare=(FRAME,),
                       result_registers=(), port_call=port_call)


def _rle_source() -> bytes:
    # The 45 output bytes include a run crossing a scanline boundary, plus
    # both repeated-color and literal packets followed by the zero sentinel.
    literal_a = bytes((0x21 + index * 13) & 0xFF for index in range(4))
    literal_b = bytes((0xA3 + index * 7) & 0xFF for index in range(17))
    return (bytes((3, 0x55, 0xFC)) + literal_a + bytes((10, 0xA6, 0xEF)) +
            literal_b + bytes((11, 0x0F, 0)))


def shape2d_runs_case(use_and: bool) -> RoutineCase:
    width, height, x, y = 9, 5, 72, 52
    encoded = _rle_source()
    initial = _frame()
    routine = "shape2d_render_bmp_as_mask" if use_and else "shape2d_op_unk4"
    writes = _sprite_machine_state(routine)
    writes += [MemoryWrite(0xA000, 0, initial),
               MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET,
                           _shape(width, height, x, y, encoded))]
    def port_call(lib, _case):
        return lib.run_shape2d_runs(initial, encoded, width, height, x, y,
                                    use_and)
    operation = "AND mask" if use_and else "OR mask"
    return RoutineCase(f"{routine} mixed RLE {operation}", routine,
                       args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT],
                       call="far", memory=writes, compare=(FRAME,),
                       result_registers=(), port_call=port_call)


def clear_rect_case() -> RoutineCase:
    source_width, source_height = 64, 48
    # The linear displacement crosses a 320-pixel destination row here:
    # x+315 = 323, so the copy lands at x=3 on source row + 1.
    args = (8, 12, 19, 6, 315)
    source_pixels = _source(source_width, source_height)
    initial = _frame()
    writes = _sprite_machine_state("clear_rect", source_width=source_width,
                                   source_height=source_height)
    writes += [MemoryWrite(0xA000, 0, initial),
               MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET,
                           _shape(source_width, source_height, 0, 0,
                                  source_pixels))]
    def port_call(lib, _case):
        return lib.run_clear_rect(initial, source_pixels, source_width,
                                  source_height, args)
    return RoutineCase("clear_rect backbuffer copy with destination offset",
                       "clear_rect", args=list(args), call="far", memory=writes,
                       compare=(FRAME,), result_registers=(),
                       port_call=port_call)


def negative_mismatch(harness, case: RoutineCase | None = None) -> MismatchError:
    case = case or draw_filled_rect_case()
    real_port_call = case.port_call
    def faulty_port_call(lib, active_case):
        result = real_port_call(lib, active_case)
        damaged = bytearray(result.memory["vram"])
        damaged[0] ^= 0x80
        result.memory["vram"] = bytes(damaged)
        return result
    faulty = replace(case, name=case.name + " (injected port defect)",
                     port_call=faulty_port_call)
    try:
        harness.run(faulty)
    except MismatchError as error:
        return error
    raise AssertionError("the negative example should have detected a changed byte")
