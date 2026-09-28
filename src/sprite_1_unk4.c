/* READABILITY: Draw the four one-pixel edges of an axis-aligned rectangle through the sprite renderer. */
extern void far sprite1_unknown2(int x, int y, int width, int height, int color);
/* Draw the top, bottom, left, and right edges of a rectangle.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): draw rectangle edges through the sprite renderer. */
void far sprite_1_unk4(int x1, int y1, int x2, int y2, int color)
{
    int width;
    int height;
    width = x2 - x1 + 1;
    height = y2 - y1;
    if (width > 0) {
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y1, width, 1, color);
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y2, width, 1, color);
    }
    if (height > 0) {
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x1, y1, 1, height, color);
    /* PLATFORM(video): draw one rectangle edge with the sprite renderer. */
        sprite1_unknown2(x2, y1, 1, height, color);
    }
}
