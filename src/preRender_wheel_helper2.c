#include "stunts_types.h"
/* READABILITY: Interpolate the wheel section corners, then derive the paired point rings. */

#define WHEEL_RING_POINT_COUNT 16
struct Point { I16 x, y; };
struct WheelRect { struct Point p0, p1, p2; };
extern I16 FAR fixedmult(I16 delta, I16 scale);
extern void FAR ellipse(struct Point *source, struct Point *output);
/* Interpolate the two wheel-section endpoint vectors and generate their point rings.
 * Params and return follow the declared C signature. */
void ring(struct WheelRect *rect, struct Point *output, I16 scale)
{
    struct WheelRect points;
    points.p0 = rect->p0;
    points.p1.x = fixedmult(rect->p1.x - rect->p0.x, scale) + rect->p0.x;
    points.p1.y = fixedmult(rect->p1.y - rect->p0.y, scale) + rect->p0.y;
    points.p2.x = fixedmult(rect->p2.x - rect->p0.x, scale) + rect->p0.x;
    points.p2.y = fixedmult(rect->p2.y - rect->p0.y, scale) + rect->p0.y;
    ellipse(&rect->p0, output);
    ellipse(&points.p0, output + WHEEL_RING_POINT_COUNT);
}
