/* Shared shape2d declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_SHAPE2D_H
#define STUNTS_SHAPE2D_H
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
extern void far sprcopy2to12(void);
extern void far sprite_blit_dither_phase(struct SHAPE2D far *, int);
extern void far sprite_clear_shape(char far *);
extern void far sprite_copy_2_to_1_clear(void);
extern void far sprite_copy_wnd_to_1(void);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far sprite_draw_beveled_frame(int, int, int, int, int, int, int);
extern void far sprite_putimage_transparent(void far *, int, int);
extern void far sprite_shape_to_1_alt(void far *);
extern void far sprite_two_color_poly_list(int, int, int, int, struct POINT2D *);
#endif
