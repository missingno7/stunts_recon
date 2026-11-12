/* Canonical SHAPE3D view; incompatible operational views stay local. */
#ifndef STUNTS_SHAPE3D_H
#define STUNTS_SHAPE3D_H
struct VECTOR;
#pragma pack()
struct SHAPE3D {
    unsigned int numverts;
    struct VECTOR far *verts;
    unsigned int numprimitives;
    char numpaints;
    char reserved;
    unsigned char far *primitives;
    long far *cull1;
    long far *cull2;
};
#endif
