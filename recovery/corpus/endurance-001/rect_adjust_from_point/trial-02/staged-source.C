struct POINT2D { int px, py; };
struct RECTANGLE { int left, right, top, bottom; };
void far rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc)
{
    int px = pt->px;
    int py = pt->py;
    int temp;
    if (rc->left > px) rc->left = px;
    temp = px + 1;
    if (rc->right < temp) rc->right = temp;
    if (rc->top > py) rc->top = py;
    temp = py + 1;
    if (rc->bottom < temp) rc->bottom = temp;
}
