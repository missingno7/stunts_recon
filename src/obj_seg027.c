#include "stunts_types.h"
static U8 saved_music_chunk_volumes[24];
static U8 saved_effect_chunk_volumes[24];

/* obj_seg027 (audio): complete object [159954,165436), MSC 5.10 /AM /Ox /Gs (TUFLAG-Ox-seg027).
 * Externs use Restunts dseg/seg027 spellings.  Source/target notes:
 * - AUDIOCHUNK is one 24-entry table (audio_init_chunk indexes 0..23 from one base);
 *   its dword at +5 is unaligned, hence #pragma pack(1) (register choice in
 *   audio_init_chunk depends on a real member, not a cast of an array member).
 * - The initialized globals below init_audio_resources are TU-owned _DATA
 *   (literal "hdr1" precedes them in the image); audio_bit_masks is not referenced
 *   by this object but occupies DGROUP 0x4E9E..0x4EBF between them.
 * - link_audio_shape_resources: count is assigned and never read (target stores it to its home),
 *   name[5] matches the target's 6-byte buffer home. */

/* PORT: Packed audio records depend on 16-bit far-pointer width and MSC field packing. */
#pragma pack(1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack()
/* READABILITY: Load and drive the DOS audio module, schedule its timer callback, and resolve audio resources. */

struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};

struct AUDIOCHUNK audiochunktable[24];
struct AUDIOVOICE snd_voices_tbl[16];
extern U8 saved_music_chunk_volumes[];
extern U8 saved_effect_chunk_volumes[];
unsigned char block_audio_num;
unsigned char audioblock[24];
unsigned char g_audchnkvalue[24];
unsigned char sfx_audio_vol;
unsigned char g_musicvolumesetting;
extern U8 g_audiodrvvoices_count;
unsigned char audiochnk_actflags[24];
extern I16 snd_sample_rate_phase, mus_samplelimit;
char g_drvaudiocode[14];
void far *kick_res;
void far *g_snaresnd;
void far *tommsampleresource;
void far *ride_audio_sound_res;
void far *audio_opp_res;
void far *chhtsample;
void far *resource_sound_hit;


void FAR * FAR audioresource_find(void FAR *resource, I8 *name);
void FAR audio_unk2(I16 index, U8 value);
void FAR audio_driver_func1E(I16 first, I16 last);
void FAR reset_audio_event_state(void);
I8 * FAR audio_make_filename(I8 *name, I8 *ext, I8 *kind);
    /* PLATFORM(file): load or decompress the named audio resource. */
void FAR * FAR file_load_binary_nofatal(I8 *name);
    /* PLATFORM(file): load or decompress the named audio resource. */
void FAR * FAR file_decomp_nofatal(I8 *name);
    /* PLATFORM(memory): release storage through the game memory manager. */
void FAR mmgr_release(void FAR *ptr);
void FAR fatal_error(I8 *format, ...);
void FAR add_exit_handler(void (FAR *handler)(void));
    /* PLATFORM(timer): use or register the game timer service. */
void FAR timer_reg_callback(void (FAR *callback)(void));
    /* PLATFORM(timer): use or register the game timer service. */
void FAR timer_remove_callback(void (FAR *callback)(void));
void FAR audiodriver_timer(void);
    /* PLATFORM(timer): use or register the game timer service. */
void FAR timer_copy_counter(I32 ticks);
    /* PLATFORM(timer): use or register the game timer service. */
void FAR timer_wait_for_dx(void);
    /* PLATFORM(file): resolve an audio resource entry in the loaded bundle. */
void FAR * FAR locate_shape_nofatal(void FAR *shapes, I8 *name);
I16 FAR audioresource_get_chunk_index(I16 start, I16 count, I8 *name, I8 FAR *names);
I16 FAR audioresource_compare_chunknames(I16 flag, I8 FAR *name1, I8 FAR *name2, I16 length);
void FAR audioresource_copy_n_bytes(U8 FAR *src, U8 FAR *dst, I16 count);
void FAR debug_printf_text(I8 *format, ...);
void FAR flush_stdin(void);
U16 strlen(const I8 *s);

void FAR audio_map_song_instruments(void FAR *song, void FAR *voice);
void FAR audio_map_song_tracks(void FAR *song);
void FAR audioresource_copy_4_bytes(U8 FAR *dst, U8 FAR *src);
U32 FAR audioresource_get_dword(U32 FAR *p);
U16 FAR audioresource_get_word(U16 FAR *p);
void FAR audio_init_chunk(I16 first, I16 last, void FAR *res, I16 offset, U8 volume, U8 priority);
I16 FAR audio_check_flag(void FAR *res, I16 chunk, U8 priority, U16 volume);
void FAR audio_init_chunk2(I16 chunk);
void FAR reset_audio_chunks(void);
void FAR set_all_audio_chunk_volume(I16 value);
void FAR reset_audio_driver_state(void);
void FAR audiodrv_atexit(void);

typedef void (FAR *DRVPROC)();

void FAR * FAR init_audio_resources(void FAR *song, void FAR *voice, I8 *name)
{
    void FAR *titleres;
    void FAR *hdrptr;
    I8 FAR *trackptr;

    if ((titleres = audioresource_find(song, name)) == 0)
        return 0;
    hdrptr = audioresource_find(titleres, "hdr1");
    if (hdrptr == 0)
        return 0;
    if (((I8 FAR *)hdrptr)[5] != 1) {
        audio_map_song_instruments(titleres, voice);
        audio_map_song_tracks(titleres);
        ((I8 FAR *)hdrptr)[5] = 1;
        trackptr = (I8 FAR *)titleres + ((U16)((U8 FAR *)titleres)[4] << 3) + 1;
        audioresource_copy_4_bytes((U8 FAR *)hdrptr, (U8 FAR *)&trackptr);
    }
    return hdrptr;
}

void FAR *audiodriverbinary = 0;
U16 audio_bit_masks[17] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};
U8 audio_pause_in_progress = 0;
U8 audioflag2 = 1;
U8 audio_song_ready = 0;
U8 audioflag6 = 1;
U8 audio_driver_mode = 0;
U8 audio_driver_extension_mode = 0;
U8 audio_driver_volume_command[4] = { 0x10, 0x00, 0x16, 0x00 };
I16 audio_update_lock = 1;
I16 audio_load_error_policy = 0;

/* Call the loaded audio driver finalization entry.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): dispatch into the loaded DOS sound-driver image. */
void FAR load_audio_finalize(void FAR *song)
{
    I16 offset;

    audio_update_lock = 1;
    reset_audio_chunks();
    if (song == 0) return;
    if (((I8 FAR *)song)[4] != 0) return;
    if (((I8 FAR *)song)[5] != 1) return;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    /* PORT: This offset selects a driver entry inside a loaded segment; preserve 16:16 pointer semantics. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x18))();
    snd_sample_rate_phase = 0;
    mus_samplelimit = 0x80;
    offset = (((U8 FAR *)song)[6] << 2) + 7;
    block_audio_num = ((I8 FAR *)song)[offset++];
    audio_init_chunk(0, block_audio_num - 1, song, offset, g_musicvolumesetting, 0x20);
    audio_song_ready = 1;
    audio_update_lock = 0;
}

/* Send the currently prepared volume command to the loaded audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): dispatch the prepared volume command into the DOS sound driver. */
void FAR audio_unk(void)
{
    struct AUDIOVOICE *channel;
    I16 i;

    audio_pause_in_progress = 1;
    audio_update_lock = 1;
    if (audio_driver_mode == 0) {
        for (i = 0; i < 24; i++) {
            if (audioflag6 == 1 || i < 16) {
                saved_music_chunk_volumes[i] = audiochunktable[i].volume;
                audio_unk2(i, 0);
            }
        }
    } else {
        audio_driver_volume_command[3] = 0;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
    }
    if (audio_driver_mode == 0) {
        for (i = 0; i < 16; i++) {
            channel = &snd_voices_tbl[i];
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x27))(channel->unk2C, channel, channel->unk2A, channel->unk10);
        }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x30))(snd_voices_tbl);
    }
    audio_update_lock = 0;
}

/* Restore the default volume through the loaded audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): restore output volume through the DOS sound driver. */
void FAR restore_audio_volume(void)
{
    I16 i;

    audio_pause_in_progress = 1;
    audio_update_lock = 1;
    if (audio_driver_mode == 0) {
        for (i = 0; i < 24; i++) {
            if (audioflag6 == 1 || i < 16)
                audio_unk2(i, saved_music_chunk_volumes[i]);
        }
    } else {
        audio_driver_volume_command[3] = 100;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
    }
    audio_update_lock = 0;
    audio_pause_in_progress = 0;
}

void FAR reset_audio_chunks(void)
{
    audio_update_lock = 1;
    audio_song_ready = 0;
    audio_driver_func1E(0, 0x0f);
    audio_init_chunk(0, 0x0f, 0, 0, g_musicvolumesetting, 0);
    block_audio_num = 0;
    reset_audio_event_state();
    audio_update_lock = 0;
}

void FAR audio_enable_flag2(void) { audioflag2 = 1; }
void FAR audio_disable_flag2(void) { audioflag2 = 0; audio_update_lock = 1; if (block_audio_num) { audio_driver_func1E(0, (U16)block_audio_num - 1); } reset_audio_event_state(); audio_update_lock = 0; }
I16 FAR audio_toggle_flag2(void) { if (audioflag2 == 1) { audio_disable_flag2(); return 0; } audio_enable_flag2(); return 1; }

I16 FAR nopsub_373FE(void)
{
    I16 i;
    if (audio_pause_in_progress == 1 || audioflag2 == 0)
        return 1;
    for (i = 0; i < block_audio_num; i++)
        if (audiochunktable[i].data != 0)
            return 0;
    return 1;
}

I16 FAR audio_check_flag2(void FAR *res, I16 chunk, U8 priority);
I16 FAR nopsub_37456(void FAR *res)
{
    return audio_check_flag2(res, -1, 0x40);
}

I16 FAR reserve_audio_chunk(I16 chunk, U8 priority)
{
    I16 i;
    if (chunk == -1) {
        for (i = 16; i <= 23; i++) {
            if (chunk == -1) {
                if (audiochunktable[i].data == 0 && audiochnk_actflags[i] == 0)
                    chunk = i;
            } else
                break;
        }
        if (chunk != -1) {
            audiochnk_actflags[chunk] = 1;
            audiochunktable[chunk].priority = priority;
        }
        return chunk;
    }
    audiochnk_actflags[chunk] = 1;
    audiochunktable[chunk].priority = priority;
    return chunk;
}

void FAR release_audio_chunk(I16 chunk)
{
    if (chunk > -1) {
        audiochnk_actflags[chunk] = 0;
        audio_init_chunk2(chunk);
    }
}

I16 FAR audio_check_flag2(void FAR *res, I16 chunk, U8 priority)
{
    return audio_check_flag(res, chunk, priority, sfx_audio_vol);
}

I16 FAR audio_check_flag(void FAR *res, I16 chunk, U8 priority, U16 volume)
{
    I16 i;
    U16 best;
    I16 offset;

    if (audioflag6 == 0)
        return -1;
    if (res == 0)
        return -1;
    if (((I8 FAR *)res)[5] != 1)
        return -1;
    if (sfx_audio_vol != 0)
        volume = (volume << 7) / sfx_audio_vol - 1;
    else
        volume = 0;
    if (chunk == -1) {
        for (i = 16; i <= 23; i++) {
            if (chunk == -1) {
                if (audiochunktable[i].data == 0 && audiochnk_actflags[i] == 0)
                    chunk = i;
            } else
                break;
        }
        if (chunk == -1) {
            best = 0xff;
            for (i = 16; i < 23; i++) {
                if (audiochunktable[i].priority <= best && audiochnk_actflags == 0) {
                    best = audiochunktable[i].priority;
                    chunk = i;
                }
            }
            if (chunk != -1 && audiochunktable[chunk].priority <= priority) {
                if (audiochnk_actflags[chunk] != 0)
                    audiochnk_actflags[chunk] = 0;
                audio_init_chunk2(chunk);
            }
        }
    }
    if (chunk == -1)
        return -1;
    offset = (((U8 FAR *)res)[6] << 2) + 8;
    audio_init_chunk(chunk, chunk, res, offset, volume, priority);
    return chunk;
}

void FAR audio_init_chunk2(I16 chunk)
{
    if (chunk < 16 || chunk > 23) return;
    audiochunktable[chunk].data = 0;
    audio_driver_func1E(chunk, chunk);
    audio_init_chunk(chunk, chunk, 0, 0, sfx_audio_vol, 0);
}

void FAR audio_enable_flag6(void)
{
    I16 i;
    if (audioflag6 != 1) {
        for (i = 16; i < 24; i++)
            audio_unk2(i, saved_effect_chunk_volumes[i]);
        audioflag6 = 1;
    }
}

void FAR audio_disable_flag6(void)
{
    I16 i;
    if (audioflag6 != 0) {
        for (i = 16; i < 24; i++) {
            saved_effect_chunk_volumes[i] = audiochunktable[i].volume;
            audio_unk2(i, 0);
        }
        audioflag6 = 0;
    }
}

I16 FAR audio_toggle_flag6(void)
{
    if (audioflag6 == 1) {
        audio_disable_flag6();
        return 0;
    }
    audio_enable_flag6();
    return 1;
}

I16 FAR audio_chunk_is_unavailable(I16 chunk)
{
    if (audioflag6 == 0) return 1;
    if (chunk < 16 || chunk > 23) return 1;
    if (audiochunktable[chunk].data == 0) return 1;
    return 0;
}

void FAR nopsub_37750(U16 chunk, I32 value)
{
    audiochunktable[chunk].unk48 = value;
}

/* Change the driver volume while waiting against the game timer.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(timer): pace the audio volume transition using the DOS timer. */
/* PLATFORM(audio): send volume commands through the loaded sound driver. */
void FAR audio_driver_func3F(I16 ticks)
{
    I16 counter;

    if (audio_driver_mode == 0) {
        for (counter = g_musicvolumesetting; counter > 0; counter -= 2) {
            audio_update_lock = 1;
            set_all_audio_chunk_volume(counter);
            audio_update_lock = 0;
    /* PLATFORM(timer): use or register the game timer service. */
            timer_copy_counter((I32)ticks);
    /* PLATFORM(timer): use or register the game timer service. */
            timer_wait_for_dx();
        }
    } else {
        for (counter = 100; counter > 0; counter -= 2) {
            audio_update_lock = 1;
            audio_driver_volume_command[3] = (U8)counter;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
            audio_update_lock = 0;
    /* PLATFORM(timer): use or register the game timer service. */
            timer_copy_counter((I32)ticks);
    /* PLATFORM(timer): use or register the game timer service. */
            timer_wait_for_dx();
        }
    }
    reset_audio_chunks();
    if (audio_driver_mode != 0) {
    /* PLATFORM(timer): use or register the game timer service. */
        timer_copy_counter(50L);
    /* PLATFORM(timer): use or register the game timer service. */
        timer_wait_for_dx();
        audio_driver_volume_command[3] = 100;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
    }
}

void FAR set_all_audio_chunk_volume(I16 value)
{
    I16 index;
    index = 0;
    while (index < block_audio_num) {
        audio_unk2(index, value);
        ++index;
    }
}

void FAR nopsub_37898(I16 value)
{
    g_musicvolumesetting = value;
    set_all_audio_chunk_volume(value);
}

U16 FAR nopsub_378AE(I16 index) { return g_audchnkvalue[index]; }

U16 FAR nopsub_378BC(I16 index) { return audioblock[index]; }

/* Load and initialize the audio driver, install its timer callback, and optionally load patches.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): load and initialize the DOS sound-driver module. */
/* PLATFORM(timer): register the audio callback with the game timer. */
I16 FAR audio_load_driver(I8 *filename, I16 unused, I16 signature)
{
    U16 len;
    I8 *path;
    void FAR *patches;

    if (signature == 0x473a)
        audio_driver_extension_mode = 1;
    if (audiodriverbinary != 0)
        audiodrv_atexit();
    else
        add_exit_handler(audiodrv_atexit);
    audiodriverbinary = 0;
    len = strlen(filename);
    while (len != 0 && filename[len] != '\\' && filename[len] != ':')
        len--;
    if (len != 0)
        len++;
    g_drvaudiocode[0] = filename[len];
    g_drvaudiocode[1] = filename[len + 1];
    g_drvaudiocode[2] = 0;
    path = audio_make_filename(filename, "drv", "");
    /* PLATFORM(file): load or decompress the named audio resource. */
    audiodriverbinary = file_load_binary_nofatal(path);
    g_musicvolumesetting = 0x7f;
    sfx_audio_vol = 0x7f;
    if (audiodriverbinary == 0)
        goto fail;
    g_audiodrvvoices_count = ((U8 (FAR *)(void))audiodriverbinary)();
    if (g_audiodrvvoices_count == 0 || g_audiodrvvoices_count == 0xff)
        return 2;
    if (g_audiodrvvoices_count > 0x7f) {
        g_audiodrvvoices_count = 16;
        audio_driver_mode = 1;
        audio_driver_extension_mode = 0;
    }
    reset_audio_driver_state();
    /* PLATFORM(timer): use or register the game timer service. */
    timer_reg_callback(audiodriver_timer);
    if (audio_driver_mode != 0) {
    /* PLATFORM(file): load or decompress the named audio resource. */
        patches = file_load_binary_nofatal("mt32.plb");
        if (patches != 0) {
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x42))(patches);
    /* PLATFORM(memory): release storage through the game memory manager. */
            mmgr_release(patches);
            audio_driver_volume_command[3] = 100;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
        }
    }
    audio_pause_in_progress = 0;
    audioflag2 = 1;
    audio_song_ready = 0;
    audioflag6 = 1;
    return 0;
fail:
    fatal_error("Can't find driver!\n");
}

/* Remove the audio timer callback, shut down the driver, and release its loaded image.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): shut down the DOS sound driver. */
/* PLATFORM(timer): remove the audio callback from the game timer. */
void FAR audiodrv_atexit(void)
{
    audio_update_lock = 1;
    if (audiodriverbinary != 0) {
    /* PLATFORM(timer): use or register the game timer service. */
        timer_remove_callback(audiodriver_timer);
        audioflag2 = 0;
        audioflag6 = 0;
        if (audio_driver_mode != 0) {
            audio_driver_volume_command[3] = 100;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x3f))(4, (void FAR *)audio_driver_volume_command);
        }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 6))();
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 3))();
    /* PLATFORM(memory): release storage through the game memory manager. */
        mmgr_release(audiodriverbinary);
        audiodriverbinary = 0;
        audio_driver_mode = 0;
        audio_driver_extension_mode = 0;
    }
    audio_update_lock = 0;
}

/* Load or decompress an audio resource using its filename, extension, and resource kind.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): fetch or decompress sound data through the game file service. */
void FAR * FAR load_sfx_ge(I8 *filename, I8 *extension, I8 *kind)
{
    I8 buffer[4];
    void FAR *resource;

    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, kind));
    if (resource != 0)
        return resource;
    buffer[0] = 'P';
    buffer[1] = extension[0];
    buffer[2] = extension[1];
    buffer[3] = 0;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, kind));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, "ge"));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, "ge"));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, ""));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, ""));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = file_load_binary_nofatal(filename);
    if (resource != 0)
        return resource;
    return resource;
}

void FAR set_audio_load_error_policy(I16 value)
{
    audio_load_error_policy = value;
}

void FAR * FAR load_sfx_file(I8 *filename)
{
    void FAR *result;

    result = 0;
    if (audio_driver_extension_mode != 0)
        result = load_sfx_ge(filename, "dsf", g_drvaudiocode);
    if (result == 0)
        result = load_sfx_ge(filename, "sfx", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        fatal_error("cannot load sfx file %s", filename);
    return result;
}

void FAR * FAR load_song_file(I8 *filename)
{
    void FAR *result;

    result = 0;
    result = load_sfx_ge(filename, "kms", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        fatal_error("cannot load song file %s", filename);
    return result;
}

void FAR * FAR load_voice_file(I8 *filename)
{
    void FAR *result;

    result = 0;
    if (audio_driver_extension_mode != 0)
        result = load_sfx_ge(filename, "dvc", g_drvaudiocode);
    if (result == 0)
        result = load_sfx_ge(filename, "vce", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        fatal_error("cannot load voice file %s", filename);
    return result;
}

void FAR * FAR nopsub_37D7A(I8 *filename)
{
    void FAR *result;

    result = load_sfx_ge(filename, "slb", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        fatal_error("cannot load sample file %s", filename);
    return result;
}

void FAR audio_init_chunk(I16 first, I16 last, void FAR *res, I16 offset, U8 volume, U8 priority)
{
    struct AUDIOCHUNK *chunk;
    I16 i;
    I8 FAR *p;

    for (i = first; i <= last; i++) {
        chunk = &audiochunktable[i];
        chunk->unk48 = 0;
        chunk->unk22 = 0x7f;
        chunk->index = i;
        chunk->unk16 = 0x0f;
        g_audchnkvalue[i] = 0;
        audioblock[i] = 0;
        chunk->unk32 = 0;
        chunk->unk04 = 0;
        chunk->priority = priority;
        chunk->unk15 = 0;
        chunk->unk18 = 0;
        chunk->unk1C = 0;
        chunk->unk1E = 0;
        chunk->volume = volume;
        chunk->unk25 = 0;
        chunk->unk26 = 0;
        chunk->unk29 = 0;
        chunk->unk2A = 0;
        chunk->unk2B = 0;
        chunk->unk2C = 0;
        chunk->unk47 = 0xff;
        if (res != 0) {
            p = (I8 FAR *)res + offset;
            chunk->unk05 = (I8 FAR *)audioresource_get_dword((U32 FAR *)p) + 4;
            chunk->data = (I8 FAR *)audioresource_get_dword((U32 FAR *)p) + 4;
            offset += 5;
            chunk->unk2E = (I8 FAR *)res + 7;
        } else {
            chunk->data = 0;
        }
    }
}

void FAR audio_map_song_instruments(void FAR *song, void FAR *voice)
{
    U8 FAR *dest;
    I8 name[5];
    void FAR *instres;
    I16 i;
    I16 j;
    U8 FAR *hdr;

    name[4] = 0;
    hdr = audioresource_find(song, "hdr1");
    if (hdr != 0) {
        for (i = 0; i < hdr[6]; i++) {
            for (j = 0; j < 4; j++)
                name[j] = hdr[i * 4 + j + 7];
            dest = hdr + i * 4 + 7;
            instres = audioresource_find(voice, name);
            audioresource_copy_4_bytes(dest, (U8 FAR *)&instres);
        }
        kick_res = audioresource_find(voice, "BASD");
        g_snaresnd = audioresource_find(voice, "SNAR");
        tommsampleresource = audioresource_find(voice, "TOMM");
        ride_audio_sound_res = audioresource_find(voice, "RIDE");
        audio_opp_res = audioresource_find(voice, "CRSH");
        chhtsample = audioresource_find(voice, "CHHT");
        resource_sound_hit = audioresource_find(voice, "OHHT");
    }
}

void FAR link_audio_shape_resources(U8 FAR *res, void FAR *shapes)
{
    I8 FAR *resptr;
    I8 name[5];
    I8 FAR *destptr;
    void FAR *shapeptr;
    I16 i;
    I16 count;
    I16 j;

    if (shapes == 0)
        return;
    if (res == 0)
        return;
    count = res[4];
    for (i = 0; i < res[4]; i++) {
        for (j = 0; j < 4; j++)
            name[j] = res[i * 4 + j + 6];
        resptr = audioresource_find(res, name);
        if ((resptr[5] == 3 || resptr[5] == 6) && resptr[10] == 0) {
            for (j = 0; j < 4; j++)
                name[j] = resptr[j + 6];
            destptr = resptr + 6;
    /* PLATFORM(file): resolve an audio resource entry in the loaded bundle. */
            shapeptr = locate_shape_nofatal(shapes, name);
            if (shapeptr != 0) {
                audioresource_copy_4_bytes((U8 FAR *)destptr, (U8 FAR *)&shapeptr);
                resptr[10] = 0xff;
            }
        }
    }
}

void FAR reset_audio_voice_length(I16 index)
{
    struct AUDIOVOICE *voice;
    voice = &snd_voices_tbl[index];
    voice->length = 1;
}

/* Reset the loaded audio driver voices and effect state.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): reset state in the loaded DOS sound-driver image. */
void FAR reset_audio_driver_state(void)
{
    I16 i;

    audio_update_lock = 1;
    audio_init_chunk(0, 0x17, 0, 0, 0x7f, 0);
    for (i = 0; i < g_audiodrvvoices_count; i++) {
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x1e))(i);
        snd_voices_tbl[i].state = 0;
        snd_voices_tbl[i].unk00 = 0xff;
        snd_voices_tbl[i].unk02 = 0;
        snd_voices_tbl[i].unk10 = 0;
        snd_voices_tbl[i].unk2C = 0xff;
    }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + 0x18))();
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + 6))();
    audio_update_lock = 0;
}

void FAR audio_map_song_tracks(U8 FAR *song)
{
    I16 ok_idx;
    I16 header_idx;
    I16 no;
    I16 numberOfChunks;
    U8 FAR *chunk_names;
    U8 header_total;
    U8 FAR *lo_chunk_data;
    U32 relative;
    U8 FAR *track_end;
    U8 FAR *cur;
    U8 FAR *p_copy;
    I8 name_buffer[5];
    U8 FAR *chunk_offsets;
    U8 param_num;

    name_buffer[4] = 0;
    numberOfChunks = audioresource_get_word((U16 FAR *)(song + 4));
    chunk_names = song + 6;
    chunk_offsets = chunk_names + numberOfChunks * 4;
    lo_chunk_data = song + numberOfChunks * 8 + 6;
    for (no = 0; no < numberOfChunks; no++) {
        cur = lo_chunk_data + (U16)audioresource_get_dword((U32 FAR *)(chunk_offsets + no * 4));
        track_end = cur + (U16)audioresource_get_dword((U32 FAR *)&cur);
        cur += 4;
        if (audioresource_compare_chunknames(0, (I8 FAR *)(chunk_names + no * 4), "hdr1", 4)) {
            cur += 2;
            header_total = *cur;
            cur += header_total * 4 + 1;
            header_total = *cur;
            cur++;
            for (header_idx = 0; header_idx < header_total; header_idx++) {
                audioresource_copy_n_bytes(cur, (U8 FAR *)name_buffer, 4);
                ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (I8 FAR *)chunk_names);
                if (ok_idx != -1) {
                    relative = audioresource_get_dword((U32 FAR *)(chunk_offsets + ok_idx * 4));
                    p_copy = lo_chunk_data + (U16)relative;
                    audioresource_copy_4_bytes(cur, (U8 FAR *)&p_copy);
                }
                cur += 5;
            }
        } else {
            while (cur < track_end) {
                while (*cur & 0x80)
                    cur++;
                cur++;
                switch (*cur - 0xd9) {
                case 13:
                    cur += 2;
                    audioresource_copy_n_bytes(cur, (U8 FAR *)name_buffer, 4);
                    ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (I8 FAR *)chunk_names);
                    if (ok_idx != -1) {
                        relative = audioresource_get_dword((U32 FAR *)(chunk_offsets + ok_idx * 4));
                        p_copy = lo_chunk_data + (U16)relative;
                        audioresource_copy_4_bytes(cur, (U8 FAR *)&p_copy);
                    }
                    cur += 4;
                    break;
                case 3: case 4: case 5: case 7: case 8: case 9: case 11: case 16: case 17:
                    cur += 2;
                    break;
                case 6: case 12:
                    cur += 3;
                    break;
                case 14: case 15:
                    cur++;
                    param_num = *cur++;
                    cur += param_num;
                    break;
                default:
                    if ((U16)*cur >= 0x80)
                        cur++;
                    do
                        cur++;
                    while (*cur & 0x80);
                case 0: case 1: case 2: case 10:
                    cur++;
                    break;
                }
            }
        }
    }
}

U32 FAR audioresource_get_dword(U32 FAR *p) { U32 value; value = *p; return value; }

U16 FAR audioresource_get_word(U16 FAR *p) { U16 value; value = *p; return value; }

void FAR audioresource_copy_4_bytes(U8 FAR *dst, U8 FAR *src)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst = *src;
}

void FAR nopsub_38570(void)
{
    I16 i;

    debug_printf_text("swPause = %d, swSong = %d, bSong = %d,swSFX = %d\n",
                 audio_pause_in_progress, audioflag2, audio_song_ready, audioflag6);
    debug_printf_text("ubMusicVolume = %d, ubSfxVolume = %d\n", g_musicvolumesetting, sfx_audio_vol);
    for (i = 0; i < 24; i++)
        debug_printf_text("T%02x-ND=%lx,DL=%ld\n", i, audiochunktable[i].data, audiochunktable[i].unk18);
    debug_printf_text("Press a Key\n");
    flush_stdin();
    for (i = 0; i < 16; i++)
        debug_printf_text("H%02x - ST=%d,TP=%lx,TL=%lx\n", i, snd_voices_tbl[i].state,
                     snd_voices_tbl[i].position, snd_voices_tbl[i].length);
}
