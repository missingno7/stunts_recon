/* Shared audrec declarations; DOS medium-model types. */
#ifndef STUNTS_AUDREC_H
#define STUNTS_AUDREC_H
struct AUDIOCHUNK;
struct AUDIOVOICE;
struct AUDIO_CAR_FRAME;
struct AudioChunk;
struct AudioEvent;
struct AudioSample;
struct AudioVoice;
struct ENGINESOUND;
#pragma pack()
struct AUDIO_CAR_FRAME {
    char reserved[6];
    short player_offsets[6];
    short opponent_offsets[6];
    short player_rpm;
    short opponent_rpm;
};
#pragma pack(1)
struct AUDIOCHUNK {
    char far *event_pos;
    unsigned char call_depth;
    char far *restart_pos;
    unsigned char return_stack_tail[12];
    unsigned char active_voices;
    unsigned char max_voices;
    unsigned char unk17;
    long event_delay;
    unsigned char unk1C;
    unsigned char unk1D;
    long sample_data;
    unsigned char velocity;
    unsigned char index;
    unsigned char priority;
    unsigned char sustain_mode;
    int pitch_bend;
    unsigned char volume;
    unsigned char driver_parameter_29;
    unsigned char unk2A;
    unsigned char driver_parameter_2b;
    unsigned char driver_parameter_2c;
    unsigned char unk2D;
    char far *sample_table;
    unsigned char loop_depth;
    unsigned char loop_state[20];
    unsigned char channel_number;
    long end_callback;
};
#pragma pack()
#pragma pack()
struct AudioChunk {
    char far *pos;
    unsigned char depth;
    char far *stack[4];
    unsigned char activeVoices;
    unsigned char maxVoices;
    unsigned char reserved17;
    unsigned long delay;
    unsigned char reserved1c[2];
    char far *data;
    unsigned char velocity;
    unsigned char resourceType;
    unsigned char priority;
    unsigned char modeValue;
    unsigned short pitch_bend;
    unsigned char volume;
    unsigned char driver_parameter_span[5];
    char far *far *samples;
    unsigned char loopDepth;
    char far *loopPos[4];
    unsigned char loopCount[4];
    unsigned char channelNumber;
    void (far *callback)(int);
};
#pragma pack()
struct AudioEvent {
    unsigned long delta;
    unsigned char command;
    unsigned char param;
    unsigned long value;
    unsigned char length;
};
#pragma pack()
struct AudioSample {
    unsigned char sample_header[0x1e];
    short level1e;
    short level20;
    short level22;
    short level24;
    short level26;
    unsigned char loopMode;
    unsigned char loopCount;
    unsigned char modulation_timing[4];
    unsigned short limit2e;
    unsigned char modulation_step_span[4];
    unsigned char loopFlags;
    unsigned char pulsePresent;
    unsigned char pulse_timing[4];
    unsigned char pulseCount;
    unsigned char pulseTable[8];
};
#pragma pack()
struct AUDIOVOICE {
    unsigned char resource_index;
    unsigned char state;
    unsigned char priority;
    unsigned char driver_note_pitch[5];
    long position;
    long length;
    long sample_data;
    unsigned char envelope_modulation_state[22];
    int owner_chunk;
    unsigned char channel_number;
    unsigned char unk2D;
};
#pragma pack()
struct AudioVoice {
    unsigned char resourceIndex;
    unsigned char active;
    unsigned char priority;
    unsigned char driver_note_pitch[5];
    unsigned long position;
    unsigned long remaining;
    char far *data;
    short envelope_value;
    unsigned char envelope_state;
    unsigned char reserved17;
    unsigned short modulation_delay;
    unsigned short modulation_duration;
    short modulation_offset;
    unsigned short pulse_delay;
    unsigned short pulse_duration;
    unsigned char pulse_value;
    unsigned char reserved23;
    unsigned short modulation_step;
    unsigned char modulation_direction;
    unsigned char modulation_step_delay;
    unsigned char pulse_step_delay;
    unsigned char pulse_table_index;
    struct AudioChunk *resource;
    unsigned char channelNumber;
    unsigned char reserved2d;
};
#pragma pack()
struct ENGINESOUND {
    int es_rate_numerator;
    int es_unk2;
    int es_unk4;
    int es_resource_state;
    char far *es_resource_slots[10];
};
typedef void (far *DRVPROC)();
#endif
