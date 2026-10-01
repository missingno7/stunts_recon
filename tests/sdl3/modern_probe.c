#include "renderer_modern.h"
#include "pattern.h"
#include <assert.h>
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
    puts("native triangles: ordered coverage, RGB6 and 8x2 screen phase pass");
    return 0;
}
