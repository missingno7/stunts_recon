#include "types.h"
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
/* Game startup, menus, high scores, and security dialog.
 * The register declarations and marked labels/stores preserve MSC 5.10 OMF output.
 */
#include "gamerec.h"

#include "math3d.h"
/* Local view: SPRITE; struct.count is 2; another unit uses 14. */
struct SPRITE { void far *image; U16S  words[13]; };
#include "car.h"

/* Local view: GAMESTATE; struct.count is 46; another unit uses 45. */
struct GAMESTATE {
    I32 game_debris_x[24], game_debris_y[24], game_debris_z[24];
    struct VECTOR game_chase_camera_pos[2], game_chase_camera_prev, game_opponent_camera_prev;
    I16S game_end_delay_elapsed, game_end_delay_limit;
    I32 game_speed_frame_sum;
    I16S game_frame, game_total_finish, game_opponent_finish_time, game_pEndFrame;
    I16S game_oEndFrame, game_penalty;
    U16S  game_impactSpeed, game_topSpeed;
    I16S game_jumpCount;
    struct CARSTATE playerstate, opponentstate;
    I16S game_player_route_anchor, game_player_last_route_node, game_startcol, game_startcol2;
    I16S game_startrow, game_startrow2;
    I16S game_debris_rot_x[24], game_debris_rot_y[24], game_debris_rot_z[24], game_debris_planar_step[24];
    I8 game_debris_vertical_step[48], kevinseed[6], game_checkpoint_valid, game_inputmode;
    I8 game_3F6autoLoadEvalFlag, game_track_camera_index[2], game_opponent_speed_limit, game_track_object_broken[48];
    I8 game_debris_active, game_debris_shape_index[24], game_debris_origin_kind[24];
    I8 game_route_recovery_mode, game_route_confirm_count, game_player_guidance, game_opponent_pass_side, field_45F;
};
/* Local view: SIMD; struct.count is 30; another unit uses 29. */
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
/* Local view: SHAPE3D; struct.count is 7; another unit uses 8. */
struct SHAPE3D {
    U16  shape3d_numverts;
    struct VECTOR far *shape3d_verts;
    U16  shape3d_numprimitives;
    U16  shape3d_numpaints;
    I8 far *shape3d_primitives;
    I8 far *shape3d_cull1;
    I8 far *shape3d_cull2;
};
#include "xformrec.h"

#include "ui.h"

/* Local view: do_fileselect_dialog; parameters[3].pointer is signed byte; another unit uses void. */
extern I8 far floadreq(I8 *directory, I8 *track,
                                     I8 *extension, I8 far *text);
#include "files.h"
/* Local view: openresource; renderer retains its byte-pointer resource prototype. */
extern void far *far openresource(I8 *);

/* Local view: hiscore_draw_text; return is void; another unit uses near pointer. */
extern void far hiscore_draw_text();
#include "shapes3d.h"
/* Local view: locate_shape_fatal; return.pointer is struct SHAPE2D; another unit uses signed byte. */
extern struct SHAPE2D far * far locate_shape_fatal(void far *, I8 *);
#include "memory.h"
#include "input.h"
#include "game.h"
/* Local math import preserves the original communal LINK ordering. */

/* Local view: selectcar; parameters[0].pointer is signed byte; another unit uses struct GAMEINFO. */
extern void far selectcar(I8 *, I8 *, I8 *, I16);
#include "video.h"
/* Local view: messagebox; renderer retains its excluded unsigned return prototype. */
extern I16 far messagebox();
#include "timer.h"
#include "track.h"

/* Local view: ADD_OBJECT_TO_BUFFER; return is unsigned word; another unit uses void. */
extern U16  far ADD_OBJECT_TO_BUFFER(struct TRANSFORMEDSHAPE3D *);

/* Local view: update_car_speed; parameters[0] is signed word; another unit uses signed byte. */
extern void far doengine(I16, I16, struct CARSTATE *, struct SIMD *);

#include "mousebuf.h"
/* Local views precede the shared UI interfaces to preserve communal first sightings. */
int unused_count;
int pixel_scales;
I8 menutimeout;
#include "viewstat.h"
unsigned char entry_score_index;
char results_entryname[17];
#include "dlgcolor.h"
static short end_hiscore_random;

void far *fntndat;
struct GAMESTATE_SNAPSHOT race_stats;
char buf_g_path[94];

struct GAMEINFO globalgamesettings;
char textstr[40];
char g_gsnashape_data[5];

void far *g_miscfile_ptr;
void far *resource_ptropp;
void far *g_opp_resources[7];



#include "textbuf.h"
char resbuftext[80];
struct HighScoreRecord far *hscore_trk11_ptr;

char opptext_label[3];
int waitm_ms;
struct SPRITE far *g_wndspr;

static short hiscore_rank_prev;
static short hiscore_old_entry;
static short hiscore_opponent_earlier;
static short hiscore_current_place;
static short hiscore_opponent_live;
short scrorder_idxs[7];

/* Local view: SHAPE2D; struct.count is 6; another unit uses 10. */
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };

#include "matrix.h"
struct PLANE;
/* Shared simulation state definitions. */
unsigned char g_rpl_init;
long centerpos;
char g_plyr_snd_state;
int wall_facingang;
int rotxvehicle;
unsigned char test_pln;
unsigned char g_cursurfacekindvalue;
int g_penaltytm;
struct SIMD ophys_7;
long veh_position;
short g_cvxintvl;
short st_hdg;
unsigned char sigframe;
struct MATRIX matrix_transform_view;
char audio_frmarr[1360];
int g_audio_frms_ix;
char intraffic;
short g_sgateopn;
unsigned short tmr2;
char inrepflg;
char tagtrk;
int nextpos_normalip;
int wallanchor_z;
int g_planidx2;
int frwhl_angadjusted;
int viewyshift;
struct PLANE far* plncurrptr;
char audiooppflag;
short x_course_part;
short wallanchor_x;
char gm_playmode;
int op_eng_sound_id;
int g_player_sound_id;
struct PLANE far* g_planlist;
int g_trackpiecescounter;
long veh_z;
short road_num;
int pln_rotate_z;
char *table_lookup;
short frmcs_time;
struct VECTOR pln_rot_output;
struct VECTOR tvec2;
short hgthgt;
int car_rotate_xc;
int car_roty_pln;
short element_min_wall;
char sndpendingstate;
char idxtrk;
short wall_wallelement;
short pl_i;
int anglerotate_car;
char g_hillf;
int yrotrotveh;
short sndposrecord;
short road_elem_ctrz;
char pen_flag_count;
unsigned char opponent_spd_tbl[16];
#include "audio.h"
#include "chartab.h"

#define SCREEN_WIDTH_PIXELS 0x140
#define SCREEN_HEIGHT_PIXELS 0xC8
#define TRACK_MAP_PAIR_BYTES 0x70A
#define ANGLE_TURN_SHIFT 10


struct GAMEINFO gmconfigbackup;

void far *def_fntadr;

short track3_gap[2];
I16S far *td3;
U16S  far *td6_ptr_b;

#include "signbuf.h"

struct SPRITE far *savedptr_ms;

char far *coursedataappend_address;
unsigned char far *trkd23adr;
char pass_check_flag;
void far *loadedresourceptr;

struct RECTANGLE *active_ui_dirty_rect = 0;
struct RECTANGLE *rectp = 0;
I8 track_file[82] = {0};
I8 replay_file[82] = {0};
I8 ReplayName[10] = "DEFAULT";
I8 TerName[5][9] = { "desert", "tropical", "alpine", "city", "country" };
I16 terheight[2] = { 0, 450 };
I16 camlen = 210;
I16 camheading = 464;
I16 campitch = 80;
I8 mousedrive = 0;
I8 is_audioloaded = 0;
I8 HKeyFlag = 0;
I8 cammd = 0;
I8 skybox_loaded = 0;
I8 mouseonstat = 0;
I8 kbormouse = 0;
I8 mouseonscreen = 0;
U8  renderlevel = 0;
I8 g_is_busy = 0;
U8  mouse_buffer_count = 0;
I8 aKevin[] = "kevin";
I8 aOpp1[] = "opp1";
I8 aCarcoun[] = "carcoun";
/* Coordinates startup, the main menu, and shutdown. */

I16 main(I16 argc, I8 *argv[])
{
  register I16 index;
  I8 menuFlag;
  register I16 result;
  I8 far *trackBlock;
  initmodes(argc, argv);
  initialize_div0();
  for (index = 0; index < 30; ++index)
  {
    lnoffsets[index] = 30 * (29 - index);
    gterrtrk[index] = 30 * index;
    r_zp[index] = 29 - index << 10;
    row_ctr_zs[index] = (29 - index << ANGLE_TURN_SHIFT) + ANGLE_HALF_TURN;
    postable[index] = index << 10;
    z_ctr_pos[index] = (index << ANGLE_TURN_SHIFT) + ANGLE_HALF_TURN;
  }
  for (index = 0; index < 30; ++index)
  {
    xcols[index] = index * 1024u;
    trackctrpos2[index] = index * 1024 + 512;
  }
  main_data_file_addr = openresource("main");
  def_fntadr = myload(0, "fontdef.fnt");
  fntndat = myload(0, "fontn.fnt");
  fontsetfontdef();
  INIT_MATRIX();
  index = TRACK_ARENA_BYTES;
  trackBlock = resmem("trakdata", index);
  g_td01_track_filecpy = (I16S far *) trackBlock;
  trackBlock += TRACK_MAP_PAIR_BYTES;
  trackdata_penalty_related = (I16S far *) trackBlock;
  trackBlock += TRACK_MAP_PAIR_BYTES;
  td3 = trackBlock;
  trackBlock += TRACK_MAP_PAIR_BYTES;
  track04_plyraero = (I16S far *) trackBlock;
  trackBlock += 0x80;
  trackdata_05_opp_aerotbl = (I16S far *) trackBlock;
  trackBlock += 0x80;
  td6_ptr_b = trackBlock;
  trackBlock += 0x80;
  trackdat7 = trackBlock;
  trackBlock += 0x80;
  g_td08d = (I16 far *) trackBlock;
  trackBlock += 0x60;
  trkptrpath = (I16 far *) trackBlock;
  trackBlock += 0x180;
  trksign = (I16 far *) trackBlock;
  trackBlock += 0x120;
  hscore_trk11_ptr = (struct HighScoreRecord far *) trackBlock;
  trackBlock += HIG_TABLE_BYTES;
  savedptr_ms = trackBlock;
  trackBlock += 0xF0;
  td13_replay_hdr = trackBlock;
  trackBlock += 0x1A;
  td14tb = (U8  far *) trackBlock;
  trackBlock += 0x385;
  td15p_9 = (U8  far *) trackBlock;
  trackBlock += 0x385;
  g_tdreplay16buf = trackBlock;
  trackBlock += 0x2EE0;
  road_trk = trackBlock;
  trackBlock += 0x385;
  td_18_ref = trackBlock;
  trackBlock += 0x385;
  td19hdl = (U8  far *) trackBlock;
  trackBlock += 0x385;
  coursedataappend_address = trackBlock;
  trackBlock += 0x7AC;
  g_column_of_trkdata21_pth = trackBlock;
  trackBlock += 0x385;
  tdfrompathrow22 = trackBlock;
  trackBlock += 0x385;
  trkd23adr = (U8  far *) trackBlock;
  /* MSC 5.10: retain the final, otherwise unused arena advance. */
  trackBlock += 0x30;
  initworld();
  mysetseeds(aKevin);
  strcpy(globalgamesettings.game_trackname, "DEFAULT");
  /* MSC 5.10: this overwritten assignment affects object output. */
  index = 0;
  hurrykey(1);
  hurrykey(1);
  mouseoff();
  kbormouse = 0;
  pass_check_flag = 0;
  initselections();
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
      makename(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path);
      loadfileat(buf_g_path, td14tb);
    }
    menutimeout = 0;
    result = run_intro_looped();
    if (result == 27)
      continue;
  show_menu:
    ensure_file_exists(2);
    if (is_audioloaded == 0)
      file_load_audio_resource("skidslct", "skidms", "SLCT");
    switch (select())
    {
      case UI_MAIN_TRACK:
        teditmain(0);
        goto show_menu;
      case UI_MAIN_OPPONENT:
        clear_keyboardbuffer();
        loading();
        selectopponent();
        goto show_menu;
      case UI_MAIN_OPTIONS:
        clear_keyboardbuffer();
        loading();
        if (run_option_menu() == 0)
          goto show_menu;
        menuFlag = 1;
        goto do_game;
      case UI_MAIN_CAR:
        clear_keyboardbuffer();
        loading();
        selectcar(globalgamesettings.game_playercarid, &globalgamesettings.game_playermaterial, &globalgamesettings.game_playertransmission, 0);
        goto show_menu;
      case UI_MAIN_START:
        menuFlag = 0;
      do_game:
        gmconfigbackup = globalgamesettings;
        for (index = 0; index < TRACK_MAP_PAIR_BYTES; ++index)
          coursedataappend_address[index] = td14tb[index];
        for (index = 0; index < UI_FILENAME_BACKUP_BYTES; ++index)
        {
          coursedataappend_address[index + TRACK_MAP_PAIR_BYTES] = track_file[index];
          coursedataappend_address[index + TRACK_ARENA_REPLAY_PATH_OFFSET] = replay_file[index];
        }
        if (menutimeout == 0)
        {
          if ((I8)compiletrack() != 0)
          {
            teditmain(1);
            goto show_menu;
          }
          seedrandom();
          if (pass_check_flag == 0)
            getprotection((I8)(drnd() % 20));
        }
        else if (findfirst("tedit.*") == 0)
          goto prepare_intro;
        else
          goto init_replay;
      init_replay:
        audio_unload();
        cvxs_a = resmem("cvx", 22400);
        initsim(-1);
        if (menuFlag != 0)
          didrace = 0;
        else
          globalgamesettings.game_recordedframes = 0;
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
      loading();
      run_game();
      if (menutimeout == 0 && didrace != 0)
      {
        switch (evaluation())
        {
          case 0:
            didrace = 4;
            continue;
          case 1:
            globalgamesettings.game_recordedframes = 0;
            continue;
        }
      }
      break;
    }
    globalgamesettings = gmconfigbackup;
    for (index = 0; index < TRACK_MAP_PAIR_BYTES; ++index)
      td14tb[index] = coursedataappend_address[index];
    for (index = 0; index < UI_FILENAME_BACKUP_BYTES; ++index)
    {
      track_file[index] = coursedataappend_address[index + TRACK_MAP_PAIR_BYTES];
      replay_file[index] = coursedataappend_address[index + TRACK_ARENA_REPLAY_PATH_OFFSET];
    }
    purgememory(cvxs_a);
    if (menutimeout != 0)
      goto do_intro0;
    goto show_menu;
  } while (messagebox(2, 1, getresl(main_data_file_addr, "dos"), UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0) < 1);
  mouseoff();
  restoretimer();
  audiodrv_atexit();
  keyboard_exit_handler();
  keyboard_shift_checking1();
  video_set_mode7();
}

/* Runs the title and credits presentation sequence. */
I16 far run_intro_looped(void)
{
    register I16 inputResult;
    file_load_audio_resource("skidtitl", "skidms", "TITL");
    loadedresourceptr = myload(2, "sdtitl");
    g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
    inputResult = run_intro();
    removewindow(g_wndspr);
    freememory(loadedresourceptr);
    if (inputResult == 0) {
        inputResult = logo();
        if (inputResult == 0) {
            loadedresourceptr = myload(2, "sdcred");
            g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
            usebufferc();
            sprite_blit_to_video(g_wndspr, 0);
            inputResult = load_intro_resources();
            removewindow(g_wndspr);
            freememory(loadedresourceptr);
        }
    }
    audio_unload();
    return inputResult;
}

/* Presents the production and title screens. */

I16 far run_intro(void)
{
    register I16 inputResult;
    mouseoff();
    usescreenc();
    mouseon();
    usebufferc();
    if (locate_shape_fatal(loadedresourceptr, "prod")->pos_y != 0)
        waitm_ms = 0xa0;
    else
        waitm_ms = 0xb4;
    movf(locate_shape_fatal(loadedresourceptr, "prod"));
    inputResult = sprite_blit_to_video(g_wndspr, 0xffff);
    if (inputResult == 0) {
        inputResult = mykeywait(0x190);
        if (inputResult == 0) {
            usebufferc();
            waitm_ms = 0xb4;
            movf(locate_shape_fatal(loadedresourceptr, "titl"));
            inputResult = sprite_blit_to_video(g_wndspr, 0xffff);
            if (inputResult == 0)
                inputResult = mykeywait(0x190);
        }
    }
    return inputResult;
}

/* Loads and animates the intro presentation resources. */

I8S  far load_intro_resources(void)
{
  I16S imageWidth;
  I16S waitLimit;
  I8 far *resources[12];
  register I16 elapsed;
  I16S scaledWidth;
  register I16 picture;
  I8 far *introFile;
  I16S step;
  I16S timerStep;
  I16 inputResult;
  I16S displayHeight;
  introFile = openresource("cred");
  locateshapes(loadedresourceptr, "arowarrwarw1arw2arw3arw4arw5arw6arw7arw8type", resources);
  waitm_ms = 0x96;
  usebufferc();
  imageWidth = ((struct SHAPE2D far *) resources[1])->pos_x;
  waitLimit = ((struct SHAPE2D far *) resources[1])->pos_y;
  scaledWidth = ((struct SHAPE2D far *) resources[1])->width * pixel_scales;
  displayHeight = ((struct SHAPE2D far *) resources[1])->height;
  strcpyfar(resbuftext, getresl(introFile, "cre"));
  shadowtext(resbuftext, 0x78, 0, intro_text_color_b, intro_text_style_b);
  strcpyfar(resbuftext, getres(introFile, "gds0"));
  shadowtext(resbuftext, 0x3c, 0x0c, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gds1"));
  shadowtext(resbuftext, 0x68, 0x14, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getresl(introFile, "des"));
  shadowtext(resbuftext, 0x14, 0x20, intro_text_color_c, intro_text_style_c);
  strcpyfar(resbuftext, getres(introFile, "gdon"));
  shadowtext(resbuftext, 0x14, 0x2c, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gkev"));
  shadowtext(resbuftext, 0x14, 0x34, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gbra"));
  shadowtext(resbuftext, 0x14, 0x3c, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "grob"));
  shadowtext(resbuftext, 0x14, 0x44, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gsta"));
  shadowtext(resbuftext, 0x14, 0x4c, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getresl(introFile, "mus"));
  shadowtext(resbuftext, 0x14, 0x5c, intro_text_color_f, intro_text_style_f);
  strcpyfar(resbuftext, getres(introFile, "gmsy"));
  shadowtext(resbuftext, 0x14, 0x68, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gkri"));
  shadowtext(resbuftext, 0x14, 0x70, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gbri"));
  shadowtext(resbuftext, 0x14, 0x78, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getresl(introFile, "pro"));
  shadowtext(resbuftext, 0xac, 0x20, intro_text_color_d, intro_text_style_d);
  strcpyfar(resbuftext, getres(introFile, "gkev"));
  shadowtext(resbuftext, 0xac, 0x2c, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getresl(introFile, "opr"));
  shadowtext(resbuftext, 0xac, 0x38, intro_text_color_d, intro_text_style_d);
  strcpyfar(resbuftext, getres(introFile, "gbra"));
  shadowtext(resbuftext, 0xac, 0x40, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gric"));
  shadowtext(resbuftext, 0xac, 0x48, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getresl(introFile, "art"));
  shadowtext(resbuftext, 0xac, 0x54, intro_text_color_e, intro_text_style_e);
  strcpyfar(resbuftext, getres(introFile, "gmsm"));
  shadowtext(resbuftext, 0xac, 0x60, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gdav"));
  shadowtext(resbuftext, 0xac, 0x68, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gnic"));
  shadowtext(resbuftext, 0xac, 0x70, intro_text_color_a, intro_text_style_a);
  strcpyfar(resbuftext, getres(introFile, "gkev"));
  shadowtext(resbuftext, 0xac, 0x78, intro_text_color_a, intro_text_style_a);
  closeresource(introFile);
  sprite_blit_to_video(g_wndspr, 0xffff);
  usescreen();
  elticks();
  elapsed = 0x14a;
  for (;;)
  {
    timerStep = elticks();
    elapsed -= timerStep << 1;
    if (imageWidth > elapsed)
      break;
    mouseoff();
    movcfxya(resources[1], elapsed, waitLimit);
    drawrectc(scaledWidth + elapsed, waitLimit, 0x20, displayHeight, 0);
    mouseon();
    inputResult = hurrykey(timerStep);
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
    usebuffer();
    clippix(0, SCREEN_WIDTH_PIXELS, waitLimit, SCREEN_HEIGHT_PIXELS);
    clearwindow(0);
    movf(resources[picture]);
    usescreen();
    clippix(0, SCREEN_WIDTH_PIXELS, waitLimit, SCREEN_HEIGHT_PIXELS);
    mouseoff();
    movcf(g_wndspr->image);
    mouseon();
    step += 5;
    while (step > elapsed)
    {
      timerStep = elticks();
      inputResult = hurrykey(timerStep);
      elapsed += timerStep;
    }
  }
  clippix(0, SCREEN_WIDTH_PIXELS, 0, SCREEN_HEIGHT_PIXELS);
  mouseoff();
  readf(g_wndspr->image);
  usebuffer();
  clippix(0, SCREEN_WIDTH_PIXELS, waitLimit, SCREEN_HEIGHT_PIXELS);
  clearwindow(0);
  movf(resources[0]);
  movf(resources[10]);
  inputResult = sprite_blit_to_video(g_wndspr, 0);
  if (inputResult != 0 || mykeywait(0x1f4) != 0)
    return 1;
  return 0;
}

I8S  menu_left[6] = { 1, 2, 4, 0, 3, 0 };
I8S  menu_right[6] = { 3, 0, 1, 4, 2, 0 };
I16 menu_buttons_x1[UI_MAIN_MENU_BUTTON_COUNT] = { 105, 66, 5, 190, 255 };
I16 menu_buttons_x2[UI_MAIN_MENU_BUTTON_COUNT] = { 208, 107, 67, 253, 312 };
I16 menu_buttons_y1[UI_MAIN_MENU_BUTTON_COUNT] = { 119, 77, 114, 76, 116 };
I16 menu_buttons_y2[UI_MAIN_MENU_BUTTON_COUNT] = { 197, 120, 170, 122, 166 };
/* Draws and processes the main menu. */

I8 far select(void)
{
    void far *data_resource;
    I16 mouseDelta;
    I8S  drawMode;
    I16 keyCode;
    I8S  hit;
    I8S  oldSelection;
    I8S  selection;

    drawMode = -1;
    selection = 0;
    oldSelection = -1;
    loading();
    waitm_ms = 180;
    g_wndspr = sprite_make_window(320, 200, 15);

    data_resource = myload(2, "sdmsel");
    usebuffer();
    movf(locate_shape_fatal(data_resource, "scrn"));
    freememory(data_resource);

    for (;;) {
        if (selection != oldSelection) {
            oldSelection = selection;
            usebuffer();
            sprite_blit_to_video(g_wndspr, drawMode);
            drawMode = -2;
            usescreen();
            docursorinit();
        }

        mouseDelta = docursor(selection,
            menu_buttons_x1, menu_buttons_x2,
            menu_buttons_y1, menu_buttons_y2, menu_hover_color_a, menu_hover_color_b);
        keyCode = mykey(mouseDelta);
        hit = mousecursor(5, menu_buttons_x1, menu_buttons_x2,
                                  menu_buttons_y1, menu_buttons_y2);
        if (hit != -1)
            selection = hit;

        unused_count += mouseDelta;
        if (unused_count > UI_MAIN_IDLE_TIMEOUT) {
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
            selection = UI_MAIN_CANCEL;
            goto menu_done;
        case 13:
        case 32:
            goto menu_done;
        }
    }

menu_done:
    removewindow(g_wndspr);
    return selection;
}

I16S trackmenu_buttons_x1[UI_TRACK_MENU_BUTTON_COUNT] = { 16, 112, 208 };
I16S trackmenu_buttons_x2[UI_TRACK_MENU_BUTTON_COUNT] = { 112, 208, 304 };
I16S trackmenu_buttons_y1[UI_TRACK_MENU_BUTTON_COUNT] = { 171, 171, 171 };
I16S trackmenu_buttons_y2[UI_TRACK_MENU_BUTTON_COUNT] = { 197, 197, 197 };
/* Presents the track list and handles track selection. */

void far teditmain(I16 restart)
{
    I8 trackSelection;
    I8 lastSelection;
    register I16 dialogRes;
    I16 keyCodePressed;
    I16 timerDiff;
    I8 choice;
    I8 displayState;
    I8 offsets[4];
    void far *trkEdit;

    ensure_file_exists(3);
    if (restart != 0)
        goto restart_game;

preview:
    displayState = -1;
    trackSelection = 0;
    lastSelection = -1;
    loading();
    waitm_ms = 0x9b;
    g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
    initscenery((U8 )td14tb[0x384]);
    init_3dpieces();
    setprojectp(0x28, 0x28, SCREEN_WIDTH_PIXELS, 0xc8);
    initsim(-2);
    usebuffer();
    clearwindow(ground_skybox);
    clippix(0, SCREEN_WIDTH_PIXELS, 0, SCREEN_HEIGHT_PIXELS);
    drawblimpworld();
    remove_3dpieces();
    removescenery();
    usebuffer();
    strcpy(resbuftext, "'");
    strcat(resbuftext, globalgamesettings.game_trackname);
    strcat(resbuftext, "'");
        shadowtext(resbuftext, mycenter(resbuftext), 6,
                        dlg_colour, 0);

    if (readhighs(0) == 0) {
        if (hscore_trk11_ptr[scrorder_idxs[0]].marker != HIG_EMPTY_TIME) {
        strcpyfar(resbuftext, getresl(main_data_file_addr, "hs0"));
        shadowtext(resbuftext, mycenter(resbuftext), 0x12,
                        dlg_colour, 0);
        mysetfont(fntndat);
        genhline(0, offsets);
        textcolor(0, 0);
        fortextxy(resbuftext + offsets[0], 16, 30);
        fortextxy(resbuftext + offsets[1], UI_HIG_CAR_COLUMN_X, 30);
        fortextxy(resbuftext + offsets[2], 224, 30);
        fortextxy(resbuftext + offsets[3], 272, 30);
        fontsetfontdef();
        }
    }

    trkEdit = openresource("tedit");
    draw3dbutton(getresl(trkEdit, "bmt"), 0x11, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    draw3dbutton(getresl(trkEdit, "bet"), 0x71, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    draw3dbutton(getresl(trkEdit, "bmm"), 0xd1, 0xac, 0x5e, 0x18,
                menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
    closeresource(trkEdit);

menu_loop:
    if (trackSelection != lastSelection) {
        lastSelection = trackSelection;
        sprite_blit_to_video(g_wndspr, displayState);
        displayState = -2;
        usescreen();
        docursorinit();
    }

    timerDiff = docursor(trackSelection,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2,
        menu_hover_color_a, menu_hover_color_b);
    unused_count += timerDiff;
    if (unused_count > UI_MAIN_IDLE_TIMEOUT) {
        unused_count = 0;
        ++menutimeout;
    }
    keyCodePressed = mykey(timerDiff);
    choice = mousecursor(3,
        trackmenu_buttons_x1, trackmenu_buttons_x2,
        trackmenu_buttons_y1, trackmenu_buttons_y2);
    if (choice != -1)
        trackSelection = choice;
    if (menutimeout != 0) {
        trackSelection = UI_TRACKS_MENU_RETURN;
        keyCodePressed = 0x0d;
    }
    if (keyCodePressed == 0)
        goto menu_loop;
    switch (keyCodePressed) {
    case 0x4b00:
        if (trackSelection != 0)
            --trackSelection;
        else
            trackSelection = UI_TRACKS_MENU_RETURN;
        goto menu_loop;
    case 0x4d00:
        if (trackSelection < UI_TRACKS_MENU_RETURN)
            ++trackSelection;
        else
            trackSelection = 0;
        goto menu_loop;
    case 0x1b:
        trackSelection = UI_TRACKS_MENU_CANCEL;
    case 0x0d:
    case 0x20:
        break;
    default:
        goto menu_loop;
    }
    switch (trackSelection) {
    default:
        removewindow(g_wndspr);
        return;
    case UI_TRACKS_MENU_LOAD:
        dialogRes = floadreq(track_file, globalgamesettings.game_trackname,
                                         ".trk",
                                         getresl(main_data_file_addr, "trk"));
        makename(track_file, globalgamesettings.game_trackname,
                        ".trk", buf_g_path);
        if (dialogRes != 0) {
            loadfileat(buf_g_path, td14tb);
            removewindow(g_wndspr);
            goto preview;
        }
        lastSelection = -1;
        goto menu_loop;
    case UI_TRACKS_MENU_EDIT:
        removewindow(g_wndspr);
    restart_game:
        clear_keyboardbuffer();
        loading();
        waitm_ms = 0x82;
        compiletrack();
        trackedit();
        goto preview;
    }
}

/* Loads or creates the track-specific high-score table. */
I8 far readhighs(I16 create_defaults)
{
  void far *loaded_data;
  I16 index;
  struct HighScoreRecord row;
  entry_score_index = -1;
  for (index = 0; index < 7; ++index)
    scrorder_idxs[index] = index;

  makename(track_file, globalgamesettings.game_trackname, ".hig", buf_g_path);
  if (create_defaults == 0)
  {
    g_is_busy = 1;
    loaded_data = read_file_with_retry(10, buf_g_path, hscore_trk11_ptr);
    g_is_busy = 0;
    if (loaded_data == 0)
    {
    /* MSC 5.10: retain these unreferenced return labels. */
    score_file_missing:
      return 1;
    }
  score_file_found:
    return 0;
  }
  strcpy((I8 *) row.bytes, "....................");
  strcpy(((I8 *) row.bytes) + HIG_RECORD_OFFSET_CAR_TEXT, ".......................");
  row.bytes[HIG_RECORD_OFFSET_CAR_STYLE] = 0;
  strcpy(((I8 *) row.bytes) + HIG_RECORD_OFFSET_OPPONENT_TEXT, "../....");
  row.marker = HIG_EMPTY_TIME;
  for (index = 0; index < 7; ++index)
    hscore_trk11_ptr[index] = row;
  index = savefilez(buf_g_path, hscore_trk11_ptr, HIG_TABLE_BYTES);
  if (index != 0)
    return 1;
  return 0;
}

/* Draws the high-score list. */

void far drawhighs(void)
{
    I16 row_color;
    I16 row_top;
    I8 offsets[4];
    I8 row_index;

    usebuffer();

    strcpyfar(resbuftext, getresl(main_data_file_addr, "hs1"));
    strcat(resbuftext, " '");
    strcat(resbuftext, globalgamesettings.game_trackname);
    strcat(resbuftext, "'");
    hiscore_draw_text(resbuftext, mycenter(resbuftext),
                      5, dlg_colour, 0);

    strcpyfar(resbuftext, getresl(main_data_file_addr, "hs2"));
    hiscore_draw_text(resbuftext, 16, 15, dlg_colour, 0);
    strcpyfar(resbuftext, getresl(main_data_file_addr, "hs3"));
    hiscore_draw_text(resbuftext, UI_HIG_CAR_COLUMN_X, 15, dlg_colour, 0);
    strcpyfar(resbuftext, getresl(main_data_file_addr, "hs5"));
    hiscore_draw_text(resbuftext, 224, 15, dlg_colour, 0);
    strcpyfar(resbuftext, getresl(main_data_file_addr, "hs4"));
    hiscore_draw_text(resbuftext, 272, 15, dlg_colour, 0);

    mysetfont(fntndat);
    row_index = 0;
    goto row_check;
row_normal:
    row_color = 0;
row_draw:
    row_top = row_index * UI_HIG_ROW_HEIGHT + UI_HIG_FIRST_ROW_Y;
    textcolor(row_color, 0);
    fortextxy(resbuftext + offsets[0], 16, row_top);
    fortextxy(resbuftext + offsets[1], UI_HIG_CAR_COLUMN_X, row_top);
    fortextxy(resbuftext + offsets[2], 224, row_top);
    fortextxy(resbuftext + offsets[3], 272, row_top);
    ++row_index;
row_check:
    if (row_index >= 7)
        goto row_done;
    genhline(row_index, offsets);
    if (row_index != entry_score_index)
        goto row_normal;
    row_color = dialog_frame_color;
    goto row_draw;
row_done:
    fontsetfontdef();
}

/* Draws one formatted high-score entry. */

void far genhline(I16 rowIndex, I8 *stringOffsets)
{
    I8 timeText[18];
    I8 textLength;
    I16 frameRate;
    struct HighScoreRecord scoreRecord;

    scoreRecord = hscore_trk11_ptr[scrorder_idxs[rowIndex]];
    stringOffsets[0] = 0;
    strcpy(resbuftext, (I8 *)scoreRecord.bytes);
    textLength = strlen(resbuftext) + 1;
    stringOffsets[1] = textLength;
    strcpy(resbuftext + textLength, (I8 *)scoreRecord.bytes + HIG_RECORD_OFFSET_CAR_TEXT);
    textLength += strlen(resbuftext + textLength) + 1;
    stringOffsets[2] = textLength;
    resbuftext[textLength] = 0;
    if (scoreRecord.bytes[HIG_RECORD_OFFSET_CAR_STYLE] == 1)
        strcat(resbuftext + textLength, "(");
    strcat(resbuftext + textLength, (I8 *)scoreRecord.bytes + HIG_RECORD_OFFSET_OPPONENT_TEXT);
    if (scoreRecord.bytes[HIG_RECORD_OFFSET_CAR_STYLE] == 1)
        strcat(resbuftext + textLength, ")");
    textLength += strlen(resbuftext + textLength) + 1;
    frameRate = rate_frame;
    rate_frame = HIG_CANONICAL_FPS;
    if (scoreRecord.marker != HIG_EMPTY_TIME)
        maketime(timeText, scoreRecord.marker, 1);
    else
        maketime(timeText, 0, 1);
    stringOffsets[3] = textLength;
    strcpy(resbuftext + textLength, timeText);
    rate_frame = frameRate;
}

/* Inserts and presents a qualifying high score. */

void far addhighs(U16S  score, I8 far *text, U8  carStyle)
{
    I8 selectedIndex;
    I8 insertionIndex;
    struct HighScoreRecord current;
    I16 dialogX;
    I16 dialogY;

    if (rate_frame == 10)
        score <<= 1;

    if (score < hscore_trk11_ptr[6].marker) {
        for (insertionIndex = 0; hscore_trk11_ptr[insertionIndex].marker <= score && insertionIndex < 7; ++insertionIndex)
            scrorder_idxs[insertionIndex] = insertionIndex;
        selectedIndex = insertionIndex;
        entry_score_index = insertionIndex;
        while (insertionIndex < 6) {
            scrorder_idxs[insertionIndex + 1] = insertionIndex;
            ++insertionIndex;
        }
        scrorder_idxs[selectedIndex] = 6;

        current.marker = score;
        current.bytes[0] = 0;
        strcpy((I8 *)current.bytes + HIG_RECORD_OFFSET_CAR_TEXT, textstr);
        current.bytes[HIG_RECORD_OFFSET_CAR_STYLE] = carStyle;
        if (globalgamesettings.game_opponenttype != 0) {
            strcpy((I8 *)current.bytes + HIG_RECORD_OFFSET_OPPONENT_TEXT, opptext_label);
            current.bytes[HIG_RECORD_OFFSET_OPPONENT_SEPARATOR] = '/';
            strcpy((I8 *)current.bytes + HIG_RECORD_OFFSET_OPPONENT_CAR_TEXT, g_gsnashape_data);
        } else {
            strcpy((I8 *)current.bytes + HIG_RECORD_OFFSET_OPPONENT_TEXT, " ");
        }
        hscore_trk11_ptr[6] = current;

        usebuffer();
        drawhighs();
        sprite_blit_to_video(g_wndspr, -1);
        messagebox(3, 0, text, -1, -1,
                    dialog_frame_color, &dialogY, 0);
        clear_keyboardbuffer();
        instringz(results_entryname, UI_HIG_NAME_LIMIT, dialogY, dialogX, 30000);
        strcpy((I8 *)current.bytes, results_entryname);
        hscore_trk11_ptr[6] = current;

        usebuffer();
        drawhighs();
        sprite_blit_to_video(g_wndspr, -1);
        savehighs();
    }

    drawhighs();
}

/* Writes the ordered high-score table. */

void far savehighs(void)
{
    I16 index;
    struct HighScoreRecord orderedScores[7];

    for (index = 0; index < 7; ++index)
        orderedScores[index] = hscore_trk11_ptr[scrorder_idxs[index]];

    makename(track_file, globalgamesettings.game_trackname, ".hig", buf_g_path);
    g_is_busy = 1;
    savefilez(buf_g_path, orderedScores, HIG_TABLE_BYTES_U32);
    g_is_busy = 0;
}

struct RECTANGLE carmenu_cliprect = { 0, 320, 0, 95 };
I16 carmenu_buttons_y1[UI_CAR_MENU_BUTTON_COUNT] = { 229, 229, 229, 229, 229 };
I16 carmenu_buttons_y2[UI_CAR_MENU_BUTTON_COUNT] = { 316, 316, 316, 316, 316 };
I16 carmenu_buttons_x1[UI_CAR_MENU_BUTTON_COUNT] = { 107, 125, 143, 161, 179 };
I16 carmenu_buttons_x2[UI_CAR_MENU_BUTTON_COUNT] = { 124, 142, 160, 178, 196 };
I8 aLnam[6] = "lnam";
struct RECTANGLE car_menu_clip_rect = { 0, 320, 0, 0 };
struct VECTOR car_menu_car_position = { 0, -840, 2880 };
/* Presents car choices and updates the selected car settings. */

/* Local view: selectcar; parameters[0].pointer is signed byte; another unit uses struct GAMEINFO. */
void far selectcar(I8 *caridptr, I8 *materialofs,
                      I8 *transmissionofs, I16 opponenttype)
{
  void far *sdcselPos;
  struct TRANSFORMEDSHAPE3D transformedCopy;
  I16 rotation;
  I8 textch;
  struct RECTANGLE unionRectY;
  I16 oldFramesPerSecond;
  struct RECTANGLE localClipId;
  I16 carSpeedY;
  I8 runStateNew;
  void far *carres;
  I8 far *buttonText;
  I8 previousCarIndexCar;
  I16 rotationDeltaY;
  I16 graphIndexY;
  U16  graphYX;
  I8 carCountLast;
  U16  innerIndexOld;
  struct SPRITE far *opponentWindowX;
  I8 blitColorY;
  I8 carids[32][5];
  I8 carIndexY;
  I16 anglePos;
  I8 hitButtonX;
  I8 *findfileX;
  I8 previousButton;
  I8 far *descriptionCar;
  struct RECTANGLE fullClip;
  I8 selectedButtonY;
  void far *opponentShapeCopy;
  I8 canLeave;
  register I16 keyCode;
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
    findfileX = myfindnext();
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
  blitColorY = -1;
  backlightovr8 = 0x2d;
  sdcselPos = file_load_shape2d_fatal_thunk("sdcsel");
  if (opponenttype == 0)
    g_miscfile_ptr = openresource("misc");
  if (opponenttype != 0)
  {
    car_menu_clip_rect.right = 0xf0;
    if (g_videoflg5 != 0)
    {
      opponentShapeCopy = g_opp_resources[opponenttype];
      opponentWindowX = sprite_make_window(((struct OPPONENTIMAGE far *) opponentShapeCopy)->width, ((struct OPPONENTIMAGE far *) opponentShapeCopy)->height, 0x0f);
      setup_mcgawnd2();
      clearwindow(0);
      nullsub_2(resource_ptropp, (I8) (opponenttype + '0'));
      tmaskxcfxya(g_opp_resources[opponenttype], 0, 0);
      readfxya(opponentWindowX->image, 0, 0);
    }
  }
  else
  {
    car_menu_clip_rect.right = SCREEN_WIDTH_PIXELS;
  }
  previousCarIndexCar = -1;
  rotation = 0;
  selectedButtonY = 0;
  docursorinit();
  rotationDeltaY = 0;
  previousButton = -1;
  setprojectp(0x24, 0x11, SCREEN_WIDTH_PIXELS, HALF_SCREEN_HEIGHT_PIXELS);
  elticks();
  g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
  for (;;)
  {
    /* MSC 5.10: deleting this unreferenced loop label changes the object. */
    menu_loop:
    if (carIndexY != previousCarIndexCar)
    {
      if (previousCarIndexCar != -1)
      {
        closeresource(carres);
        remove_3dcars();
      }
      init_3dcars(carids[carIndexY], globalgamesettings.game_opponentcarid);
      aCarcoun[3] = carids[carIndexY][0];
      aCarcoun[4] = carids[carIndexY][1];
      aCarcoun[5] = carids[carIndexY][2];
      aCarcoun[6] = carids[carIndexY][3];
      carres = openresource(aCarcoun);
      getsimdata(carres, 0);
      usebufferc();
      draw3dbutton(0, 0, 0x67, SCREEN_WIDTH_PIXELS, 0x61, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw3dbutton(0, 5, 0x6d, 0x46, 0x55, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw3dbutton(0, 0x52, 0x6d, 0x8c, 0x55, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      movf(locate_shape_fatal(sdcselPos, "grap"));
      mysetfont(fntndat);
      textcolor(0, dlg_colour);
      fortextxy("150", 9, 0x73);
      fortextxy("100", 9, 0x87);
      fortextxy(" 50", 9, 0x9b);
      fortextxy("  0", 9, 0xaf);
      fortextxy("0  20  40", 0x1a, 0xb9);
      fontsetfontdef();
      draw3dbutton(getresl(g_miscfile_ptr, "bdo"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[0] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw3dbutton(getresl(g_miscfile_ptr, "bnx"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[1] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw3dbutton(getresl(g_miscfile_ptr, "bla"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[2] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      buttonText = getresl(g_miscfile_ptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
      draw3dbutton(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      draw3dbutton(getresl(g_miscfile_ptr, "bco"), carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[4] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      oldFramesPerSecond = rate_frame;
      rate_frame = 20;
      initsim(-2);
      core.playerstate.car_transmission = 1;
      graphIndexY = 0;
      for (;;)
      {
        doengine(1, 0, &core.playerstate, &simdp7);
        carSpeedY = core.playerstate.car_speed >> Q8_FRACTION_BITS;
        graphYX = -((I16) ((((U32 ) carSpeedY) << 6) / 150 - 0xb5));
        if (graphYX >= 0x75)
        {
          innerIndexOld = ((U16 ) (0x26 * graphIndexY)) / 0x320 + 0x1c;
          writepixelc(innerIndexOld, graphYX, performGraphColor);
          ++graphIndexY;
          if (graphIndexY < 0x320)
            continue;
        }
        break;
      }

      rate_frame = oldFramesPerSecond;
      mysetfont(fntndat);
      descriptionCar = getresl(carres, "des");
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
            fortextxy(resbuftext, 0x58, graphYX);
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
      fontsetfontdef();
      elticks();
      previousButton = -1;
      fullClip.left = 0;
      fullClip.right = SCREEN_WIDTH_PIXELS;
      fullClip.top = 0;
      fullClip.bottom = SCREEN_HEIGHT_PIXELS;
      canLeave = 0;
      runStateNew = 3;
    }

    rotation += rotationDeltaY;
    switch (runStateNew)
    {
      case 0:
      case 3:
        anglePos = intatan(car_menu_car_position.y, car_menu_car_position.z);
        if (statemgmtcpy != 0)
          localClipId = empty_rectangle;
        else
          localClipId = carmenu_cliprect;
        INIT_OBJECTS_BUFFER(0, anglePos, 0, &carmenu_cliprect, 0);
        if (*materialofs >= (I8)g_shapes3d[124].shape3d_numpaints)
          *materialofs = 0;
        transformedCopy.rotvec.z = rotation;
        transformedCopy.material = *materialofs;
        ADD_OBJECT_TO_BUFFER(&transformedCopy);
        if (carIndexY == previousCarIndexCar)
          car_menu_clip_rect.bottom = 0x5f;
        else
          car_menu_clip_rect.bottom = SCREEN_HEIGHT_PIXELS;
        clipzone(&localClipId, &car_menu_clip_rect);
        mergezones(&localClipId, &fullClip, &unionRectY);
        if (runStateNew == 3)
          goto draw_full_car;
        runStateNew = 1;
        break;
      case 1:
      draw_full_car:
        runStateNew = 0;
        canLeave = 1;
        usebuffer();
        clippix(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        movcf(locate_shape_fatal(sdcselPos, "stop"));
        DRAW_OBJECTS();
        usebuffer();
        clippix(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        fullClip = localClipId;
        if (opponenttype != 0 && carIndexY != previousCarIndexCar)
        {
          usebuffer();
          if (g_videoflg5 == 0)
          {
            nullsub_2(resource_ptropp, (I8) (opponenttype + '0'));
            tmaskxcfxya(g_opp_resources[opponenttype], 0xf0, 0);
          }
          else
          {
            movcfxya(opponentWindowX->image, 0xf0, 0);
          }
        }
        usescreen();
        clippix(unionRectY.left, unionRectY.right, unionRectY.top, unionRectY.bottom);
        mouseoff();
        if (blitColorY != -2)
        {
          sprite_blit_to_video(g_wndspr, blitColorY);
          blitColorY = -2;
        }
        else
        {
          movcf(g_wndspr->image);
        }
        mouseon();
        previousCarIndexCar = carIndexY;
        break;
    }
    if (selectedButtonY != previousButton)
    {
      if (previousButton != -1)
      {
        usescreen();
        clippix(carmenu_buttons_y1[0], carmenu_buttons_y2[0] + g_vid_flg2_set & vidflg3is_minus1, carmenu_buttons_x1[0], carmenu_buttons_x2[4] + 1);
        mouseoff();
        movcf(g_wndspr->image);
        mouseon();
        usescreen();
      }
      docursorinit();
      previousButton = selectedButtonY;
    }

    usescreen();
    rotationDeltaY = docursor(selectedButtonY, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2, menu_hover_color_a, menu_hover_color_b);
    unused_count += rotationDeltaY;
    if (unused_count > UI_CAR_IDLE_TIMEOUT)
    {
      unused_count = 0;
      ++menutimeout;
    }
    keyCode = mykey(rotationDeltaY);
    hitButtonX = mousecursor(5, carmenu_buttons_y1, carmenu_buttons_y2, carmenu_buttons_x1, carmenu_buttons_x2);
    if (hitButtonX != -1)
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
          case UI_CAR_MENU_DONE:
            if (canLeave == 0)
              continue;
            removewindow(g_wndspr);
            closeresource(carres);
            remove_3dcars();
            if (opponenttype != 0 && g_videoflg5 != 0)
            removewindow(opponentWindowX);
            if (opponenttype == 0)
            closeresource(g_miscfile_ptr);
            freememory(sdcselPos);
            mouseoff();
            caridptr[0] = carids[carIndexY][0];
            caridptr[1] = carids[carIndexY][1];
            caridptr[2] = carids[carIndexY][2];
            caridptr[3] = carids[carIndexY][3];
            menutimeout = 0;
            return;

          case UI_CAR_MENU_NEXT:
            ++carIndexY;
            if (carIndexY == carCountLast)
              carIndexY = 0;
            continue;

          case UI_CAR_MENU_PREVIOUS:
            --carIndexY;
            if (carIndexY < 0)
              carIndexY = carCountLast - 1;
            continue;

          case UI_CAR_MENU_TRANSMISSION:
            *transmissionofs ^= 1;
            usebuffer();
            buttonText = getresl(g_miscfile_ptr, ((*transmissionofs) != 0) ? ("bau") : ("bma"));
            draw3dbutton(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            usescreen();
            mouseoff();
            draw3dbutton(buttonText, carmenu_buttons_y1[0] + 1, carmenu_buttons_x1[3] + 1, 0x56, 0x10, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            mouseon();
            continue;

          case UI_CAR_MENU_MATERIAL:
            ++(*materialofs);
            runStateNew = 3;
            continue;

          default:
            continue;

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

I16 opponentmenu_buttons_x1[UI_OPPONENT_MENU_BUTTON_COUNT] = { 20, 76, 132, 188, 244 };
I16 opponentmenu_buttons_x2[UI_OPPONENT_MENU_BUTTON_COUNT] = { 76, 132, 188, 244, 300 };
I16 opponentmenu_buttons_y1[UI_OPPONENT_MENU_BUTTON_COUNT] = { 177, 177, 177, 177, 177 };
I16 opponentmenu_buttons_y2[UI_OPPONENT_MENU_BUTTON_COUNT] = { 197, 197, 197, 197, 197 };
/* Presents and processes the opponent selection menu. */

void far selectopponent(void)
{
    I8 textch;
    I8 lastColor;
    I8 isLoaded;
    I16 textpos;
    register I16 key;
    I16 timeDelta;
    I16 y;
    void far *oppRes;
    I8 lastSelection;
    I8 button;
    I8 far *resourceText;
    I8 selectionIndex;
    I8 previousType;

    ensure_file_exists(4);
    g_miscfile_ptr = openresource("misc");
    resource_ptropp = myload(8, "sdosel");
    locateshapes(resource_ptropp, "opp0opp1opp2opp3opp4opp5opp6", g_opp_resources);

    selectionIndex = 0;
    isLoaded = 0;
    previousType = -1;
    lastColor = -1;
    docursorinit();
/* MSC 5.10: retain this label after replacing its jump with continue. */
menu_opponent_mouse_redraw:
    mouseon();

    for (;;) {
        if (previousType != globalgamesettings.game_opponenttype) {
            if (previousType != -1) {
                removewindow(g_wndspr);
                if (isLoaded != 0)
                    closeresource(oppRes);
            }

            ensure_file_exists(4);
            if (globalgamesettings.game_opponenttype != 0) {
                aOpp1[3] = globalgamesettings.game_opponenttype + '0';
                oppRes = openresource(aOpp1);
                isLoaded = 1;
            } else {
                isLoaded = 0;
            }

            g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
            previousType = globalgamesettings.game_opponenttype;
            lastSelection = -1;
            if (g_videoflg5 == 0)
                usebuffer();
            else
                setup_mcgawnd2();
            clearwindow(0);

            nullsub_2(resource_ptropp, 0x37);
            tmaskxf(locate_shape_fatal(resource_ptropp, "scrn"));

            draw3dbutton(getresl(g_miscfile_ptr, "bla"), 0x15,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw3dbutton(getresl(g_miscfile_ptr, "bnx"), 0x4d,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw3dbutton(getresl(g_miscfile_ptr, "bcl"), 0x85,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw3dbutton(getresl(g_miscfile_ptr, "bca"), 0xbd,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
            draw3dbutton(getresl(g_miscfile_ptr, "bdo"), 0xf5,
                        opponentmenu_buttons_y1[0] + 1, 0x36, 0x12,
                        menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);

            nullsub_2(resource_ptropp, (I8)(globalgamesettings.game_opponenttype + '0'));
            tmaskxf(g_opp_resources[globalgamesettings.game_opponenttype]);
            nullsub_2(resource_ptropp, 0x37);
            tmaskxf(locate_shape_fatal(resource_ptropp, "clip"));

            if (g_videoflg5 != 0) {
                readfxya(g_wndspr->image, 0, 0);
                usebuffer();
            }

            if (globalgamesettings.game_opponenttype != 0)
                resourceText = getresl(oppRes, "des");
            else
                resourceText = getresl(g_miscfile_ptr, "rac");
            mysetfont(fntndat);
            textcolor(0, dlg_colour);
            textpos = 0;
            y = 0;
            do {
                textch = *resourceText++;
                if (textch == ']') {
                    if (textpos != 0) {
                        resbuftext[textpos] = '\0';
                        fortextxy(resbuftext, 0x0c, y + 0x21);
                    }
                    textpos = 0;
                    y += fontdefvalue;
                } else {
                    resbuftext[textpos++] = textch;
                }
            } while (*resourceText != '\0');
            fontsetfontdef();
        }

        if (selectionIndex != lastSelection) {
            lastSelection = selectionIndex;
            sprite_blit_to_video(g_wndspr, lastColor);
            lastColor = -2;
            elticks();
            docursorinit();
        }

        timeDelta = docursor(selectionIndex, opponentmenu_buttons_x1, opponentmenu_buttons_x2, opponentmenu_buttons_y1, opponentmenu_buttons_y2,
                                           menu_hover_color_a, menu_hover_color_b);
        key = mykey(timeDelta);
        button = mousecursor(5, opponentmenu_buttons_x1,
                                        opponentmenu_buttons_x2,
                                        opponentmenu_buttons_y1,
                                        opponentmenu_buttons_y2);
        if (button != -1 &&
            !(globalgamesettings.game_opponenttype == 0 && button == 3))
            selectionIndex = button;

        if (key == 0)
            continue;

        switch (key) {
        case 0x0d:
        case 0x1b:
        case 0x20:
            switch (selectionIndex) {
            case UI_OPPONENT_MENU_PREVIOUS:
                --globalgamesettings.game_opponenttype;
                if (globalgamesettings.game_opponenttype < 1)
                    globalgamesettings.game_opponenttype = 6;
                break;
            case UI_OPPONENT_MENU_NEXT:
                ++globalgamesettings.game_opponenttype;
                if (globalgamesettings.game_opponenttype == 7)
                    globalgamesettings.game_opponenttype = 1;
                break;
            case UI_OPPONENT_MENU_NONE:
                globalgamesettings.game_opponenttype = 0;
                break;
            case UI_OPPONENT_MENU_CAR:
                if (globalgamesettings.game_opponenttype != 0) {
                    clear_keyboardbuffer();
                    mouseoff();
                    removewindow(g_wndspr);
                    closeresource(oppRes);
                    loading();
                    selectcar(globalgamesettings.game_opponentcarid,
                                 &globalgamesettings.game_opponentmaterial,
                                 &globalgamesettings.game_opponenttransmission,
                                 globalgamesettings.game_opponenttype);
                    previousType = -1;
                    mouseon();
                    continue;
                }
                break;
            case UI_OPPONENT_MENU_DONE:
                if (globalgamesettings.game_opponenttype != 0) {
                    if (globalgamesettings.game_opponentcarid[0] == -1) {
                        globalgamesettings.game_opponentcarid[0] = globalgamesettings.game_playercarid[0];
                        globalgamesettings.game_opponentcarid[1] = globalgamesettings.game_playercarid[1];
                        globalgamesettings.game_opponentcarid[2] = globalgamesettings.game_playercarid[2];
                        globalgamesettings.game_opponentcarid[3] = globalgamesettings.game_playercarid[3];
                        globalgamesettings.game_opponentmaterial =
                            (globalgamesettings.game_playermaterial & 1) ^ 1;
                        globalgamesettings.game_opponenttransmission = 0;
                    }
                } else {
                    globalgamesettings.game_opponentcarid[0] = -1;
                }

                removewindow(g_wndspr);
                if (isLoaded != 0)
                    closeresource(oppRes);
                freememory(resource_ptropp);
                closeresource(g_miscfile_ptr);
                mouseoff();
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

/* Presents and processes race options. */
I8 far run_option_menu(void)
{
    I8 active;
    I8 selection;
    I8 color_or_file;

    g_miscfile_ptr = openresource("misc");
    usescreen();
    clearwindow(menu_clear_color);

    strcpyfar(resbuftext, getres(g_miscfile_ptr, "gstu"));
    shadowtext(resbuftext, mycenter(resbuftext), 6,
                    dlg_colour, 0);

    strcpyfar(resbuftext, getres(g_miscfile_ptr, "gver"));
    shadowtext(resbuftext, mycenter(resbuftext), 16,
                    dlg_colour, 0);

    active = 1;
    while (active != 0) {
        selection = messagebox(2, 1,
            getresl(g_miscfile_ptr, "mop"),
            UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);

        switch (selection) {
        case UI_OPTION_MENU_INPUT:
            if (mousedrive != 0) {
                color_or_file = 2;
            } else if (joystick_enabled != 0) {
                color_or_file = 1;
            } else {
                color_or_file = 0;
            }
            selection = messagebox(2, 1,
                getresl(g_miscfile_ptr, "mid"),
                UI_DIALOG_CENTER, UI_DIALOG_CENTER, performGraphColor, 0, color_or_file);
            switch (selection) {
            case 0: mkeyon(); break;
            case 1: do_joystick_resource_text(); break;
            case 2: mmouseon(); break;
            }
            break;

        case UI_OPTION_MENU_MOF_RESOURCE:
            mmusictog();
            break;

        case UI_OPTION_MENU_SONSOF_RESOURCE:
            msoundtog();
            break;

        case UI_OPTION_MENU_LOAD_REPLAY:
            color_or_file = floadreq(replay_file, ReplayName, ".rpl",
                getresl(main_data_file_addr, "rep"));
            if (color_or_file != 0) {
                waitm_ms = 150;
                loading();
                loadreplay(replay_file, ReplayName);
/* MSC 5.10: repeating this assignment affects object output. */
                active = 1;
                goto cleanup;
            }
            break;

        case UI_OPTION_MENU_GRAPHICS:
            msetrender();
            break;

        case UI_OPTION_MENU_DOS:
            mdosexit();
            break;

        case -1:
        case UI_OPTION_MENU_DONE:
            active = 0;
            break;
        }
    }

cleanup:
    closeresource(g_miscfile_ptr);
    return active;
}

I16 hiscore_anim_table[28] = { 2, 1, 2, 3, 4, 1, 4, 0, 5, 0, 0, 6, 5, 6, 5, 1,
                               1, 2, 3, 5, 0, 6, 2, 3, 4, 4, 0, 6 };
I16S hiscore_rank_remap[3] = { 2, 0, 1 };
I16S hiscore_entry_remap[4] = { 1, 0, 3, 2 };
I16S hiscore_buttons_x1[UI_RESULTS_BUTTON_COUNT] = { 4, 84, 164, 244, 128 };
I16S hiscore_buttons_x2[UI_RESULTS_BUTTON_COUNT] = { 75, 155, 235, 315, 199 };
I16S hiscore_buttons_y1[UI_RESULTS_BUTTON_COUNT] = { 174, 174, 174, 174, 174 };
I16S hiscore_buttons_y2[UI_RESULTS_BUTTON_COUNT] = { 197, 197, 197, 197, 197 };
I8 aOpp2win[] = "opp2win";
I8 aOpp2lose[] = "opp2lose";
I8 aOp01[] = "op01";
/* Presents race results and handles the score screen. */
I8 far evaluation(void)
{
  I8 newEvaluation;
  I8 formattedRaceTextBuffer[18];
  I8 opponent;
  struct SPRITE far *hiddenWindow;
  I16 animTime;
  I16 textLen;
  I8 glyph;
  I8 currentWordText[32];
  void far *scoreResource;
  I8 resultMode;
  I8 screenBlitFlag;
  I16 pixels;
  void far *textFile;
  I8 far *trackFile;
  I8 far *animationFrameList;
  struct SHAPE2D far *shapePtr;
  I8 previousAnimationFrame;
  I8 resChar;
  void far *enemyRes;
  I16 buttonsX1[4];
  I16 src;
  I8 priorMenuIndex;
  register I16 inputKey;
  register I16 i;
  I16 animationTimeDelta;
  I16 y;
  I8 newRecord;
  I8 far *menuText;
  I16 parts;
  U16S  finalScoreTimeForRecord;
  I8 far *textResourceCursor;
  I8 clickedButton;
  I16 wordLen;
  I16 wordWidth;
  I16 animationXPosition;
  I16 buttonX2Positions[4];
  I8 currentMenuSelection;
  I16 animationYPosition;
  I8 currentAnimationFrame;
  I16 screenXOffset;

  ensure_file_exists(4);
  textFile = openresource("misc");
  if (globalgamesettings.game_opponenttype != 0)
  {
    aOpp1[3] = globalgamesettings.game_opponenttype + '0';
    enemyRes = openresource(aOpp1);
  }
  g_wndspr = sprite_make_window(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 0x0f);
  if (g_videoflg5 != 0)
    hiddenWindow = sprite_make_window(MENU_WINDOW_WIDTH_PIXELS, HALF_SCREEN_HEIGHT_PIXELS, 0x0f);
  screenBlitFlag = -1;
  usebufferc();
  draw3dbutton(0, 0, 0, SCREEN_WIDTH_PIXELS, HALF_SCREEN_HEIGHT_PIXELS, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  draw3dbutton(0, 0, 0x65, SCREEN_WIDTH_PIXELS, 0x63, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  y = 0x6b;
  strcpyfar(resbuftext, getresl(textFile, "elt"));
  if (race_stats.game_total_finish != 0)
  {
    maketime(formattedRaceTextBuffer, race_stats.game_total_finish - race_stats.game_penalty, 1);
    strcat(resbuftext, formattedRaceTextBuffer);
    if (didrace & 2)
      strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "con"));
    hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
    y += 10;
    if (race_stats.game_penalty != 0)
    {
      strcpyfar(resbuftext, getresl(textFile, "ppt"));
      maketime(formattedRaceTextBuffer, race_stats.game_penalty, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
      y += 10;
    }
  }
  else
  {
    strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "dnf"));
    hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  resultMode = UI_RACE_RESULT_NO_WINNER;
  if (globalgamesettings.game_opponenttype != 0)
  {
    if (race_stats.game_opponent_finish_time == 0)
    {
      strcpyfar(resbuftext, getresl(textFile, "olt"));
      strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "dnf"));
      if (race_stats.game_total_finish != 0)
        resultMode = UI_RACE_RESULT_PLAYER_WINS;
    }
    else if (race_stats.game_total_finish == 0 || (U16S ) race_stats.game_opponent_finish_time < (U16S ) race_stats.game_total_finish)
    {
      strcpyfar(resbuftext, getresl(textFile, "owt"));
      maketime(formattedRaceTextBuffer, race_stats.game_opponent_finish_time, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      resultMode = UI_RACE_RESULT_OPPONENT_WINS;
    }
    else
    {
      strcpyfar(resbuftext, getresl(textFile, "olt"));
      maketime(formattedRaceTextBuffer, race_stats.game_opponent_finish_time, 1);
      strcat(resbuftext, formattedRaceTextBuffer);
      if (race_stats.game_total_finish != 0)
        resultMode = UI_RACE_RESULT_PLAYER_WINS;
    }
    hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  if (resultMode == UI_RACE_RESULT_PLAYER_WINS)
    file_load_audio_resource("skidvict", "skidms", "VICT");
  else
    file_load_audio_resource("skidover", "skidms", "OVER");
  opponent = globalgamesettings.game_opponenttype;
  if (resultMode == UI_RACE_RESULT_NO_WINNER)
    if (race_stats.game_oEndFrame != race_stats.game_pEndFrame)
    opponent = 0;
  strcpyfar(resbuftext, getresl(textFile, "avs"));
  (race_stats.game_pEndFrame + elaptm1) != 0 ?
    (i = (I16) ((race_stats.game_speed_frame_sum / (U16S ) (race_stats.game_pEndFrame + elaptm1)) >> Q8_FRACTION_BITS)) : (i = 0);
  myitoa(formattedRaceTextBuffer, i, 0, 3);
  strcat(resbuftext, formattedRaceTextBuffer);
  strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "mph"));
  hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
  y += 10;
  if (race_stats.game_impactSpeed != 0)
  {
    strcpyfar(resbuftext, getresl(textFile, "imp"));
    myitoa(formattedRaceTextBuffer, race_stats.game_impactSpeed >> Q8_FRACTION_BITS, 0, 3);
    strcat(resbuftext, formattedRaceTextBuffer);
    strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "mph"));
    hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
    y += 10;
  }
  strcpyfar(resbuftext, getresl(textFile, "top"));
  myitoa(formattedRaceTextBuffer, race_stats.game_topSpeed >> Q8_FRACTION_BITS, 0, 3);
  strcat(resbuftext, formattedRaceTextBuffer);
  strcpyfar(resbuftext + strlen(resbuftext), getresl(textFile, "mph"));
  hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
  y += 10;
  if (race_stats.game_jumpCount != 0)
  {
    strcpyfar(resbuftext, getresl(textFile, "jum"));
    myitoa(formattedRaceTextBuffer, race_stats.game_jumpCount, 0, 3);
    strcat(resbuftext, formattedRaceTextBuffer);
    hiscore_draw_text(resbuftext, mycenter(resbuftext), y, dlg_colour, 0);
  }
  if (opponent != 0)
  {
    if ((didrace & 4) == 0)
    {
      hiscore_rank_prev = hiscore_current_place;
      hiscore_old_entry = end_hiscore_random;
      hiscore_opponent_earlier = hiscore_opponent_live;
      hiscore_current_place = drnd() % 3;
      if (hiscore_current_place == hiscore_rank_prev)
        hiscore_current_place = hiscore_rank_remap[hiscore_current_place];
      hiscore_opponent_live = drnd() % 3;
      if (hiscore_opponent_live == hiscore_opponent_earlier)
        hiscore_opponent_live = hiscore_rank_remap[hiscore_opponent_live];
      if (resultMode == UI_RACE_RESULT_OPPONENT_WINS)
      {
        if (race_stats.game_total_finish != 0)
          end_hiscore_random = drnd() % 2 + 2;
        else
          end_hiscore_random = drnd() % 2;
      }
      else
        end_hiscore_random = drnd() % 4;
      if (end_hiscore_random == hiscore_old_entry)
        end_hiscore_random = hiscore_entry_remap[end_hiscore_random];
    }
    if (resultMode == UI_RACE_RESULT_OPPONENT_WINS)
    {
      aOpp2win[3] = globalgamesettings.game_opponenttype + '0';
      scoreResource = myload(3, aOpp2win);
      animationFrameList = getres(enemyRes, "winn");
      end_hiscore_random = race_stats.game_total_finish != 0 ?
          ((myrandom() + race_stats.game_frame) & 1) + 2 : (myrandom() + race_stats.game_frame) & 1;
      resChar = 'v';
    }
    else
    {
      aOpp2lose[3] = globalgamesettings.game_opponenttype + '0';
      scoreResource = myload(3, aOpp2lose);
      animationFrameList = getres(enemyRes, "lose");
      end_hiscore_random = (myrandom() + race_stats.game_frame) & 3;
      resChar = 'd';
    }
  }
  newRecord = UI_HIG_STATE_NORMAL;
  makename(track_file, globalgamesettings.game_trackname, ".trk", buf_g_path);
  trackFile = myload(1, buf_g_path);
  if (trackFile == 0)
  {
    if (messagebox(1, 1, getresl(main_data_file_addr, "ihd"), -1, -1, dialog_frame_color, 0, 0) != 0)
      trackFile = myload(1, buf_g_path);
  }
  if (trackFile != 0)
  {
    for (i = 0; i < 0x385; ++i)
    {
      if (trackFile[i] != td14tb[i])
      {
        newRecord = UI_HIG_STATE_INELIGIBLE;
        break;
      }
    }
    purgememory(trackFile);
  }
  else
    newRecord = UI_HIG_STATE_INELIGIBLE;
  if (newRecord != UI_HIG_STATE_INELIGIBLE)
  {
    if (readhighs(0) != 0)
      if (readhighs(1) != 0)
      newRecord = UI_HIG_STATE_INELIGIBLE;
  }
  if (newRecord == UI_HIG_STATE_NORMAL)
  if (race_stats.game_total_finish != 0)
  {
    finalScoreTimeForRecord = race_stats.game_total_finish;
    if ((didrace & 6) == 0)
      if (finalScoreTimeForRecord != 0 && hscore_trk11_ptr[6].marker > finalScoreTimeForRecord)
      newRecord = UI_HIG_STATE_QUALIFYING;
  }
  currentAnimationFrame = 0;
  animTime = UI_RESULTS_ANIMATION_TICKS;
  newEvaluation = 1;
redraw:
  if (opponent != 0 && newRecord == UI_HIG_STATE_SHOW_EVALUATION)
  {
    newRecord = UI_HIG_STATE_NORMAL;
    usebuffer();
    drawhighs();
    /* MSC 5.10: keep this assignment despite the common assignment below. */
    currentMenuSelection = UI_RESULTS_BUTTON_REPLAY;
    newEvaluation = 1;
  }
  else if (opponent != 0)
  {
    aOp01[3] = '1';
    shapePtr = locate_shape_fatal(scoreResource, aOp01);
    y = shapePtr->width * pixel_scales;
    animationXPosition = 0x138 - y;
    animationYPosition = (0x63 - shapePtr->height) >> 1;
    sprite_draw_beveled_frame(animationXPosition - 3, animationYPosition - 3, y + 5, shapePtr->height + 5, dlg_colour, 0, animation_outline_color);
    aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
    movpackfxya(locate_shape_fatal(scoreResource, aOp01), animationXPosition, animationYPosition);
    previousAnimationFrame = currentAnimationFrame;
    textcolor(0, 0);
    y = 8;
    textLen = 0;
    pixels = 0;
    wordLen = 0;
    if (resultMode == UI_RACE_RESULT_NO_WINNER)
      parts = 1;
    else
      parts = 3;
    for (i = 0; i < parts; ++i)
    {
      switch (i)
      {
        case 0:
          if (resultMode == UI_RACE_RESULT_NO_WINNER)
            textResourceCursor = getresl(enemyRes, "d4a");
          else
          {
            formattedRaceTextBuffer[0] = resChar;
            formattedRaceTextBuffer[1] = '1';
            formattedRaceTextBuffer[2] = (I8) hiscore_current_place + 'a';
            textResourceCursor = getresl(enemyRes, formattedRaceTextBuffer);
          }
          break;
        case 1:
          formattedRaceTextBuffer[0] = resChar;
          formattedRaceTextBuffer[1] = '2';
          formattedRaceTextBuffer[2] = (I8) end_hiscore_random + 'a';
          textResourceCursor = getresl(enemyRes, formattedRaceTextBuffer);
          break;
        case 2:
          formattedRaceTextBuffer[0] = resChar;
          formattedRaceTextBuffer[1] = '3';
          formattedRaceTextBuffer[2] = (I8) hiscore_opponent_live + 'a';
          textResourceCursor = getresl(enemyRes, formattedRaceTextBuffer);
          break;
        default:
          break;
      }
      mysetfont(fntndat);
      do
      {
        glyph = *textResourceCursor++;
        if (glyph == ' ' || glyph == 0)
        {
          currentWordText[wordLen] = 0;
          wordWidth = textpixels(currentWordText);
          if (wordWidth + pixels < animationXPosition - 16 && textLen + wordLen < 80)
          {
            for (src = 0; src < wordLen; ++src)
              resbuftext[textLen++] = currentWordText[src];
            pixels += wordWidth;
          }
          else
          {
            resbuftext[textLen] = 0;
            fortextxy(resbuftext, 8, y);
            y += 8;
            if (currentWordText[0] == ' ')
              src = 1;
            else
              src = 0;
            for (textLen = 0; src < wordLen; ++src)
              resbuftext[textLen++] = currentWordText[src];
            resbuftext[textLen] = 0;
            pixels = textpixels(resbuftext);
          }
          wordLen = 1;
          currentWordText[0] = ' ';
        }
        else
          currentWordText[wordLen++] = glyph;
      }
      while (glyph != 0);
      fontsetfontdef();
    }
    if (textLen != 0)
    {
      mysetfont(fntndat);
      resbuftext[textLen] = 0;
      fortextxy(resbuftext, 8, y);
      fontsetfontdef();
    }
    newEvaluation = 0;
    if (newRecord > 0)
    {
      newRecord = UI_HIG_STATE_NORMAL;
      newEvaluation = 1;
      draw3dbutton(getresl(textFile, "bct"), 0x81, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      sprite_blit_to_video(g_wndspr, screenBlitFlag);
      screenBlitFlag = -2;
      docursorinit();
      clear_keyboardbuffer();
      y = 1;
      usescreen();
      do
      {
        animationTimeDelta = docursor(4, hiscore_buttons_x1, hiscore_buttons_x2, hiscore_buttons_y1, hiscore_buttons_y2, menu_hover_color_a, menu_hover_color_b);
        animTime += animationTimeDelta;
        if (animTime >= UI_RESULTS_ANIMATION_TICKS)
        {
          animTime -= UI_RESULTS_ANIMATION_TICKS;
          ++currentAnimationFrame;
          if (animationFrameList[currentAnimationFrame] == 0)
            currentAnimationFrame = 0;
        }
        if (currentAnimationFrame != previousAnimationFrame)
        {
          previousAnimationFrame = currentAnimationFrame;
          aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
          mouseoff();
          shapePtr = locate_shape_fatal(scoreResource, aOp01);
          if (g_videoflg5 != 0)
          {
            sprite_setup1_from_arg_pointer(hiddenWindow);
            movpackfxya(shapePtr, 0, 0);
            usescreen();
            clippix(animationXPosition, shapePtr->width * pixel_scales + animationXPosition, animationYPosition, shapePtr->height + animationYPosition);
            movcfxya(hiddenWindow->image, animationXPosition, animationYPosition);
            usescreen();
          }
          else
            movpackfxya(shapePtr, animationXPosition, animationYPosition);
          mouseon();
        }
        inputKey = mykey(i);
        if (inputKey == 13 || inputKey == 32 || inputKey == 27)
          y = 0;
      }
      while (y != 0);
      usebuffer();
      draw3dbutton(0, 0, 0, SCREEN_WIDTH_PIXELS, HALF_SCREEN_HEIGHT_PIXELS, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
      clippix(8, 0x138, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
      clearwindow(menu_button_color_c);
      mouseoff();
      addhighs(finalScoreTimeForRecord, getresl(textFile, "inh"), resultMode);
    }
  }
  else if (newRecord > 0)
  {
    clear_keyboardbuffer();
    mouseoff();
    addhighs(finalScoreTimeForRecord, getresl(textFile, "inh"), 0);
    newRecord = UI_HIG_STATE_NORMAL;
    screenBlitFlag = -2;
  }
  else
  {
    mouseoff();
    if (newRecord == UI_HIG_STATE_INELIGIBLE)
    {
      strcpyfar(resbuftext, getresl(textFile, "hna"));
      hiscore_draw_text(resbuftext, mycenter(resbuftext), 0x32, dlg_colour, 0);
    }
    else
      drawhighs();
  }
/* MSC 5.10: this unreferenced boundary remains object-sensitive. */
show_buttons:
  currentMenuSelection = UI_RESULTS_BUTTON_REPLAY;
  priorMenuIndex = 1;
  docursorinit();
  usebuffer();
  if (opponent == 0 || newRecord == UI_HIG_STATE_INELIGIBLE)
    screenXOffset = -36;
  else
  {
    screenXOffset = 0;
    if (newEvaluation != 0)
      menuText = getresl(textFile, "bev");
    else
      menuText = getresl(textFile, "bhi");
    draw3dbutton(menuText, hiscore_buttons_x1[0] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  }
  draw3dbutton(getresl(textFile, "brp"), screenXOffset + hiscore_buttons_x1[1] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  if (opponent != 0)
    menuText = getresl(textFile, "bra");
  else
    menuText = getresl(textFile, "bdr");
  draw3dbutton(menuText, screenXOffset + hiscore_buttons_x1[UI_RESULTS_BUTTON_DRIVE] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  draw3dbutton(getresl(textFile, "bmm"), screenXOffset + hiscore_buttons_x1[3] + 1, 0xaf, 0x46, 0x15, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
  for (i = 0; i < 4; ++i)
  {
    buttonsX1[i] = hiscore_buttons_x1[i] + screenXOffset;
    buttonX2Positions[i] = hiscore_buttons_x2[i] + screenXOffset;
  }
  clear_keyboardbuffer();
  sprite_blit_to_video(g_wndspr, screenBlitFlag);
  screenBlitFlag = -2;
  usescreen();
  for (;;)
  {
    if (currentMenuSelection != priorMenuIndex)
    {
      priorMenuIndex = currentMenuSelection;
      usescreen();
      clippix(0, SCREEN_WIDTH_PIXELS, hiscore_buttons_y1[0], hiscore_buttons_y2[0] + 1);
      mouseoff();
      movcf(g_wndspr->image);
      mouseon();
      elticks();
      docursorinit();
    }
    animationTimeDelta = docursor(currentMenuSelection, buttonsX1, buttonX2Positions, hiscore_buttons_y1, hiscore_buttons_y2, menu_hover_color_a, menu_hover_color_b);
    if (newEvaluation == 0)
    if (resultMode != UI_RACE_RESULT_NO_WINNER)
    {
      animTime += animationTimeDelta;
      if (animTime >= UI_RESULTS_ANIMATION_TICKS)
      {
        animTime -= UI_RESULTS_ANIMATION_TICKS;
        ++currentAnimationFrame;
        if (animationFrameList[currentAnimationFrame] == 0)
          currentAnimationFrame = 0;
      }
      if (currentAnimationFrame != previousAnimationFrame)
      {
        previousAnimationFrame = currentAnimationFrame;
        aOp01[3] = animationFrameList[currentAnimationFrame] + '0';
        mouseoff();
        shapePtr = locate_shape_fatal(scoreResource, aOp01);
        if (g_videoflg5 != 0)
        {
          sprite_setup1_from_arg_pointer(hiddenWindow);
          movpackfxya(shapePtr, 0, 0);
          usescreen();
          clippix(animationXPosition, shapePtr->width * pixel_scales + animationXPosition, animationYPosition, shapePtr->height + animationYPosition);
          movcfxya(hiddenWindow->image, animationXPosition, animationYPosition);
          usescreen();
        }
        else
          movpackfxya(shapePtr, animationXPosition, animationYPosition);
        movpackfxya(locate_shape_fatal(scoreResource, aOp01), animationXPosition, animationYPosition);
        mouseon();
      }
    }
    if (opponent == 0 || newRecord == UI_HIG_STATE_INELIGIBLE)
    {
      clickedButton = mousecursor(3, buttonsX1 + 1, buttonX2Positions + 1, hiscore_buttons_y1 + 1, hiscore_buttons_y2 + 1);
      if (clickedButton != -1)
        currentMenuSelection = clickedButton + 1;
    }
    else
    {
      clickedButton = mousecursor(4, buttonsX1, buttonX2Positions, hiscore_buttons_y1, hiscore_buttons_y2);
      if (clickedButton != -1)
        currentMenuSelection = clickedButton;
    }
    inputKey = mykey(animationTimeDelta);
    if (inputKey == 0)
      continue;
    switch (inputKey)
    {
      case 13:
      case 32:
        if (currentMenuSelection == UI_RESULTS_BUTTON_EVALUATE)
        {
          usebuffer();
          draw3dbutton(0, 0, 0, SCREEN_WIDTH_PIXELS, HALF_SCREEN_HEIGHT_PIXELS, menu_button_color_a, menu_button_color_b, menu_button_color_c, 0);
          if (newEvaluation != 0)
          {
            newRecord = UI_HIG_STATE_NORMAL;
            goto redraw;
          }
          else
          {
            newRecord = UI_HIG_STATE_SHOW_EVALUATION;
            goto redraw;
          }
        }
        audio_unload();
        if (opponent != 0)
          purgememory(scoreResource);
        if (g_videoflg5 != 0)
          removewindow(hiddenWindow);
        removewindow(g_wndspr);
        if (globalgamesettings.game_opponenttype != 0)
          closeresource(enemyRes);
        closeresource(textFile);
        return currentMenuSelection - 1;
      case 0x4b00:
        if (opponent == 0 || newRecord == UI_HIG_STATE_INELIGIBLE)
        {
          if (currentMenuSelection <= 1)
            currentMenuSelection = UI_RESULTS_BUTTON_MAIN_MENU;
          else
            --currentMenuSelection;
        }
        else if (currentMenuSelection != UI_RESULTS_BUTTON_EVALUATE)
          --currentMenuSelection;
        else
          currentMenuSelection = UI_RESULTS_BUTTON_MAIN_MENU;
        break;
      case 0x4d00:
        if (currentMenuSelection < UI_RESULTS_BUTTON_MAIN_MENU)
          ++currentMenuSelection;
        else if (opponent == 0 || newRecord == UI_HIG_STATE_INELIGIBLE)
          currentMenuSelection = UI_RESULTS_BUTTON_REPLAY;
        else
          currentMenuSelection = UI_RESULTS_BUTTON_EVALUATE;
        break;
      default:
        continue;
    }
  }
}

U8  score_entry_alphabet[] = "0123456789abcdefghij";
I8 aQ00[] = "q00";
I8 aA00[] = "a00";
/* Loads and presents the startup verification screen. */
void far getprotection(I16 selection)
{
    I8 prompt[1024];
    I8 questionID[6];
    I16 textLength;
    void far *resource;
    I16 failures;
    struct POINT2D points[6];
    register I16 scanIndex;
    I8 userInput[22];

    aQ00[2] = score_entry_alphabet[selection];
    aA00[2] = score_entry_alphabet[selection];
    resource = openresource("misc");
    strcpyfar(prompt, getresl(resource, "cop"));
    strcpyfar(resbuftext, getresl(resource, aQ00));
    strcat(prompt, resbuftext + 6);
    for (scanIndex = 0; scanIndex < 6; ++scanIndex)
        questionID[scanIndex] = resbuftext[scanIndex];
    messagebox(3, 1, (I8 far *)prompt, UI_DIALOG_CENTER, 0x78,
                performGraphColor, points, 0);
    resbuftext[2] = 0;
    resbuftext[0] = questionID[0];
    resbuftext[1] = questionID[1];
    fortextxy(resbuftext, points[0].x, points[0].y);
    resbuftext[0] = questionID[2];
    resbuftext[1] = questionID[3];
    fortextxy(resbuftext, points[1].x, points[1].y);
    resbuftext[0] = questionID[4];
    resbuftext[1] = questionID[5];
    fortextxy(resbuftext, points[2].x, points[2].y);
    strcpyfar(resbuftext, getresl(resource, aA00));
    points[0] = points[3];
    textLength = strlen(resbuftext);
    userInput[0] = 0;
    failures = 0;
    do {
        instringz(userInput, textLength, points[0].x, points[0].y, 30000);
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
    restoreboxwindow();
    mouseon();
    closeresource(resource);
}

void initselections(void)
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
