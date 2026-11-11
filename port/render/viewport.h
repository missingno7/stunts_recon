#ifndef PORT_RENDER_VIEWPORT_H
#define PORT_RENDER_VIEWPORT_H

/* The MCGA grid is 320x200; its intended display is 4:3 (5:6 pixel
   aspect). Presentation alone corrects this; game coordinates stay intact. */
typedef struct PortViewport { float x, y, w, h; } PortViewport;

static inline PortViewport port_viewport_fit(int width, int height)
{
    PortViewport result = {0, 0, 0, 0};
    if (width <= 0 || height <= 0) return result;
    if ((double)width * 3 <= (double)height * 4) {
        result.w = (float)width;
        result.h = (float)((double)width * 3 / 4);
    } else {
        result.h = (float)height;
        result.w = (float)((double)height * 4 / 3);
    }
    result.x = ((float)width - result.w) / 2;
    result.y = ((float)height - result.h) / 2;
    return result;
}

/* SDL events are window coordinates; the destination is in render-output
   pixels. Convert using both sizes, including fractional HiDPI scaling. */
static inline void port_viewport_mouse(int window_w, int window_h,
                                      int output_w, int output_h,
                                      float x, float y, int *lx, int *ly)
{
    PortViewport v = port_viewport_fit(output_w, output_h);
    double u = 0, t = 0;
    if (window_w > 0 && window_h > 0 && v.w > 0 && v.h > 0) {
        u = ((double)x * output_w / window_w - v.x) * 320 / v.w;
        t = ((double)y * output_h / window_h - v.y) * 200 / v.h;
    }
    *lx = u <= 0 ? 0 : u >= 320 ? 319 : (int)u;
    *ly = t <= 0 ? 0 : t >= 200 ? 199 : (int)t;
}

/* Invert the presentation-only cockpit overscan for interactive HUD bands.
   The caller supplies the clip of the displayed snapshot, never guest globals. */
static inline void port_viewport_cockpit_mouse(int window_w, int window_h,
    int output_w, int output_h, float x, float y, int top, int bottom,
    int *lx, int *ly)
{
    port_viewport_mouse(window_w,window_h,output_w,output_h,x,y,lx,ly);
    PortViewport v=port_viewport_fit(output_w,output_h);
    if (window_w<=0 || window_h<=0 || v.w<=0 || v.h<=0 || output_w<=v.w) return;
    if (top<0) top=0;
    if (top>200) top=200;
    if (bottom<top) bottom=top;
    if (bottom>200) bottom=200;
    double py=(double)y*output_h/window_h;
    double edge;
    if (top>0 && py<v.y+top*v.h/200) edge=top;
    else if (bottom<200 && py>=v.y+bottom*v.h/200) edge=bottom;
    else return;
    double u=(double)x/window_w*320;
    double scale=output_w/v.w;
    double t=edge+(py-(v.y+edge*v.h/200))*200/(v.h*scale);
    *lx=u<=0?0:u>=320?319:(int)u;
    *ly=t<=0?0:t>=200?199:(int)t;
}
#endif
