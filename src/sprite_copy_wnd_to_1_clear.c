struct SPRITE { unsigned short words[15]; };
extern struct SPRITE far sprite2;
extern struct SPRITE far *wndsprite;
void sprite_set_1_from_argptr(struct SPRITE far *argsprite);
void sprite_clear_1_color(unsigned char color);void sprite_copy_wnd_to_1_clear(void) { sprite_set_1_from_argptr(wndsprite); sprite_clear_1_color(0); }
