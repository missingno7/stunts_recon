struct RECTANGLE { int left, right, top, bottom; };
struct SPRITE { void far *image; };
extern unsigned char video_flag5_is0, byte_454A4, detail_level;
extern unsigned short slow_video_mgmt_copy;
extern int word_463D6, word_449FE;
extern struct RECTANGLE data_3554C[15], data_3595A[15], rect_array_unk3[15];
extern char byte_35520[15], rect_array_unk3_length;
extern int word_355D4[15];
extern struct SPRITE far *wndsprite;
extern void far sprite_copy_2_to_1_2(void);
extern void far sprite_set_1_size(int, int, int, int);
extern void far sprite_putimage(void far *);
extern void far mouse_draw_opaque_check(void), mouse_draw_transparent_check(void);
extern void far rectlist_add_rects(char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);
extern void far rect_array_sort_by_top(char, struct RECTANGLE *, int *);

void far sub_19F14(struct RECTANGLE *clipRect)
{
    register int i;
    struct RECTANGLE *currentRect;
    if (video_flag5_is0 != 0)
        return;
    sprite_copy_2_to_1_2();
    if (byte_454A4 == 0) {
        if (slow_video_mgmt_copy != 0) {
            for (i=0; i<15; ++i)
                byte_35520[i] = 3;
            if (detail_level == 4)
                word_449FE = word_463D6;
            if (word_449FE == word_463D6 &&
                data_3554C[5].left == data_3595A[5].left &&
                data_3554C[5].right == data_3595A[5].right &&
                data_3554C[5].top == data_3595A[5].top &&
                data_3554C[5].bottom == data_3595A[5].bottom)
                byte_35520[5] = 0;
            rect_array_unk3_length = 0;
            rectlist_add_rects(15, byte_35520, data_3554C,
                data_3595A, clipRect, &rect_array_unk3_length,
                rect_array_unk3);
            if (rect_array_unk3_length != 0) {
                rect_array_sort_by_top(rect_array_unk3_length,
                    rect_array_unk3, word_355D4);
                mouse_draw_opaque_check();
                i = 0;
                goto draw_rect_check;
                do {
                    currentRect = &rect_array_unk3[word_355D4[i]];
                    sprite_set_1_size(currentRect->left, currentRect->right,
                        currentRect->top, currentRect->bottom);
                    sprite_putimage(wndsprite->image);
                    ++i;
draw_rect_check:
                    ;
                } while (rect_array_unk3_length > i);
                goto draw_transparent;
            }
            sprite_set_1_size(0, 320, clipRect->top, clipRect->bottom);
        } else {
            sprite_set_1_size(clipRect->left, clipRect->right,
                clipRect->top, clipRect->bottom);
        }
    }
    mouse_draw_opaque_check();
    sprite_putimage(wndsprite->image);
draw_transparent:
    mouse_draw_transparent_check();
    if (slow_video_mgmt_copy != 0) {
        word_449FE = word_463D6;
        for (i=0; i<15; ++i)
            data_3595A[i] = data_3554C[i];
    }
}

