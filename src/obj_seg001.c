extern short data_349D0;


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


short sin_fast(unsigned short s);
short cos_fast(unsigned short s);

int polarAngle(int z, int y);
int polarRadius2D(int z, int y);
int polarRadius3D(struct VECTOR* vec);

unsigned rect_compare_point(struct POINT2D* pt);

void mat_mul_vector(struct VECTOR* invec, struct MATRIX* mat, struct VECTOR* outvec);
void mat_multiply(struct MATRIX* rmat, struct MATRIX* lmat, struct MATRIX* outmat);
void mat_invert(struct MATRIX* inmat, struct MATRIX* outmat);
void mat_rot_x(struct MATRIX* outmat, int angle);
void mat_rot_y(struct MATRIX* outmat, int angle);
void mat_rot_z(struct MATRIX* outmat, int angle);
struct MATRIX* mat_rot_zxy(int z, int x, int y, int unk);

void rect_adjust_from_point(struct POINT2D* pt, struct RECTANGLE* rc);

int vector_op_unk2(struct VECTOR* vec);
void vector_to_point(struct VECTOR* vec, struct POINT2D* outpt);
void vector_op_unk(struct VECTOR* vec1, struct VECTOR* vec2, struct VECTOR* outvec, short i);

short multiply_and_scale(short a1, short a2);

void rect_union(struct RECTANGLE* r1, struct RECTANGLE* r2, struct RECTANGLE* outrc);
int rect_intersect(struct RECTANGLE* r1, struct RECTANGLE* r2);





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


extern struct GAMEINFO gameconfig;
extern struct GAMEINFO gameconfigcopy;

extern struct GAMESTATE state;
extern struct SIMD simd_player;
extern struct SIMD simd_opponent;

extern short video_flag1_is1;
extern short video_flag2_is1;
extern short video_flag3_isFFFF;
extern short video_flag4_is1;
extern short video_flag5_is0;
extern short video_flag6_is1;

extern unsigned char byte_44A8A;
extern unsigned char byte_4552F;
extern unsigned short elapsed_time1;
extern unsigned short word_32D02;
extern unsigned char byte_449DA;
extern unsigned char byte_4393C;
extern unsigned char game_replay_mode; 
extern short word_44DCA;

extern short word_45A24; 
extern short word_45A00; 
extern short word_4499C; 
extern short track_angle;
extern char *steerWhlRespTable_ptr;
extern char steerWhlRespTable_10fps[];
extern char steerWhlRespTable_20fps[];
extern char startcol2, startrow2;
extern char hillFlag;
extern short hillHeightConsts[];

extern struct RECTANGLE rect_windshield;
extern short word_449EA;
extern int run_game_random;
extern char replaybar_toggle;
extern char is_in_replay;
extern char cameramode;
extern char byte_449E6;
extern char game_replay_mode_copy;
extern char byte_44346;
extern char byte_46467;
extern char dashb_toggle;
extern char byte_4432A;
extern char show_penalty_counter;
extern int word_45D94;
extern int word_45D3E;
extern char byte_3B8F2;
extern char byte_3FE00;
extern void far* gameresptr;
extern void far* dasmshapeptr;
extern int word_3F88E;
extern char dashb_toggle_copy;
extern char replaybar_toggle_copy;
extern char is_in_replay_copy;
extern char followOpponentFlag;
extern char followOpponentFlag_copy;
extern int roofbmpheight_copy;
extern char byte_449E2;
extern char replaybar_enabled;
extern int dashbmp_y_copy;
extern int height_above_replaybar;
extern char byte_454A4;
extern char byte_449D8[];
extern int dastseg;
extern int dastbmp_y;
extern int dastbmp_y2;
extern int dashbmp_y;
extern int roofbmpheight;
extern struct RECTANGLE* rectptr_unk;
extern void setup_car_shapes(int);
extern void update_frame(char, struct RECTANGLE*);
extern void loop_game(int, int, int);
extern void set_frame_callback(void);
extern void mouse_minmax_position(int);
extern int kb_get_char(void);
extern void handle_ingame_kb_shortcuts(int);

extern int mouse_butstate;
extern int mouse_xpos;
extern int mouse_ypos;
extern int performGraphColor;
extern char resID_byte1;
extern int waitflag;

extern void far* fontnptr;
extern void far* fontdefptr;
extern void far* mainresptr;
extern struct GAMESTATE huge* cvxptr;
extern int trackrows[];
extern int terrainrows[];
extern int trackpos[];
extern int trackcenterpos[];
extern int terrainpos[];
extern int terraincenterpos[];
extern int trackpos2[];
extern int trackcenterpos2[];
extern short far* td01_track_file_cpy; 
extern short far* td02_penalty_related; 
extern char far* trackdata3;
extern short far* td04_aerotable_pl; 
extern short far* td05_aerotable_op; 
extern char far* trackdata6;
extern char far* trackdata7;
extern int far* td08_direction_related; 
extern int far* trackdata9;
extern struct VECTOR far* td10_track_check_rel;
extern char far* td11_highscores; 
extern char far* trackdata12;
extern char far* td13_rpl_header; 
extern unsigned char far* td14_elem_map_main; 
extern unsigned char far* td15_terr_map_main; 
extern char far* td16_rpl_buffer; 
extern char far* td17_trk_elem_ordered; 
extern char far* trackdata18;
extern unsigned char far* trackdata19;
extern char far* td20_trk_file_appnd; 
extern char far* td21_col_from_path; 
extern char far* td22_row_from_path; 
extern unsigned char far* trackdata23; 
extern char kbormouse;
extern char passed_security;
extern char g_is_busy;
extern char g_path_buf[];
extern char byte_3B80C[];
extern char idle_expired;
extern unsigned short dialogarg2;
extern char byte_3B85E[];
extern char byte_43966;
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

extern short data_349D0;
extern unsigned short framespersec2;
extern unsigned short slow_video_mgmt;
extern unsigned short slow_video_mgmt_copy;
extern unsigned char detail_level;

extern unsigned short pspofs;
extern unsigned short pspseg;
extern unsigned word_3FF82;
extern unsigned word_3FF84;

extern struct MEMCHUNK* resptr1;
extern struct MEMCHUNK* resptr2;
extern struct MEMCHUNK* resendptr1;
extern struct MEMCHUNK* resendptr2;
extern unsigned short resmaxsize;

extern unsigned long timer_callback_counter;
extern unsigned long last_timer_callback_counter;
extern unsigned long timer_copy_unk;

extern unsigned char g_kevinrandom_seed[];
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

extern struct GAMESTATE_SNAPSHOT gState_travDist;
extern short is_audioloaded;
extern void far* songfileptr;
extern void far* voicefileptr;
extern char textresprefix; 
extern char* shapeexts[];
extern unsigned char palmap[];

extern int* material_clrlist_ptr;
extern int* material_clrlist_ptr_cpy;
extern int* material_clrlist2_ptr;
extern int* material_clrlist2_ptr_cpy;
extern int* material_patlist_ptr;
extern int* material_patlist_ptr_cpy;
extern int* material_patlist2_ptr;
extern int* material_patlist2_ptr_cpy;
extern unsigned short someZeroVideoConst;
extern void font_set_fontdef(void);
extern void init_polyinfo(void);
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
extern void far* file_load_shape2d_nofatal_thunk(const char* filename);
extern void far* file_load_shape2d_res_nofatal_thunk(const char* filename);
extern void far* file_load_shape2d_nofatal(char* shapename);
extern void far* file_load_shape2d_nofatal2(char* shapename);
extern void far* init_audio_resources(void far* songptr, void far* voiceptr, const char* name);
extern void load_audio_finalize(void far* audiores);
extern short audio_load_driver(char* driver, short a2, short a3);
extern void audio_unload(void);
extern short audio_toggle_flag2(void);
extern short audio_toggle_flag6(void);
extern void audio_stop_unk(void);
extern void audiodrv_atexit(void);

extern void check_input(void);
extern int input_do_checking(int unk);
extern void kb_exit_handler(void);
extern void kb_shift_checking1(void);
extern void kb_shift_checking2(void);
extern void kb_reg_callback(int code, void (far* callback)(void));
extern void show_graphic_levels_menu(void);
extern void do_joy_restext(void);
extern void do_key_restext(void);
extern void do_mof_restext(void);
extern void do_pau_restext(void);
extern void do_dos_restext(void);
extern void do_sonsof_restext(void);
extern short get_kb_or_joy_flags(void);

extern short mouse_init(short a1, short a2);
extern void mouse_draw_opaque_check(void);

extern void video_set_mode4(void);
extern void video_set_mode7(void);
extern void video_set_mode_13h(void);

extern void shape3d_load_car_shapes(char* carid, char* oppcarid);

extern void load_palandcursor(void);
extern void sprite_set_1_size(unsigned short left, unsigned short right, unsigned short top, unsigned short height);
extern void sprite_clear_1_color(unsigned char);
extern void sprite_blit_to_video(struct SPRITE far* sprite);

extern short intr0_handler(void);
extern short (far* old_intr0_handler)(void);
extern void timer_setup_interrupt(void);
extern unsigned long timer_get_delta_alt(void);

extern short set_criterr_handler(short (far* callback)(void));
extern void libsub_quit_to_dos_alt(short a1);
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







extern int penalty_time;

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
extern long pState_lvec1_x;
extern long pState_lvec1_y;
extern long pState_lvec1_z;
extern int pState_minusRotate_z_1;
extern int pState_minusRotate_z_2;
extern int pState_minusRotate_y_1;
extern int pState_minusRotate_y_2;
extern int pState_minusRotate_x_1;
extern int pState_minusRotate_x_2;
extern struct MATRIX mat_unk;
extern struct VECTOR vec_unk2;
extern int planindex;
extern int planindex_copy;
extern int pState_f36Mminf40sar2;
extern struct VECTOR vec_planerotopresult;
extern char current_surf_type;
extern int nextPosAndNormalIP;
extern int wallindex;
extern int elRdWallRelated;
extern int wallHeight;
extern int wallStartX;
extern int wallStartZ;
extern int wallOrientation;
extern struct PLANE far* planptr;
extern struct PLANE far* current_planptr;
extern int elem_xCenter;
extern int elem_zCenter;
extern int terrainHeight;
extern char byte_4392C;
extern struct POINT2D unk_3BD62[2];
extern struct POINT2D unk_3BD5A[2];
extern struct POINT2D unk_3BD6A[2];
extern int word_3BD72[4];
extern int word_4408C;
extern int word_43964;
extern int bto_auxiliary1(int, int, struct VECTOR*);
extern short track_pieces_counter;
extern struct PLANE far plan_memres[];
extern short word_35516;
extern void init_unknown(void);
extern unsigned const char* g_ascii_props;
extern struct SHAPE3D game3dshapes[];
extern unsigned select_cliprect_rotate(int angX, int angY, int angZ, struct RECTANGLE* cliprect, int unk);
extern void transformed_shape_op(struct TRANSFORMSHAPE3D* shape);
extern void sub_29772(void);
extern void set_projection(int, int, int, int);
extern struct SPRITE far* wndsprite;
extern struct RECTANGLE cliprect_unk;
extern int polyinfonumpolys;
extern unsigned char far* polyinfoptrs[];
extern unsigned int poly_linked_list_40ED6[];
extern void preRender_default(int color, int vertlinecount, int* vertlines);
extern unsigned char oppnentSped[];
extern struct TRACKOBJECT trkObjectList[];
extern unsigned int update_rpm_from_speed(unsigned int, unsigned int, unsigned int, int, unsigned int);
extern int abs(int);
extern unsigned short word_2BDF8[5];
extern char unk_44F4C[];
extern char byte_459D8;
extern char byte_42D26;
extern char byte_42D2A;
extern char byte_3BE02;
extern short word_449E4;
extern short word_44D1E;
extern short word_44D20;
extern void audio_op_unk(short);
extern void audio_op_unk5(short);
extern void audio_op_unk6(short);
extern void audio_op_unk7(short);
extern void audio_function2(short);
extern void sub_38178(void);
extern int word_3BE04[];
extern int word_3BE0C[];
extern struct RECTANGLE select_rect_rc;
extern struct MATRIX mat_z_rot;
extern struct MATRIX mat_x_rot;
extern struct MATRIX mat_y_rot;
extern struct MATRIX mat_rot_temp;
extern unsigned mat_y_rot_angle;
extern long sin80, cos80;
extern unsigned char atantable[];
extern int projectiondata5, projectiondata8, projectiondata9, projectiondata10;
extern struct MATRIX mat_unk2;
extern int word_3BE16;
extern struct MATRIX mat_planetmp;
extern int f36f40_whlData;
void opponent_op(void);
void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec);
void update_player_state(struct CARSTATE* arg_pState, struct SIMD* arg_pSimd, struct CARSTATE* arg_oState, struct SIMD* arg_oSimd, char arg_MplayerFlag);
void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short track_angle);
void init_game_state(short arg);
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
void sub_18D06(struct AUDIO_CAR_FRAME *record, short value);
char sub_18D60(int trackIndex, struct TRACKRESULT *result, char side,
              char *opponentSpeed);
char car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds);
void init_plantrak(void);
void do_opponent_op(void);

void update_crash_state(int arg_someFlag, int arg_MplayerFlag);
void plane_rotate_op(void);
int plane_origin_op(int arg_planindex, int x, int y, int z);
int vec_normalInnerProduct(int x, int y, int z, struct VECTOR far *normal);
void state_op_unk(int mode, short angle, short speed);
void sub_19BA0(void);

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

    if (data_349D0 == 20) {
        max = 8;
        brake = 1;
    } else {
        max = 16;
        brake = 2;
    }
    if (state.opponentstate.car_36MwhlAngle != 0 || state.game_inputmode == 2)
        skip = 1;
    else
        skip = 0;
    x1 = state.opponentstate.car_posWorld1.lx >> 6;
    y1 = state.opponentstate.car_posWorld1.ly >> 6;
    z1 = state.opponentstate.car_posWorld1.lz >> 6;
    x2 = state.playerstate.car_posWorld1.lx >> 6;
    y2 = state.playerstate.car_posWorld1.ly >> 6;
    z2 = state.playerstate.car_posWorld1.lz >> 6;
    state.opponentstate.field_CF = 0;
    state.field_45E = 0;
    tmat = mat_rot_zxy(state.opponentstate.car_rotate.z, state.opponentstate.car_rotate.y,
                           state.opponentstate.car_rotate.x, 1);
    state.opponentstate.field_CF = 1;
    if (state.opponentstate.car_crashBmpFlag != 0) {
        if (state.opponentstate.car_speed2 == 0)
            state.opponentstate.field_CF = 0;
    } else {
        waypoint = state.opponentstate.car_vec_unk3;
        if (waypoint.y != -1) {
            delta.x = waypoint.x - x1;
            delta.y = waypoint.y - y1;
            delta.z = waypoint.z - z1;
            temp = polarRadius3D(&delta);
        } else {
            temp = polarRadius2D(waypoint.x - x1, waypoint.z - z1);
        }
        if (temp < 200) {
next_waypoint:
            if (sub_18D60(((short far *)trackdata3)[state.opponentstate.car_trackdata3_index],
                          &state.opponentstate.car_vec_unk3, state.opponentstate.field_CE++,
                          &state.field_3F9) != 0) {
                state.opponentstate.car_trackdata3_index++;
                if (((short far *)trackdata3)[state.opponentstate.car_trackdata3_index] == 0) {
                    state.opponentstate.field_CD++;
                    state.opponentstate.car_trackdata3_index = 0;
                }
                state.opponentstate.field_CE = 0;
            }
        }
        if (state.game_inputmode == 2) {
no_player:
            goal = state.opponentstate.car_vec_unk3;
        } else {
            diff.x = x2 - x1;
            diff.y = y2 - y1;
            diff.z = z2 - z1;
            mat_mul_vector(&diff, tmat, &pv);
            if (pv.y > 90 || (pv.x < 0 ? -pv.x : pv.x) > 180 ||
                pv.z > 600 || pv.z < -180)
                goto no_player;
            diff.x = x2 - state.opponentstate.car_vec_unk3.x;
            if (state.opponentstate.car_vec_unk3.y == -1)
                diff.y = 0;
            else
                diff.y = y2 - state.opponentstate.car_vec_unk3.y;
            diff.z = z2 - state.opponentstate.car_vec_unk3.z;
            mat_mul_vector(&diff, tmat, &rel);
            if (rel.x < 0) {
                goal.x = ((long)state.opponentstate.car_vec_unk3.x + state.opponentstate.car_vec_unk5.x) >> 1;
                if (state.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((long)state.opponentstate.car_vec_unk3.y + state.opponentstate.car_vec_unk5.y) >> 1;
                goal.z = ((long)state.opponentstate.car_vec_unk3.z + state.opponentstate.car_vec_unk5.z) >> 1;
                if (pv.z > -78 && state.playerstate.car_crashBmpFlag == 0)
                    state.field_45E = 2;
            } else {
                goal.x = ((long)state.opponentstate.car_vec_unk3.x + state.opponentstate.car_vec_unk4.x) >> 1;
                if (state.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((long)state.opponentstate.car_vec_unk3.y + state.opponentstate.car_vec_unk4.y) >> 1;
                goal.z = ((long)state.opponentstate.car_vec_unk3.z + state.opponentstate.car_vec_unk4.z) >> 1;
                if (pv.z > -78 && state.playerstate.car_crashBmpFlag == 0)
                    state.field_45E = 1;
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
        mat_mul_vector(&diff, tmat, &waypoint);
        steerAngle = polarAngle(waypoint.x, waypoint.z);
        if (state.opponentstate.car_slidingFlag == 0 &&
            (steerAngle < 0 ? -steerAngle : steerAngle) > 0x100) {
            if (sub_18D60(((short far *)trackdata3)[state.opponentstate.car_trackdata3_index],
                          &state.opponentstate.car_vec_unk3, state.opponentstate.field_CE++,
                          &state.field_3F9) != 0) {
                state.opponentstate.car_trackdata3_index++;
                if (((short far *)trackdata3)[state.opponentstate.car_trackdata3_index] == 0) {
                    state.opponentstate.field_CD++;
                    state.opponentstate.car_trackdata3_index = 0;
                }
                state.opponentstate.field_CE = 0;
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
        if (state.opponentstate.car_sumSurfFrontWheels == 0)
            steerAngle = 0;
        temp = steerAngle - state.opponentstate.car_steeringAngle;
        if ((temp < 0 ? -temp : temp) > max) {
            if (steerAngle < state.opponentstate.car_steeringAngle)
                state.opponentstate.car_steeringAngle -= max;
            else
                state.opponentstate.car_steeringAngle += max;
        } else {
            state.opponentstate.car_steeringAngle = steerAngle;
        }
    }
    mode = 0;
    if (state.opponentstate.car_sumSurfRearWheels != 0) {
        if (state.opponentstate.car_crashBmpFlag != 0) {
            mode = 2;
        } else if (state.opponentstate.car_36MwhlAngle != 0) {
            if ((brake << 9) > state.opponentstate.car_speed2) {
                state.opponentstate.car_speed2 = 0;
                state.opponentstate.car_36MwhlAngle = 0;
            } else {
                state.opponentstate.car_speed2 -= brake << 9;
            }
        } else if (state.opponentstate.car_demandedGrip > state.opponentstate.car_surfacegrip_sum) {
            mode = 2;
        } else {
            if (state.game_inputmode == 2)
                spdLimit = 0x4000;
            else
                spdLimit = state.field_3F9 << 8;
            if (spdLimit - 0x100 > state.opponentstate.car_speed)
                mode = 1;
            else if (spdLimit + 0x300 < state.opponentstate.car_speed)
                mode = 2;
        }
    }
    update_car_speed(mode, 1, &state.opponentstate, &simd_opponent);
    update_grip(&state.opponentstate, &simd_opponent, 0);
    update_player_state(&state.opponentstate, &simd_opponent, &state.playerstate, &simd_player, 1);
    if (state.opponentstate.car_crashBmpFlag == 0) {
        diff = state.opponentstate.car_vec_unk3;
        diff.x -= state.opponentstate.car_posWorld1.lx >> 6;
        diff.y -= state.opponentstate.car_posWorld1.ly >> 6;
        diff.z -= state.opponentstate.car_posWorld1.lz >> 6;
        tmat = mat_rot_zxy(state.opponentstate.car_rotate.z, state.opponentstate.car_rotate.y,
                               state.opponentstate.car_rotate.x, 1);
        mat_mul_vector(&diff, tmat, &waypoint);
        state.opponentstate.field_48 = polarAngle(-waypoint.x, waypoint.z) & 0x3FF;
    }
    if (state.opponentstate.field_CD != 0) {
        temp = multiply_and_scale(cos_fast(track_angle),
            trackcenterpos[startrow2] - (int)(state.opponentstate.car_posWorld1.lz >> 6));
        temp += multiply_and_scale(sin_fast(track_angle),
            trackcenterpos2[startcol2] - (int)(state.opponentstate.car_posWorld1.lx >> 6));
        if (temp < 0)
            update_crash_state(3, 1);
    }
}

void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec) { struct MATRIX tmpmat; tmpmat=*mat; mat_mul_vector(invec,&tmpmat,outvec); }

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

	pState_lvec1_x = arg_pState->car_posWorld1.lx;
	arg_pState->car_posWorld2.lx = pState_lvec1_x;
	pState_lvec1_y = arg_pState->car_posWorld1.ly;
	arg_pState->car_posWorld2.ly = pState_lvec1_y;
	pState_lvec1_z = arg_pState->car_posWorld1.lz;
	arg_pState->car_posWorld2.lz = pState_lvec1_z;
	pState_minusRotate_z_2 = pState_minusRotate_z_1 = arg_pState->car_rotate.z;
	pState_minusRotate_x_2 = pState_minusRotate_x_1 = arg_pState->car_rotate.y;
	pState_minusRotate_y_2 = pState_minusRotate_y_1 = arg_pState->car_rotate.x;
	if (arg_pState->car_sumSurfAllWheels != 0)
		frontSteer = arg_pState->car_40MfrontWhlAngle >> 2;
	else
		frontSteer = 0;
	if (data_349D0 == 10)
		spd = (long)arg_pState->car_speed2 * 0x580 / 0x1E00U;
	else
		spd = (long)arg_pState->car_speed2 * 0x580 / 0x3C00U;
	mat_unk = *mat_rot_zxy(-pState_minusRotate_z_1, -pState_minusRotate_x_1, -pState_minusRotate_y_1, 0);
	if (pState_minusRotate_x_1 != 0 || pState_minusRotate_z_1 != 0) {
		wheelCrd.x = 0;
		wheelCrd.y = 0;
		wheelCrd.z = 0x82;
		mat_mul_vector(&wheelCrd, &mat_unk, &res);
		arg_pState->car_pseudoGravity = -res.y;
	} else {
		arg_pState->car_pseudoGravity = 0;
	}
	if (arg_pState->car_angle_z & 0x3FF) {
		tiltFlag = 1;
		rotMatrix = *mat_rot_zxy(0, 0, -arg_pState->car_angle_z, 0);
	} else {
		tiltFlag = 0;
	}
	wheelCrd.x = 0;
	wheelCrd.y = 30000;
	wheelCrd.z = 0;
	mat_mul_vector(&wheelCrd, &mat_unk, &res);
	if (arg_pState->car_sumSurfAllWheels != 0 && res.y < 0) {
		if (arg_pState->car_speed2 > 0x1E00) {
			liftOfs = 0xC0;
			wheelCrd.y = -0xC0;
			mat_mul_vector(&wheelCrd, &mat_unk, &hopVec);
		} else {
			liftOfs = -0xC0;
		}
	} else {
		liftOfs = 0;
	}
	vec_unk2.x = 0;
	vec_unk2.y = 0;
	planindex_copy = -1;
	curWhl = whlPos;
	prev = oldWhls;
	for (w = 0; w < 4; ++curWhl, ++prev, ++w) {
		wheelCrd = arg_pSimd->wheel_coords[w];
		wheelCrd.y = -(arg_pState->car_rc2[w] + 0x180);
		if (liftOfs < 0)
			wheelCrd.y -= liftOfs;
		if (tiltFlag != 0) {
			mat_mul_vector(&wheelCrd, &rotMatrix, &res);
			wheelCrd = res;
		}
		mat_mul_vector(&wheelCrd, &mat_unk, &res);
		curWhl->lx = res.x + pState_lvec1_x;
		curWhl->ly = res.y + pState_lvec1_y;
		curWhl->lz = res.z + pState_lvec1_z;
		prev->lx = curWhl->lx;
		prev->ly = curWhl->ly;
		prev->lz = curWhl->lz;
		if (spd != 0) {
			vec_unk2.z = spd;
			if (frontSteer != 0 && w < 2)
				pState_f36Mminf40sar2 = arg_pState->car_36MwhlAngle - frontSteer;
			else
				pState_f36Mminf40sar2 = arg_pState->car_36MwhlAngle;
			wheelAngle[w] = pState_f36Mminf40sar2;
			plane_rotate_op();
			curWhl->lx += vec_planerotopresult.x;
			curWhl->ly += vec_planerotopresult.y;
			curWhl->lz += vec_planerotopresult.z;
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
		if (state.game_inputmode == 2) {
			wallindex = -1;
			current_surf_type = 1;
			planindex = 0;
			current_planptr = planptr;
		} else {
			build_track_object(&wheelCrd, &arg_pState->car_whlWorldCrds1[w]);
		}
		arg_pState->car_surfaceWhl[w] = current_surf_type;
		wheelCrd.x = curWhl->lx >> 6;
		wheelCrd.y = curWhl->ly >> 6;
		wheelCrd.z = curWhl->lz >> 6;
		if (state.game_inputmode == 2)
			nextPosAndNormalIP = wheelCrd.y;
		else
			nextPosAndNormalIP = plane_origin_op(planindex, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (wallindex != -1 && nextPosAndNormalIP > elRdWallRelated && nextPosAndNormalIP < wallHeight) {
			oldVec.x = arg_pState->car_whlWorldCrds1[w].x - wallStartX;
			oldVec.y = 0;
			oldVec.z = arg_pState->car_whlWorldCrds1[w].z - wallStartZ;
			vec.x = (int)(curWhl->lx >> 6) - wallStartX;
			vec.y = 0;
			vec.z = (int)(curWhl->lz >> 6) - wallStartZ;
			mat_rot_y(&localPlane, -wallOrientation - 0x100);
			mat_mul_vector(&oldVec, &localPlane, &p0);
			mat_mul_vector(&vec, &localPlane, &p1);
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
				angle = (-pState_minusRotate_y_1 - wallOrientation) & 0x3FF;
				res.z = distBefore;
				res.y = 0;
				if (angle < 0x100 || angle > 0x300) {
					angle = wallOrientation;
					res.x = 0x300;
				} else {
					angle = (wallOrientation + 0x200) & 0x3FF;
					res.x = -0x300;
				}
				if (swap != 0)
					res.x = -res.x;
				rotMat = mat_rot_zxy(-pState_minusRotate_z_1, -pState_minusRotate_x_1, angle, 0);
				mat_mul_vector(&res, rotMat, &p1);
				i = (-pState_minusRotate_y_1 - angle) & 0x3FF;
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
		if (nextPosAndNormalIP > 0) {
			if (liftOfs > 0 && nextPosAndNormalIP < 24) {
				curWhl->lx += hopVec.x;
				curWhl->ly += hopVec.y;
				curWhl->lz += hopVec.z;
			} else {
				arg_pState->car_rc1[w] += word_3BD72[w];
				curWhl->ly -= arg_pState->car_rc1[w];
				if (data_349D0 == 10) {
					arg_pState->car_rc1[w] += word_3BD72[w];
					curWhl->ly -= arg_pState->car_rc1[w];
				}
				wheelCrd.y = curWhl->ly >> 6;
				if (state.game_inputmode == 2)
					nextPosAndNormalIP = wheelCrd.y;
				else
					nextPosAndNormalIP = plane_origin_op(planindex, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (nextPosAndNormalIP > 12)
					arg_pState->car_surfaceWhl[w] = 0;
			}
		}
		whlHgts[w] = nextPosAndNormalIP;
		if (nextPosAndNormalIP == 0) {
			if (arg_pState->car_rc1[w] > 250)
				arg_pState->field_CF |= 0x20;
			if (arg_pState->car_rc1[w] > 23275)
				update_crash_state(1, arg_MplayerFlag);
			arg_pState->car_rc1[w] = 0;
			goto next_wheel;
		}
		if (nextPosAndNormalIP >= 0)
			goto next_wheel;
		plane = &planptr[planindex];
		base.x = plane->plane_origin.x + elem_xCenter;
		base.y = plane->plane_origin.y + terrainHeight;
		base.z = plane->plane_origin.z + elem_zCenter;
		oldVec.x = (int)(prev->lx >> 6) - base.x;
		oldVec.y = (int)(prev->ly >> 6) - base.y;
		oldVec.z = (int)(prev->lz >> 6) - base.z;
		vec.x = (int)(curWhl->lx >> 6) - base.x;
		vec.y = (int)(curWhl->ly >> 6) - base.y;
		vec.z = (int)(curWhl->lz >> 6) - base.z;
		localPlane = plane->plane_rotation;
		mat_invert(&localPlane, &rotMatrix);
		mat_mul_vector(&oldVec, &rotMatrix, &p0);
		mat_mul_vector(&vec, &rotMatrix, &p1);
		swap = 0;
		if (byte_4392C == 0 && p0.y < -12 && p1.y < -12) {
			if (p1.y > -24) {
				update_crash_state(5, arg_MplayerFlag);
				swap = 1;
			} else {
				planindex = 0;
				current_planptr = planptr;
				byte_4392C = 1;
				wheelCrd.x = curWhl->lx >> 6;
				wheelCrd.y = curWhl->ly >> 6;
				wheelCrd.z = curWhl->lz >> 6;
				nextPosAndNormalIP = plane_origin_op(0, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				goto check_height;
			}
		}
		if (p1.y == 0) {
			vec_unk2.x = 0;
			vec_unk2.y = 0;
			vec_unk2.z = 0x40;
			planindex_copy = planindex;
			pState_f36Mminf40sar2 = wheelAngle[w];
			plane_rotate_op();
			curWhl->lx -= vec_planerotopresult.x;
			curWhl->ly -= vec_planerotopresult.y;
			curWhl->lz -= vec_planerotopresult.z;
			goto rc_check;
		}
		if (p0.y <= 0 || p1.y >= 0) {
			vec_unk2.x = 0;
			vec_unk2.y = 0;
			vec_unk2.z = spd;
			planindex_copy = planindex;
			pState_f36Mminf40sar2 = wheelAngle[w];
			plane_rotate_op();
			curWhl->lx = prev->lx + vec_planerotopresult.x;
			curWhl->ly = prev->ly + vec_planerotopresult.y;
			curWhl->lz = prev->lz + vec_planerotopresult.z;
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
			vec_unk2.x = 0;
			vec_unk2.y = 0;
			vec_unk2.z = angle;
			planindex_copy = planindex;
			pState_f36Mminf40sar2 = wheelAngle[w];
			plane_rotate_op();
			curWhl->lx = prev->lx + p0.x + vec_planerotopresult.x;
			curWhl->ly = prev->ly + p0.y + vec_planerotopresult.y;
			curWhl->lz = prev->lz + p0.z + vec_planerotopresult.z;
		}
		wheelCrd.x = curWhl->lx >> 6;
		wheelCrd.y = curWhl->ly >> 6;
		wheelCrd.z = curWhl->lz >> 6;
		if ((nextPosAndNormalIP = plane_origin_op(planindex, wheelCrd.x, wheelCrd.y, wheelCrd.z)) < 0) {
			if (swap != 0)
				nextPosAndNormalIP = -nextPosAndNormalIP + 6;
			wheelCrd.z = 0;
			wheelCrd.x = 0;
			wheelCrd.y = -nextPosAndNormalIP << 6;
			mat_mul_vector2(&wheelCrd, &planptr[planindex].plane_rotation, &res);
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
		if (pState_minusRotate_z_1 != 0 || pState_minusRotate_x_1 != 0) {
			wheelCrd.z = 0;
			wheelCrd.x = 0;
			wheelCrd.y = angle + 0x180;
			mat_mul_vector(&wheelCrd, &mat_unk, &oldVec);
			curWhl->lx += oldVec.x;
			curWhl->ly += oldVec.y;
			curWhl->lz += oldVec.z;
		} else {
			curWhl->ly += angle + 0x180;
		}
	}

	pState_lvec1_x = (whlPos[0].lx + whlPos[1].lx + whlPos[2].lx + whlPos[3].lx) >> 2;
	pState_lvec1_y = (whlPos[0].ly + whlPos[1].ly + whlPos[2].ly + whlPos[3].ly) >> 2;
	pState_lvec1_z = (whlPos[0].lz + whlPos[1].lz + whlPos[2].lz + whlPos[3].lz) >> 2;
	curWhl = whlPos;
	for (w = 0; w < 4; ++curWhl, ++w) {
		posDiffs[w].x = curWhl->lx - pState_lvec1_x;
		posDiffs[w].y = curWhl->ly - pState_lvec1_y;
		posDiffs[w].z = curWhl->lz - pState_lvec1_z;
	}
	if (pState_lvec1_y < 0)
		pState_lvec1_y = 0;
	if (pState_lvec1_x > 0x1DF100L)
		pState_lvec1_x = 0x1DF0FFL;
	else if (pState_lvec1_x < 0xF00L)
		pState_lvec1_x = 0xF00L;
	if (pState_lvec1_z > 0x1DF100L)
		pState_lvec1_z = 0x1DF0FFL;
	else if (pState_lvec1_z < 0xF00L)
		pState_lvec1_z = 0xF00L;
	angle = posDiffs[3].x + posDiffs[2].x - posDiffs[0].x - posDiffs[1].x;
	distBefore = posDiffs[3].z + posDiffs[2].z - posDiffs[0].z - posDiffs[1].z;
	pState_minusRotate_y_1 = polarAngle(angle, -distBefore) & 0x3FF;
	mat_rot_y(&rotMatrix, pState_minusRotate_y_1);
	for (w = 0; w < 4; w++) {
		res = posDiffs[w];
		mat_mul_vector(&res, &rotMatrix, &posDiffs[w]);
	}
	distBefore = posDiffs[3].z + posDiffs[2].z - posDiffs[0].z - posDiffs[1].z;
	remDist = posDiffs[3].y + posDiffs[2].y - posDiffs[0].y - posDiffs[1].y;
	if (remDist == 0 && distBefore < 0) {
		pState_minusRotate_x_1 = 0;
	} else {
		pState_minusRotate_x_1 = polarAngle(-distBefore, remDist) - 0x100;
		if ((pState_minusRotate_x_1 < 0 ? -pState_minusRotate_x_1 : pState_minusRotate_x_1) < 2)
			pState_minusRotate_x_1 = 0;
	}
	if (pState_minusRotate_x_1 != 0) {
		mat_rot_x(&rotMatrix, pState_minusRotate_x_1);
		for (w = 0; w < 4; w++) {
			res = posDiffs[w];
			mat_mul_vector(&res, &rotMatrix, &posDiffs[w]);
		}
	}
	distBefore = posDiffs[1].x + posDiffs[2].x - posDiffs[0].x - posDiffs[3].x;
	remDist = posDiffs[1].y + posDiffs[2].y - posDiffs[0].y - posDiffs[3].y;
	if (remDist == 0 && distBefore > 0) {
		pState_minusRotate_z_1 = 0;
	} else {
		pState_minusRotate_z_1 = polarAngle(distBefore, remDist) - 0x100;
		if ((pState_minusRotate_z_1 < 0 ? -pState_minusRotate_z_1 : pState_minusRotate_z_1) < 2)
			pState_minusRotate_z_1 = 0;
	}
	arg_pState->car_sumSurfFrontWheels = arg_pState->car_surfaceWhl[0] + arg_pState->car_surfaceWhl[1];
	arg_pState->car_sumSurfRearWheels = arg_pState->car_surfaceWhl[2] + arg_pState->car_surfaceWhl[3];
	if (state.game_inputmode == 2)
		goto store_state;
	if (is_in_replay == 0) {
		if (arg_MplayerFlag != 0)
			audio_unk3(arg_pState->field_CF, word_4408C);
		else
			audio_unk3(arg_pState->field_CF, word_43964);
	}
	rotMat = mat_rot_zxy(-pState_minusRotate_z_1, -pState_minusRotate_x_1, -pState_minusRotate_y_1, 0);
	for (w = 0; w < 4; w++) {
		wheelCrd = arg_pSimd->wheel_coords[w];
		wheelCrd.y = arg_pSimd->collide_points[0].py << 6;
		mat_mul_vector(&wheelCrd, rotMat, &res);
		wheelCrd.x = (res.x + pState_lvec1_x) >> 6;
		wheelCrd.y = (res.y + pState_lvec1_y) >> 6;
		wheelCrd.z = (res.z + pState_lvec1_z) >> 6;
		tmpCrds = wheelCrd;
		build_track_object(&wheelCrd, &arg_pState->car_whlWorldCrds2[w]);
		i = plane_origin_op(planindex, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (planindex < 4) {
			if (i <= 0)
				goto crash_wheel;
		} else {
			planIdx = planindex;
			wheelCrd = arg_pState->car_whlWorldCrds2[w];
			build_track_object(&wheelCrd, &tmpCrds);
			if (planIdx == planindex) {
				found = plane_origin_op(planindex, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (game_replay_mode != 1 && ((i < 0 && found > 0) || (i > 0 && found < 0))) {
crash_wheel:
					update_crash_state(5, arg_MplayerFlag);
				}
			}
		}
		arg_pState->car_whlWorldCrds2[w] = tmpCrds;
	}
	groundContact = arg_pState->car_sumSurfFrontWheels + arg_pState->car_sumSurfRearWheels;
	if (arg_MplayerFlag == 0 && groundContact == 0 && arg_pState->car_sumSurfAllWheels != 0)
		state.game_jumpCount++;
	arg_pState->car_sumSurfAllWheels = groundContact;
	self[0].x = pState_lvec1_x >> 6;
	self[0].y = pState_lvec1_y >> 6;
	self[0].z = pState_lvec1_z >> 6;
	self[1].x = pState_minusRotate_z_1;
	self[1].y = pState_minusRotate_x_1;
	self[1].z = pState_minusRotate_y_1;
	if (gameconfig.game_opponenttype != 0) {
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
			if (car_car_coll_detect_maybe(arg_pSimd->collide_points, self, unk_3BD6A, objPos)) {
				arg_pState->car_36MwhlAngle -= 0x200;
crash_return:
				update_crash_state(1, arg_MplayerFlag);
				return;
			}
		}
	}
	i = (char)trackdata19[trackrows[res.z] + res.x];
	if (i != -1 && state.field_3FA[i] == 0) {
		objPos[0].x = td10_track_check_rel[i].x;
		objPos[0].y = td10_track_check_rel[i].y;
		objPos[0].z = td10_track_check_rel[i].z;
		if (car_car_coll_detect_maybe(arg_pSimd->collide_points, self, unk_3BD5A, objPos)) {
			state.field_3FA[i] = 1;
			state_op_unk(i + 2, -arg_pState->car_rotate.x, (long)arg_pState->car_speed2 * 0x580 / 0x3C00U);
		}
	}
	if (res.x == startcol2 && res.z == startrow2) {
		objPos[0].x = trackcenterpos2[startcol2] + multiply_and_scale(sin_fast(track_angle + 0x100), 126);
		objPos[0].y = hillHeightConsts[hillFlag];
		objPos[0].z = multiply_and_scale(cos_fast(track_angle + 0x100), 126) + trackcenterpos[startrow2];
		if ((found = car_car_coll_detect_maybe(arg_pSimd->collide_points, self, unk_3BD62, objPos)) == 0) {
			objPos[0].x = multiply_and_scale(sin_fast(track_angle + 0x300), 126) + trackcenterpos2[startcol2];
			objPos[0].z = multiply_and_scale(cos_fast(track_angle + 0x300), 126) + trackcenterpos[startrow2];
			found = car_car_coll_detect_maybe(arg_pSimd->collide_points, self, unk_3BD62, objPos);
		}
		if (found != 0)
			goto crash_return;
	}
store_state:
	arg_pState->car_posWorld1.lx = pState_lvec1_x;
	arg_pState->car_posWorld1.ly = pState_lvec1_y;
	arg_pState->car_posWorld1.lz = pState_lvec1_z;
	arg_pState->car_rotate.z = pState_minusRotate_z_1;
	arg_pState->car_rotate.y = pState_minusRotate_x_1;
	arg_pState->car_rotate.x = pState_minusRotate_y_1;
	arg_pState->field_C8 = 0;
}

void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short track_angle)
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
    playerstate->car_rotate.x = track_angle;
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

void init_game_state(short arg)
{
	register int zeroValue = 0;
	register int i;
	int tmpcol, tmprow;

	if (arg == -1) {
		elapsed_time1 = zeroValue;

		for (i = 0; i < 20; ++i) {
			cvxptr[i].field_3F4 = zeroValue;
		}
	}
	
	if (data_349D0 == 10) {
		steerWhlRespTable_ptr = &steerWhlRespTable_10fps;
	}
	else {
		steerWhlRespTable_ptr = &steerWhlRespTable_20fps;
	}
	
	word_45A00 = data_349D0 * 30;
	word_4499C = 100 / data_349D0;

	if (arg != -3) {
		init_unknown();

		state.field_3F4 = 1;
		state.game_frames_per_sec = 1;
		state.game_inputmode = zeroValue;
		state.game_3F6autoLoadEvalFlag = zeroValue;
		state.game_frame_in_sec = zeroValue;
		state.field_2F4 = zeroValue;
		state.field_3F7[0] = zeroValue;
		state.field_3F7[1] = zeroValue;

		for (i = zeroValue; i < 48; ++i) {
			state.field_3FA[i] = zeroValue;
		}
				
		for (i = zeroValue; i < 24; ++i) {
			state.field_38E[i] = zeroValue;
		}

		state.game_vec1[0].x =
			  multiply_and_scale(sin_fast(word_35516 + 0x200), 4096)
			+ multiply_and_scale(sin_fast(word_35516 + 0x300),  512)
			+ ((short)startcol2 << 10);

		state.game_vec1[0].y = hillHeightConsts[hillFlag] + 960;

		state.game_vec1[0].z =
			  multiply_and_scale(cos_fast(word_35516 + 0x200), 4096)
			+ multiply_and_scale(cos_fast(word_35516 + 0x300),  512)
			+ trackpos[startrow2];

		state.game_vec1[1] = state.game_vec1[0];
		state.game_vec3[0] = state.game_vec1[0];
		state.game_vec3[1] = state.game_vec1[0];
		
		state.game_travDist = 0L;
		state.game_frame = zeroValue;
		state.game_total_finish = zeroValue;
		state.field_144 = zeroValue;
		state.game_pEndFrame = zeroValue;
		state.game_oEndFrame = zeroValue;
		state.game_penalty = zeroValue;
		state.game_impactSpeed = zeroValue;
		state.game_topSpeed = zeroValue;
		state.game_jumpCount = zeroValue;

		
		tmpcol =
			  multiply_and_scale(sin_fast(word_35516 + 0x100),  36)
			+ multiply_and_scale(sin_fast(word_35516 + 0x200), 210);
		
		tmprow =
			  multiply_and_scale(cos_fast(word_35516 + 0x100),  36)
			+ multiply_and_scale(cos_fast(word_35516 + 0x200), 210);

		init_carstate_from_simd(
			&state.playerstate,
			&simd_player,
			gameconfig.game_playertransmission,
			(long)(trackcenterpos2[startcol2] + tmpcol) * 64L,
			(long)hillHeightConsts[hillFlag] * 64L,
			(long)(trackcenterpos[startrow2] + tmprow) * 64L,
			-word_35516);

		state.field_2F2 = zeroValue;
		state.field_45D = zeroValue;
		state.field_45E = zeroValue;
		state.field_45B = zeroValue;
		state.field_45C = zeroValue;
		
		state.game_startcol  = startcol2;
		state.game_startcol2 = startcol2;
		state.game_startrow  = startrow2;
		state.game_startrow2 = startrow2;

		if (arg != -2) {
			sub_18D60(
				state.playerstate.car_trackdata3_index,
				&state.playerstate.car_vec_unk3,
				state.playerstate.field_CE++,
				0);
			
		}

		
		tmpcol =
			  multiply_and_scale(sin_fast(word_35516 + 0x300),  36)
			+ multiply_and_scale(sin_fast(word_35516 + 0x200), 210);
		
		tmprow =
			  multiply_and_scale(cos_fast(word_35516 + 0x300),  36)
			+ multiply_and_scale(cos_fast(word_35516 + 0x200), 210);

		init_carstate_from_simd(
			&state.opponentstate,
			&simd_opponent,
			1,
			(long)(trackcenterpos2[startcol2] + tmpcol) * 64L,
			(long)hillHeightConsts[hillFlag] * 64L,
			(long)(trackcenterpos[startrow2] + tmprow) * 64L,
			-word_35516);

		if (gameconfig.game_opponenttype && arg != -2) {
			sub_18D60(
				((short far *)trackdata3)[state.opponentstate.car_trackdata3_index], 
				&state.opponentstate.car_vec_unk3,
				state.opponentstate.field_CE++,
				&state.field_3F9); 
		
		}

		state.field_42A = zeroValue;
	}
}

void restore_gamestate(int frame)
{
    register int curframe;

    if (frame == 0 && elapsed_time1 == 0) {
        init_game_state(0);
    }

    curframe = frame / word_45A00;
    if (curframe == 20) {
        --curframe;
    }

    if (frame < state.game_frame)
        goto restore;
    while (word_45A00 * curframe > state.game_frame) {
        if (cvxptr[curframe].field_3F4 != 0) {
restore:
            state = cvxptr[curframe];
            init_kevinrandom(state.kevinseed);
            word_32D02 = state.game_frame;
            return;
        }
        --curframe;
    }
}

void update_gamestate() {
	char var_carInputByte;
	register int tmp;

	var_carInputByte = td16_rpl_buffer[state.game_frame];
	if (var_carInputByte != 0) {
		state.game_inputmode = 1;
	}
	
	if ((state.game_frame % word_45A00) == 0) {
		tmp = state.game_frame / word_45A00;
		get_kevinrandom_seed(state.kevinseed);

		cvxptr[tmp] = state;
	}

	state.game_frame++;
	if (state.game_3F6autoLoadEvalFlag != 0 && state.game_frame_in_sec < state.game_frames_per_sec) {
		state.game_frame_in_sec++;
		if (state.game_frame_in_sec == state.game_frames_per_sec && byte_449DA == 0) {
			if (state.playerstate.car_crashBmpFlag == 1 && state.playerstate.car_speed2 != 0) {
				state.game_frames_per_sec++;
			} else if (game_replay_mode == 0) {
				byte_449DA = 1;
			}
		}
	}

	if (state.game_inputmode != 0) {
		
		player_op(var_carInputByte);
		
		if (gameconfig.game_opponenttype != 0) {
			opponent_op();
		}

		sub_2298C();
		if (state.field_42A != 0) {
			sub_19BA0();
		}

		audio_carstate();

	} else if (game_replay_mode == 1) {
		
		audio_carstate();
		if (byte_4393C != 0) {
			if (word_44DCA < 0x1C2) {
				word_44DCA += 8;
			}

			if (byte_4393C == 1 && word_44DCA > 0x180) {
				byte_4393C++;
			}

			if (byte_4393C == 2) {
				tmp =
					multiply_and_scale(cos_fast(track_angle), trackcenterpos[startrow2] - (state.playerstate.car_posWorld1.lz >> 6))
					+ multiply_and_scale(sin_fast(track_angle), trackcenterpos2[startcol2] - (state.playerstate.car_posWorld1.lx >> 6));
				if (tmp > 0xE4) {
					if (state.playerstate.car_speed < 0x500) {
						player_op(1);
					} else {
						player_op(0);
					}
				} else {
					if (state.playerstate.car_speed != 0) {
						player_op(2);
					} else {
						byte_4393C = 0;
					}
				}
			}
		}
	}
}

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

	if (show_penalty_counter != 0)
		show_penalty_counter--;

	state.playerstate.field_CF = 1;
	if (state.playerstate.car_crashBmpFlag != 0) {
		state.field_45D = 0;
		arg_carInputByte = 2;
		if (state.playerstate.car_speed2 == 0) {
			state.playerstate.field_CF = 0;
			if (state.playerstate.car_speed == 0 && state.playerstate.car_rc1[0] == 0 &&
			    state.playerstate.car_rc1[1] == 0 && state.playerstate.car_rc1[2] == 0 &&
			    state.playerstate.car_rc1[3] == 0)
				return;
		}
	}

	update_car_speed(arg_carInputByte, 0, &state.playerstate, &simd_player);
	upd_statef20_from_steer_input((arg_carInputByte >> 2) & 3);
	update_grip(&state.playerstate, &simd_player, 1);
	update_player_state(&state.playerstate, &simd_player, &state.opponentstate, &simd_opponent, 0);
	state.game_travDist += state.playerstate.car_speed2;
	crash_mode = state.field_45B;
	tile_index = state.field_2F2;
	height = detect_penalty(&tile_index, &penalty_ctr);
	if (height != 0) {
		if (penalty_ctr == -2) {
			state.field_45B = 1;
			state.field_45C = 0;
		} else if (state.field_45B == 1) {
			state.field_45B = 0;
			state.field_45C = 0;
		}
		if (state.field_45B == 0) {
			if (tile_index == 0 && state.field_2F4 != 0) {
				state.playerstate.field_CD++;
				goto next_lap;
			}
			if (penalty_ctr >= 0 && penalty_ctr < 3) {
				state.field_45C = 0;
				state.field_2F2 = tile_index;
			} else if (penalty_ctr == -1 || penalty_ctr > 3) {
				if (td01_track_file_cpy[state.field_2F4] == tile_index ||
				    td02_penalty_related[state.field_2F4] == tile_index) {
					state.field_45C++;
				} else {
					if (td01_track_file_cpy[tile_index] == state.field_2F4 ||
					    td02_penalty_related[tile_index] == state.field_2F4)
						state.field_45B = 2;
					state.field_45C = 1;
				}
				if (state.field_45C >= 3) {
next_lap:
					state.field_2F2 = tile_index;
					state.field_45C = 0;
					if (penalty_ctr > 0) {
						penalty_time = penalty_ctr * data_349D0 * 3;
						show_penalty_counter = data_349D0 << 2;
						state.game_penalty += penalty_time;
					}
				}
			}
		}
		state.field_2F4 = tile_index;
	}

	state.field_45D = 0;
	if (state.field_45B == 1)
		return;
	planeMatrix = mat_rot_zxy(state.playerstate.car_rotate.z, state.playerstate.car_rotate.y,
	                          state.playerstate.car_rotate.x, 1);
	if (state.field_45B == 2) {
		if (state.playerstate.car_crashBmpFlag == 0)
			state.field_45D = 3;
		tile_index = state.field_2F4;
		goto search;
	}
	if (state.playerstate.car_trackdata3_index == -1) {
no_target:
		height = 0;
	} else {
		if ((crash_mode != 0 && state.field_45B == 0) ||
		    (state.playerstate.car_trackdata3_index != state.field_2F2 &&
		     td01_track_file_cpy[state.field_2F2] != state.playerstate.car_trackdata3_index &&
		     td02_penalty_related[state.field_2F2] != state.playerstate.car_trackdata3_index)) {
			state.playerstate.car_trackdata3_index = -1;
			goto no_target;
		}
		playerRelative.x = state.playerstate.car_vec_unk3.x - (int)(state.playerstate.car_posWorld1.lx >> 6);
		if (state.playerstate.car_vec_unk3.y != -1)
			playerRelative.y = state.playerstate.car_vec_unk3.y - (int)(state.playerstate.car_posWorld1.ly >> 6);
		else
			playerRelative.y = 0;
		playerRelative.z = state.playerstate.car_vec_unk3.z - (int)(state.playerstate.car_posWorld1.lz >> 6);
		mat_mul_vector(&playerRelative, planeMatrix, &player_plane_vec);
		height = player_plane_vec.z;
	}
	if (height < 0x113) {
		if (state.playerstate.car_trackdata3_index == -1) {
			tile_index = state.field_2F2;
search:
			if (td02_penalty_related[tile_index] != -1)
				goto check_lap;
			hasPenalty = 0;
			track_side = 0;
			do {
				hasPenalty = sub_18D60(tile_index, &state.playerstate.car_vec_unk3, track_side, 0);
				offset_vector = state.playerstate.car_vec_unk3;
				offset_vector.x -= state.playerstate.car_posWorld1.lx >> 6;
				if (offset_vector.y == -1)
					offset_vector.y = -(int)(state.playerstate.car_posWorld1.ly >> 6);
				else
					offset_vector.y -= state.playerstate.car_posWorld1.ly >> 6;
				offset_vector.z -= state.playerstate.car_posWorld1.lz >> 6;
				mat_mul_vector(&offset_vector, planeMatrix, &player_plane_vec);
				if (track_side == 0 ||
				    (player_plane_vec.z < playerRelative.z && player_plane_vec.z > 0)) {
					trackSide = track_side;
					playerRelative.z = player_plane_vec.z;
				}
				track_side++;
			} while (hasPenalty == 0);
			if (state.field_45B == 2) {
				if (trackSide == 0) {
					sub_18D60(tile_index, pathPoints, 0, 0);
					sub_18D60(tile_index, playerEdges, 1, 0);
				} else {
					sub_18D60(tile_index, pathPoints, (char)(trackSide - 1), 0);
					sub_18D60(tile_index, playerEdges, trackSide, 0);
				}
				height = polarAngle(pathPoints[0].x - playerEdges[0].x, playerEdges[0].z - pathPoints[0].z) & 0x3FF;
				height = (state.playerstate.car_rotate.x - height) & 0x3FF;
				if (height <= 0x380 && height >= 0x80)
					goto advance;
				state.field_45B = 0;
				state.field_45C = 1;
				state.playerstate.car_trackdata3_index = tile_index;
			} else {
				state.playerstate.car_trackdata3_index = state.field_2F2;
			}
			state.playerstate.field_CE = trackSide;
		}
advance:
		if (sub_18D60(state.playerstate.car_trackdata3_index, &state.playerstate.car_vec_unk3,
		              state.playerstate.field_CE++, 0) != 0) {
			if (td02_penalty_related[state.field_2F2] != -1)
				state.playerstate.car_trackdata3_index = -1;
			else
				state.playerstate.car_trackdata3_index = td01_track_file_cpy[state.field_2F2];
			state.playerstate.field_CE = 0;
		}
	}
	offset_vector = state.playerstate.car_vec_unk3;
	if (state.playerstate.car_trackdata3_index != -1 && state.field_45B == 0) {
		offset_vector.x -= state.playerstate.car_posWorld1.lx >> 6;
		if (offset_vector.y == -1)
			offset_vector.y = 0;
		else
			offset_vector.y -= state.playerstate.car_posWorld1.ly >> 6;
		offset_vector.z -= state.playerstate.car_posWorld1.lz >> 6;
		planeMatrix = mat_rot_zxy(state.playerstate.car_rotate.z, state.playerstate.car_rotate.y,
		                          state.playerstate.car_rotate.x, 1);
		mat_mul_vector(&offset_vector, planeMatrix, &player_plane_vec);
		state.playerstate.field_48 = polarAngle(-player_plane_vec.x, player_plane_vec.z) & 0x3FF;
		if (state.playerstate.car_crashBmpFlag == 0) {
			switch ((unsigned)((state.playerstate.field_48 + 0x80) & 0x3FF) >> 8) {
			case 1:
				state.field_45D = 1;
				break;
			case 3:
				if (state.playerstate.field_B6 == 0) {
					state.field_45D = 2;
					break;
				}
			default:
				state.field_45D = 0;
				break;
			}
		}
	}
check_lap:
	if (state.playerstate.field_CD != 0) {
		height = multiply_and_scale(cos_fast(track_angle),
			trackcenterpos[startrow2] - (int)(state.playerstate.car_posWorld1.lz >> 6));
		height += multiply_and_scale(sin_fast(track_angle),
			trackcenterpos2[startcol2] - (int)(state.playerstate.car_posWorld1.lx >> 6));
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

    carCol = (char)(state.playerstate.car_posWorld1.lx >> 16);
    playerRow = (char)(0x1D - (char)(state.playerstate.car_posWorld1.lz >> 16));
    if ((carCol == state.game_startcol || carCol == state.game_startcol2) &&
        (playerRow == state.game_startrow || playerRow == state.game_startrow2)) {
        *penaltyCounter = 0;
        return 0;
    }
    if (carCol < 0 || carCol > 0x1D || playerRow < 0 || playerRow > 0x1D)
        goto invalid_coords;
    leastDistance = 0;
    searchDepth = 0;
    distance = 0;
    for (cur = 0; cur < track_pieces_counter; cur++)
        mark[cur] = 0;
    cur = *trackIndex;
    for (;;) {
        mapIdx = td01_track_file_cpy[cur];
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
            state.game_startcol2 = state.game_startcol = carCol;
            state.game_startrow2 = state.game_startrow = playerRow;
invalid_coords:
            *penaltyCounter = -2;
            return 1;
        }
        mark[mapIdx] = 1;
        row = td22_row_from_path[mapIdx];
        multiCell = trkObjectList[(unsigned char)td17_trk_elem_ordered[mapIdx]].ss_multiTileFlag;
        last_row = (multiCell & 1) ? row + 1 : row;
        tileX = td21_col_from_path[mapIdx];
        right_col = (multiCell & 2) ? tileX + 1 : tileX;
        if ((tileX == carCol || right_col == carCol) &&
            (row == playerRow || last_row == playerRow)) {
            if (td02_penalty_related[cur] != -1)
                mapIdx = cur;
            state.game_startcol = tileX;
            state.game_startcol2 = right_col;
            state.game_startrow = row;
            state.game_startrow2 = last_row;
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
        nextPiece = td02_penalty_related[cur];
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

void update_car_speed(char arg_carInputByte, char arg_MplayerFlag, struct CARSTATE* arg_carState, struct SIMD* arg_simd) {
	int knobStep;
	int offset;
	unsigned int updatedSpeed;
	int speedDelta;
	unsigned char currTorque;

	if (data_349D0 == 20)
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
				arg_carState->car_fpsmul2 = ((char)data_349D0 >> 1) + (char)data_349D0;
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
				if (data_349D0 == 10)
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
					currTorque = -(oppnentSped[0] - 200) >> 1;
					if (currTorque != 0)
						speedDelta -= (long)currTorque * speedDelta / 200;
				}
				if (speedDelta > 0x128)
					arg_carState->car_engineLimiterTimer = 5;
			}
			break;
		}
	}
	if (data_349D0 == 10)
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
	if (arg_carState->car_speed2 > state.game_topSpeed)
		state.game_topSpeed = arg_carState->car_speed2;
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
        car->car_speed2 -= car->car_speed2 / word_2BDF8[scratch];
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

                switch (td14_elem_map_main[terrainrows[rowCoord] + column]) {
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

                switch (td14_elem_map_main[terrainrows[rowCoord] + column]) {
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
                car->car_speed2 = multiply_and_scale(
                    cos_fast(car->car_36MwhlAngle), car->car_speed2);
                if (cos_fast(car->car_36MwhlAngle) < 0)
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
    playerSin = multiply_and_scale(currentSpeed >> 8, sin_fast(pHeading));
    opponentSin = multiply_and_scale(opponentSpeed >> 8, sin_fast(opponentHeading));
    playerCosAngle = multiply_and_scale(currentSpeed >> 8, cos_fast(pHeading));
    opponentCosAng = multiply_and_scale(opponentSpeed >> 8, cos_fast(opponentHeading));
    distanceToCar = polarRadius2D(opponentSin - playerSin, opponentCosAng - playerCosAngle);
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

void upd_statef20_from_steer_input(char input)
{
    register int response;
    char responseIndex;
    register int oldAngle;

    oldAngle = state.playerstate.car_steeringAngle;
    responseIndex = (char)(((unsigned short)state.playerstate.car_speed2 >> 10) & 0xFC);
    response = steerWhlRespTable_ptr[(char)responseIndex + input];

    if (response > 0) {
        if (oldAngle < -1) response <<= 2;
    } else if (response != 0 && oldAngle > 1) {
        response <<= 2;
    }

    if (response == 0 && state.playerstate.car_speed2 != 0 && oldAngle != 0) {
        response = steerWhlRespTable_ptr[(char)responseIndex + 1] * 2;
        if ((oldAngle < 0 ? -oldAngle : oldAngle) > response) {
            if (oldAngle > 0) response = -response;
        } else {
            response = -state.playerstate.car_steeringAngle;
        }
    }

    if (data_349D0 == 10) {
        if (response > 160) response = 160;
        if (response < -160) response = -160;
    } else {
        if (response > 80) response = 80;
        if (response < -80) response = -80;
    }
    oldAngle += response;
    if (oldAngle > 240) oldAngle = 240;
    if (oldAngle < -240) oldAngle = -240;

    if (steerWhlRespTable_ptr[(char)responseIndex + input] == 0 &&
        abs(oldAngle) < 8)
        oldAngle = 0;
    state.playerstate.car_steeringAngle = oldAngle;
}

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

    if (is_in_replay != 0) {
        if (byte_459D8 != 0) {
            word_44D1E = word_449E4;
            if ((byte_42D26 & 6) != 0)
                audio_op_unk7(word_43964);
            if ((byte_42D26 & 1) != 0)
                audio_function2(word_43964);
            if (gameconfig.game_opponenttype != 0) {
                if ((byte_42D2A & 6) != 0)
                    audio_op_unk7(word_4408C);
                if ((byte_42D2A & 1) != 0)
                    audio_function2(word_4408C);
            }
            byte_459D8 = 0;
            byte_42D26 = 0;
            byte_42D2A = 0;
        }
        if (is_in_replay == byte_3BE02)
            goto audio_done;
        sub_38178();
        goto audio_done;
    }

    playerPosOld.x = (short)(state.playerstate.car_posWorld2.lx >> 6);
    playerPosOld.y = (short)(state.playerstate.car_posWorld2.ly >> 6);
    playerPosOld.z = (short)(state.playerstate.car_posWorld2.lz >> 6);
    playerPosition.x = (short)(state.playerstate.car_posWorld1.lx >> 6);
    playerPosition.y = (short)(state.playerstate.car_posWorld1.ly >> 6);
    playerPosition.z = (short)(state.playerstate.car_posWorld1.lz >> 6);
    if (gameconfig.game_opponenttype != 0) {
        opponentPrior.x = (short)(state.opponentstate.car_posWorld2.lx >> 6);
        opponentPrior.y = (short)(state.opponentstate.car_posWorld2.ly >> 6);
        opponentPrior.z = (short)(state.opponentstate.car_posWorld2.lz >> 6);
        opponentCurrent.x = (short)(state.opponentstate.car_posWorld1.lx >> 6);
        opponentCurrent.y = (short)(state.opponentstate.car_posWorld1.ly >> 6);
        opponentCurrent.z = (short)(state.opponentstate.car_posWorld1.lz >> 6);
    }

    switch (cameramode) {
    case 0:
    case 2:
        if (followOpponentFlag != 0) {
            targetNow = opponentCurrent;
            targetPast = opponentPrior;
        } else {
            targetNow = playerPosition;
            targetPast = playerPosOld;
        }
        break;
    case 1:
        targetNow = state.game_vec1[followOpponentFlag];
        targetPast = state.game_vec3[followOpponentFlag];
        break;
    case 3:
        targetNow.x = ((struct VECTOR far *)trackdata9)[state.field_3F7[followOpponentFlag]].x;
        targetNow.y = ((struct VECTOR far *)trackdata9)[state.field_3F7[followOpponentFlag]].y + word_44D20 + 90;
        targetNow.z = ((struct VECTOR far *)trackdata9)[state.field_3F7[followOpponentFlag]].z;
        targetPast = targetNow;
        break;
    }

    audioRecord = &((struct AUDIO_CAR_FRAME *)unk_44F4C)[word_449E4];
    audioRecord->player_offsets[0] = targetPast.x - playerPosOld.x;
    audioRecord->player_offsets[1] = targetPast.y - playerPosOld.y;
    audioRecord->player_offsets[2] = targetPast.z - playerPosOld.z;
    audioRecord->player_offsets[3] = targetNow.x - playerPosition.x;
    audioRecord->player_offsets[4] = targetNow.y - playerPosition.y;
    audioRecord->player_offsets[5] = targetNow.z - playerPosition.z;
    audioRecord->player_rpm = state.playerstate.car_currpm;

    if (gameconfig.game_opponenttype != 0) {
        audioRecord->opponent_offsets[0] = targetPast.x - opponentPrior.x;
        audioRecord->opponent_offsets[1] = targetPast.y - opponentPrior.y;
        audioRecord->opponent_offsets[2] = targetPast.z - opponentPrior.z;
        audioRecord->opponent_offsets[3] = targetNow.x - opponentCurrent.x;
        audioRecord->opponent_offsets[4] = targetNow.y - opponentCurrent.y;
        audioRecord->opponent_offsets[5] = targetNow.z - opponentCurrent.z;
        audioRecord->opponent_rpm = state.opponentstate.car_currpm;
        carCount = 2;
    } else {
        carCount = 1;
    }

    for (carIndex = 0; carIndex < carCount; carIndex++) {
        if (carIndex != 0) {
            selectedCar = &state.opponentstate;
            audioId = word_4408C;
            soundMode = byte_42D2A;
        } else {
            selectedCar = &state.playerstate;
            audioId = word_43964;
            soundMode = byte_42D26;
        }
        if (selectedCar->field_CF & 1) {
            if (!(soundMode & 1)) {
                soundMode |= 1;
                audio_op_unk(audioId);
            }
        } else if (soundMode & 1) {
            soundMode--;
            audio_function2(audioId);
        }
        if (selectedCar->field_CF & 6) {
            if ((soundMode & 6) != (selectedCar->field_CF & 6)) {
                if (soundMode & 6)
                    goto stop_skid;
                if (selectedCar->field_CF & 2) {
                    audio_op_unk5(audioId);
                    soundMode += 2;
                } else {
                    audio_op_unk6(audioId);
                    soundMode += 4;
                }
            }
        } else if (soundMode & 6) {
stop_skid:
            if (soundMode & 2)
                soundMode -= 2;
            if (soundMode & 4)
                soundMode -= 4;
            audio_op_unk7(audioId);
        }
        if (carIndex != 0)
            byte_42D2A = soundMode;
        else
            byte_42D26 = soundMode;
    }
    byte_459D8 = 1;
    word_449E4++;
    if (word_449E4 == 40)
        word_449E4 = 0;

audio_done:
    byte_3BE02 = is_in_replay;
}

void audio_unk3(char flags, short audioId) { if (byte_459D8 != 0) { if (flags & 0x10) audio_op_unk4(audioId); if (flags & 0x20) audio_op_unk3(audioId); } }

void sub_18D06(struct AUDIO_CAR_FRAME *record, short value)
{
    audio_op_unk2(word_43964, record->player_rpm,
        record->player_offsets[0], record->player_offsets[1],
        record->player_offsets[2], record->player_offsets[3],
        record->player_offsets[4], record->player_offsets[5], value);
    if (gameconfig.game_opponenttype != 0)
        audio_op_unk2(word_4408C, record->opponent_rpm,
            record->opponent_offsets[0], record->opponent_offsets[1],
            record->opponent_offsets[2], record->opponent_offsets[3],
            record->opponent_offsets[4], record->opponent_offsets[5], value);
}

char sub_18D60(int trackIndex, struct TRACKRESULT *result, char side,
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

    entry = (unsigned char)td17_trk_elem_ordered[trackIndex];
    objectIndex = (unsigned char)trackdata18[trackIndex] & 0x0f;
    isConnected = (unsigned char)trackdata18[trackIndex] & 0x10;
    trackElemObject = &trkObjectList[entry];
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
        *opponentSpeed = oppnentSped[trackRow + scratch];
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

    scratch = (unsigned char)td21_col_from_path[trackIndex];
    trackRow = (unsigned char)td22_row_from_path[trackIndex];
    if (vectorA.y != -1 && td15_terr_map_main[terrainrows[trackRow] + scratch] == 6) {
        vectorA.y += hillHeightConsts[1];
        edgePoint.y += hillHeightConsts[1];
    }

    if ((trackElemObject->ss_multiTileFlag & 1) != 0) {
        vectorA.z += trackpos[trackRow];
        edgePoint.z += trackpos[trackRow];
    } else {
        vectorA.z += trackcenterpos[trackRow];
        edgePoint.z += trackcenterpos[trackRow];
    }
    if ((trackElemObject->ss_multiTileFlag & 2) != 0) {
        vectorA.x += trackpos2[scratch + 1];
        edgePoint.x += trackpos2[scratch + 1];
    } else {
        vectorA.x += trackcenterpos2[scratch];
        edgePoint.x += trackcenterpos2[scratch];
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

    matrix = mat_rot_zxy(-pWorldCrds[1].x, -pWorldCrds[1].y,
                         -pWorldCrds[1].z, 0);
    for (index = 0; index < 4; ++index) {
        if (word_3BE04[index] == 0)
            point.x = pCollPoints[0].px;
        else
            point.x = -pCollPoints[0].px;
        point.y = 0;
        if (word_3BE0C[index] == 0)
            point.z = pCollPoints[1].px;
        else
            point.z = -pCollPoints[1].px;
        mat_mul_vector(&point, matrix, &result);
        result.x += pWorldCrds[0].x;
        result.y += pWorldCrds[0].y;
        result.z += pWorldCrds[0].z;
        transform[index] = result;
    }

    matrix = mat_rot_zxy(oWorldCrds[1].x, oWorldCrds[1].y,
                         oWorldCrds[1].z, 1);
    for (index = 0; index < 4; ++index) {
        point.x = oWorldCrds[0].x - transform[index].x;
        point.y = oWorldCrds[0].y - transform[index].y;
        point.z = oWorldCrds[0].z - transform[index].z;
        mat_mul_vector(&point, matrix, &result);
        if (!(result.y >= 0 && result.y <= oCollPoints[0].py &&
            result.x >= -oCollPoints[0].px && result.x <= oCollPoints[0].px &&
            result.z >= -oCollPoints[1].px && result.z <= oCollPoints[1].px))
            continue;
        return 1;
    }

    matrix = mat_rot_zxy(-oWorldCrds[1].x, -oWorldCrds[1].y,
                         -oWorldCrds[1].z, 0);
    for (index = 0; index < 4; ++index) {
        if (word_3BE04[index] == 0)
            point.x = oCollPoints[0].px;
        else
            point.x = -oCollPoints[0].px;
        point.y = 0;
        if (word_3BE0C[index] == 0)
            point.z = oCollPoints[1].px;
        else
            point.z = -oCollPoints[1].px;
        mat_mul_vector(&point, matrix, &result);
        result.x += oWorldCrds[0].x;
        result.y += oWorldCrds[0].y;
        result.z += oWorldCrds[0].z;
        transform[index] = result;
    }

    matrix = mat_rot_zxy(pWorldCrds[1].x, pWorldCrds[1].y,
                         pWorldCrds[1].z, 1);
    for (index = 0; index < 4; ++index) {
        point.x = pWorldCrds[0].x - transform[index].x;
        point.y = pWorldCrds[0].y - transform[index].y;
        point.z = pWorldCrds[0].z - transform[index].z;
        mat_mul_vector(&point, matrix, &result);
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
    init_game_state(-3);
    zeroValue = 0;
    state.game_inputmode = 2;
    planptr = plan_memres;
    startcol2 = 1;
    startrow2 = 28;
    td17_trk_elem_ordered[0] = 7; td21_col_from_path[0] = 1; td22_row_from_path[0] = startrow2; trackdata18[0] = 0;
    td17_trk_elem_ordered[1] = 6; td21_col_from_path[1] = 0; td22_row_from_path[1] = startrow2; trackdata18[1] = 0;
    td17_trk_elem_ordered[2] = 8; td21_col_from_path[2] = 0; td22_row_from_path[2] = startrow2 + 1; trackdata18[2] = 0;
    td17_trk_elem_ordered[3] = 9; td21_col_from_path[3] = 1; td22_row_from_path[3] = startrow2 + 1; trackdata18[3] = 0;
    td17_trk_elem_ordered[4] = 7; td21_col_from_path[4] = 1; td22_row_from_path[4] = startrow2; trackdata18[4] = 0;
    ((short far *)trackdata3)[0] = zeroValue;
    ((short far *)trackdata3)[1] = 1; ((short far *)trackdata3)[2] = 2; ((short far *)trackdata3)[3] = 3; ((short far *)trackdata3)[4] = 4;
    ((short far *)trackdata3)[5] = 1; ((short far *)trackdata3)[6] = 2; ((short far *)trackdata3)[7] = 3; ((short far *)trackdata3)[8] = 4;
    ((short far *)trackdata3)[9] = 1; ((short far *)trackdata3)[10] = 2; ((short far *)trackdata3)[11] = 3; ((short far *)trackdata3)[12] = 4;
    ((short far *)trackdata3)[13] = zeroValue; ((short far *)trackdata3)[14] = 1; ((short far *)trackdata3)[15] = 2; ((short far *)trackdata3)[16] = 3; ((short far *)trackdata3)[17] = zeroValue;
    oppnentSped[0] = 0xC8;
    init_carstate_from_simd(&state.opponentstate, &simd_opponent, 1,
        0x17700L, 0L, ((long)(trackpos[28] + 0x12E)) << 6, 0);
    sub_18D60(((short far *)trackdata3)[state.opponentstate.car_trackdata3_index],
        &state.opponentstate.car_vec_unk3, state.opponentstate.field_CE++,
        &state.field_3F9);
}

void do_opponent_op(void) { opponent_op(); }

void update_crash_state(int arg_someFlag, int arg_MplayerFlag) {
	char var_2;
	struct CARSTATE* var_cState;

	switch (arg_MplayerFlag) {
	case 0:
		var_cState = &state.playerstate;
		break;
	case 1:
		var_cState = &state.opponentstate;
		break;
	}
	if (var_cState->car_crashBmpFlag != 0)
		return;

	var_2 = 0;
	switch (arg_someFlag) {
	case 4:
		state.game_frame_in_sec = 1;
		state.game_frames_per_sec = 1;
		break;
	case 5:
		arg_someFlag = 1;
		var_2 = 1;
	case 1:
		var_cState->car_crashBmpFlag = 1;
		state_op_unk(arg_MplayerFlag, var_cState->car_rotate.x, 0);
		if (arg_MplayerFlag == 0) {
			state.game_impactSpeed = var_cState->car_speed2;
			state.game_frames_per_sec = data_349D0 << 2;
		}
		if (is_in_replay == 0 && byte_459D8 != 0) {
			if (arg_MplayerFlag == 0)
				audio_function2_wrap(word_43964);
			else
				audio_function2_wrap(word_4408C);
		}
		break;
	case 2:
		if (is_in_replay == 0 && byte_459D8 != 0) {
			if (arg_MplayerFlag == 0)
				audio_function2_wrap(word_43964);
			else
				audio_function2_wrap(word_4408C);
		}
		var_cState->car_crashBmpFlag = 2;
		var_2 = 1;
		if (arg_MplayerFlag == 0) {
			state.game_impactSpeed = var_cState->car_speed2;
			state.game_frames_per_sec = data_349D0 << 2;
		}
		break;
	case 3:
		var_cState->car_crashBmpFlag = 3;
		if (arg_MplayerFlag == 0) {
			state.game_total_finish = state.game_frame + state.game_penalty + elapsed_time1;
			state.game_frames_per_sec = data_349D0;
		} else {
			state.field_144 = state.game_frame + elapsed_time1;
		}
		break;
	}
	if (var_2 != 0) {
		var_cState->car_speed2 = 0;
		var_cState->car_speed = 0;
	}
	if (arg_MplayerFlag != 0)
		state.game_oEndFrame = state.game_frame;
	else
		state.game_pEndFrame = state.game_frame;
	if (state.game_3F6autoLoadEvalFlag == 0 && arg_MplayerFlag == 0)
		state.game_3F6autoLoadEvalFlag = arg_someFlag;
	if ((byte_43966 & 4) == 0)
		gState_travDist = *(struct GAMESTATE_SNAPSHOT *)&state.game_travDist;
}

void plane_rotate_op(void) {
	struct VECTOR rotatedVector;
	struct MATRIX rotationMatrix;
	struct MATRIX matrix;
	struct VECTOR vector;
	register int rotation;

	if (planindex_copy != -1) {
		if (planptr[planindex_copy].plane_xy == pState_minusRotate_x_2 &&
		    planptr[planindex_copy].plane_yz == pState_minusRotate_z_2) {
			rotation = pState_minusRotate_y_2;
		} else {
			mat_mul_vector(&vec_unk2, &mat_unk, &vector);
			matrix = planptr[planindex_copy].plane_rotation;
			mat_invert(&matrix, &rotationMatrix);
			mat_mul_vector(&vector, &rotationMatrix, &rotatedVector);
			rotation = polarAngle(-rotatedVector.x, rotatedVector.z);
		}
		if ((rotation += pState_f36Mminf40sar2) != 0) {
			if (word_3BE16 != rotation) {
				mat_rot_y(&mat_planetmp, -rotation);
				word_3BE16 = rotation;
			}
			mat_mul_vector(&vec_unk2, &mat_planetmp, &rotatedVector);
			mat_mul_vector2(&rotatedVector, &planptr[planindex_copy].plane_rotation, &vec_planerotopresult);
			return;
		}
		mat_mul_vector2(&vec_unk2, &planptr[planindex_copy].plane_rotation, &vec_planerotopresult);
		return;
	}
	if (pState_f36Mminf40sar2 != 0) {
		if (pState_f36Mminf40sar2 != f36f40_whlData) {
			mat_rot_y(&mat_unk2, -pState_f36Mminf40sar2);
			f36f40_whlData = pState_f36Mminf40sar2;
		}
		mat_mul_vector(&vec_unk2, &mat_unk2, &rotatedVector);
		mat_mul_vector(&rotatedVector, &mat_unk, &vec_planerotopresult);
		return;
	}
	mat_mul_vector(&vec_unk2, &mat_unk, &vec_planerotopresult);
}

int plane_origin_op(int arg_planindex, int x, int y, int z) {
	struct PLANE far* pPlane;
	struct VECTOR a;
	struct VECTOR b;
	
	if (arg_planindex == planindex) {
		pPlane = current_planptr;
	} else {
		pPlane = &planptr[arg_planindex];
	}

	b.y = pPlane->plane_origin.y + terrainHeight;
	a.y = y - b.y;
	if (arg_planindex < 4) {
		
		return a.y;
	}
	b.x = pPlane->plane_origin.x + elem_xCenter;
	b.z = pPlane->plane_origin.z + elem_zCenter;
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
    state.field_42A = 1;
    unusedCount = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex)
        if (state.field_38E[debrisIndex] == 0)
            ++unusedCount;
    if (unusedCount > countLimit) unusedCount = countLimit;

    made = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex) {
        if (state.field_38E[debrisIndex] == 0) {
            state.field_443[debrisIndex] = (char)mode;
            state.field_42B[debrisIndex] = (char)((made & 3) + phaseBase);
            state.game_longs1[debrisIndex] = 0;
            state.game_longs2[debrisIndex] = 0;
            state.game_longs3[debrisIndex] = 0;
            state.field_2FE[debrisIndex] = (short)(get_kevinrandom() << 2);
            state.field_32E[debrisIndex] = (short)(get_kevinrandom() << 2);
            state.field_35E[debrisIndex] = (short)((((long)speedFactor * made) / unusedCount + startAngle) & 0x3FF);
            verticalValue = (short)(((get_kevinrandom() * 6) >> 2) + speed + 0x180);
            state.field_38E[debrisIndex] = verticalValue;
            state.field_3BE[debrisIndex] = (short)((verticalStep * verticalValue) >> 2);
            if (++made == unusedCount)
                break;
        } else
            continue;
    }
}

void sub_19BA0(void)
{
    struct MATRIX *matrixPointer;
    char keepAlive = 0;
    register int i;
    for (i = 0; i < 24; ++i) {
        if (state.field_38E[i] != 0) {
            struct VECTOR inputVector;
            struct VECTOR outputVector;
            matrixPointer = mat_rot_zxy(0, 0, state.field_35E[i], 1);
            inputVector.x = 0;
            inputVector.y = 0;
            inputVector.z = state.field_38E[i];
            mat_mul_vector(&inputVector, matrixPointer, &outputVector);
            state.game_longs1[i] += outputVector.x;
            state.game_longs3[i] += outputVector.z;
            state.field_3BE[i] -= 0x13;
            state.game_longs2[i] += state.field_3BE[i];
            if (data_349D0 == 10) {
                state.field_3BE[i] -= 0x13;
                state.game_longs2[i] += state.field_3BE[i];
            }
            if (state.playerstate.car_posWorld1.ly + state.game_longs2[i] < 0) {
                state.field_38E[i] = 0;
            } else {
                keepAlive = 1;
                state.field_2FE[i] += 0x10;
                state.field_32E[i] += 0x10;
            }
        }
    }
    state.field_42A = keepAlive;
}
