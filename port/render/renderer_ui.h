#ifndef PORT_RENDER_UI_H
#define PORT_RENDER_UI_H
#include <stddef.h>
#include <stdint.h>

/* Guest-owned until video publishes a complete copy under its frame lock. */
typedef struct PortRenderLayer {
    int valid;
    int16_t clip[4]; /* left,right,top,bottom in the reference grid */
    uint8_t background[64000];
    uint8_t ui_mask[64000];
} PortRenderLayer;

void port_render_world_arm(void);
void port_render_world_clear(void);
void port_render_world_begin(uint8_t *base, size_t extent,
                             const uint16_t *rows, const int16_t *clip);
void port_render_world_end(void);
void port_render_world_begin_active(void);
void port_render_ui_write(uint8_t *destination, size_t offset, size_t count);
void port_render_ui_copy(uint8_t *destination, size_t destination_offset,
                         const uint8_t *source, size_t source_offset, size_t count);
void port_render_layer_copy(PortRenderLayer *out);

/* Standalone faithful probes need no render subsystem. Production and modern
   probes opt in explicitly, keeping their historical raster linkage intact. */
#ifdef PORT_RENDER_INSTRUMENT
#define PORT_UI_WRITE(base,offset,count) port_render_ui_write(base,offset,count)
#define PORT_UI_COPY(dst,dofs,src,sofs,count) port_render_ui_copy(dst,dofs,src,sofs,count)
#else
#define PORT_UI_WRITE(base,offset,count) ((void)0)
#define PORT_UI_COPY(dst,dofs,src,sofs,count) ((void)0)
#endif

#endif
