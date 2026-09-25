struct VECTOR { short x, y, z; };
extern int far abs(int);
extern int far polarAngle(int, int);
extern unsigned far polarRadius2D(int, int);
extern void far __aFlmul(void);
extern long var_6114, var_6118;

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
