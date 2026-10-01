#include "triangulate2d.h"

#include <float.h>
#include <math.h>

typedef struct PointRef {
    double x;
    double y;
    uint8_t input_index;
} PointRef;

static long double orient(PointRef a, PointRef b, PointRef c)
{
    return ((long double)b.x - (long double)a.x) *
               ((long double)c.y - (long double)a.y) -
           ((long double)b.y - (long double)a.y) *
               ((long double)c.x - (long double)a.x);
}

static long double polygon_area2(const PointRef *points, size_t count)
{
    size_t i;
    long double area = 0.0L;
    long double origin_x = (long double)points[0].x;
    long double origin_y = (long double)points[0].y;

    for (i = 0; i < count; ++i) {
        const PointRef a = points[i];
        const PointRef b = points[(i + 1u) % count];
        long double ax = (long double)a.x - origin_x;
        long double ay = (long double)a.y - origin_y;
        long double bx = (long double)b.x - origin_x;
        long double by = (long double)b.y - origin_y;
        area += ax * by - ay * bx;
    }
    return area;
}

static int point_equal(PointRef a, PointRef b, long double epsilon)
{
    return fabsl((long double)a.x - (long double)b.x) <= epsilon &&
           fabsl((long double)a.y - (long double)b.y) <= epsilon;
}

static int between(long double value, long double a, long double b,
                   long double epsilon)
{
    long double low = a < b ? a : b;
    long double high = a < b ? b : a;
    return value >= low - epsilon && value <= high + epsilon;
}

static int on_segment(PointRef a, PointRef b, PointRef point,
                      long double length_epsilon,
                      long double area_epsilon)
{
    return fabsl(orient(a, b, point)) <= area_epsilon &&
           between((long double)point.x, (long double)a.x,
                   (long double)b.x, length_epsilon) &&
           between((long double)point.y, (long double)a.y,
                   (long double)b.y, length_epsilon);
}

static int sign_with_epsilon(long double value, long double epsilon)
{
    if (value > epsilon) return 1;
    if (value < -epsilon) return -1;
    return 0;
}

static int segments_intersect(PointRef a, PointRef b, PointRef c, PointRef d,
                              long double length_epsilon,
                              long double area_epsilon)
{
    int ab_c = sign_with_epsilon(orient(a, b, c), area_epsilon);
    int ab_d = sign_with_epsilon(orient(a, b, d), area_epsilon);
    int cd_a = sign_with_epsilon(orient(c, d, a), area_epsilon);
    int cd_b = sign_with_epsilon(orient(c, d, b), area_epsilon);

    if (ab_c * ab_d < 0 && cd_a * cd_b < 0)
        return 1;
    if (ab_c == 0 && on_segment(a, b, c, length_epsilon, area_epsilon))
        return 1;
    if (ab_d == 0 && on_segment(a, b, d, length_epsilon, area_epsilon))
        return 1;
    if (cd_a == 0 && on_segment(c, d, a, length_epsilon, area_epsilon))
        return 1;
    if (cd_b == 0 && on_segment(c, d, b, length_epsilon, area_epsilon))
        return 1;
    return 0;
}

static int has_nonadjacent_intersection(const PointRef *points, size_t count,
                                        long double length_epsilon,
                                        long double area_epsilon)
{
    size_t i;
    size_t j;

    for (i = 0; i < count; ++i) {
        size_t i_next = (i + 1u) % count;
        for (j = i + 1u; j < count; ++j) {
            size_t j_next = (j + 1u) % count;
            if (j == i_next || j_next == i)
                continue;
            if (segments_intersect(points[i], points[i_next],
                                   points[j], points[j_next],
                                   length_epsilon, area_epsilon))
                return 1;
        }
    }
    return 0;
}

static int point_in_or_on_triangle(PointRef p, PointRef a, PointRef b,
                                   PointRef c, int winding,
                                   long double area_epsilon)
{
    long double ab = orient(a, b, p) * (long double)winding;
    long double bc = orient(b, c, p) * (long double)winding;
    long double ca = orient(c, a, p) * (long double)winding;
    return ab >= -area_epsilon && bc >= -area_epsilon &&
           ca >= -area_epsilon;
}

static void remove_point(PointRef *points, size_t *count, size_t index)
{
    size_t i;
    for (i = index; i + 1u < *count; ++i)
        points[i] = points[i + 1u];
    --*count;
}

static PortTriangulateStatus triangulate_impl(
    const double *xy,
    size_t vertex_count,
    uint8_t output[PORT_TRIANGULATE_MAX_TRIANGLES][3],
    size_t *triangle_count_out)
{
    PointRef points[PORT_TRIANGULATE_MAX_VERTICES];
    uint8_t local_triangles[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t count = 0;
    size_t triangle_count = 0;
    size_t i;
    size_t scan;
    double min_x, max_x, min_y, max_y;
    long double scale;
    long double length_epsilon;
    long double area_epsilon;
    long double signed_area;
    int winding;

    if (xy == NULL || output == NULL || triangle_count_out == NULL)
        return PORT_TRIANGULATE_INVALID_ARGUMENT;
    if (vertex_count < 3u)
        return PORT_TRIANGULATE_TOO_FEW_VERTICES;
    if (vertex_count > PORT_TRIANGULATE_MAX_VERTICES)
        return PORT_TRIANGULATE_TOO_MANY_VERTICES;

    min_x = max_x = xy[0];
    min_y = max_y = xy[1];
    for (i = 0; i < vertex_count; ++i) {
        double x = xy[i * 2u];
        double y = xy[i * 2u + 1u];
        if (!isfinite(x) || !isfinite(y))
            return PORT_TRIANGULATE_NONFINITE_COORDINATE;
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }
    scale = (long double)(max_x - min_x);
    if ((long double)(max_y - min_y) > scale)
        scale = (long double)(max_y - min_y);
    if (!(scale > 0.0L) || !isfinite((double)scale))
        return PORT_TRIANGULATE_DEGENERATE;
    length_epsilon = 64.0L * (long double)DBL_EPSILON * scale;
    area_epsilon = 64.0L * (long double)DBL_EPSILON * scale * scale;

    /* Collapse adjacent duplicate points while retaining original indices. */
    for (i = 0; i < vertex_count; ++i) {
        PointRef point;
        point.x = xy[i * 2u];
        point.y = xy[i * 2u + 1u];
        point.input_index = (uint8_t)i;
        if (count == 0u || !point_equal(points[count - 1u], point,
                                        length_epsilon))
            points[count++] = point;
    }
    if (count > 1u && point_equal(points[0], points[count - 1u],
                                  length_epsilon))
        --count;
    if (count < 3u)
        return PORT_TRIANGULATE_DEGENERATE;

    /* Classify a fully collinear outline as degenerate before edge tests. */
    for (i = 2u; i < count; ++i) {
        if (fabsl(orient(points[0], points[1], points[i])) > area_epsilon)
            break;
    }
    if (i == count)
        return PORT_TRIANGULATE_DEGENERATE;

    if (has_nonadjacent_intersection(points, count, length_epsilon,
                                     area_epsilon))
        return PORT_TRIANGULATE_SELF_INTERSECTION;

    /* Remove between-neighbor collinear points; reject a backtracking edge. */
    for (;;) {
        int removed = 0;
        for (i = 0; i < count; ++i) {
            size_t prev = (i + count - 1u) % count;
            size_t next = (i + 1u) % count;
            long double turn = orient(points[prev], points[i], points[next]);
            long double dot;
            long double ab_x, ab_y, cb_x, cb_y;
            if (fabsl(turn) > area_epsilon)
                continue;
            ab_x = (long double)points[i].x - (long double)points[prev].x;
            ab_y = (long double)points[i].y - (long double)points[prev].y;
            cb_x = (long double)points[i].x - (long double)points[next].x;
            cb_y = (long double)points[i].y - (long double)points[next].y;
            dot = ab_x * cb_x + ab_y * cb_y;
            if (dot > length_epsilon * length_epsilon)
                return PORT_TRIANGULATE_SELF_INTERSECTION;
            remove_point(points, &count, i);
            removed = 1;
            break;
        }
        if (!removed)
            break;
        if (count < 3u)
            return PORT_TRIANGULATE_DEGENERATE;
    }

    if (has_nonadjacent_intersection(points, count, length_epsilon,
                                     area_epsilon))
        return PORT_TRIANGULATE_SELF_INTERSECTION;
    signed_area = polygon_area2(points, count);
    if (fabsl(signed_area) <= area_epsilon)
        return PORT_TRIANGULATE_DEGENERATE;
    winding = signed_area > 0.0L ? 1 : -1;

    /* Always scan from the first current vertex, making tie choices stable. */
    while (count > 3u) {
        int clipped = 0;
        for (scan = 0; scan < count; ++scan) {
            size_t prev = (scan + count - 1u) % count;
            size_t next = (scan + 1u) % count;
            size_t candidate;
            int ear = 1;
            if (orient(points[prev], points[scan], points[next]) *
                    (long double)winding <= area_epsilon)
                continue;
            for (candidate = 0; candidate < count; ++candidate) {
                if (candidate == prev || candidate == scan ||
                    candidate == next)
                    continue;
                if (point_in_or_on_triangle(points[candidate], points[prev],
                                           points[scan], points[next], winding,
                                           area_epsilon)) {
                    ear = 0;
                    break;
                }
            }
            if (!ear)
                continue;
            local_triangles[triangle_count][0] = points[prev].input_index;
            local_triangles[triangle_count][1] = points[scan].input_index;
            local_triangles[triangle_count][2] = points[next].input_index;
            ++triangle_count;
            remove_point(points, &count, scan);
            clipped = 1;
            break;
        }
        if (!clipped)
            return PORT_TRIANGULATE_NO_EAR;
    }

    if (fabsl(orient(points[0], points[1], points[2])) <= area_epsilon)
        return PORT_TRIANGULATE_DEGENERATE;
    local_triangles[triangle_count][0] = points[0].input_index;
    local_triangles[triangle_count][1] = points[1].input_index;
    local_triangles[triangle_count][2] = points[2].input_index;
    ++triangle_count;

    for (i = 0; i < triangle_count; ++i) {
        output[i][0] = local_triangles[i][0];
        output[i][1] = local_triangles[i][1];
        output[i][2] = local_triangles[i][2];
    }
    *triangle_count_out = triangle_count;
    return PORT_TRIANGULATE_OK;
}

PortTriangulateStatus port_triangulate_polygon_2d(
    const double *xy,
    size_t vertex_count,
    uint8_t (*triangles)[3],
    size_t triangle_capacity,
    size_t *triangle_count_out)
{
    uint8_t local[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t count = 0;
    size_t i;
    PortTriangulateStatus status;

    if (triangle_count_out != NULL)
        *triangle_count_out = 0;
    if (xy == NULL || triangle_count_out == NULL)
        return PORT_TRIANGULATE_INVALID_ARGUMENT;
    if (vertex_count > PORT_TRIANGULATE_MAX_VERTICES)
        return PORT_TRIANGULATE_TOO_MANY_VERTICES;
    if (triangles == NULL && triangle_capacity != 0u)
        return PORT_TRIANGULATE_INVALID_ARGUMENT;

    status = triangulate_impl(xy, vertex_count, local, &count);
    if (status != PORT_TRIANGULATE_OK)
        return status;
    if (triangle_capacity < count)
        return PORT_TRIANGULATE_INSUFFICIENT_CAPACITY;
    if (triangles == NULL)
        return PORT_TRIANGULATE_INVALID_ARGUMENT;
    for (i = 0; i < count; ++i) {
        triangles[i][0] = local[i][0];
        triangles[i][1] = local[i][1];
        triangles[i][2] = local[i][2];
    }
    *triangle_count_out = count;
    return PORT_TRIANGULATE_OK;
}

const char *port_triangulate_status_name(PortTriangulateStatus status)
{
    switch (status) {
    case PORT_TRIANGULATE_OK: return "ok";
    case PORT_TRIANGULATE_INVALID_ARGUMENT: return "invalid argument";
    case PORT_TRIANGULATE_TOO_FEW_VERTICES: return "too few vertices";
    case PORT_TRIANGULATE_TOO_MANY_VERTICES: return "too many vertices";
    case PORT_TRIANGULATE_NONFINITE_COORDINATE: return "nonfinite coordinate";
    case PORT_TRIANGULATE_DEGENERATE: return "degenerate polygon";
    case PORT_TRIANGULATE_SELF_INTERSECTION: return "self-intersection";
    case PORT_TRIANGULATE_NO_EAR: return "no ear found";
    case PORT_TRIANGULATE_INSUFFICIENT_CAPACITY: return "insufficient output capacity";
    default: return "unknown status";
    }
}

int port_triangulate(const PortTriPoint *points, int count,
                     unsigned *indices, size_t index_capacity)
{
    double xy[PORT_TRIANGULATE_MAX_VERTICES * 2u];
    uint8_t triangles[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t triangle_count = 0;
    size_t i;
    PortTriangulateStatus status;

    if (count < 0 || (points == NULL && count != 0) ||
        (indices == NULL && index_capacity != 0u))
        return PORT_TRIANGULATE_ERROR_INVALID_ARGUMENT;
    if ((unsigned)count > PORT_TRIANGULATE_MAX_VERTICES)
        return PORT_TRIANGULATE_ERROR_TOO_MANY_VERTICES;
    if (count < 3)
        return 0;
    if (indices == NULL)
        return PORT_TRIANGULATE_ERROR_INVALID_ARGUMENT;
    for (i = 0; i < (size_t)count; ++i) {
        xy[i * 2u] = points[i].x;
        xy[i * 2u + 1u] = points[i].y;
    }

    status = port_triangulate_polygon_2d(
        xy, (size_t)count, triangles, PORT_TRIANGULATE_MAX_TRIANGLES,
        &triangle_count);
    switch (status) {
    case PORT_TRIANGULATE_OK:
        break;
    case PORT_TRIANGULATE_TOO_FEW_VERTICES:
    case PORT_TRIANGULATE_DEGENERATE:
        return 0;
    case PORT_TRIANGULATE_TOO_MANY_VERTICES:
        return PORT_TRIANGULATE_ERROR_TOO_MANY_VERTICES;
    case PORT_TRIANGULATE_NONFINITE_COORDINATE:
        return PORT_TRIANGULATE_ERROR_NONFINITE_COORDINATE;
    case PORT_TRIANGULATE_SELF_INTERSECTION:
        return PORT_TRIANGULATE_ERROR_SELF_INTERSECTION;
    case PORT_TRIANGULATE_NO_EAR:
        return PORT_TRIANGULATE_ERROR_NO_EAR;
    case PORT_TRIANGULATE_INSUFFICIENT_CAPACITY:
        return PORT_TRIANGULATE_ERROR_INSUFFICIENT_CAPACITY;
    case PORT_TRIANGULATE_INVALID_ARGUMENT:
    default:
        return PORT_TRIANGULATE_ERROR_INVALID_ARGUMENT;
    }

    if (index_capacity < triangle_count * 3u)
        return PORT_TRIANGULATE_ERROR_INSUFFICIENT_CAPACITY;
    for (i = 0; i < triangle_count; ++i) {
        indices[i * 3u] = triangles[i][0];
        indices[i * 3u + 1u] = triangles[i][1];
        indices[i * 3u + 2u] = triangles[i][2];
    }
    return (int)triangle_count;
}
