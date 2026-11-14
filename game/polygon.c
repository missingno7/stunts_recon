#include "types.h"
#include "math3d.h"
#include "matrix.h"
#include "trig.h"
#include "radius.h"

#include "shape3d.h"

#include "xformrec.h"

#include "shapes3d.h"
#include "viewstat.h"
#include "game.h"
#include "video.h"
#include "files.h"

static struct MATRIX mat_no_turn, mat_quarter_turn, mat_half_turn, mat_three_quarters_turn;
struct MATRIX g_matrix_yrot;
struct MATRIX matrix_x_rotation;
struct MATRIX g_rot_mat_z;
struct MATRIX matrotation_tmp;

static struct RECTANGLE selection_rect;
static I16 half_scale_flag;
int * mat_copy_clr_lst_ptr;
int * g_mat_clrlist_copy_2_ptr;
int * material_patlistptr_copy;
short material_pad_7;
int * matpatlistcopypointer2;
unsigned short video_cnstval;
static I32 vector_angle_cos, vector_angle_sin, vector_angle_sin2, vector_angle_cos2;
static I8 shape3d_unused[576];
unsigned polygonnumber;
static unsigned polyinfo_offset;
static I16 poly_list_insert_result;
static I16 polyinfo_reset_marker;
int poly_cursor1;
int facenodeiterator;
int polygon_link_3_list_iter;
int poly_link_listit4;
static I16 poly_link_list[POLYINFO_CAPACITY];
static I8 far* polyinfoptr;
static struct POLYINFO far* poly_info_ptrs[POLYINFO_CAPACITY];
static I8 poly_padding_bytes[6];
static struct POLYINFO far* shape_polyinfo;
static unsigned vertex_count;
static struct VECTOR far* obj_verts;
static U8  far* current_prim_start;
static U8  far* current_prim_verts;
static U8  far* shape_prim_next;
static I16 current_num_paints;
static U8  transformed_shape_material;
static U8  obj_flags;
unsigned char transformed_vert_count;
static struct RECTANGLE* current_rect;
static I8 current_paintjob;
char g_vector_bitix;

static struct POINT2D* projected_point_ptr_table[11];
#include "ui.h"
/* Allocates the polygon information pool and initializes cached rotations. */
void INIT_MATRIX(void)
{
    polyinfoptr = resmem("polyinfo", POLYINFO_POOL_BYTES);
    xformy(&mat_no_turn, 0);
    xformy(&mat_quarter_turn, ANGLE_QUARTER_TURN);
    xformy(&mat_half_turn, ANGLE_HALF_TURN);
    xformy(&mat_three_quarters_turn, ANGLE_THREE_QUARTERS_TURN);
    initnm();
}
/* Saves the material color and pattern tables used by polygon drawing. */
void INIT_PALETTE(void* clrlist, void* clrlist2, void* patlist, void* patlist2, U16S  videoConst)
{
    mat_copy_clr_lst_ptr = clrlist;
    g_mat_clrlist_copy_2_ptr = clrlist2;
    material_patlistptr_copy = patlist;
    matpatlistcopypointer2 = patlist2;
    video_cnstval = videoConst;
}
/* Resets polygon numbering, insertion state, and face-list cursors. */
void INIT_BUFFERS(void)
{
    polygonnumber = 0;
    polyinfo_offset = 0;
    poly_list_insert_result = 0;
    polyinfo_reset_marker = POLYINFO_RESET_SENTINEL;
    facenodeiterator = POLYINFO_CAPACITY;
}
/* Builds the requested view rotations and returns the camera-facing angle. */
unsigned INIT_OBJECTS_BUFFER(I16 angZ, I16 angX, I16 angY, struct RECTANGLE* cliprect, I16 unk)
{
    struct MATRIX* matptr;
    struct VECTOR vec, vec2;

    wkmatx = *makematrix(angZ, angX, angY, 1);
    INIT_BUFFERS();
    selection_rect = *cliprect;
    half_scale_flag = unk;
    matptr = makematrix(-angZ, -angX, -angY, 0);
    vec.z = VIEW_HEADING_AXIS_LENGTH;
    vec.y = 0;
    vec.x = 0;
    mat_vec(&vec, matptr, &vec2);
    return intatan(vec2.x, vec2.z) & ANGLE_TURN_MASK;
}
/* Transforms a 3D shape, clips its primitives, and appends visible polygons. */
/* Local view: ADD_OBJECT_TO_BUFFER; return is unsigned word; another unit uses void. */
unsigned ADD_OBJECT_TO_BUFFER(struct TRANSFORMEDSHAPE3D* ts)
{
    U8  ptype;
    I16 added;
    I16 primflag;
    I32 cullbits2;
    I16 someclipped;
    I32 depth;
    struct VECTOR position;
    unsigned npts;
    unsigned idx;
    struct POINT2D screenpts[SHAPE3D_VERTEX_CAPACITY];
    struct MATRIX invview;
    I32 far* cull2ptr;
    struct POINT2D far* out;
    struct MATRIX* matptr;
    struct MATRIX mview;
    unsigned prim;
    struct POINT2D corner;
    register I16 i;
    I8 rectcode;
    unsigned lastidx;
    register I16 j;
    struct VECTOR local;
    struct POINT2D far* srcpt;
    struct VECTOR viewvert;
    I16 ptx;
    I8 vertcode[SHAPE3D_VERTEX_CODE_CAPACITY];
    I16 pixsize;
    I16 hidden;
    I16 numdrawn;
    I32 cullmask;
    struct POINT2D ipt;
    I16 pty;
    struct VECTOR surfvec;
    struct VECTOR vertbuf[SHAPE3D_VERTEX_CAPACITY];
    I32 far* cullwords;

    if (poly_list_insert_result != 0)
        return TRANSFORM_RESULT_POOL_FULL;
    vertex_count = ts->shapeptr->numverts;
    current_prim_start = ts->shapeptr->primitives;
    obj_verts = ts->shapeptr->verts;
    current_num_paints = ts->shapeptr->numpaints;
    cullwords = ts->shapeptr->cull1;
    cull2ptr = ts->shapeptr->cull2;
    transformed_shape_material = ts->material;
    if (transformed_shape_material >= (U8 )current_num_paints)
        transformed_shape_material = 0;
    obj_flags = ts->ts_flags;
    if (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT)
        current_rect = ts->rectptr;
    for (i = 0; i < vertex_count; i++)
        vertcode[i] = VERTEX_CLIP_UNTRANSFORMED;

    if (obj_flags & SHAPE3D_FLAG_VIEW_SPACE_POSITION) {
        matptr = makematrix(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        transmult(matptr, &wkmatx, &mview);
        position = ts->pos;
        cullmask = CULL_DIRECTIONS_ALL;
        cullbits2 = 0;
    } else {
        matptr = makematrix(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        mat_vec(&ts->pos, &wkmatx, &position);
        transmult(matptr, &wkmatx, &mview);
        mat_invert(&mview, &invview);
        local.x = 0;
        local.y = 0;
        local.z = CULL_VIEW_AXIS_LENGTH;
        mat_vec(&local, &invview, &viewvert);
        if ((viewvert.y > 0 && ts->pos.y < 0) ||
            (ts->unk * 2 > abs(position.x) && ts->unk * 2 > abs(position.z))) {
            cullmask = CULL_DIRECTIONS_ALL;
            cullbits2 = 0;
        } else {
            g_vector_bitix = norm_zone(&viewvert);
            cullbits2 = cullmask = inverse_power_of_two_table[g_vector_bitix];
        }
    }

    poly_link_listit4 = poly_cursor1 = facenodeiterator;
    polygon_link_3_list_iter = 0;
    numdrawn = 0;
    if (vertex_count > SHAPE3D_INITIAL_BOUNDING_VERTICES)
        transformed_vert_count = SHAPE3D_INITIAL_BOUNDING_VERTICES;
    else
        transformed_vert_count = vertex_count;
    if (transformed_vert_count > SHAPE3D_FLAT_BOUNDING_VERTICES && obj_verts[0].y == obj_verts[4].y)
        transformed_vert_count = SHAPE3D_FLAT_BOUNDING_VERTICES;

    rectcode = CLIP_OUTCODE_ALL;
    hidden = 1;
    someclipped = 0;
    for (i = 0; i < transformed_vert_count; i++) {
        projected_point_ptr_table[i] = &screenpts[i];
        local = obj_verts[i];
        if (half_scale_flag != 0) {
            local.x >>= SHAPE3D_HALF_SCALE_SHIFT;
            local.y >>= SHAPE3D_HALF_SCALE_SHIFT;
            local.z >>= SHAPE3D_HALF_SCALE_SHIFT;
        }
        mat_vec(&local, &mview, &viewvert);
        viewvert.x += position.x;
        viewvert.y += position.y;
        viewvert.z += position.z;
        vertbuf[i] = viewvert;
        if (viewvert.z < SHAPE3D_NEAR_PLANE_Z) {
            vertcode[i] = VERTEX_CLIP_BEHIND_NEAR;
            someclipped = 1;
        } else {
            hidden = 0;
            vertcode[i] = VERTEX_CLIP_PROJECTED;
            projectp(&viewvert, projected_point_ptr_table[i]);
            if (rectcode != 0)
                rectcode &= getoutcode(projected_point_ptr_table[i]);
            if (rectcode == 0)
                goto visible;
        }
    }
    if (hidden != 0 || someclipped == 0 || ts->unk < abs(position.x))
        return TRANSFORM_RESULT_NOT_VISIBLE;

visible:
    current_prim_start = ts->shapeptr->primitives;
    do {
        shape_prim_next = current_prim_start + NP[*current_prim_start] + current_num_paints + PRIMITIVE_PREFIX_BYTES;
        primflag = current_prim_start[1];
        added = 0;
        if (*cullwords & cullmask) {
            prim = current_prim_start[0];
            transformed_vert_count = NP[prim];
            ptype = PT[prim];
            shape_polyinfo = (struct POLYINFO far*)(polyinfoptr + polyinfo_offset);
            poly_info_ptrs[polygonnumber] = shape_polyinfo;
            current_paintjob = current_prim_start[transformed_shape_material + PRIMITIVE_PREFIX_BYTES];
            current_prim_start += current_num_paints + PRIMITIVE_PREFIX_BYTES;
            rectcode = CLIP_OUTCODE_ALL;
            hidden = 1;
            someclipped = 0;
            current_prim_verts = current_prim_start;
            for (npts = 0; npts < transformed_vert_count; npts++) {
                i = *current_prim_verts++;
                projected_point_ptr_table[npts] = &screenpts[i];
                switch (vertcode[i]) {
                case 0:
                    hidden = 0;
                    if (rectcode != 0)
                        rectcode &= getoutcode(projected_point_ptr_table[npts]);
                    break;
                case VERTEX_CLIP_UNTRANSFORMED:
                    local = obj_verts[i];
                    if (half_scale_flag != 0) {
                        local.x >>= SHAPE3D_HALF_SCALE_SHIFT;
                        local.y >>= SHAPE3D_HALF_SCALE_SHIFT;
                        local.z >>= SHAPE3D_HALF_SCALE_SHIFT;
                    }
                    mat_vec(&local, &mview, &viewvert);
                    viewvert.x += position.x;
                    viewvert.y += position.y;
                    viewvert.z += position.z;
                    vertbuf[i] = viewvert;
                    if (viewvert.z < SHAPE3D_NEAR_PLANE_Z) {
                        vertcode[i] = VERTEX_CLIP_BEHIND_NEAR;
                        someclipped = 1;
                    } else {
                        hidden = 0;
                        vertcode[i] = VERTEX_CLIP_PROJECTED;
                        projectp(&viewvert, projected_point_ptr_table[npts]);
                        if (rectcode != 0)
                            rectcode &= getoutcode(projected_point_ptr_table[npts]);
                    }
                    break;
                case 1:
                    someclipped = 1;
                    break;
                }
            }
            if (hidden == 0 && (rectcode == 0 || someclipped != 0)) {
                switch (ptype) {
                case POLYINFO_TYPE_POLYGON:
                    out = shape_polyinfo->points;
                    current_prim_verts = current_prim_start;
                    depth = 0;
                    rectcode = CLIP_OUTCODE_ALL;
                    if (someclipped == 0) {
                        for (i = 0; i < transformed_vert_count; i++) {
                            idx = *current_prim_verts++;
                            depth += vertbuf[idx].z;
                            *out = *projected_point_ptr_table[i];
                            if (rectcode != 0)
                                rectcode &= getoutcode(projected_point_ptr_table[i]);
                            out++;
                        }
                    } else {
                        npts = 0;
                        lastidx = current_prim_start[transformed_vert_count - 1];
                        for (i = 0; i < transformed_vert_count; i++) {
                            idx = *current_prim_verts++;
                            depth += vertbuf[idx].z;
                            if (vertcode[idx] == VERTEX_CLIP_PROJECTED) {
                                if (vertcode[lastidx] != 0) {
                                    zclipline(&vertbuf[idx], &vertbuf[lastidx], &local, SHAPE3D_NEAR_PLANE_Z);
                                    projectp(&local, &ipt);
                                    if (screenpts[idx].x != ipt.x || screenpts[idx].y != ipt.y) {
                                        if (rectcode != 0)
                                            rectcode &= getoutcode(&ipt);
                                        *out = ipt;
                                        out++;
                                        npts++;
                                    }
                                }
                                *out = *projected_point_ptr_table[i];
                                if (rectcode != 0)
                                    rectcode &= getoutcode(projected_point_ptr_table[i]);
                                out++;
                                npts++;
                            } else {
                                if (vertcode[lastidx] == VERTEX_CLIP_PROJECTED) {
                                    zclipline(&vertbuf[lastidx], &vertbuf[idx], &local, SHAPE3D_NEAR_PLANE_Z);
                                    projectp(&local, &ipt);
                                    if (screenpts[lastidx].x != ipt.x || screenpts[lastidx].y != ipt.y) {
                                        if (rectcode != 0)
                                            rectcode &= getoutcode(&ipt);
                                        *out = ipt;
                                        out++;
                                        npts++;
                                    }
                                }
                            }
                            lastidx = idx;
                        }
                        transformed_vert_count = npts;
                    }
                    if (transformed_vert_count != 0 && rectcode == 0) {
                        if ((primflag & PRIMITIVE_FLAG_DOUBLE_SIDED) || (*cull2ptr & cullbits2) || front_facing(shape_polyinfo->points))
                            added++;
                        if (added != 0 && (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT) != 0) {
                    srcpt = shape_polyinfo->points;
                    for (npts = 0; npts < transformed_vert_count; npts++) {
                        ptx = srcpt->x;
                        pty = srcpt->y;
                        srcpt++;
                        if (ptx < current_rect->left)
                            current_rect->left = ptx;
                        if (current_rect->right < ptx + 1)
                            current_rect->right = ptx + 1;
                        if (current_rect->top > pty)
                            current_rect->top = pty;
                        if (current_rect->bottom < pty + 1)
                            current_rect->bottom = pty + 1;
                    }
                        }
                    }
                    break;
                case POLYINFO_TYPE_LINE:
                    i = current_prim_start[0];
                    j = current_prim_start[1];
                    if (vertcode[i] + vertcode[j] == 2)
                        break;
                    if (vertcode[i] != 0) {
                        zclipline(&vertbuf[j], &vertbuf[i], &local, SHAPE3D_NEAR_PLANE_Z);
                        projectp(&local, &screenpts[i]);
                    } else if (vertcode[j] != 0) {
                        zclipline(&vertbuf[i], &vertbuf[j], &local, SHAPE3D_NEAR_PLANE_Z);
                        projectp(&local, &screenpts[j]);
                    }
                    depth = vertbuf[i].z + vertbuf[j].z;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    shape_polyinfo->points[1] = *projected_point_ptr_table[1];
                    if (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT) {
                        zonecheck(projected_point_ptr_table[0], current_rect);
                        zonecheck(projected_point_ptr_table[1], current_rect);
                    }
                    transformed_vert_count = 2;
                    added++;
                    break;
                case POLYINFO_TYPE_WHEEL:
                    if (someclipped != 0)
                        break;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    shape_polyinfo->points[1] = *projected_point_ptr_table[1];
                    shape_polyinfo->points[2] = *projected_point_ptr_table[2];
                    shape_polyinfo->points[3] = *projected_point_ptr_table[3];
                    if (!front_facing(shape_polyinfo->points)) {
                        shape_polyinfo->points[0] = *projected_point_ptr_table[3];
                        shape_polyinfo->points[1] = *projected_point_ptr_table[4];
                        shape_polyinfo->points[2] = *projected_point_ptr_table[5];
                        shape_polyinfo->points[3] = *projected_point_ptr_table[0];
                        depth = (I32)vertbuf[current_prim_start[3]].z << 2;
                    } else {
                        depth = (I32)vertbuf[current_prim_start[0]].z << 2;
                    }
                    i = inthypot(shape_polyinfo->points[0].x - shape_polyinfo->points[1].x,
                                      shape_polyinfo->points[0].y - shape_polyinfo->points[1].y);
                    j = inthypot(shape_polyinfo->points[0].x - shape_polyinfo->points[2].x,
                                      shape_polyinfo->points[0].y - shape_polyinfo->points[2].y);
                    if (j > i)
                        i = j;
                    if (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT) {
                        corner.x = shape_polyinfo->points[0].x - i - 1;
                        corner.y = shape_polyinfo->points[0].y - i - 1;
                        zonecheck(&corner, current_rect);
                        corner.y = shape_polyinfo->points[0].y + i + 1;
                        corner.x = shape_polyinfo->points[0].x + i + 1;
                        zonecheck(&corner, current_rect);
                        corner.x = shape_polyinfo->points[3].x - i - 1;
                        corner.y = shape_polyinfo->points[3].y - i - 1;
                        zonecheck(&corner, current_rect);
                        corner.y = shape_polyinfo->points[3].y + i + 1;
                        corner.x = shape_polyinfo->points[3].x + i + 1;
                        zonecheck(&corner, current_rect);
                    }
                    transformed_vert_count = 4;
                    added = 1;
                    break;
                case POLYINFO_TYPE_SPHERE:
                    i = current_prim_start[0];
                    j = current_prim_start[1];
                    depth = vertbuf[i].z + vertbuf[j].z;
                    if (vertcode[i] + vertcode[j] == VERTEX_CLIP_PROJECTED) {
                        shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                        viewvert = vertbuf[i];
                        surfvec = vertbuf[j];
                        local.x = viewvert.x - surfvec.x;
                        local.y = viewvert.y - surfvec.y;
                        local.z = viewvert.z - surfvec.z;
                        pixsize = thicknessxp(int3dhypot(&local), viewvert.z);
                        shape_polyinfo->points[1].x = pixsize;
                        if (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT) {
                            corner.y = projected_point_ptr_table[0]->y - pixsize;
                            corner.x = projected_point_ptr_table[0]->x - pixsize;
                            zonecheck(&corner, current_rect);
                            corner.x = pixsize + projected_point_ptr_table[0]->x;
                            corner.x = projected_point_ptr_table[0]->y + pixsize;  /* sic: original stores .x twice */
                            zonecheck(&corner, current_rect);
                        }
                        transformed_vert_count = 2;
                        added++;
                    }
                    break;
                case POLYINFO_TYPE_PIXEL:
                    i = current_prim_start[0];
                    if (vertcode[i] != 0)
                        break;
                    depth = vertbuf[i].z;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    if (obj_flags & SHAPE3D_FLAG_ACCUMULATE_RECT)
                        zonecheck(projected_point_ptr_table[0], current_rect);
                    transformed_vert_count = 1;
                    added++;
                    break;
                }
            }
        }
        current_prim_start = shape_prim_next;
        cull2ptr++;
        cullwords++;
        if (added == 0) {
            if ((primflag & PRIMITIVE_FLAG_CONTINUATION) == 0) {
                while (current_prim_start[1] & PRIMITIVE_FLAG_CONTINUATION) {
                    current_prim_start += NP[*current_prim_start] + current_num_paints + PRIMITIVE_PREFIX_BYTES;
                    cullwords++;
                    cull2ptr++;
                }
            }
        } else {
            numdrawn++;
            shape_polyinfo->numpoints = transformed_vert_count;
            shape_polyinfo->type = ptype;
            if (current_paintjob == MATERIAL_BACKLIGHT_DYNAMIC)
                shape_polyinfo->material = backlightovr8;
            else
                shape_polyinfo->material = current_paintjob;
            switch (transformed_vert_count) {
            case 1:
                i = depth;
                break;
            case 2:
                i = depth >> 1;
                break;
            case 4:
                i = depth >> 2;
                break;
            case 8:
                i = depth >> 3;
                break;
            default:
                i = depth / transformed_vert_count;
                break;
            }
            shape_polyinfo->depth = i;
            if ((poly_list_insert_result = polysort(i, (obj_flags & SHAPE3D_FLAG_KEEP_INSERTION_POSITION) || (primflag & PRIMITIVE_FLAG_CONTINUATION) ? 0 : 1)) != 0)
                return TRANSFORM_RESULT_POOL_FULL;
        }
    } while (*current_prim_start != SHAPE3D_PRIMITIVE_STREAM_END);
    if (numdrawn == 0)
        return TRANSFORM_RESULT_NOT_VISIBLE;
    return TRANSFORM_RESULT_SUBMITTED;
}
/* Inserts the newest polygon into the linked list. */
extern unsigned polysort(unsigned depth, unsigned search_sorted_position)
{
    register I16 remaining;
    register I16 list_index;
    if (search_sorted_position == 0) {
        list_index = poly_link_list[poly_link_listit4];
    } else {
        poly_link_listit4 = poly_cursor1;
        list_index = poly_link_list[poly_cursor1];
        remaining = polygon_link_3_list_iter;
        goto scan_test;
    scan_body:
        if (remaining-- == 0) goto scan_done;
        if (poly_info_ptrs[list_index]->depth < (I16)depth) goto scan_done;
        poly_link_listit4 = list_index;
        list_index = poly_link_list[list_index];
    scan_test:
        if (list_index >= 0) goto scan_body;
    scan_done:
        ;
    }
    poly_link_list[polygonnumber] = list_index;
    poly_link_list[poly_link_listit4] = polygonnumber;
    polygon_link_3_list_iter++;
    if (list_index < 0) facenodeiterator = polygonnumber;
    poly_link_listit4 = poly_link_list[poly_link_listit4];
    polygonnumber++;
    polyinfo_offset += transformed_vert_count * sizeof(struct POINT2D) + POLYINFO_RECORD_PREFIX_BYTES;
    return polygonnumber == POLYINFO_CAPACITY || (I16)polyinfo_offset > POLYINFO_LAST_START_OFFSET;
}
/* Returns the clip-rectangle outcode for one projected point. */
unsigned getoutcode(struct POINT2D *point)
{
    register struct POINT2D *p = point;
    I8 flag;
    if (p->y < selection_rect.top) flag=CLIP_OUTCODE_TOP;
    else if (p->y > selection_rect.bottom) flag=CLIP_OUTCODE_BOTTOM;
    else flag=0;
    if (p->x < selection_rect.left) flag|=CLIP_OUTCODE_LEFT;
    else if (p->x > selection_rect.right) flag|=CLIP_OUTCODE_RIGHT;
    return flag;
}
/* Tests the winding of a projected polygon against the camera. */
I8 front_facing(struct POINT2D far *pts)
{
    I32 dx0, dx1, dy0, dy1;

    dx0 = (I32)pts[0].x - pts[1].x;
    dx1 = (I32)pts[2].x - pts[1].x;
    if (dx0 != 0 || dx1 != 0) {
        dy0 = pts[0].y - pts[1].y;
        dy1 = pts[2].y - pts[1].y;
        if (dy0 != 0 || dy1 != 0)
            return (I8)(dx1 * dy0 - dx0 * dy1 > 0);
    }
    return 0;
}
/* Draws queued polygon records through the matching primitive renderer. */
void DRAW_OBJECTS(void)
{
    struct POINT2D far* srcpts;
    struct POLYINFO far* info;
    register I16 polyIndex;
    register I16 link;
    struct POINT2D polypts[10];
    struct POINT2D* out;
    I16 fill;
    I16 materialIndex;
    unsigned idx;
    unsigned pointCount;

    link = POLYINFO_CAPACITY;
    for (polyIndex = 0; polyIndex < polygonnumber; polyIndex++) {
        link = poly_link_list[link];
        info = poly_info_ptrs[link];
        materialIndex = info->material;
        fill = mat_copy_clr_lst_ptr[materialIndex];
        switch (info->type) {
        case POLYINFO_TYPE_POLYGON:
            pointCount = info->numpoints;
            srcpts = info->points;
            out = polypts;
            for (idx = 0; idx < pointCount; idx++) {
                *out = *srcpts;
                out++;
                srcpts++;
            }
            switch (material_patlistptr_copy[materialIndex]) {
            case POLYGON_FILL_SOLID:
                drawpolypt(fill, pointCount, polypts);
                break;
            case POLYGON_FILL_PATTERNED:
                if (matpatlistcopypointer2[materialIndex] != 0)
                    preRender_patterned(matpatlistcopypointer2[materialIndex], fill, pointCount, polypts);
                break;
            case POLYGON_FILL_SECONDARY_PARAMETERS:
                sprite_two_color_poly_list(matpatlistcopypointer2[materialIndex], g_mat_clrlist_copy_2_ptr[materialIndex], fill, pointCount, polypts);
                break;
            }
            break;
        case POLYINFO_TYPE_LINE:
            preRender_line(info->points[0].x, info->points[0].y, info->points[1].x, info->points[1].y, fill);
            break;
        case POLYINFO_TYPE_WHEEL:
            for (idx = 0; idx < 4; idx++)
                polypts[idx] = info->points[idx];
            solidtire(polypts, WHEEL_INNER_RADIUS_Q14, fill, mat_copy_clr_lst_ptr[materialIndex + 1], mat_copy_clr_lst_ptr[materialIndex + 2]);
            break;
        case POLYINFO_TYPE_SPHERE:
            drawcircle(info->points[0].x, info->points[0].y, info->points[1].x, fill);
            break;
        case POLYINFO_TYPE_PIXEL:
            writepixelc(info->points[0].x, info->points[0].y, fill);
            break;
        }
    }
    INIT_BUFFERS();
}
/* Composes cached Z, X, and Y rotation matrices in the requested order. */
struct MATRIX* makematrix(I16 z, I16 x, I16 y, I16 unk)
{
    register I16 rotation_flags = 0;
    register struct MATRIX* result;

    if ((z & ANGLE_TURN_MASK) != 0) {
        rotation_flags |= ROTATION_PRESENT_Z;
        xformz(&g_rot_mat_z, z);
    }
    if ((x & ANGLE_TURN_MASK) != 0) {
        rotation_flags |= ROTATION_PRESENT_X;
        xformx(&matrix_x_rotation, x);
    }
    if ((y & ANGLE_TURN_MASK) != 0) {
        rotation_flags |= ROTATION_PRESENT_Y;
        if ((y & ANGLE_TURN_MASK) == mat_y_rot_angle) {
            result = &g_matrix_yrot;
        } else {
            switch (y & ANGLE_TURN_MASK) {
            case ANGLE_QUARTER_TURN:
                result = &mat_quarter_turn;
                break;
            case ANGLE_HALF_TURN:
                result = &mat_half_turn;
                break;
            case ANGLE_THREE_QUARTERS_TURN:
                result = &mat_three_quarters_turn;
                break;
            default:
                xformy(&g_matrix_yrot, y);
                mat_y_rot_angle = y & ANGLE_TURN_MASK;
                result = &g_matrix_yrot;
                break;
            }
        }
    }

    switch (rotation_flags) {
    case 0:
        result = &mat_no_turn;
        break;
    case 1:
        break;
    case 2:
        result = &matrix_x_rotation;
        break;
    case 3:
        if ((unk & ROTATION_COMPOSE_REVERSED) != 0)
            transmult(result, &matrix_x_rotation, &matrotation_tmp);
        else
            transmult(&matrix_x_rotation, result, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 4:
        result = &g_rot_mat_z;
        break;
    case 5:
        if ((unk & ROTATION_COMPOSE_REVERSED) != 0)
            transmult(result, &g_rot_mat_z, &matrotation_tmp);
        else
            transmult(&g_rot_mat_z, result, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 6:
        if ((unk & ROTATION_COMPOSE_REVERSED) != 0)
            transmult(&matrix_x_rotation, &g_rot_mat_z, &matrotation_tmp);
        else
            transmult(&g_rot_mat_z, &matrix_x_rotation, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 7:
        if ((unk & ROTATION_COMPOSE_REVERSED) != 0) {
            transmult(result, &matrix_x_rotation, &matrotation_tmp);
            transmult(&matrotation_tmp, &g_rot_mat_z, &matrix_x_rotation);
            result = &matrix_x_rotation;
        } else {
            transmult(&g_rot_mat_z, &matrix_x_rotation, &matrotation_tmp);
            transmult(&matrotation_tmp, result, &g_rot_mat_z);
            result = &g_rot_mat_z;
        }
        break;
    }
    return result;
}
/* Expands a rectangle to include one projected point. */
void zonecheck(struct POINT2D *pt, struct RECTANGLE *rc)
{
    register I16 x, y;
    x=pt->x; y=pt->y;
    if(rc->left>x) rc->left=x;
    { I16 temp=x+1; if(rc->right<temp) rc->right=temp; }
    if(rc->top>y) rc->top=y;
    { I16 temp=y+1; if(rc->bottom<temp) rc->bottom=temp; }
}
/* Classifies a view vector into the renderer angle and octant table. */
norm_zone(struct VECTOR* vec)
{
	I32 height;
	I32 temp;
	I8 octant;
	I16 below;
	I16 angle;

	height = abs(vec->y);

	temp = inthypot(abs(vec->x), abs(vec->z));

	if (vector_angle_cos == vector_angle_sin) {
		below = temp < height;
	} else {
		below = temp * vector_angle_cos < height * vector_angle_sin;
	}

	if (vec->y < 0) {
		if (below != 0) return CULL_LOWER_POLE_INDEX;
	} else
	if (vec->y > 0) {
		if (below != 0) return CULL_UPPER_POLE_INDEX;
	}

	if (vec->y > 0) {
		octant = CULL_UPPER_HEMISPHERE_OFFSET;
	} else {
		octant = 0;
	}

	angle = -intatan(vec->z, -vec->x);
	if (angle < 0) {
		angle += ANGLE_UNITS_PER_TURN;
	}

	octant += ((angle * CULL_AZIMUTH_BIN_COUNT) >> CULL_ANGLE_BIN_SHIFT);

	return octant;
}
/* Caches the sine and cosine values used by view-vector classification. */
void initnm(void)
{
    vector_angle_sin = intsin(ANGLE_EIGHTH_TURN);
    vector_angle_cos = intcos(ANGLE_EIGHTH_TURN);
    vector_angle_sin2 = intsin(ANGLE_EIGHTH_TURN);
    vector_angle_cos2 = intcos(ANGLE_EIGHTH_TURN);
}
/* Returns the magnitude of a signed long value. */
I32 lgabs(I32 value)
{
if (value < 0) return -value; return value; }
I16S g_vid_flg2_set;
I16S vidflg3is_minus1;
/* Writes the smallest rectangle that covers both input rectangles. */
void mergezones(struct RECTANGLE *r1, struct RECTANGLE *r2, struct RECTANGLE *out)
{
    out->left = r1->left <= r2->left ? r1->left : r2->left;
    out->right = r1->right >= r2->right ? r1->right : r2->right;
    out->top = r1->top <= r2->top ? r1->top : r2->top;
    out->bottom = r1->bottom >= r2->bottom ? r1->bottom : r2->bottom;
    if (g_vid_flg2_set != 1) {
        out->right = (out->right + g_vid_flg2_set - 1) & vidflg3is_minus1;
    }
}
/* Clips the first rectangle to the second and reports an empty intersection. */
I8 clipzone(struct RECTANGLE *r1, struct RECTANGLE *r2)
{
    if (r1->right < r1->left) return 1;
    if (r2->right <= r1->left) return 1;
    if (r1->right <= r2->left) return 1;
    if (r1->top >= r2->bottom) return 1;
    if (r1->bottom <= r2->top) return 1;

    if (r1->left < r2->left) r1->left = r2->left;
    if (r1->right > r2->right) r1->right = r2->right;
    if (r1->top < r2->top) r1->top = r2->top;
    if (r1->bottom > r2->bottom) r1->bottom = r2->bottom;
    return 0;
}
/* Merges or splits a rectangle as needed before adding it to a clip list. */
void addzone(I8* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr, struct RECTANGLE* rect)
{
	struct RECTANGLE bottom_rect;
	struct RECTANGLE merged;
	I8 shift_index;
	I8 rect_count;
	I8 top_needed;
	struct RECTANGLE* entry_ptr;
	struct RECTANGLE top_piece;
	I8 bottom_needed;

	if (g_vid_flg2_set != 1) {
		rect->right = (rect->right + g_vid_flg2_set - 1) & vidflg3is_minus1;
		/*
		mov     bx, [bp+arg_rectptr]
		mov     si, bx
		mov     ax, [si+RECTANGLE.rc_right]
		add     ax, video_flag2_is1
		dec     ax
		and     ax, video_flag3_isFFFF
		mov     [bx+RECTANGLE.rc_right], ax*/
	}

	for (rect_count = 0; rect_count < *arg_rect_array_length_ptr; rect_count++) {
		entry_ptr = &arg_rect_array_ptr[rect_count];
		if (zoneoverlap(rect, entry_ptr) == 0)
			continue;
		if (zoneinside(rect, entry_ptr) != 0)
			return ;

		if (zoneinside(entry_ptr, rect) != 0) {
			shift_index = rect_count;

			while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
				arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
				shift_index++;
			}
			(*arg_rect_array_length_ptr)--;
			continue;
		}

		merged = *entry_ptr;
		if (entry_ptr->top < rect->top) {
			top_piece = *entry_ptr;
			top_piece.bottom = rect->top;
			merged.top = rect->top;
			top_needed = 1;
		} else {
			if (rect->top < entry_ptr->top) {
				top_piece = *rect;
				top_piece.bottom = entry_ptr->top;
				top_needed = 1;
			} else {
				top_needed = 0;
			}
		}

		if (entry_ptr->bottom > rect->bottom) {
			bottom_rect = *entry_ptr;
			bottom_rect.top = rect->bottom;
			merged.bottom = rect->bottom;
			bottom_needed = 1;
		} else {
			if (rect->bottom > entry_ptr->bottom) {
				bottom_rect = *rect;
				bottom_rect.top = entry_ptr->bottom;
				bottom_needed = 1;
			} else {
				bottom_needed = 0;
			}
		}

		merged.left = rect->left > entry_ptr->left ? entry_ptr->left : rect->left;

		merged.right = rect->right < entry_ptr->right ? entry_ptr->right : rect->right;

		shift_index = rect_count;

		while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
			arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
			shift_index ++;
		}
		(*arg_rect_array_length_ptr)--;
		if (top_needed != 0) {
			addzone(arg_rect_array_length_ptr, arg_rect_array_ptr, &top_piece);
		}

		addzone(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		if (bottom_needed != 0) {
			addzone(arg_rect_array_length_ptr, arg_rect_array_ptr, &bottom_rect);
			return ;
		}
		return ;
	}

	for (rect_count = 0; rect_count < *arg_rect_array_length_ptr; rect_count++) {
		entry_ptr = &arg_rect_array_ptr[rect_count];

		if (zoneadjacent(entry_ptr, rect) != 0) {
		merged.left = entry_ptr->left <= rect->left ? entry_ptr->left : rect->left;
		merged.right = entry_ptr->right >= rect->right ? entry_ptr->right : rect->right;
		merged.top = entry_ptr->top <= rect->top ? entry_ptr->top : rect->top;
		merged.bottom = entry_ptr->bottom >= rect->bottom ? entry_ptr->bottom : rect->bottom;

		shift_index = rect_count;

		while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
			arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
			shift_index ++;
		}
		(*arg_rect_array_length_ptr)--;
		addzone(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		return ;
		}
	}

	arg_rect_array_ptr[*arg_rect_array_length_ptr] = *rect;
	(*arg_rect_array_length_ptr)++;
}
/* Tests whether two half-open rectangles overlap. */
I8 zoneoverlap(struct RECTANGLE* r1, struct RECTANGLE* r2)
{
	if (r1->right <= r2->left) {
		return 0;
	}

	if (r2->right <= r1->left) {
		return 0;
	}

	if (r1->top >= r2->bottom) {
		return 0;
	}

	if (r1->bottom <= r2->top) {
		return 0;
	}

	return 1;
}
/* Tests whether the first rectangle is contained in the second. */
I8 zoneinside(struct RECTANGLE *a,struct RECTANGLE *b)
{
return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}
/* Tests whether two rectangles share a complete edge. */
I8 zoneadjacent(struct RECTANGLE* r1, struct RECTANGLE* r2)
{
 if (r1->bottom == r2->top) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->top == r2->bottom) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->right == r2->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 else if (r2->right == r1->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 return 0;
}
/* Selects, clips, and adds the requested source rectangles to the result list. */
void makezonelist(U8  arg_rectcount, I8* arg_rectarray_indices,
	struct RECTANGLE* arg_rectarray1, struct RECTANGLE* arg_rectarray2,
	struct RECTANGLE* arg_rectptr, I8* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr)
{
	I8 has_result;
	struct RECTANGLE* selected_rect_ptr;
	struct RECTANGLE cover_rect;
	struct RECTANGLE* first_ptr;
	I8 rect_counter;
	I8 source_flags;
	struct RECTANGLE input_rect;
	struct RECTANGLE* second_rect_ptr;
/*
	return ported_rect_clip_combined_(
		arg_rectcount, arg_rectarray_indices, arg_rectarray1, arg_rectarray2, arg_rectptr,
		arg_rect_array_length_ptr, arg_rect_array_ptr);
	*/
	for (rect_counter = 0; rect_counter < arg_rectcount; rect_counter++) {

		source_flags = arg_rectarray_indices[rect_counter];
		if ((source_flags & RECTLIST_SOURCE_FIRST) != 0) {
			first_ptr = &arg_rectarray1[rect_counter];
		}

		if ((source_flags & RECTLIST_SOURCE_SECOND) != 0) {
			second_rect_ptr = &arg_rectarray2[rect_counter];
		}

		if ((source_flags & RECTLIST_SOURCE_FIRST) != 0 && first_ptr->right > first_ptr->left) {
			if ((source_flags & RECTLIST_SOURCE_SECOND) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
				mergezones(first_ptr, second_rect_ptr, &cover_rect);
				selected_rect_ptr = &cover_rect;
			} else {
				selected_rect_ptr = first_ptr;
			}
			has_result = 1;
		} else if ((source_flags & RECTLIST_SOURCE_SECOND) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
			selected_rect_ptr = second_rect_ptr;
			has_result = 1;
		} else {
			has_result = 0;
		}
		if (has_result != 0) {
			input_rect = *selected_rect_ptr;
			if (clipzone(&input_rect, arg_rectptr) == 0) {
				addzone(arg_rect_array_length_ptr, arg_rect_array_ptr, &input_rect);
			}
		}
	}

}
/* Sorts rectangle indices by their top coordinate. */
void sortzonelist(I8 arg_array_length, struct RECTANGLE* arg_rect_array, I16* arg_array_indices)
{
    register I16 sortIndex;
    I16 intbuffer[256];
    if (arg_array_length > 1) {
        for (sortIndex = 0; sortIndex < arg_array_length; sortIndex++) {
            intbuffer[sortIndex] = -arg_rect_array[sortIndex].top;
            arg_array_indices[sortIndex] = sortIndex;
        }
        shellsort(arg_array_length, intbuffer, arg_array_indices);
    } else {
        arg_array_indices[0] = 0;
    }
}

U8  NP[SHAPE3D_PRIMITIVE_TYPE_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2, 6, 3, 0, 0
};
U8  PT[SHAPE3D_PRIMITIVE_TYPE_COUNT] = {
    0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 0
};
I32 inverse_power_of_two_table[32] = {
    (-2147483647L - 1L), 1073741824L, 536870912L, 268435456L,
    134217728L, 67108864L, 33554432L, 16777216L,
    8388608L, 4194304L, 2097152L, 1048576L,
    524288L, 262144L, 131072L, 65536L,
    32768L, 16384L, 8192L, 4096L,
    2048L, 1024L, 512L, 256L,
    128L, 64L, 32L, 16L,
    8L, 4L, 2L, 1L
};
unsigned mat_y_rot_angle = MATRIX_Y_ROTATION_CACHE_INVALID;
struct RECTANGLE empty_rectangle = { RECT_EMPTY_MIN_SENTINEL, RECT_EMPTY_MAX_SENTINEL, RECT_EMPTY_MIN_SENTINEL, RECT_EMPTY_MAX_SENTINEL };
