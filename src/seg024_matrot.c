/* READABILITY: Write fixed-point X, Y, and Z rotation matrices using the fast sine and cosine helpers. */
struct MAT3 { int _11, _21, _31, _12, _22, _32, _13, _23, _33; };
struct MATRIX { struct MAT3 m; };
extern short cosfast(int);
extern short sinfast(int);

/* Write a fixed-point X-axis rotation matrix.
 * Params and return follow the declared C signature. */
void mat_rot_x(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cosfast(angle);
    s = sinfast(angle);
    outmat->m._11 = 0x4000; outmat->m._21 = 0;  outmat->m._31 = 0;
    outmat->m._12 = 0;      outmat->m._22 = c;  outmat->m._32 = s;
    outmat->m._13 = 0;      outmat->m._23 = -s; outmat->m._33 = c;
}

/* Write a fixed-point Y-axis rotation matrix.
 * Params and return follow the declared C signature. */
void matroty(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cosfast(angle);
    s = sinfast(angle);
    outmat->m._11 = c;  outmat->m._21 = 0;      outmat->m._31 = -s;
    outmat->m._12 = 0;  outmat->m._22 = 0x4000; outmat->m._32 = 0;
    outmat->m._13 = s;  outmat->m._23 = 0;      outmat->m._33 = c;
}

/* Write a fixed-point Z-axis rotation matrix.
 * Params and return follow the declared C signature. */
void mat_rot_z(struct MATRIX *outmat, int angle)
{
    int s, c;
    c = cosfast(angle);
    s = sinfast(angle);
    outmat->m._11 = c;  outmat->m._21 = s; outmat->m._31 = 0;
    outmat->m._12 = -s; outmat->m._22 = c; outmat->m._32 = 0;
    outmat->m._13 = 0;  outmat->m._23 = 0; outmat->m._33 = 0x4000;
}
