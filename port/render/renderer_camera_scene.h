#ifndef PORT_RENDER_CAMERA_SCENE_H
#define PORT_RENDER_CAMERA_SCENE_H
#include "renderer_camera.h"
#include "renderer.h"
/* Semantic snapshot ABI is independent of the historical packed game ABI. */
#pragma pack(push, 8)
#define PORT_NATIVE_MAX_COMMANDS 65536
typedef struct PortNativeCommand {
    PortCameraCommand geometry;
    PortCameraProjection projection;
    PortRenderCommand paint;
} PortNativeCommand;
int port_camera_scene_wide_requested(void);
unsigned port_camera_scene_wide_extra_tiles(void);
void port_camera_scene_wide_begin(void);
void port_camera_scene_wide_end(void);
int port_renderer_capture_only(void);
void port_camera_scene_begin(void);
/* Native cloud translation; reference vertices remain exact for binding. */
void port_camera_scene_cloud_offset(const int16_t *delta);
int port_camera_scene_valid(void);
void port_camera_scene_reject(const char *reason);
/* Diagnostic: compare only faces independently accepted by the oracle. */
void port_camera_scene_reference_only(int enabled);
void port_camera_scene_end(const int16_t *modes, const int16_t *patterns,
                           const int16_t *colors, const int16_t *secondary);
const PortNativeCommand *port_camera_scene(size_t *count);
size_t port_camera_scene_copy(PortNativeCommand *out, size_t capacity);
int port_renderer_camera_capture_enabled(void);
void port_renderer_reference_primitive(unsigned ordinal, int16_t depth, uint8_t count);
void port_renderer_capture_shape(const void *identity, const int16_t *xyz, uint16_t vertices,
    const uint8_t *primitives, uint16_t count, uint8_t paints,
    const int32_t *cull1, const int32_t *cull2, int32_t mask, int32_t bits,
    uint8_t paint, uint8_t flags, int16_t half, int8_t backlight,
    int16_t cx, int16_t cy, int16_t hw, int16_t hh, int16_t fx, int16_t fy,
    int16_t left, int16_t right, int16_t top, int16_t bottom);
#pragma pack(pop)
#endif
