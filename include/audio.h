#ifndef STUNTS_AUDIO_H
#define STUNTS_AUDIO_H

/* Audio records, resources and driver operations. */
/* Audio pool limits shared by the loader and event player. */
#define AUDIO_CHUNK_CAPACITY 24
#define AUDIO_VOICE_CAPACITY 16
/* Shared audrec declarations; DOS medium-model types. */
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
/* Protocol and geometry values used by the shared routines. */
/* Audio driver services used by the engine. */
#include "auddrv.h"
/* Voice commands use entries that establish DS. */
extern void far _loadds set_audio_voice_value(int, int);
/* Shared audapi declarations; unnamed prototype parameters keep C1 allocation stable. */
struct AudioChunk;
struct AudioEvent;
struct AudioVoice;
extern const char aLocatesound4_4sSoundNotF[];
extern unsigned int audio_bit_masks[17];
extern unsigned char audio_driver_extension_mode;
extern unsigned char audio_driver_volume_command[4];
extern char audio_frmarr[1360];
extern int audio_load_error_policy;
extern unsigned char audio_pause_in_progress;
extern unsigned char audio_song_ready;
extern int audio_tick_divider;
extern unsigned char audioblock[24];
extern unsigned char audiochnk_actflags[24];
extern char audiofiletmp[128];
extern unsigned char audioflag2;
extern char audiooppflag;
extern unsigned char block_audio_num;
extern int g_audio_frms_ix;
extern unsigned char g_audiodrvvoices_count;
extern char g_drvaudiocode[14];
extern unsigned char g_musicvolumesetting;
extern int g_player_sound_id;
extern char g_plyr_snd_state;
extern void far *musicfile;
extern int op_eng_sound_id;
extern short voicefile_gap_c[4];
extern void far *openvfile;
extern unsigned char sfx_audio_vol;
extern int snd_tick_clock;
extern void far _loadds apply_audio_voice_event(int, unsigned char, int);
extern int far audio_check_flag2(void far *, int, unsigned char);
extern void far audio_debug_dump_state(void);
extern void far audio_disable_flag2(void);
extern void far audio_disable_flag6(void);
extern void far audio_dispatch_car_events(char, short);
extern void far _loadds audio_driver_func1E(int, int);
extern void far audio_driver_func3F(int);
extern void far audio_enable_flag2(void);
extern void far audio_enable_flag6(void);
extern void far audio_function2(int);
extern void far audio_function2_wrap(int);
extern unsigned int far audio_get_chunk_marker(int);
extern unsigned int far audio_get_chunk_note(int);
extern int far audio_load_driver(char *, int, int);
extern void far *far audio_load_sample_bank(char *);
extern char *far audio_make_filename(char *, char *, char *);
extern void far audio_map_song_instruments(void far *, void far *);
extern int far audio_music_is_idle(void);
extern void far audio_op_unk(int);
extern void far audio_op_unk2(int, unsigned int, int, int, int, int, int, int, unsigned int);
extern void far audio_op_unk3(int);
extern void far audio_op_unk4(int);
extern void far audio_op_unk5(int);
extern void far audio_op_unk6(int);
extern void far audio_op_unk7(int);
extern int far audio_play_resource_default(void far *);
extern void far audio_set_chunk_callback(unsigned int, long);
extern void far audio_set_music_volume(int);
extern void far restoretimer(void);
extern int far audio_toggle_flag2(void);
extern int far audio_toggle_flag6(void);
extern void far audio_unload(void);
extern void far audiodrv_atexit(void);
extern void far audioresource_copy_4_bytes(unsigned char far *, unsigned char far *);
extern void far _loadds clear_audio_voice(struct AudioVoice *);
extern void far file_find_sounds_required(char far *, char *, char far **);
extern void far file_load_audio_resource(const char *, const char *, const char *);
extern char far *far _loadds find_audio_chunk_data(unsigned char, struct AudioChunk *);
extern void far link_audio_shape_resources(unsigned char far *, void far *);
extern void far load_audio_finalize(void far *);
extern void far *far load_song_file(char *);
extern void far *far load_voice_file(char *);
extern char far *far locate_sound_fatal(char far *, char *);
extern void far _loadds process_audio_chunk_event(int);
extern int far _loadds process_audio_event(struct AudioEvent *, int);
extern void far _loadds process_effect_audio_chunks(void);
extern void far _loadds process_music_audio_chunks(void);
extern void far _loadds read_audio_event(struct AudioEvent *, unsigned char far *);
extern void far reset_audio_chunks(void);
extern void far reset_audio_driver_state(void);
extern void far _loadds reset_audio_event_state(void);
extern void far restore_audio_volume(void);
extern int far _loadds select_audio_voice_slot(char far *, struct AudioChunk *);
extern void far set_all_audio_chunk_volume(int);
extern void far set_audio_load_error_policy(int);
extern void far _loadds set_audio_voice_parameter(int, int);
extern void far _loadds update_audio_voice_state(void);
/* Protocol and geometry values used by the shared routines. */
#define AUDIO_ALL_CHUNK_LAST 0x17
#define AUDIO_CALL_EVENT_PREFIX_BYTES 2
#define AUDIO_CAR_FRAME_RING_COUNT 40
#define AUDIO_CHANNEL_NIBBLE_MASK 0x0f
#define AUDIO_CHANNEL_NUMBER_LIMIT 0x10
#define AUDIO_CONTROLLER_HOLD 0x40
#define AUDIO_DRIVER_CHANNEL_MODE_THRESHOLD 0x7f
#define AUDIO_DRIVER_COMMAND_BYTES 4
#define AUDIO_DRIVER_COMMAND_VOLUME_INDEX 3
#define AUDIO_DRIVER_ENTRY_BIND_SAMPLE 0x21
#define AUDIO_DRIVER_ENTRY_FORCE_OFF_VOICE 0x0f
#define AUDIO_DRIVER_ENTRY_INITIALIZE 0x18
#define AUDIO_DRIVER_ENTRY_LOAD_PATCHES 0x42
#define AUDIO_DRIVER_ENTRY_RELEASE_VOICE 0x0c
#define AUDIO_DRIVER_ENTRY_RESET 6
#define AUDIO_DRIVER_ENTRY_RESET_CHANNEL 0x1e
#define AUDIO_DRIVER_ENTRY_SEND_COMMAND 0x3f
#define AUDIO_DRIVER_ENTRY_SEND_RAW_BYTES 0x39
#define AUDIO_DRIVER_ENTRY_SET_CHUNK_PARAMETER 0x1b
#define AUDIO_DRIVER_ENTRY_SET_CONTROLLER 0x15
#define AUDIO_DRIVER_ENTRY_SET_PROGRAM 0x12
#define AUDIO_DRIVER_ENTRY_SET_VOICE_VALUE 0x24
#define AUDIO_DRIVER_ENTRY_SHUTDOWN 3
#define AUDIO_DRIVER_ENTRY_START_NOTE 9
#define AUDIO_DRIVER_INIT_FAILURE_FF 0xff
#define AUDIO_DRIVER_MODE_CHANNEL 1
#define AUDIO_DRIVER_MODE_VOICE 0
#define AUDIO_DRIVER_VOLUME_FULL 100
#define AUDIO_EFFECT_CHUNK_FIRST 0x10
#define AUDIO_EFFECT_CHUNK_LAST 23
#define AUDIO_EFFECT_PROCESS_END 0x17
#define AUDIO_EFFECT_VOLUME_SCALE_SHIFT 7
#define AUDIO_ENGINE_EVENT_DURATION -32L
#define AUDIO_EVENT_BASE128_CONTINUATION 0x80
#define AUDIO_EVENT_BASE128_PAYLOAD_MASK 0x7f
#define AUDIO_EVENT_BASE128_SHIFT 7
#define AUDIO_EVENT_CALL 0xe6
#define AUDIO_EVENT_CASE_CALL 13
#define AUDIO_EVENT_CASE_CHANNEL 16
#define AUDIO_EVENT_CASE_CHUNK_VALUE 17
#define AUDIO_EVENT_CASE_CONTROLLER 6
#define AUDIO_EVENT_CASE_LOOP_BEGIN 9
#define AUDIO_EVENT_CASE_LOOP_END 10
#define AUDIO_EVENT_CASE_MAX_VOICES 7
#define AUDIO_EVENT_CASE_META 14
#define AUDIO_EVENT_CASE_NOTE_PRIORITY 8
#define AUDIO_EVENT_CASE_PROGRAM_VALUE 5
#define AUDIO_EVENT_CASE_RAW_BYTES 15
#define AUDIO_EVENT_CASE_RESTART 2
#define AUDIO_EVENT_CASE_RETURN 0
#define AUDIO_EVENT_CASE_SELECT_RESOURCE 3
#define AUDIO_EVENT_CASE_STOP 1
#define AUDIO_EVENT_CASE_TEMPO 4
#define AUDIO_EVENT_CASE_VELOCITY 11
#define AUDIO_EVENT_CASE_WORD_PARAMETER 12
#define AUDIO_EVENT_CHANNEL 0xe9
#define AUDIO_EVENT_CHANNEL_MODE_RATE_NOTE 0x3c
#define AUDIO_EVENT_CHUNK_VALUE 0xea
#define AUDIO_EVENT_CONTROLLER 0xdf
#define AUDIO_EVENT_CONTROL_FIRST 0xd9
#define AUDIO_EVENT_EXPLICIT_RATE 0xff
#define AUDIO_EVENT_LOOP_BEGIN 0xe2
#define AUDIO_EVENT_LOOP_END 0xe3
#define AUDIO_EVENT_MAX_VOICES 0xe0
#define AUDIO_EVENT_META 0xe7
#define AUDIO_EVENT_NOTE_MASK 0x7f
#define AUDIO_EVENT_NOTE_PRIORITY 0xe1
#define AUDIO_EVENT_NOTE_VELOCITY_BOUNDARY 0x80
#define AUDIO_EVENT_PROGRAM_VALUE 0xde
#define AUDIO_EVENT_RAW_BYTES 0xe8
#define AUDIO_EVENT_RESTART 0xdb
#define AUDIO_EVENT_RETURN 0xd9
#define AUDIO_EVENT_SELECT_RESOURCE 0xdc
#define AUDIO_EVENT_STOP 0xda
#define AUDIO_EVENT_TARGET_BYTES 4
#define AUDIO_EVENT_TEMPO 0xdd
#define AUDIO_EVENT_VELOCITY 0xe4
#define AUDIO_EVENT_WORD_BYTES 2
#define AUDIO_EVENT_WORD_PARAMETER 0xe5
#define AUDIO_EXTENSION_SIGNATURE 0x473a
#define AUDIO_FRAME_BYTES 0x22
#define AUDIO_FRAME_CAPACITY 0x28
#define AUDIO_HDR1_FIRST_TRACK_OFFSET_BASE 8
#define AUDIO_HDR1_INSTRUMENTS_OFFSET 7
#define AUDIO_HDR1_INSTRUMENT_COUNT_OFFSET 6
#define AUDIO_HDR1_INSTRUMENT_POINTER_BYTES 4
#define AUDIO_HDR1_POINTER_SCALE_SHIFT 2
#define AUDIO_HDR1_POST_SPAN_PREFIX_BYTES 2
#define AUDIO_HDR1_READY 1
#define AUDIO_HDR1_READY_OFFSET 5
#define AUDIO_HDR1_TRACK_ENTRY_BYTES 5
#define AUDIO_HDR1_TYPE_OFFSET 4
#define AUDIO_INVALID_RESOURCE_INDEX 0xff
#define AUDIO_LEVEL_STATE_FALL 2
#define AUDIO_LEVEL_STATE_HOLD 3
#define AUDIO_LEVEL_STATE_OFF 0
#define AUDIO_LEVEL_STATE_RELEASE 4
#define AUDIO_LEVEL_STATE_RISE 1
#define AUDIO_LOAD_ERROR_DEVICE_INIT 2
#define AUDIO_LOOP_DIRECTION_ADD 1
#define AUDIO_LOOP_DIRECTION_SUBTRACT 2
#define AUDIO_LOOP_DURATION_INFINITE 0x7fff
#define AUDIO_LOOP_REVERSE_AT_LOWER 1
#define AUDIO_LOOP_REVERSE_AT_UPPER 2
#define AUDIO_MASK_TABLE_WORDS 17
#define AUDIO_MAX_VOICES_DEFAULT 0x0f
#define AUDIO_MUSIC_CHUNK_LAST 0x0f
#define AUDIO_MUSIC_CHUNK_LIMIT 16
#define AUDIO_MUSIC_PHASE_DEFAULT_LIMIT 0x80
#define AUDIO_MUSIC_PHASE_INCREMENT 0x80
#define AUDIO_MUSIC_RESOURCE_INDEX_LAST 15
#define AUDIO_ONE_PARAM_EVENT_BYTES 2
#define AUDIO_PERCUSSION_NOTE_BASS_DRUM 0x18
#define AUDIO_PERCUSSION_NOTE_CLOSED_HIHAT 0x1e
#define AUDIO_PERCUSSION_NOTE_CRASH 0x27
#define AUDIO_PERCUSSION_NOTE_RIDE 0x25
#define AUDIO_PERCUSSION_NOTE_SNARE 0x1a
#define AUDIO_PRIORITY_MUSIC 0x20
#define AUDIO_PRIORITY_SEARCH_INITIAL 0xff
#define AUDIO_PRIORITY_STEAL_SCAN_END 23
#define AUDIO_PULSE_INDEX_MASK 7
#define AUDIO_RAW_EVENT_DRIVER_COUNT_BIAS 4
#define AUDIO_RELOC_NOTE_PARAM_BOUNDARY 0x80
#define AUDIO_RESET_VOICE_OWNER 0xff
#define AUDIO_SAMPLE_CHANNEL_OFFSET 0x43
#define AUDIO_SAMPLE_KIND_OFFSET 5
#define AUDIO_SAMPLE_KIND_PERCUSSION 5
#define AUDIO_SAMPLE_LEVEL_WORD_INDEX 0x0e
#define AUDIO_SAMPLE_LOOP_DELAY_OFFSET 0x2a
#define AUDIO_SAMPLE_LOOP_DIRECTION_OFFSET 0x34
#define AUDIO_SAMPLE_LOOP_DURATION_OFFSET 0x2c
#define AUDIO_SAMPLE_LOOP_STEP_OFFSET 0x30
#define AUDIO_SAMPLE_NOTE_BIAS_OFFSET 0x10
#define AUDIO_SAMPLE_PULSE_DELAY_OFFSET 0x36
#define AUDIO_SAMPLE_PULSE_DURATION_OFFSET 0x38
#define AUDIO_SAMPLE_VOICE_MASK_OFFSET 0x0c
#define AUDIO_SHAPE_LINK_KIND_3 3
#define AUDIO_SHAPE_LINK_KIND_6 6
#define AUDIO_SHAPE_LINK_KIND_OFFSET 5
#define AUDIO_SHAPE_LINK_READY 0xff
#define AUDIO_SHAPE_LINK_READY_OFFSET 10
#define AUDIO_SHAPE_LINK_TARGET_OFFSET 6
#define AUDIO_SUBTRACK_NAME_BYTES 4
#define AUDIO_TEMPO_NUMERATOR 32000U
#define AUDIO_TRACK_SPAN_BYTES 4
#define AUDIO_TWO_PARAM_EVENT_BYTES 3
#define AUDIO_UNASSIGNED_CHUNK_CHANNEL 0xff
#define AUDIO_UNASSIGNED_HARDWARE_CHANNEL 0xff
#define AUDIO_VELOCITY_DEFAULT 0x7f
#define AUDIO_VOICE_IDLE 0
#define AUDIO_VOICE_RELEASED 2
#define AUDIO_VOICE_SOUNDING 1
#define AUDIO_VOLUME_FADE_SETTLE_TICKS 50L
#define AUDIO_VOLUME_FADE_STEP 2
#define AUDIO_VOLUME_MAX 0x7f
#define AUDIO_WORD_PARAMETER_CENTER 0x2000
#define AUDIO_WORD_PARAMETER_HIGH_BYTE_MASK 0xff00
#define AUDIO_WORD_PARAMETER_LOW_BYTE_SIGN_BIT 0x80
#define AUD_DRV_ENTRY_UPDATE_VOICE 0x27
#define AUD_DRV_ENTRY_UPDATE_VOICE_ARRAY 0x30
#define AUD_PERC_NOTE_OPEN_HIHAT_A 0x20
#define AUD_PERC_NOTE_OPEN_HIHAT_B 0x22
#define CAR_AUDIO_OP3_FLAG 0x20
#define CAR_AUDIO_OP4_FLAG 0x10
#define CAR_AUDIO_SKID_MASK 6
#define CAR_AUDIO_SKID_OTHER 4
#define CAR_AUDIO_SKID_PRIMARY 2
#define CAR_AUDIO_WALL_IMPACT 0x10

/* Audio services and shared state. */
struct AUDIO_CAR_FRAME;
struct CARSTATE;
struct SIMD;
extern unsigned short audio_update_lock;
extern char far *audiodriverbinary;
extern char is_audioloaded;
extern unsigned short mus_samplelimit;
extern unsigned short snd_sample_rate_phase;
extern void far audio_map_song_tracks(void far *);
extern void far audio_pause_playback(void);
extern int far audioresource_compare_chunknames(int, unsigned char far *, unsigned char far *, int);
extern void far audioresource_copy_n_bytes(unsigned char far *, char far *, int);
extern char far * far audioresource_find(char huge *, unsigned char *);
extern int far audioresource_get_chunk_index(int, int, unsigned char *, char far *);
extern unsigned long far audioresource_get_dword(char far *);
extern unsigned int far audioresource_get_word(unsigned int far *);
extern void doengine(char, char, struct CARSTATE*, struct SIMD*);
extern void myupdatesfx1(struct AUDIO_CAR_FRAME *, short);
#endif
