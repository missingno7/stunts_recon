struct VECTOR { int x, y, z; };
extern int polarRadius2D(int, int);
int polarRadius3D(struct VECTOR *vec) { return polarRadius2D(polarRadius2D(vec->x, vec->y), vec->z); }
