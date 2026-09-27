struct AudioVoice { unsigned char resourceIndex,active,note,reserved03[5]; unsigned long position,remaining; char far *data; unsigned short sampleRate; unsigned char state16,reserved17; unsigned short value18,value1a,value1c,value1e,value20; unsigned char value22,reserved23; unsigned short value24; unsigned char value26,value27,value28,value29; struct AudioVoice *resource; unsigned char channelNumber,reserved2d; };
struct AudioChannel { unsigned char reserved00[0x25]; unsigned char state; unsigned char reserved26[0x26]; };
extern struct AudioChannel audiochunks_unk[]; extern struct AudioVoice unk_45A26[]; extern unsigned char byte_459D2; extern char far *word_3060A;
void far _loadds sub_3968A(struct AudioVoice *voice);
void far _loadds sub_3963C(void) { unsigned int i; i=0; if(((unsigned int)byte_459D2 != 0)) do { struct AudioVoice *voice; voice=&unk_45A26[i]; if(voice->active!=0 && voice->resourceIndex<16) sub_3968A(voice); ++i; } while(i<byte_459D2); }
void far _loadds sub_3968A(struct AudioVoice *voice) {
    voice->position++;
    if (voice->remaining == 0) {
        ((void (far *)(int, struct AudioVoice *))(word_3060A + 0x0c))(voice->channelNumber, voice);
        voice->active = 2;
        if (audiochunks_unk[voice->resourceIndex].state != 0) {
            voice->state16 = 3;
            return;
        }
        voice->state16 = 4;
        return;
    }
    voice->remaining--;
}

