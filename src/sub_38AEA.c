struct AudioChunk {
    unsigned char reserved00[0x23];
    unsigned char resourceType, reserved24, modeValue;
    unsigned char reserved26[0x21];
    unsigned char channelNumber;
    unsigned char reserved48[4];
};
struct AudioVoice {
    unsigned char resourceIndex, active, note, reserved03[5];
    unsigned long position, remaining;
    char far *data;
    unsigned short sampleRate;
    unsigned char state16, reserved17;
    unsigned short value18, value1a, value1c, value1e, value20;
    unsigned char value22, reserved23;
    unsigned short value24;
    unsigned char value26, value27, value28, value29;
    struct AudioVoice *resource;
    unsigned char channelNumber, reserved2d;
};
typedef void (far *AudioCommand)(int, struct AudioVoice *, int, int);
extern struct AudioChunk audiochunks_unk[];
extern struct AudioVoice unk_45A26[];
extern unsigned char byte_459D2, byte_40634;
extern char far *word_3060A;
void far _loadds sub_38AEA(int voiceIndex, unsigned char type, int value)
{
    struct AudioChunk *chunk;
    int i;

    chunk = &audiochunks_unk[voiceIndex];
    if (type == 0x40)
        chunk->modeValue = value;
    if (byte_40634 != 0)
        ((AudioCommand)(word_3060A + 0x15))(chunk->channelNumber, 0, type, value);
    for (i = 0; i < byte_459D2; i++) {
        if (unk_45A26[i].resourceIndex == chunk->resourceType && byte_40634 == 0)
            ((AudioCommand)(word_3060A + 0x15))(i, &unk_45A26[i], type, value);
        if (type == 0x40 && value == 0 && unk_45A26[i].active == 2 && unk_45A26[i].resourceIndex == chunk->resourceType)
            unk_45A26[i].state16 = 4;
    }
}
