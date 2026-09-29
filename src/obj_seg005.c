#define FAR far
#define NEAR near
#define HUGE huge
#define WORLD_COORDINATE_SHIFT 6
#define CAR_SPEED_Q8_SHIFT 8
#define SPEEDOMETER_HUNDREDS_BASE 100
#define SPEEDOMETER_TWO_HUNDREDS_BASE 200
#define PLATFORM_SCREEN_WIDTH_PIXELS 320
#define PLATFORM_SCREEN_HEIGHT_PIXELS 200
#define BIOS_KEY_F1 0x3B00
#define BIOS_KEY_F2 0x3C00
#define BIOS_KEY_F3 0x3D00
#define BIOS_KEY_F4 0x3E00
#define KEY_SCAN_UP 0x4800
#define KEY_SCAN_LEFT 0x4B00
#define KEY_SCAN_RIGHT 0x4D00
#define KEY_SCAN_DOWN 0x5000
#define KEY_ASCII_ESCAPE 0x1B
#define KEY_ASCII_UPPER_C 0x43
#define KEY_ASCII_UPPER_D 0x44
#define KEY_ASCII_UPPER_H 0x48
#define KEY_ASCII_UPPER_M 0x4D
#define KEY_ASCII_UPPER_R 0x52
#define KEY_ASCII_LOWER_C 0x63
#define KEY_ASCII_LOWER_D 0x64
#define KEY_ASCII_LOWER_H 0x68
#define KEY_ASCII_LOWER_M 0x6D
#define KEY_ASCII_LOWER_R 0x72
#define KEY_ASCII_LOWER_T 0x74
#include "stunts_types.h"
/* Scratch TU. Header declarations are semantic leads from Restunts; no preprocessor directives. */ /* PORT: plain char signedness follows the pinned MSC target. */
extern void sprcopy2to12(void);
extern void msdrawtransparentchk(void);



/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct VECTOR {
	I16S x, y, z;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct VECTORLONG {
	I32 lx, ly, lz;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct POINT2D {
	I16 px, py;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct MATRIX { I16 vals[9]; };;

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct PLANE {
	I16 plane_yz;
	I16 plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};


I16S sinfast(U16S  s);
I16S cosfast(U16S  s);

I16 polang(I16 z, I16 y);
I16 polradius2d(I16 z, I16 y);
I16 polarRadius3D(struct VECTOR* vec);

unsigned rect_compare_point(struct POINT2D* pt);

void mat_vec(struct VECTOR* invec, struct MATRIX* mat, struct VECTOR* outvec);
void mat_mul_vector2(struct VECTOR* invec, struct MATRIX far* mat, struct VECTOR* outvec);
void mat_multiply(struct MATRIX* rmat, struct MATRIX* lmat, struct MATRIX* outmat);
void mat_invert(struct MATRIX* inmat, struct MATRIX* outmat);
void mat_rot_x(struct MATRIX* outmat, I16 angle);
void matroty(struct MATRIX* outmat, I16 angle);
void mat_rot_z(struct MATRIX* outmat, I16 angle);
struct MATRIX* matrotzxy(I16 z, I16 x, I16 y, I16 unk);

void rect_adjust_from_point(struct POINT2D* pt, struct RECTANGLE* rc);

I16 vector_op_unk2(struct VECTOR* vec);
void vector_to_point(struct VECTOR* vec, struct POINT2D* outpt);
void vector_op_unk(struct VECTOR* vec1, struct VECTOR* vec2, struct VECTOR* outvec, I16S i);

I16S mulscl(I16S a1, I16S a2);

void rcunion(struct RECTANGLE* r1, struct RECTANGLE* r2, struct RECTANGLE* outrc);
I16 rcintersect(struct RECTANGLE* r1, struct RECTANGLE* r2);

void plnrotop(void);
I16 plnoriginop(I16 index, I16 b, I16 c, I16 d);






/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct GAMEINFO {
	I8 game_playercarid[4];
	I8 game_playermaterial;
	I8 game_playertransmission;
	I8 game_opponenttype;
	I8 game_opponentcarid[4];
	I8 game_opponentmaterial;
	I8 game_opponenttransmission;
	I8 game_trackname[9];
	I8 game_framespersec;
	I16S game_recordedframes;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; // applying the (x, y, z) vector notation to rotation
                              // angles is a source of confusion.
	I16S car_pseudoGravity;
	I16S car_steeringAngle;
	I16S car_currpm;
	I16S car_lastrpm;
	I16S car_idlerpm2;
	I16S car_speeddiff; // former gripdiff
	U16S  car_speed;     // former trackgrip
                         // value is 2^8*(mph value) and unsigned
	U16S  car_speed2;    // former trackgrip2
                         // speed is the rev-coupled speed, while speed2 is
                         // the actual car speed. They are different, for
                         // instance, during jumps (where accelerating increases
                         // revs without making the car go faster).
	U16S  car_lastspeed; // former lasttrackgrip
	U16S  car_gearratio;
	U16S  car_gearratioshr8;
	I16S car_knob_x;
	I16S car_36MwhlAngle;
	I16S car_knob_y;
	I16S car_knob_x2;
	I16S car_knob_y2;
	I16S car_angle_z;
	I16S car_40MfrontWhlAngle;
	I16S field_42;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S field_48;
	I16S car_trackdata3_index;
	I16S car_rc1[4]; // four words, one for each wheel.
	I16S car_rc2[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_rc5[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_vec_unk3;
	struct VECTOR car_vec_unk4;
	struct VECTOR car_vec_unk5;
	I16S field_B6;
	I16S field_B8;
	I16S field_BA;
	I8 car_is_braking;
	I8 car_is_accelerating;
	I8 car_current_gear;
	I8 car_sumSurfFrontWheels;
	I8 car_sumSurfRearWheels;
	I8 car_sumSurfAllWheels; // used as jump flag.
	I8 car_surfaceWhl[4];      // surface types for each of the wheels, it seems.
	I8 car_engineLimiterTimer;
	I8 car_slidingFlag;
	I8 field_C8;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 field_CD;
	I8 field_CE; // is added?
	I8 field_CF; // is initialized?
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct GAMESTATE {
	I32 game_longs1[24]; // x
	I32 game_longs2[24]; // y
	I32 game_longs3[24]; // z
	struct VECTOR game_vec1[2]; // 0 = player, 1 = opponent
	struct VECTOR game_vec3[2]; // [0] player, [1] opponent
	I16S game_frame_in_sec;
	I16S game_frames_per_sec;
	I32  game_travDist;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S field_2F2;
	I16S field_2F4;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S field_2FE[24];
	I16S field_32E[24];
	I16S field_35E[24];
	I16S field_38E[24];
	I8 field_3BE[48];
	I8 kevinseed[6];
	I8 field_3F4;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 field_3F7[2]; // 0 = player, 1 = opponent
	I8 field_3F9;
	I8 field_3FA[48];
	I8 field_42A;
	I8 field_42B[24];
	I8 field_443[24];
	I8 field_45B;
	I8 field_45C;
	I8 field_45D;
	I8 field_45E;
	I8 field_45F;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	I16S car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	I16S downshift_rpm;
	I16S upshift_rpm;
	I16S max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	I8 idle_torque;
	I8 torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding;
	I16S surface_grip[4];
	I8 simd_unk3[10];
	struct POINT2D collide_points[2];
	I16S car_height;
	struct VECTOR wheel_coords[4];
	U8  steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	U8  spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	U8  revpoints[256];
	I16S far* aerorestable;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct TRKOBJINFO {
	I8  si_noOfBlocks;      // How many shapeInfo pieces compose the element. Arbitrary for the first piece, 0 for the following ones.
	I8  si_entryPoint;      // Connectivity of the track element regarding tiles.
	I8  si_exitPoint;
	I8  si_entryType;        // Connectivity of the track element regarding element types.
	I8  si_exitType;
	I8  si_arrowType;        // Type of the element for determining penalty-arrow behaviour.
	I16S si_arrowOrient;      // Orientation angle for penalty-arrow purposes
	I16S* si_cameraDataOffset; // offset (0003B770)
	I8  si_opp1;             //Appears to affect how the opponent AI approaches an element.
	I8  si_opp2;
	I8  si_opp3;
	I8  si_oppSpedCode;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct TRACKOBJECT {
	struct TRKOBJINFO* ss_trkObjInfoPtr; // offset (0003B770)
	I16S ss_rotY;           // Horizontal orientation of the element.
	struct SHAPE3D* ss_shapePtr;       // offset (0003B770)
	struct SHAPE3D* ss_loShapePtr;     // offset (0003B770)
	U8   ss_ssOvelay;       // Renders additional sceneShapes over the current one.
	I8  ss_surfaceType;    // Paintjob. FF will induce alternating paintjobs.
	I8  ss_ignoreZBias;    // Appears to be Z-bias override flag, mostly used for roads and corners.
	I8  ss_multiTileFlag;  // 0 = one-tile, 1 = two-tile vertical, 2 = two-tile horizontal, 3 = four-tile.
	I8  ss_physicalModel;  // sets the physical model in build_track_object
	I8  scene_unk5;        // always zero.
};


extern struct GAMEINFO globalgamesettings;
extern struct GAMEINFO gmconfigbackup;

extern struct GAMESTATE core;
extern struct SIMD ophys_7;

extern I16S pixel_scales;
extern I16S g_vid_flg2_set;
short vidflg4_is1;
char g_videoflg5;
extern I16S g_vid_flag6;

unsigned char timeraud;
unsigned char slomodiv8;
unsigned short elaptm1;
unsigned short tmr2;
unsigned char sigframe;
unsigned char g_rpl_init;
char gm_playmode; // 0 = playing, 1 = paused, 2 = replay
extern I16S g_sgateopn;

extern I16S elapsed_time1; // current frame?
extern I16S g_cvxintvl; // fps * 30
extern I16S frmcs_time; // 100 / fps
extern I16S st_hdg;
extern void* table_lookup;
extern void* steerWhlRespTable_10fps;
extern void* steerWhlRespTable_20fps;
extern I8 idxtrk, tagtrk;
extern I8 g_hillf;
extern I16S hillconsts[];

struct RECTANGLE boundglassrect;
short bitmapdash;
extern I16 runrndx;
char replaybar_toggle;
char inrepflg;
extern I8 cammd;
char g_rplmodui;
char gm_saved_rpl_mode;
char numid;
char g_rplbfask;
char on_off_dash;
char cam_idg;
extern I8 pen_flag_count;
int replayrst;
char popupact;
extern I8 mouse_enabled;
extern I8 joystick_enabled;
extern void far* gamerptrs;
void far* dasm_shp_7;
extern I16 input_pushed;
char dashbtogglesaved;
char g_replaybarcpytgl;
char is_in_rplcopy;
extern I8 follow_op;
char opp_follow_flag_backup;
int roofbmphgt_saved;
char mode_flag;
char g_rplybarenable;
int dashbmpy_copy;
int rplbarabovehgt;
char g_simprect;
char g_viewinx[2];

int dasty;
struct SHAPE2D far *g_dastbmpbuf;
int dashbmy9;
int rfy5;
extern struct RECTANGLE* rectp;

extern void player_op(I8);
extern void opponent_op(void);
extern void audio_carstate(void);
extern void setup_car_shapes(I16);
extern void update_frame(I8, struct RECTANGLE*);
extern void loop_game(I16, I16, I16);
extern void set_frame_callback(void);
extern void mouse_minmax_position(I16);
extern I16 kb_get_char(void);
extern void far update_crash_state(I16, I16);
extern void far do_mou_resource_text(void);
extern void far initialize_game_state(I16);
extern I8 handle_ingame_kb_shortcuts(unsigned);

extern I16 flagsdown;
extern I16 msecoordx;
extern I16 pos_y_ms;
extern I16 performGraphColor;
extern I8 resbuftext;
extern I16 waitm_ms;

extern void far* fntndat;
extern void far* def_fntadr;
extern void far* main_data_file_addr;
extern struct GAMESTATE huge* cvxs_a;
extern I16 lnoffsets[];
extern I16 gterrtrk[];
extern I16 r_zp[];
extern I16 row_ctr_zs[];
extern I16 postable[];
extern I16 z_ctr_pos[];
extern I16 xcols[];
extern I16 trackctrpos2[];
extern I16S far* g_td01_track_filecpy; //trackdata1;
extern I16S far* trackdata_penalty_related; //trackdata2;
extern I8 far* td3;
extern I16S far* track04_plyraero; //trackdata4;
extern I16S far* trackdata_05_opp_aerotbl; //trackdata5;
extern I8 far* td6_ptr_b;
extern I8 far* trackdat7;
extern I16 far* g_td08d; //trackdata8;
extern struct VECTOR far* trkptrpath;
extern I16 far* td10checkptr;// trackdata10;
extern I8 far* hscore_trk11_ptr; //trackdata11;
extern I8 far* savedptr_ms;
char far* td13_replay_hdr; //trackdata13;
extern U8  far* td14tb; //trackdata14;
extern U8  far* td15p_9; //trackdata15;
char far* g_tdreplay16buf; //trackdata16;
extern I8 far* road_trk; //trackdata17;
extern I8 far* td_18_ref;
extern U8  far* td19hdl;
extern I8 far* coursedataappend_address; //trackdata20;
extern I8 far* g_column_of_trkdata21_pth; //trackdata21;
extern I8 far* tdfrompathrow22; //trackdata22;
extern U8  far* trkd23adr; // indexes into trkObjectList
extern I8 kbormouse;
extern I8 pass_check_flag;
extern I8 g_is_busy;
extern I8 buf_g_path[];
extern I8 track_file[];
extern I8 menutimeout;
extern U16S  dialogarg2;
extern I8 replay_file[];
char endhsdemo;
extern I8 aMain[];
extern I8 aMisc_1[];
extern I8 aFontdef_fnt[];
extern I8 aFontn_fnt[];
extern I8 aTrakdata[];
extern I8 aDefault_0[];
extern I8 aCvx[];
extern I8 aTedit__0[];
extern I8 aSlct[];
extern I8 aSkidms_0[];
extern I8 aSkidslct[];
extern I8 aDos[];

extern I16 rate_frame;
unsigned short frm_rate2;
extern U16S  slow_video_mode_state;
extern U16S  statemgmtcpy;
extern U8  detail_lvl;

extern U16S  pspofs;
extern U16S  pspseg;
extern unsigned resmem_end_seg;
extern unsigned resmem_base_seg;

extern struct MEMCHUNK* resptr1;
extern struct MEMCHUNK* resptr2;
extern struct MEMCHUNK* resendptr1;
extern struct MEMCHUNK* resendptr2;
extern U16S  resmaxsize;

extern U32  timer_callback_counter;
extern U32  last_timer_callback_counter;
extern U32  timer_copy_unk;

extern U8  randomseeds[];
extern const I8 aReservememoryO[];
extern const I8 aReservememoryOutOfMemory[];
extern const I8 aMemoryManagerB[];
extern const I8 aResizememoryNo[];
extern const I8 aResizememoryCa[];
extern const I8 aSFileError[];
extern const I8 aSFileError_0[];
extern const I8 aSFileError_1[];
extern const I8 aSInvalidPackTy[];
extern const I8 aLocateshape4_4sShapeNotF[];
extern const I8 aLocatesound4_4sSoundNotF[];
extern I8 audiodriverstring[];

extern U16S  idx_time_gm;
extern I16S is_audioloaded;
extern void far* musicfile;
extern void far* openvfile;
extern I8 textrespfxchr; // = 'e'
extern I8* shapeexts[];
extern U8  palmap[];

extern I16* material_clrlist_ptr;
extern I16* mat_copy_clr_lst_ptr;
extern I16* material_clrlist2_ptr;
extern I16* g_mat_clrlist_copy_2_ptr;
extern I16* material_patlist_ptr;
extern I16* material_patlistptr_copy;
extern I16* material_patlist2_ptr;
extern I16* matpatlistcopypointer2;
extern U16S  video_cnstval;

extern I16S track_edge_points(I16S car_trackdata3_index, struct VECTOR* car_vec_unk3, I16S field_CE, I16S* unk);
extern void fontsetfontdef(void);
extern void initialize_polyinfo(void);
extern U16S  run_intro_looped(void);
extern U16S  show_dialog(I16 unk1, I16 unk2, void far* textresptr, U16S  unk3, U16S  unk4, I16 arg, void* unk5, I16 unk6);
extern I8 run_menu(void);
extern I8 setup_track(void);
extern void run_tracks_menu(I16 unk);
extern void run_opponent_menu(void);
extern void show_waiting(void);
extern void run_car_menu(struct GAMEINFO* unk, I8* unk2, I8* unk3, U16  unk4);
extern 
/* TU function declarations needed before address-ordered definitions. */
I8 far file_load_replay(const I8 *dir, const I8 *name);
I16S far file_write_replay(const I8 far *filename);
I16 far file_write_fatal(const I8 *filename, void far *buffer, U32  length);
void far remove_frame_callback(void);
void far replay_unk2(I16 mode);
void far replay_unk(void);
void far free_player_cars(void);
extern struct SPRITE far *g_wndspr;
extern I8 aCarcoun[];
void far *eng1resourceptr;
void far *engdata;
extern I16 g_player_sound_id;
extern I8 sndpendingstate;
extern I8 g_plyr_snd_state;
extern I8 audiooppflag;
extern I16 op_eng_sound_id;
int g_audio_frms_ix;
extern I16 sndposrecord;
int snd_tick_clock;
void far *fntled_res;
void far *sdgresourcehandle;
extern void far *g_planlist;
extern void far *wallrecrecord;

void run_game(void);
extern unsigned end_hiscore(void);
extern unsigned run_option_menu(void);
extern void security_check(void);

extern void ensure_file_exists(I16 unk);

extern void far* load_song_file(const I8* filename);
extern void far* load_voice_file(const I8* filename);
extern void far* load_sfx_file(const I8* filename);
extern void far* load_shape2d_nofatal_thunk(const I8* filename);
extern void far* load_shape2d_res_nofatal_thunk(const I8* filename);
extern void far* file_load_shape2d_nofatal(I8* shapename);
extern void far* file_load_shape2d_nofatal2(I8* shapename);
extern void far* init_audio_resources(void far* songptr, void far* voiceptr, const I8* name);
extern void load_audio_finalize(void far* audiores);
extern I16S audio_load_driver(I8* driver, I16S a2, I16S a3);
extern void audio_unload(void);
extern I16S audio_toggle_flag2(void);
extern I16S audio_toggle_flag6(void);
extern void audio_stop_unknown(void);
extern void audiodrv_atexit(void);

extern void check_input(void);
extern I16 input_do_checking(I16 unk);
extern void keyboard_exit_handler(void);
extern void keyboard_shift_checking1(void);
extern void kb_shift_checking2(void);
extern void kb_reg_callback(I16 code, void (far* callback)(void));
extern void show_graphic_levels_menu(void);
extern void do_joystick_resource_text(void);
extern void do_key_resource_text(void);
extern void do_mof_resource_text(void);
extern void do_pau_restext(void);
extern void do_dos_resource_text(void);
extern void do_sonsof_resource_text(void);
extern I16S get_kb_or_joy_flags(void);

extern I16S mouse_init(I16S a1, I16S a2);
extern void msdrawopaquechk(void);

extern void video_set_mode4(void);
extern void video_set_mode7(void);
extern void video_set_mode_13h(void);

extern void shape3d_load_car_shapes(I8* carid, I8* oppcarid);

extern void load_palandcursor(void);
extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
extern void sprite_clear_1_color(U8 );
extern void sprite_blit_to_video(struct SPRITE far* sprite);

extern I16S intr0_handler(void);
extern I16S (far* old_intr0_handler)(void);
extern void timer_setup_interrupt(void);
extern I16 timer_get_delta_alt(void);

extern I16S set_criterr_handler(I16S (far* callback)(void));
extern void exit(I16S a1);
extern void fatal_error(const I8*, ...);
extern I16S do_dea_textres(void);

extern void* _memcpy(void*, const void*, unsigned);
extern I8* _strcpy(I8* dest, const I8* src);
extern I8* _strcat(I8* dest, const I8* src);
extern I16 _strcmp(const I8* dest, const I8* src);
extern I16 _stricmp(const I8* dest, const I8* src);
extern unsigned _strlen(const I8* str);
extern void far* __fmemcpy(void far*, const void far*, unsigned);
extern unsigned _abs(unsigned);
extern I16 _rand(void);
extern void _srand(U16 );



/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct ENGINESOUND {
	I16 es_unk0;
	I16 es_unk2;
	I16 es_unk4;
	I16 es_unk6;
	I8 far *es_names[10];
};
struct ENGINESOUND player_engine_profile = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
struct ENGINESOUND opponent_engine_profile = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
I8 replay_axis_magnitude[34] = { 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
	60, 64, 68, 72, 76, 84, 90, 98, 106, 114, 121, 127, 127, 127 };

/* run_game: reconstructed from the target listing (s005c) */
void loop_game(I16 mode, I16 frame_index, I16 frame_offset);
extern void far *locate_text_resource(void far *res, I8 *name);

/* Runs the game and replay interface. Params: none. Returns: none. State: reads and updates game, menu, replay and camera state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
void run_game(void) {
	I16 dialog_values[2];
	I16 replay_key_code, replaybar_height, last_game_frame;
	struct RECTANGLE temp_rect;
	I16 previous_roof;
	register I16 dialog_result;

	last_game_frame = -1;
	boundglassrect.left = 0;
	boundglassrect.right = PLATFORM_SCREEN_WIDTH_PIXELS;
	previous_roof = -1;
	bitmapdash = -1;
	runrndx = get_kevinrandom() << 3;
	replaybar_toggle = 1;
	inrepflg = 0;
	if (menutimeout != 0) {
		cammd++;
		if (cammd == 4) {
			cammd = 0;
		}
		gm_playmode = 2;
		/* PLATFORM(file): load replay data from a file. */ if (file_load_replay(0, "default") != 0) {
			return;
		}
		track_setup();
	} else if (globalgamesettings.game_recordedframes == 0) {
		cammd = 0;
		gm_playmode = 1;
	} else {
		cammd = 0;
		gm_playmode = 2;
		inrepflg = 1;
	}

	if (setup_player_cars() != 0) {
		free_player_cars();
		/* PLATFORM(input_joy): collect menu-text joystick input. */
		/* PLATFORM(input_kb): collect menu-text key input. */
		/* PLATFORM(input_mouse): collect menu-text pointer input. */
		/* PLATFORM(timer): wait for menu text input using timer ticks. */
		/* PLATFORM(video): draw the menu resource text. */
		do_mer_restext();
	} else {
		kbormouse = 0;
		g_rplmodui = 0;
		sigframe = 1;
		set_frame_callback();
		gm_saved_rpl_mode = -1;
		numid = 0;
		cam_idg = 0;
		g_rplbfask = 0;
		on_off_dash = 0;

		if (menutimeout != 0) {
			rate_frame = globalgamesettings.game_framespersec;
			initialize_game_state(-1);
		} else if (inrepflg == 0) {
			cammd = 0;
			on_off_dash = 1;
			pen_flag_count = 0;
			rate_frame = frm_rate2;
			globalgamesettings.game_framespersec = frm_rate2;
			initialize_game_state(-1);
			replayrst = 0;
			popupact = 0;
			g_rpl_init = 1;
			/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ mouse_minmax_position(mouse_enabled);
			gm_playmode = 1;
			core.playerstate.car_posWorld1.lx += (I32)mulscl(sinfast(st_hdg), -240) << 6;
			core.playerstate.car_posWorld1.lz += (I32)mulscl(cosfast(st_hdg), -240) << 6;
			core.playerstate.car_posWorld1.ly += 0x580;
			endhsdemo = 1;
		} else {
			cammd = 0;
			gm_playmode = 2;
			g_sgateopn = 500;
			rate_frame = globalgamesettings.game_framespersec;
			restore_gamestate(0);
			restore_gamestate(globalgamesettings.game_recordedframes);
			while (globalgamesettings.game_recordedframes != core.game_frame) {
				/* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ if (input_do_checking(1) != 27)
					update_gamestate();
				else
					break;
			}
			tmr2 = globalgamesettings.game_recordedframes;
		}

		for (;;) {
			while (core.game_frame != tmr2) {
				if ((mouse_enabled != 0 || joystick_enabled != 0) && gm_playmode == 0) {
					replay_unk();
				}
				update_gamestate();
			}

			if (gm_playmode == 0 && sigframe == 0 && core.game_inputmode != 0) {
				if (last_game_frame == core.game_frame)
					continue;
				last_game_frame = core.game_frame;
			}

			if (core.game_inputmode == 0 && gm_playmode == 0) {
				tmr2 = 0;
				globalgamesettings.game_recordedframes = 0;
				core.game_frame = 0;
			}

			if (statemgmtcpy != slow_video_mode_state) {
				statemgmtcpy = slow_video_mode_state;
				init_rect_arrays();
			}

			if (g_rplbfask != 0) {
				/* PLATFORM(input_kb): save the active input source. */ /* PLATFORM(input_mouse): save mouse input mode. */ input_push_status();
				/* PLATFORM(audio): update the active audio mode. */ audio_unk();
				/* PLATFORM(input_joy): collect dialog joystick input. */
				/* PLATFORM(timer): wait for dialog input using timer ticks. */
				/* PLATFORM(video): render a modal dialog. */
				dialog_result = show_dialog(2, 1, locate_text_resource(gamerptrs, "rbf"), -1, -1, dialogarg2, 0, 0);
				if (dialog_result == -1)
					dialog_result = 0;
				/* PLATFORM(audio): restore audio playback volume. */ restore_audio_volume();
				input_pushed = 0;
				/* PLATFORM(input_kb): restore the active input source. */ /* PLATFORM(input_mouse): restore mouse input mode. */ /* PLATFORM(video): restore the opaque draw mode. */ input_pop_status();
				if (dialog_result != 0) {
					update_crash_state(4, 0);
					sigframe = 1;
				}
				g_rplbfask = 0;
			}

			if (g_videoflg5 != 0) {
				setup_mcgawnd2();
				cam_idg = numid;
			} else {
				/* PLATFORM(video): copy the window sprite to the display page. */ sprite_copy_wnd_to_1();
			}

			if (gm_playmode != gm_saved_rpl_mode || on_off_dash != dashbtogglesaved || replaybar_toggle != g_replaybarcpytgl || inrepflg != is_in_rplcopy || follow_op != opp_follow_flag_backup) {
				gm_saved_rpl_mode = gm_playmode;
				dashbtogglesaved = on_off_dash;
				g_replaybarcpytgl = replaybar_toggle;
				is_in_rplcopy = inrepflg;
				opp_follow_flag_backup = follow_op;
				roofbmphgt_saved = 0;
				mode_flag = 0;

				if (gm_playmode != 2 || menutimeout != 0 || (replaybar_toggle == 0 && inrepflg == 0)) {
						g_rplybarenable = 0;
				} else {
					g_rplybarenable = 1;
					goto replaybar_done;
				}
replaybar_done:

				if (menutimeout != 0) {
					dashbmpy_copy = PLATFORM_SCREEN_HEIGHT_PIXELS;
				} else if (on_off_dash != 0 && follow_op == 0) {
					if (gm_playmode == 2 && g_rplybarenable != 0) {
						rplbarabovehgt = 151;
					} else {
						rplbarabovehgt = PLATFORM_SCREEN_HEIGHT_PIXELS;
					}
					mode_flag = 1;
					roofbmphgt_saved = rfy5;
					dashbmpy_copy = dashbmy9;
				} else if (gm_playmode == 2 && g_rplybarenable != 0) {
					dashbmpy_copy = 151;
				} else {
					dashbmpy_copy = PLATFORM_SCREEN_HEIGHT_PIXELS;
				}

				if (previous_roof != roofbmphgt_saved || dashbmpy_copy != bitmapdash || replaybar_height != rplbarabovehgt) {
					g_simprect = g_vid_flag6;
					set_projection(35, dashbmpy_copy / 6, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy);
					boundglassrect.top = roofbmphgt_saved;
					boundglassrect.bottom = dashbmpy_copy;
					previous_roof = roofbmphgt_saved;
					bitmapdash = dashbmpy_copy;
					replaybar_height = rplbarabovehgt;
				}
			}

			if (g_simprect != 0) {
				g_viewinx[cam_idg] = 0;
				if (mode_flag != 0) {
					/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy, rplbarabovehgt);
					setup_car_shapes(1);
				}
				if (g_rplybarenable != 0) {
					/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS);
					loop_game(1, core.game_frame, core.game_frame);
				}
			} else if (g_rplybarenable == 0) {
				g_viewinx[cam_idg] = 0;
			}

			update_frame(numid, &boundglassrect);
			if (dasty != 0 && mode_flag != 0) {
				if (statemgmtcpy != 0) {
					temp_rect.left = 0;
					temp_rect.right = PLATFORM_SCREEN_WIDTH_PIXELS;
					temp_rect.top = dasty;
					temp_rect.bottom = dashbmpy_copy;
					if (rectp != 0) {
						rcunion(rectp, &temp_rect, rectp);
					}
				}
				/* PLATFORM(video): draw a bitmap mask. */ shape2d_render_bmp_as_mask(dasm_shp_7);
				/* PLATFORM(video): draw a shape layer. */ shape2d_op_unk4(g_dastbmpbuf);
			}

			draw_clip(&boundglassrect);
			if (mode_flag != 0) {
				/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy, rplbarabovehgt);
				setup_car_shapes(2);
				/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS);
			}

			if (g_simprect != 0) {
				g_simprect--;
			}

			if (g_videoflg5 != 0) {
				/* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
				setup_mcgawnd1();
				numid ^= 1;
				cam_idg = numid;
				/* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
			}

			if (gm_playmode == 1 && g_rpl_init == 0) {
				gm_playmode = 0;
				rate_frame = frm_rate2;
				globalgamesettings.game_framespersec = frm_rate2;
				initialize_game_state(-1);
			}

			if (menutimeout != 0) {
				/* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ if (kb_get_char() != 0 || sigframe != 0 || get_kb_or_joy_flags() != 0)
					break;
				continue;
			}

			if (sigframe != 0) {
				if (gm_playmode == 0 && core.game_3F6autoLoadEvalFlag != 4)
					break;
				if (sigframe == 2)
					break;
				sigframe = 0;
				gm_playmode = 2;
				/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ mouse_minmax_position(0);
				loop_game(0, 0, 0);
				loop_game(2, 4, 0);
				inrepflg = 1;
				/* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
			}

			if (gm_playmode == 2) {
				loop_game(3, 0, 0);
				continue;
			}

			for (;;) {
				/* PLATFORM(input_kb): read the next key code. */ replay_key_code = kb_get_char();
				if (replay_key_code != 0) {
					/* PLATFORM(input_kb): dispatch a normalized game key. */ handle_ingame_kb_shortcuts(replay_key_code);
				}
				switch (replay_key_code) {
				case KEY_SCAN_UP:
				case KEY_SCAN_LEFT:
				case KEY_SCAN_RIGHT:
				case KEY_SCAN_DOWN:
					continue;
				}
				break;
			}

			if (gm_playmode == 1) {
				/* PLATFORM(input_mouse): read mouse buttons and coordinates. */ mouse_get_state(&flagsdown, &msecoordx, &pos_y_ms);
				/* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ if ((flagsdown & 3) != 0 || (get_kb_or_joy_flags() & 0x30) != 0) {
					gm_playmode = 0;
					g_rpl_init = 0;
					rate_frame = frm_rate2;
					globalgamesettings.game_framespersec = frm_rate2;
					initialize_game_state(-1);
				}
			}
		}

		if (g_videoflg5 != 0 && get_0() != 0) {
			/* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
			setup_mcgawnd2();
			/* PLATFORM(video): clear a display rectangle. */ clear_rect(0, 0, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0);
			setup_mcgawnd1();
			/* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
		}

		/* PLATFORM(video): copy the working sprite pages. */ sprcopy2to12();
		inrepflg = 1;
		/* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
		/* PLATFORM(audio): remove the audio timer hook. */ audio_remove_driver_timer();
		if (gm_playmode == 0 && globalgamesettings.game_opponenttype != 0 && core.opponentstate.car_crashBmpFlag == 0) {
			/* PLATFORM(input_joy): collect dialog joystick input. */
			/* PLATFORM(timer): wait for dialog input using timer ticks. */
			/* PLATFORM(video): render a modal dialog. */
			show_dialog(3, 0, locate_text_resource(gamerptrs, "cop"), -1, 80, performGraphColor, dialog_values, 0);
			popupact = 1;
			dialog_result = rate_frame;
			dialog_result--;
			do {
				replay_unk2(1);
				update_gamestate();
				if (++dialog_result == rate_frame) {
					dialog_result = 0;
					fmtframestr(&resbuftext, core.game_frame + elaptm1, 1);
					/* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
					/* PLATFORM(video): use the display text or sprite interface. */ draw_text_at(&resbuftext, font_op2_alt(&resbuftext), dialog_values[1]);
					/* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
				}
			} /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ while (input_do_checking(1) != 27 && core.opponentstate.car_crashBmpFlag == 0 &&
				 1500 * rate_frame != core.game_frame + elaptm1);
		}

		popupact = 0;
		/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ mouse_minmax_position(0);
		remove_frame_callback();
		free_player_cars();
	}

	waitm_ms = 100;
	/* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ check_input();
	/* PLATFORM(input_joy): collect waiting-dialog joystick input. */ /* PLATFORM(timer): wait for dialog input using timer ticks. */ /* PLATFORM(video): render a waiting dialog. */ show_waiting();
}
extern I8 byte_349BA;
extern I8 HKeyFlag;
/* Maps a normalized key code to an in-game action. Params: key code. Returns: action/status byte. State: updates game and menu state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
I8 handle_ingame_kb_shortcuts(unsigned key)
{
    switch (key) {
    case KEY_ASCII_ESCAPE:
        if (gm_playmode == 0) {
            update_crash_state(4, 0);
        }
        sigframe = 1;
        break;
    case BIOS_KEY_F2:
        cammd = 1;
        break;
    case BIOS_KEY_F3:
        cammd = 2;
        break;
    case BIOS_KEY_F4:
        cammd = 3;
        break;
    case KEY_ASCII_UPPER_H:
    case KEY_ASCII_LOWER_H:
        HKeyFlag ^= 1;
        break;
    /* PLATFORM(audio): pause and restore audio for mouse help. */
    /* PLATFORM(input_joy): collect mouse-help joystick input. */
    /* PLATFORM(input_kb): collect mouse-help key input. */
    /* PLATFORM(input_mouse): collect mouse-help pointer input. */
    /* PLATFORM(timer): wait for mouse help input using timer ticks. */
    /* PLATFORM(video): draw the mouse help interface. */
    case KEY_ASCII_UPPER_M:
    case KEY_ASCII_LOWER_M:
        do_mou_resource_text();
        /* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ mouse_minmax_position(mouse_enabled);
        break;
    case KEY_ASCII_UPPER_D:
    case KEY_ASCII_LOWER_D:
        on_off_dash ^= 1;
        break;
    case KEY_ASCII_UPPER_R:
    case KEY_ASCII_LOWER_R:
        replaybar_toggle ^= 1;
        break;
    case KEY_ASCII_UPPER_C:
    case KEY_ASCII_LOWER_C:
        if (gm_playmode != 1) {
            cammd++;
            if (cammd == 4) {
                cammd = 0;
            }
        }
        break;
    case BIOS_KEY_F1:
        cammd = 0;
        break;
    case KEY_ASCII_LOWER_T:
        if (globalgamesettings.game_opponenttype != 0) {
            follow_op ^= 1;
        }
        break;
    default:
        if (gm_playmode == 1) {
            gm_playmode = 0;
            g_rpl_init = 0;
            rate_frame = frm_rate2;
            globalgamesettings.game_framespersec = frm_rate2;
            initialize_game_state(-1);
            return 1;
        }
        return 0;
    }
    return 1;
}

/* semantic lead: init_unknown from restunts.c */
/* Resets frame-timer and replay state. Params: none. Returns: none. State: initializes timer and replay globals. */
void initialize_unknown(void)
{
	register I16 zero;
	timeraud = 1;
	slomodiv8 = 2;
	zero = 0;
	tmr2 = zero;
	g_rpl_init = sigframe = 0;
	g_sgateopn = zero;
}

typedef void (far *callback_t)(void);
extern void far frame_callback(void);
extern void far timer_reg_callback(callback_t callback);
unsigned g_clocks;
unsigned char call_proc_flag;
/* Starts per-frame callback processing. Params: none. Returns: none. State: resets the callback counter and registration flag. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far set_frame_callback(void) {
    g_clocks = 0;
    /* PLATFORM(timer): register the timer callback. */ timer_reg_callback(frame_callback);
    call_proc_flag = 0;
}


extern U32  far timer_get_counter_unk(U32  ticks);
extern void far timer_remove_callback(callback_t callback);
/* Stops per-frame callback processing. Params: none. Returns: none. State: removes the registered timer callback. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far remove_frame_callback(void)
{
    /* PLATFORM(timer): read the timer counter. */ (void)timer_get_counter_unk(10L);
    /* PLATFORM(timer): remove the timer callback. */ timer_remove_callback(frame_callback);
}

extern I16 far compare_ds_ss(void);
extern void far apply_audio_frame(I8* record, I16 frame_count);
extern I8 audio_frmarr[];
/* Processes one timer frame. Params: none. Returns: none. State: reads frame timing and updates audio-frame state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far frame_callback(void)
{
    if (compare_ds_ss() == 0) {
        return;
    }
    if (call_proc_flag != 0) {
        return;
    }

    call_proc_flag++;
    if (call_proc_flag != 1) {
        goto frame_callback_done;
    }

    snd_tick_clock++;
    if (snd_tick_clock >= frmcs_time && g_audio_frms_ix != sndposrecord) {
        /* PLATFORM(audio): apply recorded engine audio state. */ apply_audio_frame(&audio_frmarr[g_audio_frms_ix * 0x22], snd_tick_clock);
        snd_tick_clock = 0;
        g_audio_frms_ix++;
        if (g_audio_frms_ix == 0x28) {
            g_audio_frms_ix = 0;
        }
    }

    if (sigframe != 0 || g_rplbfask != 0) {
        goto frame_callback_done;
    }
    if (inrepflg != 0 && gm_playmode == 2) {
        goto frame_callback_done;
    }
    if (gm_playmode == 0 && core.game_frame_in_sec >= core.game_frames_per_sec) {
        inrepflg = 1;
        /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
        goto frame_callback_done;
    }

    timeraud--;
    if (timeraud == 0) {
        timeraud = (I8)frmcs_time;
        g_clocks++;
        if (gm_playmode == 2) {
            switch (g_rplmodui) {
            case 2:
                slomodiv8--;
                if (slomodiv8 == 0) {
                    replay_unk2(0);
                    slomodiv8 = 2;
                }
                goto frame_callback_done;
            case 3:
                replay_unk2(0);
                break;
            default:
                break;
            }
        }
        replay_unk2(0);
    }

frame_callback_done:
    call_proc_flag--;
}

signed char array_rpl[64];
unsigned char replay_steer_flag[64];
static I8 replay_control;
extern I16 far kb_get_key_state(I16);
extern I8 far replay_axis_value(void);
extern I16 far abs(I16);
/* Processes replay input for the selected mode. Params: mode. Returns: none. State: reads and updates replay, camera and input state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
void far replay_unk2(I16 mode)
{
    register I16 input;
    register I16 i;

    if (mode != 0) {
        input = 0;
        goto record_input;
    }
    if (gm_playmode == 2) {
        if (tmr2 < globalgamesettings.game_recordedframes) {
            tmr2++;
            return;
        }
        if (sigframe != 0)
            return;
        inrepflg = 1;
        /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
replay_finished:
        sigframe = 1;
        return;
    }
    if (sigframe != 0 || core.game_3F6autoLoadEvalFlag != 0 || gm_playmode == 1) {
        input = 0;
        goto record_input;
    }
    if (pass_check_flag == 0 && g_rpl_init == 0 && rate_frame * 4 < core.game_frame)
        update_crash_state(1, 0);
    if (mouse_enabled != 0 || joystick_enabled != 0) {
        if (mouse_enabled != 0) {
            /* PLATFORM(input_mouse): read mouse buttons and coordinates. */ mouse_get_state(&flagsdown, &msecoordx, &pos_y_ms);
            i = msecoordx - 0xA0;
            if (abs(i) < 0x12)
                i = 0;
            else if (i > 0)
                i -= 0x12;
            else
                i += 0x12;
            replay_control = i;
            if (flagsdown & 1)
                input = 2;
            else if (flagsdown & 2)
                input = 1;
            else
                input = 0;
        } else {
            replay_control = replay_axis_value();
            if (replay_control > 0)
                replay_control = replay_axis_magnitude[replay_control];
            else if (replay_control < 0)
                replay_control = -replay_axis_magnitude[-replay_control];
            /* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ input = get_kb_or_joy_flags() & 0x33;
        }
        i = tmr2 & 0x3F;
        array_rpl[i] = replay_control;
        replay_steer_flag[i] = 1;
    } else {
        /* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ input = get_kb_or_joy_flags();
    }
    /* PLATFORM(input_kb): read a key state. */ if (kb_get_key_state(0x1E))
        input |= 0x10;
    /* PLATFORM(input_kb): read a key state. */ if (kb_get_key_state(0x2C))
        input |= 0x20;
record_input:
    if (1500 * rate_frame <= tmr2 + elaptm1) {
        update_crash_state(4, 0);
        goto replay_finished;
    }
    if (tmr2 == 12000) {
        if (elaptm1 == 0 && popupact == 0) {
            popupact = 1;
            g_rplbfask = 1;
            return;
        }
        for (i = 0; i < 12000 / (30 * rate_frame) - 1; i++) {
            cvxs_a[i + 1].game_frame -= 30 * rate_frame;
            cvxs_a[i] = cvxs_a[i + 1];
        }
        for (i = 0; i < 12000 - 30 * rate_frame; i++)
            g_tdreplay16buf[i] = g_tdreplay16buf[i + 30 * rate_frame];
        tmr2 -= 30 * rate_frame;
        globalgamesettings.game_recordedframes -= 30 * rate_frame;
        elaptm1 += 30 * rate_frame;
        core.game_frame -= 30 * rate_frame;
    }
    g_tdreplay16buf[tmr2++] = input;
    globalgamesettings.game_recordedframes++;
}

extern I8 trk_sample_count;
/* Selects the car used as the camera target and refreshes its coordinates. Params: none. Returns: none. State: reads car states and updates camera-target globals. */
void far update_camera_target(void)
{
    I16 nearest_range;
    I16 car_count;
    I32 dz;
    struct CARSTATE *selected_car_state;
    struct VECTOR car_position;
    register I16 index;
    register I16 delta;
    I16S bearing;
    I16S facing;
    I16 cam_y;
    I16 climb;
    I8 track_segment;
    struct VECTOR focus;
    I16 point_range;
    I32 dx;
    I16 threshold;

    car_count = 1;
    if (globalgamesettings.game_opponenttype != 0)
        car_count = 2;
    for (index = 0; index < car_count; ++index) {
        core.game_vec3[index] = core.game_vec1[index];
        if (index == 0)
            selected_car_state = &core.playerstate;
        else
            selected_car_state = &core.opponentstate;

        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.y = (I16S)(selected_car_state->car_posWorld1.ly >> WORLD_COORDINATE_SHIFT);
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.x = (I16S)(selected_car_state->car_posWorld1.lx >> WORLD_COORDINATE_SHIFT);
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.z = (I16S)(selected_car_state->car_posWorld1.lz >> WORLD_COORDINATE_SHIFT);
        focus = selected_car_state->car_vec_unk3;
        facing = selected_car_state->field_48;

        if (index == 0 && (core.field_45B != 0 || core.field_45C != 0))
            focus = car_position;
        else if (selected_car_state->field_B6 != 0 || selected_car_state->car_crashBmpFlag != 0 ||
                 selected_car_state->car_trackdata3_index == -1 ||
                 (facing > 0x80 && facing < 0x380))
            focus = car_position;

        threshold = 0x1C2;
        cam_y = car_position.y + 0x10E;
        climb = core.game_vec1[index].y - cam_y;
        if (climb != 0) {
            delta = climb;
            if (delta > 0x1E)
                delta = 0x1E;
            else if (delta < -0x1E)
                delta = -0x1E;
            core.game_vec1[index].y -= delta;
        }

        bearing = polang(focus.x - core.game_vec1[index].x,
                                  focus.z - core.game_vec1[index].z);
        delta = polradius2d(car_position.x - core.game_vec1[index].x,
                              car_position.z - core.game_vec1[index].z);
        if (delta > threshold) {
            delta -= threshold;
            if (rate_frame == 0x14) {
                if (delta > 0x78) delta = 0x78;
            } else if (delta > 0xF0) {
                delta = 0xF0;
            }
            core.game_vec1[index].x += mulscl(delta, sinfast(bearing));
            core.game_vec1[index].z += mulscl(delta, cosfast(bearing));
        }

        if (core.game_frame % (rate_frame >> 1) == 0) {
            nearest_range = 0x2710;
            for (track_segment = 0; track_segment < trk_sample_count; ++track_segment) {
                dx = (I32)trkptrpath[track_segment].x - car_position.x;
                dz = (I32)trkptrpath[track_segment].z - car_position.z;
                if ((dx < 0 ? -dx : dx) < nearest_range &&
                    (dz < 0 ? -dz : dz) < nearest_range) {
                    point_range = polradius2d((I16)dx, (I16)dz);
                    if (point_range < nearest_range) {
                        core.field_3F7[index] = (I8)track_segment;
                        nearest_range = point_range;
                    }
                }
            }
        }
    }
}

/* semantic lead: file_load_replay from fileio.c */
/* Builds and reads the replay file header. Params: directory and replay name. Returns: load status. State: writes the replay header buffer. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
I8 file_load_replay(const I8* dir, const I8* name)
{
	/* PLATFORM(file): build a file path. */ file_build_path(dir, name, ".rpl", buf_g_path);

	g_is_busy = 1;
	/* PLATFORM(file): read replay data from a file. */ file_read_fatal(buf_g_path, td13_replay_hdr);
	globalgamesettings = *(struct GAMEINFO far*)td13_replay_hdr;
	g_is_busy = 0;
	return 0;
}

/* semantic lead: file_write_replay from fileio.c */
/* Writes the replay header and data. Params: filename. Returns: write status. State: reads replay buffers. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
I16S file_write_replay(const I8* filename)
{
	register I16 ret;
	I32 write_length;

	*(struct GAMEINFO far*)td13_replay_hdr = globalgamesettings;
	write_length = globalgamesettings.game_recordedframes + 0x724;
	g_is_busy = 1;
	/* PLATFORM(file): write replay data to a file. */ ret = file_write_fatal(filename, td13_replay_hdr, write_length);
	g_is_busy = 0;
	return (I8)ret;
}

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct SHAPE2D {
    I16S s2d_width;
    U16S  s2d_height;
    U16S  s2d_unk1;
    U16S  s2d_unk2;
    U16S  s2d_pos_x;
    U16S  s2d_pos_y;
    U8  s2d_unk3;
    U8  s2d_unk4;
    U8  s2d_unk5;
    U8  s2d_unk6;
};
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    U16S  sprite_unk1;
    U16S  sprite_unk2;
    U16S  sprite_unk3;
    U16  *sprite_lineofs;
    U16S  sprite_left;
    U16S  sprite_right;
    U16S  sprite_top;
    U16S  sprite_height;
    U16S  sprite_pitch;
    U16S  sprite_unk4;
    U16S  sprite_width2;
    U16S  sprite_left2;
    U16S  sprite_widthsum;
};

extern I8 aWhl1whl2whl3ins2gboxins1i[];
extern I8 aGnobgnabdotDotadot1dot2[];
extern I8 aDig0dig1dig2dig3dig4dig5d[];
extern I8 aDash[];
extern I8 aRoof[];
extern I8 aRoof_0[];
extern I8 aRoof_1[];
extern I8 aRoof_2[];
extern I8 aDash_0[];
extern I8 aDast[];
extern I8 aDasm[];
extern void far *file_load_resource(I16 type, const I8 *name);
extern void locate_many_resources(void far *data, I8 *names, I8 far **result);
extern I8 far *locate_shape_nofatal(void far *data, I8 *name);
extern I8 far *locate_shape_fatal(void far *data, I8 *name);
extern struct SPRITE far *sprite_make_window(U16  width, U16  height, U16  color);
extern void sprite_free_window(struct SPRITE far *sprite);
extern void sprite_setup1_from_arg_pointer(struct SPRITE far *sprite);
extern void sprite_copy_2_to_1(void);
extern void sprite_putimage_and_alt(struct SHAPE2D far *shape, I16 x, I16 y);
extern void sprite_putimage_and_alt2(struct SHAPE2D far *shape, I16 x, I16 y);
extern void sprite_putimage_or_alt(struct SHAPE2D far *shape, I16 x, I16 y);
extern void shape2d_op_unk(struct SHAPE2D far *shape);
extern void shape2d_op_unk2(struct SHAPE2D far *shape, I16 x, I16 y);
extern void shape2d_op_unk3(struct SHAPE2D far *shape);
extern void shape2d_op_unknown5(struct SHAPE2D far *shape, I16 x, I16 y);
extern void sprite_putimage_or(struct SHAPE2D far *shape, I16 x, I16 y);
extern void sprite_clear_shape_alt(struct SHAPE2D far *shape, I16 x, I16 y);
extern void preRender_line(I16 x1, I16 y1, I16 x2, I16 y2, I16 color);
extern void far *mmgr_free(I8 far *ptr);
I8 aStdaxxxx[] = "stdaxxxx";
I8 aStdbxxxx[] = "stdbxxxx";
/* Prepares car dashboard and shape sprites. Params: mode. Returns: none. State: reads car/camera state and updates shape/resource handles. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(video): draws to or configures the display surface. */
void far setup_car_shapes(I16 mode)
{
    extern struct SIMD simdp7;
    extern I16S vidflg3is_minus1;
    static I16S last_steering_step[2];
    static I8 steering_zone[2];
    static struct SPRITE far * meters_sprite;
    static struct SPRITE far * gnob_sprite;
    static struct SPRITE far * gear_base_sprite;
    static void far * stdares;
    static void far * stdbres;
    static struct SHAPE2D far * wheel_shapes[9];
    static struct SHAPE2D far * gnobshapes[6];
    static struct SHAPE2D far * digshapes[10];
    extern I16S spdneedlegaugeclr;
    static I8 gear_knob_visible_view[2];
    static I16S steering_dot_x[2];
    static I16S steering_dot_y[2];
    static I16S last_speedo[2];
    static I16S gear_knob_x_last[2];
    static I16S gear_knob_y_last[2];
    static I16S last_tacho[2];
    struct SHAPE2D far *shape;
    I16 digit;
    I8 speedo_type;
    I16 steering;
    I8 wheel_zone;
    U8  *dot;
    register I16 x;
    U8  x_pos;
    register I16 y;
    I8 changed;
    U8  y_pos;
    I8 knob_removed;
    I8 has_digit;

    switch (mode) {
    case 0:
        aStdaxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdaxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdaxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdaxxxx[7] = globalgamesettings.game_playercarid[3];
        aStdbxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdbxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdbxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdbxxxx[7] = globalgamesettings.game_playercarid[3];
        /* PLATFORM(file): load a named resource. */ stdares = file_load_resource(3, aStdaxxxx);
        /* PLATFORM(file): load a named resource. */ stdbres = file_load_resource(2, aStdbxxxx);
        locate_many_resources(stdares, "whl1whl2whl3ins2gboxins1ins3inm1inm3", (I8 far **)wheel_shapes);
        locate_many_resources(stdbres, "gnobgnabdot dotadot1dot2", (I8 far **)gnobshapes);
        if (simdp7.spdcenter.py == 0)
            locate_many_resources(stdbres, "dig0dig1dig2dig3dig4dig5dig6dig7dig8dig9", (I8 far **)digshapes);
        /* PLATFORM(video): create an offscreen sprite surface. */ meters_sprite = sprite_make_window(wheel_shapes[3]->s2d_width * pixel_scales, wheel_shapes[3]->s2d_height, 15);
        /* PLATFORM(video): create an offscreen sprite surface. */ gnob_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        /* PLATFORM(video): create an offscreen sprite surface. */ gear_base_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        shape = (struct SHAPE2D far *)locate_shape_fatal(stdares, "dash");
        /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(gear_base_sprite);
        /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk2(shape, shape->s2d_pos_x - wheel_shapes[4]->s2d_pos_x,
                        shape->s2d_pos_y - wheel_shapes[4]->s2d_pos_y);
        /* PLATFORM(video): copy the working sprite page. */ sprite_copy_2_to_1();
        dashbmy9 = shape->s2d_pos_y;
        if (locate_shape_nofatal(stdares, "roof") != 0)
            rfy5 = ((struct SHAPE2D far *)locate_shape_fatal(stdares, "roof"))->s2d_height;
        else
            rfy5 = 0;
        shape = (struct SHAPE2D far *)locate_shape_nofatal(stdares, "dast");
        if (shape != 0) {
            dasty = shape->s2d_pos_y;
            g_dastbmpbuf = shape;
            dasm_shp_7 = locate_shape_fatal(stdares, "dasm");
            return;
        }
        dasty = 0;
        return;
    /* PLATFORM(video): select the opaque drawing path. */ case 1:
        msdrawopaquechk();
        /* PLATFORM(video): draw a shape layer. */ if (locate_shape_nofatal(stdares, "roof") != 0)
            shape2d_op_unk((struct SHAPE2D far *)locate_shape_fatal(stdares, "roof"));
        /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk3((struct SHAPE2D far *)locate_shape_fatal(stdares, "dash"));
        /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk3(wheel_shapes[1]);
        /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
        x = 0;
        gear_knob_visible_view[cam_idg] = g_viewinx[cam_idg] = 0;
        steering_dot_y[cam_idg] = x;
        steering_zone[cam_idg] = (I8)x;
        x--;
        last_steering_step[cam_idg] = x;
        last_speedo[cam_idg] = x;
        last_tacho[cam_idg] = x;
        return;
    case 2:
        if ((core.playerstate.car_changing_gear | core.playerstate.car_fpsmul2) == 0 &&
            gear_knob_visible_view[cam_idg] != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                msdrawopaquechk();
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(gear_base_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
            gear_knob_visible_view[cam_idg] = 0;
        } else if (gear_knob_visible_view[cam_idg] != core.playerstate.car_changing_gear ||
                   gear_knob_x_last[cam_idg] != core.playerstate.car_knob_x ||
                   gear_knob_y_last[cam_idg] != core.playerstate.car_knob_y ||
                   (core.playerstate.car_fpsmul2 != 0 && gear_knob_visible_view[cam_idg] == 0)) {
            /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(gnob_sprite);
            gear_knob_visible_view[cam_idg] = 1;
            /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk2(wheel_shapes[4], 0, 0);
            x = core.playerstate.car_knob_x;
            y = core.playerstate.car_knob_y;
            gear_knob_x_last[cam_idg] = x;
            gear_knob_y_last[cam_idg] = y;
            /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt2(gnobshapes[1], x, y);
            /* PLATFORM(video): blit a shape with the OR raster operation. */ sprite_putimage_or_alt(gnobshapes[0], x, y);
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            else {
                /* PLATFORM(video): copy the working sprite pages. */ sprcopy2to12();
                /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            }
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(gnob_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
        }

        knob_removed = 0;
        steering = core.playerstate.car_steeringAngle / 8;
        wheel_zone = 1;
        if (steering < -10)
            wheel_zone = 0;
        else if (steering > 10)
            wheel_zone = 2;
        if (steering_zone[cam_idg] != wheel_zone || g_simprect != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                msdrawopaquechk();
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            switch (wheel_zone) {
            /* PLATFORM(video): draw a shape layer. */ case 0:
                shape2d_op_unk3(wheel_shapes[0]);
                break;
            /* PLATFORM(video): draw a shape layer. */ case 1:
                shape2d_op_unk3(wheel_shapes[1]);
                break;
            /* PLATFORM(video): draw a shape layer. */ case 2:
                shape2d_op_unk3(wheel_shapes[2]);
                break;
            }
            steering_zone[cam_idg] = wheel_zone;
            changed = 1;
        } else
            changed = 0;

        switch (simdp7.spdcenter.py) {
        case -1:
            x = 0;
            speedo_type = 2;
            break;
        default:
            speedo_type = 0;
            x = core.playerstate.car_speed / 0x280;
            if (x >= simdp7.spdnumpoints)
                x = simdp7.spdnumpoints - 1;
            break;
        case 0:
            speedo_type = 1;
            /* PORT: car_speed is unsigned 16-bit Q8 mph; this shift is a zero-fill conversion. */
            x = core.playerstate.car_speed >> CAR_SPEED_Q8_SHIFT;
        }
        y = (U16S )core.playerstate.car_currpm >> 7;
        if (y >= simdp7.revnumpoints)
            y = simdp7.revnumpoints - 1;

        if (changed != 0 || g_simprect != 0 ||
            last_speedo[cam_idg] != x || last_tacho[cam_idg] != y) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                msdrawopaquechk();
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(meters_sprite);
            /* PLATFORM(video): draw a shape layer. */ shape2d_op_unknown5(wheel_shapes[3], 0, 0);
            last_speedo[cam_idg] = x;
            last_tacho[cam_idg] = y;
            if (speedo_type == 1) {
                digit = 0;
                if (x >= SPEEDOMETER_TWO_HUNDREDS_BASE) {
                    digit = 2;
                    x -= SPEEDOMETER_TWO_HUNDREDS_BASE;
                } else if (x >= SPEEDOMETER_HUNDREDS_BASE) {
                    digit = 1;
                    x -= SPEEDOMETER_HUNDREDS_BASE;
                }
                if (digit != 0) {
                    /* PLATFORM(video): blit a shape with the OR raster operation. */ sprite_putimage_or(digshapes[digit], simdp7.spdpoints[0], simdp7.spdpoints[1]);
                    has_digit = 1;
                }
                digit = x / 10;
                if (digit != 0 || has_digit != 0) {
                    /* PLATFORM(video): blit a shape with the OR raster operation. */ sprite_putimage_or(digshapes[digit], simdp7.spdpoints[2], simdp7.spdpoints[3]);
                    x -= digit * 10;
                    has_digit = 1;
                }
                /* PLATFORM(video): blit a shape with the OR raster operation. */ sprite_putimage_or(digshapes[x], simdp7.spdpoints[4], simdp7.spdpoints[5]);
            } else if (speedo_type == 0) {
                /* PLATFORM(video): draw a preview line. */ preRender_line(simdp7.spdcenter.px, simdp7.spdcenter.py,
                               simdp7.spdpoints[x * 2], simdp7.spdpoints[x * 2 + 1],
                               spdneedlegaugeclr);
            }
            /* PLATFORM(video): draw a preview line. */ preRender_line(simdp7.revcenter.px, simdp7.revcenter.py,
                           simdp7.revpoints[y * 2], simdp7.revpoints[y * 2 + 1],
                           spdneedlegaugeclr);
            switch (wheel_zone) {
            /* PLATFORM(video): draw a bitmap mask. */ case 0:
                shape2d_render_bmp_as_mask(wheel_shapes[7]);
                /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk4(wheel_shapes[5]);
                break;
            /* PLATFORM(video): draw a bitmap mask. */ case 2:
                shape2d_render_bmp_as_mask(wheel_shapes[8]);
                /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk4(wheel_shapes[6]);
                break;
            }
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            /* PLATFORM(video): copy the working sprite pages. */ else
                sprcopy2to12();
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(meters_sprite->sprite_bitmapptr, wheel_shapes[3]->s2d_pos_x, wheel_shapes[3]->s2d_pos_y);
        }

        if (last_steering_step[cam_idg] != steering || g_simprect != 0 || knob_removed != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                msdrawopaquechk();
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
            }
            dot = &simdp7.steeringdots[(steering < 0 ? -steering : steering) * 2];
            y_pos = dot[1];
            x_pos = dot[0];
            if (steering < 0)
                x_pos -= (x_pos - simdp7.steeringdots[0]) * 2;
            steering_dot_x[cam_idg] = (x_pos - gnobshapes[2]->s2d_unk1) & vidflg3is_minus1;
            steering_dot_y[cam_idg] = y_pos - gnobshapes[2]->s2d_unk2;
            /* PLATFORM(video): use the display text or sprite interface. */ sprite_clear_shape_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
            /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt2(gnobshapes[3], x_pos, y_pos);
            /* PLATFORM(video): blit a shape with the OR raster operation. */ sprite_putimage_or_alt(gnobshapes[2], x_pos, y_pos);
            last_steering_step[cam_idg] = steering;
        }
        /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
        return;
    /* PLATFORM(video): release an offscreen sprite surface. */ case 3:
        sprite_free_window(gear_base_sprite);
        /* PLATFORM(video): release an offscreen sprite surface. */ sprite_free_window(gnob_sprite);
        /* PLATFORM(video): release an offscreen sprite surface. */ sprite_free_window(meters_sprite);
        /* PLATFORM(memory): release resource memory. */ mmgr_free((I8 far *)stdbres);
        /* PLATFORM(memory): release resource memory. */ mmgr_free((I8 far *)stdares);
        return;
    }
}

/* semantic lead: setup_player_cars from restunts.c */
extern void far *file_load_resource_file(I8 *name);
extern void far *locate_shape_alt(void far *res, I8 *name);
extern I16 audio_init_engine(I16 id, void far *data, void far *eng1, void far *eng);
extern I32 mmgr_get_res_ofs_diff_scaled(void);
/* Loads player/opponent car and engine resources. Params: none. Returns: setup status. State: updates resource handles and engine IDs. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
I16 setup_player_cars(void) {
	void far* carresptr;
	I32 mem_limit;

	g_wndspr = 0;
	/* PLATFORM(file): check required files and request missing files. */
	/* PLATFORM(input_joy): collect file-confirmation joystick input. */
	/* PLATFORM(input_kb): collect file-confirmation key input. */
	/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
	/* PLATFORM(timer): wait for file confirmation using timer ticks. */
	/* PLATFORM(video): show missing-file guidance. */
	ensure_file_exists(2);
	shape3d_load_car_shapes(globalgamesettings.game_playercarid, globalgamesettings.game_opponentcarid);
	aCarcoun[3] = globalgamesettings.game_playercarid[0];
	aCarcoun[4] = globalgamesettings.game_playercarid[1];
	aCarcoun[5] = globalgamesettings.game_playercarid[2];
	aCarcoun[6] = globalgamesettings.game_playercarid[3];
	/* PLATFORM(file): load a resource file. */ carresptr = file_load_resource_file(aCarcoun);
	setup_aero_trackdata(carresptr, 0);
	/* PLATFORM(memory): release a loaded resource. */ unload_resource(carresptr);

	if (globalgamesettings.game_opponenttype != 0) {
		aCarcoun[3] = globalgamesettings.game_opponentcarid[0];
		aCarcoun[4] = globalgamesettings.game_opponentcarid[1];
		aCarcoun[5] = globalgamesettings.game_opponentcarid[2];
		aCarcoun[6] = globalgamesettings.game_opponentcarid[3];
		/* PLATFORM(file): load a resource file. */ carresptr = file_load_resource_file(aCarcoun);
		setup_aero_trackdata(carresptr, 1);
		/* PLATFORM(memory): release a loaded resource. */ unload_resource(carresptr);
		
		/* PLATFORM(file): check required files and request missing files. */
		/* PLATFORM(input_joy): collect file-confirmation joystick input. */
		/* PLATFORM(input_kb): collect file-confirmation key input. */
		/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
		/* PLATFORM(timer): wait for file confirmation using timer ticks. */
		/* PLATFORM(video): show missing-file guidance. */
		ensure_file_exists(4);
		load_opponent_data();
	}

	/* PLATFORM(file): check required files and request missing files. */
	/* PLATFORM(input_joy): collect file-confirmation joystick input. */
	/* PLATFORM(input_kb): collect file-confirmation key input. */
	/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
	/* PLATFORM(timer): wait for file confirmation using timer ticks. */
	/* PLATFORM(video): show missing-file guidance. */
	ensure_file_exists(3);
	/* PLATFORM(file): load a named resource. */ eng1resourceptr = file_load_resource(5, "eng1");//aEng1); // "eng1"
	/* PLATFORM(file): load a named resource. */ engdata = file_load_resource(6, "eng");//aEng); // "eng"
	/* PLATFORM(audio): install the audio timer hook. */ audio_add_driver_timer();
	/* PLATFORM(audio): start an engine audio stream. */ g_player_sound_id = audio_init_engine(0x21, &player_engine_profile, eng1resourceptr, engdata);

	sndpendingstate = 0;
	g_plyr_snd_state = 0;
	audiooppflag = 0;
	if (globalgamesettings.game_opponenttype != 0) {
		/* PLATFORM(audio): start an engine audio stream. */ op_eng_sound_id = audio_init_engine(0x20, &opponent_engine_profile, eng1resourceptr, engdata);
	}

	g_audio_frms_ix = 0;
	sndposrecord = 0;
	snd_tick_clock = 0;
	/* PLATFORM(file): load a named resource. */ fntled_res = file_load_resource(0, "fontled.fnt");//aFontled_fnt); // "fontled.fnt"
	statemgmtcpy = slow_video_mode_state;
	init_rect_arrays();
	if (menutimeout == 0) {
		setup_car_shapes(0);
	}

	if (menutimeout == 0) {
		/* PLATFORM(file): load a named resource. */ sdgresourcehandle = file_load_resource(3, "sdgame");//aSdgame); // "sdgame"
		loop_game(0, 0, 0);
	}

	/* PLATFORM(file): load a resource file. */ gamerptrs = file_load_resource_file("game");
	g_planlist = locate_shape_alt(gamerptrs, "plan");//aPlan); // "plan"
	wallrecrecord = locate_shape_alt(gamerptrs, "wall");//aWall); // "wall"
	load_sdgame2_shapes();
	load_skybox(td14tb[0x384]);
	if (shape3d_load_all() != 0) {
		return 1;
	}

	if (g_videoflg5 == 0) {
		
		mem_limit = 0xFA00L / (pixel_scales * vidflg4_is1) + 0x12;
		/* PLATFORM(memory): query available resource memory. */ if (mmgr_get_res_ofs_diff_scaled() <= mem_limit) {
			return 1;
		}
		/* PLATFORM(video): create an offscreen sprite surface. */ g_wndspr = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0x0F);
	}

	follow_op = 0;
	is_in_rplcopy = -1;
	return 0;
}

/* semantic lead: free_player_cars from restunts.c */
/* Releases player/opponent resources and audio hooks. Params: none. Returns: none. State: reads and clears resource handles. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(video): draws to or configures the display surface. */
void free_player_cars(void) {
	if (g_videoflg5 == 0) {
		if (g_wndspr != 0) {
			/* PLATFORM(video): release an offscreen sprite surface. */ sprite_free_window(g_wndspr);
		}
	}
	shape3d_free_all();
	unload_skybox();
	free_sdgame2();
	/* PLATFORM(memory): release a loaded resource. */ unload_resource(gamerptrs);
	if (menutimeout == 0) {
		/* PLATFORM(memory): release resource memory. */ mmgr_free(sdgresourcehandle);
		setup_car_shapes(3);
	}

	/* PLATFORM(memory): release resource memory. */ mmgr_free(fntled_res);
	/* PLATFORM(audio): remove the audio timer hook. */ audio_remove_driver_timer();
	/* PLATFORM(memory): release resource memory. */ mmgr_free(engdata);
	/* PLATFORM(memory): release resource memory. */ mmgr_free(eng1resourceptr);
	shape3d_free_car_shapes();
}

void mouse_set_minmax(I16, I16, I16, I16);
void mouse_set_position(I16, I16);
/* Sets the in-game mouse bounds. Params: enabled flag. Returns: none. State: maps the flag to mouse range and position. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
void mouse_minmax_position(I16 enabled)
{
    if (enabled) {
        /* PLATFORM(input_mouse): set the mouse coordinate range. */ mouse_set_minmax(15, 0, 0x131, PLATFORM_SCREEN_HEIGHT_PIXELS);
        /* PLATFORM(input_mouse): set the mouse position. */ mouse_set_position(0xA0, 0x64);
        return;
    }
    /* PLATFORM(input_mouse): set the mouse coordinate range. */ mouse_set_minmax(0, 0, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);
}
/* Applies the current replay steering sample. Params: none. Returns: none. State: reads replay samples and updates steering state. */
void far replay_unk(void)
{
    register I16 frame_index = core.game_frame & 0x3F;
    register I16 steering;
    I8 speed_index;
    I8 response;
    I8 angle;

    if (replay_steer_flag[frame_index] == 0)
        return;

    steering = array_rpl[frame_index];
    speed_index = (I8)((core.playerstate.car_speed2 >> 10) & 0xFC);
    response = ((I8*)table_lookup)[(I16)speed_index + 1];

    if (core.playerstate.car_steeringAngle < steering) {
        if (core.playerstate.car_steeringAngle < -1)
            response <<= 2;
    } else if (core.playerstate.car_steeringAngle > steering) {
        if (core.playerstate.car_steeringAngle > 1)
            response <<= 2;
    }

    if (core.playerstate.car_steeringAngle > steering &&
        core.playerstate.car_steeringAngle - response >= steering) {
        angle = 8;
    } else if (core.playerstate.car_steeringAngle < steering &&
               core.playerstate.car_steeringAngle + response <= steering) {
        angle = 4;
    } else {
        angle = 0;
    }

    if (angle != 0)
        g_tdreplay16buf[core.game_frame] |= angle;
    replay_steer_flag[frame_index] = 0;
}

I8 camera_button_index = 6;
I8 camera_button_count_mode[10] = { 1, 7, 3, 4, 5, 6, 7, 8, 8, 0 };
I8 camera_mode_select_a[10] = { 0, 0, 2, 2, 3, 4, 5, 1, 7, 0 };
I8 camera_mode_select_b[10] = { 2, 6, 2, 3, 4, 5, 6, 7, 8, 0 };
I8 camera_mode_select_c[10] = { 0, 1, 0, 0, 1, 1, 1, 7, 8, 0 };
I8 game_camera_buttons_count[4] = { 6, 6, 8, 7 };
I16 game_camera_buttons_x1[9] = { 272, 109, 274, 232, 190, 151, 108, 66, 10 };
I16 game_camera_buttons_x2[9] = { 314, 151, 314, 274, 232, 190, 151, 91, 47 };
I16 game_camera_buttons_y1[9] = { 176, 176, 156, 156, 156, 156, 156, 156, 156 };
I16 game_camera_buttons_y2[9] = { 193, 193, 173, 173, 173, 173, 173, 193, 193 };
I16 gameunk_button_x1[1] = { 0 };
I16 gameunk_button_x2[1] = { 104 };
I16 gameunk_button_y1[1] = { 151 };
I16 gameunk_button_y2[1] = { PLATFORM_SCREEN_HEIGHT_PIXELS };
static I8 view_camera_choice[2];
static I8 camera_buttons_pressed[9];
/* One unreferenced target byte; semantic type/name are unrecoverable and its slot is hash-constrained. */
static I8 unused_40E73;
static I8 camera_mode_view[2];
static I8 camera_buttons_state_per_view[18];
extern I16 camera_select_fill_color;
extern I16 camera_select_outline_color;
static I16 camera_button_row[2];
static I16 camera_button_tick[2];
static I16 camera_button_col_cache[2];
int viewyshift;
extern I8 kbjoyflags;
extern I16 custom_azim_angle;
extern I16 custom_dist;
extern I16 custom_elev_angle;

extern I16 dlg_colour;
extern struct RECTANGLE *rcpunk2;
static void far *replayshapes[23];
extern void far font_setup_unknown(I16, I16);
extern void far fontsetfontdef2(void far *);
extern void far sprite_1_unk(I16, I16, I16, I16, I16);
extern void far sprite_1_unk4(I16, I16, I16, I16, I16);
extern I16 far input_checking(I16);
extern I16 far mouse_multi_hittest(I16, I16 *, I16 *, I16 *, I16 *);
extern I16 far kb_get_key_state(I16);
extern I8 far do_fileselect_dialog(I8 *, I8 *, I8 *, void far *);
extern I8 far do_savefile_dialog(I8 *, I8 *, void far *);
extern I16 far file_find(I8 *);
extern void far copy_string(I8 *, I8 far *);
extern struct RECTANGLE * far introtext(I8 *, I16, I16, I16, I16);

extern I8 aDefault_1[];
extern I16 far file_find(I8 *path);
extern struct RECTANGLE *introtext(I8 *text, I16 x, I16 y, I16 color, I16 unk);
extern void far copy_string(I8 *dst, I8 far *src);
/* Runs one game/replay loop step and updates its UI. Params: mode, frame index and frame offset. Returns: none. State: reads and updates game, replay, camera and UI state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
void loop_game(I16 mode, I16 frame_index, I16 frame_offset)
{
    I8 answer;
    unsigned key_code;
    I16 dialog_params[8];
    I16 prev_sky;
    I16 time_delta;
    register I16 i;
    register I16 j;
    I8 write_state;
    I8 button;
    I32 travel;
    I8 modifier;
    struct GAMEINFO oldcfg;

    switch (mode) {
    case 0:
        locate_many_resources(sdgresourcehandle,
            "rplyrpicrpacrpmcrptcbof6bof5bof4bof3bof2bof1bof0zoompannbon6bon5bon4bon3bon2bon1bof0zoompann",
            (I8 far **)replayshapes);
        frame_index = 4;
    case 2:
        for (i = 0; i < 9; i++)
            camera_buttons_pressed[i] = 0;
        camera_buttons_pressed[frame_index] = 1;
        break;
    case 1:
        if (g_viewinx[cam_idg] == 0) {
            g_viewinx[cam_idg] = 1;
            camera_mode_view[cam_idg] = -1;
            view_camera_choice[cam_idg] = -1;
            for (i = 0; i < 9; i++)
                camera_buttons_state_per_view[i * 2 + cam_idg] = 0;
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk(replayshapes[0]);
            camera_button_tick[cam_idg] = -1;
            camera_button_col_cache[cam_idg] = -1;
            fmtframestr(&resbuftext, globalgamesettings.game_recordedframes + elaptm1, 1);
            /* PLATFORM(video): set the font drawing state. */ font_setup_unknown(dlg_colour, 0);
            /* PLATFORM(video): select a font definition. */ fontsetfontdef2(fntled_res);
            /* PLATFORM(video): draw text at screen coordinates. */ draw_text_at(&resbuftext, 0xD8, 0xBB);
            /* PLATFORM(video): select the active font definition. */ fontsetfontdef();
        }
        if (camera_button_tick[cam_idg] != frame_offset + elaptm1) {
            camera_button_tick[cam_idg] = frame_offset + elaptm1;
            fmtframestr(&resbuftext, frame_offset + elaptm1, 1);
            /* PLATFORM(video): set the font drawing state. */ font_setup_unknown(dlg_colour, 0);
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            /* PLATFORM(video): select a font definition. */ fontsetfontdef2(fntled_res);
            /* PLATFORM(video): draw text at screen coordinates. */ draw_text_at(&resbuftext, 0x98, 0xBB);
            /* PLATFORM(video): select the active font definition. */ fontsetfontdef();
        }
        if (camera_mode_view[cam_idg] != cammd) {
            camera_mode_view[cam_idg] = cammd;
            camera_button_col_cache[cam_idg] = -1;
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk(replayshapes[cammd + 1]);
            if (game_camera_buttons_count[cammd] < camera_button_index)
                camera_button_index = game_camera_buttons_count[cammd];
            if (view_camera_choice[cam_idg] > 6)
                view_camera_choice[cam_idg] = -1;
        }
        if (globalgamesettings.game_recordedframes == 0) {
            i = 0;
            j = 0;
        } else {
            i = (I32)frame_index * 110 / globalgamesettings.game_recordedframes;
            j = (I32)frame_offset * 110 / globalgamesettings.game_recordedframes;
        }
        if (camera_button_col_cache[cam_idg] != i || camera_button_row[cam_idg] != j) {
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            camera_button_col_cache[cam_idg] = i;
            camera_button_row[cam_idg] = j;
            /* PLATFORM(video): draw a UI sprite. */ sprite_1_unk(0x9A, 0xB1, 0x74, 6, camera_select_fill_color);
            /* PLATFORM(video): draw a UI sprite. */ sprite_1_unk(i + 0x9A, 0xB1, 6, 6, dlg_colour);
            /* PLATFORM(video): draw a UI sprite outline. */ sprite_1_unk4(j + 0x9A, 0xB1, j + 0x9F, 0xB6, camera_select_outline_color);
        }
        if (view_camera_choice[cam_idg] != camera_button_index)
            goto redraw_buttons;
        for (button = 0; button < 7; button++) {
            if (camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button])
                goto redraw_buttons;
        }
        goto buttons_done;
/* PLATFORM(video): select the opaque drawing path. */ redraw_buttons:
        msdrawopaquechk();
        if (view_camera_choice[cam_idg] != -1) {
            /* PLATFORM(video): draw a shape layer. */ if (camera_buttons_state_per_view[view_camera_choice[cam_idg] * 2 + cam_idg] != 0)
                shape2d_op_unk(replayshapes[view_camera_choice[cam_idg] + 14]);
            /* PLATFORM(video): draw a shape layer. */ else
                shape2d_op_unk(replayshapes[view_camera_choice[cam_idg] + 5]);
            view_camera_choice[cam_idg] = -1;
        }
        for (button = 0; button < 7; button++) {
            if (camera_buttons_pressed[button] == 0 && camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button]) {
                /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk(replayshapes[button + 5]);
                camera_buttons_state_per_view[button * 2 + cam_idg] = 0;
            }
        }
        for (button = 0; button < 7; button++) {
            if (camera_buttons_pressed[button] != 0) {
                camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
                /* PLATFORM(video): draw a shape layer. */ shape2d_op_unk(replayshapes[button + 14]);
                camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
            }
        }
        view_camera_choice[cam_idg] = camera_button_index;
        /* PLATFORM(video): draw a UI sprite outline. */ if (camera_button_index != -1)
            sprite_1_unk4(game_camera_buttons_x1[camera_button_index], game_camera_buttons_y1[camera_button_index],
                          game_camera_buttons_x2[camera_button_index], game_camera_buttons_y2[camera_button_index], camera_select_outline_color);
/* PLATFORM(video): select the transparent drawing path. */ buttons_done:
        msdrawtransparentchk();
        break;
    case 3:
        if (game_camera_buttons_count[cammd] < camera_button_index && cammd != 2)
            camera_button_index = game_camera_buttons_count[cammd];
        /* PLATFORM(video): copy the working sprite page. */ sprite_copy_2_to_1();
        if (g_videoflg5 != 0)
            cam_idg = numid ^ 1;
/* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ /* PLATFORM(timer): read the elapsed timer interval. */ next_input:
        key_code = input_checking(timer_get_delta_alt());
        /* PLATFORM(input_mouse): test the current pointer against UI buttons. */ button = mouse_multi_hittest(game_camera_buttons_count[cammd] + 1, game_camera_buttons_x1,
                                     game_camera_buttons_x2, game_camera_buttons_y1, game_camera_buttons_y2);
        if (button != -1) {
            if (button != camera_button_index && key_code == 0)
                key_code = 1;
            camera_button_index = button;
            if ((key_code == ' ' || key_code == '\r') && camera_button_index >= 7) {
                if (camera_button_index == 7) {
                    if ((game_camera_buttons_y1[7] + game_camera_buttons_y2[7]) >> 1 < pos_y_ms)
                        key_code = KEY_SCAN_DOWN;
                    else
                        key_code = KEY_SCAN_UP;
                } else {
                    switch (((polang(msecoordx - ((game_camera_buttons_x1[8] + game_camera_buttons_x2[8]) >> 1),
                                         ((game_camera_buttons_y1[8] + game_camera_buttons_y2[8]) >> 1) - pos_y_ms)
                              + 0x80) & 0x3FF) >> 8) {
                    case 0:
                        key_code = KEY_SCAN_UP;
                        break;
                    case 1:
                        key_code = KEY_SCAN_RIGHT;
                        break;
                    case 2:
                        key_code = KEY_SCAN_DOWN;
                        break;
                    case 3:
                        key_code = KEY_SCAN_LEFT;
                        break;
                    }
                }
            }
        } else {
            /* PLATFORM(input_mouse): test the current pointer against UI buttons. */ button = mouse_multi_hittest(1, gameunk_button_x1, gameunk_button_x2, gameunk_button_y1, gameunk_button_y2);
            if (button == 0 && (key_code == ' ' || key_code == '\r'))
                key_code = 'c';
        }
        /* PLATFORM(input_kb): dispatch a normalized game key. */ if (key_code != 0 && key_code != 0x1B && handle_ingame_kb_shortcuts(key_code) != 0)
            break;
        if (inrepflg == 0 && key_code == 0) {
            if (g_rplybarenable == 0)
                break;
            loop_game(1, core.game_frame, core.game_frame);
            return;
        }
        if (g_rplybarenable == 0) {
            is_in_rplcopy = -1;
            bitmapdash = -1;
        }
        if (inrepflg != 0 && (camera_buttons_pressed[3] != 0 || camera_buttons_pressed[2] != 0))
            loop_game(2, 4, 0);
        loop_game(1, core.game_frame, core.game_frame);
        modifier = 0;
        /* PLATFORM(input_kb): read a key state. */ if (kb_get_key_state(0x1D) != 0 || (camera_button_index == 8 && (kbjoyflags & 0x30) != 0))
            modifier = 1;
        if (modifier != 0) {
            switch (key_code) {
            case KEY_SCAN_RIGHT:
                custom_azim_angle += 0x10;
                return;
            case KEY_SCAN_LEFT:
                custom_azim_angle -= 0x10;
                return;
            case KEY_SCAN_UP:
                if (custom_elev_angle + 0x10 < 0x100) {
                    custom_elev_angle += 0x10;
                    return;
                }
                break;
            case KEY_SCAN_DOWN:
                if (custom_elev_angle - 0x10 > -0x100) {
                    custom_elev_angle -= 0x10;
                    return;
                }
                break;
            case '-':
zoom_out:
                if (cammd == 3) {
                    if (viewyshift <= 0)
                        break;
                    viewyshift -= 30;
                } else {
                    if (custom_dist >= 1500)
                        break;
                    custom_dist += 30;
                }
                goto done;
            case '+':
zoom_in:
                if (cammd == 3) {
                    if (viewyshift >= 900)
                        break;
                    viewyshift += 30;
                } else {
                    if (custom_dist <= 120)
                        break;
                    custom_dist -= 30;
                }
                goto done;
            }
            key_code = 0;
        }
        switch (key_code) {
        case '+':
            goto zoom_in;
        case '-':
            goto zoom_out;
        case KEY_SCAN_LEFT:
            if (game_camera_buttons_count[cammd] >= camera_button_count_mode[camera_button_index])
                camera_button_index = camera_button_count_mode[camera_button_index];
        default:
redraw_input:
            loop_game(1, core.game_frame, core.game_frame);
            goto next_input;
        case KEY_SCAN_RIGHT:
            camera_button_index = camera_mode_select_a[camera_button_index];
            goto redraw_input;
        case KEY_SCAN_UP:
            if (camera_button_index == 7)
                goto zoom_in;
            camera_button_index = camera_mode_select_b[camera_button_index];
            goto redraw_input;
        case KEY_SCAN_DOWN:
            if (camera_button_index == 7)
                goto zoom_out;
            camera_button_index = camera_mode_select_c[camera_button_index];
            goto redraw_input;
        case '\r':
        case ' ':
            switch (camera_button_index) {
            case 6:
pause_menu:
                inrepflg = 1;
                /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                loop_game(2, 4, 0);
                loop_game(1, core.game_frame, core.game_frame);
                for (i = 0; i < 8; i++)
                    dialog_params[i] = 0;
                if (core.playerstate.car_crashBmpFlag != 0)
                    dialog_params[3] = 1;
                if (globalgamesettings.game_recordedframes == 0 || elaptm1 != 0)
                    dialog_params[5] = 1;
                if (pass_check_flag == 0) {
                    dialog_params[2] = 1;
                    dialog_params[3] = 1;
                }
                if ((endhsdemo & 4) == 0)
                    dialog_params[1] = 1;
                g_simprect = g_vid_flag6;
                /* PLATFORM(input_joy): collect dialog joystick input. */
                /* PLATFORM(timer): wait for dialog input using timer ticks. */
                /* PLATFORM(video): render a modal dialog. */
                answer = show_dialog(2, 0, locate_text_resource(gamerptrs, "men"), -1, -1, dialogarg2, dialog_params, 0);
                switch (answer) {
                /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ case 2:
                    check_input();
                    rate_frame = frm_rate2;
                    globalgamesettings.game_framespersec = frm_rate2;
                    initialize_game_state(-1);
                    tmr2 = 0;
                    globalgamesettings.game_recordedframes = 0;
                    popupact = 0;
                    endhsdemo = 1;
                    goto continue_driving;
                case 3:
                    if (endhsdemo & 2)
                        endhsdemo = 3;
                    else if (globalgamesettings.game_recordedframes != tmr2) {
                        /* PLATFORM(input_joy): collect dialog joystick input. */
                        /* PLATFORM(timer): wait for dialog input using timer ticks. */
                        /* PLATFORM(video): render a modal dialog. */
                        i = show_dialog(2, 0, locate_text_resource(gamerptrs, "con"), -1, -1, performGraphColor, 0, 0);
                        if (i < 1)
                            break;
                        endhsdemo = 3;
                    } else
                        endhsdemo = 1;
                    globalgamesettings.game_recordedframes = tmr2 = core.game_frame;
    continue_driving:
                    on_off_dash = 1;
                    pen_flag_count = 0;
                    follow_op = 0;
                    gm_playmode = 0;
                    cammd = 0;
                    core.game_3F6autoLoadEvalFlag = 0;
                    core.game_frame_in_sec = 0;
                    g_rplmodui = 0;
                    loop_game(2, 3, 0);
                    inrepflg = 0;
                    /* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ mouse_minmax_position(mouse_enabled);
                    /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ check_input();
                    kbormouse = 0;
                    break;
                case 4:
                    endhsdemo = 0;
                    /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                    /* PLATFORM(file): open the file selection interface. */
                    /* PLATFORM(input_joy): collect file-selection joystick input. */
                    /* PLATFORM(timer): wait for file selection using timer ticks. */
                    /* PLATFORM(video): draw the file selection interface. */
                    i = do_fileselect_dialog(replay_file, aDefault_1, ".rpl", locate_text_resource(main_data_file_addr, "rep"));
                    if (i == 0)
                        break;
                    waitm_ms = 150;
                    /* PLATFORM(input_joy): collect waiting-dialog joystick input. */ /* PLATFORM(timer): wait for dialog input using timer ticks. */ /* PLATFORM(video): render a waiting dialog. */ show_waiting();
                    oldcfg = globalgamesettings;
                    prev_sky = td14tb[0x384];
                    /* PLATFORM(file): load replay data from a file. */ if (file_load_replay(replay_file, aDefault_1) != 0)
                        globalgamesettings.game_recordedframes = 0;
                    on_off_dash = 0;
                    track_setup();
                    i = 0;
                    if (td14tb[0x384] != prev_sky)
                        i = 1;
                    if (oldcfg.game_playercarid[0] != globalgamesettings.game_playercarid[0] ||
                        oldcfg.game_playercarid[1] != globalgamesettings.game_playercarid[1] ||
                        oldcfg.game_playercarid[2] != globalgamesettings.game_playercarid[2] ||
                        oldcfg.game_playercarid[3] != globalgamesettings.game_playercarid[3])
                        i = 1;
                    else if (oldcfg.game_opponenttype != globalgamesettings.game_opponenttype)
                        i = 1;
                    else if (globalgamesettings.game_opponenttype != 0) {
                        if (oldcfg.game_opponentcarid[0] != globalgamesettings.game_opponentcarid[0] ||
                            oldcfg.game_opponentcarid[1] != globalgamesettings.game_opponentcarid[1] ||
                            oldcfg.game_opponentcarid[2] != globalgamesettings.game_opponentcarid[2] ||
                            oldcfg.game_opponentcarid[3] != globalgamesettings.game_opponentcarid[3])
                            i = 1;
                        else {
                            /* PLATFORM(file): check required files and request missing files. */
                            /* PLATFORM(input_joy): collect file-confirmation joystick input. */
                            /* PLATFORM(input_kb): collect file-confirmation key input. */
                            /* PLATFORM(input_mouse): collect file-confirmation pointer input. */
                            /* PLATFORM(timer): wait for file confirmation using timer ticks. */
                            /* PLATFORM(video): show missing-file guidance. */
                            ensure_file_exists(2);
                            load_opponent_data();
                        }
                    }
                    if (i != 0) {
                        free_player_cars();
                        setup_player_cars();
                    }
                    rate_frame = globalgamesettings.game_framespersec;
                    initialize_game_state(-1);
                    break;
                /* PLATFORM(audio): update engine audio from car state. */ case 5:
                    audio_carstate();
                    write_state = 0;
                    while (write_state == 0) {
                        /* PLATFORM(file): open the save-file interface. */
                        /* PLATFORM(input_joy): collect file-entry joystick input. */
                        /* PLATFORM(timer): wait for file entry using timer ticks. */
                        /* PLATFORM(video): draw the save-file interface. */
                        if (do_savefile_dialog(replay_file, aDefault_1, locate_text_resource(main_data_file_addr, "rep")) != 0) {
                            /* PLATFORM(file): build a file path. */ file_build_path(replay_file, aDefault_1, ".rpl", buf_g_path);
                            write_state = 1;
                            g_is_busy = 1;
                            /* PLATFORM(file): check whether a file exists. */ if (file_find(buf_g_path) != 0) {
                                /* PLATFORM(input_joy): collect dialog joystick input. */
                                /* PLATFORM(timer): wait for dialog input using timer ticks. */
                                /* PLATFORM(video): render a modal dialog. */
                                i = show_dialog(2, 0, locate_text_resource(main_data_file_addr, "fex"), -1, -1, performGraphColor, 0, 0);
                                if (i == -1)
                                    write_state = -1;
                                else if (i == 0)
                                    write_state = 0;
                            }
                            g_is_busy = 0;
                        } else
                            write_state = -1;
                        if (write_state == 1) {
                            /* PLATFORM(file): write replay data to a file. */ button = file_write_replay(buf_g_path);
                            if (button != 0) {
                                /* PLATFORM(input_joy): collect dialog joystick input. */
                                /* PLATFORM(timer): wait for dialog input using timer ticks. */
                                /* PLATFORM(video): render a modal dialog. */
                                show_dialog(1, 0, locate_text_resource(main_data_file_addr, "ser"), -1, -1, performGraphColor, 0, 0);
                                write_state = 0;
                            }
                        }
                    }
                    break;
                case 1:
                    update_crash_state(4, 0);
                    sigframe = 2;
                    break;
                case 7:
                    update_crash_state(4, 0);
                    endhsdemo = 0;
                    sigframe = 2;
                    break;
                case 6:
                    for (i = 0; i < 5; i++)
                        dialog_params[i] = 0;
                    if (globalgamesettings.game_opponenttype == 0)
                        dialog_params[4] = 1;
                    /* PLATFORM(input_joy): collect dialog joystick input. */
                    /* PLATFORM(timer): wait for dialog input using timer ticks. */
                    /* PLATFORM(video): render a modal dialog. */
                    answer = show_dialog(2, 0, locate_text_resource(gamerptrs, "mdo"), -1, -1, dialogarg2, dialog_params, 0);
                    switch (answer) {
                    case 0:
                        on_off_dash ^= 1;
                        break;
                    case 1:
                        replaybar_toggle ^= 1;
                        break;
                    case 2:
                        cammd++;
                        if (cammd == 4)
                            cammd = 0;
                        break;
                    /* PLATFORM(audio): pause and restore audio around the graphics menu. */
                    /* PLATFORM(input_joy): collect graphics-menu joystick input. */
                    /* PLATFORM(input_kb): collect graphics-menu key input. */
                    /* PLATFORM(input_mouse): collect graphics-menu pointer input. */
                    /* PLATFORM(timer): wait for graphics selection using timer ticks. */
                    /* PLATFORM(video): draw the graphics-level selection interface. */
                    case 3:
                        show_graphic_levels_menu();
                        break;
                    case 4:
                        follow_op ^= 1;
                        break;
                    }
                    break;
                }
                /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ check_input();
                goto done;
            case 0:
                inrepflg = 1;
                /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                loop_game(2, 0, 0);
                /* PLATFORM(timer): read the elapsed timer interval. */ timer_get_delta_alt();
                travel = 20;
                while (kbjoyflags & 0x30) {
                    j = travel / 50 + 3;
                    if (j > 100)
                        j = 100;
                    /* PLATFORM(timer): read the elapsed timer interval. */ i = (time_delta = timer_get_delta_alt()) * j;
                    travel += i;
                    if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                        travel = (I32)(globalgamesettings.game_recordedframes - tmr2) * 20;
                    loop_game(1, core.game_frame, travel / 20 + tmr2);
                    /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ input_do_checking(time_delta);
                }
                if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                    travel = (I32)(globalgamesettings.game_recordedframes - tmr2) * 20;
                i = travel / 20 + tmr2;
                if (i > globalgamesettings.game_recordedframes)
                    i = globalgamesettings.game_recordedframes;
                restore_gamestate(i);
                tmr2 = i;
                loop_game(2, 4, 0);
                copy_string(&resbuftext, locate_text_resource(gamerptrs, "wai"));
                /* PLATFORM(video): use the display text or sprite interface. */ if (statemgmtcpy != 0)
                    rcunion(rcpunk2, introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0),
                               rcpunk2);
                /* PLATFORM(video): use the display text or sprite interface. */ else
                    introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0);
                while (core.game_frame != tmr2) {
                    update_gamestate();
                    loop_game(1, core.game_frame, tmr2);
                }
                /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ input_do_checking(1000);
                goto done;
            case 1:
                inrepflg = 1;
                /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                loop_game(2, 1, 0);
                /* PLATFORM(timer): read the elapsed timer interval. */ timer_get_delta_alt();
                travel = 20;
                while (kbjoyflags & 0x30) {
                    j = travel / 50 + 3;
                    if (j > 100)
                        j = 100;
                    /* PLATFORM(timer): read the elapsed timer interval. */ time_delta = timer_get_delta_alt();
                    i = time_delta * j;
                    travel += i;
                    if ((unsigned)(travel / 20) > tmr2)
                        travel = (I32)tmr2 * 20;
                    loop_game(1, core.game_frame, tmr2 - travel / 20);
                    /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ input_do_checking(time_delta);
                }
                if ((unsigned)(travel / 20) > tmr2)
                    travel = (I32)tmr2 * 20;
                j = travel / 20;
                loop_game(2, 4, 0);
                if (j != 0) {
                    copy_string(&resbuftext, locate_text_resource(gamerptrs, "wai"));
                    /* PLATFORM(video): use the display text or sprite interface. */ if (statemgmtcpy != 0)
                        rcunion(rcpunk2, introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0),
                                   rcpunk2);
                    /* PLATFORM(video): use the display text or sprite interface. */ else
                        introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0);
                    i = tmr2 - j;
                    restore_gamestate(i);
                    tmr2 = i;
                    prev_sky = i - core.game_frame;
                    if (prev_sky != 0) {
                        i = prev_sky;
                        while (core.game_frame != tmr2) {
                            update_gamestate();
                            i--;
                            loop_game(1, (I32)i * j / prev_sky + tmr2, tmr2);
                            /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ input_do_checking(1);
                        }
                    }
                }
                loop_game(1, core.game_frame, core.game_frame);
                /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ input_do_checking(1000);
                goto done;
            case 3:
                g_rplmodui = 0;
                loop_game(2, 3, 0);
                inrepflg = 0;
                break;
            case 4:
                inrepflg = 1;
                /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                loop_game(2, 4, 0);
                loop_game(1, core.game_frame, core.game_frame);
                goto redraw_input;
            case 5:
                inrepflg = 1;
                /* PLATFORM(audio): update engine audio from car state. */ audio_carstate();
                loop_game(2, 5, 0);
                loop_game(1, core.game_frame, core.game_frame);
                restore_gamestate(0);
                /* PLATFORM(timer): read the timer counter. */ timer_get_counter_unk(50);
                loop_game(2, 4, 0);
                loop_game(1, core.game_frame, core.game_frame);
                return;
            case 2:
                loop_game(2, 2, 0);
                g_rplmodui = 3;
                inrepflg = 0;
                break;
            }
            goto redraw_input;
        case 0x1B:
            goto pause_menu;
        }
        break;
    }
done:
    ;
}

