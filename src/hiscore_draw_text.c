extern int word_6ae0, word_6ae2, word_6ae4, word_6ae6;
extern int word_a282;
extern int far font_op2(char *name);
extern void far font_set_unk(int color, int flags);
extern void far font_draw_text(char *text, int x, int y);

int * hiscore_draw_text(char *str, int x, int y, int color, int shadow)
{
    word_6ae4 = y - 1;
    word_6ae6 = y + word_a282 + 1;
    word_6ae0 = x - 1;
    word_6ae2 = x + font_op2(str) + 1;
    font_set_unk(shadow, 0);
    font_draw_text(str, x + 1, y + 1);
    font_draw_text(str, x - 1, y + 1);
    font_draw_text(str, x + 1, y - 1);
    font_draw_text(str, x - 1, y - 1);
    font_set_unk(color, 0);
    font_draw_text(str, x, y);
    return &word_6ae0;
}
