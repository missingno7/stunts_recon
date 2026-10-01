"""32-bit ctypes bridge for a 64-bit Unicorn/Python process."""
import base64
import ctypes
import json
import sys


def main():
    request = json.loads(sys.stdin.read())
    dll = ctypes.CDLL(request["dll"])
    frame_bytes = 0x10000
    reset = dll.dh_screen_reset
    reset.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32]
    reset.restype = ctypes.c_int
    read = dll.dh_read_frame
    read.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32]
    read.restype = ctypes.c_uint32

    def reset_frame(encoded):
        initial = base64.b64decode(encoded)
        storage = (ctypes.c_uint8 * len(initial)).from_buffer_copy(initial)
        if not reset(storage, len(initial)):
            raise RuntimeError("frame setup failed")

    def frame():
        output = (ctypes.c_uint8 * frame_bytes)()
        if read(output, frame_bytes) != frame_bytes:
            raise RuntimeError("frame read failed")
        return base64.b64encode(bytes(output)).decode("ascii")

    op = request["operation"]
    response = {"ok": True}
    if op in {"reset", "draw_case", "sprite_case", "sprite_1_unk_case",
              "icon_case", "runs_case", "clear_rect_case"}:
        reset_frame(request["initial"])
    if op == "frame":
        response["frame"] = frame()
    elif op == "mulscl":
        fn = dll.dh_call_mulscl
        fn.argtypes = [ctypes.c_int16, ctypes.c_int16]
        fn.restype = ctypes.c_int16
        response["ax"] = int(fn(request["left"], request["right"])) & 0xFFFF
    elif op in {"draw", "draw_case"}:
        fn = dll.dh_call_draw_filled_rect
        fn.argtypes = [ctypes.c_int16] * 5
        fn.restype = None
        fn(*request["args"])
        if op == "draw_case":
            response["frame"] = frame()
    elif op in {"sprite_1_unk3", "sprite_case"}:
        fn = dll.dh_call_sprite_1_unk3
        fn.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint16,
                       ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16,
                       ctypes.c_uint16]
        fn.restype = ctypes.c_int
        pixels = base64.b64decode(request["pixels"])
        storage = (ctypes.c_uint8 * len(pixels)).from_buffer_copy(pixels)
        response["called"] = bool(fn(storage, request["width"], request["height"],
                                     request["x"], request["y"], request["phase"]))
        if op == "sprite_case":
            response["frame"] = frame()
    elif op == "sprite_1_unk_case":
        fn = dll.dh_call_sprite_1_unk
        fn.argtypes = [ctypes.c_int16] * 5
        fn.restype = ctypes.c_int
        if not fn(*request["args"]):
            raise RuntimeError("port sprite_1_unk call failed")
        response["frame"] = frame()
    elif op == "icon_case":
        fn = dll.dh_call_icon_combine
        fn.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint16,
                       ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16,
                       ctypes.c_int]
        fn.restype = ctypes.c_int
        pixels = base64.b64decode(request["pixels"])
        storage = (ctypes.c_uint8 * len(pixels)).from_buffer_copy(pixels)
        if not fn(storage, request["width"], request["height"], request["x"],
                  request["y"], int(request["use_and"])):
            raise RuntimeError("port icon combine call failed")
        response["frame"] = frame()
    elif op == "runs_case":
        fn = dll.dh_call_shape2d_runs
        fn.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32,
                       ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16,
                       ctypes.c_uint16, ctypes.c_int]
        fn.restype = ctypes.c_int
        encoded = base64.b64decode(request["encoded"])
        storage = (ctypes.c_uint8 * len(encoded)).from_buffer_copy(encoded)
        if not fn(storage, len(encoded), request["width"], request["height"],
                  request["x"], request["y"], int(request["use_and"])):
            raise RuntimeError("port shape RLE call failed")
        response["frame"] = frame()
    elif op == "clear_rect_case":
        fn = dll.dh_call_clear_rect
        fn.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint16,
                       ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16,
                       ctypes.c_int16, ctypes.c_int16, ctypes.c_int16]
        fn.restype = ctypes.c_int
        pixels = base64.b64decode(request["source_pixels"])
        storage = (ctypes.c_uint8 * len(pixels)).from_buffer_copy(pixels)
        if not fn(storage, request["source_width"], request["source_height"],
                  *request["args"]):
            raise RuntimeError("port clear_rect call failed")
        response["frame"] = frame()
    print(json.dumps(response, separators=(",", ":")))


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(json.dumps({"ok": False, "error": str(exc)}))
        raise SystemExit(1)
