#include "stunts_types.h"
/* READABILITY: Draw the four one-pixel edges of an axis-aligned rectangle through the sprite renderer. */
#define RECTANGLE_EDGE_THICKNESS_PIXELS 1
extern void FAR sprite1_unknown2(I16 x, I16 y, I16 width, I16 height, I16 color);
/* Draw the top, bottom, left, and right edges of a rectangle.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): draw rectangle edges through the sprite renderer. */
void FAR sprite_1_unk4(I16 x1, I16 y1, I16 x2, I16 y2, I16 color)
{
    I16 width;
    I16 height;
    width = x2 - x1 + RECTANGLE_EDGE_THICKNESS_PIXELS;
    height = y2 - y1;
    if (width > 0) {
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y1, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y2, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
    }
    if (height > 0) {
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y1, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x2, y1, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
    }
}
