#ifndef STUNTS_SHAPES3D_H
#define STUNTS_SHAPES3D_H

/* Three-dimensional shape submission and primitive rendering. */
/* Shared prim3d declarations; unnamed prototype parameters keep C1 allocation stable. */
struct MATRIX;
struct POINT2D;
struct Point;
struct RECTANGLE;
struct WheelRect;
extern struct RECTANGLE car_menu_clip_rect;
extern struct RECTANGLE carmenu_cliprect;
extern struct RECTANGLE empty_rectangle;
extern int facenodeiterator;
extern int *g_mat_clrlist_copy_2_ptr;
extern struct MATRIX g_rot_mat_z;
extern struct RECTANGLE intro_cliprect;
extern long inverse_power_of_two_table[32];
extern int *mat_copy_clr_lst_ptr;
extern unsigned mat_y_rot_angle;
extern int *material_clrlist2_ptr;
extern short material_pad_7;
extern int *material_patlist2_ptr;
extern int *material_patlist_ptr;
extern int *material_patlistptr_copy;
extern int *matpatlistcopypointer2;
extern struct MATRIX matrotation_tmp;
extern unsigned char NP[16];
extern int poly_cursor1;
extern int poly_link_listit4;
extern unsigned int poly_linked_list_40ED6[];
extern int polygon_link_3_list_iter;
extern unsigned char far *polyinfoptrs[];
extern int projectiondata10;
extern int projectiondata5;
extern int projectiondata8;
extern int projectiondata9;
extern unsigned char PT[16];
extern struct RECTANGLE trackpreview_cliprect;
extern unsigned char transformed_vert_count;
extern int wall_facingang;
extern void far addzone(char *, struct RECTANGLE *, struct RECTANGLE *);
extern unsigned far clipline(unsigned, unsigned, unsigned, unsigned, int *);
extern void far dosteering(char);
extern void far DRAW_OBJECTS(void);
extern void far drawpolyp(int, int, char *);
extern char far front_facing(struct POINT2D far *);
extern unsigned far getoutcode(struct POINT2D *);
extern void far INIT_BUFFERS(void);
extern void far initnm(void);
extern long far lgabs(long);
extern void far mergezones(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
extern unsigned far polysort(unsigned, unsigned);
extern void far preRender_line(int, int, int, int, int);
extern void far preRender_patterned(int, int, int, struct POINT2D *);
extern void far preRender_wheel_helper4(int, int, ...);
extern void far ring(struct WheelRect *, struct Point *, int);
extern void far setprojectp(int, int, int, int);
extern void far remove_3dpieces(void);
extern void far remove_3dcars(void);
extern void far init_3dcars(char [], char []);
extern void far solidellipseh(int, int);
extern void far sortzonelist(char, struct RECTANGLE *, int *);
extern void far transformed_shape_add_for_sort(int, char);
extern char far zoneadjacent(struct RECTANGLE *, struct RECTANGLE *);
extern void far zonecheck(struct POINT2D *, struct RECTANGLE *);
extern char far zoneinside(struct RECTANGLE *, struct RECTANGLE *);
extern char far zoneoverlap(struct RECTANGLE *, struct RECTANGLE *);
/* Protocol and geometry values used by the shared routines. */
#define CLIP_OUTCODE_ALL 0x0F
#define CLIP_OUTCODE_BOTTOM 2
#define CLIP_OUTCODE_LEFT 4
#define CLIP_OUTCODE_RIGHT 8
#define CLIP_OUTCODE_TOP 1
#define CULL_ANGLE_BIN_SHIFT 10
#define CULL_AZIMUTH_BIN_COUNT 15L
#define CULL_DIRECTIONS_ALL -1L
#define CULL_LOWER_POLE_INDEX 0x1E
#define CULL_UPPER_HEMISPHERE_OFFSET 0x0F
#define CULL_UPPER_POLE_INDEX 0x1F
#define CULL_VIEW_AXIS_LENGTH 0x1000
#define MATERIAL_BACKLIGHT_DYNAMIC 0x2D
#define MATERIAL_BACKLIGHT_OFF 0x2E
#define MATERIAL_BACKLIGHT_ON 0x2F
#define PAINT_CYCLE_MASK 0xF
#define POLYGON_FILL_PATTERNED 1
#define POLYGON_FILL_SECONDARY_PARAMETERS 2
#define POLYGON_FILL_SOLID 0
#define POLYINFO_LAST_START_OFFSET 0x2872
#define POLYINFO_RECORD_PREFIX_BYTES 6
#define POLYINFO_TYPE_LINE 1
#define POLYINFO_TYPE_PIXEL 5
#define POLYINFO_TYPE_POLYGON 0
#define POLYINFO_TYPE_SPHERE 2
#define POLYINFO_TYPE_WHEEL 3
#define PRIMITIVE_FLAG_CONTINUATION 2
#define PRIMITIVE_FLAG_DOUBLE_SIDED 1
#define PRIMITIVE_PREFIX_BYTES 2
#define SHAPE3D_CATALOG_CAPACITY 130
#define SHAPE3D_FLAG_ACCUMULATE_RECT 8
#define SHAPE3D_FLAG_KEEP_INSERTION_POSITION 1
#define SHAPE3D_FLAG_VIEW_SPACE_POSITION 2
#define SHAPE3D_FLAT_BOUNDING_VERTICES 4
#define SHAPE3D_HALF_SCALE_SHIFT 1
#define SHAPE3D_INITIAL_BOUNDING_VERTICES 8
#define SHAPE3D_NEAR_PLANE_Z 12
#define SHAPE3D_PRIMITIVE_STREAM_END 0
#define SHAPE3D_PRIMITIVE_TYPE_COUNT 16
#define SHAPE3D_VERTEX_CAPACITY 255
#define SHAPE3D_VERTEX_CODE_CAPACITY 256
#define SHAPE_OPPONENT_AUXILIARY 129
#define SHAPE_OPPONENT_BODY 125
#define SHAPE_OPPONENT_WHEELS 127
#define SHAPE_PLAYER_AUXILIARY 128
#define SHAPE_PLAYER_BODY 124
#define SHAPE_PLAYER_WHEELS 126
#define SHAPE_SHARED_RESOURCE_COUNT 0x74
#define SHAPE_WHEEL_VERTEX_START 8
#define START_GATE_ARM_LENGTH 0x24
#define START_GATE_PIVOT_HEIGHT 0x38
#define TRANSFORM_RESULT_NOT_VISIBLE -1
#define TRANSFORM_RESULT_POOL_FULL 1
#define TRANSFORM_RESULT_SUBMITTED 0
#define VERTEX_CLIP_BEHIND_NEAR 1
#define VERTEX_CLIP_PROJECTED 0
#define VERTEX_CLIP_UNTRANSFORMED -1
#define VIEW_HEADING_AXIS_LENGTH 0x2710
#define WHEEL_GEOMETRY_ANGLE_CACHE_INDEX 4
#define WHEEL_GEOMETRY_LAST_WHEEL_VERTEX_START 18
#define WHEEL_GEOMETRY_REAR_VERTEX_START 12
#define WHEEL_GEOMETRY_VERTICES_PER_WHEEL 6
#define WHEEL_INNER_RADIUS_Q14 0x2500
#define WHEEL_QUAD_POINT_COUNT 4
/* Canonical SHAPE3DHEADER view; incompatible operational views stay local. */
#pragma pack()
struct SHAPE3DHEADER {
    unsigned char header_numverts;
    unsigned char header_numprimitives;
    unsigned char header_numpaints;
    unsigned char header_reserved;
};
/* Shared primrec declarations; DOS medium-model types. */
struct MATERIALCLRLIST;
struct RECTANGLE;
struct SHAPE3D;
struct TRANSFORMEDSHAPE;
struct VECTOR;
#include "math3d.h"
#pragma pack()
struct MATERIALCLRLIST {
    unsigned char colors_00_0f[0x20];
    short ground;
    short sky;
    unsigned char colors_12_63[0xa4];
    short water;
};
#pragma pack()
struct TRANSFORMEDSHAPE {
    struct VECTOR pos;
    struct SHAPE3D *shape;
    struct RECTANGLE *rect;
    struct VECTOR rotation;
    int scale;
    unsigned char flags;
    unsigned char material;
};
/* Shared polyinfo declarations; DOS medium-model types. */
struct POINT2D;
struct POLYINFO;
#pragma pack()
struct POLYINFO {
    int depth;
    unsigned char material;
    char numpoints;
    char type;
    char reserved;
    struct POINT2D points[10];
};
/* Polygon point generation for sphere and wheel primitives. */
#define WHEEL_RING_POINT_COUNT 16
#define WHEEL_HALF_RING_POINT_COUNT 8
#define SPHERE_RING_POINT_COUNT 16
#define SPHERE_BACK_RING_OFFSET 16

/* Named values with reviewed widths and meanings. */
#define POLYINFO_CAPACITY 400
#define POLYINFO_POOL_BYTES 0x28A0
#define POLYINFO_RESET_SENTINEL 0xffff
#define SHAPE3D_RESOURCE_CULL_RECORD_BYTES 4
#define SHAPE3D_RESOURCE_HEADER_BYTES 4
#define SHAPE3D_RESOURCE_VERTEX_BYTES 6
#define SPHERE_POINT_BUFFER_BYTES 128
#define SPHERE_SECTION_POINT_COUNT 32
#define WHEEL_DRAW_FACE_POINT_COUNT 0x12
#define WHEEL_OUTER_RING_OFFSET 32
#define WHEEL_POINT_BUFFER_COUNT 48
#define WHEEL_SECOND_RING_OFFSET 16

/* Shape submission, materials and geometry. */
struct POINT2D;
struct RECTANGLE;
struct WheelRect;
struct coord_pair;
extern unsigned polygonnumber;
extern unsigned char  renderlevel;
extern unsigned char shape_template_points_a[6];
extern struct coord_pair shape_template_points_b[12];
extern struct coord_pair shape_template_points_c[3];
extern struct coord_pair shape_template_points_d[3];
extern struct coord_pair shape_template_points_e[3];
extern struct coord_pair shape_template_points_f[6];
extern char skybox_loaded;
extern void drawpolypt(int, int, int*);
extern int init_3dpieces(void);
extern unsigned INIT_OBJECTS_BUFFER(int, int, int, struct RECTANGLE*, int);
extern void far initscenery(unsigned char);
extern void makezonelist(unsigned char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);
extern void far solidtire(struct POINT2D*, int, int, int, int);
extern void far tire(struct WheelRect *, int *, int);
#endif
