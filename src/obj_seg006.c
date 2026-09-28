/* obj_seg006: whole seg006 object (22 functions, file order = image order).
   Globals without a reviewed semantic alias use their bound data names:
   data_31852 = transshapenumpaints, transformed_shape_material = transshapematerial,
   transformed_shape_bounds = transshaperectptr, transformed_primitive_cursor = transshapeprimptr,
   transformed_primitive_paint = transprimitivepaintjob, backlightovr8 = backlights_paint_override,
   projected_point_table = polyvertpointptrtab, primitive_type_table = primtypetab, inverse_power_of_two_table = invpow2tbl. */
struct RECTANGLE { int left, right, top, bottom; };
struct VECTOR { int x, y, z; };
struct POINT2D { int x, y; };
struct MATRIX { int vals[9]; };
struct SHAPE3D {
    unsigned numverts;
    struct VECTOR far* verts;
    unsigned numprimitives;
    char numpaints;
    char reserved;
    unsigned char far* primitives;
    long far* cull1;
    long far* cull2;
};
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D* shapeptr;
    struct RECTANGLE* rectptr;
    struct VECTOR rotvec;
    int unk;
    unsigned char ts_flags;
    unsigned char material;
};
struct POLYINFO {
    int depth;
    unsigned char material;
    char numpoints;
    char type;
    char reserved;
    struct POINT2D points[10];
};

extern int far abs(int);
extern int far polang(int, int);
extern unsigned far polradius2d(int, int);
extern int far polarRadius3D(struct VECTOR*);
extern int far projectiondata9_times_ratio(int, int);
extern void far matroty(struct MATRIX*, int);
extern void far mat_rot_x(struct MATRIX*, int);
extern void far mat_rot_z(struct MATRIX*, int);
extern void far mat_vec(struct VECTOR*, struct MATRIX*, struct VECTOR*);
extern void far mat_multiply(struct MATRIX*, struct MATRIX*, struct MATRIX*);
extern void far mat_invert(struct MATRIX*, struct MATRIX*);
extern void far vector_to_point(struct VECTOR*, struct POINT2D*);
extern void far vector_op_unk(struct VECTOR*, struct VECTOR*, struct VECTOR*, int);
extern void far* far mmgr_alloc_resbytes(char*, long);
extern int far sinfast(unsigned);
extern int far cosfast(unsigned);
extern void far preRender_default(int, int, struct POINT2D*);
extern void far preRender_patterned(int, int, int, struct POINT2D*);
extern void far preRender_unk(int, int, int, int, struct POINT2D*);
extern void far preRender_line(int, int, int, int, int);
extern void far preRender_sphere(int, int, int, int);
extern void far preRender_wheel(struct POINT2D*, int, int, int, int);
extern void far putpixel_single_maybe(int, int, int);

static struct MATRIX mat_no_turn, mat_quarter_turn, mat_half_turn, mat_three_quarters_turn;
struct MATRIX g_matrix_yrot;
struct MATRIX matrix_x_rotation;
struct MATRIX g_rot_mat_z;
struct MATRIX matrotation_tmp;
extern unsigned mat_y_rot_angle;
extern struct MATRIX wkmatx;
static struct RECTANGLE selection_rect;
static int half_scale_flag;
int * mat_copy_clr_lst_ptr;
int * g_mat_clrlist_copy_2_ptr;
int * material_patlistptr_copy;
short material_pad_7;
int * matpatlistcopypointer2;
unsigned short video_cnstval;
static long vector_angle_cos, vector_angle_sin, vector_angle_sin2, vector_angle_cos2;
static char shape3d_unused[576];

unsigned polygonnumber;
static unsigned polyinfo_offset;
static int poly_list_insert_result;
static int polyinfo_reset_marker;
int poly_cursor1;
int facenodeiterator;
int polygon_link_3_list_iter;
int poly_link_listit4;
static int poly_link_list[400];
static char far* polyinfoptr;
static struct POLYINFO far* poly_info_ptrs[400];
static char poly_padding_bytes[6];
static struct POLYINFO far* shape_polyinfo;
static unsigned vertex_count;
static struct VECTOR far* obj_verts;
static unsigned char far* current_prim_start;
static unsigned char far* current_prim_verts;
static unsigned char far* shape_prim_next;
static int current_num_paints;
static unsigned char transformed_shape_material;
static unsigned char obj_flags;
unsigned char transformed_vert_count;
static struct RECTANGLE* current_rect;
static char current_paintjob;
extern char backlightovr8;
char g_vector_bitix;
extern long inverse_power_of_two_table[];
extern unsigned char primidxcounttab[];
extern unsigned char primitive_type_table[];
static struct POINT2D* projected_point_ptr_table[11];

void polyinfo_reset(void);
void calc_sincos80(void);
struct MATRIX* matrotzxy(int, int, int, int);
unsigned rect_compare_point(struct POINT2D*);
char is_facing_camera(struct POINT2D far*);
void rect_adjust_from_point(struct POINT2D*, struct RECTANGLE*);
vector_op_unk2(struct VECTOR*);
unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned, unsigned);

void initialize_polyinfo(void)
{ /* PURPOSE: Allocate the polygon information pool and initialize cached rotations. Params: none. Returns: void. Globals: reads mat_half_turn, mat_no_turn, mat_quarter_turn, mat_three_quarters_turn; writes mat_half_turn, mat_no_turn, mat_quarter_turn, mat_three_quarters_turn, polyinfoptr. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
    polyinfoptr = mmgr_alloc_resbytes("polyinfo", 0x28A0) /* PLATFORM(memory): allocate the polygon information buffer from game memory. */;
    matroty(&mat_no_turn, 0);
    matroty(&mat_quarter_turn, 0x100);
    matroty(&mat_half_turn, 0x200);
    matroty(&mat_three_quarters_turn, 0x300);
    calc_sincos80();
}

void copy_material_list_pointers(void* clrlist, void* clrlist2, void* patlist, void* patlist2, unsigned short videoConst)
{ /* PURPOSE: Save the material color and pattern tables used by polygon drawing. Params: clrlist, clrlist2, patlist, patlist2, videoConst. Returns: void. Globals: reads none; writes g_mat_clrlist_copy_2_ptr, mat_copy_clr_lst_ptr, material_patlistptr_copy, matpatlistcopypointer2, video_cnstval. */
    mat_copy_clr_lst_ptr = clrlist;
    g_mat_clrlist_copy_2_ptr = clrlist2;
    material_patlistptr_copy = patlist;
    matpatlistcopypointer2 = patlist2;
    video_cnstval = videoConst;
}

void polyinfo_reset(void)
{ /* PURPOSE: Reset polygon numbering, insertion state, and face-list cursors. Params: none. Returns: void. Globals: reads none; writes facenodeiterator, poly_list_insert_result, polygonnumber, polyinfo_offset, polyinfo_reset_marker. */
    polygonnumber = 0;
    polyinfo_offset = 0;
    poly_list_insert_result = 0;
    polyinfo_reset_marker = 0xffff;
    facenodeiterator = 0x190;
}

unsigned select_rot(int angZ, int angX, int angY, struct RECTANGLE* cliprect, int unk)
{ /* PURPOSE: Build the requested view rotations and return the camera-facing angle. Params: angZ, angX, angY, cliprect, unk. Returns: unsigned. Globals: reads none; writes half_scale_flag, selection_rect, wkmatx. */
    struct MATRIX* matptr;
    struct VECTOR vec, vec2;

    wkmatx = *matrotzxy(angZ, angX, angY, 1);
    polyinfo_reset();
    selection_rect = *cliprect;
    half_scale_flag = unk;
    matptr = matrotzxy(-angZ, -angX, -angY, 0);
    vec.z = 0x2710;
    vec.y = 0;
    vec.x = 0;
    mat_vec(&vec, matptr, &vec2);
    return polang(vec2.x, vec2.z) & 0x3FF;
}

unsigned trans_op(struct TRANSFORMEDSHAPE3D* ts)
{ /* PURPOSE: Transform a 3D shape, clip its primitives, and append visible polygons. Params: ts. Returns: unsigned. Globals: reads the shape, transform, material and clip state; writes projected-vertex, polygon-list and clipping scratch state. */
    unsigned char ptype;
    int added;
    int primflag;
    long cullbits2;
    int someclipped;
    long depth;
    struct VECTOR position;
    unsigned npts;
    unsigned idx;
    struct POINT2D screenpts[255];
    struct MATRIX invview;
    long far* cull2ptr;
    struct POINT2D far* out;
    struct MATRIX* matptr;
    struct MATRIX mview;
    unsigned prim;
    struct POINT2D corner;
    register int i;
    char rectcode;
    unsigned lastidx;
    register int j;
    struct VECTOR local;
    struct POINT2D far* srcpt;
    struct VECTOR viewvert;
    int ptx;
    char vertcode[256];
    int pixsize;
    int hidden;
    int numdrawn;
    long cullmask;
    struct POINT2D ipt;
    int pty;
    struct VECTOR surfvec;
    struct VECTOR vertbuf[255];
    long far* cullwords;

    if (poly_list_insert_result != 0)
        return 1;
    vertex_count = ts->shapeptr->numverts;
    current_prim_start = ts->shapeptr->primitives;
    obj_verts = ts->shapeptr->verts;
    current_num_paints = ts->shapeptr->numpaints;
    cullwords = ts->shapeptr->cull1;
    cull2ptr = ts->shapeptr->cull2;
    transformed_shape_material = ts->material;
    if (transformed_shape_material >= (unsigned char)current_num_paints)
        transformed_shape_material = 0;
    obj_flags = ts->ts_flags;
    if (obj_flags & 8)
        current_rect = ts->rectptr;
    for (i = 0; i < vertex_count; i++)
        vertcode[i] = -1;

    if (obj_flags & 2) {
        matptr = matrotzxy(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        mat_multiply(matptr, &wkmatx, &mview);
        position = ts->pos;
        cullmask = -1L;
        cullbits2 = 0;
    } else {
        matptr = matrotzxy(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        mat_vec(&ts->pos, &wkmatx, &position);
        mat_multiply(matptr, &wkmatx, &mview);
        mat_invert(&mview, &invview);
        local.x = 0;
        local.y = 0;
        local.z = 0x1000;
        mat_vec(&local, &invview, &viewvert);
        if ((viewvert.y > 0 && ts->pos.y < 0) ||
            (ts->unk * 2 > abs(position.x) && ts->unk * 2 > abs(position.z))) {
            cullmask = -1L;
            cullbits2 = 0;
        } else {
            g_vector_bitix = vector_op_unk2(&viewvert);
            cullbits2 = cullmask = inverse_power_of_two_table[g_vector_bitix];
        }
    }

    poly_link_listit4 = poly_cursor1 = facenodeiterator;
    polygon_link_3_list_iter = 0;
    numdrawn = 0;
    if (vertex_count > 8)
        transformed_vert_count = 8;
    else
        transformed_vert_count = vertex_count;
    if (transformed_vert_count > 4 && obj_verts[0].y == obj_verts[4].y)
        transformed_vert_count = 4;

    rectcode = 0x0F;
    hidden = 1;
    someclipped = 0;
    for (i = 0; i < transformed_vert_count; i++) {
        projected_point_ptr_table[i] = &screenpts[i];
        local = obj_verts[i];
        if (half_scale_flag != 0) {
            local.x >>= 1;
            local.y >>= 1;
            local.z >>= 1;
        }
        mat_vec(&local, &mview, &viewvert);
        viewvert.x += position.x;
        viewvert.y += position.y;
        viewvert.z += position.z;
        vertbuf[i] = viewvert;
        if (viewvert.z < 12) {
            vertcode[i] = 1;
            someclipped = 1;
        } else {
            hidden = 0;
            vertcode[i] = 0;
            vector_to_point(&viewvert, projected_point_ptr_table[i]);
            if (rectcode != 0)
                rectcode &= rect_compare_point(projected_point_ptr_table[i]);
            if (rectcode == 0)
                goto visible;
        }
    }
    if (hidden != 0 || someclipped == 0 || ts->unk < abs(position.x))
        return -1;

visible:
    current_prim_start = ts->shapeptr->primitives;
    do {
        shape_prim_next = current_prim_start + primidxcounttab[*current_prim_start] + current_num_paints + 2;
        primflag = current_prim_start[1];
        added = 0;
        if (*cullwords & cullmask) {
            prim = current_prim_start[0];
            transformed_vert_count = primidxcounttab[prim];
            ptype = primitive_type_table[prim];
            shape_polyinfo = (struct POLYINFO far*)(polyinfoptr + polyinfo_offset);
            poly_info_ptrs[polygonnumber] = shape_polyinfo;
            current_paintjob = current_prim_start[transformed_shape_material + 2];
            current_prim_start += current_num_paints + 2;
            rectcode = 0x0F;
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
                        rectcode &= rect_compare_point(projected_point_ptr_table[npts]);
                    break;
                case -1:
                    local = obj_verts[i];
                    if (half_scale_flag != 0) {
                        local.x >>= 1;
                        local.y >>= 1;
                        local.z >>= 1;
                    }
                    mat_vec(&local, &mview, &viewvert);
                    viewvert.x += position.x;
                    viewvert.y += position.y;
                    viewvert.z += position.z;
                    vertbuf[i] = viewvert;
                    if (viewvert.z < 12) {
                        vertcode[i] = 1;
                        someclipped = 1;
                    } else {
                        hidden = 0;
                        vertcode[i] = 0;
                        vector_to_point(&viewvert, projected_point_ptr_table[npts]);
                        if (rectcode != 0)
                            rectcode &= rect_compare_point(projected_point_ptr_table[npts]);
                    }
                    break;
                case 1:
                    someclipped = 1;
                    break;
                }
            }
            if (hidden == 0 && (rectcode == 0 || someclipped != 0)) {
                switch (ptype) {
                case 0:
                    out = shape_polyinfo->points;
                    current_prim_verts = current_prim_start;
                    depth = 0;
                    rectcode = 0x0F;
                    if (someclipped == 0) {
                        for (i = 0; i < transformed_vert_count; i++) {
                            idx = *current_prim_verts++;
                            depth += vertbuf[idx].z;
                            *out = *projected_point_ptr_table[i];
                            if (rectcode != 0)
                                rectcode &= rect_compare_point(projected_point_ptr_table[i]);
                            out++;
                        }
                    } else {
                        npts = 0;
                        lastidx = current_prim_start[transformed_vert_count - 1];
                        for (i = 0; i < transformed_vert_count; i++) {
                            idx = *current_prim_verts++;
                            depth += vertbuf[idx].z;
                            if (vertcode[idx] == 0) {
                                if (vertcode[lastidx] != 0) {
                                    vector_op_unk(&vertbuf[idx], &vertbuf[lastidx], &local, 12);
                                    vector_to_point(&local, &ipt);
                                    if (screenpts[idx].x != ipt.x || screenpts[idx].y != ipt.y) {
                                        if (rectcode != 0)
                                            rectcode &= rect_compare_point(&ipt);
                                        *out = ipt;
                                        out++;
                                        npts++;
                                    }
                                }
                                *out = *projected_point_ptr_table[i];
                                if (rectcode != 0)
                                    rectcode &= rect_compare_point(projected_point_ptr_table[i]);
                                out++;
                                npts++;
                            } else {
                                if (vertcode[lastidx] == 0) {
                                    vector_op_unk(&vertbuf[lastidx], &vertbuf[idx], &local, 12);
                                    vector_to_point(&local, &ipt);
                                    if (screenpts[lastidx].x != ipt.x || screenpts[lastidx].y != ipt.y) {
                                        if (rectcode != 0)
                                            rectcode &= rect_compare_point(&ipt);
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
                        if ((primflag & 1) || (*cull2ptr & cullbits2) || is_facing_camera(shape_polyinfo->points))
                            added++;
                        if (added != 0 && (obj_flags & 8) != 0) {
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
                case 1:
                    i = current_prim_start[0];
                    j = current_prim_start[1];
                    if (vertcode[i] + vertcode[j] == 2)
                        break;
                    if (vertcode[i] != 0) {
                        vector_op_unk(&vertbuf[j], &vertbuf[i], &local, 12);
                        vector_to_point(&local, &screenpts[i]);
                    } else if (vertcode[j] != 0) {
                        vector_op_unk(&vertbuf[i], &vertbuf[j], &local, 12);
                        vector_to_point(&local, &screenpts[j]);
                    }
                    depth = vertbuf[i].z + vertbuf[j].z;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    shape_polyinfo->points[1] = *projected_point_ptr_table[1];
                    if (obj_flags & 8) {
                        rect_adjust_from_point(projected_point_ptr_table[0], current_rect);
                        rect_adjust_from_point(projected_point_ptr_table[1], current_rect);
                    }
                    transformed_vert_count = 2;
                    added++;
                    break;
                case 3:
                    if (someclipped != 0)
                        break;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    shape_polyinfo->points[1] = *projected_point_ptr_table[1];
                    shape_polyinfo->points[2] = *projected_point_ptr_table[2];
                    shape_polyinfo->points[3] = *projected_point_ptr_table[3];
                    if (!is_facing_camera(shape_polyinfo->points)) {
                        shape_polyinfo->points[0] = *projected_point_ptr_table[3];
                        shape_polyinfo->points[1] = *projected_point_ptr_table[4];
                        shape_polyinfo->points[2] = *projected_point_ptr_table[5];
                        shape_polyinfo->points[3] = *projected_point_ptr_table[0];
                        depth = (long)vertbuf[current_prim_start[3]].z << 2;
                    } else {
                        depth = (long)vertbuf[current_prim_start[0]].z << 2;
                    }
                    i = polradius2d(shape_polyinfo->points[0].x - shape_polyinfo->points[1].x,
                                      shape_polyinfo->points[0].y - shape_polyinfo->points[1].y);
                    j = polradius2d(shape_polyinfo->points[0].x - shape_polyinfo->points[2].x,
                                      shape_polyinfo->points[0].y - shape_polyinfo->points[2].y);
                    if (j > i)
                        i = j;
                    if (obj_flags & 8) {
                        corner.x = shape_polyinfo->points[0].x - i - 1;
                        corner.y = shape_polyinfo->points[0].y - i - 1;
                        rect_adjust_from_point(&corner, current_rect);
                        corner.y = shape_polyinfo->points[0].y + i + 1;
                        corner.x = shape_polyinfo->points[0].x + i + 1;
                        rect_adjust_from_point(&corner, current_rect);
                        corner.x = shape_polyinfo->points[3].x - i - 1;
                        corner.y = shape_polyinfo->points[3].y - i - 1;
                        rect_adjust_from_point(&corner, current_rect);
                        corner.y = shape_polyinfo->points[3].y + i + 1;
                        corner.x = shape_polyinfo->points[3].x + i + 1;
                        rect_adjust_from_point(&corner, current_rect);
                    }
                    transformed_vert_count = 4;
                    added = 1;
                    break;
                case 2:
                    i = current_prim_start[0];
                    j = current_prim_start[1];
                    depth = vertbuf[i].z + vertbuf[j].z;
                    if (vertcode[i] + vertcode[j] == 0) {
                        shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                        viewvert = vertbuf[i];
                        surfvec = vertbuf[j];
                        local.x = viewvert.x - surfvec.x;
                        local.y = viewvert.y - surfvec.y;
                        local.z = viewvert.z - surfvec.z;
                        pixsize = projectiondata9_times_ratio(polarRadius3D(&local), viewvert.z);
                        shape_polyinfo->points[1].x = pixsize;
                        if (obj_flags & 8) {
                            corner.y = projected_point_ptr_table[0]->y - pixsize;
                            corner.x = projected_point_ptr_table[0]->x - pixsize;
                            rect_adjust_from_point(&corner, current_rect);
                            corner.x = pixsize + projected_point_ptr_table[0]->x;
                            corner.x = projected_point_ptr_table[0]->y + pixsize;  /* sic: original stores .x twice */
                            rect_adjust_from_point(&corner, current_rect);
                        }
                        transformed_vert_count = 2;
                        added++;
                    }
                    break;
                case 5:
                    i = current_prim_start[0];
                    if (vertcode[i] != 0)
                        break;
                    depth = vertbuf[i].z;
                    shape_polyinfo->points[0] = *projected_point_ptr_table[0];
                    if (obj_flags & 8)
                        rect_adjust_from_point(projected_point_ptr_table[0], current_rect);
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
            if ((primflag & 2) == 0) {
                while (current_prim_start[1] & 2) {
                    current_prim_start += primidxcounttab[*current_prim_start] + current_num_paints + 2;
                    cullwords++;
                    cull2ptr++;
                }
            }
        } else {
            numdrawn++;
            shape_polyinfo->numpoints = transformed_vert_count;
            shape_polyinfo->type = ptype;
            if (current_paintjob == 0x2D)
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
            if ((poly_list_insert_result = insert_newest_poly_in_poly_linked_list_40ED6(i, (obj_flags & 1) || (primflag & 2) ? 0 : 1)) != 0)
                return 1;
        }
    } while (*current_prim_start != 0);
    if (numdrawn == 0)
        return -1;
    return 0;
}
extern unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned depth, unsigned search_sorted_position) { /* PURPOSE: Handle the insert newest poly in poly linked list 40ed6 operation. Params: depth, search_sorted_position. Returns: unsigned. Globals: reads poly_cursor1, poly_info_ptrs, poly_link_list, poly_link_listit4, polygon_link_3_list_iter, polygonnumber, polyinfo_offset, transformed_vert_count; writes facenodeiterator, poly_link_list, poly_link_listit4, polygon_link_3_list_iter, polygonnumber, polyinfo_offset. */
    register int remaining;
    register int list_index;
    if (search_sorted_position == 0) {
        list_index = poly_link_list[poly_link_listit4];
    } else {
        poly_link_listit4 = poly_cursor1;
        list_index = poly_link_list[poly_cursor1];
        remaining = polygon_link_3_list_iter;
        goto scan_test;
    scan_body:
        if (remaining-- == 0) goto scan_done;
        if (poly_info_ptrs[list_index]->depth < (int)depth) goto scan_done;
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
    polyinfo_offset += transformed_vert_count * sizeof(struct POINT2D) + 6;
    return polygonnumber == 0x190 || (int)polyinfo_offset > 0x2872;
}

unsigned rect_compare_point(struct POINT2D *point) { /* PURPOSE: Return the clip-rectangle outcode for one projected point. Params: point. Returns: unsigned. Globals: reads selection_rect; writes none. */
    register struct POINT2D *p = point;
    char flag;
    if (p->y < selection_rect.top) flag=1;
    else if (p->y > selection_rect.bottom) flag=2;
    else flag=0;
    if (p->x < selection_rect.left) flag|=4;
    else if (p->x > selection_rect.right) flag|=8;
    return flag;
}

char is_facing_camera(struct POINT2D far *pts)
{ /* PURPOSE: Test the winding of a projected polygon against the camera. Params: pts. Returns: char. Globals: none. */
    long dx0, dx1, dy0, dy1;

    dx0 = (long)pts[0].x - pts[1].x;
    dx1 = (long)pts[2].x - pts[1].x;
    if (dx0 != 0 || dx1 != 0) {
        dy0 = pts[0].y - pts[1].y;
        dy1 = pts[2].y - pts[1].y;
        if (dy0 != 0 || dy1 != 0)
            return (char)(dx1 * dy0 - dx0 * dy1 > 0);
    }
    return 0;
}

void polyinfo(void)
{ /* PURPOSE: Draw queued polygon records through the matching primitive renderer. Params: none. Returns: void. Globals: reads g_mat_clrlist_copy_2_ptr, mat_copy_clr_lst_ptr, material_patlistptr_copy, matpatlistcopypointer2, poly_info_ptrs, poly_link_list, polygonnumber; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    struct POINT2D far* srcpts;
    struct POLYINFO far* info;
    register int polyIndex;
    register int link;
    struct POINT2D polypts[10];
    struct POINT2D* out;
    int fill;
    int materialIndex;
    unsigned idx;
    unsigned pointCount;

    link = 0x190;
    for (polyIndex = 0; polyIndex < polygonnumber; polyIndex++) {
        link = poly_link_list[link];
        info = poly_info_ptrs[link];
        materialIndex = info->material;
        fill = mat_copy_clr_lst_ptr[materialIndex];
        switch (info->type) {
        case 0:
            pointCount = info->numpoints;
            srcpts = info->points;
            out = polypts;
            for (idx = 0; idx < pointCount; idx++) {
                *out = *srcpts;
                out++;
                srcpts++;
            }
            switch (material_patlistptr_copy[materialIndex]) {
            case 0:
                preRender_default(fill, pointCount, polypts) /* PLATFORM(video): draw a filled polygon. */;
                break;
            case 1:
                if (matpatlistcopypointer2[materialIndex] != 0)
                    preRender_patterned(matpatlistcopypointer2[materialIndex], fill, pointCount, polypts) /* PLATFORM(video): draw a patterned polygon. */;
                break;
            case 2:
                preRender_unk(matpatlistcopypointer2[materialIndex], g_mat_clrlist_copy_2_ptr[materialIndex], fill, pointCount, polypts) /* PLATFORM(video): draw a polygon using the secondary fill parameters. */;
                break;
            }
            break;
        case 1:
            preRender_line(info->points[0].x, info->points[0].y, info->points[1].x, info->points[1].y, fill) /* PLATFORM(video): draw a line primitive. */;
            break;
        case 3:
            for (idx = 0; idx < 4; idx++)
                polypts[idx] = info->points[idx];
            preRender_wheel(polypts, 0x2500, fill, mat_copy_clr_lst_ptr[materialIndex + 1], mat_copy_clr_lst_ptr[materialIndex + 2]) /* PLATFORM(video): draw a wheel primitive. */;
            break;
        case 2:
            preRender_sphere(info->points[0].x, info->points[0].y, info->points[1].x, fill) /* PLATFORM(video): draw a sphere primitive. */;
            break;
        case 5:
            putpixel_single_maybe(info->points[0].x, info->points[0].y, fill) /* PLATFORM(video): plot a pixel primitive. */;
            break;
        }
    }
    polyinfo_reset();
}


struct MATRIX* matrotzxy(int z, int x, int y, int unk) { /* PURPOSE: Compose cached Z, X, and Y rotation matrices in the requested order. Params: z, x, y, unk. Returns: struct MATRIX*. Globals: reads g_matrix_yrot, g_rot_mat_z, mat_half_turn, mat_no_turn and 5 other globals; writes g_matrix_yrot, g_rot_mat_z, mat_half_turn, mat_no_turn and 5 other globals. */
    register int rotation_flags = 0;
    register struct MATRIX* result;

    if ((z & 0x3ff) != 0) {
        rotation_flags |= 4;
        mat_rot_z(&g_rot_mat_z, z);
    }
    if ((x & 0x3ff) != 0) {
        rotation_flags |= 2;
        mat_rot_x(&matrix_x_rotation, x);
    }
    if ((y & 0x3ff) != 0) {
        rotation_flags |= 1;
        if ((y & 0x3ff) == mat_y_rot_angle) {
            result = &g_matrix_yrot;
        } else {
            switch (y & 0x3ff) {
            case 0x100:
                result = &mat_quarter_turn;
                break;
            case 0x200:
                result = &mat_half_turn;
                break;
            case 0x300:
                result = &mat_three_quarters_turn;
                break;
            default:
                matroty(&g_matrix_yrot, y);
                mat_y_rot_angle = y & 0x3ff;
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
        if ((unk & 1) != 0)
            mat_multiply(result, &matrix_x_rotation, &matrotation_tmp);
        else
            mat_multiply(&matrix_x_rotation, result, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 4:
        result = &g_rot_mat_z;
        break;
    case 5:
        if ((unk & 1) != 0)
            mat_multiply(result, &g_rot_mat_z, &matrotation_tmp);
        else
            mat_multiply(&g_rot_mat_z, result, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 6:
        if ((unk & 1) != 0)
            mat_multiply(&matrix_x_rotation, &g_rot_mat_z, &matrotation_tmp);
        else
            mat_multiply(&g_rot_mat_z, &matrix_x_rotation, &matrotation_tmp);
        result = &matrotation_tmp;
        break;
    case 7:
        if ((unk & 1) != 0) {
            mat_multiply(result, &matrix_x_rotation, &matrotation_tmp);
            mat_multiply(&matrotation_tmp, &g_rot_mat_z, &matrix_x_rotation);
            result = &matrix_x_rotation;
        } else {
            mat_multiply(&g_rot_mat_z, &matrix_x_rotation, &matrotation_tmp);
            mat_multiply(&matrotation_tmp, result, &g_rot_mat_z);
            result = &g_rot_mat_z;
        }
        break;
    }
    return result;
}

void rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc) { /* PURPOSE: Expand a rectangle to include one projected point. Params: pt, rc. Returns: void. Globals: none. */
    register int x, y;
    x=pt->x; y=pt->y;
    if(rc->left>x) rc->left=x;
    { int temp=x+1; if(rc->right<temp) rc->right=temp; }
    if(rc->top>y) rc->top=y;
    { int temp=y+1; if(rc->bottom<temp) rc->bottom=temp; }
}

vector_op_unk2(struct VECTOR* vec) { /* PURPOSE: Classify a view vector into the renderer angle and octant table. Params: vec. Returns: unspecified. Globals: reads vector_angle_cos, vector_angle_sin; writes none. */
	long height;
	long temp;
	char octant;
	int below;
	int angle;
	
	height = abs(vec->y);
	
	temp = polradius2d(abs(vec->x), abs(vec->z));
	
	if (vector_angle_cos == vector_angle_sin) {
		below = temp < height;
	} else {
		below = temp * vector_angle_cos < height * vector_angle_sin;
	}
	
	if (vec->y < 0) {
		if (below != 0) return 0x1E;
	} else
	if (vec->y > 0) {
		if (below != 0) return 0x1F;
	}

	if (vec->y > 0) {
		octant = 0x0F;
	} else {
		octant = 0;
	}
	
	angle = -polang(vec->z, -vec->x);
	if (angle < 0) {
		angle += 0x400;
	}
	
	octant += (((long)angle * 15L) >> 10);
	
	return octant;
}

void calc_sincos80(void) { /* PURPOSE: Cache the sine and cosine values used by view-vector classification. Params: none. Returns: void. Globals: reads none; writes vector_angle_cos, vector_angle_cos2, vector_angle_sin, vector_angle_sin2. */
    vector_angle_sin = (long)sinfast(0x80);
    vector_angle_cos = (long)cosfast(0x80);
    vector_angle_sin2 = (long)sinfast(0x80);
    vector_angle_cos2 = (long)cosfast(0x80);
}

long nopsub_26552(long value) { /* PURPOSE: Return the magnitude of a signed long value. Params: value. Returns: long. Globals: none. */ if (value < 0) return -value; return value; }


short g_vid_flg2_set;
short vidflg3is_minus1;
extern void fatal_error(char *);
void rcunion(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
char rcintersect(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_overlapping(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_inside(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_adjacent(struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add_rect(char *, struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add(unsigned char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);

void rcunion(struct RECTANGLE *r1, struct RECTANGLE *r2, struct RECTANGLE *out)
{ /* PURPOSE: Write the smallest rectangle that covers both input rectangles. Params: r1, r2, out. Returns: void. Globals: reads g_vid_flg2_set, vidflg3is_minus1; writes vidflg3is_minus1. */
    out->left = r1->left <= r2->left ? r1->left : r2->left;
    out->right = r1->right >= r2->right ? r1->right : r2->right;
    out->top = r1->top <= r2->top ? r1->top : r2->top;
    out->bottom = r1->bottom >= r2->bottom ? r1->bottom : r2->bottom;
    if (g_vid_flg2_set != 1) {
        out->right = (out->right + g_vid_flg2_set - 1) & vidflg3is_minus1;
    }
}

char rcintersect(struct RECTANGLE *r1, struct RECTANGLE *r2)
{ /* PURPOSE: Clip the first rectangle to the second and report an empty intersection. Params: r1, r2. Returns: char. Globals: none. */
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

void rectlist_add_rect(char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr, struct RECTANGLE* rect) { /* PURPOSE: Merge or split a rectangle as needed before adding it to a clip list. Params: arg_rect_array_length_ptr, arg_rect_array_ptr, rect. Returns: void. Globals: reads g_vid_flg2_set, vidflg3is_minus1; writes vidflg3is_minus1. */	
	struct RECTANGLE bottom_rect;
	struct RECTANGLE merged;
	char shift_index;
	char rect_count;
	char top_needed;
	struct RECTANGLE* entry_ptr;
	struct RECTANGLE top_piece;
	char bottom_needed;

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
		if (rect_is_overlapping(rect, entry_ptr) == 0)
			continue;
		if (rect_is_inside(rect, entry_ptr) != 0)
			return ;

		if (rect_is_inside(entry_ptr, rect) != 0) {
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
			rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &top_piece);
		}

		rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		if (bottom_needed != 0) {
			rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &bottom_rect);
			return ;
		}
		return ;
	}

	for (rect_count = 0; rect_count < *arg_rect_array_length_ptr; rect_count++) {
		entry_ptr = &arg_rect_array_ptr[rect_count];

		if (rect_is_adjacent(entry_ptr, rect) != 0) {
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
		rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		return ;
		}
	}

	arg_rect_array_ptr[*arg_rect_array_length_ptr] = *rect;
	(*arg_rect_array_length_ptr)++;
}

char rect_is_overlapping(struct RECTANGLE* r1, struct RECTANGLE* r2) { /* PURPOSE: Test whether two half-open rectangles overlap. Params: r1, r2. Returns: char. Globals: none. */
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

char rect_is_inside(struct RECTANGLE *a,struct RECTANGLE *b){ /* PURPOSE: Test whether the first rectangle is contained in the second. Params: a, b. Returns: char. Globals: none. */return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}

char rect_is_adjacent(struct RECTANGLE* r1, struct RECTANGLE* r2) { /* PURPOSE: Test whether two rectangles share a complete edge. Params: r1, r2. Returns: char. Globals: none. */
 if (r1->bottom == r2->top) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->top == r2->bottom) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->right == r2->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 else if (r2->right == r1->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 return 0;
}

void rectlist_add(unsigned char arg_rectcount, char* arg_rectarray_indices, 
	struct RECTANGLE* arg_rectarray1, struct RECTANGLE* arg_rectarray2, 
	struct RECTANGLE* arg_rectptr, char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr) 
{ /* PURPOSE: Select, clip, and add the requested source rectangles to the result list. Params: arg_rectcount, arg_rectarray_indices, arg_rectarray1, arg_rectarray2, arg_rectptr, arg_rect_array_length_ptr, arg_rect_array_ptr. Returns: void. Globals: none. */
	char has_result;
	struct RECTANGLE* selected_rect_ptr;
	struct RECTANGLE cover_rect;
	struct RECTANGLE* first_ptr;
	char rect_counter;
	char source_flags;
	struct RECTANGLE input_rect;
	struct RECTANGLE* second_rect_ptr;
/*
	return ported_rect_clip_combined_(
		arg_rectcount, arg_rectarray_indices, arg_rectarray1, arg_rectarray2, arg_rectptr,
		arg_rect_array_length_ptr, arg_rect_array_ptr);
	*/
	for (rect_counter = 0; rect_counter < arg_rectcount; rect_counter++) {

		source_flags = arg_rectarray_indices[rect_counter];
		if ((source_flags & 1) != 0) {
			first_ptr = &arg_rectarray1[rect_counter];
		}

		if ((source_flags & 2) != 0) {
			second_rect_ptr = &arg_rectarray2[rect_counter];
		}

		if ((source_flags & 1) != 0 && first_ptr->right > first_ptr->left) {
			if ((source_flags & 2) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
				rcunion(first_ptr, second_rect_ptr, &cover_rect);
				selected_rect_ptr = &cover_rect;
			} else {
				selected_rect_ptr = first_ptr;
			}
			has_result = 1;
		} else if ((source_flags & 2) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
			selected_rect_ptr = second_rect_ptr;
			has_result = 1;
		} else {
			has_result = 0;
		}
		if (has_result != 0) {
			input_rect = *selected_rect_ptr;
			if (rcintersect(&input_rect, arg_rectptr) == 0) {
				rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &input_rect);
			}
		}
	}

}

void heapsortorder(int, int*, int*);

void rectsorttop(char arg_array_length, struct RECTANGLE* arg_rect_array, int* arg_array_indices) { /* PURPOSE: Sort rectangle indices by their top coordinate. Params: arg_array_length, arg_rect_array, arg_array_indices. Returns: void. Globals: none. */
    register int sortIndex;
    int intbuffer[256];
    if (arg_array_length > 1) {
        for (sortIndex = 0; sortIndex < arg_array_length; sortIndex++) {
            intbuffer[sortIndex] = -arg_rect_array[sortIndex].top;
            arg_array_indices[sortIndex] = sortIndex;
        }
        heapsortorder(arg_array_length, intbuffer, arg_array_indices);
    } else {
        arg_array_indices[0] = 0;
    }
}

unsigned char primidxcounttab[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2, 6, 3, 0, 0
};
unsigned char primitive_type_table[16] = {
    0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 0
};
long inverse_power_of_two_table[32] = {
    (-2147483647L - 1L), 1073741824L, 536870912L, 268435456L,
    134217728L, 67108864L, 33554432L, 16777216L,
    8388608L, 4194304L, 2097152L, 1048576L,
    524288L, 262144L, 131072L, 65536L,
    32768L, 16384L, 8192L, 4096L,
    2048L, 1024L, 512L, 256L,
    128L, 64L, 32L, 16L,
    8L, 4L, 2L, 1L
};
unsigned mat_y_rot_angle = 0xFFFF;
struct RECTANGLE clipunk = { 9999, -1, 9999, -1 };
