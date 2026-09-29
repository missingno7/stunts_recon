/* PORTING ONLY: canonical target-width aggregate schemas, not part of matching builds. */
/* Field extents/offsets below were probed with pinned MSC 5.10 /AM /O /Gs. */
#ifndef STUNTS_STRUCTS_TARGET_H
#define STUNTS_STRUCTS_TARGET_H
#include "stunts_types.h"
#include <stddef.h>
#include <stdint.h>

/* 16:16 and 16-bit target addresses are storage values, never host pointers. */
typedef struct { uint16_t offset, segment; } st_target_far_dataptr16_t;
typedef uint16_t st_target_near_dataptr16_t;
typedef struct { uint16_t offset, segment; } st_target_far_codeptr16_t;
#pragma pack(push, 1)
typedef struct { uint16_t offset, segment; } st_target_far_dataptr_array16_t;
#pragma pack(pop)

#pragma pack(push, 2)
typedef union stunts_AUDIOCHUNK stunts_AUDIOCHUNK;
typedef union stunts_AUDIOVOICE stunts_AUDIOVOICE;
typedef struct stunts_AUDIO_CAR_FRAME stunts_AUDIO_CAR_FRAME;
typedef struct stunts_AudioEvent stunts_AudioEvent;
typedef struct stunts_AudioPayload stunts_AudioPayload;
typedef struct stunts_AudioSample stunts_AudioSample;
typedef struct stunts_AudioTimer stunts_AudioTimer;
typedef struct stunts_CARSTATE stunts_CARSTATE;
typedef struct stunts_CRACK_LINE stunts_CRACK_LINE;
typedef struct stunts_ENGINESOUND stunts_ENGINESOUND;
typedef union stunts_FARRESOURCE stunts_FARRESOURCE;
typedef struct stunts_FONTDEF_PREFIX stunts_FONTDEF_PREFIX;
typedef struct stunts_GAMEINFO stunts_GAMEINFO;
typedef struct stunts_GAMESTATE_1014 stunts_GAMESTATE_1014;
typedef struct stunts_GAMESTATE_1120 stunts_GAMESTATE_1120;
typedef struct stunts_GAMESTATE_SNAPSHOT stunts_GAMESTATE_SNAPSHOT;
typedef struct stunts_HighScoreRecord stunts_HighScoreRecord;
typedef struct stunts_LOOKAHEAD_TILE stunts_LOOKAHEAD_TILE;
typedef struct stunts_MATERIALCLRLIST stunts_MATERIALCLRLIST;
typedef struct stunts_MATRIX stunts_MATRIX;
typedef struct stunts_MouseRegs stunts_MouseRegs;
typedef struct stunts_OPPONENTIMAGE stunts_OPPONENTIMAGE;
typedef struct stunts_PLANE stunts_PLANE;
typedef struct stunts_POINT2D stunts_POINT2D;
typedef struct stunts_POLYINFO stunts_POLYINFO;
typedef struct stunts_RECTANGLE stunts_RECTANGLE;
typedef struct stunts_SCREEN_RECT stunts_SCREEN_RECT;
typedef struct stunts_SHAPE2D_12 stunts_SHAPE2D_12;
typedef struct stunts_SHAPE2D_14 stunts_SHAPE2D_14;
typedef struct stunts_SHAPE2D_16 stunts_SHAPE2D_16;
typedef struct stunts_SHAPE3D stunts_SHAPE3D;
typedef struct stunts_SHAPE3DHEADER stunts_SHAPE3DHEADER;
typedef struct stunts_SIMD stunts_SIMD;
typedef struct stunts_SPRITE_8 stunts_SPRITE_8;
typedef struct stunts_SPRITE_30 stunts_SPRITE_30;
typedef struct stunts_SecurityDialogResult stunts_SecurityDialogResult;
typedef struct stunts_TRACKOBJECT stunts_TRACKOBJECT;
typedef struct stunts_TRACKRESULT stunts_TRACKRESULT;
typedef struct stunts_TRANSFORMEDSHAPE3D stunts_TRANSFORMEDSHAPE3D;
typedef struct stunts_TRKOBJINFO stunts_TRKOBJINFO;
typedef struct stunts_TRKOBJINFO_LINK_BYTES stunts_TRKOBJINFO_LINK_BYTES;
typedef struct stunts_TrackNode stunts_TrackNode;
typedef struct stunts_VECTOR stunts_VECTOR;
typedef struct stunts_VECTORLONG stunts_VECTORLONG;
typedef struct stunts_WALLREC stunts_WALLREC;
typedef struct stunts_WheelRect_12 stunts_WheelRect_12;
typedef struct stunts_WheelRect_16 stunts_WheelRect_16;
typedef struct stunts_car_exp_name_table stunts_car_exp_name_table;
typedef struct stunts_coord_pair stunts_coord_pair;

/* stunts_AUDIO_CAR_FRAME: target size 34; representative src/obj_seg001_complete.c:541; measured packing 2. */
struct stunts_AUDIO_CAR_FRAME {
        int8_t reserved[6];
        int16_t player_offsets[6];
        int16_t opponent_offsets[6];
        int16_t player_rpm;
        int16_t opponent_rpm;
};

/* stunts_AudioEvent: target size 12; representative src/obj_seg028.c:2; measured packing 2. */
struct stunts_AudioEvent {
        uint32_t delta;
        uint8_t command;
        uint8_t param;
        uint32_t value;
        uint8_t length;
};

/* stunts_AudioPayload: target size 48; representative src/obj_seg007.c:7; measured packing 2. */
struct stunts_AudioPayload {
        uint16_t sample_word;
        uint16_t unk2;
        uint16_t unk4;
        uint8_t resource_ready;
        uint8_t unk7;
        st_target_far_dataptr16_t shape;
        uint32_t unkC;
        st_target_far_dataptr_array16_t resources[8];
};

/* stunts_AudioSample: target size 68; representative src/obj_seg028.c:51; measured packing 2. */
struct stunts_AudioSample {
        uint8_t reserved00[0x1e];
        int16_t level1e;
        int16_t level20;
        int16_t level22;
        int16_t level24;
        int16_t level26;
        uint8_t loopMode;
        uint8_t loopCount;
        uint8_t reserved2a[4];
        uint16_t limit2e;
        uint8_t reserved30[4];
        uint8_t loopFlags;
        uint8_t pulsePresent;
        uint8_t reserved36[4];
        uint8_t pulseCount;
        uint8_t pulseTable[8];
};

/* stunts_AudioTimer: target size 76; representative src/obj_seg007.c:18; measured packing 2. */
struct stunts_AudioTimer {
        uint8_t active;
        uint8_t state;
        int16_t handle;
        uint16_t pitch_avg;
        uint32_t rate_avg;
        uint8_t pitch;
        uint8_t unkB;
        uint16_t rate;
        uint8_t last_pitch;
        uint8_t unkF;
        int16_t channels[4];
        uint16_t sample_word;
        uint8_t dirty;
        uint8_t retrigger;
        stunts_AudioPayload payload;
};

/* stunts_VECTOR: target size 6; representative src/obj_seg004.c:2; measured packing 2. */
struct stunts_VECTOR {
        int16_t x;
        int16_t y;
        int16_t z;
};

/* stunts_VECTORLONG: target size 12; representative src/obj_seg001_complete.c:15; measured packing 2. */
struct stunts_VECTORLONG {
        int32_t lx;
        int32_t ly;
        int32_t lz;
};

/* stunts_CARSTATE: target size 208; representative src/obj_seg001_complete.c:79; measured packing 2. */
struct stunts_CARSTATE {
        stunts_VECTORLONG car_posWorld1;
        stunts_VECTORLONG car_posWorld2;
        stunts_VECTOR car_rotate;
        int16_t car_pseudoGravity;
        int16_t car_steeringAngle;
        int16_t car_currpm;
        int16_t car_lastrpm;
        int16_t car_idlerpm2;
        int16_t car_speeddiff;
        uint16_t car_speed;
        uint16_t car_speed2;
        uint16_t car_lastspeed;
        uint16_t car_gearratio;
        uint16_t car_gearratioshr8;
        int16_t car_knob_x;
        int16_t car_36MwhlAngle;
        int16_t car_knob_y;
        int16_t car_knob_x2;
        int16_t car_knob_y2;
        int16_t car_angle_z;
        int16_t car_40MfrontWhlAngle;
        int16_t field_42;
        int16_t car_demandedGrip;
        int16_t car_surfacegrip_sum;
        int16_t field_48;
        int16_t car_trackdata3_index;
        int16_t car_rc1[4];
        int16_t car_rc2[4];
        int16_t car_rc3[4];
        int16_t car_rc4[4];
        int16_t car_rc5[4];
        stunts_VECTOR car_whlWorldCrds1[4];
        stunts_VECTOR car_whlWorldCrds2[4];
        stunts_VECTOR car_vec_unk3;
        stunts_VECTOR car_vec_unk4;
        stunts_VECTOR car_vec_unk5;
        int16_t field_B6;
        int16_t field_B8;
        int16_t field_BA;
        int8_t car_is_braking;
        int8_t car_is_accelerating;
        int8_t car_current_gear;
        int8_t car_sumSurfFrontWheels;
        int8_t car_sumSurfRearWheels;
        int8_t car_sumSurfAllWheels;
        int8_t car_surfaceWhl[4];
        int8_t car_engineLimiterTimer;
        int8_t car_slidingFlag;
        int8_t field_C8;
        int8_t car_crashBmpFlag;
        int8_t car_changing_gear;
        int8_t car_fpsmul2;
        int8_t car_transmission;
        int8_t field_CD;
        uint8_t field_CE;
        uint8_t field_CF;
};

/* stunts_POINT2D: target size 4; representative src/obj_seg000.c:24; measured packing 2. */
struct stunts_POINT2D {
        int16_t x;
        int16_t y;
};

/* stunts_CRACK_LINE: target size 8; representative src/obj_seg003.c:2885; measured packing 2. */
struct stunts_CRACK_LINE {
        stunts_POINT2D start;
        stunts_POINT2D end;
};

/* stunts_ENGINESOUND: target size 48; representative src/obj_seg005.c:576; measured packing 2. */
struct stunts_ENGINESOUND {
        int16_t es_unk0;
        int16_t es_unk2;
        int16_t es_unk4;
        int16_t es_unk6;
        st_target_far_dataptr_array16_t es_names[10];
};

/* stunts_FARRESOURCE: target size 4; representative src/obj_seg003.c:691; measured packing 2. */
union stunts_FARRESOURCE {
        st_target_far_dataptr16_t pointer;
        struct 
{
  uint16_t offset;
  uint16_t segment;
} word;
};

/* stunts_FONTDEF_PREFIX: target size 16; representative src/obj_seg008.c:46; measured packing 2. */
struct stunts_FONTDEF_PREFIX {
        uint8_t bytes[14];
        uint16_t value;
};

/* stunts_GAMEINFO: target size 26; representative src/obj_seg001_complete.c:66; measured packing 2. */
struct stunts_GAMEINFO {
        int8_t game_playercarid[4];
        int8_t game_playermaterial;
        int8_t game_playertransmission;
        int8_t game_opponenttype;
        int8_t game_opponentcarid[4];
        int8_t game_opponentmaterial;
        int8_t game_opponenttransmission;
        int8_t game_trackname[9];
        union { uint16_t game_framespersec; struct { uint8_t game_framespersec_byte_view; uint8_t game_framespersec_byte_padding; }; };
        uint16_t game_recordedframes;
};

/* stunts_GAMESTATE_1014: target size 1014; representative src/obj_seg004.c:7; measured packing 2. */
struct stunts_GAMESTATE_1014 {
        uint8_t before_game_inputmode[0x3f5];
        int8_t game_inputmode;
};

/* stunts_GAMESTATE_1120: target size 1120; representative src/obj_seg003.c:155; measured packing 2. */
struct stunts_GAMESTATE_1120 {
        int32_t game_longs1[24];
        int32_t game_longs2[24];
        int32_t game_longs3[24];
        stunts_VECTOR game_vec1[2];
        union { struct { stunts_VECTOR game_vec3; stunts_VECTOR game_vec4; }; stunts_VECTORLONG game_vec3_long_view; };
        int16_t game_frame_in_sec;
        int16_t game_frames_per_sec;
        int32_t game_travDist;
        uint16_t game_frame;
        int16_t game_total_finish;
        int16_t field_144;
        int16_t game_pEndFrame;
        int16_t game_oEndFrame;
        int16_t game_penalty;
        uint16_t game_impactSpeed;
        uint16_t game_topSpeed;
        int16_t game_jumpCount;
        stunts_CARSTATE playerstate;
        stunts_CARSTATE opponentstate;
        int16_t field_2F2;
        int16_t field_2F4;
        int16_t game_startcol;
        int16_t game_startcol2;
        int16_t game_startrow;
        int16_t game_startrow2;
        int16_t field_2FE[24];
        int16_t field_32E[24];
        int16_t field_35E[24];
        int16_t field_38E[24];
        int8_t field_3BE[48];
        int8_t kevinseed[6];
        int8_t field_3F4;
        int8_t game_inputmode;
        int8_t game_3F6autoLoadEvalFlag;
        int8_t field_3F7[2];
        int8_t field_3F9;
        int8_t field_3FA[48];
        int8_t field_42A;
        uint8_t field_42B[24];
        uint8_t field_443[24];
        int8_t field_45B;
        int8_t field_45C;
        int8_t field_45D;
        int8_t field_45E;
        int8_t field_45F;
};

/* stunts_GAMESTATE_SNAPSHOT: target size 22; representative src/obj_seg001_complete.c:192; measured packing 2. */
struct stunts_GAMESTATE_SNAPSHOT {
        int32_t game_travDist;
        uint16_t game_frame;
        int16_t game_total_finish;
        int16_t field_144;
        int16_t game_pEndFrame;
        int16_t game_oEndFrame;
        uint16_t game_penalty;
        uint16_t game_impactSpeed;
        uint16_t game_topSpeed;
        int16_t game_jumpCount;
};

/* stunts_HighScoreRecord: target size 52; representative src/obj_seg000.c:21; measured packing 2. */
struct stunts_HighScoreRecord {
        uint8_t bytes[50];
        uint16_t marker;
};

/* stunts_LOOKAHEAD_TILE: target size 3; representative src/obj_seg003.c:2611; measured packing 2. */
struct stunts_LOOKAHEAD_TILE {
        int8_t east_delta;
        int8_t south_delta;
        int8_t detail;
};

/* stunts_MATERIALCLRLIST: target size 202; representative src/obj_seg003.c:690; measured packing 2. */
struct stunts_MATERIALCLRLIST {
        uint8_t pad20[0x20];
        int16_t ground;
        int16_t sky;
        uint8_t pad24[0xa4];
        int16_t water;
};

/* stunts_MATRIX: target size 18; representative src/obj_seg003.c:29; measured packing 2. */
struct stunts_MATRIX {
        int16_t vals[9];
};

/* stunts_MouseRegs: target size 14; representative src/seg017_mouse_whole.c:3; measured packing 2. */
struct stunts_MouseRegs {
        uint16_t ax;
        uint16_t bx;
        uint16_t cx;
        uint16_t dx;
        uint16_t si;
        uint16_t di;
        uint16_t cflag;
};

/* stunts_OPPONENTIMAGE: target size 4; representative src/obj_seg000.c:104; measured packing 2. */
struct stunts_OPPONENTIMAGE {
        uint16_t width;
        uint16_t height;
};

/* stunts_PLANE: target size 34; representative src/obj_seg001_complete.c:25; measured packing 2. */
struct stunts_PLANE {
        int16_t plane_yz;
        int16_t plane_xy;
        stunts_VECTOR plane_origin;
        stunts_VECTOR plane_normal;
        stunts_MATRIX plane_rotation;
};

/* stunts_POLYINFO: target size 46; representative src/obj_seg006.c:30; measured packing 2. */
struct stunts_POLYINFO {
        int16_t depth;
        uint8_t material;
        int8_t numpoints;
        int8_t type;
        int8_t reserved;
        stunts_POINT2D points[10];
};

/* stunts_RECTANGLE: target size 8; representative src/obj_seg001_complete.c:4; measured packing 2. */
struct stunts_RECTANGLE {
        int16_t left;
        int16_t right;
        int16_t top;
        int16_t bottom;
};

/* stunts_SCREEN_RECT: target size 20; representative src/obj_seg032_group.c:3; measured packing 2. */
struct stunts_SCREEN_RECT {
        int16_t width;
        int16_t height;
        int16_t reserved[7];
        int16_t bottom;
};

/* stunts_SHAPE2D_12: target size 12; representative src/obj_seg000.c:318; measured packing 2. */
struct stunts_SHAPE2D_12 {
        int16_t width;
        int16_t height;
        int16_t unk1;
        int16_t unk2;
        int16_t pos_x;
        int16_t pos_y;
};

/* stunts_SHAPE2D_14: target size 14; representative src/obj_seg009.c:2; measured packing 2. */
struct stunts_SHAPE2D_14 {
        int16_t s2d_width;
        int16_t s2d_height;
        uint16_t s2d_unk1;
        uint16_t s2d_unk2;
        uint16_t s2d_pos_x;
        uint16_t s2d_pos_y;
        uint8_t s2d_unk3;
        uint8_t s2d_unk4;
};

/* stunts_SHAPE2D_16: target size 16; representative src/obj_seg005.c:1341; measured packing 2. */
struct stunts_SHAPE2D_16 {
union { struct { int16_t s2d_width; uint16_t s2d_height; uint16_t s2d_unk1; uint16_t s2d_unk2; uint16_t s2d_pos_x; uint16_t s2d_pos_y; }; uint16_t words[6]; };
        union { struct { uint8_t s2d_unk3; uint8_t s2d_unk4; uint8_t s2d_unk5; uint8_t s2d_unk6; }; uint8_t bytes[4]; };
};

/* stunts_SHAPE3D: target size 22; representative src/obj_seg004.c:3; measured packing 2. */
struct stunts_SHAPE3D {
        uint16_t numverts;
        st_target_far_dataptr16_t shape3d_verts;
        uint16_t numprimitives;
        union { uint16_t shape3d_numpaints; struct { uint8_t numpaints; uint8_t reserved; }; };
        st_target_far_dataptr16_t primitives;
        st_target_far_dataptr16_t cull1;
        st_target_far_dataptr16_t cull2;
};

/* stunts_SHAPE3DHEADER: target size 4; representative src/obj_seg003.c:561; measured packing 2. */
struct stunts_SHAPE3DHEADER {
        uint8_t header_numverts;
        uint8_t header_numprimitives;
        uint8_t header_numpaints;
        uint8_t header_reserved;
};

/* stunts_SIMD: target size 776; representative src/obj_seg003.c:204; measured packing 2. */
struct stunts_SIMD {
        int8_t num_gears;
        int8_t simd_unk;
        int16_t car_mass;
        int16_t braking_eff;
        int16_t idle_rpm;
        int16_t downshift_rpm;
        int16_t upshift_rpm;
        int16_t max_rpm;
        uint16_t gear_ratios[7];
        stunts_POINT2D knob_points[7];
        int16_t aero_resistance;
        int8_t idle_torque;
        int8_t torque_curve[104];
        int8_t field_A3;
        int16_t grip;
        int16_t field_A6[7];
        union { struct { int16_t sliding; int16_t surface_grip[4]; }; uint8_t sliding_view[10]; };
        int8_t simd_unk3[10];
        stunts_POINT2D collide_points[2];
        int16_t car_height;
        stunts_VECTOR wheel_coords[4];
        int8_t steeringdots[62];
        stunts_POINT2D spdcenter;
        int16_t spdnumpoints;
        int8_t spdpoints[208];
        stunts_POINT2D revcenter;
        int16_t revnumpoints;
        int8_t revpoints[256];
        st_target_far_dataptr16_t aerorestable;
};

/* stunts_SPRITE_8: target size 8; representative src/obj_seg009.c:12; measured packing 2. */
struct stunts_SPRITE_8 {
        st_target_far_dataptr16_t sprite_bitmapptr;
        uint16_t sprite_unk1;
        uint16_t sprite_unk2;
};

/* stunts_SPRITE_30: target size 30; representative src/obj_seg005.c:1353; measured packing 2. */
struct stunts_SPRITE_30 {
st_target_far_dataptr16_t sprite_bitmapptr;
        union { uint16_t words[13]; struct { uint16_t sprite_unk1; uint16_t sprite_unk2; uint16_t sprite_unk3; st_target_near_dataptr16_t sprite_lineofs; uint16_t sprite_left; uint16_t sprite_right; uint16_t sprite_top; uint16_t sprite_height; uint16_t sprite_pitch; uint16_t sprite_unk4; uint16_t sprite_width2; uint16_t sprite_left2; uint16_t sprite_widthsum; }; };
};

/* stunts_SecurityDialogResult: target size 24; representative src/obj_seg000.c:320; measured packing 2. */
struct stunts_SecurityDialogResult {
        int16_t first_x;
        int16_t first_y;
        int16_t second_x;
        int16_t second_y;
        int16_t third_x;
        int16_t third_y;
        int16_t input_x;
        int16_t input_y;
        int16_t trailing_state[4];
};

/* stunts_TRACKOBJECT: target size 14; representative src/obj_seg003.c:252; measured packing 2. */
struct stunts_TRACKOBJECT {
        union { st_target_near_dataptr16_t info; st_target_near_dataptr16_t ss_trkObjInfoPtr; };
        union { int16_t rotation_y; int16_t ss_rotY; };
        union { st_target_near_dataptr16_t shape; st_target_near_dataptr16_t ss_shapePtr; };
        union { st_target_near_dataptr16_t low_detail_shape; st_target_near_dataptr16_t ss_loShapePtr; };
        union { uint8_t overlay; uint8_t ss_ssOvelay; };
        union { int8_t surface; int8_t ss_surfaceType; };
        union { int8_t ignore_z_bias; int8_t ss_ignoreZBias; };
        union { int8_t multi_tile; int8_t ss_multiTileFlag; };
        union { int8_t physical_model; int8_t ss_physicalModel; };
        union { int8_t unknown; int8_t scene_unk5; };
};

/* stunts_TRACKRESULT: target size 20; representative src/obj_seg001_complete.c:548; measured packing 2. */
struct stunts_TRACKRESULT {
        stunts_VECTOR center;
        stunts_VECTOR edge_a;
        stunts_VECTOR edge_b;
        int16_t has_opponent_link;
};

/* stunts_TRANSFORMEDSHAPE3D: target size 20; representative src/obj_seg003.c:568; measured packing 2. */
struct stunts_TRANSFORMEDSHAPE3D {
        stunts_VECTOR pos;
        st_target_near_dataptr16_t shapeptr;
        st_target_near_dataptr16_t rectptr;
        stunts_VECTOR rotvec;
        uint16_t unk;
        uint8_t ts_flags;
        uint8_t material;
};

/* stunts_TRKOBJINFO: target size 14; representative src/obj_seg004.c:4; measured packing 2. */
struct stunts_TRKOBJINFO {
        uint8_t noOfBlocks;
        uint8_t entry;
        uint8_t exitPoint;
        uint8_t entryType;
        uint8_t exitType;
        uint8_t arrowType;
        int16_t arrowOrient;
        st_target_near_dataptr16_t cameraDataOffset;
        union 
{
  struct 
  {
    uint8_t opponent1;
    uint8_t opponent2;
  } opponent;
  uint16_t cameraOffsetOverride;
} cameraOverlay;
        uint8_t opponent3;
        uint8_t opponentSpeedCode;
};

/* stunts_TRKOBJINFO_LINK_BYTES: target size 2; representative src/obj_seg001_complete.c:237; measured packing 2. */
struct stunts_TRKOBJINFO_LINK_BYTES {
        int8_t first;
        int8_t second;
};

/* stunts_TrackNode: target size 14; representative src/obj_seg004.c:6; measured packing 2. */
struct stunts_TrackNode {
        uint8_t column;
        uint8_t row;
        uint8_t element;
        uint8_t block;
        uint8_t connection;
        uint8_t terrain;
        uint8_t previousColumn;
        uint8_t previousRow;
        uint8_t previousElement;
        uint8_t previousBlock;
        uint8_t previousReversed;
        uint8_t previousExitType;
        int16_t parent;
};

/* stunts_WALLREC: target size 6; representative src/obj_seg004.c:8; measured packing 2. */
struct stunts_WALLREC {
        int16_t orientation;
        int16_t x;
        int16_t z;
};

/* stunts_WheelRect_12: target size 12; representative src/preRender_wheel_helper2.c:3; measured packing 2. */
struct stunts_WheelRect_12 {
        stunts_POINT2D p0;
        stunts_POINT2D p1;
        stunts_POINT2D p2;
};

/* stunts_WheelRect_16: target size 16; representative src/preRender_wheel.c:3; measured packing 2. */
struct stunts_WheelRect_16 {
        int16_t left;
        int16_t top;
        int16_t unused0;
        int16_t unused1;
        int16_t unused2;
        int16_t unused3;
        int16_t x;
        int16_t y;
};

/* stunts_car_exp_name_table: target size 76; representative src/track_constants_module.c:842; measured packing 2. */
struct stunts_car_exp_name_table {
        int8_t entry[15][5];
        int8_t trailing_zero;
};

/* stunts_coord_pair: target size 4; representative src/track_constants_module.c:802; measured packing 2. */
struct stunts_coord_pair {
        int16_t x;
        int16_t z;
};

/* audiochunktable[24], address 0x3396C: both accepted TU tags view the same 76-byte record. */
#pragma pack(push, 2)
union stunts_AUDIOCHUNK {
    struct {
        union { st_target_far_dataptr16_t resource_data; st_target_far_dataptr16_t pos; };
        union { uint8_t depth; uint8_t unk04; };
        union {
            st_target_far_dataptr_array16_t stack[4];
            struct { st_target_far_dataptr_array16_t first_stack_pointer; uint8_t stack_tail[12]; } legacy_stack_view;
        };
        union { uint8_t activeVoices; uint8_t unk15; };
        union { uint8_t maxVoices; uint8_t unk16; };
        union { uint8_t reserved17; uint8_t unk17; };
        union { uint32_t delay; int32_t unk18; };
        union { uint8_t reserved1c[2]; struct { uint8_t unk1C, unk1D; } legacy_delay_view; };
        union { st_target_far_dataptr16_t sample_data; st_target_far_dataptr16_t data; int32_t unk1E; };
        union { uint8_t velocity; uint8_t unk22; };
        union { uint8_t resourceType; uint8_t index; };
        union { uint8_t note; uint8_t priority; };
        union { uint8_t modeValue; uint8_t unk25; };
        union { uint16_t value26; int16_t unk26; };
        union { uint8_t program; uint8_t volume; };
        union { uint8_t reserved29[5]; struct { uint8_t unk29, unk2A, unk2B, unk2C, unk2D; } legacy_volume_view; };
        union { st_target_far_dataptr16_t samples; st_target_far_dataptr16_t unk2E; };
        union { uint8_t loopDepth; uint8_t unk32; };
        union {
            uint8_t unk33[20];
            struct { st_target_far_dataptr_array16_t loopPos[4]; uint8_t loopCount[4]; } voice_loop_view;
        };
        union { uint8_t channelNumber; uint8_t unk47; };
        union { st_target_far_codeptr16_t callback; int32_t unk48; };
    };
};
#pragma pack(pop)

/* snd_voices_tbl[16], address 0x35A26: AUDIOVOICE and AudioVoice are overlays. */
#pragma pack(push, 2)
union stunts_AUDIOVOICE {
    struct {
        union { uint8_t resourceIndex; uint8_t unk00; };
        union { uint8_t active; uint8_t state; };
        union { uint8_t note; uint8_t unk02; };
        uint8_t reserved03[5];
        uint32_t position;
        union { uint32_t remaining; int32_t length; };
        union { st_target_far_dataptr16_t data; int32_t unk10; };
        union {
            uint8_t unk14[22];
            struct {
                int16_t sampleRate; uint8_t state16, reserved17;
                uint16_t value18, value1a; int16_t value1c;
                uint16_t value1e, value20; uint8_t value22, reserved23;
                uint16_t value24; uint8_t value26, value27, value28, value29;
            } runtime_voice_view;
        };
        union { st_target_near_dataptr16_t resource; int16_t unk2A; };
        union { uint8_t channelNumber; uint8_t unk2C; };
        union { uint8_t reserved2d; uint8_t unk2D; };
    };
};
#pragma pack(pop)

/* Every assertion is host-compiled but checks the original 16-bit storage layout. */
#define STUNTS_LAYOUT_ASSERT(name, expr) typedef char name[(expr) ? 1 : -1]
STUNTS_LAYOUT_ASSERT(stunts_far_data_pointer_size, sizeof(st_target_far_dataptr16_t) == 4);
STUNTS_LAYOUT_ASSERT(stunts_near_data_pointer_size, sizeof(st_target_near_dataptr16_t) == 2);
STUNTS_LAYOUT_ASSERT(stunts_far_array_pointer_size, sizeof(st_target_far_dataptr_array16_t) == 4);
STUNTS_LAYOUT_ASSERT(stunts_far_array_pointer_alignment, _Alignof(st_target_far_dataptr_array16_t) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIOCHUNK_size, sizeof(stunts_AUDIOCHUNK) == 76);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIOVOICE_size, sizeof(stunts_AUDIOVOICE) == 46);

STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_size, sizeof(stunts_AUDIO_CAR_FRAME) == 34);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_reserved_0, offsetof(stunts_AUDIO_CAR_FRAME, reserved) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_player_offsets_1, offsetof(stunts_AUDIO_CAR_FRAME, player_offsets) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_opponent_offsets_2, offsetof(stunts_AUDIO_CAR_FRAME, opponent_offsets) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_player_rpm_3, offsetof(stunts_AUDIO_CAR_FRAME, player_rpm) == 30);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AUDIO_CAR_FRAME_opponent_rpm_4, offsetof(stunts_AUDIO_CAR_FRAME, opponent_rpm) == 32);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_size, sizeof(stunts_AudioEvent) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_delta_0, offsetof(stunts_AudioEvent, delta) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_command_1, offsetof(stunts_AudioEvent, command) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_param_2, offsetof(stunts_AudioEvent, param) == 5);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_value_3, offsetof(stunts_AudioEvent, value) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioEvent_length_4, offsetof(stunts_AudioEvent, length) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_size, sizeof(stunts_AudioPayload) == 48);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_sample_word_0, offsetof(stunts_AudioPayload, sample_word) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_unk2_1, offsetof(stunts_AudioPayload, unk2) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_unk4_2, offsetof(stunts_AudioPayload, unk4) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_resource_ready_3, offsetof(stunts_AudioPayload, resource_ready) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_unk7_4, offsetof(stunts_AudioPayload, unk7) == 7);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_shape_5, offsetof(stunts_AudioPayload, shape) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_unkC_6, offsetof(stunts_AudioPayload, unkC) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioPayload_resources_7, offsetof(stunts_AudioPayload, resources) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_size, sizeof(stunts_AudioSample) == 68);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_reserved00_0, offsetof(stunts_AudioSample, reserved00) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_level1e_1, offsetof(stunts_AudioSample, level1e) == 30);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_level20_2, offsetof(stunts_AudioSample, level20) == 32);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_level22_3, offsetof(stunts_AudioSample, level22) == 34);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_level24_4, offsetof(stunts_AudioSample, level24) == 36);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_level26_5, offsetof(stunts_AudioSample, level26) == 38);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_loopMode_6, offsetof(stunts_AudioSample, loopMode) == 40);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_loopCount_7, offsetof(stunts_AudioSample, loopCount) == 41);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_reserved2a_8, offsetof(stunts_AudioSample, reserved2a) == 42);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_limit2e_9, offsetof(stunts_AudioSample, limit2e) == 46);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_reserved30_10, offsetof(stunts_AudioSample, reserved30) == 48);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_loopFlags_11, offsetof(stunts_AudioSample, loopFlags) == 52);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_pulsePresent_12, offsetof(stunts_AudioSample, pulsePresent) == 53);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_reserved36_13, offsetof(stunts_AudioSample, reserved36) == 54);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_pulseCount_14, offsetof(stunts_AudioSample, pulseCount) == 58);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioSample_pulseTable_15, offsetof(stunts_AudioSample, pulseTable) == 59);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_size, sizeof(stunts_AudioTimer) == 76);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_active_0, offsetof(stunts_AudioTimer, active) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_state_1, offsetof(stunts_AudioTimer, state) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_handle_2, offsetof(stunts_AudioTimer, handle) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_pitch_avg_3, offsetof(stunts_AudioTimer, pitch_avg) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_rate_avg_4, offsetof(stunts_AudioTimer, rate_avg) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_pitch_5, offsetof(stunts_AudioTimer, pitch) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_unkB_6, offsetof(stunts_AudioTimer, unkB) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_rate_7, offsetof(stunts_AudioTimer, rate) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_last_pitch_8, offsetof(stunts_AudioTimer, last_pitch) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_unkF_9, offsetof(stunts_AudioTimer, unkF) == 15);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_channels_10, offsetof(stunts_AudioTimer, channels) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_sample_word_11, offsetof(stunts_AudioTimer, sample_word) == 24);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_dirty_12, offsetof(stunts_AudioTimer, dirty) == 26);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_retrigger_13, offsetof(stunts_AudioTimer, retrigger) == 27);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_AudioTimer_payload_14, offsetof(stunts_AudioTimer, payload) == 28);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_size, sizeof(stunts_CARSTATE) == 208);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_posWorld1_0, offsetof(stunts_CARSTATE, car_posWorld1) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_posWorld2_1, offsetof(stunts_CARSTATE, car_posWorld2) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rotate_2, offsetof(stunts_CARSTATE, car_rotate) == 24);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_pseudoGravity_3, offsetof(stunts_CARSTATE, car_pseudoGravity) == 30);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_steeringAngle_4, offsetof(stunts_CARSTATE, car_steeringAngle) == 32);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_currpm_5, offsetof(stunts_CARSTATE, car_currpm) == 34);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_lastrpm_6, offsetof(stunts_CARSTATE, car_lastrpm) == 36);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_idlerpm2_7, offsetof(stunts_CARSTATE, car_idlerpm2) == 38);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_speeddiff_8, offsetof(stunts_CARSTATE, car_speeddiff) == 40);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_speed_9, offsetof(stunts_CARSTATE, car_speed) == 42);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_speed2_10, offsetof(stunts_CARSTATE, car_speed2) == 44);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_lastspeed_11, offsetof(stunts_CARSTATE, car_lastspeed) == 46);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_gearratio_12, offsetof(stunts_CARSTATE, car_gearratio) == 48);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_gearratioshr8_13, offsetof(stunts_CARSTATE, car_gearratioshr8) == 50);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_knob_x_14, offsetof(stunts_CARSTATE, car_knob_x) == 52);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_36MwhlAngle_15, offsetof(stunts_CARSTATE, car_36MwhlAngle) == 54);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_knob_y_16, offsetof(stunts_CARSTATE, car_knob_y) == 56);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_knob_x2_17, offsetof(stunts_CARSTATE, car_knob_x2) == 58);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_knob_y2_18, offsetof(stunts_CARSTATE, car_knob_y2) == 60);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_angle_z_19, offsetof(stunts_CARSTATE, car_angle_z) == 62);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_40MfrontWhlAngle_20, offsetof(stunts_CARSTATE, car_40MfrontWhlAngle) == 64);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_42_21, offsetof(stunts_CARSTATE, field_42) == 66);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_demandedGrip_22, offsetof(stunts_CARSTATE, car_demandedGrip) == 68);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_surfacegrip_sum_23, offsetof(stunts_CARSTATE, car_surfacegrip_sum) == 70);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_48_24, offsetof(stunts_CARSTATE, field_48) == 72);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_trackdata3_index_25, offsetof(stunts_CARSTATE, car_trackdata3_index) == 74);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rc1_26, offsetof(stunts_CARSTATE, car_rc1) == 76);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rc2_27, offsetof(stunts_CARSTATE, car_rc2) == 84);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rc3_28, offsetof(stunts_CARSTATE, car_rc3) == 92);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rc4_29, offsetof(stunts_CARSTATE, car_rc4) == 100);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_rc5_30, offsetof(stunts_CARSTATE, car_rc5) == 108);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_whlWorldCrds1_31, offsetof(stunts_CARSTATE, car_whlWorldCrds1) == 116);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_whlWorldCrds2_32, offsetof(stunts_CARSTATE, car_whlWorldCrds2) == 140);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_vec_unk3_33, offsetof(stunts_CARSTATE, car_vec_unk3) == 164);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_vec_unk4_34, offsetof(stunts_CARSTATE, car_vec_unk4) == 170);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_vec_unk5_35, offsetof(stunts_CARSTATE, car_vec_unk5) == 176);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_B6_36, offsetof(stunts_CARSTATE, field_B6) == 182);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_B8_37, offsetof(stunts_CARSTATE, field_B8) == 184);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_BA_38, offsetof(stunts_CARSTATE, field_BA) == 186);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_is_braking_39, offsetof(stunts_CARSTATE, car_is_braking) == 188);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_is_accelerating_40, offsetof(stunts_CARSTATE, car_is_accelerating) == 189);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_current_gear_41, offsetof(stunts_CARSTATE, car_current_gear) == 190);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_sumSurfFrontWheels_42, offsetof(stunts_CARSTATE, car_sumSurfFrontWheels) == 191);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_sumSurfRearWheels_43, offsetof(stunts_CARSTATE, car_sumSurfRearWheels) == 192);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_sumSurfAllWheels_44, offsetof(stunts_CARSTATE, car_sumSurfAllWheels) == 193);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_surfaceWhl_45, offsetof(stunts_CARSTATE, car_surfaceWhl) == 194);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_engineLimiterTimer_46, offsetof(stunts_CARSTATE, car_engineLimiterTimer) == 198);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_slidingFlag_47, offsetof(stunts_CARSTATE, car_slidingFlag) == 199);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_C8_48, offsetof(stunts_CARSTATE, field_C8) == 200);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_crashBmpFlag_49, offsetof(stunts_CARSTATE, car_crashBmpFlag) == 201);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_changing_gear_50, offsetof(stunts_CARSTATE, car_changing_gear) == 202);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_fpsmul2_51, offsetof(stunts_CARSTATE, car_fpsmul2) == 203);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_car_transmission_52, offsetof(stunts_CARSTATE, car_transmission) == 204);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_CD_53, offsetof(stunts_CARSTATE, field_CD) == 205);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_CE_54, offsetof(stunts_CARSTATE, field_CE) == 206);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CARSTATE_field_CF_55, offsetof(stunts_CARSTATE, field_CF) == 207);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CRACK_LINE_size, sizeof(stunts_CRACK_LINE) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CRACK_LINE_start_0, offsetof(stunts_CRACK_LINE, start) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_CRACK_LINE_end_1, offsetof(stunts_CRACK_LINE, end) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_size, sizeof(stunts_ENGINESOUND) == 48);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_es_unk0_0, offsetof(stunts_ENGINESOUND, es_unk0) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_es_unk2_1, offsetof(stunts_ENGINESOUND, es_unk2) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_es_unk4_2, offsetof(stunts_ENGINESOUND, es_unk4) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_es_unk6_3, offsetof(stunts_ENGINESOUND, es_unk6) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_ENGINESOUND_es_names_4, offsetof(stunts_ENGINESOUND, es_names) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FARRESOURCE_size, sizeof(stunts_FARRESOURCE) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FARRESOURCE_pointer_0, offsetof(stunts_FARRESOURCE, pointer) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FARRESOURCE_word_1, offsetof(stunts_FARRESOURCE, word) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FARRESOURCE_word_offset_2, offsetof(stunts_FARRESOURCE, word.offset) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FARRESOURCE_word_segment_3, offsetof(stunts_FARRESOURCE, word.segment) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FONTDEF_PREFIX_size, sizeof(stunts_FONTDEF_PREFIX) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FONTDEF_PREFIX_bytes_0, offsetof(stunts_FONTDEF_PREFIX, bytes) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_FONTDEF_PREFIX_value_1, offsetof(stunts_FONTDEF_PREFIX, value) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_size, sizeof(stunts_GAMEINFO) == 26);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_playercarid_0, offsetof(stunts_GAMEINFO, game_playercarid) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_playermaterial_1, offsetof(stunts_GAMEINFO, game_playermaterial) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_playertransmission_2, offsetof(stunts_GAMEINFO, game_playertransmission) == 5);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_opponenttype_3, offsetof(stunts_GAMEINFO, game_opponenttype) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_opponentcarid_4, offsetof(stunts_GAMEINFO, game_opponentcarid) == 7);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_opponentmaterial_5, offsetof(stunts_GAMEINFO, game_opponentmaterial) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_opponenttransmission_6, offsetof(stunts_GAMEINFO, game_opponenttransmission) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_trackname_7, offsetof(stunts_GAMEINFO, game_trackname) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_framespersec_8, offsetof(stunts_GAMEINFO, game_framespersec) == 22);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_framespersec_byte_view_8, offsetof(stunts_GAMEINFO, game_framespersec_byte_view) == 22);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMEINFO_game_recordedframes_9, offsetof(stunts_GAMEINFO, game_recordedframes) == 24);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1014_size, sizeof(stunts_GAMESTATE_1014) == 1014);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1014_before_game_inputmode_0, offsetof(stunts_GAMESTATE_1014, before_game_inputmode) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1014_game_inputmode_1, offsetof(stunts_GAMESTATE_1014, game_inputmode) == 1013);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_size, sizeof(stunts_GAMESTATE_1120) == 1120);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_longs1_0, offsetof(stunts_GAMESTATE_1120, game_longs1) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_longs2_1, offsetof(stunts_GAMESTATE_1120, game_longs2) == 96);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_longs3_2, offsetof(stunts_GAMESTATE_1120, game_longs3) == 192);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_vec1_3, offsetof(stunts_GAMESTATE_1120, game_vec1) == 288);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_vec3_4, offsetof(stunts_GAMESTATE_1120, game_vec3) == 300);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_vec3_long_view_4, offsetof(stunts_GAMESTATE_1120, game_vec3_long_view) == 300);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_vec4_5, offsetof(stunts_GAMESTATE_1120, game_vec4) == 306);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_frame_in_sec_6, offsetof(stunts_GAMESTATE_1120, game_frame_in_sec) == 312);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_frames_per_sec_7, offsetof(stunts_GAMESTATE_1120, game_frames_per_sec) == 314);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_travDist_8, offsetof(stunts_GAMESTATE_1120, game_travDist) == 316);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_frame_9, offsetof(stunts_GAMESTATE_1120, game_frame) == 320);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_total_finish_10, offsetof(stunts_GAMESTATE_1120, game_total_finish) == 322);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_144_11, offsetof(stunts_GAMESTATE_1120, field_144) == 324);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_pEndFrame_12, offsetof(stunts_GAMESTATE_1120, game_pEndFrame) == 326);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_oEndFrame_13, offsetof(stunts_GAMESTATE_1120, game_oEndFrame) == 328);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_penalty_14, offsetof(stunts_GAMESTATE_1120, game_penalty) == 330);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_impactSpeed_15, offsetof(stunts_GAMESTATE_1120, game_impactSpeed) == 332);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_topSpeed_16, offsetof(stunts_GAMESTATE_1120, game_topSpeed) == 334);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_jumpCount_17, offsetof(stunts_GAMESTATE_1120, game_jumpCount) == 336);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_playerstate_18, offsetof(stunts_GAMESTATE_1120, playerstate) == 338);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_opponentstate_19, offsetof(stunts_GAMESTATE_1120, opponentstate) == 546);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_2F2_20, offsetof(stunts_GAMESTATE_1120, field_2F2) == 754);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_2F4_21, offsetof(stunts_GAMESTATE_1120, field_2F4) == 756);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_startcol_22, offsetof(stunts_GAMESTATE_1120, game_startcol) == 758);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_startcol2_23, offsetof(stunts_GAMESTATE_1120, game_startcol2) == 760);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_startrow_24, offsetof(stunts_GAMESTATE_1120, game_startrow) == 762);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_startrow2_25, offsetof(stunts_GAMESTATE_1120, game_startrow2) == 764);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_2FE_26, offsetof(stunts_GAMESTATE_1120, field_2FE) == 766);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_32E_27, offsetof(stunts_GAMESTATE_1120, field_32E) == 814);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_35E_28, offsetof(stunts_GAMESTATE_1120, field_35E) == 862);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_38E_29, offsetof(stunts_GAMESTATE_1120, field_38E) == 910);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_3BE_30, offsetof(stunts_GAMESTATE_1120, field_3BE) == 958);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_kevinseed_31, offsetof(stunts_GAMESTATE_1120, kevinseed) == 1006);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_3F4_32, offsetof(stunts_GAMESTATE_1120, field_3F4) == 1012);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_inputmode_33, offsetof(stunts_GAMESTATE_1120, game_inputmode) == 1013);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_game_3F6autoLoadEvalFlag_34, offsetof(stunts_GAMESTATE_1120, game_3F6autoLoadEvalFlag) == 1014);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_3F7_35, offsetof(stunts_GAMESTATE_1120, field_3F7) == 1015);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_3F9_36, offsetof(stunts_GAMESTATE_1120, field_3F9) == 1017);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_3FA_37, offsetof(stunts_GAMESTATE_1120, field_3FA) == 1018);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_42A_38, offsetof(stunts_GAMESTATE_1120, field_42A) == 1066);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_42B_39, offsetof(stunts_GAMESTATE_1120, field_42B) == 1067);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_443_40, offsetof(stunts_GAMESTATE_1120, field_443) == 1091);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_45B_41, offsetof(stunts_GAMESTATE_1120, field_45B) == 1115);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_45C_42, offsetof(stunts_GAMESTATE_1120, field_45C) == 1116);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_45D_43, offsetof(stunts_GAMESTATE_1120, field_45D) == 1117);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_45E_44, offsetof(stunts_GAMESTATE_1120, field_45E) == 1118);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_1120_field_45F_45, offsetof(stunts_GAMESTATE_1120, field_45F) == 1119);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_size, sizeof(stunts_GAMESTATE_SNAPSHOT) == 22);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_travDist_0, offsetof(stunts_GAMESTATE_SNAPSHOT, game_travDist) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_frame_1, offsetof(stunts_GAMESTATE_SNAPSHOT, game_frame) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_total_finish_2, offsetof(stunts_GAMESTATE_SNAPSHOT, game_total_finish) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_field_144_3, offsetof(stunts_GAMESTATE_SNAPSHOT, field_144) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_pEndFrame_4, offsetof(stunts_GAMESTATE_SNAPSHOT, game_pEndFrame) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_oEndFrame_5, offsetof(stunts_GAMESTATE_SNAPSHOT, game_oEndFrame) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_penalty_6, offsetof(stunts_GAMESTATE_SNAPSHOT, game_penalty) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_impactSpeed_7, offsetof(stunts_GAMESTATE_SNAPSHOT, game_impactSpeed) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_topSpeed_8, offsetof(stunts_GAMESTATE_SNAPSHOT, game_topSpeed) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_GAMESTATE_SNAPSHOT_game_jumpCount_9, offsetof(stunts_GAMESTATE_SNAPSHOT, game_jumpCount) == 20);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_HighScoreRecord_size, sizeof(stunts_HighScoreRecord) == 52);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_HighScoreRecord_bytes_0, offsetof(stunts_HighScoreRecord, bytes) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_HighScoreRecord_marker_1, offsetof(stunts_HighScoreRecord, marker) == 50);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_LOOKAHEAD_TILE_size, sizeof(stunts_LOOKAHEAD_TILE) == 3);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_LOOKAHEAD_TILE_east_delta_0, offsetof(stunts_LOOKAHEAD_TILE, east_delta) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_LOOKAHEAD_TILE_south_delta_1, offsetof(stunts_LOOKAHEAD_TILE, south_delta) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_LOOKAHEAD_TILE_detail_2, offsetof(stunts_LOOKAHEAD_TILE, detail) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_size, sizeof(stunts_MATERIALCLRLIST) == 202);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_pad20_0, offsetof(stunts_MATERIALCLRLIST, pad20) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_ground_1, offsetof(stunts_MATERIALCLRLIST, ground) == 32);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_sky_2, offsetof(stunts_MATERIALCLRLIST, sky) == 34);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_pad24_3, offsetof(stunts_MATERIALCLRLIST, pad24) == 36);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATERIALCLRLIST_water_4, offsetof(stunts_MATERIALCLRLIST, water) == 200);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATRIX_size, sizeof(stunts_MATRIX) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MATRIX_vals_0, offsetof(stunts_MATRIX, vals) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_size, sizeof(stunts_MouseRegs) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_ax_0, offsetof(stunts_MouseRegs, ax) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_bx_1, offsetof(stunts_MouseRegs, bx) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_cx_2, offsetof(stunts_MouseRegs, cx) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_dx_3, offsetof(stunts_MouseRegs, dx) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_si_4, offsetof(stunts_MouseRegs, si) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_di_5, offsetof(stunts_MouseRegs, di) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_MouseRegs_cflag_6, offsetof(stunts_MouseRegs, cflag) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_OPPONENTIMAGE_size, sizeof(stunts_OPPONENTIMAGE) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_OPPONENTIMAGE_width_0, offsetof(stunts_OPPONENTIMAGE, width) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_OPPONENTIMAGE_height_1, offsetof(stunts_OPPONENTIMAGE, height) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_size, sizeof(stunts_PLANE) == 34);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_plane_yz_0, offsetof(stunts_PLANE, plane_yz) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_plane_xy_1, offsetof(stunts_PLANE, plane_xy) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_plane_origin_2, offsetof(stunts_PLANE, plane_origin) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_plane_normal_3, offsetof(stunts_PLANE, plane_normal) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_PLANE_plane_rotation_4, offsetof(stunts_PLANE, plane_rotation) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POINT2D_size, sizeof(stunts_POINT2D) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POINT2D_x_0, offsetof(stunts_POINT2D, x) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POINT2D_y_1, offsetof(stunts_POINT2D, y) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_size, sizeof(stunts_POLYINFO) == 46);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_depth_0, offsetof(stunts_POLYINFO, depth) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_material_1, offsetof(stunts_POLYINFO, material) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_numpoints_2, offsetof(stunts_POLYINFO, numpoints) == 3);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_type_3, offsetof(stunts_POLYINFO, type) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_reserved_4, offsetof(stunts_POLYINFO, reserved) == 5);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_POLYINFO_points_5, offsetof(stunts_POLYINFO, points) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_RECTANGLE_size, sizeof(stunts_RECTANGLE) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_RECTANGLE_left_0, offsetof(stunts_RECTANGLE, left) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_RECTANGLE_right_1, offsetof(stunts_RECTANGLE, right) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_RECTANGLE_top_2, offsetof(stunts_RECTANGLE, top) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_RECTANGLE_bottom_3, offsetof(stunts_RECTANGLE, bottom) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SCREEN_RECT_size, sizeof(stunts_SCREEN_RECT) == 20);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SCREEN_RECT_width_0, offsetof(stunts_SCREEN_RECT, width) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SCREEN_RECT_height_1, offsetof(stunts_SCREEN_RECT, height) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SCREEN_RECT_reserved_2, offsetof(stunts_SCREEN_RECT, reserved) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SCREEN_RECT_bottom_3, offsetof(stunts_SCREEN_RECT, bottom) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_size, sizeof(stunts_SHAPE2D_12) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_width_0, offsetof(stunts_SHAPE2D_12, width) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_height_1, offsetof(stunts_SHAPE2D_12, height) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_unk1_2, offsetof(stunts_SHAPE2D_12, unk1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_unk2_3, offsetof(stunts_SHAPE2D_12, unk2) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_pos_x_4, offsetof(stunts_SHAPE2D_12, pos_x) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_12_pos_y_5, offsetof(stunts_SHAPE2D_12, pos_y) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_size, sizeof(stunts_SHAPE2D_14) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_width_0, offsetof(stunts_SHAPE2D_14, s2d_width) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_height_1, offsetof(stunts_SHAPE2D_14, s2d_height) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_unk1_2, offsetof(stunts_SHAPE2D_14, s2d_unk1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_unk2_3, offsetof(stunts_SHAPE2D_14, s2d_unk2) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_pos_x_4, offsetof(stunts_SHAPE2D_14, s2d_pos_x) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_pos_y_5, offsetof(stunts_SHAPE2D_14, s2d_pos_y) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_unk3_6, offsetof(stunts_SHAPE2D_14, s2d_unk3) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_14_s2d_unk4_7, offsetof(stunts_SHAPE2D_14, s2d_unk4) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_size, sizeof(stunts_SHAPE2D_16) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_width_0, offsetof(stunts_SHAPE2D_16, s2d_width) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_0, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_height_1, offsetof(stunts_SHAPE2D_16, s2d_height) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_1, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk1_2, offsetof(stunts_SHAPE2D_16, s2d_unk1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_2, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk2_3, offsetof(stunts_SHAPE2D_16, s2d_unk2) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_3, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_pos_x_4, offsetof(stunts_SHAPE2D_16, s2d_pos_x) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_4, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_pos_y_5, offsetof(stunts_SHAPE2D_16, s2d_pos_y) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_words_5, offsetof(stunts_SHAPE2D_16, words) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk3_6, offsetof(stunts_SHAPE2D_16, s2d_unk3) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_bytes_6, offsetof(stunts_SHAPE2D_16, bytes) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk4_7, offsetof(stunts_SHAPE2D_16, s2d_unk4) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_bytes_7, offsetof(stunts_SHAPE2D_16, bytes) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk5_8, offsetof(stunts_SHAPE2D_16, s2d_unk5) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_bytes_8, offsetof(stunts_SHAPE2D_16, bytes) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_s2d_unk6_9, offsetof(stunts_SHAPE2D_16, s2d_unk6) == 15);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE2D_16_bytes_9, offsetof(stunts_SHAPE2D_16, bytes) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_size, sizeof(stunts_SHAPE3D) == 22);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_numverts_0, offsetof(stunts_SHAPE3D, numverts) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_shape3d_verts_1, offsetof(stunts_SHAPE3D, shape3d_verts) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_numprimitives_2, offsetof(stunts_SHAPE3D, numprimitives) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_numpaints_3, offsetof(stunts_SHAPE3D, numpaints) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_shape3d_numpaints_3, offsetof(stunts_SHAPE3D, shape3d_numpaints) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_reserved_4, offsetof(stunts_SHAPE3D, reserved) == 9);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_primitives_5, offsetof(stunts_SHAPE3D, primitives) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_cull1_6, offsetof(stunts_SHAPE3D, cull1) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3D_cull2_7, offsetof(stunts_SHAPE3D, cull2) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3DHEADER_size, sizeof(stunts_SHAPE3DHEADER) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3DHEADER_header_numverts_0, offsetof(stunts_SHAPE3DHEADER, header_numverts) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3DHEADER_header_numprimitives_1, offsetof(stunts_SHAPE3DHEADER, header_numprimitives) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3DHEADER_header_numpaints_2, offsetof(stunts_SHAPE3DHEADER, header_numpaints) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SHAPE3DHEADER_header_reserved_3, offsetof(stunts_SHAPE3DHEADER, header_reserved) == 3);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_size, sizeof(stunts_SIMD) == 776);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_num_gears_0, offsetof(stunts_SIMD, num_gears) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_simd_unk_1, offsetof(stunts_SIMD, simd_unk) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_car_mass_2, offsetof(stunts_SIMD, car_mass) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_braking_eff_3, offsetof(stunts_SIMD, braking_eff) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_idle_rpm_4, offsetof(stunts_SIMD, idle_rpm) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_downshift_rpm_5, offsetof(stunts_SIMD, downshift_rpm) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_upshift_rpm_6, offsetof(stunts_SIMD, upshift_rpm) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_max_rpm_7, offsetof(stunts_SIMD, max_rpm) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_gear_ratios_8, offsetof(stunts_SIMD, gear_ratios) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_knob_points_9, offsetof(stunts_SIMD, knob_points) == 28);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_aero_resistance_10, offsetof(stunts_SIMD, aero_resistance) == 56);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_idle_torque_11, offsetof(stunts_SIMD, idle_torque) == 58);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_torque_curve_12, offsetof(stunts_SIMD, torque_curve) == 59);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_field_A3_13, offsetof(stunts_SIMD, field_A3) == 163);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_grip_14, offsetof(stunts_SIMD, grip) == 164);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_field_A6_15, offsetof(stunts_SIMD, field_A6) == 166);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_sliding_16, offsetof(stunts_SIMD, sliding) == 180);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_sliding_view_16, offsetof(stunts_SIMD, sliding_view) == 180);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_surface_grip_17, offsetof(stunts_SIMD, surface_grip) == 182);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_simd_unk3_18, offsetof(stunts_SIMD, simd_unk3) == 190);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_collide_points_19, offsetof(stunts_SIMD, collide_points) == 200);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_car_height_20, offsetof(stunts_SIMD, car_height) == 208);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_wheel_coords_21, offsetof(stunts_SIMD, wheel_coords) == 210);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_steeringdots_22, offsetof(stunts_SIMD, steeringdots) == 234);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_spdcenter_23, offsetof(stunts_SIMD, spdcenter) == 296);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_spdnumpoints_24, offsetof(stunts_SIMD, spdnumpoints) == 300);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_spdpoints_25, offsetof(stunts_SIMD, spdpoints) == 302);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_revcenter_26, offsetof(stunts_SIMD, revcenter) == 510);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_revnumpoints_27, offsetof(stunts_SIMD, revnumpoints) == 514);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_revpoints_28, offsetof(stunts_SIMD, revpoints) == 516);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SIMD_aerorestable_29, offsetof(stunts_SIMD, aerorestable) == 772);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_8_size, sizeof(stunts_SPRITE_8) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_8_sprite_bitmapptr_0, offsetof(stunts_SPRITE_8, sprite_bitmapptr) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_8_sprite_unk1_1, offsetof(stunts_SPRITE_8, sprite_unk1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_8_sprite_unk2_2, offsetof(stunts_SPRITE_8, sprite_unk2) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_size, sizeof(stunts_SPRITE_30) == 30);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_bitmapptr_0, offsetof(stunts_SPRITE_30, sprite_bitmapptr) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_unk1_1, offsetof(stunts_SPRITE_30, sprite_unk1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_words_1, offsetof(stunts_SPRITE_30, words) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_unk2_2, offsetof(stunts_SPRITE_30, sprite_unk2) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_unk3_3, offsetof(stunts_SPRITE_30, sprite_unk3) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_lineofs_4, offsetof(stunts_SPRITE_30, sprite_lineofs) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_left_5, offsetof(stunts_SPRITE_30, sprite_left) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_right_6, offsetof(stunts_SPRITE_30, sprite_right) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_top_7, offsetof(stunts_SPRITE_30, sprite_top) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_height_8, offsetof(stunts_SPRITE_30, sprite_height) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_pitch_9, offsetof(stunts_SPRITE_30, sprite_pitch) == 20);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_unk4_10, offsetof(stunts_SPRITE_30, sprite_unk4) == 22);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_width2_11, offsetof(stunts_SPRITE_30, sprite_width2) == 24);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_left2_12, offsetof(stunts_SPRITE_30, sprite_left2) == 26);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SPRITE_30_sprite_widthsum_13, offsetof(stunts_SPRITE_30, sprite_widthsum) == 28);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_size, sizeof(stunts_SecurityDialogResult) == 24);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_first_x_0, offsetof(stunts_SecurityDialogResult, first_x) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_first_y_1, offsetof(stunts_SecurityDialogResult, first_y) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_second_x_2, offsetof(stunts_SecurityDialogResult, second_x) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_second_y_3, offsetof(stunts_SecurityDialogResult, second_y) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_third_x_4, offsetof(stunts_SecurityDialogResult, third_x) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_third_y_5, offsetof(stunts_SecurityDialogResult, third_y) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_input_x_6, offsetof(stunts_SecurityDialogResult, input_x) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_input_y_7, offsetof(stunts_SecurityDialogResult, input_y) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_SecurityDialogResult_trailing_state_8, offsetof(stunts_SecurityDialogResult, trailing_state) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_size, sizeof(stunts_TRACKOBJECT) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_trkObjInfoPtr_0, offsetof(stunts_TRACKOBJECT, ss_trkObjInfoPtr) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_info_0, offsetof(stunts_TRACKOBJECT, info) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_rotY_1, offsetof(stunts_TRACKOBJECT, ss_rotY) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_rotation_y_1, offsetof(stunts_TRACKOBJECT, rotation_y) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_shapePtr_2, offsetof(stunts_TRACKOBJECT, ss_shapePtr) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_shape_2, offsetof(stunts_TRACKOBJECT, shape) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_loShapePtr_3, offsetof(stunts_TRACKOBJECT, ss_loShapePtr) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_low_detail_shape_3, offsetof(stunts_TRACKOBJECT, low_detail_shape) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_ssOvelay_4, offsetof(stunts_TRACKOBJECT, ss_ssOvelay) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_overlay_4, offsetof(stunts_TRACKOBJECT, overlay) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_surfaceType_5, offsetof(stunts_TRACKOBJECT, ss_surfaceType) == 9);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_surface_5, offsetof(stunts_TRACKOBJECT, surface) == 9);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_ignoreZBias_6, offsetof(stunts_TRACKOBJECT, ss_ignoreZBias) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ignore_z_bias_6, offsetof(stunts_TRACKOBJECT, ignore_z_bias) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_multiTileFlag_7, offsetof(stunts_TRACKOBJECT, ss_multiTileFlag) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_multi_tile_7, offsetof(stunts_TRACKOBJECT, multi_tile) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_ss_physicalModel_8, offsetof(stunts_TRACKOBJECT, ss_physicalModel) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_physical_model_8, offsetof(stunts_TRACKOBJECT, physical_model) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_scene_unk5_9, offsetof(stunts_TRACKOBJECT, scene_unk5) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKOBJECT_unknown_9, offsetof(stunts_TRACKOBJECT, unknown) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKRESULT_size, sizeof(stunts_TRACKRESULT) == 20);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKRESULT_center_0, offsetof(stunts_TRACKRESULT, center) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKRESULT_edge_a_1, offsetof(stunts_TRACKRESULT, edge_a) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKRESULT_edge_b_2, offsetof(stunts_TRACKRESULT, edge_b) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRACKRESULT_has_opponent_link_3, offsetof(stunts_TRACKRESULT, has_opponent_link) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_size, sizeof(stunts_TRANSFORMEDSHAPE3D) == 20);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_pos_0, offsetof(stunts_TRANSFORMEDSHAPE3D, pos) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_shapeptr_1, offsetof(stunts_TRANSFORMEDSHAPE3D, shapeptr) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_rectptr_2, offsetof(stunts_TRANSFORMEDSHAPE3D, rectptr) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_rotvec_3, offsetof(stunts_TRANSFORMEDSHAPE3D, rotvec) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_unk_4, offsetof(stunts_TRANSFORMEDSHAPE3D, unk) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_ts_flags_5, offsetof(stunts_TRANSFORMEDSHAPE3D, ts_flags) == 18);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRANSFORMEDSHAPE3D_material_6, offsetof(stunts_TRANSFORMEDSHAPE3D, material) == 19);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_size, sizeof(stunts_TRKOBJINFO) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_noOfBlocks_0, offsetof(stunts_TRKOBJINFO, noOfBlocks) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_entry_1, offsetof(stunts_TRKOBJINFO, entry) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_exitPoint_2, offsetof(stunts_TRKOBJINFO, exitPoint) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_entryType_3, offsetof(stunts_TRKOBJINFO, entryType) == 3);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_exitType_4, offsetof(stunts_TRKOBJINFO, exitType) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_arrowType_5, offsetof(stunts_TRKOBJINFO, arrowType) == 5);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_arrowOrient_6, offsetof(stunts_TRKOBJINFO, arrowOrient) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraDataOffset_7, offsetof(stunts_TRKOBJINFO, cameraDataOffset) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraOverlay_8, offsetof(stunts_TRKOBJINFO, cameraOverlay) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraOverlay_opponent_9, offsetof(stunts_TRKOBJINFO, cameraOverlay.opponent) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraOverlay_opponent_opponent1_10, offsetof(stunts_TRKOBJINFO, cameraOverlay.opponent.opponent1) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraOverlay_opponent_opponent2_11, offsetof(stunts_TRKOBJINFO, cameraOverlay.opponent.opponent2) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_cameraOverlay_cameraOffsetOverride_12, offsetof(stunts_TRKOBJINFO, cameraOverlay.cameraOffsetOverride) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_opponent3_13, offsetof(stunts_TRKOBJINFO, opponent3) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_opponentSpeedCode_14, offsetof(stunts_TRKOBJINFO, opponentSpeedCode) == 13);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_LINK_BYTES_size, sizeof(stunts_TRKOBJINFO_LINK_BYTES) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_LINK_BYTES_first_0, offsetof(stunts_TRKOBJINFO_LINK_BYTES, first) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TRKOBJINFO_LINK_BYTES_second_1, offsetof(stunts_TRKOBJINFO_LINK_BYTES, second) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_size, sizeof(stunts_TrackNode) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_column_0, offsetof(stunts_TrackNode, column) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_row_1, offsetof(stunts_TrackNode, row) == 1);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_element_2, offsetof(stunts_TrackNode, element) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_block_3, offsetof(stunts_TrackNode, block) == 3);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_connection_4, offsetof(stunts_TrackNode, connection) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_terrain_5, offsetof(stunts_TrackNode, terrain) == 5);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousColumn_6, offsetof(stunts_TrackNode, previousColumn) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousRow_7, offsetof(stunts_TrackNode, previousRow) == 7);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousElement_8, offsetof(stunts_TrackNode, previousElement) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousBlock_9, offsetof(stunts_TrackNode, previousBlock) == 9);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousReversed_10, offsetof(stunts_TrackNode, previousReversed) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_previousExitType_11, offsetof(stunts_TrackNode, previousExitType) == 11);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_TrackNode_parent_12, offsetof(stunts_TrackNode, parent) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTOR_size, sizeof(stunts_VECTOR) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTOR_x_0, offsetof(stunts_VECTOR, x) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTOR_y_1, offsetof(stunts_VECTOR, y) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTOR_z_2, offsetof(stunts_VECTOR, z) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTORLONG_size, sizeof(stunts_VECTORLONG) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTORLONG_lx_0, offsetof(stunts_VECTORLONG, lx) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTORLONG_ly_1, offsetof(stunts_VECTORLONG, ly) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_VECTORLONG_lz_2, offsetof(stunts_VECTORLONG, lz) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WALLREC_size, sizeof(stunts_WALLREC) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WALLREC_orientation_0, offsetof(stunts_WALLREC, orientation) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WALLREC_x_1, offsetof(stunts_WALLREC, x) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WALLREC_z_2, offsetof(stunts_WALLREC, z) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_12_size, sizeof(stunts_WheelRect_12) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_12_p0_0, offsetof(stunts_WheelRect_12, p0) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_12_p1_1, offsetof(stunts_WheelRect_12, p1) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_12_p2_2, offsetof(stunts_WheelRect_12, p2) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_size, sizeof(stunts_WheelRect_16) == 16);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_left_0, offsetof(stunts_WheelRect_16, left) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_top_1, offsetof(stunts_WheelRect_16, top) == 2);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_unused0_2, offsetof(stunts_WheelRect_16, unused0) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_unused1_3, offsetof(stunts_WheelRect_16, unused1) == 6);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_unused2_4, offsetof(stunts_WheelRect_16, unused2) == 8);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_unused3_5, offsetof(stunts_WheelRect_16, unused3) == 10);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_x_6, offsetof(stunts_WheelRect_16, x) == 12);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_WheelRect_16_y_7, offsetof(stunts_WheelRect_16, y) == 14);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_car_exp_name_table_size, sizeof(stunts_car_exp_name_table) == 76);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_car_exp_name_table_entry_0, offsetof(stunts_car_exp_name_table, entry) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_car_exp_name_table_trailing_zero_1, offsetof(stunts_car_exp_name_table, trailing_zero) == 75);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_coord_pair_size, sizeof(stunts_coord_pair) == 4);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_coord_pair_x_0, offsetof(stunts_coord_pair, x) == 0);
STUNTS_LAYOUT_ASSERT(stunts_assert_stunts_coord_pair_z_1, offsetof(stunts_coord_pair, z) == 2);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_data_0, offsetof(stunts_AUDIOCHUNK, resource_data) == 0);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk04_1, offsetof(stunts_AUDIOCHUNK, unk04) == 4);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk05_2, offsetof(stunts_AUDIOCHUNK, legacy_stack_view.first_stack_pointer) == 5);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk09_3, offsetof(stunts_AUDIOCHUNK, legacy_stack_view.stack_tail) == 9);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk15_4, offsetof(stunts_AUDIOCHUNK, unk15) == 21);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk16_5, offsetof(stunts_AUDIOCHUNK, unk16) == 22);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk17_6, offsetof(stunts_AUDIOCHUNK, unk17) == 23);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk18_7, offsetof(stunts_AUDIOCHUNK, unk18) == 24);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk1C_8, offsetof(stunts_AUDIOCHUNK, legacy_delay_view.unk1C) == 28);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk1D_9, offsetof(stunts_AUDIOCHUNK, legacy_delay_view.unk1D) == 29);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk1E_10, offsetof(stunts_AUDIOCHUNK, unk1E) == 30);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk22_11, offsetof(stunts_AUDIOCHUNK, unk22) == 34);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_index_12, offsetof(stunts_AUDIOCHUNK, index) == 35);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_priority_13, offsetof(stunts_AUDIOCHUNK, priority) == 36);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk25_14, offsetof(stunts_AUDIOCHUNK, unk25) == 37);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk26_15, offsetof(stunts_AUDIOCHUNK, unk26) == 38);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_volume_16, offsetof(stunts_AUDIOCHUNK, volume) == 40);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk29_17, offsetof(stunts_AUDIOCHUNK, legacy_volume_view.unk29) == 41);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk2A_18, offsetof(stunts_AUDIOCHUNK, legacy_volume_view.unk2A) == 42);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk2B_19, offsetof(stunts_AUDIOCHUNK, legacy_volume_view.unk2B) == 43);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk2C_20, offsetof(stunts_AUDIOCHUNK, legacy_volume_view.unk2C) == 44);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk2D_21, offsetof(stunts_AUDIOCHUNK, legacy_volume_view.unk2D) == 45);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk2E_22, offsetof(stunts_AUDIOCHUNK, unk2E) == 46);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk32_23, offsetof(stunts_AUDIOCHUNK, unk32) == 50);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk33_24, offsetof(stunts_AUDIOCHUNK, unk33) == 51);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk47_25, offsetof(stunts_AUDIOCHUNK, unk47) == 71);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg027_unk48_26, offsetof(stunts_AUDIOCHUNK, unk48) == 72);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_pos_0, offsetof(stunts_AUDIOCHUNK, resource_data) == 0);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_depth_1, offsetof(stunts_AUDIOCHUNK, depth) == 4);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_stack_2, offsetof(stunts_AUDIOCHUNK, stack) == 5);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_activeVoices_3, offsetof(stunts_AUDIOCHUNK, activeVoices) == 21);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_maxVoices_4, offsetof(stunts_AUDIOCHUNK, maxVoices) == 22);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_reserved17_5, offsetof(stunts_AUDIOCHUNK, reserved17) == 23);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_delay_6, offsetof(stunts_AUDIOCHUNK, delay) == 24);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_reserved1c_7, offsetof(stunts_AUDIOCHUNK, reserved1c) == 28);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_data_8, offsetof(stunts_AUDIOCHUNK, sample_data) == 30);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_velocity_9, offsetof(stunts_AUDIOCHUNK, velocity) == 34);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_resourceType_10, offsetof(stunts_AUDIOCHUNK, resourceType) == 35);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_note_11, offsetof(stunts_AUDIOCHUNK, note) == 36);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_modeValue_12, offsetof(stunts_AUDIOCHUNK, modeValue) == 37);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_value26_13, offsetof(stunts_AUDIOCHUNK, value26) == 38);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_program_14, offsetof(stunts_AUDIOCHUNK, program) == 40);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_reserved29_15, offsetof(stunts_AUDIOCHUNK, reserved29) == 41);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_samples_16, offsetof(stunts_AUDIOCHUNK, samples) == 46);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_loopDepth_17, offsetof(stunts_AUDIOCHUNK, loopDepth) == 50);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_loopPos_18, offsetof(stunts_AUDIOCHUNK, voice_loop_view.loopPos) == 51);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_loopCount_19, offsetof(stunts_AUDIOCHUNK, voice_loop_view.loopCount) == 67);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_channelNumber_20, offsetof(stunts_AUDIOCHUNK, channelNumber) == 71);
STUNTS_LAYOUT_ASSERT(stunts_audiochunk_obj_seg028_callback_21, offsetof(stunts_AUDIOCHUNK, callback) == 72);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk00_0, offsetof(stunts_AUDIOVOICE, unk00) == 0);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_state_1, offsetof(stunts_AUDIOVOICE, state) == 1);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk02_2, offsetof(stunts_AUDIOVOICE, unk02) == 2);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk03_3, offsetof(stunts_AUDIOVOICE, reserved03) == 3);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_position_4, offsetof(stunts_AUDIOVOICE, position) == 8);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_length_5, offsetof(stunts_AUDIOVOICE, length) == 12);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk10_6, offsetof(stunts_AUDIOVOICE, unk10) == 16);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk14_7, offsetof(stunts_AUDIOVOICE, unk14) == 20);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk2A_8, offsetof(stunts_AUDIOVOICE, unk2A) == 42);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk2C_9, offsetof(stunts_AUDIOVOICE, unk2C) == 44);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg027_unk2D_10, offsetof(stunts_AUDIOVOICE, unk2D) == 45);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_resourceIndex_0, offsetof(stunts_AUDIOVOICE, resourceIndex) == 0);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_active_1, offsetof(stunts_AUDIOVOICE, active) == 1);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_note_2, offsetof(stunts_AUDIOVOICE, note) == 2);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_reserved03_3, offsetof(stunts_AUDIOVOICE, reserved03) == 3);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_position_4, offsetof(stunts_AUDIOVOICE, position) == 8);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_remaining_5, offsetof(stunts_AUDIOVOICE, remaining) == 12);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_data_6, offsetof(stunts_AUDIOVOICE, data) == 16);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_sampleRate_7, offsetof(stunts_AUDIOVOICE, runtime_voice_view.sampleRate) == 20);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_state16_8, offsetof(stunts_AUDIOVOICE, runtime_voice_view.state16) == 22);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_reserved17_9, offsetof(stunts_AUDIOVOICE, runtime_voice_view.reserved17) == 23);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value18_10, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value18) == 24);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value1a_11, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value1a) == 26);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value1c_12, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value1c) == 28);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value1e_13, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value1e) == 30);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value20_14, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value20) == 32);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value22_15, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value22) == 34);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_reserved23_16, offsetof(stunts_AUDIOVOICE, runtime_voice_view.reserved23) == 35);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value24_17, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value24) == 36);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value26_18, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value26) == 38);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value27_19, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value27) == 39);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value28_20, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value28) == 40);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_value29_21, offsetof(stunts_AUDIOVOICE, runtime_voice_view.value29) == 41);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_resource_22, offsetof(stunts_AUDIOVOICE, resource) == 42);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_channelNumber_23, offsetof(stunts_AUDIOVOICE, channelNumber) == 44);
STUNTS_LAYOUT_ASSERT(stunts_audiovoice_obj_seg028_reserved2d_24, offsetof(stunts_AUDIOVOICE, reserved2d) == 45);

#undef STUNTS_LAYOUT_ASSERT
#endif /* STUNTS_STRUCTS_TARGET_H */
