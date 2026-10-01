"""Exercise SDL3 crash reports and their minidump exception records."""
from __future__ import annotations

import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
PORT = ROOT / "port"
BUILD = ROOT / "build" / "sdl3"
NATIVE_EXE = BUILD / "stunts.exe"
BUILD_REPORT = BUILD / "build-report.json"
ASSETS = BUILD / "runtime" / "assets"
SDL_ROOT = Path(os.environ.get("SDL3_ROOT", r"C:\tools\sdl3-3.4.16-i686"))
PROBE_SOURCE = ROOT / "tests" / "sdl3" / "diagnostics_probe.c"

MINIDUMP_SIGNATURE = int.from_bytes(b"MDMP", "little")
MINIDUMP_EXCEPTION_STREAM = 6
STATUS_ACCESS_VIOLATION = 0xC0000005
PROBE_ABORT_EXCEPTION = 0xE0000001


def compiler_path() -> str | None:
    supplied = os.environ.get("CC")
    if supplied:
        return shutil.which(supplied) or (supplied if Path(supplied).is_file() else None)
    for candidate in (Path(r"C:\msys64\mingw32\bin\gcc.exe"),
                      Path(r"C:\msys64\mingw64\bin\gcc.exe")):
        if candidate.is_file():
            return str(candidate)
    return shutil.which("gcc")


def make_test_environment(compiler: str | None = None) -> dict[str, str]:
    environment = os.environ.copy()
    path_parts = []
    if compiler:
        path_parts.append(str(Path(compiler).resolve().parent))
    path_parts.extend((str(SDL_ROOT / "bin"),
                       str(Path(os.environ.get("SystemRoot", r"C:\Windows")) /
                           "System32")))
    path_parts.append(environment.get("PATH", ""))
    environment["PATH"] = os.pathsep.join(part for part in path_parts if part)
    environment["SDL_VIDEODRIVER"] = "dummy"
    environment["SDL_AUDIODRIVER"] = "dummy"
    return environment


def parse_exception_stream(path: Path) -> tuple[int, int]:
    """Return the MINIDUMP_EXCEPTION_STREAM thread ID and exception code."""
    data = path.read_bytes()
    if len(data) < 32:
        raise AssertionError(f"minidump is too short to contain a header: {path}")
    signature, = struct.unpack_from("<I", data, 0)
    if signature != MINIDUMP_SIGNATURE:
        raise AssertionError(f"minidump has invalid signature: {path}")
    stream_count, = struct.unpack_from("<I", data, 8)
    directory_rva, = struct.unpack_from("<I", data, 12)
    if directory_rva + stream_count * 12 > len(data):
        raise AssertionError("minidump stream directory is outside the file")

    for index in range(stream_count):
        stream_type, data_size, rva = struct.unpack_from(
            "<III", data, directory_rva + index * 12)
        if stream_type != MINIDUMP_EXCEPTION_STREAM:
            continue
        # The stream has a 4-byte thread id, 4-byte alignment field, the
        # 152-byte MINIDUMP_EXCEPTION, and an 8-byte context location.
        if data_size < 168 or rva + data_size > len(data):
            raise AssertionError("minidump exception stream is truncated")
        thread_id, = struct.unpack_from("<I", data, rva)
        exception_code, = struct.unpack_from("<I", data, rva + 8)
        return thread_id, exception_code
    raise AssertionError("minidump has no exception stream")


def read_report_field(report: str, key: str) -> str:
    prefix = f"{key}="
    for line in report.splitlines():
        if line.startswith(prefix):
            return line[len(prefix):]
    raise AssertionError(f"crash report has no {key}= field:\n{report}")


class Sdl3DiagnosticsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        if os.name != "nt":
            raise unittest.SkipTest("SDL3 diagnostics use Win32 exception records")
        compiler = compiler_path()
        if compiler is None:
            raise unittest.SkipTest("GCC is required for the SDL3 host checks")
        if not (SDL_ROOT / "include" / "SDL3" / "SDL.h").is_file():
            raise unittest.SkipTest(f"SDL3 headers not found under {SDL_ROOT}")
        if not (SDL_ROOT / "lib" / "libSDL3.dll.a").is_file():
            raise unittest.SkipTest(f"SDL3 import library not found under {SDL_ROOT}")

        target = subprocess.run([compiler, "-dumpmachine"], capture_output=True,
                                text=True, check=False)
        if target.returncode != 0 or target.stdout.strip() != "i686-w64-mingw32":
            raise unittest.SkipTest(
                "SDL3 diagnostics probe requires the i686 MinGW compiler "
                f"matching SDL3; got {target.stdout.strip()!r}")

        cls._temporary = tempfile.TemporaryDirectory(prefix="stunts-sdl3-diagnostics-")
        cls.temp_root = Path(cls._temporary.name)
        cls.probe_exe = cls.temp_root / "diagnostics-probe.exe"
        command = [
            compiler, "-std=gnu11", "-O0", "-g", "-Wall", "-Wextra",
            "-Wpedantic", "-Werror", "-Wno-unused-parameter",
            "-I", str(PORT), "-I", str(SDL_ROOT / "include"),
            str(PROBE_SOURCE), str(PORT / "diagnostics.c"),
            "-L", str(SDL_ROOT / "lib"), "-lSDL3", "-o", str(cls.probe_exe),
        ]
        environment = make_test_environment(compiler)
        result = subprocess.run(command, cwd=ROOT, env=environment,
                                capture_output=True, text=True, check=False)
        if result.returncode != 0:
            raise AssertionError(
                "SDL3 diagnostics probe did not compile:\n" +
                result.stdout + result.stderr)
        cls.environment = environment

    @classmethod
    def tearDownClass(cls) -> None:
        temporary = getattr(cls, "_temporary", None)
        if temporary is not None:
            temporary.cleanup()

    def run_probe(self, root: Path, mode: str = "normal",
                  expected_thread_file: Path | None = None,
                  timeout: int = 25, *, debug: bool = True) -> subprocess.CompletedProcess[str]:
        command = [str(self.probe_exe),
                   f"--diagnostics-dir={root}", f"--mode={mode}"]
        if debug:
            command.append("--debug")
        if expected_thread_file is not None:
            command.append(f"--expected-thread-file={expected_thread_file}")
        return subprocess.run(command, cwd=ROOT, env=self.environment,
                              capture_output=True, text=True, timeout=timeout,
                              check=False)

    def assert_crash_capture(self, mode: str, expected_code: int) -> None:
        with tempfile.TemporaryDirectory(prefix=f"stunts-sdl3-{mode}-") as temporary:
            temporary_root = Path(temporary)
            diagnostics_root = temporary_root / "diagnostics"
            expected_thread_file = temporary_root / "expected-thread.txt"
            result = self.run_probe(diagnostics_root, mode, expected_thread_file,
                                    debug=mode != "main-fault")
            self.assertNotEqual(result.returncode, 0,
                                "the deliberate child-process fault unexpectedly returned")
            expected_thread_id = int(expected_thread_file.read_text(encoding="ascii").strip())
            sessions = sorted(diagnostics_root.glob("stunts-*"))
            self.assertEqual(len(sessions), 1, f"unexpected sessions: {sessions}")
            session = sessions[0]
            crash_path = session / "crash.txt"
            dump_path = session / "crash.dmp"
            self.assertTrue(crash_path.is_file(), "crash report was not written")
            self.assertTrue(dump_path.is_file(), "minidump was not written")
            report = crash_path.read_text(encoding="ascii", errors="replace")
            self.assertEqual(int(read_report_field(report, "exception_code"), 16),
                             expected_code)
            self.assertEqual(int(read_report_field(report, "thread_id")),
                             expected_thread_id)
            self.assertIn("minidump_written=yes", report)

            dump_thread_id, dump_exception_code = parse_exception_stream(dump_path)
            self.assertEqual(dump_thread_id, expected_thread_id,
                             "minidump exception stream names a different thread")
            self.assertEqual(dump_exception_code, expected_code,
                             "minidump exception stream names a different exception")

            copied_exe = session / "stunts-crashed.exe"
            self.assertTrue(copied_exe.is_file(), "crashed executable was not preserved")
            self.assertEqual(copied_exe.read_bytes(), self.probe_exe.read_bytes())

    def test_main_thread_access_violation_is_reported_in_dump(self) -> None:
        self.assert_crash_capture("main-fault", STATUS_ACCESS_VIOLATION)

    def test_guest_sdl_thread_access_violation_is_reported_in_dump(self) -> None:
        self.assert_crash_capture("guest-fault", STATUS_ACCESS_VIOLATION)

    def test_abort_is_reported_in_dump(self) -> None:
        self.assert_crash_capture("abort", PROBE_ABORT_EXCEPTION)

    def test_debug_creates_unique_sessions_and_captures_normal_exit(self) -> None:
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-debug-sessions-") as temporary:
            root = Path(temporary) / "diagnostics"
            first = self.run_probe(root)
            second = self.run_probe(root)
            self.assertEqual(first.returncode, 0, first.stdout + first.stderr)
            self.assertEqual(second.returncode, 0, second.stdout + second.stderr)

            sessions = sorted(root.glob("stunts-*"))
            self.assertEqual(len(sessions), 2, f"sessions were not unique: {sessions}")
            self.assertNotEqual(sessions[0].name, sessions[1].name)
            trace_paths = set()
            for session in sessions:
                log = (session / "session.log").read_text(
                    encoding="utf-8", errors="replace")
                self.assertIn("debug=1", log)
                self.assertEqual(read_report_field(log, "exit_status"), "0")
                self.assertEqual(read_report_field(log, "stop_reason"),
                                 "probe normal exit")
                trace_path = Path(read_report_field(log, "trace_path"))
                self.assertTrue(trace_path.is_file(), f"trace missing: {trace_path}")
                trace_paths.add(trace_path)
                header = json.loads(trace_path.read_text(encoding="utf-8").splitlines()[0])
                build_id = read_report_field(log, "build_id")
                self.assertEqual(header.get("build_id"), build_id)
                self.assertTrue(build_id.startswith("standalone-diagnostics-probe"))
            self.assertEqual(len(trace_paths), 2)

    def test_unavailable_diagnostics_path_does_not_prevent_normal_exit(self) -> None:
        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-no-diagnostics-") as temporary:
            blocked_root = Path(temporary) / "regular-file"
            blocked_root.write_text("preserve", encoding="ascii")
            result = self.run_probe(blocked_root)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("diagnostics directory could not be created",
                          result.stdout + result.stderr)
            self.assertEqual(blocked_root.read_text(encoding="ascii"), "preserve")

    def test_native_debug_trace_and_explicit_trace_override(self) -> None:
        if not NATIVE_EXE.is_file() or not BUILD_REPORT.is_file() or not ASSETS.is_dir():
            self.skipTest("build the complete SDL3 executable and runtime assets first")
        report = json.loads(BUILD_REPORT.read_text(encoding="utf-8"))
        self.assertEqual(report.get("target"), "i686-w64-mingw32")
        self.assertEqual(report.get("game_c_objects"), 38)
        self.assertEqual(report.get("port_objects"), 32)
        self.assertEqual(report.get("function_stub_count"), 0)
        self.assertEqual(report.get("data_stub_count"), 0)
        inputs = [PORT / "diagnostics.c", PORT / "port_runtime.h",
                  PORT / "main.c", PORT / "trace.c", PORT / "build.py"]
        newest_input = max(path.stat().st_mtime_ns for path in inputs)
        self.assertGreaterEqual(NATIVE_EXE.stat().st_mtime_ns, newest_input,
                                "SDL3 executable predates diagnostics inputs")

        with tempfile.TemporaryDirectory(prefix="stunts-sdl3-native-debug-") as temporary:
            temporary_root = Path(temporary)
            diagnostics_root = temporary_root / "diagnostics"
            environment = make_test_environment()

            def run_native(*extra: str, debug: bool = True) -> subprocess.CompletedProcess[str]:
                return subprocess.run(
                    [str(NATIVE_EXE), "--run-ms=250", "--audio=none",
                     f"--assets={ASSETS}", f"--diagnostics-dir={diagnostics_root}",
                     *(["--debug"] if debug else []), *extra],
                    cwd=temporary_root, env=environment, capture_output=True,
                    text=True, timeout=30, check=False)

            first = run_native()
            self.assertEqual(first.returncode, 0, first.stdout + first.stderr)
            sessions = sorted(diagnostics_root.glob("stunts-*"))
            self.assertEqual(len(sessions), 1, f"unexpected sessions: {sessions}")
            first_session = sessions[0]
            first_log = (first_session / "session.log").read_text(
                encoding="utf-8", errors="replace")
            self.assertEqual(int(read_report_field(first_log, "exit_status")),
                             first.returncode)
            build_id = read_report_field(first_log, "build_id")
            self.assertTrue(build_id.startswith("sdl3-"), build_id)
            default_trace = first_session / "trace.jsonl"
            self.assertTrue(default_trace.is_file())
            default_header = json.loads(default_trace.read_text(
                encoding="utf-8").splitlines()[0])
            self.assertEqual(default_header.get("build_id"), build_id)

            explicit_trace = temporary_root / "user-selected-trace.jsonl"
            second = run_native(f"--trace={explicit_trace}")
            self.assertEqual(second.returncode, 0, second.stdout + second.stderr)
            sessions = sorted(diagnostics_root.glob("stunts-*"))
            self.assertEqual(len(sessions), 2, f"expected a fresh session: {sessions}")
            second_session = next(session for session in sessions
                                  if session != first_session)
            second_log = (second_session / "session.log").read_text(
                encoding="utf-8", errors="replace")
            self.assertEqual(int(read_report_field(second_log, "exit_status")),
                             second.returncode)
            self.assertEqual(Path(read_report_field(second_log, "trace_path")),
                             explicit_trace)
            self.assertTrue(explicit_trace.is_file())
            explicit_header = json.loads(explicit_trace.read_text(
                encoding="utf-8").splitlines()[0])
            self.assertEqual(explicit_header.get("build_id"),
                             read_report_field(second_log, "build_id"))
            self.assertFalse((second_session / "trace.jsonl").exists(),
                             "--trace override was ignored")
            normal = run_native(debug=False)
            self.assertEqual(normal.returncode, 0, normal.stdout + normal.stderr)
            normal_session = next(session for session in diagnostics_root.glob("stunts-*")
                                  if session not in sessions)
            normal_log = (normal_session / "session.log").read_text()
            self.assertEqual(read_report_field(normal_log, "debug"), "0")
            self.assertEqual(read_report_field(normal_log, "exit_status"), "0")
            self.assertFalse((normal_session / "trace.jsonl").exists())
            self.assertFalse((normal_session / "crash.txt").exists())


if __name__ == "__main__":
    unittest.main()
