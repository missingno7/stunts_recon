#include "renderer_modern.h"
#include "pattern.h"
#include "triangulate2d.h"
#include "modern_wheel.h"
#include "edge_chain_tessellator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static SDL_FColor rgb(const uint8_t *palette, unsigned index)
{
    const uint8_t *p = palette + (index & 255u) * 3;
    SDL_FColor color;
    color.r = ((p[0] << 2) | (p[0] >> 4)) / 255.0f;
    color.g = ((p[1] << 2) | (p[1] >> 4)) / 255.0f;
    color.b = ((p[2] << 2) | (p[2] >> 4)) / 255.0f;
    color.a = 1;
    return color;
}

int port_modern_init(PortModernBackend *b, SDL_Renderer *renderer)
{
    memset(b, 0, sizeof(*b));
    b->renderer = renderer;
    b->pattern = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                  SDL_TEXTUREACCESS_STREAMING, 8, 2);
    if (!b->pattern) return 0;
    SDL_SetTextureBlendMode(b->pattern, SDL_BLENDMODE_BLEND);
    return SDL_SetTextureScaleMode(b->pattern, SDL_SCALEMODE_NEAREST);
}

void port_modern_destroy(PortModernBackend *b)
{
    SDL_DestroyTexture(b->background);
    SDL_DestroyTexture(b->ui);
    SDL_DestroyTexture(b->pattern);
    SDL_DestroyTexture(b->panorama);
    free(b->mesh);
    free(b->rgba);
    free(b->sky_rgba);
    memset(b, 0, sizeof(*b));
}

static int reserve(PortModernBackend *b, size_t count)
{
    if (count <= b->capacity) return 1;
    size_t cap = b->capacity ? b->capacity * 2 : 1024;
    if (cap < count) cap = count;
    SDL_Vertex *next = realloc(b->mesh, cap * sizeof(*next));
    if (!next) return 0;
    b->mesh = next; b->capacity = cap;
    return 1;
}

static int vertex(PortModernBackend *b, double x, double y, double u, double t,
                  PortViewport v, SDL_FColor color)
{
    if (!reserve(b, b->vertices + 1)) return 0;
    SDL_Vertex *out = &b->mesh[b->vertices++];
    out->position = (SDL_FPoint){v.x + (float)(x * v.w / 320),
                                v.y + (float)(y * v.h / 200)};
    out->color = color;
    out->tex_coord = (SDL_FPoint){(float)u, (float)t};
    return 1;
}

/* Intersection of a triangle and a reference tile is convex, so fans here
   are proven safe. Original resource polygons are triangulated separately. */
static int clip_axis(const PortTriPoint *in, int count, PortTriPoint *out,
                     int axis, double edge, int keep_greater)
{
    int n = 0;
    for (int i = 0; i < count; ++i) {
        PortTriPoint a = in[(i + count - 1) % count], c = in[i];
        double av = axis ? a.y : a.x, cv = axis ? c.y : c.x;
        int ai = keep_greater ? av >= edge : av <= edge;
        int ci = keep_greater ? cv >= edge : cv <= edge;
        if (ai != ci) {
            double t = (edge - av) / (cv - av);
            out[n++] = (PortTriPoint){a.x + t * (c.x - a.x), a.y + t * (c.y - a.y)};
        }
        if (ci) out[n++] = c;
    }
    return n;
}

static int triangle(PortModernBackend *b, const PortTriPoint *p, PortViewport v,
                    SDL_FColor color, int patterned, double minx, double maxx,
                    double miny, double maxy)
{
    PortTriPoint work[12], temp[12];
    memcpy(work, p, 3 * sizeof(*p));
    int n = clip_axis(work, 3, temp, 0, minx, 1);
    n = clip_axis(temp, n, work, 0, maxx, 0);
    n = clip_axis(work, n, temp, 1, miny, 1);
    n = clip_axis(temp, n, work, 1, maxy, 0);
    if (n < 3) return 1;
    if (!patterned) {
        for (int i = 1; i + 1 < n; ++i) {
            if (!vertex(b, work[0].x, work[0].y, 0, 0, v, color) ||
                !vertex(b, work[i].x, work[i].y, 0, 0, v, color) ||
                !vertex(b, work[i+1].x, work[i+1].y, 0, 0, v, color)) return 0;
        }
        return 1;
    }
    double left = work[0].x, right = left, top = work[0].y, bottom = top;
    for (int i = 1; i < n; ++i) {
        left = fmin(left, work[i].x); right = fmax(right, work[i].x);
        top = fmin(top, work[i].y); bottom = fmax(bottom, work[i].y);
    }
    for (int ty = (int)floor(top / 2); ty < (int)ceil(bottom / 2); ++ty)
        for (int tx = (int)floor(left / 8); tx < (int)ceil(right / 8); ++tx) {
            PortTriPoint tile[12], tmp[12];
            int k = clip_axis(work, n, tmp, 0, tx * 8, 1);
            k = clip_axis(tmp, k, tile, 0, tx * 8 + 8, 0);
            k = clip_axis(tile, k, tmp, 1, ty * 2, 1);
            k = clip_axis(tmp, k, tile, 1, ty * 2 + 2, 0);
            for (int i = 1; i + 1 < k; ++i) {
                const int indices[3] = {0, i, i+1};
                for (int j = 0; j < 3; ++j) {
                    PortTriPoint point = tile[indices[j]];
                    if (!vertex(b, point.x, point.y,
                                (point.x - tx*8) / 8, (point.y - ty*2) / 2,
                                v, (SDL_FColor){1,1,1,1})) return 0;
                }
            }
        }
    return 1;
}

static int pattern_texture(PortModernBackend *b, const PortRenderCommand *c,
                           const uint8_t *palette)
{
    if (b->pattern_valid && b->cached_pattern == c->pattern &&
        b->cached_mode == c->pattern_mode && b->cached_color == c->color &&
        b->cached_secondary == c->secondary && !memcmp(b->cached_palette, palette, 768)) return 1;
    uint8_t rgba[64];
    for (int y = 0; y < 2; ++y) for (int x = 0; x < 8; ++x) {
        int set = port_render_pattern_bit(c->pattern, x, y);
        unsigned color = set ? c->color : c->secondary;
        for (int channel = 0; channel < 3; ++channel) {
            unsigned value = palette[color * 3 + channel];
            rgba[(y*8+x)*4+channel] = (uint8_t)((value << 2) | (value >> 4));
        }
        rgba[(y*8+x)*4+3] = set || c->pattern_mode == PORT_RENDER_DITHER ? 255 : 0;
    }
    if (!SDL_UpdateTexture(b->pattern, NULL, rgba, 32)) return 0;
    b->cached_pattern = c->pattern; b->cached_mode = c->pattern_mode;
    b->cached_color = c->color; b->cached_secondary = c->secondary;
    memcpy(b->cached_palette, palette, 768); b->pattern_valid = 1;
    return 1;
}

static int polygon(PortModernBackend *b, const PortTriPoint *p, int count,
                   const PortRenderCommand *c, const uint8_t *palette,
                   PortViewport v, int width, int height)
{
    b->vertices = 0;
    double minx = -v.x * 320 / v.w, maxx = (width - v.x) * 320 / v.w;
    double miny = -v.y * 200 / v.h, maxy = (height - v.y) * 200 / v.h;
    minx = fmax(minx,c->clip[0]); maxx = fmin(maxx,c->clip[1]);
    miny = fmax(miny,c->clip[2]); maxy = fmin(maxy,c->clip[3]);
    double left=p[0].x,right=left,top=p[0].y,bottom=top;
    for (int i=1;i<count;++i) {
        left=fmin(left,p[i].x);right=fmax(right,p[i].x);
        top=fmin(top,p[i].y);bottom=fmax(bottom,p[i].y);
    }
    if (right+1<minx || left-1>maxx || bottom+1<miny || top-1>maxy) return 1;
    unsigned indices[3 * 64];
    int triangles = port_triangulate(p, count, indices, 3 * 64);
    SDL_FColor color = rgb(palette, c->color);
    if (triangles<=0) {
        EdgeChainPoint contour[20];
        EdgeChainSpan spans[EDGE_CHAIN_MAX_SPANS];
        for (int i=0;i<count;++i) contour[i]=(EdgeChainPoint){p[i].x,p[i].y};
        int n=edge_chain_tessellate(contour,(size_t)count,spans,EDGE_CHAIN_MAX_SPANS);
        if (n<0) { SDL_SetError("Edge-chain contract failure %d",n);return 0; }
        for (int i=0;i<n;++i) {
            PortTriPoint points[4]={{spans[i].top_left.x,spans[i].top_left.y},
                {spans[i].top_right.x,spans[i].top_right.y},
                {spans[i].bottom_right.x,spans[i].bottom_right.y},
                {spans[i].bottom_left.x,spans[i].bottom_left.y}};
            PortTriPoint a[3]={points[0],points[1],points[2]};
            PortTriPoint d[3]={points[0],points[2],points[3]};
            if (!triangle(b,a,v,color,c->pattern_mode!=0,minx,maxx,miny,maxy) ||
                !triangle(b,d,v,color,c->pattern_mode!=0,minx,maxx,miny,maxy)) return 0;
        }
    } else for (int i = 0; i < triangles; ++i) {
        PortTriPoint points[3] = {p[indices[i*3]],p[indices[i*3+1]],p[indices[i*3+2]]};
        if (!triangle(b, points, v, color, c->pattern_mode != 0, minx,maxx,miny,maxy)) return 0;
    }
    if (!b->vertices) return 1;
    if (c->pattern_mode && !pattern_texture(b, c, palette)) return 0;
    return SDL_RenderGeometry(b->renderer, c->pattern_mode ? b->pattern : NULL,
                              b->mesh, (int)b->vertices, NULL, 0);
}

typedef struct WheelDraw {
    PortModernBackend *backend;
    PortRenderCommand command;
    const uint8_t *palette;
    PortViewport viewport;
    int width,height,ok;
} WheelDraw;

static void wheel_face(void *user, const ModernWheelPoint *points, size_t count, int color)
{
    WheelDraw *draw=user;
    PortTriPoint xy[20];
    for (size_t i=0;i<count;++i) xy[i]=(PortTriPoint){points[i].x,points[i].y};
    draw->command.color=(uint8_t)color;
    draw->command.pattern_mode=0;
    if (!polygon(draw->backend,xy,(int)count,&draw->command,draw->palette,
                 draw->viewport,draw->width,draw->height)) draw->ok=0;
}

static int draw_command(PortModernBackend *b, const PortRenderCommand *c,
                        const PortTriPoint *xy, double radius, const uint8_t *palette,
                        PortViewport v, int width, int height)
{
        if (c->primitive == PORT_RENDER_POLYGON) {
            if (c->pattern_mode == PORT_RENDER_STIPPLE && !c->pattern) return 1;
            PortTriPoint points[10];
            for (int j=0;j<c->point_count;++j) points[j]=(PortTriPoint){xy[j].x,xy[j].y};
            if (!polygon(b,points,c->point_count,c,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_LINE) {
            double dx=xy[1].x-xy[0].x;
            double dy=xy[1].y-xy[0].y;
            double length=hypot(dx,dy);
            double nx=length ? -dy*.5/length : .5;
            double ny=length ? dx*.5/length : 0;
            PortTriPoint points[4] = {
                {xy[0].x+nx,xy[0].y+ny},
                {xy[1].x+nx,xy[1].y+ny},
                {xy[1].x-nx,xy[1].y-ny},
                {xy[0].x-nx,xy[0].y-ny}
            };
            PortRenderCommand solid=*c; solid.pattern_mode=0;
            if (!polygon(b,points,4,&solid,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_PIXEL) {
            double x=xy[0].x,y=xy[0].y;
            PortTriPoint points[4]={{x,y},{x+1,y},{x+1,y+1},{x,y+1}};
            PortRenderCommand solid=*c; solid.pattern_mode=0;
            if (!polygon(b,points,4,&solid,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_SPHERE) {
            double x=xy[0].x,y=xy[0].y,r=radius;
            b->vertices=0;
            SDL_FColor color=rgb(palette,c->color);
            for (int j=0;j<64;++j) {
                double a=j*6.283185307179586/64, next=(j+1)*6.283185307179586/64;
                PortTriPoint points[3]={{x,y},{x+r*cos(a),y+r*sin(a)},
                                       {x+r*cos(next),y+r*sin(next)}};
                if (!triangle(b,points,v,color,0,c->clip[0],c->clip[1],c->clip[2],c->clip[3])) return 0;
            }
            if (b->vertices && !SDL_RenderGeometry(b->renderer,NULL,b->mesh,(int)b->vertices,NULL,0)) return 0;
        } else if (c->primitive == PORT_RENDER_WHEEL) {
            ModernWheelPoint points[4];
            for (int j=0;j<4;++j) points[j]=(ModernWheelPoint){xy[j].x,xy[j].y};
            WheelDraw draw={b,*c,palette,v,width,height,1};
            if (!modern_wheel_emit(points,c->color,c->wheel_colors[0],c->wheel_colors[1],wheel_face,&draw) || !draw.ok) return 0;
        } else {
            SDL_SetError("Unsupported semantic primitive %u",c->primitive);
            return 0;
        }
    return 1;
}

int port_modern_draw_projected(PortModernBackend *b, const PortRenderCommand *cmd,
                              size_t count, const uint8_t *palette, int width, int height)
{
    PortViewport v=port_viewport_fit(width,height);
    if (v.w<=0 || v.h<=0) return 1;
    for (size_t i=0;i<count;++i) {
        PortTriPoint xy[10]={{0,0}};
        for (unsigned j=0;j<cmd[i].point_count;++j)
            xy[j]=(PortTriPoint){cmd[i].points[j].x,cmd[i].points[j].y};
        if (!draw_command(b,&cmd[i],xy,xy[1].x,palette,v,width,height)) return 0;
    }
    return 1;
}

int port_modern_draw_camera(PortModernBackend *b, const PortNativeCommand *commands,
                           size_t count, const uint8_t *palette, int width,int height)
{
    PortViewport v=port_viewport_fit(width,height);
    if (v.w<=0 || v.h<=0) return 1;
    for (size_t i=0;i<count;++i) {
        const PortNativeCommand *c=&commands[i];
        PortTriPoint xy[10];
        for (unsigned j=0;j<c->geometry.point_count;++j) {
            PortCameraVec3d p=c->geometry.xyz[j];
            xy[j]=(PortTriPoint){c->projection.center_x+p.x*c->projection.scale_x/p.z,
                                 c->projection.center_y-p.y*c->projection.scale_y/p.z};
        }
        PortRenderCommand paint=c->paint;
        paint.point_count=c->geometry.point_count;
        /* Render the complete world behind the independently composited
           cockpit bands. Projection framing stays fixed. */
        paint.clip[0]=-32768;paint.clip[1]=32767;
        paint.clip[2]=0;paint.clip[3]=200;
        double radius=c->geometry.radius*c->projection.scale_x/c->geometry.xyz[0].z;
        if (!draw_command(b,&paint,xy,radius,palette,v,width,height)) return 0;
    }
    return 1;
}

typedef struct SkyVertex { double x,y,z,u,v; } SkyVertex;
static SkyVertex sky_lerp(SkyVertex a,SkyVertex c,double t)
{
    return (SkyVertex){a.x+t*(c.x-a.x),a.y+t*(c.y-a.y),a.z+t*(c.z-a.z),
                       a.u+t*(c.u-a.u),a.v+t*(c.v-a.v)};
}
static double sky_plane(SkyVertex p,int plane,const PortSkyView *s,
                        double left,double right,double top,double bottom)
{
    switch (plane) {
    case 0:return p.z-.001;
    case 1:return (s->projection.center_x-left)*p.z+s->projection.scale_x*p.x;
    case 2:return (right-s->projection.center_x)*p.z-s->projection.scale_x*p.x;
    case 3:return (s->projection.center_y-top)*p.z-s->projection.scale_y*p.y;
    default:return (bottom-s->projection.center_y)*p.z+s->projection.scale_y*p.y;
    }
}
static int sky_clip(SkyVertex *points,int n,const PortSkyView *s,
                     double left,double right,double top,double bottom)
{
    SkyVertex temp[16];
    for (int plane=0;plane<5&&n;++plane) {
        int count=0;
        for (int i=0;i<n;++i) {
            SkyVertex a=points[(i+n-1)%n],c=points[i];
            double da=sky_plane(a,plane,s,left,right,top,bottom);
            double dc=sky_plane(c,plane,s,left,right,top,bottom);
            if ((da>=0)!=(dc>=0)) temp[count++]=sky_lerp(a,c,da/(da-dc));
            if (dc>=0) temp[count++]=c;
        }
        n=count;memcpy(points,temp,(size_t)n*sizeof *points);
    }
    return n;
}
static SkyVertex sky_point(const PortSkyView *s,double u,double v)
{
    /* Original panorama wraps at 1024 yaw units. The original forward center
       samples center_x+512-yaw; turn that angular atlas into a real cylinder. */
    double theta=(u*1024-s->projection.center_x-512+s->yaw)*6.283185307179586/1024;
    double p[3]={sin(theta),(s->height-v*s->height)/(double)s->projection.scale_y-
                              s->camera_height/15000.0,cos(theta)};
    SkyVertex out={0,0,0,u,v};
    out.x=(s->rotation[0]*p[0]+s->rotation[3]*p[1]+s->rotation[6]*p[2])/16384.0;
    out.y=(s->rotation[1]*p[0]+s->rotation[4]*p[1]+s->rotation[7]*p[2])/16384.0;
    out.z=(s->rotation[2]*p[0]+s->rotation[5]*p[1]+s->rotation[8]*p[2])/16384.0;
    return out;
}
static int draw_native_sky(PortModernBackend *b,const PortSkyView *s,
                           const uint8_t *palette,int width,int height)
{
    if (!s->valid || s->height>PORT_SKY_HEIGHT || !s->height ||
        s->projection.scale_x<=0 || s->projection.scale_y<=0)
        return SDL_SetError("missing or invalid native sky snapshot");
    PortViewport v=port_viewport_fit(width,height);
    SDL_FColor sky=rgb(palette,s->sky_color),ground=rgb(palette,s->ground_color);
    if (!SDL_SetRenderDrawColorFloat(b->renderer,sky.r,sky.g,sky.b,1) ||
        !SDL_RenderFillRect(b->renderer,NULL)) return 0;
    double left=-v.x*320/v.w,right=(width-v.x)*320/v.w;
    double top=-v.y*200/v.h,bottom=(height-v.y)*200/v.h;
    /* Clip the ground hemisphere against the camera's world-up halfplane.
       Pitch and roll come from the original Q14 matrix; no raster horizon. */
    PortTriPoint rect[4]={{left,top},{right,top},{right,bottom},{left,bottom}},poly[8];
    int n=0;
    for (int i=0;i<4;++i) {
        PortTriPoint a=rect[(i+3)%4],c=rect[i];
        double da=s->rotation[3]*(a.x-s->projection.center_x)/s->projection.scale_x-
                  s->rotation[4]*(a.y-s->projection.center_y)/s->projection.scale_y+s->rotation[5];
        double dc=s->rotation[3]*(c.x-s->projection.center_x)/s->projection.scale_x-
                  s->rotation[4]*(c.y-s->projection.center_y)/s->projection.scale_y+s->rotation[5];
        if ((da<=0)!=(dc<=0)) {
            double t=da/(da-dc);poly[n++]=(PortTriPoint){a.x+t*(c.x-a.x),a.y+t*(c.y-a.y)};
        }
        if (dc<=0) poly[n++]=c;
    }
    b->vertices=0;
    for (int i=1;i+1<n;++i) {
        int ids[3]={0,i,i+1};
        for (int k=0;k<3;++k) if (!vertex(b,poly[ids[k]].x,poly[ids[k]].y,0,0,v,ground)) return 0;
    }
    if (b->vertices && !SDL_RenderGeometry(b->renderer,NULL,b->mesh,(int)b->vertices,NULL,0)) return 0;
    if (!b->panorama) b->panorama=SDL_CreateTexture(b->renderer,SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,PORT_SKY_WIDTH,PORT_SKY_HEIGHT);
    if (!b->sky_rgba) b->sky_rgba=malloc(PORT_SKY_WIDTH*PORT_SKY_HEIGHT*4u);
    if (!b->panorama || !b->sky_rgba) return 0;
    for (unsigned i=0;i<PORT_SKY_WIDTH*PORT_SKY_HEIGHT;++i) {
        for (unsigned channel=0;channel<3;++channel) {
            unsigned value=palette[s->pixels[i]*3+channel];
            b->sky_rgba[i*4+channel]=(uint8_t)((value<<2)|(value>>4));
        }
        b->sky_rgba[i*4+3]=255;
    }
    if (!SDL_SetTextureScaleMode(b->panorama,SDL_SCALEMODE_NEAREST) ||
        !SDL_UpdateTexture(b->panorama,NULL,b->sky_rgba,PORT_SKY_WIDTH*4)) return 0;
    b->vertices=0;
    SDL_FColor white={1,1,1,1};
    /* Small angular/vertical cells bound affine interpolation error in SDL's
       triangle API. Geometry and frustum clipping are native resolution. */
    for (int x=0;x<256;++x) for (int y=0;y<8;++y) {
        SkyVertex p[16]={sky_point(s,x/256.0,y/8.0),sky_point(s,(x+1)/256.0,y/8.0),
                        sky_point(s,(x+1)/256.0,(y+1)/8.0),sky_point(s,x/256.0,(y+1)/8.0)};
        int count=sky_clip(p,4,s,left,right,top,bottom);
        for (int i=1;i+1<count;++i) {
            int ids[3]={0,i,i+1};
            for (int k=0;k<3;++k) {
                SkyVertex a=p[ids[k]];
                double px=s->projection.center_x+s->projection.scale_x*a.x/a.z;
                double py=s->projection.center_y-s->projection.scale_y*a.y/a.z;
                /* Texture storage has fixed height; visible asset is top aligned. */
                if (!vertex(b,px,py,a.u,a.v*s->height/PORT_SKY_HEIGHT,v,white)) return 0;
            }
        }
    }
    return !b->vertices || SDL_RenderGeometry(b->renderer,b->panorama,b->mesh,(int)b->vertices,NULL,0);
}

static int draw_ui(PortModernBackend *b, const PortRenderLayer *layer,
                   PortViewport v, int width)
{
    SDL_FRect destination={v.x,v.y,v.w,v.h};
    if (port_render_mode()!=2 || width<=v.w || v.w<=0 || v.h<=0)
        return SDL_RenderTexture(b->renderer,b->ui,NULL,&destination);

    int top=layer->clip[2], bottom=layer->clip[3];
    if (top<0) top=0;
    if (top>200) top=200;
    if (bottom<top) bottom=top;
    if (bottom>200) bottom=200;
    float scale=(float)width/v.w;
    /* Keep the inner edges of the cockpit at their reference heights. The
       scaled roof overflows above the viewport; the dashboard overflows
       below it. Both use the same additional x/y scale after pixel-aspect
       correction. Transparent pixels still reveal the expanded world. */
    if (top>0) {
        SDL_FRect source={0,0,320,(float)top};
        float h=top*v.h/200*scale;
        SDL_FRect target={0,v.y+top*v.h/200-h,(float)width,h};
        if (!SDL_RenderTexture(b->renderer,b->ui,&source,&target)) return 0;
    }
    if (bottom>top) {
        SDL_FRect source={0,(float)top,320,(float)(bottom-top)};
        SDL_FRect target={v.x,v.y+top*v.h/200,v.w,(bottom-top)*v.h/200};
        if (!SDL_RenderTexture(b->renderer,b->ui,&source,&target)) return 0;
    }
    if (bottom<200) {
        SDL_FRect source={0,(float)bottom,320,(float)(200-bottom)};
        SDL_FRect target={0,v.y+bottom*v.h/200,(float)width,
                          (200-bottom)*v.h/200*scale};
        if (!SDL_RenderTexture(b->renderer,b->ui,&source,&target)) return 0;
    }
    return 1;
}

int port_modern_compose(PortModernBackend *b, const PortRenderCommand *cmd, size_t count,
                        const PortNativeCommand *native, size_t native_count,
                        const PortRenderLayer *layer, const uint8_t *frame,
                        const uint8_t *palette, int width, int height)
{
    int native_mode=port_render_mode()==2;
    if (!native_mode && !b->background) b->background=SDL_CreateTexture(b->renderer,SDL_PIXELFORMAT_RGBA32,
                                                       SDL_TEXTUREACCESS_STREAMING,320,200);
    if (!b->ui) b->ui=SDL_CreateTexture(b->renderer,SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STREAMING,320,200);
    if ((!native_mode && !b->background) || !b->ui) return 0;
    if (!native_mode) SDL_SetTextureScaleMode(b->background,SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(b->ui,SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(b->ui,SDL_BLENDMODE_BLEND);
    if (!b->rgba) b->rgba=malloc(64000u*8u);
    if (!b->rgba) return 0;
    uint8_t *background=b->rgba, *ui=b->rgba+64000u*4u;
    for (unsigned i=0;i<64000;++i) {
        for (int channel=0;channel<3;++channel) {
            unsigned value;
            if (!native_mode) {
                value=palette[layer->background[i]*3+channel];
                background[i*4+channel]=(uint8_t)((value<<2)|(value>>4));
            }
            value=palette[frame[i]*3+channel];
            ui[i*4+channel]=(uint8_t)((value<<2)|(value>>4));
        }
        background[i*4+3]=255;
        ui[i*4+3]=layer->ui_mask[i]?255:0;
    }
    if ((!native_mode && !SDL_UpdateTexture(b->background,NULL,background,320*4)) ||
        !SDL_UpdateTexture(b->ui,NULL,ui,320*4)) return 0;
    PortViewport v=port_viewport_fit(width,height);
    SDL_FRect destination={v.x,v.y,v.w,v.h};
    if (port_render_mode()==2) {
        if (!draw_native_sky(b,&layer->sky,palette,width,height) ||
            !port_modern_draw_camera(b,native,native_count,palette,width,height)) return 0;
    } else if (!SDL_RenderTexture(b->renderer,b->background,NULL,&destination) ||
        !port_modern_draw_projected(b,cmd,count,palette,width,height)) return 0;
    return draw_ui(b,layer,v,width);
}
