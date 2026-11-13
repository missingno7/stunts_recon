#ifndef STUNTS_UI_H
#define STUNTS_UI_H

/* Text, dialogs and menus. */
/* Game menus, dialogs and user interface limits. */

#define BUTTON_TEXT_LINE_HEIGHT 8
#define CAMERA_CUSTOM_DISTANCE_MIN 120
#define CAMERA_FOLLOW_DISTANCE 0x1C2
#define CAMERA_FOLLOW_HEIGHT 0x10E
#define CAMERA_FOLLOW_MAX_STEP_10FPS 0xF0
#define CAMERA_FOLLOW_MAX_STEP_20FPS 0x78
#define CAMERA_FOLLOW_VERTICAL_STEP 0x1E
#define CAMERA_MANUAL_DISTANCE_STEP 30
#define CAMERA_MANUAL_HEIGHT_STEP 30
#define CAMERA_MODE_COCKPIT 0
#define CAMERA_MODE_COUNT 4
#define CAMERA_MODE_F2 1
#define CAMERA_MODE_F3 2
#define CAMERA_MODE_F4 3
#define DIALOG_COMPACT_LINE_SPACING 4
#define DIALOG_CONTENT_WIDTH_INSET 16
#define DIALOG_FRAME_MARGIN 8
#define DIALOG_LINE_SPACING 2
#define DIALOG_MODE_CHOICE 2
#define DIALOG_MODE_DRAW_ONLY 0
#define DIALOG_MODE_MEASURE_MARKERS 3
#define DIALOG_MODE_SHORT_WAIT 4
#define DIALOG_MODE_WAIT_KEY 1
#define DIALOG_OUTLINE_MARGIN 4
#define DIALOG_POSITION_CENTER -1
#define DIALOG_SHORT_WAIT_TICKS 8L
#define DIALOG_TEXT_MIN_WIDTH 32
#define DIALOG_TEXT_WIDTH_ALIGNMENT_MASK 0xfff8
#define DIALOG_WIDTH_PADDING 24
#define GAME_CAMERA_BUTTON_CAPACITY 9
#define HUD_DEMO_MESSAGE_FIRST_Y 0xAA
#define HUD_DEMO_MESSAGE_SECOND_Y 0xB6
#define HUD_OPPONENT_MARKER_Y 0x71
#define HUD_OPPONENT_MESSAGE_Y 0x74
#define HUD_PENALTY_MESSAGE_Y 0x66
#define HUD_PLAYER_MARKER_X 0x94
#define HUD_PRIMARY_MESSAGE_Y 0x5D
#define INTRO_CAR_HEADING_UNSET 0xffff
#define MENU_CURSOR_BLINK_HALF_PERIOD 30
#define MENU_CURSOR_BLINK_PERIOD 60
#define MENU_WINDOW_WIDTH_PIXELS 0xC8
#define SINKING_DURATION_SECONDS 4
#define SPEEDOMETER_HUNDREDS_BASE 100
#define SPEEDOMETER_TWO_HUNDREDS_BASE 200
#define STARTUP_CPU_BENCH_INNER 255
#define STARTUP_CPU_BENCH_OUTER 146
#define STARTUP_DETAIL_THRESHOLD_HIGH 100
#define STARTUP_DETAIL_THRESHOLD_LOW 55
#define STARTUP_DETAIL_THRESHOLD_LOWEST 35
#define STARTUP_DETAIL_THRESHOLD_MEDIUM 75
#define STARTUP_RANDOM_FALLBACK_CHECK 1024
#define STARTUP_RANDOM_SEED_BYTE_MASK 0xff
#define STARTUP_RECT_BENCH_HEIGHT 60
#define STARTUP_RECT_BENCH_POINTS 400
#define STARTUP_VIDEO_BENCH_HEIGHT 120
#define STARTUP_VIDEO_BENCH_REPEATS 15
#define STARTUP_VIDEO_WAIT_LIMIT 12000
#define TIME_FIELD_DIGITS 2
#define TIME_HUNDREDTHS_PER_SECOND 100
#define TIME_SECONDS_PER_MINUTE 60
#define TRACK_CACHE_INVALID 0xff
#define TRACK_EDITOR_CELL_ABOVE_Y (-12)
#define TRACK_EDITOR_CELL_BELOW_Y 20
#define TRACK_EDITOR_CELL_LEFT_X (-8)
#define TRACK_EDITOR_CELL_ORIGIN_X 8
#define TRACK_EDITOR_CELL_ORIGIN_Y 4
#define TRACK_EDITOR_CELL_PIXELS 16
#define TRACK_EDITOR_CELL_RIGHT_X 24
#define TRACK_EDITOR_CELL_SHIFT 4
#define TRACK_EDITOR_CURSOR_COUNT 4
#define TRACK_EDITOR_GRID_X_ORIGIN 220
#define TRACK_EDITOR_GRID_Y_ORIGIN 36
#define TRACK_EDITOR_PALETTE_ROW_Y_ORIGIN 28
#define TRACK_PALETTE_GRID_SIDE 6
#define TRACK_PALETTE_PAGE_CELL_COUNT 36
#define TRACK_PREVIEW_CACHE_COUNT 132
#define TRACK_PREVIEW_COLUMN_COUNT 12
#define TRACK_PREVIEW_ROW_COUNT 11
#define TRACK_SHAPE_CACHE_INVALID 0xff
#define UI_AUDIOFILETMP_CAPACITY 128
#define UI_CAR_IDLE_TIMEOUT 0x2ee0
#define UI_CAR_MENU_BUTTON_COUNT 5
#define UI_CAR_MENU_DONE 0
#define UI_CAR_MENU_MATERIAL 4
#define UI_CAR_MENU_NEXT 1
#define UI_CAR_MENU_PREVIOUS 2
#define UI_CAR_MENU_TRANSMISSION 3
#define UI_DASHBOARD_OPERATION_DRAW_BASE 1
#define UI_DASHBOARD_OPERATION_LOAD 0
#define UI_DASHBOARD_OPERATION_UNLOAD 3
#define UI_DASHBOARD_OPERATION_UPDATE 2
#define UI_DIALOG_BUTTON_CAPACITY 20
#define UI_DIALOG_CENTER 0xFFFF
#define UI_DISPLAY_MENU_CAMERA 2
#define UI_DISPLAY_MENU_DASHBOARD 0
#define UI_DISPLAY_MENU_FOLLOW_OPPONENT 4
#define UI_DISPLAY_MENU_GRAPHICS 3
#define UI_DISPLAY_MENU_REPLAY_BAR 1
#define UI_EDITOR_COMMAND_ROW_LOAD_CLEAR 8
#define UI_EDITOR_COMMAND_ROW_PAGE 6
#define UI_EDITOR_COMMAND_ROW_SKY 7
#define UI_EDITOR_PALETTE_FIRST_TRACK 1
#define UI_EDITOR_PALETTE_LAST_TRACK 10
#define UI_EDITOR_PALETTE_TERRAIN 0
#define UI_EDITOR_PREVIEW_LAST_COLUMN 11
#define UI_EDITOR_PREVIEW_LAST_ROW 10
#define UI_EDITOR_TRACK_PAGE_COUNT 10
#define UI_HIG_CAR_COLUMN_X 120
#define UI_HIG_FIRST_ROW_Y 25
#define UI_HIG_NAME_LIMIT 16
#define UI_HIG_ROW_HEIGHT 10
#define UI_HIG_STATE_INELIGIBLE -1
#define UI_HIG_STATE_NORMAL 0
#define UI_HIG_STATE_QUALIFYING 1
#define UI_HIG_STATE_SHOW_EVALUATION 2
#define UI_LINE_CURSOR_BAR_PIXELS 1
#define UI_LINE_CURSOR_BLOCK_PIXELS 8
#define UI_LINE_EDIT_CLEAR_BUFFER 1
#define UI_LINE_EDIT_CURSOR_AT_START 2
#define UI_LINE_EDIT_KEEP_DOWN_KEY 8
#define UI_LINE_EDIT_KEEP_FIRST_TEXT 4
#define UI_LINE_EDIT_KEEP_TAB_KEY 0x10
#define UI_MAIN_CANCEL -1
#define UI_MAIN_CAR 1
#define UI_MAIN_IDLE_TIMEOUT 6000
#define UI_MAIN_MENU_BUTTON_COUNT 5
#define UI_MAIN_OPPONENT 2
#define UI_MAIN_OPTIONS 4
#define UI_MAIN_START 0
#define UI_MAIN_TRACK 3
#define UI_ONE_THROUGH_FOURTEEN_TABLE_CAPACITY 14
#define UI_OPPONENT_MENU_BUTTON_COUNT 5
#define UI_OPPONENT_MENU_CAR 3
#define UI_OPPONENT_MENU_DONE 4
#define UI_OPPONENT_MENU_NEXT 1
#define UI_OPPONENT_MENU_NONE 2
#define UI_OPPONENT_MENU_PREVIOUS 0
#define UI_OPTION_MENU_DONE 6
#define UI_OPTION_MENU_DOS 5
#define UI_OPTION_MENU_GRAPHICS 4
#define UI_OPTION_MENU_INPUT 0
#define UI_OPTION_MENU_LOAD_REPLAY 3
#define UI_OPTION_MENU_MOF_RESOURCE 1
#define UI_OPTION_MENU_SONSOF_RESOURCE 2
#define UI_PAUSE_MENU_CONTINUE_DRIVING 3
#define UI_PAUSE_MENU_DISPLAY_OPTIONS 6
#define UI_PAUSE_MENU_LOAD_REPLAY 4
#define UI_PAUSE_MENU_MAIN_MENU 7
#define UI_PAUSE_MENU_RESTART 2
#define UI_PAUSE_MENU_RESULTS 1
#define UI_PAUSE_MENU_SAVE_REPLAY 5
#define UI_RACE_RESULT_NO_WINNER 2
#define UI_RACE_RESULT_OPPONENT_WINS 1
#define UI_RACE_RESULT_PLAYER_WINS 0
#define UI_REPLAY_BUTTON_DOUBLE_SPEED 2
#define UI_REPLAY_BUTTON_MENU 6
#define UI_REPLAY_BUTTON_PAN 8
#define UI_REPLAY_BUTTON_PAUSE 4
#define UI_REPLAY_BUTTON_PLAY 3
#define UI_REPLAY_BUTTON_REWIND_START 5
#define UI_REPLAY_BUTTON_SEEK_BACKWARD 1
#define UI_REPLAY_BUTTON_SEEK_FORWARD 0
#define UI_REPLAY_BUTTON_ZOOM 7
#define UI_REPLAY_OPERATION_DRAW 1
#define UI_REPLAY_OPERATION_HANDLE_INPUT 3
#define UI_REPLAY_OPERATION_INITIALIZE 0
#define UI_REPLAY_OPERATION_SET_PRESSED 2
#define UI_REPLAY_PROGRESS_TRAVEL 110
#define UI_RESULTS_ANIMATION_TICKS 30
#define UI_RESULTS_BUTTON_COUNT 5
#define UI_RESULTS_BUTTON_DRIVE 2
#define UI_RESULTS_BUTTON_EVALUATE 0
#define UI_RESULTS_BUTTON_MAIN_MENU 3
#define UI_RESULTS_BUTTON_REPLAY 1
#define UI_SAVE_STATE_CANCEL -1
#define UI_SAVE_STATE_RETRY 0
#define UI_SAVE_STATE_WRITE 1
#define UI_TRACKS_MENU_CANCEL -1
#define UI_TRACKS_MENU_EDIT 1
#define UI_TRACKS_MENU_LOAD 0
#define UI_TRACKS_MENU_RETURN 2
#define UI_TRACK_EDITOR_REGION_COUNT 5
#define UI_TRACK_MENU_BUTTON_COUNT 3

/* Shared fontrec declarations; DOS medium-model types. */
struct FONTDEF_PREFIX;
struct SCREEN_RECT;
#pragma pack()
struct FONTDEF_PREFIX {
    unsigned char bytes[14];
    unsigned short value;
};
#pragma pack()
struct SCREEN_RECT {
    short foreground;
    short background;
    short font_header_middle[7];
    short line_height;
};
/* Shared text interface. */
struct RECTANGLE;
struct SCREEN_RECT;
extern char aFontdef_fnt[];
extern char aFontn_fnt[];
extern char audiodriverstring[];
extern char *findfiletexts[4];
extern unsigned int font_secondary_color;
extern unsigned char far fontdef_default[1408];
extern unsigned int fontdefvalue;
extern short fontled_free_4[2];
extern struct SCREEN_RECT far *line_input_font_header;
extern char opptext_label[3];
extern struct RECTANGLE rect_ingame_text2;
extern struct RECTANGLE rect_ingame_text3;
extern struct RECTANGLE rect_ingame_text4;
extern int text_bounds_right;
extern int text_bounds_upper;
extern int text_cursor_outline_color;
extern int text_outline_bottom;
extern int text_outline_right;
extern int textboundsleft;
extern int textoutlineleft;
extern char textrespfxchr;
extern char textstr[40];
extern char *far _strcat(char *, const char *);
extern int far _strcmp(const char *, const char *);
extern char *far _strcpy(char *, const char *);
extern int far _stricmp(const char *, const char *);
extern unsigned far _strlen(const char *);
extern int far cap(int);
extern short far do_dea_textres(void);
extern void far mdosexit(void);
extern void far do_joystick_resource_text(void);
extern void far mmusictog(void);
extern void far mmouseon(void);
extern void far msoundtog(void);
extern struct RECTANGLE *far drawtext(void);
extern void far fortextxy(char *, int, int);
extern void far fontsetfontdef(void);
extern void far mysetfont(void far *);
extern void far drawhighs(void);
extern void far genhline(int, char *);
extern void far setfont(void far *);
extern char *far strcat(char *, const char *);
extern int far strcmp(char *, char *);
extern char *far strcpy(char *, const char *);
extern void far strcpyfar(char *, char far *);
extern int far stricmp(const char *, const char *);
extern char *far strrchr(const char *, int);
extern int far textnpixels(char *, int);
extern int far textpixels(char *);
/* Shared dialog interface. */
struct GAMEINFO;
struct GAMESTATE_SNAPSHOT;
struct RECTANGLE;
struct SHAPE3D;
struct SPRITE;
extern char aA00[];
extern char aCarcoun[];
extern struct RECTANGLE *active_ui_dirty_rect;
extern char aKevin[];
extern char aLnam[6];
extern int animation_outline_color;
extern char aOp01[];
extern char aOpp1[];
extern char aOpp2lose[];
extern char aOpp2win[];
extern char aQ00[];
extern int camlen;
extern int camera_select_fill_color;
extern int camera_select_outline_color;
extern char cammd;
extern char buf_g_path[94];
extern int carmenu_buttons_x1[5];
extern int carmenu_buttons_x2[5];
extern int carmenu_buttons_y1[5];
extern int carmenu_buttons_y2[5];
extern void far *def_fntadr;
extern int camheading;
extern int campitch;
extern char far *coursedataappend_address;
extern unsigned char entry_score_index;
extern int g_animphase;
extern char g_gsnashape_data[5];
extern int g_hovercolor_idle;
extern struct GAMEINFO globalgamesettings;
extern struct GAMEINFO gmconfigbackup;
extern int hiscore_anim_table[28];
extern short hiscore_buttons_x1[5];
extern short hiscore_buttons_x2[5];
extern short hiscore_buttons_y1[5];
extern short hiscore_buttons_y2[5];
extern short hiscore_entry_remap[4];
extern short hiscore_rank_remap[3];
extern int intro_color_max;
extern int joyflags;
extern int joyinputcode;
extern int menu_button_color_a;
extern int menu_button_color_b;
extern int menu_button_color_c;
extern int menu_buttons_x1[5];
extern int pixel_scales;
extern int menu_buttons_y1[5];
extern int menu_buttons_y2[5];
extern int menu_clear_color;
extern int menu_hover_color_a;
extern int menu_hover_color_b;
extern signed char menu_left[6];
extern signed char menu_right[6];
extern int msecoordx;
extern int newjoyflags;
extern unsigned short one_through_fourteen_table[14];
extern int opponentmenu_buttons_x1[5];
extern int opponentmenu_buttons_x2[5];
extern int opponentmenu_buttons_y1[5];
extern int opponentmenu_buttons_y2[5];
extern char pass_check_flag;
extern int menu_buttons_x2[5];
extern int pos_y_ms;
extern struct GAMESTATE_SNAPSHOT race_stats;
extern struct RECTANGLE *rectp;
extern char ReplayName[10];
extern char results_entryname[17];
extern unsigned char score_entry_alphabet[];
extern short scrorder_idxs[7];
extern int terheight[2];
extern int waitm_ms;
extern int far doslider(int, int, int, int, int, int, int, int);
extern void far draw3dbutton(char far *, int, int, int, int, int, int, int, int);
extern void far addhighs(unsigned short, char far *, unsigned char);
extern int far drnd(void);
extern void far savehighs(void);
extern int far main(int, char *[]);
extern void far makename(char *, char *, char *, char *);
extern void far makeobjdef(char far *, struct SHAPE3D *);
extern void far memerr(void);
extern void far mkeyon(void);
extern void far mouseoff(void);
extern void far mouseon(void);
extern void far msystempause(void);
extern int far mycenter(char *);
extern void far namecopy(char *, char *);
extern char far point_in_rectangle(int, int, int, int);
extern void far docursorinit(void);
extern void far restoreboxwindow(void);
extern int far run_intro(void);
extern void far selectopponent(void);
extern char far select(void);
extern void far initselections(void);
extern void far msetrender(void);
extern void far teditmain(int);
extern void far waitforkey(void);

/* Ui services and shared state. */
struct HighScoreRecord;
extern unsigned short  dialog_frame_color;
extern struct HighScoreRecord far *hscore_trk11_ptr;
extern unsigned short  intro_text_color_a;
extern unsigned short  intro_text_color_b;
extern unsigned short  intro_text_color_c;
extern unsigned short  intro_text_color_d;
extern unsigned short  intro_text_color_e;
extern unsigned short  intro_text_color_f;
extern unsigned short  intro_text_style_a;
extern unsigned short  intro_text_style_b;
extern unsigned short  intro_text_style_c;
extern unsigned short  intro_text_style_d;
extern unsigned short  intro_text_style_e;
extern unsigned short  intro_text_style_f;
extern char menutimeout;
extern char popupact;
extern char *table_lookup;
extern int unused_count;
extern void far abortmessage(char *);
extern void far debug_printf_text(char *, int);
extern void drawpiecebox();
extern void drawtrackbox();
extern char far evaluation(void);
extern int far get_pstring(int, char *, int, int, int, int, int, void (far *)(void), int, int);
extern int far instringz(char *, int, int, int, long);
extern 

char far loadreplay(const char *, const char *);
extern void far maketime(char *, unsigned short, int);
extern void far myitoa(char *, int, int, int);
extern char far readhighs(int);
extern char far run_option_menu(void);
extern int * shadowtext(char *, int, int, int, int);
extern void far textcolor(int, int);
struct GAMEINFO;
extern void selectcar(struct GAMEINFO *, char *, char *, unsigned int);
#endif
