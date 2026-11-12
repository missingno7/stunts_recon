/* Shared plane declarations; DOS medium-model types. */
#ifndef STUNTS_PLANE_H
#define STUNTS_PLANE_H
struct MATRIX;
struct PLANE;
struct VECTOR;
#include "vec3.h"
#pragma pack()
struct PLANE {
    int plane_yz;
    int plane_xy;
    struct VECTOR plane_origin;
    struct VECTOR plane_normal;
    struct MATRIX plane_rotation;
};
#endif
