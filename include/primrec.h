/* Shared primrec declarations; DOS medium-model types. */
#ifndef STUNTS_PRIMREC_H
#define STUNTS_PRIMREC_H
struct MATERIALCLRLIST;
struct RECTANGLE;
struct SHAPE3D;
struct TRANSFORMEDSHAPE;
struct VECTOR;
#include "vec3.h"
#pragma pack()
struct MATERIALCLRLIST {
    unsigned char colors_00_0f[0x20];
    short ground;
    short sky;
    unsigned char colors_12_63[0xa4];
    short water;
};
#pragma pack()
struct TRANSFORMEDSHAPE {
    struct VECTOR pos;
    struct SHAPE3D *shape;
    struct RECTANGLE *rect;
    struct VECTOR rotation;
    int scale;
    unsigned char flags;
    unsigned char material;
};
#endif
