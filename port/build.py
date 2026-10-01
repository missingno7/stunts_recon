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
import zipfile

from game_abi import route_audio_vectors, adapt_aggregate_views, adapt_polygon_storage, host_view_contracts, adapt_preview_word_arithmetic, adapt_renderer_word_arithmetic, adapt_word_sentinels
from dependencies import NUKED_OPL3_COMMIT, NUKED_OPL3_FILES, nuked_opl3_root

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "sdl3"
DEFAULT_GCC = Path(r"C:\msys64\mingw32\bin\gcc.exe")
DEFAULT_SDL = Path(r"C:\tools\sdl3-3.4.16-i686")
DEFAULT_ASSETS = ROOT / "assets"
DATA_STUB_BYTES = 65536


def adapt_gameplay_word_arithmetic(source: str) -> str:
    """Keep two reachable 16-bit gameplay expressions exact in the host view."""
    include = '#include "stunts_types.h"\n'
    if source.count(include) != 1:
        raise RuntimeError("Could not locate the gameplay scalar type include")
    helpers = r'''/* PORT_BUILD: preserve locked 16-bit gameplay arithmetic. */
static int16_t port_game_s16_from_u16(uint16_t bits)
{
    int32_t value = (int32_t)bits;
    if (value >= 0x8000)
        value -= 0x10000;
    return (int16_t)value;
}

static int16_t port_game_mul_sar16(int16_t left, int16_t right, unsigned shift)
{
    int32_t product = (int32_t)left * (int32_t)right;
    uint16_t low_word = (uint16_t)(uint32_t)product;
    int32_t signed_word = low_word < 0x8000u
        ? (int32_t)low_word : (int32_t)low_word - 0x10000;
    uint32_t divisor = (uint32_t)1u << shift;
    if (signed_word >= 0)
        return (int16_t)(signed_word / (int32_t)divisor);
    return (int16_t)(-((-signed_word + (int32_t)divisor - 1) /
                       (int32_t)divisor));
}

static int16_t port_game_wall_hit_threshold(int16_t angle)
{
    int16_t scaled = port_game_mul_sar16(70, angle, 8);
    uint16_t base = (uint16_t)(uint32_t)(100 - (int32_t)scaled);
    return port_game_s16_from_u16((uint16_t)(base << 8));
}

'''
    transformed = source.replace(include, include + helpers, 1)
    replacements = (
        ("threshold = (100 - ((70 * i) >> 8)) << Q8_FRACTION_BITS; "
         "/* PORT: Q8 value is converted at the legacy boundary. */",
         "threshold = port_game_wall_hit_threshold(i); "
         "/* PORT_BUILD: locked IMUL/SAR and unsigned speed comparison. */"),
        ("if (activeCarState->car_speed2 > threshold) {",
         "if ((U16S)activeCarState->car_speed2 > (U16S)threshold) {"),
        ("speedPenalty = (0x300 * distanceToCar) >> 2;",
         "speedPenalty = port_game_mul_sar16(0x300, distanceToCar, 2); "
         "/* PORT_BUILD: locked IMUL/SAR word result. */"),
        ("if (player->car_speed2 < speedPenalty)",
         "if ((U16S)player->car_speed2 < (U16S)speedPenalty)"),
        ("player->car_speed2 -= speedPenalty;",
         "player->car_speed2 = (U16S)((U16S)player->car_speed2 - "
         "(U16S)speedPenalty);"),
    )
    for old, new in replacements:
        if transformed.count(old) != 1:
            raise RuntimeError(f"Could not locate one gameplay arithmetic anchor: {old}")
        transformed = transformed.replace(old, new, 1)
    return transformed


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
    original_port_include = ROOT / "tools" / "porting" / "port_include"
    executable_include = BUILD / "host-build" / "include"
    shutil.copytree(original_port_include, executable_include, dirs_exist_ok=True)
    aggregate_header = executable_include / "stunts_structs.h"
    aggregate_header.write_text(adapt_aggregate_views(probe.legacy_target_widths(
        aggregate_header.read_text(encoding="latin-1"))), encoding="latin-1")
    probe.PORT = executable_include
    probe.HOST = ROOT / "tools" / "porting" / "host"
    probe.WORK = BUILD / "host-build"
    probe.GCC = gcc.resolve()
    probe.STRICT_CENTRAL = False
    declarations.ROOT = ROOT
    # Executable views describe the DOS width, rather than resolving I32 via
    # the host int typedef used by the diagnostic-only declaration survey.
    declarations.ALIASES.update(I32="long", U32="unsigned long")
    declarations.HOST_OVERRIDES.update({
        "file_load_shape2d_fatal_thunk": "extern void *file_load_shape2d_fatal_thunk(char *);",
        "mmgr_get_chunk_size": "extern uint16_t mmgr_get_chunk_size(void *);",
        "locate_shape_nofatal": "extern void *locate_shape_nofatal(void *, const char *);",
        "file_load_3dres": "extern void *file_load_3dres(char *);",
        "file_load_resource": "extern void *file_load_resource(I16, const char *);",
        "file_find": "extern char *file_find(const char *);",
        "locate_shape_alt": "extern char *locate_shape_alt(char *, char *);",
        "file_load_binary_nofatal": "extern void *file_load_binary_nofatal(const char *);",
        "mmgr_op_unk": "extern void *mmgr_op_unk(void *);",
        "parse_shape2d_helper": "extern int32_t parse_shape2d_helper(uint8_t *);",
        "file_read_nofatal": "extern void *file_read_nofatal(const char *, void *);",
        "file_read_fatal": "extern void *file_read_fatal(const char *, void *);",
        "read_file_with_retry": "extern void *read_file_with_retry(I16, char *, void *);",
        "sub_35DC8": "extern void sub_35DC8(const uint8_t *);",
        "sub_35DE6": "extern void sub_35DE6(uint16_t, uint16_t, const uint8_t *);",
        "file_combine_and_find": "extern char *file_combine_and_find(char *, char *, char *);",
        "font_op2": "extern I16 font_op2(const char *);",
        "shape2d_op_unk4": "extern void shape2d_op_unk4(const struct SHAPE2D *);",
        "shape2d_render_bmp_as_mask": "extern void shape2d_render_bmp_as_mask(const struct SHAPE2D *);",
        "nopsub_37750": "extern void nopsub_37750(U16, void (*)(I16));",
        "do_fileselect_dialog": "extern I16 do_fileselect_dialog(char *, char *, char *, char *);",
    })
    # Local declaration surveys may omit a service entirely. Apply the same
    # executable ABI to the derived central header as well as each TU config.
    central_header = executable_include / "stunts_decls.h"
    central = central_header.read_text(encoding="latin-1")
    for name, prototype in declarations.HOST_OVERRIDES.items():
        central = re.sub(r"(?m)^extern [^;]*\b" + re.escape(name) + r"\([^;]*;",
                         lambda _: prototype, central)
    central_header.write_text(central, encoding="latin-1")
    probe.FLAGS = list(probe.FLAGS) + [
        # Preserve legacy diagnostics, but reject pointer/integer conversion
        # mistakes at the first compilation rather than a runtime screen.
        "-g", "-I", str(ROOT / "port"),
        "-Wno-error=implicit-int",
        "-Werror=int-conversion",
        "-Werror=type-limits",
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
            # The historical near-pointer extension occupied one word. Keep
            # that parameter as a native pointer in the host view, consistently
            # with the .trk/.rpl callers and file_combine_and_find's callee.
            old = "I16 far do_fileselect_dialog(I8 *path, I8 *selected_name, I16 attributes,"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the file-dialog extension pointer")
            transformed = transformed.replace(
                old, "I16 far do_fileselect_dialog(I8 *path, I8 *selected_name, I8 *attributes,")
            transformed = transformed.replace(
                "extern I8 *file_combine_and_find(I8 *, I8 *, I16);",
                "extern I8 *file_combine_and_find(I8 *, I8 *, I8 *);")
            # Native directories can exceed the signed-byte list capacity.
            # Stop before its count wraps and becomes a negative array index.
            file_row = "        parse_filepath_separators(names[files_found], found);"
            if transformed.count(file_row) != 1:
                raise RuntimeError("Could not locate the bounded file-dialog list")
            transformed = transformed.replace(
                file_row, "        if (files_found >= 127) break; /* Host list bound. */\n" + file_row)
            # This legacy test is unreachable after the existing host bound.
            # Remove it rather than exempting the TU from range diagnostics.
            old = "        if (files_found == 0x80)\n            break;"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the superseded file-list bound")
            transformed = transformed.replace(old, "", 1)
            transformed = "extern I16 port_random_test_rand(void);\n" + transformed
            transformed = "extern void port_video_publish(const char *reason);\n" + transformed
            for cursor_draw in ("mouse_draw_transparent", "mouse_draw_opaque"):
                transformed, count = re.subn(
                    r"(?m)^void\s+far\s+" + cursor_draw + r"\(void\)",
                    "void far stunts_" + cursor_draw + "(void)", transformed, count=1)
                if count != 1:
                    raise RuntimeError(f"Could not locate cursor publication boundary {cursor_draw}")
                transformed += ("\nvoid " + cursor_draw + "(void) { stunts_" + cursor_draw +
                                "(); port_video_publish(\"" + cursor_draw + "\"); }\n")

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
            transformed = "extern int32_t port_audio_is_silent(void);\nextern void port_audio_shutdown(void);\n" + transformed
            transformed, count = re.subn(
                r"(?m)^(void\s+FAR\s+audiodrv_atexit\s*\([^\n]*\)\s*\{)",
                r"\1\n    port_audio_shutdown(); return;", transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not locate host audio teardown ownership boundary")
            transformed, count = re.subn(
                r"(?m)^(void\s+FAR\s+load_audio_finalize\s*\([^\n]*\)\s*\{)",
                r"\1\n    /* An unavailable host backend retains resources without dispatch; do not "
                "call the DOS driver image. */\n    if (port_audio_is_silent()) return;",
                transformed, count=1)
            if count != 1:
                raise RuntimeError("Could not isolate the audio finalizer boundary")
            transformed, count = re.subn(
                r"(?m)^(void\s+FAR\s+audio_driver_func3F\s*\([^\n]*\)\s*\{)",
                r"\1\n    /* Dispatch only after the host backend has initialized. */\n"
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
        elif source == "src/obj_seg035_group.c":
            # The DOS FAR scanner wraps its 16-bit offset and can examine
            # unrelated allocator bytes beyond the image. Native pointers
            # must stay within the unconsumed pixels, including zero at the
            # final pending-literal flush.
            old = "repeat = parse_shape2d_helper3((I8 FAR *)src);"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the bounded shape run scanner")
            transformed = (
                "extern int16_t port_parse_shape2d_helper3(const uint8_t *, uint16_t);\n"
                + transformed.replace(old,
                    "repeat = port_parse_shape2d_helper3((const uint8_t *)src, "
                    "(uint16_t)(page_remaining - literal_cnt));", 1))
        elif source == "src/obj_seg005.c":
            original = "\t\tfor (;;) {\n\t\t\twhile (core.game_frame != tmr2) {"
            if transformed.count(original) != 1:
                raise RuntimeError("Could not locate the race timer dispatch boundary")
            transformed = "extern void port_guest_check_stop(void);\n" + transformed.replace(
                original, "\t\tfor (;;) {\n\t\t\tport_guest_check_stop();\n"
                          "\t\t\twhile (core.game_frame != tmr2) {")
        elif source == "src/obj_seg004.c":
            for block in ("blk[sub]", "blk"):
                field = block + ("." if block.endswith("]") else "->")
                old = "camData = (struct VECTOR *) " + field + "cameraOverlay.cameraOffsetOverride;"
                new = (
                    "camData = (struct VECTOR *) ((U8 *)" + field + "cameraDataOffset + "
                    "sizeof(struct VECTOR) * 7u); /* PORT_BUILD: target DGROUP camera "
                    "override is cameraDataOffset + 0x2A. */"
                )
                if transformed.count(old) != 1:
                    raise RuntimeError("Could not locate the track camera DGROUP-offset adapter")
                transformed = transformed.replace(old, new, 1)

        if source == "src/obj_seg001_complete.c":
            old = "trackData = (struct VECTOR far *)objectInfo->link.dataPointer;"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the track-edge DOS near-offset adapter")
            # All six nonzero frozen links point 42 bytes beyond their camera
            # data (DS:1972 and DS:191E in shapeinfos[104..109]).
            transformed = transformed.replace(old,
                "trackData = (struct VECTOR far *)((uint8_t *)"
                "objectInfo->si_cameraDataOffset + sizeof(struct VECTOR) * 7u);", 1)
            transformed = adapt_gameplay_word_arithmetic(transformed)

        if source == "src/obj_seg003.c":
            transformed = adapt_preview_word_arithmetic(transformed)

        transformed = adapt_word_sentinels(transformed, Path(source).name)

        if source == "src/obj_seg006.c":
            transformed = adapt_renderer_word_arithmetic(transformed)

        if source == "src/obj_seg031.c":
            if transformed.count("exit(1);") != 1:
                raise RuntimeError("Could not locate guest audio-failure exit")
            transformed = "extern void port_guest_exit(int status);\n" + transformed.replace("exit(1);", "port_guest_exit(1);")

        if source in {"src/obj_seg027.c", "src/obj_seg028.c"}:
            transformed = '#include "audio_backend.h"\n' + route_audio_vectors(transformed)
        if source == "src/obj_seg027.c":
            old = "void FAR nopsub_37750(U16 chunk, I32 value)"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the audio callback setter signature")
            transformed = transformed.replace(old,
                "void FAR nopsub_37750(U16 chunk, void (*value)(I16))", 1)
        if source == "src/obj_seg028.c":
            # SKIDOVER's KEYS instrument is absent from the PC speaker bank.
            # DOS reads 0000:0005 before rejecting the null far pointer; a
            # native process cannot perform that read. Preserve its rejection
            # before accessing the instrument record.
            old = "    sample = resource->data;\n    if (sample[5] == 5) {"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate the nullable music instrument boundary")
            transformed = transformed.replace(old,
                "    sample = resource->data;\n    if (sample == 0) return -1;\n"
                "    if (sample[5] == 5) {", 1)
        if source in {"src/obj_seg007.c", "src/obj_seg028.c"}:
            transformed = "extern int32_t port_audio_is_active(void);\n" + transformed
            boundaries = ({"audio_op_unk": "return;", "audio_driver_timer": "return;"}
                          if source.endswith("007.c") else {"process_audio_event": "return -1;"})
            for name, stop in boundaries.items():
                pattern = r"(?m)^([^;\n]*\b" + name + r"\([^;\n]*\)\s*\{)"
                transformed, count = re.subn(pattern, lambda m: m.group(1) +
                    "\n    if (!port_audio_is_active()) " + stop, transformed, count=1)
                if count != 1:
                    raise RuntimeError(f"Could not locate inactive audio boundary {name}")

        # Compatibility views retain source-local aggregate names, but DOS
        # int/unsigned/long storage still needs its original scalar width.
        # This also covers private tables which central declarations cannot fix.
        transformed = adapt_aggregate_views(probe.legacy_target_widths(transformed))
        if source == "src/toupper.c":
            transformed = transformed.replace("I16 toupper(I16 ch)", "int toupper(int ch)")
        host_config = probe.legacy_target_widths(config.read_text(encoding="latin-1"))
        if source == "src/obj_seg001_complete.c":
            for name in ("centerpos", "veh_position", "veh_z"):
                host_config = host_config.replace(f"extern I16 {name};", f"extern I32 {name};")
        if source == "src/obj_seg006.c":
            host_config = host_config.replace("extern I16 inverse_power_of_two_table[32];",
                                               "extern I32 inverse_power_of_two_table[32];")
        if source == "src/toupper.c":
            host_config = host_config.replace("I16 toupper(I16", "int toupper(int")
        for name, prototype in declarations.HOST_OVERRIDES.items():
            host_config = re.sub(r"(?m)^extern [^\n;]*\b" + re.escape(name) +
                                 r"\([^\n]*;", lambda _: prototype, host_config)
        config.write_text(host_config, encoding="latin-1")

        if source == "src/obj_seg006.c":
            # DOS links index 400 to the adjacent reset marker at DS:5A86.
            # Host BSS order instead placed the pool pointer there. Give the
            # head its own element and alias the marker deliberately.
            transformed = adapt_polygon_storage(transformed)
            transformed += ("\nuint16_t port_game_y_rotation_snapshot(void) { "
                            "return (uint16_t)mat_y_rot_angle; }\n")

        if source == "src/obj_seg008.c":
            old = "read_file_with_retry(I16 type, U16  first, U16  second, U16  third)"
            if transformed.count(old) != 1:
                raise RuntimeError("Could not locate split-word file read ABI")
            transformed = transformed.replace(old, "read_file_with_retry(I16 type, char *first, void *second)")
            transformed = transformed.replace("file_read_nofatal(first, second, third)",
                                              "file_read_nofatal(first, second)")
        transformed = transformed.replace("((int16_t (*)())call_read_line)", "port_call_read_line")
        transformed = probe.rewrite_calls(transformed, "call_read_line", "rename:port_call_read_line")
        transformed = "extern int16_t port_call_read_line(char *, int16_t, int16_t, int16_t, int32_t);\n" + transformed
        transformed += host_view_contracts(source)

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
            "rename DOS audio loader and route its vectors to typed native driver entries",
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
    opl_source = nuked_opl3_root()
    port_sources = [
        "main.c", "sdl_host.c", "video.c", "input.c", "timer.c", "memory.c",
        "test_seed.c",
        "sprite.c", "sprite_aux.c", "legacy_views.c",
        "random.c",
        "font.c",
        "sincos.c",
        "polang.c",
        "projection.c",
        "matrix.c",
        "vehicle.c",
        "file.c", "resource.c", "audio.c", "audio_sdl.c", "pc_speaker.c", "cleanup.c", "platform.c", "input_script.c",
        "ad15_driver.c", "port_opl3.c",
        "trace.c", "trace_hooks.c",
    ]
    out_dir = BUILD / "port-obj"
    out_dir.mkdir(parents=True, exist_ok=True)
    common = [
        str(gcc), "-std=gnu11", "-O0", "-g", "-Wall", "-Wextra", "-Wpedantic",
        "-Wno-unused-parameter", "-Werror=int-conversion", "-DPORT_BUILD=1",
        "-include", str(ROOT / "tools" / "porting" / "host" / "compat.h"),
        "-I", str(ROOT / "port"),
        "-I", str(ROOT / "tools" / "porting" / "port_include"),
        "-I", str(ROOT / "tools" / "porting" / "host" / "include"),
        "-I", str(sdl_root / "include"),
        "-I", str(opl_source),
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
    # Keep the upstream implementation separate from historical compatibility
    # headers and compile the chip's audio-rate inner loop with optimization.
    opl_object = out_dir / "nuked_opl3.o"
    checked_run([str(gcc), "-std=gnu11", "-O2", "-g", "-I", str(opl_source),
                 "-c", str(opl_source / "opl3.c"), "-o", str(opl_object)])
    objects.append(opl_object)
    shutil.copy2(opl_source / "LICENSE", BUILD / "Nuked-OPL3-LICENSE.txt")
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
    if stubs:
        raise RuntimeError(
            f"SDL3 build has {len(stubs)} unresolved services/data bindings; "
            "implement their original contracts before accepting the executable "
            "(see build/sdl3/stub-inventory.json)")
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
        "nuked_opl3": {"commit": NUKED_OPL3_COMMIT, "sha256": NUKED_OPL3_FILES},
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
    if args.capture_dir:
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
    command.append(f"--audio={args.audio}")
    prepare_environment(args.gcc)
    return subprocess.run(command, cwd=ROOT, check=False).returncode


def package(args) -> Path:
    """Create the files to extract beside an existing game's original assets."""
    if not args.no_build:
        build(args)
    report = json.loads((BUILD / "build-report.json").read_text(encoding="utf-8"))
    if (report.get("target") != "i686-w64-mingw32" or
            report.get("function_stub_count") != 0 or report.get("data_stub_count") != 0):
        raise RuntimeError("Package requires a complete i686 SDL3 build")
    inputs = [Path(__file__), ROOT / "port" / "game_abi.py", ROOT / "port" / "dependencies.py"]
    for directory in (ROOT / "port", ROOT / "src", ROOT / "include",
                      ROOT / "tools" / "porting" / "host",
                      ROOT / "tools" / "porting" / "port_include"):
        inputs.extend(path for path in directory.rglob("*")
                      if path.is_file() and path.suffix.lower() in {".c", ".h"})
    newest = max(path.stat().st_mtime_ns for path in inputs)
    if min((BUILD / "stunts.exe").stat().st_mtime_ns,
           (BUILD / "build-report.json").stat().st_mtime_ns) < newest:
        raise RuntimeError("SDL3 build is stale; rebuild before packaging")
    destination = BUILD / "drop-in"
    destination.mkdir(parents=True, exist_ok=True)
    files = {
        "stunts-sdl3.exe": BUILD / "stunts.exe",
        "SDL3.dll": BUILD / "SDL3.dll",
        "Nuked-OPL3-LICENSE.txt": BUILD / "Nuked-OPL3-LICENSE.txt",
        "SDL3-LICENSE.txt": args.sdl_root / "share" / "licenses" / "SDL3" / "LICENSE.txt",
    }
    for name, source in files.items():
        shutil.copy2(source, destination / name)
    readme = destination / "README-SDL3.txt"
    readme.write_text(
        "Stunts SDL3 build for Windows (32-bit)\n\n"
        "Extract these files into your existing Stunts 1.1 game folder.\n"
        "Keep all the original game data files there.\n"
        "Double-click stunts-sdl3.exe to play.\n"
        "No Python, compiler, installer, or launcher is needed.\n\n"
        "New tracks, replays, and high scores go into the saves subfolder.\n"
        "Original game data remains in the game folder.\n"
        "AdLib/Sound Blaster FM audio is the default.\n",
        encoding="utf-8")
    archive = BUILD / "stunts-sdl3-win32.zip"
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as bundle:
        for name in [*files, readme.name]:
            bundle.write(destination / name, arcname=name)
    print(f"Drop-in package: {archive}")
    return archive


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    build_parser = sub.add_parser("build", help="compile, link, and copy local runtime inputs")
    build_parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC)
    build_parser.add_argument("--sdl-root", type=Path, default=DEFAULT_SDL)
    build_parser.add_argument("--assets-source", type=Path, default=DEFAULT_ASSETS)
    package_parser = sub.add_parser("package", help="build a ZIP to extract into an existing game folder")
    package_parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC)
    package_parser.add_argument("--sdl-root", type=Path, default=DEFAULT_SDL)
    package_parser.add_argument("--assets-source", type=Path, default=DEFAULT_ASSETS)
    package_parser.add_argument("--no-build", action="store_true")
    run_parser = sub.add_parser("run", help="build and launch the SDL3 startup host")
    run_parser.add_argument("--gcc", type=Path, default=DEFAULT_GCC)
    run_parser.add_argument("--sdl-root", type=Path, default=DEFAULT_SDL)
    run_parser.add_argument("--assets-source", type=Path, default=DEFAULT_ASSETS)
    run_parser.add_argument("--assets", default="build/sdl3/runtime/assets")
    run_parser.add_argument("--trace", default="")
    run_parser.add_argument("--capture-dir", default="")
    run_parser.add_argument("--run-ms", type=int)
    run_parser.add_argument("--input-script", type=Path)
    run_parser.add_argument("--test-auto-protection", action="store_true",
                            help="test only: type the live protection answer through DOS keyboard input")
    run_parser.add_argument("--test-startup-seed", type=Path,
                            help="test only: replay a Port Forge random_wait/timer/PRNG capture")
    run_parser.add_argument("--stop-after-sim-steps", type=int)
    run_parser.add_argument("--audio", choices=("ad15", "pc15", "none"), default="ad15")
    run_parser.add_argument("--no-build", action="store_true")
    args = parser.parse_args(argv)
    try:
        if args.command == "build":
            build(args)
            return 0
        if args.command == "package":
            package(args)
            return 0
        return run(args)
    except (OSError, RuntimeError, subprocess.SubprocessError) as exc:
        print(f"port build failed: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
