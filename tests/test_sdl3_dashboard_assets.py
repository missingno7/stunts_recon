"""Differential regression for the locked STDA mode-3 dashboard resources."""
from __future__ import annotations

import base64
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
ASSETS = Path(os.environ.get("STUNTS_ASSETS", ROOT / "assets"))
sys.path.insert(0, str(ROOT / "build" / "python"))
sys.path.insert(0, str(ROOT / "tools" / "porting"))
sys.path.insert(0, str(ROOT))

try:
    import format_reference
    from unicorn import Uc, UC_ARCH_X86, UC_HOOK_CODE, UC_HOOK_INTR, UC_MODE_16
    from unicorn.x86_const import (
        UC_X86_REG_AX, UC_X86_REG_BP, UC_X86_REG_CS, UC_X86_REG_DS,
        UC_X86_REG_EFLAGS, UC_X86_REG_ES, UC_X86_REG_IP, UC_X86_REG_SP,
        UC_X86_REG_SS,
    )
    from tools.porting.diffharness.emulator import RealModeRunner
    from tools.porting.diffharness.examples import (
        FRAME, LOAD_SEGMENT, LINE_TABLE_OFFSET, SPRITE_SOURCE_OFFSET,
        SPRITE_SOURCE_SEGMENT, _frame, _sprite_machine_state,
    )
    from tools.porting.diffharness.model import MemoryRegion, MemoryWrite, RoutineCase
    from tools.porting.diffharness.oracle import (
        OracleImage, SymbolMap, dgroup_segment,
    )
except ImportError as error:  # Unicorn is a pinned optional local oracle dependency.
    format_reference = None
    Uc = None
    UC_ARCH_X86 = UC_HOOK_CODE = UC_HOOK_INTR = UC_MODE_16 = None
    UC_X86_REG_AX = UC_X86_REG_BP = UC_X86_REG_CS = UC_X86_REG_DS = None
    UC_X86_REG_EFLAGS = UC_X86_REG_ES = UC_X86_REG_IP = None
    UC_X86_REG_SP = UC_X86_REG_SS = None
    RealModeRunner = None
    FRAME = None
    LOAD_SEGMENT = None
    LINE_TABLE_OFFSET = None
    SPRITE_SOURCE_OFFSET = None
    SPRITE_SOURCE_SEGMENT = None
    _frame = None
    _sprite_machine_state = None
    MemoryRegion = MemoryWrite = RoutineCase = None
    OracleImage = SymbolMap = dgroup_segment = None
    ORACLE_IMPORT_ERROR = error
else:
    ORACLE_IMPORT_ERROR = None


# These are the eleven original car archives provisioned with the locked game.
# Their bytes remain external, read-only assets; the hashes pin this regression
# to that exact source set without checking game resources into the repository.
STDA_SHA256 = {
    "STDAANSX.PVS": "9a0e8e91ad6ffed3b6f7e225dd6b4e7b31eaca6bf0aa35c4c64009d5c9cb35a8",
    "STDAAUDI.PVS": "d75610884ef4f0aaff9e53362ed9ba4d913c8f699a9f8299e71954f3b3d41377",
    "STDACOUN.PVS": "1bebc0b9831acad8c6b5f2f8372993d4eacd759e9d4358f6235f97d27851e48b",
    "STDAFGTO.PVS": "cf208925803939e99f75456cbbf511d21b2b15cec397d6b535c9c997948c0126",
    "STDAJAGU.PVS": "609a82e0a96fb703d8f671b011a0a779fb7fdc6c94a97a39360d20d21d8287a8",
    "STDALANC.PVS": "47b9768caaa4cc88c67ff79e847dd0bc2c3fccd21c61d34a1aecb71b1b585061",
    "STDALM02.PVS": "2b0c1a141bbfc2d237a7ad6f555fd10888275df3063d69430b7dd91d7658798e",
    "STDAP962.PVS": "a17178accd8c46bdfbba9b81908f554db4821fa59afa6f2a0c329b22ce331d35",
    "STDAPC04.PVS": "06b5f575323e608e03093dda4c0e7560b3c26f09e40198f0e9c488d7f05fbc36",
    "STDAPMIN.PVS": "2ae22dd37858a33dab37e38a232a3de20a6d77f9a6826a575a9196d904c1522a",
    "STDAVETT.PVS": "3c4c2408ae481bf21332cce8716ca0af14d5bf31761a5902595490b26cb989a1",
}

MAINCLIP_BOTTOM_REPLAY = 151
MAINCLIP_BOTTOM_FULL = 200
MACHINE_SOURCE_SEGMENT = 0xB000
MACHINE_DEST_SEGMENT = 0x8000
MACHINE_DEST_CAPACITY = 0x20000
MACHINE_SENTINEL_SEGMENT = 0x7000


def gcc_path() -> Path:
    configured = os.environ.get("DIFFHARNESS_GCC")
    candidates = [Path(configured)] if configured else []
    candidates.append(Path(r"C:\msys64\mingw32\bin\gcc.exe"))
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    found = shutil.which("i686-w64-mingw32-gcc")
    if found:
        return Path(found).resolve()
    raise unittest.SkipTest("the SDL3 dashboard regression requires i686 MinGW GCC")


def python32_path() -> Path:
    supplied = os.environ.get("DIFFHARNESS_PYTHON32")
    candidates = [Path(supplied)] if supplied else []
    candidates.append(Path(r"C:\msys64\mingw32\bin\python.exe"))
    for candidate in candidates:
        if not candidate.is_file():
            continue
        result = subprocess.run(
            [str(candidate), "-c", "import struct; print(struct.calcsize('P')*8)"],
            capture_output=True, text=True, check=False)
        if result.returncode == 0 and result.stdout.strip() == "32":
            return candidate.resolve()
    raise unittest.SkipTest("the i686 SDL3 dashboard regression needs 32-bit Python")


def _linear(segment: int, offset: int) -> int:
    return ((segment << 4) + offset) & 0xFFFFF


def locked_machine_unflip(oracle: OracleImage, compressed: bytes,
                          label: str) -> bytes:
    decoded, _ = format_reference.decompress(compressed)
    _, shapes = format_reference.parse_shape_archive(decoded, "PVS")
    scratch_size = max((shape["width"] * shape["height"] for shape in shapes),
                       default=1) + 64
    case = RoutineCase(
        label, "file_unflip_shape2d",
        args=[0, 0x6000, 0x0100, 0x9000], call="far",
        memory=[
            MemoryWrite(0x6000, 0, decoded),
            MemoryWrite(0x9000, 0x0100, bytes(scratch_size)),
        ],
        compare=(MemoryRegion("unflipped", 0x6000, 0, len(decoded)),),
        result_registers=(),
    )
    result = RealModeRunner(oracle, instruction_budget=5_000_000).call(case)
    actual = result.memory["unflipped"]
    expected = format_reference.unflip_pvs(decoded)
    if actual != expected:
        at = next(index for index, (left, right) in
                  enumerate(zip(expected, actual)) if left != right)
        raise AssertionError(
            f"{label}: locked DOS unflip differs at {at:#x}: "
            f"DOS={actual[at]:02x}, reference={expected[at]:02x}")
    return actual


def locked_machine_parse(oracle: OracleImage, archive: bytes,
                         label: str, *,
                         helper_trace: list[dict[str, object]] | None = None,
                         trace_segment: int | None = None,
                         trace_offsets: tuple[int, int] | None = None) -> bytes:
    """Execute locked parse_shape2d; stub only its final allocator resize.

    The parser writes every archive and RLE byte before asking the unrelated
    DOS memory manager to shrink its allocation. The diff harness starts at a
    single routine and has no DOS allocator table, so the hook supplies only
    that final RETF and captures its paragraph count.
    """
    parser = SymbolMap().resolve("parse_shape2d")
    resize = SymbolMap().resolve("mmgr_resize_memory")
    code_segment, entry_ip = parser.far_at(LOAD_SEGMENT)
    helper_ip = parser.end - parser.code_base
    helper_address = _linear(code_segment, helper_ip)
    resize_segment, resize_ip = resize.far_at(LOAD_SEGMENT)
    data_segment = dgroup_segment(LOAD_SEGMENT)
    machine = Uc(UC_ARCH_X86, UC_MODE_16)
    machine.mem_map(0, 0x100000)
    machine.mem_write(LOAD_SEGMENT << 4, oracle.relocated(LOAD_SEGMENT))
    machine.mem_write(_linear(MACHINE_SOURCE_SEGMENT, 0), archive)
    machine.mem_write(_linear(MACHINE_DEST_SEGMENT, 0),
                      bytes(MACHINE_DEST_CAPACITY))

    # The original far helper at parse_shape2d+0x2cc dereferences a BP-relative
    # long through DS (its caller's stack is in DGROUP), so model the DOS
    # compiler's SS==DS stack instead of a convenient independent stack segment.
    stack_segment, stack_pointer = data_segment, 0xF000
    stack_words = [
        0, MACHINE_SENTINEL_SEGMENT,
        0, MACHINE_SOURCE_SEGMENT,
        0, MACHINE_DEST_SEGMENT,
    ]
    machine.mem_write(
        _linear(stack_segment, stack_pointer),
        b"".join(struct.pack("<H", word) for word in stack_words))
    for register, value in (
        (UC_X86_REG_CS, code_segment), (UC_X86_REG_IP, entry_ip),
        (UC_X86_REG_DS, data_segment), (UC_X86_REG_ES, 0xA000),
        (UC_X86_REG_SS, stack_segment), (UC_X86_REG_SP, stack_pointer),
        (UC_X86_REG_BP, 0), (UC_X86_REG_AX, 0), (UC_X86_REG_EFLAGS, 0x0202),
    ):
        machine.reg_write(register, value)

    state: dict[str, int | str | None] = {
        "resize_count": 0, "output_size": 0, "trap": None, "returned": 0,
    }
    resize_address = _linear(resize_segment, resize_ip)
    sentinel_address = _linear(MACHINE_SENTINEL_SEGMENT, 0)
    pending_helper: dict[str, object] | None = None

    def on_code(uc, address, _size, _user):
        nonlocal pending_helper
        if (pending_helper is not None and
                address == int(pending_helper["return_address"])):
            pending_helper["return_ax"] = uc.reg_read(UC_X86_REG_AX) & 0xFFFF
            helper_trace.append(pending_helper)
            pending_helper = None
        if address == sentinel_address:
            state["returned"] = 1
            uc.emu_stop()
            return
        if address == helper_address and helper_trace is not None:
            sp = uc.reg_read(UC_X86_REG_SP) & 0xFFFF
            ss = uc.reg_read(UC_X86_REG_SS) & 0xFFFF
            return_ip, return_cs, source_offset, source_segment = struct.unpack(
                "<4H", bytes(uc.mem_read(_linear(ss, sp), 8)))
            selected = (trace_segment is None or source_segment == trace_segment)
            if selected and trace_offsets is not None:
                source_linear = _linear(source_segment, source_offset)
                selected = trace_offsets[0] <= source_linear < trace_offsets[1]
            if selected:
                bp = uc.reg_read(UC_X86_REG_BP) & 0xFFFF
                stack_base = _linear(ss, bp)
                locals_data = bytes(uc.mem_read(stack_base - 0x38, 0x38))
                dest_offset = struct.unpack_from("<H", locals_data, 0x30)[0]
                dest_segment = struct.unpack_from("<H", locals_data, 0x32)[0]
                page_remaining = struct.unpack_from("<H", locals_data, 0x1A)[0]
                literal_count = struct.unpack_from("<H", locals_data, 0x2A)[0]
                src_window = bytes(uc.mem_read(
                    _linear(source_segment, source_offset), 24))
                dst_address = _linear(dest_segment, dest_offset)
                dst_before = bytes(uc.mem_read(max(0, dst_address - 8), 24))
                pending_helper = {
                    "source_segment": source_segment,
                    "source_offset": source_offset,
                    "source_window": src_window.hex(),
                    "dest_segment": dest_segment,
                    "dest_offset": dest_offset,
                    "dest_before": dst_before.hex(),
                    "page_remaining": page_remaining,
                    "literal_count": literal_count,
                    "return_address": _linear(return_cs, return_ip),
                }
        if address != resize_address:
            return
        sp = uc.reg_read(UC_X86_REG_SP) & 0xFFFF
        ss = uc.reg_read(UC_X86_REG_SS) & 0xFFFF
        frame = bytes(uc.mem_read(_linear(ss, sp), 10))
        return_ip, return_cs, _buffer_offset, _buffer_segment, paragraphs = \
            struct.unpack_from("<5H", frame)
        state["resize_count"] = int(state["resize_count"]) + 1
        state["output_size"] = paragraphs * 16
        uc.reg_write(UC_X86_REG_SP, (sp + 4) & 0xFFFF)
        uc.reg_write(UC_X86_REG_CS, return_cs)
        uc.reg_write(UC_X86_REG_IP, return_ip)

    def on_interrupt(uc, interrupt, _user):
        cs = uc.reg_read(UC_X86_REG_CS) & 0xFFFF
        ip = uc.reg_read(UC_X86_REG_IP) & 0xFFFF
        state["trap"] = f"INT {interrupt:#x} at {cs:04X}:{ip:04X}"
        uc.emu_stop()

    machine.hook_add(UC_HOOK_CODE, on_code)
    machine.hook_add(UC_HOOK_INTR, on_interrupt)
    try:
        machine.emu_start(_linear(code_segment, entry_ip), 0,
                          count=30_000_000)
    except Exception as error:
        raise AssertionError(f"{label}: locked DOS parser failed: {error}") from error
    if state["trap"] is not None:
        raise AssertionError(f"{label}: locked DOS parser {state['trap']}")
    if state["returned"] != 1 or state["resize_count"] != 1:
        cs = machine.reg_read(UC_X86_REG_CS) & 0xFFFF
        ip = machine.reg_read(UC_X86_REG_IP) & 0xFFFF
        raise AssertionError(
            f"{label}: parser did not finish once at resize boundary "
            f"(count={state['resize_count']}, PC={cs:04X}:{ip:04X})")
    output_size = int(state["output_size"] or 0)
    if output_size < 6 or output_size > MACHINE_DEST_CAPACITY:
        raise AssertionError(f"{label}: invalid locked parse output size {output_size}")
    return bytes(machine.mem_read(
        _linear(MACHINE_DEST_SEGMENT, 0), output_size))


def loaded_archive_shapes(archive: bytes) -> dict[str, bytes]:
    if len(archive) < 6:
        raise AssertionError("loaded STDA archive is shorter than its header")
    count = struct.unpack_from("<H", archive, 4)[0]
    payload = 6 + count * 8
    if count > 4096 or payload > len(archive):
        raise AssertionError(f"invalid loaded STDA directory: count={count}")
    offsets = [struct.unpack_from("<I", archive, 6 + count * 4 + 4 * index)[0]
               for index in range(count)]
    payload_extent = len(archive) - payload
    result: dict[str, bytes] = {}
    for index, relative in enumerate(offsets):
        if relative >= payload_extent:
            raise AssertionError(f"shape directory offset {relative:#x} is outside archive")
        next_offset = min((candidate for candidate in offsets if candidate > relative),
                          default=payload_extent)
        name = archive[6 + index * 4:10 + index * 4].decode("latin-1").strip()
        result[name] = archive[payload + relative:payload + next_offset]
    return result


def decode_mode3_shape(
        shape: bytes) -> tuple[bytes, list[tuple[int, int, bytes]], int]:
    """Decode a parser-produced stream and return its terminator offset."""
    pixels = bytearray()
    packets: list[tuple[int, int, bytes]] = []
    cursor = 16
    while cursor < len(shape):
        start = len(pixels)
        control = struct.unpack_from("b", shape, cursor)[0]
        cursor += 1
        if control == 0:
            return bytes(pixels), packets, cursor
        if control > 0:
            value = shape[cursor:cursor + 1]
            if len(value) != 1:
                raise AssertionError("short repeated-color mode3 packet")
            cursor += 1
            pixels.extend(value * control)
            packets.append((start, control, value))
        else:
            count = -control
            literal = shape[cursor:cursor + count]
            if len(literal) != count:
                raise AssertionError("short literal mode3 packet")
            cursor += count
            pixels.extend(literal)
            packets.append((start, control, literal))
    raise AssertionError("mode3 shape has no zero terminator")


def validate_loaded_archive_pixels(source: bytes, loaded: bytes,
                                   label: str) -> None:
    """Check logical image data and the allocator's paragraph-rounded extent.

    RLE packet boundaries can differ when the original FAR run scanner reaches
    the end of a shape. Validate each stream against the original unflipped
    pixels and account for the exact zero terminator before comparing packed
    archives byte-for-byte.
    """
    source_shapes = loaded_archive_shapes(source)
    loaded_shapes = loaded_archive_shapes(loaded)
    if source_shapes.keys() != loaded_shapes.keys():
        raise AssertionError(
            f"{label}: shape names differ: source={sorted(source_shapes)} "
            f"loaded={sorted(loaded_shapes)}")
    if len(loaded) < 6:
        raise AssertionError(f"{label}: loaded archive header is truncated")
    count = struct.unpack_from("<H", loaded, 4)[0]
    if count != len(loaded_shapes):
        raise AssertionError(
            f"{label}: directory count {count} differs from parsed shapes "
            f"{len(loaded_shapes)}")
    payload = 6 + count * 8
    offsets: dict[str, int] = {}
    for index in range(count):
        name = loaded[6 + index * 4:10 + index * 4].decode("latin-1").strip()
        offsets[name] = struct.unpack_from(
            "<I", loaded, 6 + count * 4 + index * 4)[0]

    logical_end = 0
    for name, raw_shape in source_shapes.items():
        if len(raw_shape) < 16:
            raise AssertionError(f"{label}:{name}: source shape header is short")
        width, height = struct.unpack_from("<2H", raw_shape)
        pixel_count = width * height
        if len(raw_shape) < 16 + pixel_count:
            raise AssertionError(f"{label}:{name}: source pixels are truncated")
        expected_pixels = raw_shape[16:16 + pixel_count]
        output_shape = loaded_shapes[name]
        if output_shape[:16] != raw_shape[:16]:
            raise AssertionError(f"{label}:{name}: decoded shape header changed")
        actual_pixels, _packets, terminator = decode_mode3_shape(output_shape)
        if len(actual_pixels) != pixel_count or actual_pixels != expected_pixels:
            raise AssertionError(
                f"{label}:{name}: mode-3 pixels differ from unflipped source "
                f"(expected {pixel_count}, decoded {len(actual_pixels)})")
        if any(output_shape[terminator:]):
            raise AssertionError(f"{label}:{name}: nonzero bytes follow RLE terminator")
        logical_end = max(logical_end, offsets[name] + terminator)

    expected_extent = (payload + logical_end + 15) & ~15
    if len(loaded) != expected_extent:
        raise AssertionError(
            f"{label}: paragraph-rounded logical extent is {expected_extent}, "
            f"loaded allocation is {len(loaded)}")
    if len(loaded) & 15:
        raise AssertionError(f"{label}: mode-3 allocation is not paragraph aligned")

def machine_shape_descriptor(shape: bytes) -> tuple[int, int, int, int, int, int]:
    if len(shape) < 16:
        raise AssertionError("loaded shape is shorter than the SHAPE2D header")
    return struct.unpack_from("<6H", shape)


def set_machine_clip(writes: list[MemoryWrite], routine_name: str,
                     clip: tuple[int, int, int, int]) -> list[MemoryWrite]:
    routine = SymbolMap().resolve(routine_name)
    code_segment, _ = routine.far_at(LOAD_SEGMENT)
    data_symbols = json.loads((ROOT / "layout" / "data-symbols.json").read_text(
        encoding="utf-8"))["symbols"]
    descriptor_offset = (data_symbols["_sprite1"]["load_address"] -
                         routine.code_base)
    for index, write in enumerate(writes):
        if write.segment != code_segment or write.offset != descriptor_offset:
            continue
        words = list(struct.unpack("<15H", write.data))
        words[6:10] = clip
        writes[index] = MemoryWrite(
            write.segment, write.offset, struct.pack("<15H", *words))
        return writes
    raise AssertionError(f"{routine_name}: sprite1 descriptor was not initialized")


def framebuffer_case(routine_name: str, shape: bytes, *,
                     clip: tuple[int, int, int, int], operation: int,
                     label: str) -> tuple[RoutineCase, dict[str, object]]:
    initial = _frame()
    writes = set_machine_clip(_sprite_machine_state(routine_name),
                              routine_name, clip)
    writes.extend((
        MemoryWrite(0xA000, 0, initial),
        MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET, shape),
    ))
    case = RoutineCase(
        label, routine_name, args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT],
        call="far", memory=writes, compare=(FRAME,), result_registers=(),
    )
    return case, {
        "label": label, "operation": operation,
        "clip": list(clip),
    }


def gear_window_case(shape: bytes, width: int, height: int,
                     x: int, y: int, label: str,
                     asset_index: int) -> tuple[RoutineCase, dict[str, object]]:
    routine = SymbolMap().resolve("shape2d_op_unk2")
    code_segment, _ = routine.far_at(LOAD_SEGMENT)
    data_symbols = json.loads((ROOT / "layout" / "data-symbols.json").read_text(
        encoding="utf-8"))["symbols"]
    descriptor_offset = data_symbols["_sprite1"]["load_address"] - routine.code_base
    words = [0] * 15
    words[0], words[1] = 0x0100, 0x5000
    words[5] = LINE_TABLE_OFFSET
    words[6], words[7], words[8], words[9], words[10] = (
        0, width, 0, height, width)
    words[12], words[14] = width, width
    descriptor = struct.pack("<15H", *words)
    line_table = struct.pack(
        f"<{height}H", *(0x0100 + 16 + row * width for row in range(height)))
    target = struct.pack("<6H4s", width, height, 0, 0, 0, 0, bytes(4))
    target += bytes([0x35]) * (width * height)
    case = RoutineCase(
        label, "shape2d_op_unk2",
        args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT, x, y], call="far",
        memory=[
            MemoryWrite(code_segment, descriptor_offset, descriptor),
            MemoryWrite(code_segment, LINE_TABLE_OFFSET, line_table),
            MemoryWrite(0x5000, 0x0100, target),
            MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET, shape),
        ],
        compare=(MemoryRegion("gear_window", 0x5000, 0x0110,
                              width * height),),
        result_registers=(),
    )
    return case, {
        "label": label, "operation": 2, "asset_index": asset_index,
        "shape_name": "dash", "x": x, "y": y,
        "target_width": width, "target_height": height,
        "output_size": width * height,
    }


def instrument_surface_case(shape: bytes, width: int, height: int,
                            label: str, asset_index: int
                            ) -> tuple[RoutineCase, dict[str, object]]:
    """Run the actual ins2/unknown5 decode into setup_car_shapes' wheel window."""
    routine = SymbolMap().resolve("shape2d_op_unk5")
    code_segment, _ = routine.far_at(LOAD_SEGMENT)
    data_symbols = json.loads((ROOT / "layout" / "data-symbols.json").read_text(
        encoding="utf-8"))["symbols"]
    descriptor_offset = data_symbols["_sprite1"]["load_address"] - routine.code_base
    target_offset, target_segment = 0x0100, 0x5000
    line_table_offset = LINE_TABLE_OFFSET
    words = [0] * 15
    words[0], words[1] = target_offset, target_segment
    words[5] = line_table_offset
    words[6], words[7], words[8], words[9], words[10] = (
        0, width, 0, height, width)
    words[12], words[14] = width, width
    descriptor = struct.pack("<15H", *words)
    lines = struct.pack(
        f"<{height}H",
        *(target_offset + 16 + row * width for row in range(height)))
    initial_window = struct.pack("<6H4s", width, height, 0, 0, 0, 0, bytes(4))
    initial_window += bytes(width * height)
    case = RoutineCase(
        label, "shape2d_op_unk5",
        args=[SPRITE_SOURCE_OFFSET, SPRITE_SOURCE_SEGMENT, 0, 0], call="far",
        memory=[
            MemoryWrite(code_segment, descriptor_offset, descriptor),
            MemoryWrite(code_segment, line_table_offset, lines),
            MemoryWrite(target_segment, target_offset, initial_window),
            MemoryWrite(SPRITE_SOURCE_SEGMENT, SPRITE_SOURCE_OFFSET, shape),
        ],
        compare=(MemoryRegion("instrument_window", target_segment,
                              target_offset + 16, width * height),),
        result_registers=(),
    )
    return case, {
        "label": label, "operation": 3, "asset_index": asset_index,
        "shape_name": "ins2", "x": 0, "y": 0,
        "target_width": width, "target_height": height,
        "output_size": width * height,
    }


def host_source_overlays(directory: Path, gcc: Path) -> list[tuple[Path, Path]]:
    """Generate only the three production C TUs in the mode-3 loader path."""
    port_root = ROOT / "tools" / "porting" / "port_include"
    host_root = ROOT / "tools" / "porting" / "host"
    generated = directory / "mode3-source"
    include_root = generated / "include"
    shutil.copytree(port_root, include_root, dirs_exist_ok=True)
    sys.path.insert(0, str(ROOT / "port"))
    import game_abi
    include_structs = include_root / "stunts_structs.h"
    include_structs.write_text(
        game_abi.adapt_aggregate_views(include_structs.read_text(encoding="latin-1")),
        encoding="latin-1")
    # The generated shared header lost the return type from this declaration.
    # Use its pinned evidence record in this isolated host-only overlay.
    declarations_header = include_root / "stunts_decls.h"
    declarations_text = declarations_header.read_text(encoding="latin-1")
    broken_declaration = "extern vector_op_unk2(struct VECTOR* vec);"
    if broken_declaration in declarations_text:
        declarations_text = declarations_text.replace(
            broken_declaration,
            "extern char vector_op_unk2(struct VECTOR* vec);")
        declarations_header.write_text(declarations_text, encoding="latin-1")

    import host_probe_declarations as declarations
    import host_probe_modes as probe

    probe.ROOT = ROOT
    probe.PORT = include_root
    probe.HOST = host_root
    probe.WORK = generated
    probe.GCC = gcc
    probe.STRICT_CENTRAL = False
    declarations.ROOT = ROOT
    declarations.ALIASES.update(I32="long", U32="unsigned long")
    overrides = {
        "file_load_shape2d_fatal_thunk":
            "extern void *file_load_shape2d_fatal_thunk(char *);",
        "mmgr_get_chunk_size": "extern uint16_t mmgr_get_chunk_size(void *);",
        "locate_shape_nofatal":
            "extern void *locate_shape_nofatal(void *, const char *);",
        "mmgr_op_unk": "extern void *mmgr_op_unk(void *);",
        "parse_shape2d_helper":
            "extern int32_t parse_shape2d_helper(uint8_t *);",
    }
    declarations.HOST_OVERRIDES.update(overrides)
    manifest = json.loads((ROOT / "layout" / "manifest.json").read_text(
        encoding="utf-8"))
    owners = declarations.collect_sources(manifest)
    shared_tags = set(json.loads(
        (include_root / "declaration-evidence.json").read_text(
            encoding="utf-8")).get("shared_struct_tags", []))
    selected = (
        "src/seg034_shape2d_group.c",
        "src/obj_seg035_group.c",
        "src/file_load_shape2d_expandedsize.c",
    )
    generated_pairs: list[tuple[Path, Path]] = []
    for index, source in enumerate(selected, 1):
        owner = owners.get(source)
        if owner is None:
            raise AssertionError(f"mode-3 loader source is absent from manifest: {source}")
        original = (ROOT / source).read_text(encoding="latin-1")
        config, local_fns, local_data = probe.local_config(source, original, owner)
        transformed, _ = probe.transformed_source(
            source, original, local_fns, local_data, shared_tags)
        if source == "src/obj_seg035_group.c":
            transformed = (
                "extern int16_t port_parse_shape2d_helper3(const uint8_t *, uint16_t);\n"
                + transformed)
            old = "repeat = parse_shape2d_helper3((I8 FAR *)src);"
            if transformed.count(old) != 1:
                raise AssertionError("could not locate mode-3 helper3 call")
            transformed = transformed.replace(old,
                "repeat = port_parse_shape2d_helper3((const uint8_t *)src, "
                "(uint16_t)(page_remaining - literal_cnt));",
                1)
        transformed = game_abi.adapt_aggregate_views(
            probe.legacy_target_widths(transformed))
        host_config = probe.legacy_target_widths(config.read_text(encoding="latin-1"))
        for name, prototype in declarations.HOST_OVERRIDES.items():
            host_config = __import__("re").sub(
                r"(?m)^extern [^\n;]*\b" + __import__("re").escape(name) +
                r"\([^\n]*;", lambda _match, value=prototype: value, host_config)
        config.write_text(host_config, encoding="latin-1")
        overlay = generated / "overlay" / source
        overlay.parent.mkdir(parents=True, exist_ok=True)
        overlay.write_text(transformed, encoding="latin-1")
        generated_pairs.append((overlay, config))
    return generated_pairs


def build_asset_dll(directory: Path, gcc: Path) -> Path:
    output = directory / "dashboard_asset_port.dll"
    sources = [
        ROOT / "tests" / "sdl3" / "dashboard_asset_adapter.c",
        ROOT / "tools" / "porting" / "diffharness" / "host_adapter.c",
        ROOT / "tools" / "porting" / "diffharness" / "host_shim.c",
        ROOT / "port" / "sprite.c",
        ROOT / "port" / "sprite_aux.c",
        ROOT / "port" / "memory.c",
        ROOT / "port" / "sincos.c",
        ROOT / "port" / "resource.c",
        ROOT / "src" / "file_get_unflip_size.c",
    ]
    includes = [
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(ROOT / "include"), "-I", str(ROOT / "src"),
    ]
    env = os.environ.copy()
    env["PATH"] = os.pathsep.join((
        str(gcc.parent), r"C:\tools\sdl3-3.4.16-i686\bin",
        env.get("PATH", ""),
    ))
    objects: list[Path] = []
    for index, source in enumerate(sources):
        obj = directory / f"dashboard_native_{index:02d}.o"
        command = [
            str(gcc), "-m32", "-std=gnu11", "-O2", "-DPORT_BUILD=1",
            "-ffunction-sections", "-fdata-sections", *includes,
            "-c", str(source), "-o", str(obj),
        ]
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True, errors="replace", check=False, env=env)
        if result.returncode:
            raise AssertionError(
                f"dashboard native object compile failed for {source}:\n" +
                result.stdout + result.stderr)
        objects.append(obj)

    overlays = host_source_overlays(directory, gcc)
    for index, (overlay, config) in enumerate(overlays):
        obj = directory / f"dashboard_game_{index:02d}.o"
        command = [
            str(gcc), "-m32", "-std=gnu11", "-O2", "-DPORT_BUILD=1",
            "-ffunction-sections", "-fdata-sections",
            "-I", str(config.parent.parent / "include"), *includes,
            "-include", str(config), "-c", str(overlay), "-o", str(obj),
        ]
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True, errors="replace", check=False, env=env)
        if result.returncode:
            raise AssertionError(
                f"dashboard mode-3 source compile failed for {overlay}:\n" +
                result.stdout + result.stderr)
        objects.append(obj)

    command = [
        str(gcc), "-m32", "-shared", "-static-libgcc", "-Wl,--gc-sections",
        "-Wl,--wrap=file_find",
        *[str(path) for path in objects], "-o", str(output),
    ]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                            errors="replace", check=False, env=env)
    if result.returncode:
        raise AssertionError("dashboard asset DLL link failed:\n" +
                             result.stdout + result.stderr)
    return output


def port_results(dll_path: Path, assets: list[dict[str, object]],
                 cases: list[dict[str, object]], initial: bytes,
                 python32: Path, gcc: Path
                 ) -> tuple[list[bytes], list[bytes]]:
    request = {
        "dll": str(dll_path),
        "initial": base64.b64encode(initial).decode("ascii"),
        "assets": [
            {**item, "compressed": base64.b64encode(item["compressed"]).decode("ascii")}
            for item in assets
        ],
        "cases": cases,
    }
    env = os.environ.copy()
    env["PATH"] = os.pathsep.join((
        str(gcc.parent), r"C:\tools\sdl3-3.4.16-i686\bin",
        env.get("PATH", ""),
    ))
    worker = ROOT / "tests" / "sdl3" / "dashboard_asset_worker.py"
    result = subprocess.run(
        [str(python32), str(worker)], input=json.dumps(request, separators=(",", ":")),
        capture_output=True, text=True, errors="replace", check=False,
        env=env, timeout=240,
    )
    if result.returncode:
        raise AssertionError(f"dashboard asset i686 worker exited {result.returncode}:\n" +
                             result.stdout[-1500:] + result.stderr[-4000:])
    try:
        reply = json.loads(result.stdout)
    except json.JSONDecodeError as error:
        raise AssertionError("dashboard asset worker returned malformed JSON: " +
                             result.stdout[:500]) from error
    if not reply.get("ok"):
        raise AssertionError("dashboard asset worker failed: " +
                             str(reply.get("error")))
    return ([base64.b64decode(item) for item in reply["archives"]],
            [base64.b64decode(item) for item in reply["results"]])


def loaded_shape(data: dict[str, bytes], name: str, label: str) -> bytes:
    try:
        return data[name]
    except KeyError as error:
        raise AssertionError(f"{label}: missing shape {name!r}") from error


@unittest.skipUnless(ASSETS.is_dir(), "original game assets are not provisioned")
class Sdl3DashboardAssetTests(unittest.TestCase):
    def test_frozen_mode3_dashboard_loader_and_mainclip_match_locked_dos(self) -> None:
        if ORACLE_IMPORT_ERROR is not None:
            self.skipTest(
                f"locked machine oracle dependency is unavailable: {ORACLE_IMPORT_ERROR}")
        paths = sorted(ASSETS.glob("STDA*.PVS"))
        self.assertEqual({path.name for path in paths}, set(STDA_SHA256),
                         "the regression expects exactly the frozen eleven STDA car archives")
        for path in paths:
            self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(),
                             STDA_SHA256[path.name],
                             f"{path.name} differs from the frozen game resource")

        gcc = gcc_path()
        python32 = python32_path()
        oracle = OracleImage.load(ASSETS)
        runner = RealModeRunner(oracle, instruction_budget=5_000_000)
        all_cases: list[tuple[RoutineCase, dict[str, object]]] = []
        dos_archives: list[bytes] = []
        dos_source_archives: list[bytes] = []
        asset_requests: list[dict[str, object]] = []

        for asset_index, path in enumerate(paths):
            compressed = path.read_bytes()
            unflipped = locked_machine_unflip(oracle, compressed,
                                              f"{path.name} DOS PVS unflip")
            dos_source_archives.append(unflipped)
            dos_archive = locked_machine_parse(oracle, unflipped,
                                               f"{path.name} DOS mode-3 parser")
            dos_archives.append(dos_archive)
            asset_requests.append({
                "label": path.name, "stem": path.stem,
                "compressed": compressed,
            })
            shapes = loaded_archive_shapes(dos_archive)
            dash = loaded_shape(shapes, "dash", path.name)
            dash_header = machine_shape_descriptor(dash)
            gbox = machine_shape_descriptor(loaded_shape(shapes, "gbox", path.name))
            mainclip_top = dash_header[5]

            # setup_car_shapes(1) renders dash and whl1 to the display through
            # the RLE/clipped routine. whl2/whl3 are the remaining live steering
            # variants and are kept in the per-asset matrix for fidelity.
            for name in ("dash", "whl1", "whl2", "whl3"):
                shape = loaded_shape(shapes, name, path.name)
                for clip_bottom, suffix in (
                    (MAINCLIP_BOTTOM_REPLAY, "replay151"),
                    (MAINCLIP_BOTTOM_FULL, "full200"),
                ):
                    clip = (0, 320, mainclip_top, clip_bottom)
                    label = f"{path.name}:{name} op3 mainclip-{suffix}"
                    case, port_case = framebuffer_case(
                        "shape2d_op_unk3", shape, clip=clip, operation=0,
                        label=label)
                    port_case.update(asset_index=asset_index, shape_name=name)
                    all_cases.append((case, port_case))

            # Roof is drawn through the literal-run path at its own y=0 origin.
            # Keep both clip bottoms to prove whether the replay bar hides any
            # row at the screen edge; the DOS routine itself may ignore bounds.
            if "roof" in shapes:
                roof = shapes["roof"]
                for clip_bottom, suffix in (
                    (MAINCLIP_BOTTOM_REPLAY, "replay151"),
                    (MAINCLIP_BOTTOM_FULL, "full200"),
                ):
                    label = f"{path.name}:roof opunk mainclip-{suffix}"
                    case, port_case = framebuffer_case(
                        "shape2d_op_unk", roof,
                        clip=(0, 320, mainclip_top, clip_bottom), operation=1,
                        label=label)
                    port_case.update(asset_index=asset_index, shape_name="roof")
                    all_cases.append((case, port_case))

            # setup_car_shapes(0) decodes the dash into the gbox-sized surface,
            # retaining the original dashboard-to-wheel origin displacement.
            dx = dash_header[4] - gbox[4]
            dy = dash_header[5] - gbox[5]
            case, port_case = gear_window_case(
                dash, gbox[0], gbox[1], dx, dy,
                f"{path.name}:dash op2 gear window", asset_index)
            all_cases.append((case, port_case))

            ins2 = loaded_shape(shapes, "ins2", path.name)
            ins2_header = machine_shape_descriptor(ins2)
            case, port_case = instrument_surface_case(
                ins2, ins2_header[0], ins2_header[1],
                f"{path.name}:ins2 op5 wheel window", asset_index)
            all_cases.append((case, port_case))

            for name in ("ins1", "ins3"):
                case, port_case = framebuffer_case(
                    "shape2d_op_unk4", loaded_shape(shapes, name, path.name),
                    clip=(0, 320, mainclip_top, MAINCLIP_BOTTOM_REPLAY),
                    operation=4, label=f"{path.name}:{name} op4 gauge overlay")
                port_case.update(asset_index=asset_index, shape_name=name)
                all_cases.append((case, port_case))
            for name in ("inm1", "inm3"):
                case, port_case = framebuffer_case(
                    "shape2d_render_bmp_as_mask",
                    loaded_shape(shapes, name, path.name),
                    clip=(0, 320, mainclip_top, MAINCLIP_BOTTOM_REPLAY),
                    operation=5, label=f"{path.name}:{name} AND gauge mask")
                port_case.update(asset_index=asset_index, shape_name=name)
                all_cases.append((case, port_case))

        with tempfile.TemporaryDirectory(prefix="sdl3-dashboard-assets-") as temp:
            directory = Path(temp)
            dll = build_asset_dll(directory, gcc)
            host_archives, actual_frames = port_results(
                dll, asset_requests, [item for _, item in all_cases], _frame(),
                python32, gcc)

        self.assertEqual(len(host_archives), len(dos_archives))
        packed_differences: list[str] = []
        for path, source_archive, dos, host in zip(
                paths, dos_source_archives, dos_archives, host_archives):
            validate_loaded_archive_pixels(
                source_archive, dos, f"{path.name} locked DOS parser")
            validate_loaded_archive_pixels(
                source_archive, host, f"{path.name} bounded SDL parser")
            if dos == host:
                continue
            shared = min(len(dos), len(host))
            mismatch = next((index for index, (left, right) in
                             enumerate(zip(dos[:shared], host[:shared]))
                             if left != right), shared)
            dos_shapes = loaded_archive_shapes(dos)
            host_shapes = loaded_archive_shapes(host)
            for shape_name in dos_shapes.keys() & host_shapes.keys():
                left, right = dos_shapes[shape_name], host_shapes[shape_name]
                if left == right:
                    continue
                dos_pixels, dos_packets, _ = decode_mode3_shape(left)
                host_pixels, host_packets, _ = decode_mode3_shape(right)
                if dos_pixels != host_pixels:
                    raise AssertionError(
                        f"{path.name}:{shape_name}: packed outputs differ in pixels")
                shared_shape = min(len(left), len(right))
                shape_at = next((index for index, (a, b) in enumerate(
                    zip(left, right)) if a != b), shared_shape)
                first_packet = next((index for index, (dos_packet, host_packet)
                                     in enumerate(zip(dos_packets, host_packets))
                                     if dos_packet != host_packet),
                                    min(len(dos_packets), len(host_packets)))
                packet_detail = "end"
                if first_packet < min(len(dos_packets), len(host_packets)):
                    dp, hp = dos_packets[first_packet], host_packets[first_packet]
                    out_at = min(dp[0], hp[0])
                    packet_detail = f"packet#{first_packet} pixel={out_at} DOS={dp} host={hp}"
                packed_differences.append(
                    f"{path.name}:{shape_name}: equivalent RLE differs "
                    f"(bytes DOS={len(left)} host={len(right)}, "
                    f"first shape byte={shape_at:#x}, {packet_detail})")
            packed_differences.append(
                f"{path.name}: paragraph-rounded archive extents DOS={len(dos)} "
                f"host={len(host)}, first archive byte={mismatch:#x}")

        expected_frames: list[bytes] = []
        for case, _ in all_cases:
            expected_result = runner.call(case)
            memory_name = {
                "shape2d_op_unk2": "gear_window",
                "shape2d_op_unk5": "instrument_window",
            }.get(case.routine, "vram")
            expected_frames.append(expected_result.memory[memory_name])

        self.assertEqual(len(actual_frames), len(expected_frames))
        frame_mismatches: list[str] = []
        for (_, port_case), dos, port in zip(all_cases, expected_frames, actual_frames):
            if dos == port:
                continue
            mismatch = next(index for index, (left, right) in
                            enumerate(zip(dos, port)) if left != right)
            width = int(port_case.get("target_width", 320))
            frame_mismatches.append(
                f"{port_case['label']}: {sum(a != b for a, b in zip(dos, port))} bytes differ; "
                f"first at ({mismatch % width},{mismatch // width}) "
                f"DOS={dos[mismatch]:02x} SDL-port={port[mismatch]:02x}")

        if os.environ.get("SDL3_DASHBOARD_PACKED_DIAGNOSTICS"):
            print("\n".join(packed_differences), file=sys.stderr)
        self.assertFalse(frame_mismatches,
                         "mode-3 STDA framebuffer differential failed:\n" +
                         "\n".join(packed_differences[:12] + frame_mismatches[:12]))
        self.assertEqual(len(all_cases), 174,
                         "coverage should include 88 dash/wheel clips, 20 roofs, 11 gboxes, 11 ins2 windows, 22 OR gauge overlays, and 22 AND masks")


if __name__ == "__main__":
    unittest.main()
