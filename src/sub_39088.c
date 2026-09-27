struct AudioVoice {
    unsigned char resourceIndex, active, note, reserved03[5];
    unsigned long position, remaining;
    char far *data;
    short sampleRate;
    unsigned char state16, reserved17;
    unsigned short value18, value1a;
    short value1c;
    unsigned short value1e, value20;
    unsigned char value22, reserved23;
    unsigned short value24;
    unsigned char value26, value27, value28, value29;
    struct AudioVoice *resource;
    unsigned char channelNumber, reserved2d;
};
extern struct AudioVoice unk_45A26[];
extern char far *word_3060A;
void far _loadds sub_39088(int voiceNum, int value)
{
    ((void (far *)(int, struct AudioVoice *, int))(word_3060A + 0x24))(
        unk_45A26[voiceNum].channelNumber, &unk_45A26[voiceNum], value);
}
