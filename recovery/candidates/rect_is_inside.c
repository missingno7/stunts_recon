struct R {int left,right,top,bottom;};
int rect_is_inside(struct R *a,struct R *b){return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}
