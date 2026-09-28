/* READABILITY: Generate wheel perimeter points and offset their paired ring by the rectangle origin delta. */
struct WheelRect { int left, top, unused0, unused1, unused2, unused3, x, y; };
struct Point { int x, y; };
extern void far preRender_wheel_helper2(struct WheelRect *, struct Point *, int);
/* Generate wheel section rings and copy a translated ring using the rectangle origin delta.
 * Params and return follow the declared C signature. */
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
