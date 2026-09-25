struct GAMECONFIG {
    unsigned char game_playercarid[4];
    unsigned char game_playermaterial;
    unsigned char game_playertransmission;
    unsigned char game_opponenttype;
    unsigned char game_opponentcarid[4];
    unsigned char game_opponentmaterial;
};
extern struct GAMECONFIG gameconfig;

void set_default_car(void)
{
    gameconfig.game_playercarid[0] = 'C';
    gameconfig.game_playercarid[1] = 'O';
    gameconfig.game_playercarid[2] = 'U';
    gameconfig.game_playercarid[3] = 'N';
    gameconfig.game_playermaterial = 0;
    gameconfig.game_opponenttype = 0;
    gameconfig.game_opponentmaterial = 0;
    gameconfig.game_playertransmission = 1;
    gameconfig.game_opponentcarid[0] = 0xff;
}
