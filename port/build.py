#!/usr/bin/env python3
"""Build and run the PORT_BUILD SDL3 host executable on Windows."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "sdl3"
DEFAULT_GCC = Path(r"C:\msys64\mingw32\bin\gcc.exe")
DEFAULT_SDL = Path(r"C:\tools\sdl3-3.4.16-i686")
DEFAULT_ASSETS = Path(r"D:\Prog\stunts_recon\assets")
DATA_STUB_BYTES = 65536


def checked_run(command: list[str], *, cwd: Path = ROOT,
                quiet: bool = False) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True,
                            errors="replace", check=False)
    if result.returncode != 0:
        if result.stdout:
            print(result.stdout, end="")
        if result.stderr:
            print(result.stderr, end="", file=sys.stderr)
        if not quiet:
            raise RuntimeError(f"command failed with exit {result.returncode}: {command[0]}")
    return result


def prepare_environment(gcc: Path) -> None:
    mingw_bin = str(gcc.resolve().parent)
    os.environ["PATH"] = mingw_bin + os.pathsep + os.environ.get("PATH", "")


def build_game_objects(gcc: Path) -> tuple[list[Path], dict[str, object]]:
    sys.path.insert(0, str(ROOT / "tools" / "porting"))
    import host_probe_declarations as declarations
    import host_probe_modes as probe

    probe.ROOT = ROOT
    probe.PORT = ROOT / "tools" / "porting" / "port_include"
    probe.HOST = ROOT / "tools" / "porting" / "host"
    probe.WORK = BUILD / "host-build"
    probe.GCC = gcc.resolve()
    probe.STRICT_CENTRAL = False
    declarations.ROOT = ROOT
    probe.FLAGS = list(probe.FLAGS) + [
        # GCC 16 diagnoses legacy source-only ABI views as hard errors by
        # default. They remain visible as warnings in the build report.
        "-Wno-error=implicit-int",
        "-Wno-error=int-conversion",
        "-Wno-error=incompatible-pointer-types",
    ]
    work = probe.WORK
    (work / "overlay" / "src").mkdir(parents=True, exist_ok=True)
    (work / "config").mkdir(parents=True, exist_ok=True)
    (work / "out").mkdir(parents=True, exist_ok=True)
    manifest = json.loads((ROOT / "layout" / "manifest.json").read_text(encoding="utf-8"))
    sources = declarations.collect_sources(manifest)
    shared_tags = set(json.loads((probe.PORT / "declaration-evidence.json").read_text(
        encoding="utf-8")).get("shared_struct_tags", []))
    if len(sources) != 38:
        raise RuntimeError(f"Expected the frozen 38-C-source port baseline; found {len(sources)}")

    rows: list[dict[str, object]] = []
    objects: list[Path] = []
    for index, (source, owner) in enumerate(sources.items(), start=1):
        text = (ROOT / source).read_text(encoding="latin-1")
        config, local_fns, local_data = probe.local_config(source, text, owner)
        if source == "src/obj_seg006.c":
            cfg = config.read_text(encoding="latin-1")
            cfg = cfg.replace("vector_op_unk2(struct VECTOR* vec);",
                              "extern I16 vector_op_unk2(struct VECTOR* vec);")
            config.write_text(cfg, encoding="latin-1")
        transformed, removed = probe.transformed_source(
            source, text, local_fns, local_data, shared_tags)
        if source == "src/obj_seg000.c":
            transformed = (
                "extern int port_input_test_auto_protection_enabled(void);\n"
                "extern int port_input_type_test_text(const char *text);\n"
                "extern void port_test_random_wait_begin(void);\n"
                "extern void port_test_random_wait_end(void);\n" + transformed)
            random_wait_call = "          random_wait();\n          if (pass_check_flag == 0)"
            if transformed.count(random_wait_call) != 1:
                raise RuntimeError("Could not locate the startup random_wait boundary")
            transformed = transformed.replace(
                random_wait_call,
                "          port_test_random_wait_begin();\n"
                "          random_wait();\n"
                "          port_test_random_wait_end();\n"
                "          if (pass_check_flag == 0)", 1)
            answer_read = (
                "        if (port_input_test_auto_protection_enabled())\n"
                "            port_input_type_test_text(resbuftext);\n"
                "        call_read_line(userInput, textLength, points[0].x, points[0].y, 30000);")
            if transformed.count(
                    "        call_read_line(userInput, textLength, points[0].x, points[0].y, 30000);") != 1:
                raise RuntimeError("Could not locate the original protection line-editor call")
            transformed = transformed.replace(
                "        call_read_line(userInput, textLength, points[0].x, points[0].y, 30000);",
                answer_read, 1)
            transformed, count = re.subn(
                r"(?m)^(\s*)(?:int|I16)\s+main(?=\s*\()",
                r"\1int stunts_game_main",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not generate the host main-entry adapter")
        elif source == "src/obj_seg008.c":
            transformed = "extern I16 port_random_test_rand(void);\n" + transformed
            match = re.search(
                r"(?ms)^I16\s+get_super_random\s*\(\s*void\s*\)\s*\{.*?^\}",
                transformed)
            if match is None:
                raise RuntimeError("Could not locate get_super_random for the test-seed adapter")
            body = match.group(0)
            if body.count("rand()") != 1:
                raise RuntimeError("Expected one rand call in get_super_random")
            transformed = (transformed[:match.start()] +
                           body.replace("rand()", "port_random_test_rand()", 1) +
                           transformed[match.end():])
        elif source == "src/obj_seg001_complete.c":
            transformed, count = re.subn(
                r"(?m)^void\s+update_gamestate\s*\(\s*\)\s*\{",
                "void stunts_update_gamestate(void) {",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not locate update_gamestate for the trace wrapper")
            transformed += (
                "\n/* Port-only state observation at the accepted GAMESTATE view. */\n"
                "uint16_t port_game_frame_snapshot(void) { "
                "return (uint16_t)core.game_frame; }\n"
                "size_t port_game_state_copy(void *output, size_t capacity) { "
                "size_t i; if (output == NULL || capacity < sizeof(core)) "
                "return 0; for (i = 0; i < sizeof(core); ++i) "
                "((unsigned char *)output)[i] = ((const unsigned char *)&core)[i]; "
                "return sizeof(core); }\n"
                "uint8_t port_game_mode_snapshot(void) { return (uint8_t)gm_playmode; }\n"
                "uint8_t port_game_inputmode_snapshot(void) { "
                "return (uint8_t)core.game_inputmode; }\n"
                "uint8_t port_game_replaymode_snapshot(void) { "
                "return (uint8_t)inrepflg; }\n"
                "uint16_t port_game_rate_snapshot(void) { "
                "return (uint16_t)rate_frame; }\n"
            )
        elif source == "src/obj_seg027.c":
            transformed, count = re.subn(
                r"(?m)^I16\s+(?:FAR\s+)?audio_load_driver\s*\(",
                "I16 FAR stunts_audio_load_driver_legacy(",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not isolate the original 16-bit audio loader")
            transformed = "extern int port_audio_is_silent(void);\n" + transformed
            transformed, count = re.subn(
                r"(?m)^(void\s+FAR\s+load_audio_finalize\s*\([^\n]*\)\s*\{)",
                r"\1\n    /* The host adapter selects silence; retain loaded resources but do not "
                "call the DOS driver image. */\n    if (port_audio_is_silent()) return;",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not isolate the audio finalizer boundary")
            transformed, count = re.subn(
                r"(?m)^(void\s+FAR\s+audio_driver_func3F\s*\([^\n]*\)\s*\{)",
                r"\1\n    /* The silent host adapter has no DOS driver entry points to dispatch. */\n"
                "    if (port_audio_is_silent()) return;",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not isolate the audio volume/cleanup boundary")
        elif source == "src/obj_seg006.c":
            transformed = transformed.replace(
                "vector_op_unk2(struct VECTOR*);",
                "I16 vector_op_unk2(struct VECTOR*);")
            transformed = transformed.replace(
                "vector_op_unk2(struct VECTOR* vec) {",
                "I16 vector_op_unk2(struct VECTOR* vec) {")
        elif source == "src/obj_seg032_group.c":
            old = "timer_copy_counter(timerOffset, timerSegment);"
            if transformed.count(old) != 2:
                raise RuntimeError("Could not locate both line-editor timer deadline calls")
            transformed = (
                "extern void port_timer_copy_counter_words(U16 ticks_low, "
                "U16 ticks_high);\n" + transformed)
            transformed = transformed.replace(
                old, "port_timer_copy_counter_words((U16)timerOffset, (U16)timerSegment);")
        elif source == "src/obj_seg004.c":
            old = "camData = (struct VECTOR *) blk[sub].cameraOverlay.cameraOffsetOverride;"
            new = (
                "camData = (struct VECTOR *) ((U8 *)blk[sub].cameraDataOffset + "
                "sizeof(struct VECTOR) * 7u); /* PORT_BUILD: target DGROUP camera "
                "override is cameraDataOffset + 0x2A. */"
            )
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the track camera DGROUP-offset adapter")
            transformed = transformed.replace(old, new, 1)

        overlay = work / "overlay" / source
        overlay.parent.mkdir(parents=True, exist_ok=True)
        overlay.write_text(transformed, encoding="latin-1")
        result = probe.run_one(source, overlay, config, index)
        row = {"source": source, "owner": owner,
               "syntax": result["status"]["syntax"],
               "object": result["status"]["object"],
               "warnings": result["warning_count"],
               "errors": result["errors"],
               "removed_declarations": removed}
        rows.append(row)
        print(f"[{index:02d}/{len(sources)}] {source}: "
              f"object={row['object']} warnings={row['warnings']}")
        if row["syntax"] != "ok" or row["object"] != "ok":
            raise RuntimeError(f"Host compile failed for {source}: {row['errors']}")
        objects.append(work / "out" / f"{index:03d}_{Path(source).stem}.o")

    report = {
        "compiler": str(gcc.resolve()),
        "compiler_target": checked_run([str(gcc), "-dumpmachine"]).stdout.strip(),
        "source_count": len(rows),
        "syntax_passes": sum(row["syntax"] == "ok" for row in rows),
        "object_passes": sum(row["object"] == "ok" for row in rows),
        "port_adapters": [
            "rename recovered main to stunts_game_main",
            "wrap update_gamestate for post-commit tracing",
            "expose full GAMESTATE, frame, mode, and rate to port tracing",
            "rename DOS audio loader and route startup to silent port adapter",
            "type vector_op_unk2's host overlay declaration as I16",
            "resolve static track camera override offsets relative to camera data",
            "provide the legacy line editor screen rectangle as a host pointer view",
            "adapt line-editor two-word timer deadlines to the callback counter",
        ],
        "sources": rows,
    }
    (work / "compile-report.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
    return objects, report


def compile_port_sources(gcc: Path, sdl_root: Path) -> list[Path]:
    port_sources = [
        "main.c", "sdl_host.c", "video.c", "input.c", "timer.c", "memory.c",
        "test_seed.c",
        "sprite.c", "legacy_views.c",
        "random.c",
        "font.c",
        "sincos.c",
        "polang.c",
        "projection.c",
        "matrix.c",
        "vehicle.c",
        "file.c", "resource.c", "audio.c", "platform.c", "input_script.c",
        "trace.c", "trace_hooks.c",
    ]
    out_dir = BUILD / "port-obj"
    out_dir.mkdir(parents=True, exist_ok=True)
    common = [
        str(gcc), "-std=gnu11", "-O0", "-Wall", "-Wextra", "-Wpedantic",
        "-Wno-unused-parameter", "-DPORT_BUILD=1",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(sdl_root / "include"),
    ]
    objects = []
    for source in port_sources:
        src = ROOT / "port" / source
        obj = out_dir / f"{src.stem}.o"
        result = checked_run([*common, "-c", str(src), "-o", str(obj)], quiet=True)
        if result.returncode != 0:
            raise RuntimeError(f"Port source compile failed: {source}")
        if result.stderr:
            warnings = [line for line in result.stderr.splitlines() if "warning:" in line]
            if warnings:
                print(f"{source}: {len(warnings)} compiler warning(s)")
        objects.append(obj)
    return objects


def coff_identifier(symbol: str) -> str | None:
    name = symbol[1:] if symbol.startswith("_") else symbol
    if re.fullmatch(r"[A-Za-z_]\w*", name):
        return name
    return None


def unresolved_symbols(output: str) -> list[str]:
    names = set()
    for line in output.splitlines():
        match = re.search(r"undefined reference to [`']([^`']+)[`']", line)
        if match:
            names.add(match.group(1))
    return sorted(names)


def call_relocations(objdump: Path, objects: list[Path]) -> set[str]:
    result: set[str] = set()
    for obj in objects:
        output = checked_run([str(objdump), "-r", str(obj)]).stdout
        for line in output.splitlines():
            match = re.match(r"\s*(?:DISP|REL)\w*\s+(\S+)", line)
            if match:
                result.add(match.group(1))
    return result


def symbol_is_function(symbol: str, calls: set[str], search_text: str) -> bool:
    if symbol in calls:
        return True
    name = coff_identifier(symbol)
    if name is None:
        return False
    return re.search(r"\b" + re.escape(name) + r"\s*\(", search_text) is not None


def render_stubs(entries: list[dict[str, object]]) -> str:
    lines = [
        "/* Generated from the first strict host link diagnostics. Do not edit. */",
        '#include "port_runtime.h"',
        "#include <stdint.h>",
        "",
    ]
    for entry in entries:
        name = entry["c_identifier"]
        if name is None:
            continue
        if entry["kind"] == "function":
            lines += [f"void {name}(void)", "{",
                      f"    port_stub_fail(\"{entry['symbol']}\");", "}", ""]
        else:
            lines.append(f"uint8_t {name}[{DATA_STUB_BYTES}];")
    return "\n".join(lines) + "\n"


def link_executable(gcc: Path, sdl_root: Path, objects: list[Path],
                    port_objects: list[Path], stub_object: Path | None) -> subprocess.CompletedProcess[str]:
    exe = BUILD / "stunts.exe"
    command = [str(gcc), "-mconsole", "-static-libgcc", "-Wl,--no-undefined",
               *[str(path) for path in objects],
               *[str(path) for path in port_objects]]
    if stub_object is not None:
        command.append(str(stub_object))
    command += ["-L", str(sdl_root / "lib"), "-lSDL3", "-o", str(exe)]
    return subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                          errors="replace", check=False)


def generate_link_stubs(gcc: Path, sdl_root: Path, objects: list[Path],
                        port_objects: list[Path]) -> list[dict[str, object]]:
    objdump = gcc.parent / "objdump.exe"
    calls = call_relocations(objdump, [*objects, *port_objects])
    search_paths = [BUILD / "host-build" / "overlay", BUILD / "host-build" / "config",
                    ROOT / "port", ROOT / "tools" / "porting" / "port_include"]
    chunks = []
    for directory in search_paths:
        for path in directory.rglob("*"):
            if path.is_file() and path.suffix.lower() in {".c", ".h"}:
                chunks.append(path.read_text(encoding="latin-1", errors="replace"))
    search_text = "\n".join(chunks)
    entries: dict[str, dict[str, object]] = {}
    stub_c = BUILD / "generated-stubs.c"
    stub_object = BUILD / "generated-stubs.o"
    (BUILD / "link-first.log").write_text("", encoding="utf-8")

    first = link_executable(gcc, sdl_root, objects, port_objects, None)
    (BUILD / "link-first.log").write_text(first.stdout + first.stderr, encoding="utf-8")
    if first.returncode == 0:
        inventory = []
        (BUILD / "stub-inventory.json").write_text(json.dumps({
            "source": "first link diagnostics", "function_stub_count": 0,
            "data_stub_count": 0, "entries": inventory,
        }, indent=2), encoding="utf-8")
        return inventory

    unresolved = unresolved_symbols(first.stdout + first.stderr)
    if not unresolved:
        raise RuntimeError("Initial SDL3 link failed without undefined-symbol diagnostics; see build/sdl3/link-first.log")

    current_output = first.stdout + first.stderr
    while unresolved:
        new_names = [name for name in unresolved if name not in entries]
        if not new_names:
            raise RuntimeError("Linker repeated unresolved symbols after stubbing; see build/sdl3/link-final.log")
        for symbol in new_names:
            identifier = coff_identifier(symbol)
            if identifier is None:
                raise RuntimeError(f"Cannot create a safe C stub for linker symbol {symbol!r}")
            kind = "function" if symbol_is_function(symbol, calls, search_text) else "data"
            entries[symbol] = {
                "symbol": symbol,
                "c_identifier": identifier,
                "kind": kind,
                "linker_source": "first link undefined reference",
                "action": ("log and unwind guest startup" if kind == "function"
                           else "zero-filled unresolved data placeholder"),
                "placeholder_bytes": None if kind == "function" else DATA_STUB_BYTES,
            }
        inventory = list(entries.values())
        stub_c.write_text(render_stubs(inventory), encoding="utf-8")
        compile_result = checked_run([
            str(gcc), "-std=gnu11", "-O0", "-DPORT_BUILD=1", "-I", str(ROOT / "port"),
            "-c", str(stub_c), "-o", str(stub_object)], quiet=True)
        if compile_result.returncode != 0:
            raise RuntimeError("Generated unresolved-symbol stubs did not compile")
        if compile_result.stderr:
            print(compile_result.stderr, end="", file=sys.stderr)
        linked = link_executable(gcc, sdl_root, objects, port_objects, stub_object)
        current_output = linked.stdout + linked.stderr
        (BUILD / "link-final.log").write_text(current_output, encoding="utf-8")
        if linked.returncode == 0:
            unresolved = []
        else:
            unresolved = unresolved_symbols(current_output)
            if not unresolved:
                raise RuntimeError("Final link failed for a non-symbol error; see build/sdl3/link-final.log")

    inventory = list(entries.values())
    (BUILD / "stub-inventory.json").write_text(json.dumps({
        "source": "first link diagnostics",
        "function_stub_count": sum(row["kind"] == "function" for row in inventory),
        "data_stub_count": sum(row["kind"] == "data" for row in inventory),
        "data_placeholder_bytes_each": DATA_STUB_BYTES,
        "entries": inventory,
    }, indent=2), encoding="utf-8")
    return inventory


def prepare_assets(source: Path) -> Path:
    source = source.resolve()
    destination = (BUILD / "runtime" / "assets").resolve()
    if not destination.is_relative_to(ROOT.resolve()):
        raise RuntimeError(f"Asset-copy target escaped workspace: {destination}")
    destination.mkdir(parents=True, exist_ok=True)
    if not source.is_dir():
        print(f"Asset source unavailable; startup will report missing input: {source}")
        return destination
    shutil.copytree(source, destination, dirs_exist_ok=True)
    return destination


def build(args) -> Path:
    gcc = args.gcc.resolve()
    sdl_root = args.sdl_root.resolve()
    if not gcc.is_file():
        raise RuntimeError(f"GCC not found: {gcc}")
    if not (sdl_root / "include" / "SDL3" / "SDL.h").is_file() or not (sdl_root / "lib" / "libSDL3.dll.a").is_file():
        raise RuntimeError(f"SDL3 i686 development package is incomplete: {sdl_root}")
    prepare_environment(gcc)
    BUILD.mkdir(parents=True, exist_ok=True)
    game_objects, compile_report = build_game_objects(gcc)
    port_objects = compile_port_sources(gcc, sdl_root)
    stubs = generate_link_stubs(gcc, sdl_root, game_objects, port_objects)
    dll = sdl_root / "bin" / "SDL3.dll"
    if dll.is_file():
        shutil.copy2(dll, BUILD / "SDL3.dll")
    assets = prepare_assets(args.assets_source)
    summary = {
        "compiler": str(gcc),
        "target": checked_run([str(gcc), "-dumpmachine"]).stdout.strip(),
        "sdl3_root": str(sdl_root),
        "game_c_objects": len(game_objects),
        "port_objects": len(port_objects),
        "function_stub_count": sum(row["kind"] == "function" for row in stubs),
        "data_stub_count": sum(row["kind"] == "data" for row in stubs),
        "compile": {"syntax_passes": compile_report["syntax_passes"],
                    "object_passes": compile_report["object_passes"]},
        "assets_source": str(args.assets_source),
        "assets_runtime_copy": str(assets),
        "executable": str(BUILD / "stunts.exe"),
    }
    (BUILD / "build-report.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
    print(f"Build complete: {BUILD / 'stunts.exe'}")
    print(f"Active game C objects: {len(game_objects)}; host objects: {len(port_objects)}")
    print(f"Generated link stubs: {summary['function_stub_count']} functions, "
          f"{summary['data_stub_count']} data symbols")
    return BUILD / "stunts.exe"


def run(args) -> int:
    if not args.no_build:
        build(args)
    Path(args.capture_dir).mkdir(parents=True, exist_ok=True)
    exe = BUILD / "stunts.exe"
    command = [str(exe), f"--trace={args.trace}", f"--assets={args.assets}",
               f"--capture-dir={args.capture_dir}"]
    if args.run_ms is not None:
        command.append(f"--run-ms={args.run_ms}")
    if args.input_script is not None:
        command.append(f"--input-script={Path(args.input_script).resolve()}")
    if args.test_auto_protection:
        command.append("--test-auto-protection")
    if args.test_startup_seed is not None:
        command.append(f"--test-startup-seed={Path(args.test_startup_seed).resolve()}")
    if args.stop_after_sim_steps is not None:
        command.append(f"--stop-after-sim-steps={args.stop_after_sim_steps}")
    prepare_environment(args.gcc)
    return subprocess.run(command, cwd=ROOT, check=False).returncode


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    build_parser = sub.add_parser("build", help="compile, link, and copy local runtime inputs")
    build_parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC)
    build_parser.add_argument("--sdl-root", type=Path, default=DEFAULT_SDL)
    build_parser.add_argument("--assets-source", type=Path, default=DEFAULT_ASSETS)
    run_parser = sub.add_parser("run", help="build and launch the SDL3 startup host")
    run_parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC)
    run_parser.add_argument("--sdl-root", type=Path, default=DEFAULT_SDL)
    run_parser.add_argument("--assets-source", type=Path, default=DEFAULT_ASSETS)
    run_parser.add_argument("--assets", default="build/sdl3/runtime/assets")
    run_parser.add_argument("--trace", default="build/sdl3/runtime-trace.jsonl")
    run_parser.add_argument("--capture-dir", default="build/sdl3/captures")
    run_parser.add_argument("--run-ms", type=int)
    run_parser.add_argument("--input-script", type=Path)
    run_parser.add_argument("--test-auto-protection", action="store_true",
                            help="test only: type the live protection answer through DOS keyboard input")
    run_parser.add_argument("--test-startup-seed", type=Path,
                            help="test only: replay a Port Forge random_wait/timer/PRNG capture")
    run_parser.add_argument("--stop-after-sim-steps", type=int)
    run_parser.add_argument("--no-build", action="store_true")
    args = parser.parse_args(argv)
    try:
        if args.command == "build":
            build(args)
            return 0
        return run(args)
    except (OSError, RuntimeError, subprocess.SubprocessError) as exc:
        print(f"port build failed: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
