extern void far sprite1_unknown2(int x, int y, int width, int height, int color);
void far sprite_1_unk4(int x1, int y1, int x2, int y2, int color)
{
    int width;
    int height;
    width = x2 - x1 + 1;
    height = y2 - y1;
    if (width > 0) {
        sprite1_unknown2(x1, y1, width, 1, color);
        sprite1_unknown2(x1, y2, width, 1, color);
    }
    if (height > 0) {
        sprite1_unknown2(x1, y1, 1, height, color);
        sprite1_unknown2(x2, y1, 1, height, color);
    }
}
