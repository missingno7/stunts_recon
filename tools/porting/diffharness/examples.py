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


def _frame() -> bytes:
    return bytes((i * 17 + (i >> 8) * 3 + 11) & 0xFF for i in range(FRAME.size))


def _source(width: int, height: int) -> bytes:
    return bytes((i * 29 + (i // width) * 11 + 3) & 0xFF
                 for i in range(width * height))


def _sprite_machine_state() -> list[MemoryWrite]:
    symbols = SymbolMap()
    routine = symbols.resolve("sprite_1_unk3")
    code_segment, _ = routine.far_at(LOAD_SEGMENT)
    data_map = json.loads((ROOT / "layout/data-symbols.json").read_text(
        encoding="utf-8"))["symbols"]["_sprite1"]
    descriptor_offset = data_map["load_address"] - routine.code_base
    words = [0] * 15
    words[0], words[1] = 0, 0xA000
    words[5] = LINE_TABLE_OFFSET
    words[6], words[7], words[8], words[9], words[10] = 0, 320, 0, 200, 320
    descriptor = struct.pack("<15H", *words)
    line_table = struct.pack("<200H", *(row * 320 for row in range(200)))
    return [MemoryWrite(code_segment, descriptor_offset, descriptor),
            MemoryWrite(code_segment, LINE_TABLE_OFFSET, line_table)]


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
