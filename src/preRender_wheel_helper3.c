#include "stunts_types.h"
/* READABILITY: Build the sixteen front/back perimeter points for a wheel section from three source points. */

#define WHEEL_HALF_RING_POINT_COUNT 8
typedef struct Point {
    I16 x;
    I16 y;
} Point;
extern I16 FAR fixedmult(I16, I16);
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
    output[2].x = fixedmult(output[0].x + output[4].x, 0x2d41);
    output[2].y = fixedmult(output[0].y + output[4].y, 0x2d41);
    output[1].x = fixedmult(output[0].x + (output[4].x >> 1), 0x393e);
    output[1].y = fixedmult(output[0].y + (output[4].y >> 1), 0x393e);
    /* PORT: Signed geometry coordinates use arithmetic right shifts when halved. */
    half_dx = output[0].x >> 1;
    output[3].x = fixedmult(output[4].x + half_dx, 0x393e);
    half_dy = output[0].y >> 1;
    output[3].y = fixedmult(output[4].y + half_dy, 0x393e);
    ndx = -output[0].x;
    output[6].x = fixedmult(output[4].x + ndx, 0x2d41);
    ndy = -output[0].y;
    output[6].y = fixedmult(output[4].y + ndy, 0x2d41);
    output[7].x = fixedmult((output[4].x >> 1) + ndx, 0x393e);
    output[7].y = fixedmult((output[4].y >> 1) + ndy, 0x393e);
    output[5].x = fixedmult(output[4].x - half_dx, 0x393e);
    output[5].y = fixedmult(output[4].y - half_dy, 0x393e);
    for (vert = 0; vert < WHEEL_HALF_RING_POINT_COUNT; ++vert) {
        output[vert + WHEEL_HALF_RING_POINT_COUNT].x = source[0].x - output[vert].x;
        output[vert + WHEEL_HALF_RING_POINT_COUNT].y = source[0].y - output[vert].y;
        output[vert].x += source[0].x;
        output[vert].y += source[0].y;
    }
}
