#include "stunts_types.h"
/* READABILITY: Build a sphere point buffer and pass it to the default renderer. */

#define SPHERE_SECTION_POINT_COUNT 32
#define SPHERE_POINT_BUFFER_BYTES 128 /* 32 two-coordinate points at four bytes each. */
extern void FAR preRender_sphere_helper2(I16, I8 *);
extern void FAR preRender_default_alt(I16, I16, I8 *);
/* Build sphere points in a temporary buffer, then submit them to the default renderer.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): submit the generated sphere points to the default renderer. */
void preRender_sphere_helper(I16 source, I16 destination)
{
    I8 buffer[SPHERE_POINT_BUFFER_BYTES];
    preRender_sphere_helper2(source, buffer);
    /* PLATFORM(video): submit the sphere vertex buffer to the renderer. */
    preRender_default_alt(destination, SPHERE_SECTION_POINT_COUNT, buffer);
}
