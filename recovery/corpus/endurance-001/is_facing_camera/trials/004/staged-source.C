struct POINT2D { int px, py; };
char is_facing_camera(struct POINT2D far *pts)
{
    long dx0, dy0, dx1, dy1;
    long temp;
    dx0 = (long)pts[0].px - pts[1].px;
    dx1 = (long)pts[2].px - pts[1].px;
    if (dx0 == 0 && dx1 == 0) return 0;
    dy0 = (long)pts[0].py - pts[1].py;
    dy1 = (long)pts[2].py - pts[1].py;
    if (dy0 == 0 && dy1 == 0) return 0;
    temp = (dx1 * dy0) - (dx0 * dy1);
    return temp <= 0 ? 0 : 1;
}
