#include "types.h"
/* Interpolate the wheel section corners, then derive the paired point rings. */
#include "matrix.h"
#include "trig.h"
#include "math3d.h"
#include "radius.h"
#include "ellipse.h"
#include "shapes3d.h"
/* Local three-point wheel view; other units expose a larger rectangle. */
/* Local view: WheelRect; struct.count is 3; another unit uses 8. */
struct WheelRect { struct Point p0, p1, p2; };
#include "viewstat.h"

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
