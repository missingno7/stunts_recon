extern void far sub_35B76(int, int, int, int, int);

void sub_3702E(int x1, int y1, int x2, int y2, int color)
{
    int width;
    int height;

    width = x2 - x1 + 1;
    height = y2 - y1 - 1;

    if (width > 0) {
        sub_35B76(x1, y1, width, 1, color);
        sub_35B76(x1, y2, width, 1, color);
    }

    if (height > 0) {
        sub_35B76(x1, y1 + 1, 1, height, color);
        sub_35B76(x2, y1 + 1, 1, height, color);
    }
}
