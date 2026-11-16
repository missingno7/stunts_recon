"""Collect GCC coverage and map executed native functions onto port-layer rows."""
from __future__ import annotations

import argparse
import ast
from collections import Counter, defaultdict
import gzip
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_GCOV = Path(r"C:\msys64\mingw32\bin\gcov.exe")
PORT_LAYER = ROOT / "tools" / "porting" / "port-layer.json"
PORT_DOC = ROOT / "docs" / "porting" / "port-layer.md"
CATEGORIES = (
    "executed_in_agreeing_original_scenarios",
    "executed_only_in_other_tests",
    "never_executed",
    "executed_in_original_scenarios_with_known_differences",
)


def normal_source(value: str) -> str:
    value = str(value).replace("\\", "/")
    for marker in ("/host-build/overlay/", "/overlay/"):
        if marker in value:
            tail = value.split(marker, 1)[1]
            if tail.startswith(("game/", "lib/")):
                return tail
    if "/boundary-overlay/" in value:
        return "port/" + value.split("/boundary-overlay/", 1)[1]
    root = ROOT.as_posix().rstrip("/") + "/"
    if value.lower().startswith(root.lower()):
        return value[len(root):]
    for prefix in ("tools/porting/", "port/", "game/", "lib/", "include/"):
        at = value.rfind("/" + prefix)
        if at >= 0:
            return value[at + 1:]
        if value.startswith(prefix):
            return value
    return value


def collect_gcov(build_dir: Path, output: Path, gcov: Path = DEFAULT_GCOV) -> dict:
    """Convert existing .gcda/.gcno pairs to a compact function/line profile."""
    build_dir = build_dir.resolve()
    output = output.resolve()
    if not gcov.is_file():
        raise FileNotFoundError(f"gcov not found: {gcov}")
    pairs = [(data, data.with_suffix(".gcno"))
             for data in build_dir.rglob("*.gcda")
             if data.with_suffix(".gcno").is_file()]
    if not pairs:
        raise RuntimeError(f"No GCC coverage data found below {build_dir}")
    gcov_dir = build_dir / "gcov-json" / output.stem
    if gcov_dir.exists():
        shutil.rmtree(gcov_dir)
    gcov_dir.mkdir(parents=True)
    merged: dict[str, dict] = {}
    errors = []
    for data, notes in pairs:
        result = subprocess.run(
            [str(gcov), "--json-format", "--long-file-names", "--preserve-paths",
             "--object-directory", str(data.parent), str(notes)],
            cwd=gcov_dir, capture_output=True, text=True, errors="replace", check=False)
        if result.returncode:
            errors.append((str(notes), result.stderr.strip() or result.stdout.strip()))
            continue
    json_files = sorted(gcov_dir.glob("*.gcov.json.gz"))
    for path in json_files:
        with gzip.open(path, "rt", encoding="utf-8") as stream:
            report = json.load(stream)
        for source_row in report.get("files", []):
            source = normal_source(source_row.get("file", ""))
            entry = merged.setdefault(source, {
                "physical_paths": [], "functions": {}, "lines": {},
            })
            physical = source_row.get("file")
            if physical and physical not in entry["physical_paths"]:
                entry["physical_paths"].append(physical)
            for function in source_row.get("functions", []):
                name = function.get("name")
                if not name:
                    continue
                current = entry["functions"].setdefault(name, {
                    "execution_count": 0,
                    "start_line": function.get("start_line", 0),
                    "end_line": function.get("end_line", function.get("start_line", 0)),
                })
                current["execution_count"] += int(function.get("execution_count", 0))
                current["start_line"] = min(current["start_line"],
                                             function.get("start_line", current["start_line"]))
                current["end_line"] = max(current["end_line"],
                                           function.get("end_line", current["end_line"]))
            for line in source_row.get("lines", []):
                number = str(line.get("line_number"))
                entry["lines"][number] = entry["lines"].get(number, 0) + int(line.get("count", 0))
    if not merged:
        detail = "\n".join(f"{path}: {message}" for path, message in errors[:12])
        raise RuntimeError(f"gcov produced no readable JSON reports below {build_dir}\n{detail}")
    profile = {
        "schema": "stunts-gcc-gcov-profile-v1",
        "build_dir": build_dir.relative_to(ROOT).as_posix()
            if build_dir.is_relative_to(ROOT) else str(build_dir),
        "gcov": str(gcov),
        "gcda_pairs": len(pairs),
        "gcov_reports": len(json_files),
        "gcov_errors": errors,
        "files": merged,
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(profile, indent=2) + "\n", encoding="utf-8")
    print(f"GCC coverage: {sum(len(f['functions']) for f in merged.values())} functions "
          f"in {len(merged)} source files; profile {output}")
    return profile


def function_index(profile: dict) -> dict[tuple[str, str], dict]:
    index = {}
    for source, file in profile.get("files", {}).items():
        for name, function in file.get("functions", {}).items():
            index[(source, name)] = function
    return index


def targets_for_name(index: dict, name: str, source: str | None = None) -> set[tuple[str, str]]:
    if not name:
        return set()
    name = name.lstrip("_")
    if source:
        source = normal_source(source)
        exact = (source, name)
        if exact in index:
            return {exact}
    found = {key for key in index if key[1].lstrip("_") == name}
    if not found:
        # Some gcov versions report a local clone suffix. Match only a clear
        # compiler suffix, never a prefix that is another source identifier.
        found = {key for key in index
                 if key[1].lstrip("_").split(".", 1)[0] == name}
    return found


def funcs_for_source(index: dict, prefix: str) -> set[tuple[str, str]]:
    prefix = prefix.rstrip("/")
    return {key for key in index
            if key[0] == prefix or key[0].startswith(prefix + "/")
            or key[0].startswith(prefix + ".")}


def files_under(profile: dict, prefixes: tuple[str, ...]):
    return [(source, row) for source, row in profile.get("files", {}).items()
            if source.startswith(prefixes)]


def functions_using_identifier(profile: dict, identifier: str) -> set[tuple[str, str]]:
    if not identifier or not re.fullmatch(r"[A-Za-z_]\w*", identifier):
        return set()
    pattern = re.compile(r"\b" + re.escape(identifier) + r"\b")
    targets = set()
    for source, file in profile.get("files", {}).items():
        ranges = []
        for name, function in file.get("functions", {}).items():
            ranges.append((name, int(function.get("start_line", 0)),
                           int(function.get("end_line", function.get("start_line", 0)))))
        paths = list(file.get("physical_paths", []))
        if not paths:
            paths = [str(ROOT / source)]
        for physical in paths:
            path = Path(physical)
            if not path.is_file():
                fallback = ROOT / source
                if fallback.is_file():
                    path = fallback
                else:
                    continue
            try:
                lines = path.read_text(encoding="latin-1", errors="replace").splitlines()
            except OSError:
                continue
            for line_number, line in enumerate(lines, 1):
                if not pattern.search(line):
                    continue
                for name, start, end in ranges:
                    if start <= line_number <= end:
                        targets.add((source, name))
    return targets


def python_function_source(source: str, function_name: str) -> str:
    path = ROOT / source
    if not path.is_file():
        return ""
    text = path.read_text(encoding="utf-8", errors="replace")
    try:
        tree = ast.parse(text)
    except SyntaxError:
        return ""
    for node in ast.walk(tree):
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) and node.name == function_name:
            return ast.get_source_segment(text, node) or ""
    return ""


def c_source_strings(text: str) -> set[str]:
    try:
        tree = ast.parse("_candidate = (" + text + ")")
    except SyntaxError:
        return set(re.findall(r"['\"]([^'\"]{1,100})['\"]", text))
    return {node.value for node in ast.walk(tree)
            if isinstance(node, ast.Constant) and isinstance(node.value, str)}


def source_guard_context(source: str, line: int) -> set[str]:
    path = ROOT / source
    if not path.is_file():
        return set()
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    start = max(0, line - 220)
    for row in reversed(lines[start:line]):
        match = re.search(r"(?:if|elif)\s+source\s*==\s*['\"]((?:game|lib)/[^'\"]+\.c)['\"]", row)
        if match:
            return {match.group(1)}
    return set()


def adapter_targets(row: dict, index: dict) -> tuple[set[tuple[str, str]], str]:
    portable = row.get("portable") or []
    source = portable[0].get("source", "") if portable else ""
    name = row.get("name", "")
    operation = row.get("operation") or {}
    expression = operation.get("expression", "")
    targets = set()
    for function_name in index:
        c_name = function_name[1].lstrip("_")
        if re.search(r"\b" + re.escape(c_name) + r"\b", expression):
            targets.add(function_name)
    if targets:
        return targets, "operation-names"

    source_text = python_function_source(source, name)
    if source_text and name != "build_game_objects":
        for literal in c_source_strings(source_text):
            for source_name, function_name in index:
                if re.search(r"\b" + re.escape(function_name.lstrip("_")) + r"\b", literal):
                    targets.add((source_name, function_name))
        if targets:
            return targets, "adapter-output-names"
        guarded_sources = set(re.findall(r"['\"]((?:game|lib)/[^'\"]+\.c)['\"]", source_text))
        for guarded in guarded_sources:
            targets.update(funcs_for_source(index, guarded))
        if targets:
            return targets, "adapter-source-units"

    if name == "build_game_objects":
        guarded_sources = source_guard_context(source, int(operation.get("line", 0)))
        for guarded in guarded_sources:
            targets.update(funcs_for_source(index, guarded))
        if targets:
            return targets, "guarded-source-unit"
        # Shared declaration/width transformations feed every generated C
        # translation unit and are therefore mapped to the generated functions.
        targets = funcs_for_source(index, "game") | funcs_for_source(index, "lib")
        return targets, "all-generated-game-units"

    if source.startswith(("tools/porting/host_probe_", "tools/porting/project_headers.py")):
        targets = funcs_for_source(index, "game") | funcs_for_source(index, "lib")
        return targets, "generated-game-declarations"
    unit_map = {
        ("port/build.py", "adapt_gameplay_word_arithmetic"): ("game/carsim.c",),
        ("port/build.py", "run"): ("game", "lib", "port"),
        ("port/game_abi.py", "adapt_aggregate_views"): ("game", "lib"),
        ("port/game_abi.py", "adapt_word_sentinels"): ("game", "lib"),
        ("port/game_abi.py", "adapt_preview_word_arithmetic"): ("game/render.c",),
        ("port/game_abi.py", "adapt_polygon_storage"): ("game/polygon.c",),
        ("port/game_abi.py", "adapt_wheel_update_inputs"): ("game/trkscene.c",),
        ("port/game_abi.py", "route_audio_vectors"): ("lib/audiores.c", "lib/audioply.c"),
        ("port/game_abi.py", "adapt"): ("lib/audiores.c", "lib/audioply.c"),
        ("port/game_abi.py", "edit"): ("game/frontend.c",),
        ("port/render/camera_adapter.py", "inject"): ("game/polygon.c",),
        ("port/render/legacy_adapter.py", "replace"): ("game/polygon.c",),
        ("port/render/tile_adapter.py", "_replace_once"): ("game/render.c",),
        ("port/render/tile_adapter.py", "adapt_update_frame"): ("game/render.c",),
        ("port/sound_adapter.py", "replace"): ("lib/audiores.c", "lib/audioply.c"),
    }
    affected_units = unit_map.get((source, name), ())
    if affected_units:
        for unit in affected_units:
            targets.update(funcs_for_source(index, unit))
        if targets:
            return targets, "generated-translation-unit"
    if source == "port/boundary_adapter.py":
        try:
            sys.path.insert(0, str(ROOT / "port"))
            from boundary_adapter import CONTRACTS, GRAPHICS_CONTRACTS
            for contract in CONTRACTS + GRAPHICS_CONTRACTS:
                targets.update(targets_for_name(index, contract["name"]))
        except (ImportError, KeyError, TypeError):
            pass
        if targets:
            return targets, "boundary-contract-functions"
    return set(), "no-generated-function-match"


def host_targets(row: dict, index: dict, profile: dict) -> tuple[set[tuple[str, str, int | None]], str]:
    name = row.get("name", "")
    direct = targets_for_name(index, name)
    if direct:
        return {(source, function, None) for source, function in direct}, "same-name-native-function"
    interface = row.get("interface", "")
    interrupt = re.search(r"INT\s*([0-9A-F]{1,2})h", interface, re.I)
    selector = re.search(r"AX\s*=\s*([0-9A-F]{1,4})h?", interface, re.I)
    if interrupt and selector:
        interrupt_number = int(interrupt.group(1), 16)
        value = int(selector.group(1), 16)
        platform = profile.get("files", {}).get("port/platform.c", {})
        code_path = next((Path(p) for p in platform.get("physical_paths", [])
                          if Path(p).is_file()), ROOT / "port" / "platform.c")
        try:
            source_lines = code_path.read_text(encoding="latin-1", errors="replace").splitlines()
        except OSError:
            source_lines = []
        if interrupt_number == 0x33:
            function = next(iter(targets_for_name(index, "int86", "port/platform.c")), None)
            if function:
                case = re.compile(r"\bcase\s+(?:0x)?0*" + format(value, "x") + r"\s*:", re.I)
                lines = [i for i, text in enumerate(source_lines, 1) if case.search(text)]
                if lines:
                    return {(function[0], function[1], line) for line in lines}, "INT-33-selector-branch"
        if interrupt_number == 0x15:
            function = next(iter(targets_for_name(index, "int86", "port/platform.c")), None)
            if function:
                needle = f"0x{value:04x}"
                lines = [i for i, text in enumerate(source_lines, 1) if needle in text.lower()]
                if lines:
                    return {(function[0], function[1], line) for line in lines}, "INT-15-selector-branch"
    try:
        services = json.loads((ROOT / "tools/porting/platform-services.json").read_text())
        subsystem = services.get("by_name", {}).get(name, [])
    except (OSError, json.JSONDecodeError):
        subsystem = []
    if isinstance(subsystem, str):
        subsystem = [subsystem]
    source_groups = {
        "audio": ("port/audio", "port/ad15_driver.c", "port/pc_speaker.c"),
        "file": ("port/file.c", "port/resource.c"),
        "input_kb": ("port/input.c", "port/input_script.c"),
        "input_mouse": ("port/input.c", "port/input_script.c"),
        "timer": ("port/timer.c", "port/tick_clock.c"),
        "video": ("port/video.c", "port/sprite.c", "port/render/"),
        "memory": ("port/memory.c", "port/resource.c"),
    }
    for service in subsystem:
        for prefix in source_groups.get(service, ()):
            found = funcs_for_source(index, prefix)
            if found:
                return {(source, function, None) for source, function in found}, f"host-service-{service}"
    return set(), "no-native-service-match"


def row_targets(row: dict, index: dict, profile: dict) -> tuple[set[tuple[str, str, int | None]], str]:
    kind = row.get("kind")
    if kind == "build_adapter":
        functions, mapping = adapter_targets(row, index)
        return {(source, name, None) for source, name in functions}, mapping
    if kind == "host":
        return host_targets(row, index, profile)
    if kind in ("semantic_macro", "semantic_data"):
        name = row.get("name", "")
        functions = functions_using_identifier(profile, name)
        if functions:
            return {(source, function, None) for source, function in functions}, "expanded-or-referenced-functions"
        return set(), "no-native-reference"
    names = {row.get("name", "")}
    if kind == "semantic_contract":
        names.update(row.get("routines") or [])
    names.update(p.get("name", "") for p in row.get("portable") or [])
    targets: set[tuple[str, str, int | None]] = set()
    mapping = "portable-function-name"
    for portable in row.get("portable") or []:
        source = portable.get("source")
        if source and source.endswith(".py"):
            continue
        for target in targets_for_name(index, portable.get("name", ""), source):
            targets.add((target[0], target[1], None))
    if not targets:
        for name in names:
            for target in targets_for_name(index, name):
                targets.add((target[0], target[1], None))
    return targets, mapping if targets else "no-portable-function-match"


def target_hits(profile: dict, target: tuple[str, str, int | None]) -> bool:
    source, name, line = target
    file = profile.get("files", {}).get(source)
    if not file:
        return False
    if line is not None:
        return int(file.get("lines", {}).get(str(line), 0)) > 0
    return int(file.get("functions", {}).get(name, {}).get("execution_count", 0)) > 0


def subsystem(row: dict) -> str:
    if row.get("kind") == "host":
        return row.get("subsystem") or "host services"
    if row.get("kind") == "asm":
        return row.get("category") or row.get("original_module") or "assembly routines"
    if row.get("kind") == "build_adapter":
        return "build adapters"
    if row.get("kind") == "semantic_contract":
        return "semantic contracts"
    source = next((p.get("source", "") for p in row.get("portable") or []), "")
    route = (source + " " + row.get("name", "")).lower()
    groups = (
        (("audio", "sound", "opl", "speaker", "ad15", "pc15"), "audio"),
        (("render", "polygon", "sprite", "font", "raster", "video", "camera", "tile", "shape"), "graphics"),
        (("input", "key", "mouse", "joystick"), "input"),
        (("file", "save", "load", "resource", "track", "replay"), "files and game data"),
        (("timer", "tick", "clock", "wait"), "timing"),
        (("memory", "alloc", "free", "heap"), "memory"),
        (("window", "sdl", "platform", "int86", "int33"), "platform"),
    )
    for tokens, name in groups:
        if any(token in route for token in tokens):
            return name
    if source.startswith("game/"):
        return "game logic"
    if source.startswith("lib/"):
        return "runtime library"
    if source.startswith("port/"):
        return source.removeprefix("port/").split("/", 1)[0].replace("_", " ")
    if row.get("kind") == "semantic_macro":
        return "port macros"
    if row.get("kind") == "semantic_data":
        return "runtime data"
    return "other"


def never_note(row: dict, mapping: str, targets: set[tuple[str, str, int | None]]) -> str:
    route = " ".join((row.get("name", ""), row.get("subsystem", ""),
                      row.get("original_module", ""),
                      " ".join(p.get("source", "") for p in row.get("portable") or []))).lower()
    if row.get("kind") == "build_adapter":
        target_names = {name for _, name, _ in targets}
        sample = ", ".join(sorted(target_names)[:3])
        if "ellipseh" in target_names:
            return "`ellipseh` builds a sphere perimeter, but the game has no active `solidellipseh` caller; a sphere-rendering path would be needed."
        if "audio_set_chunk_callback" in target_names:
            return "Reach it when AD15/PC15 installs an audio chunk callback; exercise the longer music/results transition path."
        if "port_guest_exit" in target_names:
            return "Reach it on the guest fatal-startup or audio-initialization error path; successful scenarios do not request a nonzero exit."
        if sample:
            target_sources = {source for source, _, _ in targets}
            if any(source in ("game/render.c", "game/polygon.c") for source in target_sources):
                return f"Generated functions ({sample}) need a matching race/menu render scene, including the relevant clip or transition."
            if any(source.startswith("lib/audio") for source in target_sources):
                return f"Generated functions ({sample}) need AD15/PC15 playback through the chunk/driver path."
            if "game/carsim.c" in target_sources or "game/race.c" in target_sources:
                return f"Generated functions ({sample}) need the corresponding driving, collision, or replay action in a race."
            if "game/frontend.c" in target_sources or "game/uires.c" in target_sources:
                return f"Generated functions ({sample}) need the frontend, dialog, or text-input path that calls them."
            if any(token in route for token in ("modern", "wide", "wheel", "tile")):
                return f"Generated functions ({sample}) need --renderer=modern and a race scene with the corresponding geometry."
            if any(token in route for token in ("input", "idle", "wait", "line")):
                return f"Generated functions ({sample}) need menu/text input, replay seeking, or a host-idle wait."
            if any(token in route for token in ("audio", "sound")):
                return f"Generated functions ({sample}) need AD15/PC15 playback in a race or results sequence."
            return f"Generated functions ({sample}) need the caller path in startup, a menu, a race, or an error case."
        if "word_arithmetic" in route or "collision" in route:
            return "Reach the generated word-arithmetic helpers during a car collision or wall-hit race path."
        if "tile" in route or "update_frame" in route:
            return "Reach the generated frame path with --renderer=modern during a race scene."
        return "The Python rewrite has no mapped generated runtime function; inspect the named overlay before adding a caller case."
    if row.get("kind") == "semantic_macro":
        sample = ", ".join(sorted({name for _, name, _ in targets})[:3])
        if sample:
            return f"Expansion in ({sample}) needs its caller scenario; use the menu, race, or hardware path named by the subsystem."
        return "This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions."
    if row.get("kind") == "semantic_data":
        return "A caller that reads the ctype table must run; ordinary text parsing or case conversion reaches it."
    if row.get("kind") == "host" and not targets:
        service = row.get("interface", "").lower()
        subsystem_name = row.get("subsystem", "").lower()
        if subsystem_name == "dos.exception":
            return "Only a divide-by-zero or critical-error path reaches this DOS handler; ordinary menu/race play does not trigger it."
        if subsystem_name == "dos.file":
            return "Reach it through track/replay load or save; a missing file or failed write exercises its error path."
        if subsystem_name == "dos.memory":
            return "Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path."
        if subsystem_name == "dos.runtime":
            return "Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper."
        if subsystem_name.startswith("input."):
            return "Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race."
        if subsystem_name.startswith("video.") or subsystem_name in ("legacy_software_renderer", "platform"):
            return "Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene."
        if subsystem_name.startswith("timer."):
            return "Reach it from the matching timer/audio callback or hardware timing option."
        if "int 33" in service or "mouse" in route:
            return "Reach it with menu/race mouse input or the matching INT 33h function; this service has no mapped native branch."
        if "int 21" in service or "file" in route or "save" in route:
            return "Reach it through track/replay load or save; a missing file reaches its error path."
        if "timer" in route or "pit" in service:
            return "Reach it during timer callbacks or an explicit manual-clock wait."
        if "sound" in route or "audio" in route:
            return "Reach it with AD15/PC15 playback or the matching audio hardware option."
        return "The old DOS service has no mapped native entry; its scenario requires a direct service adapter."
    if row.get("kind") == "asm" and not targets:
        if row.get("category") == "HISTORICAL_CRT-DEAD-NOP":
            return "No native semantic mapping is established. The mixed CRT/dead/no-op category does not prove guest unreachability; compare the original entry with its host replacement."
        return "No native implementation mapping is established; inspect the accepted public entry and its host replacement before choosing a routine probe."
    if any(token in route for token in ("modern", "wheel")):
        return "Reach it with --renderer=modern during a race scene containing the corresponding wheel geometry."
    if any(token in route for token in ("file", "save", "load", "resource")):
        return "Reach it from track/replay load or save; a missing asset exercises the error path."
    if any(token in route for token in ("editor", "track")):
        return "Reach it from the track editor by loading, placing/removing, or saving track data."
    if any(token in route for token in ("replay", "seek")):
        return "Reach it after entering replay, then pause, seek, or save the replay."
    if any(token in route for token in ("audio", "sound", "speaker", "opl", "driver")):
        return "Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option."
    if any(token in route for token in ("input", "key", "mouse", "joystick")):
        return "Reach it with the named keyboard, mouse, or joystick path in menus or a race."
    if any(token in route for token in ("timer", "tick", "clock", "wait")):
        return "Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race."
    if any(token in route for token in ("render", "polygon", "sprite", "font", "video", "shape")):
        return "Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene."
    reach = row.get("reachability", {})
    candidate = reach.get("scenario")
    if candidate:
        return f"Candidate route: {candidate}; the required caller or operand case is not present in the selected runs."
    return "No selected scenario reached this mapped function; inspect its caller and add the matching menu/error/hardware case."


def annotate(layer_path: Path, original_path: Path, other_path: Path,
             docs_path: Path = PORT_DOC) -> dict:
    layer = json.loads(layer_path.read_text(encoding="utf-8"))
    original = json.loads(original_path.read_text(encoding="utf-8"))
    other = json.loads(other_path.read_text(encoding="utf-8"))
    original_index = function_index(original)
    other_index = function_index(other)
    combined_index = dict(original_index)
    combined_index.update(other_index)
    original_passed = bool(original.get("scenario_tests_passed", True))
    kind_counts: dict[str, Counter] = defaultdict(Counter)
    subsystem_counts: Counter = Counter()
    never_groups: dict[str, list[dict]] = defaultdict(list)
    for row in layer["rows"]:
        targets, mapping = row_targets(row, combined_index, original)
        original_hits = sorted(t for t in targets if target_hits(original, t))
        other_hits = sorted(t for t in targets if target_hits(other, t))
        if original_hits and row.get("status") == "exercised and differs":
            classification = CATEGORIES[3]
        elif original_hits and original_passed:
            classification = CATEGORIES[0]
        elif other_hits:
            classification = CATEGORIES[1]
        else:
            classification = CATEGORIES[2]
        group = subsystem(row)
        kind = row.get("kind", "unknown")
        kind_counts[kind][classification] += 1
        detail = {
            "classification": classification,
            "mapping": mapping,
            "mapped_function_count": len(targets),
            "original_compared_functions": [f"{s}::{n}" for s, n, _ in original_hits[:20]],
            "other_test_functions": [f"{s}::{n}" for s, n, _ in other_hits[:20]],
            "agreement_scope": "selected original-reference test assertions only; no whole-domain claim",
        }
        if classification == CATEGORIES[2]:
            note = never_note(row, mapping, targets)
            detail["never_executed_note"] = note
            never_groups[group].append({"id": row["id"], "name": row.get("name", ""),
                                        "kind": kind, "note": note})
            subsystem_counts[group] += 1
        row["execution_coverage"] = detail

    total_counts = Counter()
    for counts in kind_counts.values():
        total_counts.update(counts)
    never_groups = {key: sorted(rows, key=lambda row: row["id"])
                    for key, rows in sorted(never_groups.items())}
    summary = {
        "schema": "stunts-port-layer-execution-coverage-v1",
        "method": "GCC --coverage function execution mapped to native replacements, generated overlay functions/translation units, and macro users",
        "scenario_tests_passed": original_passed,
        "agreement_scope": "A row is in the agreeing bucket when its mapped code ran during the selected original-reference tests and those asserted comparisons passed. Rows whose existing status records a known difference are counted separately. This does not prove every row's full domain.",
        "profiles": {
            "original_compared": original_path.relative_to(ROOT).as_posix()
                if original_path.is_relative_to(ROOT) else str(original_path),
            "other_port_tests": other_path.relative_to(ROOT).as_posix()
                if other_path.is_relative_to(ROOT) else str(other_path),
        },
        "counts": {category: total_counts.get(category, 0) for category in CATEGORIES},
        "by_kind": {kind: {category: counts.get(category, 0) for category in CATEGORIES}
                    for kind, counts in sorted(kind_counts.items())},
        "never_by_subsystem": {key: len(rows) for key, rows in never_groups.items()},
        "never_rows": never_groups,
    }
    layer.pop("prior_clock_audit", None)
    layer["execution_coverage"] = summary
    layer_path.write_text(json.dumps(layer, indent=2, ensure_ascii=False) + "\n",
                          encoding="utf-8", newline="\n")
    if docs_path.exists():
        docs = docs_path.read_text(encoding="utf-8")
        marker = "## Measured execution coverage"
        if marker in docs:
            docs = docs.split(marker, 1)[0].rstrip() + "\n"
        docs = docs.rstrip() + "\n\n" + render_markdown(summary, never_groups)
        docs_path.write_text(docs, encoding="utf-8", newline="\n")
    print(json.dumps({"counts": summary["counts"], "by_kind": summary["by_kind"],
                      "never_by_subsystem": summary["never_by_subsystem"]}, indent=2))
    return layer


def render_markdown(summary: dict, never_groups: dict[str, list[dict]]) -> str:
    lines = [
        "## Measured execution coverage",
        "",
        "The SDL3 executable was built with GCC `--coverage` in a separate build directory. "
        "The first profile covers the original-reference scenario tests; the second covers "
        "the remaining port tests. GCC function counts are joined to each row's portable "
        "function, generated overlay target or affected translation unit, or macro expansion "
        "users. `status` above remains "
        "the original observation status.",
        "",
        summary["agreement_scope"],
        "",
        "| Port-layer kind | Agreeing original scenarios | Other tests only | Never executed | Original runs with known differences |",
        "|---|---:|---:|---:|---:|",
    ]
    for kind, counts in summary["by_kind"].items():
        lines.append(f"| `{kind}` | {counts[CATEGORIES[0]]} | {counts[CATEGORIES[1]]} | "
                     f"{counts[CATEGORIES[2]]} | {counts[CATEGORIES[3]]} |")
    counts = summary["counts"]
    lines += [f"| **Total ({sum(counts.values())})** | {counts[CATEGORIES[0]]} | "
              f"{counts[CATEGORIES[1]]} | {counts[CATEGORIES[2]]} | {counts[CATEGORIES[3]]} |", ""]
    if never_groups:
        lines += ["Never-executed rows are grouped below. Each note names a likely way to "
                  "reach the path or explains when the native game has no corresponding entry.", ""]
        for group, rows in never_groups.items():
            lines.append(f"### {group} ({len(rows)})")
            lines.append("")
            for row in rows:
                lines.append(f"- `{row['id']}` — {row['note']}")
            lines.append("")
    else:
        lines += ["No rows mapped to unexecuted functions in these runs.", ""]
    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    collect = sub.add_parser("collect", help="convert GCC .gcda files to a JSON profile")
    collect.add_argument("--build-dir", type=Path, required=True)
    collect.add_argument("--out", type=Path, required=True)
    collect.add_argument("--gcov", type=Path, default=DEFAULT_GCOV)
    annotate_parser = sub.add_parser("annotate", help="map two profiles into port-layer.json and docs")
    annotate_parser.add_argument("--layer", type=Path, default=PORT_LAYER)
    annotate_parser.add_argument("--original-profile", type=Path, required=True)
    annotate_parser.add_argument("--other-profile", type=Path, required=True)
    annotate_parser.add_argument("--docs", type=Path, default=PORT_DOC)
    args = parser.parse_args()
    if args.command == "collect":
        collect_gcov(args.build_dir, args.out, args.gcov)
    else:
        annotate(args.layer, args.original_profile, args.other_profile, args.docs)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
