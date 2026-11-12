/* Fixed-point arithmetic, rotations and projection in DOS word coordinates. */
#ifndef MATH3D_H
#define MATH3D_H
#include "vec3.h"
struct MATRIX { I16 vals[9]; };
struct POINT2D;
extern I16S FAR intsin(U16S);
extern I16S FAR intcos(U16S);
extern I16 FAR fixedmult(I16, I16);
/* norm_zone widens this result by zero extension in the locked code. */
extern U16 FAR inthypot(I16, I16);
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

/* Named values with reviewed widths and meanings. */
#define ANGLE_EIGHTH_TURN 0x80 /* I16, 128 */
#define ANGLE_HALF_TURN 0x200 /* I16, 512 */
#define ANGLE_NEGATIVE_EIGHTH_TURN_U16 0xff80 /* U16, 65408 */
#define ANGLE_QUADRANT_OFFSET_MASK 0xff /* I16, 255 */
#define ANGLE_QUARTER_TURN 0x100 /* I16, 256 */
#define ANGLE_THREE_QUARTERS_TURN 0x300 /* I16, 768 */
#define ANGLE_TURN_MASK 0x03ff /* I16, 1023 */
#define ANGLE_UNITS_PER_TURN 0x400 /* I16, 1024 */
#define ANGLE_ZERO 0 /* I16, 0 */
#define HEADING_OCTANT_SHIFT 7 /* I16, 7 */
#define MATRIX_ELEMENT_COUNT 9 /* I16, 9 */
#define MATRIX_IDENTITY_Q14 0x4000 /* I16, 16384 */
#define MATRIX_Y_ROTATION_CACHE_INVALID 0xFFFF /* U16, 65535 */
#define PLANE_NORMAL_DOT_DIVISOR 0x2000 /* I16, 8192 */
#define Q8_FRACTION_BITS 8 /* I16, 8 */
#define RING_HALF_COMPONENT_SHIFT 1 /* I16, 1 */
#define RING_WEIGHT_DIAGONAL_Q14 0x2d41 /* I16, 11585 */
#define RING_WEIGHT_HALF_AXIS_Q14 0x393e /* I16, 14654 */
#define RING_WEIGHT_QUARTER_AXIS_Q14 0x3e17 /* I16, 15895 */
#define RING_WEIGHT_THREE_QUARTER_AXIS_Q14 0x3333 /* I16, 13107 */
#define ROTATION_COMPOSE_REVERSED 1 /* I16, 1 */
#define ROTATION_PRESENT_X 2 /* I16, 2 */
#define ROTATION_PRESENT_Y 1 /* I16, 1 */
#define ROTATION_PRESENT_Z 4 /* I16, 4 */
#define WORLD_COORDINATE_SCALE 64L /* I32, 64 */
#define WORLD_COORDINATE_SCALE_I16 64 /* I16, 64 */
#define WORLD_COORDINATE_SHIFT 6 /* I16, 6 */
#endif
