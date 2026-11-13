/* Pointer views of the sphere and wheel ring generators. */
#ifndef STUNTS_ELLIPSE_H
#define STUNTS_ELLIPSE_H
#include "math3d.h"
extern void FAR ellipse(struct Point *, struct Point *);

/* Ellipse services and shared state. */
struct Point;
extern void far ellipseh(struct Point *, struct Point *);
#endif
