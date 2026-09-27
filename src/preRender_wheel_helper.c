struct WheelRect { int left, top, unused0, unused1, unused2, unused3, x, y; };
struct Point { int x, y; };
extern void far preRender_wheel_helper2(struct WheelRect *, struct Point *, int);
void far preRender_wheel_helper(struct WheelRect *rect, struct Point *coords, int count)
{
    int dx, dy;
    int k;
    struct Point *src;
    struct Point *dst;
    preRender_wheel_helper2(rect, coords, count);
    dx = rect->x - rect->left;
    dy = rect->y - rect->top;
    src = coords; dst = coords + 32;
    for (k = 0; k < 16; ++k) {
        dst->x = src->x + dx;
        dst->y = src->y + dy;
        ++src; ++dst;
    }
}
