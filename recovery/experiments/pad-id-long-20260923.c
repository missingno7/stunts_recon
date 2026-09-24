extern unsigned char word_42242[5];

unsigned char *pad_id(unsigned long far *id)
{
    int i;
    *(unsigned long *)word_42242 = *id;
    word_42242[4] = 0;
    for (i = 0; i < 4; ++i)
        if (word_42242[i] == 0) word_42242[i] = ' ';
    return word_42242;
}
