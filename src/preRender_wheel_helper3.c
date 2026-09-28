/* READABILITY: Build the sixteen front/back perimeter points for a wheel section from three source points. */
typedef struct Point {
    int x;
    int y;
} Point;
extern int far mulscl(int, int);
/* Interpolate and translate the front and rear wheel-section point rings.
 * Params and return follow the declared C signature. */
void preRender_wheel_helper3(Point *source, Point *output)
{
    int half_dx;
    int half_dy;
    int ndx;
    int ndy;
    int vert;
    output[0].x = source[1].x - source[0].x;
    output[0].y = source[1].y - source[0].y;
    output[4].x = source[2].x - source[0].x;
    output[4].y = source[2].y - source[0].y;
    output[2].x = mulscl(output[0].x + output[4].x, 0x2d41);
    output[2].y = mulscl(output[0].y + output[4].y, 0x2d41);
    output[1].x = mulscl(output[0].x + (output[4].x >> 1), 0x393e);
    output[1].y = mulscl(output[0].y + (output[4].y >> 1), 0x393e);
    half_dx = output[0].x >> 1;
    output[3].x = mulscl(output[4].x + half_dx, 0x393e);
    half_dy = output[0].y >> 1;
    output[3].y = mulscl(output[4].y + half_dy, 0x393e);
    ndx = -output[0].x;
    output[6].x = mulscl(output[4].x + ndx, 0x2d41);
    ndy = -output[0].y;
    output[6].y = mulscl(output[4].y + ndy, 0x2d41);
    output[7].x = mulscl((output[4].x >> 1) + ndx, 0x393e);
    output[7].y = mulscl((output[4].y >> 1) + ndy, 0x393e);
    output[5].x = mulscl(output[4].x - half_dx, 0x393e);
    output[5].y = mulscl(output[4].y - half_dy, 0x393e);
    for (vert = 0; vert < 8; ++vert) {
        output[vert + 8].x = source[0].x - output[vert].x;
        output[vert + 8].y = source[0].y - output[vert].y;
        output[vert].x += source[0].x;
        output[vert].y += source[0].y;
    }
}
