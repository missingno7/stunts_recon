#include "renderer_ui.h"
#include "port_runtime.h"
int port_sprite_active_view(uint8_t **base, size_t *extent, PortSprite **sprite)
{ (void)base; (void)extent; (void)sprite; return 0; }
#include <assert.h>
#include <string.h>
static uint8_t screen[64000];
uint8_t *port_video_pixels(void) { return screen; }

int main(void) {
    uint8_t window[64016];
    uint16_t rows[200];int16_t clip[4]={0,320,24,151};
    for (int y=0;y<200;++y) rows[y]=(uint16_t)(16+y*320);
    memset(window,7,sizeof window);memset(screen,7,sizeof screen);
    PortRenderLayer layer;
    port_render_world_arm();
    port_render_world_begin(window,sizeof window,rows,clip);
    port_render_ui_write(window,rows[40]+25,1); /* world drawing must not mark UI */
    port_render_world_end();
    port_render_layer_copy(&layer);
    assert(layer.valid&&layer.background[320*40+25]==7);
    assert(!layer.ui_mask[320*40+25]);
    assert(layer.ui_mask[0]&&layer.ui_mask[199*320]);
    /* Explicit identical-colour HUD write survives, unlike frame differencing. */
    port_render_ui_write(window,rows[40]+25,1);
    port_render_layer_copy(&layer);
    assert(layer.ui_mask[320*40+25]&&layer.background[320*40+25]==7);
    /* A partial world dirty rectangle is not an opaque HUD bitmap. */
    port_render_ui_copy(screen,70*320+116,window,rows[70]+116,204);
    port_render_layer_copy(&layer);
    assert(!layer.ui_mask[70*320+116]&&!layer.ui_mask[70*320+319]);
    port_render_ui_write(screen,70*320+116,1);
    port_render_layer_copy(&layer);
    assert(layer.ui_mask[70*320+116]&&!layer.ui_mask[70*320+319]);
    for (int y=0;y<200;++y) port_render_ui_copy(screen,y*320,window,rows[y],320);
    port_render_ui_write(screen,40*320+26,1);
    port_render_layer_copy(&layer);
    assert(layer.ui_mask[40*320+26]&&!layer.ui_mask[40*320+27]);
    port_render_ui_write(screen,0,64000);
    port_render_layer_copy(&layer);
    assert(!layer.valid); /* A wholly 2D screen must not retain widescreen world. */
    port_render_world_clear();port_render_layer_copy(&layer);assert(!layer.valid);
    return 0;
}
