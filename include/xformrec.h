/* Canonical TRANSFORMEDSHAPE3D view; incompatible operational views stay local. */
#ifndef STUNTS_XFORMREC_H
#define STUNTS_XFORMREC_H
struct RECTANGLE;
struct SHAPE3D;
struct VECTOR;
#include "vec3.h"
#pragma pack()
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    int unk;
    unsigned char ts_flags;
    unsigned char material;
};
#endif
