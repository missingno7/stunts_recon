#include "types.h"
/* Interpolate the wheel section corners, then derive the paired point rings. */
#include "matrix.h"
#include "trig.h"
#include "math3d.h"
#include "radius.h"
#include "ellipse.h"
#include "shapes3d.h"
/* Local three-point wheel view; other units expose a larger rectangle. */
#include "wheel.h"
#include "viewstat.h"

/* Interpolate the two wheel-section endpoint vectors and generate their point rings.
 * Params and return follow the declared C signature. */
void ring(struct WheelRect *rect, struct Point *output, I16 scale)
{
    struct { struct Point p0, p1, p2; } points;
    points.p0 = *(struct Point *)&rect->left;
    points.p1.x = fixedmult(rect->section_p1_x - rect->left, scale) + rect->left;
    points.p1.y = fixedmult(rect->section_p1_y - rect->top, scale) + rect->top;
    points.p2.x = fixedmult(rect->section_p2_x - rect->left, scale) + rect->left;
    points.p2.y = fixedmult(rect->section_p2_y - rect->top, scale) + rect->top;
    ellipse((struct Point *)&rect->left, output);
    ellipse(&points.p0, output + WHEEL_RING_POINT_COUNT);
}
