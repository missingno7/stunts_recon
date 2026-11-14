/* Shared audio chunk and voice pools. */
#ifndef STUNTS_AUDPOOL_H
#define STUNTS_AUDPOOL_H
#include "audio.h"
extern struct AUDIOCHUNK audiochunktable[AUDIO_CHUNK_CAPACITY];
extern struct AUDIOVOICE snd_voices_tbl[AUDIO_VOICE_CAPACITY];
#endif
