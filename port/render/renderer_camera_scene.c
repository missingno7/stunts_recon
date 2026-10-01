#include "renderer_camera_scene.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Guest-owned, synchronously copied vertices. Resource pointers remain valid
   only until the world pass ends; published commands contain no such pointers. */
typedef struct Capture {
    PortCameraShapeCapture shape;
    int16_t xyz[255*3];
    int16_t reference_depth[512];
    uint8_t reference_valid[512], reference_count[512];
    int16_t clip[4];
    uint64_t hash;
    int matched;
} Capture;
static Capture s_shapes[8192];
static size_t s_shapes_count, s_count;
static int s_collecting, s_reference_only, s_capture_only, s_binding;
static size_t s_reference_target;
static int s_failed;
int port_camera_scene_valid(void) { return !s_failed; }
void port_camera_scene_reject(const char *reason)
{
    if (!s_failed) fprintf(stderr,"Modern scene rejected; faithful presentation: %s\n",reason);
    s_failed=1;s_count=0;
}
void port_camera_scene_reference_only(int enabled) { s_reference_only=enabled; }
static PortNativeCommand s_commands[PORT_NATIVE_MAX_COMMANDS];

int port_camera_scene_wide_requested(void) { return port_render_mode()==2; }
unsigned port_camera_scene_wide_extra_tiles(void) { return 12; }
int port_renderer_capture_only(void) { return s_capture_only; }
void port_camera_scene_wide_begin(void)
{
    s_binding=0;s_capture_only=1;s_shapes_count=s_count=0;s_collecting=1;s_failed=0;
}
void port_camera_scene_wide_end(void) { s_capture_only=0;s_binding=1; }
void port_camera_scene_begin(void)
{
    if (s_binding) return;
    s_shapes_count=s_count=0;s_collecting=port_render_mode()==2;s_failed=0;
}
int port_renderer_camera_capture_enabled(void) { return s_collecting; }

void port_renderer_capture_shape(const void *identity, const int16_t *xyz, uint16_t vertices,
    const uint8_t *primitives, uint16_t count, uint8_t paints,
    const int32_t *cull1, const int32_t *cull2, int32_t mask, int32_t bits,
    uint8_t paint, uint8_t flags, int16_t half, int8_t backlight,
    int16_t cx, int16_t cy, int16_t hw, int16_t hh, int16_t fx, int16_t fy,
    int16_t left, int16_t right, int16_t top, int16_t bottom)
{
    if (!s_collecting || s_failed) return;
    uint64_t hash=1469598103934665603ull;
    for (unsigned i=0;i<vertices*3u;++i) {
        hash^=(uint16_t)xyz[i];hash*=1099511628211ull;
    }
    if (s_binding) for (size_t i=0;i<s_shapes_count;++i) {
        Capture *c=&s_shapes[i];
        if (!c->matched && c->hash==hash && c->shape.shape_identity==(uintptr_t)identity &&
            c->shape.vertex_count==vertices && c->shape.selected_paint==paint &&
            c->shape.object_flags==flags && c->shape.backlight_override==backlight &&
            c->shape.projection.center_x==cx && c->shape.projection.center_y==cy &&
            c->shape.projection.half_width==hw && c->shape.projection.half_height==hh &&
            c->shape.projection.scale_x==fx && c->shape.projection.scale_y==fy &&
            !memcmp(c->xyz,xyz,vertices*6u)) {
            c->matched=1;s_reference_target=i;return;
        }
    }
    if (s_binding) {
        port_camera_scene_reject("reference shape has no exact camera-space capture");return;
    }
    if (s_shapes_count>=8192 || vertices>255 || count>512) {
        port_camera_scene_reject("shape capture capacity exceeded");return;
    }
    s_reference_target=s_shapes_count;
    Capture *c=&s_shapes[s_shapes_count++];
    memset(c,0,sizeof *c);
    c->hash=hash;c->matched=s_binding;
    memcpy(c->xyz,xyz,vertices*6u);
    c->shape=(PortCameraShapeCapture){
        .shape_identity=(uintptr_t)identity,.camera_xyz=c->xyz,.vertex_count=vertices,
        .primitive_stream=primitives,.primitive_count=count,.paint_count=paints,
        .cull1=cull1,.cull2=cull2,.cullmask=mask,.cullbits2=bits,
        .selected_paint=paint,.object_flags=flags,.half_scale=half,
        .backlight_override=backlight,.projection={cx,cy,hw,hh,fx,fy}};
    c->clip[0]=left;c->clip[1]=right;c->clip[2]=top;c->clip[3]=bottom;
}

void port_renderer_reference_primitive(unsigned ordinal, int16_t depth, uint8_t count)
{
    if (!s_collecting || !s_shapes_count || s_failed) return;
    Capture *c=&s_shapes[s_reference_target];
    if (ordinal>=c->shape.primitive_count) abort();
    c->reference_valid[ordinal]=1;c->reference_depth[ordinal]=depth;
    c->reference_count[ordinal]=count;
}

void port_camera_scene_end(const int16_t *modes, const int16_t *patterns,
                           const int16_t *colors, const int16_t *secondary)
{
    if (!s_collecting) return;
    s_collecting=0;s_binding=s_capture_only=0;
    if (s_failed) return;
    for (size_t j=0;j<s_shapes_count;++j) {
        Capture *capture=&s_shapes[j];
        capture->shape.camera_xyz=capture->xyz;
        capture->shape.reference_depth=capture->reference_depth;
        capture->shape.reference_point_count=capture->reference_count;
        capture->shape.reference_accepted=capture->reference_valid;
        capture->shape.accepted_only=(uint8_t)s_reference_only;
        PortCameraCommand commands[400];
        PortCameraUnsupported unsupported[512];
        size_t count=0,diagnostics=0;
        int result=port_camera_build_shape(&capture->shape,commands,400,&count,
                                          unsupported,512,&diagnostics);
        if (result || diagnostics || s_count+count>PORT_NATIVE_MAX_COMMANDS) {
            fprintf(stderr,"Camera command contract failure: %d, unsupported=%u\n",
                    result,(unsigned)diagnostics);
            port_camera_scene_reject("unsupported primitive or command capacity");return;
        }
        for (size_t i=0;i<count;++i) {
            PortNativeCommand *c=&s_commands[s_count++];
            memset(c,0,sizeof *c);
            c->geometry=commands[i];c->projection=capture->shape.projection;
            unsigned material=c->geometry.material;
            if (material>=129 || (c->geometry.type==3 && material+2>=129)) abort();
            c->paint.primitive=c->geometry.type;c->paint.material=(uint8_t)material;
            c->paint.depth=c->geometry.depth;c->paint.order=(uint16_t)(s_count-1);
            c->paint.color=colors[material];c->paint.secondary=secondary[material];
            c->paint.pattern_mode=modes[material];c->paint.pattern=patterns[material];
            if (c->geometry.type==3) {
                c->paint.wheel_colors[0]=colors[material+1];
                c->paint.wheel_colors[1]=colors[material+2];
            }
            memcpy(c->paint.clip,capture->clip,sizeof c->paint.clip);
        }
    }
}

size_t port_camera_scene_copy(PortNativeCommand *out,size_t capacity)
{
    if (s_collecting) return 0;
    if (out && capacity>=s_count) memcpy(out,s_commands,s_count*sizeof *out);
    return s_count;
}

const PortNativeCommand *port_camera_scene(size_t *count)
{ *count=s_collecting?0:s_count; return s_commands; }
