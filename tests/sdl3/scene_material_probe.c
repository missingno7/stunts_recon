#include "renderer_camera_scene.h"
#include <stdio.h>
#include <stdlib.h>
#define check(x) do { if (!(x)) { fprintf(stderr,"scene failure line %d: %s\n",__LINE__,#x);exit(1); } } while (0)
static const int16_t xyz[]={-10,-10,100,-10,10,100,10,10,100,10,-10,100};
static const uint8_t primitive[]={4,1,45,0,1,2,3};
static const int32_t cull[]={-1};
static void capture(int8_t backlight)
{
    port_renderer_capture_shape(primitive,xyz,4,primitive,1,1,cull,cull,-1,0,
        0,5,0,backlight,160,100,160,100,230,171,0,320,0,200);
}
int main(void)
{
    int16_t modes[129]={0},patterns[129]={0},colors[129]={0},secondary[129]={0};
    colors[46]=17;colors[47]=23;
    check(port_render_set_mode("modern"));
    port_camera_scene_wide_begin();capture(47);port_camera_scene_wide_end();
    capture(46);check(port_camera_scene_valid());
    port_camera_scene_end(modes,patterns,colors,secondary);
    size_t count;const PortNativeCommand *cmd=port_camera_scene(&count);
    check(port_camera_scene_valid()&&count==1);
    check(cmd[0].geometry.material==46&&cmd[0].paint.material==46&&cmd[0].paint.color==17);
    port_camera_scene_wide_begin();capture(46);port_camera_scene_wide_end();
    capture(47);check(port_camera_scene_valid());
    port_camera_scene_end(modes,patterns,colors,secondary);cmd=port_camera_scene(&count);
    check(count==1&&cmd[0].paint.material==47&&cmd[0].paint.color==23);
    return 0;
}
