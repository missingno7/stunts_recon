/* READABILITY: Define far plane and vector state initialized in the far-data contribution. */

#include "math3d.h"

#include "matrix.h"

#include "viewstat.h"
#include "game.h"

/* Local view: plan_memres; type is struct PLANE; another unit uses array[None]. */
struct PLANE far plan_memres = { 0, 0, { 0, 0, 0 }, { 0, 8192, 0 } };
int far unk_3B1E2[7] = { 0 };
