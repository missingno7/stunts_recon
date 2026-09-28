/* Scratch TU. Header declarations are semantic leads from Restunts; no preprocessor directives. */
extern void sprcopy2to12(void);
extern void msdrawtransparentchk(void);



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


short sinfast(unsigned short s);
short cosfast(unsigned short s);

int polang(int z, int y);
int polradius2d(int z, int y);
int polarRadius3D(struct VECTOR* vec);

unsigned rect_compare_point(struct POINT2D* pt);

void mat_vec(struct VECTOR* invec, struct MATRIX* mat, struct VECTOR* outvec);
void mat_mul_vector2(struct VECTOR* invec, struct MATRIX far* mat, struct VECTOR* outvec);
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

void plnrotop(void);
int plnoriginop(int index, int b, int c, int d);






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


extern struct GAMEINFO globalgamesettings;
extern struct GAMEINFO gmconfigbackup;

extern struct GAMESTATE core;
extern struct SIMD ophys_7;

extern short pixel_scales;
extern short g_vid_flg2_set;
short vidflg4_is1;
char g_videoflg5;
extern short g_vid_flag6;

unsigned char timeraud;
unsigned char slomodiv8;
unsigned short elaptm1;
unsigned short tmr2;
unsigned char sigframe;
unsigned char g_rpl_init;
char gm_playmode; // 0 = playing, 1 = paused, 2 = replay
extern short g_sgateopn;

extern short word_45A24; // current frame?
extern short g_cvxintvl; // fps * 30
extern short frmcs_time; // 100 / fps
extern short st_hdg;
extern void* table_lookup;
extern void* steerWhlRespTable_10fps;
extern void* steerWhlRespTable_20fps;
extern char idxtrk, tagtrk;
extern char g_hillf;
extern short hillconsts[];

struct RECTANGLE boundglassrect;
short bitmapdash;
extern int runrndx;
char replaybar_toggle;
char inrepflg;
extern char cammd;
char g_rplmodui;
char gm_saved_rpl_mode;
char numid;
char g_rplbfask;
char on_off_dash;
char cam_idg;
extern char pen_flag_count;
int replayrst;
char popupact;
extern char byte_3B8F2;
extern char byte_3FE00;
extern void far* gamerptrs;
void far* dasm_shp_7;
extern int word_3F88E;
char dashbtogglesaved;
char g_replaybarcpytgl;
char is_in_rplcopy;
extern char follow_op;
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
extern void far do_mou_resource_text(void);
extern void far initialize_game_state(int);
extern char handle_ingame_kb_shortcuts(unsigned);

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
extern short far* g_td01_track_filecpy; //trackdata1;
extern short far* trackdata_penalty_related; //trackdata2;
extern char far* td3;
extern short far* track04_plyraero; //trackdata4;
extern short far* trackdata_05_opp_aerotbl; //trackdata5;
extern char far* td6_ptr_b;
extern char far* trackdat7;
extern int far* g_td08d; //trackdata8;
extern struct VECTOR far* trkptrpath;
extern int far* td10checkptr;// trackdata10;
extern char far* hscore_trk11_ptr; //trackdata11;
extern char far* savedptr_ms;
char far* td13_replay_hdr; //trackdata13;
extern unsigned char far* td14tb; //trackdata14;
extern unsigned char far* td15p_9; //trackdata15;
char far* g_tdreplay16buf; //trackdata16;
extern char far* road_trk; //trackdata17;
extern char far* td_18_ref;
extern unsigned char far* td19hdl;
extern char far* coursedataappend_address; //trackdata20;
extern char far* g_column_of_trkdata21_pth; //trackdata21;
extern char far* tdfrompathrow22; //trackdata22;
extern unsigned char far* trkd23adr; // indexes into trkObjectList
extern char kbormouse;
extern char pass_check_flag;
extern char g_is_busy;
extern char buf_g_path[];
extern char track_file[];
extern char menutimeout;
extern unsigned short dialogarg2;
extern char replay_file[];
char endhsdemo;
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

extern int rate_frame;
unsigned short frm_rate2;
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

extern unsigned short idx_time_gm;
extern short is_audioloaded;
extern void far* musicfile;
extern void far* openvfile;
extern char textrespfxchr; // = 'e'
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

extern short track_edge_points(short car_trackdata3_index, struct VECTOR* car_vec_unk3, short field_CE, short* unk);
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
extern 
/* TU function declarations needed before address-ordered definitions. */
char far file_load_replay(const char *dir, const char *name);
short far file_write_replay(const char far *filename);
int far file_write_fatal(const char *filename, void far *buffer, unsigned long length);
void far remove_frame_callback(void);
void far replay_unk2(int mode);
void far replay_unk(void);
void far free_player_cars(void);
extern struct SPRITE far *g_wndspr;
extern char aCarcoun[];
void far *eng1resourceptr;
void far *engdata;
extern int g_player_sound_id;
extern char sndpendingstate;
extern char g_plyr_snd_state;
extern char audiooppflag;
extern int op_eng_sound_id;
int g_audio_frms_ix;
extern int sndposrecord;
int snd_tick_clock;
void far *fntled_res;
void far *sdgresourcehandle;
extern void far *g_planlist;
extern void far *wallrecrecord;

void run_game(void);
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



struct ENGINESOUND {
	int es_unk0;
	int es_unk2;
	int es_unk4;
	int es_unk6;
	char far *es_names[10];
};
struct ENGINESOUND player_engine_profile = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
struct ENGINESOUND opponent_engine_profile = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
char replay_axis_magnitude[34] = { 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
	60, 64, 68, 72, 76, 84, 90, 98, 106, 114, 121, 127, 127, 127 };

/* run_game: reconstructed from the target listing (s005c) */
void loop_game(int mode, int frame_index, int frame_offset);
extern void far *locate_text_resource(void far *res, char *name);

void run_game(void) {
	int dialog_values[2];
	int replay_key_code, replaybar_height, last_game_frame;
	struct RECTANGLE temp_rect;
	int previous_roof;
	register int dialog_result;

	last_game_frame = -1;
	boundglassrect.left = 0;
	boundglassrect.right = 320;
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
		if (file_load_replay(0, "default") != 0) {
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
			mouse_minmax_position(byte_3B8F2);
			gm_playmode = 1;
			core.playerstate.car_posWorld1.lx += (long)mulscl(sinfast(st_hdg), -240) << 6;
			core.playerstate.car_posWorld1.lz += (long)mulscl(cosfast(st_hdg), -240) << 6;
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
				if (input_do_checking(1) != 27)
					update_gamestate();
				else
					break;
			}
			tmr2 = globalgamesettings.game_recordedframes;
		}

		for (;;) {
			while (core.game_frame != tmr2) {
				if ((byte_3B8F2 != 0 || byte_3FE00 != 0) && gm_playmode == 0) {
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
				input_push_status();
				audio_unk();
				dialog_result = show_dialog(2, 1, locate_text_resource(gamerptrs, "rbf"), -1, -1, dialogarg2, 0, 0);
				if (dialog_result == -1)
					dialog_result = 0;
				restore_audio_volume();
				word_3F88E = 0;
				input_pop_status();
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
				sprite_copy_wnd_to_1();
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
					dashbmpy_copy = 200;
				} else if (on_off_dash != 0 && follow_op == 0) {
					if (gm_playmode == 2 && g_rplybarenable != 0) {
						rplbarabovehgt = 151;
					} else {
						rplbarabovehgt = 200;
					}
					mode_flag = 1;
					roofbmphgt_saved = rfy5;
					dashbmpy_copy = dashbmy9;
				} else if (gm_playmode == 2 && g_rplybarenable != 0) {
					dashbmpy_copy = 151;
				} else {
					dashbmpy_copy = 200;
				}

				if (previous_roof != roofbmphgt_saved || dashbmpy_copy != bitmapdash || replaybar_height != rplbarabovehgt) {
					g_simprect = g_vid_flag6;
					set_projection(35, dashbmpy_copy / 6, 320, dashbmpy_copy);
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
					sprset1size(0, 320, dashbmpy_copy, rplbarabovehgt);
					setup_car_shapes(1);
				}
				if (g_rplybarenable != 0) {
					sprset1size(0, 320, 0, 200);
					loop_game(1, core.game_frame, core.game_frame);
				}
			} else if (g_rplybarenable == 0) {
				g_viewinx[cam_idg] = 0;
			}

			update_frame(numid, &boundglassrect);
			if (dasty != 0 && mode_flag != 0) {
				if (statemgmtcpy != 0) {
					temp_rect.left = 0;
					temp_rect.right = 320;
					temp_rect.top = dasty;
					temp_rect.bottom = dashbmpy_copy;
					if (rectp != 0) {
						rcunion(rectp, &temp_rect, rectp);
					}
				}
				shape2d_render_bmp_as_mask(dasm_shp_7);
				shape2d_op_unk4(g_dastbmpbuf);
			}

			draw_clip(&boundglassrect);
			if (mode_flag != 0) {
				sprset1size(0, 320, dashbmpy_copy, rplbarabovehgt);
				setup_car_shapes(2);
				sprset1size(0, 320, 0, 200);
			}

			if (g_simprect != 0) {
				g_simprect--;
			}

			if (g_videoflg5 != 0) {
				msdrawopaquechk();
				setup_mcgawnd1();
				numid ^= 1;
				cam_idg = numid;
				msdrawtransparentchk();
			}

			if (gm_playmode == 1 && g_rpl_init == 0) {
				gm_playmode = 0;
				rate_frame = frm_rate2;
				globalgamesettings.game_framespersec = frm_rate2;
				initialize_game_state(-1);
			}

			if (menutimeout != 0) {
				if (kb_get_char() != 0 || sigframe != 0 || get_kb_or_joy_flags() != 0)
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
				mouse_minmax_position(0);
				loop_game(0, 0, 0);
				loop_game(2, 4, 0);
				inrepflg = 1;
				audio_carstate();
			}

			if (gm_playmode == 2) {
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

			if (gm_playmode == 1) {
				mouse_get_state(&flagsdown, &msecoordx, &pos_y_ms);
				if ((flagsdown & 3) != 0 || (get_kb_or_joy_flags() & 0x30) != 0) {
					gm_playmode = 0;
					g_rpl_init = 0;
					rate_frame = frm_rate2;
					globalgamesettings.game_framespersec = frm_rate2;
					initialize_game_state(-1);
				}
			}
		}

		if (g_videoflg5 != 0 && get_0() != 0) {
			msdrawopaquechk();
			setup_mcgawnd2();
			clear_rect(0, 0, 320, 200, 0);
			setup_mcgawnd1();
			msdrawtransparentchk();
		}

		sprcopy2to12();
		inrepflg = 1;
		audio_carstate();
		audio_remove_driver_timer();
		if (gm_playmode == 0 && globalgamesettings.game_opponenttype != 0 && core.opponentstate.car_crashBmpFlag == 0) {
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
					msdrawopaquechk();
					draw_text_at(&resbuftext, font_op2_alt(&resbuftext), dialog_values[1]);
					msdrawtransparentchk();
				}
			} while (input_do_checking(1) != 27 && core.opponentstate.car_crashBmpFlag == 0 &&
				 1500 * rate_frame != core.game_frame + elaptm1);
		}

		popupact = 0;
		mouse_minmax_position(0);
		remove_frame_callback();
		free_player_cars();
	}

	waitm_ms = 100;
	check_input();
	show_waiting();
}
extern char byte_349BA;
extern char HKeyFlag;
char handle_ingame_kb_shortcuts(unsigned key)
{
    switch (key) {
    case 0x1B:
        if (gm_playmode == 0) {
            update_crash_state(4, 0);
        }
        sigframe = 1;
        break;
    case 0x3C00:
        cammd = 1;
        break;
    case 0x3D00:
        cammd = 2;
        break;
    case 0x3E00:
        cammd = 3;
        break;
    case 0x48:
    case 0x68:
        HKeyFlag ^= 1;
        break;
    case 0x4D:
    case 0x6D:
        do_mou_resource_text();
        mouse_minmax_position(byte_3B8F2);
        break;
    case 0x44:
    case 0x64:
        on_off_dash ^= 1;
        break;
    case 0x52:
    case 0x72:
        replaybar_toggle ^= 1;
        break;
    case 0x43:
    case 0x63:
        if (gm_playmode != 1) {
            cammd++;
            if (cammd == 4) {
                cammd = 0;
            }
        }
        break;
    case 0x3B00:
        cammd = 0;
        break;
    case 0x74:
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
void initialize_unknown(void)
{
	register int zero;
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
void far set_frame_callback(void) {
    g_clocks = 0;
    timer_reg_callback(frame_callback);
    call_proc_flag = 0;
}


extern unsigned long far timer_get_counter_unk(unsigned long ticks);
extern void far timer_remove_callback(callback_t callback);
void far remove_frame_callback(void)
{
    (void)timer_get_counter_unk(10L);
    timer_remove_callback(frame_callback);
}

extern int far compare_ds_ss(void);
extern void far apply_audio_frame(char* record, int frame_count);
extern char audio_frmarr[];
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
        apply_audio_frame(&audio_frmarr[g_audio_frms_ix * 0x22], snd_tick_clock);
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
        audio_carstate();
        goto frame_callback_done;
    }

    timeraud--;
    if (timeraud == 0) {
        timeraud = (char)frmcs_time;
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
static char replay_control;
extern int far kb_get_key_state(int);
extern char far replay_axis_value(void);
extern int far abs(int);
void far replay_unk2(int mode)
{
    register int input;
    register int i;

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
        audio_carstate();
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
    if (byte_3B8F2 != 0 || byte_3FE00 != 0) {
        if (byte_3B8F2 != 0) {
            mouse_get_state(&flagsdown, &msecoordx, &pos_y_ms);
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
            input = get_kb_or_joy_flags() & 0x33;
        }
        i = tmr2 & 0x3F;
        array_rpl[i] = replay_control;
        replay_steer_flag[i] = 1;
    } else {
        input = get_kb_or_joy_flags();
    }
    if (kb_get_key_state(0x1E))
        input |= 0x10;
    if (kb_get_key_state(0x2C))
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

extern char trk_sample_count;
void far update_camera_target(void)
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
    if (globalgamesettings.game_opponenttype != 0)
        car_count = 2;
    for (index = 0; index < car_count; ++index) {
        core.game_vec3[index] = core.game_vec1[index];
        if (index == 0)
            selected_car_state = &core.playerstate;
        else
            selected_car_state = &core.opponentstate;

        car_position.y = (short)(selected_car_state->car_posWorld1.ly >> 6);
        car_position.x = (short)(selected_car_state->car_posWorld1.lx >> 6);
        car_position.z = (short)(selected_car_state->car_posWorld1.lz >> 6);
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
                dx = (long)trkptrpath[track_segment].x - car_position.x;
                dz = (long)trkptrpath[track_segment].z - car_position.z;
                if ((dx < 0 ? -dx : dx) < nearest_range &&
                    (dz < 0 ? -dz : dz) < nearest_range) {
                    point_range = polradius2d((int)dx, (int)dz);
                    if (point_range < nearest_range) {
                        core.field_3F7[index] = (char)track_segment;
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
	file_build_path(dir, name, ".rpl", buf_g_path);

	g_is_busy = 1;
	file_read_fatal(buf_g_path, td13_replay_hdr);
	globalgamesettings = *(struct GAMEINFO far*)td13_replay_hdr;
	g_is_busy = 0;
	return 0;
}

/* semantic lead: file_write_replay from fileio.c */
short file_write_replay(const char* filename)
{
	register int ret;
	long write_length;

	*(struct GAMEINFO far*)td13_replay_hdr = globalgamesettings;
	write_length = globalgamesettings.game_recordedframes + 0x724;
	g_is_busy = 1;
	ret = file_write_fatal(filename, td13_replay_hdr, write_length);
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
extern struct SPRITE far *sprite_make_window(unsigned int width, unsigned int height, unsigned int color);
extern void sprite_free_window(struct SPRITE far *sprite);
extern void sprite_setup1_from_arg_pointer(struct SPRITE far *sprite);
extern void sprite_copy_2_to_1(void);
extern void sprite_putimage_and_alt(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_and_alt2(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_or_alt(struct SHAPE2D far *shape, int x, int y);
extern void shape2d_op_unk(struct SHAPE2D far *shape);
extern void shape2d_op_unk2(struct SHAPE2D far *shape, int x, int y);
extern void shape2d_op_unk3(struct SHAPE2D far *shape);
extern void shape2d_op_unknown5(struct SHAPE2D far *shape, int x, int y);
extern void sprite_putimage_or(struct SHAPE2D far *shape, int x, int y);
extern void sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
extern void preRender_line(int x1, int y1, int x2, int y2, int color);
extern void far *mmgr_free(char far *ptr);
char aStdaxxxx[] = "stdaxxxx";
char aStdbxxxx[] = "stdbxxxx";
void far setup_car_shapes(int mode)
{
    extern struct SIMD simdp7;
    extern short vidflg3is_minus1;
    static short last_steering_step[2];
    static char steering_zone[2];
    static struct SPRITE far * meters_sprite;
    static struct SPRITE far * gnob_sprite;
    static struct SPRITE far * gear_base_sprite;
    static void far * stdares;
    static void far * stdbres;
    static struct SHAPE2D far * wheel_shapes[9];
    static struct SHAPE2D far * gnobshapes[6];
    static struct SHAPE2D far * digshapes[10];
    extern short spdneedlegaugeclr;
    static char gear_knob_visible_view[2];
    static short steering_dot_x[2];
    static short steering_dot_y[2];
    static short last_speedo[2];
    static short gear_knob_x_last[2];
    static short gear_knob_y_last[2];
    static short last_tacho[2];
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
        aStdaxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdaxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdaxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdaxxxx[7] = globalgamesettings.game_playercarid[3];
        aStdbxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdbxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdbxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdbxxxx[7] = globalgamesettings.game_playercarid[3];
        stdares = file_load_resource(3, aStdaxxxx);
        stdbres = file_load_resource(2, aStdbxxxx);
        locate_many_resources(stdares, "whl1whl2whl3ins2gboxins1ins3inm1inm3", (char far **)wheel_shapes);
        locate_many_resources(stdbres, "gnobgnabdot dotadot1dot2", (char far **)gnobshapes);
        if (simdp7.spdcenter.py == 0)
            locate_many_resources(stdbres, "dig0dig1dig2dig3dig4dig5dig6dig7dig8dig9", (char far **)digshapes);
        meters_sprite = sprite_make_window(wheel_shapes[3]->s2d_width * pixel_scales, wheel_shapes[3]->s2d_height, 15);
        gnob_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        gear_base_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        shape = (struct SHAPE2D far *)locate_shape_fatal(stdares, "dash");
        sprite_setup1_from_arg_pointer(gear_base_sprite);
        shape2d_op_unk2(shape, shape->s2d_pos_x - wheel_shapes[4]->s2d_pos_x,
                        shape->s2d_pos_y - wheel_shapes[4]->s2d_pos_y);
        sprite_copy_2_to_1();
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
    case 1:
        msdrawopaquechk();
        if (locate_shape_nofatal(stdares, "roof") != 0)
            shape2d_op_unk((struct SHAPE2D far *)locate_shape_fatal(stdares, "roof"));
        shape2d_op_unk3((struct SHAPE2D far *)locate_shape_fatal(stdares, "dash"));
        shape2d_op_unk3(wheel_shapes[1]);
        msdrawtransparentchk();
        x = 0;
        gear_knob_visible_view[cam_idg] = g_viewinx[cam_idg] = 0;
        steering_dot_y[cam_idg] = x;
        steering_zone[cam_idg] = (char)x;
        x--;
        last_steering_step[cam_idg] = x;
        last_speedo[cam_idg] = x;
        last_tacho[cam_idg] = x;
        return;
    case 2:
        if ((core.playerstate.car_changing_gear | core.playerstate.car_fpsmul2) == 0 &&
            gear_knob_visible_view[cam_idg] != 0) {
            if (g_videoflg5 == 0)
                msdrawopaquechk();
            sprset1size(0, 0x140, 0, rplbarabovehgt);
            sprite_putimage_and_alt(gear_base_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
            gear_knob_visible_view[cam_idg] = 0;
        } else if (gear_knob_visible_view[cam_idg] != core.playerstate.car_changing_gear ||
                   gear_knob_x_last[cam_idg] != core.playerstate.car_knob_x ||
                   gear_knob_y_last[cam_idg] != core.playerstate.car_knob_y ||
                   (core.playerstate.car_fpsmul2 != 0 && gear_knob_visible_view[cam_idg] == 0)) {
            sprite_setup1_from_arg_pointer(gnob_sprite);
            gear_knob_visible_view[cam_idg] = 1;
            shape2d_op_unk2(wheel_shapes[4], 0, 0);
            x = core.playerstate.car_knob_x;
            y = core.playerstate.car_knob_y;
            gear_knob_x_last[cam_idg] = x;
            gear_knob_y_last[cam_idg] = y;
            sprite_putimage_and_alt2(gnobshapes[1], x, y);
            sprite_putimage_or_alt(gnobshapes[0], x, y);
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            else {
                sprcopy2to12();
                msdrawopaquechk();
            }
            sprset1size(0, 0x140, 0, rplbarabovehgt);
            sprite_putimage_and_alt(gnob_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
        }

        knob_removed = 0;
        steering = core.playerstate.car_steeringAngle / 8;
        wheel_zone = 1;
        if (steering < -10)
            wheel_zone = 0;
        else if (steering > 10)
            wheel_zone = 2;
        if (steering_zone[cam_idg] != wheel_zone || g_simprect != 0) {
            if (g_videoflg5 == 0)
                msdrawopaquechk();
            if (steering_dot_y[cam_idg] != 0) {
                sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            switch (wheel_zone) {
            case 0:
                shape2d_op_unk3(wheel_shapes[0]);
                break;
            case 1:
                shape2d_op_unk3(wheel_shapes[1]);
                break;
            case 2:
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
            x = core.playerstate.car_speed >> 8;
        }
        y = (unsigned short)core.playerstate.car_currpm >> 7;
        if (y >= simdp7.revnumpoints)
            y = simdp7.revnumpoints - 1;

        if (changed != 0 || g_simprect != 0 ||
            last_speedo[cam_idg] != x || last_tacho[cam_idg] != y) {
            if (g_videoflg5 == 0)
                msdrawopaquechk();
            if (steering_dot_y[cam_idg] != 0) {
                sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            sprite_setup1_from_arg_pointer(meters_sprite);
            shape2d_op_unknown5(wheel_shapes[3], 0, 0);
            last_speedo[cam_idg] = x;
            last_tacho[cam_idg] = y;
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
                    sprite_putimage_or(digshapes[digit], simdp7.spdpoints[0], simdp7.spdpoints[1]);
                    has_digit = 1;
                }
                digit = x / 10;
                if (digit != 0 || has_digit != 0) {
                    sprite_putimage_or(digshapes[digit], simdp7.spdpoints[2], simdp7.spdpoints[3]);
                    x -= digit * 10;
                    has_digit = 1;
                }
                sprite_putimage_or(digshapes[x], simdp7.spdpoints[4], simdp7.spdpoints[5]);
            } else if (speedo_type == 0) {
                preRender_line(simdp7.spdcenter.px, simdp7.spdcenter.py,
                               simdp7.spdpoints[x * 2], simdp7.spdpoints[x * 2 + 1],
                               spdneedlegaugeclr);
            }
            preRender_line(simdp7.revcenter.px, simdp7.revcenter.py,
                           simdp7.revpoints[y * 2], simdp7.revpoints[y * 2 + 1],
                           spdneedlegaugeclr);
            switch (wheel_zone) {
            case 0:
                shape2d_render_bmp_as_mask(wheel_shapes[7]);
                shape2d_op_unk4(wheel_shapes[5]);
                break;
            case 2:
                shape2d_render_bmp_as_mask(wheel_shapes[8]);
                shape2d_op_unk4(wheel_shapes[6]);
                break;
            }
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            else
                sprcopy2to12();
            sprset1size(0, 0x140, 0, rplbarabovehgt);
            sprite_putimage_and_alt(meters_sprite->sprite_bitmapptr, wheel_shapes[3]->s2d_pos_x, wheel_shapes[3]->s2d_pos_y);
        }

        if (last_steering_step[cam_idg] != steering || g_simprect != 0 || knob_removed != 0) {
            if (g_videoflg5 == 0)
                msdrawopaquechk();
            sprset1size(0, 0x140, 0, rplbarabovehgt);
            if (steering_dot_y[cam_idg] != 0) {
                sprite_putimage_and_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
            }
            dot = &simdp7.steeringdots[(steering < 0 ? -steering : steering) * 2];
            y_pos = dot[1];
            x_pos = dot[0];
            if (steering < 0)
                x_pos -= (x_pos - simdp7.steeringdots[0]) * 2;
            steering_dot_x[cam_idg] = (x_pos - gnobshapes[2]->s2d_unk1) & vidflg3is_minus1;
            steering_dot_y[cam_idg] = y_pos - gnobshapes[2]->s2d_unk2;
            sprite_clear_shape_alt(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
            sprite_putimage_and_alt2(gnobshapes[3], x_pos, y_pos);
            sprite_putimage_or_alt(gnobshapes[2], x_pos, y_pos);
            last_steering_step[cam_idg] = steering;
        }
        msdrawtransparentchk();
        return;
    case 3:
        sprite_free_window(gear_base_sprite);
        sprite_free_window(gnob_sprite);
        sprite_free_window(meters_sprite);
        mmgr_free((char far *)stdbres);
        mmgr_free((char far *)stdares);
        return;
    }
}

/* semantic lead: setup_player_cars from restunts.c */
extern void far *file_load_resource_file(char *name);
extern void far *locate_shape_alt(void far *res, char *name);
extern int audio_init_engine(int id, void far *data, void far *eng1, void far *eng);
extern long mmgr_get_res_ofs_diff_scaled(void);
int setup_player_cars(void) {
	void far* carresptr;
	long mem_limit;

	g_wndspr = 0;
	ensure_file_exists(2);
	shape3d_load_car_shapes(globalgamesettings.game_playercarid, globalgamesettings.game_opponentcarid);
	aCarcoun[3] = globalgamesettings.game_playercarid[0];
	aCarcoun[4] = globalgamesettings.game_playercarid[1];
	aCarcoun[5] = globalgamesettings.game_playercarid[2];
	aCarcoun[6] = globalgamesettings.game_playercarid[3];
	carresptr = file_load_resource_file(aCarcoun);
	setup_aero_trackdata(carresptr, 0);
	unload_resource(carresptr);

	if (globalgamesettings.game_opponenttype != 0) {
		aCarcoun[3] = globalgamesettings.game_opponentcarid[0];
		aCarcoun[4] = globalgamesettings.game_opponentcarid[1];
		aCarcoun[5] = globalgamesettings.game_opponentcarid[2];
		aCarcoun[6] = globalgamesettings.game_opponentcarid[3];
		carresptr = file_load_resource_file(aCarcoun);
		setup_aero_trackdata(carresptr, 1);
		unload_resource(carresptr);
		
		ensure_file_exists(4);
		load_opponent_data();
	}

	ensure_file_exists(3);
	eng1resourceptr = file_load_resource(5, "eng1");//aEng1); // "eng1"
	engdata = file_load_resource(6, "eng");//aEng); // "eng"
	audio_add_driver_timer();
	g_player_sound_id = audio_init_engine(0x21, &player_engine_profile, eng1resourceptr, engdata);

	sndpendingstate = 0;
	g_plyr_snd_state = 0;
	audiooppflag = 0;
	if (globalgamesettings.game_opponenttype != 0) {
		op_eng_sound_id = audio_init_engine(0x20, &opponent_engine_profile, eng1resourceptr, engdata);
	}

	g_audio_frms_ix = 0;
	sndposrecord = 0;
	snd_tick_clock = 0;
	fntled_res = file_load_resource(0, "fontled.fnt");//aFontled_fnt); // "fontled.fnt"
	statemgmtcpy = slow_video_mode_state;
	init_rect_arrays();
	if (menutimeout == 0) {
		setup_car_shapes(0);
	}

	if (menutimeout == 0) {
		sdgresourcehandle = file_load_resource(3, "sdgame");//aSdgame); // "sdgame"
		loop_game(0, 0, 0);
	}

	gamerptrs = file_load_resource_file("game");
	g_planlist = locate_shape_alt(gamerptrs, "plan");//aPlan); // "plan"
	wallrecrecord = locate_shape_alt(gamerptrs, "wall");//aWall); // "wall"
	load_sdgame2_shapes();
	load_skybox(td14tb[0x384]);
	if (shape3d_load_all() != 0) {
		return 1;
	}

	if (g_videoflg5 == 0) {
		
		mem_limit = 0xFA00L / (pixel_scales * vidflg4_is1) + 0x12;
		if (mmgr_get_res_ofs_diff_scaled() <= mem_limit) {
			return 1;
		}
		g_wndspr = sprite_make_window(0x140, 0xC8, 0x0F);
	}

	follow_op = 0;
	is_in_rplcopy = -1;
	return 0;
}

/* semantic lead: free_player_cars from restunts.c */
void free_player_cars(void) {
	if (g_videoflg5 == 0) {
		if (g_wndspr != 0) {
			sprite_free_window(g_wndspr);
		}
	}
	shape3d_free_all();
	unload_skybox();
	free_sdgame2();
	unload_resource(gamerptrs);
	if (menutimeout == 0) {
		mmgr_free(sdgresourcehandle);
		setup_car_shapes(3);
	}

	mmgr_free(fntled_res);
	audio_remove_driver_timer();
	mmgr_free(engdata);
	mmgr_free(eng1resourceptr);
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
void far replay_unk(void)
{
    register int frame_index = core.game_frame & 0x3F;
    register int steering;
    char speed_index;
    char response;
    char angle;

    if (replay_steer_flag[frame_index] == 0)
        return;

    steering = array_rpl[frame_index];
    speed_index = (char)((core.playerstate.car_speed2 >> 10) & 0xFC);
    response = ((char*)table_lookup)[(int)speed_index + 1];

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

char camera_button_index = 6;
char camera_button_count_mode[10] = { 1, 7, 3, 4, 5, 6, 7, 8, 8, 0 };
char camera_mode_select_a[10] = { 0, 0, 2, 2, 3, 4, 5, 1, 7, 0 };
char camera_mode_select_b[10] = { 2, 6, 2, 3, 4, 5, 6, 7, 8, 0 };
char camera_mode_select_c[10] = { 0, 1, 0, 0, 1, 1, 1, 7, 8, 0 };
char game_camera_buttons_count[4] = { 6, 6, 8, 7 };
int game_camera_buttons_x1[9] = { 272, 109, 274, 232, 190, 151, 108, 66, 10 };
int game_camera_buttons_x2[9] = { 314, 151, 314, 274, 232, 190, 151, 91, 47 };
int game_camera_buttons_y1[9] = { 176, 176, 156, 156, 156, 156, 156, 156, 156 };
int game_camera_buttons_y2[9] = { 193, 193, 173, 173, 173, 173, 173, 193, 193 };
int gameunk_button_x1[1] = { 0 };
int gameunk_button_x2[1] = { 104 };
int gameunk_button_y1[1] = { 151 };
int gameunk_button_y2[1] = { 200 };
static char view_camera_choice[2];
static char camera_buttons_pressed[9];
static char unused_40E73;
static char camera_mode_view[2];
static char camera_buttons_state_per_view[18];
extern int camera_select_fill_color;
extern int camera_select_outline_color;
static int camera_button_row[2];
static int camera_button_tick[2];
static int camera_button_col_cache[2];
int viewyshift;
extern char kbjoyflags;
extern int custom_azim_angle;
extern int custom_dist;
extern int custom_elev_angle;

extern int dlg_colour;
extern struct RECTANGLE *rcpunk2;
static void far *replayshapes[23];
extern void far font_setup_unknown(int, int);
extern void far fontsetfontdef2(void far *);
extern void far sprite_1_unk(int, int, int, int, int);
extern void far sprite_1_unk4(int, int, int, int, int);
extern int far input_checking(int);
extern int far mouse_multi_hittest(int, int *, int *, int *, int *);
extern int far kb_get_key_state(int);
extern char far do_fileselect_dialog(char *, char *, char *, void far *);
extern char far do_savefile_dialog(char *, char *, void far *);
extern int far file_find(char *);
extern void far copy_string(char *, char far *);
extern struct RECTANGLE * far introtext(char *, int, int, int, int);

extern char aDefault_1[];
extern int far file_find(char *path);
extern struct RECTANGLE *introtext(char *text, int x, int y, int color, int unk);
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
        locate_many_resources(sdgresourcehandle,
            "rplyrpicrpacrpmcrptcbof6bof5bof4bof3bof2bof1bof0zoompannbon6bon5bon4bon3bon2bon1bof0zoompann",
            (char far **)replayshapes);
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
            msdrawopaquechk();
            shape2d_op_unk(replayshapes[0]);
            camera_button_tick[cam_idg] = -1;
            camera_button_col_cache[cam_idg] = -1;
            fmtframestr(&resbuftext, globalgamesettings.game_recordedframes + elaptm1, 1);
            font_setup_unknown(dlg_colour, 0);
            fontsetfontdef2(fntled_res);
            draw_text_at(&resbuftext, 0xD8, 0xBB);
            fontsetfontdef();
        }
        if (camera_button_tick[cam_idg] != frame_offset + elaptm1) {
            camera_button_tick[cam_idg] = frame_offset + elaptm1;
            fmtframestr(&resbuftext, frame_offset + elaptm1, 1);
            font_setup_unknown(dlg_colour, 0);
            msdrawopaquechk();
            fontsetfontdef2(fntled_res);
            draw_text_at(&resbuftext, 0x98, 0xBB);
            fontsetfontdef();
        }
        if (camera_mode_view[cam_idg] != cammd) {
            camera_mode_view[cam_idg] = cammd;
            camera_button_col_cache[cam_idg] = -1;
            msdrawopaquechk();
            shape2d_op_unk(replayshapes[cammd + 1]);
            if (game_camera_buttons_count[cammd] < camera_button_index)
                camera_button_index = game_camera_buttons_count[cammd];
            if (view_camera_choice[cam_idg] > 6)
                view_camera_choice[cam_idg] = -1;
        }
        if (globalgamesettings.game_recordedframes == 0) {
            i = 0;
            j = 0;
        } else {
            i = (long)frame_index * 110 / globalgamesettings.game_recordedframes;
            j = (long)frame_offset * 110 / globalgamesettings.game_recordedframes;
        }
        if (camera_button_col_cache[cam_idg] != i || camera_button_row[cam_idg] != j) {
            msdrawopaquechk();
            camera_button_col_cache[cam_idg] = i;
            camera_button_row[cam_idg] = j;
            sprite_1_unk(0x9A, 0xB1, 0x74, 6, camera_select_fill_color);
            sprite_1_unk(i + 0x9A, 0xB1, 6, 6, dlg_colour);
            sprite_1_unk4(j + 0x9A, 0xB1, j + 0x9F, 0xB6, camera_select_outline_color);
        }
        if (view_camera_choice[cam_idg] != camera_button_index)
            goto redraw_buttons;
        for (button = 0; button < 7; button++) {
            if (camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button])
                goto redraw_buttons;
        }
        goto buttons_done;
redraw_buttons:
        msdrawopaquechk();
        if (view_camera_choice[cam_idg] != -1) {
            if (camera_buttons_state_per_view[view_camera_choice[cam_idg] * 2 + cam_idg] != 0)
                shape2d_op_unk(replayshapes[view_camera_choice[cam_idg] + 14]);
            else
                shape2d_op_unk(replayshapes[view_camera_choice[cam_idg] + 5]);
            view_camera_choice[cam_idg] = -1;
        }
        for (button = 0; button < 7; button++) {
            if (camera_buttons_pressed[button] == 0 && camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button]) {
                shape2d_op_unk(replayshapes[button + 5]);
                camera_buttons_state_per_view[button * 2 + cam_idg] = 0;
            }
        }
        for (button = 0; button < 7; button++) {
            if (camera_buttons_pressed[button] != 0) {
                camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
                shape2d_op_unk(replayshapes[button + 14]);
                camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
            }
        }
        view_camera_choice[cam_idg] = camera_button_index;
        if (camera_button_index != -1)
            sprite_1_unk4(game_camera_buttons_x1[camera_button_index], game_camera_buttons_y1[camera_button_index],
                          game_camera_buttons_x2[camera_button_index], game_camera_buttons_y2[camera_button_index], camera_select_outline_color);
buttons_done:
        msdrawtransparentchk();
        break;
    case 3:
        if (game_camera_buttons_count[cammd] < camera_button_index && cammd != 2)
            camera_button_index = game_camera_buttons_count[cammd];
        sprite_copy_2_to_1();
        if (g_videoflg5 != 0)
            cam_idg = numid ^ 1;
next_input:
        key_code = input_checking(timer_get_delta_alt());
        button = mouse_multi_hittest(game_camera_buttons_count[cammd] + 1, game_camera_buttons_x1,
                                     game_camera_buttons_x2, game_camera_buttons_y1, game_camera_buttons_y2);
        if (button != -1) {
            if (button != camera_button_index && key_code == 0)
                key_code = 1;
            camera_button_index = button;
            if ((key_code == ' ' || key_code == '\r') && camera_button_index >= 7) {
                if (camera_button_index == 7) {
                    if ((game_camera_buttons_y1[7] + game_camera_buttons_y2[7]) >> 1 < pos_y_ms)
                        key_code = 0x5000;
                    else
                        key_code = 0x4800;
                } else {
                    switch (((polang(msecoordx - ((game_camera_buttons_x1[8] + game_camera_buttons_x2[8]) >> 1),
                                         ((game_camera_buttons_y1[8] + game_camera_buttons_y2[8]) >> 1) - pos_y_ms)
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
        if (kb_get_key_state(0x1D) != 0 || (camera_button_index == 8 && (kbjoyflags & 0x30) != 0))
            modifier = 1;
        if (modifier != 0) {
            switch (key_code) {
            case 0x4D00:
                custom_azim_angle += 0x10;
                return;
            case 0x4B00:
                custom_azim_angle -= 0x10;
                return;
            case 0x4800:
                if (custom_elev_angle + 0x10 < 0x100) {
                    custom_elev_angle += 0x10;
                    return;
                }
                break;
            case 0x5000:
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
        case 0x4B00:
            if (game_camera_buttons_count[cammd] >= camera_button_count_mode[camera_button_index])
                camera_button_index = camera_button_count_mode[camera_button_index];
        default:
redraw_input:
            loop_game(1, core.game_frame, core.game_frame);
            goto next_input;
        case 0x4D00:
            camera_button_index = camera_mode_select_a[camera_button_index];
            goto redraw_input;
        case 0x4800:
            if (camera_button_index == 7)
                goto zoom_in;
            camera_button_index = camera_mode_select_b[camera_button_index];
            goto redraw_input;
        case 0x5000:
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
                audio_carstate();
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
                answer = show_dialog(2, 0, locate_text_resource(gamerptrs, "men"), -1, -1, dialogarg2, dialog_params, 0);
                switch (answer) {
                case 2:
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
                    mouse_minmax_position(byte_3B8F2);
                    check_input();
                    kbormouse = 0;
                    break;
                case 4:
                    endhsdemo = 0;
                    audio_carstate();
                    i = do_fileselect_dialog(replay_file, aDefault_1, ".rpl", locate_text_resource(main_data_file_addr, "rep"));
                    if (i == 0)
                        break;
                    waitm_ms = 150;
                    show_waiting();
                    oldcfg = globalgamesettings;
                    prev_sky = td14tb[0x384];
                    if (file_load_replay(replay_file, aDefault_1) != 0)
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
                case 5:
                    audio_carstate();
                    write_state = 0;
                    while (write_state == 0) {
                        if (do_savefile_dialog(replay_file, aDefault_1, locate_text_resource(main_data_file_addr, "rep")) != 0) {
                            file_build_path(replay_file, aDefault_1, ".rpl", buf_g_path);
                            write_state = 1;
                            g_is_busy = 1;
                            if (file_find(buf_g_path) != 0) {
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
                            button = file_write_replay(buf_g_path);
                            if (button != 0) {
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
                    case 3:
                        show_graphic_levels_menu();
                        break;
                    case 4:
                        follow_op ^= 1;
                        break;
                    }
                    break;
                }
                check_input();
                goto done;
            case 0:
                inrepflg = 1;
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
                    if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                        travel = (long)(globalgamesettings.game_recordedframes - tmr2) * 20;
                    loop_game(1, core.game_frame, travel / 20 + tmr2);
                    input_do_checking(time_delta);
                }
                if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                    travel = (long)(globalgamesettings.game_recordedframes - tmr2) * 20;
                i = travel / 20 + tmr2;
                if (i > globalgamesettings.game_recordedframes)
                    i = globalgamesettings.game_recordedframes;
                restore_gamestate(i);
                tmr2 = i;
                loop_game(2, 4, 0);
                copy_string(&resbuftext, locate_text_resource(gamerptrs, "wai"));
                if (statemgmtcpy != 0)
                    rcunion(rcpunk2, introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0),
                               rcpunk2);
                else
                    introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0);
                while (core.game_frame != tmr2) {
                    update_gamestate();
                    loop_game(1, core.game_frame, tmr2);
                }
                input_do_checking(1000);
                goto done;
            case 1:
                inrepflg = 1;
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
                    if ((unsigned)(travel / 20) > tmr2)
                        travel = (long)tmr2 * 20;
                    loop_game(1, core.game_frame, tmr2 - travel / 20);
                    input_do_checking(time_delta);
                }
                if ((unsigned)(travel / 20) > tmr2)
                    travel = (long)tmr2 * 20;
                j = travel / 20;
                loop_game(2, 4, 0);
                if (j != 0) {
                    copy_string(&resbuftext, locate_text_resource(gamerptrs, "wai"));
                    if (statemgmtcpy != 0)
                        rcunion(rcpunk2, introtext(&resbuftext, font_op2_alt(&resbuftext), 100, dlg_colour, 0),
                                   rcpunk2);
                    else
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
                            loop_game(1, (long)i * j / prev_sky + tmr2, tmr2);
                            input_do_checking(1);
                        }
                    }
                }
                loop_game(1, core.game_frame, core.game_frame);
                input_do_checking(1000);
                goto done;
            case 3:
                g_rplmodui = 0;
                loop_game(2, 3, 0);
                inrepflg = 0;
                break;
            case 4:
                inrepflg = 1;
                audio_carstate();
                loop_game(2, 4, 0);
                loop_game(1, core.game_frame, core.game_frame);
                goto redraw_input;
            case 5:
                inrepflg = 1;
                audio_carstate();
                loop_game(2, 5, 0);
                loop_game(1, core.game_frame, core.game_frame);
                restore_gamestate(0);
                timer_get_counter_unk(50);
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

