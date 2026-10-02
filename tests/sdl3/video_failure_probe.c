#include "render/renderer_modern.h"
#include "port_runtime.h"
#include <assert.h>
#include <string.h>
static int failed,stops,presents,publications,scene_valid=1,world;
void port_sprite_init(void) {}
int port_test_random_wait_status(uint8_t *status) { (void)status;return 0; }
uint64_t port_timer_machine_time_ns(void) { return SDL_GetTicksNS(); }
void port_render_world_clear(void) {}
int port_render_mode(void) { return 2; }
void port_sky_copy(PortSkyView *view) { memset(view,0,sizeof *view); }
void port_render_layer_copy(PortRenderLayer *layer) { memset(layer,0,sizeof *layer);layer->valid=world; }
size_t port_render_stream_copy(PortRenderCommand *out,size_t capacity) { (void)out;(void)capacity;return 0; }
int port_camera_scene_valid(void) { return scene_valid; }
const PortNativeCommand *port_camera_scene(size_t *count) { *count=0;return NULL; }
size_t port_camera_scene_copy(PortNativeCommand *out,size_t capacity) { (void)out;(void)capacity;return 0; }
int port_modern_init(PortModernBackend *b,SDL_Renderer *r) { (void)b;(void)r;return 1; }
void port_modern_destroy(PortModernBackend *b) { (void)b; }
int port_modern_compose(PortModernBackend *b,const PortRenderCommand *a,size_t n,
    const PortNativeCommand *c,size_t m,const PortRenderLayer *l,const uint8_t *f,
    const uint8_t *p,int w,int h)
{ (void)b;(void)a;(void)n;(void)c;(void)m;(void)l;(void)f;(void)p;(void)w;(void)h;return SDL_SetError("injected GPU failure"); }
void port_guest_set_exit_status(int status) { failed=status; }
void port_guest_request_stop(const char *reason) { assert(strstr(reason,"modern"));++stops; }
void port_guest_check_stop(void) {}
void port_guest_stop_after_publication(void) {}
void port_trace_video_publication(const char *reason) { (void)reason;++publications; }
void port_trace_host_present(uint64_t frame) { (void)frame;++presents; }
void port_trace_debug_capture(uint64_t frame,const char *directory) { (void)frame;(void)directory; }
void port_diagnostics_note(const char *label,const char *value) { (void)label;(void)value; }
int main(void)
{
    SDL_Surface *surface=SDL_CreateSurface(960,720,SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);
    assert(r&&port_video_init(r));
    port_video_publish("2D menu");port_video_present();assert(presents==1&&publications==1&&!failed);
    world=1;port_video_publish("3D world");port_video_present();
    assert(presents==1&&failed==1&&stops==1); /* no faithful world presented */
    failed=stops=0;scene_valid=0;port_video_publish("invalid scene");
    assert(publications==2&&failed==1&&stops==1); /* no invalid frame published */
    port_video_shutdown();SDL_DestroyRenderer(r);SDL_DestroySurface(surface);return 0;
}
