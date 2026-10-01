"""Full-shape parity through the frozen DOS and current SDL3 render pipelines."""
from __future__ import annotations

import ctypes
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "assets"
POOL_BYTES = 0x28A0
LINK_COUNT = 401
FRAME_BYTES = 0x10000
MENU_FIXTURE = {
    "camera": (0, 978, 0),
    "pos": (0, -840, 2880),
    "rotation": (0, 0, 69),
    "unk": 30000,
    "flags": 0,
    "material": 0,
    "projection": (36, 17, 320, 100),
    "clip": (0, 320, 0, 95),
}


def _native_worker(dll_path: Path, spec_path: Path, output_path: Path) -> int:
    """Run the 32-bit host DLL from a helper process when tests use 64-bit Python."""
    spec = json.loads(spec_path.read_text(encoding="utf-8"))
    dll = ctypes.CDLL(str(dll_path))
    run = dll.renderer_parity_run
    u8p = ctypes.POINTER(ctypes.c_uint8)
    i16p = ctypes.POINTER(ctypes.c_int16)
    run.argtypes = ([u8p, ctypes.c_uint32] + [ctypes.c_int16] * 10 +
                    [ctypes.c_uint8] * 2 + [ctypes.c_int16] * 8 +
                    [u8p, u8p, i16p, ctypes.POINTER(ctypes.c_uint16)])
    run.restype = ctypes.c_int
    output_path.parent.mkdir(parents=True, exist_ok=True)
    rows = []
    with output_path.open("wb") as stream:
        for case in spec["cases"]:
            chunk = Path(case["chunk_path"]).read_bytes()
            fixture = case["fixture"]
            words = (fixture["camera"] + fixture["pos"] + fixture["rotation"] +
                     [fixture["unk"]] + fixture["projection"] + fixture["clip"])
            chunk_buf = (ctypes.c_uint8 * len(chunk)).from_buffer_copy(chunk)
            frame = (ctypes.c_uint8 * FRAME_BYTES)()
            pool = (ctypes.c_uint8 * POOL_BYTES)()
            links = (ctypes.c_int16 * LINK_COUNT)()
            count = ctypes.c_uint16()
            ok = run(chunk_buf, len(chunk), *words[:10], fixture["flags"],
                     fixture["material"], *words[10:], frame, pool, links,
                     ctypes.byref(count))
            if ok != 1:
                raise RuntimeError(f"native renderer rejected {case['key']}")
            rows.append({"key": case["key"], "count": count.value})
            stream.write(bytes(frame))
            stream.write(bytes(pool))
            stream.write(struct.pack("<401h", *links))
    output_path.with_suffix(".json").write_text(
        json.dumps(rows, separators=(",", ":")), encoding="utf-8")
    return 0


if __name__ == "__main__" and len(sys.argv) == 5 and sys.argv[1] == "--native-worker":
    raise SystemExit(_native_worker(Path(sys.argv[2]), Path(sys.argv[3]),
                                    Path(sys.argv[4])))


sys.path.insert(0, str(ROOT / "build" / "python"))
sys.path.insert(0, str(ROOT / "tools" / "porting"))

import format_reference as fmt  # noqa: E402
from tools.porting.diffharness.oracle import (  # noqa: E402
    DEFAULT_LOAD_SEGMENT,
    OracleImage,
    SymbolMap,
    dgroup_segment,
)
from unicorn import (  # noqa: E402
    Uc,
    UC_ARCH_X86,
    UC_HOOK_CODE,
    UC_HOOK_INTR,
    UC_MODE_16,
)
from unicorn.x86_const import (  # noqa: E402
    UC_X86_REG_AX,
    UC_X86_REG_BX,
    UC_X86_REG_CX,
    UC_X86_REG_DX,
    UC_X86_REG_SI,
    UC_X86_REG_DI,
    UC_X86_REG_BP,
    UC_X86_REG_SP,
    UC_X86_REG_CS,
    UC_X86_REG_DS,
    UC_X86_REG_ES,
    UC_X86_REG_SS,
    UC_X86_REG_IP,
    UC_X86_REG_EFLAGS,
)


REGS = {
    "ax": UC_X86_REG_AX, "bx": UC_X86_REG_BX, "cx": UC_X86_REG_CX,
    "dx": UC_X86_REG_DX, "si": UC_X86_REG_SI, "di": UC_X86_REG_DI,
    "bp": UC_X86_REG_BP, "sp": UC_X86_REG_SP, "cs": UC_X86_REG_CS,
    "ds": UC_X86_REG_DS, "es": UC_X86_REG_ES, "ss": UC_X86_REG_SS,
    "ip": UC_X86_REG_IP,
}


def _linear(segment: int, offset: int) -> int:
    return ((segment << 4) + offset) & 0xFFFFF


class FrozenMachine:
    """Fresh real-mode state per fixture, with only the pool allocation trapped."""

    def __init__(self, image: OracleImage, symbols: SymbolMap):
        self.symbols = symbols
        self.load = DEFAULT_LOAD_SEGMENT
        self.dgroup = dgroup_segment(self.load)
        self.uc = Uc(UC_ARCH_X86, UC_MODE_16)
        self.uc.mem_map(0, 0x100000)
        self.uc.mem_write(self.load << 4, image.relocated(self.load))
        self.uc.reg_write(UC_X86_REG_EFLAGS, 0x0202)
        self.state: dict[str, object] = {}

        def on_code(machine, address, _size, _user):
            if address == _linear(0x7000, 0):
                self.state["returned"] = True
                machine.emu_stop()

        def on_intr(machine, number, _user):
            self.state["trap"] = f"INT {number:#04x}"
            machine.emu_stop()

        self.uc.hook_add(UC_HOOK_CODE, on_code)
        self.uc.hook_add(UC_HOOK_INTR, on_intr)

    def call(self, name: str, args: tuple[int, ...] = (), budget: int = 3_000_000) -> None:
        address = self.symbols.resolve(name)
        cs, ip = address.far_at(self.load)
        # In the DOS small model SS and DS are one segment. The original ASM
        # helpers dereference C stack locals through DS near pointers.
        stack_segment, stack_pointer = self.dgroup, 0xF000
        words = [0, 0x7000, *(int(value) & 0xFFFF for value in args)]
        self.uc.mem_write(
            _linear(stack_segment, stack_pointer),
            struct.pack("<" + "H" * len(words), *words),
        )
        registers = {
            "cs": cs, "ip": ip, "ds": self.dgroup, "es": 0xA000,
            "ss": stack_segment, "sp": stack_pointer, "bp": 0,
            "ax": 0, "bx": 0, "cx": 0, "dx": 0, "si": 0, "di": 0,
        }
        for name_, value in registers.items():
            self.uc.reg_write(REGS[name_], value & 0xFFFF)
        self.state = {"returned": False, "trap": None}
        try:
            self.uc.emu_start(_linear(cs, ip), 0, count=budget)
        except Exception as error:
            pc = (self.uc.reg_read(UC_X86_REG_CS) & 0xFFFF,
                  self.uc.reg_read(UC_X86_REG_IP) & 0xFFFF)
            raise AssertionError(f"original {name} faulted at {pc[0]:04x}:{pc[1]:04x}: {error}") from error
        if self.state["trap"]:
            raise AssertionError(f"original {name} trapped: {self.state['trap']}")
        if not self.state["returned"]:
            raise AssertionError(f"original {name} did not reach the return sentinel")

    def write_ds(self, offset: int, data: bytes) -> None:
        self.uc.mem_write(_linear(self.dgroup, offset), data)

    def read_ds(self, offset: int, size: int) -> bytes:
        return bytes(self.uc.mem_read(_linear(self.dgroup, offset), size))


def _pack_far(offset: int, segment: int) -> bytes:
    return struct.pack("<HH", offset, segment)


def _oracle_render(image: OracleImage, symbols: SymbolMap, chunk: bytes,
                   fixture: dict[str, object]) -> dict[str, object]:
    machine = FrozenMachine(image, symbols)
    ds = machine.dgroup
    resource_segment, pool_segment = 0x6800, 0x7800
    chunk_offset, ts_offset, clip_offset, shape_offset = 0x0100, 0xFF00, 0xFF30, 0xFF40
    machine.uc.mem_write(_linear(resource_segment, chunk_offset), chunk)
    vertex_count, primitive_count, paint_count = chunk[0], chunk[1], chunk[2]
    cull1_offset = chunk_offset + 4 + vertex_count * 6
    cull2_offset = cull1_offset + primitive_count * 4
    primitive_offset = cull2_offset + primitive_count * 4
    if primitive_offset - chunk_offset > len(chunk):
        raise AssertionError("3D shape resource has a truncated primitive stream")

    descriptor = bytearray(22)
    struct.pack_into("<H", descriptor, 0, vertex_count)
    descriptor[2:6] = _pack_far(chunk_offset + 4, resource_segment)
    struct.pack_into("<HBB", descriptor, 6, primitive_count, paint_count, chunk[3])
    descriptor[10:14] = _pack_far(primitive_offset, resource_segment)
    descriptor[14:18] = _pack_far(cull1_offset, resource_segment)
    descriptor[18:22] = _pack_far(cull2_offset, resource_segment)
    machine.write_ds(shape_offset, bytes(descriptor))

    position = fixture["pos"]
    rotation = fixture["rotation"]
    ts = struct.pack("<hhhHHhhhHBB", *position, shape_offset, 0,
                     *rotation, fixture["unk"], fixture["flags"], fixture["material"])
    machine.write_ds(ts_offset, ts)
    machine.write_ds(clip_offset, struct.pack("<hhhh", *fixture["clip"]))

    color_offsets = (0xD000, 0xD200, 0xD400, 0xD600)
    tables = (
        [index & 0xFF for index in range(256)],
        [(index + 1) & 0xFF for index in range(256)],
        [0] * 256,
        [0] * 256,
    )
    for offset, values in zip(color_offsets, tables):
        machine.write_ds(offset, struct.pack("<256h", *values))
    machine.call("copy_material_list_pointers", (*color_offsets, 0))

    # Keep the original cache setup; intercept only its memory-manager boundary
    # and return a fresh, zero-filled paragraph segment for this fixture.
    allocation = symbols.resolve("mmgr_alloc_resbytes")
    alloc_cs, alloc_ip = allocation.far_at(machine.load)
    alloc_entry = _linear(alloc_cs, alloc_ip)
    allocation_calls = []

    def pool_allocator(machine_uc, address, _size, _user):
        if address != alloc_entry:
            return
        allocation_calls.append(address)
        sp = machine_uc.reg_read(UC_X86_REG_SP) & 0xFFFF
        ss = machine_uc.reg_read(UC_X86_REG_SS) & 0xFFFF
        return_ip, return_cs = struct.unpack(
            "<HH", machine_uc.mem_read(_linear(ss, sp), 4))
        machine_uc.reg_write(UC_X86_REG_AX, 0)
        machine_uc.reg_write(UC_X86_REG_DX, pool_segment)
        machine_uc.reg_write(UC_X86_REG_IP, return_ip)
        machine_uc.reg_write(UC_X86_REG_CS, return_cs)
        machine_uc.reg_write(UC_X86_REG_SP, (sp + 4) & 0xFFFF)

    hook = machine.uc.hook_add(UC_HOOK_CODE, pool_allocator)
    machine.call("initialize_polyinfo")
    machine.uc.hook_del(hook)
    if len(allocation_calls) != 1:
        raise AssertionError(f"original initializer made {len(allocation_calls)} pool requests")
    machine.write_ds(0x5762, _pack_far(0, pool_segment))
    machine.call("set_projection", fixture["projection"])
    camera = fixture["camera"]
    machine.call("select_rot", (*camera, clip_offset, 1 if fixture["flags"] & 1 else 0))
    machine.call("trans_op", (ts_offset,))

    count = struct.unpack("<H", machine.read_ds(0x8B76, 2))[0]
    pool = bytes(machine.uc.mem_read(_linear(pool_segment, 0), POOL_BYTES))
    links = struct.unpack("<401h", machine.read_ds(0x5766, LINK_COUNT * 2))
    machine.uc.mem_write(_linear(0xA000, 0), bytes(FRAME_BYTES))
    machine.call("polyinfo")
    frame = bytes(machine.uc.mem_read(_linear(0xA000, 0), FRAME_BYTES))
    return {"count": count, "pool": pool, "links": links, "frame": frame}


NATIVE_DRIVER = r'''\
static I8 renderer_parity_pool[0x28A0];
static I16 renderer_parity_colors[256], renderer_parity_color2[256];
static I16 renderer_parity_patterns[256], renderer_parity_patterns2[256];
extern U8 *port_video_pixels(void);
extern void port_sprite_init(void);
extern void initialize_polyinfo(void);

__declspec(dllexport) int renderer_parity_run(
    const U8 *chunk, U32 chunk_size, I16 z, I16 x, I16 y,
    I16 posx, I16 posy, I16 posz, I16 rotx, I16 roty, I16 rotz,
    I16 unk, U8 flags, U8 material,
    I16 proj_left, I16 proj_top, I16 proj_width, I16 proj_height,
    I16 clip_left, I16 clip_right, I16 clip_top, I16 clip_bottom,
    U8 *frame_out, U8 *pool_out, I16 *links_out, U16 *count_out)
{
    U16 nv, np, npaint, i;
    U32 primitive_offset, cull1_offset, cull2_offset, j;
    struct SHAPE3D shape;
    struct RECTANGLE clip = {clip_left, clip_right, clip_top, clip_bottom};
    struct TRANSFORMEDSHAPE3D ts;
    if (chunk == 0 || chunk_size < 4 || frame_out == 0 || pool_out == 0 ||
        links_out == 0 || count_out == 0)
        return 0;
    nv = chunk[0]; np = chunk[1]; npaint = chunk[2];
    cull1_offset = 4u + (U32)nv * 6u;
    cull2_offset = cull1_offset + (U32)np * 4u;
    primitive_offset = cull2_offset + (U32)np * 4u;
    if (primitive_offset > chunk_size)
        return 0;
    for (i = 0; i < 401; ++i) poly_link_list[i] = 0;
    for (i = 0; i < 400; ++i) poly_info_ptrs[i] = 0;
    for (j = 0; j < sizeof(renderer_parity_pool); ++j) renderer_parity_pool[j] = 0;
    mat_y_rot_angle = 0xFFFFu;
    backlightovr8 = 0;
    initialize_polyinfo();
    polyinfoptr = renderer_parity_pool;
    for (i = 0; i < 256; ++i) {
        renderer_parity_colors[i] = (I16)(i & 0xFFu);
        renderer_parity_color2[i] = (I16)((i + 1u) & 0xFFu);
        renderer_parity_patterns[i] = renderer_parity_patterns2[i] = 0;
    }
    copy_material_list_pointers(renderer_parity_colors, renderer_parity_color2,
                                renderer_parity_patterns, renderer_parity_patterns2, 0);
    shape.numverts = nv;
    shape.verts = (struct VECTOR *)(chunk + 4);
    shape.numprimitives = np;
    shape.numpaints = (U8)npaint;
    shape.reserved = chunk[3];
    shape.primitives = (U8 *)(chunk + primitive_offset);
    shape.cull1 = (I32 *)(chunk + cull1_offset);
    shape.cull2 = (I32 *)(chunk + cull2_offset);
    ts.pos.x = posx; ts.pos.y = posy; ts.pos.z = posz;
    ts.shapeptr = &shape; ts.rectptr = 0;
    ts.rotvec.x = rotx; ts.rotvec.y = roty; ts.rotvec.z = rotz;
    ts.unk = unk; ts.ts_flags = flags; ts.material = material;
    for (j = 0; j < 0x10000u; ++j) port_video_pixels()[j] = 0;
    port_sprite_init();
    set_projection(proj_left, proj_top, proj_width, proj_height);
    (void)select_rot(z, x, y, &clip, (flags & 1u) != 0u);
    (void)trans_op(&ts);
    *count_out = (U16)polygonnumber;
    for (j = 0; j < sizeof(renderer_parity_pool); ++j)
        pool_out[j] = (U8)renderer_parity_pool[j];
    for (i = 0; i < 401; ++i) links_out[i] = poly_link_list[i];
    polyinfo();
    for (j = 0; j < 0x10000u; ++j) frame_out[j] = port_video_pixels()[j];
    return 1;
}
'''


NATIVE_SHIM = r'''#include "port_runtime.h"
#include <stdlib.h>
#include <stdio.h>
struct RendererMatrix { int16_t vals[9]; };
struct RendererMatrix wkmatx;
int8_t backlightovr8;
uint8_t renderer_parity_framebuffer[PORT_VIDEO_MEMORY_BYTES];
uint8_t *port_video_pixels(void) { return renderer_parity_framebuffer; }
void port_video_publish(const char *reason) { (void)reason; }
void port_guest_unwind(const char *reason)
{
    fprintf(stderr, "renderer parity abort: %s\\n", reason ? reason : "unknown");
    abort();
}
void port_stub_fail(const char *symbol) { port_guest_unwind(symbol); }
int port_memory_extent(const void *pointer, size_t *remaining_out)
{ (void)pointer; (void)remaining_out; return 0; }
void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{ (void)name; (void)paragraphs; return 0; }
void mmgr_free(void *pointer) { (void)pointer; }
int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out)
{ (void)pointer; (void)address_out; (void)remaining_out; return 0; }
void *port_far_resolve(PortFarPtr pointer, size_t extent)
{ (void)pointer; (void)extent; return 0; }
void *mmgr_alloc_resbytes(char *name, int32_t bytes)
{ (void)name; (void)bytes; return 0; }
'''


def _compile_native_dll(temporary: Path) -> Path:
    compiler = Path(r"C:\msys64\mingw32\bin\gcc.exe")
    if not compiler.is_file():
        raise unittest.SkipTest("the pinned 32-bit GCC is not installed")
    sys.path.insert(0, str(ROOT / "port"))
    sys.path.insert(0, str(ROOT / "tools" / "porting"))
    import host_probe_modes as probe
    import importlib.util

    build_path = ROOT / "port" / "build.py"
    spec = importlib.util.spec_from_file_location("stunts_pipeline_port_build", build_path)
    if spec is None or spec.loader is None:
        raise AssertionError("could not load the SDL3 production build adapter")
    builder = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(builder)
    builder.BUILD = temporary / "private-build"
    original_run_one = probe.run_one
    original_flags = list(probe.FLAGS)
    probe_state = {name: getattr(probe, name) for name in
                   ("ROOT", "PORT", "HOST", "WORK", "GCC", "STRICT_CENTRAL")}
    import host_probe_declarations as declarations
    declaration_state = (declarations.ROOT, dict(declarations.ALIASES),
                         dict(declarations.HOST_OVERRIDES))

    def append_harness(source: str, overlay: Path, config: Path, index: int):
        if source == "src/obj_seg006.c":
            generated = overlay.read_text(encoding="latin-1")
            if "port_renderer_double_word(ts->unk)" not in generated:
                raise AssertionError("fresh production overlay omitted the locked renderer adapter")
            if "poly_link_list[POLYINFO_CAPACITY + 1]" not in generated:
                raise AssertionError("fresh production overlay omitted the DOS queue-head storage adapter")
            overlay.write_text(generated + "\n" + NATIVE_DRIVER, encoding="latin-1")
        return original_run_one(source, overlay, config, index)

    private_path = os.pathsep.join(filter(None, [
        str(compiler.parent), os.environ.get("PATH", "")]))
    build_environment = os.environ.copy()
    build_environment["PATH"] = private_path
    with patch.dict(os.environ, {"PATH": private_path}):
        probe.run_one = append_harness
        # Split text/data so the isolated renderer link does not pull unrelated
        # startup, audio, file, or SDL entry points from this real host overlay.
        probe.FLAGS = original_flags + ["-ffunction-sections", "-fdata-sections"]
        try:
            game_objects, report = builder.build_game_objects(compiler)
        finally:
            probe.run_one = original_run_one
            probe.FLAGS = original_flags
            for name, value in probe_state.items():
                setattr(probe, name, value)
            declarations.ROOT = declaration_state[0]
            declarations.ALIASES.clear()
            declarations.ALIASES.update(declaration_state[1])
            declarations.HOST_OVERRIDES.clear()
            declarations.HOST_OVERRIDES.update(declaration_state[2])
    if report["source_count"] != 38 or report["object_passes"] != 38:
        raise AssertionError("fresh production renderer object build did not compile all 38 sources")
    obj006 = next(path for path in game_objects if path.name.endswith("_obj_seg006.o"))
    render_support = [
        obj006,
        *(next(path for path in game_objects if path.name.endswith("_" + suffix + ".o"))
          for suffix in ("polarRadius3D", "preRender_wheel", "preRender_wheel_helper",
                         "preRender_wheel_helper2", "preRender_wheel_helper3",
                         "seg024_matrot", "heapsort_by_order")),
    ]
    common = [
        str(compiler), "-std=gnu11", "-O0", "-g", "-DPORT_BUILD=1",
        "-ffunction-sections", "-fdata-sections",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
    ]
    support_objects = []
    for source in ("matrix.c", "polang.c", "projection.c", "sincos.c", "sprite.c"):
        obj = temporary / f"{Path(source).stem}.o"
        command = [*common, "-c", str(ROOT / "port" / source), "-o", str(obj)]
        built = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                               errors="replace", timeout=60, env=build_environment)
        if built.returncode:
            raise AssertionError(f"renderer support compile failed for {source}:\n{built.stderr}")
        support_objects.append(obj)

    shim_source = temporary / "renderer_shim.c"
    shim_source.write_text(NATIVE_SHIM, encoding="utf-8")
    shim_object = temporary / "renderer_shim.o"
    built = subprocess.run([*common, "-c", str(shim_source), "-o", str(shim_object)],
                           cwd=ROOT, capture_output=True, text=True,
                           errors="replace", timeout=60, env=build_environment)
    if built.returncode:
        raise AssertionError(f"renderer shim compile failed:\n{built.stderr}")

    dll = temporary / "renderer_parity.dll"
    linked = subprocess.run([
        str(compiler), "-shared", "-Wl,--gc-sections",
        *(str(path) for path in render_support),
        *(str(path) for path in support_objects), str(shim_object), "-o", str(dll),
    ], cwd=ROOT, capture_output=True, text=True, errors="replace", timeout=60,
       env=build_environment)
    if linked.returncode:
        raise AssertionError(f"fresh renderer DLL link failed:\n{linked.stderr}")
    return dll


def _shape_resources() -> list[dict[str, object]]:
    files = sorted(list(ASSETS.glob("ST*.P3S")) +
                   [ASSETS / "GAME1.P3S", ASSETS / "GAME2.P3S"])
    if len(files) != 13:
        raise unittest.SkipTest("all 11 car and both track shape archives are required")
    rows = []
    for path in files:
        decoded, _header = fmt.decompress(path.read_bytes())
        archive = fmt.parse_archive(decoded)
        for entry in archive["entries"]:
            chunk = decoded[entry["start"]:entry["end"]]
            fmt.parse_shape3d_chunk(chunk)
            rows.append({"key": f"{path.name}_{entry['name']}",
                         "archive": path.name, "name": entry["name"], "chunk": chunk})
    if len(rows) != 194:
        raise AssertionError(f"expected 194 stock car/track shapes, found {len(rows)}")
    return rows


def _ordered_chain(links: tuple[int, ...], count: int, label: str) -> tuple[int, ...]:
    if count == 0:
        return ()
    link = links[400]
    chain = []
    for _ in range(count):
        if link < 0 or link >= 400:
            raise AssertionError(f"{label}: invalid linked-list index {link}")
        chain.append(link)
        link = links[link]
    if link >= 0:
        raise AssertionError(f"{label}: queue did not terminate after {count} records")
    return tuple(chain)


def _fixture_dict(fixture: dict[str, object]) -> dict[str, object]:
    return {key: list(value) if isinstance(value, tuple) else value
            for key, value in fixture.items()}


class Sdl3ShapePipelineTests(unittest.TestCase):
    def test_all_stock_models_match_frozen_transforms_queues_and_full_frames(self):
        if not ASSETS.is_dir():
            self.skipTest("original game assets are not installed")
        resources = _shape_resources()
        python32 = Path(r"C:\msys64\mingw32\bin\python.exe")
        if not python32.is_file():
            self.skipTest("the pinned 32-bit Python needed to load the SDL3 DLL is unavailable")

        with tempfile.TemporaryDirectory(prefix="sdl3-shape-pipeline-") as temporary:
            temp = Path(temporary)
            dll = _compile_native_dll(temp)
            chunks_dir = temp / "chunks"
            chunks_dir.mkdir()
            cases = []
            for index, row in enumerate(resources):
                chunk_path = chunks_dir / f"shape-{index:03}.bin"
                chunk_path.write_bytes(row["chunk"])
                cases.append({"key": row["key"], "chunk_path": str(chunk_path),
                              "fixture": _fixture_dict(MENU_FIXTURE)})
            spec_path = temp / "native-spec.json"
            spec_path.write_text(json.dumps({"cases": cases}), encoding="utf-8")
            native_path = temp / "native-results.bin"
            environment = os.environ.copy()
            environment["PATH"] = os.pathsep.join(filter(None, [
                str(Path(r"C:\msys64\mingw32\bin")), environment.get("PATH", "")]))
            worker = subprocess.run(
                [str(python32), str(Path(__file__).resolve()), "--native-worker",
                 str(dll), str(spec_path), str(native_path)],
                cwd=ROOT, capture_output=True, text=True, errors="replace",
                timeout=180, env=environment)
            self.assertEqual(worker.returncode, 0,
                             f"native replay worker failed:\n{worker.stdout}\n{worker.stderr}")

            # One immutable image is shared as bytes; each oracle fixture gets
            # a newly mapped CPU, zeroed BSS, reset VGA aperture, and trapped
            # allocator return.
            image = OracleImage.load(ASSETS)
            symbols = SymbolMap()
            oracle_counts = []
            frame_mismatches = []
            native_rows = json.loads(native_path.with_suffix(".json").read_text(
                encoding="utf-8"))
            self.assertEqual(len(native_rows), len(resources))
            record_bytes = FRAME_BYTES + POOL_BYTES + LINK_COUNT * 2
            with native_path.open("rb") as stream:
                for index, row in enumerate(resources):
                    with self.subTest(shape=row["key"]):
                        native_blob = stream.read(record_bytes)
                        self.assertEqual(len(native_blob), record_bytes)
                        native_frame = native_blob[:FRAME_BYTES]
                        native_pool = native_blob[FRAME_BYTES:FRAME_BYTES + POOL_BYTES]
                        native_links = struct.unpack(
                            "<401h", native_blob[FRAME_BYTES + POOL_BYTES:])
                        oracle = _oracle_render(image, symbols, row["chunk"], MENU_FIXTURE)
                        native_count = native_rows[index]["count"]
                        self.assertEqual(native_rows[index]["key"], row["key"])
                        self.assertEqual(oracle["count"], native_count,
                                         "polygon count differs")
                        self.assertEqual(oracle["pool"], native_pool,
                                         "full 0x28A0-byte polygon pool differs")
                        oracle_chain = _ordered_chain(oracle["links"], oracle["count"],
                                                      row["key"] + " oracle")
                        native_chain = _ordered_chain(native_links, native_count,
                                                      row["key"] + " native")
                        self.assertEqual(oracle_chain, native_chain,
                                         "ordered polygon queue differs")
                        if oracle["frame"] != native_frame:
                            offsets = [i for i, (left, right) in enumerate(
                                zip(oracle["frame"], native_frame)) if left != right]
                            frame_mismatches.append({
                                "shape": row["key"], "bytes": len(offsets),
                                "first": [(offset, oracle["frame"][offset],
                                           native_frame[offset]) for offset in offsets[:8]],
                            })
                        oracle_counts.append(oracle["count"])
                self.assertEqual(stream.read(1), b"", "native worker produced extra records")
            self.assertEqual(sum(count > 0 for count in oracle_counts), 161,
                             "the menu fixture must exercise 161 nonculled resources")
            self.assertEqual(sum(count == 0 for count in oracle_counts), 33,
                             "the menu fixture must retain its 33 deliberate cull cases")
            self.assertEqual(frame_mismatches, [],
                             "full 64 KiB frozen/native frames differ: " +
                             json.dumps(frame_mismatches, separators=(",", ":")))


if __name__ == "__main__":
    unittest.main()
