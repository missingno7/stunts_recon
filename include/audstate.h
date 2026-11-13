/* Audio payload and timer state (MSC medium model). */
#ifndef AUDIO_H
#define AUDIO_H
#include "types.h"
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

/* Reviewed audio protocol values. */
#define AUDIO_ENGINE_APPROACH_SCALE 100
#define AUDIO_ENGINE_ATTENUATION_FULL_SCALE 127
#define AUDIO_ENGINE_AUDIBLE_DISTANCE 6000
#define AUDIO_ENGINE_CHANNEL_MODE_TICK_DIVIDER 2
#define AUDIO_ENGINE_DOPPLER_BASE 6000
#define AUDIO_ENGINE_LAST_VOLUME_INVALID 0xff
#define AUDIO_ENGINE_PROFILE_BYTES 0x30
#define AUDIO_ENGINE_RECEDING_ATTENUATION_SHIFT 4
#define AUDIO_ENGINE_RESOURCE_0 0
#define AUDIO_ENGINE_RESOURCE_1 1
#define AUDIO_ENGINE_RESOURCE_2 2
#define AUDIO_ENGINE_RESOURCE_3 3
#define AUDIO_ENGINE_RESOURCE_4 4
#define AUDIO_ENGINE_RESOURCE_5 5
#define AUDIO_ENGINE_RESOURCE_6 6
#define AUDIO_ENGINE_RESOURCE_7 7
#define AUDIO_ENGINE_SECONDARY_VOLUME_BIAS 10
#define AUDIO_ENGINE_SHAPE_RATE_BIAS_OFFSET 0x0f
#define AUDIO_ENGINE_SHAPE_RATE_BIAS_SHIFT 4
#define AUDIO_ENGINE_SHAPE_RATE_DIVISOR_OFFSET 0x0e
#define AUDIO_ENGINE_SLOT_CACHED_RATE 0
#define AUDIO_ENGINE_SLOT_CONTINUOUS_VOICE 1
#define AUDIO_ENGINE_SLOT_PRIMARY_EFFECT 2
#define AUDIO_ENGINE_SLOT_SECONDARY_EFFECT 3
#define AUDIO_ENGINE_SMOOTH_DIV_SHIFT 3
#define AUDIO_ENGINE_SMOOTH_FRACTION_BITS 4
#define AUDIO_ENGINE_SMOOTH_HISTORY_WEIGHT 7
#define AUDIO_ENGINE_TIMER_COUNT 25
#define AUDIO_INVALID_HANDLE -1
#define AUDIO_PRIORITY_EFFECT_DEFAULT 0x40
#define AUDIO_PRIORITY_ENGINE_FOURTH_RESOURCE 0x64
#define AUDIO_PRIORITY_ENGINE_RESERVED 0x7f
#endif
