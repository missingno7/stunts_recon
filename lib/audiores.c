#include "audio.h"
#include "types.h"
static U8 saved_music_chunk_volumes[AUDIO_CHUNK_CAPACITY];
static U8 saved_effect_chunk_volumes[AUDIO_CHUNK_CAPACITY];

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

#pragma pack()
/* READABILITY: Load and drive the DOS audio module, schedule its timer callback, and resolve audio resources. */

/* Local view: audiochunktable; array is 24; another unit uses None. */
struct AUDIOCHUNK audiochunktable[AUDIO_CHUNK_CAPACITY];
/* Local view: snd_voices_tbl; array is 16; another unit uses None. */
struct AUDIOVOICE snd_voices_tbl[AUDIO_VOICE_CAPACITY];

unsigned char block_audio_num;
unsigned char audioblock[AUDIO_CHUNK_CAPACITY];
unsigned char g_audchnkvalue[AUDIO_CHUNK_CAPACITY];
unsigned char sfx_audio_vol;
unsigned char g_musicvolumesetting;

unsigned char audiochnk_actflags[AUDIO_CHUNK_CAPACITY];

char g_drvaudiocode[14];
/* Local view: kick_res; pointer is void; another unit uses signed byte. */
void far *kick_res;
/* Local view: g_snaresnd; pointer is void; another unit uses signed byte. */
void far *g_snaresnd;
/* Local view: tommsampleresource; pointer is void; another unit uses signed byte. */
void far *tommsampleresource;
/* Local view: ride_audio_sound_res; pointer is void; another unit uses signed byte. */
void far *ride_audio_sound_res;
/* Local view: audio_opp_res; pointer is void; another unit uses signed byte. */
void far *audio_opp_res;
/* Local view: chhtsample; pointer is void; another unit uses signed byte. */
void far *chhtsample;
/* Local view: resource_sound_hit; pointer is void; another unit uses signed byte. */
void far *resource_sound_hit;

/* Local view: audio_set_chunk_volume; parameters[1] is unsigned byte; another unit uses signed word. */
void FAR audio_set_chunk_volume(I16 index, U8 value);

    /* PLATFORM(file): load or decompress the named audio resource. */

    /* PLATFORM(file): load or decompress the named audio resource. */

    /* PLATFORM(memory): release storage through the game memory manager. */
#include "ui.h"

    /* PLATFORM(timer): use or register the game timer service. */

    /* PLATFORM(timer): use or register the game timer service. */

    /* PLATFORM(timer): use or register the game timer service. */
/* Local view: setticks; parameters.count is 1; another unit uses 2. */
void FAR setticks(I32 ticks);
    /* PLATFORM(timer): use or register the game timer service. */

    /* PLATFORM(file): resolve an audio resource entry in the loaded bundle. */
#include "files.h"
#include "memory.h"

/* Local view: audio_init_chunk; parameters.count is 6; another unit uses 7. */
void FAR audio_init_chunk(I16 first, I16 last, void FAR *res, I16 offset, U8 volume, U8 priority);
#include "auddrv.h"

/* Local view: timer registration uses an ordinary far callback; the entry definition uses _loadds. */
extern void far audiodriver_timer(void);
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "timer.h"

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
    if (((I8 FAR *)hdrptr)[AUDIO_HDR1_READY_OFFSET] != AUDIO_HDR1_READY) {
        audio_map_song_instruments(titleres, voice);
        audio_map_song_tracks(titleres);
        ((I8 FAR *)hdrptr)[AUDIO_HDR1_READY_OFFSET] = AUDIO_HDR1_READY;
        trackptr = (I8 FAR *)titleres + (((U8 FAR *)titleres)[4] << 3) + 1;
        audioresource_copy_4_bytes((U8 FAR *)hdrptr, (U8 FAR *)&trackptr);
    }
    return hdrptr;
}
I8 FAR *audiodriverbinary = 0;
U16 audio_bit_masks[AUDIO_MASK_TABLE_WORDS] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};
U8 audio_pause_in_progress = 0;
U8 audioflag2 = 1;
U8 audio_song_ready = 0;
U8 audioflag6 = 1;
U8 audio_driver_mode = 0;
U8 audio_driver_extension_mode = 0;
U8 audio_driver_volume_command[AUDIO_DRIVER_COMMAND_BYTES] = { 0x10, 0x00, 0x16, 0x00 };
U16S audio_update_lock = 1;
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
    if (((I8 FAR *)song)[AUDIO_HDR1_TYPE_OFFSET] != 0) return;
    if (((I8 FAR *)song)[AUDIO_HDR1_READY_OFFSET] != AUDIO_HDR1_READY) return;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    /* PORT: This offset selects a driver entry inside a loaded segment; preserve 16:16 pointer semantics. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_INITIALIZE))();
    snd_sample_rate_phase = 0;
    mus_samplelimit = AUDIO_MUSIC_PHASE_DEFAULT_LIMIT;
    offset = (((U8 FAR *)song)[AUDIO_HDR1_INSTRUMENT_COUNT_OFFSET] << AUDIO_HDR1_POINTER_SCALE_SHIFT) + AUDIO_HDR1_INSTRUMENTS_OFFSET;
    block_audio_num = ((I8 FAR *)song)[offset++];
    audio_init_chunk(0, block_audio_num - 1, song, offset, g_musicvolumesetting, AUDIO_PRIORITY_MUSIC);
    audio_song_ready = 1;
    audio_update_lock = 0;
}

/* Send the currently prepared volume command to the loaded audio driver.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(audio): dispatch the prepared volume command into the DOS sound driver. */
void FAR audio_pause_playback(void)
{
    struct AUDIOVOICE *channel;
    I16 i;

    audio_pause_in_progress = 1;
    audio_update_lock = 1;
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        for (i = 0; i < AUDIO_CHUNK_CAPACITY; i++) {
            if (audioflag6 == 1 || i < AUDIO_MUSIC_CHUNK_LIMIT) {
                saved_music_chunk_volumes[i] = audiochunktable[i].volume;
                audio_set_chunk_volume(i, 0);
            }
        }
    } else {
        audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = 0;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
    }
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        for (i = 0; i < AUDIO_VOICE_CAPACITY; i++) {
            channel = &snd_voices_tbl[i];
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + AUD_DRV_ENTRY_UPDATE_VOICE))(channel->channel_number, channel, channel->owner_chunk, channel->sample_data);
        }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUD_DRV_ENTRY_UPDATE_VOICE_ARRAY))(snd_voices_tbl);
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
    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        for (i = 0; i < AUDIO_CHUNK_CAPACITY; i++) {
            if (audioflag6 == 1 || i < AUDIO_MUSIC_CHUNK_LIMIT)
                audio_set_chunk_volume(i, saved_music_chunk_volumes[i]);
        }
    } else {
        audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = AUDIO_DRIVER_VOLUME_FULL;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
    }
    audio_update_lock = 0;
    audio_pause_in_progress = 0;
}

void FAR reset_audio_chunks(void)
{
    audio_update_lock = 1;
    audio_song_ready = 0;
    audio_driver_func1E(0, AUDIO_MUSIC_CHUNK_LAST);
    audio_init_chunk(0, AUDIO_MUSIC_CHUNK_LAST, 0, 0, g_musicvolumesetting, 0);
    block_audio_num = 0;
    reset_audio_event_state();
    audio_update_lock = 0;
}

void FAR audio_enable_flag2(void) { audioflag2 = 1; }
void FAR audio_disable_flag2(void) { audioflag2 = 0; audio_update_lock = 1; if (block_audio_num) { audio_driver_func1E(0, block_audio_num - 1); } reset_audio_event_state(); audio_update_lock = 0; }
I16 FAR audio_toggle_flag2(void) { if (audioflag2 == 1) { audio_disable_flag2(); return 0; } audio_enable_flag2(); return 1; }

I16 FAR audio_music_is_idle(void)
{
    I16 i;
    if (audio_pause_in_progress == 1 || audioflag2 == 0)
        return 1;
    for (i = 0; i < block_audio_num; i++)
        if (audiochunktable[i].event_pos != 0)
            return 0;
    return 1;
}

I16 FAR audio_play_resource_default(void FAR *res)
{
    return audio_check_flag2(res, -1, 0x40);
}
I16 FAR reserve_audio_chunk(I16 chunk, U8 priority)
{
    I16 i;
    if (chunk == -1) {
        for (i = AUDIO_MUSIC_CHUNK_LIMIT; i <= AUDIO_EFFECT_CHUNK_LAST; i++) {
            if (chunk == -1) {
                if (audiochunktable[i].event_pos == 0 && audiochnk_actflags[i] == 0)
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
    if (((I8 FAR *)res)[AUDIO_HDR1_READY_OFFSET] != AUDIO_HDR1_READY)
        return -1;
    if (sfx_audio_vol != 0)
        volume = (volume << AUDIO_EFFECT_VOLUME_SCALE_SHIFT) / sfx_audio_vol - 1;
    else
        volume = 0;
    if (chunk == -1) {
        for (i = AUDIO_MUSIC_CHUNK_LIMIT; i <= AUDIO_EFFECT_CHUNK_LAST; i++) {
            if (chunk == -1) {
                if (audiochunktable[i].event_pos == 0 && audiochnk_actflags[i] == 0)
                    chunk = i;
            } else
                break;
        }
        if (chunk == -1) {
            best = AUDIO_PRIORITY_SEARCH_INITIAL;
            for (i = AUDIO_MUSIC_CHUNK_LIMIT; i < AUDIO_PRIORITY_STEAL_SCAN_END; i++) {
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
    offset = (((U8 FAR *)res)[AUDIO_HDR1_INSTRUMENT_COUNT_OFFSET] << AUDIO_HDR1_POINTER_SCALE_SHIFT) + AUDIO_HDR1_FIRST_TRACK_OFFSET_BASE;
    audio_init_chunk(chunk, chunk, res, offset, volume, priority);
    return chunk;
}

void FAR audio_init_chunk2(I16 chunk)
{
    if (chunk < AUDIO_MUSIC_CHUNK_LIMIT || chunk > AUDIO_EFFECT_CHUNK_LAST) return;
    audiochunktable[chunk].event_pos = 0;
    audio_driver_func1E(chunk, chunk);
    audio_init_chunk(chunk, chunk, 0, 0, sfx_audio_vol, 0);
}

void FAR audio_enable_flag6(void)
{
    I16 i;
    if (audioflag6 != 1) {
        for (i = AUDIO_MUSIC_CHUNK_LIMIT; i < AUDIO_CHUNK_CAPACITY; i++)
            audio_set_chunk_volume(i, saved_effect_chunk_volumes[i]);
        audioflag6 = 1;
    }
}

void FAR audio_disable_flag6(void)
{
    I16 i;
    if (audioflag6 != 0) {
        for (i = AUDIO_MUSIC_CHUNK_LIMIT; i < AUDIO_CHUNK_CAPACITY; i++) {
            saved_effect_chunk_volumes[i] = audiochunktable[i].volume;
            audio_set_chunk_volume(i, 0);
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
    if (chunk < AUDIO_MUSIC_CHUNK_LIMIT || chunk > AUDIO_EFFECT_CHUNK_LAST) return 1;
    if (audiochunktable[chunk].event_pos == 0) return 1;
    return 0;
}

void FAR audio_set_chunk_callback(U16 chunk, I32 value)
{
    audiochunktable[chunk].end_callback = value;
}

/* Change the driver volume while waiting against the game timer.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(timer): pace the audio volume transition using the DOS timer. */
/* PLATFORM(audio): send volume commands through the loaded sound driver. */
void FAR audio_driver_func3F(I16 ticks)
{
    I16 counter;

    if (audio_driver_mode == AUDIO_DRIVER_MODE_VOICE) {
        for (counter = g_musicvolumesetting; counter > 0; counter -= AUDIO_VOLUME_FADE_STEP) {
            audio_update_lock = 1;
            set_all_audio_chunk_volume(counter);
            audio_update_lock = 0;
    /* PLATFORM(timer): use or register the game timer service. */
            setticks(ticks);
    /* PLATFORM(timer): use or register the game timer service. */
            waitticks();
        }
    } else {
        for (counter = AUDIO_DRIVER_VOLUME_FULL; counter > 0; counter -= AUDIO_VOLUME_FADE_STEP) {
            audio_update_lock = 1;
            audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = counter;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
            audio_update_lock = 0;
    /* PLATFORM(timer): use or register the game timer service. */
            setticks(ticks);
    /* PLATFORM(timer): use or register the game timer service. */
            waitticks();
        }
    }
    reset_audio_chunks();
    if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE) {
    /* PLATFORM(timer): use or register the game timer service. */
        setticks(AUDIO_VOLUME_FADE_SETTLE_TICKS);
    /* PLATFORM(timer): use or register the game timer service. */
        waitticks();
        audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = AUDIO_DRIVER_VOLUME_FULL;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
    }
}

void FAR set_all_audio_chunk_volume(I16 value)
{
    I16 index;
    index = 0;
    while (index < block_audio_num) {
        audio_set_chunk_volume(index, value);
        ++index;
    }
}

void FAR audio_set_music_volume(I16 value)
{
    g_musicvolumesetting = value;
    set_all_audio_chunk_volume(value);
}

U16 FAR audio_get_chunk_marker(I16 index) { return g_audchnkvalue[index]; }

U16 FAR audio_get_chunk_note(I16 index) { return audioblock[index]; }

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

    if (signature == AUDIO_EXTENSION_SIGNATURE)
        audio_driver_extension_mode = 1;
    if (audiodriverbinary != 0)
        audiodrv_atexit();
    else
        addexit(audiodrv_atexit);
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
    audiodriverbinary = loadfilez(path);
    g_musicvolumesetting = AUDIO_VOLUME_MAX;
    sfx_audio_vol = AUDIO_VOLUME_MAX;
    if (audiodriverbinary != 0) {
        g_audiodrvvoices_count = ((U8 (FAR *)(void))audiodriverbinary)();
        if (g_audiodrvvoices_count == 0 || g_audiodrvvoices_count == AUDIO_DRIVER_INIT_FAILURE_FF)
            return AUDIO_LOAD_ERROR_DEVICE_INIT;
        if (g_audiodrvvoices_count > AUDIO_DRIVER_CHANNEL_MODE_THRESHOLD) {
            g_audiodrvvoices_count = AUDIO_VOICE_CAPACITY;
            audio_driver_mode = AUDIO_DRIVER_MODE_CHANNEL;
            audio_driver_extension_mode = 0;
        }
        reset_audio_driver_state();
        /* PLATFORM(timer): use or register the game timer service. */
        addtimer(audiodriver_timer);
        if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE) {
        /* PLATFORM(file): load or decompress the named audio resource. */
            patches = loadfilez("mt32.plb");
            if (patches != 0) {
        /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
                ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_LOAD_PATCHES))(patches);
        /* PLATFORM(memory): release storage through the game memory manager. */
                purgememory(patches);
                audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = AUDIO_DRIVER_VOLUME_FULL;
        /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
                ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
            }
        }
        audio_pause_in_progress = 0;
        audioflag2 = 1;
        audio_song_ready = 0;
        audioflag6 = 1;
        return 0;
    } else {
        abortmessage("Can't find driver!\n");
    }
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
        deltimer(audiodriver_timer);
        audioflag2 = 0;
        audioflag6 = 0;
        if (audio_driver_mode != AUDIO_DRIVER_MODE_VOICE) {
            audio_driver_volume_command[AUDIO_DRIVER_COMMAND_VOLUME_INDEX] = AUDIO_DRIVER_VOLUME_FULL;
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
            ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SEND_COMMAND))(AUDIO_DRIVER_COMMAND_BYTES, (void FAR *)audio_driver_volume_command);
        }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_RESET))();
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_SHUTDOWN))();
    /* PLATFORM(memory): release storage through the game memory manager. */
        purgememory(audiodriverbinary);
        audiodriverbinary = 0;
        audio_driver_mode = AUDIO_DRIVER_MODE_VOICE;
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
    resource = loadfilez(audio_make_filename(filename, extension, kind));
    if (resource != 0)
        return resource;
    buffer[0] = 'P';
    buffer[1] = extension[0];
    buffer[2] = extension[1];
    buffer[3] = 0;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadpackz(audio_make_filename(filename, buffer, kind));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadfilez(audio_make_filename(filename, extension, "ge"));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadpackz(audio_make_filename(filename, buffer, "ge"));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadfilez(audio_make_filename(filename, extension, ""));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadpackz(audio_make_filename(filename, buffer, ""));
    if (resource != 0)
        return resource;
    /* PLATFORM(file): load or decompress the named audio resource. */
    resource = loadfilez(filename);
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
        abortmessage("cannot load sfx file %s", filename);
    return result;
}

void FAR * FAR load_song_file(I8 *filename)
{
    void FAR *result;

    result = 0;
    result = load_sfx_ge(filename, "kms", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        abortmessage("cannot load song file %s", filename);
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
        abortmessage("cannot load voice file %s", filename);
    return result;
}

void FAR * FAR audio_load_sample_bank(I8 *filename)
{
    void FAR *result;

    result = load_sfx_ge(filename, "slb", g_drvaudiocode);
    if (result == 0 && audio_load_error_policy != 0)
        abortmessage("cannot load sample file %s", filename);
    return result;
}

/* Local view: audio_init_chunk; parameters.count is 6; another unit uses 7. */
void FAR audio_init_chunk(I16 first, I16 last, void FAR *res, I16 offset, U8 volume, U8 priority)
{
    struct AUDIOCHUNK *chunk;
    I16 i;
    I8 FAR *p;

    for (i = first; i <= last; i++) {
        chunk = &audiochunktable[i];
        chunk->end_callback = 0;
        chunk->velocity = AUDIO_VELOCITY_DEFAULT;
        chunk->index = i;
        chunk->max_voices = AUDIO_MAX_VOICES_DEFAULT;
        g_audchnkvalue[i] = 0;
        audioblock[i] = 0;
        chunk->loop_depth = 0;
        chunk->call_depth = 0;
        chunk->priority = priority;
        chunk->active_voices = 0;
        chunk->event_delay = 0;
        chunk->unk1C = 0;
        chunk->sample_data = 0;
        chunk->volume = volume;
        chunk->sustain_mode = 0;
        chunk->pitch_bend = 0;
        chunk->driver_parameter_29 = 0;
        chunk->unk2A = 0;
        chunk->driver_parameter_2b = 0;
        chunk->driver_parameter_2c = 0;
        chunk->channel_number = AUDIO_UNASSIGNED_CHUNK_CHANNEL;
        if (res != 0) {
            p = (I8 FAR *)res + offset;
            chunk->restart_pos = (I8 FAR *)audioresource_get_dword((U32 FAR *)p) + AUDIO_TRACK_SPAN_BYTES;
            chunk->event_pos = (I8 FAR *)audioresource_get_dword((U32 FAR *)p) + AUDIO_TRACK_SPAN_BYTES;
            offset += AUDIO_HDR1_TRACK_ENTRY_BYTES;
            chunk->sample_table = (I8 FAR *)res + AUDIO_HDR1_INSTRUMENTS_OFFSET;
        } else {
            chunk->event_pos = 0;
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
        for (i = 0; i < hdr[AUDIO_HDR1_INSTRUMENT_COUNT_OFFSET]; i++) {
            for (j = 0; j < 4; j++)
                name[j] = hdr[i * 4 + j + AUDIO_HDR1_INSTRUMENTS_OFFSET];
            dest = hdr + i * 4 + AUDIO_HDR1_INSTRUMENTS_OFFSET;
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
    count = res[RESOURCE_COUNT_OFFSET];
    for (i = 0; i < res[RESOURCE_COUNT_OFFSET]; i++) {
        for (j = 0; j < 4; j++)
            name[j] = res[i * 4 + j + RESOURCE_NAMES_OFFSET];
        resptr = audioresource_find(res, name);
        if ((resptr[AUDIO_SHAPE_LINK_KIND_OFFSET] == AUDIO_SHAPE_LINK_KIND_3 || resptr[AUDIO_SHAPE_LINK_KIND_OFFSET] == AUDIO_SHAPE_LINK_KIND_6) && resptr[AUDIO_SHAPE_LINK_READY_OFFSET] == 0) {
            for (j = 0; j < 4; j++)
                name[j] = resptr[j + AUDIO_SHAPE_LINK_TARGET_OFFSET];
            destptr = resptr + AUDIO_SHAPE_LINK_TARGET_OFFSET;
    /* PLATFORM(file): resolve an audio resource entry in the loaded bundle. */
            shapeptr = locateshapez(shapes, name);
            if (shapeptr != 0) {
                audioresource_copy_4_bytes((U8 FAR *)destptr, (U8 FAR *)&shapeptr);
                resptr[AUDIO_SHAPE_LINK_READY_OFFSET] = AUDIO_SHAPE_LINK_READY;
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
    audio_init_chunk(0, AUDIO_ALL_CHUNK_LAST, 0, 0, AUDIO_VOLUME_MAX, 0);
    for (i = 0; i < g_audiodrvvoices_count; i++) {
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
        ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_RESET_CHANNEL))(i);
        snd_voices_tbl[i].state = 0;
        snd_voices_tbl[i].resource_index = AUDIO_RESET_VOICE_OWNER;
        snd_voices_tbl[i].priority = 0;
        snd_voices_tbl[i].sample_data = 0;
        snd_voices_tbl[i].channel_number = AUDIO_UNASSIGNED_HARDWARE_CHANNEL;
    }
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_INITIALIZE))();
    /* PLATFORM(audio): dispatch an operation to the loaded DOS audio driver. */
    ((DRVPROC)((I8 FAR *)audiodriverbinary + AUDIO_DRIVER_ENTRY_RESET))();
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
    numberOfChunks = audioresource_get_word((U16 FAR *)(song + RESOURCE_COUNT_OFFSET));
    chunk_names = song + RESOURCE_DIRECTORY_HEADER_BYTES;
    chunk_offsets = chunk_names + numberOfChunks * RESOURCE_OFFSET_BYTES;
    lo_chunk_data = song + numberOfChunks * 8 + RESOURCE_DIRECTORY_HEADER_BYTES;
    for (no = 0; no < numberOfChunks; no++) {
        cur = lo_chunk_data + audioresource_get_dword((U32 FAR *)(chunk_offsets + no * RESOURCE_OFFSET_BYTES));
        track_end = cur + audioresource_get_dword((U32 FAR *)&cur);
        cur += AUDIO_TRACK_SPAN_BYTES;
        if (audioresource_compare_chunknames(0, (I8 FAR *)(chunk_names + no * 4), "hdr1", 4)) {
            cur += AUDIO_HDR1_POST_SPAN_PREFIX_BYTES;
            header_total = *cur;
            cur += header_total * AUDIO_HDR1_INSTRUMENT_POINTER_BYTES + 1;
            header_total = *cur;
            cur++;
            for (header_idx = 0; header_idx < header_total; header_idx++) {
                audioresource_copy_n_bytes(cur, (U8 FAR *)name_buffer, 4);
                ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (I8 FAR *)chunk_names);
                if (ok_idx != -1) {
                    relative = audioresource_get_dword((U32 FAR *)(chunk_offsets + ok_idx * RESOURCE_OFFSET_BYTES));
                    p_copy = lo_chunk_data + relative;
                    audioresource_copy_4_bytes(cur, (U8 FAR *)&p_copy);
                }
                cur += AUDIO_HDR1_TRACK_ENTRY_BYTES;
            }
        } else {
            while (cur < track_end) {
                while (*cur & AUDIO_EVENT_BASE128_CONTINUATION)
                    cur++;
                cur++;
                switch (*cur - AUDIO_EVENT_CONTROL_FIRST) {
                case AUDIO_EVENT_CASE_CALL:
                    cur += AUDIO_CALL_EVENT_PREFIX_BYTES;
                    audioresource_copy_n_bytes(cur, (U8 FAR *)name_buffer, 4);
                    ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (I8 FAR *)chunk_names);
                    if (ok_idx != -1) {
                        relative = audioresource_get_dword((U32 FAR *)(chunk_offsets + ok_idx * RESOURCE_OFFSET_BYTES));
                        p_copy = lo_chunk_data + relative;
                        audioresource_copy_4_bytes(cur, (U8 FAR *)&p_copy);
                    }
                    cur += AUDIO_SUBTRACK_NAME_BYTES;
                    break;
                case AUDIO_EVENT_CASE_SELECT_RESOURCE: case AUDIO_EVENT_CASE_TEMPO: case AUDIO_EVENT_CASE_PROGRAM_VALUE: case AUDIO_EVENT_CASE_MAX_VOICES: case AUDIO_EVENT_CASE_NOTE_PRIORITY: case AUDIO_EVENT_CASE_LOOP_BEGIN: case AUDIO_EVENT_CASE_VELOCITY: case AUDIO_EVENT_CASE_CHANNEL: case AUDIO_EVENT_CASE_CHUNK_VALUE:
                    cur += AUDIO_ONE_PARAM_EVENT_BYTES;
                    break;
                case AUDIO_EVENT_CASE_CONTROLLER: case AUDIO_EVENT_CASE_WORD_PARAMETER:
                    cur += AUDIO_TWO_PARAM_EVENT_BYTES;
                    break;
                case AUDIO_EVENT_CASE_META: case AUDIO_EVENT_CASE_RAW_BYTES:
                    cur++;
                    param_num = *cur++;
                    cur += param_num;
                    break;
                default:
                    if ((U16)*cur >= AUDIO_RELOC_NOTE_PARAM_BOUNDARY)
                        cur++;
                    do
                        cur++;
                    while (*cur & AUDIO_EVENT_BASE128_CONTINUATION);
                case AUDIO_EVENT_CASE_RETURN: case AUDIO_EVENT_CASE_STOP: case AUDIO_EVENT_CASE_RESTART: case AUDIO_EVENT_CASE_LOOP_END:
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

void FAR audio_debug_dump_state(void)
{
    I16 i;

    debug_printf_text("swPause = %d, swSong = %d, bSong = %d,swSFX = %d\n",
                 audio_pause_in_progress, audioflag2, audio_song_ready, audioflag6);
    debug_printf_text("ubMusicVolume = %d, ubSfxVolume = %d\n", g_musicvolumesetting, sfx_audio_vol);
    for (i = 0; i < AUDIO_CHUNK_CAPACITY; i++)
        debug_printf_text("T%02x-ND=%lx,DL=%ld\n", i, audiochunktable[i].event_pos, audiochunktable[i].event_delay);
    debug_printf_text("Press a Key\n");
    flush_stdin();
    for (i = 0; i < AUDIO_VOICE_CAPACITY; i++)
        debug_printf_text("H%02x - ST=%d,TP=%lx,TL=%lx\n", i, snd_voices_tbl[i].state,
                     snd_voices_tbl[i].position, snd_voices_tbl[i].length);
}
