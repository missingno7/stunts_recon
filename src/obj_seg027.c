/* obj_seg027 (audio): complete object [159954,165436), MSC 5.10 /AM /Ox /Gs (TUFLAG-Ox-seg027).
 * Externs use Restunts dseg/seg027 spellings.  Source/target notes:
 * - AUDIOCHUNK is one 24-entry table (audio_init_chunk indexes 0..23 from one base);
 *   its dword at +5 is unaligned, hence #pragma pack(1) (register choice in
 *   audio_init_chunk depends on a real member, not a cast of an array member).
 * - The initialized globals below init_audio_resources are TU-owned _DATA
 *   (literal "hdr1" precedes them in the image); audio_bit_masks is not referenced
 *   by this object but occupies DGROUP 0x4E9E..0x4EBF between them.
 * - sub_3803C: count is assigned and never read (target stores it to its home),
 *   name[5] matches the target's 6-byte buffer home. */

#pragma pack(1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    char far *data;                 /* 00 */
    unsigned char unk04;            /* 04 */
    char far *unk05;                /* 05 */
    unsigned char unk09[12];        /* 09 */
    unsigned char unk15;            /* 15 */
    unsigned char unk16;            /* 16 */
    unsigned char unk17;            /* 17 */
    long unk18;                     /* 18 */
    unsigned char unk1C;            /* 1C */
    unsigned char unk1D;            /* 1D */
    long unk1E;                     /* 1E */
    unsigned char unk22;            /* 22 */
    unsigned char index;            /* 23 */
    unsigned char priority;         /* 24 */
    unsigned char unk25;            /* 25 */
    int unk26;                      /* 26 */
    unsigned char volume;           /* 28 */
    unsigned char unk29;            /* 29 */
    unsigned char unk2A;            /* 2A */
    unsigned char unk2B;            /* 2B */
    unsigned char unk2C;            /* 2C */
    unsigned char unk2D;            /* 2D */
    char far *unk2E;                /* 2E */
    unsigned char unk32;            /* 32 */
    unsigned char unk33[20];        /* 33 */
    unsigned char unk47;            /* 47 */
    long unk48;                     /* 48 */
};
#pragma pack()

struct AUDIOVOICE {                 /* 0x2E bytes */
    unsigned char unk00;            /* 00 */
    unsigned char state;            /* 01 */
    unsigned char unk02;            /* 02 */
    unsigned char unk03[5];         /* 03 */
    long position;                  /* 08 */
    long length;                    /* 0C */
    long unk10;                     /* 10 */
    unsigned char unk14[22];        /* 14 */
    int unk2A;                      /* 2A */
    unsigned char unk2C;            /* 2C */
    unsigned char unk2D;            /* 2D */
};

extern struct AUDIOCHUNK audiochunks_unk[24];
extern struct AUDIOVOICE unk_45A26[16];
extern unsigned char byte_428BE[], byte_428D6[], byte_44290, byte_44ACA[], byte_44D06[];
extern unsigned char byte_45948, byte_45950, byte_459D2, byte_45D9A[];
extern int word_44D48, word_454BA;
extern char audiodriverstring2[];
extern void far *basdres, far *snarres, far *tommres, far *rideres;
extern void far *crshres, far *chhtres, far *ohhtres;


void far * far audioresource_find(void far *resource, char *name);
void far audio_unk2(int index, unsigned char value);
void far audio_driver_func1E(int first, int last);
void far sub_39700(void);
char * far audio_make_filename(char *name, char *ext, char *kind);
void far * far file_load_binary_nofatal(char *name);
void far * far file_decomp_nofatal(char *name);
void far mmgr_release(void far *ptr);
void far fatal_error(char *format, ...);
void far add_exit_handler(void (far *handler)(void));
void far timer_reg_callback(void (far *callback)(void));
void far timer_remove_callback(void (far *callback)(void));
void far audiodriver_timer(void);
void far timer_copy_counter(long ticks);
void far timer_wait_for_dx(void);
void far * far locate_shape_nofatal(void far *shapes, char *name);
int far audioresource_get_chunk_index(int start, int count, char *name, char far *names);
int far audioresource_compare_chunknames(int flag, char far *name1, char far *name2, int length);
void far audioresource_copy_n_bytes(unsigned char far *src, unsigned char far *dst, int count);
void far nopsub_3219D(char *format, ...);
void far flush_stdin(void);
unsigned int strlen(const char *s);

void far audio_map_song_instruments(void far *song, void far *voice);
void far audio_map_song_tracks(void far *song);
void far audioresource_copy_4_bytes(unsigned char far *dst, unsigned char far *src);
unsigned long far audioresource_get_dword(unsigned long far *p);
unsigned int far audioresource_get_word(unsigned int far *p);
void far audio_init_chunk(int first, int last, void far *res, int offset, unsigned char volume, unsigned char priority);
int far audio_check_flag(void far *res, int chunk, unsigned char priority, unsigned int volume);
void far audio_init_chunk2(int chunk);
void far sub_3736A(void);
void far sub_37868(int value);
void far sub_38178(void);
void far audiodrv_atexit(void);

typedef void (far *DRVPROC)();

void far * far init_audio_resources(void far *song, void far *voice, char *name)
{
    void far *titleres;
    void far *hdrptr;
    char far *trackptr;

    if ((titleres = audioresource_find(song, name)) == 0)
        return 0;
    hdrptr = audioresource_find(titleres, "hdr1");
    if (hdrptr == 0)
        return 0;
    if (((char far *)hdrptr)[5] != 1) {
        audio_map_song_instruments(titleres, voice);
        audio_map_song_tracks(titleres);
        ((char far *)hdrptr)[5] = 1;
        trackptr = (char far *)titleres + ((unsigned int)((unsigned char far *)titleres)[4] << 3) + 1;
        audioresource_copy_4_bytes((unsigned char far *)hdrptr, (unsigned char far *)&trackptr);
    }
    return hdrptr;
}

void far *audiodriverbinary = 0;
unsigned int audio_bit_masks[17] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};
unsigned char byte_40630 = 0;
unsigned char audioflag2 = 1;
unsigned char byte_40632 = 0;
unsigned char audioflag6 = 1;
unsigned char byte_40634 = 0;
unsigned char byte_40635 = 0;
unsigned char unk_40636[4] = { 0x10, 0x00, 0x16, 0x00 };
int word_4063A = 1;
int word_4063C = 0;

void far load_audio_finalize(void far *song)
{
    int offset;

    word_4063A = 1;
    sub_3736A();
    if (song == 0) return;
    if (((char far *)song)[4] != 0) return;
    if (((char far *)song)[5] != 1) return;
    ((DRVPROC)((char far *)audiodriverbinary + 0x18))();
    word_44D48 = 0;
    word_454BA = 0x80;
    offset = (((unsigned char far *)song)[6] << 2) + 7;
    byte_44290 = ((char far *)song)[offset++];
    audio_init_chunk(0, byte_44290 - 1, song, offset, byte_45950, 0x20);
    byte_40632 = 1;
    word_4063A = 0;
}

void far audio_unk(void)
{
    struct AUDIOVOICE *channel;
    int i;

    byte_40630 = 1;
    word_4063A = 1;
    if (byte_40634 == 0) {
        for (i = 0; i < 24; i++) {
            if (audioflag6 == 1 || i < 16) {
                byte_428BE[i] = audiochunks_unk[i].volume;
                audio_unk2(i, 0);
            }
        }
    } else {
        unk_40636[3] = 0;
        ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
    }
    if (byte_40634 == 0) {
        for (i = 0; i < 16; i++) {
            channel = &unk_45A26[i];
            ((DRVPROC)((char far *)audiodriverbinary + 0x27))(channel->unk2C, channel, channel->unk2A, channel->unk10);
        }
        ((DRVPROC)((char far *)audiodriverbinary + 0x30))(unk_45A26);
    }
    word_4063A = 0;
}

void far sub_372F4(void)
{
    int i;

    byte_40630 = 1;
    word_4063A = 1;
    if (byte_40634 == 0) {
        for (i = 0; i < 24; i++) {
            if (audioflag6 == 1 || i < 16)
                audio_unk2(i, byte_428BE[i]);
        }
    } else {
        unk_40636[3] = 100;
        ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
    }
    word_4063A = 0;
    byte_40630 = 0;
}

void far sub_3736A(void)
{
    word_4063A = 1;
    byte_40632 = 0;
    audio_driver_func1E(0, 0x0f);
    audio_init_chunk(0, 0x0f, 0, 0, byte_45950, 0);
    byte_44290 = 0;
    sub_39700();
    word_4063A = 0;
}

void far audio_enable_flag2(void) { audioflag2 = 1; }
void far audio_disable_flag2(void) { audioflag2 = 0; word_4063A = 1; if (byte_44290) { audio_driver_func1E(0, (unsigned int)byte_44290 - 1); } sub_39700(); word_4063A = 0; }
int far audio_toggle_flag2(void) { if (audioflag2 == 1) { audio_disable_flag2(); return 0; } audio_enable_flag2(); return 1; }

int far nopsub_373FE(void)
{
    int i;
    if (byte_40630 == 1 || audioflag2 == 0)
        return 1;
    for (i = 0; i < byte_44290; i++)
        if (audiochunks_unk[i].data != 0)
            return 0;
    return 1;
}

int far audio_check_flag2(void far *res, int chunk, unsigned char priority);
int far nopsub_37456(void far *res)
{
    return audio_check_flag2(res, -1, 0x40);
}

int far sub_37470(int chunk, unsigned char priority)
{
    int i;
    if (chunk == -1) {
        for (i = 16; i <= 23; i++) {
            if (chunk == -1) {
                if (audiochunks_unk[i].data == 0 && byte_45D9A[i] == 0)
                    chunk = i;
            } else
                break;
        }
        if (chunk != -1) {
            byte_45D9A[chunk] = 1;
            audiochunks_unk[chunk].priority = priority;
        }
        return chunk;
    }
    byte_45D9A[chunk] = 1;
    audiochunks_unk[chunk].priority = priority;
    return chunk;
}

void far sub_374DE(int chunk)
{
    if (chunk > -1) {
        byte_45D9A[chunk] = 0;
        audio_init_chunk2(chunk);
    }
}

int far audio_check_flag2(void far *res, int chunk, unsigned char priority)
{
    return audio_check_flag(res, chunk, priority, byte_45948);
}

int far audio_check_flag(void far *res, int chunk, unsigned char priority, unsigned int volume)
{
    int i;
    unsigned int best;
    int offset;

    if (audioflag6 == 0)
        return -1;
    if (res == 0)
        return -1;
    if (((char far *)res)[5] != 1)
        return -1;
    if (byte_45948 != 0)
        volume = (volume << 7) / byte_45948 - 1;
    else
        volume = 0;
    if (chunk == -1) {
        for (i = 16; i <= 23; i++) {
            if (chunk == -1) {
                if (audiochunks_unk[i].data == 0 && byte_45D9A[i] == 0)
                    chunk = i;
            } else
                break;
        }
        if (chunk == -1) {
            best = 0xff;
            for (i = 16; i < 23; i++) {
                if (audiochunks_unk[i].priority <= best && byte_45D9A == 0) {
                    best = audiochunks_unk[i].priority;
                    chunk = i;
                }
            }
            if (chunk != -1 && audiochunks_unk[chunk].priority <= priority) {
                if (byte_45D9A[chunk] != 0)
                    byte_45D9A[chunk] = 0;
                audio_init_chunk2(chunk);
            }
        }
    }
    if (chunk == -1)
        return -1;
    offset = (((unsigned char far *)res)[6] << 2) + 8;
    audio_init_chunk(chunk, chunk, res, offset, volume, priority);
    return chunk;
}

void far audio_init_chunk2(int chunk)
{
    if (chunk < 16 || chunk > 23) return;
    audiochunks_unk[chunk].data = 0;
    audio_driver_func1E(chunk, chunk);
    audio_init_chunk(chunk, chunk, 0, 0, byte_45948, 0);
}

void far audio_enable_flag6(void)
{
    int i;
    if (audioflag6 != 1) {
        for (i = 16; i < 24; i++)
            audio_unk2(i, byte_428D6[i]);
        audioflag6 = 1;
    }
}

void far audio_disable_flag6(void)
{
    int i;
    if (audioflag6 != 0) {
        for (i = 16; i < 24; i++) {
            byte_428D6[i] = audiochunks_unk[i].volume;
            audio_unk2(i, 0);
        }
        audioflag6 = 0;
    }
}

int far audio_toggle_flag6(void)
{
    if (audioflag6 == 1) {
        audio_disable_flag6();
        return 0;
    }
    audio_enable_flag6();
    return 1;
}

int far sub_3771E(int chunk)
{
    if (audioflag6 == 0) return 1;
    if (chunk < 16 || chunk > 23) return 1;
    if (audiochunks_unk[chunk].data == 0) return 1;
    return 0;
}

void far nopsub_37750(unsigned int chunk, long value)
{
    audiochunks_unk[chunk].unk48 = value;
}

void far audio_driver_func3F(int ticks)
{
    int counter;

    if (byte_40634 == 0) {
        for (counter = byte_45950; counter > 0; counter -= 2) {
            word_4063A = 1;
            sub_37868(counter);
            word_4063A = 0;
            timer_copy_counter((long)ticks);
            timer_wait_for_dx();
        }
    } else {
        for (counter = 100; counter > 0; counter -= 2) {
            word_4063A = 1;
            unk_40636[3] = (unsigned char)counter;
            ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
            word_4063A = 0;
            timer_copy_counter((long)ticks);
            timer_wait_for_dx();
        }
    }
    sub_3736A();
    if (byte_40634 != 0) {
        timer_copy_counter(50L);
        timer_wait_for_dx();
        unk_40636[3] = 100;
        ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
    }
}

void far sub_37868(int value)
{
    int index;
    index = 0;
    while (index < byte_44290) {
        audio_unk2(index, value);
        ++index;
    }
}

void far nopsub_37898(int value)
{
    byte_45950 = value;
    sub_37868(value);
}

unsigned int far nopsub_378AE(int index) { return byte_44D06[index]; }

unsigned int far nopsub_378BC(int index) { return byte_44ACA[index]; }

int far audio_load_driver(char *filename, int unused, int signature)
{
    unsigned int len;
    char *path;
    void far *patches;

    if (signature == 0x473a)
        byte_40635 = 1;
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
    audiodriverstring2[0] = filename[len];
    audiodriverstring2[1] = filename[len + 1];
    audiodriverstring2[2] = 0;
    path = audio_make_filename(filename, "drv", "");
    audiodriverbinary = file_load_binary_nofatal(path);
    byte_45950 = 0x7f;
    byte_45948 = 0x7f;
    if (audiodriverbinary == 0)
        goto fail;
    byte_459D2 = ((unsigned char (far *)(void))audiodriverbinary)();
    if (byte_459D2 == 0 || byte_459D2 == 0xff)
        return 2;
    if (byte_459D2 > 0x7f) {
        byte_459D2 = 16;
        byte_40634 = 1;
        byte_40635 = 0;
    }
    sub_38178();
    timer_reg_callback(audiodriver_timer);
    if (byte_40634 != 0) {
        patches = file_load_binary_nofatal("mt32.plb");
        if (patches != 0) {
            ((DRVPROC)((char far *)audiodriverbinary + 0x42))(patches);
            mmgr_release(patches);
            unk_40636[3] = 100;
            ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
        }
    }
    byte_40630 = 0;
    audioflag2 = 1;
    byte_40632 = 0;
    audioflag6 = 1;
    return 0;
fail:
    fatal_error("Can't find driver!\n");
}

void far audiodrv_atexit(void)
{
    word_4063A = 1;
    if (audiodriverbinary != 0) {
        timer_remove_callback(audiodriver_timer);
        audioflag2 = 0;
        audioflag6 = 0;
        if (byte_40634 != 0) {
            unk_40636[3] = 100;
            ((DRVPROC)((char far *)audiodriverbinary + 0x3f))(4, (void far *)unk_40636);
        }
        ((DRVPROC)((char far *)audiodriverbinary + 6))();
        ((DRVPROC)((char far *)audiodriverbinary + 3))();
        mmgr_release(audiodriverbinary);
        audiodriverbinary = 0;
        byte_40634 = 0;
        byte_40635 = 0;
    }
    word_4063A = 0;
}

void far * far load_sfx_ge(char *filename, char *extension, char *kind)
{
    char buffer[4];
    void far *resource;

    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, kind));
    if (resource != 0)
        return resource;
    buffer[0] = 'P';
    buffer[1] = extension[0];
    buffer[2] = extension[1];
    buffer[3] = 0;
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, kind));
    if (resource != 0)
        return resource;
    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, "ge"));
    if (resource != 0)
        return resource;
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, "ge"));
    if (resource != 0)
        return resource;
    resource = file_load_binary_nofatal(audio_make_filename(filename, extension, ""));
    if (resource != 0)
        return resource;
    resource = file_decomp_nofatal(audio_make_filename(filename, buffer, ""));
    if (resource != 0)
        return resource;
    resource = file_load_binary_nofatal(filename);
    if (resource != 0)
        return resource;
    return resource;
}

void far sub_37C38(int value)
{
    word_4063C = value;
}

void far * far load_sfx_file(char *filename)
{
    void far *result;

    result = 0;
    if (byte_40635 != 0)
        result = load_sfx_ge(filename, "dsf", audiodriverstring2);
    if (result == 0)
        result = load_sfx_ge(filename, "sfx", audiodriverstring2);
    if (result == 0 && word_4063C != 0)
        fatal_error("cannot load sfx file %s", filename);
    return result;
}

void far * far load_song_file(char *filename)
{
    void far *result;

    result = 0;
    result = load_sfx_ge(filename, "kms", audiodriverstring2);
    if (result == 0 && word_4063C != 0)
        fatal_error("cannot load song file %s", filename);
    return result;
}

void far * far load_voice_file(char *filename)
{
    void far *result;

    result = 0;
    if (byte_40635 != 0)
        result = load_sfx_ge(filename, "dvc", audiodriverstring2);
    if (result == 0)
        result = load_sfx_ge(filename, "vce", audiodriverstring2);
    if (result == 0 && word_4063C != 0)
        fatal_error("cannot load voice file %s", filename);
    return result;
}

void far * far nopsub_37D7A(char *filename)
{
    void far *result;

    result = load_sfx_ge(filename, "slb", audiodriverstring2);
    if (result == 0 && word_4063C != 0)
        fatal_error("cannot load sample file %s", filename);
    return result;
}

void far audio_init_chunk(int first, int last, void far *res, int offset, unsigned char volume, unsigned char priority)
{
    struct AUDIOCHUNK *chunk;
    int i;
    char far *p;

    for (i = first; i <= last; i++) {
        chunk = &audiochunks_unk[i];
        chunk->unk48 = 0;
        chunk->unk22 = 0x7f;
        chunk->index = i;
        chunk->unk16 = 0x0f;
        byte_44D06[i] = 0;
        byte_44ACA[i] = 0;
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
            p = (char far *)res + offset;
            chunk->unk05 = (char far *)audioresource_get_dword((unsigned long far *)p) + 4;
            chunk->data = (char far *)audioresource_get_dword((unsigned long far *)p) + 4;
            offset += 5;
            chunk->unk2E = (char far *)res + 7;
        } else {
            chunk->data = 0;
        }
    }
}

void far audio_map_song_instruments(void far *song, void far *voice)
{
    unsigned char far *dest;
    char name[5];
    void far *instres;
    int i;
    int j;
    unsigned char far *hdr;

    name[4] = 0;
    hdr = audioresource_find(song, "hdr1");
    if (hdr != 0) {
        for (i = 0; i < hdr[6]; i++) {
            for (j = 0; j < 4; j++)
                name[j] = hdr[i * 4 + j + 7];
            dest = hdr + i * 4 + 7;
            instres = audioresource_find(voice, name);
            audioresource_copy_4_bytes(dest, (unsigned char far *)&instres);
        }
        basdres = audioresource_find(voice, "BASD");
        snarres = audioresource_find(voice, "SNAR");
        tommres = audioresource_find(voice, "TOMM");
        rideres = audioresource_find(voice, "RIDE");
        crshres = audioresource_find(voice, "CRSH");
        chhtres = audioresource_find(voice, "CHHT");
        ohhtres = audioresource_find(voice, "OHHT");
    }
}

void far sub_3803C(unsigned char far *res, void far *shapes)
{
    char far *resptr;
    char name[5];
    char far *destptr;
    void far *shapeptr;
    int i;
    int count;
    int j;

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
            shapeptr = locate_shape_nofatal(shapes, name);
            if (shapeptr != 0) {
                audioresource_copy_4_bytes((unsigned char far *)destptr, (unsigned char far *)&shapeptr);
                resptr[10] = 0xff;
            }
        }
    }
}

void far sub_38156(int index)
{
    struct AUDIOVOICE *voice;
    voice = &unk_45A26[index];
    voice->length = 1;
}

void far sub_38178(void)
{
    int i;

    word_4063A = 1;
    audio_init_chunk(0, 0x17, 0, 0, 0x7f, 0);
    for (i = 0; i < byte_459D2; i++) {
        ((DRVPROC)((char far *)audiodriverbinary + 0x1e))(i);
        unk_45A26[i].state = 0;
        unk_45A26[i].unk00 = 0xff;
        unk_45A26[i].unk02 = 0;
        unk_45A26[i].unk10 = 0;
        unk_45A26[i].unk2C = 0xff;
    }
    ((DRVPROC)((char far *)audiodriverbinary + 0x18))();
    ((DRVPROC)((char far *)audiodriverbinary + 6))();
    word_4063A = 0;
}

void far audio_map_song_tracks(unsigned char far *song)
{
    int ok_idx;
    int header_idx;
    int no;
    int numberOfChunks;
    unsigned char far *chunk_names;
    unsigned char header_total;
    unsigned char far *lo_chunk_data;
    unsigned long relative;
    unsigned char far *track_end;
    unsigned char far *cur;
    unsigned char far *p_copy;
    char name_buffer[5];
    unsigned char far *chunk_offsets;
    unsigned char param_num;

    name_buffer[4] = 0;
    numberOfChunks = audioresource_get_word((unsigned int far *)(song + 4));
    chunk_names = song + 6;
    chunk_offsets = chunk_names + numberOfChunks * 4;
    lo_chunk_data = song + numberOfChunks * 8 + 6;
    for (no = 0; no < numberOfChunks; no++) {
        cur = lo_chunk_data + (unsigned int)audioresource_get_dword((unsigned long far *)(chunk_offsets + no * 4));
        track_end = cur + (unsigned int)audioresource_get_dword((unsigned long far *)&cur);
        cur += 4;
        if (audioresource_compare_chunknames(0, (char far *)(chunk_names + no * 4), "hdr1", 4)) {
            cur += 2;
            header_total = *cur;
            cur += header_total * 4 + 1;
            header_total = *cur;
            cur++;
            for (header_idx = 0; header_idx < header_total; header_idx++) {
                audioresource_copy_n_bytes(cur, (unsigned char far *)name_buffer, 4);
                ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (char far *)chunk_names);
                if (ok_idx != -1) {
                    relative = audioresource_get_dword((unsigned long far *)(chunk_offsets + ok_idx * 4));
                    p_copy = lo_chunk_data + (unsigned int)relative;
                    audioresource_copy_4_bytes(cur, (unsigned char far *)&p_copy);
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
                    audioresource_copy_n_bytes(cur, (unsigned char far *)name_buffer, 4);
                    ok_idx = audioresource_get_chunk_index(0, numberOfChunks, name_buffer, (char far *)chunk_names);
                    if (ok_idx != -1) {
                        relative = audioresource_get_dword((unsigned long far *)(chunk_offsets + ok_idx * 4));
                        p_copy = lo_chunk_data + (unsigned int)relative;
                        audioresource_copy_4_bytes(cur, (unsigned char far *)&p_copy);
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
                    if ((unsigned int)*cur >= 0x80)
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

unsigned long far audioresource_get_dword(unsigned long far *p) { unsigned long value; value = *p; return value; }

unsigned int far audioresource_get_word(unsigned int far *p) { unsigned int value; value = *p; return value; }

void far audioresource_copy_4_bytes(unsigned char far *dst, unsigned char far *src)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst = *src;
}

void far nopsub_38570(void)
{
    int i;

    nopsub_3219D("swPause = %d, swSong = %d, bSong = %d,swSFX = %d\n",
                 byte_40630, audioflag2, byte_40632, audioflag6);
    nopsub_3219D("ubMusicVolume = %d, ubSfxVolume = %d\n", byte_45950, byte_45948);
    for (i = 0; i < 24; i++)
        nopsub_3219D("T%02x-ND=%lx,DL=%ld\n", i, audiochunks_unk[i].data, audiochunks_unk[i].unk18);
    nopsub_3219D("Press a Key\n");
    flush_stdin();
    for (i = 0; i < 16; i++)
        nopsub_3219D("H%02x - ST=%d,TP=%lx,TL=%lx\n", i, unk_45A26[i].state,
                     unk_45A26[i].position, unk_45A26[i].length);
}
