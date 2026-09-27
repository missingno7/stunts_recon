struct AudioChunk {
    unsigned char reserved00[0x1e];
    char far *data;
    unsigned char reserved22;
    unsigned char resourceType;
    unsigned char reserved24[4];
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
void far _loadds sub_38CF8(int voiceIndex, char far *voiceData)
{
    struct AudioChunk *chunk;
    int i;

    audiochunks_unk[voiceIndex].data = voiceData;
    if ((unsigned char)voiceData[0x43] < 0x10)
        audiochunks_unk[voiceIndex].channelNumber = voiceData[0x43];
    else
        audiochunks_unk[voiceIndex].channelNumber = (voiceIndex & 0x0f) + 1;
    if (byte_40634 == 0) {
        i = 0;
        if ((unsigned int)i < byte_459D2) {
            chunk = &audiochunks_unk[voiceIndex];
            do {
                if (unk_45A26[i].resourceIndex == voiceIndex)
                    ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, char far *))
                        (word_3060A + 0x21))(i, &unk_45A26[i], chunk, voiceData);
                ++i;
            } while ((unsigned int)i < byte_459D2);
        }
    } else {
        ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, char far *))
            (word_3060A + 0x21))(
                audiochunks_unk[voiceIndex].channelNumber, 0,
                &audiochunks_unk[voiceIndex], voiceData);
    }
}
