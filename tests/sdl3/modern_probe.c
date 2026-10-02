#include "renderer_modern.h"
#include "pattern.h"
#include <stdlib.h>
#define assert(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); exit(1); } } while (0)
#include <stdio.h>
#include <string.h>

int main(void)
{
    const int sizes[][2]={{960,720},{1280,720},{1001,701}};
    uint8_t palette[768]={0};
    palette[1*3]=63; palette[2*3+1]=63; palette[3*3+2]=63;
    for (unsigned mode=1;mode<=2;++mode) for (unsigned z=0;z<3;++z) {
        int w=sizes[z][0],h=sizes[z][1];
        SDL_Surface *surface=SDL_CreateSurface(w,h,SDL_PIXELFORMAT_RGBA32);
        SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);
        assert(r);
        PortModernBackend b;
        assert(port_modern_init(&b,r));
        PortRenderCommand c[2]={0};
        for (int i=0;i<2;++i) {
            c[i].primitive=PORT_RENDER_POLYGON; c[i].point_count=4;
            c[i].clip[1]=320;c[i].clip[3]=200;
            c[i].points[0]=(PortRenderPoint){10,10};
            c[i].points[1]=(PortRenderPoint){80,10};
            c[i].points[2]=(PortRenderPoint){80,80};
            c[i].points[3]=(PortRenderPoint){10,80};
        }
        c[0].color=3;
        c[1].color=1;c[1].secondary=2;c[1].pattern_mode=(uint8_t)mode;c[1].pattern=0x77dd;
        SDL_SetRenderDrawColor(r,0,0,0,255);SDL_RenderClear(r);
        assert(port_modern_draw_projected(&b,c,2,palette,w,h));
        SDL_Surface *read=SDL_RenderReadPixels(r,NULL);
        assert(read);
        PortViewport v=port_viewport_fit(w,h);
        for (int y=20;y<70;++y) for (int x=20;x<70;++x) {
            int px=(int)(v.x+(x+.5)*v.w/320),py=(int)(v.y+(y+.5)*v.h/200);
            uint8_t red,green,blue,alpha;
            assert(SDL_ReadSurfacePixel(read,px,py,&red,&green,&blue,&alpha));
            int bit=port_render_pattern_bit(c[1].pattern,x,y);
            assert(red==(bit?255:0));
            assert(green==(!bit&&mode==2?255:0));
            assert(blue==(!bit&&mode==1?255:0));
        }
        SDL_DestroySurface(read);
        port_modern_destroy(&b);SDL_DestroyRenderer(r);SDL_DestroySurface(surface);
    }

    /* Every semantic special primitive reaches the native triangle backend. */
    {
        SDL_Surface *surface=SDL_CreateSurface(960,720,SDL_PIXELFORMAT_RGBA32);
        SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);
        PortModernBackend b;assert(r&&port_modern_init(&b,r));
        PortRenderCommand commands[4]={0};
        for (int i=0;i<4;++i) {
            commands[i].color=1;commands[i].clip[1]=320;commands[i].clip[3]=200;
        }
        commands[0].primitive=PORT_RENDER_SPHERE;commands[0].point_count=2;
        commands[0].points[0]=(PortRenderPoint){50,50};
        commands[0].points[1]=(PortRenderPoint){12,0};
        commands[1].primitive=PORT_RENDER_LINE;commands[1].point_count=2;
        commands[1].points[0]=(PortRenderPoint){100,50};
        commands[1].points[1]=(PortRenderPoint){140,50};
        commands[2].primitive=PORT_RENDER_PIXEL;commands[2].point_count=1;
        commands[2].points[0]=(PortRenderPoint){180,100};
        commands[3].primitive=PORT_RENDER_WHEEL;commands[3].point_count=4;
        commands[3].points[0]=(PortRenderPoint){240,80};
        commands[3].points[1]=(PortRenderPoint){252,80};
        commands[3].points[2]=(PortRenderPoint){240,90};
        commands[3].points[3]=(PortRenderPoint){260,80};
        commands[3].wheel_colors[0]=2;commands[3].wheel_colors[1]=3;
        SDL_SetRenderDrawColor(r,0,0,0,255);SDL_RenderClear(r);
        assert(port_modern_draw_projected(&b,commands,4,palette,960,720));
        SDL_Surface *read=SDL_RenderReadPixels(r,NULL);assert(read);
        const int samples[][2]={{150,180},{360,180},{541,362}};
        for (unsigned i=0;i<3;++i) {
            uint8_t red,green,blue,alpha;
            assert(SDL_ReadSurfacePixel(read,samples[i][0],samples[i][1],&red,&green,&blue,&alpha));
            assert(red==255&&green==0&&blue==0);
        }
        unsigned wheel_pixels=0;
        for (int y=240;y<330;++y) for (int x=680;x<830;++x) {
            uint8_t red,green,blue,alpha;
            assert(SDL_ReadSurfacePixel(read,x,y,&red,&green,&blue,&alpha));
            wheel_pixels+=red||green||blue;
        }
        assert(wheel_pixels>500);
        SDL_DestroySurface(read);port_modern_destroy(&b);
        SDL_DestroyRenderer(r);SDL_DestroySurface(surface);
    }

    /* Same camera snapshot, central composition and UI at three aspect ratios.
       Side polygons were never quantized or admitted by a 320-wide viewport. */
    assert(port_render_set_mode("modern"));
    SDL_Surface *baseline=NULL;
    const int widths[]={960,1280,1680};
    PortNativeCommand native[2]={0};
    for (int i=0;i<2;++i) {
        native[i].projection=(PortCameraProjection){160,75,160,75,150,120};
        native[i].geometry.type=PORT_CAMERA_FILLED_POLYGON;
        native[i].geometry.point_count=4;
        native[i].paint.primitive=PORT_RENDER_POLYGON;native[i].paint.color=1;
        native[i].paint.clip[1]=320;native[i].paint.clip[3]=151;
        double left=i?-380:-13;
        native[i].geometry.xyz[0]=(PortCameraVec3d){left,40,300};
        native[i].geometry.xyz[1]=(PortCameraVec3d){left+30,40,300};
        native[i].geometry.xyz[2]=(PortCameraVec3d){left+30,-350,300};
        native[i].geometry.xyz[3]=(PortCameraVec3d){left,-350,300};
    }
    PortRenderLayer *layer=SDL_calloc(1,sizeof *layer);
    uint8_t *frame=SDL_malloc(64000);
    assert(layer&&frame);
    memset(frame,2,64000);memset(layer->background,3,64000);
    layer->valid=1;layer->clip[1]=320;layer->clip[3]=151;
    layer->sky.valid=1;layer->sky.height=1;
    layer->sky.sky_color=layer->sky.ground_color=3;
    layer->sky.rotation[0]=layer->sky.rotation[4]=layer->sky.rotation[8]=16384;
    layer->sky.projection=native[0].projection;
    memset(layer->sky.pixels,3,sizeof layer->sky.pixels);
    memset(layer->ui_mask+151*320,1,49*320);
    for (unsigned size=0;size<3;++size) {
        int w=widths[size];
        SDL_Surface *surface=SDL_CreateSurface(w,720,SDL_PIXELFORMAT_RGBA32);
        SDL_Renderer *r=SDL_CreateSoftwareRenderer(surface);
        PortModernBackend b;assert(port_modern_init(&b,r));
        assert(port_modern_compose(&b,NULL,0,native,2,layer,frame,palette,w,720));
        SDL_Surface *read=SDL_RenderReadPixels(r,NULL);assert(read);
        int margin=(w-960)/2;
        if (!baseline) baseline=SDL_DuplicateSurface(read);
        else {
            assert(read->format==baseline->format);
            for (int y=0;y<720;++y)
                assert(!memcmp((uint8_t *)baseline->pixels+y*baseline->pitch,
                               (uint8_t *)read->pixels+y*read->pitch+margin*4,960*4));
        }
        uint8_t red,green,blue,alpha;
        /* Dashboard stays central; the extra area contains native world below
           the old windscreen selection bottom rather than extended UI. */
        if (margin) {
            assert(SDL_ReadSurfacePixel(read,margin-75,600,&red,&green,&blue,&alpha));
            assert(red==255&&green==0&&blue==0);
            assert(SDL_ReadSurfacePixel(read,margin-10,600,&red,&green,&blue,&alpha));
            if (!(red==0&&green==0&&blue==255)) fprintf(stderr,"side background %u %u %u at width%d\n",red,green,blue,w);
            assert(red==0&&green==0&&blue==255);
        }
        /* Left boundary is x=153.5, giving a half-logical-pixel edge. */
        assert(SDL_ReadSurfacePixel(read,margin+461,240,&red,&green,&blue,&alpha));
        assert(red==255);
        SDL_DestroySurface(read);port_modern_destroy(&b);
        SDL_DestroyRenderer(r);SDL_DestroySurface(surface);
    }
    SDL_DestroySurface(baseline);SDL_free(layer);SDL_free(frame);
    puts("native triangles: ordered coverage, RGB6 and 8x2 screen phase pass");
    return 0;
}
