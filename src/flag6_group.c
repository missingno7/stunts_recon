struct AUDIOCHUNK { int unk0; int unk2; char unk4[36]; unsigned char program; char unk41[35]; };
extern unsigned char audioflag6;
extern unsigned char byte_428D6[];
extern struct AUDIOCHUNK audiochunks_unk2[];
extern void audio_unk2(int, unsigned char);

void audio_enable_flag6(void)
{
    int i;
    if (audioflag6 != 1) {
        for (i = 16; i < 24; i++)
            audio_unk2(i, byte_428D6[i]);
        audioflag6 = 1;
    }
}

void audio_disable_flag6(void)
{
    int i;
    if (audioflag6 != 0) {
        for (i = 16; i < 24; i++) {
            byte_428D6[i] = audiochunks_unk2[i - 16].program;
            audio_unk2(i, 0);
        }
        audioflag6 = 0;
    }
}

int audio_toggle_flag6(void)
{
    if (audioflag6 == 1) {
        audio_disable_flag6();
        return 0;
    }
    audio_enable_flag6();
    return 1;
}
