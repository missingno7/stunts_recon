"""Audited camera-space observation in generated host overlays only.

The native build uses this transformation without editing historical source.
The command-line form is also available for isolated instrumentation probes.
"""

from __future__ import annotations

import argparse
from pathlib import Path


DECL_ANCHOR = '#include "stunts_types.h"'
HOOK_ANCHOR = "    poly_link_listit4 = poly_cursor1 = facenodeiterator;"


DECLARATIONS = r'''

/* PORT_MODERN_CAPTURE: observational camera-space ABI; copied synchronously. */
extern void port_projection_snapshot(I16 *out);

static I16 port_modern_capture_add_word(I16 a, I16 b)
{
    U16 bits = (U16)((U16)a + (U16)b);
    return (I16)(bits < 0x8000u ? (I32)bits : (I32)bits - 0x10000L);
}
'''


HOOK = r'''
    /* Capture every transformed source vertex before the legacy rectangle
       reject below. This read-only pass repeats the already selected mview /
       position path with the same 16-bit mat_vec and word-add semantics. */
    if (port_renderer_camera_capture_enabled()) {
        I16 port_capture_projection[6];
        port_projection_snapshot(port_capture_projection);
        struct VECTOR port_capture_vertices[255];
        struct VECTOR port_capture_local;
        unsigned port_capture_index;
        for (port_capture_index = 0; port_capture_index < vertex_count;
             ++port_capture_index) {
            port_capture_local = obj_verts[port_capture_index];
            if (half_scale_flag != 0) {
                port_capture_local.x >>= 1;
                port_capture_local.y >>= 1;
                port_capture_local.z >>= 1;
            }
            mat_vec(&port_capture_local, &mview,
                    &port_capture_vertices[port_capture_index]);
            port_capture_vertices[port_capture_index].x =
                port_modern_capture_add_word(port_capture_vertices[port_capture_index].x,
                                             position.x);
            port_capture_vertices[port_capture_index].y =
                port_modern_capture_add_word(port_capture_vertices[port_capture_index].y,
                                             position.y);
            port_capture_vertices[port_capture_index].z =
                port_modern_capture_add_word(port_capture_vertices[port_capture_index].z,
                                             position.z);
        }
        port_renderer_capture_shape(
            ts->shapeptr, (const I16 *)port_capture_vertices, vertex_count,
            ts->shapeptr->primitives, ts->shapeptr->numprimitives,
            (U8)current_num_paints, ts->shapeptr->cull1,
            ts->shapeptr->cull2, cullmask, cullbits2,
            transformed_shape_material, obj_flags, half_scale_flag,
            backlightovr8,
            port_capture_projection[0], port_capture_projection[1],
            port_capture_projection[2], port_capture_projection[3],
            port_capture_projection[4], port_capture_projection[5],
            selection_rect.left, selection_rect.right,
            selection_rect.top, selection_rect.bottom);
    }

'''


def inject(source: str) -> str:
    if source.count(DECL_ANCHOR) != 1:
        raise ValueError("generated overlay has no unique VECTOR declaration")
    if source.count(HOOK_ANCHOR) != 1:
        raise ValueError("generated overlay has no unique post-transform/pre-reject anchor")
    source = source.replace(DECL_ANCHOR, DECL_ANCHOR + DECLARATIONS, 1)
    source = source.replace(HOOK_ANCHOR, HOOK + HOOK_ANCHOR, 1)
    anchor="            shape_polyinfo->depth = i;"
    if source.count(anchor) != 1:
        raise ValueError("Missing unique reference depth anchor")
    return source.replace(anchor,anchor+"\n            port_renderer_reference_primitive((unsigned)(cullwords - ts->shapeptr->cull1 - 1), i, (uint8_t)transformed_vert_count);",1)
