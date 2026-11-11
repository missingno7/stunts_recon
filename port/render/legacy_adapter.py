"""Audited instrumentation in the generated host view, never historical C."""
def adapt_render_stream(source: str) -> str:
    def replace(old, new):
        nonlocal source
        if source.count(old) != 1:
            raise ValueError(f"Missing unique render-stream boundary: {old}")
        source = source.replace(old, new, 1)
    replace("    link = POLYINFO_CAPACITY;\n    for (polyIndex",
            "    port_render_stream_begin((const int16_t *)&selection_rect);\n"
            "    port_render_world_begin_active();\n"
            "    link = POLYINFO_CAPACITY;\n    for (polyIndex")
    replace("        fill = mat_copy_clr_lst_ptr[materialIndex];\n        switch (info->type)",
            "        fill = mat_copy_clr_lst_ptr[materialIndex];\n"
            "        port_render_stream_emit((uint16_t)link, info->depth, info->type,\n"
            "            info->material, info->numpoints, (const int16_t *)info->points,\n"
            "            material_patlistptr_copy[materialIndex],\n"
            "            matpatlistcopypointer2[materialIndex], (uint8_t)fill,\n"
            "            g_mat_clrlist_copy_2_ptr[materialIndex],\n"
            "            info->type == 3 ? mat_copy_clr_lst_ptr[materialIndex + 1] : 0,\n"
            "            info->type == 3 ? mat_copy_clr_lst_ptr[materialIndex + 2] : 0);\n"
            "        switch (info->type)")
    replace("    polyinfo_reset();\n}", "    port_camera_scene_end(material_patlistptr_copy, matpatlistcopypointer2,\n        mat_copy_clr_lst_ptr, g_mat_clrlist_copy_2_ptr);\n    port_render_world_end();\n    polyinfo_reset();\n}")
    return '#include "render/renderer.h"\n#include "render/renderer_ui.h"\n#include "render/renderer_camera_scene.h"\n' + source


def adapt_render_layers(unit: str, source: str) -> str:
    if unit == "src/obj_seg003.c":
        anchor="void drawworld(I8 page, struct RECTANGLE* clip) {"
        call="port_render_world_arm(); port_camera_scene_begin();"
    elif unit == "src/obj_seg000.c":
        anchor="select(void)\n{"
        call="port_render_world_clear();"
    else:
        return source
    if source.count(anchor) != 1:
        raise ValueError(f"Missing unique compositor boundary: {unit}")
    return '#include "render/renderer_ui.h"\n#include "render/renderer_camera_scene.h"\n'+source.replace(anchor,anchor+"\n    "+call,1)
