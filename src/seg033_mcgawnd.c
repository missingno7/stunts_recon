struct SHAPE2D;
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_words[13];
};
extern struct SPRITE far sprite2;
extern struct SPRITE far *mcgawndsprite;
extern struct SPRITE far * far sprite_make_wnd(unsigned width, unsigned height, unsigned color);
extern void far sprite_set_1_from_argptr(struct SPRITE far *argsprite);
extern void far sprite_putimage(struct SHAPE2D far *shape);

void far setup_mcgawnd1(void)
{
    if (mcgawndsprite == 0)
        mcgawndsprite = sprite_make_wnd(320, 200, 15);
    sprite_set_1_from_argptr(&sprite2);
    sprite_putimage(mcgawndsprite->sprite_bitmapptr);
}

void far setup_mcgawnd2(void)
{
    if (mcgawndsprite == 0)
        mcgawndsprite = sprite_make_wnd(320, 200, 15);
    sprite_set_1_from_argptr(mcgawndsprite);
}
