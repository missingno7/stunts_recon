struct SHAPE2D;
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_words[13];
};
extern struct SPRITE far sprite2;
struct SPRITE far *mcgawnd_window_sprite;
extern struct SPRITE far * far sprite_make_window(unsigned width, unsigned height, unsigned color);
extern void far sprite_setup1_from_arg_pointer(struct SPRITE far *argsprite);
extern void far sprputimage(struct SHAPE2D far *shape);

void far setup_mcgawnd1(void)
{
    if (mcgawnd_window_sprite == 0)
        mcgawnd_window_sprite = sprite_make_window(320, 200, 15);
    sprite_setup1_from_arg_pointer(&sprite2);
    sprputimage(mcgawnd_window_sprite->sprite_bitmapptr);
}

void far setup_mcgawnd2(void)
{
    if (mcgawnd_window_sprite == 0)
        mcgawnd_window_sprite = sprite_make_window(320, 200, 15);
    sprite_setup1_from_arg_pointer(mcgawnd_window_sprite);
}
