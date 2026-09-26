struct GAMEINFO {
    char game_playercarid[4];
    char game_playermaterial;
    char game_playertransmission;
    char game_opponenttype;
    char game_opponentcarid[4];
    char game_opponentmaterial;
    char game_opponenttransmission;
    char game_trackname[9];
    unsigned short game_framespersec;
    unsigned short game_recordedframes;
};
struct HighScoreRecord { unsigned char bytes[50]; unsigned short marker; };

extern struct GAMEINFO gameconfig;
extern char far *mainresptr;
extern char resID_byte1[];
extern unsigned short dialog_fnt_colour;
extern unsigned short dialogarg2;
extern unsigned char byte_449CE;
extern unsigned short framespersec;
extern void far *fontnptr;
extern struct HighScoreRecord far *td11_highscores;
extern short word_46170[7];

extern void far sprite_copy_wnd_to_1(void);
extern char far * far locate_text_res(char far *data, char *name);
extern void copy_string(char *destination, char far *source);
extern int far font_op2_alt(char *name);
extern void far hiscore_draw_text(int x, int width, int style,
                                  int colour, int mode);
extern void far font_set_fontdef2(void far *data);
extern void far font_set_fontdef(void);
extern void far font_set_unk(int colour, int mode);
extern void far font_draw_text(char *text, int x, int y);
extern void far format_frame_as_string(char *destination,
                                       unsigned short frames, int mode);
extern char *strcpy(char *destination, char *source);
extern char *strcat(char *destination, char *source);
extern int strlen(char *text);
extern void far print_highscore_entry(int rowIndex, char *stringOffsets);


void far print_highscore_entry(int rowIndex, char *stringOffsets)
{
    char timeText[18];
    char textLength;
    int frameRate;
    struct HighScoreRecord scoreRecord;

    scoreRecord = td11_highscores[word_46170[rowIndex]];
    stringOffsets[0] = 0;
    strcpy(resID_byte1, (char *)scoreRecord.bytes);
    textLength = strlen(resID_byte1) + 1;
    stringOffsets[1] = textLength;
    strcpy(resID_byte1 + textLength, (char *)scoreRecord.bytes + 17);
    textLength += strlen(resID_byte1 + textLength) + 1;
    stringOffsets[2] = textLength;
    resID_byte1[textLength] = 0;
    if (scoreRecord.bytes[41] == 1)
        strcat(resID_byte1 + textLength, "(");
    strcat(resID_byte1 + textLength, (char *)scoreRecord.bytes + 42);
    if (scoreRecord.bytes[41] == 1)
        strcat(resID_byte1 + textLength, ")");
    textLength += strlen(resID_byte1 + textLength) + 1;
    frameRate = framespersec;
    framespersec = 20;
    if (scoreRecord.marker != 0xffff)
        format_frame_as_string(timeText, scoreRecord.marker, 1);
    else
        format_frame_as_string(timeText, 0, 1);
    stringOffsets[3] = textLength;
    strcpy(resID_byte1 + textLength, timeText);
    framespersec = frameRate;
}
