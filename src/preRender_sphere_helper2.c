#include "stunts_types.h"
/* READABILITY: Build the thirty-two front/back perimeter points for a sphere section from three source points. */

#define SPHERE_RING_POINT_COUNT 16
#define SPHERE_BACK_RING_OFFSET 16
struct Point { I16 x; I16 y; };
extern I16 FAR mulscl(I16, I16);
/* Interpolate and translate the front and rear sphere-section point rings.
 * Params and return follow the declared C signature. */
void FAR preRender_sphere_helper2(struct Point *source, struct Point *output)
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
    startHalfX = output[0].x >> 1;
    initialQuarterX = startHalfX >> 1;
    threeQuarterX0 = startHalfX + initialQuarterX;
    startHalfY = output[0].y >> 1;
    initialQuarterY = startHalfY >> 1;
    threeQuarterY0 = startHalfY + initialQuarterY;
    endHalfX = output[8].x >> 1;
    outermostQuarterX = endHalfX >> 1;
    threeQuarterX8 = endHalfX + outermostQuarterX;
    endHalfY = output[8].y >> 1;
    outermostQuarterY = endHalfY >> 1;
    threeQuarterY8 = endHalfY + outermostQuarterY;

    output[4].x = mulscl(output[0].x + output[8].x, 0x2d41);
    output[4].y = mulscl(output[0].y + output[8].y, 0x2d41);
    output[2].x = mulscl(output[0].x + endHalfX, 0x393e);
    output[2].y = mulscl(output[0].y + endHalfY, 0x393e);
    output[6].x = mulscl(output[8].x + startHalfX, 0x393e);
    output[6].y = mulscl(output[8].y + startHalfY, 0x393e);
    output[1].x = mulscl(output[0].x + outermostQuarterX, 0x3e17);
    output[1].y = mulscl(output[0].y + outermostQuarterY, 0x3e17);
    output[7].x = mulscl(output[8].x + initialQuarterX, 0x3e17);
    output[7].y = mulscl(output[8].y + initialQuarterY, 0x3e17);
    output[3].x = mulscl(output[0].x + threeQuarterX8, 0x3333);
    output[3].y = mulscl(output[0].y + threeQuarterY8, 0x3333);
    output[5].x = mulscl(output[8].x + threeQuarterX0, 0x3333);
    output[5].y = mulscl(output[8].y + threeQuarterY0, 0x3333);

    output[12].x = mulscl(output[8].x + -output[0].x, 0x2d41);
    output[12].y = mulscl(output[8].y + -output[0].y, 0x2d41);
    output[14].x = mulscl(endHalfX + -output[0].x, 0x393e);
    output[14].y = mulscl(endHalfY + -output[0].y, 0x393e);
    output[10].x = mulscl(output[8].x - startHalfX, 0x393e);
    output[10].y = mulscl(output[8].y - startHalfY, 0x393e);
    output[15].x = mulscl(outermostQuarterX + -output[0].x, 0x3e17);
    output[15].y = mulscl(outermostQuarterY + -output[0].y, 0x3e17);
    output[9].x = mulscl(output[8].x - initialQuarterX, 0x3e17);
    output[9].y = mulscl(output[8].y - initialQuarterY, 0x3e17);
    output[13].x = mulscl(threeQuarterX8 + -output[0].x, 0x3333);
    output[13].y = mulscl(threeQuarterY8 + -output[0].y, 0x3333);
    output[11].x = mulscl(output[8].x - threeQuarterX0, 0x3333);
    output[11].y = mulscl(output[8].y - threeQuarterY0, 0x3333);

    for (index = 0; index < SPHERE_RING_POINT_COUNT; ++index) {
        output[index + SPHERE_BACK_RING_OFFSET].x = source[0].x - output[index].x;
        output[index + SPHERE_BACK_RING_OFFSET].y = source[0].y - output[index].y;
        output[index].x += source[0].x;
        output[index].y += source[0].y;
    }
}
