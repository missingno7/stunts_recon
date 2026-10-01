#include "renderer.h"
#include <string.h>
#include <stdlib.h>

static PortRenderCommand s_stream[PORT_RENDER_MAX_COMMANDS];
static size_t s_count;
static int16_t s_clip[4];
static int s_mode;

int port_render_mode(void) { return s_mode; }
int port_render_set_mode(const char *name)
{
    if (!strcmp(name,"faithful")) s_mode=0;
    else if (!strcmp(name,"projected")) s_mode=1;
    else if (!strcmp(name,"modern")) s_mode=2;
    else return 0;
    return 1;
}

void port_render_stream_begin(const int16_t *clip)
{ s_count = 0; memcpy(s_clip, clip, sizeof s_clip); }

void port_render_stream_emit(uint16_t index, int16_t depth, uint8_t type,
                            uint8_t material, uint8_t count, const int16_t *points,
                            uint8_t mode, uint16_t pattern, uint8_t color,
                            uint8_t secondary, uint8_t wheel_a, uint8_t wheel_b)
{
    if (s_count >= PORT_RENDER_MAX_COMMANDS || count > PORT_RENDER_MAX_POINTS)
        abort(); /* Explicit contract failure, never silently drop a primitive. */
    PortRenderCommand *c = &s_stream[s_count];
    memset(c, 0, sizeof(*c));
    c->order = (uint16_t)s_count++;
    c->pool_index = index; c->depth = depth;
    c->primitive = type; c->material = material; c->point_count = count;
    c->pattern_mode = mode; c->pattern = pattern;
    c->color = color; c->secondary = secondary;
    c->wheel_colors[0] = wheel_a; c->wheel_colors[1] = wheel_b;
    memcpy(c->clip, s_clip, sizeof c->clip);
    for (unsigned i = 0; i < count; ++i) {
        c->points[i].x = points[i * 2];
        c->points[i].y = points[i * 2 + 1];
    }
}

size_t port_render_stream_copy(PortRenderCommand *out, size_t capacity)
{
    if (out && capacity >= s_count) memcpy(out, s_stream, s_count * sizeof(*out));
    return s_count;
}

const PortRenderCommand *port_render_stream(size_t *count)
{ *count = s_count; return s_stream; }
