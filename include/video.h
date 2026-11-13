#ifndef STUNTS_VIDEO_H
#define STUNTS_VIDEO_H

/* Indexed video, sprites and two-dimensional drawing. */
/* Indexed 320 by 200 display geometry and primitive drawing. */
#include "types.h"
#include "input.h"

extern void FAR xorrectc(I16, I16, I16, I16, I16);

#define HALF_SCREEN_HEIGHT_PIXELS 0x64
#define RECTANGLE_EDGE_THICKNESS_PIXELS 1
#define RECT_EMPTY_MAX_SENTINEL -1
#define RECT_EMPTY_MIN_SENTINEL 9999
#define SKYBOX_HORIZONTAL_HALF_WRAP 0x200
#define SKYBOX_HORIZONTAL_WRAP_SIZE 0x400
#define SKYBOX_PANEL3_X 0x200
#define SKYBOX_PANEL4_X 0x340
#define VIDEO_MATERIAL_TABLE_COUNT 129

/* Shared gfxapi declarations; unnamed prototype parameters keep C1 allocation stable. */
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
/* Protocol and geometry values used by the shared routines. */
#define CRACK_SOURCE_CANVAS_HEIGHT 200
#define MCGA_LAST_X 0x13f
#define MCGA_SCREEN_WIDTH_PIXELS 320
#define PREVIEW_VIEWPORT_HEIGHT 0x64
#define RECTLIST_SOURCE_FIRST 1
#define RECTLIST_SOURCE_SECOND 2
/* Shared shape2d declarations; unnamed prototype parameters keep C1 allocation stable. */
struct POINT2D;
struct SHAPE2D;
struct SPRITE;
extern struct SPRITE far *mcgawnd_window_sprite;
extern char *shape2d_extension_suffixes[6];
extern struct SPRITE far sprite2;
extern void far *far file_load_shape2d(char *, int);
extern void far file_load_shape2d_expand(unsigned char far *, char far *);
extern void far *far file_load_shape2d_nofatal2(char *);
extern void far file_load_shape2d_palmap_apply(unsigned char far *, unsigned char []);
extern void far file_load_shape2d_palmap_init(unsigned char far *);
extern void far file_unflip_shape2d(unsigned char far *, char far *);
extern void far file_unflip_shape2d_pes(unsigned char far *, char far *);
extern void far *far load_shape2d_nofatal_thunk(const char *);
extern void far *far load_shape2d_res_nofatal_thunk(const char *);
extern void far *far loadandshapepack(char *);
extern void far *far loadandshapepacka(char *, int);
extern void far *far loadandshapepackz(char *);
extern void far *far loadshape(char *);
extern void far *far loadshapez(char *);
extern int far parse_shape2d_helper3(char far *);
extern void far setup_mcgawnd1(void);
extern void far setup_mcgawnd2(void);
extern void far shapepack(void far *, void far *);
extern void far usescreen(void);
extern void far sprite_blit_dither_phase(struct SHAPE2D far *, int);
extern void far readf(char far *);
extern void far usescreenc(void);
extern void far usebuffer(void);
extern void far usebufferc(void);
extern void far sprite_draw_beveled_frame(int, int, int, int, int, int, int);
extern void far tmaskxcfxya(void far *, int, int);
extern void far movf(void far *);
extern void far sprite_two_color_poly_list(int, int, int, int, struct POINT2D *);
/* Protocol and geometry values used by the shared routines. */
#define PES_UNFLIP_WORK_PARAGRAPHS 1000
#define SHAPE2D_RLE_END 0
#define SHAPE2D_RLE_LITERAL_127 0x81
#define SHAPE2D_RLE_MAX_PACKET_PIXELS 127
#define SHAPE2D_RLE_MIN_RUN_MINUS_ONE 3
#define SPRITE_CLIP_RECT_CAPACITY 15
#define SPRITE_SAVE_PAIR_STRIDE 2
#define SPRITE_WINDOW_OVERHEAD_BYTES 18L
/* Two-dimensional sprites, shape resources and palette remapping. */
#include "types.h"

extern void FAR drawrectc(I16 x, I16 y, I16 width, I16 height, I16 color);

#define PALETTE_REMAP_ENTRY_COUNT 16
#define SHAPE2D_HEADER_BYTES 16
#define SHAPE_UNFLIP_BUFFER_OVERHEAD_BYTES 0x20

#endif
