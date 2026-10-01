#include "render/viewport.h"
#include <assert.h>
#include <math.h>

static void rectangle(int w, int h, float x, float y, float vw, float vh)
{
    PortViewport v = port_viewport_fit(w, h);
    assert(fabs(v.x - x) < .001 && fabs(v.y - y) < .001);
    assert(fabs(v.w - vw) < .001 && fabs(v.h - vh) < .001);
}

int main(void)
{
    rectangle(960, 720, 0, 0, 960, 720);
    rectangle(1920, 1080, 240, 0, 1440, 1080);
    rectangle(900, 1600, 0, 462.5f, 900, 675);
    rectangle(1001, 701, 199.0f / 6, 0, 2804.0f / 3, 701);
    rectangle(160, 100, 40.0f / 3, 0, 400.0f / 3, 100);
    rectangle(0, 0, 0, 0, 0, 0);
    const int sizes[][4] = {
        {960,720,960,720}, {1280,720,1920,1080},
        {701,1001,1402,2002}, {1001,701,1502,1052}, {160,100,160,100}
    };
    for (unsigned i = 0; i < sizeof sizes / sizeof sizes[0]; ++i) {
        const int *s = sizes[i];
        PortViewport v = port_viewport_fit(s[2], s[3]);
        for (int y = 0; y < 200; ++y) for (int x = 0; x < 320; ++x) {
            /* Every logical pixel centre maps back, including non-integer
               scaling, portrait letterboxes and differing window/output. */
            float wx = (v.x + (x + .5f) * v.w / 320) * s[0] / s[2];
            float wy = (v.y + (y + .5f) * v.h / 200) * s[1] / s[3];
            int lx, ly;
            port_viewport_mouse(s[0], s[1], s[2], s[3], wx, wy, &lx, &ly);
            assert(lx == x && ly == y);
        }
        int lx, ly;
        port_viewport_mouse(s[0], s[1], s[2], s[3], -100, -100, &lx, &ly);
        assert(lx == 0 && ly == 0);
        port_viewport_mouse(s[0], s[1], s[2], s[3], s[0]+100, s[1]+100, &lx, &ly);
        assert(lx == 319 && ly == 199);
    }
    return 0;
}
