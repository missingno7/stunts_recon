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
	char field_CF; 
};

struct GAMESTATE {
	long game_longs1[24]; 
	long game_longs2[24]; 
	long game_longs3[24]; 
	struct VECTOR game_vec1[2]; 
	struct VECTOR game_vec3;
	struct VECTOR game_vec4;
	short game_frame_in_sec;
	short game_frames_per_sec;
	long  game_travDist;
	unsigned short game_frame;
	short game_total_finish; 
	short field_144;
	short game_pEndFrame;
	short game_oEndFrame;   
	short game_penalty; 
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

struct SIMD {
	char num_gears;
	char simd_unk;
	short car_mass;
	short braking_eff;
	short idle_rpm;
	short downshift_rpm;
	short upshift_rpm;
	short max_rpm;
	unsigned short gear_ratios[7];
	struct POINT2D knob_points[7];
	short aero_resistance;
	char idle_torque;
	char torque_curve[104];
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
extern unsigned short elapsed_time2;
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
extern struct GAMESTATE far* cvxptr;
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
extern int far* td10_track_check_rel;
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

extern unsigned short gState_frame;
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
extern long gState_travDist;
extern short gState_total_finish_time;
extern short gState_144;
extern short gState_pEndFrame;
extern short gState_oEndFrame;
extern short gState_penalty;
extern short gState_impactSpeed;
extern short gState_topSpeed;
extern short gState_jumpCount;
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
void update_player_state(struct CARSTATE* arg_pState, struct SIMD* arg_pSimd, struct CARSTATE* arg_oState, struct SIMD* arg_oSimd, int arg_MplayerFlag);
void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short track_angle);
void init_game_state(short arg);
void restore_gamestate(unsigned short frame);
void update_gamestate();
void player_op(char arg_carInputByte);
int detect_penalty(int *trackIndex, int *penaltyCounter);
void update_car_speed(char arg_carInputByte, int arg_MplayerFlag, struct CARSTATE* arg_carState, struct SIMD* arg_simd);
void update_grip(struct CARSTATE *car, struct SIMD *simd, int isOpponent);
int car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent);
int carState_rc_op(struct CARSTATE *car, int value, int wheel);
void upd_statef20_from_steer_input(char input);
void audio_carstate(void);
void audio_unk3(char flags, short audioId);
void sub_18D06(struct AUDIO_CAR_FRAME *record, short value);
int sub_18D60(int trackIndex, struct TRACKRESULT *result, char side,
              char *opponentSpeed);
int car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
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

int vec_normalInnerProduct(int x, int y, int z, struct VECTOR far *normal)
{
    return (((long)normal->x * x) + ((long)normal->y * y) +
            ((long)normal->z * z)) / 0x2000;
}
