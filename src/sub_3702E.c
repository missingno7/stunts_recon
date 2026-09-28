#include "stunts_types.h"
/* READABILITY: Draw the four one-pixel edges of an axis-aligned rectangle through the filled-rectangle renderer. */
#define RECTANGLE_EDGE_THICKNESS_PIXELS 1
extern void FAR draw_filled_rect(I16, I16, I16, I16, I16);

/* Draw the four one-pixel edges of an axis-aligned rectangle.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): draw all four outline edges through the rectangle renderer. */
void draw_rect_outline(I16 x1, I16 y1, I16 x2, I16 y2, I16 color)
{
    I16 width;
    I16 height;

    width = x2 - x1 + RECTANGLE_EDGE_THICKNESS_PIXELS;
    height = y2 - y1 - RECTANGLE_EDGE_THICKNESS_PIXELS;

    if (width > 0) {
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y1, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y2, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
    }

    if (height > 0) {
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y1 + RECTANGLE_EDGE_THICKNESS_PIXELS, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x2, y1 + RECTANGLE_EDGE_THICKNESS_PIXELS, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
    }
}
