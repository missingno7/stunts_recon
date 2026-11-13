#include "matrix.h"
#include "trig.h"
#include "math3d.h"
#include "radius.h"
#include "types.h"
/* READABILITY: Build the thirty-two front/back perimeter points for a sphere section from three source points. */
#include "ellipse.h"
#include "shapes3d.h"
/* Interpolate and translate the front and rear sphere-section point rings.
 * Params and return follow the declared C signature. */

void FAR ellipseh(struct Point *source, struct Point *output)
{
    I16 startHalfX, startHalfY;
    I16 threeQuarterX0, threeQuarterY0;
    I16 initialQuarterX, initialQuarterY;
    I16 index;
    I16 endHalfX, endHalfY;
    I16 threeQuarterX8, threeQuarterY8;
    I16 outermostQuarterX, outermostQuarterY;

    output[0].x = source[1].x - source[0].x;
    output[0].y = source[1].y - source[0].y;
    output[8].x = source[2].x - source[0].x;
    output[8].y = source[2].y - source[0].y;
    /* PORT: Signed geometry coordinates use arithmetic right shifts when halved. */
    startHalfX = output[0].x >> RING_HALF_COMPONENT_SHIFT;
    initialQuarterX = startHalfX >> RING_HALF_COMPONENT_SHIFT;
    threeQuarterX0 = startHalfX + initialQuarterX;
    startHalfY = output[0].y >> RING_HALF_COMPONENT_SHIFT;
    initialQuarterY = startHalfY >> RING_HALF_COMPONENT_SHIFT;
    threeQuarterY0 = startHalfY + initialQuarterY;
    endHalfX = output[8].x >> RING_HALF_COMPONENT_SHIFT;
    outermostQuarterX = endHalfX >> RING_HALF_COMPONENT_SHIFT;
    threeQuarterX8 = endHalfX + outermostQuarterX;
    endHalfY = output[8].y >> RING_HALF_COMPONENT_SHIFT;
    outermostQuarterY = endHalfY >> RING_HALF_COMPONENT_SHIFT;
    threeQuarterY8 = endHalfY + outermostQuarterY;

    output[4].x = fixedmult(output[0].x + output[8].x, RING_WEIGHT_DIAGONAL_Q14);
    output[4].y = fixedmult(output[0].y + output[8].y, RING_WEIGHT_DIAGONAL_Q14);
    output[2].x = fixedmult(output[0].x + endHalfX, RING_WEIGHT_HALF_AXIS_Q14);
    output[2].y = fixedmult(output[0].y + endHalfY, RING_WEIGHT_HALF_AXIS_Q14);
    output[6].x = fixedmult(output[8].x + startHalfX, RING_WEIGHT_HALF_AXIS_Q14);
    output[6].y = fixedmult(output[8].y + startHalfY, RING_WEIGHT_HALF_AXIS_Q14);
    output[1].x = fixedmult(output[0].x + outermostQuarterX, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[1].y = fixedmult(output[0].y + outermostQuarterY, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[7].x = fixedmult(output[8].x + initialQuarterX, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[7].y = fixedmult(output[8].y + initialQuarterY, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[3].x = fixedmult(output[0].x + threeQuarterX8, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[3].y = fixedmult(output[0].y + threeQuarterY8, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[5].x = fixedmult(output[8].x + threeQuarterX0, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[5].y = fixedmult(output[8].y + threeQuarterY0, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);

    output[12].x = fixedmult(output[8].x + -output[0].x, RING_WEIGHT_DIAGONAL_Q14);
    output[12].y = fixedmult(output[8].y + -output[0].y, RING_WEIGHT_DIAGONAL_Q14);
    output[14].x = fixedmult(endHalfX + -output[0].x, RING_WEIGHT_HALF_AXIS_Q14);
    output[14].y = fixedmult(endHalfY + -output[0].y, RING_WEIGHT_HALF_AXIS_Q14);
    output[10].x = fixedmult(output[8].x - startHalfX, RING_WEIGHT_HALF_AXIS_Q14);
    output[10].y = fixedmult(output[8].y - startHalfY, RING_WEIGHT_HALF_AXIS_Q14);
    output[15].x = fixedmult(outermostQuarterX + -output[0].x, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[15].y = fixedmult(outermostQuarterY + -output[0].y, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[9].x = fixedmult(output[8].x - initialQuarterX, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[9].y = fixedmult(output[8].y - initialQuarterY, RING_WEIGHT_QUARTER_AXIS_Q14);
    output[13].x = fixedmult(threeQuarterX8 + -output[0].x, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[13].y = fixedmult(threeQuarterY8 + -output[0].y, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[11].x = fixedmult(output[8].x - threeQuarterX0, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);
    output[11].y = fixedmult(output[8].y - threeQuarterY0, RING_WEIGHT_THREE_QUARTER_AXIS_Q14);

    for (index = 0; index < SPHERE_RING_POINT_COUNT; ++index) {
        output[index + SPHERE_BACK_RING_OFFSET].x = source[0].x - output[index].x;
        output[index + SPHERE_BACK_RING_OFFSET].y = source[0].y - output[index].y;
        output[index].x += source[0].x;
        output[index].y += source[0].y;
    }
}
