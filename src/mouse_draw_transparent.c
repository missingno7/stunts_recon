struct SHAPE2D { unsigned short words[6]; unsigned char bytes[4]; };
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; unsigned short words[3]; unsigned int *lineofs; unsigned short words2[9]; };
extern int mouse_xpos;
extern int mouse_ypos;
extern int video_flag2_is1;
extern char mouse_isdirty;
extern struct SPRITE far *mouseunkspriteptr;
extern struct SPRITE far *mmouspriteptr;
extern struct SPRITE far *smouspriteptr;
void far sprite_copy_both_to_arg(struct SPRITE *argsprite);
void far sprite_copy_2_to_1(void);
void far sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
void far sprite_putimage_and(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_putimage_or(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_copy_arg_to_both(struct SPRITE *argsprite);

void far mouse_draw_transparent(void)
{
    struct SPRITE saved_sprite[2];
    register int xpos;
    xpos = mouse_xpos;
    xpos -= xpos % video_flag2_is1;
    sprite_copy_both_to_arg(saved_sprite);
    sprite_copy_2_to_1();
    sprite_clear_shape_alt(mouseunkspriteptr->sprite_bitmapptr, xpos, mouse_ypos);
    sprite_putimage_and(mmouspriteptr->sprite_bitmapptr, mouse_xpos, mouse_ypos);
    sprite_putimage_or(smouspriteptr->sprite_bitmapptr, mouse_xpos, mouse_ypos);
    sprite_copy_arg_to_both(saved_sprite);
    mouse_isdirty = 1;
}
