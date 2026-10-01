"""Audited instrumentation in the generated host view, never historical C."""
def adapt_render_stream(source: str) -> str:
    def replace(old, new):
        nonlocal source
        if source.count(old) != 1:
            raise ValueError(f"Missing unique render-stream boundary: {old}")
        source = source.replace(old, new, 1)
    replace("    link = POLYINFO_CAPACITY;\n    for (polyIndex",
            "    port_render_stream_begin((const int16_t *)&selection_rect);\n"
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
    return '#include "render/renderer.h"\n' + source
