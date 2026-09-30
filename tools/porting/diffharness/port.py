"""Build and call the selected PORT_BUILD routines through an i686 DLL."""
from __future__ import annotations

import ctypes
import os
from pathlib import Path
import shutil
import struct
import subprocess
import os
import json
import base64

from .model import CallResult, HarnessError, RoutineCase
from .oracle import ROOT


PACKAGE = Path(__file__).resolve().parent
BUILD = ROOT / "build/porting/diffharness"
PORT = ROOT / "port"
DEFAULT_GCC = Path(r"C:\msys64\mingw32\bin\gcc.exe")
FRAME_BYTES = 0x10000


def find_gcc(gcc: str | Path | None = None) -> Path:
    if gcc is not None:
        candidate = Path(gcc)
    elif os.environ.get("DIFFHARNESS_GCC"):
        candidate = Path(os.environ["DIFFHARNESS_GCC"])
    elif DEFAULT_GCC.is_file():
        candidate = DEFAULT_GCC
    else:
        found = shutil.which("i686-w64-mingw32-gcc") or shutil.which("gcc")
        if not found:
            raise HarnessError("i686 MinGW GCC not found; pass --gcc or set DIFFHARNESS_GCC")
        candidate = Path(found)
    candidate = candidate.resolve()
    if not candidate.is_file():
        raise HarnessError(f"GCC not found: {candidate}")
    target = subprocess.run([str(candidate), "-dumpmachine"], capture_output=True,
                            text=True, check=False).stdout.strip()
    if "i686" not in target and "mingw32" not in target and "win32" not in target:
        raise HarnessError(f"diffharness requires a 32-bit Windows GCC target; got {target!r}")
    return candidate


class PortLibrary:
    def __init__(self, gcc: str | Path | None = None, *, force_build: bool = False):
        self.gcc = find_gcc(gcc)
        self.path = self.build(force=force_build)
        self.python32 = None
        self.dll = None
        if struct.calcsize("P") * 8 == 32:
            if hasattr(os, "add_dll_directory"):
                self._dll_directory = os.add_dll_directory(str(self.gcc.parent))
            self._load_local()
        else:
            self.python32 = self._find_python32()

    def build(self, *, force: bool = False) -> Path:
        BUILD.mkdir(parents=True, exist_ok=True)
        output = BUILD / "diffharness_port.dll"
        sources = [PACKAGE / "host_adapter.c", PACKAGE / "host_shim.c",
                   PORT / "sprite.c", PORT / "memory.c", PORT / "sincos.c"]
        dependencies = sources + [PORT / "port_runtime.h",
                                  ROOT / "tools/porting/host/compat.h"]
        newest_source = max(path.stat().st_mtime for path in dependencies)
        if output.exists() and output.stat().st_mtime >= newest_source and not force:
            return output
        command = [str(self.gcc), "-m32", "-std=gnu11", "-O2", "-DPORT_BUILD=1",
                   "-ffunction-sections", "-fdata-sections", "-shared",
                   "-static-libgcc", "-Wl,--gc-sections", "-Wl,--no-undefined",
                   "-include", str(ROOT / "tools/porting/host/compat.h"),
                   "-I", str(PORT), "-I", str(ROOT / "tools/porting/port_include"),
                   *[str(path) for path in sources], "-o", str(output)]
        env = os.environ.copy()
        env["PATH"] = str(self.gcc.parent) + os.pathsep + env.get("PATH", "")
        result = subprocess.run(command, cwd=ROOT, capture_output=True, env=env,
                                text=True, errors="replace", check=False)
        if result.returncode:
            raise HarnessError(f"failed to build the PORT_BUILD test DLL (exit {result.returncode}):\n" +
                               result.stdout + result.stderr)
        return output

    def _load_local(self) -> None:
        try:
            self.dll = ctypes.CDLL(str(self.path))
        except OSError as error:
            raise HarnessError(f"could not load i686 port DLL {self.path}: {error}") from error
        self._reset = self.dll.dh_screen_reset
        self._reset.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32]
        self._reset.restype = ctypes.c_int
        self._unk3 = self.dll.dh_call_sprite_1_unk3
        self._unk3.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint16,
                               ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16,
                               ctypes.c_uint16]
        self._unk3.restype = ctypes.c_int
        self._rect = self.dll.dh_call_draw_filled_rect
        self._rect.argtypes = [ctypes.c_int16] * 5
        self._rect.restype = None
        self._mulscl = self.dll.dh_call_mulscl
        self._mulscl.argtypes = [ctypes.c_int16, ctypes.c_int16]
        self._mulscl.restype = ctypes.c_int16
        self._read = self.dll.dh_read_frame
        self._read.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32]
        self._read.restype = ctypes.c_uint32

    @staticmethod
    def _find_python32() -> Path:
        candidates = []
        if os.environ.get("DIFFHARNESS_PYTHON32"):
            candidates.append(Path(os.environ["DIFFHARNESS_PYTHON32"]))
        candidates.append(Path(r"C:\msys64\mingw32\bin\python.exe"))
        for candidate in candidates:
            if not candidate.is_file():
                continue
            result = subprocess.run([str(candidate), "-c",
                                     "import struct; print(struct.calcsize('P')*8)"],
                                    capture_output=True, text=True, check=False)
            if result.returncode == 0 and result.stdout.strip() == "32":
                return candidate.resolve()
        raise HarnessError("the port DLL is i686 but this Python is 64-bit; install or "
                           "select a 32-bit Python with ctypes and set DIFFHARNESS_PYTHON32")

    def _worker_call(self, operation: str, **payload):
        request = {"dll": str(self.path), "operation": operation, **payload}
        env = os.environ.copy()
        env["PATH"] = str(self.gcc.parent) + os.pathsep + env.get("PATH", "")
        result = subprocess.run([str(self.python32), str(PACKAGE / "port_worker.py")],
                                input=json.dumps(request), capture_output=True,
                                text=True, errors="replace", env=env, check=False)
        if result.returncode:
            raise HarnessError("i686 ctypes worker failed:\n" + result.stderr + result.stdout)
        try:
            response = json.loads(result.stdout)
        except ValueError as error:
            raise HarnessError("i686 ctypes worker returned invalid output: " +
                               result.stdout[:300]) from error
        if not response.get("ok"):
            raise HarnessError("i686 ctypes call failed: " + response.get("error", "unknown error"))
        return response

    def call(self, case: RoutineCase) -> CallResult:
        if case.port_call is None:
            raise HarnessError(f"{case.name}: no PORT_BUILD call adapter")
        return case.port_call(self, case)

    def reset_frame(self, initial: bytes) -> None:
        if self.dll is None:
            self._worker_call("reset", initial=base64.b64encode(initial).decode("ascii"))
            return
        data = (ctypes.c_uint8 * len(initial)).from_buffer_copy(initial)
        if not self._reset(data, len(initial)):
            raise HarnessError("port frame setup failed")

    def frame(self) -> bytes:
        if self.dll is None:
            response = self._worker_call("frame")
            return base64.b64decode(response["frame"])
        output = (ctypes.c_uint8 * FRAME_BYTES)()
        written = self._read(output, FRAME_BYTES)
        if written != FRAME_BYTES:
            raise HarnessError(f"port frame read returned {written} bytes")
        return bytes(output)

    def call_mulscl(self, left: int, right: int) -> int:
        if self.dll is None:
            return int(self._worker_call("mulscl", left=left, right=right)["ax"])
        return int(self._mulscl(left, right)) & 0xFFFF

    def call_draw_filled_rect(self, *args: int) -> None:
        if self.dll is None:
            self._worker_call("draw", args=list(args))
            return
        self._rect(*args)

    def call_sprite_1_unk3(self, pixels: bytes, width: int, height: int,
                           x: int, y: int, phase: int) -> None:
        if self.dll is None:
            response = self._worker_call("sprite_1_unk3",
                                         pixels=base64.b64encode(pixels).decode("ascii"),
                                         width=width, height=height, x=x, y=y,
                                         phase=phase)
            if response.get("called") is not True:
                raise HarnessError("port sprite source setup failed")
            return
        data = (ctypes.c_uint8 * len(pixels)).from_buffer_copy(pixels)
        if not self._unk3(data, width, height, x, y, phase):
            raise HarnessError("port sprite source setup failed")

    def run_draw_filled_rect(self, initial: bytes, args: tuple[int, ...]) -> CallResult:
        if self.dll is None:
            response = self._worker_call("draw_case", args=list(args),
                                         initial=base64.b64encode(initial).decode("ascii"))
            frame = base64.b64decode(response["frame"])
        else:
            self.reset_frame(initial)
            self.call_draw_filled_rect(*args)
            frame = self.frame()
        return CallResult({}, 0, {"vram": frame}, 0, "cdecl")

    def run_sprite_1_unk3(self, initial: bytes, pixels: bytes, width: int,
                          height: int, x: int, y: int, phase: int) -> CallResult:
        if self.dll is None:
            response = self._worker_call(
                "sprite_case", initial=base64.b64encode(initial).decode("ascii"),
                pixels=base64.b64encode(pixels).decode("ascii"), width=width,
                height=height, x=x, y=y, phase=phase)
            frame = base64.b64decode(response["frame"])
        else:
            self.reset_frame(initial)
            self.call_sprite_1_unk3(pixels, width, height, x, y, phase)
            frame = self.frame()
        return CallResult({}, 0, {"vram": frame}, 0, "cdecl")
