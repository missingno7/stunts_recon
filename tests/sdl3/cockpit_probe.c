#include "renderer_modern.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define check(x) do { if (!(x)) { fprintf(stderr,"cockpit line %d: %s\n",__LINE__,#x); exit(1); } } while (0)

static void pixel(SDL_Surface *image,int x,int y,const uint8_t *palette,int index)
{
    Uint8 r,g,b,a;
    check(SDL_ReadSurfacePixel(image,x,y,&r,&g,&b,&a));
    const uint8_t *p=palette+index*3;
    if (r!=((p[0]<<2)|(p[0]>>4)) || g!=((p[1]<<2)|(p[1]>>4)) || b!=((p[2]<<2)|(p[2]>>4)))
        fprintf(stderr,"pixel (%d,%d): %u,%u,%u expected index %d\n",x,y,r,g,b,index);
    check(r==((p[0]<<2)|(p[0]>>4)));
    check(g==((p[1]<<2)|(p[1]>>4)));
    check(b==((p[2]<<2)|(p[2]>>4)));
}

static void row_pixel(SDL_Surface *image,int x,int y,const uint8_t *palette,int index)
{
    Uint8 r,g,b,a;
    check(SDL_ReadSurfacePixel(image,x,y,&r,&g,&b,&a));
    /* SDL software texture clipping rounds at fractional source/destination
       edges. Permit one neighbouring row texel, not a different band, scale
       or opacity; stripe and world-clearance assertions remain exact. */
    for (int n=index-1;n<=index+1;++n) {
        if (n<0||n>=200) continue;
        const uint8_t *p=palette+n*3;
        if (r==((p[0]<<2)|(p[0]>>4)) && g==((p[1]<<2)|(p[1]>>4)) && b==((p[2]<<2)|(p[2]>>4))) return;
    }
    fprintf(stderr,"row pixel (%d,%d): %u,%u,%u expected row %d +/-1\n",x,y,r,g,b,index);
    check(0);
}

int main(void)
{
    const int sizes[][2]={{960,720},{1280,720},{1680,720},{1001,701},{720,960}};
    uint8_t palette[768],frame[64000];
    for (int i=0;i<256;++i) {
        palette[i*3]=(uint8_t)(i&63);
        palette[i*3+1]=(uint8_t)((i>>2)&63);
        palette[i*3+2]=(uint8_t)(63-(i&63));
    }
    PortRenderLayer *layer=calloc(1,sizeof *layer);check(layer);
    layer->valid=1;layer->clip[1]=320;layer->clip[2]=24;layer->clip[3]=151;
    layer->sky.valid=1;layer->sky.height=1;
    layer->sky.sky_color=layer->sky.ground_color=250;
    layer->sky.rotation[0]=layer->sky.rotation[4]=layer->sky.rotation[8]=16384;
    layer->sky.projection=(PortCameraProjection){160,100,160,100,230,171};
    memset(layer->sky.pixels,250,sizeof layer->sky.pixels);
    for (int y=0;y<200;++y) for (int x=0;x<320;++x) {
        frame[y*320+x]=(uint8_t)y;
        layer->ui_mask[y*320+x]=(uint8_t)(y<24||y>=151);
        /* A vertical stripe checks horizontal scaling; a clear stripe checks
           alpha provenance rather than painting the whole band opaque. */
        if (x>=40&&x<60) frame[y*320+x]=240;
        if (x>=80&&x<100) layer->ui_mask[y*320+x]=0;
    }
    layer->ui_mask[80*320+160]=1;frame[80*320+160]=245;
    check(port_render_set_mode("modern"));
    for (unsigned i=0;i<sizeof sizes/sizeof sizes[0];++i) {
        int w=sizes[i][0],h=sizes[i][1];
        PortViewport v=port_viewport_fit(w,h);
        float k=w/v.w;
        SDL_Surface *surface=SDL_CreateSurface(w,h,SDL_PIXELFORMAT_RGBA32);
        SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);PortModernBackend backend;
        check(r&&port_modern_init(&backend,r));
        SDL_SetRenderDrawColor(r,0,0,0,255);check(SDL_RenderClear(r));
        check(port_modern_compose(&backend,NULL,0,NULL,0,layer,frame,palette,w,h));
        SDL_Surface *image=SDL_RenderReadPixels(r,NULL);check(image);
        int roof_end=(int)floor(v.y+24*v.h/200);
        int dash_start=(int)ceil(v.y+151*v.h/200);
        for (int y=(int)ceil(v.y)+2;y<roof_end-2;++y) {
            double source=24+(y+.5-(v.y+24*v.h/200))/(v.h*k/200);
            row_pixel(image,w/2,y,palette,(int)floor(source));
        }
        for (int y=dash_start+2;y<(int)(v.y+v.h)-2;++y) {
            double source=151+(y+.5-(v.y+151*v.h/200))/(v.h*k/200);
            row_pixel(image,w/2,y,palette,(int)floor(source));
        }
        int ys[]={roof_end-5,dash_start+5};
        for (int band=0;band<2;++band) {
            pixel(image,(int)(50*w/320.0),ys[band],palette,240);
            pixel(image,(int)(90*w/320.0),ys[band],palette,250);
            /* Both output edges are covered by the same cockpit source. */
            double source=band?151+(ys[band]+.5-(v.y+151*v.h/200))/(v.h*k/200):
                24+(ys[band]+.5-(v.y+24*v.h/200))/(v.h*k/200);
            row_pixel(image,0,ys[band],palette,(int)floor(source));
            row_pixel(image,w-1,ys[band],palette,(int)floor(source));
        }
        pixel(image,w/2,roof_end+3,palette,250);
        pixel(image,w/2,dash_start-3,palette,250);
        pixel(image,(int)(v.x+160.5*v.w/320),(int)(v.y+80.5*v.h/200),palette,245);
        /* The inverse transform returns every visible HUD texel centre,
           including HiDPI windows, without moving world-area coordinates. */
        for (int y=0;y<200;++y) for (int x=0;x<320;++x) {
            double px=v.x+(x+.5)*v.w/320;
            double py=v.y+(y+.5)*v.h/200;
            if (w>v.w && (y<24||y>=151)) {
                int edge=y<24?24:151;
                px=(x+.5)*w/320;
                py=v.y+edge*v.h/200+(y+.5-edge)*v.h*k/200;
            }
            if (py<v.y||py>=v.y+v.h) continue;
            int lx,ly;
            port_viewport_cockpit_mouse(w/2,h/2,w,h,
                (float)(px*(w/2)/w),(float)(py*(h/2)/h),24,151,&lx,&ly);
            check(lx==x&&ly==y);
        }
        SDL_DestroySurface(image);port_modern_destroy(&backend);
        SDL_DestroyRenderer(r);SDL_DestroySurface(surface);
    }
    free(layer);return 0;
}
