/* Canonical SHAPE2D view; incompatible operational views stay local. */
#ifndef STUNTS_SHP2REC_H
#define STUNTS_SHP2REC_H
#pragma pack()
struct SHAPE2D {
    short s2d_width;
    unsigned short s2d_height;
    unsigned short s2d_hotspot_x;
    unsigned short s2d_hotspot_y;
    unsigned short s2d_pos_x;
    unsigned short s2d_pos_y;
    unsigned char s2d_plane_color0;
    unsigned char s2d_plane_color1_bg;
    unsigned char s2d_plane_color2_flip;
    unsigned char s2d_plane_color3_flags;
};
#endif
