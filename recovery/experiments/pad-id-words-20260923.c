extern unsigned short word_42242;
extern unsigned short word_42244;
extern unsigned char byte_42246;

unsigned char *pad_id(unsigned short far *id)
{
    int i;
    word_42242 = id[0];
    word_42244 = id[1];
    byte_42246 = 0;
    for (i = 0; i < 4; ++i)
        if (((unsigned char *)&word_42242)[i] == 0)
            ((unsigned char *)&word_42242)[i] = ' ';
    return (unsigned char *)&word_42242;
}
