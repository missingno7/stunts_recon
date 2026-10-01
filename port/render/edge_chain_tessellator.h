#ifndef PORT_RENDER_EDGE_CHAIN_H
#define PORT_RENDER_EDGE_CHAIN_H

#include <stddef.h>

#define EDGE_CHAIN_MAX_POINTS 20
#define EDGE_CHAIN_MAX_SPANS 2048

typedef struct EdgeChainPoint { double x, y; } EdgeChainPoint;
/* One continuous scanline band. Left/right are evaluated at each endpoint. */
typedef struct EdgeChainSpan {
    EdgeChainPoint top_left, top_right, bottom_right, bottom_left;
} EdgeChainSpan;

enum {
    EDGE_CHAIN_ERROR_ARGUMENT = -1,
    EDGE_CHAIN_ERROR_COUNT = -2,
    EDGE_CHAIN_ERROR_NONFINITE = -3,
    EDGE_CHAIN_ERROR_CAPACITY = -4
};

/*
 * Continuous adaptation of the recovered raster_polygon edge-chain rule.
 * It builds the forward/reverse ascending paths from the last minimum-y
 * vertex to the first maximum-y vertex, keeps input-order tie resolution,
 * seeds from the forward chain, then applies each reverse edge to its stable
 * left/right side. It is deliberately not an even-odd fill or a triangle fan.
 *
 * Collinear/collapsed geometry is emitted as a width-one logical-pixel strip
 * around its farthest input-point pair. Returns span count, zero for empty,
 * or a negative EDGE_CHAIN_ERROR code. Each span is a trapezoid (possibly a
 * triangle at a tip); renderer callers split it into two triangles.
 */
int edge_chain_tessellate(const EdgeChainPoint *points, size_t count,
                          EdgeChainSpan *spans, size_t span_capacity);

#endif
