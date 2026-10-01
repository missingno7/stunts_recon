#include "renderer_camera.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void triangle(uint8_t *stream, uint8_t material, uint8_t a, uint8_t b, uint8_t c)
{
    stream[0] = 3; stream[1] = 0; stream[2] = material;
    stream[3] = a; stream[4] = b; stream[5] = c;
}

int main(void)
{
    PortCameraShapeCapture shape;
    PortCameraCommand commands[8];
    PortCameraUnsupported unsupported[8];
    size_t command_count, unsupported_count;
    int16_t xyz[] = {
        -10, -10, 80, 10, -10, 80, 0, 10, 80,
        -10, -10, 100, 10, -10, 100, 0, 10, 100
    };
    int32_t cull1[3] = {-1, -1, -1};
    int32_t cull2[3] = {0, 0, 0};
    uint8_t stream[18];

    triangle(stream, 7, 0, 2, 1);
    triangle(stream + 6, 9, 3, 5, 4);
    stream[12] = 13; stream[13] = 0; stream[14] = 11;
    stream[15] = 0; stream[16] = 1; stream[17] = 2;
    memset(&shape, 0, sizeof(shape));
    shape.shape_identity = 42;
    shape.camera_xyz = xyz;
    shape.vertex_count = 6;
    shape.primitive_stream = stream;
    shape.primitive_count = 3;
    shape.paint_count = 1;
    shape.cull1 = cull1;
    shape.cull2 = cull2;
    shape.cullmask = -1;
    shape.selected_paint = 0;
    shape.projection.center_x = 160;
    shape.projection.center_y = 100;
    shape.projection.half_width = 160;
    shape.projection.half_height = 100;
    shape.projection.scale_x = 100;
    shape.projection.scale_y = 100;

    if (port_camera_build_shape(&shape, commands, 8, &command_count,
                                unsupported, 8, &unsupported_count) != PORT_CAMERA_BUILD_OK)
        return 10;
    if (command_count != 2 || unsupported_count != 1) return 11;
    if (commands[0].material != 9 || commands[0].depth != 100) return 12;
    if (commands[1].material != 7 || commands[1].depth != 80) return 13;
    if (commands[0].primitive_ordinal != 1 || commands[1].primitive_ordinal != 0) return 14;
    if (unsupported[0].source_code != 13 || unsupported[0].primitive_ordinal != 2)
        return 15;

    shape.object_flags = 1;
    if (port_camera_build_shape(&shape, commands, 8, &command_count,
                                unsupported, 8, &unsupported_count) != PORT_CAMERA_BUILD_OK)
        return 20;
    if (command_count != 2 || commands[0].material != 7 || commands[1].material != 9)
        return 21;

    /* A near-plane crossing triangle should become a four-point camera-space
       polygon at z=12 without touching its source geometry. */
    {
        int16_t clipped_xyz[] = {-10, -10, 6, 10, -10, 20, 0, 10, 20};
        uint8_t clipped_stream[] = {3, 0, 5, 0, 2, 1};
        int32_t one_cull[] = {-1};
        int16_t reference_depth[] = {321};
        uint8_t reference_count[] = {3};
        uint8_t reference_accepted[] = {1};
        shape.camera_xyz = clipped_xyz;
        shape.vertex_count = 3;
        shape.primitive_stream = clipped_stream;
        shape.primitive_count = 1;
        shape.cull1 = one_cull;
        shape.cull2 = cull2;
        shape.object_flags = 0;
        if (port_camera_build_shape(&shape, commands, 8, &command_count,
                                    unsupported, 8, &unsupported_count) != PORT_CAMERA_BUILD_OK)
            return 30;
        if (command_count != 1 || commands[0].point_count != 4) return 31;
        for (size_t i = 0; i < commands[0].point_count; ++i)
            if (commands[0].xyz[i].z < PORT_CAMERA_NEAR_Z) return 32;
        if (commands[0].reference_point_count != 4) return 33;

        shape.reference_depth = reference_depth;
        shape.reference_point_count = reference_count;
        shape.reference_accepted = reference_accepted;
        if (port_camera_build_shape(&shape, commands, 8, &command_count,
                                    unsupported, 8, &unsupported_count) != PORT_CAMERA_BUILD_OK)
            return 34;
        if (command_count != 1 || commands[0].depth != 321 ||
            commands[0].reference_point_count != 3)
            return 35;

        shape.reference_accepted = NULL;
        shape.reference_depth = NULL;
        shape.reference_point_count = NULL;
        shape.filter_legacy_clip = 1;
        shape.legacy_clip.left = 0;
        shape.legacy_clip.right = 100;
        shape.legacy_clip.top = 0;
        shape.legacy_clip.bottom = 95;
        if (port_camera_build_shape(&shape, commands, 8, &command_count,
                                    unsupported, 8, &unsupported_count) != PORT_CAMERA_BUILD_OK)
            return 36;
        if (command_count != 0) return 37;
    }
    puts("renderer_camera prototype: pass");
    return 0;
}

