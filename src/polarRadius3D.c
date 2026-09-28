struct VECTOR { int x, y, z; };
extern int polradius2d(int, int);
int polarRadius3D(struct VECTOR *vec) { return polradius2d(polradius2d(vec->x, vec->y), vec->z); }
