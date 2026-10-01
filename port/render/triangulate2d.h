#ifndef PORT_TRIANGULATE2D_H
#define PORT_TRIANGULATE2D_H

#include <stddef.h>
#include <stdint.h>

#define PORT_TRIANGULATE_MAX_VERTICES 20u
#define PORT_TRIANGULATE_MAX_TRIANGLES (PORT_TRIANGULATE_MAX_VERTICES - 2u)

typedef struct PortTriPoint {
    double x;
    double y;
} PortTriPoint;

enum {
    PORT_TRIANGULATE_ERROR_INVALID_ARGUMENT = -1,
    PORT_TRIANGULATE_ERROR_TOO_MANY_VERTICES = -2,
    PORT_TRIANGULATE_ERROR_NONFINITE_COORDINATE = -3,
    PORT_TRIANGULATE_ERROR_SELF_INTERSECTION = -4,
    PORT_TRIANGULATE_ERROR_NO_EAR = -5,
    PORT_TRIANGULATE_ERROR_INSUFFICIENT_CAPACITY = -6
};

typedef enum PortTriangulateStatus {
    PORT_TRIANGULATE_OK = 0,
    PORT_TRIANGULATE_INVALID_ARGUMENT,
    PORT_TRIANGULATE_TOO_FEW_VERTICES,
    PORT_TRIANGULATE_TOO_MANY_VERTICES,
    PORT_TRIANGULATE_NONFINITE_COORDINATE,
    PORT_TRIANGULATE_DEGENERATE,
    PORT_TRIANGULATE_SELF_INTERSECTION,
    PORT_TRIANGULATE_NO_EAR,
    PORT_TRIANGULATE_INSUFFICIENT_CAPACITY
} PortTriangulateStatus;

/*
 * Deterministically triangulate a simple screen-space polygon by ear clipping.
 * xy contains interleaved double x/y values. Output indices refer to the input
 * array. Consecutive duplicate points, a repeated closing point, and collinear
 * points lying between their neighbors are removed from the triangulation.
 *
 * The function preserves input winding. Any non-adjacent edge crossing or
 * touch is reported as SELF_INTERSECTION. It never falls back to a triangle
 * fan. triangle_count_out is set to zero on every failure and output triangles
 * are copied only after the complete triangulation succeeds.
 */
PortTriangulateStatus port_triangulate_polygon_2d(
    const double *xy,
    size_t vertex_count,
    uint8_t (*triangles)[3],
    size_t triangle_capacity,
    size_t *triangle_count_out);

const char *port_triangulate_status_name(PortTriangulateStatus status);

/*
 * Renderer-facing adapter: indices is a flat sequence of triangle triplets,
 * and index_capacity is measured in unsigned elements. Returns the number of
 * triangles, 0 for a degenerate outline, or one of the negative error codes.
 */
int port_triangulate(const PortTriPoint *points, int count,
                     unsigned *indices, size_t index_capacity);

#endif
