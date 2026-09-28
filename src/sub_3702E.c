/* READABILITY: Draw the four one-pixel edges of an axis-aligned rectangle through the filled-rectangle renderer. */
extern void far draw_filled_rect(int, int, int, int, int);

/* Draw the four one-pixel edges of an axis-aligned rectangle.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): draw all four outline edges through the rectangle renderer. */
void draw_rect_outline(int x1, int y1, int x2, int y2, int color)
{
    int width;
    int height;

    width = x2 - x1 + 1;
    height = y2 - y1 - 1;

    if (width > 0) {
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y1, width, 1, color);
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y2, width, 1, color);
    }

    if (height > 0) {
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x1, y1 + 1, 1, height, color);
    /* PLATFORM(video): draw one rectangle edge through the game video renderer. */
        draw_filled_rect(x2, y1 + 1, 1, height, color);
    }
}
