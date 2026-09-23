struct POINT2D { int x, y; };

int is_facing_camera(struct POINT2D far *pts)
{
    long dx0;
    long dy0;
    long dx1;
    long dy1;

    dx0 = (long)pts[0].x - pts[1].x;
    dx1 = (long)pts[2].x - pts[1].x;
    if (dx0 == 0 && dx1 == 0) return 0;
    dy0 = pts[0].y - pts[1].y;
    dy1 = pts[2].y - pts[1].y;
    if (dy0 == 0 && dy1 == 0) return 0;
    return dx1 * dy0 - dx0 * dy1 > 0 ? (char)1 : (char)0;
}
