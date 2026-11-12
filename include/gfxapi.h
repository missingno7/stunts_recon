/* Shared gfxapi declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_GFXAPI_H
#define STUNTS_GFXAPI_H
struct POINT2D;
struct RECTANGLE;
extern int g_skybox_sky_clr;
extern int g_skyboxwat_clr;
extern int ground_skybox;
extern unsigned char palette_column_limits[2];
extern unsigned char palette_row_limits[2];
extern int palette_window_fill_color;
extern int palette_window_line_color;
extern int palette_window_line_style;
extern unsigned char palmap[16];
extern unsigned short SDPAL_mcgacol1[129];
extern unsigned short SDPAL_mcgadither[129];
extern unsigned short SDPAL_mcgatype[129];
extern unsigned short slow_video_mode_state;
extern unsigned short video_cnstval;
extern void far drawcircle(int, int, int, int);
extern void far INIT_PALETTE(void *, void *, void *, void *, unsigned short);
extern void far writepixelc(int, int, int);
extern void far skybox_op_helper(unsigned, unsigned, struct POINT2D, struct POINT2D, struct POINT2D, struct POINT2D);
extern void far skybox_op_helper2(struct RECTANGLE *, int, int);
extern void far vgapalette(unsigned short, unsigned short, unsigned char *);
extern void far video_set_mode4(void);
extern void far video_set_mode7(void);
#endif
