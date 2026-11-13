#include "types.h"
/* READABILITY: Translate parsed music and sound events into DOS audio-driver voice operations. */
#include "audio.h"

/* PORT: Runtime audio records contain 16-bit far pointers; host pointer width changes their layout. */
#include "audtick.h"
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "timer.h"
unsigned short snd_sample_rate_phase;
unsigned short mus_samplelimit;

/* Local view: audiochunktable; array is None; another unit uses 24. */
extern struct AudioChunk audiochunktable[];
/* Local view: snd_voices_tbl; array is None; another unit uses 16. */
extern struct AudioVoice snd_voices_tbl[];
unsigned char g_audiodrvvoices_count;

static U8 audio_resource_buffer[260];
static struct AudioEvent audio_event_read_buffer;
static struct AudioEvent audioevent;
static struct AudioEvent audio_event_send_buffer;
#include "audstate.h"

/* Local view: audio_init_chunk; parameters.count is 7; another unit uses 6. */
extern void FAR audio_init_chunk(I16, I16, I16, I16, I16, I16, I16);

/* Local view: audio_set_chunk_volume; parameters[1] is unsigned byte; another unit uses signed word. */
void FAR _loadds audio_set_chunk_volume(I16 voiceIndex, U8 type);

void FAR _loadds audiodriver_timer(void)
{
    if (audiodriverbinary == 0)
        return;
    if (audio_update_lock != 0 || audio_timer_reentry_lock != 0)
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

    snd_sample_rate_phase += AUDIO_MUSIC_PHASE_INCREMENT;
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

    for (chunkIndex = AUDIO_EFFECT_CHUNK_FIRST; chunkIndex < AUDIO_EFFECT_PROCESS_END; chunkIndex++)
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
            if (audioevent.command >= AUDIO_EVENT_CONTROL_FIRST) {
                param = audioevent.param;
                switch (audioevent.command) {
                case AUDIO_EVENT_RETURN:
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
                case AUDIO_EVENT_RAW_BYTES:
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                    ((void (FAR *)(I16, U8 *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_RAW_BYTES))(
                        audioevent.length - AUDIO_RAW_EVENT_DRIVER_COUNT_BIAS, audio_resource_buffer);
                    break;
                case AUDIO_EVENT_CHUNK_VALUE:
                    g_audchnkvalue[chunkIndex] = param;
                    break;
                case AUDIO_EVENT_CALL:
                    chunk->depth++;
                    chunk->stack[chunk->depth] = chunk->pos;
                    chunk->pos = (I8 FAR *)audioevent.value + AUDIO_TRACK_SPAN_BYTES;
                    break;
                case AUDIO_EVENT_STOP:
                    chunk->pos = 0;
                    callback = chunk->callback;
                    audio_driver_func1E(chunkIndex, chunkIndex);
                    audio_init_chunk(chunkIndex, chunkIndex, 0, 0, 0, g_musicvolumesetting, 0);
                    if (callback != 0)
                        callback(chunkIndex);
                    break;
                case AUDIO_EVENT_RESTART:
                    chunk->depth = 0;
                    chunk->loopDepth = 0;
                    chunk->pos = chunk->stack[0];
                    break;
                case AUDIO_EVENT_SELECT_RESOURCE:
                    chunk->data = find_audio_chunk_data(param, chunk);
                    if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE) {
                        if ((U8)chunk->data[AUDIO_SAMPLE_CHANNEL_OFFSET] < AUDIO_CHANNEL_NUMBER_LIMIT)
                            chunk->channelNumber = chunk->data[AUDIO_SAMPLE_CHANNEL_OFFSET];
                        else
                            chunk->channelNumber = (chunkIndex & AUDIO_CHANNEL_NIBBLE_MASK) + 1;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        ((void (FAR *)(I16, I16, I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_PROGRAM))(
                            (U8)chunk->channelNumber, 0, chunk->volume);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        ((void (FAR *)(I16, I16, struct AudioChunk *, I8 FAR *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_BIND_SAMPLE))(
                            chunk->channelNumber, 0, chunk, chunk->data);
                    }
                    break;
                case AUDIO_EVENT_TEMPO:
                    if (chunkIndex < AUDIO_EFFECT_CHUNK_FIRST)
                        mus_samplelimit = AUDIO_TEMPO_NUMERATOR / param;
                    break;
                case AUDIO_EVENT_PROGRAM_VALUE:
                    audio_set_chunk_volume(chunkIndex, param);
                    break;
                case AUDIO_EVENT_CONTROLLER:
                    apply_audio_voice_event(chunkIndex, param, audioevent.value);
                    break;
                case AUDIO_EVENT_MAX_VOICES:
                    chunk->maxVoices = param;
                    break;
                case AUDIO_EVENT_NOTE_PRIORITY:
                    chunk->priority = param;
                    break;
                case AUDIO_EVENT_LOOP_BEGIN:
                    chunk->loopPos[chunk->loopDepth] = chunk->pos;
                    chunk->loopCount[chunk->loopDepth] = param - 1;
                    chunk->loopDepth++;
                    break;
                case AUDIO_EVENT_LOOP_END:
                    if (chunk->loopDepth != 0) {
                        chunk->pos = chunk->loopPos[chunk->loopDepth - 1];
                        if (chunk->loopCount[chunk->loopDepth - 1]-- == 0)
                            chunk->loopDepth--;
                    }
                    break;
                case AUDIO_EVENT_VELOCITY:
                    chunk->velocity = param;
                    break;
                case AUDIO_EVENT_CHANNEL:
                    chunk->channelNumber = param;
                    break;
                case AUDIO_EVENT_WORD_PARAMETER:
                    set_audio_voice_parameter(chunkIndex, audioevent.value);
                    break;
                }
            } else {
                if (audioevent.command < AUDIO_EVENT_NOTE_VELOCITY_BOUNDARY)
                    audioevent.param = chunk->velocity;
                audioevent.command &= AUDIO_EVENT_NOTE_MASK;
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
    if (type == AUDIO_CONTROLLER_HOLD)
        chunk->modeValue = value;
    if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE)
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *, I16, I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_CONTROLLER))(chunk->channelNumber, 0, type, value);
    for (i = 0; i < g_audiodrvvoices_count; i++) {
        if (snd_voices_tbl[i].resourceIndex == chunk->resourceType && audio_driver_mode == AUDIO_DRIVER_MODE_VOICE)
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *, I16, I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_CONTROLLER))(i, &snd_voices_tbl[i], type, value);
        if (type == AUDIO_CONTROLLER_HOLD && value == 0 && snd_voices_tbl[i].active == AUDIO_VOICE_RELEASED && snd_voices_tbl[i].resourceIndex == chunk->resourceType)
            snd_voices_tbl[i].envelope_state = AUDIO_LEVEL_STATE_RELEASE;
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
        value |= AUDIO_WORD_PARAMETER_LOW_BYTE_SIGN_BIT;
    value = ((value & AUDIO_WORD_PARAMETER_HIGH_BYTE_MASK) >> 1) + ((I8S)value - AUDIO_WORD_PARAMETER_CENTER);
    voice->pitch_bend = value;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(struct AudioChunk *, I16, I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_CHUNK_PARAMETER))(voice, value, voice->channelNumber);
}

/* Send a voice parameter to one channel or to the full driver voice table.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): route channel parameter updates to the DOS sound driver. */
/* Local view: audio_set_chunk_volume; parameters[1] is unsigned byte; another unit uses signed word. */
void FAR _loadds audio_set_chunk_volume(I16 voiceIndex, U8 type)
{
    struct AudioChunk *audioChunk;
    I16 i;
    U16 typeCode;

    audioChunk = &audiochunktable[voiceIndex];
    audioChunk->volume = type;
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        i = 0;
        if (i < g_audiodrvvoices_count) {
            typeCode = type;
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (FAR *)(I16, struct AudioVoice *, I16))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        (audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_PROGRAM))(i, &snd_voices_tbl[i], typeCode);
                ++i;
            } while (i < g_audiodrvvoices_count);
        }
    } else {
        ((void (FAR *)(I16, struct AudioVoice *, I16))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_PROGRAM))(audioChunk->channelNumber, 0, type);
    }
}

/* Start a sample on the selected voice through the audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): start sample playback through the DOS sound driver. */
#include "auddrv.h"
void FAR _loadds start_audio_voice_sample(I16 voiceIndex, I8 FAR *voiceData)
{
    struct AudioChunk *chunk;
    I16 i;

    audiochunktable[voiceIndex].data = voiceData;
    if ((U8)voiceData[AUDIO_SAMPLE_CHANNEL_OFFSET] < AUDIO_CHANNEL_NUMBER_LIMIT)
        audiochunktable[voiceIndex].channelNumber = voiceData[AUDIO_SAMPLE_CHANNEL_OFFSET];
    else
        audiochunktable[voiceIndex].channelNumber = (voiceIndex & AUDIO_CHANNEL_NIBBLE_MASK) + 1;
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        i = 0;
        if (i < g_audiodrvvoices_count) {
            chunk = &audiochunktable[voiceIndex];
            do {
                if (snd_voices_tbl[i].resourceIndex == voiceIndex)
                    ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                        (audiodriverbinary + AUDIO_DRIVER_ENTRY_BIND_SAMPLE))(i, &snd_voices_tbl[i], chunk, voiceData);
                ++i;
            } while (i < g_audiodrvvoices_count);
        }
    } else {
        ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + AUDIO_DRIVER_ENTRY_BIND_SAMPLE))(
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
    if (sample[AUDIO_SAMPLE_KIND_OFFSET] == AUDIO_SAMPLE_KIND_PERCUSSION) {
        switch (event->command) {
        case AUDIO_PERCUSSION_NOTE_BASS_DRUM:
            sample = kick_res;
            break;
        case AUDIO_PERCUSSION_NOTE_SNARE:
            sample = g_snaresnd;
            break;
        case AUDIO_PERCUSSION_NOTE_RIDE:
            sample = ride_audio_sound_res;
            break;
        case AUDIO_PERCUSSION_NOTE_CRASH:
            sample = audio_opp_res;
            break;
        case AUDIO_PERCUSSION_NOTE_CLOSED_HIHAT:
            sample = chhtsample;
            break;
        case AUD_PERC_NOTE_OPEN_HIHAT_A:
        case AUD_PERC_NOTE_OPEN_HIHAT_B:
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
        if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE)
            ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                (audiodriverbinary + AUDIO_DRIVER_ENTRY_BIND_SAMPLE))(voiceNum, channel, resource, sample);
    }
    channel->resourceIndex = chunkIndex;
    channel->resource = resource;
    channel->active = AUDIO_VOICE_SOUNDING;
    channel->envelope_state = AUDIO_LEVEL_STATE_RISE;
    channel->envelope_value = ((U16S FAR *)channel->data)[AUDIO_SAMPLE_LEVEL_WORD_INDEX];
    channel->priority = resource->priority;
    channel->position = 0;
    channel->remaining = event->value - 1;
    channel->modulation_delay = *(U16S FAR *)(sample + AUDIO_SAMPLE_LOOP_DELAY_OFFSET);
    channel->modulation_duration = *(U16S FAR *)(sample + AUDIO_SAMPLE_LOOP_DURATION_OFFSET);
    channel->modulation_step = *(U16S FAR *)(sample + AUDIO_SAMPLE_LOOP_STEP_OFFSET);
    channel->modulation_offset = 0;
    channel->modulation_direction = sample[AUDIO_SAMPLE_LOOP_DIRECTION_OFFSET];
    channel->modulation_step_delay = 0;
    channel->pulse_delay = *(U16S FAR *)(sample + AUDIO_SAMPLE_PULSE_DELAY_OFFSET);
    channel->pulse_duration = *(U16S FAR *)(sample + AUDIO_SAMPLE_PULSE_DURATION_OFFSET);
    channel->pulse_step_delay = 0;
    channel->pulse_value = 0;
    channel->pulse_table_index = 0;
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE)
        channel->channelNumber = voiceNum;
    else
        channel->channelNumber = resource->channelNumber;
    if (event->command == AUDIO_EVENT_EXPLICIT_RATE) {
        ((void (FAR *)(I16, struct AudioVoice *, U16S))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            (audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_VOICE_VALUE))(channel->channelNumber, channel, event->delta);
        if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE)
            event->command = AUDIO_EVENT_CHANNEL_MODE_RATE_NOTE;
    }
    ((void (FAR *)(I16, struct AudioVoice *, struct AudioChunk *, I16, I16, I8 FAR *))
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        (audiodriverbinary + AUDIO_DRIVER_ENTRY_START_NOTE))(channel->channelNumber, channel, resource,
                                 (I8)event->command + sample[AUDIO_SAMPLE_NOTE_BIAS_OFFSET], event->param, sample);
    audioblock[chunkIndex] = event->command;
    return voiceNum;
}

I16 FAR _loadds send_audio_stop_event(U16 rate, I16 handle)
{
    audio_event_send_buffer.command = AUDIO_EVENT_EXPLICIT_RATE;
    audio_event_send_buffer.delta = rate;
    audio_event_send_buffer.value = AUDIO_ENGINE_EVENT_DURATION;
    return process_audio_event(&audio_event_send_buffer, handle);
}

/* Send a value update for one selected audio voice.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): update the selected DOS sound-driver voice. */
void FAR _loadds set_audio_voice_value(I16 voiceNum, I16 value)
{
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(I16, struct AudioVoice *, I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_SET_VOICE_VALUE))(
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
    if (*(U16S FAR *)(sample + AUDIO_SAMPLE_VOICE_MASK_OFFSET) == 0)
        return -1;
    if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE) {
        for (slot = 0; slot < AUDIO_VOICE_CAPACITY; slot++) {
            voice = &snd_voices_tbl[slot];
            if (voice->active == AUDIO_VOICE_IDLE)
                return slot;
            if (voice->active == AUDIO_VOICE_SOUNDING && voice->position > longestSounding) {
                longestSounding = voice->position;
                slotSounding = slot;
            }
            if (voice->active == AUDIO_VOICE_RELEASED && voice->position > longestReleased) {
                longestReleased = voice->position;
                slotReleased = slot;
            }
        }
        if (slotReleased != -1)
            return slotReleased;
        if (slotSounding == -1)
            return -1;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(
            snd_voices_tbl[slotSounding].channelNumber, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    }
    if (chunk->activeVoices >= chunk->maxVoices) {
        for (slot = 0; slot < g_audiodrvvoices_count; slot++) {
            if (*(U16S FAR *)(sample + AUDIO_SAMPLE_VOICE_MASK_OFFSET) & audio_bit_masks[slot]) {
                voice = &snd_voices_tbl[slot];
                if (voice->resourceIndex == chunk->resourceType) {
                    if (voice->active == AUDIO_VOICE_IDLE) {
                        chunk->activeVoices++;
                        return slot;
                    }
                    if (chunk->priority >= voice->priority) {
                        if (voice->active == AUDIO_VOICE_SOUNDING && voice->position > longestSounding) {
                            longestSounding = voice->position;
                            slotSounding = slot;
                        }
                        if (voice->active == AUDIO_VOICE_RELEASED && voice->position > longestReleased) {
                            longestReleased = voice->position;
                            slotReleased = slot;
                        }
                    }
                }
            }
        }
        if (slotReleased != -1) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(slotReleased, &snd_voices_tbl[slotReleased]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(slotSounding, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(slotSounding, &snd_voices_tbl[slotSounding]);
        return slotSounding;
    } else {
        for (slot = 0; slot < g_audiodrvvoices_count; slot++) {
            voice = &snd_voices_tbl[slot];
            if (*(U16S FAR *)(sample + AUDIO_SAMPLE_VOICE_MASK_OFFSET) & audio_bit_masks[slot]) {
                if (voice->active == AUDIO_VOICE_IDLE) {
                    chunk->activeVoices++;
                    return slot;
                }
                if (chunk->priority >= voice->priority) {
                    if (voice->active == AUDIO_VOICE_SOUNDING && voice->position > longestSounding) {
                        longestSounding = voice->position;
                        slotSounding = slot;
                    }
                    if (voice->active == AUDIO_VOICE_RELEASED && voice->position > longestReleased) {
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
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(slotReleased, &snd_voices_tbl[slotReleased]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
            ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(slotReleased, &snd_voices_tbl[slotReleased]);
            return slotReleased;
        }
        if (slotSounding == -1)
            return -1;
        if (snd_voices_tbl[slotSounding].resource != chunk) {
            snd_voices_tbl[slotSounding].resource->activeVoices--;
            chunk->activeVoices++;
        }
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(slotSounding, &snd_voices_tbl[slotSounding]);
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(slotSounding, &snd_voices_tbl[slotSounding]);
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
        event->delta = (event->delta << AUDIO_EVENT_BASE128_SHIFT) + (*stream & AUDIO_EVENT_BASE128_PAYLOAD_MASK);
    } while (*stream++ & AUDIO_EVENT_BASE128_CONTINUATION);

    event->command = *stream++;
    command = event->command;
    switch (command) {
        case AUDIO_EVENT_RAW_BYTES:
            for (i = 0; i < *stream; i++)
                audio_resource_buffer[i] = stream[i + 1];
        case AUDIO_EVENT_META:
            stream += *stream;
            stream++;
            break;
        case AUDIO_EVENT_CALL:
            event->param = *stream++;
            event->value = audioresource_get_dword(stream);
            stream += AUDIO_EVENT_TARGET_BYTES;
            break;
        case AUDIO_EVENT_SELECT_RESOURCE: case AUDIO_EVENT_TEMPO: case AUDIO_EVENT_PROGRAM_VALUE: case AUDIO_EVENT_MAX_VOICES: case AUDIO_EVENT_NOTE_PRIORITY:
        case AUDIO_EVENT_LOOP_BEGIN: case AUDIO_EVENT_VELOCITY: case AUDIO_EVENT_CHANNEL: case AUDIO_EVENT_CHUNK_VALUE:
            event->param = *stream++;
            break;
        case AUDIO_EVENT_CONTROLLER:
            event->param = *stream++;
            event->value = *stream++;
            break;
        case AUDIO_EVENT_WORD_PARAMETER:
            event->value = audioresource_get_word(stream);
            stream += AUDIO_EVENT_WORD_BYTES;
            break;
        case AUDIO_EVENT_RETURN: case AUDIO_EVENT_STOP: case AUDIO_EVENT_RESTART:
            break;
    }
    if (command < AUDIO_EVENT_CONTROL_FIRST) {
        if (command > AUDIO_EVENT_NOTE_VELOCITY_BOUNDARY)
            event->param = *stream++;
        event->value = 0;
        do {
            event->value = (event->value << AUDIO_EVENT_BASE128_SHIFT) + (*stream & AUDIO_EVENT_BASE128_PAYLOAD_MASK);
        } while (*stream++ & AUDIO_EVENT_BASE128_CONTINUATION);
    }
    event->length = stream - start;
}

void FAR _loadds update_audio_voice_state(void)
{
    U16 i;
    i = 0;
    if ((U16)g_audiodrvvoices_count != 0) {
        do {
            struct AudioVoice *voice;
            voice = &snd_voices_tbl[i];
            if (voice->active != AUDIO_VOICE_IDLE && voice->resourceIndex < AUDIO_EFFECT_CHUNK_FIRST)
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
        ((void (FAR *)(I16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RELEASE_VOICE))(voice->channelNumber, voice);
        voice->active = AUDIO_VOICE_RELEASED;
        if (audiochunktable[voice->resourceIndex].modeValue != 0) {
            voice->envelope_state = AUDIO_LEVEL_STATE_HOLD;
            return;
        }
        voice->envelope_state = AUDIO_LEVEL_STATE_RELEASE;
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
    for (i = 0; i < g_audiodrvvoices_count; ++i) {
        struct AudioVoice *voice;
        struct AudioSample FAR *sample;
        voice = &snd_voices_tbl[i];
        if (voice->active == AUDIO_VOICE_IDLE)
            continue;
        if (voice->resourceIndex > AUDIO_MUSIC_RESOURCE_INDEX_LAST)
            clear_audio_voice(voice);
        sample = (struct AudioSample FAR *)voice->data;

        if (voice->envelope_state == AUDIO_LEVEL_STATE_RISE) {
            voice->envelope_value += sample->level20;
            if (voice->envelope_value >= sample->level1e) {
                voice->envelope_value = sample->level1e;
                if (sample->level24 >= sample->level1e)
                    voice->envelope_state = AUDIO_LEVEL_STATE_HOLD;
                else
                    voice->envelope_state = AUDIO_LEVEL_STATE_FALL;
            }
        }
        if (voice->envelope_state == AUDIO_LEVEL_STATE_FALL) {
            voice->envelope_value -= sample->level22;
            if (voice->envelope_value <= sample->level24) {
                voice->envelope_state = AUDIO_LEVEL_STATE_HOLD;
                voice->envelope_value = sample->level24;
            }
        }
        if (voice->envelope_state == AUDIO_LEVEL_STATE_HOLD && sample->level24 == 0)
            voice->envelope_state = AUDIO_LEVEL_STATE_RELEASE;
        if (voice->envelope_state == AUDIO_LEVEL_STATE_RELEASE) {
            voice->envelope_value -= sample->level26;
            if (voice->envelope_value <= 0) {
                voice->envelope_value = 0;
                voice->envelope_state = AUDIO_LEVEL_STATE_OFF;
                voice->active = AUDIO_VOICE_IDLE;
                --audiochunktable[voice->resourceIndex].activeVoices;
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(U16, struct AudioVoice *))(audiodriverbinary + AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE))(voice->channelNumber, voice);
                audioblock[voice->resourceIndex] = 0;
            }
        }

        if (sample->loopMode != 0) {
            if (voice->modulation_delay != 0) {
                --voice->modulation_delay;
            } else if (voice->modulation_duration != 0) {
                if (voice->modulation_duration != AUDIO_LOOP_DURATION_INFINITE)
                    --voice->modulation_duration;
                if (voice->modulation_step_delay == 0) {
                    voice->modulation_step_delay = sample->loopCount;
                    if (voice->modulation_direction == AUDIO_LOOP_DIRECTION_SUBTRACT) {
                        voice->modulation_offset -= voice->modulation_step;
                        if ((abs(voice->modulation_offset)) >= sample->limit2e) {
                            if (sample->loopFlags & AUDIO_LOOP_REVERSE_AT_LOWER)
                                voice->modulation_direction = AUDIO_LOOP_DIRECTION_ADD;
                            else
                                voice->modulation_offset = 0;
                        }
                    } else {
                        voice->modulation_offset += voice->modulation_step;
                        if ((abs(voice->modulation_offset)) >= sample->limit2e) {
                            if (sample->loopFlags & AUDIO_LOOP_REVERSE_AT_UPPER)
                                voice->modulation_direction = AUDIO_LOOP_DIRECTION_SUBTRACT;
                            else
                                voice->modulation_offset = 0;
                        }
                    }
                } else {
                    --voice->modulation_step_delay;
                }
            }
        }

        if (sample->pulsePresent != 0) {
            if (voice->pulse_delay != 0) {
                --voice->pulse_delay;
            } else if (voice->pulse_duration != 0) {
                --voice->pulse_duration;
                if (voice->pulse_step_delay != 0) {
                    --voice->pulse_step_delay;
                } else {
                    voice->pulse_step_delay = sample->pulseCount;
                    voice->pulse_value = sample->pulseTable[voice->pulse_table_index++ & AUDIO_PULSE_INDEX_MASK];
                }
            }
        }
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
        ((void (FAR *)(U16, struct AudioVoice *, struct AudioChunk *, I8 FAR *))(audiodriverbinary + AUD_DRV_ENTRY_UPDATE_VOICE))(
            voice->channelNumber, voice, voice->resource, voice->data);
    }
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
    ((void (FAR *)(struct AudioVoice *))(audiodriverbinary + AUD_DRV_ENTRY_UPDATE_VOICE_ARRAY))(snd_voices_tbl);
}

/* Send the function-1Eh driver operation for the selected chunk channels.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): send function 1Eh to the DOS sound driver. */
void FAR _loadds audio_driver_func1E(I16 first, I16 last)
{
    I16 i;
    I16 j;

    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        for (i = 0; i < g_audiodrvvoices_count; i++) {
            if (snd_voices_tbl[i].resourceIndex <= last && snd_voices_tbl[i].resourceIndex >= first) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RESET_CHANNEL))(i);
                snd_voices_tbl[i].active = AUDIO_VOICE_IDLE;
                snd_voices_tbl[i].data = 0;
                snd_voices_tbl[i].resourceIndex = AUDIO_INVALID_RESOURCE_INDEX;
                snd_voices_tbl[i].priority = 0;
            }
        }
    } else {
        for (i = first; i <= last; i++) {
            if (audiochunktable[i].channelNumber < AUDIO_CHANNEL_NUMBER_LIMIT) {
    /* PLATFORM(audio): dispatch this event or voice operation to the loaded DOS audio driver. */
                ((void (FAR *)(I16))(audiodriverbinary + AUDIO_DRIVER_ENTRY_RESET_CHANNEL))(audiochunktable[i].channelNumber);
                for (j = 0; j < AUDIO_VOICE_CAPACITY; j++) {
                    if (snd_voices_tbl[j].resourceIndex == i) {
                        snd_voices_tbl[j].active = AUDIO_VOICE_IDLE;
                        snd_voices_tbl[j].data = 0;
                        snd_voices_tbl[j].resourceIndex = AUDIO_INVALID_RESOURCE_INDEX;
                        snd_voices_tbl[j].priority = 0;
                    }
                }
            }
        }
    }
    for (i = first; i <= last; i++)
        audiochunktable[i].activeVoices = 0;
}

U16S audio_timer_reentry_lock = 0;
