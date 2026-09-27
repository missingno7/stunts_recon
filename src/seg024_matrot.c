struct MAT3 { int _11, _21, _31, _12, _22, _32, _13, _23, _33; };
struct MATRIX { struct MAT3 m; };
extern short cos_fast(int);
extern short sin_fast(int);

void mat_rot_x(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cos_fast(angle);
    s = sin_fast(angle);
    outmat->m._11 = 0x4000; outmat->m._21 = 0;  outmat->m._31 = 0;
    outmat->m._12 = 0;      outmat->m._22 = c;  outmat->m._32 = s;
    outmat->m._13 = 0;      outmat->m._23 = -s; outmat->m._33 = c;
}

void mat_rot_y(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cos_fast(angle);
    s = sin_fast(angle);
    outmat->m._11 = c;  outmat->m._21 = 0;      outmat->m._31 = -s;
    outmat->m._12 = 0;  outmat->m._22 = 0x4000; outmat->m._32 = 0;
    outmat->m._13 = s;  outmat->m._23 = 0;      outmat->m._33 = c;
}

void mat_rot_z(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cos_fast(angle);
    s = sin_fast(angle);
    outmat->m._11 = c;  outmat->m._21 = s; outmat->m._31 = 0;
    outmat->m._12 = -s; outmat->m._22 = c; outmat->m._32 = 0;
    outmat->m._13 = 0;  outmat->m._23 = 0; outmat->m._33 = 0x4000;
}
