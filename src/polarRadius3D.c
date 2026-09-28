#include "stunts_types.h"
/* READABILITY: Compute the 3D polar radius by reducing x/y and then z through the project polarRadius helper. */
struct VECTOR { I16 x, y, z; };
extern I16 polradius2d(I16, I16);
/* Reduce a 3D vector to its polar radius using the shared 2D helper.
 * Params and return follow the declared C signature. */
I16 polarRadius3D(struct VECTOR *vec) { return polradius2d(polradius2d(vec->x, vec->y), vec->z); }
