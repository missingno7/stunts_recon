/* Shared polyinfo declarations; DOS medium-model types. */
#ifndef STUNTS_POLYINFO_H
#define STUNTS_POLYINFO_H
struct POINT2D;
struct POLYINFO;
#pragma pack()
struct POLYINFO {
    int depth;
    unsigned char material;
    char numpoints;
    char type;
    char reserved;
    struct POINT2D points[10];
};
#endif
