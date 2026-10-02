#include "renderer_modern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define check(x) do { if (!(x)) { fprintf(stderr,"sky failure line %d: %s\n",__LINE__,#x);exit(1); } } while (0)
static unsigned sample(SDL_Renderer *r,int x,int y)
{
    SDL_Surface *image=SDL_RenderReadPixels(r,NULL);check(image);
    Uint8 red,green,blue,alpha;check(SDL_ReadSurfacePixel(image,x,y,&red,&green,&blue,&alpha));
    SDL_DestroySurface(image);
    return ((unsigned)red<<16)|((unsigned)green<<8)|blue;
}
int main(void)
{
    uint8_t palette[768]={0};
    palette[3]=63;palette[7]=63;palette[11]=63;palette[12]=palette[13]=63;
    PortRenderLayer *layer=calloc(1,sizeof *layer);uint8_t *frame=calloc(64000,1);
    check(layer&&frame&&port_render_set_mode("modern"));
    layer->valid=1;layer->sky.valid=1;layer->sky.height=40;
    layer->sky.sky_color=0;layer->sky.ground_color=4;
    layer->sky.projection=(PortCameraProjection){160,100,160,100,230,171};
    layer->sky.rotation[0]=layer->sky.rotation[4]=layer->sky.rotation[8]=16384;
    for (int y=0;y<40;++y) for (int x=0;x<1024;++x)
        layer->sky.pixels[y*1024+x]=(uint8_t)(1+(x/128)%3);
    SDL_Surface *surface=SDL_CreateSurface(1680,720,SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);PortModernBackend b;
    check(r&&port_modern_init(&b,r));
    check(port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    check(sample(r,840,330)==0x0000ff); /* forward panorama */
    check(sample(r,30,330)==0x00ff00); /* distinct extra horizontal world */
    check(sample(r,840,650)==0xffff00); /* native ground */
    layer->sky.yaw=256;
    check(port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    check(sample(r,840,330)==0xff0000); /* quarter-turn panorama scroll */
    memset(layer->sky.rotation,0,sizeof layer->sky.rotation);
    layer->sky.rotation[3]=-16384;layer->sky.rotation[1]=layer->sky.rotation[8]=16384;
    check(port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    check(sample(r,1600,300)==0xffff00); /* roll rotates horizon */
    check(sample(r,30,300)==0); /* sky on other side */
    memset(layer->sky.rotation,0,sizeof layer->sky.rotation);
    layer->sky.rotation[0]=16384;layer->sky.rotation[7]=-16384;layer->sky.rotation[5]=16384;
    check(port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    check(sample(r,840,360)==0); /* camera looks straight up */
    layer->sky.rotation[7]=16384;layer->sky.rotation[5]=-16384;
    check(port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    check(sample(r,840,360)==0xffff00); /* camera looks straight down */
    layer->sky.valid=0;
    check(!port_modern_compose(&b,NULL,0,NULL,0,layer,frame,palette,1680,720));
    port_modern_destroy(&b);SDL_DestroyRenderer(r);SDL_DestroySurface(surface);free(layer);free(frame);
    return 0;
}
