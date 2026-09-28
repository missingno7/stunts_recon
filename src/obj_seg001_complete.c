extern short rate_frame;


struct RECTANGLE {
	int left, right;
	int top, bottom;
	
	
};

struct VECTOR {
	short x, y, z;
};

struct VECTORLONG {
	long lx, ly, lz;
};

struct POINT2D {
	int px, py;
};

struct MATRIX { int vals[9]; };

struct PLANE {
	int plane_yz;
	int plane_xy;
	struct VECTOR plane_origin;
	struct VECTOR plane_normal;
	struct MATRIX plane_rotation;
};


short sinfast(unsigned short s);
short cosfast(unsigned short s);

int polang(int z, int y);
int polradius2d(int z, int y);
int polarRadius3D(struct VECTOR* vec);

unsigned rect_compare_point(struct POINT2D* pt);

void mat_vec(struct VECTOR* invec, struct MATRIX* mat, struct VECTOR* outvec);
void mat_multiply(struct MATRIX* rmat, struct MATRIX* lmat, struct MATRIX* outmat);
void mat_invert(struct MATRIX* inmat, struct MATRIX* outmat);
void mat_rot_x(struct MATRIX* outmat, int angle);
void matroty(struct MATRIX* outmat, int angle);
void mat_rot_z(struct MATRIX* outmat, int angle);
struct MATRIX* matrotzxy(int z, int x, int y, int unk);

void rect_adjust_from_point(struct POINT2D* pt, struct RECTANGLE* rc);

int vector_op_unk2(struct VECTOR* vec);
void vector_to_point(struct VECTOR* vec, struct POINT2D* outpt);
void vector_op_unk(struct VECTOR* vec1, struct VECTOR* vec2, struct VECTOR* outvec, short i);

short mulscl(short a1, short a2);

void rcunion(struct RECTANGLE* r1, struct RECTANGLE* r2, struct RECTANGLE* outrc);
int rcintersect(struct RECTANGLE* r1, struct RECTANGLE* r2);





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

struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; 
                              
	short car_pseudoGravity;
	short car_steeringAngle;
	short car_currpm;
	short car_lastrpm;
	short car_idlerpm2;
	short car_speeddiff; 
	unsigned short car_speed;     
                         
	unsigned short car_speed2;    
                         
                         
                         
                         
	unsigned short car_lastspeed; 
	unsigned short car_gearratio;
	unsigned short car_gearratioshr8;
	short car_knob_x;
	short car_36MwhlAngle;
	short car_knob_y;
	short car_knob_x2;
	short car_knob_y2;
	short car_angle_z;
	short car_40MfrontWhlAngle;
	short field_42;
	short car_demandedGrip;
	short car_surfacegrip_sum;
	short field_48;
	short car_trackdata3_index;
	short car_rc1[4]; 
	short car_rc2[4];
	short car_rc3[4];
	short car_rc4[4];
	short car_rc5[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_vec_unk3;
	struct VECTOR car_vec_unk4;
	struct VECTOR car_vec_unk5;
	short field_B6;
	short field_B8;
	short field_BA;
	char car_is_braking;
	char car_is_accelerating;
	char car_current_gear;
	char car_sumSurfFrontWheels;
	char car_sumSurfRearWheels;
	char car_sumSurfAllWheels; 
	char car_surfaceWhl[4];      
	char car_engineLimiterTimer;
	char car_slidingFlag;
	char field_C8;
	char car_crashBmpFlag;
	char car_changing_gear;
	char car_fpsmul2;
	char car_transmission;
	char field_CD;
	unsigned char field_CE; 
	unsigned char field_CF; 
};

struct GAMESTATE {
	long game_longs1[24]; 
	long game_longs2[24]; 
	long game_longs3[24]; 
	struct VECTOR game_vec1[2]; 
	struct VECTOR game_vec3[2];
	short game_frame_in_sec;
	short game_frames_per_sec;
	long  game_travDist;
	unsigned short game_frame;
	short game_total_finish; 
	short field_144;
	short game_pEndFrame;
	short game_oEndFrame;   
	unsigned short game_penalty;
	unsigned short game_impactSpeed;
	unsigned short game_topSpeed;
	short game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	short field_2F2;
	short field_2F4;
	short game_startcol;
	short game_startcol2;
	short game_startrow;
	short game_startrow2;
	short field_2FE[24];
	short field_32E[24];
	short field_35E[24];
	short field_38E[24];
	short field_3BE[24];
	char kevinseed[6];
	char field_3F4;
	char game_inputmode; 
	char game_3F6autoLoadEvalFlag;
	char field_3F7[2]; 
	char field_3F9;
	char field_3FA[48];
	char field_42A;
	char field_42B[24];
	char field_443[24];
	char field_45B;
	char field_45C;
	char field_45D;
	char field_45E;
	char field_45F;
};

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

struct SIMD {
	char num_gears;
	char simd_unk;
	unsigned short car_mass;
	short braking_eff;
	short idle_rpm;
	unsigned short downshift_rpm;
	unsigned short upshift_rpm;
	unsigned short max_rpm;
	unsigned short gear_ratios[7];
	struct POINT2D knob_points[7];
	short aero_resistance;
	unsigned char idle_torque;
	unsigned char torque_curve[104];
	char field_A3;
	short grip;
	short field_A6[7];
	short sliding[5];
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
	short far* aerorestable;
};

struct TRKOBJINFO_LINK_BYTES { char first, second; };
struct TRKOBJINFO {
    char si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    short si_arrowOrient;
    short *si_cameraDataOffset;
    union { short *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    char si_opp3, si_oppSpedCode;
};

struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    short ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    unsigned char ss_ssOvelay;
    char ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};


extern struct GAMEINFO globalgamesettings;
extern struct GAMEINFO gmconfigbackup;

extern struct GAMESTATE core;
extern struct SIMD simdp7;
extern struct SIMD ophys_7;

extern short pixel_scales;
extern short g_vid_flg2_set;
extern short vidflg3is_minus1;
extern short vidflg4_is1;
extern short g_videoflg5;
extern short g_vid_flag6;

extern unsigned char timeraud;
extern unsigned char slomodiv8;
extern unsigned short elaptm1;
extern unsigned short tmr2;
extern unsigned char sigframe;
extern unsigned char g_rpl_init;
extern unsigned char gm_playmode; 
extern short g_sgateopn;

extern short elapsed_time1; 
extern short g_cvxintvl; 
extern short frmcs_time; 
extern short st_hdg;
extern char *table_lookup;
extern char steerWhlRespTable_10fps[];
extern char steerWhlRespTable_20fps[];
extern char idxtrk, tagtrk;
extern char g_hillf;
extern short hillconsts[];

extern struct RECTANGLE boundglassrect;
extern short bitmapdash;
extern int runrndx;
extern char replaybar_toggle;
extern char inrepflg;
extern char cammd;
extern char g_rplmodui;
extern char gm_saved_rpl_mode;
extern char numid;
extern char g_rplbfask;
extern char on_off_dash;
extern char cam_idg;
extern char pen_flag_count;
extern int replayrst;
extern int popupact;
extern char byte_3B8F2;
extern char byte_3FE00;
extern void far* gamerptrs;
extern void far* dasm_shp_7;
extern int word_3F88E;
extern char dashbtogglesaved;
extern char g_replaybarcpytgl;
extern char is_in_rplcopy;
extern char follow_op;
extern char opp_follow_flag_backup;
extern int roofbmphgt_saved;
extern char mode_flag;
extern char g_rplybarenable;
extern int dashbmpy_copy;
extern int rplbarabovehgt;
extern char g_simprect;
extern char g_viewinx[];
extern int dastseg;
extern int dasty;
extern int g_dastbmpbuf;
extern int dashbmy9;
extern int rfy5;
extern struct RECTANGLE* rectp;
extern void setup_car_shapes(int);
extern void update_frame(char, struct RECTANGLE*);
extern void loop_game(int, int, int);
extern void set_frame_callback(void);
extern void mouse_minmax_position(int);
extern int kb_get_char(void);
extern void handle_ingame_kb_shortcuts(int);

extern int flagsdown;
extern int msecoordx;
extern int pos_y_ms;
extern int performGraphColor;
extern char resbuftext;
extern int waitm_ms;

extern void far* fntndat;
extern void far* def_fntadr;
extern void far* main_data_file_addr;
extern struct GAMESTATE huge* cvxs_a;
extern int lnoffsets[];
extern int gterrtrk[];
extern int r_zp[];
extern int row_ctr_zs[];
extern int postable[];
extern int z_ctr_pos[];
extern int xcols[];
extern int trackctrpos2[];
extern short far* g_td01_track_filecpy; 
extern short far* trackdata_penalty_related; 
extern char far* td3;
extern short far* track04_plyraero; 
extern short far* trackdata_05_opp_aerotbl; 
extern char far* td6_ptr_b;
extern char far* trackdat7;
extern int far* g_td08d; 
extern int far* trkptrpath;
extern struct VECTOR far* td10checkptr;
extern char far* hscore_trk11_ptr; 
extern char far* savedptr_ms;
extern char far* td13_replay_hdr; 
extern unsigned char far* td14tb; 
extern unsigned char far* td15p_9; 
extern char far* g_tdreplay16buf; 
extern char far* road_trk; 
extern char far* td_18_ref;
extern unsigned char far* td19hdl;
extern char far* coursedataappend_address; 
extern char far* g_column_of_trkdata21_pth; 
extern char far* tdfrompathrow22; 
extern unsigned char far* trkd23adr; 
extern char kbormouse;
extern char pass_check_flag;
extern char g_is_busy;
extern char buf_g_path[];
extern char track_file[];
extern char menutimeout;
extern unsigned short dialogarg2;
extern char replay_file[];
extern char endhsdemo;
extern char aMain[];
extern char aMisc_1[];
extern char aFontdef_fnt[];
extern char aFontn_fnt[];
extern char aTrakdata[];
extern char aDefault_0[];
extern char aCvx[];
extern char aTedit__0[];
extern char aSlct[];
extern char aSkidms_0[];
extern char aSkidslct[];
extern char aDos[];
extern unsigned short frm_rate2;
extern unsigned short slow_video_mode_state;
extern unsigned short statemgmtcpy;
extern unsigned char detail_lvl;

extern unsigned short pspofs;
extern unsigned short pspseg;
extern unsigned resmem_end_seg;
extern unsigned resmem_base_seg;

extern struct MEMCHUNK* resptr1;
extern struct MEMCHUNK* resptr2;
extern struct MEMCHUNK* resendptr1;
extern struct MEMCHUNK* resendptr2;
extern unsigned short resmaxsize;

extern unsigned long timer_callback_counter;
extern unsigned long last_timer_callback_counter;
extern unsigned long timer_copy_unk;

extern unsigned char randomseeds[];
extern const char aReservememoryO[];
extern const char aReservememoryOutOfMemory[];
extern const char aMemoryManagerB[];
extern const char aResizememoryNo[];
extern const char aResizememoryCa[];
extern const char aSFileError[];
extern const char aSFileError_0[];
extern const char aSFileError_1[];
extern const char aSInvalidPackTy[];
extern const char aLocateshape4_4sShapeNotF[];
extern const char aLocatesound4_4sSoundNotF[];
extern char audiodriverstring[];

extern struct GAMESTATE_SNAPSHOT race_stats;
extern short is_audioloaded;
extern void far* musicfile;
extern void far* openvfile;
extern char textrespfxchr; 
extern char* shapeexts[];
extern unsigned char palmap[];

extern int* material_clrlist_ptr;
extern int* mat_copy_clr_lst_ptr;
extern int* material_clrlist2_ptr;
extern int* g_mat_clrlist_copy_2_ptr;
extern int* material_patlist_ptr;
extern int* material_patlistptr_copy;
extern int* material_patlist2_ptr;
extern int* matpatlistcopypointer2;
extern unsigned short video_cnstval;
extern void fontsetfontdef(void);
extern void initialize_polyinfo(void);
extern unsigned short run_intro_looped(void);
extern unsigned short show_dialog(int unk1, int unk2, void far* textresptr, unsigned short unk3, unsigned short unk4, int arg, void* unk5, int unk6);
extern char run_menu(void);
extern char setup_track(void);
extern void run_tracks_menu(int unk);
extern void run_opponent_menu(void);
extern void show_waiting(void);
extern void run_car_menu(struct GAMEINFO* unk, char* unk2, char* unk3, unsigned int unk4);
extern void run_game(void);
extern unsigned end_hiscore(void);
extern unsigned run_option_menu(void);
extern void security_check(void);

extern void ensure_file_exists(int unk);

extern void far* load_song_file(const char* filename);
extern void far* load_voice_file(const char* filename);
extern void far* load_sfx_file(const char* filename);
extern void far* load_shape2d_nofatal_thunk(const char* filename);
extern void far* load_shape2d_res_nofatal_thunk(const char* filename);
extern void far* file_load_shape2d_nofatal(char* shapename);
extern void far* file_load_shape2d_nofatal2(char* shapename);
extern void far* init_audio_resources(void far* songptr, void far* voiceptr, const char* name);
extern void load_audio_finalize(void far* audiores);
extern short audio_load_driver(char* driver, short a2, short a3);
extern void audio_unload(void);
extern short audio_toggle_flag2(void);
extern short audio_toggle_flag6(void);
extern void audio_stop_unknown(void);
extern void audiodrv_atexit(void);

extern void check_input(void);
extern int input_do_checking(int unk);
extern void keyboard_exit_handler(void);
extern void keyboard_shift_checking1(void);
extern void kb_shift_checking2(void);
extern void kb_reg_callback(int code, void (far* callback)(void));
extern void show_graphic_levels_menu(void);
extern void do_joystick_resource_text(void);
extern void do_key_resource_text(void);
extern void do_mof_resource_text(void);
extern void do_pau_restext(void);
extern void do_dos_resource_text(void);
extern void do_sonsof_resource_text(void);
extern short get_kb_or_joy_flags(void);

extern short mouse_init(short a1, short a2);
extern void msdrawopaquechk(void);

extern void video_set_mode4(void);
extern void video_set_mode7(void);
extern void video_set_mode_13h(void);

extern void shape3d_load_car_shapes(char* carid, char* oppcarid);

extern void load_palandcursor(void);
extern void sprset1size(unsigned short left, unsigned short right, unsigned short top, unsigned short height);
extern void sprite_clear_1_color(unsigned char);
extern void sprite_blit_to_video(struct SPRITE far* sprite);

extern short intr0_handler(void);
extern short (far* old_intr0_handler)(void);
extern void timer_setup_interrupt(void);
extern unsigned long timer_get_delta_alt(void);

extern short set_criterr_handler(short (far* callback)(void));
extern void exit(short a1);
extern void fatal_error(const char*, ...);
extern short do_dea_textres(void);

extern void* _memcpy(void*, const void*, unsigned);
extern char* _strcpy(char* dest, const char* src);
extern char* _strcat(char* dest, const char* src);
extern int _strcmp(const char* dest, const char* src);
extern int _stricmp(const char* dest, const char* src);
extern unsigned _strlen(const char* str);
extern void far* __fmemcpy(void far*, const void far*, unsigned);
extern unsigned _abs(unsigned);
extern int _rand(void);
extern void _srand(unsigned int);







extern int g_penaltytm;

struct AUDIO_CAR_FRAME {
    char reserved[6];
    short player_offsets[6];
    short opponent_offsets[6];
    short player_rpm;
    short opponent_rpm;
};
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    short has_opponent_link;
};
extern long centerpos;
extern long veh_position;
extern long veh_z;
extern int anglerotate_car;
extern int pln_rotate_z;
extern int rotxvehicle;
extern int car_rotate_xc;
extern int yrotrotveh;
extern int car_roty_pln;
extern struct MATRIX matrix_transform_view;
extern struct VECTOR tvec2;
extern int pl_i;
extern int g_planidx2;
extern int frwhl_angadjusted;
extern struct VECTOR pln_rot_output;
extern char g_cursurfacekindvalue;
extern int nextpos_normalip;
extern int road_num;
extern int element_min_wall;
extern int wall_wallelement;
extern int wallanchor_x;
extern int wallanchor_z;
extern int wall_facingang;
extern struct PLANE far* g_planlist;
extern struct PLANE far* plncurrptr;
extern int x_course_part;
extern int road_elem_ctrz;
extern int hgthgt;
extern char test_pln;
extern struct POINT2D collision_point_set_b[2];
extern struct POINT2D collision_point_set_a[2];
extern struct POINT2D collision_point_set_c[2];
extern int collision_rotation_offsets[4];
extern int op_eng_sound_id;
extern int g_player_sound_id;
extern int bto_auxiliary1(int, int, struct VECTOR*);
extern short g_trackpiecescounter;
extern struct PLANE far plan_memres[];
extern void initialize_unknown(void);
extern unsigned const char* g_ascii_props;
extern struct SHAPE3D g_shapes3d[];
extern unsigned select_rot(int angX, int angY, int angZ, struct RECTANGLE* cliprect, int unk);
extern void trans_op(struct TRANSFORMSHAPE3D* shape);
extern void reset_idle_counters(void);
extern void set_projection(int, int, int, int);
extern struct SPRITE far* g_wndspr;
extern struct RECTANGLE clipunk;
extern int polygonnumber;
extern unsigned char far* polyinfoptrs[];
extern unsigned int poly_linked_list_40ED6[];
extern void preRender_default(int color, int vertlinecount, int* vertlines);
extern unsigned char opponent_spd_tbl[];
extern struct TRACKOBJECT trklst[];
extern unsigned int update_rpm_from_speed(unsigned int, unsigned int, unsigned int, int, unsigned int);
extern int abs(int);
extern unsigned short speed_recovery_divisors[5];
extern char audio_frmarr[];
extern char sndpendingstate;
extern char g_plyr_snd_state;
extern char audiooppflag;
extern char replay_state_cache;
extern short sndposrecord;
extern short g_audio_frms_ix;
extern short viewyshift;
extern void audio_op_unk(short);
extern void audio_op_unk5(short);
extern void audio_op_unk6(short);
extern void audio_op_unk7(short);
extern void audio_function2(short);
extern void reset_audio_driver_state(void);
extern int collision_point_x_signs[];
extern int collision_point_y_signs[];
extern struct RECTANGLE select_rect_rc;
extern struct MATRIX g_rot_mat_z;
extern struct MATRIX matrix_x_rotation;
extern struct MATRIX g_matrix_yrot;
extern struct MATRIX matrotation_tmp;
extern unsigned mat_y_rot_angle;
extern long sin80, cos80;
extern unsigned char atantable[];
extern int projectiondata5, projectiondata8, projectiondata9, projectiondata10;
static struct MATRIX fallback_rotation_cache;
extern int last_track_rotation;
static struct MATRIX plane_rotation_cache;
extern int f36f40_whlData;
void opponent_op(void);
void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec);
void update_player_state(struct CARSTATE* arg_pState, struct SIMD* arg_pSimd, struct CARSTATE* arg_oState, struct SIMD* arg_oSimd, char arg_MplayerFlag);
void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short trkang);
void initialize_game_state(short arg);
void restore_gamestate(int frame);
void update_gamestate();
void player_op(char arg_carInputByte);
char detect_penalty(int *trackIndex, int *penaltyCounter);
void update_car_speed(char arg_carInputByte, char arg_MplayerFlag, struct CARSTATE* arg_carState, struct SIMD* arg_simd);
void update_grip(struct CARSTATE *car, struct SIMD *simd, int isOpponent);
char car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent);
int carState_rc_op(struct CARSTATE *car, int value, int wheel);
void upd_statef20_from_steer_input(char input);
void audio_carstate(void);
void audio_unk3(char flags, short audioId);
void apply_audio_frame(struct AUDIO_CAR_FRAME *record, short value);
char track_edge_points(int trackIndex, struct TRACKRESULT *result, char side,
              char *opponentSpeed);
char car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds);
void init_plantrak(void);
void do_opponent_op(void);

void update_crash_state(int arg_someFlag, int arg_MplayerFlag);
void plnrotop(void);
int plnoriginop(int arg_planindex, int x, int y, int z);
int vec_normalInnerProduct(int x, int y, int z, struct VECTOR far *normal);
void state_op_unk(int mode, short angle, short speed);
void update_crash_debris(void);

int collision_response_offsets[4] = { 10, 50, 10, 20 };
struct POINT2D collision_point_set_a[2] = { { 5, 40 }, { 5, 10 } };
struct POINT2D collision_point_set_b[2] = { { 6, 121 }, { 6, 9 } };
struct POINT2D collision_point_set_c[2] = { { 1, 10 }, { 1, 10 } };
int collision_rotation_offsets[4] = { 21, 21, 15, 15 };
char steerWhlRespTable_20fps[64] = {
    0, 8, -8, 0, 0, 7, -7, 0, 0, 6, -6, 0, 0, 5, -5, 0,
    0, 4, -4, 0, 0, 4, -4, 0, 0, 3, -3, 0, 0, 3, -3, 0,
    0, 2, -2, 0, 0, 2, -2, 0, 0, 2, -2, 0, 0, 1, -1, 0,
    0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0
};
char steerWhlRespTable_10fps[62] = {
    0, 16, -16, 0, 0, 14, -14, 0, 0, 12, -12, 0, 0, 10, -10, 0,
    0, 8, -8, 0, 0, 8, -8, 0, 0, 6, -6, 0, 0, 6, -6, 0,
    0, 4, -4, 0, 0, 4, -4, 0, 0, 4, -4, 0, 0, 2, -2, 0,
    0, 2, -2, 0, 0, 1, -1, 0, 0, 1, -1, 0, 0, 1
};
unsigned short speed_recovery_divisors[5] = { 255, 256, 192, 128, 64 };
char replay_state_cache = -1;
int collision_point_x_signs[4] = { 1, 0, 0, 1 };
int collision_point_y_signs[4] = { 0, 0, 1, 1 };
int f36f40_whlData = 9999;
int last_track_rotation = 9999;

/* Purpose: Advances opponent driving state at the selected frame rate.
 * Parameters: none.
 * Returns: none.
 * Globals read: core, idxtrk, ophys_7, rate_frame, row_ctr_zs, simdp7, st_hdg, tagtrk, td3,
 *            trackctrpos2
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void opponent_op(void)
{
    struct VECTOR goal;
    struct VECTOR rel;
    int brake;
    char mode;
    register int temp;
    struct VECTOR pv;
    struct MATRIX *tmat;
    int max;
    int spdLimit;
    char skip;
    int x1;
    struct VECTOR diff;
    int x2;
    struct VECTOR delta;
    int steerAngle;
    int y1;
    int y2;
    struct VECTOR waypoint;
    int z1;
    int z2;

    if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 20) {
        max = 8;
        brake = 1;
    } else {
        max = 16;
        brake = 2;
    }
    if (core.opponentstate.car_36MwhlAngle != 0 || core.game_inputmode == 2)
        skip = 1;
    else
        skip = 0;
    x1 = core.opponentstate.car_posWorld1.lx >> 6;
    y1 = core.opponentstate.car_posWorld1.ly >> 6;
    z1 = core.opponentstate.car_posWorld1.lz >> 6;
    x2 = core.playerstate.car_posWorld1.lx >> 6;
    y2 = core.playerstate.car_posWorld1.ly >> 6;
    z2 = core.playerstate.car_posWorld1.lz >> 6;
    core.opponentstate.field_CF = 0;
    core.field_45E = 0;
    tmat = matrotzxy(core.opponentstate.car_rotate.z, core.opponentstate.car_rotate.y,
                           core.opponentstate.car_rotate.x, 1);
    core.opponentstate.field_CF = 1;
    if (core.opponentstate.car_crashBmpFlag != 0) {
        if (core.opponentstate.car_speed2 == 0)
            core.opponentstate.field_CF = 0;
    } else {
        waypoint = core.opponentstate.car_vec_unk3;
        if (waypoint.y != -1) {
            delta.x = waypoint.x - x1;
            delta.y = waypoint.y - y1;
            delta.z = waypoint.z - z1;
            temp = polarRadius3D(&delta);
        } else {
            temp = polradius2d(waypoint.x - x1, waypoint.z - z1);
        }
        if (temp < 200) {
next_waypoint:
            if (track_edge_points(((short far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
                          &core.field_3F9) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((short far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
                    core.opponentstate.field_CD++;
                    core.opponentstate.car_trackdata3_index = 0;
                }
                core.opponentstate.field_CE = 0;
            }
        }
        if (core.game_inputmode == 2) {
no_player:
            goal = core.opponentstate.car_vec_unk3;
        } else {
            diff.x = x2 - x1;
            diff.y = y2 - y1;
            diff.z = z2 - z1;
            mat_vec(&diff, tmat, &pv);
            if (pv.y > 90 || (pv.x < 0 ? -pv.x : pv.x) > 180 ||
                pv.z > 600 || pv.z < -180)
                goto no_player;
            diff.x = x2 - core.opponentstate.car_vec_unk3.x;
            if (core.opponentstate.car_vec_unk3.y == -1)
                diff.y = 0;
            else
                diff.y = y2 - core.opponentstate.car_vec_unk3.y;
            diff.z = z2 - core.opponentstate.car_vec_unk3.z;
            mat_vec(&diff, tmat, &rel);
            if (rel.x < 0) {
                goal.x = ((long)core.opponentstate.car_vec_unk3.x + core.opponentstate.car_vec_unk5.x) >> 1;
                if (core.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((long)core.opponentstate.car_vec_unk3.y + core.opponentstate.car_vec_unk5.y) >> 1;
                goal.z = ((long)core.opponentstate.car_vec_unk3.z + core.opponentstate.car_vec_unk5.z) >> 1;
                if (pv.z > -78 && core.playerstate.car_crashBmpFlag == 0)
                    core.field_45E = 2;
            } else {
                goal.x = ((long)core.opponentstate.car_vec_unk3.x + core.opponentstate.car_vec_unk4.x) >> 1;
                if (core.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((long)core.opponentstate.car_vec_unk3.y + core.opponentstate.car_vec_unk4.y) >> 1;
                goal.z = ((long)core.opponentstate.car_vec_unk3.z + core.opponentstate.car_vec_unk4.z) >> 1;
                if (pv.z > -78 && core.playerstate.car_crashBmpFlag == 0)
                    core.field_45E = 1;
            }
            goto steer;
        }
steer:
        diff = goal;
        diff.x -= x1;
        if (goal.y == -1)
            diff.y = 0;
        else
            diff.y -= y1;
        diff.z -= z1;
        mat_vec(&diff, tmat, &waypoint);
        steerAngle = polang(waypoint.x, waypoint.z);
        if (core.opponentstate.car_slidingFlag == 0 &&
            (steerAngle < 0 ? -steerAngle : steerAngle) > 0x100) {
            if (track_edge_points(((short far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
                          &core.field_3F9) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((short far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
                    core.opponentstate.field_CD++;
                    core.opponentstate.car_trackdata3_index = 0;
                }
                core.opponentstate.field_CE = 0;
            }
        }
        if (steerAngle > 65) {
            if (skip == 0) {
                skip = 1;
                goto next_waypoint;
            }
            steerAngle = 65;
        } else if (steerAngle < -65) {
            if (skip == 0) {
                skip = 1;
                goto next_waypoint;
            }
            steerAngle = -65;
        }
        if (core.opponentstate.car_sumSurfFrontWheels == 0)
            steerAngle = 0;
        temp = steerAngle - core.opponentstate.car_steeringAngle;
        if ((temp < 0 ? -temp : temp) > max) {
            if (steerAngle < core.opponentstate.car_steeringAngle)
                core.opponentstate.car_steeringAngle -= max;
            else
                core.opponentstate.car_steeringAngle += max;
        } else {
            core.opponentstate.car_steeringAngle = steerAngle;
        }
    }
    mode = 0;
    if (core.opponentstate.car_sumSurfRearWheels != 0) {
        if (core.opponentstate.car_crashBmpFlag != 0) {
            mode = 2;
        } else if (core.opponentstate.car_36MwhlAngle != 0) {
            if ((brake << 9) > core.opponentstate.car_speed2) {
                core.opponentstate.car_speed2 = 0;
                core.opponentstate.car_36MwhlAngle = 0;
            } else {
                core.opponentstate.car_speed2 -= brake << 9;
            }
        } else if (core.opponentstate.car_demandedGrip > core.opponentstate.car_surfacegrip_sum) {
            mode = 2;
        } else {
            if (core.game_inputmode == 2)
                spdLimit = 0x4000;
            else
                spdLimit = core.field_3F9 << 8;
            if (spdLimit - 0x100 > core.opponentstate.car_speed)
                mode = 1;
            else if (spdLimit + 0x300 < core.opponentstate.car_speed)
                mode = 2;
        }
    }
    update_car_speed(mode, 1, &core.opponentstate, &ophys_7);
    update_grip(&core.opponentstate, &ophys_7, 0);
    update_player_state(&core.opponentstate, &ophys_7, &core.playerstate, &simdp7, 1);
    if (core.opponentstate.car_crashBmpFlag == 0) {
        diff = core.opponentstate.car_vec_unk3;
        diff.x -= core.opponentstate.car_posWorld1.lx >> 6;
        diff.y -= core.opponentstate.car_posWorld1.ly >> 6;
        diff.z -= core.opponentstate.car_posWorld1.lz >> 6;
        tmat = matrotzxy(core.opponentstate.car_rotate.z, core.opponentstate.car_rotate.y,
                               core.opponentstate.car_rotate.x, 1);
        mat_vec(&diff, tmat, &waypoint);
        core.opponentstate.field_48 = polang(-waypoint.x, waypoint.z) & 0x3FF;
    }
    if (core.opponentstate.field_CD != 0) {
        temp = mulscl(cosfast(st_hdg),
            row_ctr_zs[tagtrk] - (int)(core.opponentstate.car_posWorld1.lz >> 6));
        temp += mulscl(sinfast(st_hdg),
            trackctrpos2[idxtrk] - (int)(core.opponentstate.car_posWorld1.lx >> 6));
        if (temp < 0)
            update_crash_state(3, 1);
    }
}

void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec) { struct MATRIX tmpmat; tmpmat=*mat; mat_vec(invec,&tmpmat,outvec); }

/* Purpose: Updates player and opponent vehicle simulation state.
 * Parameters: arg_pState, arg_pSimd, arg_oState, arg_oSimd, arg_MplayerFlag.
 * Returns: none.
 * Globals read: anglerotate_car, centerpos, collision_point_set_a, collision_point_set_b,
 *            collision_point_set_c, collision_rotation_offsets, core, element_min_wall,
 *            frwhl_angadjusted, g_cursurfacekindvalue, g_hillf, g_planlist,
 *            g_player_sound_id, globalgamesettings, gm_playmode, hgthgt, hillconsts,
 *            idxtrk, inrepflg, lnoffsets, matrix_transform_view, nextpos_normalip,
 *            op_eng_sound_id, pl_i, pln_rot_output, rate_frame, road_elem_ctrz, road_num,
 *            rotxvehicle, row_ctr_zs, st_hdg, tagtrk, td10checkptr, td19hdl, test_pln,
 *            trackctrpos2, veh_position, veh_z, wall_facingang, wall_wallelement,
 *            wallanchor_x, wallanchor_z, x_course_part, yrotrotveh
 * Globals written: anglerotate_car, car_rotate_xc, car_roty_pln, centerpos, core,
 *            frwhl_angadjusted, g_cursurfacekindvalue, g_planidx2,
 *            matrix_transform_view, nextpos_normalip, pl_i, pln_rotate_z, plncurrptr,
 *            road_num, rotxvehicle, test_pln, tvec2, veh_position, veh_z, yrotrotveh
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_player_state(struct CARSTATE* arg_pState, struct SIMD* arg_pSimd, struct CARSTATE* arg_oState, struct SIMD* arg_oSimd, char arg_MplayerFlag) {
	int planIdx;
	struct VECTOR p0;
	struct PLANE far * plane;
	char num;
	struct VECTOR p1;
	int whlHgts[4];
	struct VECTOR objPts[32];
	struct VECTOR hopVec;
	struct VECTORLONG * curWhl;
	int distBefore;
	int liftOfs;
	int angle;
	unsigned char tiltFlag;
	struct MATRIX * rotMat;
	char w;
	int spd;
	int remDist;
	register int i;
	struct VECTOR self[2];
	struct MATRIX rotMatrix;
	struct VECTOR res;
	char swap;
	struct MATRIX localPlane;
	struct VECTOR base;
	char groundContact;
	int frontSteer;
	int wheelAngle[4];
	int found;
	struct VECTORLONG whlPos[4];
	unsigned int threshold;
	struct VECTOR objPos[2];
	struct VECTOR oldVec;
	struct VECTOR tmpCrds;
	struct VECTORLONG oldWhls[4];
	struct VECTORLONG * prev;
	struct VECTOR * nextVec;
	struct VECTOR vec;
	struct VECTOR posDiffs[4];
	struct VECTOR wheelCrd;

	centerpos = arg_pState->car_posWorld1.lx;
	arg_pState->car_posWorld2.lx = centerpos;
	veh_position = arg_pState->car_posWorld1.ly;
	arg_pState->car_posWorld2.ly = veh_position;
	veh_z = arg_pState->car_posWorld1.lz;
	arg_pState->car_posWorld2.lz = veh_z;
	pln_rotate_z = anglerotate_car = arg_pState->car_rotate.z;
	car_roty_pln = yrotrotveh = arg_pState->car_rotate.y;
	car_rotate_xc = rotxvehicle = arg_pState->car_rotate.x;
	if (arg_pState->car_sumSurfAllWheels != 0)
		frontSteer = arg_pState->car_40MfrontWhlAngle >> 2;
	else
		frontSteer = 0;
	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
		spd = (long)arg_pState->car_speed2 * 0x580 / 0x1E00U;
	else
		spd = (long)arg_pState->car_speed2 * 0x580 / 0x3C00U;
	matrix_transform_view = *matrotzxy(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
	if (yrotrotveh != 0 || anglerotate_car != 0) {
		wheelCrd.x = 0;
		wheelCrd.y = 0;
		wheelCrd.z = 0x82;
		mat_vec(&wheelCrd, &matrix_transform_view, &res);
		arg_pState->car_pseudoGravity = -res.y;
	} else {
		arg_pState->car_pseudoGravity = 0;
	}
	if (arg_pState->car_angle_z & 0x3FF) {
		tiltFlag = 1;
		rotMatrix = *matrotzxy(0, 0, -arg_pState->car_angle_z, 0);
	} else {
		tiltFlag = 0;
	}
	wheelCrd.x = 0;
	wheelCrd.y = 30000;
	wheelCrd.z = 0;
	mat_vec(&wheelCrd, &matrix_transform_view, &res);
	if (arg_pState->car_sumSurfAllWheels != 0 && res.y < 0) {
		if (arg_pState->car_speed2 > 0x1E00) {
			liftOfs = 0xC0;
			wheelCrd.y = -0xC0;
			mat_vec(&wheelCrd, &matrix_transform_view, &hopVec);
		} else {
			liftOfs = -0xC0;
		}
	} else {
		liftOfs = 0;
	}
	tvec2.x = 0;
	tvec2.y = 0;
	g_planidx2 = -1;
	curWhl = whlPos;
	prev = oldWhls;
	for (w = 0; w < 4; ++curWhl, ++prev, ++w) {
		wheelCrd = arg_pSimd->wheel_coords[w];
		wheelCrd.y = -(arg_pState->car_rc2[w] + 0x180);
		if (liftOfs < 0)
			wheelCrd.y -= liftOfs;
		if (tiltFlag != 0) {
			mat_vec(&wheelCrd, &rotMatrix, &res);
			wheelCrd = res;
		}
		mat_vec(&wheelCrd, &matrix_transform_view, &res);
		curWhl->lx = res.x + centerpos;
		curWhl->ly = res.y + veh_position;
		curWhl->lz = res.z + veh_z;
		prev->lx = curWhl->lx;
		prev->ly = curWhl->ly;
		prev->lz = curWhl->lz;
		if (spd != 0) {
			tvec2.z = spd;
			if (frontSteer != 0 && w < 2)
				frwhl_angadjusted = arg_pState->car_36MwhlAngle - frontSteer;
			else
				frwhl_angadjusted = arg_pState->car_36MwhlAngle;
			wheelAngle[w] = frwhl_angadjusted;
			plnrotop();
			curWhl->lx += pln_rot_output.x;
			curWhl->ly += pln_rot_output.y;
			curWhl->lz += pln_rot_output.z;
		}
	}

	num = 0;
retry:
	if (++num == 5) {
		arg_pState->car_36MwhlAngle = 0x200;
		update_crash_state(1, arg_MplayerFlag);
		goto wheels_done;
	}
	curWhl = whlPos;
	prev = oldWhls;
	for (w = 0; w < 4; ++curWhl, ++prev, ++w) {
		wheelCrd.x = curWhl->lx >> 6;
		wheelCrd.y = curWhl->ly >> 6;
		wheelCrd.z = curWhl->lz >> 6;
		if (core.game_inputmode == 2) {
			road_num = -1;
			g_cursurfacekindvalue = 1;
			pl_i = 0;
			plncurrptr = g_planlist;
		} else {
			build_obj(&wheelCrd, &arg_pState->car_whlWorldCrds1[w]);
		}
		arg_pState->car_surfaceWhl[w] = g_cursurfacekindvalue;
		wheelCrd.x = curWhl->lx >> 6;
		wheelCrd.y = curWhl->ly >> 6;
		wheelCrd.z = curWhl->lz >> 6;
		if (core.game_inputmode == 2)
			nextpos_normalip = wheelCrd.y;
		else
			nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (road_num != -1 && nextpos_normalip > element_min_wall && nextpos_normalip < wall_wallelement) {
			oldVec.x = arg_pState->car_whlWorldCrds1[w].x - wallanchor_x;
			oldVec.y = 0;
			oldVec.z = arg_pState->car_whlWorldCrds1[w].z - wallanchor_z;
			vec.x = (int)(curWhl->lx >> 6) - wallanchor_x;
			vec.y = 0;
			vec.z = (int)(curWhl->lz >> 6) - wallanchor_z;
			matroty(&localPlane, -wall_facingang - 0x100);
			mat_vec(&oldVec, &localPlane, &p0);
			mat_vec(&vec, &localPlane, &p1);
			if ((p1.z <= 0 || p0.z <= 0) && (p1.z >= 0 || p0.z >= 0)) {
				if (p1.z > p0.z) {
					swap = 1;
					res = p1;
					p1 = p0;
					p0 = res;
				} else {
					swap = 0;
				}
				if (p1.z == 0) {
					remDist = spd;
					distBefore = 0;
				} else if (p0.z == 0) {
					remDist = 0;
					distBefore = spd;
				} else {
					vector_op_unk(&p1, &p0, &res, 0);
					tmpCrds.x = (p1.x - res.x) << 6;
					tmpCrds.y = (p1.y - res.y) << 6;
					tmpCrds.z = (p1.z - res.z) << 6;
					distBefore = polarRadius3D(&tmpCrds);
					remDist = spd - distBefore;
				}
				angle = (-rotxvehicle - wall_facingang) & 0x3FF;
				res.z = distBefore;
				res.y = 0;
				if (angle < 0x100 || angle > 0x300) {
					angle = wall_facingang;
					res.x = 0x300;
				} else {
					angle = (wall_facingang + 0x200) & 0x3FF;
					res.x = -0x300;
				}
				if (swap != 0)
					res.x = -res.x;
				rotMat = matrotzxy(-anglerotate_car, -yrotrotveh, angle, 0);
				mat_vec(&res, rotMat, &p1);
				i = (-rotxvehicle - angle) & 0x3FF;
				found = 0;
				if (i > 0x100) {
					i = 0x400 - i;
					found = 1;
				}
				threshold = (100 - ((70 * i) >> 8)) << 8;
				if (arg_pState->car_speed2 > threshold) {
					arg_pState->car_36MwhlAngle = found = (found != 0 ? -i : i) << 1;
					update_crash_state(1, arg_MplayerFlag);
				}
				arg_pState->field_CF |= 0x10;
				curWhl = whlPos;
				prev = oldWhls;
				for (i = 0; i < 4; ++curWhl, ++prev, ++i) {
					if (remDist != 0) {
						p0.x = (curWhl->lx - prev->lx) * remDist / spd;
						p0.y = (curWhl->ly - prev->ly) * remDist / spd;
						p0.z = (curWhl->lz - prev->lz) * remDist / spd;
					} else {
						p0.x = 0;
						p0.y = 0;
						p0.z = 0;
					}
					curWhl->lx = prev->lx + (short)(p0.x + p1.x);
					curWhl->ly = prev->ly + (short)(p0.y + p1.y);
					curWhl->lz = prev->lz + (short)(p0.z + p1.z);
				}
				goto retry;
			}
		}
check_height:
		if (nextpos_normalip > 0) {
			if (liftOfs > 0 && nextpos_normalip < 24) {
				curWhl->lx += hopVec.x;
				curWhl->ly += hopVec.y;
				curWhl->lz += hopVec.z;
			} else {
				arg_pState->car_rc1[w] += collision_rotation_offsets[w];
				curWhl->ly -= arg_pState->car_rc1[w];
				if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
					arg_pState->car_rc1[w] += collision_rotation_offsets[w];
					curWhl->ly -= arg_pState->car_rc1[w];
				}
				wheelCrd.y = curWhl->ly >> 6;
				if (core.game_inputmode == 2)
					nextpos_normalip = wheelCrd.y;
				else
					nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (nextpos_normalip > 12)
					arg_pState->car_surfaceWhl[w] = 0;
			}
		}
		whlHgts[w] = nextpos_normalip;
		if (nextpos_normalip == 0) {
			if (arg_pState->car_rc1[w] > 250)
				arg_pState->field_CF |= 0x20;
			if (arg_pState->car_rc1[w] > 23275)
				update_crash_state(1, arg_MplayerFlag);
			arg_pState->car_rc1[w] = 0;
			goto next_wheel;
		}
		if (nextpos_normalip >= 0)
			goto next_wheel;
		plane = &g_planlist[pl_i];
		base.x = plane->plane_origin.x + x_course_part;
		base.y = plane->plane_origin.y + hgthgt;
		base.z = plane->plane_origin.z + road_elem_ctrz;
		oldVec.x = (int)(prev->lx >> 6) - base.x;
		oldVec.y = (int)(prev->ly >> 6) - base.y;
		oldVec.z = (int)(prev->lz >> 6) - base.z;
		vec.x = (int)(curWhl->lx >> 6) - base.x;
		vec.y = (int)(curWhl->ly >> 6) - base.y;
		vec.z = (int)(curWhl->lz >> 6) - base.z;
		localPlane = plane->plane_rotation;
		mat_invert(&localPlane, &rotMatrix);
		mat_vec(&oldVec, &rotMatrix, &p0);
		mat_vec(&vec, &rotMatrix, &p1);
		swap = 0;
		if (test_pln == 0 && p0.y < -12 && p1.y < -12) {
			if (p1.y > -24) {
				update_crash_state(5, arg_MplayerFlag);
				swap = 1;
			} else {
				pl_i = 0;
				plncurrptr = g_planlist;
				test_pln = 1;
				wheelCrd.x = curWhl->lx >> 6;
				wheelCrd.y = curWhl->ly >> 6;
				wheelCrd.z = curWhl->lz >> 6;
				nextpos_normalip = plnoriginop(0, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				goto check_height;
			}
		}
		if (p1.y == 0) {
			tvec2.x = 0;
			tvec2.y = 0;
			tvec2.z = 0x40;
			g_planidx2 = pl_i;
			frwhl_angadjusted = wheelAngle[w];
			plnrotop();
			curWhl->lx -= pln_rot_output.x;
			curWhl->ly -= pln_rot_output.y;
			curWhl->lz -= pln_rot_output.z;
			goto rc_check;
		}
		if (p0.y <= 0 || p1.y >= 0) {
			tvec2.x = 0;
			tvec2.y = 0;
			tvec2.z = spd;
			g_planidx2 = pl_i;
			frwhl_angadjusted = wheelAngle[w];
			plnrotop();
			curWhl->lx = prev->lx + pln_rot_output.x;
			curWhl->ly = prev->ly + pln_rot_output.y;
			curWhl->lz = prev->lz + pln_rot_output.z;
		} else {
			angle = p0.z;
			p0.z = -p0.y;
			p0.y = angle;
			angle = p1.z;
			p1.z = -p1.y;
			p1.y = angle;
			vector_op_unk(&p1, &p0, &res, 0);
			tmpCrds.x = (p1.x - res.x) << 6;
			tmpCrds.y = (p1.y - res.y) << 6;
			tmpCrds.z = (p1.z - res.z) << 6;
			angle = polarRadius3D(&tmpCrds);
			remDist = arg_pState->car_rc1[w] + spd;
			distBefore = remDist - angle;
			p0.x = (curWhl->lx - prev->lx) * distBefore / remDist;
			p0.y = (curWhl->ly - prev->ly) * distBefore / remDist;
			p0.z = (curWhl->lz - prev->lz) * distBefore / remDist;
			tvec2.x = 0;
			tvec2.y = 0;
			tvec2.z = angle;
			g_planidx2 = pl_i;
			frwhl_angadjusted = wheelAngle[w];
			plnrotop();
			curWhl->lx = prev->lx + p0.x + pln_rot_output.x;
			curWhl->ly = prev->ly + p0.y + pln_rot_output.y;
			curWhl->lz = prev->lz + p0.z + pln_rot_output.z;
		}
		wheelCrd.x = curWhl->lx >> 6;
		wheelCrd.y = curWhl->ly >> 6;
		wheelCrd.z = curWhl->lz >> 6;
		if ((nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z)) < 0) {
			if (swap != 0)
				nextpos_normalip = -nextpos_normalip + 6;
			wheelCrd.z = 0;
			wheelCrd.x = 0;
			wheelCrd.y = -nextpos_normalip << 6;
			mat_mul_vector2(&wheelCrd, &g_planlist[pl_i].plane_rotation, &res);
			curWhl->lx += res.x;
			curWhl->ly += res.y;
			curWhl->lz += res.z;
		}
rc_check:
		if (arg_pState->car_rc1[w] > 250)
			arg_pState->field_CF |= 0x20;
		if (arg_pState->car_rc1[w] > 23275)
			update_crash_state(1, arg_MplayerFlag);
		arg_pState->car_rc1[w] = 0;
next_wheel:
		;
	}
wheels_done:
	if (arg_pState->car_surfaceWhl[0] == 5 && arg_pState->car_surfaceWhl[1] == 5 &&
	    arg_pState->car_surfaceWhl[2] == 5 && arg_pState->car_surfaceWhl[3] == 5)
		update_crash_state(2, arg_MplayerFlag);
	curWhl = whlPos;
	for (w = 0; w < 4; ++curWhl, ++w) {
		arg_pState->car_whlWorldCrds1[w].x = curWhl->lx >> 6;
		arg_pState->car_whlWorldCrds1[w].y = curWhl->ly >> 6;
		arg_pState->car_whlWorldCrds1[w].z = curWhl->lz >> 6;
		angle = carState_rc_op(arg_pState, whlHgts[w], w);
		if (anglerotate_car != 0 || yrotrotveh != 0) {
			wheelCrd.z = 0;
			wheelCrd.x = 0;
			wheelCrd.y = angle + 0x180;
			mat_vec(&wheelCrd, &matrix_transform_view, &oldVec);
			curWhl->lx += oldVec.x;
			curWhl->ly += oldVec.y;
			curWhl->lz += oldVec.z;
		} else {
			curWhl->ly += angle + 0x180;
		}
	}

	centerpos = (whlPos[0].lx + whlPos[1].lx + whlPos[2].lx + whlPos[3].lx) >> 2;
	veh_position = (whlPos[0].ly + whlPos[1].ly + whlPos[2].ly + whlPos[3].ly) >> 2;
	veh_z = (whlPos[0].lz + whlPos[1].lz + whlPos[2].lz + whlPos[3].lz) >> 2;
	curWhl = whlPos;
	for (w = 0; w < 4; ++curWhl, ++w) {
		posDiffs[w].x = curWhl->lx - centerpos;
		posDiffs[w].y = curWhl->ly - veh_position;
		posDiffs[w].z = curWhl->lz - veh_z;
	}
	if (veh_position < 0)
		veh_position = 0;
	if (centerpos > 0x1DF100L)
		centerpos = 0x1DF0FFL;
	else if (centerpos < 0xF00L)
		centerpos = 0xF00L;
	if (veh_z > 0x1DF100L)
		veh_z = 0x1DF0FFL;
	else if (veh_z < 0xF00L)
		veh_z = 0xF00L;
	angle = posDiffs[3].x + posDiffs[2].x - posDiffs[0].x - posDiffs[1].x;
	distBefore = posDiffs[3].z + posDiffs[2].z - posDiffs[0].z - posDiffs[1].z;
	rotxvehicle = polang(angle, -distBefore) & 0x3FF;
	matroty(&rotMatrix, rotxvehicle);
	for (w = 0; w < 4; w++) {
		res = posDiffs[w];
		mat_vec(&res, &rotMatrix, &posDiffs[w]);
	}
	distBefore = posDiffs[3].z + posDiffs[2].z - posDiffs[0].z - posDiffs[1].z;
	remDist = posDiffs[3].y + posDiffs[2].y - posDiffs[0].y - posDiffs[1].y;
	if (remDist == 0 && distBefore < 0) {
		yrotrotveh = 0;
	} else {
		yrotrotveh = polang(-distBefore, remDist) - 0x100;
		if ((yrotrotveh < 0 ? -yrotrotveh : yrotrotveh) < 2)
			yrotrotveh = 0;
	}
	if (yrotrotveh != 0) {
		mat_rot_x(&rotMatrix, yrotrotveh);
		for (w = 0; w < 4; w++) {
			res = posDiffs[w];
			mat_vec(&res, &rotMatrix, &posDiffs[w]);
		}
	}
	distBefore = posDiffs[1].x + posDiffs[2].x - posDiffs[0].x - posDiffs[3].x;
	remDist = posDiffs[1].y + posDiffs[2].y - posDiffs[0].y - posDiffs[3].y;
	if (remDist == 0 && distBefore > 0) {
		anglerotate_car = 0;
	} else {
		anglerotate_car = polang(distBefore, remDist) - 0x100;
		if ((anglerotate_car < 0 ? -anglerotate_car : anglerotate_car) < 2)
			anglerotate_car = 0;
	}
	arg_pState->car_sumSurfFrontWheels = arg_pState->car_surfaceWhl[0] + arg_pState->car_surfaceWhl[1];
	arg_pState->car_sumSurfRearWheels = arg_pState->car_surfaceWhl[2] + arg_pState->car_surfaceWhl[3];
	if (core.game_inputmode == 2)
		goto store_state;
	if (inrepflg == 0) {
		if (arg_MplayerFlag != 0)
			/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unk3(arg_pState->field_CF, op_eng_sound_id);
		else
			/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unk3(arg_pState->field_CF, g_player_sound_id);
	}
	rotMat = matrotzxy(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
	for (w = 0; w < 4; w++) {
		wheelCrd = arg_pSimd->wheel_coords[w];
		wheelCrd.y = arg_pSimd->collide_points[0].py << 6;
		mat_vec(&wheelCrd, rotMat, &res);
		wheelCrd.x = (res.x + centerpos) >> 6;
		wheelCrd.y = (res.y + veh_position) >> 6;
		wheelCrd.z = (res.z + veh_z) >> 6;
		tmpCrds = wheelCrd;
		build_obj(&wheelCrd, &arg_pState->car_whlWorldCrds2[w]);
		i = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (pl_i < 4) {
			if (i <= 0)
				goto crash_wheel;
		} else {
			planIdx = pl_i;
			wheelCrd = arg_pState->car_whlWorldCrds2[w];
			build_obj(&wheelCrd, &tmpCrds);
			if (planIdx == pl_i) {
				found = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (gm_playmode != 1 && ((i < 0 && found > 0) || (i > 0 && found < 0))) {
crash_wheel:
					update_crash_state(5, arg_MplayerFlag);
				}
			}
		}
		arg_pState->car_whlWorldCrds2[w] = tmpCrds;
	}
	groundContact = arg_pState->car_sumSurfFrontWheels + arg_pState->car_sumSurfRearWheels;
	if (arg_MplayerFlag == 0 && groundContact == 0 && arg_pState->car_sumSurfAllWheels != 0)
		core.game_jumpCount++;
	arg_pState->car_sumSurfAllWheels = groundContact;
	self[0].x = centerpos >> 6;
	self[0].y = veh_position >> 6;
	self[0].z = veh_z >> 6;
	self[1].x = anglerotate_car;
	self[1].y = yrotrotveh;
	self[1].z = rotxvehicle;
	if (globalgamesettings.game_opponenttype != 0) {
		objPos[0].x = arg_oState->car_posWorld1.lx >> 6;
		objPos[0].y = arg_oState->car_posWorld1.ly >> 6;
		objPos[0].z = arg_oState->car_posWorld1.lz >> 6;
		objPos[1].x = arg_oState->car_rotate.z;
		objPos[1].y = arg_oState->car_rotate.y;
		objPos[1].z = arg_oState->car_rotate.x;
		if (car_car_coll_detect_maybe(arg_pSimd->collide_points, self, arg_oSimd->collide_points, objPos)) {
			if (arg_pState->field_C8 != 0)
				return;
			if (car_car_speed_adjust_maybe(arg_pState, arg_oState) == 0)
				return;
			update_crash_state(1, arg_MplayerFlag);
			update_crash_state(1, arg_MplayerFlag ^ 1);
			return;
		}
	}
	res.x = self[0].x >> 10;
	res.z = 29 - (self[0].z >> 10);
	objPos[1].x = 0;
	objPos[1].y = 0;
	objPos[1].z = 0;
	if (res.x < 0 || res.x >= 30 || res.z < 0 || res.z >= 30)
		goto store_state;
	tiltFlag = bto_auxiliary1(res.x, res.z, objPts);
	if (tiltFlag != 0) {
		for (i = 0; i < tiltFlag; nextVec++, i++) {
			objPos[0].x = objPts[i].x;
			objPos[0].y = objPts[i].y;
			objPos[0].z = objPts[i].z;
			if (car_car_coll_detect_maybe(arg_pSimd->collide_points, self, collision_point_set_c, objPos)) {
				arg_pState->car_36MwhlAngle -= 0x200;
crash_return:
				update_crash_state(1, arg_MplayerFlag);
				return;
			}
		}
	}
	i = (char)td19hdl[lnoffsets[res.z] + res.x];
	if (i != -1 && core.field_3FA[i] == 0) {
		objPos[0].x = td10checkptr[i].x;
		objPos[0].y = td10checkptr[i].y;
		objPos[0].z = td10checkptr[i].z;
		if (car_car_coll_detect_maybe(arg_pSimd->collide_points, self, collision_point_set_a, objPos)) {
			core.field_3FA[i] = 1;
			state_op_unk(i + 2, -arg_pState->car_rotate.x, (long)arg_pState->car_speed2 * 0x580 / 0x3C00U);
		}
	}
	if (res.x == idxtrk && res.z == tagtrk) {
		objPos[0].x = trackctrpos2[idxtrk] + mulscl(sinfast(st_hdg + 0x100), 126);
		objPos[0].y = hillconsts[g_hillf];
		objPos[0].z = mulscl(cosfast(st_hdg + 0x100), 126) + row_ctr_zs[tagtrk];
		if ((found = car_car_coll_detect_maybe(arg_pSimd->collide_points, self, collision_point_set_b, objPos)) == 0) {
			objPos[0].x = mulscl(sinfast(st_hdg + 0x300), 126) + trackctrpos2[idxtrk];
			objPos[0].z = mulscl(cosfast(st_hdg + 0x300), 126) + row_ctr_zs[tagtrk];
			found = car_car_coll_detect_maybe(arg_pSimd->collide_points, self, collision_point_set_b, objPos);
		}
		if (found != 0)
			goto crash_return;
	}
store_state:
	arg_pState->car_posWorld1.lx = centerpos;
	arg_pState->car_posWorld1.ly = veh_position;
	arg_pState->car_posWorld1.lz = veh_z;
	arg_pState->car_rotate.z = anglerotate_car;
	arg_pState->car_rotate.y = yrotrotveh;
	arg_pState->car_rotate.x = rotxvehicle;
	arg_pState->field_C8 = 0;
}

void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short trkang)
{
    register int zero = 0;
    register int i;
    struct VECTOR whlPos;
    playerstate->car_posWorld1.lx = posX;
    playerstate->car_posWorld2.lx = posX;
    playerstate->car_posWorld1.ly = posY + 512;
    playerstate->car_posWorld2.ly = posY;
    playerstate->car_posWorld1.lz = posZ;
    playerstate->car_posWorld2.lz = posZ;
    playerstate->car_rotate.x = trkang;
    playerstate->car_rotate.y = zero;
    playerstate->car_rotate.z = zero;
    playerstate->car_36MwhlAngle = zero;
    playerstate->car_pseudoGravity = zero;
    playerstate->car_steeringAngle = zero;
    playerstate->car_is_accelerating = playerstate->car_is_braking = zero;
    playerstate->car_currpm = simd->idle_rpm;
    playerstate->car_lastrpm = playerstate->car_currpm;
    playerstate->car_idlerpm2 = playerstate->car_currpm;
    playerstate->car_current_gear = 1;
    playerstate->car_speeddiff = zero;
    playerstate->car_speed = zero;
    playerstate->car_speed2 = zero;
    playerstate->car_lastspeed = zero;
    playerstate->car_gearratio = simd->gear_ratios[1];
    playerstate->car_gearratioshr8 = playerstate->car_gearratio >> 8;
    playerstate->car_knob_x = simd->knob_points[1].px;
    playerstate->car_knob_x2 = playerstate->car_knob_x;
    playerstate->car_knob_y = simd->knob_points[1].py;
    playerstate->car_knob_y2 = playerstate->car_knob_y;
    playerstate->car_angle_z = zero;
    playerstate->car_40MfrontWhlAngle = zero;
    playerstate->field_42 = zero;
    playerstate->field_48 = zero;
    playerstate->car_trackdata3_index = zero;
    playerstate->car_sumSurfFrontWheels = 2;
    playerstate->car_sumSurfRearWheels = 2;
    playerstate->car_sumSurfAllWheels = 4;
    playerstate->car_demandedGrip = zero;
    playerstate->car_surfacegrip_sum = 1000;
    whlPos.x = posX >> 6;
    whlPos.y = posY >> 6;
    whlPos.z = posZ >> 6;
    for (i = 0; i < 4; ++i) {
        playerstate->car_surfaceWhl[i] = 1;
        playerstate->car_rc1[i] = zero;
        playerstate->car_rc2[i] = zero;
        playerstate->car_rc3[i] = zero;
        playerstate->car_rc4[i] = zero;
        playerstate->car_rc5[i] = zero;
        playerstate->car_whlWorldCrds1[i] = whlPos;
        playerstate->car_whlWorldCrds2[i] = whlPos;
    }
    playerstate->car_engineLimiterTimer = zero;
    playerstate->car_slidingFlag = zero;
    playerstate->field_C8 = zero;
    playerstate->car_crashBmpFlag = zero;
    playerstate->car_changing_gear = zero;
    playerstate->car_fpsmul2 = zero;
    playerstate->car_transmission = transmission;
    playerstate->field_CD = zero;
    playerstate->field_CE = zero;
    playerstate->field_CF = 1;
}

/* Purpose: Initializes race state and frame-rate dependent timing.
 * Parameters: arg.
 * Returns: none.
 * Globals read: core, g_hillf, globalgamesettings, hillconsts, idxtrk, ophys_7, r_zp,
 *            rate_frame, row_ctr_zs, simdp7, st_hdg, steerWhlRespTable_10fps,
 *            steerWhlRespTable_20fps, tagtrk, td3, trackctrpos2
 * Globals written: core, cvxs_a, elaptm1, frmcs_time, g_cvxintvl, table_lookup
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void initialize_game_state(short arg)
{
	register int zeroValue = 0;
	register int i;
	int tmpcol, tmprow;

	if (arg == -1) {
		elaptm1 = zeroValue;

		for (i = 0; i < 20; ++i) {
			cvxs_a[i].field_3F4 = zeroValue;
		}
	}
	
	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
		table_lookup = &steerWhlRespTable_10fps;
	}
	else {
		table_lookup = &steerWhlRespTable_20fps;
	}
	
	g_cvxintvl = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame * 30;
	frmcs_time = 100 / /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;

	if (arg != -3) {
		initialize_unknown();

		core.field_3F4 = 1;
		core.game_frames_per_sec = 1;
		core.game_inputmode = zeroValue;
		core.game_3F6autoLoadEvalFlag = zeroValue;
		core.game_frame_in_sec = zeroValue;
		core.field_2F4 = zeroValue;
		core.field_3F7[0] = zeroValue;
		core.field_3F7[1] = zeroValue;

		for (i = zeroValue; i < 48; ++i) {
			core.field_3FA[i] = zeroValue;
		}
				
		for (i = zeroValue; i < 24; ++i) {
			core.field_38E[i] = zeroValue;
		}

		core.game_vec1[0].x =
			  mulscl(sinfast(st_hdg + 0x200), 4096)
			+ mulscl(sinfast(st_hdg + 0x300),  512)
			+ ((short)idxtrk << 10);

		core.game_vec1[0].y = hillconsts[g_hillf] + 960;

		core.game_vec1[0].z =
			  mulscl(cosfast(st_hdg + 0x200), 4096)
			+ mulscl(cosfast(st_hdg + 0x300),  512)
			+ r_zp[tagtrk];

		core.game_vec1[1] = core.game_vec1[0];
		core.game_vec3[0] = core.game_vec1[0];
		core.game_vec3[1] = core.game_vec1[0];
		
		core.game_travDist = 0L;
		core.game_frame = zeroValue;
		core.game_total_finish = zeroValue;
		core.field_144 = zeroValue;
		core.game_pEndFrame = zeroValue;
		core.game_oEndFrame = zeroValue;
		core.game_penalty = zeroValue;
		core.game_impactSpeed = zeroValue;
		core.game_topSpeed = zeroValue;
		core.game_jumpCount = zeroValue;

		
		tmpcol =
			  mulscl(sinfast(st_hdg + 0x100),  36)
			+ mulscl(sinfast(st_hdg + 0x200), 210);
		
		tmprow =
			  mulscl(cosfast(st_hdg + 0x100),  36)
			+ mulscl(cosfast(st_hdg + 0x200), 210);

		init_carstate_from_simd(
			&core.playerstate,
			&simdp7,
			globalgamesettings.game_playertransmission,
			(long)(trackctrpos2[idxtrk] + tmpcol) * 64L,
			(long)hillconsts[g_hillf] * 64L,
			(long)(row_ctr_zs[tagtrk] + tmprow) * 64L,
			-st_hdg);

		core.field_2F2 = zeroValue;
		core.field_45D = zeroValue;
		core.field_45E = zeroValue;
		core.field_45B = zeroValue;
		core.field_45C = zeroValue;
		
		core.game_startcol  = idxtrk;
		core.game_startcol2 = idxtrk;
		core.game_startrow  = tagtrk;
		core.game_startrow2 = tagtrk;

		if (arg != -2) {
			track_edge_points(
				core.playerstate.car_trackdata3_index,
				&core.playerstate.car_vec_unk3,
				core.playerstate.field_CE++,
				0);
			
		}

		
		tmpcol =
			  mulscl(sinfast(st_hdg + 0x300),  36)
			+ mulscl(sinfast(st_hdg + 0x200), 210);
		
		tmprow =
			  mulscl(cosfast(st_hdg + 0x300),  36)
			+ mulscl(cosfast(st_hdg + 0x200), 210);

		init_carstate_from_simd(
			&core.opponentstate,
			&ophys_7,
			1,
			(long)(trackctrpos2[idxtrk] + tmpcol) * 64L,
			(long)hillconsts[g_hillf] * 64L,
			(long)(row_ctr_zs[tagtrk] + tmprow) * 64L,
			-st_hdg);

		if (globalgamesettings.game_opponenttype && arg != -2) {
			track_edge_points(
				((short far *)td3)[core.opponentstate.car_trackdata3_index], 
				&core.opponentstate.car_vec_unk3,
				core.opponentstate.field_CE++,
				&core.field_3F9); 
		
		}

		core.field_42A = zeroValue;
	}
}

/* Purpose: Restores saved game state and timer position.
 * Parameters: frame.
 * Returns: none.
 * Globals read: core, cvxs_a, elaptm1, g_cvxintvl
 * Globals written: core, tmr2
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void restore_gamestate(int frame)
{
    register int curframe;

    if (frame == 0 && elaptm1 == 0) {
        initialize_game_state(0);
    }

    curframe = frame / g_cvxintvl;
    if (curframe == 20) {
        --curframe;
    }

    if (frame < core.game_frame)
        goto restore;
    while (g_cvxintvl * curframe > core.game_frame) {
        if (cvxs_a[curframe].field_3F4 != 0) {
restore:
            core = cvxs_a[curframe];
            initialize_kevin_random(core.kevinseed);
            /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ tmr2 = core.game_frame;
            return;
        }
        --curframe;
    }
}

/* Purpose: Advances race state and dispatches engine audio updates.
 * Parameters: none.
 * Returns: none.
 * Globals read: core, g_cvxintvl, g_rpl_init, g_sgateopn, g_tdreplay16buf, globalgamesettings,
 *            gm_playmode, idxtrk, row_ctr_zs, sigframe, st_hdg, tagtrk, trackctrpos2
 * Globals written: core, cvxs_a, g_rpl_init, g_sgateopn, sigframe
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void update_gamestate() {
	char var_carInputByte;
	register int tmp;

	var_carInputByte = g_tdreplay16buf[core.game_frame];
	if (var_carInputByte != 0) {
		core.game_inputmode = 1;
	}
	
	if ((core.game_frame % g_cvxintvl) == 0) {
		tmp = core.game_frame / g_cvxintvl;
		get_kevinrandom_seed(core.kevinseed);

		cvxs_a[tmp] = core;
	}

	core.game_frame++;
	if (core.game_3F6autoLoadEvalFlag != 0 && core.game_frame_in_sec < core.game_frames_per_sec) {
		core.game_frame_in_sec++;
		if (core.game_frame_in_sec == core.game_frames_per_sec && sigframe == 0) {
			if (core.playerstate.car_crashBmpFlag == 1 && core.playerstate.car_speed2 != 0) {
				core.game_frames_per_sec++;
			} else if (gm_playmode == 0) {
				sigframe = 1;
			}
		}
	}

	if (core.game_inputmode != 0) {
		
		player_op(var_carInputByte);
		
		if (globalgamesettings.game_opponenttype != 0) {
			opponent_op();
		}

		update_camera_target();
		if (core.field_42A != 0) {
			update_crash_debris();
		}

		/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_carstate();

	} else if (gm_playmode == 1) {
		
		/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_carstate();
		if (g_rpl_init != 0) {
			if (g_sgateopn < 0x1C2) {
				g_sgateopn += 8;
			}

			if (g_rpl_init == 1 && g_sgateopn > 0x180) {
				g_rpl_init++;
			}

			if (g_rpl_init == 2) {
				tmp =
					mulscl(cosfast(st_hdg), row_ctr_zs[tagtrk] - (core.playerstate.car_posWorld1.lz >> 6))
					+ mulscl(sinfast(st_hdg), trackctrpos2[idxtrk] - (core.playerstate.car_posWorld1.lx >> 6));
				if (tmp > 0xE4) {
					if (core.playerstate.car_speed < 0x500) {
						player_op(1);
					} else {
						player_op(0);
					}
				} else {
					if (core.playerstate.car_speed != 0) {
						player_op(2);
					} else {
						g_rpl_init = 0;
					}
				}
			}
		}
	}
}

/* Purpose: Applies player driving input to the race simulation.
 * Parameters: arg_carInputByte.
 * Returns: none.
 * Globals read: core, g_penaltytm, g_td01_track_filecpy, idxtrk, ophys_7, pen_flag_count,
 *            rate_frame, row_ctr_zs, simdp7, st_hdg, tagtrk, trackctrpos2,
 *            trackdata_penalty_related
 * Globals written: core, g_penaltytm, pen_flag_count
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void player_op(char arg_carInputByte) {
	struct VECTOR player_plane_vec;
	struct VECTOR playerRelative;
	struct VECTOR pathPoints[4];
	struct MATRIX *planeMatrix;
	unsigned char trackSide;
	unsigned char track_side;
	char hasPenalty;
	struct VECTOR offset_vector;
	int penalty_ctr;
	char crash_mode;
	struct VECTOR playerEdges[4];
	int tile_index;
	register int height;

	if (pen_flag_count != 0)
		pen_flag_count--;

	core.playerstate.field_CF = 1;
	if (core.playerstate.car_crashBmpFlag != 0) {
		core.field_45D = 0;
		arg_carInputByte = 2;
		if (core.playerstate.car_speed2 == 0) {
			core.playerstate.field_CF = 0;
			if (core.playerstate.car_speed == 0 && core.playerstate.car_rc1[0] == 0 &&
			    core.playerstate.car_rc1[1] == 0 && core.playerstate.car_rc1[2] == 0 &&
			    core.playerstate.car_rc1[3] == 0)
				return;
		}
	}

	update_car_speed(arg_carInputByte, 0, &core.playerstate, &simdp7);
	upd_statef20_from_steer_input((arg_carInputByte >> 2) & 3);
	update_grip(&core.playerstate, &simdp7, 1);
	update_player_state(&core.playerstate, &simdp7, &core.opponentstate, &ophys_7, 0);
	core.game_travDist += core.playerstate.car_speed2;
	crash_mode = core.field_45B;
	tile_index = core.field_2F2;
	height = detect_penalty(&tile_index, &penalty_ctr);
	if (height != 0) {
		if (penalty_ctr == -2) {
			core.field_45B = 1;
			core.field_45C = 0;
		} else if (core.field_45B == 1) {
			core.field_45B = 0;
			core.field_45C = 0;
		}
		if (core.field_45B == 0) {
			if (tile_index == 0 && core.field_2F4 != 0) {
				core.playerstate.field_CD++;
				goto next_lap;
			}
			if (penalty_ctr >= 0 && penalty_ctr < 3) {
				core.field_45C = 0;
				core.field_2F2 = tile_index;
			} else if (penalty_ctr == -1 || penalty_ctr > 3) {
				if (g_td01_track_filecpy[core.field_2F4] == tile_index ||
				    trackdata_penalty_related[core.field_2F4] == tile_index) {
					core.field_45C++;
				} else {
					if (g_td01_track_filecpy[tile_index] == core.field_2F4 ||
					    trackdata_penalty_related[tile_index] == core.field_2F4)
						core.field_45B = 2;
					core.field_45C = 1;
				}
				if (core.field_45C >= 3) {
next_lap:
					core.field_2F2 = tile_index;
					core.field_45C = 0;
					if (penalty_ctr > 0) {
						g_penaltytm = penalty_ctr * /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame * 3;
						pen_flag_count = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
						core.game_penalty += g_penaltytm;
					}
				}
			}
		}
		core.field_2F4 = tile_index;
	}

	core.field_45D = 0;
	if (core.field_45B == 1)
		return;
	planeMatrix = matrotzxy(core.playerstate.car_rotate.z, core.playerstate.car_rotate.y,
	                          core.playerstate.car_rotate.x, 1);
	if (core.field_45B == 2) {
		if (core.playerstate.car_crashBmpFlag == 0)
			core.field_45D = 3;
		tile_index = core.field_2F4;
		goto search;
	}
	if (core.playerstate.car_trackdata3_index == -1) {
no_target:
		height = 0;
	} else {
		if ((crash_mode != 0 && core.field_45B == 0) ||
		    (core.playerstate.car_trackdata3_index != core.field_2F2 &&
		     g_td01_track_filecpy[core.field_2F2] != core.playerstate.car_trackdata3_index &&
		     trackdata_penalty_related[core.field_2F2] != core.playerstate.car_trackdata3_index)) {
			core.playerstate.car_trackdata3_index = -1;
			goto no_target;
		}
		playerRelative.x = core.playerstate.car_vec_unk3.x - (int)(core.playerstate.car_posWorld1.lx >> 6);
		if (core.playerstate.car_vec_unk3.y != -1)
			playerRelative.y = core.playerstate.car_vec_unk3.y - (int)(core.playerstate.car_posWorld1.ly >> 6);
		else
			playerRelative.y = 0;
		playerRelative.z = core.playerstate.car_vec_unk3.z - (int)(core.playerstate.car_posWorld1.lz >> 6);
		mat_vec(&playerRelative, planeMatrix, &player_plane_vec);
		height = player_plane_vec.z;
	}
	if (height < 0x113) {
		if (core.playerstate.car_trackdata3_index == -1) {
			tile_index = core.field_2F2;
search:
			if (trackdata_penalty_related[tile_index] != -1)
				goto check_lap;
			hasPenalty = 0;
			track_side = 0;
			do {
				hasPenalty = track_edge_points(tile_index, &core.playerstate.car_vec_unk3, track_side, 0);
				offset_vector = core.playerstate.car_vec_unk3;
				offset_vector.x -= core.playerstate.car_posWorld1.lx >> 6;
				if (offset_vector.y == -1)
					offset_vector.y = -(int)(core.playerstate.car_posWorld1.ly >> 6);
				else
					offset_vector.y -= core.playerstate.car_posWorld1.ly >> 6;
				offset_vector.z -= core.playerstate.car_posWorld1.lz >> 6;
				mat_vec(&offset_vector, planeMatrix, &player_plane_vec);
				if (track_side == 0 ||
				    (player_plane_vec.z < playerRelative.z && player_plane_vec.z > 0)) {
					trackSide = track_side;
					playerRelative.z = player_plane_vec.z;
				}
				track_side++;
			} while (hasPenalty == 0);
			if (core.field_45B == 2) {
				if (trackSide == 0) {
					track_edge_points(tile_index, pathPoints, 0, 0);
					track_edge_points(tile_index, playerEdges, 1, 0);
				} else {
					track_edge_points(tile_index, pathPoints, (char)(trackSide - 1), 0);
					track_edge_points(tile_index, playerEdges, trackSide, 0);
				}
				height = polang(pathPoints[0].x - playerEdges[0].x, playerEdges[0].z - pathPoints[0].z) & 0x3FF;
				height = (core.playerstate.car_rotate.x - height) & 0x3FF;
				if (height <= 0x380 && height >= 0x80)
					goto advance;
				core.field_45B = 0;
				core.field_45C = 1;
				core.playerstate.car_trackdata3_index = tile_index;
			} else {
				core.playerstate.car_trackdata3_index = core.field_2F2;
			}
			core.playerstate.field_CE = trackSide;
		}
advance:
		if (track_edge_points(core.playerstate.car_trackdata3_index, &core.playerstate.car_vec_unk3,
		              core.playerstate.field_CE++, 0) != 0) {
			if (trackdata_penalty_related[core.field_2F2] != -1)
				core.playerstate.car_trackdata3_index = -1;
			else
				core.playerstate.car_trackdata3_index = g_td01_track_filecpy[core.field_2F2];
			core.playerstate.field_CE = 0;
		}
	}
	offset_vector = core.playerstate.car_vec_unk3;
	if (core.playerstate.car_trackdata3_index != -1 && core.field_45B == 0) {
		offset_vector.x -= core.playerstate.car_posWorld1.lx >> 6;
		if (offset_vector.y == -1)
			offset_vector.y = 0;
		else
			offset_vector.y -= core.playerstate.car_posWorld1.ly >> 6;
		offset_vector.z -= core.playerstate.car_posWorld1.lz >> 6;
		planeMatrix = matrotzxy(core.playerstate.car_rotate.z, core.playerstate.car_rotate.y,
		                          core.playerstate.car_rotate.x, 1);
		mat_vec(&offset_vector, planeMatrix, &player_plane_vec);
		core.playerstate.field_48 = polang(-player_plane_vec.x, player_plane_vec.z) & 0x3FF;
		if (core.playerstate.car_crashBmpFlag == 0) {
			switch ((unsigned)((core.playerstate.field_48 + 0x80) & 0x3FF) >> 8) {
			case 1:
				core.field_45D = 1;
				break;
			case 3:
				if (core.playerstate.field_B6 == 0) {
					core.field_45D = 2;
					break;
				}
			default:
				core.field_45D = 0;
				break;
			}
		}
	}
check_lap:
	if (core.playerstate.field_CD != 0) {
		height = mulscl(cosfast(st_hdg),
			row_ctr_zs[tagtrk] - (int)(core.playerstate.car_posWorld1.lz >> 6));
		height += mulscl(sinfast(st_hdg),
			trackctrpos2[idxtrk] - (int)(core.playerstate.car_posWorld1.lx >> 6));
		if (height < 0)
			update_crash_state(3, 0);
	}
}

char detect_penalty(int *trackIndex, int *penaltyCounter)
{
    register int cur;
    int mapIdx;
    int leastDistance;
    char carCol;
    char playerRow;
    int nextPiece;
    char tileX;
    int previousDistance[128];
    char row;
    int minNode;
    unsigned char mark[901];
    register int distance;
    int searchDepth;
    char last_row;
    int node_stack[128];
    char multiCell;
    char right_col;

    carCol = (char)(core.playerstate.car_posWorld1.lx >> 16);
    playerRow = (char)(0x1D - (char)(core.playerstate.car_posWorld1.lz >> 16));
    if ((carCol == core.game_startcol || carCol == core.game_startcol2) &&
        (playerRow == core.game_startrow || playerRow == core.game_startrow2)) {
        *penaltyCounter = 0;
        return 0;
    }
    if (carCol < 0 || carCol > 0x1D || playerRow < 0 || playerRow > 0x1D)
        goto invalid_coords;
    leastDistance = 0;
    searchDepth = 0;
    distance = 0;
    for (cur = 0; cur < g_trackpiecescounter; cur++)
        mark[cur] = 0;
    cur = *trackIndex;
    for (;;) {
        mapIdx = g_td01_track_filecpy[cur];
        if (mark[mapIdx] != 0) {
            if (searchDepth != 0) {
                searchDepth--;
                cur = node_stack[searchDepth];
                distance = previousDistance[searchDepth];
                continue;
            }
            if (leastDistance != 0) {
                *trackIndex = minNode;
                *penaltyCounter = leastDistance;
                return 1;
            }
            core.game_startcol2 = core.game_startcol = carCol;
            core.game_startrow2 = core.game_startrow = playerRow;
invalid_coords:
            *penaltyCounter = -2;
            return 1;
        }
        mark[mapIdx] = 1;
        row = tdfrompathrow22[mapIdx];
        multiCell = trklst[(unsigned char)road_trk[mapIdx]].ss_multiTileFlag;
        last_row = (multiCell & 1) ? row + 1 : row;
        tileX = g_column_of_trkdata21_pth[mapIdx];
        right_col = (multiCell & 2) ? tileX + 1 : tileX;
        if ((tileX == carCol || right_col == carCol) &&
            (row == playerRow || last_row == playerRow)) {
            if (trackdata_penalty_related[cur] != -1)
                mapIdx = cur;
            core.game_startcol = tileX;
            core.game_startcol2 = right_col;
            core.game_startrow = row;
            core.game_startrow2 = last_row;
            if (distance > 0) {
                if (leastDistance == 0 || leastDistance > distance) {
                    minNode = mapIdx;
                    leastDistance = distance;
                }
            } else {
                *trackIndex = mapIdx;
                *penaltyCounter = distance;
                return 1;
            }
        }
        nextPiece = trackdata_penalty_related[cur];
        if (nextPiece != -1) {
            previousDistance[searchDepth] = distance;
            node_stack[searchDepth++] = nextPiece;
        }
        if (mapIdx != 0) {
            if (distance != -1)
                distance++;
        } else
            distance = -1;
        cur = mapIdx;
    }
}

/* Purpose: Updates gear, speed, and engine state for one car.
 * Parameters: arg_carInputByte, arg_MplayerFlag, arg_carState, arg_simd.
 * Returns: none.
 * Globals read: core, opponent_spd_tbl, rate_frame
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_car_speed(char arg_carInputByte, char arg_MplayerFlag, struct CARSTATE* arg_carState, struct SIMD* arg_simd) {
	int knobStep;
	int offset;
	unsigned int updatedSpeed;
	int speedDelta;
	unsigned char currTorque;

	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 20)
		knobStep = 6;
	else
		knobStep = 12;

	if (arg_carState->car_engineLimiterTimer != 0)
		arg_carState->car_engineLimiterTimer--;

	arg_carState->car_speeddiff = arg_carState->car_speed2 - arg_carState->car_lastspeed;
	arg_carState->car_lastspeed = arg_carState->car_speed2;
	arg_carState->car_lastrpm = arg_carState->car_currpm;
	if (arg_carState->car_transmission == 0 && arg_carState->car_changing_gear == 0) {
		if (arg_carInputByte & 0x10)
			goto upshift;
		else if (arg_carInputByte & 0x20)
			goto downshift;
	} else if (arg_carState->car_current_gear != 0 && arg_carState->car_changing_gear == 0 &&
	           arg_carState->car_sumSurfRearWheels != 0) {
		if (arg_carState->car_currpm > arg_simd->upshift_rpm) {
upshift:
			if (arg_carState->car_current_gear != arg_simd->num_gears) {
				arg_carState->car_current_gear++;
				goto shifted;
			}
		} else if (arg_carState->car_currpm < arg_simd->downshift_rpm) {
downshift:
			if (arg_carState->car_current_gear > 1) {
				arg_carState->car_current_gear--;
shifted:
				arg_carState->car_changing_gear = 1;
				arg_carState->car_fpsmul2 = ((char)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame >> 1) + (char)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
				arg_carState->car_knob_x2 = arg_simd->knob_points[arg_carState->car_current_gear].px;
				arg_carState->car_knob_y2 = arg_simd->knob_points[arg_carState->car_current_gear].py;
			}
		}
	}

	if (arg_carState->car_changing_gear != 0) {
		if (arg_carState->car_knob_x == arg_carState->car_knob_x2) {
			if ((offset = arg_carState->car_knob_y2 - arg_carState->car_knob_y) == 0) {
				arg_carState->car_changing_gear = 0;
				arg_carState->car_gearratio = arg_simd->gear_ratios[arg_carState->car_current_gear];
				arg_carState->car_gearratioshr8 = arg_carState->car_gearratio >> 8;
			} else if (abs(offset) <= knobStep) {
				arg_carState->car_knob_y = arg_carState->car_knob_y2;
			} else if (offset > 0) {
				arg_carState->car_knob_y += knobStep;
			} else {
				arg_carState->car_knob_y -= knobStep;
			}
		} else if (arg_simd->knob_points[0].py == arg_carState->car_knob_y) {
			offset = arg_carState->car_knob_x2 - arg_carState->car_knob_x;
			if (abs(offset) <= knobStep)
				arg_carState->car_knob_x = arg_carState->car_knob_x2;
			else if (offset > 0)
				arg_carState->car_knob_x += knobStep;
			else
				arg_carState->car_knob_x -= knobStep;
		} else {
			offset = arg_simd->knob_points[0].py - arg_carState->car_knob_y;
			if (abs(offset) <= knobStep)
				arg_carState->car_knob_y = arg_simd->knob_points[0].py;
			else if (offset > 0)
				arg_carState->car_knob_y += knobStep;
			else
				arg_carState->car_knob_y -= knobStep;
		}
	} else if (arg_carState->car_fpsmul2 != 0) {
		arg_carState->car_fpsmul2--;
	}

	updatedSpeed = arg_carState->car_speed;
	speedDelta = arg_carState->car_pseudoGravity - arg_simd->aerorestable[updatedSpeed >> 10];
	if (arg_carState->car_currpm > arg_simd->max_rpm) {
		arg_carState->car_currpm = arg_simd->max_rpm - 1;
brake:
		speedDelta -= arg_simd->braking_eff;
	} else {
		switch (arg_carInputByte & 3) {
		case 2:
			arg_carState->car_is_accelerating = 0;
			arg_carState->car_engineLimiterTimer = 0;
			arg_carState->car_is_braking = 1;
			if (arg_MplayerFlag == 0)
				goto brake;
			speedDelta -= arg_simd->braking_eff << 1;
			break;
		default:
			arg_carState->car_is_accelerating = 0;
			arg_carState->car_is_braking = 0;
			break;
		case 1:
			arg_carState->car_is_braking = 0;
			arg_carState->car_is_accelerating = 1;
			if (arg_carState->car_changing_gear != 0) {
				arg_carState->car_engineLimiterTimer = 0;
				if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
					arg_carState->car_currpm -= 80;
				else
					arg_carState->car_currpm -= 40;
			} else if (arg_carState->car_sumSurfRearWheels == 0) {
				if (arg_carState->car_currpm < arg_simd->max_rpm && updatedSpeed < 0xFA00)
					speedDelta += 0x300;
			} else {
				if (arg_carState->car_current_gear <= 1 && arg_carState->car_currpm < 0xA28)
					currTorque = arg_simd->idle_torque;
				else
					currTorque = arg_simd->torque_curve[(unsigned)arg_carState->car_currpm >> 7];
				if (arg_carState->car_engineLimiterTimer != 0 && arg_carState->car_currpm < 5000)
					currTorque = (arg_simd->idle_torque + currTorque) >> 1;
				speedDelta += (arg_carState->car_gearratioshr8 * currTorque) >> 4;
				speedDelta = (int)((long)speedDelta * 25 / arg_simd->car_mass) >> 1;
				if (arg_MplayerFlag != 0) {
					currTorque = -(opponent_spd_tbl[0] - 200) >> 1;
					if (currTorque != 0)
						speedDelta -= (long)currTorque * speedDelta / 200;
				}
				if (speedDelta > 0x128)
					arg_carState->car_engineLimiterTimer = 5;
			}
			break;
		}
	}
	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
		speedDelta += speedDelta;

if (speedDelta >= 0) {
if (updatedSpeed < 0x8000) {
updatedSpeed += speedDelta;
} else {
updatedSpeed += speedDelta;
if (updatedSpeed < 0x8000 || updatedSpeed > 0xF500)
updatedSpeed = 0xF500;
}
} else {
if (-speedDelta > updatedSpeed)
updatedSpeed = 0;
else
updatedSpeed += speedDelta;
}
	if (arg_carState->car_sumSurfRearWheels != 0) {
		if (((offset = arg_carState->car_speed2 - updatedSpeed) < 0 ? -offset : offset) > 0x1400) {
			arg_carState->car_speed = ((long)arg_carState->car_speed + arg_carState->car_speed2) >> 1;
			arg_carState->car_speed2 = arg_carState->car_speed;
			arg_carState->car_engineLimiterTimer = 5;
		} else {
			arg_carState->car_speed = updatedSpeed;
			arg_carState->car_speed2 = updatedSpeed;
		}
	} else {
		arg_carState->car_speed = updatedSpeed;
	}

	arg_carState->car_currpm = update_rpm_from_speed(arg_carState->car_currpm, arg_carState->car_speed,
		arg_carState->car_gearratio, arg_carState->car_changing_gear, arg_simd->idle_rpm);
	if (arg_carState->car_sumSurfAllWheels != 0 && arg_carState->car_lastrpm > arg_carState->car_currpm) {
		if (arg_carState->car_lastrpm - arg_carState->car_currpm > 2000) {
			if ((int)(arg_simd->idle_torque * arg_carState->car_gearratioshr8) > 12000)
				arg_carState->car_engineLimiterTimer = 30;
		} else if (arg_carState->car_currpm - arg_carState->car_lastrpm > 2000) {
			arg_carState->car_engineLimiterTimer = 10;
			arg_carState->car_speed2 -= 0x500;
		}
	}
	if (arg_carState->car_speed2 > core.game_topSpeed)
		core.game_topSpeed = arg_carState->car_speed2;
}

void update_grip(struct CARSTATE *car, struct SIMD *simd, int isOpponent)
{
    int speed;
    unsigned int demandedGrip;
    int totalGrip;
    int scratch;
    unsigned char column;
    int baseAngleValue;
    unsigned char rowCoord;
    int limit;

    if (car->car_sumSurfAllWheels == 0) {
        car->car_40MfrontWhlAngle = 0;
        car->car_slidingFlag = 0;
        return;
    }

    scratch = 0;
    if (car->car_surfaceWhl[0] == 4)
        scratch++;
    if (car->car_surfaceWhl[1] == 4)
        scratch++;
    if (car->car_surfaceWhl[2] == 4)
        scratch++;
    if (car->car_surfaceWhl[3] == 4)
        scratch++;

    if (scratch != 0) {
        car->car_speed2 -= car->car_speed2 / speed_recovery_divisors[scratch];
        car->car_speed = car->car_speed2;
    }

    baseAngleValue = car->car_steeringAngle + car->car_36MwhlAngle;
    limit = baseAngleValue;
    speed = car->car_speed >> 8;

    scratch = (limit < 0 ? -limit : limit) >> 3;

    demandedGrip = (((unsigned int)speed * (unsigned int)speed) >> 6) * (unsigned int)scratch;
    totalGrip = simd->grip * 2;
    totalGrip = (int)(((long)(simd->sliding[car->car_surfaceWhl[0]] +
                                simd->sliding[car->car_surfaceWhl[1]] +
                                simd->sliding[car->car_surfaceWhl[2]] +
                                simd->sliding[car->car_surfaceWhl[3]]) *
                         totalGrip) >> 10);

    car->car_demandedGrip = demandedGrip;
    car->car_surfacegrip_sum = totalGrip;

    if (isOpponent != 0) {
        if (car->car_steeringAngle == 0) {
            scratch = (unsigned char)car->car_rotate.x;
            if (scratch > 127)
                scratch -= 256;
            if (scratch != 0) {
                if ((scratch < 0 ? -scratch : scratch) < 8) {
                    if (scratch > 0)
                        car->car_rotate.x--;
                    else
                        car->car_rotate.x++;
                }
            }
        }

        if (totalGrip < (int)demandedGrip) {
            car->car_slidingFlag = 1;
            limit = (int)(((long)totalGrip << 8) /
                                  ((long)speed * speed));
            if (baseAngleValue < 0)
                limit = -1 * limit;
            limit = (limit * 3 + baseAngleValue) >> 2;
            car->field_42 = baseAngleValue - limit;
        } else {
            car->car_slidingFlag = 0;
            if (car->field_42 != 0) {
                car->field_42 -= car->field_42 >> 4;
                if ((car->field_42 < 0 ? -car->field_42 : car->field_42) < 16)
                    car->field_42 >>= 1;
            }
        }

        if (car->car_angle_z == 0 && car->car_crashBmpFlag != 1)
            car->car_40MfrontWhlAngle = limit;
        else
            car->car_40MfrontWhlAngle = 0;

        if (car->car_rotate.z != 0 &&
            (car->car_rotate.z < 0 ? -car->car_rotate.z : car->car_rotate.z) > 4) {
                column = (unsigned char)((unsigned long)car->car_posWorld1.lx >> 16);
                rowCoord = (unsigned char)((unsigned long)car->car_posWorld1.lz >> 16);

                switch (td14tb[gterrtrk[rowCoord] + column]) {
                case 0xFD:
                    column--;
                    rowCoord++;
                    break;
                case 0xFE:
                    rowCoord++;
                    break;
                case 0xFF:
                    column--;
                    break;
                }

                switch (td14tb[gterrtrk[rowCoord] + column]) {
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                    car->car_40MfrontWhlAngle += car->car_rotate.z / 5;
                    break;
                }
        }

        if (totalGrip + 1000 < (int)demandedGrip) {
            car->car_angle_z += (limit - baseAngleValue) / 14;
            car->car_angle_z /= 2;
        } else if (car->car_angle_z != 0) {
            car->car_angle_z += (limit - baseAngleValue) / 14;
            car->car_angle_z /= 2;
            if (car->car_angle_z == 0) {
                car->car_speed2 = mulscl(
                    cosfast(car->car_36MwhlAngle), car->car_speed2);
                if (cosfast(car->car_36MwhlAngle) < 0)
                    car->car_speed2 = 0;
                car->car_36MwhlAngle = 0;
            }
        }
    } else {
        car->car_40MfrontWhlAngle = car->car_steeringAngle * 4;
        if (car->car_angle_z != 0)
            car->car_angle_z = (car->car_angle_z * 15) >> 4;
    }

    if (car->car_36MwhlAngle != 0 && car->car_angle_z == 0)
        car->car_36MwhlAngle = (car->car_36MwhlAngle * 15) >> 4;
    if (car->car_angle_z != 0)
        car->car_36MwhlAngle -= car->car_angle_z;

    if (car->car_slidingFlag != 0) {
        scratch = (car->field_42 < 0 ? -car->field_42 : car->field_42) << 1;
        if (car->car_speed > scratch) {
            if (car->car_speed2 > scratch) {
                car->car_speed -= scratch;
                car->car_speed2 -= scratch;
            } else {
                car->car_speed = 0;
                car->car_speed2 = 0;
            }

            if (car->car_crashBmpFlag == 0) {
                if (car->car_surfaceWhl[0] == 1 ||
                    car->car_surfaceWhl[1] == 1 ||
                    car->car_surfaceWhl[2] == 1 ||
                    car->car_surfaceWhl[3] == 1)
                    car->field_CF |= 2;
                else
                    car->field_CF |= 4;
            }
        } else {
            car->car_speed = 0;
            car->car_speed2 = 0;
        }
    }

    car->field_42 = 0;
}

char car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent)
{
    short pHeading;
    short opponentHeading;
    int speedPenalty;
    short playerSin;
    short playerCosAngle;
    short opponentSin;
    short hitForce;
    unsigned short opponentSpeed;
    int distanceToCar;
    short turnDifference;
    unsigned short currentSpeed;
    short opponentCosAng;

    player->field_C8 = 1;
    opponent->field_C8 = 1;
    currentSpeed = player->car_speed2;
    opponentSpeed = opponent->car_speed2;
    pHeading = player->car_rotate.x;
    opponentHeading = opponent->car_rotate.x;
    playerSin = mulscl(currentSpeed >> 8, sinfast(pHeading));
    opponentSin = mulscl(opponentSpeed >> 8, sinfast(opponentHeading));
    playerCosAngle = mulscl(currentSpeed >> 8, cosfast(pHeading));
    opponentCosAng = mulscl(opponentSpeed >> 8, cosfast(opponentHeading));
    distanceToCar = polradius2d(opponentSin - playerSin, opponentCosAng - playerCosAngle);
    if (distanceToCar < 10)
        distanceToCar = 10;
    turnDifference = (pHeading - opponentHeading) & 0x3FF;
    hitForce = distanceToCar << 8;
    speedPenalty = (0x300 * distanceToCar) >> 2;
    if (player->car_speed2 < speedPenalty)
        player->car_speed2 = 0;
    else
        player->car_speed2 -= speedPenalty;

    player->car_36MwhlAngle = opponentHeading - pHeading;
    if (player->car_36MwhlAngle >= 0x200)
        player->car_36MwhlAngle -= 0x400;
    if (player->car_36MwhlAngle <= -0x200)
        player->car_36MwhlAngle += 0x400;

    opponent->car_36MwhlAngle = pHeading - opponentHeading;
    if (opponent->car_36MwhlAngle >= 0x200)
        opponent->car_36MwhlAngle -= 0x400;
    if (opponent->car_36MwhlAngle <= -0x200)
        opponent->car_36MwhlAngle += 0x400;

    player->car_speed = player->car_speed2;
    opponent->car_speed = opponent->car_speed2;
    return distanceToCar > 30;
}

int carState_rc_op(struct CARSTATE *car, int value, int wheel)
{
    short oldRc;
    short target;
    short adjustment;

    oldRc = car->car_rc2[wheel];
    target = 0;
    adjustment = 0;
    if (car->car_rc5[wheel] != 0) {
        if (car->car_rc5[wheel] < 0) {
            car->car_rc5[wheel] += 4;
            if (car->car_rc5[wheel] > adjustment)
                car->car_rc5[wheel] = adjustment;
        } else {
            car->car_rc5[wheel] -= 4;
            if (car->car_rc5[wheel] < adjustment)
                car->car_rc5[wheel] = adjustment;
        }
    }
    car->car_rc5[wheel] = car->car_rc5[wheel];

    if (value < 0 && car->car_rc2[wheel] > -value)
        value = 0;
    if (value == 0) {
        if (car->car_rc2[wheel] > car->car_rc5[wheel]) {
            car->car_rc2[wheel] -= 0x80;
            if (car->car_rc2[wheel] < car->car_rc5[wheel])
                car->car_rc2[wheel] = car->car_rc5[wheel];
            target = oldRc - car->car_rc2[wheel];
        } else if (car->car_rc2[wheel] < car->car_rc5[wheel]) {
            car->car_rc2[wheel] += 0x80;
            if (car->car_rc2[wheel] > car->car_rc5[wheel])
                car->car_rc2[wheel] = car->car_rc5[wheel];
        }
    } else if (value > 0) {
        if (value > 0xC0)
            car->car_rc2[wheel] += 0xC0;
        else
            car->car_rc2[wheel] += value;
        if (car->car_rc2[wheel] > 0x180)
            car->car_rc2[wheel] = 0x180;
        car->car_rc4[wheel] = 0;
    } else {
        if (value + car->car_rc2[wheel] > -0x120) {
            car->car_rc2[wheel] += value;
        } else {
            car->car_rc2[wheel] += (value * 3) >> 2;
            if (car->car_rc2[wheel] < -0x180)
                car->car_rc2[wheel] = -0x180;
        }
        target = oldRc - car->car_rc2[wheel] + value;
    }
    return oldRc + target;
}

/* Purpose: Updates steering response for the selected frame rate.
 * Parameters: input.
 * Returns: none.
 * Globals read: core, rate_frame, table_lookup
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void upd_statef20_from_steer_input(char input)
{
    register int response;
    char responseIndex;
    register int oldAngle;

    oldAngle = core.playerstate.car_steeringAngle;
    responseIndex = (char)(((unsigned short)core.playerstate.car_speed2 >> 10) & 0xFC);
    response = table_lookup[(char)responseIndex + input];

    if (response > 0) {
        if (oldAngle < -1) response <<= 2;
    } else if (response != 0 && oldAngle > 1) {
        response <<= 2;
    }

    if (response == 0 && core.playerstate.car_speed2 != 0 && oldAngle != 0) {
        response = table_lookup[(char)responseIndex + 1] * 2;
        if ((oldAngle < 0 ? -oldAngle : oldAngle) > response) {
            if (oldAngle > 0) response = -response;
        } else {
            response = -core.playerstate.car_steeringAngle;
        }
    }

    if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
        if (response > 160) response = 160;
        if (response < -160) response = -160;
    } else {
        if (response > 80) response = 80;
        if (response < -80) response = -80;
    }
    oldAngle += response;
    if (oldAngle > 240) oldAngle = 240;
    if (oldAngle < -240) oldAngle = -240;

    if (table_lookup[(char)responseIndex + input] == 0 &&
        abs(oldAngle) < 8)
        oldAngle = 0;
    core.playerstate.car_steeringAngle = oldAngle;
}

/* Purpose: Updates engine audio from the current car state.
 * Parameters: none.
 * Returns: none.
 * Globals read: audio_frmarr, audiooppflag, cammd, core, follow_op, g_player_sound_id,
 *            g_plyr_snd_state, globalgamesettings, inrepflg, op_eng_sound_id,
 *            replay_state_cache, sndpendingstate, sndposrecord, trkptrpath, viewyshift
 * Globals written: audiooppflag, g_audio_frms_ix, g_plyr_snd_state, replay_state_cache,
 *            sndpendingstate, sndposrecord
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void audio_carstate(void)
{
    struct AUDIO_CAR_FRAME *audioRecord;
    struct VECTOR playerPosition;
    struct VECTOR opponentCurrent;
    struct VECTOR targetNow;
    short carIndex;
    struct VECTOR playerPosOld;
    char soundMode;
    short carCount;
    short audioId;
    struct CARSTATE *selectedCar;
    struct VECTOR targetPast;
    struct VECTOR opponentPrior;

    if (inrepflg != 0) {
        if (sndpendingstate != 0) {
            g_audio_frms_ix = sndposrecord;
            if ((g_plyr_snd_state & 6) != 0)
                /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk7(g_player_sound_id);
            if ((g_plyr_snd_state & 1) != 0)
                /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2(g_player_sound_id);
            if (globalgamesettings.game_opponenttype != 0) {
                if ((audiooppflag & 6) != 0)
                    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk7(op_eng_sound_id);
                if ((audiooppflag & 1) != 0)
                    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2(op_eng_sound_id);
            }
            sndpendingstate = 0;
            g_plyr_snd_state = 0;
            audiooppflag = 0;
        }
        if (inrepflg == replay_state_cache)
            goto audio_done;
        reset_audio_driver_state();
        goto audio_done;
    }

    playerPosOld.x = (short)(core.playerstate.car_posWorld2.lx >> 6);
    playerPosOld.y = (short)(core.playerstate.car_posWorld2.ly >> 6);
    playerPosOld.z = (short)(core.playerstate.car_posWorld2.lz >> 6);
    playerPosition.x = (short)(core.playerstate.car_posWorld1.lx >> 6);
    playerPosition.y = (short)(core.playerstate.car_posWorld1.ly >> 6);
    playerPosition.z = (short)(core.playerstate.car_posWorld1.lz >> 6);
    if (globalgamesettings.game_opponenttype != 0) {
        opponentPrior.x = (short)(core.opponentstate.car_posWorld2.lx >> 6);
        opponentPrior.y = (short)(core.opponentstate.car_posWorld2.ly >> 6);
        opponentPrior.z = (short)(core.opponentstate.car_posWorld2.lz >> 6);
        opponentCurrent.x = (short)(core.opponentstate.car_posWorld1.lx >> 6);
        opponentCurrent.y = (short)(core.opponentstate.car_posWorld1.ly >> 6);
        opponentCurrent.z = (short)(core.opponentstate.car_posWorld1.lz >> 6);
    }

    switch (cammd) {
    case 0:
    case 2:
        if (follow_op != 0) {
            targetNow = opponentCurrent;
            targetPast = opponentPrior;
        } else {
            targetNow = playerPosition;
            targetPast = playerPosOld;
        }
        break;
    case 1:
        targetNow = core.game_vec1[follow_op];
        targetPast = core.game_vec3[follow_op];
        break;
    case 3:
        targetNow.x = ((struct VECTOR far *)trkptrpath)[core.field_3F7[follow_op]].x;
        targetNow.y = ((struct VECTOR far *)trkptrpath)[core.field_3F7[follow_op]].y + viewyshift + 90;
        targetNow.z = ((struct VECTOR far *)trkptrpath)[core.field_3F7[follow_op]].z;
        targetPast = targetNow;
        break;
    }

    audioRecord = &((struct AUDIO_CAR_FRAME *)audio_frmarr)[sndposrecord];
    audioRecord->player_offsets[0] = targetPast.x - playerPosOld.x;
    audioRecord->player_offsets[1] = targetPast.y - playerPosOld.y;
    audioRecord->player_offsets[2] = targetPast.z - playerPosOld.z;
    audioRecord->player_offsets[3] = targetNow.x - playerPosition.x;
    audioRecord->player_offsets[4] = targetNow.y - playerPosition.y;
    audioRecord->player_offsets[5] = targetNow.z - playerPosition.z;
    audioRecord->player_rpm = core.playerstate.car_currpm;

    if (globalgamesettings.game_opponenttype != 0) {
        audioRecord->opponent_offsets[0] = targetPast.x - opponentPrior.x;
        audioRecord->opponent_offsets[1] = targetPast.y - opponentPrior.y;
        audioRecord->opponent_offsets[2] = targetPast.z - opponentPrior.z;
        audioRecord->opponent_offsets[3] = targetNow.x - opponentCurrent.x;
        audioRecord->opponent_offsets[4] = targetNow.y - opponentCurrent.y;
        audioRecord->opponent_offsets[5] = targetNow.z - opponentCurrent.z;
        audioRecord->opponent_rpm = core.opponentstate.car_currpm;
        carCount = 2;
    } else {
        carCount = 1;
    }

    for (carIndex = 0; carIndex < carCount; carIndex++) {
        if (carIndex != 0) {
            selectedCar = &core.opponentstate;
            audioId = op_eng_sound_id;
            soundMode = audiooppflag;
        } else {
            selectedCar = &core.playerstate;
            audioId = g_player_sound_id;
            soundMode = g_plyr_snd_state;
        }
        if (selectedCar->field_CF & 1) {
            if (!(soundMode & 1)) {
                soundMode |= 1;
                /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk(audioId);
            }
        } else if (soundMode & 1) {
            soundMode--;
            /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2(audioId);
        }
        if (selectedCar->field_CF & 6) {
            if ((soundMode & 6) != (selectedCar->field_CF & 6)) {
                if (soundMode & 6)
                    goto stop_skid;
                if (selectedCar->field_CF & 2) {
                    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk5(audioId);
                    soundMode += 2;
                } else {
                    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk6(audioId);
                    soundMode += 4;
                }
            }
        } else if (soundMode & 6) {
stop_skid:
            if (soundMode & 2)
                soundMode -= 2;
            if (soundMode & 4)
                soundMode -= 4;
            /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk7(audioId);
        }
        if (carIndex != 0)
            audiooppflag = soundMode;
        else
            g_plyr_snd_state = soundMode;
    }
    sndpendingstate = 1;
    sndposrecord++;
    if (sndposrecord == 40)
        sndposrecord = 0;

audio_done:
    replay_state_cache = inrepflg;
}

/* Purpose: Dispatches pending audio events selected by the state flags.
 * Parameters: flags, audioId.
 * Returns: none.
 * Globals read: sndpendingstate
 * Globals written: none detected
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void audio_unk3(char flags, short audioId) { if (sndpendingstate != 0) { if (flags & 0x10) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk4(audioId); if (flags & 0x20) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk3(audioId); } }

/* Purpose: Applies a recorded audio frame to the player and opponent.
 * Parameters: record, value.
 * Returns: none.
 * Globals read: g_player_sound_id, globalgamesettings, op_eng_sound_id
 * Globals written: none detected
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void apply_audio_frame(struct AUDIO_CAR_FRAME *record, short value)
{
    /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk2(g_player_sound_id, record->player_rpm,
        record->player_offsets[0], record->player_offsets[1],
        record->player_offsets[2], record->player_offsets[3],
        record->player_offsets[4], record->player_offsets[5], value);
    if (globalgamesettings.game_opponenttype != 0)
        /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk2(op_eng_sound_id, record->opponent_rpm,
            record->opponent_offsets[0], record->opponent_offsets[1],
            record->opponent_offsets[2], record->opponent_offsets[3],
            record->opponent_offsets[4], record->opponent_offsets[5], value);
}

char track_edge_points(int trackIndex, struct TRACKRESULT *result, char side,
              char *opponentSpeed)
{
    unsigned char entry;
    struct TRKOBJINFO far *objectInfo;
    struct VECTOR vectorA;
    unsigned char edge;
    short rotateTemp;
    struct TRKOBJINFO far *infoBase;
    unsigned char scratch;
    unsigned char arrow;
    unsigned char trackRow;
    struct VECTOR far *edgeData;
    struct VECTOR far *trackData;
    unsigned char isConnected;
    struct TRACKOBJECT *trackElemObject;
    int linkPresent;
    struct VECTOR edgePoint;
    unsigned char objectIndex;

    entry = (unsigned char)road_trk[trackIndex];
    objectIndex = (unsigned char)td_18_ref[trackIndex] & 0x0f;
    isConnected = (unsigned char)td_18_ref[trackIndex] & 0x10;
    trackElemObject = &trklst[entry];
    infoBase = (struct TRKOBJINFO far *)trackElemObject->ss_trkObjInfoPtr;
    objectInfo = infoBase + objectIndex;
    linkPresent = 0;
    arrow = (unsigned char)objectInfo->si_arrowType;

    if (isConnected == 0)
        edge = (unsigned char)(side * 2);
    else
        edge = (unsigned char)((arrow - side) * 2 - 2);

    if (opponentSpeed != 0) {
        scratch = (unsigned char)objectInfo->si_oppSpedCode;
        trackRow = (unsigned char)trackElemObject->ss_surfaceType;
        *opponentSpeed = opponent_spd_tbl[trackRow + scratch];
    }

    if (objectInfo->link.dataPointer != 0)
        linkPresent = 1;

    if (isConnected != 0) {
        if (linkPresent != 0) {
            trackData = (struct VECTOR far *)objectInfo->link.dataPointer;
            goto forward;
        }
        trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset;
        edgeData = trackData + edge;
        vectorA = edgeData[1];
        edgePoint = edgeData[0];
    } else {
        trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset;
forward:
        edgeData = trackData + edge;
        vectorA = edgeData[0];
        edgePoint = edgeData[1];
    }

    switch (objectInfo->si_arrowOrient) {
    case 0x300:
        rotateTemp = vectorA.x;
        vectorA.x = -vectorA.z;
        vectorA.z = rotateTemp;
        rotateTemp = edgePoint.x;
        edgePoint.x = -edgePoint.z;
        edgePoint.z = rotateTemp;
        break;
    case 0x200:
        vectorA.z = -vectorA.z;
        vectorA.x = -vectorA.x;
        edgePoint.z = -edgePoint.z;
        edgePoint.x = -edgePoint.x;
        break;
    case 0x100:
        rotateTemp = vectorA.x;
        vectorA.x = vectorA.z;
        vectorA.z = -rotateTemp;
        rotateTemp = edgePoint.x;
        edgePoint.x = edgePoint.z;
        edgePoint.z = -rotateTemp;
        break;
    }

    scratch = (unsigned char)g_column_of_trkdata21_pth[trackIndex];
    trackRow = (unsigned char)tdfrompathrow22[trackIndex];
    if (vectorA.y != -1 && td15p_9[gterrtrk[trackRow] + scratch] == 6) {
        vectorA.y += hillconsts[1];
        edgePoint.y += hillconsts[1];
    }

    if ((trackElemObject->ss_multiTileFlag & 1) != 0) {
        vectorA.z += r_zp[trackRow];
        edgePoint.z += r_zp[trackRow];
    } else {
        vectorA.z += row_ctr_zs[trackRow];
        edgePoint.z += row_ctr_zs[trackRow];
    }
    if ((trackElemObject->ss_multiTileFlag & 2) != 0) {
        vectorA.x += xcols[scratch + 1];
        edgePoint.x += xcols[scratch + 1];
    } else {
        vectorA.x += trackctrpos2[scratch];
        edgePoint.x += trackctrpos2[scratch];
    }

    result->center.x = ((long)vectorA.x + edgePoint.x) >> 1;
    if (vectorA.y == -1)
        result->center.y = -1;
    else
        result->center.y = ((long)vectorA.y + edgePoint.y) >> 1;
    result->center.z = ((long)vectorA.z + edgePoint.z) >> 1;
    result->edge_a = vectorA;
    result->edge_b = edgePoint;
    result->has_opponent_link = (short)linkPresent;

    return (int)(arrow - 1) == side;
}

char car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds)
{
    register int reach;
    struct MATRIX *matrix;
    char index;
    struct VECTOR point;
    struct VECTOR transform[4];
    struct VECTOR result;

    reach = pCollPoints[1].py + oCollPoints[1].py;
    if ((pWorldCrds[0].x - oWorldCrds[0].x < 0 ? -(pWorldCrds[0].x - oWorldCrds[0].x) : pWorldCrds[0].x - oWorldCrds[0].x) > reach ||
        (pWorldCrds[0].z - oWorldCrds[0].z < 0 ? -(pWorldCrds[0].z - oWorldCrds[0].z) : pWorldCrds[0].z - oWorldCrds[0].z) > reach ||
        (pWorldCrds[0].y - oWorldCrds[0].y < 0 ? -(pWorldCrds[0].y - oWorldCrds[0].y) : pWorldCrds[0].y - oWorldCrds[0].y) > reach)
        return 0;

    result.x = pWorldCrds[0].x - oWorldCrds[0].x;
    result.y = pWorldCrds[0].y - oWorldCrds[0].y;
    result.z = pWorldCrds[0].z - oWorldCrds[0].z;
    if ((unsigned)polarRadius3D(&result) > (unsigned)reach)
        return 0;

    matrix = matrotzxy(-pWorldCrds[1].x, -pWorldCrds[1].y,
                         -pWorldCrds[1].z, 0);
    for (index = 0; index < 4; ++index) {
        if (collision_point_x_signs[index] == 0)
            point.x = pCollPoints[0].px;
        else
            point.x = -pCollPoints[0].px;
        point.y = 0;
        if (collision_point_y_signs[index] == 0)
            point.z = pCollPoints[1].px;
        else
            point.z = -pCollPoints[1].px;
        mat_vec(&point, matrix, &result);
        result.x += pWorldCrds[0].x;
        result.y += pWorldCrds[0].y;
        result.z += pWorldCrds[0].z;
        transform[index] = result;
    }

    matrix = matrotzxy(oWorldCrds[1].x, oWorldCrds[1].y,
                         oWorldCrds[1].z, 1);
    for (index = 0; index < 4; ++index) {
        point.x = oWorldCrds[0].x - transform[index].x;
        point.y = oWorldCrds[0].y - transform[index].y;
        point.z = oWorldCrds[0].z - transform[index].z;
        mat_vec(&point, matrix, &result);
        if (!(result.y >= 0 && result.y <= oCollPoints[0].py &&
            result.x >= -oCollPoints[0].px && result.x <= oCollPoints[0].px &&
            result.z >= -oCollPoints[1].px && result.z <= oCollPoints[1].px))
            continue;
        return 1;
    }

    matrix = matrotzxy(-oWorldCrds[1].x, -oWorldCrds[1].y,
                         -oWorldCrds[1].z, 0);
    for (index = 0; index < 4; ++index) {
        if (collision_point_x_signs[index] == 0)
            point.x = oCollPoints[0].px;
        else
            point.x = -oCollPoints[0].px;
        point.y = 0;
        if (collision_point_y_signs[index] == 0)
            point.z = oCollPoints[1].px;
        else
            point.z = -oCollPoints[1].px;
        mat_vec(&point, matrix, &result);
        result.x += oWorldCrds[0].x;
        result.y += oWorldCrds[0].y;
        result.z += oWorldCrds[0].z;
        transform[index] = result;
    }

    matrix = matrotzxy(pWorldCrds[1].x, pWorldCrds[1].y,
                         pWorldCrds[1].z, 1);
    for (index = 0; index < 4; ++index) {
        point.x = pWorldCrds[0].x - transform[index].x;
        point.y = pWorldCrds[0].y - transform[index].y;
        point.z = pWorldCrds[0].z - transform[index].z;
        mat_vec(&point, matrix, &result);
        if (!(result.y >= 0 && result.y <= pCollPoints[0].py &&
            result.x >= -pCollPoints[0].px && result.x <= pCollPoints[0].px &&
            result.z >= -pCollPoints[1].px && result.z <= pCollPoints[1].px))
            continue;
        return 1;
    }
    return 0;
}

void init_plantrak(void)
{
    register short zeroValue;
    initialize_game_state(-3);
    zeroValue = 0;
    core.game_inputmode = 2;
    g_planlist = plan_memres;
    idxtrk = 1;
    tagtrk = 28;
    road_trk[0] = 7; g_column_of_trkdata21_pth[0] = 1; tdfrompathrow22[0] = tagtrk; td_18_ref[0] = 0;
    road_trk[1] = 6; g_column_of_trkdata21_pth[1] = 0; tdfrompathrow22[1] = tagtrk; td_18_ref[1] = 0;
    road_trk[2] = 8; g_column_of_trkdata21_pth[2] = 0; tdfrompathrow22[2] = tagtrk + 1; td_18_ref[2] = 0;
    road_trk[3] = 9; g_column_of_trkdata21_pth[3] = 1; tdfrompathrow22[3] = tagtrk + 1; td_18_ref[3] = 0;
    road_trk[4] = 7; g_column_of_trkdata21_pth[4] = 1; tdfrompathrow22[4] = tagtrk; td_18_ref[4] = 0;
    ((short far *)td3)[0] = zeroValue;
    ((short far *)td3)[1] = 1; ((short far *)td3)[2] = 2; ((short far *)td3)[3] = 3; ((short far *)td3)[4] = 4;
    ((short far *)td3)[5] = 1; ((short far *)td3)[6] = 2; ((short far *)td3)[7] = 3; ((short far *)td3)[8] = 4;
    ((short far *)td3)[9] = 1; ((short far *)td3)[10] = 2; ((short far *)td3)[11] = 3; ((short far *)td3)[12] = 4;
    ((short far *)td3)[13] = zeroValue; ((short far *)td3)[14] = 1; ((short far *)td3)[15] = 2; ((short far *)td3)[16] = 3; ((short far *)td3)[17] = zeroValue;
    opponent_spd_tbl[0] = 0xC8;
    init_carstate_from_simd(&core.opponentstate, &ophys_7, 1,
        0x17700L, 0L, ((long)(r_zp[28] + 0x12E)) << 6, 0);
    track_edge_points(((short far *)td3)[core.opponentstate.car_trackdata3_index],
        &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
        &core.field_3F9);
}

void do_opponent_op(void) { opponent_op(); }

/* Purpose: Applies crash transitions and updates engine audio state.
 * Parameters: arg_someFlag, arg_MplayerFlag.
 * Returns: none.
 * Globals read: core, elaptm1, endhsdemo, g_player_sound_id, inrepflg, op_eng_sound_id,
 *            rate_frame, sndpendingstate
 * Globals written: core, race_stats
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_crash_state(int arg_someFlag, int arg_MplayerFlag) {
	char suppress_car_speed;
	struct CARSTATE* var_cState;

	switch (arg_MplayerFlag) {
	case 0:
		var_cState = &core.playerstate;
		break;
	case 1:
		var_cState = &core.opponentstate;
		break;
	}
	if (var_cState->car_crashBmpFlag != 0)
		return;

	suppress_car_speed = 0;
	switch (arg_someFlag) {
	case 4:
		core.game_frame_in_sec = 1;
		core.game_frames_per_sec = 1;
		break;
	case 5:
		arg_someFlag = 1;
		suppress_car_speed = 1;
	case 1:
		var_cState->car_crashBmpFlag = 1;
		state_op_unk(arg_MplayerFlag, var_cState->car_rotate.x, 0);
		if (arg_MplayerFlag == 0) {
			core.game_impactSpeed = var_cState->car_speed2;
			core.game_frames_per_sec = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
		}
		if (inrepflg == 0 && sndpendingstate != 0) {
			if (arg_MplayerFlag == 0)
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(g_player_sound_id);
			else
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(op_eng_sound_id);
		}
		break;
	case 2:
		if (inrepflg == 0 && sndpendingstate != 0) {
			if (arg_MplayerFlag == 0)
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(g_player_sound_id);
			else
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(op_eng_sound_id);
		}
		var_cState->car_crashBmpFlag = 2;
		suppress_car_speed = 1;
		if (arg_MplayerFlag == 0) {
			core.game_impactSpeed = var_cState->car_speed2;
			core.game_frames_per_sec = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
		}
		break;
	case 3:
		var_cState->car_crashBmpFlag = 3;
		if (arg_MplayerFlag == 0) {
			core.game_total_finish = core.game_frame + core.game_penalty + elaptm1;
			core.game_frames_per_sec = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
		} else {
			core.field_144 = core.game_frame + elaptm1;
		}
		break;
	}
	if (suppress_car_speed != 0) {
		var_cState->car_speed2 = 0;
		var_cState->car_speed = 0;
	}
	if (arg_MplayerFlag != 0)
		core.game_oEndFrame = core.game_frame;
	else
		core.game_pEndFrame = core.game_frame;
	if (core.game_3F6autoLoadEvalFlag == 0 && arg_MplayerFlag == 0)
		core.game_3F6autoLoadEvalFlag = arg_someFlag;
	if ((endhsdemo & 4) == 0)
		race_stats = *(struct GAMESTATE_SNAPSHOT *)&core.game_travDist;
}

void plnrotop(void) {
	struct VECTOR rotatedVector;
	struct MATRIX rotationMatrix;
	struct MATRIX matrix;
	struct VECTOR vector;
	register int rotation;

	if (g_planidx2 != -1) {
		if (g_planlist[g_planidx2].plane_xy == car_roty_pln &&
		    g_planlist[g_planidx2].plane_yz == pln_rotate_z) {
			rotation = car_rotate_xc;
		} else {
			mat_vec(&tvec2, &matrix_transform_view, &vector);
			matrix = g_planlist[g_planidx2].plane_rotation;
			mat_invert(&matrix, &rotationMatrix);
			mat_vec(&vector, &rotationMatrix, &rotatedVector);
			rotation = polang(-rotatedVector.x, rotatedVector.z);
		}
		if ((rotation += frwhl_angadjusted) != 0) {
			if (last_track_rotation != rotation) {
				matroty(&plane_rotation_cache, -rotation);
				last_track_rotation = rotation;
			}
			mat_vec(&tvec2, &plane_rotation_cache, &rotatedVector);
			mat_mul_vector2(&rotatedVector, &g_planlist[g_planidx2].plane_rotation, &pln_rot_output);
			return;
		}
		mat_mul_vector2(&tvec2, &g_planlist[g_planidx2].plane_rotation, &pln_rot_output);
		return;
	}
	if (frwhl_angadjusted != 0) {
		if (frwhl_angadjusted != f36f40_whlData) {
			matroty(&fallback_rotation_cache, -frwhl_angadjusted);
			f36f40_whlData = frwhl_angadjusted;
		}
		mat_vec(&tvec2, &fallback_rotation_cache, &rotatedVector);
		mat_vec(&rotatedVector, &matrix_transform_view, &pln_rot_output);
		return;
	}
	mat_vec(&tvec2, &matrix_transform_view, &pln_rot_output);
}

int plnoriginop(int arg_planindex, int x, int y, int z) {
	struct PLANE far* pPlane;
	struct VECTOR a;
	struct VECTOR b;
	
	if (arg_planindex == pl_i) {
		pPlane = plncurrptr;
	} else {
		pPlane = &g_planlist[arg_planindex];
	}

	b.y = pPlane->plane_origin.y + hgthgt;
	a.y = y - b.y;
	if (arg_planindex < 4) {
		
		return a.y;
	}
	b.x = pPlane->plane_origin.x + x_course_part;
	b.z = pPlane->plane_origin.z + road_elem_ctrz;
	a.x = x - b.x;
	a.z = z - b.z;
	return vec_normalInnerProduct(a.x, a.y, a.z, &pPlane->plane_normal);
}

int vec_normalInnerProduct(int x, int y, int z, struct VECTOR far *normal)
{
    return (((long)normal->x * x) + ((long)normal->y * y) +
            ((long)normal->z * z)) / 0x2000;
}

void state_op_unk(int mode, short angle, short speed)
{
    int speedFactor;
    register int debrisIndex;
    int startAngle;
    int verticalStep;
    int made;
    register short verticalValue;
    int countLimit;
    int phaseBase;
    int unusedCount;

    if (mode < 2) {
        startAngle = angle;
        speedFactor = 0x400;
        countLimit = 0x12;
        phaseBase = mode * 4 + 4;
        verticalStep = 6;
    } else {
        startAngle = angle - 0x60;
        speedFactor = 0xC0;
        countLimit = 8;
        phaseBase = 0;
        verticalStep = 1;
    }
    core.field_42A = 1;
    unusedCount = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex)
        if (core.field_38E[debrisIndex] == 0)
            ++unusedCount;
    if (unusedCount > countLimit) unusedCount = countLimit;

    made = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex) {
        if (core.field_38E[debrisIndex] == 0) {
            core.field_443[debrisIndex] = (char)mode;
            core.field_42B[debrisIndex] = (char)((made & 3) + phaseBase);
            core.game_longs1[debrisIndex] = 0;
            core.game_longs2[debrisIndex] = 0;
            core.game_longs3[debrisIndex] = 0;
            core.field_2FE[debrisIndex] = (short)(get_kevinrandom() << 2);
            core.field_32E[debrisIndex] = (short)(get_kevinrandom() << 2);
            core.field_35E[debrisIndex] = (short)((((long)speedFactor * made) / unusedCount + startAngle) & 0x3FF);
            verticalValue = (short)(((get_kevinrandom() * 6) >> 2) + speed + 0x180);
            core.field_38E[debrisIndex] = verticalValue;
            core.field_3BE[debrisIndex] = (short)((verticalStep * verticalValue) >> 2);
            if (++made == unusedCount)
                break;
        } else
            continue;
    }
}

/* Purpose: Updates crash debris using frame-rate dependent timing.
 * Parameters: none.
 * Returns: none.
 * Globals read: core, rate_frame
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_crash_debris(void)
{
    struct MATRIX *matrixPointer;
    char keepAlive = 0;
    register int i;
    for (i = 0; i < 24; ++i) {
        if (core.field_38E[i] != 0) {
            struct VECTOR inputVector;
            struct VECTOR outputVector;
            matrixPointer = matrotzxy(0, 0, core.field_35E[i], 1);
            inputVector.x = 0;
            inputVector.y = 0;
            inputVector.z = core.field_38E[i];
            mat_vec(&inputVector, matrixPointer, &outputVector);
            core.game_longs1[i] += outputVector.x;
            core.game_longs3[i] += outputVector.z;
            core.field_3BE[i] -= 0x13;
            core.game_longs2[i] += core.field_3BE[i];
            if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
                core.field_3BE[i] -= 0x13;
                core.game_longs2[i] += core.field_3BE[i];
            }
            if (core.playerstate.car_posWorld1.ly + core.game_longs2[i] < 0) {
                core.field_38E[i] = 0;
            } else {
                keepAlive = 1;
                core.field_2FE[i] += 0x10;
                core.field_32E[i] += 0x10;
            }
        }
    }
    core.field_42A = keepAlive;
}
extern char far *locate_shape_alt(char far *data, char *name);
extern void copy_string(char *destination, char far *source);
extern char textstr[];
extern char g_gsnashape_data[];

void setup_aero_trackdata(void far *carresptr, int is_opponent)
{
    register int i;
    if (is_opponent == 0) {
        simdp7 = *(struct SIMD far *)locate_shape_alt(carresptr, "simd");
        simdp7.aerorestable = track04_plyraero;
        for (i = 0; i < 0x40; i++) {
            track04_plyraero[i] = ((long)simdp7.aero_resistance * (long)i * (long)i) >> 9;
        }
        copy_string(textstr, locate_shape_alt(carresptr, "gnam"));
    } else {
        ophys_7 = *(struct SIMD far *)locate_shape_alt(carresptr, "simd");
        ophys_7.aerorestable = trackdata_05_opp_aerotbl;
        for (i = 0; i < 0x40; i++) {
            trackdata_05_opp_aerotbl[i] = ((long)ophys_7.aero_resistance * (long)i * (long)i) >> 9;
        }
        copy_string(g_gsnashape_data, locate_shape_alt(carresptr, "gsna"));
    }
}

/* Communals defined by this module (tentative definitions). */
char g_plyr_snd_state;
char audiooppflag;
int wallanchor_z;
short far* trackdata_penalty_related;
int g_player_sound_id;
int op_eng_sound_id;
struct MATRIX matrix_transform_view;
int rotxvehicle;
struct GAMESTATE core;
int yrotrotveh;
int anglerotate_car;
char idxtrk;
char g_hillf;
char tagtrk;
char pen_flag_count;
long centerpos;
long veh_position;
long veh_z;
short rate_frame;
char far* road_trk;
short sndposrecord;
char *table_lookup;
short far* track04_plyraero;
short far* trackdata_05_opp_aerotbl;
unsigned char opponent_spd_tbl[16];
struct PLANE far* plncurrptr;
short g_sgateopn;
struct VECTOR tvec2;
struct VECTOR pln_rot_output;
int pln_rotate_z;
int car_roty_pln;
int car_rotate_xc;
int frwhl_angadjusted;
int g_planidx2;
char audio_frmarr[1360];
struct PLANE far* g_planlist;
char far* td_18_ref;
short st_hdg;
char far* g_column_of_trkdata21_pth;
struct SIMD ophys_7;
char sndpendingstate;
char far* tdfrompathrow22;
int nextpos_normalip;
short g_cvxintvl;
struct GAMESTATE huge* cvxs_a;
int wall_facingang;
int trackctrpos2[30];
struct SIMD simdp7;
int row_ctr_zs[30];
int g_penaltytm;
short framerate_pad_0;
short extra_rclist4[2];
short spare_td22_1;
short fontled_free_4[2];




