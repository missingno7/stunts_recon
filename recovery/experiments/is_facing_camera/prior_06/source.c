struct POINT2D { int x, y; };

int is_facing_camera(struct POINT2D far *pts)
{
    long ax, ay, bx, by;

    ax = (long)pts[0].x - pts[1].x;
    bx = (long)pts[2].x - pts[1].x;
    if (ax == 0 && bx == 0) return 0;
    ay = pts[0].y - pts[1].y;
    by = pts[2].y - pts[1].y;
    if (ay == 0 && by == 0) return 0;
    return bx * ay - ax * by > 0;
}
