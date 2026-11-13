#ifndef STUNTS_GAMESTAT_H
#define STUNTS_GAMESTAT_H

/* Canonical game configuration and race state records. */
/* Canonical GAMEINFO view; incompatible operational views stay local. */
#pragma pack()
struct GAMEINFO {
    char game_playercarid[4];
    char game_playermaterial;
    char game_playertransmission;
    char game_opponenttype;
    char game_opponentcarid[4];
    char game_opponentmaterial;
    char game_opponenttransmission;
    char game_trackname[9];
    char game_framespersec;
    short game_recordedframes;
};
/* Canonical GAMESTATE view; incompatible operational views stay local. */
struct CARSTATE;
struct VECTOR;
#include "math3d.h"
#pragma pack()
struct GAMESTATE {
    long game_debris_x[24];
    long game_debris_y[24];
    long game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2];
    struct VECTOR game_chase_camera_prev[2];
    short game_end_delay_elapsed;
    short game_end_delay_limit;
    long game_speed_frame_sum;
    unsigned short game_frame;
    short game_total_finish;
    short game_opponent_finish_time;
    short game_pEndFrame;
    short game_oEndFrame;
    short game_penalty;
    unsigned short game_impactSpeed;
    unsigned short game_topSpeed;
    short game_jumpCount;
    struct CARSTATE playerstate;
    struct CARSTATE opponentstate;
    short game_player_route_anchor;
    short game_player_last_route_node;
    short game_startcol;
    short game_startcol2;
    short game_startrow;
    short game_startrow2;
    short game_debris_rot_x[24];
    short game_debris_rot_y[24];
    short game_debris_rot_z[24];
    short game_debris_planar_step[24];
    char game_debris_vertical_step[48];
    char kevinseed[6];
    char game_checkpoint_valid;
    char game_inputmode;
    char game_3F6autoLoadEvalFlag;
    char game_track_camera_index[2];
    char game_opponent_speed_limit;
    char game_track_object_broken[48];
    char game_debris_active;
    char game_debris_shape_index[24];
    char game_debris_origin_kind[24];
    char game_route_recovery_mode;
    char game_route_confirm_count;
    char game_player_guidance;
    char game_opponent_pass_side;
    char field_45F;
};

#endif
