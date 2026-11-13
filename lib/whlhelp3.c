
#include "matrix.h"
#include "trig.h"
#include "math3d.h"
#include "radius.h"
#include "types.h"
/* READABILITY: Build the sixteen front/back perimeter points for a wheel section from three source points. */

#include "ellipse.h"
#include "shapes3d.h"
/* Interpolate and translate the front and rear wheel-section point rings.
 * Params and return follow the declared C signature. */
void ellipse(Point *source, Point *output)
{
    I16 half_dx;
    I16 half_dy;
    I16 ndx;
    I16 ndy;
    I16 vert;
    output[0].x = source[1].x - source[0].x;
    output[0].y = source[1].y - source[0].y;
    output[4].x = source[2].x - source[0].x;
    output[4].y = source[2].y - source[0].y;
    output[2].x = fixedmult(output[0].x + output[4].x, RING_WEIGHT_DIAGONAL_Q14);
    output[2].y = fixedmult(output[0].y + output[4].y, RING_WEIGHT_DIAGONAL_Q14);
    output[1].x = fixedmult(output[0].x + (output[4].x >> RING_HALF_COMPONENT_SHIFT), RING_WEIGHT_HALF_AXIS_Q14);
    output[1].y = fixedmult(output[0].y + (output[4].y >> RING_HALF_COMPONENT_SHIFT), RING_WEIGHT_HALF_AXIS_Q14);
    /* PORT: Signed geometry coordinates use arithmetic right shifts when halved. */
    half_dx = output[0].x >> RING_HALF_COMPONENT_SHIFT;
    output[3].x = fixedmult(output[4].x + half_dx, RING_WEIGHT_HALF_AXIS_Q14);
    half_dy = output[0].y >> RING_HALF_COMPONENT_SHIFT;
    output[3].y = fixedmult(output[4].y + half_dy, RING_WEIGHT_HALF_AXIS_Q14);
    ndx = -output[0].x;
    output[6].x = fixedmult(output[4].x + ndx, RING_WEIGHT_DIAGONAL_Q14);
    ndy = -output[0].y;
    output[6].y = fixedmult(output[4].y + ndy, RING_WEIGHT_DIAGONAL_Q14);
    output[7].x = fixedmult((output[4].x >> RING_HALF_COMPONENT_SHIFT) + ndx, RING_WEIGHT_HALF_AXIS_Q14);
    output[7].y = fixedmult((output[4].y >> RING_HALF_COMPONENT_SHIFT) + ndy, RING_WEIGHT_HALF_AXIS_Q14);
    output[5].x = fixedmult(output[4].x - half_dx, RING_WEIGHT_HALF_AXIS_Q14);
    output[5].y = fixedmult(output[4].y - half_dy, RING_WEIGHT_HALF_AXIS_Q14);
    for (vert = 0; vert < WHEEL_HALF_RING_POINT_COUNT; ++vert) {
        output[vert + WHEEL_HALF_RING_POINT_COUNT].x = source[0].x - output[vert].x;
        output[vert + WHEEL_HALF_RING_POINT_COUNT].y = source[0].y - output[vert].y;
        output[vert].x += source[0].x;
        output[vert].y += source[0].y;
    }
}
