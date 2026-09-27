struct AudioChunk {
    unsigned char reserved00[0x28];
    unsigned char type28;
    unsigned char reserved29[0x1e];
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
extern struct AudioChunk audiochunks_unk[];
extern struct AudioVoice unk_45A26[];
extern unsigned char byte_459D2, byte_40634;
extern char far *word_3060A;
void far _loadds audio_unk2(int voiceIndex, unsigned char type)
{
    struct AudioChunk *audioChunk;
    int i;
    unsigned int typeCode;

    audioChunk = &audiochunks_unk[voiceIndex];
    audioChunk->type28 = type;
    if (byte_40634 == 0) {
        i = 0;
        if ((unsigned int)i < byte_459D2) {
            typeCode = type;
            do {
                if (unk_45A26[i].resourceIndex == voiceIndex)
                    ((void (far *)(int, struct AudioVoice *, int))
                        (word_3060A + 0x12))(i, &unk_45A26[i], typeCode);
                ++i;
            } while ((unsigned int)i < byte_459D2);
        }
    } else {
        ((void (far *)(int, struct AudioVoice *, int))
            (word_3060A + 0x12))(audioChunk->channelNumber, 0, type);
    }
}

