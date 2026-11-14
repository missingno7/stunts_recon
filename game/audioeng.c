/* seg007 audio engine - whole-object reconstruction for the MSC 6.00 non-A C6 profile.
   Build: CL /c /AM /Os /Oe /Og /Gs /Zi UNIT.C  (MSC 6.00 non-A, DOS pass chain). Pinned target profile. */
#include "audstate.h"
#include "resource.h"

static struct AudioTimer audio_timer_table[AUDIO_ENGINE_TIMER_COUNT];
static int audio_timer_count;
static char id_buffer[RESOURCE_ID_STRING_BYTES];
/* MSC 6 /Zi: only called interfaces precede the public definitions; other API types shift PUBLIC type indices. */
#include "auddrv.h"
#include "radius2d.h"
#include "tmrhook.h"
/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);
/* Local view: abortmessage; ui.h changes _audio_add_driver_timer PUBLIC type index 552 to 645. */
extern void far abortmessage(char *);


/* Local view: set_audio_voice_value; MSC 6 /Zi requires the original ordinary far entry type; the shared entry uses _loadds. */
extern void far set_audio_voice_value(int channel, int rate);
/* Local view: debug_printf_text; ui.h changes _audio_add_driver_timer PUBLIC type index 552 to 645. */
extern void far debug_printf_text(char *, int);
/* Clears audio timer slots and registers the audio update callback. */
void far audio_add_driver_timer(void)
{
    struct AudioTimer *timer;

    for (timer = audio_timer_table; timer < audio_timer_table + AUDIO_ENGINE_TIMER_COUNT; timer++)
        timer->active = 0;
    audio_timer_count = 0x16;
    addtimer(audio_driver_timer);
}
/* Stops active audio handles, clears timer slots, and unregisters the callback. */
void far audio_remove_driver_timer(void)
{
    register struct AudioTimer *timer;

    for (timer = audio_timer_table; timer < audio_timer_table + AUDIO_ENGINE_TIMER_COUNT; timer++) {
        if (timer->active == 1)
            release_audio_chunk(timer->handle);
        timer->active = 0;
    }
    deltimer(audio_driver_timer);
}
/* Converts a four-byte resource id to a space-padded lookup string. */
char * far pad_id(u32 far *id)
{
    register int i;

    *(u32 *)id_buffer = *id;
    id_buffer[RESOURCE_NAME_BYTES] = 0;
    for (i = 0; i < RESOURCE_NAME_BYTES; i++)
        if (id_buffer[i] == 0)
            id_buffer[i] = ' ';
    return id_buffer;
}
/* Copies an audio payload, resolves its resources, and allocates a timer slot. */
/* Local view: audio_init_engine; parameters[1].pointer is unsigned byte; another unit uses void. */
int far audio_init_engine(int unused, u8 huge *blob, void far *lookup, void far *resource)
{
    int slot;
    int i;
    struct AudioTimer *timer;
    struct AudioPayload far *payload;
    u8 huge *src;
    u8 *dst;

    slot = AUDIO_INVALID_HANDLE;
    for (i = 0, timer = audio_timer_table; timer < audio_timer_table + AUDIO_ENGINE_TIMER_COUNT && slot < 0; timer++, i++)
        if (timer->active == 0)
            slot = i;
    if (slot >= 0) {
        timer = &audio_timer_table[slot];
        payload = &timer->payload;
        src = blob;
        dst = (u8 *)payload;
        for (i = 0; i < AUDIO_ENGINE_PROFILE_BYTES; i++)
            *dst++ = *src++;
        if (payload->resource_ready == 0) {
            payload->shape = locate_shape_fatal(lookup, pad_id((u32 far *)payload->shape));
            payload->resources[AUDIO_ENGINE_RESOURCE_0] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_0]));
            payload->resources[AUDIO_ENGINE_RESOURCE_1] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_1]));
            payload->resources[AUDIO_ENGINE_RESOURCE_2] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_2]));
            payload->resources[AUDIO_ENGINE_RESOURCE_3] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_3]));
            payload->resources[AUDIO_ENGINE_RESOURCE_4] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_4]));
            payload->resources[AUDIO_ENGINE_RESOURCE_5] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_5]));
            payload->resources[AUDIO_ENGINE_RESOURCE_6] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_6]));
            payload->resources[AUDIO_ENGINE_RESOURCE_7] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[AUDIO_ENGINE_RESOURCE_7]));
            payload->resource_ready = 1;
        }
        timer->handle = reserve_audio_chunk(AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_ENGINE_RESERVED);
        timer->state = 0;
        timer->pitch_avg = 0;
        timer->rate_avg = 0;
        timer->pitch = 0;
        timer->rate = payload->sample_word / payload->shape[AUDIO_ENGINE_SHAPE_RATE_DIVISOR_OFFSET] + (payload->shape[AUDIO_ENGINE_SHAPE_RATE_BIAS_OFFSET] << AUDIO_ENGINE_SHAPE_RATE_BIAS_SHIFT);
        timer->last_pitch = AUDIO_ENGINE_LAST_VOLUME_INVALID;
        timer->channels[AUDIO_ENGINE_SLOT_CACHED_RATE] = AUDIO_INVALID_HANDLE;
        timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE] = AUDIO_INVALID_HANDLE;
        timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = AUDIO_INVALID_HANDLE;
        timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] = AUDIO_INVALID_HANDLE;
        timer->sample_word = payload->sample_word;
        timer->retrigger = timer->dirty = 0;
        timer->active = 1;
        return slot;
    }
    abortmessage("InitEngine: All handles used.");
}
/* Starts the sample and channel state for an active audio timer slot. */
void far audio_op_unk(int index)
{
    register struct AudioTimer *timer;
    register struct AudioPayload *payload;

    timer = &audio_timer_table[index];
    if (timer->active == 1 && timer->state == 0) {
        payload = &timer->payload;
        start_audio_voice_sample(timer->handle, payload->shape);
        timer->rate = payload->sample_word / payload->shape[AUDIO_ENGINE_SHAPE_RATE_DIVISOR_OFFSET] + (payload->shape[AUDIO_ENGINE_SHAPE_RATE_BIAS_OFFSET] << AUDIO_ENGINE_SHAPE_RATE_BIAS_SHIFT);
        timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE] = send_audio_stop_event(timer->rate, timer->handle);
        timer->dirty = timer->state = 1;
        audio_set_chunk_volume(timer->handle, 0);
    }
}
/* Stops the current playback channel and returns the slot to its idle state. */
void far audio_function2(int index)
{
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    if (timer->active == 1 && timer->state == 1) {
        reset_audio_voice_length(timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE]);
        timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE] = AUDIO_INVALID_HANDLE;
        timer->state = 0;
        timer->dirty = 1;
    }
}

int audio_tick_divider = 0;
/* Updates active audio pitch and rate values from the timer callback. */
#pragma optimize("tl", on)
void far audio_driver_timer(void)
{
    register struct AudioTimer *timer;
    register int value;
    int i;
    u8 pitch;

    if (!sseqds())
        return;
    if (++audio_tick_divider < AUDIO_ENGINE_CHANNEL_MODE_TICK_DIVIDER && audio_driver_mode)
        return;
    for (i = 0, timer = audio_timer_table; i < AUDIO_ENGINE_TIMER_COUNT; timer++, i++) {
        if (timer->active == 0 || audioflag6 == 0)
            continue;
        timer->pitch_avg = ((timer->pitch << AUDIO_ENGINE_SMOOTH_FRACTION_BITS) + timer->pitch_avg * AUDIO_ENGINE_SMOOTH_HISTORY_WEIGHT) >> AUDIO_ENGINE_SMOOTH_DIV_SHIFT;
        pitch = timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS;
        if (pitch != timer->last_pitch || timer->dirty) {
            value = pitch;
            audio_set_chunk_volume(timer->handle, value);
            value -= AUDIO_ENGINE_SECONDARY_VOLUME_BIAS;
            if (value < 0)
                value = 0;
            if (timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] != AUDIO_INVALID_HANDLE)
                audio_set_chunk_volume(timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT], value);
            if (timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] != AUDIO_INVALID_HANDLE)
                audio_set_chunk_volume(timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT], value);
            timer->last_pitch = pitch;
        }
        timer->rate_avg = (timer->rate_avg * AUDIO_ENGINE_SMOOTH_HISTORY_WEIGHT + ((u32)timer->rate << AUDIO_ENGINE_SMOOTH_FRACTION_BITS)) >> AUDIO_ENGINE_SMOOTH_DIV_SHIFT;
        value = (int)(timer->rate_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
        if ((value != timer->channels[AUDIO_ENGINE_SLOT_CACHED_RATE] || timer->dirty) && timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE] != AUDIO_INVALID_HANDLE) {
            set_audio_voice_value(timer->channels[AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE], value);
            timer->channels[AUDIO_ENGINE_SLOT_CACHED_RATE] = value;
        }
        timer->dirty = 0;
        if (timer->retrigger) {
            if (timer->state) {
                audio_init_chunk2(timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT]);
                timer->retrigger = 0;
            } else if (audio_chunk_is_unavailable(timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT])) {
                audio_op_unk(i);
                timer->retrigger = 0;
            }
        }
    }
    if (audio_tick_divider >= AUDIO_ENGINE_CHANNEL_MODE_TICK_DIVIDER)
        audio_tick_divider = 0;
} /* It calculates distance-based audio volume and playback rate for a timer slot. */
void far audio_op_unk2(int index, u16 sample_word, int x2, int y2, int z2, int x, int y, int z, u16 speed)
{
    struct AudioTimer *timer;
    u16 newrate;
    int temp;
    u16 volume;
    int distance2;
    int approach;
    u16 freq;
    int n;
    struct AudioPayload far *hdr;
    int curdist;

    timer = &audio_timer_table[index];
    curdist = polarRadius2D(polarRadius2D(x, z), y);
    if (curdist > AUDIO_ENGINE_AUDIBLE_DISTANCE) {
        volume = 0;
        timer->pitch = 0;
        return;
    }
    distance2 = polarRadius2D(polarRadius2D(x2, z2), y2);
    approach = distance2 - curdist;
    _asm {
        mov ax, AUDIO_ENGINE_APPROACH_SCALE
        xor dx, dx
        div speed
        imul approach
        mov approach, ax
        mov ax, AUDIO_ENGINE_ATTENUATION_FULL_SCALE
        mul curdist
        mov cx, AUDIO_ENGINE_AUDIBLE_DISTANCE
        div cx
        neg ax
        add ax, AUDIO_ENGINE_ATTENUATION_FULL_SCALE
        mov volume, ax
    }
    if (approach > 0)
        volume -= volume >> AUDIO_ENGINE_RECEDING_ATTENUATION_SHIFT;
    hdr = &timer->payload;
    freq = sample_word / hdr->shape[AUDIO_ENGINE_SHAPE_RATE_DIVISOR_OFFSET] + (hdr->shape[AUDIO_ENGINE_SHAPE_RATE_BIAS_OFFSET] << AUDIO_ENGINE_SHAPE_RATE_BIAS_SHIFT);
    newrate = AUDIO_ENGINE_DOPPLER_BASE - approach;
    if (newrate != 0) {
        _asm {
            mov ax, AUDIO_ENGINE_DOPPLER_BASE
            mul freq
            div newrate
            mov newrate, ax
        }
        timer->rate = newrate;
    }
    timer->pitch = (u8)volume;
}

#pragma optimize("tl", off)
/* Starts the first engine sample and marks its timer slot dirty. */
void far nopsub_27220(int index)
{
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_0], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    debug_printf_text("startengine() - new handle = %d\n", timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT]);
    timer->retrigger = timer->dirty = 1;
}
/* Starts the second engine sample and stops its previous channel. */
void far nopsub_2726C(int index)
{
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_1], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
    audio_function2(index);
}
/* Starts the third engine sample and stops its previous channel. */
void far nopsub_272B0(int index)
{
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_2], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
    audio_function2(index);
}
/* Starts the fourth engine sample and stops its previous channel. */
void far audio_function2_wrap(int index)
{
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_3], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_ENGINE_FOURTH_RESOURCE, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
    audio_function2(index);
}
/* Starts the seventh engine sample and marks its timer slot dirty. */
void far audio_op_unk3(int index)
{
    struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_6], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
}
/* Starts the eighth engine sample and marks its timer slot dirty. */
void far audio_op_unk4(int index)
{
    struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT] = audio_check_flag(timer->payload.resources[AUDIO_ENGINE_RESOURCE_7], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
}
/* Replaces the fifth engine channel with its new sample. */
void far audio_op_unk5(int index)
{
    register struct AudioTimer *timer;
    struct AudioPayload far *payload;

    timer = &audio_timer_table[index];
    payload = &timer->payload;
    if (timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] != AUDIO_INVALID_HANDLE)
        audio_init_chunk2(timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT]);
    timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] = audio_check_flag(payload->resources[AUDIO_ENGINE_RESOURCE_4], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
}
/* Replaces the sixth engine channel with its new sample. */
void far audio_op_unk6(int index)
{
    register struct AudioTimer *timer;
    struct AudioPayload far *payload;

    timer = &audio_timer_table[index];
    payload = &timer->payload;
    if (timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] != AUDIO_INVALID_HANDLE)
        audio_init_chunk2(timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT]);
    timer->channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] = audio_check_flag(payload->resources[AUDIO_ENGINE_RESOURCE_5], AUDIO_INVALID_HANDLE, AUDIO_PRIORITY_EFFECT_DEFAULT, timer->pitch_avg >> AUDIO_ENGINE_SMOOTH_FRACTION_BITS);
    timer->dirty = 1;
}
/* Stops the fourth playback channel and clears its handle. */
void far audio_op_unk7(int index)
{
    audio_init_chunk2(audio_timer_table[index].channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT]);
    audio_timer_table[index].channels[AUDIO_ENGINE_SLOT_SECONDARY_EFFECT] = AUDIO_INVALID_HANDLE;
}
/* Reports whether the secondary channel has finished playing. */
int far nopsub_27489(int index)
{
    register int channel;

    channel = audio_timer_table[index].channels[AUDIO_ENGINE_SLOT_PRIMARY_EFFECT];
    if (channel > AUDIO_INVALID_HANDLE)
        return audio_chunk_is_unavailable(channel);
    return 1;
}
