/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
/* Shared public aggregate definitions selected for each accepted TU layout. */
/* PORT_BUILD field aliases preserve these same-offset target members; their
   source-selected basis is mapped in AGGREGATE_VIEW_MAP.md and port-headers.md. */
#include "stunts_types.h"
#include "stunts_structs_target.h" /* shared measured target-width schemas */

/* INT 33h register packet: seven target words (14 bytes). */
typedef struct MouseRegs { uint16_t ax, bx, cx, dx, si, di, cflag; } MouseRegs;

#if defined(STUNTS_TU_audiofn)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/planes.c:2; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { int x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/planes.c:3; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { int m[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/planes.c:4; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
    int plane_yz;
    int plane_xy;
    struct VECTOR plane_origin;
    struct VECTOR plane_normal;
    struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/unflipsz.c:7; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    U16 s2d_hotspot_x;
    U16 s2d_hotspot_y;
    U16 s2d_pos_x;
    U16 s2d_pos_y;
    U8 s2d_plane_color0;
    U8 s2d_plane_color1_bg;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/expandsz.c:6; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 width; I16 height; I16 s2d_hotspot_x; I16 s2d_hotspot_y;
    I16 x; I16 y; I8 s2d_plane_color0, s2d_plane_color1_bg, s2d_plane_color2_flip, s2d_plane_color3_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:39; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:34; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:112; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; 
                              
	I16S car_pseudoGravity;
	I16S car_steeringAngle;
	I16S car_currpm;
	I16S car_lastrpm;
	I16S car_idlerpm2;
	I16S car_speeddiff; 
	U16S  car_speed;     
                         
	U16S  car_speed2;    
                         
                         
                         
                         
	U16S  car_lastspeed; 
	U16S  car_gearratio;
	U16S  car_gearratioshr8;
	I16S car_knob_x;
	I16S car_36MwhlAngle;
	I16S car_knob_y;
	I16S car_knob_x2;
	I16S car_knob_y2;
	I16S car_angle_z;
	I16S car_40MfrontWhlAngle;
	I16S car_grip_angle_excess;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S car_route_bearing;
	I16S car_trackdata3_index;
	I16S car_wheel_fall_step[4]; 
	I16S car_wheel_suspension_offset[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_wheel_suspension_target[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_route_center;
	struct VECTOR car_route_edge_a;
	struct VECTOR car_route_edge_b;
	I16S car_route_has_link;
	I16S field_B8;
	I16S field_BA;
	I8 car_is_braking;
	I8 car_is_accelerating;
	I8 car_current_gear;
	I8 car_sumSurfFrontWheels;
	I8 car_sumSurfRearWheels;
	I8 car_sumSurfAllWheels; 
	I8 car_surfaceWhl[4];      
	I8 car_engineLimiterTimer;
	I8 car_slidingFlag;
	I8 car_contact_processed;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 car_finish_route_passed;
	U8  car_route_point_index; 
	U8  car_audio_flags; 
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:98; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
	I8 game_playercarid[4];
	I8 game_playermaterial;
	I8 game_playertransmission;
	I8 game_opponenttype;
	I8 game_opponentcarid[4];
	I8 game_opponentmaterial;
	I8 game_opponenttransmission;
	I8 game_trackname[9];
	U16S  game_framespersec;
	U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:178; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_debris_x[24]; 
	I32 game_debris_y[24]; 
	I32 game_debris_z[24]; 
	struct VECTOR game_chase_camera_pos[2]; 
	struct VECTOR game_chase_camera_prev[2];
	I16S game_end_delay_elapsed;
	I16S game_end_delay_limit;
	I32  game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish; 
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   
	U16S  game_penalty;
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S game_player_route_anchor;
	I16S game_player_last_route_node;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S game_debris_rot_x[24];
	I16S game_debris_rot_y[24];
	I16S game_debris_rot_z[24];
	I16S game_debris_planar_step[24];
	I16S game_debris_vertical_step[24];
	I8 kevinseed[6];
	I8 game_checkpoint_valid;
	I8 game_inputmode; 
	I8 game_3F6autoLoadEvalFlag;
	I8 game_track_camera_index[2]; 
	I8 game_opponent_speed_limit;
	I8 game_track_object_broken[48];
	I8 game_debris_active;
	I8 game_debris_shape_index[24];
	I8 game_debris_origin_kind[24];
	I8 game_route_recovery_mode;
	I8 game_route_confirm_count;
	I8 game_player_guidance;
	I8 game_opponent_pass_side;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:227; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
	I32 game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish;
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;
	U16S  game_penalty;
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:44; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:26; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	
	
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:241; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	U16S  car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	U16S  downshift_rpm;
	U16S  upshift_rpm;
	U16S  max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	U8  idle_torque;
	U8  torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding[5];
	I8 simd_unk3[10];
	struct POINT2D collide_points[2];
	I16S car_height;
	struct VECTOR wheel_coords[4];
	I8 steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	I8 spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	I8 revpoints[256];
	I16S far* aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias;
  #ifdef PORT_BUILD
  union { unsigned char multi_tile; unsigned char ss_multiTileFlag; unsigned char multiTile; };
  union { unsigned char physical_model; unsigned char ss_physicalModel; unsigned char physicalModel; };
  #else
  unsigned char multi_tile, physical_model;
  #endif
  unsigned char unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:45; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:41; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:114; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; // applying the (x, y, z) vector notation to rotation
                              // angles is a source of confusion.
	I16S car_pseudoGravity;
	I16S car_steeringAngle;
	I16S car_currpm;
	I16S car_lastrpm;
	I16S car_idlerpm2;
	I16S car_speeddiff; // former gripdiff
	U16S  car_speed;     // former trackgrip
                         // value is 2^8*(mph value) and unsigned
	U16S  car_speed2;    // former trackgrip2
                         // speed is the rev-coupled speed, while speed2 is
                         // the actual car speed. They are different, for
                         // instance, during jumps (where accelerating increases
                         // revs without making the car go faster).
	U16S  car_lastspeed; // former lasttrackgrip
	U16S  car_gearratio;
	U16S  car_gearratioshr8;
	I16S car_knob_x;
	I16S car_36MwhlAngle;
	I16S car_knob_y;
	I16S car_knob_x2;
	I16S car_knob_y2;
	I16S car_angle_z;
	I16S car_40MfrontWhlAngle;
	I16S car_grip_angle_excess;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S car_route_bearing;
	I16S car_trackdata3_index;
	I16S car_wheel_fall_step[4]; // four words, one for each wheel.
	I16S car_wheel_suspension_offset[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_wheel_suspension_target[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_route_center;
	struct VECTOR car_route_edge_a;
	struct VECTOR car_route_edge_b;
	I16S car_route_has_link;
	I16S field_B8;
	I16S field_BA;
	I8 car_is_braking;
	I8 car_is_accelerating;
	I8 car_current_gear;
	I8 car_sumSurfFrontWheels;
	I8 car_sumSurfRearWheels;
	I8 car_sumSurfAllWheels; // used as jump flag.
	I8 car_surfaceWhl[4];      // surface types for each of the wheels, it seems.
	I8 car_engineLimiterTimer;
	I8 car_slidingFlag;
	I8 car_contact_processed;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 car_finish_route_passed;
	I8 car_route_point_index; // is added?
	I8 car_audio_flags; // is initialized?
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:101; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
	I8 game_playercarid[4];
	I8 game_playermaterial;
	I8 game_playertransmission;
	I8 game_opponenttype;
	I8 game_opponentcarid[4];
	I8 game_opponentmaterial;
	I8 game_opponenttransmission;
	I8 game_trackname[9];
	U16S  game_framespersec;
	U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:179; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_debris_x[24]; // x
	I32 game_debris_y[24]; // y
	I32 game_debris_z[24]; // z
	struct VECTOR game_chase_camera_pos[2]; // 0 = player, 1 = opponent
	struct VECTOR game_chase_camera_prev;
	struct VECTOR game_opponent_camera_prev;
	I16S game_end_delay_elapsed;
	I16S game_end_delay_limit;
	I32  game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S game_player_route_anchor;
	I16S game_player_last_route_node;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S game_debris_rot_x[24];
	I16S game_debris_rot_y[24];
	I16S game_debris_rot_z[24];
	I16S game_debris_planar_step[24];
	I8 game_debris_vertical_step[48];
	I8 kevinseed[6];
	I8 game_checkpoint_valid;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 game_track_camera_index[2]; // 0 = player, 1 = opponent
	I8 game_opponent_speed_limit;
	I8 game_track_object_broken[48];
	I8 game_debris_active;
	U8  game_debris_shape_index[24];
	U8  game_debris_origin_kind[24];
	I8 game_route_recovery_mode;
	I8 game_route_confirm_count;
	I8 game_player_guidance;
	I8 game_opponent_pass_side;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:53; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX {
	I16 vals[9];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:57; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:49; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:713; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:575; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
	U16S  shape3d_numverts;
	struct VECTOR far* shape3d_verts;
	U16S  shape3d_numprimitives;
	U16S  shape3d_numpaints;
	I8 far* shape3d_primitives;
	I8 far* shape3d_cull1;
	I8 far* shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:228; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	I16S car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	I16S downshift_rpm;
	I16S upshift_rpm;
	I16S max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	I8 idle_torque;
	I8 torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding;
	I16S surface_grip[4];
	I8 simd_unk3[10];
	struct POINT2D collide_points[2];
	I16S car_height;
	struct VECTOR wheel_coords[4];
	I8 steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	I8 spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	I8 revpoints[256];
	I16S far* aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:717; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:276; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
	struct TRKOBJINFO* ss_trkObjInfoPtr; // offset (0003B770)
	I16S ss_rotY;           // Horizontal orientation of the element.
	struct SHAPE3D* ss_shapePtr;       // offset (0003B770)
	struct SHAPE3D* ss_loShapePtr;     // offset (0003B770)
	U8   ss_ssOvelay;       // Renders additional sceneShapes over the current one.
	I8  ss_surfaceType;    // Paintjob. FF will induce alternating paintjobs.
	I8  ss_ignoreZBias;    // Appears to be Z-bias override flag, mostly used for roads and corners.
	I8  ss_multiTileFlag;  // 0 = one-tile, 1 = two-tile vertical, 2 = two-tile horizontal, 3 = four-tile.
	I8  ss_physicalModel;  // sets the physical model in build_track_object
	I8  scene_unk5;        // always zero.
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:592; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
	struct VECTOR pos;
	struct SHAPE3D* shapeptr;
	struct RECTANGLE* rectptr;
	struct VECTOR rotvec;
	U16S  unk;
	U8  ts_flags;
	U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/render.c:261; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
	I8  si_noOfBlocks;      // How many shapeInfo pieces compose the element. Arbitrary for the first piece, 0 for the following ones.
	I8  si_entryPoint;      // Connectivity of the track element regarding tiles.
	I8  si_exitPoint;
	I8  si_entryType;        // Connectivity of the track element regarding element types.
	I8  si_exitType;
	I8  si_arrowType;        // Type of the element for determining penalty-arrow behaviour.
	I16S si_arrowOrient;      // Orientation angle for penalty-arrow purposes
	I16S* si_cameraDataOffset; // offset (0003B770)
	I8  si_opp1;             //Appears to affect how the opponent AI approaches an element.
	I8  si_opp2;
	I8  si_opp3;
	I8  si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias;
  #ifdef PORT_BUILD
  union { unsigned char multi_tile; unsigned char ss_multiTileFlag; unsigned char multiTile; };
  union { unsigned char physical_model; unsigned char ss_physicalModel; unsigned char physicalModel; };
  #else
  unsigned char multi_tile, physical_model;
  #endif
  unsigned char unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:8; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16S x; I16S y; I16S z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:22; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO { I8 game_playercarid[4],game_playermaterial,game_playertransmission,game_opponenttype,game_opponentcarid[4],game_opponentmaterial,game_opponenttransmission,game_trackname[9]; U16S  game_framespersec,game_recordedframes; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:18; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE { U8  state_prefix_000[0x3f5]; I8 game_inputmode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:10; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; struct VECTOR far *shape3d_verts; U16S  numprimitives; U8  numpaints; U8  reserved; void far *primitives; void far *cull1; void far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:14; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT { struct TRKOBJINFO *info; I16S rotation; struct SHAPE3D *shape,*lowShape; U8  overlay; I8 surface,ignoreZ,multiTile,physicalModel,unknown; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:12; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO { U8  noOfBlocks,entry,exitPoint,entryType,exitType,arrowType; I16S arrowOrient; I16S *cameraDataOffset; union { struct { U8  opponent1,opponent2; } opponent; U16S  cameraOffsetOverride; } cameraOverlay; U8  opponent3,opponentSpeedCode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias;
  #ifdef PORT_BUILD
  union { unsigned char multi_tile; unsigned char ss_multiTileFlag; unsigned char multiTile; };
  union { unsigned char physical_model; unsigned char ss_physicalModel; unsigned char physicalModel; };
  #else
  unsigned char multi_tile, physical_model;
  #endif
  unsigned char unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:51; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:46; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:125; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; // applying the (x, y, z) vector notation to rotation
                              // angles is a source of confusion.
	I16S car_pseudoGravity;
	I16S car_steeringAngle;
	I16S car_currpm;
	I16S car_lastrpm;
	I16S car_idlerpm2;
	I16S car_speeddiff; // former gripdiff
	U16S  car_speed;     // former trackgrip
                         // value is 2^8*(mph value) and unsigned
	U16S  car_speed2;    // former trackgrip2
                         // speed is the rev-coupled speed, while speed2 is
                         // the actual car speed. They are different, for
                         // instance, during jumps (where accelerating increases
                         // revs without making the car go faster).
	U16S  car_lastspeed; // former lasttrackgrip
	U16S  car_gearratio;
	U16S  car_gearratioshr8;
	I16S car_knob_x;
	I16S car_36MwhlAngle;
	I16S car_knob_y;
	I16S car_knob_x2;
	I16S car_knob_y2;
	I16S car_angle_z;
	I16S car_40MfrontWhlAngle;
	I16S car_grip_angle_excess;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S car_route_bearing;
	I16S car_trackdata3_index;
	I16S car_wheel_fall_step[4]; // four words, one for each wheel.
	I16S car_wheel_suspension_offset[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_wheel_suspension_target[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_route_center;
	struct VECTOR car_route_edge_a;
	struct VECTOR car_route_edge_b;
	I16S car_route_has_link;
	I16S field_B8;
	I16S field_BA;
	I8 car_is_braking;
	I8 car_is_accelerating;
	I8 car_current_gear;
	I8 car_sumSurfFrontWheels;
	I8 car_sumSurfRearWheels;
	I8 car_sumSurfAllWheels; // used as jump flag.
	I8 car_surfaceWhl[4];      // surface types for each of the wheels, it seems.
	I8 car_engineLimiterTimer;
	I8 car_slidingFlag;
	I8 car_contact_processed;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 car_finish_route_passed;
	I8 car_route_point_index; // is added?
	I8 car_audio_flags; // is initialized?
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:111; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
	I8 game_playercarid[4];
	I8 game_playermaterial;
	I8 game_playertransmission;
	I8 game_opponenttype;
	I8 game_opponentcarid[4];
	I8 game_opponentmaterial;
	I8 game_opponenttransmission;
	I8 game_trackname[9];
	I8 game_framespersec;
	I16S game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:191; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_debris_x[24]; // x
	I32 game_debris_y[24]; // y
	I32 game_debris_z[24]; // z
	struct VECTOR game_chase_camera_pos[2]; // 0 = player, 1 = opponent
	struct VECTOR game_chase_camera_prev[2]; // [0] player, [1] opponent
	I16S game_end_delay_elapsed;
	I16S game_end_delay_limit;
	I32  game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S game_player_route_anchor;
	I16S game_player_last_route_node;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S game_debris_rot_x[24];
	I16S game_debris_rot_y[24];
	I16S game_debris_rot_z[24];
	I16S game_debris_planar_step[24];
	I8 game_debris_vertical_step[48];
	I8 kevinseed[6];
	I8 game_checkpoint_valid;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 game_track_camera_index[2]; // 0 = player, 1 = opponent
	I8 game_opponent_speed_limit;
	I8 game_track_object_broken[48];
	I8 game_debris_active;
	I8 game_debris_shape_index[24];
	I8 game_debris_origin_kind[24];
	I8 game_route_recovery_mode;
	I8 game_route_confirm_count;
	I8 game_player_guidance;
	I8 game_opponent_pass_side;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:61; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:64; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:56; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:38; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:1390; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16S s2d_width;
    U16S  s2d_height;
    U16S  s2d_hotspot_x;
    U16S  s2d_hotspot_y;
    U16S  s2d_pos_x;
    U16S  s2d_pos_y;
    U8  s2d_plane_color0;
    U8  s2d_plane_color1_bg;
    U8  s2d_plane_color2_flip;
    U8  s2d_plane_color3_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:240; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	I16S car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	I16S downshift_rpm;
	I16S upshift_rpm;
	I16S max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	I8 idle_torque;
	I8 torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding;
	I16S surface_grip[4];
	I8 simd_unk3[10];
	struct POINT2D collide_points[2];
	I16S car_height;
	struct VECTOR wheel_coords[4];
	U8  steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	U8  spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	U8  revpoints[256];
	I16S far* aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:1403; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
    U16S  sprite_unk3;
    U16  *sprite_lineofs;
    U16S  sprite_left;
    U16S  sprite_right;
    U16S  sprite_top;
    U16S  sprite_height;
    U16S  sprite_pitch;
    U16S  sprite_unk4;
    U16S  sprite_width2;
    U16S  sprite_left2;
    U16S  sprite_widthsum;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:290; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
	struct TRKOBJINFO* ss_trkObjInfoPtr; // offset (0003B770)
	I16S ss_rotY;           // Horizontal orientation of the element.
	struct SHAPE3D* ss_shapePtr;       // offset (0003B770)
	struct SHAPE3D* ss_loShapePtr;     // offset (0003B770)
	U8   ss_ssOvelay;       // Renders additional sceneShapes over the current one.
	I8  ss_surfaceType;    // Paintjob. FF will induce alternating paintjobs.
	I8  ss_ignoreZBias;    // Appears to be Z-bias override flag, mostly used for roads and corners.
	I8  ss_multiTileFlag;  // 0 = one-tile, 1 = two-tile vertical, 2 = two-tile horizontal, 3 = four-tile.
	I8  ss_physicalModel;  // sets the physical model in build_track_object
	I8  scene_unk5;        // always zero.
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/race.c:274; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
	I8  si_noOfBlocks;      // How many shapeInfo pieces compose the element. Arbitrary for the first piece, 0 for the following ones.
	I8  si_entryPoint;      // Connectivity of the track element regarding tiles.
	I8  si_exitPoint;
	I8  si_entryType;        // Connectivity of the track element regarding element types.
	I8  si_exitType;
	I8  si_arrowType;        // Type of the element for determining penalty-arrow behaviour.
	I16S si_arrowOrient;      // Orientation angle for penalty-arrow purposes
	I16S* si_cameraDataOffset; // offset (0003B770)
	I8  si_opp1;             //Appears to affect how the opponent AI approaches an element.
	I8  si_opp2;
	I8  si_opp3;
	I8  si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:20; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:22; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:21; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:19; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:24; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16 numverts;
    struct VECTOR far* verts;
    U16 numprimitives;
    I8 numpaints;
    I8 reserved;
    U8  far* primitives;
    I32 far* cull1;
    I32 far* cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/polygon.c:35; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D* shapeptr;
    struct RECTANGLE* rectptr;
    struct VECTOR rotvec;
    I16 unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:72; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:32; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { U16S  words[6]; U8  bytes[4]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:53; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; I8 far *verts; U16S  numprimitives; U8  numpaints, reserved; I8 far *primitives; I8 far *cull1; I8 far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:51; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER { U8  numverts, numprimitives, numpaints, reserved; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/uires.c:34; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; U16S  words[3]; U16  *lineofs; U16S  words2[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trackedt.c:62; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trackedt.c:33; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    unsigned s2d_hotspot_x;
    unsigned s2d_hotspot_y;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    U8  s2d_plane_color0;
    U8  s2d_plane_color1_bg;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trackedt.c:43; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trackedt.c:49; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    struct SHAPE3D *ss_shapePtr;
    struct SHAPE3D *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType;
    I8 ss_ignoreZBias;
    I8 ss_multiTileFlag;
    I8 ss_physicalModel;
    I8 scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias;
  #ifdef PORT_BUILD
  union { unsigned char multi_tile; unsigned char ss_multiTileFlag; unsigned char multiTile; };
  union { unsigned char physical_model; unsigned char ss_physicalModel; unsigned char physicalModel; };
  #else
  unsigned char multi_tile, physical_model;
  #endif
  unsigned char unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/startup.c:42; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 px, py; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/startup.c:43; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/shp2res.c:4; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16 width,height,s2d_hotspot_x,s2d_hotspot_y,pos_x,pos_y; U8 s2d_plane_color0,s2d_plane_color1_bg,s2d_plane_color2_flip,s2d_plane_color3_flags; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/polarrad.c:3; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/sphhelp2.c:6; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/wheel.c:10; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/wheel.c:11; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/whlhelp.c:7; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/whlhelp.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/whlhelp2.c:5; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/matrot.c:6; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { struct MAT3 m; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/mcgawnd.c:6; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:95; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audiores.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audiores.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:580; AUDIO_CAR_FRAME layout. */
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioply.c:12; AudioChunk layout. */
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioply.c:3; AudioEvent layout. */
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioeng.c:7; AudioPayload layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioply.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioeng.c:18; AudioTimer layout. */
#pragma pack(push, 2)
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
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/audioply.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/race.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:18; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:52; PLANE layout. */
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/polygon.c:45; POLYINFO layout. */
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/lineedit.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:494; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { unsigned char opaque_layout[22]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/render.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:74; SIMD layout. */
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:331; SecurityDialogResult layout. */
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:284; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/carsim.c:275; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:721; scene_shape layout. */
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* src/trkconst.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if !(defined(STUNTS_TU_audiofn) || defined(STUNTS_TU_planes) || defined(STUNTS_TU_deffont) || defined(STUNTS_TU_unflipsz) || defined(STUNTS_TU_expandsz) || defined(STUNTS_TU_heapsort) || defined(STUNTS_TU_mousehk) || defined(STUNTS_TU_frontend) || defined(STUNTS_TU_carsim) || defined(STUNTS_TU_render) || defined(STUNTS_TU_trkscene) || defined(STUNTS_TU_race) || defined(STUNTS_TU_polygon) || defined(STUNTS_TU_audioeng) || defined(STUNTS_TU_uires) || defined(STUNTS_TU_trackedt) || defined(STUNTS_TU_reslook) || defined(STUNTS_TU_audiores) || defined(STUNTS_TU_audioply) || defined(STUNTS_TU_audiochk) || defined(STUNTS_TU_startup) || defined(STUNTS_TU_lineedit) || defined(STUNTS_TU_shp2res) || defined(STUNTS_TU_polarrad) || defined(STUNTS_TU_sphhelp) || defined(STUNTS_TU_sphhelp2) || defined(STUNTS_TU_wheel) || defined(STUNTS_TU_whlhelp) || defined(STUNTS_TU_whlhelp2) || defined(STUNTS_TU_whlhelp3) || defined(STUNTS_TU_mouse) || defined(STUNTS_TU_matrot) || defined(STUNTS_TU_mcgawnd) || defined(STUNTS_TU_shp2file) || defined(STUNTS_TU_sprrect) || defined(STUNTS_TU_rectout) || defined(STUNTS_TU_toupper) || defined(STUNTS_TU_trkconst))
/* Default layout for header-only checks and new host modules. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *event_pos;                 /* 00 */
    U8 call_depth;            /* 04 */
    I8 FAR *restart_pos;                /* 05 */
    U8 return_stack_tail[12];        /* 09 */
    U8 active_voices;            /* 15 */
    U8 max_voices;            /* 16 */
    U8 unk17;            /* 17 */
    I32 event_delay;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 sample_data;                     /* 1E */
    U8 velocity;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 sustain_mode;            /* 25 */
    I16 pitch_bend;                      /* 26 */
    U8 volume;           /* 28 */
    U8 driver_parameter_29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 driver_parameter_2b;            /* 2B */
    U8 driver_parameter_2c;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *sample_table;                /* 2E */
    U8 loop_depth;            /* 32 */
    U8 loop_state[20];        /* 33 */
    U8 channel_number;            /* 47 */
    I32 end_callback;                     /* 48 */
};
#pragma pack(pop)
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 resource_index;            /* 00 */
    U8 state;            /* 01 */
    U8 priority;            /* 02 */
    U8 driver_note_pitch[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 sample_data;                     /* 10 */
    U8 envelope_modulation_state[22];        /* 14 */
    I16 owner_chunk;                      /* 2A */
    U8 channel_number;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#pragma pack(push, 2)
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct AudioChunk {
    I8 FAR *pos;
    U8 depth;
    I8 FAR *stack[4];
    U8 activeVoices;
    U8 maxVoices;
    U8 reserved17;
    U32 delay;
    U8 reserved1c[2];
    I8 FAR *data;
    U8 velocity;
    U8 resourceType;
    U8 priority;
    U8 modeValue;
    U16S pitch_bend;
    U8 volume;
    U8 driver_parameter_span[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#pragma pack(push, 2)
struct AudioEvent {
    U32 delta;
    U8 command;
    U8 param;
    U32 value;
    U8 length;
};
#pragma pack(pop)
#pragma pack(push, 2)
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
#pragma pack(pop)
#pragma pack(push, 2)
struct AudioSample {
    U8 sample_header[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 modulation_timing[4];
    U16S limit2e;
    U8 modulation_step_span[4];
    U8 loopFlags, pulsePresent;
    U8 pulse_timing[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#pragma pack(push, 2)
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
#pragma pack(pop)
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, priority, driver_note_pitch[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S envelope_value;
    U8 envelope_state, reserved17;
    U16S modulation_delay, modulation_duration;
    I16S modulation_offset;
    U16S pulse_delay, pulse_duration;
    U8 pulse_value, reserved23;
    U16S modulation_step;
    U8 modulation_direction, modulation_step_delay, pulse_step_delay, pulse_table_index;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, car_grip_angle_excess, car_demandedGrip;
    I16S car_surfacegrip_sum, car_route_bearing, car_trackdata3_index;
    I16S car_wheel_fall_step[4], car_wheel_suspension_offset[4], car_rc3[4], car_rc4[4], car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_route_center, car_route_edge_a, car_route_edge_b;
    I16S car_route_has_link, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, car_contact_processed;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 car_finish_route_passed, car_route_point_index, car_audio_flags;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_rate_numerator;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_resource_state;
	I8 far *es_resource_slots[10];
};
#pragma pack(pop)
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#pragma pack(push, 2)
struct GAMEINFO {
    I8 game_playercarid[4];
    I8 game_playermaterial;
    I8 game_playertransmission;
    I8 game_opponenttype;
    I8 game_opponentcarid[4];
    I8 game_opponentmaterial;
    I8 game_opponenttransmission;
    I8 game_trackname[9];
    U16S  game_framespersec;
    U16S  game_recordedframes;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_speed_frame_sum;
    U16S  game_frame;
    I16S game_total_finish;
    I16S game_opponent_finish_time;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#pragma pack(push, 2)
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#pragma pack(push, 2)
struct POLYINFO {
    I16 depth;
    U8  material;
    I8 numpoints;
    I8 type;
    I8 reserved;
    struct POINT2D points[10];
};
#pragma pack(pop)
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S foreground;
    I16S background;
    I16S font_header_middle[7];
    I16S line_height;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#pragma pack(push, 2)
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct SIMD {
    I8 num_gears, simd_unk;
    I16S car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    U16S  gear_ratios[7];
    struct POINT2D knob_points[7];
    I16S aero_resistance;
    I8 idle_torque, torque_curve[104], field_A3;
    I16S grip, field_A6[7], sliding, surface_grip[4];
    I8 simd_unk3[10];
    struct POINT2D collide_points[2];
    I16S car_height;
    struct VECTOR wheel_coords[4];
    I8 steeringdots[62];
    struct POINT2D spdcenter;
    I16S spdnumpoints;
    I8 spdpoints[208];
    struct POINT2D revcenter;
    I16S revnumpoints;
    I8 revpoints[256];
    I16S far *aerorestable;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#pragma pack(push, 2)
struct SecurityDialogResult {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
};
#pragma pack(pop)
#pragma pack(push, 2)
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    U16  unk;
    U8  ts_flags;
    U8  material;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#pragma pack(push, 2)
struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#pragma pack(push, 2)
struct scene_shape {
  unsigned short opaque_first_word;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct track_object {
  struct track_object_info *info;
  short rotation_y;
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_INCLUDE_LAYOUT_CATALOG)
/* Optional namespace-only layout evidence; not part of the port ABI. */
/* src/render.c:715; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_FARRESOURCE_obj_seg003 {
    void far *pointer;
    struct { U16S  offset, segment; } word;
} stunts_FARRESOURCE_obj_seg003;
#pragma pack(pop)

/* src/planes.c:3; size=18; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_MATRIX_fardata_11036 {
    int m[9];
} stunts_MATRIX_fardata_11036;
#pragma pack(pop)

/* src/lineedit.c:4; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SCREEN_RECT_obj_seg032_group {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
} stunts_SCREEN_RECT_obj_seg032_group;
#pragma pack(pop)

/* src/frontend.c:328; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg000 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg000;
#pragma pack(pop)

/* src/render.c:713; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg003 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg003;
#pragma pack(pop)

/* src/trkconst.c:494; size=22; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_SHAPE3D_track_constants_module {
    unsigned char opaque_layout[22];
} stunts_SHAPE3D_track_constants_module;
#pragma pack(pop)

/* src/frontend.c:35; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg000 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg000;
#pragma pack(pop)

/* src/render.c:717; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg003 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg003;
#pragma pack(pop)

/* src/trackedt.c:43; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg009 {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
} stunts_SPRITE_obj_seg009;
#pragma pack(pop)

/* src/mcgawnd.c:6; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_seg033_mcgawnd {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
} stunts_SPRITE_seg033_mcgawnd;
#pragma pack(pop)

/* src/frontend.c:331; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SecurityDialogResult_obj_seg000 {
    I16S first_x;
    I16S first_y;
    I16S second_x;
    I16S second_y;
    I16S third_x;
    I16S third_y;
    I16S input_x;
    I16S input_y;
    I16S trailing_state[4];
} stunts_SecurityDialogResult_obj_seg000;
#pragma pack(pop)

/* src/carsim.c:34; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg001_complete {
    I16S x, y, z;
} stunts_VECTOR_obj_seg001_complete;
#pragma pack(pop)

/* src/render.c:41; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg003 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg003;
#pragma pack(pop)

/* src/trkscene.c:8; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg004 {
    I16S x;
    I16S y;
    I16S z;
} stunts_VECTOR_obj_seg004;
#pragma pack(pop)

/* src/race.c:46; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg005 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg005;
#pragma pack(pop)

/* src/planes.c:2; size=6; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_fardata_11036 {
    int x, y, z;
} stunts_VECTOR_fardata_11036;
#pragma pack(pop)

/* src/trkscene.c:20; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_WALLREC_obj_seg004 {
    I16S orientation,x,z;
} stunts_WALLREC_obj_seg004;
#pragma pack(pop)

/* src/trkconst.c:842; size=76; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_car_exp_name_table_track_constants_module {
    char entry[15][5];
    char trailing_zero;
} stunts_car_exp_name_table_track_constants_module;
#pragma pack(pop)

/* src/trkconst.c:802; size=4; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_coord_pair_track_constants_module {
    short x, z;
} stunts_coord_pair_track_constants_module;
#pragma pack(pop)

/* src/trkconst.c:721; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_scene_shape_track_constants_module {
    unsigned short opaque_first_word;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_scene_shape_track_constants_module;
#pragma pack(pop)

/* src/trkconst.c:496; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_track_constants_module {
    struct track_object_info *info;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_track_object_track_constants_module;
#pragma pack(pop)

/* src/trkconst.c:365; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_info_track_constants_module {
    unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
    short arrow_orientation;
    unsigned char *camera_data;
    unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
} stunts_track_object_info_track_constants_module;
#pragma pack(pop)

#endif /* STUNTS_INCLUDE_LAYOUT_CATALOG */
