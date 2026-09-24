struct RECTANGLE { int left, right, top, bottom; };

int rect_intersect(struct RECTANGLE *r1, struct RECTANGLE *r2)
{
    if (r1->right < r1->left) return 1;
    if (r2->right <= r1->left) return 1;
    if (r1->right <= r2->left) return 1;
    if (r1->top >= r2->bottom) return 1;
    if (r1->bottom <= r2->top) return 1;

    if (r1->left < r2->left) r1->left = r2->left;
    if (r1->right > r2->right) r1->right = r2->right;
    if (r1->top < r2->top) r1->top = r2->top;
    if (r1->bottom > r2->bottom) r1->bottom = r2->bottom;
    return 0;
}
