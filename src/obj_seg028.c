#include "stunts_types.h"
/* READABILITY: Translate parsed music and sound events into DOS audio-driver voice operations. */
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};

/* PORT: Runtime audio records contain 16-bit far pointers; host pointer width changes their layout. */
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};

struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};

struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};

extern I8 FAR *audiodriverbinary;
extern U16S audio_update_lock;
extern U16S audio_timer_reentry_lock;
extern U8 audio_song_ready;
extern U8 audioflag2;
extern U8 audio_pause_in_progress;
unsigned short snd_sample_rate_phase;
unsigned short mus_samplelimit;
extern U8 block_audio_num;
extern struct AudioChunk audiochunktable[];
extern struct AudioVoice snd_voices_tbl[];
unsigned char g_audiodrvvoices_count;
extern U8 audio_driver_mode;
extern U8 audioblock[];
extern U8 g_audchnkvalue[];
extern U8 g_musicvolumesetting;
static U8 audio_resource_buffer[260];
static struct AudioEvent audio_event_read_buffer;
static struct AudioEvent audioevent;
static struct AudioEvent audio_event_send_buffer;
extern U16S audio_bit_masks[];
extern I8 FAR *kick_res;
extern I8 FAR *g_snaresnd;
extern I8 FAR *ride_audio_sound_res;
extern I8 FAR *audio_opp_res;
extern I8 FAR *chhtsample;
extern I8 FAR *resource_sound_hit;
extern I8 FAR *tommsampleresource;
extern U32 FAR audioresource_get_dword(U8 FAR *);
extern U16S FAR audioresource_get_word(U8 FAR *);
extern void FAR audio_init_chunk(I16, I16, I16, I16, I16, I16, I16);

void FAR _loadds reset_audio_event_state(void);
void FAR _loadds process_music_audio_chunks(void);
void FAR _loadds update_audio_voice_state(void);
void FAR _loadds process_effect_audio_chunks(void);
void FAR _loadds process_audio_chunk_event(I16 chunkIndex);
I8 FAR * FAR _loadds find_audio_chunk_data(U8 index, struct AudioChunk *chunk);
void FAR _loadds apply_audio_voice_event(I16 voiceIndex, U8 type, I16 value);
void FAR _loadds set_audio_voice_parameter(I16 voiceIndex, I16 value);
void FAR _loadds audio_unk2(I16 voiceIndex, U8 type);
I16 FAR _loadds process_audio_event(struct AudioEvent *event, I16 chunkIndex);
I16 FAR _loadds select_audio_voice_slot(I8 FAR *sample, struct AudioChunk *chunk);
void FAR _loadds read_audio_event(struct AudioEvent *event, U8 FAR *stream);
void FAR _loadds clear_audio_voice(struct AudioVoice *voice);
void FAR _loadds audio_driver_func1E(I16 first, I16 last);

void FAR _loadds audiodriver_timer(void)
{
    if (audiodriverbinary == (I8 FAR *)0)
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

void FAR _loadds process_music_audio_chunks(void)
{
    U8 chunkIndex;

    snd_sample_rate_phase += 0x80;
    while (snd_sample_rate_phase >= mus_samplelimit) {
        update_audio_voice_state();
        snd_sample_rate_phase -= mus_samplelimit;
        for (chunkIndex = 0; chunkIndex < block_audio_num; chunkIndex++)
            process_audio_chunk_event(chunkIndex);
    }
}

void FAR _loadds process_effect_audio_chunks(void)
{
    U8 chunkIndex;

    for (chunkIndex = 0x10; chunkIndex < 0x17; chunkIndex++)
        process_audio_chunk_event(chunkIndex);
}

/* Apply one parsed chunk event to the selected channel or driver voice.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): send parsed chunk commands to the loaded DOS sound driver. */
void FAR _loadds process_audio_chunk_event(I16 chunkIndex)
{
    U8 param;
    struct AudioChunk *chunk;
    void (FAR *callback)(I16);

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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                    ((void (FAR *)(I16, U8 *))(audiodriverbinary + 0x39))(
                        audioevent.length - 4, audio_resource_buffer);
                    break;
                case 0xea:
                    g_audchnkvalue[chunkIndex] = param;
                    break;
                case 0xe6:
                    chunk->depth++;
                    chunk->stack[chunk->depth] = chunk->pos;
                    chunk->pos = (I8 FAR *)audioevent.value + 4;
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
                        if ((U8)chunk->data[0x43] < 0x10)
                            chunk->channelNumber = chunk->data[0x43];
                        else
                            chunk->channelNumber = (chunkIndex & 0x0f) + 1;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        ((void (FAR *)(I16, I16, I16))(audiodriverbinary + 0x12))(
                            (U8)chunk->channelNumber, 0, chunk->program);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        ((void (FAR *)(I16, I16, struct AudioChunk *, I8 FAR *))(audiodriverbinary + 0x21))(
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
                    apply_audio_voice_event(chunkIndex, param, (I16)audioevent.value);
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
                    set_audio_voice_parameter(chunkIndex, (I16)audioevent.value);
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

static void FAR _loadds set_chunk_channel(I16 chunkIndex, U8 channel)
{
    audiochunktable[chunkIndex].channelNumber = channel;
}

I8 FAR * FAR _loadds find_audio_chunk_data(U8 index, struct AudioChunk *chunk)
{
    return chunk->samples[index];
}

/* Send a voice parameter change to the appropriate driver channel.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): apply voice commands through the loaded DOS sound driver. */
void FAR _loadds apply_audio_voice_event(I16 voiceIndex, U8 type, I16 value)
{
    struct AudioChunk *chunk;
    I16 i;

    chunk = &audiochunktable[voiceIndex];
    if (type == 0x40)
        chunk->modeValue = value;
    if (audio_driver_mode != 0)
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *, I16, I16))(audiodriverbinary + 0x15))(chunk->channelNumber, 0, type, value);
    for (i = 0; i < g_audiodrvvoices_count; i++) {
        if (snd_voices_tbl[i].resourceIndex == chunk->resourceType && audio_driver_mode == 0)
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *, I16, I16))(audiodriverbinary + 0x15))(i, &snd_voices_tbl[i], type, value);
        if (type == 0x40 && value == 0 && snd_voices_tbl[i].active == 2 && snd_voices_tbl[i].resourceIndex == chunk->resourceType)
            snd_voices_tbl[i].state16 = 4;
    }
}

/* Send the packed voice parameter update to the audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): set one voice parameter in the DOS sound driver. */
void FAR _loadds set_audio_voice_parameter(I16 voiceIndex, I16 value)
{
    struct AudioChunk *voice;

    voice = &audiochunktable[voiceIndex];
    if (value & 0x100)
        value |= 0x80;
    value = ((value & 0xff00) >> 1) + ((I8S)value - 0x2000);
    voice->value26 = value;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(struct AudioChunk *, I16, I16))(audiodriverbinary + 0x1b))(voice, value, voice->channelNumber);
}

/* Send a voice parameter to one channel or to the full driver voice table.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): route channel parameter updates to the DOS sound driver. */
void FAR _loadds audio_unk2(I16 voiceIndex, U8 type)
{
    struct AudioChunk *audioChunk;
    I16 i;
    U16 typeCode;

    audioChunk = &audiochunktable[voiceIndex];
    audioChunk->program = type;
    if (audio_driver_mode == 0) {
        i = 0;
        if ((U16)i < g_audiodrvvoices_count) {
            typeCode = type;
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (FAR *)(I16, struct AudioVoice *, I16))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        (audiodriverbinary + 0x12))(i, &snd_voices_tbl[i], typeCode);
                ++i;
            } while ((U16)i < g_audiodrvvoices_count);
        }
    } else {
        ((void (FAR *)(I16, struct AudioVoice *, I16))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + 0x12))(audioChunk->channelNumber, 0, type);
    }
}

/* Start a sample on the selected voice through the audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): start sample playback through the DOS sound driver. */
void FAR _loadds start_audio_voice_sample(I16 voiceIndex, I8 FAR *voiceData)
{
    struct AudioChunk *chunk;
    I16 i;

    audiochunktable[voiceIndex].data = voiceData;
    if ((U8)voiceData[0x43] < 0x10)
        audiochunktable[voiceIndex].channelNumber = voiceData[0x43];
    else
        audiochunktable[voiceIndex].channelNumber = (voiceIndex & 0x0f) + 1;
    if (audio_driver_mode == 0) {
        i = 0;
        if ((U16)i < g_audiodrvvoices_count) {
            chunk = &audiochunktable[voiceIndex];
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        (audiodriverbinary + 0x21))(i, &snd_voices_tbl[i], chunk, voiceData);
                ++i;
            } while ((U16)i < g_audiodrvvoices_count);
        }
    } else {
        ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + 0x21))(
                audiochunktable[voiceIndex].channelNumber, 0,
                &audiochunktable[voiceIndex], voiceData);
    }
}

/* Dispatch a timed music event to the driver voice or channel.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): dispatch timed event commands to the DOS sound driver. */
I16 FAR _loadds process_audio_event(struct AudioEvent *event, I16 chunkIndex)
{
    struct AudioChunk *resource;
    I8 FAR *sample;
    I16 voiceNum;
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
            ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                (audiodriverbinary + 0x21))(voiceNum, channel, resource, sample);
    }
    channel->resourceIndex = (U8)chunkIndex;
    channel->resource = resource;
    channel->active = 1;
    channel->state16 = 1;
    channel->sampleRate = ((U16S FAR *)channel->data)[0x0e];
    channel->note = resource->note;
    channel->position = 0;
    channel->remaining = event->value - 1;
    channel->value18 = *(U16S FAR *)(sample + 0x2a);
    channel->value1a = *(U16S FAR *)(sample + 0x2c);
    channel->value24 = *(U16S FAR *)(sample + 0x30);
    channel->value1c = 0;
    channel->value26 = sample[0x34];
    channel->value27 = 0;
    channel->value1e = *(U16S FAR *)(sample + 0x36);
    channel->value20 = *(U16S FAR *)(sample + 0x38);
    channel->value28 = 0;
    channel->value22 = 0;
    channel->value29 = 0;
    if (audio_driver_mode == 0)
        channel->channelNumber = (U8)voiceNum;
    else
        channel->channelNumber = resource->channelNumber;
    if (event->command == 0xff) {
        ((void (FAR *)(I16, struct AudioVoice *, U16S))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + 0x24))(channel->channelNumber, channel, (U16S)event->delta);
        if (audio_driver_mode != 0)
            event->command = 0x3c;
    }
    ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I16, I16, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        (audiodriverbinary + 9))(channel->channelNumber, channel, resource,
                                 (I8)event->command + sample[0x10], event->param, sample);
    audioblock[chunkIndex] = event->command;
    return voiceNum;
}

I16 FAR _loadds send_audio_stop_event(U16 rate, I16 handle)
{
    audio_event_send_buffer.command = 0xff;
    audio_event_send_buffer.delta = rate;
    audio_event_send_buffer.value = -32L;
    return process_audio_event(&audio_event_send_buffer, handle);
}

/* Send a value update for one selected audio voice.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): update the selected DOS sound-driver voice. */
void FAR _loadds set_audio_voice_value(I16 voiceNum, I16 value)
{
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(I16, struct AudioVoice *, I16))(audiodriverbinary + 0x24))(
        snd_voices_tbl[voiceNum].channelNumber, &snd_voices_tbl[voiceNum], value);
}

/* Stop or reset driver voices while selecting a reusable voice slot.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): stop/reset candidate voices through the DOS sound driver. */
I16 FAR _loadds select_audio_voice_slot(I8 FAR *sample, struct AudioChunk *chunk)
{
    I16 slotSounding;
    I16 slotReleased;
    U32 longestSounding;
    U32 longestReleased;
    I16 slot;
    struct AudioVoice *voice;

    slotSounding = -1;
    slotReleased = -1;
    longestSounding = 0;
    longestReleased = 0;
    if (*(U16S FAR *)(sample + 0x0c) == 0)
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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0f))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    }
    if (chunk->activeVoices >= chunk->maxVoices) {
        for (slot = 0; (U16)slot < g_audiodrvvoices_count; slot++) {
            if (*(U16S FAR *)(sample + 0x0c) & audio_bit_masks[slot]) {
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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotReleased, &snd_voices_tbl[slotReleased]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotSounding, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotSounding, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    } else {
        for (slot = 0; (U16)slot < g_audiodrvvoices_count; slot++) {
            voice = &snd_voices_tbl[slot];
            if (*(U16S FAR *)(sample + 0x0c) & audio_bit_masks[slot]) {
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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotReleased, &snd_voices_tbl[slotReleased]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
        if (snd_voices_tbl[slotSounding].resource != chunk) {
            snd_voices_tbl[slotSounding].resource->activeVoices--;
            chunk->activeVoices++;
        }
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(slotSounding, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0f))(slotSounding, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    }
}

void FAR _loadds read_audio_event(struct AudioEvent *event, U8 FAR *stream)
{
    U8 FAR *start;
    U8 command;
    U16 i;

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
    event->length = (U8)(stream - start);
}

void FAR _loadds update_audio_voice_state(void)
{
    U16 i;
    i = 0;
    if ((U16)g_audiodrvvoices_count != 0) {
        do {
            struct AudioVoice *voice;
            voice = &snd_voices_tbl[i];
            if (voice->active != 0 && voice->resourceIndex < 16)
                clear_audio_voice(voice);
            ++i;
        } while (i < g_audiodrvvoices_count);
    }
}

/* Clear one voice through the audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): clear the selected DOS sound-driver voice. */
void FAR _loadds clear_audio_voice(struct AudioVoice *voice)
{
    voice->position++;
    if (voice->remaining == 0) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + 0x0c))(voice->channelNumber, voice);
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

/* Reset driver voice and event state after processing an audio event.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): clear timed voice state in the DOS sound driver. */
void FAR _loadds reset_audio_event_state(void)
{
    I16 i;
    for (i = 0; (U16)i < g_audiodrvvoices_count; ++i) {
        struct AudioVoice *voice;
        struct AudioSample FAR *sample;
        voice = &snd_voices_tbl[i];
        if (voice->active == 0)
            continue;
        if (voice->resourceIndex > 15)
            clear_audio_voice(voice);
        sample = (struct AudioSample FAR *)voice->data;

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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(U16, struct AudioVoice *))(audiodriverbinary + 0x0f))(voice->channelNumber, voice);
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
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(U16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))(audiodriverbinary + 0x27))(
            voice->channelNumber, voice, voice->resource, voice->data);
    }
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(struct AudioVoice *))(audiodriverbinary + 0x30))(snd_voices_tbl);
}

/* Send the function-1Eh driver operation for the selected chunk channels.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): send function 1Eh to the DOS sound driver. */
void FAR _loadds audio_driver_func1E(I16 first, I16 last)
{
    I16 i;
    I16 j;

    if (audio_driver_mode == 0) {
        for (i = 0; i < g_audiodrvvoices_count; i++) {
            if (snd_voices_tbl[i].resourceIndex <= last && snd_voices_tbl[i].resourceIndex >= first) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(I16))(audiodriverbinary + 0x1e))(i);
                snd_voices_tbl[i].active = 0;
                snd_voices_tbl[i].data = 0;
                snd_voices_tbl[i].resourceIndex = 0xff;
                snd_voices_tbl[i].note = 0;
            }
        }
    } else {
        for (i = first; i <= last; i++) {
            if (audiochunktable[i].channelNumber < 16) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(I16))(audiodriverbinary + 0x1e))(audiochunktable[i].channelNumber);
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

U16S audio_timer_reentry_lock = 0;
