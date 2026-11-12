
#include "prim3d.h"
#include "stunts_types.h"
/* READABILITY: Build a sphere point buffer and pass it to the default renderer. */

#define SPHERE_SECTION_POINT_COUNT 32
#define SPHERE_POINT_BUFFER_BYTES 128 /* 32 two-coordinate points at four bytes each. */
/* Local view: ellipseh; parameters[0] is signed word; another unit uses near pointer. */
extern void FAR ellipseh(I16, I8 *);

/* Build sphere points in a temporary buffer, then submit them to the default renderer.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): submit the generated sphere points to the default renderer. */
void solidellipseh(I16 source, I16 destination)
{
    I8 buffer[SPHERE_POINT_BUFFER_BYTES];
    ellipseh(source, buffer);
    /* PLATFORM(video): submit the sphere vertex buffer to the renderer. */
    drawpolyp(destination, SPHERE_SECTION_POINT_COUNT, buffer);
}
