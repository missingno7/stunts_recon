"""Content-addressed build artifacts for test-only native harnesses."""
from __future__ import annotations

from contextlib import contextmanager
from functools import lru_cache
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
CACHE_ROOT = ROOT / "build" / "validation" / "fixture-cache"
RAW_RUN = subprocess.run
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".s", ".S", ".i", ".ii", ".asm"}
INPUT_SUFFIXES = SOURCE_SUFFIXES | {
    ".h", ".hpp", ".inc", ".def", ".o", ".obj", ".a", ".lib", ".dll", ".res"
}
HEADER_SUFFIXES = {".h", ".hpp", ".inc", ".def"}
LIBRARY_SUFFIXES = {".a", ".lib", ".dll", ".dll.a"}
def _sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _file_identity(path: Path) -> dict[str, object]:
    data = path.read_bytes()
    return {"path": str(path.resolve()), "size": len(data), "sha256": _sha(data)}


def _directory_identity(path: Path, suffixes: set[str]) -> dict[str, object]:
    path = path.resolve()
    files = []
    if not path.is_dir():
        return {"path": str(path), "missing": True, "files": []}
    for candidate in sorted(path.rglob("*")):
        if candidate.is_file() and candidate.suffix.lower() in {s.lower() for s in suffixes}:
            data = candidate.read_bytes()
            files.append({"relative": candidate.relative_to(path).as_posix(),
                          "size": len(data), "sha256": _sha(data)})
    return {"path": str(path), "files": files}


@lru_cache(maxsize=32)
def _cached_tool_identity(executable: str, cwd_string: str, path_value: str | None,
                          executable_size: int, executable_mtime_ns: int,
                          executable_sha256: str) -> dict[str, object]:
    cwd = Path(cwd_string)
    env = dict(os.environ)
    if path_value is not None:
        env["PATH"] = path_value
    candidate = Path(executable)
    commands = {}
    for option in ("--version", "-dumpmachine", "-print-sysroot", "-print-search-dirs"):
        try:
            result = RAW_RUN([str(candidate), option], cwd=cwd, env=env,
                             capture_output=True, text=True, errors="replace", check=False,
                             timeout=15)
            commands[option] = [result.returncode, result.stdout, result.stderr]
        except (OSError, subprocess.SubprocessError) as error:
            commands[option] = [type(error).__name__, str(error)]
    return {"executable": {"path": str(candidate), "size": executable_size,
                            "sha256": executable_sha256}, "probes": commands}


def _tool_identity(executable: str | os.PathLike[str], cwd: Path,
                   env: dict[str, str] | None) -> dict[str, object]:
    candidate = Path(executable)
    if not candidate.is_absolute():
        found = shutil.which(str(executable), path=(env or os.environ).get("PATH"))
        if found:
            candidate = Path(found)
        else:
            candidate = (cwd / candidate).resolve()
    candidate = candidate.resolve()
    if not candidate.is_file():
        return {"path": str(candidate), "missing": True}
    stat = candidate.stat()
    executable_sha256 = _sha(candidate.read_bytes())
    return _cached_tool_identity(str(candidate), str(cwd),
                                 (os.environ if env is None else env).get("PATH"),
                                 stat.st_size, stat.st_mtime_ns, executable_sha256)


def _output_argument(command: list[str]) -> tuple[int, Path] | None:
    found = []
    index = 0
    while index < len(command):
        argument = command[index]
        if argument == "-o" and index + 1 < len(command):
            found.append((index + 1, Path(command[index + 1])))
            index += 2
            continue
        if argument.startswith("-o") and len(argument) > 2:
            found.append((index, Path(argument[2:])))
        index += 1
    return found[0] if len(found) == 1 else None


def _resolved(path: Path, cwd: Path) -> Path:
    return path.resolve() if path.is_absolute() else (cwd / path).resolve()


def _is_gcc(command: list[str]) -> bool:
    if not command:
        return False
    stem = Path(command[0]).name.lower()
    return stem in {"gcc", "gcc.exe", "i686-w64-mingw32-gcc", "i686-w64-mingw32-gcc.exe"}


def _fingerprint(command: list[str], output_index: int, output: Path,
                 cwd: Path, env: dict[str, str] | None,
                 extra_inputs: list[Path] | None = None) -> str:
    effective_env = os.environ if env is None else env
    normalized = []
    inputs: dict[str, Path] = {str(path.resolve()): path.resolve() for path in (extra_inputs or [])}
    include_dirs: dict[str, Path] = {}
    library_dirs: dict[str, Path] = {}
    source_dirs: dict[str, Path] = {}
    index = 0
    while index < len(command):
        argument = command[index]
        if argument == "-o" and index + 1 == output_index:
            normalized.extend(("-o", "<artifact>"))
            index += 2
            continue
        if index == output_index and argument.startswith("-o") and argument != "-o":
            normalized.append("-o<artifact>")
            index += 1
            continue
        flag = None
        value = None
        if argument in ("-I", "-isystem", "-iquote", "-L", "-include", "-imacros") and index + 1 < len(command):
            flag, value = argument, command[index + 1]
            index += 2
        else:
            for prefix in ("-I", "-L"):
                if argument.startswith(prefix) and len(argument) > len(prefix):
                    flag, value = prefix, argument[len(prefix):]
                    index += 1
                    break
            if flag is None:
                index += 1
        if flag is not None:
            path = _resolved(Path(value), cwd)
            normalized.extend((flag, str(path)))
            if flag in ("-I", "-isystem", "-iquote"):
                include_dirs[str(path)] = path
            elif flag == "-L":
                library_dirs[str(path)] = path
            elif path.is_file():
                inputs[str(path)] = path
            continue
        path = _resolved(Path(argument), cwd)
        if path.is_file() and path.suffix in INPUT_SUFFIXES:
            normalized.append(str(path))
            inputs[str(path)] = path
            if path.suffix in SOURCE_SUFFIXES:
                source_dirs[str(path.parent)] = path.parent
        elif path.is_dir() and argument not in (".", ".."):
            normalized.append(str(path))
        else:
            normalized.append(argument)

    file_rows = [_file_identity(path) for path in sorted(inputs.values(), key=lambda p: str(p))
                 if path.is_file()]
    include_rows = [_directory_identity(path, HEADER_SUFFIXES)
                    for path in sorted(include_dirs.values(), key=lambda p: str(p))]
    include_rows.extend(_directory_identity(path, HEADER_SUFFIXES)
                        for path in sorted(source_dirs.values(), key=lambda p: str(p)))
    library_rows = [_directory_identity(path, LIBRARY_SUFFIXES)
                    for path in sorted(library_dirs.values(), key=lambda p: str(p))]
    environment = sorted(effective_env.items())
    tool = _tool_identity(command[0], cwd, env)
    payload = {"schema": 1, "command": normalized, "cwd": str(cwd),
               "environment": environment, "tool": tool, "inputs": file_rows,
               "include_dirs": include_rows, "library_dirs": library_rows,
               "output_suffix": output.suffix.lower()}
    return _sha(json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8"))


def _cached_paths(key: str, output: Path) -> tuple[Path, Path]:
    directory = CACHE_ROOT / key[:2] / key
    return directory / ("artifact" + output.suffix.lower()), directory / "metadata.json"


def _metadata(artifact: Path, metadata_path: Path, key: str) -> dict[str, object] | None:
    try:
        value = json.loads(metadata_path.read_text(encoding="utf-8"))
        data = artifact.read_bytes()
    except (OSError, ValueError):
        return None
    if (value.get("key") != key or value.get("size") != len(data)
            or value.get("sha256") != _sha(data)):
        return None
    return value


@contextmanager
def _locked(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+b") as stream:
        stream.seek(0, os.SEEK_END)
        if stream.tell() == 0:
            stream.write(b"0")
            stream.flush()
        stream.seek(0)
        if os.name == "nt":
            import msvcrt
            while True:
                try:
                    msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
                    break
                except OSError:
                    time.sleep(0.05)
            try:
                yield
            finally:
                stream.seek(0)
                msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)
        else:
            import fcntl
            fcntl.flock(stream.fileno(), fcntl.LOCK_EX)
            try:
                yield
            finally:
                fcntl.flock(stream.fileno(), fcntl.LOCK_UN)


def _store_metadata(path: Path, value: dict[str, object]) -> None:
    temporary = path.with_name(path.name + f".{os.getpid()}.tmp")
    temporary.write_text(json.dumps(value, sort_keys=True), encoding="utf-8")
    os.replace(temporary, path)


def _copy_to(source: Path, destination: Path) -> None:
    destination = destination.resolve()
    source = source.resolve()
    if source == destination:
        return
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = destination.with_name(f".{destination.name}.{os.getpid()}.tmp")
    try:
        shutil.copyfile(source, temporary)
        os.replace(temporary, destination)
    finally:
        temporary.unlink(missing_ok=True)


def build_cached(command: list[str], output: str | os.PathLike[str], *,
                 inputs: list[str | os.PathLike[str]], cwd: str | os.PathLike[str] = ROOT,
                 env: dict[str, str] | None = None,
                 runner=RAW_RUN) -> tuple[Path | None, subprocess.CompletedProcess]:
    """Build one native fixture at an immutable path keyed by all build inputs."""
    working = Path(cwd).resolve()
    output_path = _resolved(Path(output), working)
    found = _output_argument(command)
    if found is None:
        raise ValueError("cached build needs one -o output")
    output_index, command_output = found
    if _resolved(command_output, working) != output_path:
        raise ValueError("cached build output does not match command -o")
    key = _fingerprint(command, output_index, output_path, working, env,
                       [_resolved(Path(path), working) for path in inputs])
    artifact, metadata_path = _cached_paths(key, output_path)
    lock_path = artifact.with_suffix(artifact.suffix + ".lock")
    with _locked(lock_path):
        metadata = _metadata(artifact, metadata_path, key)
        if metadata is not None:
            completed = subprocess.CompletedProcess(
                command, int(metadata.get("returncode", 0)),
                metadata.get("stdout"), metadata.get("stderr"))
            return artifact, completed
        artifact.parent.mkdir(parents=True, exist_ok=True)
        temporary = artifact.with_name(f".artifact.{os.getpid()}.tmp{artifact.suffix}")
        rewritten = list(command)
        if command[output_index - 1] == "-o":
            rewritten[output_index] = str(temporary)
        else:
            rewritten[output_index] = "-o" + str(temporary)
        kwargs = {"cwd": working, "env": env, "capture_output": True, "text": True,
                  "errors": "replace", "check": False}
        completed = runner(rewritten, **kwargs)
        if completed.returncode != 0 or not temporary.is_file():
            temporary.unlink(missing_ok=True)
            return None, subprocess.CompletedProcess(command, completed.returncode,
                                                      completed.stdout, completed.stderr)
        os.replace(temporary, artifact)
        data = artifact.read_bytes()
        metadata = {"key": key, "size": len(data), "sha256": _sha(data),
                    "returncode": completed.returncode, "stdout": completed.stdout,
                    "stderr": completed.stderr}
        _store_metadata(metadata_path, metadata)
        return artifact, subprocess.CompletedProcess(command, completed.returncode,
                                                       completed.stdout, completed.stderr)


def _cacheable_command(args, kwargs) -> tuple[list[str], int, Path] | None:
    if isinstance(args, (str, bytes)) or not isinstance(args, (list, tuple)):
        return None
    command = [os.fspath(argument) for argument in args]
    if not _is_gcc(command) or "-fsyntax-only" in command or "-E" in command or "-S" in command:
        return None
    if (kwargs.get("capture_output") is not True
            or not (kwargs.get("text") or kwargs.get("universal_newlines"))
            or "stdout" in kwargs or "stderr" in kwargs or kwargs.get("input") is not None):
        return None
    found = _output_argument(command)
    if found is None:
        return None
    output_index, output_arg = found
    suffix = output_arg.suffix.lower()
    if suffix not in {".exe", ".dll", ".o", ".obj", ".a", ".lib"}:
        return None
    working = Path(kwargs.get("cwd") or os.getcwd()).resolve()
    output = _resolved(output_arg, working)
    if not any(_resolved(Path(argument), working).is_file()
               and Path(argument).suffix in SOURCE_SUFFIXES for argument in command):
        return None
    return command, output_index, output


def cached_compiler_run(original_run, args, *positional, **kwargs):
    found = _cacheable_command(args, kwargs)
    if found is None:
        return original_run(args, *positional, **kwargs)
    command, output_index, output = found
    cwd = Path(kwargs.get("cwd") or os.getcwd()).resolve()
    env = kwargs.get("env")
    cache_command = list(command)
    cache_command[output_index] = str(output)
    artifact, completed = build_cached(
        cache_command, output, inputs=[], cwd=cwd, env=env,
        runner=lambda rewritten, **build_kwargs: original_run(
            rewritten, *positional, **{**kwargs, **build_kwargs, "check": False}))
    if completed.returncode == 0 and artifact is not None:
        _copy_to(artifact, output)
    result = subprocess.CompletedProcess(args, completed.returncode,
                                         completed.stdout, completed.stderr)
    if kwargs.get("check") and result.returncode:
        raise subprocess.CalledProcessError(result.returncode, args,
                                            output=result.stdout, stderr=result.stderr)
    return result


UNCACHED_MODULES = {
    # These cases intentionally exercise their compiler or artifact builder.
    "test_host_probe", "test_port_header_layout", "test_port_semantic_audit",
    "test_sdl3_audio_layout", "test_sdl3_dependencies", "test_matrix_machine_semantics",
    "test_toolcache",
}


@contextmanager
def compiler_cache(module: str):
    previous = subprocess.run
    if module not in UNCACHED_MODULES:
        subprocess.run = lambda args, *positional, **kwargs: cached_compiler_run(
            RAW_RUN, args, *positional, **kwargs)
    try:
        yield
    finally:
        subprocess.run = previous


def install_compiler_cache(module: str) -> None:
    """Install for a worker process that exits after its one module."""
    if module not in UNCACHED_MODULES:
        subprocess.run = lambda args, *positional, **kwargs: cached_compiler_run(
            RAW_RUN, args, *positional, **kwargs)

