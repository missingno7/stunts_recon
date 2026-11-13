#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "video.h"
#include "types.h"
/* Draw the four one-pixel edges of an axis-aligned rectangle through the filled-rectangle renderer. */

/* Draw the four one-pixel edges of an axis-aligned rectangle.
 * Params and return follow the declared C signature. */
void xorbox(I16 x1, I16 y1, I16 x2, I16 y2, I16 color)
{
    I16 width;
    I16 height;

    width = x2 - x1 + RECTANGLE_EDGE_THICKNESS_PIXELS;
    height = y2 - y1 - RECTANGLE_EDGE_THICKNESS_PIXELS;

    if (width > 0) {
        xorrectc(x1, y1, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
        xorrectc(x1, y2, width, RECTANGLE_EDGE_THICKNESS_PIXELS, color);
    }

    if (height > 0) {
        xorrectc(x1, y1 + RECTANGLE_EDGE_THICKNESS_PIXELS, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
        xorrectc(x2, y1 + RECTANGLE_EDGE_THICKNESS_PIXELS, RECTANGLE_EDGE_THICKNESS_PIXELS, height, color);
    }
}
