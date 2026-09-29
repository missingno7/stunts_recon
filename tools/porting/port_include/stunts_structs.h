/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
/* Shared public aggregate definitions selected for each accepted TU layout. */
/* PORT_BUILD field aliases preserve these same-offset target members; their
   source-selected basis is mapped in AGGREGATE_VIEW_MAP.md and port-headers.md. */
#include "stunts_types.h"
#include "stunts_structs_target.h" /* shared measured target-width schemas */

/* INT 33h register packet: seven target words (14 bytes). */
typedef struct MouseRegs { uint16_t ax, bx, cx, dx, si, di, cflag; } MouseRegs;

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_audio_make_filename)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_audio_make_filename)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/fardata_11036.c:2; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { int x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/fardata_11036.c:3; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { int m[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/fardata_11036.c:4; PLANE layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11036)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_fardata_11036)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_fardata_11039)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_fardata_11039)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/file_get_unflip_size.c:7; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    U16 s2d_unk1;
    U16 s2d_unk2;
    U16 s2d_pos_x;
    U16 s2d_pos_y;
    U8 s2d_unk3;
    U8 s2d_unk4;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_file_get_unflip_size)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/file_load_shape2d_expandedsize.c:6; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 width; I16 height; I16 unknown1; I16 unknown2;
    I16 x; I16 y; I8 unknown3, unknown4, unknown5, unknown6;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_file_load_shape2d_expandedsize)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_heapsort_by_order)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_heapsort_by_order)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_nopsub_36AF2)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg000)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg000)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:39; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:34; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:112; CARSTATE layout. */
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
	I16S field_42;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S field_48;
	I16S car_trackdata3_index;
	I16S car_rc1[4]; 
	I16S car_rc2[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_rc5[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_vec_unk3;
	struct VECTOR car_vec_unk4;
	struct VECTOR car_vec_unk5;
	I16S field_B6;
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
	I8 field_C8;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 field_CD;
	U8  field_CE; 
	U8  field_CF; 
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:98; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:178; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_longs1[24]; 
	I32 game_longs2[24]; 
	I32 game_longs3[24]; 
	struct VECTOR game_vec1[2]; 
	struct VECTOR game_vec3[2];
	I16S game_frame_in_sec;
	I16S game_frames_per_sec;
	I32  game_travDist;
	U16S  game_frame;
	I16S game_total_finish; 
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   
	U16S  game_penalty;
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S field_2F2;
	I16S field_2F4;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S field_2FE[24];
	I16S field_32E[24];
	I16S field_35E[24];
	I16S field_38E[24];
	I16S field_3BE[24];
	I8 kevinseed[6];
	I8 field_3F4;
	I8 game_inputmode; 
	I8 game_3F6autoLoadEvalFlag;
	I8 field_3F7[2]; 
	I8 field_3F9;
	I8 field_3FA[48];
	I8 field_42A;
	I8 field_42B[24];
	I8 field_443[24];
	I8 field_45B;
	I8 field_45C;
	I8 field_45D;
	I8 field_45E;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:227; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
	I32 game_travDist;
	U16S  game_frame;
	I16S game_total_finish;
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;
	U16S  game_penalty;
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:44; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:26; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	
	
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:241; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg001_complete)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:45; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:41; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:114; CARSTATE layout. */
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
	I16S field_42;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S field_48;
	I16S car_trackdata3_index;
	I16S car_rc1[4]; // four words, one for each wheel.
	I16S car_rc2[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_rc5[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_vec_unk3;
	struct VECTOR car_vec_unk4;
	struct VECTOR car_vec_unk5;
	I16S field_B6;
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
	I8 field_C8;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 field_CD;
	I8 field_CE; // is added?
	I8 field_CF; // is initialized?
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:101; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:179; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_longs1[24]; // x
	I32 game_longs2[24]; // y
	I32 game_longs3[24]; // z
	struct VECTOR game_vec1[2]; // 0 = player, 1 = opponent
	struct VECTOR game_vec3;
	struct VECTOR game_vec4;
	I16S game_frame_in_sec;
	I16S game_frames_per_sec;
	I32  game_travDist;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S field_2F2;
	I16S field_2F4;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S field_2FE[24];
	I16S field_32E[24];
	I16S field_35E[24];
	I16S field_38E[24];
	I8 field_3BE[48];
	I8 kevinseed[6];
	I8 field_3F4;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 field_3F7[2]; // 0 = player, 1 = opponent
	I8 field_3F9;
	I8 field_3FA[48];
	I8 field_42A;
	U8  field_42B[24];
	U8  field_443[24];
	I8 field_45B;
	I8 field_45C;
	I8 field_45D;
	I8 field_45E;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:53; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX {
	I16 vals[9];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:57; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:49; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:713; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:575; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:228; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:717; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:276; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:592; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg003.c:261; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg003)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg003)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:8; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16S x; I16S y; I16S z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:22; GAMEINFO layout. */
#pragma pack(push, 2)
struct GAMEINFO { I8 game_playercarid[4],game_playermaterial,game_playertransmission,game_opponenttype,game_opponentcarid[4],game_opponentmaterial,game_opponenttransmission,game_trackname[9]; U16S  game_framespersec,game_recordedframes; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:18; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE { U8  before_game_inputmode[0x3f5]; I8 game_inputmode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:10; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; struct VECTOR far *shape3d_verts; U16S  numprimitives; U8  numpaints; U8  reserved; void far *primitives; void far *cull1; void far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:14; TRACKOBJECT layout. */
#pragma pack(push, 2)
struct TRACKOBJECT { struct TRKOBJINFO *info; I16S rotation; struct SHAPE3D *shape,*lowShape; U8  overlay; I8 surface,ignoreZ,multiTile,physicalModel,unknown; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:12; TRKOBJINFO layout. */
#pragma pack(push, 2)
struct TRKOBJINFO { U8  noOfBlocks,entry,exitPoint,entryType,exitType,arrowType; I16S arrowOrient; I16S *cameraDataOffset; union { struct { U8  opponent1,opponent2; } opponent; U16S  cameraOffsetOverride; } cameraOverlay; U8  opponent3,opponentSpeedCode; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg004)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg004)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:51; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG {
	I32 lx, ly, lz;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:46; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR {
	I16S x, y, z;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:125; CARSTATE layout. */
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
	I16S field_42;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S field_48;
	I16S car_trackdata3_index;
	I16S car_rc1[4]; // four words, one for each wheel.
	I16S car_rc2[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_rc5[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_vec_unk3;
	struct VECTOR car_vec_unk4;
	struct VECTOR car_vec_unk5;
	I16S field_B6;
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
	I8 field_C8;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 field_CD;
	I8 field_CE; // is added?
	I8 field_CF; // is initialized?
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:111; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:191; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
	I32 game_longs1[24]; // x
	I32 game_longs2[24]; // y
	I32 game_longs3[24]; // z
	struct VECTOR game_vec1[2]; // 0 = player, 1 = opponent
	struct VECTOR game_vec3[2]; // [0] player, [1] opponent
	I16S game_frame_in_sec;
	I16S game_frames_per_sec;
	I32  game_travDist;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S field_2F2;
	I16S field_2F4;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S field_2FE[24];
	I16S field_32E[24];
	I16S field_35E[24];
	I16S field_38E[24];
	I8 field_3BE[48];
	I8 kevinseed[6];
	I8 field_3F4;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 field_3F7[2]; // 0 = player, 1 = opponent
	I8 field_3F9;
	I8 field_3FA[48];
	I8 field_42A;
	I8 field_42B[24];
	I8 field_443[24];
	I8 field_45B;
	I8 field_45C;
	I8 field_45D;
	I8 field_45E;
	I8 field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:61; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:64; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:56; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D {
	I16 px, py;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:38; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:1390; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16S s2d_width;
    U16S  s2d_height;
    U16S  s2d_unk1;
    U16S  s2d_unk2;
    U16S  s2d_pos_x;
    U16S  s2d_pos_y;
    U8  s2d_unk3;
    U8  s2d_unk4;
    U8  s2d_unk5;
    U8  s2d_unk6;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:240; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:1403; SPRITE layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:290; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg005.c:274; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg005)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg005)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:20; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:22; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:21; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:19; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:24; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D {
    unsigned numverts;
    struct VECTOR far* verts;
    unsigned numprimitives;
    I8 numpaints;
    I8 reserved;
    U8  far* primitives;
    I32 far* cull1;
    I32 far* cull2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg006.c:35; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg006)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg006)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg007)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg007)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:72; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:32; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { U16S  words[6]; U8  bytes[4]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:53; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { U16S  numverts; I8 far *verts; U16S  numprimitives; U8  numpaints, reserved; I8 far *primitives; I8 far *cull1; I8 far *cull2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:51; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER { U8  numverts, numprimitives, numpaints, reserved; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg008.c:34; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; U16S  words[3]; U16  *lineofs; U16S  words2[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg008)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg008)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg009.c:62; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg009.c:33; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    unsigned s2d_unk1;
    unsigned s2d_unk2;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    U8  s2d_unk3;
    U8  s2d_unk4;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg009.c:43; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg009.c:49; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg009)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg009)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg016_group)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg016_group)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg027)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg027)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg028)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg028)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg029)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg029)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg031.c:42; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 px, py; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg031.c:43; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg031)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg031)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg032_group)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg032_group)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg035_group.c:4; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16 width,height,unknown1,unknown2,pos_x,pos_y; U8 unknown3,unknown4,unknown5,unknown6; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_obj_seg035_group)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_obj_seg035_group)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/polarRadius3D.c:3; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_polarRadius3D)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_polarRadius3D)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/preRender_sphere_helper2.c:6; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_sphere_helper2)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/preRender_wheel.c:10; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x; I16 y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/preRender_wheel.c:11; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, unused0, unused1, unused2, unused3, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_wheel)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/preRender_wheel_helper.c:7; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/preRender_wheel_helper.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { I16 left, top, unused0, unused1, unused2, unused3, x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/preRender_wheel_helper2.c:5; Point layout. */
#pragma pack(push, 2)
struct Point { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper2)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_preRender_wheel_helper3)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_seg017_mouse_whole)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/seg024_matrot.c:6; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { struct MAT3 m; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg024_matrot)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_seg024_matrot)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/seg033_mcgawnd.c:6; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_seg033_mcgawnd)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_seg034_shape2d_group)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_sprite_1_unk4)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_sub_3702E)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_sub_3702E)
/* src/track_constants_module.c:365; track_object_info layout. */
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
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
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
/* src/obj_seg028.c:3; AudioEvent layout. */
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
/* src/obj_seg007.c:7; AudioPayload layout. */
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
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg007.c:18; AudioTimer layout. */
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
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
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
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg006.c:45; POLYINFO layout. */
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
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:95; SHAPE3D layout. */
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
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
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
/* src/obj_seg000.c:74; SIMD layout. */
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
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
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
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_toupper)
/* src/track_constants_module.c:721; scene_shape layout. */
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
/* src/track_constants_module.c:496; track_object layout. */
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
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg027.c:18; AUDIOCHUNK layout. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg027.c:50; AUDIOVOICE layout. */
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:580; AUDIO_CAR_FRAME layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg028.c:12; AudioChunk layout. */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
    I8 FAR *FAR *samples;
    U8 loopDepth;
    I8 FAR *loopPos[4];
    U8 loopCount[4];
    U8 channelNumber;
    void (FAR *callback)(I16);
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg028.c:3; AudioEvent layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg007.c:7; AudioPayload layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg028.c:53; AudioSample layout. */
#pragma pack(push, 2)
struct AudioSample {
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
    U8 pulseCount;
    U8 pulseTable[8];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg007.c:18; AudioTimer layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg028.c:37; AudioVoice layout. */
#pragma pack(push, 2)
struct AudioVoice {
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
    struct AudioChunk *resource;
    U8 channelNumber, reserved2d;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:32; VECTORLONG layout. */
#pragma pack(push, 2)
struct VECTORLONG { I32 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:31; VECTOR layout. */
#pragma pack(push, 2)
struct VECTOR { I16 x, y, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:36; CARSTATE layout. */
#pragma pack(push, 2)
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    I16S car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    I16S car_idlerpm2, car_speeddiff;
    U16S  car_speed, car_speed2, car_lastspeed;
    U16S  car_gearratio, car_gearratioshr8;
    I16S car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg005.c:619; ENGINESOUND layout. */
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg003.c:715; FARRESOURCE layout. */
#pragma pack(push, 2)
union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg008.c:59; FONTDEF_PREFIX layout. */
#pragma pack(push, 2)
struct FONTDEF_PREFIX { U8  bytes[14]; U16S  value; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:18; GAMEINFO layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:56; GAMESTATE layout. */
#pragma pack(push, 2)
struct GAMESTATE {
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:249; GAMESTATE_SNAPSHOT layout. */
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
    I16S game_pEndFrame;
    I16S game_oEndFrame;
    U16S  game_penalty;
    U16S  game_impactSpeed;
    U16S  game_topSpeed;
    I16S game_jumpCount;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:30; HighScoreRecord layout. */
#pragma pack(push, 2)
struct HighScoreRecord { U8  bytes[50]; U16S  marker; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg003.c:2635; LOOKAHEAD_TILE layout. */
#pragma pack(push, 2)
struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/seg024_matrot.c:5; MAT3 layout. */
#pragma pack(push, 2)
struct MAT3 { I16 _11, _21, _31, _12, _22, _32, _13, _23, _33; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg003.c:714; MATERIALCLRLIST layout. */
#pragma pack(push, 2)
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:49; MATRIX layout. */
#pragma pack(push, 2)
struct MATRIX { I16 vals[9]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:113; OPPONENTIMAGE layout. */
#pragma pack(push, 2)
struct OPPONENTIMAGE { U16  width, height; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:52; PLANE layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:33; POINT2D layout. */
#pragma pack(push, 2)
struct POINT2D { I16 x, y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg006.c:45; POLYINFO layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/preRender_wheel_helper3.c:5; Point layout. */
#pragma pack(push, 2)
typedef struct Point {
    I16 x;
    I16 y;
} Point;
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:34; RECTANGLE layout. */
#pragma pack(push, 2)
struct RECTANGLE { I16 left, right, top, bottom; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg032_group.c:4; SCREEN_RECT layout. */
#pragma pack(push, 2)
struct SCREEN_RECT {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:328; SHAPE2D layout. */
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:494; SHAPE3D layout. */
#pragma pack(push, 2)
struct SHAPE3D { unsigned char opaque_layout[22]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg003.c:585; SHAPE3DHEADER layout. */
#pragma pack(push, 2)
struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:74; SIMD layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:35; SPRITE layout. */
#pragma pack(push, 2)
struct SPRITE { void far *image; U16S  words[13]; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:331; SecurityDialogResult layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:284; TRACKOBJECT layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:587; TRACKRESULT layout. */
#pragma pack(push, 2)
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg003.c:716; TRANSFORMEDSHAPE layout. */
#pragma pack(push, 2)
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg000.c:104; TRANSFORMEDSHAPE3D layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:274; TRKOBJINFO_LINK_BYTES layout. */
#pragma pack(push, 2)
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg001_complete.c:275; TRKOBJINFO layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg004.c:16; TrackNode layout. */
#pragma pack(push, 2)
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/obj_seg004.c:20; WALLREC layout. */
#pragma pack(push, 2)
struct WALLREC { I16S orientation,x,z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/preRender_wheel_helper2.c:6; WheelRect layout. */
#pragma pack(push, 2)
struct WheelRect { struct Point p0, p1, p2; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:842; car_exp_name_table layout. */
#pragma pack(push, 2)
struct car_exp_name_table {
  char entry[15][5];
  char trailing_zero;
};
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:802; coord_pair layout. */
#pragma pack(push, 2)
struct coord_pair { short x, z; };
#pragma pack(pop)
#endif

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:721; scene_shape layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:496; track_object layout. */
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

#if defined(STUNTS_TU_track_constants_module)
/* src/track_constants_module.c:365; track_object_info layout. */
#pragma pack(push, 2)
struct track_object_info {
  unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
  short arrow_orientation;
  unsigned char *camera_data;
  unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
};
#pragma pack(pop)
#endif

#if !(defined(STUNTS_TU_audio_make_filename) || defined(STUNTS_TU_fardata_11036) || defined(STUNTS_TU_fardata_11039) || defined(STUNTS_TU_file_get_unflip_size) || defined(STUNTS_TU_file_load_shape2d_expandedsize) || defined(STUNTS_TU_heapsort_by_order) || defined(STUNTS_TU_nopsub_36AF2) || defined(STUNTS_TU_obj_seg000) || defined(STUNTS_TU_obj_seg001_complete) || defined(STUNTS_TU_obj_seg003) || defined(STUNTS_TU_obj_seg004) || defined(STUNTS_TU_obj_seg005) || defined(STUNTS_TU_obj_seg006) || defined(STUNTS_TU_obj_seg007) || defined(STUNTS_TU_obj_seg008) || defined(STUNTS_TU_obj_seg009) || defined(STUNTS_TU_obj_seg016_group) || defined(STUNTS_TU_obj_seg027) || defined(STUNTS_TU_obj_seg028) || defined(STUNTS_TU_obj_seg029) || defined(STUNTS_TU_obj_seg031) || defined(STUNTS_TU_obj_seg032_group) || defined(STUNTS_TU_obj_seg035_group) || defined(STUNTS_TU_polarRadius3D) || defined(STUNTS_TU_preRender_sphere_helper) || defined(STUNTS_TU_preRender_sphere_helper2) || defined(STUNTS_TU_preRender_wheel) || defined(STUNTS_TU_preRender_wheel_helper) || defined(STUNTS_TU_preRender_wheel_helper2) || defined(STUNTS_TU_preRender_wheel_helper3) || defined(STUNTS_TU_seg017_mouse_whole) || defined(STUNTS_TU_seg024_matrot) || defined(STUNTS_TU_seg033_mcgawnd) || defined(STUNTS_TU_seg034_shape2d_group) || defined(STUNTS_TU_sprite_1_unk4) || defined(STUNTS_TU_sub_3702E) || defined(STUNTS_TU_toupper) || defined(STUNTS_TU_track_constants_module))
/* Default layout for header-only checks and new host modules. */
#pragma pack(push, 1)
struct AUDIOCHUNK {                 /* 0x4C bytes */
    I8 FAR *data;                 /* 00 */
    U8 unk04;            /* 04 */
    I8 FAR *unk05;                /* 05 */
    U8 unk09[12];        /* 09 */
    U8 unk15;            /* 15 */
    U8 unk16;            /* 16 */
    U8 unk17;            /* 17 */
    I32 unk18;                     /* 18 */
    U8 unk1C;            /* 1C */
    U8 unk1D;            /* 1D */
    I32 unk1E;                     /* 1E */
    U8 unk22;            /* 22 */
    U8 index;            /* 23 */
    U8 priority;         /* 24 */
    U8 unk25;            /* 25 */
    I16 unk26;                      /* 26 */
    U8 volume;           /* 28 */
    U8 unk29;            /* 29 */
    U8 unk2A;            /* 2A */
    U8 unk2B;            /* 2B */
    U8 unk2C;            /* 2C */
    U8 unk2D;            /* 2D */
    I8 FAR *unk2E;                /* 2E */
    U8 unk32;            /* 32 */
    U8 unk33[20];        /* 33 */
    U8 unk47;            /* 47 */
    I32 unk48;                     /* 48 */
};
#pragma pack(pop)
#pragma pack(push, 2)
struct AUDIOVOICE {                 /* 0x2E bytes */
    U8 unk00;            /* 00 */
    U8 state;            /* 01 */
    U8 unk02;            /* 02 */
    U8 unk03[5];         /* 03 */
    I32 position;                  /* 08 */
    I32 length;                    /* 0C */
    I32 unk10;                     /* 10 */
    U8 unk14[22];        /* 14 */
    I16 unk2A;                      /* 2A */
    U8 unk2C;            /* 2C */
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
    U8 note;
    U8 modeValue;
    U16S value26;
    U8 program;
    U8 reserved29[5];
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
    U8 reserved00[0x1e];
    I16S level1e, level20, level22, level24, level26;
    U8 loopMode, loopCount;
    U8 reserved2a[4];
    U16S limit2e;
    U8 reserved30[4];
    U8 loopFlags, pulsePresent;
    U8 reserved36[4];
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
    U8 resourceIndex, active, note, reserved03[5];
    U32 position, remaining;
    I8 FAR *data;
    I16S sampleRate;
    U8 state16, reserved17;
    U16S value18, value1a;
    I16S value1c;
    U16S value1e, value20;
    U8 value22, reserved23;
    U16S value24;
    U8 value26, value27, value28, value29;
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
    I16S car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    I16S car_surfacegrip_sum, field_48, car_trackdata3_index;
    I16S car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    I16S field_B6, field_B8, field_BA;
    I8 car_is_braking, car_is_accelerating, car_current_gear;
    I8 car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    I8 car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    I8 car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    I8 field_CD, field_CE, field_CF;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
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
    I32 game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    I16S game_frame_in_sec, game_frames_per_sec;
    I32 game_travDist;
    I16S game_frame, game_total_finish, field_144, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S field_2F2, field_2F4, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    I8 field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    I8 field_42A, field_42B[24], field_443[24];
    I8 field_45B, field_45C, field_45D, field_45E, field_45F;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct GAMESTATE_SNAPSHOT {
    I32 game_travDist;
    U16S  game_frame;
    I16S game_total_finish;
    I16S field_144;
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
struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
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
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
};
#pragma pack(pop)
#pragma pack(push, 2)
struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
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
/* src/obj_seg003.c:715; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_FARRESOURCE_obj_seg003 {
    void far *pointer;
    struct { U16S  offset, segment; } word;
} stunts_FARRESOURCE_obj_seg003;
#pragma pack(pop)

/* src/fardata_11036.c:3; size=18; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_MATRIX_fardata_11036 {
    int m[9];
} stunts_MATRIX_fardata_11036;
#pragma pack(pop)

/* src/obj_seg032_group.c:4; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SCREEN_RECT_obj_seg032_group {
    I16S width;
    I16S height;
    I16S reserved[7];
    I16S bottom;
} stunts_SCREEN_RECT_obj_seg032_group;
#pragma pack(pop)

/* src/obj_seg000.c:328; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg000 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg000;
#pragma pack(pop)

/* src/obj_seg003.c:713; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SHAPE2D_obj_seg003 {
    I16S width, height, unk1, unk2, pos_x, pos_y;
} stunts_SHAPE2D_obj_seg003;
#pragma pack(pop)

/* src/track_constants_module.c:494; size=22; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_SHAPE3D_track_constants_module {
    unsigned char opaque_layout[22];
} stunts_SHAPE3D_track_constants_module;
#pragma pack(pop)

/* src/obj_seg000.c:35; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg000 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg000;
#pragma pack(pop)

/* src/obj_seg003.c:717; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg003 {
    void far *image;
    U16S  words[13];
} stunts_SPRITE_obj_seg003;
#pragma pack(pop)

/* src/obj_seg009.c:43; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_obj_seg009 {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
} stunts_SPRITE_obj_seg009;
#pragma pack(pop)

/* src/seg033_mcgawnd.c:6; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_SPRITE_seg033_mcgawnd {
    struct SHAPE2D FAR *sprite_bitmapptr;
    U16S sprite_words[13];
} stunts_SPRITE_seg033_mcgawnd;
#pragma pack(pop)

/* src/obj_seg000.c:331; size=None; unresolved */
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

/* src/obj_seg001_complete.c:34; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg001_complete {
    I16S x, y, z;
} stunts_VECTOR_obj_seg001_complete;
#pragma pack(pop)

/* src/obj_seg003.c:41; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg003 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg003;
#pragma pack(pop)

/* src/obj_seg004.c:8; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg004 {
    I16S x;
    I16S y;
    I16S z;
} stunts_VECTOR_obj_seg004;
#pragma pack(pop)

/* src/obj_seg005.c:46; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_obj_seg005 {
    I16S x, y, z;
} stunts_VECTOR_obj_seg005;
#pragma pack(pop)

/* src/fardata_11036.c:2; size=6; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_VECTOR_fardata_11036 {
    int x, y, z;
} stunts_VECTOR_fardata_11036;
#pragma pack(pop)

/* src/obj_seg004.c:20; size=None; unresolved */
#pragma pack(push, 2)
typedef struct stunts_WALLREC_obj_seg004 {
    I16S orientation,x,z;
} stunts_WALLREC_obj_seg004;
#pragma pack(pop)

/* src/track_constants_module.c:842; size=76; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_car_exp_name_table_track_constants_module {
    char entry[15][5];
    char trailing_zero;
} stunts_car_exp_name_table_track_constants_module;
#pragma pack(pop)

/* src/track_constants_module.c:802; size=4; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_coord_pair_track_constants_module {
    short x, z;
} stunts_coord_pair_track_constants_module;
#pragma pack(pop)

/* src/track_constants_module.c:721; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_scene_shape_track_constants_module {
    unsigned short opaque_first_word;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_scene_shape_track_constants_module;
#pragma pack(pop)

/* src/track_constants_module.c:496; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_track_constants_module {
    struct track_object_info *info;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay, surface, ignore_z_bias, multi_tile, physical_model, unknown;
} stunts_track_object_track_constants_module;
#pragma pack(pop)

/* src/track_constants_module.c:365; size=14; compiler-measured */
#pragma pack(push, 2)
typedef struct stunts_track_object_info_track_constants_module {
    unsigned char block_count, entry_point, exit_point, entry_type, exit_type, arrow_type;
    short arrow_orientation;
    unsigned char *camera_data;
    unsigned char opponent1, opponent2, opponent3, opponent_speed_code;
} stunts_track_object_info_track_constants_module;
#pragma pack(pop)

#endif /* STUNTS_INCLUDE_LAYOUT_CATALOG */
