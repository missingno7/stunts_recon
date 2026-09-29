/* seg007 audio engine - whole-object reconstruction for the MSC 6.00 non-A C6 profile.
   Build: CL /c /AM /Os /Oe /Og /Gs /Zi UNIT.C  (MSC 6.00 non-A, DOS pass chain). Scratch only. */
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

struct AudioPayload {           /* 0x30 bytes */
    u16 sample_word;            /* 00 */
    u16 unk2;                   /* 02 */
    u16 unk4;                   /* 04 */
    u8 resource_ready;          /* 06 */
    u8 unk7;                    /* 07 */
    u8 far *shape;              /* 08 */
    u32 unkC;                   /* 0C */
    void far *resources[8];     /* 10 */
};

struct AudioTimer {             /* 0x4C bytes */
    u8 active;                  /* 00 */
    u8 state;                   /* 01 */
    int handle;                 /* 02 */
    u16 pitch_avg;              /* 04 */
    u32 rate_avg;               /* 06 */
    u8 pitch;                   /* 0A */
    u8 unkB;                    /* 0B */
    u16 rate;                   /* 0C */
    u8 last_pitch;              /* 0E */
    u8 unkF;                    /* 0F */
    int channels[4];            /* 10 */
    u16 sample_word;            /* 18 */
    u8 dirty;                   /* 1A */
    u8 retrigger;               /* 1B */
    struct AudioPayload payload;/* 1C */
};

static struct AudioTimer audio_timer_table[25];
static int audio_timer_count;
static char id_buffer[5];
extern unsigned char audioflag6;
extern unsigned char audio_driver_mode;

extern int far compare_ds_ss(void);
extern void far timer_reg_callback(void (far *callback)(void));
extern void far timer_remove_callback(void (far *callback)(void));
extern void far release_audio_chunk(int handle);
extern void far reset_audio_voice_length(int channel);
extern u8 far * far locate_shape_fatal(void far *lookup, char *name);
extern void far * far init_audio_resources(void far *resource, void far *lookup, char *name);
extern int far reserve_audio_chunk(int a, int b);
extern void far fatal_error(char *message);
extern void far start_audio_voice_sample(int handle, u8 far *shape);
extern short far send_audio_stop_event(u16 rate, int handle);
extern void far audio_unk2(int handle, int value);
extern void far set_audio_voice_value(int channel, int rate);
extern void far audio_init_chunk2(int channel);
extern int far audio_chunk_is_unavailable(int channel);
extern int far polarRadius2D(int x, int y);
extern int far audio_check_flag(void far *resource, int a, int b, int c);
extern void far debug_printf_text(char *format, int value);

void far audio_driver_timer(void);

void far audio_add_driver_timer(void)
{ /* PURPOSE: Clear audio timer slots and register the audio update callback. Params: none. Returns: void. Globals: reads audio_timer_table; writes audio_timer_count. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    struct AudioTimer *timer;

    for (timer = audio_timer_table; timer < audio_timer_table + 25; timer++)
        timer->active = 0;
    audio_timer_count = 0x16;
    timer_reg_callback(audio_driver_timer) /* PLATFORM(timer): register the audio update routine with the timer service. */;
}

void far audio_remove_driver_timer(void)
{ /* PURPOSE: Stop active audio handles, clear timer slots, and unregister the callback. Params: none. Returns: void. Globals: reads audio_timer_table; writes none. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    register struct AudioTimer *timer;

    for (timer = audio_timer_table; timer < audio_timer_table + 25; timer++) {
        if (timer->active == 1)
            release_audio_chunk(timer->handle) /* PLATFORM(audio): stop an audio timer handle. */;
        timer->active = 0;
    }
    timer_remove_callback(audio_driver_timer) /* PLATFORM(timer): remove the audio update routine from the timer service. */;
}

char * far pad_id(u32 far *id)
{ /* PURPOSE: Convert a four-byte resource id to a space-padded lookup string. Params: id. Returns: char *. Globals: reads id_buffer; writes id_buffer. */
    register int i;

    *(u32 *)id_buffer = *id;
    id_buffer[4] = 0;
    for (i = 0; i < 4; i++)
        if (id_buffer[i] == 0)
            id_buffer[i] = ' ';
    return id_buffer;
}

int far audio_init_engine(int unused, u8 huge *blob, void far *lookup, void far *resource)
{ /* PURPOSE: Copy an audio payload, resolve its resources, and allocate a timer slot. Params: unused, blob, lookup, resource. Returns: int. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    int slot;
    int i;
    struct AudioTimer *timer;
    struct AudioPayload far *payload;
    u8 huge *src;
    u8 *dst;

    slot = -1;
    for (i = 0, timer = audio_timer_table; timer < audio_timer_table + 25 && slot < 0; timer++, i++)
        if (timer->active == 0)
            slot = i;
    if (slot >= 0) {
        timer = &audio_timer_table[slot];
        payload = &timer->payload;
        src = blob;
        dst = (u8 *)payload;
        for (i = 0; i < 0x30; i++)
            *dst++ = *src++;
        if (payload->resource_ready == 0) {
            payload->shape = locate_shape_fatal(lookup, pad_id((u32 far *)payload->shape)) /* PLATFORM(file): locate a named shape in resource data. */;
            payload->resources[0] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[0])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[1] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[1])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[2] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[2])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[3] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[3])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[4] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[4])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[5] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[5])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[6] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[6])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resources[7] = init_audio_resources(resource, lookup, pad_id((u32 far *)payload->resources[7])) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
            payload->resource_ready = 1;
        }
        timer->handle = reserve_audio_chunk(-1, 0x7f) /* PLATFORM(audio): allocate an audio timer handle. */;
        timer->state = 0;
        timer->pitch_avg = 0;
        timer->rate_avg = 0;
        timer->pitch = 0;
        timer->rate = payload->sample_word / payload->shape[0x0e] + (payload->shape[0x0f] << 4);
        timer->last_pitch = 0xff;
        timer->channels[0] = -1;
        timer->channels[1] = -1;
        timer->channels[2] = -1;
        timer->channels[3] = -1;
        timer->sample_word = payload->sample_word;
        timer->retrigger = timer->dirty = 0;
        timer->active = 1;
        return slot;
    }
    fatal_error("InitEngine: All handles used.");
}

void far audio_op_unk(int index)
{ /* PURPOSE: Start the sample and channel state for an active audio timer slot. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;
    register struct AudioPayload *payload;

    timer = &audio_timer_table[index];
    if (timer->active == 1 && timer->state == 0) {
        payload = &timer->payload;
        start_audio_voice_sample(timer->handle, payload->shape) /* PLATFORM(audio): bind a sample shape to an audio handle. */;
        timer->rate = payload->sample_word / payload->shape[0x0e] + (payload->shape[0x0f] << 4);
        timer->channels[1] = send_audio_stop_event(timer->rate, timer->handle) /* PLATFORM(audio): start a sample channel at the requested rate. */;
        timer->dirty = timer->state = 1;
        audio_unk2(timer->handle, 0) /* PLATFORM(audio): update an audio handle parameter. */;
    }
}

void far audio_function2(int index)
{ /* PURPOSE: Stop the current playback channel and return the slot to its idle state. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    if (timer->active == 1 && timer->state == 1) {
        reset_audio_voice_length(timer->channels[1]) /* PLATFORM(audio): stop an audio channel. */;
        timer->channels[1] = -1;
        timer->state = 0;
        timer->dirty = 1;
    }
}

int audio_tick_divider = 0;

#pragma optimize("tl", on)
void far audio_driver_timer(void)
{ /* PURPOSE: Update active audio pitch and rate values from the timer callback. Params: none. Returns: void. Globals: reads audio_driver_mode, audio_tick_divider, audio_timer_table, audioflag6; writes audio_driver_mode, audio_tick_divider. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;
    register int value;
    int i;
    u8 pitch;

    if (!compare_ds_ss())
        return;
    if (++audio_tick_divider < 2 && audio_driver_mode)
        return;
    for (i = 0, timer = audio_timer_table; i < 25; timer++, i++) {
        if (timer->active == 0 || audioflag6 == 0)
            continue;
        timer->pitch_avg = ((timer->pitch << 4) + timer->pitch_avg * 7) >> 3;
        pitch = timer->pitch_avg >> 4;
        if (pitch != timer->last_pitch || timer->dirty) {
            value = pitch;
            audio_unk2(timer->handle, value) /* PLATFORM(audio): update an audio handle parameter. */;
            value -= 10;
            if (value < 0)
                value = 0;
            if (timer->channels[2] != -1)
                audio_unk2(timer->channels[2], value) /* PLATFORM(audio): update an audio handle parameter. */;
            if (timer->channels[3] != -1)
                audio_unk2(timer->channels[3], value) /* PLATFORM(audio): update an audio handle parameter. */;
            timer->last_pitch = pitch;
        }
        timer->rate_avg = (timer->rate_avg * 7 + ((u32)timer->rate << 4)) >> 3;
        value = (int)(timer->rate_avg >> 4);
        if ((value != timer->channels[0] || timer->dirty) && timer->channels[1] != -1) {
            set_audio_voice_value(timer->channels[1], value) /* PLATFORM(audio): update the playback rate for an audio channel. */;
            timer->channels[0] = value;
        }
        timer->dirty = 0;
        if (timer->retrigger) {
            if (timer->state) {
                audio_init_chunk2(timer->channels[2]) /* PLATFORM(audio): stop or reset an audio channel. */;
                timer->retrigger = 0;
            } else if (audio_chunk_is_unavailable(timer->channels[2]) /* PLATFORM(audio): query whether an audio channel is still active. */) {
                audio_op_unk(i) /* PLATFORM(audio): start playback for the selected timer slot. */;
                timer->retrigger = 0;
            }
        }
    }
    if (audio_tick_divider >= 2)
        audio_tick_divider = 0;
}
void far audio_op_unk2(int index, u16 sample_word, int x2, int y2, int z2, int x, int y, int z, u16 speed)
{ /* PURPOSE: Calculate distance-based audio volume and playback rate for a timer slot. Params: index, sample_word, x2, y2, z2, x, y, z, speed. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */
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
    if (curdist > 6000) {
        volume = 0;
        timer->pitch = 0;
        return;
    }
    distance2 = polarRadius2D(polarRadius2D(x2, z2), y2);
    approach = distance2 - curdist;
    _asm {
        mov ax, 100
        xor dx, dx
        div speed
        imul approach
        mov approach, ax
        mov ax, 127
        mul curdist
        mov cx, 6000
        div cx
        neg ax
        add ax, 127
        mov volume, ax
    }
    if (approach > 0)
        volume -= volume >> 4;
    hdr = &timer->payload;
    freq = sample_word / hdr->shape[0x0e] + (hdr->shape[0x0f] << 4);
    newrate = 6000 - approach;
    if (newrate != 0) {
        _asm {
            mov ax, 6000
            mul freq
            div newrate
            mov newrate, ax
        }
        timer->rate = newrate;
    }
    timer->pitch = (u8)volume;
}

#pragma optimize("tl", off)

void far nopsub_27220(int index)
{ /* PURPOSE: Start the first engine sample and mark its timer slot dirty. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[0], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    debug_printf_text("startengine() - new handle = %d\n", timer->channels[2]);
    timer->retrigger = timer->dirty = 1;
}

void far nopsub_2726C(int index)
{ /* PURPOSE: Start the second engine sample and stop its previous channel. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[1], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
    audio_function2(index) /* PLATFORM(audio): stop playback for the selected timer slot. */;
}

void far nopsub_272B0(int index)
{ /* PURPOSE: Start the third engine sample and stop its previous channel. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[2], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
    audio_function2(index) /* PLATFORM(audio): stop playback for the selected timer slot. */;
}

void far audio_function2_wrap(int index)
{ /* PURPOSE: Start the fourth engine sample and stop its previous channel. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[3], -1, 0x64, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
    audio_function2(index) /* PLATFORM(audio): stop playback for the selected timer slot. */;
}

void far audio_op_unk3(int index)
{ /* PURPOSE: Start the seventh engine sample and mark its timer slot dirty. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[6], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
}

void far audio_op_unk4(int index)
{ /* PURPOSE: Start the eighth engine sample and mark its timer slot dirty. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    struct AudioTimer *timer;

    timer = &audio_timer_table[index];
    timer->channels[2] = audio_check_flag(timer->payload.resources[7], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
}

void far audio_op_unk5(int index)
{ /* PURPOSE: Replace the fifth engine channel with its new sample. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;
    struct AudioPayload far *payload;

    timer = &audio_timer_table[index];
    payload = &timer->payload;
    if (timer->channels[3] != -1)
        audio_init_chunk2(timer->channels[3]) /* PLATFORM(audio): stop or reset an audio channel. */;
    timer->channels[3] = audio_check_flag(payload->resources[4], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
}

void far audio_op_unk6(int index)
{ /* PURPOSE: Replace the sixth engine channel with its new sample. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register struct AudioTimer *timer;
    struct AudioPayload far *payload;

    timer = &audio_timer_table[index];
    payload = &timer->payload;
    if (timer->channels[3] != -1)
        audio_init_chunk2(timer->channels[3]) /* PLATFORM(audio): stop or reset an audio channel. */;
    timer->channels[3] = audio_check_flag(payload->resources[5], -1, 0x40, timer->pitch_avg >> 4) /* PLATFORM(audio): start a named sample through the audio service. */;
    timer->dirty = 1;
}

void far audio_op_unk7(int index)
{ /* PURPOSE: Stop the fourth playback channel and clear its handle. Params: index. Returns: void. Globals: reads audio_timer_table; writes audio_timer_table. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    audio_init_chunk2(audio_timer_table[index].channels[3]) /* PLATFORM(audio): stop or reset an audio channel. */;
    audio_timer_table[index].channels[3] = -1;
}

int far nopsub_27489(int index)
{ /* PURPOSE: Report whether the secondary channel has finished playing. Params: index. Returns: int. Globals: reads audio_timer_table; writes none. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
    register int channel;

    channel = audio_timer_table[index].channels[2];
    if (channel > -1)
        return audio_chunk_is_unavailable(channel) /* PLATFORM(audio): query whether an audio channel is still active. */;
    return 1;
}
