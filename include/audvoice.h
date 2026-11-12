/* Voice commands use entries that establish DS. */
#ifndef STUNTS_AUDVOICE_H
#define STUNTS_AUDVOICE_H
extern int far _loadds send_audio_stop_event(unsigned int, int);
extern void far _loadds set_audio_voice_value(int, int);
#endif
