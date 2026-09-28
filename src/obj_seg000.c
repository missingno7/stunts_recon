/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
/* Scratch reconstruction TU: source bodies ordered by locked seg000 extents. */
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
struct VECTOR { int x, y, z; };
struct VECTORLONG { long x, y, z; };
struct POINT2D { int x, y; };
struct RECTANGLE { int left, right, top, bottom; };
struct SPRITE { void far *image; unsigned short words[13]; };
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    short car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm;
    short car_idlerpm2, car_speeddiff;
    unsigned short car_speed, car_speed2, car_lastspeed;
    unsigned short car_gearratio, car_gearratioshr8;
    short car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    short car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    short car_surfacegrip_sum, field_48, car_trackdata3_index;
    short car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    short field_B6, field_B8, field_BA;
    char car_is_braking, car_is_accelerating, car_current_gear;
    char car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    char car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    char car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    char field_CD, field_CE, field_CF;
};
struct GAMESTATE {
    long game_longs1[24], game_longs2[24], game_longs3[24];
    struct VECTOR game_vec1[2], game_vec3, game_vec4;
    short game_frame_in_sec, game_frames_per_sec;
    long game_travDist;
    short game_frame, game_total_finish, field_144, game_pEndFrame;
    short game_oEndFrame, game_penalty;
    unsigned short game_impactSpeed, game_topSpeed;
    short game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    short field_2F2, field_2F4, game_startcol, game_startcol2;
    short game_startrow, game_startrow2;
    short field_2FE[24], field_32E[24], field_35E[24], field_38E[24];
    char field_3BE[48], kevinseed[6], field_3F4, game_inputmode;
    char game_3F6autoLoadEvalFlag, field_3F7[2], field_3F9, field_3FA[48];
    char field_42A, field_42B[24], field_443[24];
    char field_45B, field_45C, field_45D, field_45E, field_45F;
};
struct SIMD {
    char num_gears, simd_unk;
    short car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    unsigned short gear_ratios[7];
    struct POINT2D knob_points[7];
    short aero_resistance;
    char idle_torque, torque_curve[104], field_A3;
    short grip, field_A6[7], sliding, surface_grip[4];
    char simd_unk3[10];
    struct POINT2D collide_points[2];
    short car_height;
    struct VECTOR wheel_coords[4];
    char steeringdots[62];
    struct POINT2D spdcenter;
    short spdnumpoints;
    char spdpoints[208];
    struct POINT2D revcenter;
    short revnumpoints;
    char revpoints[256];
    short far *aerorestable;
};
struct SHAPE3D {
    unsigned int shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    unsigned int shape3d_numprimitives;
    unsigned int shape3d_numpaints;
    char far *shape3d_primitives;
    char far *shape3d_cull1;
    char far *shape3d_cull2;
};
struct TRANSFORMEDSHAPE3D {
    struct VECTOR pos;
    struct SHAPE3D *shapeptr;
    struct RECTANGLE *rectptr;
    struct VECTOR rotvec;
    unsigned int unk;
    unsigned char ts_flags;
    unsigned char material;
};
struct OPPONENTIMAGE { unsigned int width, height; };
extern void far audio_unload(void);
extern void far call_read_line(char *destination, int maxLength, int x, int y, long source);
extern void far check_input(void);
extern void copy_string(char *destination, char far *source);
extern void far do_dos_resource_text(void);
extern char far do_fileselect_dialog(char *directory, char *track,
                                     char *extension, char far *text);
extern void far do_joystick_resource_text(void);
extern void far do_key_resource_text(void);
extern void far do_mof_resource_text(void);
extern void far do_mou_resource_text(void);
extern void far do_sonsof_resource_text(void);
extern void far draw_button(char far *text, int id, int x, int y, int width,
                            int a, int b, int c, int mode);
extern void far draw_lines_unknown(int x, int y, int width, int lineCount,
                               int colour, int mode, int style);
extern void far draw_track_preview(void);
extern void far ensure_file_exists(int kind);
extern void far enter_hiscore(unsigned short score, char far *text, unsigned char carStyle);
extern void far file_build_path(char *directory, char *name, char *extension,
                                char *destination);
extern char * far file_combine_and_find(char *, char *, char *);
extern char * far file_find_next_alt(void);
extern void far file_load_audio_resource(char *name1, char *name2, char *name3);
extern short far file_load_replay(char *dir, char *name);
extern void far *far file_load_resource_file(char *name);
extern void far * far file_load_resource();
extern void far * file_load_shape2d_fatal_thunk(char *);
extern void far file_read_fatal(char *path, unsigned char far *destination);
extern short far file_write_fatal(char *path, struct HighScoreRecord far *source,
                                  unsigned long length);
extern void far font_draw_text(char *text, int x, int y);
extern int far font_op2(char *text);
extern int far font_op2_alt(char *name);
extern void far fontsetfontdef(void);
extern void far fontsetfontdef2(void far *data);
extern void far font_setup_unknown(int colour, int mode);
extern void far fmtframestr(char *destination, unsigned short frames, int mode);
extern void far polyinfo(void);
extern void far highscore_text_unknown(void);
extern char far highscore_write_a(int create_defaults);
extern void far highscore_write_b(void);
extern void far hiscore_draw_text();
extern void far initialize_game_state(int state);
extern int far input_checking(int delta);
extern void far introtext(char *text, int width, int x, int colour, int mode);
extern void far load_skybox(unsigned char skybox);
extern void far load_tracks_menu_shapes(void);
extern void far locate_many_resources(void far *, char *, void far **);
extern void far * far locate_shape_alt();
extern struct SHAPE2D far * far locate_shape_fatal(void far *, char *);
extern char far * far locate_text_resource(char far *data, char *name);
extern void far mmgr_free(void far *);
extern void far mmgr_release(void far *resource);
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern int far mouse_multi_hittest();
extern int far mouse_timer_sprite_unknown(int selection, int *x1, int *x2,
                                      int *y1, int *y2, int left, int right);
extern void far nullsub_1(void);
extern void far nullsub_2(void far *, int);
extern int far polang(int, int);
extern void far print_highscore_entry(int rowIndex, char *stringOffsets);
extern int far print_int_as_string_maybe(char *text, int value, int mode, int width);
extern void far putpixel_single_maybe(int, int, int);
extern int far rcintersect(struct RECTANGLE *, struct RECTANGLE *);
extern void far rcunion(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
extern void far run_car_menu(char *, char *, char *, int);
extern int far select_rot(int, int, int, struct RECTANGLE *, int);
extern void far set_projection(int left, int top, int width, int height);
extern void far setup_aero_trackdata(void far *, int);
extern void far setup_mcgawnd2(void);
extern void far shape2d_op_unknown5(void far *shape, int x, int y);
extern void far shape3d_free_all(void);
extern void far shape3d_free_car_shapes(void);
extern void far shape3d_load_all(void);
extern void far shape3d_load_car_shapes(char *, char *);
extern int far show_dialog();
extern void far show_graphic_levels_menu(void);
extern void far show_waiting(void);
extern int far sprite_blit_to_video(void far *sprite, int effect);
extern void far sprite_clear_1_color(int colour);
extern void far sprite_clear_shape_alt(void far *, int, int);
extern void far sprcopy2to12(void);
extern void far sprite_copy_wnd_to_1(void);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far sprite_free_window(void far *sprite);
extern void far *far sprite_make_window(int width, int height, int depth);
extern void far sprputimage(void far *);
extern void far sprite_putimage_and_alt(void far *, int, int);
extern void far sprite_putimage_transparent(void far *, int, int);
extern void far sprite_setup1_from_arg_pointer(void far *sprite);
extern void far sprset1size(int x, int y, int width, int height);
extern void far sprite_shape_to_1_alt(void far *);
extern char *strcat(char *destination, char *source);
extern int far strcmp(char *, char *);
extern char *strcpy(char *destination, char *source);
extern int strlen(char *text);
extern void far reset_idle_counters(void);
extern void far * far read_file_with_retry(int, char *, void far *);
extern void far release_shape_resources(void far *);
extern void far timer_get_delta_alt(void);
extern char far track_setup(void);
extern unsigned int far trans_op(struct TRANSFORMEDSHAPE3D *);
extern void far unload_resource(void far *resource);
extern void far unload_skybox(void);
extern void far update_car_speed(int, int, struct CARSTATE *, struct SIMD *);
extern char aAvs[];
extern char aBct[];
extern char aBdr[];
extern char aBev[];
extern char aBhi[];
extern char aBmm_0[];
extern char aBra[];
extern char aBrp[];
extern char aCon[];
extern char aD4a[];
extern char aDnf[];
extern char aDnf_0[];
extern char aElt[];
extern char aHna[];
extern char aIhd[];
extern char aImp[];
extern char aInh[];
extern char aInh_0[];
extern char aJum[];
extern char aLose[];
extern char aMisc_2[];
extern char aMph[];
extern char aMph_0[];
extern char aMph_1[];
extern char aOlt[];
extern char aOlt_0[];
extern char aOver[];
extern char aOwt[];
struct GAMESTATE_SNAPSHOT {
    long game_travDist;
    unsigned short game_frame;
    short game_total_finish;
    short field_144;
    short game_pEndFrame;
    short game_oEndFrame;
    unsigned short game_penalty;
    unsigned short game_impactSpeed;
    unsigned short game_topSpeed;
    short game_jumpCount;
};

extern char aPpt[];
extern char aSkidms_1[];
extern char aSkidms_2[];
extern char aSkidover[];
extern char aSkidvict[];
extern char aTop[];
extern char aVict[];
extern char aWinn[];
extern char a_trk_5[];
extern unsigned char backlightovr8;
extern char byte_3FE00;
extern unsigned char endhsdemo;
unsigned char entry_score_index;
char results_entryname[17];
extern struct RECTANGLE clipunk;
extern int dlg_colour;
extern int dialogarg2;
extern short elaptm1;
static short end_hiscore_random;
extern unsigned int fontdefvalue;
void far *fntndat;
extern unsigned short rate_frame;
struct GAMESTATE_SNAPSHOT race_stats;
char buf_g_path[94];
extern struct SHAPE3D g_shapes3d[125];
struct GAMEINFO globalgamesettings;
char textstr[40];
char g_gsnashape_data[5];
int unused_count;
unsigned char menutimeout;
extern char far *main_data_file_addr;
void far *g_miscfile_ptr;
void far *resource_ptropp;
void far *g_opp_resources[7];
extern int performGraphColor;
char resbuftext[80];
extern struct SIMD simdp7;
extern short ground_skybox;
extern unsigned int slow_video_mode_state;
extern unsigned int statemgmtcpy;
extern struct GAMESTATE core;
struct HighScoreRecord far *hscore_trk11_ptr;
extern char far *td14tb;
extern int z_ctr_pos[];
char opptext_label[3];
int pixel_scales;
extern unsigned int g_vid_flg2_set;
extern unsigned int vidflg3is_minus1;
extern unsigned char g_videoflg5;
int waitm_ms;
struct SPRITE far *g_wndspr;
extern int menu_hover_color_a;
extern int menu_hover_color_b;
extern short animation_outline_color;
extern int menu_button_color_a;
extern int menu_button_color_b;
extern int menu_button_color_c;
extern int menu_clear_color;
static short hiscore_rank_prev;
static short hiscore_old_entry;
static short hiscore_opponent_earlier;
static short hiscore_current_place;
static short hiscore_opponent_live;
short scrorder_idxs[7];

struct SHAPE2D { short width, height, unk1, unk2, pos_x, pos_y; };

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

struct GAMEINFO gmconfigbackup;
extern int lnoffsets[], gterrtrk[], r_zp[], row_ctr_zs[];
extern int postable[], xcols[], trackctrpos2[];
void far *def_fntadr;
extern short far *g_td01_track_filecpy;
extern short far *trackdata_penalty_related;
short track3_gap[2];
char far *td3;
extern short far *track04_plyraero;
extern short far *trackdata_05_opp_aerotbl;
char far *td6_ptr_b;
extern char far *trackdat7;
extern int far *g_td08d;
extern int far *trkptrpath;
extern int far *td10checkptr;
char far *savedptr_ms;
extern char far *td13_replay_hdr;
extern unsigned char far *td15p_9;
extern char far *g_tdreplay16buf;
extern char far *road_trk;
extern char far *td_18_ref;
extern unsigned char far *td19hdl;
char far *coursedataappend_address;
extern char far *g_column_of_trkdata21_pth;
extern char far *tdfrompathrow22;
unsigned char far *trkd23adr;
extern struct GAMESTATE far *cvxs_a;
char pass_check_flag;
extern void far initialize_main(int, char *[]);
extern void far initialize_div0(void);
extern void far initialize_polyinfo(void);
extern void far *mmgr_alloc_resbytes(char *, unsigned long);
extern void far initialize_unknown(void);
extern void far initialize_kevin_random(char *);
extern int far input_do_checking(int);
extern int far run_intro_looped(void);
extern char far run_menu(void);
extern void far run_opponent_menu(void);
extern void far run_tracks_menu(int);
extern char far run_option_menu(void);
extern void far _memcpy(void *, void *, unsigned int);
extern void far random_wait(void);
extern int far get_super_random(void);
extern int far get_kevinrandom(void);
extern void far security_check(int);
extern int far file_find(char *);
extern void far run_game(void);
extern char far end_hiscore(void);
extern void far audio_stop_unknown(void);
extern void far audiodrv_atexit(void);
extern void far keyboard_exit_handler(void);
extern void far keyboard_shift_checking1(void);
extern void far video_set_mode7(void);
extern void far set_default_car(void);
void far *loadedresourceptr;
extern void far sprite_copy_2_to_1_clear(void);
extern int far input_repeat_check(int);
extern int far run_intro(void);
extern signed char far setup_intro(void);
extern signed char far load_intro_resources(void);
extern void far sprite_clear_shape(char far*);
extern void far sprite1_unknown2(int, int, int, int, int);
extern unsigned short intro_text_color_a, intro_text_style_a, intro_text_color_b, intro_text_style_b;
extern unsigned short intro_text_color_c, intro_text_style_c, intro_text_color_d, intro_text_style_d;
extern unsigned short intro_text_color_e, intro_text_style_e, intro_text_color_f, intro_text_style_f;
extern unsigned char _ctype[];
extern void far restore_mouse_sprite(void);

struct RECTANGLE *rcpunk2 = 0;
struct RECTANGLE *rectp = 0;
char track_file[82] = {0};
char replay_file[82] = {0};
char aDefault_1[10] = "DEFAULT";
char scenery_names[5][9] = { "desert", "tropical", "alpine", "city", "country" };
int hillconsts[2] = { 0, 450 };
int custom_dist = 210;
int custom_azim_angle = 464;
int custom_elev_angle = 80;
char byte_3B8F2 = 0;
char is_audioloaded = 0;
char HKeyFlag = 0;
char cammd = 0;
char skybox_loaded = 0;
char mouse_transparent_mode = 0;
char kbormouse = 0;
char mouse_isdirty = 0;
char detail_lvl = 0;
unsigned char g_is_busy = 0;
char mouse_buffer_count = 0;
char aKevin[] = "kevin";
char aOpp1[] = "opp1";
char aCarcoun[] = "carcoun";
/* Purpose: Coordinates startup, the main menu, and shutdown.
 * Parameters: argc, argv.
 * Returns: int.
 * Globals read: aKevin, buf_g_path, coursedataappend_address, cvxs_a, dialogarg2, endhsdemo,
 *            globalgamesettings, gmconfigbackup, is_audioloaded, main_data_file_addr,
 *            menutimeout, pass_check_flag, replay_file, td14tb, track_file
 * Globals written: coursedataappend_address, cvxs_a, def_fntadr, endhsdemo, fntndat,
 *            g_column_of_trkdata21_pth, g_td01_track_filecpy, g_td08d, g_tdreplay16buf,
 *            globalgamesettings, gmconfigbackup, hscore_trk11_ptr, kbormouse,
 *            main_data_file_addr, menutimeout, pass_check_flag, replay_file, road_trk,
 *            row_ctr_zs, savedptr_ms, td10checkptr, td13_replay_hdr, td14tb, td15p_9,
 *            td19hdl, td3, td6_ptr_b, td_18_ref, tdfrompathrow22, track04_plyraero,
 *            track_file, trackctrpos2, trackdat7, trackdata_05_opp_aerotbl,
 *            trackdata_penalty_related, trkd23adr, trkptrpath, z_ctr_pos
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

int main(int argc, char *argv[])
{
  register int index;
  char menuFlag;
  register int result;
  char far *trackBlock;
  initialize_main(argc, argv);
  initialize_div0();
  for (index = 0; index < 30; ++index)
  {
    lnoffsets[index] = 30 * (29 - index);
    gterrtrk[index] = 30 * index;
    r_zp[index] = 29 - index << 10;
    row_ctr_zs[index] = (29 - index << 10) + 0x200;
    postable[index] = index << 10;
    z_ctr_pos[index] = (index << 10) + 0x200;
  }
  for (index = 0; index < 30; ++index)
  {
    xcols[index] = index * 1024u;
    trackctrpos2[index] = index * 1024 + 512;
  }
  main_data_file_addr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("main");
  def_fntadr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(0, "fontdef.fnt");
  fntndat = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(0, "fontn.fnt");
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
  initialize_polyinfo();
  index = 0x6BF3;
  trackBlock = /* PLATFORM(memory): Game memory-manager API. */ mmgr_alloc_resbytes("trakdata", index);
  g_td01_track_filecpy = (short far *) trackBlock;
  trackBlock += 0x70A;
  trackdata_penalty_related = (short far *) trackBlock;
  trackBlock += 0x70A;
  td3 = trackBlock;
  trackBlock += 0x70A;
  track04_plyraero = (short far *) trackBlock;
  trackBlock += 0x80;
  trackdata_05_opp_aerotbl = (short far *) trackBlock;
  trackBlock += 0x80;
  td6_ptr_b = trackBlock;
  trackBlock += 0x80;
  trackdat7 = trackBlock;
  trackBlock += 0x80;
  g_td08d = (int far *) trackBlock;
  trackBlock += 0x60;
  trkptrpath = (int far *) trackBlock;
  trackBlock += 0x180;
  td10checkptr = (int far *) trackBlock;
  trackBlock += 0x120;
  hscore_trk11_ptr = (struct HighScoreRecord far *) trackBlock;
  trackBlock += 0x16C;
  savedptr_ms = trackBlock;
  trackBlock += 0xF0;
  td13_replay_hdr = trackBlock;
  trackBlock += 0x1A;
  td14tb = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  td15p_9 = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  g_tdreplay16buf = trackBlock;
  trackBlock += 0x2EE0;
  road_trk = trackBlock;
  trackBlock += 0x385;
  td_18_ref = trackBlock;
  trackBlock += 0x385;
  td19hdl = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  coursedataappend_address = trackBlock;
  trackBlock += 0x7AC;
  g_column_of_trkdata21_pth = trackBlock;
  trackBlock += 0x385;
  tdfrompathrow22 = trackBlock;
  trackBlock += 0x385;
  trkd23adr = (unsigned char far *) trackBlock;
  trackBlock += 0x30;
  initialize_unknown();
  initialize_kevin_random(aKevin);
  strcpy(globalgamesettings.game_trackname, "DEFAULT");
  index = 0;
  input_do_checking(1);
  input_do_checking(1);
  msdrawopaquechk();
  kbormouse = 0;
  pass_check_flag = 0;
  set_default_car();
  index = 1;
  goto do_intro;
  do
  {
  do_intro0:
    index = 0;
  do_intro:
    ensure_file_exists(2);
    if (index != 0)
    {
      /* PLATFORM(file): Legacy file and resource API. */ file_build_path(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path);
      /* PLATFORM(file): Legacy file and resource API. */ file_read_fatal(buf_g_path, td14tb);
    }
    menutimeout = 0;
    result = run_intro_looped();
    if (result == 27)
      continue;
  show_menu:
    ensure_file_exists(2);
    if (is_audioloaded == 0)
      /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ file_load_audio_resource("skidslct", "skidms", "SLCT");
    switch (run_menu())
    {
      case 3:
        run_tracks_menu(0);
        goto show_menu;
      case 2:
        /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
        show_waiting();
        run_opponent_menu();
        goto show_menu;
      case 4:
        /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
        show_waiting();
        if (run_option_menu() == 0)
          goto show_menu;
        menuFlag = 1;
        goto do_game;
      case 1:
        /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
        show_waiting();
        run_car_menu(globalgamesettings.game_playercarid, &globalgamesettings.game_playermaterial, &globalgamesettings.game_playertransmission, 0);
        goto show_menu;
      case 0:
        menuFlag = 0;
      do_game:
        gmconfigbackup = globalgamesettings;
        for (index = 0; index < 0x70A; ++index)
          coursedataappend_address[index] = td14tb[index];
        for (index = 0; index < 0x51; ++index)
        {
          coursedataappend_address[index + 0x70A] = track_file[index];
          coursedataappend_address[index + 0x75B] = replay_file[index];
        }
        if (menutimeout == 0)
        {
          if (track_setup() != 0)
          {
            run_tracks_menu(1);
            goto show_menu;
          }
          random_wait();
          if (pass_check_flag == 0)
            security_check((char)(get_super_random() % 20));
        }
        else if (/* PLATFORM(file): Legacy file and resource API. */ file_find("tedit.*") == 0)
          goto prepare_intro;
        else
          goto init_replay;
      init_replay:
        /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unload();
        cvxs_a = (struct GAMESTATE far *) /* PLATFORM(memory): Game memory-manager API. */ mmgr_alloc_resbytes("cvx", 22400);
        initialize_game_state(-1);
        if (menuFlag != 0)
          endhsdemo = 0;
        else
          globalgamesettings.game_recordedframes = 0;
        break;
      case -1:
      prepare_intro:
        /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unload();
        goto do_intro0;
      default:
        goto show_menu;
    }
    for (;;)
    {
      show_waiting();
      run_game();
      if (menutimeout == 0 && endhsdemo != 0)
      {
        switch (end_hiscore())
        {
          case 0:
            endhsdemo = 4;
            continue;
          case 1:
            globalgamesettings.game_recordedframes = 0;
            continue;
        }
      }
      break;
    }
    globalgamesettings = gmconfigbackup;
    for (index = 0; index < 0x70A; ++index)
      td14tb[index] = coursedataappend_address[index];
    for (index = 0; index < 0x51; ++index)
    {
      track_file[index] = coursedataappend_address[index + 0x70A];
      replay_file[index] = coursedataappend_address[index + 0x75B];
    }
    /* PLATFORM(memory): Game memory-manager API. */ mmgr_release(cvxs_a);
    if (menutimeout != 0)
      goto do_intro0;
    goto show_menu;
  } while (show_dialog(2, 1, locate_text_resource(main_data_file_addr, "dos"), 0xFFFF, 0xFFFF, dialogarg2, 0, 0) < 1);
  msdrawopaquechk();
  /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_stop_unknown();
  audiodrv_atexit();
  /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ keyboard_exit_handler();
  /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ keyboard_shift_checking1();
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ video_set_mode7();
}

/* Purpose: Runs the title and credits presentation sequence.
 * Parameters: none.
 * Returns: far.
 * Globals read: g_wndspr, loadedresourceptr
 * Globals written: g_wndspr, loadedresourceptr
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

int far run_intro_looped(void)
{
    register int inputResult;
    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ file_load_audio_resource("skidtitl", "skidms", "TITL");
    loadedresourceptr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(2, "sdtitl");
    g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
    inputResult = run_intro();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
    /* PLATFORM(memory): Game memory-manager API. */ mmgr_free(loadedresourceptr);
    if (inputResult == 0) {
        inputResult = setup_intro();
        if (inputResult == 0) {
            loadedresourceptr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(2, "sdcred");
            g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, 0);
            inputResult = load_intro_resources();
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
            /* PLATFORM(memory): Game memory-manager API. */ mmgr_free(loadedresourceptr);
        }
    }
    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unload();
    return inputResult;
}

/* Purpose: Presents the production and title screens.
 * Parameters: none.
 * Returns: far.
 * Globals read: g_wndspr, loadedresourceptr
 * Globals written: waitm_ms
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

int far run_intro(void)
{
    register int inputResult;
    msdrawopaquechk();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_2_to_1_clear();
    msdrawtransparentchk();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
    if (locate_shape_fatal(loadedresourceptr, "prod")->pos_y != 0)
        waitm_ms = 0xa0;
    else
        waitm_ms = 0xb4;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(locate_shape_fatal(loadedresourceptr, "prod"));
    inputResult = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, 0xffff);
    if (inputResult == 0) {
        inputResult = input_repeat_check(0x190);
        if (inputResult == 0) {
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
            waitm_ms = 0xb4;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(locate_shape_fatal(loadedresourceptr, "titl"));
            inputResult = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, 0xffff);
            if (inputResult == 0)
                inputResult = input_repeat_check(0x190);
        }
    }
    return inputResult;
}

/* Purpose: Loads and animates the intro presentation resources.
 * Parameters: none.
 * Returns: far.
 * Globals read: g_wndspr, intro_text_style_b, intro_text_style_d, intro_text_style_f,
 *            loadedresourceptr, pixel_scales, resbuftext
 * Globals written: waitm_ms
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

signed char far load_intro_resources(void)
{
  short imageWidth;
  short waitLimit;
  char far *resources[12];
  register int elapsed;
  short scaledWidth;
  register int picture;
  char far *introFile;
  short step;
  short timerStep;
  int inputResult;
  short displayHeight;
  introFile = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("cred");
  locate_many_resources(loadedresourceptr, "arowarrwarw1arw2arw3arw4arw5arw6arw7arw8type", resources);
  waitm_ms = 0x96;
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
  imageWidth = ((struct SHAPE2D far *) resources[1])->pos_x;
  waitLimit = ((struct SHAPE2D far *) resources[1])->pos_y;
  scaledWidth = ((struct SHAPE2D far *) resources[1])->width * pixel_scales;
  displayHeight = ((struct SHAPE2D far *) resources[1])->height;
  copy_string(resbuftext, locate_text_resource(introFile, "cre"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x78, 0, intro_text_color_b, intro_text_style_b);
  copy_string(resbuftext, locate_shape_alt(introFile, "gds0"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x3c, 0x0c, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gds1"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x68, 0x14, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_text_resource(introFile, "des"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x20, intro_text_color_c, intro_text_style_c);
  copy_string(resbuftext, locate_shape_alt(introFile, "gdon"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x2c, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gkev"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x34, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gbra"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x3c, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "grob"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x44, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gsta"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x4c, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_text_resource(introFile, "mus"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x5c, intro_text_color_f, intro_text_style_f);
  copy_string(resbuftext, locate_shape_alt(introFile, "gmsy"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x68, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gkri"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x70, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gbri"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0x14, 0x78, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_text_resource(introFile, "pro"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x20, intro_text_color_d, intro_text_style_d);
  copy_string(resbuftext, locate_shape_alt(introFile, "gkev"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x2c, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_text_resource(introFile, "opr"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x38, intro_text_color_d, intro_text_style_d);
  copy_string(resbuftext, locate_shape_alt(introFile, "gbra"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x40, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gric"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x48, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_text_resource(introFile, "art"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x54, intro_text_color_e, intro_text_style_e);
  copy_string(resbuftext, locate_shape_alt(introFile, "gmsm"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x60, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gdav"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x68, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gnic"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x70, intro_text_color_a, intro_text_style_a);
  copy_string(resbuftext, locate_shape_alt(introFile, "gkev"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, 0xac, 0x78, intro_text_color_a, intro_text_style_a);
  unload_resource(introFile);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, 0xffff);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
  /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
  elapsed = 0x14a;
  for (;;)
  {
    timerStep = /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
    elapsed -= timerStep << 1;
    if (imageWidth > elapsed)
      break;
    msdrawopaquechk();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_and_alt(resources[1], elapsed, waitLimit);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite1_unknown2(scaledWidth + elapsed, waitLimit, 0x20, displayHeight, 0);
    msdrawtransparentchk();
    inputResult = input_do_checking(timerStep);
    if (inputResult != 0)
      break;
  }
  waitLimit = ((struct SHAPE2D far *) resources[0])->pos_y;
  step = 0;
  elapsed = 0;
  for (picture = 2; picture < 10; ++picture)
  {
    if (inputResult != 0)
      break;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, waitLimit, 0xc8);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(0);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(resources[picture]);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, waitLimit, 0xc8);
    msdrawopaquechk();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprputimage(((struct SPRITE far *) g_wndspr)->image);
    msdrawtransparentchk();
    step += 5;
    while (step > elapsed)
    {
      timerStep = /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
      inputResult = input_do_checking(timerStep);
      elapsed += timerStep;
    }
  }
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, 0, 0xc8);
  msdrawopaquechk();
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_shape(((struct SPRITE far *) g_wndspr)->image);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, waitLimit, 0xc8);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(0);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(resources[0]);
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(resources[10]);
  inputResult = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, 0);
  if (inputResult != 0 || input_repeat_check(0x1f4) != 0)
    return 1;
  return 0;
}

signed char menu_left[6] = { 1, 2, 4, 0, 3, 0 };
signed char menu_right[6] = { 3, 0, 1, 4, 2, 0 };
int menu_buttons_x1[5] = { 105, 66, 5, 190, 255 };
int menu_buttons_x2[5] = { 208, 107, 67, 253, 312 };
int menu_buttons_y1[5] = { 119, 77, 114, 76, 116 };
int menu_buttons_y2[5] = { 197, 120, 170, 122, 166 };
/* Purpose: Draws and processes the main menu.
 * Parameters: none.
 * Returns: far.
 * Globals read: g_wndspr, menu_buttons_x1, menu_buttons_x2, menu_buttons_y1, menu_buttons_y2,
 *            menu_hover_color_a, menu_hover_color_b, menu_right, menutimeout, unused_count
 * Globals written: g_wndspr, menutimeout, unused_count, waitm_ms
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(input_mouse): Legacy mouse input for menu interaction.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

char far run_menu(void)
{
    void far *data_resource;
    int mouseDelta;
    signed char drawMode;
    int keyCode;
    signed char hit;
    signed char oldSelection;
    signed char selection;

    drawMode = (signed char)-1;
    selection = 0;
    oldSelection = (signed char)-1;
    show_waiting();
    waitm_ms = 180;
    g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(320, 200, 15);

    data_resource = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(2, "sdmsel");
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(locate_shape_fatal((char far *)data_resource, "scrn"));
    /* PLATFORM(memory): Game memory-manager API. */ mmgr_free(data_resource);

    for (;;) {
        if (selection != oldSelection) {
            oldSelection = selection;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, drawMode);
            drawMode = (signed char)-2;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
            reset_idle_counters();
        }

        mouseDelta = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(selection,
            menu_buttons_x1, menu_buttons_x2,
            menu_buttons_y1, menu_buttons_y2, menu_hover_color_a, menu_hover_color_b);
        keyCode = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(mouseDelta);
        hit = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(5, menu_buttons_x1, menu_buttons_x2,
                                  menu_buttons_y1, menu_buttons_y2);
        if (hit != (signed char)-1)
            selection = hit;

        unused_count += mouseDelta;
        if (unused_count > 6000) {
            unused_count = 0;
            ++menutimeout;
        }
        if (menutimeout != 0) {
            selection = 0;
            keyCode = 13;
        }

        if (keyCode == 0)
            continue;
        switch (keyCode) {
        case 0x4b00:
            selection = menu_left[selection];
            break;
        case 0x4d00:
            selection = menu_right[selection];
            break;
        case 27:
            selection = (signed char)-1;
            goto menu_done;
        case 13:
        case 32:
            goto menu_done;
        }
    }

menu_done:
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
    return selection;
}

short trackmenu_buttons_x1[3] = { 16, 112, 208 };
short trackmenu_buttons_x2[3] = { 112, 208, 304 };
short trackmenu_buttons_y1[3] = { 171, 171, 171 };
short trackmenu_buttons_y2[3] = { 197, 197, 197 };
/* Purpose: Presents the track list and handles track selection.
 * Parameters: restart.
 * Returns: far.
 * Globals read: buf_g_path, dlg_colour, fntndat, g_wndspr, globalgamesettings, ground_skybox,
 *            hscore_trk11_ptr, main_data_file_addr, menu_button_color_a,
 *            menu_button_color_b, menu_button_color_c, menu_hover_color_a,
 *            menu_hover_color_b, menutimeout, resbuftext, scrorder_idxs, td14tb,
 *            track_file, trackmenu_buttons_x2, trackmenu_buttons_y1, trackmenu_buttons_y2,
 *            unused_count
 * Globals written: g_wndspr, menutimeout, unused_count, waitm_ms
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(input_mouse): Legacy mouse input for menu interaction.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far run_tracks_menu(int restart)
{
    char trackSelection;
    char lastSelection;
    register int dialogRes;
    int keyCodePressed;
    int timerDiff;
    char choice;
    char displayState;
    char offsets[4];
    void far *trkEdit;

    ensure_file_exists(3);
    if (restart != 0)
        goto restart_game;

preview:
    displayState = -1;
    trackSelection = 0;
    lastSelection = -1;
    show_waiting();
    waitm_ms = 0x9b;
    g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
    load_skybox((unsigned char)td14tb[0x384]);
    shape3d_load_all();
    set_projection(0x28, 0x28, 0x140, 0xc8);
    initialize_game_state(-2);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(ground_skybox);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, 0, 0xc8);
    draw_track_preview();
    shape3d_free_all();
    unload_skybox();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    strcpy(resbuftext, "'");
    strcat(resbuftext, globalgamesettings.game_trackname);
    strcat(resbuftext, "'");
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), 6,
                        dlg_colour, 0);

    if (highscore_write_a(0) == 0) {
        if (hscore_trk11_ptr[scrorder_idxs[0]].marker != 0xffff) {
        copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs0"));
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), 0x12,
                        dlg_colour, 0);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
        print_highscore_entry(0, offsets);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_setup_unknown(0, 0);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[0], 16, 30);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[1], 120, 30);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[2], 224, 30);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[3], 272, 30);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
        }
    }

    trkEdit = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("tedit");
    draw_button(locate_text_resource(trkEdit, "bmt"), 0x11, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    draw_button(locate_text_resource(trkEdit, "bet"), 0x71, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    draw_button(locate_text_resource(trkEdit, "bmm"), 0xd1, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    unload_resource(trkEdit);

menu_loop:
    if (trackSelection != lastSelection) {
        lastSelection = trackSelection;
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, displayState);
        displayState = -2;
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
        reset_idle_counters();
    }

    timerDiff = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(trackSelection,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2,
        menu_hover_color_a, menu_hover_color_b);
    unused_count += timerDiff;
    if (unused_count > 0x1770) {
        unused_count = 0;
        ++menutimeout;
    }
    keyCodePressed = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(timerDiff);
    choice = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(3,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2);
    if (choice != -1)
        trackSelection = choice;
    if (menutimeout != 0) {
        trackSelection = 2;
        keyCodePressed = 0x0d;
    }
    if (keyCodePressed == 0)
        goto menu_loop;
    switch (keyCodePressed) {
    case 0x4b00:
        if (trackSelection != 0)
            --trackSelection;
        else
            trackSelection = 2;
        goto menu_loop;
    case 0x4d00:
        if (trackSelection < 2)
            ++trackSelection;
        else
            trackSelection = 0;
        goto menu_loop;
    case 0x1b:
        trackSelection = -1;
    case 0x0d:
    case 0x20:
        break;
    default:
        goto menu_loop;
    }
    switch (trackSelection) {
    default:
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
        return;
    case 0:
        dialogRes = do_fileselect_dialog(track_file, globalgamesettings.game_trackname,
                                         ".trk",
                                         locate_text_resource(main_data_file_addr, "trk"));
        /* PLATFORM(file): Legacy file and resource API. */ file_build_path(track_file, globalgamesettings.game_trackname,
                        ".trk", buf_g_path);
        if (dialogRes != 0) {
            /* PLATFORM(file): Legacy file and resource API. */ file_read_fatal(buf_g_path, td14tb);
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
            goto preview;
        }
        lastSelection = -1;
        goto menu_loop;
    case 1:
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
    restart_game:
        /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
        show_waiting();
        waitm_ms = 0x82;
        track_setup();
        load_tracks_menu_shapes();
        goto preview;
    }
}

/* Purpose: Loads or creates the track-specific high-score table.
 * Parameters: create_defaults.
 * Returns: far.
 * Globals read: buf_g_path, globalgamesettings, hscore_trk11_ptr, track_file
 * Globals written: entry_score_index, g_is_busy, hscore_trk11_ptr, scrorder_idxs
 * PLATFORM(file): Legacy file and resource API.
 */

char far highscore_write_a(int create_defaults)
{
  void far *loaded_data;
  int index;
  struct HighScoreRecord row;
  entry_score_index = (unsigned char) (-1);
  for (index = 0; index < 7; ++index)
    scrorder_idxs[index] = index;

  /* PLATFORM(file): Legacy file and resource API. */ file_build_path(track_file, globalgamesettings.game_trackname, ".hig", buf_g_path);
  if (create_defaults == 0)
  {
    g_is_busy = 1;
    loaded_data = /* PLATFORM(file): Legacy file and resource API. */ read_file_with_retry(10, buf_g_path, hscore_trk11_ptr);
    g_is_busy = 0;
    if (loaded_data == 0)
    {
    score_file_missing:
      return 1;
    }
  score_file_found:
    return 0;
  }
  strcpy((char *) row.bytes, "....................");
  strcpy(((char *) row.bytes) + 17, ".......................");
  row.bytes[41] = 0;
  strcpy(((char *) row.bytes) + 42, "../....");
  row.marker = 0xffff;
  for (index = 0; index < 7; ++index)
    hscore_trk11_ptr[index] = row;
  index = /* PLATFORM(file): Legacy file and resource API. */ file_write_fatal(buf_g_path, hscore_trk11_ptr, 0x16C);
  if (index != 0)
    goto score_file_missing;
  goto score_file_found;
}

/* Purpose: Draws the high-score list.
 * Parameters: none.
 * Returns: far.
 * Globals read: dialogarg2, dlg_colour, entry_score_index, fntndat, globalgamesettings,
 *            main_data_file_addr, resbuftext
 * Globals written: none detected
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far highscore_text_unknown(void)
{
    int row_color;
    int row_top;
    char offsets[4];
    char row_index;

    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();

    copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs1"));
    strcat(resbuftext, " '");
    strcat(resbuftext, globalgamesettings.game_trackname);
    strcat(resbuftext, "'");
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext),
                      5, dlg_colour, 0);

    copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs2"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, 16, 15, dlg_colour, 0);
    copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs3"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, 120, 15, dlg_colour, 0);
    copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs5"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, 224, 15, dlg_colour, 0);
    copy_string(resbuftext, locate_text_resource(main_data_file_addr, "hs4"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, 272, 15, dlg_colour, 0);

    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
    row_index = 0;
    goto row_check;
row_normal:
    row_color = 0;
row_draw:
    row_top = row_index * 10 + 25;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_setup_unknown(row_color, 0);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[0], 16, row_top);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[1], 120, row_top);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[2], 224, row_top);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext + offsets[3], 272, row_top);
    ++row_index;
row_check:
    if (row_index >= 7)
        goto row_done;
    print_highscore_entry(row_index, offsets);
    if (row_index != entry_score_index)
        goto row_normal;
    row_color = dialogarg2;
    goto row_draw;
row_done:
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
}

/* Purpose: Draws one formatted high-score entry.
 * Parameters: rowIndex, stringOffsets.
 * Returns: far.
 * Globals read: hscore_trk11_ptr, rate_frame, resbuftext, scrorder_idxs
 * Globals written: rate_frame, resbuftext
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void far print_highscore_entry(int rowIndex, char *stringOffsets)
{
    char timeText[18];
    char textLength;
    int frameRate;
    struct HighScoreRecord scoreRecord;

    scoreRecord = hscore_trk11_ptr[scrorder_idxs[rowIndex]];
    stringOffsets[0] = 0;
    strcpy(resbuftext, (char *)scoreRecord.bytes);
    textLength = strlen(resbuftext) + 1;
    stringOffsets[1] = textLength;
    strcpy(resbuftext + textLength, (char *)scoreRecord.bytes + 17);
    textLength += strlen(resbuftext + textLength) + 1;
    stringOffsets[2] = textLength;
    resbuftext[textLength] = 0;
    if (scoreRecord.bytes[41] == 1)
        strcat(resbuftext + textLength, "(");
    strcat(resbuftext + textLength, (char *)scoreRecord.bytes + 42);
    if (scoreRecord.bytes[41] == 1)
        strcat(resbuftext + textLength, ")");
    textLength += strlen(resbuftext + textLength) + 1;
    frameRate = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
    /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame = 20;
    if (scoreRecord.marker != 0xffff)
        fmtframestr(timeText, scoreRecord.marker, 1);
    else
        fmtframestr(timeText, 0, 1);
    stringOffsets[3] = textLength;
    strcpy(resbuftext + textLength, timeText);
    /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame = frameRate;
}

/* Purpose: Inserts and presents a qualifying high score.
 * Parameters: score, text, carStyle.
 * Returns: far.
 * Globals read: dialogarg2, g_gsnashape_data, g_wndspr, globalgamesettings, hscore_trk11_ptr,
 *            opptext_label, rate_frame, results_entryname, textstr
 * Globals written: entry_score_index, hscore_trk11_ptr, scrorder_idxs
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far enter_hiscore(unsigned short score, char far *text, unsigned char carStyle)
{
    char selectedIndex;
    char insertionIndex;
    struct HighScoreRecord current;
    int dialogX;
    int dialogY;

    if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
        score <<= 1;

    if (score < hscore_trk11_ptr[6].marker) {
        for (insertionIndex = 0; hscore_trk11_ptr[insertionIndex].marker <= score && insertionIndex < 7; ++insertionIndex)
            scrorder_idxs[insertionIndex] = insertionIndex;
        selectedIndex = insertionIndex;
        entry_score_index = insertionIndex;
        goto secondary_check;
secondary_body:
        scrorder_idxs[insertionIndex + 1] = insertionIndex;
        ++insertionIndex;
secondary_check:
        if (insertionIndex < 6)
            goto secondary_body;
        scrorder_idxs[selectedIndex] = 6;

        current.marker = score;
        current.bytes[0] = 0;
        strcpy((char *)current.bytes + 17, textstr);
        current.bytes[41] = carStyle;
        if (globalgamesettings.game_opponenttype != 0) {
            strcpy((char *)current.bytes + 42, opptext_label);
            current.bytes[44] = '/';
            strcpy((char *)current.bytes + 45, g_gsnashape_data);
        } else {
            strcpy((char *)current.bytes + 42, " ");
        }
        hscore_trk11_ptr[6] = current;

        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
        highscore_text_unknown();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, -1);
        show_dialog(3, 0, text, -1, -1,
                    dialogarg2, &dialogY, 0);
        /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
        call_read_line(results_entryname, 16, dialogY, dialogX, 30000);
        strcpy((char *)current.bytes, results_entryname);
        hscore_trk11_ptr[6] = current;

        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
        highscore_text_unknown();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, -1);
        highscore_write_b();
    }

    highscore_text_unknown();
}

/* Purpose: Writes the ordered high-score table.
 * Parameters: none.
 * Returns: far.
 * Globals read: buf_g_path, globalgamesettings, hscore_trk11_ptr, scrorder_idxs, track_file
 * Globals written: g_is_busy
 * PLATFORM(file): Legacy file and resource API.
 */

void far highscore_write_b(void)
{
    int index;
    struct HighScoreRecord orderedScores[7];

    for (index = 0; index < 7; ++index)
        orderedScores[index] = hscore_trk11_ptr[scrorder_idxs[index]];

    /* PLATFORM(file): Legacy file and resource API. */ file_build_path(track_file, globalgamesettings.game_trackname, ".hig", buf_g_path);
    g_is_busy = 1;
    /* PLATFORM(file): Legacy file and resource API. */ file_write_fatal(buf_g_path, orderedScores, 0x16cUL);
    g_is_busy = 0;
}

struct RECTANGLE carmenu_cliprect = { 0, 320, 0, 95 };
int carmenu_buttons_y1[5] = { 229, 229, 229, 229, 229 };
int carmenu_buttons_y2[5] = { 316, 316, 316, 316, 316 };
int carmenu_buttons_x1[5] = { 107, 125, 143, 161, 179 };
int carmenu_buttons_x2[5] = { 124, 142, 160, 178, 196 };
char aLnam[6] = "lnam";
struct RECTANGLE rectangle_unknown16 = { 0, 320, 0, 0 };
struct VECTOR car_menu_car_position = { 0, -840, 2880 };
/* Purpose: Presents car choices and updates the selected car settings.
 * Parameters: not listed.
 * Returns: far.
 * Globals read: aCarcoun, car_menu_car_position, carmenu_buttons_x1, carmenu_buttons_x2,
 *            carmenu_buttons_y1, carmenu_buttons_y2, clipunk, core, dlg_colour, fntndat,
 *            fontdefvalue, g_miscfile_ptr, g_opp_resources, g_shapes3d, g_vid_flg2_set,
 *            g_videoflg5, g_wndspr, globalgamesettings, menu_button_color_a,
 *            menu_button_color_b, menu_button_color_c, menu_hover_color_a,
 *            menu_hover_color_b, menutimeout, performGraphColor, rate_frame,
 *            rectangle_unknown16, resbuftext, resource_ptropp, simdp7,
 *            slow_video_mode_state, statemgmtcpy, unused_count, vidflg3is_minus1
 * Globals written: aCarcoun, backlightovr8, core, g_miscfile_ptr, g_wndspr, menutimeout,
 *            rate_frame, rectangle_unknown16, resbuftext, statemgmtcpy, unused_count,
 *            waitm_ms
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(input_mouse): Legacy mouse input for menu interaction.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far run_car_menu(char *caridptr, char *materialofs,
                      char *transmissionofs, int opponenttype)
{
  void far *sdcselPos;
  struct TRANSFORMEDSHAPE3D transformedCopy;
  int rotation;
  char textch;
  struct RECTANGLE unionRectY;
  int oldFramesPerSecond;
  struct RECTANGLE localClipId;
  int carSpeedY;
  char runStateNew;
  void far *carres;
  char far *buttonText;
  char previousCarIndexCar;
  int rotationDeltaY;
  int graphIndexY;
  unsigned int graphYX;
  char carCountLast;
  unsigned int innerIndexOld;
  struct SPRITE far *opponentWindowX;
  char blitColorY;
  char carids[32][5];
  char carIndexY;
  int anglePos;
  char hitButtonX;
  char *findfileX;
  char previousButton;
  char far *descriptionCar;
  struct RECTANGLE fullClip;
  char selectedButtonY;
  void far *opponentShapeCopy;
  char canLeave;
  register int keyCode;
  transformedCopy.pos = car_menu_car_position;
  transformedCopy.shapeptr = &g_shapes3d[124];
  transformedCopy.rotvec.x = 0;
  transformedCopy.rotvec.y = 0;
  transformedCopy.unk = 0x7530;
  statemgmtcpy = slow_video_mode_state;
  if (slow_video_mode_state != 0)
  {
    transformedCopy.rectptr = &localClipId;
    transformedCopy.ts_flags = 8;
  }
  else
  {
    transformedCopy.ts_flags = 0;
  }
  ensure_file_exists(2);
  findfileX = /* PLATFORM(file): Legacy file and resource API. */ file_combine_and_find(0, "car*", ".res");
  if (findfileX == 0)
  {
    nullsub_1();
    return;
  }
  carids[0][0] = findfileX[3];
  carids[0][1] = findfileX[4];
  carids[0][2] = findfileX[5];
  carids[0][3] = findfileX[6];
  carids[0][4] = 0;
  carCountLast = 1;
  do
  {
    findfileX = /* PLATFORM(file): Legacy file and resource API. */ file_find_next_alt();
    if (findfileX == 0)
      break;
    else
    {
      carids[carCountLast][0] = findfileX[3];
      carids[carCountLast][1] = findfileX[4];
      carids[carCountLast][2] = findfileX[5];
      carids[carCountLast][3] = findfileX[6];
    }
    carids[carCountLast][4] = 0;
    ++carCountLast;
  }
  while (carCountLast != 32);
  nullsub_1();
  if (carCountLast > 1)
  {
    for (graphIndexY = 0; graphIndexY < carCountLast - 1; ++graphIndexY)
    {
      for (innerIndexOld = graphIndexY + 1; innerIndexOld < carCountLast; ++innerIndexOld)
      {
        if (strcmp(carids[graphIndexY], carids[innerIndexOld]) > 0)
        {
          strcpy(resbuftext, carids[graphIndexY]);
          strcpy(carids[graphIndexY], carids[innerIndexOld]);
          strcpy(carids[innerIndexOld], resbuftext);
        }
      }

    }

  }
  carIndexY = 0;
  for (hitButtonX = 0; hitButtonX < carCountLast; ++hitButtonX)
  {
    if (carids[hitButtonX][0] == caridptr[0] && carids[hitButtonX][1] == caridptr[1] && carids[hitButtonX][2] == caridptr[2] && carids[hitButtonX][3] == caridptr[3])
    {
      carIndexY = hitButtonX;
      break;
    }
  }

  waitm_ms = 0x5a;
  blitColorY = (char) 0xff;
  backlightovr8 = 0x2d;
  sdcselPos = /* PLATFORM(file): Legacy file and resource API. */ file_load_shape2d_fatal_thunk("sdcsel");
  if (opponenttype == 0)
    g_miscfile_ptr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("misc");
  if (opponenttype != 0)
  {
    rectangle_unknown16.right = 0xf0;
    if (g_videoflg5 != 0)
    {
      opponentShapeCopy = g_opp_resources[opponenttype];
      opponentWindowX = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(((struct OPPONENTIMAGE far *) opponentShapeCopy)->width, ((struct OPPONENTIMAGE far *) opponentShapeCopy)->height, 0x0f);
      setup_mcgawnd2();
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(0);
      nullsub_2(resource_ptropp, (char) (opponenttype + '0'));
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_transparent(g_opp_resources[opponenttype], 0, 0);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_shape_alt(opponentWindowX->image, 0, 0);
    }
  }
  else
  {
    rectangle_unknown16.right = 0x140;
  }
  previousCarIndexCar = (char) 0xff;
  rotation = 0;
  selectedButtonY = 0;
  reset_idle_counters();
  rotationDeltaY = 0;
  previousButton = (char) 0xff;
  set_projection(0x24, 0x11, 0x140, 0x64);
  /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
  g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
  for (;;)
  {
    menu_loop:
    if (carIndexY != previousCarIndexCar)
    {
      if (previousCarIndexCar != ((char) 0xff))
      {
        unload_resource(carres);
        shape3d_free_car_shapes();
      }
      shape3d_load_car_shapes(carids[carIndexY], globalgamesettings.game_opponentcarid);
      aCarcoun[3] = carids[carIndexY][0];
      aCarcoun[4] = carids[carIndexY][1];
      aCarcoun[5] = carids[carIndexY][2];
      aCarcoun[6] = carids[carIndexY][3];
      carres = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file(aCarcoun);
      setup_aero_trackdata(carres, 0);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
      draw_button(0, 0, 0x67, 0x140, 0x61, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw_button(0, 5, 0x6d, 0x46, 0x55, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw_button(0, 0x52, 0x6d, 0x8c, 0x55, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_shape_to_1_alt(locate_shape_fatal(sdcselPos, "grap"));
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_setup_unknown(0, dlg_colour);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text("150", 9, 0x73);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text("100", 9, 0x87);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(" 50", 9, 0x9b);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text("  0", 9, 0xaf);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text("0  20  40", 0x1a, 0xb9);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
      draw_button(locate_text_resource(g_miscfile_ptr, "bdo"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[0] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw_button(locate_text_resource(g_miscfile_ptr, "bnx"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[1] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw_button(locate_text_resource(g_miscfile_ptr, "bla"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[2] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      buttonText = locate_text_resource(g_miscfile_ptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
      draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw_button(locate_text_resource(g_miscfile_ptr, "bco"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[4] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      oldFramesPerSecond = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
      /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame = 20;
      initialize_game_state(-2);
      core.playerstate.car_transmission = 1;
      graphIndexY = 0;
      for (;;)
      {
        update_car_speed(1, 0, &core.playerstate, &simdp7);
        carSpeedY = core.playerstate.car_speed >> 8;
        if ((graphYX = -((int) ((((unsigned long) carSpeedY) << 6) / 150 - 0xb5))) >= 0x75)
        {
          innerIndexOld = ((unsigned int) (0x26 * graphIndexY)) / 0x320 + 0x1c;
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ putpixel_single_maybe(innerIndexOld, graphYX, performGraphColor);
          ++graphIndexY;
          if (graphIndexY < 0x320)
            continue;
        }
        break;
      }

      /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame = oldFramesPerSecond;
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
      descriptionCar = locate_text_resource(carres, "des");
      innerIndexOld = 0;
      graphYX = 0x74;
      do
      {
        textch = *(descriptionCar++);
        if (textch == ']')
        {
          if (innerIndexOld != 0)
          {
            resbuftext[innerIndexOld] = '\0';
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, 0x58, graphYX);
          }
          innerIndexOld = 0;
          graphYX += fontdefvalue;
        }
        else
        {
          resbuftext[innerIndexOld++] = textch;
        }
      }
      while ((*descriptionCar) != '\0');
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
      /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
      previousButton = (char) 0xff;
      fullClip.left = 0;
      fullClip.right = 0x140;
      fullClip.top = 0;
      fullClip.bottom = 0xc8;
      canLeave = 0;
      runStateNew = 3;
    }

    rotation += rotationDeltaY;
    switch (runStateNew)
    {
      case 0:
      case 3:
        anglePos = polang(car_menu_car_position.y, car_menu_car_position.z);
        if (statemgmtcpy != 0)
          localClipId = clipunk;
        else
          localClipId = carmenu_cliprect;
        select_rot(0, anglePos, 0, &carmenu_cliprect, 0);
        if (*materialofs >= (char)g_shapes3d[124].shape3d_numpaints)
          *materialofs = 0;
        transformedCopy.rotvec.z = rotation;
        transformedCopy.material = *materialofs;
        trans_op(&transformedCopy);
        if (carIndexY == previousCarIndexCar)
          rectangle_unknown16.bottom = 0x5f;
        else
          rectangle_unknown16.bottom = 0xc8;
        rcintersect(&localClipId, &rectangle_unknown16);
        rcunion(&localClipId, &fullClip, &unionRectY);
        if (runStateNew == 3)
          goto draw_full_car;
        runStateNew = 1;
        break;
      case 1:
      draw_full_car:
        runStateNew = 0;
        canLeave = 1;
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprputimage(locate_shape_fatal(sdcselPos, "stop"));
        polyinfo();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        fullClip = localClipId;
        if (opponenttype != 0 && carIndexY != previousCarIndexCar)
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
          if (g_videoflg5 == 0)
          {
            nullsub_2(resource_ptropp, (char) (opponenttype + '0'));
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_transparent(g_opp_resources[opponenttype], 0xf0, 0);
          }
          else
          {
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_and_alt(opponentWindowX->image, 0xf0, 0);
          }
        }
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        msdrawopaquechk();
        if (blitColorY != ((char) 0xfe))
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, blitColorY);
          blitColorY = (char) 0xfe;
        }
        else
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprputimage(g_wndspr->image);
        }
        msdrawtransparentchk();
        previousCarIndexCar = carIndexY;
        break;
    }
    if (selectedButtonY != previousButton)
    {
      if (previousButton != ((char) 0xff))
      {
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(carmenu_buttons_y1[0], carmenu_buttons_y2[0] + g_vid_flg2_set & vidflg3is_minus1, carmenu_buttons_x1[0], carmenu_buttons_x2[4] + 1);
        msdrawopaquechk();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprputimage(g_wndspr->image);
        msdrawtransparentchk();
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
      }
      reset_idle_counters();
      previousButton = selectedButtonY;
    }

    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
    rotationDeltaY = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(selectedButtonY, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2, menu_hover_color_a, menu_hover_color_b);
    unused_count += rotationDeltaY;
    if (unused_count > 0x2ee0)
    {
      unused_count = 0;
      ++menutimeout;
    }
    keyCode = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(rotationDeltaY);
    hitButtonX = (char) /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(5, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2);
    if (hitButtonX != ((char) 0xff))
      selectedButtonY = hitButtonX;
    if (menutimeout != 0)
    {
      selectedButtonY = 0;
      keyCode = 0x0d;
    }
    if (keyCode != 0)
    {
      switch (keyCode)
      {
        case 0x0d:

        case 0x1b:

        case 0x20:
          switch (selectedButtonY)
        {
          case 0:
            if (canLeave == 0)
            goto menu_loop;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
            unload_resource(carres);
            shape3d_free_car_shapes();
            if (opponenttype != 0 && g_videoflg5 != 0)
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(opponentWindowX);
            if (opponenttype == 0)
            unload_resource(g_miscfile_ptr);
            /* PLATFORM(memory): Game memory-manager API. */ mmgr_free(sdcselPos);
            msdrawopaquechk();
            caridptr[0] = carids[carIndexY][0];
            caridptr[1] = carids[carIndexY][1];
            caridptr[2] = carids[carIndexY][2];
            caridptr[3] = carids[carIndexY][3];
            menutimeout = 0;
            return;

          case 1:
            ++carIndexY;
            if (carIndexY == carCountLast)
            carIndexY = 0;
            goto menu_loop;

          case 2:
            --carIndexY;
            if (carIndexY < 0)
            carIndexY = carCountLast - 1;
            goto menu_loop;

          case 3:
            *transmissionofs ^= 1;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
            buttonText = locate_text_resource(g_miscfile_ptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
            draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
            msdrawopaquechk();
            draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            msdrawtransparentchk();
            goto menu_loop;

          case 4:
            ++(*materialofs);
            runStateNew = 3;
            goto menu_loop;

          default:
            goto menu_loop;

        }

          break;

        case 0x4800:
          if (selectedButtonY != 0)
          --selectedButtonY;
        else
          selectedButtonY = 4;
          break;

        case 0x5000:
          if (selectedButtonY < 4)
          ++selectedButtonY;
        else
          selectedButtonY = 0;
          break;


      }

    }
  }

}

int opponentmenu_buttons_x1[5] = { 20, 76, 132, 188, 244 };
int opponentmenu_buttons_x2[5] = { 76, 132, 188, 244, 300 };
int opponentmenu_buttons_y1[5] = { 177, 177, 177, 177, 177 };
int opponentmenu_buttons_y2[5] = { 197, 197, 197, 197, 197 };
/* Purpose: Presents and processes the opponent selection menu.
 * Parameters: none.
 * Returns: far.
 * Globals read: aOpp1, dlg_colour, fntndat, fontdefvalue, g_miscfile_ptr, g_opp_resources,
 *            g_videoflg5, g_wndspr, globalgamesettings, menu_button_color_a,
 *            menu_button_color_b, menu_button_color_c, menu_hover_color_a,
 *            menu_hover_color_b, opponentmenu_buttons_x2, opponentmenu_buttons_y1,
 *            opponentmenu_buttons_y2, resbuftext, resource_ptropp
 * Globals written: aOpp1, g_miscfile_ptr, g_wndspr, globalgamesettings, resbuftext,
 *            resource_ptropp
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(input_mouse): Legacy mouse input for menu interaction.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far run_opponent_menu(void)
{
    char textch;
    char lastColor;
    char isLoaded;
    int textpos;
    register int key;
    int timeDelta;
    int y;
    void far *oppRes;
    char lastSelection;
    char button;
    char far *resourceText;
    char selectionIndex;
    char previousType;

    ensure_file_exists(4);
    g_miscfile_ptr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("misc");
    resource_ptropp = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(8, "sdosel");
    locate_many_resources(resource_ptropp, "opp0opp1opp2opp3opp4opp5opp6", g_opp_resources);

    selectionIndex = 0;
    isLoaded = 0;
    previousType = (char)0xff;
    lastColor = (char)0xff;
    reset_idle_counters();
menu_opponent_mouse_redraw:
    msdrawtransparentchk();

    for (;;) {
        if (previousType != globalgamesettings.game_opponenttype) {
            if (previousType != (char)0xff) {
                /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
                if (isLoaded != 0)
                    unload_resource(oppRes);
            }

            ensure_file_exists(4);
            if (globalgamesettings.game_opponenttype != 0) {
                aOpp1[3] = (char)(globalgamesettings.game_opponenttype + '0');
                oppRes = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file(aOpp1);
                isLoaded = 1;
            } else {
                isLoaded = 0;
            }

            g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
            previousType = globalgamesettings.game_opponenttype;
            lastSelection = (char)0xff;
            if (g_videoflg5 == 0)
                /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
            else
                setup_mcgawnd2();
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(0);

            nullsub_2(resource_ptropp, 0x37);
            release_shape_resources(locate_shape_fatal(resource_ptropp, "scrn"));

            draw_button(locate_text_resource(g_miscfile_ptr, "bla"), 0x15,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw_button(locate_text_resource(g_miscfile_ptr, "bnx"), 0x4d,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw_button(locate_text_resource(g_miscfile_ptr, "bcl"), 0x85,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw_button(locate_text_resource(g_miscfile_ptr, "bca"), 0xbd,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw_button(locate_text_resource(g_miscfile_ptr, "bdo"), 0xf5,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);

            nullsub_2(resource_ptropp, (char)(globalgamesettings.game_opponenttype + '0'));
            release_shape_resources(g_opp_resources[globalgamesettings.game_opponenttype]);
            nullsub_2(resource_ptropp, 0x37);
            release_shape_resources(locate_shape_fatal(resource_ptropp, "clip"));

            if (g_videoflg5 != 0) {
                /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_shape_alt(g_wndspr->image, 0, 0);
                /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
            }

            if (globalgamesettings.game_opponenttype != 0)
                resourceText = locate_text_resource(oppRes, "des");
            else
                resourceText = locate_text_resource(g_miscfile_ptr, "rac");
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_setup_unknown(0, dlg_colour);
            textpos = 0;
            y = 0;
            do {
                textch = *resourceText++;
                if (textch == ']') {
                    if (textpos != 0) {
                        resbuftext[textpos] = '\0';
                        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, 0x0c, y + 0x21);
                    }
                    textpos = 0;
                    y += fontdefvalue;
                } else {
                    resbuftext[textpos++] = textch;
                }
            } while (*resourceText != '\0');
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
        }

        if (selectionIndex != lastSelection) {
            lastSelection = selectionIndex;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, lastColor);
            lastColor = (char)0xfe;
            /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
            reset_idle_counters();
        }

        timeDelta = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(selectionIndex, opponentmenu_buttons_x1, opponentmenu_buttons_x2, opponentmenu_buttons_y1, opponentmenu_buttons_y2,
                                           menu_hover_color_a, menu_hover_color_b);
        key = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(timeDelta);
        button = (char)/* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(5, opponentmenu_buttons_x1,
                                        opponentmenu_buttons_x2,
                                        opponentmenu_buttons_y1,
                                        opponentmenu_buttons_y2);
        if (button != (char)0xff &&
            !(globalgamesettings.game_opponenttype == 0 && button == 3))
            selectionIndex = button;

        if (key == 0)
            continue;

        switch (key) {
        case 0x0d:
        case 0x1b:
        case 0x20:
            switch (selectionIndex) {
            case 0:
                --globalgamesettings.game_opponenttype;
                if (globalgamesettings.game_opponenttype < 1)
                    globalgamesettings.game_opponenttype = 6;
                break;
            case 1:
                ++globalgamesettings.game_opponenttype;
                if (globalgamesettings.game_opponenttype == 7)
                    globalgamesettings.game_opponenttype = 1;
                break;
            case 2:
                globalgamesettings.game_opponenttype = 0;
                break;
            case 3:
                if (globalgamesettings.game_opponenttype != 0) {
                    /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
                    msdrawopaquechk();
                    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
                    unload_resource(oppRes);
                    show_waiting();
                    run_car_menu(globalgamesettings.game_opponentcarid,
                                 &globalgamesettings.game_opponentmaterial,
                                 &globalgamesettings.game_opponenttransmission,
                                 globalgamesettings.game_opponenttype);
                    previousType = (char)0xff;
                    goto menu_opponent_mouse_redraw;
                }
                break;
            case 4:
                if (globalgamesettings.game_opponenttype != 0) {
                    if (globalgamesettings.game_opponentcarid[0] == (char)0xff) {
                        globalgamesettings.game_opponentcarid[0] = globalgamesettings.game_playercarid[0];
                        globalgamesettings.game_opponentcarid[1] = globalgamesettings.game_playercarid[1];
                        globalgamesettings.game_opponentcarid[2] = globalgamesettings.game_playercarid[2];
                        globalgamesettings.game_opponentcarid[3] = globalgamesettings.game_playercarid[3];
                        globalgamesettings.game_opponentmaterial =
                            (char)((globalgamesettings.game_playermaterial & 1) ^ 1);
                        globalgamesettings.game_opponenttransmission = 0;
                    }
                } else {
                    globalgamesettings.game_opponentcarid[0] = (char)0xff;
                }

                /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
                if (isLoaded != 0)
                    unload_resource(oppRes);
                /* PLATFORM(memory): Game memory-manager API. */ mmgr_free(resource_ptropp);
                unload_resource(g_miscfile_ptr);
                msdrawopaquechk();
                return;
            }
            break;
        case 0x4b00:
            if (selectionIndex != 0)
                --selectionIndex;
            else
                selectionIndex = 4;
            if (globalgamesettings.game_opponenttype == 0 && selectionIndex == 3)
                --selectionIndex;
            break;
        case 0x4d00:
            if (selectionIndex < 4)
                ++selectionIndex;
            else
                selectionIndex = 0;
            if (globalgamesettings.game_opponenttype == 0 && selectionIndex == 3)
                ++selectionIndex;
            break;
        }

    }
}

/* Purpose: Presents and processes race options.
 * Parameters: none.
 * Returns: far.
 * Globals read: aDefault_1, byte_3B8F2, byte_3FE00, dialogarg2, dlg_colour, g_miscfile_ptr,
 *            main_data_file_addr, menu_clear_color, performGraphColor, replay_file,
 *            resbuftext
 * Globals written: g_miscfile_ptr, waitm_ms
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

char far run_option_menu(void)
{
    char active;
    char selection;
    char color_or_file;

    g_miscfile_ptr = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("misc");
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(menu_clear_color);

    copy_string(resbuftext, locate_shape_alt((char far *)g_miscfile_ptr, "gstu"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), 6,
                    dlg_colour, 0);

    copy_string(resbuftext, locate_shape_alt((char far *)g_miscfile_ptr, "gver"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ introtext(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), 16,
                    dlg_colour, 0);

    active = 1;
    while (active != 0) {
        selection = (char)show_dialog(2, 1,
            locate_text_resource((char far *)g_miscfile_ptr, "mop"),
            0xffff, 0xffff, dialogarg2, 0, 0);

        switch (selection) {
        case 0:
            if (byte_3B8F2 != 0) {
                color_or_file = 2;
            } else if (byte_3FE00 != 0) {
                color_or_file = 1;
            } else {
                color_or_file = 0;
            }
            selection = (char)show_dialog(2, 1,
                locate_text_resource((char far *)g_miscfile_ptr, "mid"),
                0xffff, 0xffff, performGraphColor, 0, color_or_file);
            switch (selection) {
            case 0: do_key_resource_text(); break;
            case 1: do_joystick_resource_text(); break;
            case 2: do_mou_resource_text(); break;
            }
            break;

        case 1:
            do_mof_resource_text();
            break;

        case 2:
            do_sonsof_resource_text();
            break;

        case 3:
            color_or_file = do_fileselect_dialog(replay_file, aDefault_1, ".rpl",
                locate_text_resource((char far *)main_data_file_addr, "rep"));
            if (color_or_file != 0) {
                waitm_ms = 150;
                show_waiting();
                /* PLATFORM(file): Legacy file and resource API. */ file_load_replay(replay_file, aDefault_1);
                active = 1;
                goto cleanup;
            }
            break;

        case 4:
            show_graphic_levels_menu();
            break;

        case 5:
            do_dos_resource_text();
            break;

        case -1:
        case 6:
            active = 0;
            break;
        }
    }

cleanup:
    unload_resource(g_miscfile_ptr);
    return active;
}

int hiscore_anim_table[28] = { 2, 1, 2, 3, 4, 1, 4, 0, 5, 0, 0, 6, 5, 6, 5, 1,
                               1, 2, 3, 5, 0, 6, 2, 3, 4, 4, 0, 6 };
short hiscore_rank_remap[3] = { 2, 0, 1 };
short hiscore_entry_remap[4] = { 1, 0, 3, 2 };
short hiscore_buttons_x1[5] = { 4, 84, 164, 244, 128 };
short hiscore_buttons_x2[5] = { 75, 155, 235, 315, 199 };
short hiscore_buttons_y1[5] = { 174, 174, 174, 174, 174 };
short hiscore_buttons_y2[5] = { 197, 197, 197, 197, 197 };
char aOpp2win[] = "opp2win";
char aOpp2lose[] = "opp2lose";
char aOp01[] = "op01";
/* Purpose: Presents race results and handles the score screen.
 * Parameters: none.
 * Returns: far.
 * Globals read: aOp01, aOpp1, aOpp2lose, aOpp2win, animation_outline_color, buf_g_path,
 *            dialogarg2, dlg_colour, elaptm1, end_hiscore_random, endhsdemo, fntndat,
 *            g_videoflg5, g_wndspr, globalgamesettings, hiscore_buttons_x1,
 *            hiscore_buttons_x2, hiscore_buttons_y1, hiscore_buttons_y2,
 *            hiscore_current_place, hiscore_entry_remap, hiscore_old_entry,
 *            hiscore_opponent_earlier, hiscore_opponent_live, hiscore_rank_prev,
 *            hiscore_rank_remap, hscore_trk11_ptr, main_data_file_addr,
 *            menu_button_color_a, menu_button_color_b, menu_button_color_c,
 *            menu_hover_color_a, menu_hover_color_b, pixel_scales, race_stats, resbuftext,
 *            td14tb, track_file
 * Globals written: aOp01, aOpp1, aOpp2lose, aOpp2win, end_hiscore_random, g_wndspr,
 *            hiscore_current_place, hiscore_old_entry, hiscore_opponent_earlier,
 *            hiscore_opponent_live, hiscore_rank_prev, resbuftext
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration.
 * PLATFORM(input_mouse): Legacy mouse input for menu interaction.
 * PLATFORM(memory): Game memory-manager API.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

char far end_hiscore(void)
{
  char newEvaluation;
  char formattedRaceTextBuffer[18];
  char opponent;
  struct SPRITE far *hiddenWindow;
  int animTime;
  int textLen;
  char glyph;
  char currentWordText[32];
  void far *scoreResource;
  char resultMode;
  char screenBlitFlag;
  int pixels;
  void far *textFile;
  char far *trackFile;
  char far *animationFrameList;
  struct SHAPE2D far *shapePtr;
  char previousAnimationFrame;
  char resChar;
  void far *enemyRes;
  int buttonsX1[4];
  int src;
  char priorMenuIndex;
  register int inputKey;
  register int i;
  int animationTimeDelta;
  int y;
  char newRecord;
  char far *menuText;
  int parts;
  unsigned short finalScoreTimeForRecord;
  char far *textResourceCursor;
  char clickedButton;
  int wordLen;
  int wordWidth;
  int animationXPosition;
  int buttonX2Positions[4];
  char currentMenuSelection;
  int animationYPosition;
  char currentAnimationFrame;
  int screenXOffset;

  ensure_file_exists(4);
  textFile = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("misc");
  if (globalgamesettings.game_opponenttype != 0)
  {
    aOpp1[3] = globalgamesettings.game_opponenttype + '0';
    enemyRes = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file(aOpp1);
  }
  g_wndspr = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0x140, 0xc8, 0x0f);
  if (g_videoflg5 != 0)
    hiddenWindow = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_make_window(0xc8, 0x64, 0x0f);
  screenBlitFlag = -1;
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1_clear();
  draw_button(0, 0, 0, 0x140, 0x64, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  draw_button(0, 0, 0x65, 0x140, 0x63, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  y = 0x6b;
  copy_string(resbuftext, locate_text_resource(textFile, "elt"));
  if (race_stats.game_total_finish != 0)
  {
    fmtframestr(formattedRaceTextBuffer, race_stats.game_total_finish - race_stats.game_penalty, 1);
    strcat(resbuftext, formattedRaceTextBuffer);
    if (endhsdemo & 2)
      copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "con"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
    y += 10;
    if (race_stats.game_penalty != 0)
    {
      copy_string(resbuftext, locate_text_resource(textFile, "ppt"));
      fmtframestr(formattedRaceTextBuffer, race_stats.game_penalty, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
      y += 10;
    }
  }
  else
  {
    copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "dnf"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  resultMode = 2;
  if (globalgamesettings.game_opponenttype != 0)
  {
    if (race_stats.field_144 == 0)
    {
      copy_string(resbuftext, locate_text_resource(textFile, "olt"));
      copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "dnf"));
      if (race_stats.game_total_finish != 0)
        resultMode = 0;
    }
    else if (race_stats.game_total_finish == 0 || (unsigned short) race_stats.field_144 < (unsigned short) race_stats.game_total_finish)
    {
      copy_string(resbuftext, locate_text_resource(textFile, "owt"));
      fmtframestr(formattedRaceTextBuffer, race_stats.field_144, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      resultMode = 1;
    }
    else
    {
      copy_string(resbuftext, locate_text_resource(textFile, "olt"));
      fmtframestr(formattedRaceTextBuffer, race_stats.field_144, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      if (race_stats.game_total_finish != 0)
        resultMode = 0;
    }
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  if (resultMode == 0)
    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ file_load_audio_resource("skidvict", "skidms", "VICT");
  else
    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ file_load_audio_resource("skidover", "skidms", "OVER");
  opponent = globalgamesettings.game_opponenttype;
  if (resultMode == 2)
    if (race_stats.game_oEndFrame != race_stats.game_pEndFrame)
    opponent = 0;
  copy_string(resbuftext, locate_text_resource(textFile, "avs"));
  (race_stats.game_pEndFrame + elaptm1) != 0 ?
    (i = (int) ((race_stats.game_travDist / (unsigned short) (race_stats.game_pEndFrame + elaptm1)) >> 8)) : (i = 0);
  print_int_as_string_maybe(formattedRaceTextBuffer, i, 0, 3);
  strcat(resbuftext, formattedRaceTextBuffer);
  copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "mph"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
  y += 10;
  if (race_stats.game_impactSpeed != 0)
  {
    copy_string(resbuftext, locate_text_resource(textFile, "imp"));
    print_int_as_string_maybe(formattedRaceTextBuffer, (unsigned short) race_stats.game_impactSpeed >> 8, 0, 3);
    strcat(resbuftext, formattedRaceTextBuffer);
    copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "mph"));
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  copy_string(resbuftext, locate_text_resource(textFile, "top"));
  print_int_as_string_maybe(formattedRaceTextBuffer, (unsigned short) race_stats.game_topSpeed >> 8, 0, 3);
  strcat(resbuftext, formattedRaceTextBuffer);
  copy_string(resbuftext + strlen(resbuftext), locate_text_resource(textFile, "mph"));
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
  y += 10;
  if (race_stats.game_jumpCount != 0)
  {
    copy_string(resbuftext, locate_text_resource(textFile, "jum"));
    print_int_as_string_maybe(formattedRaceTextBuffer, race_stats.game_jumpCount, 0, 3);
    strcat(resbuftext, formattedRaceTextBuffer);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), y, dlg_colour, 0);
  }
  if (opponent != 0)
  {
    if ((endhsdemo & 4) == 0)
    {
      hiscore_rank_prev = hiscore_current_place;
      hiscore_old_entry = end_hiscore_random;
      hiscore_opponent_earlier = hiscore_opponent_live;
      hiscore_current_place = get_super_random() % 3;
      if (hiscore_current_place == hiscore_rank_prev)
        hiscore_current_place = hiscore_rank_remap[hiscore_current_place];
      hiscore_opponent_live = get_super_random() % 3;
      if (hiscore_opponent_live == hiscore_opponent_earlier)
        hiscore_opponent_live = hiscore_rank_remap[hiscore_opponent_live];
      if (resultMode == 1)
      {
        if (race_stats.game_total_finish != 0)
          end_hiscore_random = get_super_random() % 2 + 2;
        else
          end_hiscore_random = get_super_random() % 2;
      }
      else
        end_hiscore_random = get_super_random() % 4;
      if (end_hiscore_random == hiscore_old_entry)
        end_hiscore_random = hiscore_entry_remap[end_hiscore_random];
    }
    if (resultMode == 1)
    {
      aOpp2win[3] = globalgamesettings.game_opponenttype + '0';
      scoreResource = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(3, aOpp2win);
      animationFrameList = locate_shape_alt(enemyRes, "winn");
      end_hiscore_random = race_stats.game_total_finish != 0 ?
          ((get_kevinrandom() + race_stats.game_frame) & 1) + 2 : (get_kevinrandom() + race_stats.game_frame) & 1;
      resChar = 'v';
    }
    else
    {
      aOpp2lose[3] = globalgamesettings.game_opponenttype + '0';
      scoreResource = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(3, aOpp2lose);
      animationFrameList = locate_shape_alt(enemyRes, "lose");
      end_hiscore_random = (get_kevinrandom() + race_stats.game_frame) & 3;
      resChar = 'd';
    }
  }
  newRecord = 0;
  /* PLATFORM(file): Legacy file and resource API. */ file_build_path(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path);
  trackFile = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(1, buf_g_path);
  if (trackFile == 0)
  {
    if (show_dialog(1, 1, locate_text_resource(main_data_file_addr, "ihd"), -1, -1, dialogarg2, 0, 0) != 0)
      trackFile = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource(1, buf_g_path);
  }
  if (trackFile != 0)
  {
    for (i = 0; i < 0x385; ++i)
    {
      if (trackFile[i] != td14tb[i])
      {
        newRecord = -1;
        break;
      }
    }
    /* PLATFORM(memory): Game memory-manager API. */ mmgr_release(trackFile);
  }
  else
    newRecord = -1;
  if (newRecord != -1)
  {
    if (highscore_write_a(0) != 0)
      if (highscore_write_a(1) != 0)
      newRecord = -1;
  }
  if (newRecord == 0)
  if (race_stats.game_total_finish != 0)
  {
    finalScoreTimeForRecord = race_stats.game_total_finish;
    if ((endhsdemo & 6) == 0)
      if (finalScoreTimeForRecord != 0 && hscore_trk11_ptr[6].marker > finalScoreTimeForRecord)
      newRecord = 1;
  }
  currentAnimationFrame = 0;
  animTime = 30;
  newEvaluation = 1;
redraw:
  if (opponent != 0)
  if (newRecord == 2)
  {
    newRecord = 0;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    highscore_text_unknown();
    currentMenuSelection = 1;
    newEvaluation = 1;
    goto show_buttons;
  }
  if (opponent != 0)
  {
    aOp01[3] = '1';
    shapePtr = locate_shape_fatal(scoreResource, aOp01);
    y = shapePtr->width * pixel_scales;
    animationXPosition = 0x138 - y;
    animationYPosition = (0x63 - shapePtr->height) >> 1;
    draw_lines_unknown(animationXPosition - 3, animationYPosition - 3, y + 5, shapePtr->height + 5, dlg_colour, 0, animation_outline_color);
    aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
    shape2d_op_unknown5(locate_shape_fatal(scoreResource, aOp01), animationXPosition, animationYPosition);
    previousAnimationFrame = currentAnimationFrame;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_setup_unknown(0, 0);
    y = 8;
    textLen = 0;
    pixels = 0;
    wordLen = 0;
    if (resultMode == 2)
      parts = 1;
    else
      parts = 3;
    for (i = 0; i < parts; ++i)
    {
      switch (i)
      {
        case 0:
          if (resultMode == 2)
            textResourceCursor = locate_text_resource(enemyRes, "d4a");
          else
          {
            formattedRaceTextBuffer[0] = resChar;
            formattedRaceTextBuffer[1] = '1';
            formattedRaceTextBuffer[2] = (char) hiscore_current_place + 'a';
            textResourceCursor = locate_text_resource(enemyRes, formattedRaceTextBuffer);
          }
          break;
        case 1:
          formattedRaceTextBuffer[0] = resChar;
          formattedRaceTextBuffer[1] = '2';
          formattedRaceTextBuffer[2] = (char) end_hiscore_random + 'a';
          textResourceCursor = locate_text_resource(enemyRes, formattedRaceTextBuffer);
          break;
        case 2:
          formattedRaceTextBuffer[0] = resChar;
          formattedRaceTextBuffer[1] = '3';
          formattedRaceTextBuffer[2] = (char) hiscore_opponent_live + 'a';
          textResourceCursor = locate_text_resource(enemyRes, formattedRaceTextBuffer);
          break;
        default:
          break;
      }
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
      do
      {
        glyph = *textResourceCursor++;
        if (glyph == ' ' || glyph == 0)
        {
          currentWordText[wordLen] = 0;
          wordWidth = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2(currentWordText);
          if (wordWidth + pixels < animationXPosition - 16 && textLen + wordLen < 80)
          {
            for (src = 0; src < wordLen; ++src)
              resbuftext[textLen++] = currentWordText[src];
            pixels += wordWidth;
          }
          else
          {
            resbuftext[textLen] = 0;
            /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, 8, y);
            y += 8;
            if (currentWordText[0] == ' ')
              src = 1;
            else
              src = 0;
            for (textLen = 0; src < wordLen; ++src)
              resbuftext[textLen++] = currentWordText[src];
            resbuftext[textLen] = 0;
            pixels = /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2(resbuftext);
          }
          wordLen = 1;
          currentWordText[0] = ' ';
        }
        else
          currentWordText[wordLen++] = glyph;
      }
      while (glyph != 0);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
    }
    if (textLen != 0)
    {
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef2(fntndat);
      resbuftext[textLen] = 0;
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, 8, y);
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ fontsetfontdef();
    }
    newEvaluation = 0;
    if (newRecord <= 0)
      goto show_buttons;
    newRecord = 0;
    newEvaluation = 1;
    draw_button(locate_text_resource(textFile, "bct"), 0x81, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, screenBlitFlag);
    screenBlitFlag = -2;
    reset_idle_counters();
    /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
    y = 1;
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
    do
    {
      animationTimeDelta = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(4, hiscore_buttons_x1, hiscore_buttons_x2, hiscore_buttons_y1, hiscore_buttons_y2, menu_hover_color_a, menu_hover_color_b);
      animTime += animationTimeDelta;
      if (animTime >= 30)
      {
        animTime -= 30;
        ++currentAnimationFrame;
        if (animationFrameList[currentAnimationFrame] == 0)
          currentAnimationFrame = 0;
      }
      if (currentAnimationFrame != previousAnimationFrame)
      {
        previousAnimationFrame = currentAnimationFrame;
        aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
        msdrawopaquechk();
        shapePtr = locate_shape_fatal(scoreResource, aOp01);
        if (g_videoflg5 != 0)
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_setup1_from_arg_pointer(hiddenWindow);
          shape2d_op_unknown5(shapePtr, 0, 0);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(animationXPosition, shapePtr->width * pixel_scales + animationXPosition, animationYPosition, shapePtr->height + animationYPosition);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_and_alt(hiddenWindow->image, animationXPosition, animationYPosition);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
        }
        else
          shape2d_op_unknown5(shapePtr, animationXPosition, animationYPosition);
        msdrawtransparentchk();
      }
      inputKey = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(i);
      if (inputKey == 13 || inputKey == 32 || inputKey == 27)
        y = 0;
    }
    while (y != 0);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
    draw_button(0, 0, 0, 0x140, 0x64, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(8, 0x138, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_clear_1_color(menu_button_color_c);
    msdrawopaquechk();
    enter_hiscore(finalScoreTimeForRecord, locate_text_resource(textFile, "inh"), resultMode);
  }
  else if (newRecord > 0)
  {
    /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
    msdrawopaquechk();
    enter_hiscore(finalScoreTimeForRecord, locate_text_resource(textFile, "inh"), 0);
    newRecord = 0;
    screenBlitFlag = -2;
  }
  else
  {
    msdrawopaquechk();
    if (newRecord == -1)
    {
      copy_string(resbuftext, locate_text_resource(textFile, "hna"));
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ hiscore_draw_text(resbuftext, /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_op2_alt(resbuftext), 0x32, dlg_colour, 0);
    }
    else
      highscore_text_unknown();
  }
show_buttons:
  currentMenuSelection = 1;
  priorMenuIndex = 1;
  reset_idle_counters();
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
  if (opponent == 0 || newRecord == -1)
    screenXOffset = -36;
  else
  {
    screenXOffset = 0;
    if (newEvaluation != 0)
      menuText = locate_text_resource(textFile, "bev");
    else
      menuText = locate_text_resource(textFile, "bhi");
    draw_button(menuText, hiscore_buttons_x1[0] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  }
  draw_button(locate_text_resource(textFile, "brp"), screenXOffset + hiscore_buttons_x1[1] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  if (opponent != 0)
    menuText = locate_text_resource(textFile, "bra");
  else
    menuText = locate_text_resource(textFile, "bdr");
  draw_button(menuText, screenXOffset + hiscore_buttons_x1[2] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  draw_button(locate_text_resource(textFile, "bmm"), screenXOffset + hiscore_buttons_x1[3] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  for (i = 0; i < 4; ++i)
  {
    buttonsX1[i] = hiscore_buttons_x1[i] + screenXOffset;
    buttonX2Positions[i] = hiscore_buttons_x2[i] + screenXOffset;
  }
  /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ check_input();
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_blit_to_video(g_wndspr, screenBlitFlag);
  screenBlitFlag = -2;
  /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
  for (;;)
  {
    if (currentMenuSelection != priorMenuIndex)
    {
      priorMenuIndex = currentMenuSelection;
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(0, 0x140, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
      msdrawopaquechk();
      /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprputimage(g_wndspr->image);
      msdrawtransparentchk();
      /* PLATFORM(timer): Timer-selected frame rate or timer position. */ timer_get_delta_alt();
      reset_idle_counters();
    }
    animationTimeDelta = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_timer_sprite_unknown(currentMenuSelection, buttonsX1, buttonX2Positions, hiscore_buttons_y1, hiscore_buttons_y2, menu_hover_color_a, menu_hover_color_b);
    if (newEvaluation == 0)
    if (resultMode != 2)
    {
      animTime += animationTimeDelta;
      if (animTime >= 30)
      {
        animTime -= 30;
        ++currentAnimationFrame;
        if (animationFrameList[currentAnimationFrame] == 0)
          currentAnimationFrame = 0;
      }
      if (currentAnimationFrame != previousAnimationFrame)
      {
        previousAnimationFrame = currentAnimationFrame;
        aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
        msdrawopaquechk();
        shapePtr = locate_shape_fatal(scoreResource, aOp01);
        if (g_videoflg5 != 0)
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_setup1_from_arg_pointer(hiddenWindow);
          shape2d_op_unknown5(shapePtr, 0, 0);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprset1size(animationXPosition, shapePtr->width * pixel_scales + animationXPosition, animationYPosition, shapePtr->height + animationYPosition);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_putimage_and_alt(hiddenWindow->image, animationXPosition, animationYPosition);
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprcopy2to12();
        }
        else
          shape2d_op_unknown5(shapePtr, animationXPosition, animationYPosition);
        shape2d_op_unknown5(locate_shape_fatal(scoreResource, aOp01), animationXPosition, animationYPosition);
        msdrawtransparentchk();
      }
    }
    if (opponent == 0 || newRecord == -1)
    {
      clickedButton = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(3, buttonsX1 + 1, buttonX2Positions + 1, hiscore_buttons_y1 + 1, hiscore_buttons_y2 + 1);
      if (clickedButton != -1)
        currentMenuSelection = clickedButton + 1;
    }
    else
    {
      clickedButton = /* PLATFORM(input_mouse): Legacy mouse input for menu interaction. */ mouse_multi_hittest(4, buttonsX1, buttonX2Positions, hiscore_buttons_y1, hiscore_buttons_y2);
      if (clickedButton != -1)
        currentMenuSelection = clickedButton;
    }
    inputKey = /* PLATFORM(input_kb): Legacy keyboard input or keyboard-state restoration. */ input_checking(animationTimeDelta);
    if (inputKey == 0)
      continue;
    switch (inputKey)
    {
      case 13:
      case 32:
        if (currentMenuSelection == 0)
        {
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_copy_wnd_to_1();
          draw_button(0, 0, 0, 0x140, 0x64, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
          if (newEvaluation != 0)
          {
            newRecord = 0;
            goto redraw;
          }
          else
          {
            newRecord = 2;
            goto redraw;
          }
        }
        /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unload();
        if (opponent != 0)
          /* PLATFORM(memory): Game memory-manager API. */ mmgr_release(scoreResource);
        if (g_videoflg5 != 0)
          /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(hiddenWindow);
        /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ sprite_free_window(g_wndspr);
        if (globalgamesettings.game_opponenttype != 0)
          unload_resource(enemyRes);
        unload_resource(textFile);
        return currentMenuSelection - 1;
      case 0x4b00:
        if (opponent == 0 || newRecord == -1)
        {
          if (currentMenuSelection <= 1)
            currentMenuSelection = 3;
          else
            --currentMenuSelection;
        }
        else if (currentMenuSelection != 0)
          --currentMenuSelection;
        else
          currentMenuSelection = 3;
        break;
      case 0x4d00:
        if (currentMenuSelection < 3)
          ++currentMenuSelection;
        else if (opponent == 0 || newRecord == -1)
          currentMenuSelection = 1;
        else
          currentMenuSelection = 0;
        break;
      default:
        continue;
    }
  }
}

unsigned char score_entry_alphabet[] = "0123456789abcdefghij";
char aQ00[] = "q00";
char aA00[] = "a00";
/* Purpose: Loads and presents the startup verification screen.
 * Parameters: selection.
 * Returns: far.
 * Globals read: aA00, aQ00, pass_check_flag, performGraphColor, resbuftext
 * Globals written: aA00, aQ00, pass_check_flag, resbuftext
 * PLATFORM(file): Legacy file and resource API.
 * PLATFORM(video): Legacy video mode, sprite, pixel, or font API.
 */

void far security_check(int selection)
{
    char prompt[1024];
    char questionID[6];
    int textLength;
    void far *resource;
    int failures;
    struct POINT2D points[6];
    register int scanIndex;
    char userInput[22];

    aQ00[2] = score_entry_alphabet[selection];
    aA00[2] = score_entry_alphabet[selection];
    resource = /* PLATFORM(file): Legacy file and resource API. */ file_load_resource_file("misc");
    copy_string(prompt, locate_text_resource(resource, "cop"));
    copy_string(resbuftext, locate_text_resource(resource, aQ00));
    strcat(prompt, resbuftext + 6);
    for (scanIndex = 0; scanIndex < 6; ++scanIndex)
        questionID[scanIndex] = resbuftext[scanIndex];
    show_dialog(3, 1, (char far *)prompt, 0xffff, 0x78,
                performGraphColor, points, 0);
    resbuftext[2] = 0;
    resbuftext[0] = questionID[0];
    resbuftext[1] = questionID[1];
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, points[0].x, points[0].y);
    resbuftext[0] = questionID[2];
    resbuftext[1] = questionID[3];
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, points[1].x, points[1].y);
    resbuftext[0] = questionID[4];
    resbuftext[1] = questionID[5];
    /* PLATFORM(video): Legacy video mode, sprite, pixel, or font API. */ font_draw_text(resbuftext, points[2].x, points[2].y);
    copy_string(resbuftext, locate_text_resource(resource, aA00));
    points[0] = points[3];
    textLength = strlen(resbuftext);
    userInput[0] = 0;
    failures = 0;
    do {
        call_read_line(userInput, textLength, points[0].x, points[0].y, 30000);
        scanIndex = 0;
        while (userInput[scanIndex] != 0)
        {
            if (isupper(userInput[scanIndex]))
                userInput[scanIndex] = tolower(userInput[scanIndex]);
            ++scanIndex;
        }
        if (strcmp(userInput, resbuftext) == 0)
            pass_check_flag = 1;
        else
            ++failures;
    } while (pass_check_flag == 0 && failures != 3);
    restore_mouse_sprite();
    msdrawtransparentchk();
    unload_resource(resource);
}

void set_default_car(void)
{
    globalgamesettings.game_playercarid[0] = 'C';
    globalgamesettings.game_playercarid[1] = 'O';
    globalgamesettings.game_playercarid[2] = 'U';
    globalgamesettings.game_playercarid[3] = 'N';
    globalgamesettings.game_playermaterial = 0;
    globalgamesettings.game_opponenttype = 0;
    globalgamesettings.game_opponentmaterial = 0;
    globalgamesettings.game_playertransmission = 1;
    globalgamesettings.game_opponentcarid[0] = 0xff;
}
