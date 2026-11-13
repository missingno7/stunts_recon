/* Shared gamerec declarations; DOS medium-model types. */
#ifndef STUNTS_GAMEREC_H
#define STUNTS_GAMEREC_H
struct GAMESTATE_SNAPSHOT;
struct HighScoreRecord;
struct LOOKAHEAD_TILE;
struct SecurityDialogResult;
#pragma pack()
struct GAMESTATE_SNAPSHOT {
    long game_speed_frame_sum;
    unsigned short game_frame;
    short game_total_finish;
    short game_opponent_finish_time;
    short game_pEndFrame;
    short game_oEndFrame;
    unsigned short game_penalty;
    unsigned short game_impactSpeed;
    unsigned short game_topSpeed;
    short game_jumpCount;
};
#pragma pack()
struct HighScoreRecord {
    unsigned char bytes[50];
    unsigned short marker;
};
#pragma pack()
struct LOOKAHEAD_TILE {
    char east_delta;
    char south_delta;
    char detail;
};
#pragma pack()
struct SecurityDialogResult {
    short first_x;
    short first_y;
    short second_x;
    short second_y;
    short third_x;
    short third_y;
    short input_x;
    short input_y;
    short trailing_state[4];
};

/* Game configuration. */
struct GAMEINFO;
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
#endif
