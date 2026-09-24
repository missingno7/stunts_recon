struct RECTANGLE { int left, right, top, bottom; };
extern short video_flag2_is1;
extern short video_flag3_isFFFF;
void rect_union(struct RECTANGLE *r1, struct RECTANGLE *r2, struct RECTANGLE *out)
{
    out->left = r1->left <= r2->left ? r1->left : r2->left;
    out->right = r1->right >= r2->right ? r1->right : r2->right;
    out->top = r1->top <= r2->top ? r1->top : r2->top;
    out->bottom = r1->bottom >= r2->bottom ? r1->bottom : r2->bottom;
    if (video_flag2_is1 != 1) {
        out->right = (out->right + video_flag2_is1 - 1) & video_flag3_isFFFF;
    }
}
