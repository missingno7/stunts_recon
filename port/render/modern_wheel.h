#ifndef MODERN_WHEEL_H
#define MODERN_WHEEL_H

#include <stddef.h>

typedef struct ModernWheelPoint {
    double x;
    double y;
} ModernWheelPoint;

typedef void (*ModernWheelPolygonFn)(void *user,
                                     const ModernWheelPoint *points,
                                     size_t point_count,
                                     int color);

/*
 * Emit the recovered wheel construction at floating-point precision.
 *
 * source[0..2] are the projected center and two projected section axes;
 * source[3] is the translated rear center, matching POLYINFO's four points.
 * The callback is synchronous and may copy the temporary point array.
 * Returns 19 on success (16 tread quads, 2 side faces, 1 ring face), or 0
 * when required input or callback pointers are null.
 */
size_t modern_wheel_emit(const ModernWheelPoint source[4],
                         int tread_color,
                         int side_color,
                         int ring_color,
                         ModernWheelPolygonFn emit,
                         void *user);

#endif
