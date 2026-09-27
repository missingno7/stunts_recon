struct sample_bank {
    char unk0[0x2e];
    long far *offsets;
};

long far _loadds sub_38AC4(unsigned char index, struct sample_bank *bank)
{
    return bank->offsets[index];
}
