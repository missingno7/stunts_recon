#ifndef STUNTS_TIMER_H
#define STUNTS_TIMER_H

/* Game clock and timer callbacks. */
/* Timer callback registration and audio tick entry points. */
#include "tmrhook.h"
typedef void (far *callback_t)(void);
/* Shared timer declarations; DOS medium-model types. */
extern unsigned short audio_timer_reentry_lock;
extern short framerate_pad_0;
extern unsigned short frm_rate2;
extern unsigned long last_timer_callback_counter;
extern unsigned long timer_callback_counter;
extern unsigned long timer_deadline_ticks;
extern unsigned char timeraud;
extern void far setbiosticks(long);
extern int far testbiosticks(void);
extern int far testticks(void);
extern void far timer_setup_interrupt(void);
extern void far waitticks(void);

#endif
