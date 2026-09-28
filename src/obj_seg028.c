struct AudioEvent {
    unsigned long delta;
    unsigned char command;
    unsigned char param;
    unsigned long value;
    unsigned char length;
};

struct AudioChunk {
    char far *pos;
    unsigned char depth;
    char far *stack[4];
    unsigned char activeVoices;
    unsigned char maxVoices;
    unsigned char reserved17;
    unsigned long delay;
    unsigned char reserved1c[2];
    char far *data;
    unsigned char velocity;
    unsigned char resourceType;
    unsigned char note;
    unsigned char modeValue;
    unsigned short value26;
    unsigned char program;
    unsigned char reserved29[5];
    char far *far *samples;
    unsigned char loopDepth;
    char far *loopPos[4];
    unsigned char loopCount[4];
    unsigned char channelNumber;
    void (far *callback)(int);
};

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
    struct AudioChunk *resource;
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

extern char far *audiodriverbinary;
extern unsigned short audio_update_lock;
extern unsigned short audio_timer_reentry_lock;
extern unsigned char audio_song_ready;
extern unsigned char audioflag2;
extern unsigned char audio_pause_in_progress;
unsigned short snd_sample_rate_phase;
unsigned short mus_samplelimit;
extern unsigned char block_audio_num;
extern struct AudioChunk audiochunktable[];
extern struct AudioVoice snd_voices_tbl[];
unsigned char g_audiodrvvoices_count;
extern unsigned char audio_driver_mode;
extern unsigned char audioblock[];
extern unsigned char g_audchnkvalue[];
extern unsigned char g_musicvolumesetting;
static unsigned char audio_resource_buffer[260];
static struct AudioEvent audio_event_read_buffer;
static struct AudioEvent audioevent;
static struct AudioEvent audio_event_send_buffer;
extern unsigned short audio_bit_masks[];
extern char far *kick_res;
extern char far *g_snaresnd;
extern char far *ride_audio_sound_res;
extern char far *audio_opp_res;
extern char far *chhtsample;
extern char far *resource_sound_hit;
extern char far *tommsampleresource;
extern unsigned long far audioresource_get_dword(unsigned char far *);
extern unsigned short far audioresource_get_word(unsigned char far *);
extern void far audio_init_chunk(int, int, int, int, int, int, int);

void far _loadds reset_audio_event_state(void);
void far _loadds process_music_audio_chunks(void);
void far _loadds update_audio_voice_state(void);
void far _loadds process_effect_audio_chunks(void);
void far _loadds process_audio_chunk_event(int chunkIndex);
char far * far _loadds find_audio_chunk_data(unsigned char index, struct AudioChunk *chunk);
void far _loadds apply_audio_voice_event(int voiceIndex, unsigned char type, int value);
void far _loadds set_audio_voice_parameter(int voiceIndex, int value);
void far _loadds audio_unk2(int voiceIndex, unsigned char type);
int far _loadds process_audio_event(struct AudioEvent *event, int chunkIndex);
int far _loadds select_audio_voice_slot(char far *sample, struct AudioChunk *chunk);
void far _loadds read_audio_event(struct AudioEvent *event, unsigned char far *stream);
void far _loadds clear_audio_voice(struct AudioVoice *voice);
void far _loadds audio_driver_func1E(int first, int last);

void far _loadds audiodriver_timer(void)
{
    if (audiodriverbinary == (char far *)0)
        return;
    if (audio_update_lock != 0)
        return;
    if (audio_timer_reentry_lock != 0)
        return;

    audio_timer_reentry_lock++;
    reset_audio_event_state();
    if (audio_song_ready == 1 && audioflag2 == 1 && audio_pause_in_progress == 0)
        process_music_audio_chunks();
    else
        update_audio_voice_state();
    process_effect_audio_chunks();
    audio_timer_reentry_lock--;
}

void far _loadds process_music_audio_chunks(void)
{
    unsigned char chunkIndex;

    snd_sample_rate_phase += 0x80;
    while (snd_sample_rate_phase >= mus_samplelimit) {
        update_audio_voice_state();
        snd_sample_rate_phase -= mus_samplelimit;
        for (chunkIndex = 0; chunkIndex < block_audio_num; chunkIndex++)
            process_audio_chunk_event(chunkIndex);
    }
}

void far _loadds process_effect_audio_chunks(void)
{
    unsigned char chunkIndex;

    for (chunkIndex = 0x10; chunkIndex < 0x17; chunkIndex++)
        process_audio_chunk_event(chunkIndex);
}

void far _loadds process_audio_chunk_event(int chunkIndex)
{
    unsigned char param;
    struct AudioChunk *chunk;
    void (far *callback)(int);

    param = 0;
    chunk = &audiochunktable[chunkIndex];
    if (chunk->delay == 0) {
        if (chunk->pos == 0)
            return;
        while (chunk->delay == 0 && chunk->pos != 0) {
            read_audio_event(&audioevent, chunk->pos);
            chunk->pos += audioevent.length;
            if (audioevent.command >= 0xd9) {
                param = audioevent.param;
                switch (audioevent.command) {
                case 0xd9:
                    if (chunk->depth != 0) {
                        chunk->pos = chunk->stack[chunk->depth];
                        chunk->depth--;
                        break;
                    }
                    callback = chunk->callback;
                    chunk->pos = 0;
                    audio_driver_func1E(chunkIndex, chunkIndex);
                    if (callback != 0)
                        callback(chunkIndex);
                    break;
                case 0xe8:
                    ((void (far *)(int, unsigned char *))(audiodriverbinary + 0x39))(
                        audioevent.length - 4, audio_resource_buffer);
                    break;
                case 0xea:
                    g_audchnkvalue[chunkIndex] = param;
                    break;
                case 0xe6:
                    chunk->depth++;
                    chunk->stack[chunk->depth] = chunk->pos;
                    chunk->pos = (char far *)audioevent.value + 4;
                    break;
                case 0xda:
                    chunk->pos = 0;
                    callback = chunk->callback;
                    audio_driver_func1E(chunkIndex, chunkIndex);
                    audio_init_chunk(chunkIndex, chunkIndex, 0, 0, 0, g_musicvolumesetting, 0);
                    if (callback != 0)
                        callback(chunkIndex);
                    break;
                case 0xdb:
                    chunk->depth = 0;
                    chunk->loopDepth = 0;
                    chunk->pos = chunk->stack[0];
                    break;
                case 0xdc:
                    chunk->data = find_audio_chunk_data(param, chunk);
                    if (audio_driver_mode != 0) {
                        if ((unsigned char)chunk->data[0x43] < 0x10)
                            chunk->channelNumber = chunk->data[0x43];
                        else
                            chunk->channelNumber = (chunkIndex & 0x0f) + 1;
                        ((void (far *)(int, int, int))(audiodriverbinary + 0x12))(
                            (unsigned char)chunk->channelNumber, 0, chunk->program);
                        ((void (far *)(int, int, struct AudioChunk *, char far *))(audiodriverbinary + 0x21))(
                            chunk->channelNumber, 0, chunk, chunk->data);
                    }
                    break;
                case 0xdd:
                    if (chunkIndex < 0x10)
                        mus_samplelimit = 32000U / param;
                    break;
                case 0xde:
                    audio_unk2(chunkIndex, param);
                    break;
                case 0xdf:
                    apply_audio_voice_event(chunkIndex, param, (int)audioevent.value);
                    break;
                case 0xe0:
                    chunk->maxVoices = param;
                    break;
                case 0xe1:
                    chunk->note = param;
                    break;
                case 0xe2:
                    chunk->loopPos[chunk->loopDepth] = chunk->pos;
                    chunk->loopCount[chunk->loopDepth] = param - 1;
                    chunk->loopDepth++;
                    break;
                case 0xe3:
                    if (chunk->loopDepth != 0) {
                        chunk->pos = chunk->loopPos[chunk->loopDepth - 1];
                        if (chunk->loopCount[chunk->loopDepth - 1]-- == 0)
                            chunk->loopDepth--;
                    }
                    break;
                case 0xe4:
                    chunk->velocity = param;
                    break;
                case 0xe9:
                    chunk->channelNumber = param;
                    break;
                case 0xe5:
                    set_audio_voice_parameter(chunkIndex, (int)audioevent.value);
                    break;
                }
            } else {
                if (audioevent.command < 0x80)
                    audioevent.param = chunk->velocity;
                audioevent.command &= 0x7f;
                process_audio_event(&audioevent, chunkIndex);
            }
            if (chunk->pos != 0) {
                read_audio_event(&audio_event_read_buffer, chunk->pos);
                chunk->delay = audio_event_read_buffer.delta;
            }
        }
    }
    chunk->delay--;
}

static void far _loadds set_chunk_channel(int chunkIndex, unsigned char channel)
{
    audiochunktable[chunkIndex].channelNumber = channel;
}

char far * far _loadds find_audio_chunk_data(unsigned char index, struct AudioChunk *chunk)
{
    return chunk->samples[index];
}

void far _loadds apply_audio_voice_event(int voiceIndex, unsigned char type, int value)
{
    struct AudioChunk *chunk;
    int i;

    chunk = &audiochunktable[voiceIndex];
    if (type == 0x40)
        chunk->modeValue = value;
    if (audio_driver_mode != 0)
        ((void (far *)(int, struct AudioVoice *, int, int))(audiodriverbinary + 0x15))(chunk->channelNumber, 0, type, value);
    for (i = 0; i < g_audiodrvvoices_count; i++) {
        if (snd_voices_tbl[i].resourceIndex == chunk->resourceType && audio_driver_mode == 0)
            ((void (far *)(int, struct AudioVoice *, int, int))(audiodriverbinary + 0x15))(i, &snd_voices_tbl[i], type, value);
        if (type == 0x40 && value == 0 && snd_voices_tbl[i].active == 2 && snd_voices_tbl[i].resourceIndex == chunk->resourceType)
            snd_voices_tbl[i].state16 = 4;
    }
}

void far _loadds set_audio_voice_parameter(int voiceIndex, int value)
{
    struct AudioChunk *voice;

    voice = &audiochunktable[voiceIndex];
    if (value & 0x100)
        value |= 0x80;
    value = ((value & 0xff00) >> 1) + ((signed char)value - 0x2000);
    voice->value26 = value;
    ((void (far *)(struct AudioChunk *, int, int))(audiodriverbinary + 0x1b))(voice, value, voice->channelNumber);
}

void far _loadds audio_unk2(int voiceIndex, unsigned char type)
{
    struct AudioChunk *audioChunk;
    int i;
    unsigned int typeCode;

    audioChunk = &audiochunktable[voiceIndex];
    audioChunk->program = type;
    if (audio_driver_mode == 0) {
        i = 0;
        if ((unsigned int)i < g_audiodrvvoices_count) {
            typeCode = type;
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (far *)(int, struct AudioVoice *, int))
                        (audiodriverbinary + 0x12))(i, &snd_voices_tbl[i], typeCode);
                ++i;
            } while ((unsigned int)i < g_audiodrvvoices_count);
        }
    } else {
        ((void (far *)(int, struct AudioVoice *, int))
            (audiodriverbinary + 0x12))(audioChunk->channelNumber, 0, type);
    }
}

void far _loadds start_audio_voice_sample(int voiceIndex, char far *voiceData)
{
    struct AudioChunk *chunk;
    int i;

    audiochunktable[voiceIndex].data = voiceData;
    if ((unsigned char)voiceData[0x43] < 0x10)
        audiochunktable[voiceIndex].channelNumber = voiceData[0x43];
    else
        audiochunktable[voiceIndex].channelNumber = (voiceIndex & 0x0f) + 1;
    if (audio_driver_mode == 0) {
        i = 0;
        if ((unsigned int)i < g_audiodrvvoices_count) {
            chunk = &audiochunktable[voiceIndex];
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, char far *))
                        (audiodriverbinary + 0x21))(i, &snd_voices_tbl[i], chunk, voiceData);
                ++i;
            } while ((unsigned int)i < g_audiodrvvoices_count);
        }
    } else {
        ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, char far *))
            (audiodriverbinary + 0x21))(
                audiochunktable[voiceIndex].channelNumber, 0,
                &audiochunktable[voiceIndex], voiceData);
    }
}

int far _loadds process_audio_event(struct AudioEvent *event, int chunkIndex)
{
    struct AudioChunk *resource;
    char far *sample;
    int voiceNum;
    struct AudioVoice *channel;

    resource = &audiochunktable[chunkIndex];
    sample = resource->data;
    if (sample[5] == 5) {
        switch (event->command) {
        case 0x18:
            sample = kick_res;
            break;
        case 0x1a:
            sample = g_snaresnd;
            break;
        case 0x25:
            sample = ride_audio_sound_res;
            break;
        case 0x27:
            sample = audio_opp_res;
            break;
        case 0x1e:
            sample = chhtsample;
            break;
        case 0x20:
        case 0x22:
            sample = resource_sound_hit;
            break;
        default:
            sample = tommsampleresource;
            break;
        }
    }
    if (sample == 0)
        return -1;

    voiceNum = select_audio_voice_slot(sample, resource);
    if (voiceNum == -1)
        return -1;

    channel = &snd_voices_tbl[voiceNum];
    if (channel->data != sample) {
        channel->data = sample;
        if (audio_driver_mode == 0)
            ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, char far *))
                (audiodriverbinary + 0x21))(voiceNum, channel, resource, sample);
    }
    channel->resourceIndex = (unsigned char)chunkIndex;
    channel->resource = resource;
    channel->active = 1;
    channel->state16 = 1;
    channel->sampleRate = ((unsigned short far *)channel->data)[0x0e];
    channel->note = resource->note;
    channel->position = 0;
    channel->remaining = event->value - 1;
    channel->value18 = *(unsigned short far *)(sample + 0x2a);
    channel->value1a = *(unsigned short far *)(sample + 0x2c);
    channel->value24 = *(unsigned short far *)(sample + 0x30);
    channel->value1c = 0;
    channel->value26 = sample[0x34];
    channel->value27 = 0;
    channel->value1e = *(unsigned short far *)(sample + 0x36);
    channel->value20 = *(unsigned short far *)(sample + 0x38);
    channel->value28 = 0;
    channel->value22 = 0;
    channel->value29 = 0;
    if (audio_driver_mode == 0)
        channel->channelNumber = (unsigned char)voiceNum;
    else
        channel->channelNumber = resource->channelNumber;
    if (event->command == 0xff) {
        ((void (far *)(int, struct AudioVoice *, unsigned short))
            (audiodriverbinary + 0x24))(channel->channelNumber, channel, (unsigned short)event->delta);
        if (audio_driver_mode != 0)
            event->command = 0x3c;
    }
    ((void (far *)(int, struct AudioVoice *, struct AudioChunk *, int, int, char far *))
        (audiodriverbinary + 9))(channel->channelNumber, channel, resource,
                                 (char)event->command + sample[0x10], event->param, sample);
    audioblock[chunkIndex] = event->command;
    return voiceNum;
}

void far _loadds send_audio_stop_event(int first, int chunkIndex)
{
    audio_event_send_buffer.command = 0xff;
    audio_event_send_buffer.delta = (unsigned int)first;
    audio_event_send_buffer.value = -32L;
    process_audio_event(&audio_event_send_buffer, chunkIndex);
}

void far _loadds set_audio_voice_value(int voiceNum, int value)
{
    ((void (far *)(int, struct AudioVoice *, int))(audiodriverbinary + 0x24))(
        snd_voices_tbl[voiceNum].channelNumber, &snd_voices_tbl[voiceNum], value);
}

int far _loadds select_audio_voice_slot(char far *sample, struct AudioChunk *chunk)
{
    int slotSounding;
    int slotReleased;
    unsigned long longestSounding;
    unsigned long longestReleased;
    int slot;
    struct AudioVoice *voice;

    slotSounding = -1;
    slotReleased = -1;
    longestSounding = 0;
    longestReleased = 0;
    if (*(unsigned short far *)(sample + 0x0c) == 0)
        return -1;
    if (audio_driver_mode != 0) {
        for (slot = 0; slot < 16; slot++) {
            voice = &snd_voices_tbl[slot];
            if (voice->active == 0)
                return slot;
            if (voice->active == 1 && voice->position > longestSounding) {
                longestSounding = voice->position;
                slotSounding = slot;
            }
            if (voice->active == 2 && voice->position > longestReleased) {
                longestReleased = voice->position;
                slotReleased = slot;
            }
        }
        if (slotReleased != -1)
            return slotReleased;
        if (slotSounding == -1)
            return -1;
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0f))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    }
    if (chunk->activeVoices >= chunk->maxVoices) {
        for (slot = 0; (unsigned int)slot < g_audiodrvvoices_count; slot++) {
            if (*(unsigned short far *)(sample + 0x0c) & audio_bit_masks[slot]) {
                voice = &snd_voices_tbl[slot];
                if (voice->resourceIndex == chunk->resourceType) {
                    if (voice->active == 0) {
                        chunk->activeVoices++;
                        return slot;
                    }
                    if (chunk->note >= voice->note) {
                        if (voice->active == 1 && voice->position > longestSounding) {
                            longestSounding = voice->position;
                            slotSounding = slot;
                        }
                        if (voice->active == 2 && voice->position > longestReleased) {
                            longestReleased = voice->position;
                            slotReleased = slot;
                        }
                    }
                }
            }
        }
        if (slotReleased != -1) {
            ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotReleased, &snd_voices_tbl[slotReleased]);
            ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotSounding, &snd_voices_tbl[slotSounding]);
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotSounding, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    } else {
        for (slot = 0; (unsigned int)slot < g_audiodrvvoices_count; slot++) {
            voice = &snd_voices_tbl[slot];
            if (*(unsigned short far *)(sample + 0x0c) & audio_bit_masks[slot]) {
                if (voice->active == 0) {
                    chunk->activeVoices++;
                    return slot;
                }
                if (chunk->note >= voice->note) {
                    if (voice->active == 1 && voice->position > longestSounding) {
                        longestSounding = voice->position;
                        slotSounding = slot;
                    }
                    if (voice->active == 2 && voice->position > longestReleased) {
                        longestReleased = voice->position;
                        slotReleased = slot;
                    }
                }
            }
        }
        if (slotReleased != -1) {
            if (snd_voices_tbl[slotReleased].resource != chunk) {
                snd_voices_tbl[slotReleased].resource->activeVoices--;
                chunk->activeVoices++;
            }
            ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotReleased, &snd_voices_tbl[slotReleased]);
            ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
        if (snd_voices_tbl[slotSounding].resource != chunk) {
            snd_voices_tbl[slotSounding].resource->activeVoices--;
            chunk->activeVoices++;
        }
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotSounding, &snd_voices_tbl[slotSounding]);
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotSounding, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    }
}

void far _loadds read_audio_event(struct AudioEvent *event, unsigned char far *stream)
{
    unsigned char far *start;
    unsigned char command;
    unsigned int i;

    start = stream;
    event->delta = 0;
    do {
        event->delta = (event->delta << 7) + (*stream & 0x7f);
    } while (*stream++ & 0x80);

    event->command = *stream++;
    command = event->command;
    switch (command) {
        case 0xe8:
            for (i = 0; i < *stream; i++)
                audio_resource_buffer[i] = stream[i + 1];
        case 0xe7:
            stream += *stream;
            stream++;
            break;
        case 0xe6:
            event->param = *stream++;
            event->value = audioresource_get_dword(stream);
            stream += 4;
            break;
        case 0xdc: case 0xdd: case 0xde: case 0xe0: case 0xe1:
        case 0xe2: case 0xe4: case 0xe9: case 0xea:
            event->param = *stream++;
            break;
        case 0xdf:
            event->param = *stream++;
            event->value = *stream++;
            break;
        case 0xe5:
            event->value = audioresource_get_word(stream);
            stream += 2;
            break;
        case 0xd9: case 0xda: case 0xdb:
            break;
    }
    if (command < 0xd9) {
        if (command > 0x80)
            event->param = *stream++;
        event->value = 0;
        do {
            event->value = (event->value << 7) + (*stream & 0x7f);
        } while (*stream++ & 0x80);
    }
    event->length = (unsigned char)(stream - start);
}

void far _loadds update_audio_voice_state(void)
{
    unsigned int i;
    i = 0;
    if ((unsigned int)g_audiodrvvoices_count != 0) {
        do {
            struct AudioVoice *voice;
            voice = &snd_voices_tbl[i];
            if (voice->active != 0 && voice->resourceIndex < 16)
                clear_audio_voice(voice);
            ++i;
        } while (i < g_audiodrvvoices_count);
    }
}

void far _loadds clear_audio_voice(struct AudioVoice *voice)
{
    voice->position++;
    if (voice->remaining == 0) {
        ((void (far *)(int, struct AudioVoice *))(audiodriverbinary + 0x0c))(voice->channelNumber, voice);
        voice->active = 2;
        if (audiochunktable[voice->resourceIndex].modeValue != 0) {
            voice->state16 = 3;
            return;
        }
        voice->state16 = 4;
        return;
    }
    voice->remaining--;
}

void far _loadds reset_audio_event_state(void)
{
    int i;
    for (i = 0; (unsigned int)i < g_audiodrvvoices_count; ++i) {
        struct AudioVoice *voice;
        struct AudioSample far *sample;
        voice = &snd_voices_tbl[i];
        if (voice->active == 0)
            continue;
        if (voice->resourceIndex > 15)
            clear_audio_voice(voice);
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
                --audiochunktable[voice->resourceIndex].activeVoices;
                ((void (far *)(unsigned int, struct AudioVoice *))(audiodriverbinary + 0x0f))(voice->channelNumber, voice);
                audioblock[voice->resourceIndex] = 0;
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
        ((void (far *)(unsigned int, struct AudioVoice *, struct AudioChunk *, char far *))(audiodriverbinary + 0x27))(
            voice->channelNumber, voice, voice->resource, voice->data);
    }
    ((void (far *)(struct AudioVoice *))(audiodriverbinary + 0x30))(snd_voices_tbl);
}

void far _loadds audio_driver_func1E(int first, int last)
{
    int i;
    int j;

    if (audio_driver_mode == 0) {
        for (i = 0; i < g_audiodrvvoices_count; i++) {
            if (snd_voices_tbl[i].resourceIndex <= last && snd_voices_tbl[i].resourceIndex >= first) {
                ((void (far *)(int))(audiodriverbinary + 0x1e))(i);
                snd_voices_tbl[i].active = 0;
                snd_voices_tbl[i].data = 0;
                snd_voices_tbl[i].resourceIndex = 0xff;
                snd_voices_tbl[i].note = 0;
            }
        }
    } else {
        for (i = first; i <= last; i++) {
            if (audiochunktable[i].channelNumber < 16) {
                ((void (far *)(int))(audiodriverbinary + 0x1e))(audiochunktable[i].channelNumber);
                for (j = 0; j < 16; j++) {
                    if (snd_voices_tbl[j].resourceIndex == i) {
                        snd_voices_tbl[j].active = 0;
                        snd_voices_tbl[j].data = 0;
                        snd_voices_tbl[j].resourceIndex = 0xff;
                        snd_voices_tbl[j].note = 0;
                    }
                }
            }
        }
    }
    for (i = first; i <= last; i++)
        audiochunktable[i].activeVoices = 0;
}

unsigned short audio_timer_reentry_lock = 0;
