#include "math3d.h"
#include "viewstat.h"
#include "shapes3d.h"
#include "types.h"
/* Build a sphere point buffer and pass it to the default renderer. */

#define SPHERE_SECTION_POINT_COUNT 32
#define SPHERE_POINT_BUFFER_BYTES 128 /* 32 two-coordinate points at four bytes each. */
#include "ellipse.h"

/* Build sphere points in a temporary buffer, then submit them to the default renderer.
 * Params and return follow the declared C signature. */
void solidellipseh(I16 source, I16 destination)
{
    I8 buffer[SPHERE_POINT_BUFFER_BYTES];
    ellipseh(source, buffer);
    drawpolyp(destination, SPHERE_SECTION_POINT_COUNT, buffer);
}
