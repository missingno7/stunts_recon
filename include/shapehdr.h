/* Canonical SHAPE3DHEADER view; incompatible operational views stay local. */
#ifndef STUNTS_SHAPEHDR_H
#define STUNTS_SHAPEHDR_H
#pragma pack()
struct SHAPE3DHEADER {
    unsigned char header_numverts;
    unsigned char header_numprimitives;
    unsigned char header_numpaints;
    unsigned char header_reserved;
};
#endif
