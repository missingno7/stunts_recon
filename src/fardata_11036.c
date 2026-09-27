struct VECTOR { int x, y, z; };
struct MATRIX { int m[9]; };
struct PLANE {
    int plane_yz;
    int plane_xy;
    struct VECTOR plane_origin;
    struct VECTOR plane_normal;
    struct MATRIX plane_rotation;
};
struct PLANE far plan_memres = { 0, 0, { 0, 0, 0 }, { 0, 8192, 0 } };
int far unk_3B1E2[7] = { 0 };
