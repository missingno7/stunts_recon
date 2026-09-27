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
struct AudioSample {
    unsigned char reserved00[0x1e];
    short level1e, level20, level22, level24, level26;
    unsigned char loopMode, loopCount;
    unsigned char reserved2a[4];
    unsigned short limit2e;
    unsigned char reserved30[4];
    unsigned char loopFlags, pulsePresent;
    unsigned char reserved36[4];
    unsigned char pulseCount;
    unsigned char pulseTable[8];
};
struct AudioChunkCounters {
    unsigned char reserved00[0x15];
    unsigned char activeChannels;
    unsigned char reserved16[0x36];
};
struct AudioChunkChannel {
    unsigned char reserved00[0x25];
    unsigned char state;
    unsigned char reserved26[0x26];
};
union AudioChunk {
    struct AudioChunkCounters counters;
    struct AudioChunkChannel channel;
};
typedef void (far *AudioCommand)(unsigned int, struct AudioVoice *);
typedef void (far *AudioUpdate)(unsigned int, struct AudioVoice *, struct AudioVoice *, char far *);
extern union AudioChunk audiochunks_unk[];
extern struct AudioVoice unk_45A26[];
extern unsigned char byte_459D2, byte_44ACA[];
extern char far *word_3060A;
void far _loadds sub_3968A(struct AudioVoice *voice);

void far _loadds sub_3963C(void)
{
    unsigned int i;
    i = 0;
    if ((unsigned int)byte_459D2 != 0) {
        do {
            struct AudioVoice *voice;
            voice = &unk_45A26[i];
            if (voice->active != 0 && voice->resourceIndex < 16)
                sub_3968A(voice);
            ++i;
        } while (i < byte_459D2);
    }
}

void far _loadds sub_3968A(struct AudioVoice *voice)
{
    voice->position++;
    if (voice->remaining == 0) {
        ((void (far *)(int, struct AudioVoice *))(word_3060A + 0x0c))(voice->channelNumber, voice);
        voice->active = 2;
        if (audiochunks_unk[voice->resourceIndex].channel.state != 0) {
            voice->state16 = 3;
            return;
        }
        voice->state16 = 4;
        return;
    }
    voice->remaining--;
}

void far _loadds sub_39700(void)
{
    int i;
    for (i = 0; (unsigned int)i < byte_459D2; ++i) {
        struct AudioVoice *voice;
        struct AudioSample far *sample;
        voice = &unk_45A26[i];
        if (voice->active == 0)
            continue;
        if (voice->resourceIndex > 15)
            sub_3968A(voice);
        sample = (struct AudioSample far *)voice->data;

        if (voice->state16 == 1) {
            voice->sampleRate += sample->level20;
            if (voice->sampleRate >= sample->level1e) {
                voice->sampleRate = sample->level1e;
                if (sample->level24 >= sample->level1e)
                    voice->state16 = 3;
                else
                    voice->state16 = 2;
            }
        }
        if (voice->state16 == 2) {
            voice->sampleRate -= sample->level22;
            if (voice->sampleRate <= sample->level24) {
                voice->state16 = 3;
                voice->sampleRate = sample->level24;
            }
        }
        if (voice->state16 == 3 && sample->level24 == 0)
            voice->state16 = 4;
        if (voice->state16 == 4) {
            voice->sampleRate -= sample->level26;
            if (voice->sampleRate <= 0) {
                voice->sampleRate = 0;
                voice->state16 = 0;
                voice->active = 0;
                --audiochunks_unk[voice->resourceIndex].counters.activeChannels;
                ((AudioCommand)(word_3060A + 0x0f))(voice->channelNumber, voice);
                byte_44ACA[voice->resourceIndex] = 0;
            }
        }

        if (sample->loopMode != 0) {
            if (voice->value18 != 0) {
                --voice->value18;
            } else if (voice->value1a != 0) {
                if (voice->value1a != 0x7fff)
                    --voice->value1a;
                if (voice->value27 == 0) {
                    voice->value27 = sample->loopCount;
                    if (voice->value26 == 2) {
                        voice->value1c -= voice->value24;
                        if ((abs(voice->value1c)) >= sample->limit2e) {
                            if (sample->loopFlags & 1)
                                voice->value26 = 1;
                            else
                                voice->value1c = 0;
                        }
                    } else {
                        voice->value1c += voice->value24;
                        if ((abs(voice->value1c)) >= sample->limit2e) {
                            if (sample->loopFlags & 2)
                                voice->value26 = 2;
                            else
                                voice->value1c = 0;
                        }
                    }
                } else {
                    --voice->value27;
                }
            }
        }

        if (sample->pulsePresent != 0) {
            if (voice->value1e != 0) {
                --voice->value1e;
            } else if (voice->value20 != 0) {
                --voice->value20;
                if (voice->value28 != 0) {
                    --voice->value28;
                    goto pulseUpdateDone;
                }
                voice->value28 = sample->pulseCount;
                voice->value22 = sample->pulseTable[voice->value29++ & 7];
            }
        }
pulseUpdateDone:
        ((AudioUpdate)(word_3060A + 0x27))(voice->channelNumber, voice, voice->resource, voice->data);
    }
    ((void (far *)(struct AudioVoice *))(word_3060A + 0x30))(unk_45A26);
}

