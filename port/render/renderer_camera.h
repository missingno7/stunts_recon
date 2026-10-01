#ifndef PORT_RENDER_CAMERA_H
#define PORT_RENDER_CAMERA_H

#include <stddef.h>
#include <stdint.h>

/* Semantic snapshot ABI is independent of the historical packed game ABI. */
#pragma pack(push, 8)
#define PORT_CAMERA_NEAR_Z 12
#define PORT_CAMERA_MAX_POINTS 10

typedef enum PortCameraPrimitiveType {
    PORT_CAMERA_FILLED_POLYGON = 0,
    PORT_CAMERA_LINE = 1,
    PORT_CAMERA_SPHERE = 2,
    PORT_CAMERA_WHEEL = 3,
    PORT_CAMERA_UNSUPPORTED = 4,
    PORT_CAMERA_PIXEL = 5
} PortCameraPrimitiveType;

typedef struct PortCameraVec3i {
    int16_t x, y, z;
} PortCameraVec3i;

typedef struct PortCameraVec3d {
    double x, y, z;
} PortCameraVec3d;

typedef struct PortCameraPoint2i {
    int16_t x, y;
} PortCameraPoint2i;

typedef struct PortCameraClipRect {
    int16_t left, right, top, bottom;
} PortCameraClipRect;

typedef struct PortCameraProjection {
    int16_t center_x, center_y;
    int16_t half_width, half_height;
    int16_t scale_x, scale_y;
} PortCameraProjection;

typedef struct PortCameraShapeCapture {
    uintptr_t shape_identity;
    const int16_t *camera_xyz;          /* vertex_count packed XYZ words */
    uint16_t vertex_count;
    const uint8_t *primitive_stream;    /* [code, flags, paints..., indices...] */
    uint16_t primitive_count;
    uint8_t paint_count;
    const int32_t *cull1;
    const int32_t *cull2;
    int32_t cullmask;
    int32_t cullbits2;
    uint8_t selected_paint;             /* already clamped as trans_op does */
    uint8_t object_flags;
    int16_t half_scale;
    int16_t backlight_override;
    PortCameraProjection projection;
    PortCameraClipRect legacy_clip;
    uint8_t filter_legacy_clip;
    uint8_t accepted_only;
    /* Optional evidence captured from accepted POLYINFO records. Arrays are
       indexed by primitive ordinal; `reference_accepted` is the valid bit. */
    const int16_t *reference_depth;
    const uint8_t *reference_point_count;
    const uint8_t *reference_accepted;
} PortCameraShapeCapture;

typedef struct PortCameraCommand {
    uintptr_t shape_identity;
    uint16_t primitive_ordinal;
    uint16_t primitive_offset;
    uint8_t source_code;
    uint8_t primitive_flags;
    uint8_t type;
    uint8_t material;
    uint8_t extra_materials[2];
    uint8_t point_count;
    uint8_t reference_point_count;
    uint8_t reference_accepted;
    int16_t depth;
    double radius;
    PortCameraVec3d xyz[PORT_CAMERA_MAX_POINTS];
    PortCameraPoint2i reference_xy[PORT_CAMERA_MAX_POINTS];
} PortCameraCommand;

typedef struct PortCameraUnsupported {
    uintptr_t shape_identity;
    uint16_t primitive_ordinal;
    uint16_t primitive_offset;
    uint8_t source_code;
    uint8_t recovered_type;
} PortCameraUnsupported;

typedef enum PortCameraBuildStatus {
    PORT_CAMERA_BUILD_OK = 0,
    PORT_CAMERA_BUILD_INVALID = -1,
    PORT_CAMERA_BUILD_COMMAND_CAPACITY = -2,
    PORT_CAMERA_BUILD_DIAGNOSTIC_CAPACITY = -3,
    PORT_CAMERA_BUILD_TOO_MANY_POINTS = -4,
    PORT_CAMERA_BUILD_BAD_INDEX = -5,
    PORT_CAMERA_BUILD_BAD_CULL_COUNT = -6,
    PORT_CAMERA_BUILD_BAD_STREAM = -7,
    PORT_CAMERA_BUILD_TOO_MANY_REFERENCE_POINTS = -8
} PortCameraBuildStatus;

/* Pure conversion: does not mutate source data, game state, or input queues.
   Output commands are already in this trans_op call's linked painter order.
   Unsupported stock type codes are reported separately and never disappear. */
PortCameraBuildStatus port_camera_build_shape(
    const PortCameraShapeCapture *capture,
    PortCameraCommand *commands, size_t command_capacity,
    size_t *command_count_out,
    PortCameraUnsupported *unsupported, size_t unsupported_capacity,
    size_t *unsupported_count_out);

#pragma pack(pop)
#endif
