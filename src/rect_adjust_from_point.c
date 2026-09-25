struct RECTANGLE { int left, right, top, bottom; };
struct POINT2D { int px, py; };
void rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc) {
    register int x, y;
    x=pt->px; y=pt->py;
    if(rc->left>x) rc->left=x;
    { int temp=x+1; if(rc->right<temp) rc->right=temp; }
    if(rc->top>y) rc->top=y;
    { int temp=y+1; if(rc->bottom<temp) rc->bottom=temp; }
}
