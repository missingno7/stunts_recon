/* Canonical GAMEINFO view; incompatible operational views stay local. */
#ifndef STUNTS_GAMEINFO_H
#define STUNTS_GAMEINFO_H
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
#endif
