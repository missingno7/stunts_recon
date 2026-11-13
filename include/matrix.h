#ifndef STUNTS_MATRIX_H
#define STUNTS_MATRIX_H

/* Canonical matrix and plane records. */
#include "types.h"
#define MATRIX_ELEMENT_COUNT 9
struct MATRIX { I16 vals[MATRIX_ELEMENT_COUNT]; };
/* Shared plane declarations; DOS medium-model types. */
struct MATRIX;
struct PLANE;
struct VECTOR;
#include "math3d.h"
#pragma pack()
struct PLANE {
    int plane_yz;
    int plane_xy;
    struct VECTOR plane_origin;
    struct VECTOR plane_normal;
    struct MATRIX plane_rotation;
};

#endif
