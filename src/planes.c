/* READABILITY: Define far plane and vector state initialized in the far-data contribution. */

#include "vec3.h"
/* Local view: MATRIX; local MATRIX member view differs from math3d.h. */
struct MATRIX { int m[9]; };
#include "plane.h"

#include "game.h"

/* Local view: plan_memres; type is struct PLANE; another unit uses array[None]. */
struct PLANE far plan_memres = { 0, 0, { 0, 0, 0 }, { 0, 8192, 0 } };
int far unk_3B1E2[7] = { 0 };
