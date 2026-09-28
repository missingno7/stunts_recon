extern void far draw_filled_rect(int, int, int, int, int);

void draw_rect_outline(int x1, int y1, int x2, int y2, int color)
{
    int width;
    int height;

    width = x2 - x1 + 1;
    height = y2 - y1 - 1;

    if (width > 0) {
        draw_filled_rect(x1, y1, width, 1, color);
        draw_filled_rect(x1, y2, width, 1, color);
    }

    if (height > 0) {
        draw_filled_rect(x1, y1 + 1, 1, height, color);
        draw_filled_rect(x2, y1 + 1, 1, height, color);
    }
}
