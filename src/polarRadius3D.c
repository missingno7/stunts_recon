/* READABILITY: Compute the 3D polar radius by reducing x/y and then z through the project polarRadius helper. */
struct VECTOR { int x, y, z; };
extern int polradius2d(int, int);
/* Reduce a 3D vector to its polar radius using the shared 2D helper.
 * Params and return follow the declared C signature. */
int polarRadius3D(struct VECTOR *vec) { return polradius2d(polradius2d(vec->x, vec->y), vec->z); }
