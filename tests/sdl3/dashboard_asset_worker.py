"""i686 ctypes runner for the locked STDA mode-3 SDL regression."""
from __future__ import annotations

import base64
import ctypes
import json
import sys


FRAME_BYTES = 0x10000
ARCHIVE_CAPACITY = 0x20000


def loaded_shapes(archive: bytes) -> dict[str, bytes]:
    if len(archive) < 6:
        raise ValueError("mode-3 loader returned a short archive")
    count = int.from_bytes(archive[4:6], "little")
    payload = 6 + count * 8
    if count > 4096 or payload > len(archive):
        raise ValueError(f"mode-3 loader returned an invalid count: {count}")
    offsets = [int.from_bytes(archive[6 + count * 4 + 4 * index:
                                     10 + count * 4 + 4 * index], "little")
               for index in range(count)]
    payload_extent = len(archive) - payload
    result: dict[str, bytes] = {}
    for index, relative in enumerate(offsets):
        if relative >= payload_extent:
            raise ValueError(f"shape {index} offset {relative:#x} is out of range")
        next_offset = min((candidate for candidate in offsets if candidate > relative),
                          default=payload_extent)
        name = archive[6 + index * 4:10 + index * 4].decode("latin-1").strip()
        result[name] = archive[payload + relative:payload + next_offset]
    return result


def main() -> None:
    request = json.load(sys.stdin)
    dll = ctypes.CDLL(request["dll"])

    bounded_run_test = dll.dh_test_bounded_shape_run
    bounded_run_test.argtypes = []
    bounded_run_test.restype = ctypes.c_int
    test_result = bounded_run_test()
    if test_result != 0:
        raise RuntimeError(f"bounded mode-3 run helper test failed at case {test_result}")

    load = dll.dh_load_dashboard_archive
    load.argtypes = [
        ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32, ctypes.c_char_p,
        ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32,
    ]
    load.restype = ctypes.c_int

    render = dll.dh_render_loaded_dashboard_shape
    render.argtypes = [
        ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32,
        ctypes.POINTER(ctypes.c_uint8), ctypes.c_int16,
        ctypes.c_int16, ctypes.c_int16, ctypes.c_int16, ctypes.c_int16,
        ctypes.c_int16, ctypes.c_int16, ctypes.c_uint16, ctypes.c_uint16,
        ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32,
    ]
    render.restype = ctypes.c_int

    initial = base64.b64decode(request["initial"])
    if len(initial) != FRAME_BYTES:
        raise ValueError(f"initial frame is {len(initial)} bytes, expected {FRAME_BYTES}")
    initial_buffer = (ctypes.c_uint8 * len(initial)).from_buffer_copy(initial)

    archives: list[bytes] = []
    shapes_by_asset: list[dict[str, bytes]] = []
    for item in request["assets"]:
        compressed = base64.b64decode(item["compressed"])
        compressed_buffer = (ctypes.c_uint8 * len(compressed)).from_buffer_copy(compressed)
        archive_buffer = (ctypes.c_uint8 * ARCHIVE_CAPACITY)()
        written = load(
            compressed_buffer, len(compressed), item["stem"].encode("ascii"),
            archive_buffer, ARCHIVE_CAPACITY,
        )
        if written <= 0 or written > ARCHIVE_CAPACITY:
            raise RuntimeError(f"{item['label']}: loader returned {written}")
        archive = bytes(archive_buffer[:written])
        archives.append(archive)
        shapes_by_asset.append(loaded_shapes(archive))

    results: list[str] = []
    for item in request["cases"]:
        shape = shapes_by_asset[item["asset_index"]][item["shape_name"]]
        shape_buffer = (ctypes.c_uint8 * len(shape)).from_buffer_copy(shape)
        output_size = int(item.get("output_size", FRAME_BYTES))
        output_buffer = (ctypes.c_uint8 * output_size)()
        clip = item.get("clip", [0, 320, 0, 200])
        written = render(
            shape_buffer, len(shape), initial_buffer, item["operation"],
            *clip, item.get("x", 0), item.get("y", 0),
            item.get("target_width", 0), item.get("target_height", 0),
            output_buffer, output_size,
        )
        if written != output_size:
            raise RuntimeError(
                f"{item['label']}: renderer returned {written}, expected {output_size} bytes"
            )
        results.append(base64.b64encode(bytes(output_buffer)).decode("ascii"))

    print(json.dumps({
        "ok": True,
        "archives": [base64.b64encode(item).decode("ascii") for item in archives],
        "results": results,
    }, separators=(",", ":")))


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(json.dumps({"ok": False, "error": str(error)}))
        raise SystemExit(1)
