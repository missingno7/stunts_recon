extern int word_6ad8, word_6ada, word_6adc, word_6ade, word_a282;
extern int far font_op2(char *name);
int * intro_draw_text(char *str, int x, int y, int color, int shadow)
{
    extern int word_6ad8, word_6ada, word_6adc, word_6ade;
    extern int word_a282;
    extern void far font_set_unk(int color, int flags);
    extern void far font_draw_text(char *text, int x, int y);

    word_6adc = y;
    word_6ade = y + word_a282 + 1;
    word_6ad8 = x;
    word_6ada = x + font_op2(str) + 1;
    font_set_unk(shadow, 0);
    font_draw_text(str, x + 1, y + 1);
    font_set_unk(color, 0);
    font_draw_text(str, x, y);
    return &word_6ad8;
}