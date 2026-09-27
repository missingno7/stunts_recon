struct RECTANGLE { int left, right, top, bottom; };
extern unsigned short slow_video_mgmt_copy;
extern struct RECTANGLE data_3554C[15];
extern struct RECTANGLE data_3595A[15];
extern struct RECTANGLE rect_unk5;
extern struct RECTANGLE cliprect_unk;

void far init_rect_arrays(void)
{
    register int i;

    if (slow_video_mgmt_copy != 0) {
        data_3554C[0] = rect_unk5;
        data_3595A[0] = rect_unk5;
        for (i = 1; i < 15; ++i) {
            data_3554C[i] = cliprect_unk;
            data_3595A[i] = cliprect_unk;
        }
    }
}
