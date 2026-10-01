#ifndef PORT_RENDERER_H
#define PORT_RENDERER_H
#include <stdint.h>
#include <stddef.h>

#define PORT_RENDER_MAX_COMMANDS 400
#define PORT_RENDER_MAX_POINTS 10

typedef struct PortRenderPoint { int16_t x, y; } PortRenderPoint;
typedef struct PortRenderXYZ { double x, y, z; } PortRenderXYZ;
typedef enum PortRenderPrimitive {
    PORT_RENDER_POLYGON = 0, PORT_RENDER_LINE = 1, PORT_RENDER_SPHERE = 2,
    PORT_RENDER_WHEEL = 3, PORT_RENDER_PIXEL = 5
} PortRenderPrimitive;
typedef enum PortRenderPattern {
    PORT_RENDER_SOLID = 0, PORT_RENDER_STIPPLE = 1, PORT_RENDER_DITHER = 2
} PortRenderPattern;

/* No GPU types and no pointers to mutable game memory. Point order and
   linked-list order are the faithful dispatch order, including suppressed
   pattern-zero faces. Sphere point[1].x is a radius, not a position. */
typedef struct PortRenderCommand {
    uint16_t order, pool_index;
    int16_t depth;
    uint8_t primitive, material, point_count, pattern_mode;
    uint16_t pattern;
    uint8_t color, secondary, wheel_colors[2];
    int16_t clip[4];
    PortRenderPoint points[PORT_RENDER_MAX_POINTS];
} PortRenderCommand;

void port_render_stream_begin(const int16_t *clip);
void port_render_stream_emit(uint16_t pool_index, int16_t depth,
                            uint8_t type, uint8_t material, uint8_t count,
                            const int16_t *points, uint8_t mode,
                            uint16_t pattern, uint8_t color, uint8_t secondary,
                            uint8_t wheel_a, uint8_t wheel_b);
size_t port_render_stream_copy(PortRenderCommand *out, size_t capacity);
const PortRenderCommand *port_render_stream(size_t *count);
#endif
