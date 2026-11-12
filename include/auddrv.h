/* Audio driver services used by the engine. */
#ifndef STUNTS_AUDDRV_H
#define STUNTS_AUDDRV_H
extern unsigned char audioflag6;
extern unsigned char audio_driver_mode;
extern int far sseqds(void);
extern void far release_audio_chunk(int);
extern void far reset_audio_voice_length(int);
extern void far *far init_audio_resources(void far *, void far *, char *);
extern void far audio_init_chunk2(int);
extern int far audio_chunk_is_unavailable(int);
#endif
