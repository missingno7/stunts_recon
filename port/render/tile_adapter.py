"""Audited generated host-overlay transform for the widened scene pass."""

from __future__ import annotations

import re


UPDATE_SIGNATURE = "void update_frame(I8 page, struct RECTANGLE* clip) {"
TRANS_CAPTURE_END = "            selection_rect.top, selection_rect.bottom);"
WIDE_DECLARATIONS = r'''
#include "render/tile_expansion.h"
#include "render/renderer_camera_scene.h"
#include "render/renderer_sky.h"
#define PORT_WIDE_AHEAD_MAX_COUNT PORT_AHEAD_MAX_COUNT
extern int memcmp(const void *, const void *, size_t);
extern void abort(void);
extern size_t port_game_state_copy(void *, size_t);
'''



def _matching_brace(source: str, opening: int) -> int:
    depth = 0
    i = opening
    state = "code"
    while i < len(source):
        ch = source[i]
        nxt = source[i + 1] if i + 1 < len(source) else ""
        if state == "code":
            if ch == "/" and nxt == "/":
                state = "line_comment"
                i += 2
                continue
            if ch == "/" and nxt == "*":
                state = "block_comment"
                i += 2
                continue
            if ch == '"':
                state = "string"
            elif ch == "'":
                state = "char"
            elif ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    return i
        elif state == "line_comment":
            if ch == "\n":
                state = "code"
        elif state == "block_comment":
            if ch == "*" and nxt == "/":
                state = "code"
                i += 2
                continue
        elif state == "string":
            if ch == "\\":
                i += 2
                continue
            if ch == '"':
                state = "code"
        elif state == "char":
            if ch == "\\":
                i += 2
                continue
            if ch == "'":
                state = "code"
        i += 1
    raise ValueError("unterminated update_frame body")


def _function_span(source: str, signature: str) -> tuple[int, int]:
    if source.count(signature) != 1:
        raise ValueError("generated source has no unique update_frame definition")
    start = source.index(signature)
    opening = start + signature.rfind("{")
    return start, _matching_brace(source, opening) + 1


def _replace_once(text: str, old: str, new: str, what: str) -> str:
    count = text.count(old)
    if count != 1:
        raise ValueError(f"expected one {what} anchor, found {count}")
    return text.replace(old, new, 1)


def adapt_update_frame(source: str) -> str:
    """Keep the current function as faithful pass and generate a wide clone.

    The wrapper runs the capture-only clone before the unchanged body. The
    clone returns at `draw_done`, before skybox, polyinfo, or 2D composition.
    """
    start, end = _function_span(source, UPDATE_SIGNATURE)
    original = source[start:end]
    if "port_render_world_arm(); port_camera_scene_begin();" not in original:
        raise ValueError("expected current compositor begin hook in update_frame")

    clone = original.replace(UPDATE_SIGNATURE,
        "static void port_update_frame_wide_capture(I8 page, struct RECTANGLE* clip) {", 1)
    clone = clone.replace("port_render_world_arm(); port_camera_scene_begin();",
                          "/* wide scene bank armed by wrapper */", 1)

    arrays = ("I8 skipped[24];", "U8  terrain_list[24];", "I8 east_list[24];",
              "I8 south_tab[24];", "I8 lod_levels[24];", "U8  elem_list[24];")
    for declaration in arrays:
        clone = _replace_once(clone, declaration,
                              declaration.replace("[24]", "[PORT_WIDE_AHEAD_MAX_COUNT + 1]"),
                              declaration)
    local_anchor = "I8 skipped[PORT_WIDE_AHEAD_MAX_COUNT + 1];"
    clone = _replace_once(clone, local_anchor,
        local_anchor + "\n\tI8 port_wide_ahead_table[PORT_WIDE_AHEAD_MAX_COUNT * 3];"
        "\n\tI16 port_wide_tile_count = 0x17;", "wide table locals")

    clone = _replace_once(clone, "tile_table = ahead_tables[si];",
        "tile_table = ahead_tables[si];\n"
        "\tif (port_camera_scene_wide_requested()) {\n"
        "\t\tunsigned port_wide_count = port_expand_ahead_table(\n"
        "\t\t    (const int8_t *)tile_table, 23u,\n"
        "\t\t    port_camera_scene_wide_extra_tiles(),\n"
        "\t\t    (int8_t *)port_wide_ahead_table, PORT_WIDE_AHEAD_MAX_COUNT);\n"
        "\t\tif (port_wide_count < 23u) {\n"
        "\t\t    port_camera_scene_reject(\"invalid recovered ahead-table expansion\"); return;\n"
        "\t\t}\n"
        "\t\ttile_table = port_wide_ahead_table;\n"
        "\t\tport_wide_tile_count = (I16)port_wide_count;\n"
        "\t}", "ahead table selection")

    loop_replacements = (
        ("for (si = 0; si < 0x17; si++)", "for (si = 0; si < port_wide_tile_count; si++)", 2),
        ("for (si = 0x16; si >= 0; si--)", "for (si = port_wide_tile_count - 1; si >= 0; si--)", 1),
        ("for (si = 0x16; si > j; si--)", "for (si = port_wide_tile_count - 1; si > j; si--)", 2),
    )
    for old, new, expected in loop_replacements:
        if clone.count(old) != expected:
            raise ValueError(f"expected {expected} widened-loop anchors for {old!r}; found {clone.count(old)}")
        clone = clone.replace(old, new)

    # The original table never visits side-border neighbours while an absent
    # car carries the -1 sentinel. The expanded table does; do not let that
    # neighbour sentinel select an unloaded car or read an unset wheel row.
    for east, south in (("car_east1", "wheel_south"),
                        ("cur_opp_east", "opp_wheel_south_val")):
        predicate = f"if (({east} == tx || {east} == tile_east2) && ({south} == trow || {south} == drow))"
        clone = _replace_once(clone, predicate,
            predicate.replace("if ((", f"if ({east} != -1 && (", 1),
            f"{east} visibility sentinel")

    # Native clouds use their circular camera-relative placement rather than
    # the DOS fixed-depth billboard translation. Retain DOS XYZ for matching.
    clone = _replace_once(clone, "struct MATRIX sky_mat;",
        "struct MATRIX sky_mat;\n\tstruct VECTOR port_cloud_native_pos, port_cloud_delta;",
        "native cloud locals")
    clone = _replace_once(clone, "rot_ofs.z = 0x3A98; //15000",
        "mat_vec(&rot_ofs, &view_mat, &port_cloud_native_pos);\n\t\t\t\t"
        "rot_ofs.z = 0x3A98; //15000", "cloud forced depth")
    clone = _replace_once(clone, "if (cur_shps->pos.z > PLATFORM_SCREEN_HEIGHT_PIXELS)",
        "if (1)", "cloud legacy depth reject")
    cloud_call = "result = trans_op(&cur_shps[0]);"
    cloud_end = "\n\t\t\t\t\t/* We cannot be out of memory as we are just starting to process. */"
    clone = _replace_once(clone, cloud_call + cloud_end,
        "port_cloud_delta.x = (I16)((U16)port_cloud_native_pos.x - (U16)cur_shps->pos.x);\n"
        "\t\t\t\t\tport_cloud_delta.y = (I16)((U16)port_cloud_native_pos.y - (U16)cur_shps->pos.y);\n"
        "\t\t\t\t\tport_cloud_delta.z = (I16)((U16)port_cloud_native_pos.z - (U16)cur_shps->pos.z);\n"
        "\t\t\t\t\tport_camera_scene_cloud_offset((const I16 *)&port_cloud_delta);\n"
        "\t\t\t\t\t" + cloud_call + "\n\t\t\t\t\tport_camera_scene_cloud_offset(NULL);" + cloud_end,
        "cloud native translation boundary")

    done = "draw_done:\n"
    if clone.count(done) != 1:
        raise ValueError("Missing unique draw completion anchor")
    clone = clone[:clone.index(done)] + "draw_done:\n\treturn;\n}"
    # The original sky-shape angle reject is a 4:3 submission optimization.
    # Native projection clips these already transformed background shapes.
    clone = _replace_once(clone, "if (si < 0x87 || si > 0x379)",
                          "if (1)", "sky-shape angular filter")

    # The original function is retained verbatim as a private body; the public
    # wrapper owns only pass ordering and the pending-wide-scene lifecycle.
    faithful = original.replace(UPDATE_SIGNATURE,
        "static void port_update_frame_faithful(I8 page, struct RECTANGLE* clip) {", 1)
    wrapper = r'''void update_frame(I8 page, struct RECTANGLE* clip) {
    if (port_camera_scene_wide_requested()) {
        unsigned char port_state_before[0x460];
        unsigned char port_state_after[0x460];
        I8 port_saved_backlight = backlightovr8;
        if (port_game_state_copy(port_state_before, sizeof(port_state_before)) != 0x460u)
            abort();
        port_camera_scene_wide_begin();
        port_update_frame_wide_capture(page, clip);
        port_camera_scene_wide_end();
        backlightovr8 = port_saved_backlight;
        if (port_game_state_copy(port_state_after, sizeof(port_state_after)) != 0x460u ||
            memcmp(port_state_before, port_state_after, sizeof(port_state_before)) != 0)
            abort();
    }
    port_update_frame_faithful(page, clip);
}
'''
    faithful = _replace_once(faithful,
        "skybox_result = skybox_op(page, clip, cur_sky_sign, &view_mat, bank, view_rot_x, viewpoint.y);",
        "port_sky_capture((const I16 *)&view_mat, view_rot_x, viewpoint.y,\n"
        "        g_skybox_sky_clr, ground_skybox, detail_lvl, (const void *const *)skypics);\n\t"
        "skybox_result = skybox_op(page, clip, cur_sky_sign, &view_mat, bank, view_rot_x, viewpoint.y);",
        "native panorama snapshot")
    transformed = source[:start] + clone + "\n\n" + faithful + "\n\n" + wrapper + source[end:]
    if transformed.count("void update_frame(I8 page, struct RECTANGLE* clip) {") != 1:
        raise AssertionError("public update_frame wrapper was not emitted exactly once")
    return WIDE_DECLARATIONS + transformed


def adapt_trans_op(source: str) -> str:
    """Let capture-only clones observe shapes without consuming the old pool."""
    entry = "    if (poly_list_insert_result != 0)\n        return 1;"
    source = _replace_once(source, entry,
        "    if (poly_list_insert_result != 0 && !port_renderer_capture_only())\n"
        "        return 1;", "trans_op pool guard")
    source = _replace_once(source, "    vertex_count = ts->shapeptr->numverts;",
        "    if (port_render_mode()==2 && (!ts || !ts->shapeptr)) {\n"
        "        port_camera_scene_reject(\"null shape submitted to original transform\"); return 1;\n"
        "    }\n    vertex_count = ts->shapeptr->numverts;", "native null-shape contract")
    source = _replace_once(source, TRANS_CAPTURE_END,
        TRANS_CAPTURE_END + "\n        if (port_renderer_capture_only()) return 0;",
        "camera capture return")
    return "extern int port_renderer_capture_only(void);\n" + source
