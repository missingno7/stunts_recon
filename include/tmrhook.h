/* Timer callback registration and audio tick entry points. */
#ifndef STUNTS_TMRHOOK_H
#define STUNTS_TMRHOOK_H
extern void far addtimer(void (far *)(void));
extern void far deltimer(void (far *)(void));
extern void far audio_add_driver_timer(void);
extern void far audio_driver_timer(void);
extern void far audio_remove_driver_timer(void);
#endif
