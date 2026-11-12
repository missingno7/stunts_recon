/* Canonical SPRITE view; incompatible operational views stay local. */
#ifndef STUNTS_SPRITE_H
#define STUNTS_SPRITE_H
struct SHAPE2D;
#pragma pack()
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_unk1;
    unsigned short sprite_unk2;
    unsigned short sprite_unk3;
    unsigned int *sprite_lineofs;
    unsigned short sprite_left;
    unsigned short sprite_right;
    unsigned short sprite_top;
    unsigned short sprite_height;
    unsigned short sprite_pitch;
    unsigned short sprite_unk4;
    unsigned short sprite_width2;
    unsigned short sprite_left2;
    unsigned short sprite_widthsum;
};
#endif
