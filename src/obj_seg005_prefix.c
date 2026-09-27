/* Scratch TU. Header declarations are semantic leads from Restunts; no preprocessor directives. */
extern void sprite_copy_2_to_1_2(void);
extern void mouse_draw_transparent_check(void);



struct RECTANGLE {
	int left, right;
	int top, bottom;
	//int x1, y1;
	//int x2, y2;
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

struct MATRIX { int vals[9]; };;

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
void mat_mul_vector2(struct VECTOR* invec, struct MATRIX far* mat, struct VECTOR* outvec);
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

void plane_rotate_op(void);
int plane_origin_op(int index, int b, int c, int d);






struct GAMEINFO {
	char game_playercarid[4];
	char game_playermaterial;
	char game_playertransmission;
	char game_opponenttype;
	char game_opponentcarid[4];
	char game_opponentmaterial;
	char game_opponenttransmission;
	char game_trackname[9];
	char game_framespersec;
	short game_recordedframes;
};

struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; // applying the (x, y, z) vector notation to rotation
                              // angles is a source of confusion.
	short car_pseudoGravity;
	short car_steeringAngle;
	short car_currpm;
	short car_lastrpm;
	short car_idlerpm2;
	short car_speeddiff; // former gripdiff
	unsigned short car_speed;     // former trackgrip
                         // value is 2^8*(mph value) and unsigned
	unsigned short car_speed2;    // former trackgrip2
                         // speed is the rev-coupled speed, while speed2 is
                         // the actual car speed. They are different, for
                         // instance, during jumps (where accelerating increases
                         // revs without making the car go faster).
	unsigned short car_lastspeed; // former lasttrackgrip
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
	short car_rc1[4]; // four words, one for each wheel.
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
	char car_sumSurfAllWheels; // used as jump flag.
	char car_surfaceWhl[4];      // surface types for each of the wheels, it seems.
	char car_engineLimiterTimer;
	char car_slidingFlag;
	char field_C8;
	char car_crashBmpFlag;
	char car_changing_gear;
	char car_fpsmul2;
	char car_transmission;
	char field_CD;
	char field_CE; // is added?
	char field_CF; // is initialized?
};

struct GAMESTATE {
	long game_longs1[24]; // x
	long game_longs2[24]; // y
	long game_longs3[24]; // z
	struct VECTOR game_vec1[2]; // 0 = player, 1 = opponent
	struct VECTOR game_vec3[2]; // [0] player, [1] opponent
	short game_frame_in_sec;
	short game_frames_per_sec;
	long  game_travDist;
	unsigned short game_frame;
	short game_total_finish; // finish time + penalty when crossed finish line
	short field_144;
	short game_pEndFrame;
	short game_oEndFrame;   // former game_frame2
	short game_penalty; // probably penalty counter
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
	char field_3BE[48];
	char kevinseed[6];
	char field_3F4;
	char game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	char game_3F6autoLoadEvalFlag;
	char field_3F7[2]; // 0 = player, 1 = opponent
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
	short sliding;
	short surface_grip[4];
	char simd_unk3[10];
	struct POINT2D collide_points[2];
	short car_height;
	struct VECTOR wheel_coords[4];
	unsigned char steeringdots[62];
	struct POINT2D spdcenter;
	short spdnumpoints;
	unsigned char spdpoints[208];
	struct POINT2D revcenter;
	short revnumpoints;
	unsigned char revpoints[256];
	short far* aerorestable;
};

struct TRKOBJINFO {
	char  si_noOfBlocks;      // How many shapeInfo pieces compose the element. Arbitrary for the first piece, 0 for the following ones.
	char  si_entryPoint;      // Connectivity of the track element regarding tiles.
	char  si_exitPoint;
	char  si_entryType;        // Connectivity of the track element regarding element types.
	char  si_exitType;
	char  si_arrowType;        // Type of the element for determining penalty-arrow behaviour.
	short si_arrowOrient;      // Orientation angle for penalty-arrow purposes
	short* si_cameraDataOffset; // offset (0003B770)
	char  si_opp1;             //Appears to affect how the opponent AI approaches an element.
	char  si_opp2;
	char  si_opp3;
	char  si_oppSpedCode;
};

struct TRACKOBJECT {
	struct TRKOBJINFO* ss_trkObjInfoPtr; // offset (0003B770)
	short ss_rotY;           // Horizontal orientation of the element.
	struct SHAPE3D* ss_shapePtr;       // offset (0003B770)
	struct SHAPE3D* ss_loShapePtr;     // offset (0003B770)
	unsigned char  ss_ssOvelay;       // Renders additional sceneShapes over the current one.
	char  ss_surfaceType;    // Paintjob. FF will induce alternating paintjobs.
	char  ss_ignoreZBias;    // Appears to be Z-bias override flag, mostly used for roads and corners.
	char  ss_multiTileFlag;  // 0 = one-tile, 1 = two-tile vertical, 2 = two-tile horizontal, 3 = four-tile.
	char  ss_physicalModel;  // sets the physical model in build_track_object
	char  scene_unk5;        // always zero.
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
extern char video_flag5_is0;
extern short video_flag6_is1;

extern unsigned char byte_44A8A;
extern unsigned char byte_4552F;
extern unsigned short elapsed_time1;
extern unsigned short elapsed_time2;
extern unsigned char byte_449DA;
extern unsigned char byte_4393C;
extern char game_replay_mode; // 0 = playing, 1 = paused, 2 = replay
extern short word_44DCA;

extern short word_45A24; // current frame?
extern short word_45A00; // fps * 30
extern short word_4499C; // 100 / fps
extern short track_angle;
extern void* steerWhlRespTable_ptr;
extern void* steerWhlRespTable_10fps;
extern void* steerWhlRespTable_20fps;
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
extern char word_45D3E;
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

extern int dastbmp_y;
extern struct SHAPE2D far *dastshapeptr;
extern int dashbmp_y;
extern int roofbmpheight;
extern struct RECTANGLE* rectptr_unk;

extern void player_op(char);
extern void opponent_op(void);
extern void audio_carstate(void);
extern void setup_car_shapes(int);
extern void update_frame(char, struct RECTANGLE*);
extern void loop_game(int, int, int);
extern void set_frame_callback(void);
extern void mouse_minmax_position(int);
extern int kb_get_char(void);
extern void far update_crash_state(int, int);
extern void far do_mou_restext(void);
extern void far init_game_state(int);
extern char handle_ingame_kb_shortcuts(unsigned);

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
extern short far* td01_track_file_cpy; //trackdata1;
extern short far* td02_penalty_related; //trackdata2;
extern char far* trackdata3;
extern short far* td04_aerotable_pl; //trackdata4;
extern short far* td05_aerotable_op; //trackdata5;
extern char far* trackdata6;
extern char far* trackdata7;
extern int far* td08_direction_related; //trackdata8;
extern struct VECTOR far* trackdata9;
extern int far* td10_track_check_rel;// trackdata10;
extern char far* td11_highscores; //trackdata11;
extern char far* trackdata12;
extern char far* td13_rpl_header; //trackdata13;
extern unsigned char far* td14_elem_map_main; //trackdata14;
extern unsigned char far* td15_terr_map_main; //trackdata15;
extern char far* td16_rpl_buffer; //trackdata16;
extern char far* td17_trk_elem_ordered; //trackdata17;
extern char far* trackdata18;
extern unsigned char far* trackdata19;
extern char far* td20_trk_file_appnd; //trackdata20;
extern char far* td21_col_from_path; //trackdata21;
extern char far* td22_row_from_path; //trackdata22;
extern unsigned char far* trackdata23; // indexes into trkObjectList
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

extern int framespersec;
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
extern char textresprefix; // = 'e'
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

extern short sub_18D60(short car_trackdata3_index, struct VECTOR* car_vec_unk3, short field_CE, short* unk);
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
extern 
/* TU function declarations needed before address-ordered definitions. */
char far file_load_replay(const char *dir, const char *name);
short far file_write_replay(const char far *filename);
int far file_write_fatal(const char *filename, void far *buffer, unsigned long length);
void far remove_frame_callback(void);
void far replay_unk2(int mode);
void far replay_unk(void);
void far free_player_cars(void);
extern struct SPRITE far *wndsprite;
extern char aCarcoun[];
extern void far *eng1ptr;
extern void far *engptr;
extern int word_43964;
extern char byte_459D8;
extern char byte_42D26;
extern char byte_42D2A;
extern int word_4408C;
extern int word_44D1E;
extern int word_449E4;
extern int word_443F4;
extern void far *fontledresptr;
extern void far *sdgameresptr;
extern void far *planptr;
extern void far *wallptr;

void run_game(void);
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



struct ENGINESOUND {
	int es_unk0;
	int es_unk2;
	int es_unk4;
	int es_unk6;
	char far *es_names[10];
};
struct ENGINESOUND unk_3E7FC = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
struct ENGINESOUND unk_3E82C = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
char byte_3E85C[34] = { 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
	60, 64, 68, 72, 76, 84, 90, 98, 106, 114, 121, 127, 127, 127 };

/* run_game: reconstructed from the target listing (s005c) */
void loop_game(int mode, int frame_index, int frame_offset);
extern void far *locate_text_res(void far *res, char *name);

void run_game(void) {
	int dialog_values[2];
	int replay_key_code, replaybar_height, last_game_frame;
	struct RECTANGLE temp_rect;
	int previous_roof;
	register int dialog_result;

	last_game_frame = -1;
	rect_windshield.left = 0;
	rect_windshield.right = 320;
	previous_roof = -1;
	word_449EA = -1;
	run_game_random = get_kevinrandom() << 3;
	replaybar_toggle = 1;
	is_in_replay = 0;
	if (idle_expired != 0) {
		cameramode++;
		if (cameramode == 4) {
			cameramode = 0;
		}
		game_replay_mode = 2;
		if (file_load_replay(0, "default") != 0) {
			return;
		}
		track_setup();
	} else if (gameconfig.game_recordedframes == 0) {
		cameramode = 0;
		game_replay_mode = 1;
	} else {
		cameramode = 0;
		game_replay_mode = 2;
		is_in_replay = 1;
	}

	if (setup_player_cars() != 0) {
		free_player_cars();
		do_mer_restext();
	} else {
		kbormouse = 0;
		byte_449E6 = 0;
		byte_449DA = 1;
		set_frame_callback();
		game_replay_mode_copy = -1;
		byte_44346 = 0;
		byte_4432A = 0;
		byte_46467 = 0;
		dashb_toggle = 0;

		if (idle_expired != 0) {
			framespersec = gameconfig.game_framespersec;
			init_game_state(-1);
		} else if (is_in_replay == 0) {
			cameramode = 0;
			dashb_toggle = 1;
			show_penalty_counter = 0;
			framespersec = framespersec2;
			gameconfig.game_framespersec = framespersec2;
			init_game_state(-1);
			word_45D94 = 0;
			word_45D3E = 0;
			byte_4393C = 1;
			mouse_minmax_position(byte_3B8F2);
			game_replay_mode = 1;
			state.playerstate.car_posWorld1.lx += (long)multiply_and_scale(sin_fast(track_angle), -240) << 6;
			state.playerstate.car_posWorld1.lz += (long)multiply_and_scale(cos_fast(track_angle), -240) << 6;
			state.playerstate.car_posWorld1.ly += 0x580;
			byte_43966 = 1;
		} else {
			cameramode = 0;
			game_replay_mode = 2;
			word_44DCA = 500;
			framespersec = gameconfig.game_framespersec;
			restore_gamestate(0);
			restore_gamestate(gameconfig.game_recordedframes);
			while (gameconfig.game_recordedframes != state.game_frame) {
				if (input_do_checking(1) != 27)
					update_gamestate();
				else
					break;
			}
			elapsed_time2 = gameconfig.game_recordedframes;
		}

		for (;;) {
			while (state.game_frame != elapsed_time2) {
				if ((byte_3B8F2 != 0 || byte_3FE00 != 0) && game_replay_mode == 0) {
					replay_unk();
				}
				update_gamestate();
			}

			if (game_replay_mode == 0 && byte_449DA == 0 && state.game_inputmode != 0) {
				if (last_game_frame == state.game_frame)
					continue;
				last_game_frame = state.game_frame;
			}

			if (state.game_inputmode == 0 && game_replay_mode == 0) {
				elapsed_time2 = 0;
				gameconfig.game_recordedframes = 0;
				state.game_frame = 0;
			}

			if (slow_video_mgmt_copy != slow_video_mgmt) {
				slow_video_mgmt_copy = slow_video_mgmt;
				init_rect_arrays();
			}

			if (byte_46467 != 0) {
				input_push_status();
				audio_unk();
				dialog_result = show_dialog(2, 1, locate_text_res(gameresptr, "rbf"), -1, -1, dialogarg2, 0, 0);
				if (dialog_result == -1)
					dialog_result = 0;
				sub_372F4();
				word_3F88E = 0;
				input_pop_status();
				if (dialog_result != 0) {
					update_crash_state(4, 0);
					byte_449DA = 1;
				}
				byte_46467 = 0;
			}

			if (video_flag5_is0 != 0) {
				setup_mcgawnd2();
				byte_4432A = byte_44346;
			} else {
				sprite_copy_wnd_to_1();
			}

			if (game_replay_mode != game_replay_mode_copy || dashb_toggle != dashb_toggle_copy || replaybar_toggle != replaybar_toggle_copy || is_in_replay != is_in_replay_copy || followOpponentFlag != followOpponentFlag_copy) {
				game_replay_mode_copy = game_replay_mode;
				dashb_toggle_copy = dashb_toggle;
				replaybar_toggle_copy = replaybar_toggle;
				is_in_replay_copy = is_in_replay;
				followOpponentFlag_copy = followOpponentFlag;
				roofbmpheight_copy = 0;
				byte_449E2 = 0;

				if (game_replay_mode != 2 || idle_expired != 0 || (replaybar_toggle == 0 && is_in_replay == 0)) {
						replaybar_enabled = 0;
				} else {
					replaybar_enabled = 1;
					goto replaybar_done;
				}
replaybar_done:

				if (idle_expired != 0) {
					dashbmp_y_copy = 200;
				} else if (dashb_toggle != 0 && followOpponentFlag == 0) {
					if (game_replay_mode == 2 && replaybar_enabled != 0) {
						height_above_replaybar = 151;
					} else {
						height_above_replaybar = 200;
					}
					byte_449E2 = 1;
					roofbmpheight_copy = roofbmpheight;
					dashbmp_y_copy = dashbmp_y;
				} else if (game_replay_mode == 2 && replaybar_enabled != 0) {
					dashbmp_y_copy = 151;
				} else {
					dashbmp_y_copy = 200;
				}

				if (previous_roof != roofbmpheight_copy || dashbmp_y_copy != word_449EA || replaybar_height != height_above_replaybar) {
					byte_454A4 = video_flag6_is1;
					set_projection(35, dashbmp_y_copy / 6, 320, dashbmp_y_copy);
					rect_windshield.top = roofbmpheight_copy;
					rect_windshield.bottom = dashbmp_y_copy;
					previous_roof = roofbmpheight_copy;
					word_449EA = dashbmp_y_copy;
					replaybar_height = height_above_replaybar;
				}
			}

			if (byte_454A4 != 0) {
				byte_449D8[byte_4432A] = 0;
				if (byte_449E2 != 0) {
					sprite_set_1_size(0, 320, dashbmp_y_copy, height_above_replaybar);
					setup_car_shapes(1);
				}
				if (replaybar_enabled != 0) {
					sprite_set_1_size(0, 320, 0, 200);
					loop_game(1, state.game_frame, state.game_frame);
				}
			} else if (replaybar_enabled == 0) {
				byte_449D8[byte_4432A] = 0;
			}

			update_frame(byte_44346, &rect_windshield);
			if (dastbmp_y != 0 && byte_449E2 != 0) {
				if (slow_video_mgmt_copy != 0) {
					temp_rect.left = 0;
					temp_rect.right = 320;
					temp_rect.top = dastbmp_y;
					temp_rect.bottom = dashbmp_y_copy;
					if (rectptr_unk != 0) {
						rect_union(rectptr_unk, &temp_rect, rectptr_unk);
					}
				}
				shape2d_render_bmp_as_mask(dasmshapeptr);
				shape2d_op_unk4(dastshapeptr);
			}

			sub_19F14(&rect_windshield);
			if (byte_449E2 != 0) {
				sprite_set_1_size(0, 320, dashbmp_y_copy, height_above_replaybar);
				setup_car_shapes(2);
				sprite_set_1_size(0, 320, 0, 200);
			}

			if (byte_454A4 != 0) {
				byte_454A4--;
			}

			if (video_flag5_is0 != 0) {
				mouse_draw_opaque_check();
				setup_mcgawnd1();
				byte_44346 ^= 1;
				byte_4432A = byte_44346;
				mouse_draw_transparent_check();
			}

			if (game_replay_mode == 1 && byte_4393C == 0) {
				game_replay_mode = 0;
				framespersec = framespersec2;
				gameconfig.game_framespersec = framespersec2;
				init_game_state(-1);
			}

			if (idle_expired != 0) {
				if (kb_get_char() != 0 || byte_449DA != 0 || get_kb_or_joy_flags() != 0)
					break;
				continue;
			}

			if (byte_449DA != 0) {
				if (game_replay_mode == 0 && state.game_3F6autoLoadEvalFlag != 4)
					break;
				if (byte_449DA == 2)
					break;
				byte_449DA = 0;
				game_replay_mode = 2;
				mouse_minmax_position(0);
				loop_game(0, 0, 0);
				loop_game(2, 4, 0);
				is_in_replay = 1;
				audio_carstate();
			}

			if (game_replay_mode == 2) {
				loop_game(3, 0, 0);
				continue;
			}

			for (;;) {
				replay_key_code = kb_get_char();
				if (replay_key_code != 0) {
					handle_ingame_kb_shortcuts(replay_key_code);
				}
				switch (replay_key_code) {
				case 0x4800:
				case 0x4B00:
				case 0x4D00:
				case 0x5000:
					continue;
				}
				break;
			}

			if (game_replay_mode == 1) {
				mouse_get_state(&mouse_butstate, &mouse_xpos, &mouse_ypos);
				if ((mouse_butstate & 3) != 0 || (get_kb_or_joy_flags() & 0x30) != 0) {
					game_replay_mode = 0;
					byte_4393C = 0;
					framespersec = framespersec2;
					gameconfig.game_framespersec = framespersec2;
					init_game_state(-1);
				}
			}
		}

		if (video_flag5_is0 != 0 && get_0() != 0) {
			mouse_draw_opaque_check();
			setup_mcgawnd2();
			sub_35C4E(0, 0, 320, 200, 0);
			setup_mcgawnd1();
			mouse_draw_transparent_check();
		}

		sprite_copy_2_to_1_2();
		is_in_replay = 1;
		audio_carstate();
		audio_remove_driver_timer();
		if (game_replay_mode == 0 && gameconfig.game_opponenttype != 0 && state.opponentstate.car_crashBmpFlag == 0) {
			show_dialog(3, 0, locate_text_res(gameresptr, "cop"), -1, 80, performGraphColor, dialog_values, 0);
			word_45D3E = 1;
			dialog_result = framespersec;
			dialog_result--;
			do {
				replay_unk2(1);
				update_gamestate();
				if (++dialog_result == framespersec) {
					dialog_result = 0;
					format_frame_as_string(&resID_byte1, state.game_frame + elapsed_time1, 1);
					mouse_draw_opaque_check();
					sub_345BC(&resID_byte1, font_op2_alt(&resID_byte1), dialog_values[1]);
					mouse_draw_transparent_check();
				}
			} while (input_do_checking(1) != 27 && state.opponentstate.car_crashBmpFlag == 0 &&
				 1500 * framespersec != state.game_frame + elapsed_time1);
		}

		word_45D3E = 0;
		mouse_minmax_position(0);
		remove_frame_callback();
		free_player_cars();
	}

	waitflag = 100;
	check_input();
	show_waiting();
}

extern char byte_33950;
extern char byte_349AA;
extern char byte_349BA;
extern char HKeyFlag;
char handle_ingame_kb_shortcuts(unsigned key)
{
    switch (key) {
    case 0x1B:
        if (game_replay_mode == 0) {
            update_crash_state(4, 0);
        }
        byte_449DA = 1;
        break;
    case 0x3C00:
        cameramode = 1;
        break;
    case 0x3D00:
        cameramode = 2;
        break;
    case 0x3E00:
        cameramode = 3;
        break;
    case 0x48:
    case 0x68:
        HKeyFlag ^= 1;
        break;
    case 0x4D:
    case 0x6D:
        do_mou_restext();
        mouse_minmax_position(byte_3B8F2);
        break;
    case 0x44:
    case 0x64:
        byte_33950 ^= 1;
        break;
    case 0x52:
    case 0x72:
        replaybar_toggle ^= 1;
        break;
    case 0x43:
    case 0x63:
        if (game_replay_mode != 1) {
            cameramode++;
            if (cameramode == 4) {
                cameramode = 0;
            }
        }
        break;
    case 0x3B00:
        cameramode = 0;
        break;
    case 0x74:
        if (byte_349AA != 0) {
            followOpponentFlag ^= 1;
        }
        break;
    default:
        if (game_replay_mode == 1) {
            game_replay_mode = 0;
            byte_4393C = 0;
            framespersec = framespersec2;
            gameconfig.game_framespersec = framespersec2;
            init_game_state(-1);
            return 1;
        }
        return 0;
    }
    return 1;
}

/* semantic lead: init_unknown from restunts.c */
void init_unknown(void)
{
	register int zero;
	byte_44A8A = 1;
	byte_4552F = 2;
	zero = 0;
	elapsed_time2 = zero;
	byte_4393C = byte_449DA = 0;
	word_44DCA = zero;
}

typedef void (far *callback_t)(void);
extern void far frame_callback(void);
extern void far timer_reg_callback(callback_t callback);
extern unsigned word_46468;
extern unsigned char byte_442E4;
void far set_frame_callback(void) {
    word_46468 = 0;
    timer_reg_callback(frame_callback);
    byte_442E4 = 0;
}


extern unsigned long far timer_get_counter_unk(unsigned long ticks);
extern void far timer_remove_callback(callback_t callback);
void far remove_frame_callback(void)
{
    (void)timer_get_counter_unk(10L);
    timer_remove_callback(frame_callback);
}

extern int far compare_ds_ss(void);
extern void far sub_18D06(char* record, int frame_count);
extern short word_345CC;
extern short word_345CE;
extern char unk_44F4C[];
void far frame_callback(void)
{
    if (compare_ds_ss() == 0) {
        return;
    }
    if (byte_442E4 != 0) {
        return;
    }

    byte_442E4++;
    if (byte_442E4 != 1) {
        goto frame_callback_done;
    }

    word_443F4++;
    if (word_443F4 >= word_4499C && word_44D1E != word_449E4) {
        sub_18D06(&unk_44F4C[word_44D1E * 0x22], word_443F4);
        word_443F4 = 0;
        word_44D1E++;
        if (word_44D1E == 0x28) {
            word_44D1E = 0;
        }
    }

    if (byte_449DA != 0 || byte_46467 != 0) {
        goto frame_callback_done;
    }
    if (is_in_replay != 0 && game_replay_mode == 2) {
        goto frame_callback_done;
    }
    if (game_replay_mode == 0 && word_345CC >= word_345CE) {
        is_in_replay = 1;
        audio_carstate();
        goto frame_callback_done;
    }

    byte_44A8A--;
    if (byte_44A8A == 0) {
        byte_44A8A = (char)word_4499C;
        word_46468++;
        if (game_replay_mode == 2) {
            switch (byte_449E6) {
            case 2:
                byte_4552F--;
                if (byte_4552F == 0) {
                    replay_unk2(0);
                    byte_4552F = 2;
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
    byte_442E4--;
}

extern signed char byte_44292[64];
extern unsigned char byte_442EA[64];
extern char byte_40D6A;
extern int far kb_get_key_state(int);
extern char far sub_307E3(void);
extern int far abs(int);
void far replay_unk2(int mode)
{
    register int input;
    register int i;

    if (mode != 0) {
        input = 0;
        goto record_input;
    }
    if (game_replay_mode == 2) {
        if (elapsed_time2 < gameconfig.game_recordedframes) {
            elapsed_time2++;
            return;
        }
        if (byte_449DA != 0)
            return;
        is_in_replay = 1;
        audio_carstate();
replay_finished:
        byte_449DA = 1;
        return;
    }
    if (byte_449DA != 0 || state.game_3F6autoLoadEvalFlag != 0 || game_replay_mode == 1) {
        input = 0;
        goto record_input;
    }
    if (passed_security == 0 && byte_4393C == 0 && framespersec * 4 < state.game_frame)
        update_crash_state(1, 0);
    if (byte_3B8F2 != 0 || byte_3FE00 != 0) {
        if (byte_3B8F2 != 0) {
            mouse_get_state(&mouse_butstate, &mouse_xpos, &mouse_ypos);
            i = mouse_xpos - 0xA0;
            if (abs(i) < 0x12)
                i = 0;
            else if (i > 0)
                i -= 0x12;
            else
                i += 0x12;
            byte_40D6A = i;
            if (mouse_butstate & 1)
                input = 2;
            else if (mouse_butstate & 2)
                input = 1;
            else
                input = 0;
        } else {
            byte_40D6A = sub_307E3();
            if (byte_40D6A > 0)
                byte_40D6A = byte_3E85C[byte_40D6A];
            else if (byte_40D6A < 0)
                byte_40D6A = -byte_3E85C[-byte_40D6A];
            input = get_kb_or_joy_flags() & 0x33;
        }
        i = elapsed_time2 & 0x3F;
        byte_44292[i] = byte_40D6A;
        byte_442EA[i] = 1;
    } else {
        input = get_kb_or_joy_flags();
    }
    if (kb_get_key_state(0x1E))
        input |= 0x10;
    if (kb_get_key_state(0x2C))
        input |= 0x20;
record_input:
    if (1500 * framespersec <= elapsed_time2 + elapsed_time1) {
        update_crash_state(4, 0);
        goto replay_finished;
    }
    if (elapsed_time2 == 12000) {
        if (elapsed_time1 == 0 && word_45D3E == 0) {
            word_45D3E = 1;
            byte_46467 = 1;
            return;
        }
        for (i = 0; i < 12000 / (30 * framespersec) - 1; i++) {
            cvxptr[i + 1].game_frame -= 30 * framespersec;
            cvxptr[i] = cvxptr[i + 1];
        }
        for (i = 0; i < 12000 - 30 * framespersec; i++)
            td16_rpl_buffer[i] = td16_rpl_buffer[i + 30 * framespersec];
        elapsed_time2 -= 30 * framespersec;
        gameconfig.game_recordedframes -= 30 * framespersec;
        elapsed_time1 += 30 * framespersec;
        state.game_frame -= 30 * framespersec;
    }
    td16_rpl_buffer[elapsed_time2++] = input;
    gameconfig.game_recordedframes++;
}

extern char byte_4616E;
void far sub_2298C(void)
{
    int nearest_range;
    int car_count;
    long dz;
    struct CARSTATE *selected_car_state;
    struct VECTOR car_position;
    register int index;
    register int delta;
    short bearing;
    short facing;
    int cam_y;
    int climb;
    char track_segment;
    struct VECTOR focus;
    int point_range;
    long dx;
    int threshold;

    car_count = 1;
    if (gameconfig.game_opponenttype != 0)
        car_count = 2;
    for (index = 0; index < car_count; ++index) {
        state.game_vec3[index] = state.game_vec1[index];
        if (index == 0)
            selected_car_state = &state.playerstate;
        else
            selected_car_state = &state.opponentstate;

        car_position.y = (short)(selected_car_state->car_posWorld1.ly >> 6);
        car_position.x = (short)(selected_car_state->car_posWorld1.lx >> 6);
        car_position.z = (short)(selected_car_state->car_posWorld1.lz >> 6);
        focus = selected_car_state->car_vec_unk3;
        facing = selected_car_state->field_48;

        if (index == 0 && (state.field_45B != 0 || state.field_45C != 0))
            focus = car_position;
        else if (selected_car_state->field_B6 != 0 || selected_car_state->car_crashBmpFlag != 0 ||
                 selected_car_state->car_trackdata3_index == -1 ||
                 (facing > 0x80 && facing < 0x380))
            focus = car_position;

        threshold = 0x1C2;
        cam_y = car_position.y + 0x10E;
        climb = state.game_vec1[index].y - cam_y;
        if (climb != 0) {
            delta = climb;
            if (delta > 0x1E)
                delta = 0x1E;
            else if (delta < -0x1E)
                delta = -0x1E;
            state.game_vec1[index].y -= delta;
        }

        bearing = polarAngle(focus.x - state.game_vec1[index].x,
                                  focus.z - state.game_vec1[index].z);
        delta = polarRadius2D(car_position.x - state.game_vec1[index].x,
                              car_position.z - state.game_vec1[index].z);
        if (delta > threshold) {
            delta -= threshold;
            if (framespersec == 0x14) {
                if (delta > 0x78) delta = 0x78;
            } else if (delta > 0xF0) {
                delta = 0xF0;
            }
            state.game_vec1[index].x += multiply_and_scale(delta, sin_fast(bearing));
            state.game_vec1[index].z += multiply_and_scale(delta, cos_fast(bearing));
        }

        if (state.game_frame % (framespersec >> 1) == 0) {
            nearest_range = 0x2710;
            for (track_segment = 0; track_segment < byte_4616E; ++track_segment) {
                dx = (long)trackdata9[track_segment].x - car_position.x;
                dz = (long)trackdata9[track_segment].z - car_position.z;
                if ((dx < 0 ? -dx : dx) < nearest_range &&
                    (dz < 0 ? -dz : dz) < nearest_range) {
                    point_range = polarRadius2D((int)dx, (int)dz);
                    if (point_range < nearest_range) {
                        state.field_3F7[index] = (char)track_segment;
                        nearest_range = point_range;
                    }
                }
            }
        }
    }
}

/* semantic lead: file_load_replay from fileio.c */
char file_load_replay(const char* dir, const char* name)
{
	file_build_path(dir, name, ".rpl", g_path_buf);

	g_is_busy = 1;
	file_read_fatal(g_path_buf, td13_rpl_header);
	gameconfig = *(struct GAMEINFO far*)td13_rpl_header;
	g_is_busy = 0;
	return 0;
}

/* semantic lead: file_write_replay from fileio.c */
short file_write_replay(const char* filename)
{
	register int ret;
	long write_length;

	*(struct GAMEINFO far*)td13_rpl_header = gameconfig;
	write_length = gameconfig.game_recordedframes + 0x724;
	g_is_busy = 1;
	ret = file_write_fatal(filename, td13_rpl_header, write_length);
	g_is_busy = 0;
	return (char)ret;
}

struct SHAPE2D {
    short s2d_width;
    unsigned short s2d_height;
    unsigned short s2d_unk1;
    unsigned short s2d_unk2;
    unsigned short s2d_pos_x;
    unsigned short s2d_pos_y;
    unsigned char s2d_unk3;
    unsigned char s2d_unk4;
    unsigned char s2d_unk5;
    unsigned char s2d_unk6;
};
struct SPRITE {
    struct SHAPE2D far *sprite_bitmapptr;
    unsigned short sprite_unk1;
    unsigned short sprite_unk2;
    unsigned short sprite_unk3;
    unsigned int *sprite_lineofs;
    unsigned short sprite_left;
    unsigned short sprite_right;
    unsigned short sprite_top;
    unsigned short sprite_height;
    unsigned short sprite_pitch;
    unsigned short sprite_unk4;
    unsigned short sprite_width2;
    unsigned short sprite_left2;
    unsigned short sprite_widthsum;
};

extern void far *stdaresptr;
extern void far *stdbresptr;
extern struct SHAPE2D far *whlshapes[10];
extern struct SHAPE2D far *gnobshapes[6];
extern struct SHAPE2D far *digshapes[10];
extern struct SPRITE far *whlsprite1;
extern struct SPRITE far *whlsprite2;
extern struct SPRITE far *whlsprite3;
extern char aWhl1whl2whl3ins2gboxins1i[];
extern char aGnobgnabdotDotadot1dot2[];
extern char aDig0dig1dig2dig3dig4dig5d[];
extern char aDash[];
extern char aRoof[];
extern char aRoof_0[];
extern char aRoof_1[];
extern char aRoof_2[];
extern char aDash_0[];
extern char aDast[];
extern char aDasm[];
extern void far *file_load_resource(int type, const char *name);
extern void locate_many_resources(void far *data, char *names, char far **result);
extern char far *locate_shape_nofatal(void far *data, char *name);
extern char far *locate_shape_fatal(void far *data, char *name);
extern struct SPRITE far *sprite_make_wnd(unsigned int width, unsigned int height, unsigned int color);
extern void sprite_free_wnd(struct SPRITE far *sprite);
extern void sprite_set_1_from_argptr(struct SPRITE far *sprite);
extern void sprite_copy_2_to_1(void);
extern void sprite_putimage_and_alt(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_and_alt2(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_or_alt(struct SHAPE2D far *shape, int x, int y);
extern void shape2d_op_unk(struct SHAPE2D far *shape);
extern void shape2d_op_unk2(struct SHAPE2D far *shape, int x, int y);
extern void shape2d_op_unk3(struct SHAPE2D far *shape);
extern void shape2d_op_unk5(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_or(struct SHAPE2D far *shape, int x, int y);
extern void sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
extern void preRender_line(int x1, int y1, int x2, int y2, int color);
extern void far *mmgr_free(char far *ptr);
extern char byte_40DF0[];
extern char byte_454A4;
extern short meter_needle_color;
extern short word_40DF2[];
extern short word_40DF6[];
extern char byte_40DFA[];
extern short word_40D6C[];
extern short word_40D70[];
extern short word_40D74[];
extern short word_40D78[];
extern short word_40E00[];
char aStdaxxxx[] = "stdaxxxx";
char aStdbxxxx[] = "stdbxxxx";
void far setup_car_shapes(int mode)
{
    struct SHAPE2D far *shape;
    int digit;
    char speedo_type;
    int steering;
    char wheel_zone;
    unsigned char *dot;
    register int x;
    unsigned char x_pos;
    register int y;
    char changed;
    unsigned char y_pos;
    char knob_removed;
    char has_digit;

    switch (mode) {
    case 0:
        aStdaxxxx[4] = gameconfig.game_playercarid[0];
        aStdaxxxx[5] = gameconfig.game_playercarid[1];
        aStdaxxxx[6] = gameconfig.game_playercarid[2];
        aStdaxxxx[7] = gameconfig.game_playercarid[3];
        aStdbxxxx[4] = gameconfig.game_playercarid[0];
        aStdbxxxx[5] = gameconfig.game_playercarid[1];
        aStdbxxxx[6] = gameconfig.game_playercarid[2];
        aStdbxxxx[7] = gameconfig.game_playercarid[3];
        stdaresptr = file_load_resource(3, aStdaxxxx);
        stdbresptr = file_load_resource(2, aStdbxxxx);
        locate_many_resources(stdaresptr, "whl1whl2whl3ins2gboxins1ins3inm1inm3", (char far **)whlshapes);
        locate_many_resources(stdbresptr, "gnobgnabdot dotadot1dot2", (char far **)gnobshapes);
        if (simd_player.spdcenter.py == 0)
            locate_many_resources(stdbresptr, "dig0dig1dig2dig3dig4dig5dig6dig7dig8dig9", (char far **)digshapes);
        whlsprite1 = sprite_make_wnd(whlshapes[3]->s2d_width * video_flag1_is1, whlshapes[3]->s2d_height, 15);
        whlsprite2 = sprite_make_wnd(whlshapes[4]->s2d_width * video_flag1_is1, whlshapes[4]->s2d_height, 15);
        whlsprite3 = sprite_make_wnd(whlshapes[4]->s2d_width * video_flag1_is1, whlshapes[4]->s2d_height, 15);
        shape = (struct SHAPE2D far *)locate_shape_fatal(stdaresptr, "dash");
        sprite_set_1_from_argptr(whlsprite3);
        shape2d_op_unk2(shape, shape->s2d_pos_x - whlshapes[4]->s2d_pos_x,
                        shape->s2d_pos_y - whlshapes[4]->s2d_pos_y);
        sprite_copy_2_to_1();
        dashbmp_y = shape->s2d_pos_y;
        if (locate_shape_nofatal(stdaresptr, "roof") != 0)
            roofbmpheight = ((struct SHAPE2D far *)locate_shape_fatal(stdaresptr, "roof"))->s2d_height;
        else
            roofbmpheight = 0;
        shape = (struct SHAPE2D far *)locate_shape_nofatal(stdaresptr, "dast");
        if (shape != 0) {
            dastbmp_y = shape->s2d_pos_y;
            dastshapeptr = shape;
            dasmshapeptr = locate_shape_fatal(stdaresptr, "dasm");
            return;
        }
        dastbmp_y = 0;
        return;
    case 1:
        mouse_draw_opaque_check();
        if (locate_shape_nofatal(stdaresptr, "roof") != 0)
            shape2d_op_unk((struct SHAPE2D far *)locate_shape_fatal(stdaresptr, "roof"));
        shape2d_op_unk3((struct SHAPE2D far *)locate_shape_fatal(stdaresptr, "dash"));
        shape2d_op_unk3(whlshapes[1]);
        mouse_draw_transparent_check();
        x = 0;
        byte_40DFA[byte_4432A] = byte_449D8[byte_4432A] = 0;
        word_40DF6[byte_4432A] = x;
        byte_40DF0[byte_4432A] = (char)x;
        x--;
        word_40E00[byte_4432A] = x;
        word_40D78[byte_4432A] = x;
        word_40D6C[byte_4432A] = x;
        return;
    case 2:
        if ((state.playerstate.car_changing_gear | state.playerstate.car_fpsmul2) == 0 &&
            byte_40DFA[byte_4432A] != 0) {
            if (video_flag5_is0 == 0)
                mouse_draw_opaque_check();
            sprite_set_1_size(0, 0x140, 0, height_above_replaybar);
            sprite_putimage_and_alt(whlsprite3->sprite_bitmapptr, whlshapes[4]->s2d_pos_x, whlshapes[4]->s2d_pos_y);
            byte_40DFA[byte_4432A] = 0;
        } else if (byte_40DFA[byte_4432A] != state.playerstate.car_changing_gear ||
                   word_40D70[byte_4432A] != state.playerstate.car_knob_x ||
                   word_40D74[byte_4432A] != state.playerstate.car_knob_y ||
                   (state.playerstate.car_fpsmul2 != 0 && byte_40DFA[byte_4432A] == 0)) {
            sprite_set_1_from_argptr(whlsprite2);
            byte_40DFA[byte_4432A] = 1;
            shape2d_op_unk2(whlshapes[4], 0, 0);
            x = state.playerstate.car_knob_x;
            y = state.playerstate.car_knob_y;
            word_40D70[byte_4432A] = x;
            word_40D74[byte_4432A] = y;
            sprite_putimage_and_alt2(gnobshapes[1], x, y);
            sprite_putimage_or_alt(gnobshapes[0], x, y);
            if (video_flag5_is0 != 0)
                setup_mcgawnd2();
            else {
                sprite_copy_2_to_1_2();
                mouse_draw_opaque_check();
            }
            sprite_set_1_size(0, 0x140, 0, height_above_replaybar);
            sprite_putimage_and_alt(whlsprite2->sprite_bitmapptr, whlshapes[4]->s2d_pos_x, whlshapes[4]->s2d_pos_y);
        }

        knob_removed = 0;
        steering = state.playerstate.car_steeringAngle / 8;
        wheel_zone = 1;
        if (steering < -10)
            wheel_zone = 0;
        else if (steering > 10)
            wheel_zone = 2;
        if (byte_40DF0[byte_4432A] != wheel_zone || byte_454A4 != 0) {
            if (video_flag5_is0 == 0)
                mouse_draw_opaque_check();
            if (word_40DF6[byte_4432A] != 0) {
                sprite_putimage_and_alt(gnobshapes[byte_44346 + 4], word_40DF2[byte_4432A], word_40DF6[byte_4432A]);
                word_40DF6[byte_4432A] = 0;
                knob_removed = 1;
            }
            switch (wheel_zone) {
            case 0:
                shape2d_op_unk3(whlshapes[0]);
                break;
            case 1:
                shape2d_op_unk3(whlshapes[1]);
                break;
            case 2:
                shape2d_op_unk3(whlshapes[2]);
                break;
            }
            byte_40DF0[byte_4432A] = wheel_zone;
            changed = 1;
        } else
            changed = 0;

        switch (simd_player.spdcenter.py) {
        case -1:
            x = 0;
            speedo_type = 2;
            break;
        default:
            speedo_type = 0;
            x = state.playerstate.car_speed / 0x280;
            if (x >= simd_player.spdnumpoints)
                x = simd_player.spdnumpoints - 1;
            break;
        case 0:
            speedo_type = 1;
            x = state.playerstate.car_speed >> 8;
        }
        y = (unsigned short)state.playerstate.car_currpm >> 7;
        if (y >= simd_player.revnumpoints)
            y = simd_player.revnumpoints - 1;

        if (changed != 0 || byte_454A4 != 0 ||
            word_40D78[byte_4432A] != x || word_40D6C[byte_4432A] != y) {
            if (video_flag5_is0 == 0)
                mouse_draw_opaque_check();
            if (word_40DF6[byte_4432A] != 0) {
                sprite_putimage_and_alt(gnobshapes[byte_44346 + 4], word_40DF2[byte_4432A], word_40DF6[byte_4432A]);
                word_40DF6[byte_4432A] = 0;
                knob_removed = 1;
            }
            sprite_set_1_from_argptr(whlsprite1);
            shape2d_op_unk5(whlshapes[3], 0, 0);
            word_40D78[byte_4432A] = x;
            word_40D6C[byte_4432A] = y;
            if (speedo_type == 1) {
                digit = 0;
                if (x >= 200) {
                    digit = 2;
                    x -= 200;
                } else if (x >= 100) {
                    digit = 1;
                    x -= 100;
                }
                if (digit != 0) {
                    sprite_putimage_or(digshapes[digit], simd_player.spdpoints[0], simd_player.spdpoints[1]);
                    has_digit = 1;
                }
                digit = x / 10;
                if (digit != 0 || has_digit != 0) {
                    sprite_putimage_or(digshapes[digit], simd_player.spdpoints[2], simd_player.spdpoints[3]);
                    x -= digit * 10;
                    has_digit = 1;
                }
                sprite_putimage_or(digshapes[x], simd_player.spdpoints[4], simd_player.spdpoints[5]);
            } else if (speedo_type == 0) {
                preRender_line(simd_player.spdcenter.px, simd_player.spdcenter.py,
                               simd_player.spdpoints[x * 2], simd_player.spdpoints[x * 2 + 1],
                               meter_needle_color);
            }
            preRender_line(simd_player.revcenter.px, simd_player.revcenter.py,
                           simd_player.revpoints[y * 2], simd_player.revpoints[y * 2 + 1],
                           meter_needle_color);
            switch (wheel_zone) {
            case 0:
                shape2d_render_bmp_as_mask(whlshapes[7]);
                shape2d_op_unk4(whlshapes[5]);
                break;
            case 2:
                shape2d_render_bmp_as_mask(whlshapes[8]);
                shape2d_op_unk4(whlshapes[6]);
                break;
            }
            if (video_flag5_is0 != 0)
                setup_mcgawnd2();
            else
                sprite_copy_2_to_1_2();
            sprite_set_1_size(0, 0x140, 0, height_above_replaybar);
            sprite_putimage_and_alt(whlsprite1->sprite_bitmapptr, whlshapes[3]->s2d_pos_x, whlshapes[3]->s2d_pos_y);
        }

        if (word_40E00[byte_4432A] != steering || byte_454A4 != 0 || knob_removed != 0) {
            if (video_flag5_is0 == 0)
                mouse_draw_opaque_check();
            sprite_set_1_size(0, 0x140, 0, height_above_replaybar);
            if (word_40DF6[byte_4432A] != 0) {
                sprite_putimage_and_alt(gnobshapes[byte_44346 + 4], word_40DF2[byte_4432A], word_40DF6[byte_4432A]);
                word_40DF6[byte_4432A] = 0;
            }
            dot = &simd_player.steeringdots[(steering < 0 ? -steering : steering) * 2];
            y_pos = dot[1];
            x_pos = dot[0];
            if (steering < 0)
                x_pos -= (x_pos - simd_player.steeringdots[0]) * 2;
            word_40DF2[byte_4432A] = (x_pos - gnobshapes[2]->s2d_unk1) & video_flag3_isFFFF;
            word_40DF6[byte_4432A] = y_pos - gnobshapes[2]->s2d_unk2;
            sprite_clear_shape_alt(gnobshapes[byte_44346 + 4], word_40DF2[byte_4432A], y_pos - gnobshapes[2]->s2d_unk2);
            sprite_putimage_and_alt2(gnobshapes[3], x_pos, y_pos);
            sprite_putimage_or_alt(gnobshapes[2], x_pos, y_pos);
            word_40E00[byte_4432A] = steering;
        }
        mouse_draw_transparent_check();
        return;
    case 3:
        sprite_free_wnd(whlsprite3);
        sprite_free_wnd(whlsprite2);
        sprite_free_wnd(whlsprite1);
        mmgr_free((char far *)stdbresptr);
        mmgr_free((char far *)stdaresptr);
        return;
    }
}

/* semantic lead: setup_player_cars from restunts.c */
extern void far *file_load_resfile(char *name);
extern void far *locate_shape_alt(void far *res, char *name);
extern int audio_init_engine(int id, void far *data, void far *eng1, void far *eng);
extern long mmgr_get_res_ofs_diff_scaled(void);
int setup_player_cars(void) {
	void far* carresptr;
	long mem_limit;

	wndsprite = 0;
	ensure_file_exists(2);
	shape3d_load_car_shapes(gameconfig.game_playercarid, gameconfig.game_opponentcarid);
	aCarcoun[3] = gameconfig.game_playercarid[0];
	aCarcoun[4] = gameconfig.game_playercarid[1];
	aCarcoun[5] = gameconfig.game_playercarid[2];
	aCarcoun[6] = gameconfig.game_playercarid[3];
	carresptr = file_load_resfile(aCarcoun);
	setup_aero_trackdata(carresptr, 0);
	unload_resource(carresptr);

	if (gameconfig.game_opponenttype != 0) {
		aCarcoun[3] = gameconfig.game_opponentcarid[0];
		aCarcoun[4] = gameconfig.game_opponentcarid[1];
		aCarcoun[5] = gameconfig.game_opponentcarid[2];
		aCarcoun[6] = gameconfig.game_opponentcarid[3];
		carresptr = file_load_resfile(aCarcoun);
		setup_aero_trackdata(carresptr, 1);
		unload_resource(carresptr);
		
		ensure_file_exists(4);
		load_opponent_data();
	}

	ensure_file_exists(3);
	eng1ptr = file_load_resource(5, "eng1");//aEng1); // "eng1"
	engptr = file_load_resource(6, "eng");//aEng); // "eng"
	audio_add_driver_timer();
	word_43964 = audio_init_engine(0x21, &unk_3E7FC, eng1ptr, engptr);

	byte_459D8 = 0;
	byte_42D26 = 0;
	byte_42D2A = 0;
	if (gameconfig.game_opponenttype != 0) {
		word_4408C = audio_init_engine(0x20, &unk_3E82C, eng1ptr, engptr);
	}

	word_44D1E = 0;
	word_449E4 = 0;
	word_443F4 = 0;
	fontledresptr = file_load_resource(0, "fontled.fnt");//aFontled_fnt); // "fontled.fnt"
	slow_video_mgmt_copy = slow_video_mgmt;
	init_rect_arrays();
	if (idle_expired == 0) {
		setup_car_shapes(0);
	}

	if (idle_expired == 0) {
		sdgameresptr = file_load_resource(3, "sdgame");//aSdgame); // "sdgame"
		loop_game(0, 0, 0);
	}

	gameresptr = file_load_resfile("game");
	planptr = locate_shape_alt(gameresptr, "plan");//aPlan); // "plan"
	wallptr = locate_shape_alt(gameresptr, "wall");//aWall); // "wall"
	load_sdgame2_shapes();
	load_skybox(td14_elem_map_main[0x384]);
	if (shape3d_load_all() != 0) {
		return 1;
	}

	if (video_flag5_is0 == 0) {
		
		mem_limit = 0xFA00L / (video_flag1_is1 * video_flag4_is1) + 0x12;
		if (mmgr_get_res_ofs_diff_scaled() <= mem_limit) {
			return 1;
		}
		wndsprite = sprite_make_wnd(0x140, 0xC8, 0x0F);
	}

	followOpponentFlag = 0;
	is_in_replay_copy = -1;
	return 0;
}

/* semantic lead: free_player_cars from restunts.c */
void free_player_cars(void) {
	if (video_flag5_is0 == 0) {
		if (wndsprite != 0) {
			sprite_free_wnd(wndsprite);
		}
	}
	shape3d_free_all();
	unload_skybox();
	free_sdgame2();
	unload_resource(gameresptr);
	if (idle_expired == 0) {
		mmgr_free(sdgameresptr);
		setup_car_shapes(3);
	}

	mmgr_free(fontledresptr);
	audio_remove_driver_timer();
	mmgr_free(engptr);
	mmgr_free(eng1ptr);
	shape3d_free_car_shapes();
}

void mouse_set_minmax(int, int, int, int);
void mouse_set_position(int, int);
void mouse_minmax_position(int enabled)
{
    if (enabled) {
        mouse_set_minmax(15, 0, 0x131, 0xC8);
        mouse_set_position(0xA0, 0x64);
        return;
    }
    mouse_set_minmax(0, 0, 0x140, 0xC8);
}


extern signed char byte_44292[64];
extern unsigned char byte_442EA[64];
void far replay_unk(void)
{
    register int frame_index = state.game_frame & 0x3F;
    register int steering;
    char speed_index;
    char response;
    char angle;

    if (byte_442EA[frame_index] == 0)
        return;

    steering = byte_44292[frame_index];
    speed_index = (char)((state.playerstate.car_speed2 >> 10) & 0xFC);
    response = ((char*)steerWhlRespTable_ptr)[(int)speed_index + 1];

    if (state.playerstate.car_steeringAngle < steering) {
        if (state.playerstate.car_steeringAngle < -1)
            response <<= 2;
    } else if (state.playerstate.car_steeringAngle > steering) {
        if (state.playerstate.car_steeringAngle > 1)
            response <<= 2;
    }

    if (state.playerstate.car_steeringAngle > steering &&
        state.playerstate.car_steeringAngle - response >= steering) {
        angle = 8;
    } else if (state.playerstate.car_steeringAngle < steering &&
               state.playerstate.car_steeringAngle + response <= steering) {
        angle = 4;
    } else {
        angle = 0;
    }

    if (angle != 0)
        td16_rpl_buffer[state.game_frame] |= angle;
    byte_442EA[frame_index] = 0;
}

char byte_3E9DB = 6;
char byte_3E9DC[10] = { 1, 7, 3, 4, 5, 6, 7, 8, 8, 0 };
char byte_3E9E6[10] = { 0, 0, 2, 2, 3, 4, 5, 1, 7, 0 };
char byte_3E9F0[10] = { 2, 6, 2, 3, 4, 5, 6, 7, 8, 0 };
char byte_3E9FA[10] = { 0, 1, 0, 0, 1, 1, 1, 7, 8, 0 };
char game_camera_buttons_count[4] = { 6, 6, 8, 7 };
int game_camera_buttons_x1[9] = { 272, 109, 274, 232, 190, 151, 108, 66, 10 };
int game_camera_buttons_x2[9] = { 314, 151, 314, 274, 232, 190, 151, 91, 47 };
int game_camera_buttons_y1[9] = { 176, 176, 156, 156, 156, 156, 156, 156, 156 };
int game_camera_buttons_y2[9] = { 193, 193, 173, 173, 173, 173, 173, 193, 193 };
int gameunk_button_x1[1] = { 0 };
int gameunk_button_x2[1] = { 104 };
int gameunk_button_y1[1] = { 151 };
int gameunk_button_y2[1] = { 200 };
extern char byte_40E08[];
extern char byte_40E6A[];
extern char byte_40E6C;
extern char byte_40E6D;
extern char byte_40E74[];
extern char byte_40E7A[];
extern int word_407FC;
extern int word_407FE;
extern int word_40E04[];
extern int word_40E0A[];
extern int word_40E76[];
extern int word_44D20;
extern char kbjoyflags;
extern int custom_camera_azimuth_angle;
extern int custom_camera_distance;
extern int custom_camera_elevation_angle;

extern int dialog_fnt_colour;
extern struct RECTANGLE *rectptr_unk2;
extern void far *rplyshapes[16];
extern void far font_set_unk(int, int);
extern void far font_set_fontdef2(void far *);
extern void far sprite_1_unk(int, int, int, int, int);
extern void far sprite_1_unk4(int, int, int, int, int);
extern int far input_checking(int);
extern int far mouse_multi_hittest(int, int *, int *, int *, int *);
extern int far kb_get_key_state(int);
extern char far do_fileselect_dialog(char *, char *, char *, void far *);
extern char far do_savefile_dialog(char *, char *, void far *);
extern int far file_find(char *);
extern void far copy_string(char *, char far *);
extern struct RECTANGLE * far intro_draw_text(char *, int, int, int, int);

extern char byte_3B8B0[];
extern int far file_find(char *path);
extern struct RECTANGLE *intro_draw_text(char *text, int x, int y, int color, int unk);
extern void far copy_string(char *dst, char far *src);
void loop_game(int mode, int frame_index, int frame_offset)
{
    char answer;
    unsigned key_code;
    int dialog_params[8];
    int prev_sky;
    int time_delta;
    register int i;
    register int j;
    char write_state;
    char button;
    long travel;
    char modifier;
    struct GAMEINFO oldcfg;

    switch (mode) {
    case 0:
        locate_many_resources(sdgameresptr,
            "rplyrpicrpacrpmcrptcbof6bof5bof4bof3bof2bof1bof0zoompannbon6bon5bon4bon3bon2bon1bof0zoompann",
            (char far **)rplyshapes);
        frame_index = 4;
    case 2:
        for (i = 0; i < 9; i++)
            byte_40E6A[i] = 0;
        byte_40E6A[frame_index] = 1;
        break;
    case 1:
        if (byte_449D8[byte_4432A] == 0) {
            byte_449D8[byte_4432A] = 1;
            byte_40E74[byte_4432A] = -1;
            byte_40E08[byte_4432A] = -1;
            for (i = 0; i < 9; i++)
                byte_40E7A[i * 2 + byte_4432A] = 0;
            mouse_draw_opaque_check();
            shape2d_op_unk(rplyshapes[0]);
            word_40E0A[byte_4432A] = -1;
            word_40E76[byte_4432A] = -1;
            format_frame_as_string(&resID_byte1, gameconfig.game_recordedframes + elapsed_time1, 1);
            font_set_unk(dialog_fnt_colour, 0);
            font_set_fontdef2(fontledresptr);
            sub_345BC(&resID_byte1, 0xD8, 0xBB);
            font_set_fontdef();
        }
        if (word_40E0A[byte_4432A] != frame_offset + elapsed_time1) {
            word_40E0A[byte_4432A] = frame_offset + elapsed_time1;
            format_frame_as_string(&resID_byte1, frame_offset + elapsed_time1, 1);
            font_set_unk(dialog_fnt_colour, 0);
            mouse_draw_opaque_check();
            font_set_fontdef2(fontledresptr);
            sub_345BC(&resID_byte1, 0x98, 0xBB);
            font_set_fontdef();
        }
        if (byte_40E74[byte_4432A] != cameramode) {
            byte_40E74[byte_4432A] = cameramode;
            word_40E76[byte_4432A] = -1;
            mouse_draw_opaque_check();
            shape2d_op_unk(rplyshapes[cameramode + 1]);
            if (game_camera_buttons_count[cameramode] < byte_3E9DB)
                byte_3E9DB = game_camera_buttons_count[cameramode];
            if (byte_40E08[byte_4432A] > 6)
                byte_40E08[byte_4432A] = -1;
        }
        if (gameconfig.game_recordedframes == 0) {
            i = 0;
            j = 0;
        } else {
            i = (long)frame_index * 110 / gameconfig.game_recordedframes;
            j = (long)frame_offset * 110 / gameconfig.game_recordedframes;
        }
        if (word_40E76[byte_4432A] != i || word_40E04[byte_4432A] != j) {
            mouse_draw_opaque_check();
            word_40E76[byte_4432A] = i;
            word_40E04[byte_4432A] = j;
            sprite_1_unk(0x9A, 0xB1, 0x74, 6, word_407FC);
            sprite_1_unk(i + 0x9A, 0xB1, 6, 6, dialog_fnt_colour);
            sprite_1_unk4(j + 0x9A, 0xB1, j + 0x9F, 0xB6, word_407FE);
        }
        if (byte_40E08[byte_4432A] != byte_3E9DB)
            goto redraw_buttons;
        for (button = 0; button < 7; button++) {
            if (byte_40E7A[button * 2 + byte_4432A] != byte_40E6A[button])
                goto redraw_buttons;
        }
        goto buttons_done;
redraw_buttons:
        mouse_draw_opaque_check();
        if (byte_40E08[byte_4432A] != -1) {
            if (byte_40E7A[byte_40E08[byte_4432A] * 2 + byte_4432A] != 0)
                shape2d_op_unk(rplyshapes[byte_40E08[byte_4432A] + 14]);
            else
                shape2d_op_unk(rplyshapes[byte_40E08[byte_4432A] + 5]);
            byte_40E08[byte_4432A] = -1;
        }
        for (button = 0; button < 7; button++) {
            if (byte_40E6A[button] == 0 && byte_40E7A[button * 2 + byte_4432A] != byte_40E6A[button]) {
                shape2d_op_unk(rplyshapes[button + 5]);
                byte_40E7A[button * 2 + byte_4432A] = 0;
            }
        }
        for (button = 0; button < 7; button++) {
            if (byte_40E6A[button] != 0) {
                byte_40E7A[button * 2 + byte_4432A] = 1;
                shape2d_op_unk(rplyshapes[button + 14]);
                byte_40E7A[button * 2 + byte_4432A] = 1;
            }
        }
        byte_40E08[byte_4432A] = byte_3E9DB;
        if (byte_3E9DB != -1)
            sprite_1_unk4(game_camera_buttons_x1[byte_3E9DB], game_camera_buttons_y1[byte_3E9DB],
                          game_camera_buttons_x2[byte_3E9DB], game_camera_buttons_y2[byte_3E9DB], word_407FE);
buttons_done:
        mouse_draw_transparent_check();
        break;
    case 3:
        if (game_camera_buttons_count[cameramode] < byte_3E9DB && cameramode != 2)
            byte_3E9DB = game_camera_buttons_count[cameramode];
        sprite_copy_2_to_1();
        if (video_flag5_is0 != 0)
            byte_4432A = byte_44346 ^ 1;
next_input:
        key_code = input_checking(timer_get_delta_alt());
        button = mouse_multi_hittest(game_camera_buttons_count[cameramode] + 1, game_camera_buttons_x1,
                                     game_camera_buttons_x2, game_camera_buttons_y1, game_camera_buttons_y2);
        if (button != -1) {
            if (button != byte_3E9DB && key_code == 0)
                key_code = 1;
            byte_3E9DB = button;
            if ((key_code == ' ' || key_code == '\r') && byte_3E9DB >= 7) {
                if (byte_3E9DB == 7) {
                    if ((game_camera_buttons_y1[7] + game_camera_buttons_y2[7]) >> 1 < mouse_ypos)
                        key_code = 0x5000;
                    else
                        key_code = 0x4800;
                } else {
                    switch (((polarAngle(mouse_xpos - ((game_camera_buttons_x1[8] + game_camera_buttons_x2[8]) >> 1),
                                         ((game_camera_buttons_y1[8] + game_camera_buttons_y2[8]) >> 1) - mouse_ypos)
                              + 0x80) & 0x3FF) >> 8) {
                    case 0:
                        key_code = 0x4800;
                        break;
                    case 1:
                        key_code = 0x4D00;
                        break;
                    case 2:
                        key_code = 0x5000;
                        break;
                    case 3:
                        key_code = 0x4B00;
                        break;
                    }
                }
            }
        } else {
            button = mouse_multi_hittest(1, gameunk_button_x1, gameunk_button_x2, gameunk_button_y1, gameunk_button_y2);
            if (button == 0 && (key_code == ' ' || key_code == '\r'))
                key_code = 'c';
        }
        if (key_code != 0 && key_code != 0x1B && handle_ingame_kb_shortcuts(key_code) != 0)
            break;
        if (is_in_replay == 0 && key_code == 0) {
            if (replaybar_enabled == 0)
                break;
            loop_game(1, state.game_frame, state.game_frame);
            return;
        }
        if (replaybar_enabled == 0) {
            is_in_replay_copy = -1;
            word_449EA = -1;
        }
        if (is_in_replay != 0 && (byte_40E6D != 0 || byte_40E6C != 0))
            loop_game(2, 4, 0);
        loop_game(1, state.game_frame, state.game_frame);
        modifier = 0;
        if (kb_get_key_state(0x1D) != 0 || (byte_3E9DB == 8 && (kbjoyflags & 0x30) != 0))
            modifier = 1;
        if (modifier != 0) {
            switch (key_code) {
            case 0x4D00:
                custom_camera_azimuth_angle += 0x10;
                return;
            case 0x4B00:
                custom_camera_azimuth_angle -= 0x10;
                return;
            case 0x4800:
                if (custom_camera_elevation_angle + 0x10 < 0x100) {
                    custom_camera_elevation_angle += 0x10;
                    return;
                }
                break;
            case 0x5000:
                if (custom_camera_elevation_angle - 0x10 > -0x100) {
                    custom_camera_elevation_angle -= 0x10;
                    return;
                }
                break;
            case '-':
zoom_out:
                if (cameramode == 3) {
                    if (word_44D20 <= 0)
                        break;
                    word_44D20 -= 30;
                } else {
                    if (custom_camera_distance >= 1500)
                        break;
                    custom_camera_distance += 30;
                }
                goto done;
            case '+':
zoom_in:
                if (cameramode == 3) {
                    if (word_44D20 >= 900)
                        break;
                    word_44D20 += 30;
                } else {
                    if (custom_camera_distance <= 120)
                        break;
                    custom_camera_distance -= 30;
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
        case 0x4B00:
            if (game_camera_buttons_count[cameramode] >= byte_3E9DC[byte_3E9DB])
                byte_3E9DB = byte_3E9DC[byte_3E9DB];
        default:
redraw_input:
            loop_game(1, state.game_frame, state.game_frame);
            goto next_input;
        case 0x4D00:
            byte_3E9DB = byte_3E9E6[byte_3E9DB];
            goto redraw_input;
        case 0x4800:
            if (byte_3E9DB == 7)
                goto zoom_in;
            byte_3E9DB = byte_3E9F0[byte_3E9DB];
            goto redraw_input;
        case 0x5000:
            if (byte_3E9DB == 7)
                goto zoom_out;
            byte_3E9DB = byte_3E9FA[byte_3E9DB];
            goto redraw_input;
        case '\r':
        case ' ':
            switch (byte_3E9DB) {
            case 6:
pause_menu:
                is_in_replay = 1;
                audio_carstate();
                loop_game(2, 4, 0);
                loop_game(1, state.game_frame, state.game_frame);
                for (i = 0; i < 8; i++)
                    dialog_params[i] = 0;
                if (state.playerstate.car_crashBmpFlag != 0)
                    dialog_params[3] = 1;
                if (gameconfig.game_recordedframes == 0 || elapsed_time1 != 0)
                    dialog_params[5] = 1;
                if (passed_security == 0) {
                    dialog_params[2] = 1;
                    dialog_params[3] = 1;
                }
                if ((byte_43966 & 4) == 0)
                    dialog_params[1] = 1;
                byte_454A4 = video_flag6_is1;
                answer = show_dialog(2, 0, locate_text_res(gameresptr, "men"), -1, -1, dialogarg2, dialog_params, 0);
                switch (answer) {
                case 2:
                    check_input();
                    framespersec = framespersec2;
                    gameconfig.game_framespersec = framespersec2;
                    init_game_state(-1);
                    elapsed_time2 = 0;
                    gameconfig.game_recordedframes = 0;
                    word_45D3E = 0;
                    byte_43966 = 1;
                    goto continue_driving;
                case 3:
                    if (byte_43966 & 2)
                        byte_43966 = 3;
                    else if (gameconfig.game_recordedframes != elapsed_time2) {
                        i = show_dialog(2, 0, locate_text_res(gameresptr, "con"), -1, -1, performGraphColor, 0, 0);
                        if (i < 1)
                            break;
                        byte_43966 = 3;
                    } else
                        byte_43966 = 1;
                    gameconfig.game_recordedframes = elapsed_time2 = state.game_frame;
    continue_driving:
                    dashb_toggle = 1;
                    show_penalty_counter = 0;
                    followOpponentFlag = 0;
                    game_replay_mode = 0;
                    cameramode = 0;
                    state.game_3F6autoLoadEvalFlag = 0;
                    state.game_frame_in_sec = 0;
                    byte_449E6 = 0;
                    loop_game(2, 3, 0);
                    is_in_replay = 0;
                    mouse_minmax_position(byte_3B8F2);
                    check_input();
                    kbormouse = 0;
                    break;
                case 4:
                    byte_43966 = 0;
                    audio_carstate();
                    i = do_fileselect_dialog(byte_3B85E, byte_3B8B0, ".rpl", locate_text_res(mainresptr, "rep"));
                    if (i == 0)
                        break;
                    waitflag = 150;
                    show_waiting();
                    oldcfg = gameconfig;
                    prev_sky = td14_elem_map_main[0x384];
                    if (file_load_replay(byte_3B85E, byte_3B8B0) != 0)
                        gameconfig.game_recordedframes = 0;
                    dashb_toggle = 0;
                    track_setup();
                    i = 0;
                    if (td14_elem_map_main[0x384] != prev_sky)
                        i = 1;
                    if (oldcfg.game_playercarid[0] != gameconfig.game_playercarid[0] ||
                        oldcfg.game_playercarid[1] != gameconfig.game_playercarid[1] ||
                        oldcfg.game_playercarid[2] != gameconfig.game_playercarid[2] ||
                        oldcfg.game_playercarid[3] != gameconfig.game_playercarid[3])
                        i = 1;
                    else if (oldcfg.game_opponenttype != gameconfig.game_opponenttype)
                        i = 1;
                    else if (gameconfig.game_opponenttype != 0) {
                        if (oldcfg.game_opponentcarid[0] != gameconfig.game_opponentcarid[0] ||
                            oldcfg.game_opponentcarid[1] != gameconfig.game_opponentcarid[1] ||
                            oldcfg.game_opponentcarid[2] != gameconfig.game_opponentcarid[2] ||
                            oldcfg.game_opponentcarid[3] != gameconfig.game_opponentcarid[3])
                            i = 1;
                        else {
                            ensure_file_exists(2);
                            load_opponent_data();
                        }
                    }
                    if (i != 0) {
                        free_player_cars();
                        setup_player_cars();
                    }
                    framespersec = gameconfig.game_framespersec;
                    init_game_state(-1);
                    break;
                case 5:
                    audio_carstate();
                    write_state = 0;
                    while (write_state == 0) {
                        if (do_savefile_dialog(byte_3B85E, byte_3B8B0, locate_text_res(mainresptr, "rep")) != 0) {
                            file_build_path(byte_3B85E, byte_3B8B0, ".rpl", g_path_buf);
                            write_state = 1;
                            g_is_busy = 1;
                            if (file_find(g_path_buf) != 0) {
                                i = show_dialog(2, 0, locate_text_res(mainresptr, "fex"), -1, -1, performGraphColor, 0, 0);
                                if (i == -1)
                                    write_state = -1;
                                else if (i == 0)
                                    write_state = 0;
                            }
                            g_is_busy = 0;
                        } else
                            write_state = -1;
                        if (write_state == 1) {
                            button = file_write_replay(g_path_buf);
                            if (button != 0) {
                                show_dialog(1, 0, locate_text_res(mainresptr, "ser"), -1, -1, performGraphColor, 0, 0);
                                write_state = 0;
                            }
                        }
                    }
                    break;
                case 1:
                    update_crash_state(4, 0);
                    byte_449DA = 2;
                    break;
                case 7:
                    update_crash_state(4, 0);
                    byte_43966 = 0;
                    byte_449DA = 2;
                    break;
                case 6:
                    for (i = 0; i < 5; i++)
                        dialog_params[i] = 0;
                    if (gameconfig.game_opponenttype == 0)
                        dialog_params[4] = 1;
                    answer = show_dialog(2, 0, locate_text_res(gameresptr, "mdo"), -1, -1, dialogarg2, dialog_params, 0);
                    switch (answer) {
                    case 0:
                        dashb_toggle ^= 1;
                        break;
                    case 1:
                        replaybar_toggle ^= 1;
                        break;
                    case 2:
                        cameramode++;
                        if (cameramode == 4)
                            cameramode = 0;
                        break;
                    case 3:
                        show_graphic_levels_menu();
                        break;
                    case 4:
                        followOpponentFlag ^= 1;
                        break;
                    }
                    break;
                }
                check_input();
                goto done;
            case 0:
                is_in_replay = 1;
                audio_carstate();
                loop_game(2, 0, 0);
                timer_get_delta_alt();
                travel = 20;
                while (kbjoyflags & 0x30) {
                    j = travel / 50 + 3;
                    if (j > 100)
                        j = 100;
                    i = (time_delta = timer_get_delta_alt()) * j;
                    travel += i;
                    if (gameconfig.game_recordedframes - elapsed_time2 < (unsigned)(travel / 20))
                        travel = (long)(gameconfig.game_recordedframes - elapsed_time2) * 20;
                    loop_game(1, state.game_frame, travel / 20 + elapsed_time2);
                    input_do_checking(time_delta);
                }
                if (gameconfig.game_recordedframes - elapsed_time2 < (unsigned)(travel / 20))
                    travel = (long)(gameconfig.game_recordedframes - elapsed_time2) * 20;
                i = travel / 20 + elapsed_time2;
                if (i > gameconfig.game_recordedframes)
                    i = gameconfig.game_recordedframes;
                restore_gamestate(i);
                elapsed_time2 = i;
                loop_game(2, 4, 0);
                copy_string(&resID_byte1, locate_text_res(gameresptr, "wai"));
                if (slow_video_mgmt_copy != 0)
                    rect_union(rectptr_unk2, intro_draw_text(&resID_byte1, font_op2_alt(&resID_byte1), 100, dialog_fnt_colour, 0),
                               rectptr_unk2);
                else
                    intro_draw_text(&resID_byte1, font_op2_alt(&resID_byte1), 100, dialog_fnt_colour, 0);
                while (state.game_frame != elapsed_time2) {
                    update_gamestate();
                    loop_game(1, state.game_frame, elapsed_time2);
                }
                input_do_checking(1000);
                goto done;
            case 1:
                is_in_replay = 1;
                audio_carstate();
                loop_game(2, 1, 0);
                timer_get_delta_alt();
                travel = 20;
                while (kbjoyflags & 0x30) {
                    j = travel / 50 + 3;
                    if (j > 100)
                        j = 100;
                    time_delta = timer_get_delta_alt();
                    i = time_delta * j;
                    travel += i;
                    if ((unsigned)(travel / 20) > elapsed_time2)
                        travel = (long)elapsed_time2 * 20;
                    loop_game(1, state.game_frame, elapsed_time2 - travel / 20);
                    input_do_checking(time_delta);
                }
                if ((unsigned)(travel / 20) > elapsed_time2)
                    travel = (long)elapsed_time2 * 20;
                j = travel / 20;
                loop_game(2, 4, 0);
                if (j != 0) {
                    copy_string(&resID_byte1, locate_text_res(gameresptr, "wai"));
                    if (slow_video_mgmt_copy != 0)
                        rect_union(rectptr_unk2, intro_draw_text(&resID_byte1, font_op2_alt(&resID_byte1), 100, dialog_fnt_colour, 0),
                                   rectptr_unk2);
                    else
                        intro_draw_text(&resID_byte1, font_op2_alt(&resID_byte1), 100, dialog_fnt_colour, 0);
                    i = elapsed_time2 - j;
                    restore_gamestate(i);
                    elapsed_time2 = i;
                    prev_sky = i - state.game_frame;
                    if (prev_sky != 0) {
                        i = prev_sky;
                        while (state.game_frame != elapsed_time2) {
                            update_gamestate();
                            i--;
                            loop_game(1, (long)i * j / prev_sky + elapsed_time2, elapsed_time2);
                            input_do_checking(1);
                        }
                    }
                }
                loop_game(1, state.game_frame, state.game_frame);
                input_do_checking(1000);
                goto done;
            case 3:
                byte_449E6 = 0;
                loop_game(2, 3, 0);
                is_in_replay = 0;
                break;
            case 4:
                is_in_replay = 1;
                audio_carstate();
                loop_game(2, 4, 0);
                loop_game(1, state.game_frame, state.game_frame);
                goto redraw_input;
            case 5:
                is_in_replay = 1;
                audio_carstate();
                loop_game(2, 5, 0);
                loop_game(1, state.game_frame, state.game_frame);
                restore_gamestate(0);
                timer_get_counter_unk(50);
                loop_game(2, 4, 0);
                loop_game(1, state.game_frame, state.game_frame);
                return;
            case 2:
                loop_game(2, 2, 0);
                byte_449E6 = 3;
                is_in_replay = 0;
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

