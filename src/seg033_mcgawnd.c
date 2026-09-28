/* READABILITY: Create the 320 by 200 window sprite and prepare the two MCGA window draw paths. */
struct SHAPE2D;
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_words[13];
};
extern struct SPRITE far sprite2;
struct SPRITE far *mcgawnd_window_sprite;
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern struct SPRITE far * far sprite_make_window(unsigned width, unsigned height, unsigned color);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern void far sprite_setup1_from_arg_pointer(struct SPRITE far *argsprite);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern void far sprputimage(struct SHAPE2D far *shape);

/* Create the MCGA window sprite if needed, select the scene sprite, and draw the window image.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): allocate and select the MCGA window sprite, then draw its image. */
void far setup_mcgawnd1(void)
{
    if (mcgawnd_window_sprite == 0)
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
        mcgawnd_window_sprite = sprite_make_window(320, 200, 15);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprite_setup1_from_arg_pointer(&sprite2);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprputimage(mcgawnd_window_sprite->sprite_bitmapptr);
}

/* Create the MCGA window sprite if needed and select it for subsequent drawing.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): allocate and select the MCGA window sprite. */
void far setup_mcgawnd2(void)
{
    if (mcgawnd_window_sprite == 0)
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
        mcgawnd_window_sprite = sprite_make_window(320, 200, 15);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprite_setup1_from_arg_pointer(mcgawnd_window_sprite);
}
