/* Shared fontrec declarations; DOS medium-model types. */
#ifndef STUNTS_FONTREC_H
#define STUNTS_FONTREC_H
struct FONTDEF_PREFIX;
struct SCREEN_RECT;
#pragma pack()
struct FONTDEF_PREFIX {
    unsigned char bytes[14];
    unsigned short value;
};
#pragma pack()
struct SCREEN_RECT {
    short foreground;
    short background;
    short font_header_middle[7];
    short line_height;
};
#endif
