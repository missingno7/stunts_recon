#include "renderer_modern.h"
#include "pattern.h"
#include "triangulate2d.h"
#include "modern_wheel.h"
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
    free(b->mesh);
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
    unsigned indices[3 * 64];
    int triangles = port_triangulate(p, count, indices, 3 * 64);
    if (triangles < 0) {
        SDL_SetError("Primitive triangulation failed: %d (material %u)",triangles,c->material);
        return 0;
    }
    b->vertices = 0;
    double minx = -v.x * 320 / v.w, maxx = (width - v.x) * 320 / v.w;
    double miny = -v.y * 200 / v.h, maxy = (height - v.y) * 200 / v.h;
    minx = fmax(minx,c->clip[0]); maxx = fmin(maxx,c->clip[1]);
    miny = fmax(miny,c->clip[2]); maxy = fmin(maxy,c->clip[3]);
    SDL_FColor color = rgb(palette, c->color);
    for (int i = 0; i < triangles; ++i) {
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

int port_modern_draw_projected(PortModernBackend *b, const PortRenderCommand *cmd,
                              size_t count, const uint8_t *palette,
                              int width, int height)
{
    PortViewport v = port_viewport_fit(width,height);
    if (v.w <= 0 || v.h <= 0) return 1;
    for (size_t i = 0; i < count; ++i) {
        const PortRenderCommand *c = &cmd[i];
        if (c->primitive == PORT_RENDER_POLYGON) {
            if (c->pattern_mode == PORT_RENDER_STIPPLE && !c->pattern) continue;
            PortTriPoint points[10];
            for (int j=0;j<c->point_count;++j) points[j]=(PortTriPoint){c->points[j].x,c->points[j].y};
            if (!polygon(b,points,c->point_count,c,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_LINE) {
            double dx=c->points[1].x-c->points[0].x;
            double dy=c->points[1].y-c->points[0].y;
            double length=hypot(dx,dy);
            double nx=length ? -dy*.5/length : .5;
            double ny=length ? dx*.5/length : 0;
            PortTriPoint points[4] = {
                {c->points[0].x+nx,c->points[0].y+ny},
                {c->points[1].x+nx,c->points[1].y+ny},
                {c->points[1].x-nx,c->points[1].y-ny},
                {c->points[0].x-nx,c->points[0].y-ny}
            };
            PortRenderCommand solid=*c; solid.pattern_mode=0;
            if (!polygon(b,points,4,&solid,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_PIXEL) {
            double x=c->points[0].x,y=c->points[0].y;
            PortTriPoint points[4]={{x,y},{x+1,y},{x+1,y+1},{x,y+1}};
            PortRenderCommand solid=*c; solid.pattern_mode=0;
            if (!polygon(b,points,4,&solid,palette,v,width,height)) return 0;
        } else if (c->primitive == PORT_RENDER_SPHERE) {
            double x=c->points[0].x,y=c->points[0].y,r=c->points[1].x;
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
            for (int j=0;j<4;++j) points[j]=(ModernWheelPoint){c->points[j].x,c->points[j].y};
            WheelDraw draw={b,*c,palette,v,width,height,1};
            if (!modern_wheel_emit(points,c->color,c->wheel_colors[0],c->wheel_colors[1],wheel_face,&draw) || !draw.ok) return 0;
        } else {
            SDL_SetError("Unsupported semantic primitive %u",c->primitive);
            return 0;
        }
    }
    return 1;
}

int port_modern_compose(PortModernBackend *b, const PortRenderCommand *cmd, size_t count,
                        const PortRenderLayer *layer, const uint8_t *frame,
                        const uint8_t *palette, int width, int height)
{
    if (!b->background) b->background=SDL_CreateTexture(b->renderer,SDL_PIXELFORMAT_RGBA32,
                                                       SDL_TEXTUREACCESS_STREAMING,320,200);
    if (!b->ui) b->ui=SDL_CreateTexture(b->renderer,SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STREAMING,320,200);
    if (!b->background || !b->ui) return 0;
    SDL_SetTextureScaleMode(b->background,SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(b->ui,SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(b->ui,SDL_BLENDMODE_BLEND);
    uint8_t background[64000*4], ui[64000*4];
    for (unsigned i=0;i<64000;++i) {
        for (int channel=0;channel<3;++channel) {
            unsigned value=palette[layer->background[i]*3+channel];
            background[i*4+channel]=(uint8_t)((value<<2)|(value>>4));
            value=palette[frame[i]*3+channel];
            ui[i*4+channel]=(uint8_t)((value<<2)|(value>>4));
        }
        background[i*4+3]=255;
        ui[i*4+3]=layer->ui_mask[i]?255:0;
    }
    if (!SDL_UpdateTexture(b->background,NULL,background,320*4) ||
        !SDL_UpdateTexture(b->ui,NULL,ui,320*4)) return 0;
    PortViewport v=port_viewport_fit(width,height);
    SDL_FRect destination={v.x,v.y,v.w,v.h};
    if (!SDL_RenderTexture(b->renderer,b->background,NULL,&destination) ||
        !port_modern_draw_projected(b,cmd,count,palette,width,height) ||
        !SDL_RenderTexture(b->renderer,b->ui,NULL,&destination)) return 0;
    return 1;
}
