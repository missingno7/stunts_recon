#include "modern_wheel.h"

enum {
    WHEEL_RING_POINT_COUNT = 16,
    WHEEL_HALF_RING_POINT_COUNT = 8,
    WHEEL_SECOND_RING_OFFSET = 16,
    WHEEL_OUTER_RING_OFFSET = 32,
    WHEEL_POINT_BUFFER_COUNT = 48,
    WHEEL_FACE_POINT_COUNT = 18,
    WHEEL_POLYGON_COUNT = 19
};

/* `mulscl` is a signed Q14 product with nearest-integer rounding in the
   recovered port. These exact rational coefficients preserve its scale
   while postponing raster-grid quantization to the native renderer. */
static const double WHEEL_SCALE = 0x2500 / 16384.0;
static const double RING_DIAGONAL_SCALE = 0x2d41 / 16384.0;
static const double RING_INTERMEDIATE_SCALE = 0x393e / 16384.0;

static ModernWheelPoint point_add(ModernWheelPoint a, ModernWheelPoint b)
{
    ModernWheelPoint result = {a.x + b.x, a.y + b.y};
    return result;
}

static ModernWheelPoint point_sub(ModernWheelPoint a, ModernWheelPoint b)
{
    ModernWheelPoint result = {a.x - b.x, a.y - b.y};
    return result;
}

static ModernWheelPoint point_scale(ModernWheelPoint p, double scale)
{
    ModernWheelPoint result = {p.x * scale, p.y * scale};
    return result;
}

/* Floating-point counterpart of preRender_wheel_helper3. The eight
   recovered offsets and their mirrored points retain the original order. */
static void build_section_ring(const ModernWheelPoint source[3],
                               ModernWheelPoint output[16])
{
    ModernWheelPoint axis_x = point_sub(source[1], source[0]);
    ModernWheelPoint axis_y = point_sub(source[2], source[0]);
    ModernWheelPoint half_x = point_scale(axis_x, 0.5);
    ModernWheelPoint half_y = point_scale(axis_y, 0.5);
    ModernWheelPoint offsets[8];
    size_t i;

    offsets[0] = axis_x;
    offsets[4] = axis_y;
    offsets[2] = point_scale(point_add(axis_x, axis_y),
                             RING_DIAGONAL_SCALE);
    offsets[1] = point_scale(point_add(axis_x, half_y),
                             RING_INTERMEDIATE_SCALE);
    offsets[3] = point_scale(point_add(axis_y, half_x),
                             RING_INTERMEDIATE_SCALE);
    offsets[6] = point_scale(point_sub(axis_y, axis_x),
                             RING_DIAGONAL_SCALE);
    offsets[7] = point_scale(point_sub(half_y, axis_x),
                             RING_INTERMEDIATE_SCALE);
    offsets[5] = point_scale(point_sub(axis_y, half_x),
                             RING_INTERMEDIATE_SCALE);

    for (i = 0; i < WHEEL_HALF_RING_POINT_COUNT; ++i) {
        output[i] = point_add(source[0], offsets[i]);
        output[i + WHEEL_HALF_RING_POINT_COUNT] =
            point_sub(source[0], offsets[i]);
    }
}

static size_t wrap_ring_index(int index)
{
    index %= WHEEL_RING_POINT_COUNT;
    if (index < 0)
        index += WHEEL_RING_POINT_COUNT;
    return (size_t)index;
}

static void emit_tread_faces(const ModernWheelPoint rings[48],
                             int color,
                             ModernWheelPolygonFn emit,
                             void *user)
{
    size_t i;
    for (i = 0; i < WHEEL_RING_POINT_COUNT; ++i) {
        size_t next = (i + 1) % WHEEL_RING_POINT_COUNT;
        ModernWheelPoint quad[4] = {
            rings[i], rings[next],
            rings[WHEEL_OUTER_RING_OFFSET + next],
            rings[WHEEL_OUTER_RING_OFFSET + i]
        };
        emit(user, quad, 4, color);
    }
}

static size_t find_top_point(const ModernWheelPoint ring[16])
{
    size_t i;
    size_t top = 0;
    for (i = 1; i < WHEEL_RING_POINT_COUNT; ++i) {
        if (ring[i].y < ring[top].y)
            top = i;
    }
    return top;
}

static void emit_side_face(const ModernWheelPoint rings[48],
                           size_t top,
                           int direction,
                           int color,
                           ModernWheelPolygonFn emit,
                           void *user)
{
    ModernWheelPoint face[WHEEL_FACE_POINT_COUNT];
    size_t i;

    /* First half walks one section from the selected top point to its
       antipode; the second half returns over the other section in reverse. */
    for (i = 0; i <= WHEEL_HALF_RING_POINT_COUNT; ++i)
        face[i] = rings[wrap_ring_index((int)top + direction * (int)i)];
    for (i = 0; i <= WHEEL_HALF_RING_POINT_COUNT; ++i) {
        size_t ring_index = wrap_ring_index(
            (int)top + direction * (int)(WHEEL_HALF_RING_POINT_COUNT - i));
        face[WHEEL_HALF_RING_POINT_COUNT + 1 + i] =
            rings[WHEEL_SECOND_RING_OFFSET + ring_index];
    }
    emit(user, face, WHEEL_FACE_POINT_COUNT, color);
}

size_t modern_wheel_emit(const ModernWheelPoint source[4],
                         int tread_color,
                         int side_color,
                         int ring_color,
                         ModernWheelPolygonFn emit,
                         void *user)
{
    ModernWheelPoint rings[WHEEL_POINT_BUFFER_COUNT];
    ModernWheelPoint scaled_section[3];
    ModernWheelPoint translation;
    size_t top;
    size_t i;

    if (source == NULL || emit == NULL)
        return 0;

    build_section_ring(source, rings);
    scaled_section[0] = source[0];
    scaled_section[1] = point_add(source[0],
        point_scale(point_sub(source[1], source[0]), WHEEL_SCALE));
    scaled_section[2] = point_add(source[0],
        point_scale(point_sub(source[2], source[0]), WHEEL_SCALE));
    build_section_ring(scaled_section, rings + WHEEL_SECOND_RING_OFFSET);

    translation = point_sub(source[3], source[0]);
    for (i = 0; i < WHEEL_RING_POINT_COUNT; ++i)
        rings[WHEEL_OUTER_RING_OFFSET + i] = point_add(rings[i], translation);

    emit_tread_faces(rings, tread_color, emit, user);
    top = find_top_point(rings);
    emit_side_face(rings, top, 1, side_color, emit, user);
    emit_side_face(rings, top, -1, side_color, emit, user);
    emit(user, rings + WHEEL_SECOND_RING_OFFSET,
         WHEEL_RING_POINT_COUNT, ring_color);
    return WHEEL_POLYGON_COUNT;
}
