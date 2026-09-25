/* Scratch whole-TU hypothesis for seg006 14D64..16552. */
struct RECTANGLE { int left, right, top, bottom; };
struct VECTOR { short x, y, z; };
struct POINT2D { int px, py; };
struct MATRIX { int vals[9]; };
struct SHAPE3D {
    unsigned short shape3d_numverts;
    struct VECTOR far* shape3d_verts;
    unsigned short shape3d_numprimitives;
    unsigned short shape3d_numpaints;
    char far* shape3d_primitives;
    char far* shape3d_cull1;
    char far* shape3d_cull2;
};
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D* shapeptr;
    struct RECTANGLE* rectptr;
    struct VECTOR rotvec;
    unsigned short unk;
    unsigned char ts_flags;
    unsigned char material;
};

extern int far abs(int);
extern int far polarAngle(int, int);
extern unsigned far polarRadius2D(int, int);
extern void far mat_rot_y(struct MATRIX*, int);
extern void far mat_rot_x(struct MATRIX*, int);
extern void far mat_rot_z(struct MATRIX*, int);
extern struct MATRIX mat_z_rot, mat_x_rot, mat_y_rot, mat_rot_temp;
extern unsigned mat_y_rot_angle;
extern void far mat_mul_vector(struct VECTOR*, struct MATRIX*, struct VECTOR*);
extern void far mat_multiply(struct MATRIX*, struct MATRIX*, struct MATRIX*);
extern void far mat_invert(struct MATRIX*, struct MATRIX*);
extern struct MATRIX* mat_rot_zxy(int, int, int, int);
extern void far __aFuldiv(void);
extern void far ported_get_a_poly_info_(void);
extern void far* mmgr_alloc_resbytes(const char*, long);
extern struct MATRIX mat_y0, mat_y100, mat_y200, mat_y300;
extern struct RECTANGLE select_rect_rc;
extern unsigned select_rect_param;
extern int* material_clrlist_ptr_cpy;
extern int* material_clrlist2_ptr_cpy;
extern int* material_clrlist2_ptr_cpy;
extern int* material_patlist_ptr_cpy;
extern int* material_patlist2_ptr_cpy;
extern unsigned short someZeroVideoConst;

/* Recovered seg006 data references; shared accepted owners ground signed words. */
extern int polyinfonumpolys;
extern int word_40ECE;
extern int word_31850;
extern int word_411F6;
extern int poly_linklist_40ED6_iter2;
extern unsigned transshapenumverts;
extern unsigned char far* transshapeprimitives;
extern struct VECTOR far* transshapeverts;
extern unsigned transshapenumpaints;
extern unsigned char transshapematerial;
extern unsigned char transshapeflags;
extern struct RECTANGLE* transshaperectptr;
extern struct MATRIX mat_temp;
extern long invpow2tbl[32];
extern unsigned char byte_4393D;
extern unsigned poly_linklist_40ED6_iter1;
extern unsigned poly_linklist_40ED6_iter3;
extern unsigned poly_linklist_40ED6_iter4;
extern unsigned char transshapenumvertscopy;
extern struct POINT2D* polyvertpointptrtab[];
extern unsigned char primidxcounttab[];
extern unsigned char primtypetab[];
extern char far* transshapeprimptr;
extern unsigned polyinfoptrnext;
extern char far* polyinfoptr;
extern char far* transshapepolyinfo;
extern struct POINT2D far* transshapepolyinfopts;
extern int far* polyinfoptrs[];
extern char transprimitivepaintjob;
extern unsigned char far* transshapeprimindexptr;
extern char backlights_paint_override;
extern int poly_linked_list_40ED6[];
extern long sin80, cos80;

void polyinfo_reset(void);
void calc_sincos80(void);
void init_polyinfo(void);
void get_a_poly_info(void);
void copy_material_list_pointers(void*, void*, void*, void*, unsigned short);
unsigned select_cliprect_rotate(int, int, int, struct RECTANGLE*, int);
unsigned transformed_shape_op(struct TRANSFORMEDSHAPE3D*);
unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned, unsigned);
unsigned rect_compare_point(struct POINT2D*);
int is_facing_camera(struct POINT2D far*);
void rect_adjust_from_point(struct POINT2D*, struct RECTANGLE*);
int vector_op_unk2(struct VECTOR*);



void copy_material_list_pointers(void* clrlist, void* clrlist2, void* patlist, void* patlist2, unsigned short videoConst)
{
	material_clrlist_ptr_cpy = clrlist;
	material_clrlist2_ptr_cpy = clrlist2;
	material_patlist_ptr_cpy = patlist;
	material_patlist2_ptr_cpy = patlist2;
	someZeroVideoConst = videoConst;
}
