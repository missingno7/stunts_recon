struct SPRITE { unsigned short words[15]; };
extern struct SPRITE far sprite2;
void sprite_set_1_from_argptr(struct SPRITE far *argsprite);
void far sprite_copy_2_to_1_2(void)
{
    sprite_set_1_from_argptr(&sprite2);
}
