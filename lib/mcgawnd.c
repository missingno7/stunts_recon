#include "types.h"
#include "input.h"
/* Create the 320 by 200 window sprite and prepare the two MCGA window draw paths. */
struct SHAPE2D;
/* Local view: SPRITE; thirteen-word suffix differs from the shared fields and near line-offset pointer. */
struct SPRITE {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
};
#include "video.h"

struct SPRITE far *mcgawnd_window_sprite;


/* Create the MCGA window sprite if needed, select the scene sprite, and draw the window image.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setup_mcgawnd1(void)
{
    if (mcgawnd_window_sprite == 0)
        mcgawnd_window_sprite = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, PLATFORM_VGA_COLOR_WHITE);
    sprite_setup1_from_arg_pointer(&sprite2);
    movcf(mcgawnd_window_sprite->sprite_bitmapptr);
}

/* Create the MCGA window sprite if needed and select it for subsequent drawing.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setup_mcgawnd2(void)
{
    if (mcgawnd_window_sprite == 0)
        mcgawnd_window_sprite = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, PLATFORM_VGA_COLOR_WHITE);
    sprite_setup1_from_arg_pointer(mcgawnd_window_sprite);
}
