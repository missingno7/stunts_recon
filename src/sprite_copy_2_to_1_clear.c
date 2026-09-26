struct SHAPE2D;
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; unsigned short words[3]; unsigned int *lineofs; unsigned short words2[9]; };
extern struct SPRITE far sprite2;
void sprite_set_1_from_argptr(struct SPRITE far *argsprite);
void sprite_clear_1_color(unsigned char color);
void sprite_clear_1_color(unsigned char color);void sprite_copy_2_to_1_clear(void) { sprite_set_1_from_argptr(&sprite2); sprite_clear_1_color(0); }
