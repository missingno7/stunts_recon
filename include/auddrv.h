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

/* Auddrv services and shared state. */
extern int far audio_check_flag(void far *, int, int, int);
#ifndef AUDRES_H
#define AUDRES_H
extern int far reserve_audio_chunk(int, int);
#endif
extern int far _loadds send_audio_stop_event(unsigned int, int);
extern void far _loadds start_audio_voice_sample(int, char far *);
#endif
