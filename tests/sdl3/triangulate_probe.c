#include "triangulate2d.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static double area2(const double *xy, size_t count)
{
    size_t i;
    double area = 0.0;
    double ox = xy[0];
    double oy = xy[1];
    for (i = 0; i < count; ++i) {
        size_t next = (i + 1u) % count;
        double ax = xy[i * 2u] - ox;
        double ay = xy[i * 2u + 1u] - oy;
        double bx = xy[next * 2u] - ox;
        double by = xy[next * 2u + 1u] - oy;
        area += ax * by - ay * bx;
    }
    return area;
}

static double triangle_area2(const double *xy, const uint8_t tri[3])
{
    double ax = xy[tri[0] * 2u];
    double ay = xy[tri[0] * 2u + 1u];
    double bx = xy[tri[1] * 2u];
    double by = xy[tri[1] * 2u + 1u];
    double cx = xy[tri[2] * 2u];
    double cy = xy[tri[2] * 2u + 1u];
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

static int check_success(const char *label, const double *xy, size_t input_count,
                         size_t expected_triangles,
                         uint8_t output[PORT_TRIANGULATE_MAX_TRIANGLES][3])
{
    size_t count = 999u;
    size_t repeat_count = 999u;
    uint8_t repeated[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t i;
    double total = 0.0;
    int winding = area2(xy, input_count) > 0.0 ? 1 : -1;
    PortTriangulateStatus status = port_triangulate_polygon_2d(
        xy, input_count, output, PORT_TRIANGULATE_MAX_TRIANGLES, &count);
    if (status != PORT_TRIANGULATE_OK) {
        fprintf(stderr, "%s: got %s\n", label,
                port_triangulate_status_name(status));
        return 1;
    }
    if (count != expected_triangles) {
        fprintf(stderr, "%s: expected %lu triangles, got %lu\n", label,
                (unsigned long)expected_triangles, (unsigned long)count);
        return 1;
    }
    status = port_triangulate_polygon_2d(
        xy, input_count, repeated, PORT_TRIANGULATE_MAX_TRIANGLES,
        &repeat_count);
    if (status != PORT_TRIANGULATE_OK || repeat_count != count ||
        memcmp(output, repeated, count * sizeof(output[0])) != 0) {
        fprintf(stderr, "%s: triangulation is not deterministic\n", label);
        return 1;
    }
    for (i = 0; i < count; ++i) {
        double triangle_area = triangle_area2(xy, output[i]);
        if (triangle_area * (double)winding <= 0.0) {
            fprintf(stderr, "%s: triangle %lu has wrong/zero winding\n", label,
                    (unsigned long)i);
            return 1;
        }
        total += triangle_area;
    }
    if (fabs(total - area2(xy, input_count)) >
        1e-10 * fmax(1.0, fabs(area2(xy, input_count)))) {
        fprintf(stderr, "%s: triangle areas do not cover polygon (%g != %g)\n",
                label, total, area2(xy, input_count));
        return 1;
    }
    return 0;
}

static int check_failure(const char *label, const double *xy, size_t input_count,
                         PortTriangulateStatus expected)
{
    uint8_t output[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    uint8_t before[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t count = 999u;
    PortTriangulateStatus status;
    memset(output, 0xA5, sizeof(output));
    memcpy(before, output, sizeof(output));
    status = port_triangulate_polygon_2d(
        xy, input_count, output, PORT_TRIANGULATE_MAX_TRIANGLES, &count);
    if (status != expected || count != 0u || memcmp(output, before, sizeof(output))) {
        fprintf(stderr, "%s: expected %s, got %s, count %lu\n", label,
                port_triangulate_status_name(expected),
                port_triangulate_status_name(status), (unsigned long)count);
        return 1;
    }
    return 0;
}

int main(void)
{
    static const double ccw[] = {0,0, 4,0, 4,4, 0,4};
    static const double cw[] = {0,0, 0,4, 4,4, 4,0};
    static const double concave[] = {0,0, 4,0, 4,4, 2,1, 0,4};
    static const double collinear[] = {0,0, 2,0, 4,0, 4,4, 0,4};
    static const double repeated[] = {
        0,0, 4,0, 4,0, 4,4, 0,4, 0,0
    };
    static const double bowtie[] = {0,0, 4,4, 0,4, 4,0};
    static const double touching[] = {0,0, 4,0, 4,4, 2,0, 0,4};
    static const double repeated_nonadjacent[] = {0,0, 4,0, 4,4, 0,0, 0,4};
    static const double collinear_all[] = {0,0, 1,1, 2,2, 3,3};
    static const double translated[] = {
        1000000000000.0, 1000000000000.0,
        1000000000004.0, 1000000000000.0,
        1000000000004.0, 1000000000004.0,
        1000000000000.0, 1000000000004.0
    };
    static const double tiny[] = {0,0, 1e-12,0, 1e-12,1e-12, 0,1e-12};
    double twenty[PORT_TRIANGULATE_MAX_VERTICES * 2u];
    double too_many[(PORT_TRIANGULATE_MAX_VERTICES + 1u) * 2u];
    uint8_t triangles[PORT_TRIANGULATE_MAX_TRIANGLES][3];
    size_t count = 0;
    size_t i;

    CHECK(check_success("ccw convex", ccw, 4, 2, triangles) == 0);
    CHECK(check_success("cw convex", cw, 4, 2, triangles) == 0);
    CHECK(check_success("concave pentagon", concave, 5, 3, triangles) == 0);
    CHECK(check_success("collinear edge point", collinear, 5, 2, triangles) == 0);
    CHECK(check_success("repeated adjacent/closing point", repeated, 6, 2,
                        triangles) == 0);
    CHECK(check_success("large translated square", translated, 4, 2,
                        triangles) == 0);
    CHECK(check_success("small square", tiny, 4, 2, triangles) == 0);

    /* A radial, alternating-radius 20-gon is concave but remains simple. */
    for (i = 0; i < PORT_TRIANGULATE_MAX_VERTICES; ++i) {
        double angle = 6.2831853071795864769 * (double)i /
                       (double)PORT_TRIANGULATE_MAX_VERTICES;
        double radius = (i & 1u) ? 8.0 : 10.0;
        twenty[i * 2u] = radius * cos(angle);
        twenty[i * 2u + 1u] = radius * sin(angle);
    }
    CHECK(check_success("20 vertex concave polygon", twenty,
                        PORT_TRIANGULATE_MAX_VERTICES,
                        PORT_TRIANGULATE_MAX_TRIANGLES, triangles) == 0);

    CHECK(check_failure("bowtie self-intersection", bowtie, 4,
                        PORT_TRIANGULATE_SELF_INTERSECTION) == 0);
    CHECK(check_failure("nonadjacent edge touch", touching, 5,
                        PORT_TRIANGULATE_SELF_INTERSECTION) == 0);
    CHECK(check_failure("nonadjacent repeated point", repeated_nonadjacent, 5,
                        PORT_TRIANGULATE_SELF_INTERSECTION) == 0);
    CHECK(check_failure("collinear polygon", collinear_all, 4,
                        PORT_TRIANGULATE_DEGENERATE) == 0);
    CHECK(check_failure("too few points", ccw, 2,
                        PORT_TRIANGULATE_TOO_FEW_VERTICES) == 0);
    for (i = 0; i < PORT_TRIANGULATE_MAX_VERTICES + 1u; ++i) {
        too_many[2u * i] = (double)i;
        too_many[2u * i + 1u] = (double)(i * i);
    }
    CHECK(check_failure("too many points", too_many,
                        PORT_TRIANGULATE_MAX_VERTICES + 1u,
                        PORT_TRIANGULATE_TOO_MANY_VERTICES) == 0);

    memset(triangles, 0x5A, sizeof(triangles));
    count = 777u;
    CHECK(port_triangulate_polygon_2d(ccw, 4, triangles, 1, &count) ==
          PORT_TRIANGULATE_INSUFFICIENT_CAPACITY);
    CHECK(count == 0u);
    {
        double nonfinite[] = {0,0, 1,0, NAN,1};
        CHECK(check_failure("nonfinite input", nonfinite, 3,
                            PORT_TRIANGULATE_NONFINITE_COORDINATE) == 0);
    }
    CHECK(port_triangulate_polygon_2d(NULL, 4, triangles,
             PORT_TRIANGULATE_MAX_TRIANGLES, &count) ==
          PORT_TRIANGULATE_INVALID_ARGUMENT);
    CHECK(count == 0u);

    {
        const PortTriPoint points[] = {
            {0, 0}, {4, 0}, {4, 4}, {2, 1}, {0, 4}
        };
        const PortTriPoint line[] = {{0, 0}, {1, 1}, {2, 2}};
        const PortTriPoint crossed[] = {{0, 0}, {4, 4}, {0, 4}, {4, 0}};
        unsigned flat[PORT_TRIANGULATE_MAX_TRIANGLES * 3u];
        unsigned before[PORT_TRIANGULATE_MAX_TRIANGLES * 3u];
        int result;
        memset(flat, 0xA5, sizeof(flat));
        memcpy(before, flat, sizeof(flat));
        result = port_triangulate(points, 5, flat,
                                  PORT_TRIANGULATE_MAX_TRIANGLES * 3u);
        CHECK(result == 3);
        for (i = 0; i < (size_t)result * 3u; ++i)
            CHECK(flat[i] < 5u);
        CHECK(port_triangulate(line, 3, flat,
                               PORT_TRIANGULATE_MAX_TRIANGLES * 3u) == 0);
        CHECK(port_triangulate(crossed, 4, flat,
                               PORT_TRIANGULATE_MAX_TRIANGLES * 3u) ==
              PORT_TRIANGULATE_ERROR_SELF_INTERSECTION);
        memcpy(before, flat, sizeof(flat));
        CHECK(port_triangulate(points, 5, flat, 3u) ==
              PORT_TRIANGULATE_ERROR_INSUFFICIENT_CAPACITY);
        CHECK(memcmp(flat, before, sizeof(flat)) == 0);
    }

    puts("triangulate2d scratch harness: all checks passed");
    return 0;
}
