/* Canonical TRACKOBJECT view; incompatible operational views stay local. */
#ifndef STUNTS_TRKOBJ_H
#define STUNTS_TRKOBJ_H
struct SHAPE3D;
struct TRKOBJINFO;
#pragma pack()
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    short ss_rotY;
    struct SHAPE3D *ss_shapePtr;
    struct SHAPE3D *ss_loShapePtr;
    unsigned char ss_ssOvelay;
    char ss_surfaceType;
    char ss_ignoreZBias;
    char ss_multiTileFlag;
    char ss_physicalModel;
    char scene_unk5;
};
#endif
