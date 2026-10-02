#include "renderer_sky.h"
#include "../port_runtime.h"
#include "renderer_camera_scene.h"
#include <string.h>
static PortSkyView s_sky;
extern void port_projection_snapshot(int16_t *out);
void port_sky_capture(const int16_t *rotation,int16_t yaw,int16_t camera_height,
    int16_t sky_color,int16_t ground_color,int16_t detail,
    const void *const *panels)
{
    if (port_render_mode()!=2) return;
    memset(&s_sky,0,sizeof s_sky);
    memcpy(s_sky.rotation,rotation,sizeof s_sky.rotation);
    s_sky.yaw=yaw;s_sky.camera_height=camera_height;
    s_sky.sky_color=(uint8_t)sky_color;s_sky.ground_color=(uint8_t)ground_color;
    port_projection_snapshot((int16_t *)&s_sky.projection);
    if (detail!=4) for (unsigned i=0;i<4;++i) {
        size_t extent;
        const PortShape2D *p=panels[i];
        if (!p || !port_memory_extent(p,&extent) || extent<16u ||
            (size_t)p->width*p->height>extent-16u || p->height>PORT_SKY_HEIGHT) {
            port_camera_scene_reject("invalid original panorama resource");return;
        }
        if (p->height>s_sky.height) s_sky.height=p->height;
    }
    if (!s_sky.height) s_sky.height=1;
    memset(s_sky.pixels,s_sky.sky_color,sizeof s_sky.pixels);
    /* Same ordered panel blits as skybox_op_helper2, modulo 1024. */
    static const unsigned offsets[4]={0,320,512,832};
    if (detail!=4) for (unsigned i=0;i<4;++i) {
        const PortShape2D *p=panels[i];
        const uint8_t *pixels=(const uint8_t *)p+16;
        for (unsigned y=0;y<p->height;++y) for (unsigned x=0;x<p->width && offsets[i]+x<PORT_SKY_WIDTH;++x)
            s_sky.pixels[(s_sky.height-p->height+y)*PORT_SKY_WIDTH+
                         (offsets[i]+x)]=pixels[y*p->width+x];
    }
    s_sky.valid=1;
}
void port_sky_copy(PortSkyView *view) { *view=s_sky; }
