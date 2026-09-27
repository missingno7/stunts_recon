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
extern void far do_dos_restext(void);
extern char far do_fileselect_dialog(char *directory, char *track,
                                     char *extension, char far *text);
extern void far do_joy_restext(void);
extern void far do_key_restext(void);
extern void far do_mof_restext(void);
extern void far do_mou_restext(void);
extern void far do_sonsof_restext(void);
extern void far draw_button(char far *text, int id, int x, int y, int width,
                            int a, int b, int c, int mode);
extern void far draw_lines_unk(int x, int y, int width, int lineCount,
                               int colour, int mode, int style);
extern void far draw_track_preview(void);
extern void far ensure_file_exists(int kind);
extern void far enter_hiscore(unsigned short score, char far *text, unsigned char carStyle);
extern void far file_build_path(char *directory, char *name, char *extension,
                                char *destination);
extern char * far file_combine_and_find(char *, char *, char *);
extern char * far file_find_next_alt(void);
extern void far file_load_audiores(char *name1, char *name2, char *name3);
extern short far file_load_replay(char *dir, char *name);
extern void far *far file_load_resfile(char *name);
extern void far * far file_load_resource();
extern void far * file_load_shape2d_fatal_thunk(char *);
extern void far file_read_fatal(char *path, unsigned char far *destination);
extern short far file_write_fatal(char *path, struct HighScoreRecord far *source,
                                  unsigned long length);
extern void far font_draw_text(char *text, int x, int y);
extern int far font_op2(char *text);
extern int far font_op2_alt(char *name);
extern void far font_set_fontdef(void);
extern void far font_set_fontdef2(void far *data);
extern void far font_set_unk(int colour, int mode);
extern void far format_frame_as_string(char *destination, unsigned short frames, int mode);
extern void far get_a_poly_info(void);
extern void far highscore_text_unk(void);
extern char far highscore_write_a(int create_defaults);
extern void far highscore_write_b(void);
extern void far hiscore_draw_text();
extern void far init_game_state(int state);
extern int far input_checking(int delta);
extern void far intro_draw_text(char *text, int width, int x, int colour, int mode);
extern void far load_skybox(unsigned char skybox);
extern void far load_tracks_menu_shapes(void);
extern void far locate_many_resources(void far *, char *, void far **);
extern void far * far locate_shape_alt();
extern struct SHAPE2D far * far locate_shape_fatal(void far *, char *);
extern char far * far locate_text_res(char far *data, char *name);
extern void far mmgr_free(void far *);
extern void far mmgr_release(void far *resource);
extern void far mouse_draw_opaque_check(void);
extern void far mouse_draw_transparent_check(void);
extern int far mouse_multi_hittest();
extern int far mouse_timer_sprite_unk(int selection, int *x1, int *x2,
                                      int *y1, int *y2, int left, int right);
extern void far nullsub_1(void);
extern void far nullsub_2(void far *, int);
extern int far polarAngle(int, int);
extern void far print_highscore_entry(int rowIndex, char *stringOffsets);
extern int far print_int_as_string_maybe(char *text, int value, int mode, int width);
extern void far putpixel_single_maybe(int, int, int);
extern int far rect_intersect(struct RECTANGLE *, struct RECTANGLE *);
extern void far rect_union(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
extern void far run_car_menu(char *, char *, char *, int);
extern int far select_cliprect_rotate(int, int, int, struct RECTANGLE *, int);
extern void far set_projection(int left, int top, int width, int height);
extern void far setup_aero_trackdata(void far *, int);
extern void far setup_mcgawnd2(void);
extern void far shape2d_op_unk5(void far *shape, int x, int y);
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
extern void far sprite_copy_2_to_1_2(void);
extern void far sprite_copy_wnd_to_1(void);
extern void far sprite_copy_wnd_to_1_clear(void);
extern void far sprite_free_wnd(void far *sprite);
extern void far *far sprite_make_wnd(int width, int height, int depth);
extern void far sprite_putimage(void far *);
extern void far sprite_putimage_and_alt(void far *, int, int);
extern void far sprite_putimage_transparent(void far *, int, int);
extern void far sprite_set_1_from_argptr(void far *sprite);
extern void far sprite_set_1_size(int x, int y, int width, int height);
extern void far sprite_shape_to_1_alt(void far *);
extern char *strcat(char *destination, char *source);
extern int far strcmp(char *, char *);
extern char *strcpy(char *destination, char *source);
extern int strlen(char *text);
extern void far sub_29772(void);
extern void far * far sub_29A86(int, char *, void far *);
extern void far sub_34526(void far *);
extern void far timer_get_delta_alt(void);
extern char far track_setup(void);
extern unsigned int far transformed_shape_op(struct TRANSFORMEDSHAPE3D *);
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
extern char aPpt[];
extern char aSkidms_1[];
extern char aSkidms_2[];
extern char aSkidover[];
extern char aSkidvict[];
extern char aTop[];
extern char aVict[];
extern char aWinn[];
extern char a_trk_5[];
extern unsigned char backlights_paint_override;
extern char byte_3FE00;
extern unsigned char byte_43966;
extern unsigned char byte_449CE;
extern char byte_459E0[];
extern struct RECTANGLE cliprect_unk;
extern int dialog_fnt_colour;
extern int dialogarg2;
extern short elapsed_time1;
extern short end_hiscore_random;
extern unsigned int fontdef_unk_0E;
extern void far *fontnptr;
extern unsigned short framespersec;
extern short gState_144;
extern short gState_frame;
extern short gState_impactSpeed;
extern short gState_jumpCount;
extern short gState_oEndFrame;
extern short gState_pEndFrame;
extern short gState_penalty;
extern short gState_topSpeed;
extern short gState_total_finish_time;
extern unsigned long gState_travDist;
extern char g_path_buf[];
extern struct SHAPE3D game3dshapes[125];
extern struct GAMEINFO gameconfig;
extern char gnam_string[];
extern char gsna_string[];
extern int idle_counter;
extern unsigned char idle_expired;
extern char far *mainresptr;
extern void far *miscptr;
extern void far *opp_res;
extern void far *oppresources[7];
extern int performGraphColor;
extern char resID_byte1[];
extern struct SIMD simd_player;
extern short skybox_grd_color;
extern unsigned int slow_video_mgmt;
extern unsigned int slow_video_mgmt_copy;
extern struct GAMESTATE state;
extern struct HighScoreRecord far *td11_highscores;
extern char far *td14_elem_map_main;
extern int terraincenterpos[];
extern char unk_46464[];
extern int video_flag1_is1;
extern unsigned int video_flag2_is1;
extern unsigned int video_flag3_isFFFF;
extern unsigned char video_flag5_is0;
extern int waitflag;
extern struct SPRITE far *wndsprite;
extern int word_407CE;
extern int word_407D0;
extern short word_407D2;
extern int word_407F4;
extern int word_407F6;
extern int word_407F8;
extern int word_407FA;
extern short word_40D3A;
extern short word_40D3C;
extern short word_40D3E;
extern short word_40D40;
extern short word_40D44;
extern short word_46170[7];
extern short word_46172[7];

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

extern struct GAMEINFO gameconfig, gameconfigcopy;
extern int trackrows[], terrainrows[], trackpos[], trackcenterpos[];
extern int terrainpos[], terraincenterpos[], trackpos2[], trackcenterpos2[];
extern void far *fontdefptr;
extern short far *td01_track_file_cpy;
extern short far *td02_penalty_related;
extern char far *trackdata3;
extern short far *td04_aerotable_pl;
extern short far *td05_aerotable_op;
extern char far *trackdata6;
extern char far *trackdata7;
extern int far *td08_direction_related;
extern int far *trackdata9;
extern int far *td10_track_check_rel;
extern char far *trackdata12;
extern char far *td13_rpl_header;
extern unsigned char far *td15_terr_map_main;
extern char far *td16_rpl_buffer;
extern char far *td17_trk_elem_ordered;
extern char far *trackdata18;
extern unsigned char far *trackdata19;
extern char far *td20_trk_file_appnd;
extern char far *td21_col_from_path;
extern char far *td22_row_from_path;
extern unsigned char far *trackdata23;
extern struct GAMESTATE far *cvxptr;
extern char passed_security;
extern void far init_main(int, char *[]);
extern void far init_div0(void);
extern void far init_polyinfo(void);
extern void far *mmgr_alloc_resbytes(char *, unsigned long);
extern void far init_unknown(void);
extern void far init_kevinrandom(char *);
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
extern void far audio_stop_unk(void);
extern void far audiodrv_atexit(void);
extern void far kb_exit_handler(void);
extern void far kb_shift_checking1(void);
extern void far video_set_mode7(void);
extern void far set_default_car(void);
extern void far *tempdataptr;
extern void far sprite_copy_2_to_1_clear(void);
extern int far input_repeat_check(int);
extern int far run_intro(void);
extern signed char far setup_intro(void);
extern signed char far load_intro_resources(void);
extern void far sprite_clear_shape(char far*);
extern void far sprite_1_unk2(int, int, int, int, int);
extern unsigned short word_407D4, word_407D6, word_407D8, word_407DA;
extern unsigned short word_407DC, word_407DE, word_407E0, word_407E2;
extern unsigned short word_407E4, word_407E6, word_407E8, word_407EA;
extern char byte_363E5[];
extern char byte_363E6;
extern char unk_463EA[];
extern const unsigned char g_ascii_props[256];
extern void far sub_275C6(void);

struct RECTANGLE *rectptr_unk2 = 0;
struct RECTANGLE *rectptr_unk = 0;
char byte_3B80C[82] = {0};
char byte_3B85E[82] = {0};
char aDefault_1[10] = "DEFAULT";
char scenery_names[5][9] = { "desert", "tropical", "alpine", "city", "country" };
int hillHeightConsts[2] = { 0, 450 };
int custom_camera_distance = 210;
int custom_camera_azimuth_angle = 464;
int custom_camera_elevation_angle = 80;
char byte_3B8F2 = 0;
char is_audioloaded = 0;
char HKeyFlag = 0;
char cameramode = 0;
char byte_3B8F6 = 0;
char byte_3B8F7 = 0;
char kbormouse = 0;
char mouse_isdirty = 0;
char detail_level = 0;
unsigned char g_is_busy = 0;
char byte_3B8FC = 0;
char aKevin[] = "kevin";
char aOpp1[] = "opp1";
char aCarcoun[] = "carcoun";
int ported_stuntsmain_(int argc, char *argv[])
{
  register int index;
  char menuFlag;
  register int result;
  char far *trackBlock;
  init_main(argc, argv);
  init_div0();
  for (index = 0; index < 30; ++index)
  {
    trackrows[index] = 30 * (29 - index);
    terrainrows[index] = 30 * index;
    trackpos[index] = 29 - index << 10;
    trackcenterpos[index] = (29 - index << 10) + 0x200;
    terrainpos[index] = index << 10;
    terraincenterpos[index] = (index << 10) + 0x200;
  }
  for (index = 0; index < 30; ++index)
  {
    trackpos2[index] = index * 1024u;
    trackcenterpos2[index] = index * 1024 + 512;
  }
  mainresptr = file_load_resfile("main");
  fontdefptr = file_load_resource(0, "fontdef.fnt");
  fontnptr = file_load_resource(0, "fontn.fnt");
  font_set_fontdef();
  init_polyinfo();
  index = 0x6BF3;
  trackBlock = mmgr_alloc_resbytes("trakdata", index);
  td01_track_file_cpy = (short far *) trackBlock;
  trackBlock += 0x70A;
  td02_penalty_related = (short far *) trackBlock;
  trackBlock += 0x70A;
  trackdata3 = trackBlock;
  trackBlock += 0x70A;
  td04_aerotable_pl = (short far *) trackBlock;
  trackBlock += 0x80;
  td05_aerotable_op = (short far *) trackBlock;
  trackBlock += 0x80;
  trackdata6 = trackBlock;
  trackBlock += 0x80;
  trackdata7 = trackBlock;
  trackBlock += 0x80;
  td08_direction_related = (int far *) trackBlock;
  trackBlock += 0x60;
  trackdata9 = (int far *) trackBlock;
  trackBlock += 0x180;
  td10_track_check_rel = (int far *) trackBlock;
  trackBlock += 0x120;
  td11_highscores = (struct HighScoreRecord far *) trackBlock;
  trackBlock += 0x16C;
  trackdata12 = trackBlock;
  trackBlock += 0xF0;
  td13_rpl_header = trackBlock;
  trackBlock += 0x1A;
  td14_elem_map_main = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  td15_terr_map_main = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  td16_rpl_buffer = trackBlock;
  trackBlock += 0x2EE0;
  td17_trk_elem_ordered = trackBlock;
  trackBlock += 0x385;
  trackdata18 = trackBlock;
  trackBlock += 0x385;
  trackdata19 = (unsigned char far *) trackBlock;
  trackBlock += 0x385;
  td20_trk_file_appnd = trackBlock;
  trackBlock += 0x7AC;
  td21_col_from_path = trackBlock;
  trackBlock += 0x385;
  td22_row_from_path = trackBlock;
  trackBlock += 0x385;
  trackdata23 = (unsigned char far *) trackBlock;
  trackBlock += 0x30;
  init_unknown();
  init_kevinrandom(aKevin);
  strcpy(gameconfig.game_trackname, "DEFAULT");
  index = 0;
  input_do_checking(1);
  input_do_checking(1);
  mouse_draw_opaque_check();
  kbormouse = 0;
  passed_security = 0;
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
      file_build_path(byte_3B80C, gameconfig.game_trackname, ".trk", g_path_buf);
      file_read_fatal(g_path_buf, td14_elem_map_main);
    }
    idle_expired = 0;
    result = run_intro_looped();
    if (result == 27)
      continue;
  show_menu:
    ensure_file_exists(2);
    if (is_audioloaded == 0)
      file_load_audiores("skidslct", "skidms", "SLCT");
    switch (run_menu())
    {
      case 3:
        run_tracks_menu(0);
        goto show_menu;
      case 2:
        check_input();
        show_waiting();
        run_opponent_menu();
        goto show_menu;
      case 4:
        check_input();
        show_waiting();
        if (run_option_menu() == 0)
          goto show_menu;
        menuFlag = 1;
        goto do_game;
      case 1:
        check_input();
        show_waiting();
        run_car_menu(gameconfig.game_playercarid, &gameconfig.game_playermaterial, &gameconfig.game_playertransmission, 0);
        goto show_menu;
      case 0:
        menuFlag = 0;
      do_game:
        gameconfigcopy = gameconfig;
        for (index = 0; index < 0x70A; ++index)
          td20_trk_file_appnd[index] = td14_elem_map_main[index];
        for (index = 0; index < 0x51; ++index)
        {
          td20_trk_file_appnd[index + 0x70A] = byte_3B80C[index];
          td20_trk_file_appnd[index + 0x75B] = byte_3B85E[index];
        }
        if (idle_expired == 0)
        {
          if (track_setup() != 0)
          {
            run_tracks_menu(1);
            goto show_menu;
          }
          random_wait();
          if (passed_security == 0)
            security_check((char)(get_super_random() % 20));
        }
        else if (file_find("tedit.*") == 0)
          goto prepare_intro;
        else
          goto init_replay;
      init_replay:
        audio_unload();
        cvxptr = (struct GAMESTATE far *) mmgr_alloc_resbytes("cvx", 22400);
        init_game_state(-1);
        if (menuFlag != 0)
          byte_43966 = 0;
        else
          gameconfig.game_recordedframes = 0;
        break;
      case -1:
      prepare_intro:
        audio_unload();
        goto do_intro0;
      default:
        goto show_menu;
    }
    for (;;)
    {
      show_waiting();
      run_game();
      if (idle_expired == 0 && byte_43966 != 0)
      {
        switch (end_hiscore())
        {
          case 0:
            byte_43966 = 4;
            continue;
          case 1:
            gameconfig.game_recordedframes = 0;
            continue;
        }
      }
      break;
    }
    gameconfig = gameconfigcopy;
    for (index = 0; index < 0x70A; ++index)
      td14_elem_map_main[index] = td20_trk_file_appnd[index];
    for (index = 0; index < 0x51; ++index)
    {
      byte_3B80C[index] = td20_trk_file_appnd[index + 0x70A];
      byte_3B85E[index] = td20_trk_file_appnd[index + 0x75B];
    }
    mmgr_release(cvxptr);
    if (idle_expired != 0)
      goto do_intro0;
    goto show_menu;
  } while (show_dialog(2, 1, locate_text_res(mainresptr, "dos"), 0xFFFF, 0xFFFF, dialogarg2, 0, 0) < 1);
  mouse_draw_opaque_check();
  audio_stop_unk();
  audiodrv_atexit();
  kb_exit_handler();
  kb_shift_checking1();
  video_set_mode7();
}

int far run_intro_looped(void)
{
    register int inputResult;
    file_load_audiores("skidtitl", "skidms", "TITL");
    tempdataptr = file_load_resource(2, "sdtitl");
    wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
    inputResult = run_intro();
    sprite_free_wnd(wndsprite);
    mmgr_free(tempdataptr);
    if (inputResult == 0) {
        inputResult = setup_intro();
        if (inputResult == 0) {
            tempdataptr = file_load_resource(2, "sdcred");
            wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
            sprite_copy_wnd_to_1_clear();
            sprite_blit_to_video(wndsprite, 0);
            inputResult = load_intro_resources();
            sprite_free_wnd(wndsprite);
            mmgr_free(tempdataptr);
        }
    }
    audio_unload();
    return inputResult;
}

int far run_intro(void)
{
    register int inputResult;
    mouse_draw_opaque_check();
    sprite_copy_2_to_1_clear();
    mouse_draw_transparent_check();
    sprite_copy_wnd_to_1_clear();
    if (locate_shape_fatal(tempdataptr, "prod")->pos_y != 0)
        waitflag = 0xa0;
    else
        waitflag = 0xb4;
    sprite_shape_to_1_alt(locate_shape_fatal(tempdataptr, "prod"));
    inputResult = sprite_blit_to_video(wndsprite, 0xffff);
    if (inputResult == 0) {
        inputResult = input_repeat_check(0x190);
        if (inputResult == 0) {
            sprite_copy_wnd_to_1_clear();
            waitflag = 0xb4;
            sprite_shape_to_1_alt(locate_shape_fatal(tempdataptr, "titl"));
            inputResult = sprite_blit_to_video(wndsprite, 0xffff);
            if (inputResult == 0)
                inputResult = input_repeat_check(0x190);
        }
    }
    return inputResult;
}

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
  introFile = file_load_resfile("cred");
  locate_many_resources(tempdataptr, "arowarrwarw1arw2arw3arw4arw5arw6arw7arw8type", resources);
  waitflag = 0x96;
  sprite_copy_wnd_to_1_clear();
  imageWidth = ((struct SHAPE2D far *) resources[1])->pos_x;
  waitLimit = ((struct SHAPE2D far *) resources[1])->pos_y;
  scaledWidth = ((struct SHAPE2D far *) resources[1])->width * video_flag1_is1;
  displayHeight = ((struct SHAPE2D far *) resources[1])->height;
  copy_string(resID_byte1, locate_text_res(introFile, "cre"));
  intro_draw_text(resID_byte1, 0x78, 0, word_407D8, word_407DA);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gds0"));
  intro_draw_text(resID_byte1, 0x3c, 0x0c, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gds1"));
  intro_draw_text(resID_byte1, 0x68, 0x14, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_text_res(introFile, "des"));
  intro_draw_text(resID_byte1, 0x14, 0x20, word_407DC, word_407DE);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gdon"));
  intro_draw_text(resID_byte1, 0x14, 0x2c, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gkev"));
  intro_draw_text(resID_byte1, 0x14, 0x34, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gbra"));
  intro_draw_text(resID_byte1, 0x14, 0x3c, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "grob"));
  intro_draw_text(resID_byte1, 0x14, 0x44, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gsta"));
  intro_draw_text(resID_byte1, 0x14, 0x4c, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_text_res(introFile, "mus"));
  intro_draw_text(resID_byte1, 0x14, 0x5c, word_407E8, word_407EA);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gmsy"));
  intro_draw_text(resID_byte1, 0x14, 0x68, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gkri"));
  intro_draw_text(resID_byte1, 0x14, 0x70, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gbri"));
  intro_draw_text(resID_byte1, 0x14, 0x78, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_text_res(introFile, "pro"));
  intro_draw_text(resID_byte1, 0xac, 0x20, word_407E0, word_407E2);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gkev"));
  intro_draw_text(resID_byte1, 0xac, 0x2c, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_text_res(introFile, "opr"));
  intro_draw_text(resID_byte1, 0xac, 0x38, word_407E0, word_407E2);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gbra"));
  intro_draw_text(resID_byte1, 0xac, 0x40, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gric"));
  intro_draw_text(resID_byte1, 0xac, 0x48, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_text_res(introFile, "art"));
  intro_draw_text(resID_byte1, 0xac, 0x54, word_407E4, word_407E6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gmsm"));
  intro_draw_text(resID_byte1, 0xac, 0x60, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gdav"));
  intro_draw_text(resID_byte1, 0xac, 0x68, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gnic"));
  intro_draw_text(resID_byte1, 0xac, 0x70, word_407D4, word_407D6);
  copy_string(resID_byte1, locate_shape_alt(introFile, "gkev"));
  intro_draw_text(resID_byte1, 0xac, 0x78, word_407D4, word_407D6);
  unload_resource(introFile);
  sprite_blit_to_video(wndsprite, 0xffff);
  sprite_copy_2_to_1_2();
  timer_get_delta_alt();
  elapsed = 0x14a;
  for (;;)
  {
    timerStep = timer_get_delta_alt();
    elapsed -= timerStep << 1;
    if (imageWidth > elapsed)
      break;
    mouse_draw_opaque_check();
    sprite_putimage_and_alt(resources[1], elapsed, waitLimit);
    sprite_1_unk2(scaledWidth + elapsed, waitLimit, 0x20, displayHeight, 0);
    mouse_draw_transparent_check();
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
    sprite_copy_wnd_to_1();
    sprite_set_1_size(0, 0x140, waitLimit, 0xc8);
    sprite_clear_1_color(0);
    sprite_shape_to_1_alt(resources[picture]);
    sprite_copy_2_to_1_2();
    sprite_set_1_size(0, 0x140, waitLimit, 0xc8);
    mouse_draw_opaque_check();
    sprite_putimage(((struct SPRITE far *) wndsprite)->image);
    mouse_draw_transparent_check();
    step += 5;
    while (step > elapsed)
    {
      timerStep = timer_get_delta_alt();
      inputResult = input_do_checking(timerStep);
      elapsed += timerStep;
    }
  }
  sprite_set_1_size(0, 0x140, 0, 0xc8);
  mouse_draw_opaque_check();
  sprite_clear_shape(((struct SPRITE far *) wndsprite)->image);
  sprite_copy_wnd_to_1();
  sprite_set_1_size(0, 0x140, waitLimit, 0xc8);
  sprite_clear_1_color(0);
  sprite_shape_to_1_alt(resources[0]);
  sprite_shape_to_1_alt(resources[10]);
  inputResult = sprite_blit_to_video(wndsprite, 0);
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
    waitflag = 180;
    wndsprite = sprite_make_wnd(320, 200, 15);

    data_resource = file_load_resource(2, "sdmsel");
    sprite_copy_wnd_to_1();
    sprite_shape_to_1_alt(locate_shape_fatal((char far *)data_resource, "scrn"));
    mmgr_free(data_resource);

    for (;;) {
        if (selection != oldSelection) {
            oldSelection = selection;
            sprite_copy_wnd_to_1();
            sprite_blit_to_video(wndsprite, drawMode);
            drawMode = (signed char)-2;
            sprite_copy_2_to_1_2();
            sub_29772();
        }

        mouseDelta = mouse_timer_sprite_unk(selection,
            menu_buttons_x1, menu_buttons_x2,
            menu_buttons_y1, menu_buttons_y2, word_407CE, word_407D0);
        keyCode = input_checking(mouseDelta);
        hit = mouse_multi_hittest(5, menu_buttons_x1, menu_buttons_x2,
                                  menu_buttons_y1, menu_buttons_y2);
        if (hit != (signed char)-1)
            selection = hit;

        idle_counter += mouseDelta;
        if (idle_counter > 6000) {
            idle_counter = 0;
            ++idle_expired;
        }
        if (idle_expired != 0) {
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
    sprite_free_wnd(wndsprite);
    return selection;
}

short trackmenu_buttons_x1[3] = { 16, 112, 208 };
short trackmenu_buttons_x2[3] = { 112, 208, 304 };
short trackmenu_buttons_y1[3] = { 171, 171, 171 };
short trackmenu_buttons_y2[3] = { 197, 197, 197 };
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
    waitflag = 0x9b;
    wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
    load_skybox((unsigned char)td14_elem_map_main[0x384]);
    shape3d_load_all();
    set_projection(0x28, 0x28, 0x140, 0xc8);
    init_game_state(-2);
    sprite_copy_wnd_to_1();
    sprite_clear_1_color(skybox_grd_color);
    sprite_set_1_size(0, 0x140, 0, 0xc8);
    draw_track_preview();
    shape3d_free_all();
    unload_skybox();
    sprite_copy_wnd_to_1();
    strcpy(resID_byte1, "'");
    strcat(resID_byte1, gameconfig.game_trackname);
    strcat(resID_byte1, "'");
        intro_draw_text(resID_byte1, font_op2_alt(resID_byte1), 6,
                        dialog_fnt_colour, 0);

    if (highscore_write_a(0) == 0) {
        if (td11_highscores[word_46170[0]].marker != 0xffff) {
        copy_string(resID_byte1, locate_text_res(mainresptr, "hs0"));
        intro_draw_text(resID_byte1, font_op2_alt(resID_byte1), 0x12,
                        dialog_fnt_colour, 0);
        font_set_fontdef2(fontnptr);
        print_highscore_entry(0, offsets);
        font_set_unk(0, 0);
        font_draw_text(resID_byte1 + offsets[0], 16, 30);
        font_draw_text(resID_byte1 + offsets[1], 120, 30);
        font_draw_text(resID_byte1 + offsets[2], 224, 30);
        font_draw_text(resID_byte1 + offsets[3], 272, 30);
        font_set_fontdef();
        }
    }

    trkEdit = file_load_resfile("tedit");
    draw_button(locate_text_res(trkEdit, "bmt"), 0x11, 0xac, 0x5e, 0x18,
                word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(trkEdit, "bet"), 0x71, 0xac, 0x5e, 0x18,
                word_407F4, word_407F6, word_407F8, 0);
    draw_button(locate_text_res(trkEdit, "bmm"), 0xd1, 0xac, 0x5e, 0x18,
                word_407F4, word_407F6, word_407F8, 0);
    unload_resource(trkEdit);

menu_loop:
    if (trackSelection != lastSelection) {
        lastSelection = trackSelection;
        sprite_blit_to_video(wndsprite, displayState);
        displayState = -2;
        sprite_copy_2_to_1_2();
        sub_29772();
    }

    timerDiff = mouse_timer_sprite_unk(trackSelection,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2,
        word_407CE, word_407D0);
    idle_counter += timerDiff;
    if (idle_counter > 0x1770) {
        idle_counter = 0;
        ++idle_expired;
    }
    keyCodePressed = input_checking(timerDiff);
    choice = mouse_multi_hittest(3,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2);
    if (choice != -1)
        trackSelection = choice;
    if (idle_expired != 0) {
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
        sprite_free_wnd(wndsprite);
        return;
    case 0:
        dialogRes = do_fileselect_dialog(byte_3B80C, gameconfig.game_trackname,
                                         ".trk",
                                         locate_text_res(mainresptr, "trk"));
        file_build_path(byte_3B80C, gameconfig.game_trackname,
                        ".trk", g_path_buf);
        if (dialogRes != 0) {
            file_read_fatal(g_path_buf, td14_elem_map_main);
            sprite_free_wnd(wndsprite);
            goto preview;
        }
        lastSelection = -1;
        goto menu_loop;
    case 1:
        sprite_free_wnd(wndsprite);
    restart_game:
        check_input();
        show_waiting();
        waitflag = 0x82;
        track_setup();
        load_tracks_menu_shapes();
        goto preview;
    }
}

char far highscore_write_a(int create_defaults)
{
  void far *loaded_data;
  int index;
  struct HighScoreRecord row;
  byte_449CE = (unsigned char) (-1);
  for (index = 0; index < 7; ++index)
    word_46170[index] = index;

  file_build_path(byte_3B80C, gameconfig.game_trackname, ".hig", g_path_buf);
  if (create_defaults == 0)
  {
    g_is_busy = 1;
    loaded_data = sub_29A86(10, g_path_buf, td11_highscores);
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
    td11_highscores[index] = row;
  index = file_write_fatal(g_path_buf, td11_highscores, 0x16C);
  if (index != 0)
    goto score_file_missing;
  goto score_file_found;
}

void far highscore_text_unk(void)
{
    int row_color;
    int row_top;
    char offsets[4];
    char row_index;

    sprite_copy_wnd_to_1();

    copy_string(resID_byte1, locate_text_res(mainresptr, "hs1"));
    strcat(resID_byte1, " '");
    strcat(resID_byte1, gameconfig.game_trackname);
    strcat(resID_byte1, "'");
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1),
                      5, dialog_fnt_colour, 0);

    copy_string(resID_byte1, locate_text_res(mainresptr, "hs2"));
    hiscore_draw_text(resID_byte1, 16, 15, dialog_fnt_colour, 0);
    copy_string(resID_byte1, locate_text_res(mainresptr, "hs3"));
    hiscore_draw_text(resID_byte1, 120, 15, dialog_fnt_colour, 0);
    copy_string(resID_byte1, locate_text_res(mainresptr, "hs5"));
    hiscore_draw_text(resID_byte1, 224, 15, dialog_fnt_colour, 0);
    copy_string(resID_byte1, locate_text_res(mainresptr, "hs4"));
    hiscore_draw_text(resID_byte1, 272, 15, dialog_fnt_colour, 0);

    font_set_fontdef2(fontnptr);
    row_index = 0;
    goto row_check;
row_normal:
    row_color = 0;
row_draw:
    row_top = row_index * 10 + 25;
    font_set_unk(row_color, 0);
    font_draw_text(resID_byte1 + offsets[0], 16, row_top);
    font_draw_text(resID_byte1 + offsets[1], 120, row_top);
    font_draw_text(resID_byte1 + offsets[2], 224, row_top);
    font_draw_text(resID_byte1 + offsets[3], 272, row_top);
    ++row_index;
row_check:
    if (row_index >= 7)
        goto row_done;
    print_highscore_entry(row_index, offsets);
    if (row_index != byte_449CE)
        goto row_normal;
    row_color = dialogarg2;
    goto row_draw;
row_done:
    font_set_fontdef();
}

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

void far enter_hiscore(unsigned short score, char far *text, unsigned char carStyle)
{
    char selectedIndex;
    char insertionIndex;
    struct HighScoreRecord current;
    int dialogX;
    int dialogY;

    if (framespersec == 10)
        score <<= 1;

    if (score < td11_highscores[6].marker) {
        for (insertionIndex = 0; td11_highscores[insertionIndex].marker <= score && insertionIndex < 7; ++insertionIndex)
            word_46170[insertionIndex] = insertionIndex;
        selectedIndex = insertionIndex;
        byte_449CE = insertionIndex;
        goto secondary_check;
secondary_body:
        word_46172[insertionIndex] = insertionIndex;
        ++insertionIndex;
secondary_check:
        if (insertionIndex < 6)
            goto secondary_body;
        word_46170[selectedIndex] = 6;

        current.marker = score;
        current.bytes[0] = 0;
        strcpy((char *)current.bytes + 17, gnam_string);
        current.bytes[41] = carStyle;
        if (gameconfig.game_opponenttype != 0) {
            strcpy((char *)current.bytes + 42, unk_46464);
            current.bytes[44] = '/';
            strcpy((char *)current.bytes + 45, gsna_string);
        } else {
            strcpy((char *)current.bytes + 42, " ");
        }
        td11_highscores[6] = current;

        sprite_copy_wnd_to_1();
        highscore_text_unk();
        sprite_blit_to_video(wndsprite, -1);
        show_dialog(3, 0, text, -1, -1,
                    dialogarg2, &dialogY, 0);
        check_input();
        call_read_line(byte_459E0, 16, dialogY, dialogX, 30000);
        strcpy((char *)current.bytes, byte_459E0);
        td11_highscores[6] = current;

        sprite_copy_wnd_to_1();
        highscore_text_unk();
        sprite_blit_to_video(wndsprite, -1);
        highscore_write_b();
    }

    highscore_text_unk();
}

void far highscore_write_b(void)
{
    int index;
    struct HighScoreRecord orderedScores[7];

    for (index = 0; index < 7; ++index)
        orderedScores[index] = td11_highscores[word_46170[index]];

    file_build_path(byte_3B80C, gameconfig.game_trackname, ".hig", g_path_buf);
    g_is_busy = 1;
    file_write_fatal(g_path_buf, orderedScores, 0x16cUL);
    g_is_busy = 0;
}

struct RECTANGLE carmenu_cliprect = { 0, 320, 0, 95 };
int carmenu_buttons_y1[5] = { 229, 229, 229, 229, 229 };
int carmenu_buttons_y2[5] = { 316, 316, 316, 316, 316 };
int carmenu_buttons_x1[5] = { 107, 125, 143, 161, 179 };
int carmenu_buttons_x2[5] = { 124, 142, 160, 178, 196 };
char aLnam[6] = "lnam";
struct RECTANGLE rect_unk16 = { 0, 320, 0, 0 };
struct VECTOR carmenu_carpos = { 0, -840, 2880 };
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
  transformedCopy.pos = carmenu_carpos;
  transformedCopy.shapeptr = &game3dshapes[124];
  transformedCopy.rotvec.x = 0;
  transformedCopy.rotvec.y = 0;
  transformedCopy.unk = 0x7530;
  slow_video_mgmt_copy = slow_video_mgmt;
  if (slow_video_mgmt != 0)
  {
    transformedCopy.rectptr = &localClipId;
    transformedCopy.ts_flags = 8;
  }
  else
  {
    transformedCopy.ts_flags = 0;
  }
  ensure_file_exists(2);
  findfileX = file_combine_and_find(0, "car*", ".res");
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
    findfileX = file_find_next_alt();
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
          strcpy(resID_byte1, carids[graphIndexY]);
          strcpy(carids[graphIndexY], carids[innerIndexOld]);
          strcpy(carids[innerIndexOld], resID_byte1);
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

  waitflag = 0x5a;
  blitColorY = (char) 0xff;
  backlights_paint_override = 0x2d;
  sdcselPos = file_load_shape2d_fatal_thunk("sdcsel");
  if (opponenttype == 0)
    miscptr = file_load_resfile("misc");
  if (opponenttype != 0)
  {
    rect_unk16.right = 0xf0;
    if (video_flag5_is0 != 0)
    {
      opponentShapeCopy = oppresources[opponenttype];
      opponentWindowX = sprite_make_wnd(((struct OPPONENTIMAGE far *) opponentShapeCopy)->width, ((struct OPPONENTIMAGE far *) opponentShapeCopy)->height, 0x0f);
      setup_mcgawnd2();
      sprite_clear_1_color(0);
      nullsub_2(opp_res, (char) (opponenttype + '0'));
      sprite_putimage_transparent(oppresources[opponenttype], 0, 0);
      sprite_clear_shape_alt(opponentWindowX->image, 0, 0);
    }
  }
  else
  {
    rect_unk16.right = 0x140;
  }
  previousCarIndexCar = (char) 0xff;
  rotation = 0;
  selectedButtonY = 0;
  sub_29772();
  rotationDeltaY = 0;
  previousButton = (char) 0xff;
  set_projection(0x24, 0x11, 0x140, 0x64);
  timer_get_delta_alt();
  wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
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
      shape3d_load_car_shapes(carids[carIndexY], gameconfig.game_opponentcarid);
      aCarcoun[3] = carids[carIndexY][0];
      aCarcoun[4] = carids[carIndexY][1];
      aCarcoun[5] = carids[carIndexY][2];
      aCarcoun[6] = carids[carIndexY][3];
      carres = file_load_resfile(aCarcoun);
      setup_aero_trackdata(carres, 0);
      sprite_copy_wnd_to_1_clear();
      draw_button(0, 0, 0x67, 0x140, 0x61, word_407F4, word_407F6, word_407F8, 0);
      draw_button(0, 5, 0x6d, 0x46, 0x55, word_407F4, word_407F6, word_407F8, 0);
      draw_button(0, 0x52, 0x6d, 0x8c, 0x55, word_407F4, word_407F6, word_407F8, 0);
      sprite_shape_to_1_alt(locate_shape_fatal(sdcselPos, "grap"));
      font_set_fontdef2(fontnptr);
      font_set_unk(0, dialog_fnt_colour);
      font_draw_text("150", 9, 0x73);
      font_draw_text("100", 9, 0x87);
      font_draw_text(" 50", 9, 0x9b);
      font_draw_text("  0", 9, 0xaf);
      font_draw_text("0  20  40", 0x1a, 0xb9);
      font_set_fontdef();
      draw_button(locate_text_res(miscptr, "bdo"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[0] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
      draw_button(locate_text_res(miscptr, "bnx"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[1] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
      draw_button(locate_text_res(miscptr, "bla"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[2] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
      buttonText = locate_text_res(miscptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
      draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
      draw_button(locate_text_res(miscptr, "bco"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[4] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
      oldFramesPerSecond = framespersec;
      framespersec = 20;
      init_game_state(-2);
      state.playerstate.car_transmission = 1;
      graphIndexY = 0;
      for (;;)
      {
        update_car_speed(1, 0, &state.playerstate, &simd_player);
        carSpeedY = state.playerstate.car_speed >> 8;
        if ((graphYX = -((int) ((((unsigned long) carSpeedY) << 6) / 150 - 0xb5))) >= 0x75)
        {
          innerIndexOld = ((unsigned int) (0x26 * graphIndexY)) / 0x320 + 0x1c;
          putpixel_single_maybe(innerIndexOld, graphYX, performGraphColor);
          ++graphIndexY;
          if (graphIndexY < 0x320)
            continue;
        }
        break;
      }

      framespersec = oldFramesPerSecond;
      font_set_fontdef2(fontnptr);
      descriptionCar = locate_text_res(carres, "des");
      innerIndexOld = 0;
      graphYX = 0x74;
      do
      {
        textch = *(descriptionCar++);
        if (textch == ']')
        {
          if (innerIndexOld != 0)
          {
            resID_byte1[innerIndexOld] = '\0';
            font_draw_text(resID_byte1, 0x58, graphYX);
          }
          innerIndexOld = 0;
          graphYX += fontdef_unk_0E;
        }
        else
        {
          resID_byte1[innerIndexOld++] = textch;
        }
      }
      while ((*descriptionCar) != '\0');
      font_set_fontdef();
      timer_get_delta_alt();
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
        anglePos = polarAngle(carmenu_carpos.y, carmenu_carpos.z);
        if (slow_video_mgmt_copy != 0)
          localClipId = cliprect_unk;
        else
          localClipId = carmenu_cliprect;
        select_cliprect_rotate(0, anglePos, 0, &carmenu_cliprect, 0);
        if (*materialofs >= (char)game3dshapes[124].shape3d_numpaints)
          *materialofs = 0;
        transformedCopy.rotvec.z = rotation;
        transformedCopy.material = *materialofs;
        transformed_shape_op(&transformedCopy);
        if (carIndexY == previousCarIndexCar)
          rect_unk16.bottom = 0x5f;
        else
          rect_unk16.bottom = 0xc8;
        rect_intersect(&localClipId, &rect_unk16);
        rect_union(&localClipId, &fullClip, &unionRectY);
        if (runStateNew == 3)
          goto draw_full_car;
        runStateNew = 1;
        break;
      case 1:
      draw_full_car:
        runStateNew = 0;
        canLeave = 1;
        sprite_copy_wnd_to_1();
        sprite_set_1_size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        sprite_putimage(locate_shape_fatal(sdcselPos, "stop"));
        get_a_poly_info();
        sprite_copy_wnd_to_1();
        sprite_set_1_size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        fullClip = localClipId;
        if (opponenttype != 0 && carIndexY != previousCarIndexCar)
        {
          sprite_copy_wnd_to_1();
          if (video_flag5_is0 == 0)
          {
            nullsub_2(opp_res, (char) (opponenttype + '0'));
            sprite_putimage_transparent(oppresources[opponenttype], 0xf0, 0);
          }
          else
          {
            sprite_putimage_and_alt(opponentWindowX->image, 0xf0, 0);
          }
        }
        sprite_copy_2_to_1_2();
        sprite_set_1_size(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        mouse_draw_opaque_check();
        if (blitColorY != ((char) 0xfe))
        {
          sprite_blit_to_video(wndsprite, blitColorY);
          blitColorY = (char) 0xfe;
        }
        else
        {
          sprite_putimage(wndsprite->image);
        }
        mouse_draw_transparent_check();
        previousCarIndexCar = carIndexY;
        break;
    }
    if (selectedButtonY != previousButton)
    {
      if (previousButton != ((char) 0xff))
      {
        sprite_copy_2_to_1_2();
        sprite_set_1_size(carmenu_buttons_y1[0], carmenu_buttons_y2[0] + video_flag2_is1 & video_flag3_isFFFF, carmenu_buttons_x1[0], carmenu_buttons_x2[4] + 1);
        mouse_draw_opaque_check();
        sprite_putimage(wndsprite->image);
        mouse_draw_transparent_check();
        sprite_copy_2_to_1_2();
      }
      sub_29772();
      previousButton = selectedButtonY;
    }

    sprite_copy_2_to_1_2();
    rotationDeltaY = mouse_timer_sprite_unk(selectedButtonY, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2, word_407CE, word_407D0);
    idle_counter += rotationDeltaY;
    if (idle_counter > 0x2ee0)
    {
      idle_counter = 0;
      ++idle_expired;
    }
    keyCode = input_checking(rotationDeltaY);
    hitButtonX = (char) mouse_multi_hittest(5, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2);
    if (hitButtonX != ((char) 0xff))
      selectedButtonY = hitButtonX;
    if (idle_expired != 0)
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
            sprite_free_wnd(wndsprite);
            unload_resource(carres);
            shape3d_free_car_shapes();
            if (opponenttype != 0 && video_flag5_is0 != 0)
            sprite_free_wnd(opponentWindowX);
            if (opponenttype == 0)
            unload_resource(miscptr);
            mmgr_free(sdcselPos);
            mouse_draw_opaque_check();
            caridptr[0] = carids[carIndexY][0];
            caridptr[1] = carids[carIndexY][1];
            caridptr[2] = carids[carIndexY][2];
            caridptr[3] = carids[carIndexY][3];
            idle_expired = 0;
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
            sprite_copy_wnd_to_1();
            buttonText = locate_text_res(miscptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
            draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
            sprite_copy_2_to_1_2();
            mouse_draw_opaque_check();
            draw_button(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, word_407F4, word_407F6, word_407F8, 0);
            mouse_draw_transparent_check();
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
    miscptr = file_load_resfile("misc");
    opp_res = file_load_resource(8, "sdosel");
    locate_many_resources(opp_res, "opp0opp1opp2opp3opp4opp5opp6", oppresources);

    selectionIndex = 0;
    isLoaded = 0;
    previousType = (char)0xff;
    lastColor = (char)0xff;
    sub_29772();
menu_opponent_mouse_redraw:
    mouse_draw_transparent_check();

    for (;;) {
        if (previousType != gameconfig.game_opponenttype) {
            if (previousType != (char)0xff) {
                sprite_free_wnd(wndsprite);
                if (isLoaded != 0)
                    unload_resource(oppRes);
            }

            ensure_file_exists(4);
            if (gameconfig.game_opponenttype != 0) {
                aOpp1[3] = (char)(gameconfig.game_opponenttype + '0');
                oppRes = file_load_resfile(aOpp1);
                isLoaded = 1;
            } else {
                isLoaded = 0;
            }

            wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
            previousType = gameconfig.game_opponenttype;
            lastSelection = (char)0xff;
            if (video_flag5_is0 == 0)
                sprite_copy_wnd_to_1();
            else
                setup_mcgawnd2();
            sprite_clear_1_color(0);

            nullsub_2(opp_res, 0x37);
            sub_34526(locate_shape_fatal(opp_res, "scrn"));

            draw_button(locate_text_res(miscptr, "bla"), 0x15,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        word_407F4, word_407F6, word_407F8, 0);
            draw_button(locate_text_res(miscptr, "bnx"), 0x4d,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        word_407F4, word_407F6, word_407F8, 0);
            draw_button(locate_text_res(miscptr, "bcl"), 0x85,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        word_407F4, word_407F6, word_407F8, 0);
            draw_button(locate_text_res(miscptr, "bca"), 0xbd,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        word_407F4, word_407F6, word_407F8, 0);
            draw_button(locate_text_res(miscptr, "bdo"), 0xf5,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        word_407F4, word_407F6, word_407F8, 0);

            nullsub_2(opp_res, (char)(gameconfig.game_opponenttype + '0'));
            sub_34526(oppresources[gameconfig.game_opponenttype]);
            nullsub_2(opp_res, 0x37);
            sub_34526(locate_shape_fatal(opp_res, "clip"));

            if (video_flag5_is0 != 0) {
                sprite_clear_shape_alt(wndsprite->image, 0, 0);
                sprite_copy_wnd_to_1();
            }

            if (gameconfig.game_opponenttype != 0)
                resourceText = locate_text_res(oppRes, "des");
            else
                resourceText = locate_text_res(miscptr, "rac");
            font_set_fontdef2(fontnptr);
            font_set_unk(0, dialog_fnt_colour);
            textpos = 0;
            y = 0;
            do {
                textch = *resourceText++;
                if (textch == ']') {
                    if (textpos != 0) {
                        resID_byte1[textpos] = '\0';
                        font_draw_text(resID_byte1, 0x0c, y + 0x21);
                    }
                    textpos = 0;
                    y += fontdef_unk_0E;
                } else {
                    resID_byte1[textpos++] = textch;
                }
            } while (*resourceText != '\0');
            font_set_fontdef();
        }

        if (selectionIndex != lastSelection) {
            lastSelection = selectionIndex;
            sprite_blit_to_video(wndsprite, lastColor);
            lastColor = (char)0xfe;
            timer_get_delta_alt();
            sub_29772();
        }

        timeDelta = mouse_timer_sprite_unk(selectionIndex, opponentmenu_buttons_x1, opponentmenu_buttons_x2, opponentmenu_buttons_y1, opponentmenu_buttons_y2,
                                           word_407CE, word_407D0);
        key = input_checking(timeDelta);
        button = (char)mouse_multi_hittest(5, opponentmenu_buttons_x1,
                                        opponentmenu_buttons_x2,
                                        opponentmenu_buttons_y1,
                                        opponentmenu_buttons_y2);
        if (button != (char)0xff &&
            !(gameconfig.game_opponenttype == 0 && button == 3))
            selectionIndex = button;

        if (key == 0)
            continue;

        switch (key) {
        case 0x0d:
        case 0x1b:
        case 0x20:
            switch (selectionIndex) {
            case 0:
                --gameconfig.game_opponenttype;
                if (gameconfig.game_opponenttype < 1)
                    gameconfig.game_opponenttype = 6;
                break;
            case 1:
                ++gameconfig.game_opponenttype;
                if (gameconfig.game_opponenttype == 7)
                    gameconfig.game_opponenttype = 1;
                break;
            case 2:
                gameconfig.game_opponenttype = 0;
                break;
            case 3:
                if (gameconfig.game_opponenttype != 0) {
                    check_input();
                    mouse_draw_opaque_check();
                    sprite_free_wnd(wndsprite);
                    unload_resource(oppRes);
                    show_waiting();
                    run_car_menu(gameconfig.game_opponentcarid,
                                 &gameconfig.game_opponentmaterial,
                                 &gameconfig.game_opponenttransmission,
                                 gameconfig.game_opponenttype);
                    previousType = (char)0xff;
                    goto menu_opponent_mouse_redraw;
                }
                break;
            case 4:
                if (gameconfig.game_opponenttype != 0) {
                    if (gameconfig.game_opponentcarid[0] == (char)0xff) {
                        gameconfig.game_opponentcarid[0] = gameconfig.game_playercarid[0];
                        gameconfig.game_opponentcarid[1] = gameconfig.game_playercarid[1];
                        gameconfig.game_opponentcarid[2] = gameconfig.game_playercarid[2];
                        gameconfig.game_opponentcarid[3] = gameconfig.game_playercarid[3];
                        gameconfig.game_opponentmaterial =
                            (char)((gameconfig.game_playermaterial & 1) ^ 1);
                        gameconfig.game_opponenttransmission = 0;
                    }
                } else {
                    gameconfig.game_opponentcarid[0] = (char)0xff;
                }

                sprite_free_wnd(wndsprite);
                if (isLoaded != 0)
                    unload_resource(oppRes);
                mmgr_free(opp_res);
                unload_resource(miscptr);
                mouse_draw_opaque_check();
                return;
            }
            break;
        case 0x4b00:
            if (selectionIndex != 0)
                --selectionIndex;
            else
                selectionIndex = 4;
            if (gameconfig.game_opponenttype == 0 && selectionIndex == 3)
                --selectionIndex;
            break;
        case 0x4d00:
            if (selectionIndex < 4)
                ++selectionIndex;
            else
                selectionIndex = 0;
            if (gameconfig.game_opponenttype == 0 && selectionIndex == 3)
                ++selectionIndex;
            break;
        }

    }
}

char far run_option_menu(void)
{
    char active;
    char selection;
    char color_or_file;

    miscptr = file_load_resfile("misc");
    sprite_copy_2_to_1_2();
    sprite_clear_1_color(word_407FA);

    copy_string(resID_byte1, locate_shape_alt((char far *)miscptr, "gstu"));
    intro_draw_text(resID_byte1, font_op2_alt(resID_byte1), 6,
                    dialog_fnt_colour, 0);

    copy_string(resID_byte1, locate_shape_alt((char far *)miscptr, "gver"));
    intro_draw_text(resID_byte1, font_op2_alt(resID_byte1), 16,
                    dialog_fnt_colour, 0);

    active = 1;
    while (active != 0) {
        selection = (char)show_dialog(2, 1,
            locate_text_res((char far *)miscptr, "mop"),
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
                locate_text_res((char far *)miscptr, "mid"),
                0xffff, 0xffff, performGraphColor, 0, color_or_file);
            switch (selection) {
            case 0: do_key_restext(); break;
            case 1: do_joy_restext(); break;
            case 2: do_mou_restext(); break;
            }
            break;

        case 1:
            do_mof_restext();
            break;

        case 2:
            do_sonsof_restext();
            break;

        case 3:
            color_or_file = do_fileselect_dialog(byte_3B85E, aDefault_1, ".rpl",
                locate_text_res((char far *)mainresptr, "rep"));
            if (color_or_file != 0) {
                waitflag = 150;
                show_waiting();
                file_load_replay(byte_3B85E, aDefault_1);
                active = 1;
                goto cleanup;
            }
            break;

        case 4:
            show_graphic_levels_menu();
            break;

        case 5:
            do_dos_restext();
            break;

        case -1:
        case 6:
            active = 0;
            break;
        }
    }

cleanup:
    unload_resource(miscptr);
    return active;
}

int hiscore_anim_table[28] = { 2, 1, 2, 3, 4, 1, 4, 0, 5, 0, 0, 6, 5, 6, 5, 1,
                               1, 2, 3, 5, 0, 6, 2, 3, 4, 4, 0, 6 };
short word_3BCDE[3] = { 2, 0, 1 };
short word_3BCE4[4] = { 1, 0, 3, 2 };
short hiscore_buttons_x1[5] = { 4, 84, 164, 244, 128 };
short hiscore_buttons_x2[5] = { 75, 155, 235, 315, 199 };
short hiscore_buttons_y1[5] = { 174, 174, 174, 174, 174 };
short hiscore_buttons_y2[5] = { 197, 197, 197, 197, 197 };
char aOpp2win[] = "opp2win";
char aOpp2lose[] = "opp2lose";
char aOp01[] = "op01";
char far end_hiscore(void)
{
  char newEval;
  char lineBuffer[18];
  char opponent;
  struct SPRITE far *hiddenWindow;
  int animTime;
  int textLen;
  char glyph;
  char fragment[32];
  void far *scoreResource;
  char resultMode;
  char blitFlag;
  int pixels;
  void far *textFile;
  char far *trackFile;
  char far *frameList;
  struct SHAPE2D far *shapePtr;
  char prevFrame;
  char resChar;
  void far *enemyRes;
  int buttonsX1[4];
  int src;
  char lastMenu;
  register int key;
  register int i;
  int timeDelta;
  int y;
  char newRecord;
  char far *menuText;
  int parts;
  unsigned short scoreTime;
  char far *textPtr;
  char clickedButton;
  int wordLen;
  int wordWidth;
  int animX;
  int btnX2[4];
  char selectedMenu;
  int animY;
  char currentFrame;
  int xOffset;

  ensure_file_exists(4);
  textFile = file_load_resfile("misc");
  if (gameconfig.game_opponenttype != 0)
  {
    aOpp1[3] = gameconfig.game_opponenttype + '0';
    enemyRes = file_load_resfile(aOpp1);
  }
  wndsprite = sprite_make_wnd(0x140, 0xc8, 0x0f);
  if (video_flag5_is0 != 0)
    hiddenWindow = sprite_make_wnd(0xc8, 0x64, 0x0f);
  blitFlag = -1;
  sprite_copy_wnd_to_1_clear();
  draw_button(0, 0, 0, 0x140, 0x64, word_407F4, word_407F6, word_407F8, 0);
  draw_button(0, 0, 0x65, 0x140, 0x63, word_407F4, word_407F6, word_407F8, 0);
  y = 0x6b;
  copy_string(resID_byte1, locate_text_res(textFile, "elt"));
  if (gState_total_finish_time != 0)
  {
    format_frame_as_string(lineBuffer, gState_total_finish_time - gState_penalty, 1);
    strcat(resID_byte1, lineBuffer);
    if (byte_43966 & 2)
      copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "con"));
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
    y += 10;
    if (gState_penalty != 0)
    {
      copy_string(resID_byte1, locate_text_res(textFile, "ppt"));
      format_frame_as_string(lineBuffer, gState_penalty, 1);
      strcat(resID_byte1, lineBuffer);
      hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
      y += 10;
    }
  }
  else
  {
    copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "dnf"));
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
    y += 10;
  }
  resultMode = 2;
  if (gameconfig.game_opponenttype != 0)
  {
    if (gState_144 == 0)
    {
      copy_string(resID_byte1, locate_text_res(textFile, "olt"));
      copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "dnf"));
      if (gState_total_finish_time != 0)
        resultMode = 0;
    }
    else if (gState_total_finish_time == 0 || (unsigned short) gState_144 < (unsigned short) gState_total_finish_time)
    {
      copy_string(resID_byte1, locate_text_res(textFile, "owt"));
      format_frame_as_string(lineBuffer, gState_144, 1);
      strcat(resID_byte1, lineBuffer);
      resultMode = 1;
    }
    else
    {
      copy_string(resID_byte1, locate_text_res(textFile, "olt"));
      format_frame_as_string(lineBuffer, gState_144, 1);
      strcat(resID_byte1, lineBuffer);
      if (gState_total_finish_time != 0)
        resultMode = 0;
    }
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
    y += 10;
  }
  if (resultMode == 0)
    file_load_audiores("skidvict", "skidms", "VICT");
  else
    file_load_audiores("skidover", "skidms", "OVER");
  opponent = gameconfig.game_opponenttype;
  if (resultMode == 2)
    if (gState_oEndFrame != gState_pEndFrame)
    opponent = 0;
  copy_string(resID_byte1, locate_text_res(textFile, "avs"));
  (gState_pEndFrame + elapsed_time1) != 0 ?
    (i = (int) ((gState_travDist / (unsigned short) (gState_pEndFrame + elapsed_time1)) >> 8)) : (i = 0);
  print_int_as_string_maybe(lineBuffer, i, 0, 3);
  strcat(resID_byte1, lineBuffer);
  copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "mph"));
  hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
  y += 10;
  if (gState_impactSpeed != 0)
  {
    copy_string(resID_byte1, locate_text_res(textFile, "imp"));
    print_int_as_string_maybe(lineBuffer, (unsigned short) gState_impactSpeed >> 8, 0, 3);
    strcat(resID_byte1, lineBuffer);
    copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "mph"));
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
    y += 10;
  }
  copy_string(resID_byte1, locate_text_res(textFile, "top"));
  print_int_as_string_maybe(lineBuffer, (unsigned short) gState_topSpeed >> 8, 0, 3);
  strcat(resID_byte1, lineBuffer);
  copy_string(resID_byte1 + strlen(resID_byte1), locate_text_res(textFile, "mph"));
  hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
  y += 10;
  if (gState_jumpCount != 0)
  {
    copy_string(resID_byte1, locate_text_res(textFile, "jum"));
    print_int_as_string_maybe(lineBuffer, gState_jumpCount, 0, 3);
    strcat(resID_byte1, lineBuffer);
    hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), y, dialog_fnt_colour, 0);
  }
  if (opponent != 0)
  {
    if ((byte_43966 & 4) == 0)
    {
      word_40D3A = word_40D40;
      word_40D3C = end_hiscore_random;
      word_40D3E = word_40D44;
      word_40D40 = get_super_random() % 3;
      if (word_40D40 == word_40D3A)
        word_40D40 = word_3BCDE[word_40D40];
      word_40D44 = get_super_random() % 3;
      if (word_40D44 == word_40D3E)
        word_40D44 = word_3BCDE[word_40D44];
      if (resultMode == 1)
      {
        if (gState_total_finish_time != 0)
          end_hiscore_random = get_super_random() % 2 + 2;
        else
          end_hiscore_random = get_super_random() % 2;
      }
      else
        end_hiscore_random = get_super_random() % 4;
      if (end_hiscore_random == word_40D3C)
        end_hiscore_random = word_3BCE4[end_hiscore_random];
    }
    if (resultMode == 1)
    {
      aOpp2win[3] = gameconfig.game_opponenttype + '0';
      scoreResource = file_load_resource(3, aOpp2win);
      frameList = locate_shape_alt(enemyRes, "winn");
      end_hiscore_random = gState_total_finish_time != 0 ?
          ((get_kevinrandom() + gState_frame) & 1) + 2 : (get_kevinrandom() + gState_frame) & 1;
      resChar = 'v';
    }
    else
    {
      aOpp2lose[3] = gameconfig.game_opponenttype + '0';
      scoreResource = file_load_resource(3, aOpp2lose);
      frameList = locate_shape_alt(enemyRes, "lose");
      end_hiscore_random = (get_kevinrandom() + gState_frame) & 3;
      resChar = 'd';
    }
  }
  newRecord = 0;
  file_build_path(byte_3B80C, gameconfig.game_trackname, ".trk", g_path_buf);
  trackFile = file_load_resource(1, g_path_buf);
  if (trackFile == 0)
  {
    if (show_dialog(1, 1, locate_text_res(mainresptr, "ihd"), -1, -1, dialogarg2, 0, 0) != 0)
      trackFile = file_load_resource(1, g_path_buf);
  }
  if (trackFile != 0)
  {
    for (i = 0; i < 0x385; ++i)
    {
      if (trackFile[i] != td14_elem_map_main[i])
      {
        newRecord = -1;
        break;
      }
    }
    mmgr_release(trackFile);
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
  if (gState_total_finish_time != 0)
  {
    scoreTime = gState_total_finish_time;
    if ((byte_43966 & 6) == 0)
      if (scoreTime != 0 && td11_highscores[6].marker > scoreTime)
      newRecord = 1;
  }
  currentFrame = 0;
  animTime = 30;
  newEval = 1;
redraw:
  if (opponent != 0)
  if (newRecord == 2)
  {
    newRecord = 0;
    sprite_copy_wnd_to_1();
    highscore_text_unk();
    selectedMenu = 1;
    newEval = 1;
    goto show_buttons;
  }
  if (opponent != 0)
  {
    aOp01[3] = '1';
    shapePtr = locate_shape_fatal(scoreResource, aOp01);
    y = shapePtr->width * video_flag1_is1;
    animX = 0x138 - y;
    animY = (0x63 - shapePtr->height) >> 1;
    draw_lines_unk(animX - 3, animY - 3, y + 5, shapePtr->height + 5, dialog_fnt_colour, 0, word_407D2);
    aOp01[3] = frameList[currentFrame] + '0';
    shape2d_op_unk5(locate_shape_fatal(scoreResource, aOp01), animX, animY);
    prevFrame = currentFrame;
    font_set_unk(0, 0);
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
            textPtr = locate_text_res(enemyRes, "d4a");
          else
          {
            lineBuffer[0] = resChar;
            lineBuffer[1] = '1';
            lineBuffer[2] = (char) word_40D40 + 'a';
            textPtr = locate_text_res(enemyRes, lineBuffer);
          }
          break;
        case 1:
          lineBuffer[0] = resChar;
          lineBuffer[1] = '2';
          lineBuffer[2] = (char) end_hiscore_random + 'a';
          textPtr = locate_text_res(enemyRes, lineBuffer);
          break;
        case 2:
          lineBuffer[0] = resChar;
          lineBuffer[1] = '3';
          lineBuffer[2] = (char) word_40D44 + 'a';
          textPtr = locate_text_res(enemyRes, lineBuffer);
          break;
        default:
          break;
      }
      font_set_fontdef2(fontnptr);
      do
      {
        glyph = *textPtr++;
        if (glyph == ' ' || glyph == 0)
        {
          fragment[wordLen] = 0;
          wordWidth = font_op2(fragment);
          if (wordWidth + pixels < animX - 16 && textLen + wordLen < 80)
          {
            for (src = 0; src < wordLen; ++src)
              resID_byte1[textLen++] = fragment[src];
            pixels += wordWidth;
          }
          else
          {
            resID_byte1[textLen] = 0;
            font_draw_text(resID_byte1, 8, y);
            y += 8;
            if (fragment[0] == ' ')
              src = 1;
            else
              src = 0;
            for (textLen = 0; src < wordLen; ++src)
              resID_byte1[textLen++] = fragment[src];
            resID_byte1[textLen] = 0;
            pixels = font_op2(resID_byte1);
          }
          wordLen = 1;
          fragment[0] = ' ';
        }
        else
          fragment[wordLen++] = glyph;
      }
      while (glyph != 0);
      font_set_fontdef();
    }
    if (textLen != 0)
    {
      font_set_fontdef2(fontnptr);
      resID_byte1[textLen] = 0;
      font_draw_text(resID_byte1, 8, y);
      font_set_fontdef();
    }
    newEval = 0;
    if (newRecord <= 0)
      goto show_buttons;
    newRecord = 0;
    newEval = 1;
    draw_button(locate_text_res(textFile, "bct"), 0x81, 0xaf, 0x46, 0x15, word_407F4, word_407F6, word_407F8, 0);
    sprite_blit_to_video(wndsprite, blitFlag);
    blitFlag = -2;
    sub_29772();
    check_input();
    y = 1;
    sprite_copy_2_to_1_2();
    do
    {
      timeDelta = mouse_timer_sprite_unk(4, hiscore_buttons_x1, hiscore_buttons_x2, hiscore_buttons_y1, hiscore_buttons_y2, word_407CE, word_407D0);
      animTime += timeDelta;
      if (animTime >= 30)
      {
        animTime -= 30;
        ++currentFrame;
        if (frameList[currentFrame] == 0)
          currentFrame = 0;
      }
      if (currentFrame != prevFrame)
      {
        prevFrame = currentFrame;
        aOp01[3] = frameList[currentFrame] + '0';
        mouse_draw_opaque_check();
        shapePtr = locate_shape_fatal(scoreResource, aOp01);
        if (video_flag5_is0 != 0)
        {
          sprite_set_1_from_argptr(hiddenWindow);
          shape2d_op_unk5(shapePtr, 0, 0);
          sprite_copy_2_to_1_2();
          sprite_set_1_size(animX, shapePtr->width * video_flag1_is1 + animX, animY, shapePtr->height + animY);
          sprite_putimage_and_alt(hiddenWindow->image, animX, animY);
          sprite_copy_2_to_1_2();
        }
        else
          shape2d_op_unk5(shapePtr, animX, animY);
        mouse_draw_transparent_check();
      }
      key = input_checking(i);
      if (key == 13 || key == 32 || key == 27)
        y = 0;
    }
    while (y != 0);
    sprite_copy_wnd_to_1();
    draw_button(0, 0, 0, 0x140, 0x64, word_407F4, word_407F6, word_407F8, 0);
    sprite_set_1_size(8, 0x138, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
    sprite_clear_1_color(word_407F8);
    mouse_draw_opaque_check();
    enter_hiscore(scoreTime, locate_text_res(textFile, "inh"), resultMode);
  }
  else if (newRecord > 0)
  {
    check_input();
    mouse_draw_opaque_check();
    enter_hiscore(scoreTime, locate_text_res(textFile, "inh"), 0);
    newRecord = 0;
    blitFlag = -2;
  }
  else
  {
    mouse_draw_opaque_check();
    if (newRecord == -1)
    {
      copy_string(resID_byte1, locate_text_res(textFile, "hna"));
      hiscore_draw_text(resID_byte1, font_op2_alt(resID_byte1), 0x32, dialog_fnt_colour, 0);
    }
    else
      highscore_text_unk();
  }
show_buttons:
  selectedMenu = 1;
  lastMenu = 1;
  sub_29772();
  sprite_copy_wnd_to_1();
  if (opponent == 0 || newRecord == -1)
    xOffset = -36;
  else
  {
    xOffset = 0;
    if (newEval != 0)
      menuText = locate_text_res(textFile, "bev");
    else
      menuText = locate_text_res(textFile, "bhi");
    draw_button(menuText, hiscore_buttons_x1[0] + 1, 0xaf, 0x46, 0x15, word_407F4, word_407F6, word_407F8, 0);
  }
  draw_button(locate_text_res(textFile, "brp"), xOffset + hiscore_buttons_x1[1] + 1, 0xaf, 0x46, 0x15, word_407F4, word_407F6, word_407F8, 0);
  if (opponent != 0)
    menuText = locate_text_res(textFile, "bra");
  else
    menuText = locate_text_res(textFile, "bdr");
  draw_button(menuText, xOffset + hiscore_buttons_x1[2] + 1, 0xaf, 0x46, 0x15, word_407F4, word_407F6, word_407F8, 0);
  draw_button(locate_text_res(textFile, "bmm"), xOffset + hiscore_buttons_x1[3] + 1, 0xaf, 0x46, 0x15, word_407F4, word_407F6, word_407F8, 0);
  for (i = 0; i < 4; ++i)
  {
    buttonsX1[i] = hiscore_buttons_x1[i] + xOffset;
    btnX2[i] = hiscore_buttons_x2[i] + xOffset;
  }
  check_input();
  sprite_blit_to_video(wndsprite, blitFlag);
  blitFlag = -2;
  sprite_copy_2_to_1_2();
  for (;;)
  {
    if (selectedMenu != lastMenu)
    {
      lastMenu = selectedMenu;
      sprite_copy_2_to_1_2();
      sprite_set_1_size(0, 0x140, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
      mouse_draw_opaque_check();
      sprite_putimage(wndsprite->image);
      mouse_draw_transparent_check();
      timer_get_delta_alt();
      sub_29772();
    }
    timeDelta = mouse_timer_sprite_unk(selectedMenu, buttonsX1, btnX2, hiscore_buttons_y1, hiscore_buttons_y2, word_407CE, word_407D0);
    if (newEval == 0)
    if (resultMode != 2)
    {
      animTime += timeDelta;
      if (animTime >= 30)
      {
        animTime -= 30;
        ++currentFrame;
        if (frameList[currentFrame] == 0)
          currentFrame = 0;
      }
      if (currentFrame != prevFrame)
      {
        prevFrame = currentFrame;
        aOp01[3] = frameList[currentFrame] + '0';
        mouse_draw_opaque_check();
        shapePtr = locate_shape_fatal(scoreResource, aOp01);
        if (video_flag5_is0 != 0)
        {
          sprite_set_1_from_argptr(hiddenWindow);
          shape2d_op_unk5(shapePtr, 0, 0);
          sprite_copy_2_to_1_2();
          sprite_set_1_size(animX, shapePtr->width * video_flag1_is1 + animX, animY, shapePtr->height + animY);
          sprite_putimage_and_alt(hiddenWindow->image, animX, animY);
          sprite_copy_2_to_1_2();
        }
        else
          shape2d_op_unk5(shapePtr, animX, animY);
        shape2d_op_unk5(locate_shape_fatal(scoreResource, aOp01), animX, animY);
        mouse_draw_transparent_check();
      }
    }
    if (opponent == 0 || newRecord == -1)
    {
      clickedButton = mouse_multi_hittest(3, buttonsX1 + 1, btnX2 + 1, hiscore_buttons_y1 + 1, hiscore_buttons_y2 + 1);
      if (clickedButton != -1)
        selectedMenu = clickedButton + 1;
    }
    else
    {
      clickedButton = mouse_multi_hittest(4, buttonsX1, btnX2, hiscore_buttons_y1, hiscore_buttons_y2);
      if (clickedButton != -1)
        selectedMenu = clickedButton;
    }
    key = input_checking(timeDelta);
    if (key == 0)
      continue;
    switch (key)
    {
      case 13:
      case 32:
        if (selectedMenu == 0)
        {
          sprite_copy_wnd_to_1();
          draw_button(0, 0, 0, 0x140, 0x64, word_407F4, word_407F6, word_407F8, 0);
          if (newEval != 0)
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
        audio_unload();
        if (opponent != 0)
          mmgr_release(scoreResource);
        if (video_flag5_is0 != 0)
          sprite_free_wnd(hiddenWindow);
        sprite_free_wnd(wndsprite);
        if (gameconfig.game_opponenttype != 0)
          unload_resource(enemyRes);
        unload_resource(textFile);
        return selectedMenu - 1;
      case 0x4b00:
        if (opponent == 0 || newRecord == -1)
        {
          if (selectedMenu <= 1)
            selectedMenu = 3;
          else
            --selectedMenu;
        }
        else if (selectedMenu != 0)
          --selectedMenu;
        else
          selectedMenu = 3;
        break;
      case 0x4d00:
        if (selectedMenu < 3)
          ++selectedMenu;
        else if (opponent == 0 || newRecord == -1)
          selectedMenu = 1;
        else
          selectedMenu = 0;
        break;
      default:
        continue;
    }
  }
}

unsigned char byte_3BD34[] = "0123456789abcdefghij";
char aQ00[] = "q00";
char aA00[] = "a00";
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

    aQ00[2] = byte_3BD34[selection];
    aA00[2] = byte_3BD34[selection];
    resource = file_load_resfile("misc");
    copy_string(prompt, locate_text_res(resource, "cop"));
    copy_string(resID_byte1, locate_text_res(resource, aQ00));
    strcat(prompt, unk_463EA);
    for (scanIndex = 0; scanIndex < 6; ++scanIndex)
        questionID[scanIndex] = resID_byte1[scanIndex];
    show_dialog(3, 1, (char far *)prompt, 0xffff, 0x78,
                performGraphColor, points, 0);
    byte_363E6 = 0;
    resID_byte1[0] = questionID[0];
    byte_363E5[0] = questionID[1];
    font_draw_text(resID_byte1, points[0].x, points[0].y);
    resID_byte1[0] = questionID[2];
    byte_363E5[0] = questionID[3];
    font_draw_text(resID_byte1, points[1].x, points[1].y);
    resID_byte1[0] = questionID[4];
    byte_363E5[0] = questionID[5];
    font_draw_text(resID_byte1, points[2].x, points[2].y);
    copy_string(resID_byte1, locate_text_res(resource, aA00));
    points[0] = points[3];
    textLength = strlen(resID_byte1);
    userInput[0] = 0;
    failures = 0;
    do {
        call_read_line(userInput, textLength, points[0].x, points[0].y, 30000);
        scanIndex = 0;
        while (userInput[scanIndex] != 0)
        {
            if (g_ascii_props[userInput[scanIndex]] & 1)
                userInput[scanIndex] = (g_ascii_props[userInput[scanIndex]] & 1) ?
                                    userInput[scanIndex] - 'A' + 'a' : userInput[scanIndex];
            ++scanIndex;
        }
        if (strcmp(userInput, resID_byte1) == 0)
            passed_security = 1;
        else
            ++failures;
    } while (passed_security == 0 && failures != 3);
    sub_275C6();
    mouse_draw_transparent_check();
    unload_resource(resource);
}

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
