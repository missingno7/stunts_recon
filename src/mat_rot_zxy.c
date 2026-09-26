struct MATRIX { int vals[9]; };
extern void far mat_rot_z(struct MATRIX*, int);
extern void far mat_rot_x(struct MATRIX*, int);
extern void far mat_rot_y(struct MATRIX*, int);
extern void far mat_multiply(struct MATRIX*, struct MATRIX*, struct MATRIX*);
extern struct MATRIX mat_y0, mat_y100, mat_y200, mat_y300;
extern struct MATRIX mat_y_rot, mat_x_rot, mat_z_rot, mat_rot_temp;
extern unsigned mat_y_rot_angle;

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
