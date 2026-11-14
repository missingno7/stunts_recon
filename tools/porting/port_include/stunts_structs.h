/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
/* Shared public aggregate definitions selected for each accepted TU layout. */
/* PORT_BUILD field aliases preserve these same-offset target members; their
   source-selected basis is mapped in AGGREGATE_VIEW_MAP.md and port-headers.md. */
#include "stunts_types.h"
#include "stunts_structs_target.h" /* shared measured target-width schemas */

/* INT 33h register packet: seven target words (14 bytes). */
typedef struct MouseRegs { uint16_t ax, bx, cx, dx, si, di, cflag; } MouseRegs;

#if defined(STUNTS_TU_audiofn)
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiofn)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/planes.c:2; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { int x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/planes.c:3; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { int vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/planes.c:4; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_planes)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_deffont)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* lib/unflipsz.c:7; SHAPE2D layout. */
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
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_unflipsz)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* lib/expandsz.c:6; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 width; I16 height; I16 s2d_hotspot_x; I16 s2d_hotspot_y;
    I16 x; I16 y; I8 s2d_plane_color0, s2d_plane_color1_bg, s2d_plane_color2_flip, s2d_plane_color3_flags;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_expandsz)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mousehk)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 lx, ly, lz; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_frontend)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/carsim.c:39; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:34; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:112; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:98; GAMEINFO layout. */
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
/* game/carsim.c:178; GAMESTATE layout. */
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
/* game/carsim.c:227; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:52; PLANE layout. */
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
/* game/carsim.c:44; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 x, y;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:26; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	
	
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/carsim.c:241; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_carsim)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
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
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/render.c:45; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:41; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:114; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:101; GAMEINFO layout. */
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
/* game/render.c:179; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:53; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX {
	I16 vals[9];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:57; PLANE layout. */
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
/* game/render.c:49; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 x, y;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:34; RECTANGLE layout. */
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
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/render.c:713; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:575; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/render.c:228; SIMD layout. */
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
/* game/render.c:717; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/render.c:276; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:592; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/render.c:261; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_render)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
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
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:8; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16S x; I16S y; I16S z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:22; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO { I8 game_playercarid[4],game_playermaterial,game_playertransmission,game_opponenttype,game_opponentcarid[4],game_opponentmaterial,game_opponenttransmission,game_trackname[9]; U16S  game_framespersec,game_recordedframes; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:18; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE { U8  state_prefix_000[0x3f5]; I8 game_inputmode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:10; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; struct VECTOR far *shape3d_verts; U16S  numprimitives; U8  numpaints; U8  reserved; void far *primitives; void far *cull1; void far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/trkscene.c:14; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT { struct TRKOBJINFO *ss_trkObjInfoPtr; I16S ss_rotY; struct SHAPE3D *ss_shapePtr,*ss_loShapePtr; U8  ss_ssOvelay; I8 ss_surfaceType,ss_ignoreZBias,ss_multiTileFlag,ss_physicalModel,scene_unk5; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:12; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO { U8  noOfBlocks,entry,exitPoint,entryType,exitType,arrowType; I16S arrowOrient; I16S *cameraDataOffset; union { struct { U8  opponent1,opponent2; } opponent; U16S  cameraOffsetOverride; } cameraOverlay; U8  opponent3,opponentSpeedCode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkscene)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
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
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/race.c:51; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:46; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:125; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:111; GAMEINFO layout. */
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
/* game/race.c:191; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:61; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:64; PLANE layout. */
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
/* game/race.c:56; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 x, y;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:38; RECTANGLE layout. */
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
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/race.c:1390; SHAPE2D layout. */
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
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/race.c:240; SIMD layout. */
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
/* game/race.c:1403; SPRITE layout. */
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
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/race.c:290; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/race.c:274; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_race)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:20; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:22; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/carsim.c:52; PLANE layout. */
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
/* game/polygon.c:21; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:19; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:24; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/polygon.c:35; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polygon)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioeng)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/uires.c:72; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/uires.c:32; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { U16S  words[6]; U8  bytes[4]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/uires.c:53; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; I8 far *verts; U16S  numprimitives; U8  numpaints, reserved; I8 far *primitives; I8 far *cull1; I8 far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/uires.c:51; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER { U8  header_numverts, header_numprimitives, header_numpaints, header_reserved; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:74; SIMD layout. */
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
/* game/uires.c:34; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; U16S  words[3]; U16  *lineofs; U16S  words2[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_uires)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/trackedt.c:62; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/trackedt.c:33; SHAPE2D layout. */
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
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/trackedt.c:43; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/trackedt.c:49; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trackedt)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
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
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_reslook)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiores)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audioply)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audiochk)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/carsim.c:52; PLANE layout. */
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
/* lib/startup.c:42; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* lib/startup.c:43; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_startup)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_lineedit)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* lib/shp2res.c:4; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16 width,height,s2d_hotspot_x,s2d_hotspot_y,pos_x,pos_y; U8 s2d_plane_color0,s2d_plane_color1_bg,s2d_plane_color2_flip,s2d_plane_color3_flags; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2res)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* lib/polarrad.c:3; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarrad)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/sphhelp2.c:6; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sphhelp2)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/wheel.c:10; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* lib/wheel.c:11; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_wheel)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp.c:7; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* lib/whlhelp.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp2.c:5; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* include/wheel.h; shared WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp2)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_whlhelp3)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mouse)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* lib/matrot.c:6; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { struct MAT3 m; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_matrot)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* lib/mcgawnd.c:6; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_mcgawnd)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_shp2file)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprrect)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_rectout)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:95; SHAPE3D layout. */
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
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* game/trkconst.c:365; track_object_info layout. */
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
/* lib/audiores.c:18; AUDIOCHUNK layout. */
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
/* lib/audiores.c:50; AUDIOVOICE layout. */
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
/* game/carsim.c:580; AUDIO_CAR_FRAME layout. */
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
/* lib/audioply.c:12; AudioChunk layout. */
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
/* lib/audioply.c:3; AudioEvent layout. */
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
/* game/audioeng.c:7; AudioPayload layout. */
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
/* lib/audioply.c:53; AudioSample layout. */
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
/* game/audioeng.c:18; AudioTimer layout. */
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
/* lib/audioply.c:37; AudioVoice layout. */
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
/* game/frontend.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:36; CARSTATE layout. */
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
/* game/race.c:619; ENGINESOUND layout. */
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
/* game/render.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/uires.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:18; GAMEINFO layout. */
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
/* game/frontend.c:56; GAMESTATE layout. */
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
/* game/frontend.c:249; GAMESTATE_SNAPSHOT layout. */
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
/* game/frontend.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/render.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* lib/matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/render.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  colors_00_0f[0x20]; I16S ground, sky; U8  colors_12_63[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/carsim.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/carsim.c:52; PLANE layout. */
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
/* game/frontend.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/polygon.c:45; POLYINFO layout. */
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
/* lib/whlhelp3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* lib/lineedit.c:4; SCREEN_RECT layout. */
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
/* game/frontend.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkconst.c:494; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { unsigned char opaque_layout[22]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/render.c:585; SHAPE3DHEADER layout. */
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
/* game/frontend.c:74; SIMD layout. */
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
/* game/frontend.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:331; SecurityDialogResult layout. */
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
/* game/carsim.c:284; TRACKOBJECT layout. */
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
/* game/carsim.c:587; TRACKRESULT layout. */
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
/* game/render.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/frontend.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* game/carsim.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/carsim.c:275; TRKOBJINFO layout. */
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
/* game/trkscene.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkscene.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* lib/whlhelp2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkconst.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkconst.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkconst.c:721; scene_shape layout. */
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
/* game/trkconst.c:496; track_object layout. */
#pragma pack(push, 2)
struct track_object {
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
  struct SHAPE3D *shape;
  struct SHAPE3D *low_detail_shape;
  unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_trkconst)
/* game/trkconst.c:365; track_object_info layout. */
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
  #ifdef PORT_BUILD
  union { struct track_object_info *info; struct TRKOBJINFO *ss_trkObjInfoPtr; };
  #else
  struct track_object_info *info;
  #endif
  #ifdef PORT_BUILD
  union { short rotation_y; short rotation; short ss_rotY; };
  #else
  short rotation_y;
  #endif
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
/* game/render.c:715; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_FARRESOURCE_obj_seg003 {
    void far *pointer;
    struct { U16S  offset, segment; } word;
} stunts_FARRESOURCE_obj_seg003;
#pragma pack(pop)

/* lib/planes.c:3; size=18; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_MATRIX_fardata_11036 {
    int m[9];
} stunts_MATRIX_fardata_11036;
#pragma pack(pop)

/* lib/lineedit.c:4; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SCREEN_RECT_obj_seg032_group {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
} stunts_SCREEN_RECT_obj_seg032_group;
#pragma pack(pop)

/* game/frontend.c:328; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg000 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg000;
#pragma pack(pop)

/* game/render.c:713; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg003 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg003;
#pragma pack(pop)

/* game/trkconst.c:494; size=22; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_SHAPE3D_track_constants_module {
    unsigned char opaque_layout[22];
} stunts_SHAPE3D_track_constants_module;
#pragma pack(pop)

/* game/frontend.c:35; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg000 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg000;
#pragma pack(pop)

/* game/render.c:717; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg003 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg003;
#pragma pack(pop)

/* game/trackedt.c:43; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg009 {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
} stunts_SPRITE_obj_seg009;
#pragma pack(pop)

/* lib/mcgawnd.c:6; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_seg033_mcgawnd {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
} stunts_SPRITE_seg033_mcgawnd;
#pragma pack(pop)

/* game/frontend.c:331; size=None; unresolved */
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

/* game/carsim.c:34; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg001_complete {
    I16S x, y, z;
} stunts_VECTOR_obj_seg001_complete;
#pragma pack(pop)

/* game/render.c:41; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg003 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg003;
#pragma pack(pop)

/* game/trkscene.c:8; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg004 {
    I16S x;
    I16S y;
    I16S z;
} stunts_VECTOR_obj_seg004;
#pragma pack(pop)

/* game/race.c:46; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg005 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg005;
#pragma pack(pop)

/* lib/planes.c:2; size=6; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_fardata_11036 {
    int x, y, z;
} stunts_VECTOR_fardata_11036;
#pragma pack(pop)

/* game/trkscene.c:20; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_WALLREC_obj_seg004 {
    I16S orientation,x,z;
} stunts_WALLREC_obj_seg004;
#pragma pack(pop)

/* game/trkconst.c:842; size=76; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_car_exp_name_table_track_constants_module {
    char entry[15][5];
    char trailing_zero;
} stunts_car_exp_name_table_track_constants_module;
#pragma pack(pop)

/* game/trkconst.c:802; size=4; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_coord_pair_track_constants_module {
    short x, z;
} stunts_coord_pair_track_constants_module;
#pragma pack(pop)

/* game/trkconst.c:721; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_scene_shape_track_constants_module {
    unsigned short opaque_first_word;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_scene_shape_track_constants_module;
#pragma pack(pop)

/* game/trkconst.c:496; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_track_constants_module {
    struct track_object_info *info;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_track_object_track_constants_module;
#pragma pack(pop)

/* game/trkconst.c:365; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_info_track_constants_module {
    unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
    short arrow_orientation;
    unsigned char *camera_data;
    unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
} stunts_track_object_info_track_constants_module;
#pragma pack(pop)

#endif /* STUNTS_INCLUDE_LAYOUT_CATALOG */
