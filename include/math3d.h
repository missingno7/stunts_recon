#ifndef STUNTS_MATH3D_H
#define STUNTS_MATH3D_H

/* Fixed-point geometry and matrix operations. */
/* Signed 16-bit world and camera vectors (DOS stride: six bytes). */
#include "types.h"
struct VECTOR { I16S x, y, z; };
/* Canonical VECTORLONG view; incompatible operational views stay local. */
#pragma pack()
struct VECTORLONG {
    long lx;
    long ly;
    long lz;
};
/* Canonical POINT2D view; incompatible operational views stay local. */
#pragma pack()
struct POINT2D {
    int x;
    int y;
};
/* Shared geom2d declarations; DOS medium-model types. */
/* Shared geom2d declarations; DOS medium-model types. */
#ifndef STUNTS_GEOM2D_H
#define STUNTS_GEOM2D_H
struct OPPONENTIMAGE;
struct RECTANGLE;
struct coord_pair;
#pragma pack()
struct coord_pair {
    short x;
    short z;
};
#pragma pack()
struct OPPONENTIMAGE {
    unsigned int width;
    unsigned int height;
};
#pragma pack()
struct RECTANGLE {
    int left;
    int right;
    int top;
    int bottom;
};
#endif

/* Shared mathrec declarations; DOS medium-model types. */
struct MAT3;
#pragma pack()
struct MAT3 {
    int _11;
    int _21;
    int _31;
    int _12;
    int _22;
    int _32;
    int _13;
    int _23;
    int _33;
};
#include "types.h"
struct Point { I16 x, y; };
typedef struct Point Point;
/* Fixed-point arithmetic, rotations and projection in DOS word coordinates. */
struct POINT2D;
/* norm_zone widens this result by zero extension in the locked code. */

/* Named values with reviewed widths and meanings. */
#define ANGLE_EIGHTH_TURN 0x80
#define ANGLE_HALF_TURN 0x200
#define ANGLE_QUARTER_TURN 0x100
#define ANGLE_THREE_QUARTERS_TURN 0x300
#define ANGLE_TURN_MASK 0x03ff
#define ANGLE_UNITS_PER_TURN 0x400
#define MATRIX_IDENTITY_Q14 0x4000
#define Q8_FRACTION_BITS 8
#define RING_HALF_COMPONENT_SHIFT 1
#define RING_WEIGHT_DIAGONAL_Q14 0x2d41
#define RING_WEIGHT_HALF_AXIS_Q14 0x393e
#define RING_WEIGHT_QUARTER_AXIS_Q14 0x3e17
#define RING_WEIGHT_THREE_QUARTER_AXIS_Q14 0x3333
#define WORLD_COORDINATE_SHIFT 6
/* Shared mathapi declarations; unnamed prototype parameters keep C1 allocation stable. */
#include "radius2d.h"
struct MATRIX;
struct VECTOR;
extern struct MATRIX g_matrix_yrot;
extern struct VECTOR g_op_carvector2;
extern char g_vector_bitix;
extern struct MATRIX matrix_transform_view;
extern struct MATRIX matrix_x_rotation;
extern void far INIT_MATRIX(void);
extern void far shellsort(int, int *, int *);
#include "types.h"
struct VECTOR;
struct MATRIX;
struct POINT2D;
extern I16 FAR fixedmult(I16, I16);
extern I16 FAR intatan(I16, I16);
extern I16 FAR int3dhypot(struct VECTOR *);
extern void FAR mat_vec(struct VECTOR *, struct MATRIX *, struct VECTOR *);
extern void FAR mat_mul_vector2(struct VECTOR *, struct MATRIX FAR *, struct VECTOR *);
extern void FAR transmult(struct MATRIX *, struct MATRIX *, struct MATRIX *);
extern void FAR mat_invert(struct MATRIX *, struct MATRIX *);
extern void FAR xformx(struct MATRIX *, I16);
extern void FAR xformy(struct MATRIX *, I16);
extern void FAR xformz(struct MATRIX *, I16);
extern struct MATRIX * FAR makematrix(I16, I16, I16, I16);
extern I16 FAR norm_zone(struct VECTOR *);
extern void FAR projectp(struct VECTOR *, struct POINT2D *);
extern void FAR zclipline(struct VECTOR *, struct VECTOR *, struct VECTOR *, I16S);
/* Protocol and geometry values used by the shared routines. */
#define ANGLE_NEGATIVE_EIGHTH_TURN_U16 0xff80
#define ANGLE_QUADRANT_OFFSET_MASK 0xff
#define HEADING_OCTANT_SHIFT 7
#define MATRIX_Y_ROTATION_CACHE_INVALID 0xFFFF
#define PLANE_NORMAL_DOT_DIVISOR 0x2000
#define ROTATION_COMPOSE_REVERSED 1
#define ROTATION_PRESENT_X 2
#define ROTATION_PRESENT_Y 1
#define ROTATION_PRESENT_Z 4
#define WORLD_COORDINATE_SCALE 64L
#define WORLD_COORDINATE_SCALE_I16 64

/* Math3d services and shared state. */
struct PLANE;
struct RECTANGLE;
extern struct PLANE far* g_planlist;
extern struct PLANE far plan_memres[];
extern struct PLANE far* plncurrptr;
extern char clipzone(struct RECTANGLE *, struct RECTANGLE *);
#endif
