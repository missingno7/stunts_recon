#ifndef PORT_RENDER_SKY_H
#define PORT_RENDER_SKY_H
#include "renderer_camera.h"
#pragma pack(push,8)
#define PORT_SKY_WIDTH 1024
#define PORT_SKY_HEIGHT 256
/* Original panorama assets, independent of the legacy screen raster. */
typedef struct PortSkyView {
    int valid;
    int16_t rotation[9], yaw, camera_height;
    uint16_t height;
    uint8_t sky_color, ground_color;
    PortCameraProjection projection;
    uint8_t pixels[PORT_SKY_WIDTH*PORT_SKY_HEIGHT];
} PortSkyView;
void port_sky_capture(const int16_t *rotation, int16_t yaw, int16_t camera_height,
                     int16_t sky_color, int16_t ground_color, int16_t detail,
                     const void *const *panels);
void port_sky_copy(PortSkyView *view);
#pragma pack(pop)
#endif
