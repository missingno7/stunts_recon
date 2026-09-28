#include "stunts_types.h"

#include "platform_hw.h"
/* READABILITY: Create the 320 by 200 window sprite and prepare the two MCGA window draw paths. */
struct SHAPE2D;
struct SPRITE {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
};
extern struct SPRITE FAR sprite2;
struct SPRITE far *mcgawnd_window_sprite;
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern struct SPRITE FAR * FAR sprite_make_window(U16 width, U16 height, U16 color);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern void FAR sprite_setup1_from_arg_pointer(struct SPRITE FAR *argsprite);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
extern void FAR sprputimage(struct SHAPE2D FAR *shape);

/* Create the MCGA window sprite if needed, select the scene sprite, and draw the window image.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): allocate and select the MCGA window sprite, then draw its image. */
void FAR setup_mcgawnd1(void)
{
    if (mcgawnd_window_sprite == 0)
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
        mcgawnd_window_sprite = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, PLATFORM_VGA_COLOR_WHITE);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprite_setup1_from_arg_pointer(&sprite2);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprputimage(mcgawnd_window_sprite->sprite_bitmapptr);
}

/* Create the MCGA window sprite if needed and select it for subsequent drawing.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): allocate and select the MCGA window sprite. */
void FAR setup_mcgawnd2(void)
{
    if (mcgawnd_window_sprite == 0)
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
        mcgawnd_window_sprite = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, PLATFORM_VGA_COLOR_WHITE);
    /* PLATFORM(video): create, select, or draw the MCGA window sprite. */
    sprite_setup1_from_arg_pointer(mcgawnd_window_sprite);
}
