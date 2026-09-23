struct RECTANGLE {int left,right,top,bottom;};
int rect_is_overlapping(struct RECTANGLE* r1, struct RECTANGLE* r2) {
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

struct R {int left,right,top,bottom;};
int rect_is_inside(struct R *a,struct R *b){return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}
