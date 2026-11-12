/* Shared geom2d declarations; DOS medium-model types. */
#ifndef STUNTS_GEOM2D_H
#define STUNTS_GEOM2D_H
struct OPPONENTIMAGE;
struct RECTANGLE;
struct coord_pair;
#pragma pack()
struct coord_pair {
    short x;
    short z;
};
#pragma pack()
struct OPPONENTIMAGE {
    unsigned int width;
    unsigned int height;
};
#pragma pack()
struct RECTANGLE {
    int left;
    int right;
    int top;
    int bottom;
};
#endif
