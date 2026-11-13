#include "types.h"
/* Generate wheel perimeter points and offset their paired ring by the rectangle origin delta. */

#define WHEEL_RING_POINT_COUNT 16
#define WHEEL_OUTER_RING_OFFSET 32
#include "wheel.h"
#include "math3d.h"
#include "viewstat.h"
#include "shapes3d.h"

/* Generate wheel section rings and copy a translated ring using the rectangle origin delta.
 * Params and return follow the declared C signature. */
void FAR tire(struct WheelRect *rect, struct Point *coords, I16 count)
{
    I16 dx, dy;
    I16 k;
    struct Point *src;
    struct Point *dst;
    ring(rect, coords, count);
    dx = rect->x - rect->left;
    dy = rect->y - rect->top;
    src = coords; dst = coords + WHEEL_OUTER_RING_OFFSET;
    for (k = 0; k < WHEEL_RING_POINT_COUNT; ++k) {
        dst->x = src->x + dx;
        dst->y = src->y + dy;
        ++src; ++dst;
    }
}
