struct POINT2D { int px, py; };
struct RECTANGLE { int left, right, top, bottom; };
void far rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc)
{
    int temp;
    if (rc->left > pt->px) rc->left = pt->px;
    temp = pt->px + 1;
    if (rc->right < temp) rc->right = temp;
    if (rc->top > pt->py) rc->top = pt->py;
    temp = pt->py + 1;
    if (rc->bottom < temp) rc->bottom = temp;
}
