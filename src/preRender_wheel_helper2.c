struct Point { int x, y; };
struct WheelRect { struct Point p0, p1, p2; };
extern int far mulscl(int delta, int scale);
extern void far preRender_wheel_helper3(struct Point *source, struct Point *output);
void preRender_wheel_helper2(struct WheelRect *rect, struct Point *output, int scale)
{
    struct WheelRect points;
    points.p0 = rect->p0;
    points.p1.x = mulscl(rect->p1.x - rect->p0.x, scale) + rect->p0.x;
    points.p1.y = mulscl(rect->p1.y - rect->p0.y, scale) + rect->p0.y;
    points.p2.x = mulscl(rect->p2.x - rect->p0.x, scale) + rect->p0.x;
    points.p2.y = mulscl(rect->p2.y - rect->p0.y, scale) + rect->p0.y;
    preRender_wheel_helper3(&rect->p0, output);
    preRender_wheel_helper3(&points.p0, output + 16);
}
