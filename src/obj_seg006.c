/* obj_seg006: whole seg006 object (22 functions, file order = image order).
   Globals without a reviewed semantic alias use their bound data names:
   data_31852 = transshapenumpaints, byte_3187A = transshapematerial,
   word_31878 = transshaperectptr, word_3186A = transshapeprimptr,
   byte_31882 = transprimitivepaintjob, byte_35514 = backlights_paint_override,
   word_31854 = polyvertpointptrtab, byte_2EA72 = primtypetab, word_2EA82 = invpow2tbl. */
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
extern int far polarAngle(int, int);
extern unsigned far polarRadius2D(int, int);
extern int far polarRadius3D(struct VECTOR*);
extern int far projectiondata9_times_ratio(int, int);
extern void far mat_rot_y(struct MATRIX*, int);
extern void far mat_rot_x(struct MATRIX*, int);
extern void far mat_rot_z(struct MATRIX*, int);
extern void far mat_mul_vector(struct VECTOR*, struct MATRIX*, struct VECTOR*);
extern void far mat_multiply(struct MATRIX*, struct MATRIX*, struct MATRIX*);
extern void far mat_invert(struct MATRIX*, struct MATRIX*);
extern void far vector_to_point(struct VECTOR*, struct POINT2D*);
extern void far vector_op_unk(struct VECTOR*, struct VECTOR*, struct VECTOR*, int);
extern void far* far mmgr_alloc_resbytes(char*, long);
extern int far sin_fast(unsigned);
extern int far cos_fast(unsigned);
extern void far preRender_default(int, int, struct POINT2D*);
extern void far preRender_patterned(int, int, int, struct POINT2D*);
extern void far preRender_unk(int, int, int, int, struct POINT2D*);
extern void far preRender_line(int, int, int, int, int);
extern void far preRender_sphere(int, int, int, int);
extern void far preRender_wheel(struct POINT2D*, int, int, int, int);
extern void far putpixel_single_maybe(int, int, int);

extern struct MATRIX mat_y0, mat_y100, mat_y200, mat_y300;
extern struct MATRIX mat_y_rot, mat_x_rot, mat_z_rot, mat_rot_temp;
extern unsigned mat_y_rot_angle;
extern struct MATRIX mat_temp;
extern struct RECTANGLE select_rect_rc;
extern int select_rect_param;
extern int* material_clrlist_ptr_cpy;
extern int* material_clrlist2_ptr_cpy;
extern int* material_patlist_ptr_cpy;
extern int* material_patlist2_ptr_cpy;
extern unsigned short someZeroVideoConst;
extern long var_6114, var_6118, var_6120, var_611c;

extern unsigned polyinfonumpolys;
extern unsigned polyinfoptrnext;
extern int word_40ECE;
extern int word_411F6;
extern int poly_linklist_40ED6_iter1;
extern int poly_linklist_40ED6_iter2;
extern int poly_linklist_40ED6_iter3;
extern int poly_linklist_40ED6_iter4;
extern int poly_linked_list_40ED6[];
extern char far* polyinfoptr;
extern struct POLYINFO far* polyinfoptrs[];
extern struct POLYINFO far* transshapepolyinfo;
extern unsigned transshapenumverts;
extern struct VECTOR far* transshapeverts;
extern unsigned char far* transshapeprimitives;
extern unsigned char far* transshapeprimindexptr;
extern unsigned char far* word_3186A;
extern int data_31852;
extern unsigned char byte_3187A;
extern unsigned char transshapeflags;
extern unsigned char transshapenumvertscopy;
extern struct RECTANGLE* word_31878;
extern char byte_31882;
extern char byte_35514;
extern char byte_4393D;
extern long word_2EA82[];
extern unsigned char primidxcounttab[];
extern unsigned char byte_2EA72[];
extern struct POINT2D* word_31854[];

void polyinfo_reset(void);
void calc_sincos80(void);
struct MATRIX* mat_rot_zxy(int, int, int, int);
unsigned rect_compare_point(struct POINT2D*);
char is_facing_camera(struct POINT2D far*);
void rect_adjust_from_point(struct POINT2D*, struct RECTANGLE*);
vector_op_unk2(struct VECTOR*);
unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned, unsigned);

void init_polyinfo(void)
{
    polyinfoptr = mmgr_alloc_resbytes("polyinfo", 0x28A0);
    mat_rot_y(&mat_y0, 0);
    mat_rot_y(&mat_y100, 0x100);
    mat_rot_y(&mat_y200, 0x200);
    mat_rot_y(&mat_y300, 0x300);
    calc_sincos80();
}

void copy_material_list_pointers(void* clrlist, void* clrlist2, void* patlist, void* patlist2, unsigned short videoConst)
{
    material_clrlist_ptr_cpy = clrlist;
    material_clrlist2_ptr_cpy = clrlist2;
    material_patlist_ptr_cpy = patlist;
    material_patlist2_ptr_cpy = patlist2;
    someZeroVideoConst = videoConst;
}

void polyinfo_reset(void)
{
    polyinfonumpolys = 0;
    polyinfoptrnext = 0;
    word_40ECE = 0;
    word_411F6 = 0xffff;
    poly_linklist_40ED6_iter2 = 0x190;
}

unsigned select_cliprect_rotate(int angZ, int angX, int angY, struct RECTANGLE* cliprect, int unk)
{
    struct MATRIX* matptr;
    struct VECTOR vec, vec2;

    mat_temp = *mat_rot_zxy(angZ, angX, angY, 1);
    polyinfo_reset();
    select_rect_rc = *cliprect;
    select_rect_param = unk;
    matptr = mat_rot_zxy(-angZ, -angX, -angY, 0);
    vec.z = 0x2710;
    vec.y = 0;
    vec.x = 0;
    mat_mul_vector(&vec, matptr, &vec2);
    return polarAngle(vec2.x, vec2.z) & 0x3FF;
}

unsigned transformed_shape_op(struct TRANSFORMEDSHAPE3D* ts)
{
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

    if (word_40ECE != 0)
        return 1;
    transshapenumverts = ts->shapeptr->numverts;
    transshapeprimitives = ts->shapeptr->primitives;
    transshapeverts = ts->shapeptr->verts;
    data_31852 = ts->shapeptr->numpaints;
    cullwords = ts->shapeptr->cull1;
    cull2ptr = ts->shapeptr->cull2;
    byte_3187A = ts->material;
    if (byte_3187A >= (unsigned char)data_31852)
        byte_3187A = 0;
    transshapeflags = ts->ts_flags;
    if (transshapeflags & 8)
        word_31878 = ts->rectptr;
    for (i = 0; i < transshapenumverts; i++)
        vertcode[i] = -1;

    if (transshapeflags & 2) {
        matptr = mat_rot_zxy(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        mat_multiply(matptr, &mat_temp, &mview);
        position = ts->pos;
        cullmask = -1L;
        cullbits2 = 0;
    } else {
        matptr = mat_rot_zxy(ts->rotvec.x, ts->rotvec.y, ts->rotvec.z, 0);
        mat_mul_vector(&ts->pos, &mat_temp, &position);
        mat_multiply(matptr, &mat_temp, &mview);
        mat_invert(&mview, &invview);
        local.x = 0;
        local.y = 0;
        local.z = 0x1000;
        mat_mul_vector(&local, &invview, &viewvert);
        if ((viewvert.y > 0 && ts->pos.y < 0) ||
            (ts->unk * 2 > abs(position.x) && ts->unk * 2 > abs(position.z))) {
            cullmask = -1L;
            cullbits2 = 0;
        } else {
            byte_4393D = vector_op_unk2(&viewvert);
            cullbits2 = cullmask = word_2EA82[byte_4393D];
        }
    }

    poly_linklist_40ED6_iter4 = poly_linklist_40ED6_iter1 = poly_linklist_40ED6_iter2;
    poly_linklist_40ED6_iter3 = 0;
    numdrawn = 0;
    if (transshapenumverts > 8)
        transshapenumvertscopy = 8;
    else
        transshapenumvertscopy = transshapenumverts;
    if (transshapenumvertscopy > 4 && transshapeverts[0].y == transshapeverts[4].y)
        transshapenumvertscopy = 4;

    rectcode = 0x0F;
    hidden = 1;
    someclipped = 0;
    for (i = 0; i < transshapenumvertscopy; i++) {
        word_31854[i] = &screenpts[i];
        local = transshapeverts[i];
        if (select_rect_param != 0) {
            local.x >>= 1;
            local.y >>= 1;
            local.z >>= 1;
        }
        mat_mul_vector(&local, &mview, &viewvert);
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
            vector_to_point(&viewvert, word_31854[i]);
            if (rectcode != 0)
                rectcode &= rect_compare_point(word_31854[i]);
            if (rectcode == 0)
                goto visible;
        }
    }
    if (hidden != 0 || someclipped == 0 || ts->unk < abs(position.x))
        return -1;

visible:
    transshapeprimitives = ts->shapeptr->primitives;
    do {
        word_3186A = transshapeprimitives + primidxcounttab[*transshapeprimitives] + data_31852 + 2;
        primflag = transshapeprimitives[1];
        added = 0;
        if (*cullwords & cullmask) {
            prim = transshapeprimitives[0];
            transshapenumvertscopy = primidxcounttab[prim];
            ptype = byte_2EA72[prim];
            transshapepolyinfo = (struct POLYINFO far*)(polyinfoptr + polyinfoptrnext);
            polyinfoptrs[polyinfonumpolys] = transshapepolyinfo;
            byte_31882 = transshapeprimitives[byte_3187A + 2];
            transshapeprimitives += data_31852 + 2;
            rectcode = 0x0F;
            hidden = 1;
            someclipped = 0;
            transshapeprimindexptr = transshapeprimitives;
            for (npts = 0; npts < transshapenumvertscopy; npts++) {
                i = *transshapeprimindexptr++;
                word_31854[npts] = &screenpts[i];
                switch (vertcode[i]) {
                case 0:
                    hidden = 0;
                    if (rectcode != 0)
                        rectcode &= rect_compare_point(word_31854[npts]);
                    break;
                case -1:
                    local = transshapeverts[i];
                    if (select_rect_param != 0) {
                        local.x >>= 1;
                        local.y >>= 1;
                        local.z >>= 1;
                    }
                    mat_mul_vector(&local, &mview, &viewvert);
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
                        vector_to_point(&viewvert, word_31854[npts]);
                        if (rectcode != 0)
                            rectcode &= rect_compare_point(word_31854[npts]);
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
                    out = transshapepolyinfo->points;
                    transshapeprimindexptr = transshapeprimitives;
                    depth = 0;
                    rectcode = 0x0F;
                    if (someclipped == 0) {
                        for (i = 0; i < transshapenumvertscopy; i++) {
                            idx = *transshapeprimindexptr++;
                            depth += vertbuf[idx].z;
                            *out = *word_31854[i];
                            if (rectcode != 0)
                                rectcode &= rect_compare_point(word_31854[i]);
                            out++;
                        }
                    } else {
                        npts = 0;
                        lastidx = transshapeprimitives[transshapenumvertscopy - 1];
                        for (i = 0; i < transshapenumvertscopy; i++) {
                            idx = *transshapeprimindexptr++;
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
                                *out = *word_31854[i];
                                if (rectcode != 0)
                                    rectcode &= rect_compare_point(word_31854[i]);
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
                        transshapenumvertscopy = npts;
                    }
                    if (transshapenumvertscopy != 0 && rectcode == 0) {
                        if ((primflag & 1) || (*cull2ptr & cullbits2) || is_facing_camera(transshapepolyinfo->points))
                            added++;
                        if (added != 0 && (transshapeflags & 8) != 0) {
                    srcpt = transshapepolyinfo->points;
                    for (npts = 0; npts < transshapenumvertscopy; npts++) {
                        ptx = srcpt->x;
                        pty = srcpt->y;
                        srcpt++;
                        if (ptx < word_31878->left)
                            word_31878->left = ptx;
                        if (word_31878->right < ptx + 1)
                            word_31878->right = ptx + 1;
                        if (word_31878->top > pty)
                            word_31878->top = pty;
                        if (word_31878->bottom < pty + 1)
                            word_31878->bottom = pty + 1;
                    }
                        }
                    }
                    break;
                case 1:
                    i = transshapeprimitives[0];
                    j = transshapeprimitives[1];
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
                    transshapepolyinfo->points[0] = *word_31854[0];
                    transshapepolyinfo->points[1] = *word_31854[1];
                    if (transshapeflags & 8) {
                        rect_adjust_from_point(word_31854[0], word_31878);
                        rect_adjust_from_point(word_31854[1], word_31878);
                    }
                    transshapenumvertscopy = 2;
                    added++;
                    break;
                case 3:
                    if (someclipped != 0)
                        break;
                    transshapepolyinfo->points[0] = *word_31854[0];
                    transshapepolyinfo->points[1] = *word_31854[1];
                    transshapepolyinfo->points[2] = *word_31854[2];
                    transshapepolyinfo->points[3] = *word_31854[3];
                    if (!is_facing_camera(transshapepolyinfo->points)) {
                        transshapepolyinfo->points[0] = *word_31854[3];
                        transshapepolyinfo->points[1] = *word_31854[4];
                        transshapepolyinfo->points[2] = *word_31854[5];
                        transshapepolyinfo->points[3] = *word_31854[0];
                        depth = (long)vertbuf[transshapeprimitives[3]].z << 2;
                    } else {
                        depth = (long)vertbuf[transshapeprimitives[0]].z << 2;
                    }
                    i = polarRadius2D(transshapepolyinfo->points[0].x - transshapepolyinfo->points[1].x,
                                      transshapepolyinfo->points[0].y - transshapepolyinfo->points[1].y);
                    j = polarRadius2D(transshapepolyinfo->points[0].x - transshapepolyinfo->points[2].x,
                                      transshapepolyinfo->points[0].y - transshapepolyinfo->points[2].y);
                    if (j > i)
                        i = j;
                    if (transshapeflags & 8) {
                        corner.x = transshapepolyinfo->points[0].x - i - 1;
                        corner.y = transshapepolyinfo->points[0].y - i - 1;
                        rect_adjust_from_point(&corner, word_31878);
                        corner.y = transshapepolyinfo->points[0].y + i + 1;
                        corner.x = transshapepolyinfo->points[0].x + i + 1;
                        rect_adjust_from_point(&corner, word_31878);
                        corner.x = transshapepolyinfo->points[3].x - i - 1;
                        corner.y = transshapepolyinfo->points[3].y - i - 1;
                        rect_adjust_from_point(&corner, word_31878);
                        corner.y = transshapepolyinfo->points[3].y + i + 1;
                        corner.x = transshapepolyinfo->points[3].x + i + 1;
                        rect_adjust_from_point(&corner, word_31878);
                    }
                    transshapenumvertscopy = 4;
                    added = 1;
                    break;
                case 2:
                    i = transshapeprimitives[0];
                    j = transshapeprimitives[1];
                    depth = vertbuf[i].z + vertbuf[j].z;
                    if (vertcode[i] + vertcode[j] == 0) {
                        transshapepolyinfo->points[0] = *word_31854[0];
                        viewvert = vertbuf[i];
                        surfvec = vertbuf[j];
                        local.x = viewvert.x - surfvec.x;
                        local.y = viewvert.y - surfvec.y;
                        local.z = viewvert.z - surfvec.z;
                        pixsize = projectiondata9_times_ratio(polarRadius3D(&local), viewvert.z);
                        transshapepolyinfo->points[1].x = pixsize;
                        if (transshapeflags & 8) {
                            corner.y = word_31854[0]->y - pixsize;
                            corner.x = word_31854[0]->x - pixsize;
                            rect_adjust_from_point(&corner, word_31878);
                            corner.x = pixsize + word_31854[0]->x;
                            corner.x = word_31854[0]->y + pixsize;  /* sic: original stores .x twice */
                            rect_adjust_from_point(&corner, word_31878);
                        }
                        transshapenumvertscopy = 2;
                        added++;
                    }
                    break;
                case 5:
                    i = transshapeprimitives[0];
                    if (vertcode[i] != 0)
                        break;
                    depth = vertbuf[i].z;
                    transshapepolyinfo->points[0] = *word_31854[0];
                    if (transshapeflags & 8)
                        rect_adjust_from_point(word_31854[0], word_31878);
                    transshapenumvertscopy = 1;
                    added++;
                    break;
                }
            }
        }
        transshapeprimitives = word_3186A;
        cull2ptr++;
        cullwords++;
        if (added == 0) {
            if ((primflag & 2) == 0) {
                while (transshapeprimitives[1] & 2) {
                    transshapeprimitives += primidxcounttab[*transshapeprimitives] + data_31852 + 2;
                    cullwords++;
                    cull2ptr++;
                }
            }
        } else {
            numdrawn++;
            transshapepolyinfo->numpoints = transshapenumvertscopy;
            transshapepolyinfo->type = ptype;
            if (byte_31882 == 0x2D)
                transshapepolyinfo->material = byte_35514;
            else
                transshapepolyinfo->material = byte_31882;
            switch (transshapenumvertscopy) {
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
                i = depth / transshapenumvertscopy;
                break;
            }
            transshapepolyinfo->depth = i;
            if ((word_40ECE = insert_newest_poly_in_poly_linked_list_40ED6(i, (transshapeflags & 1) || (primflag & 2) ? 0 : 1)) != 0)
                return 1;
        }
    } while (*transshapeprimitives != 0);
    if (numdrawn == 0)
        return -1;
    return 0;
}
extern unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned arg_0, unsigned arg_2) {
    register int remaining;
    register int list_index;
    if (arg_2 == 0) {
        list_index = poly_linked_list_40ED6[poly_linklist_40ED6_iter4];
    } else {
        poly_linklist_40ED6_iter4 = poly_linklist_40ED6_iter1;
        list_index = poly_linked_list_40ED6[poly_linklist_40ED6_iter1];
        remaining = poly_linklist_40ED6_iter3;
        goto scan_test;
    scan_body:
        if (remaining-- == 0) goto scan_done;
        if (polyinfoptrs[list_index]->depth < (int)arg_0) goto scan_done;
        poly_linklist_40ED6_iter4 = list_index;
        list_index = poly_linked_list_40ED6[list_index];
    scan_test:
        if (list_index >= 0) goto scan_body;
    scan_done:
        ;
    }
    poly_linked_list_40ED6[polyinfonumpolys] = list_index;
    poly_linked_list_40ED6[poly_linklist_40ED6_iter4] = polyinfonumpolys;
    poly_linklist_40ED6_iter3++;
    if (list_index < 0) poly_linklist_40ED6_iter2 = polyinfonumpolys;
    poly_linklist_40ED6_iter4 = poly_linked_list_40ED6[poly_linklist_40ED6_iter4];
    polyinfonumpolys++;
    polyinfoptrnext += transshapenumvertscopy * sizeof(struct POINT2D) + 6;
    return polyinfonumpolys == 0x190 || (int)polyinfoptrnext > 0x2872;
}

unsigned rect_compare_point(struct POINT2D *point) {
    register struct POINT2D *p = point;
    char flag;
    if (p->y < select_rect_rc.top) flag=1;
    else if (p->y > select_rect_rc.bottom) flag=2;
    else flag=0;
    if (p->x < select_rect_rc.left) flag|=4;
    else if (p->x > select_rect_rc.right) flag|=8;
    return flag;
}

char is_facing_camera(struct POINT2D far *pts)
{
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

void get_a_poly_info(void)
{
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
    for (polyIndex = 0; polyIndex < polyinfonumpolys; polyIndex++) {
        link = poly_linked_list_40ED6[link];
        info = polyinfoptrs[link];
        materialIndex = info->material;
        fill = material_clrlist_ptr_cpy[materialIndex];
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
            switch (material_patlist_ptr_cpy[materialIndex]) {
            case 0:
                preRender_default(fill, pointCount, polypts);
                break;
            case 1:
                if (material_patlist2_ptr_cpy[materialIndex] != 0)
                    preRender_patterned(material_patlist2_ptr_cpy[materialIndex], fill, pointCount, polypts);
                break;
            case 2:
                preRender_unk(material_patlist2_ptr_cpy[materialIndex], material_clrlist2_ptr_cpy[materialIndex], fill, pointCount, polypts);
                break;
            }
            break;
        case 1:
            preRender_line(info->points[0].x, info->points[0].y, info->points[1].x, info->points[1].y, fill);
            break;
        case 3:
            for (idx = 0; idx < 4; idx++)
                polypts[idx] = info->points[idx];
            preRender_wheel(polypts, 0x2500, fill, material_clrlist_ptr_cpy[materialIndex + 1], material_clrlist_ptr_cpy[materialIndex + 2]);
            break;
        case 2:
            preRender_sphere(info->points[0].x, info->points[0].y, info->points[1].x, fill);
            break;
        case 5:
            putpixel_single_maybe(info->points[0].x, info->points[0].y, fill);
            break;
        }
    }
    polyinfo_reset();
}


struct MATRIX* mat_rot_zxy(int z, int x, int y, int unk) {
    register int rotation_flags = 0;
    register struct MATRIX* result;

    if ((z & 0x3ff) != 0) {
        rotation_flags |= 4;
        mat_rot_z(&mat_z_rot, z);
    }
    if ((x & 0x3ff) != 0) {
        rotation_flags |= 2;
        mat_rot_x(&mat_x_rot, x);
    }
    if ((y & 0x3ff) != 0) {
        rotation_flags |= 1;
        if ((y & 0x3ff) == mat_y_rot_angle) {
            result = &mat_y_rot;
        } else {
            switch (y & 0x3ff) {
            case 0x100:
                result = &mat_y100;
                break;
            case 0x200:
                result = &mat_y200;
                break;
            case 0x300:
                result = &mat_y300;
                break;
            default:
                mat_rot_y(&mat_y_rot, y);
                mat_y_rot_angle = y & 0x3ff;
                result = &mat_y_rot;
                break;
            }
        }
    }

    switch (rotation_flags) {
    case 0:
        result = &mat_y0;
        break;
    case 1:
        break;
    case 2:
        result = &mat_x_rot;
        break;
    case 3:
        if ((unk & 1) != 0)
            mat_multiply(result, &mat_x_rot, &mat_rot_temp);
        else
            mat_multiply(&mat_x_rot, result, &mat_rot_temp);
        result = &mat_rot_temp;
        break;
    case 4:
        result = &mat_z_rot;
        break;
    case 5:
        if ((unk & 1) != 0)
            mat_multiply(result, &mat_z_rot, &mat_rot_temp);
        else
            mat_multiply(&mat_z_rot, result, &mat_rot_temp);
        result = &mat_rot_temp;
        break;
    case 6:
        if ((unk & 1) != 0)
            mat_multiply(&mat_x_rot, &mat_z_rot, &mat_rot_temp);
        else
            mat_multiply(&mat_z_rot, &mat_x_rot, &mat_rot_temp);
        result = &mat_rot_temp;
        break;
    case 7:
        if ((unk & 1) != 0) {
            mat_multiply(result, &mat_x_rot, &mat_rot_temp);
            mat_multiply(&mat_rot_temp, &mat_z_rot, &mat_x_rot);
            result = &mat_x_rot;
        } else {
            mat_multiply(&mat_z_rot, &mat_x_rot, &mat_rot_temp);
            mat_multiply(&mat_rot_temp, result, &mat_z_rot);
            result = &mat_z_rot;
        }
        break;
    }
    return result;
}

void rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc) {
    register int x, y;
    x=pt->x; y=pt->y;
    if(rc->left>x) rc->left=x;
    { int temp=x+1; if(rc->right<temp) rc->right=temp; }
    if(rc->top>y) rc->top=y;
    { int temp=y+1; if(rc->bottom<temp) rc->bottom=temp; }
}

vector_op_unk2(struct VECTOR* vec) {
	long height;
	long temp;
	char octant;
	int below;
	int angle;
	
	height = abs(vec->y);
	
	temp = polarRadius2D(abs(vec->x), abs(vec->z));
	
	if (var_6114 == var_6118) {
		below = temp < height;
	} else {
		below = temp * var_6114 < height * var_6118;
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
	
	angle = -polarAngle(vec->z, -vec->x);
	if (angle < 0) {
		angle += 0x400;
	}
	
	octant += (((long)angle * 15L) >> 10);
	
	return octant;
}

void calc_sincos80(void) {
    var_6118 = (long)sin_fast(0x80);
    var_6114 = (long)cos_fast(0x80);
    var_6120 = (long)sin_fast(0x80);
    var_611c = (long)cos_fast(0x80);
}

long nopsub_26552(long value) { if (value < 0) return -value; return value; }


extern short video_flag2_is1;
extern short video_flag3_isFFFF;
extern void fatal_error(char *);
void rect_union(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
char rect_intersect(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_overlapping(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_inside(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_adjacent(struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add_rect(char *, struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add_rects(unsigned char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);

void rect_union(struct RECTANGLE *r1, struct RECTANGLE *r2, struct RECTANGLE *out)
{
    out->left = r1->left <= r2->left ? r1->left : r2->left;
    out->right = r1->right >= r2->right ? r1->right : r2->right;
    out->top = r1->top <= r2->top ? r1->top : r2->top;
    out->bottom = r1->bottom >= r2->bottom ? r1->bottom : r2->bottom;
    if (video_flag2_is1 != 1) {
        out->right = (out->right + video_flag2_is1 - 1) & video_flag3_isFFFF;
    }
}

char rect_intersect(struct RECTANGLE *r1, struct RECTANGLE *r2)
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

void rectlist_add_rect(char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr, struct RECTANGLE* rect) {	
	struct RECTANGLE bottom_rect;
	struct RECTANGLE merged;
	char shift_index;
	char rect_count;
	char top_needed;
	struct RECTANGLE* entry_ptr;
	struct RECTANGLE top_piece;
	char bottom_needed;

	if (video_flag2_is1 != 1) {
		rect->right = (rect->right + video_flag2_is1 - 1) & video_flag3_isFFFF;
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

char rect_is_overlapping(struct RECTANGLE* r1, struct RECTANGLE* r2) {
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

char rect_is_inside(struct RECTANGLE *a,struct RECTANGLE *b){return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}

char rect_is_adjacent(struct RECTANGLE* r1, struct RECTANGLE* r2) {
 if (r1->bottom == r2->top) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->top == r2->bottom) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->right == r2->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 else if (r2->right == r1->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 return 0;
}

void rectlist_add_rects(unsigned char arg_rectcount, char* arg_rectarray_indices, 
	struct RECTANGLE* arg_rectarray1, struct RECTANGLE* arg_rectarray2, 
	struct RECTANGLE* arg_rectptr, char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr) 
{
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
				rect_union(first_ptr, second_rect_ptr, &cover_rect);
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
			if (rect_intersect(&input_rect, arg_rectptr) == 0) {
				rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &input_rect);
			}
		}
	}

}

void heapsort_by_order(int, int*, int*);

void rect_array_sort_by_top(char arg_array_length, struct RECTANGLE* arg_rect_array, int* arg_array_indices) {
    register int sortIndex;
    int intbuffer[256];
    if (arg_array_length > 1) {
        for (sortIndex = 0; sortIndex < arg_array_length; sortIndex++) {
            intbuffer[sortIndex] = -arg_rect_array[sortIndex].top;
            arg_array_indices[sortIndex] = sortIndex;
        }
        heapsort_by_order(arg_array_length, intbuffer, arg_array_indices);
    } else {
        arg_array_indices[0] = 0;
    }
}
