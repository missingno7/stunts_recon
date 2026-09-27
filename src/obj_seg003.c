/* obj_seg003 whole-object candidate (s003d).  RECONSTRUCTION NOTE: original identifiers are not
 * recoverable.  Local/parameter names and the scoping of extern declarations (K&R-style
 * function-scope externs in sub_19F14/init_rect_arrays) are pressure-constrained reconstruction
 * choices: MSC 5.10 C2's CSE capacity in update_frame depends on symbol-table memory (names of
 * referenced globals and of update_frame's locals).  They are not recovered names.
 * Data shapes: rect_unk[15] (15-iteration copy loop + rect_unk[si], labels at 8-byte stride),
 * unk_3C0A2[9][2] (pair table, see comment); fence pair lists by their pinned labels; plane state kept separate. */


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

struct MATRIX {
	int vals[9];
};

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
char rcintersect(struct RECTANGLE* r1, struct RECTANGLE* r2);

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
	unsigned short game_framespersec;
	unsigned short game_recordedframes;
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
	struct VECTOR game_vec3;
	struct VECTOR game_vec4;
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
	unsigned char field_42B[24];
	unsigned char field_443[24];
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
	char steeringdots[62];
	struct POINT2D spdcenter;
	short spdnumpoints;
	char spdpoints[208];
	struct POINT2D revcenter;
	short revnumpoints;
	char revpoints[256];
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


extern struct GAMEINFO gmcfg;
extern struct GAMEINFO gameconfigcopy;

extern struct GAMESTATE state;
extern struct SIMD simdpl;
extern struct SIMD simdopp;

extern short video_flag1_is1;
extern short video_flag2_is1;
extern short video_flag3_isFFFF;
extern short video_flag4_is1;
extern char vidflag5is0;
extern short video_flag6_is1;

extern unsigned char byte_44A8A;
extern unsigned char byte_4552F;
extern unsigned short elaptm1;
extern unsigned short elaptm2;
extern unsigned char byte_449DA;
extern unsigned char byte_4393C;
extern unsigned char replay_mode; // 0 = playing, 1 = paused, 2 = replay
extern short word_44DCA;

extern short word_45A24; // current frame?
extern short word_45A00; // fps * 30
extern short word_4499C; // 100 / fps
extern short trkang;
extern void* steerWhlRespTable_ptr;
extern void* steerWhlRespTable_10fps;
extern void* steerWhlRespTable_20fps;
extern char stcol2, strow2;
extern char hillFlag;
extern short hillconsts[];

extern struct RECTANGLE rect_windshield;
extern short word_449EA;
extern int runrandom;
extern char replaybar_toggle;
extern char is_in_replay;
extern char cammd;
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
extern char followflg;
extern char followOpponentFlag_copy;
extern int roofbmpheight_copy;
extern char byte_449E2;
extern char replaybar_enabled;
extern int dashbmp_y_copy;
extern int height_above_replaybar;
extern char byte_449D8[];
extern int dastseg;
extern int dastbmp_y;
extern int dastbmp_y2;
extern int dashbmp_y;
extern int roofhgt;
extern struct RECTANGLE* rectp;

extern void player_op(char);
extern void opponent_op(void);
extern void audio_carstate(void);
extern void setup_car_shapes(int);
extern void update_frame(char a, struct RECTANGLE* rc);
extern void loop_game(int, int, int);
extern void set_frame_callback(void);
extern void mouse_minmax_position(int);
extern int kb_get_char(void);
extern void handle_ingame_kb_shortcuts(int);

extern int mouse_butstate;
extern int mouse_xpos;
extern int mouse_ypos;
extern int performGraphColor;
extern char residbyte1;
extern int waitflag;

extern void far* fontnptr;
extern void far* fontdefptr;
extern void far* mainresourcepointer;
extern struct GAMESTATE far* cvxptr;
extern int trackrows[];
extern int terrrows[];
extern int trkp[];
extern int trackctrpos[];
extern int terrainposition[];
extern int terraincenterposition[];
extern int trkp2[];
extern int trackctrpos2[];
extern short far* trackdata01_track_file_cpy; //trackdata1;
extern short far* trackdata02_penalty_related; //trackdata2;
extern char far* trackdata3;
extern short far* trackdata04_aerotable_pl; //trackdata4;
extern short far* trackdata05_aerotable_op; //trackdata5;
extern char far* trackdata6;
extern char far* trackdata7;
extern int far* td08dir; //trackdata8;
extern struct VECTOR far* trackdata9;
extern int far* td10checkrel;// trackdata10;
extern char far* trackdata11_highscores; //trackdata11;
extern char far* trackdata12;
extern char far* trackdata13_replay_header; //trackdata13;
extern unsigned char far* td14map; //trackdata14;
extern unsigned char far* td15map; //trackdata15;
extern char far* trackdata16_replay_buffer; //trackdata16;
extern char far* trackdata17_trk_elem_ordered; //trackdata17;
extern char far* trackdata18;
extern unsigned char far* trkdata19;
extern char far* trackdata20_trk_file_appnd; //trackdata20;
extern char far* trackdata21_col_from_path; //trackdata21;
extern char far* trackdata22_row_from_path; //trackdata22;
extern unsigned char far* trkdata23; // indexes into trkObjectList
extern char kbormouse;
extern char passed_security;
extern char g_is_busy;
extern char g_path_buffer[];
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

extern unsigned short framespersec;
extern unsigned short framespersec2;
extern unsigned short slow_video_management;
extern unsigned short slowmgmtcopy;
extern unsigned char detail_lvl;

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

extern struct MATERIALCLRLIST *material_clrlist_ptr;
extern int* material_clrlist_ptr_cpy;
extern int* material_clrlist2_ptr;
extern int* material_clrlist2_ptr_cpy;
extern int* material_patlist_ptr;
extern int* material_patlist_ptr_cpy;
extern int* material_patlist2_ptr;
extern int* material_patlist2_ptr_cpy;
extern unsigned short someZeroVideoConst;

extern short sub_18D60(short car_trackdata3_index, struct VECTOR* car_vec_unk3, short field_CE, short* unk);
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

extern void video_set_mode4(void);
extern void video_set_mode7(void);
extern void video_set_mode_13h(void);

extern void shape3d_load_car_shapes(char* carid, char* oppcarid);

extern void load_palandcursor(void);
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





struct SHAPE3D {
	unsigned short shape3d_numverts;
	struct VECTOR far* shape3d_verts;
	unsigned short shape3d_numprimitives;
	unsigned short shape3d_numpaints;
	char far* shape3d_primitives;
	char far* shape3d_cull1;
	char far* shape3d_cull2;
};

struct SHAPE3DHEADER {
	unsigned char header_numverts;
	unsigned char header_numprimitives;
	unsigned char header_numpaints;
	unsigned char header_reserved;
};

struct TRANSFORMEDSHAPE3D {
	struct VECTOR pos;
	struct SHAPE3D* shapeptr;
	struct RECTANGLE* rectptr;
	struct VECTOR rotvec;
	unsigned short unk;
	unsigned char ts_flags;
	unsigned char material;
};


int shape3d_load_all(void);
void shape3d_free_all(void);
void shape3d_init_shape(char far* shapeptr, struct SHAPE3D* gameshape);
char trans_op(struct TRANSFORMEDSHAPE3D* arg_transshapeptr);
void set_projection(int i1, int i2, int i3, int i4);
int polang(int z, int y);
unsigned select_rot(int angZ, int angX, int angY, struct RECTANGLE* cliprect, int unk);
void initialize_polyinfo(void);
void polyinfo_reset(void);
void polyinfo(void);
void sub_204AE(struct VECTOR far* arg_verts, int arg_4, short* arg_6, short* arg_8, struct VECTOR* arg_vecarray, struct VECTOR* arg_vecptr);


extern struct RECTANGLE* rcpunk2;
extern struct RECTANGLE rectarr[];
extern struct RECTANGLE rectarr2[];
extern struct RECTANGLE rcunk[];
extern struct RECTANGLE clipunk;
extern struct VECTOR vunk2;
extern struct VECTOR vplanerotopresult;
extern int planindexcpy;
extern int pstateminusz2, pstateminusx2, pstateminusy2, pstf36mminf40sar2;
extern struct MATRIX mattmp;
extern int custom_dist;
extern int custom_elev_angle;
extern int custom_azim_angle;
extern int word_44D20;
extern char detthrlevel[];
extern char byte_3C0C6[];
extern unsigned word_46468;
extern int word_3BE34[];
extern char* ahead_tables[];
extern struct SHAPE3D* off_3BE44[];
extern int terrhgt;
extern int planindex;
extern char byte_4392C;
extern struct TRANSFORMEDSHAPE3D curtshape[29];
//extern struct TRANSFORMEDSHAPE3D transshapeunk;
extern struct TRANSFORMEDSHAPE3D* curtshapep;
extern struct TRACKOBJECT trklst[215]; // 215 entries
extern unsigned char fence_codes[];

extern char unk_3C0EE[];         /* fence (dx,dz) pair lists: 1-,2-,3-,4-entry lists at the pinned labels unk_3C0EE/3C0F0/3C0F8/3C0F4 */
extern char unk_3C0F0[];
extern char unk_3C0F8[];
extern char unk_3C0F4[];
extern int word_3C0D6[];
extern int unk_3C0A2[9][2];      /* hill (x,z) offsets: 1,2,2,4 pairs at pairs 0,1,3,5; exactly fills [0x3C0A2,0x3C0C6) */
extern int unk_3C0A6[];
extern int unk_3C0AE[];
extern int unk_3C0B6[];
extern struct TRACKOBJECT scene2[];
extern struct TRACKOBJECT scene3[];
extern struct SHAPE3D game3dshapes[130];
extern struct VECTOR carvec;
extern struct VECTOR carvecs[6];
extern short word_443E8[];
extern struct VECTOR ocarvec;
extern struct VECTOR ocarvecs[6];
extern short word_4448A[];
extern char backlights_ovr;
extern int tshapezs[];
extern int tshapeixs[];
extern char tshapearg2arr[];
extern short sdg2widths[3];
extern struct SHAPE2D far *sdg2shps[3];
extern void far* fontledresptr;
extern int dlg_colour;
extern char tshapecnt;

void build_obj(struct VECTOR* a, struct VECTOR* b);
void transformed_shape_add_for_sort(int zadjust, char arg_2);
unsigned char subst_hillroad(unsigned char a, unsigned char b);
int skybox_op(int a, struct RECTANGLE* rectptr, int e, struct MATRIX* matptr, int c, int f, int g);
struct RECTANGLE* draw_ingame_text(void);
struct RECTANGLE* init_crak(int frame, int top, int height);
struct RECTANGLE* do_sinking(int frame, int top, int height);
struct RECTANGLE* introtext(char* str, int a, int b, int c, int d);
void fontsetfontdef2(void far* data);
void fmtframestr(char* s, int time, int c);
void shapeexpl(int a, void far* shp, int x, int y);
void heapsortorder(int n, int* heap, int* data);


extern struct RECTANGLE rect_ingame_text, rect_ingame_text2, rect_ingame_text3, rect_ingame_text4;
extern char aDm1[], aDm2[], aPre[], aSe1[], aSe2[], aWww[], aOpp[], aPen[], aRpl_0[], aOpp_0[];
extern char far * far locate_text_resource(void far *data, char *name);
extern void copy_string(char *destination, char far *source);
extern int far font_op2_alt(char *name);
extern void sprite_putimage_transparent(void far *shape, int x, int y);
extern unsigned strlen(char *s);
extern int penalty_time;
extern int skybox_wat_color;
extern void preRender_line(int x1, int y1, int x2, int y2, int color);
extern char far *locate_shape_alt(char far *data, char *name);
extern int skybox_current, word_454CE;
extern unsigned short skybox_ptr1, skybox_ptr2, skybox_ptr3, skybox_ptr4;
extern int skybox_sky_color, skybox_ground_color;
extern struct SHAPE2D far *skyboxes[4];
extern void sprite_putimage_and_alt(void far *shape, int x, int y);
extern struct RECTANGLE trackpreview_cliprect;
extern short word_2C0FC[];
extern int word_3C112, word_3C10C, word_3C10E, word_3C108, word_3C110, word_3C10A;
extern struct VECTOR unk_3C114;
extern unsigned draw_line_related(unsigned, unsigned, unsigned, unsigned, int *);
void far skybox_op_helper(unsigned color, unsigned count, struct POINT2D p1, struct POINT2D p2, struct POINT2D p3, struct POINT2D p4);
void far skybox_op_helper2(struct RECTANGLE *rectptr, int x, int horizon);
void draw_track_preview(void);


struct SHAPE2D { short width, height, unk1, unk2, pos_x, pos_y; };
struct MATERIALCLRLIST { unsigned char pad20[0x20]; short ground, sky; unsigned char pad24[0xa4]; short water; };
union FARRESOURCE { void far *pointer; struct { unsigned short offset, segment; } word; };
struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; int scale; unsigned char flags, material; };
struct SPRITE { void far *image; unsigned short words[13]; };

extern struct SHAPE3D logoshape, logo2shape, bravshape;
extern char far *tempdatapointer;
extern int word_34A06, word_34A08, word_34A0A;
extern int intro_colorvalue, word_407CC;
extern void far *file_load_3dres(char *);
extern void far locate_many_resources(void far *, char *, char far **);
extern void far *sprite_make_window(int, int, int);
extern int far get_kevinrandom(void);
extern char far *file_load_resource_file(char *);
extern void far setup_aero_trackdata(char far *, int);
extern void far unload_resource(char far *);
extern void far init_plantrak(void);
extern int far timer_get_delta(void);
extern void far do_opponent_op(void);
extern void far setup_mcgawnd2(void);
extern void far setup_mcgawnd1(void);
extern void far sprite_copy_wnd_to_1(void);
extern int far get_0(void);
extern void far sub_35C4E(int, int, int, int, int);
extern void far sprite_free_window(void far *);
extern void far mmgr_free(void far *);

extern int word_449FE, word_44DCC;
extern int meter_needle_color;
extern unsigned char byte_3B8F6, byte_46167;
extern struct RECTANGLE rect_unk3;
extern struct RECTANGLE intro_cliprect;
extern union FARRESOURCE skybox_res_ofs;
extern char aDesert[];
extern void far *file_load_shape2d_fatal_thunk(char *);
extern char far *sdgame2ptr;
extern void far *file_load_resource(int, char *);
extern void far load_sdgame2_shapes(void);
extern void far free_sdgame2(void);
extern void far init_rect_arrays(void);
extern void far sub_19F14(struct RECTANGLE *);
extern void far load_skybox(char);
extern void far unload_skybox(void);
extern char far setup_intro(void);
extern void far intro_op(int, int, int, int, int, int, int, struct VECTOR *, struct POINT2D *, int *, struct RECTANGLE, struct RECTANGLE *, struct RECTANGLE *);


void far sub_19F14(struct RECTANGLE *clipRect)
{
	extern char byte_454A4;
	extern void far msdrawopaquechk(void);
	extern void far msdrawtransparentchk(void);
	extern void far rectsorttop(char, struct RECTANGLE *, int *);
	extern struct RECTANGLE rectarr3[15];
	extern int word_355D4[15];
	extern char rectarr3len;
	extern char rcarrayixs[15];
	extern void far rectlist_add(char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);
	extern void far sprcopy2to12(void);
	extern void far sprputimage(void far *);
	extern void sprset1size(unsigned short left, unsigned short right, unsigned short top, unsigned short height);
	extern struct SPRITE far *wndsprite;
	extern int word_449FC[];
	extern int word_463D6;
    register int i;
    struct RECTANGLE *currentRect;
    if (vidflag5is0 != 0)
        return;
    sprcopy2to12();
    if (byte_454A4 == 0) {
        if (slowmgmtcopy != 0) {
            for (i=0; i<15; ++i)
                rcarrayixs[i] = 3;
            if (detail_lvl == 4)
                word_449FC[1] = word_463D6;
            if (word_449FC[1] == word_463D6 &&
                rectarr[5].left == rectarr2[5].left &&
                rectarr[5].right == rectarr2[5].right &&
                rectarr[5].top == rectarr2[5].top &&
                rectarr[5].bottom == rectarr2[5].bottom)
                rcarrayixs[5] = 0;
            rectarr3len = 0;
            rectlist_add(15, rcarrayixs, rectarr,
                rectarr2, clipRect, &rectarr3len,
                rectarr3);
            if (rectarr3len != 0) {
                rectsorttop(rectarr3len,
                    rectarr3, word_355D4);
                msdrawopaquechk();
                i = 0;
                goto draw_rect_check;
                do {
                    currentRect = &rectarr3[word_355D4[i]];
                    sprset1size(currentRect->left, currentRect->right,
                        currentRect->top, currentRect->bottom);
                    sprputimage(wndsprite->image);
                    ++i;
draw_rect_check:
                    ;
                } while (rectarr3len > i);
                goto draw_transparent;
            }
            sprset1size(0, 320, clipRect->top, clipRect->bottom);
        } else {
            sprset1size(clipRect->left, clipRect->right,
                clipRect->top, clipRect->bottom);
        }
    }
    msdrawopaquechk();
    sprputimage(wndsprite->image);
draw_transparent:
    msdrawtransparentchk();
    if (slowmgmtcopy != 0) {
        word_449FC[1] = word_463D6;
        for (i=0; i<15; ++i)
            rectarr2[i] = rectarr[i];
    }
}

void far init_rect_arrays(void)
{
	extern struct RECTANGLE rcunk5;
    register int i;

    if (slowmgmtcopy != 0) {
        rectarr[0] = rcunk5;
        rectarr2[0] = rcunk5;
        for (i = 1; i < 15; ++i) {
            rectarr[i] = clipunk;
            rectarr2[i] = clipunk;
        }
    }
}
void update_frame(char page, struct RECTANGLE* clip) {
	extern void sprset1size(unsigned short left, unsigned short right, unsigned short top, unsigned short height);
	extern int word_449FC[];
	extern int word_463D6;
	char skipped[24];
	unsigned char terrain_list[24];
	int cur_sky_sign;
	struct TRACKOBJECT *ovl;
	int ground_dist;
	int nfences;
	struct RECTANGLE *rect_ptr;
	char car_east1;
	int objx;
	char checkpoint_no;
	char cur_opp_east;
	int hill_elev;
	int car_angle_y;
	struct VECTOR elem_pos;
	struct VECTOR viewpoint;
	int cur_view_heading_val;
	char *tile_table;
	char ovl_pending;
	int obj_zpos;
	int view_rot_x;
	int my_car_bank;
	char opp_wheel_south_val;
	int car_zofs;
	struct RECTANGLE cliprect1;
	unsigned char track_elem;
	char wheel_south;
	struct MATRIX sky_mat;
	struct RECTANGLE cliprect2;
	char east_list[24];
	char south_tab[24];
	char lod_levels[24];
	int opp_zbias_val;
	int wheel_idx;
	int j;
	char crashed[2];
	int *offsets;
	char car_x_tile;
	struct MATRIX *car_matrix;
	struct TRACKOBJECT *trkobj;
	char drow;
	int start_bias;
	struct VECTOR rot_ofs;
	char flagbits;
	struct MATRIX view_mat;
	char *fence_list;
	unsigned char tile_ground;
	char cam_tx;
	struct VECTOR far *vptr;
	struct CARSTATE *followed;
	char trow;
	char tile_east2;
	int bank;
	char tile_lod;
	int cam_elev;
	register int si;
	char tx;
	int rot_x;
	struct VECTOR offset_v;
	struct VECTOR car_coord;
	register int j2;
	char paint;
	char car_z_tile;
	int cam_bank;
	unsigned char elem_list[24];
	char cam_tz;
	int skybox_result;
	char thresh;
	int result;

	crashed[0] = 0;
	crashed[1] = 0;
	if (vidflag5is0 != 0 && page != 0) {
		rcpunk2 = rectarr;
		rectp = rectarr2;
	} else {
		rectp = rectarr;
		rcpunk2 = rectarr2;
	}

	if (slowmgmtcopy != 0) {
		flagbits = 8;
		rect_ptr = rcunk;
		for (si = 0; si < 15; si++) {
			*rect_ptr++ = clipunk;
		}
	} else {
		flagbits = 0;
	}

	// Set car position (own or opponent's)
	if (followflg == 0) {		
		car_coord.x = state.playerstate.car_posWorld1.lx >> 6;
		car_coord.y = state.playerstate.car_posWorld1.ly >> 6;
		car_coord.z = state.playerstate.car_posWorld1.lz >> 6;
		car_angle_y = state.playerstate.car_rotate.y;
		my_car_bank = state.playerstate.car_rotate.z;
		rot_x = state.playerstate.car_rotate.x;
	} else {
		car_coord.x = state.opponentstate.car_posWorld1.lx >> 6;
		car_coord.y = state.opponentstate.car_posWorld1.ly >> 6;
		car_coord.z = state.opponentstate.car_posWorld1.lz >> 6;
		car_angle_y = state.opponentstate.car_rotate.y;
		my_car_bank = state.opponentstate.car_rotate.z;
		rot_x = state.opponentstate.car_rotate.x;
	}

	view_rot_x = -1;
	cam_bank = 0;
	
	// Set camera position, based on the car position and the camera mode
	switch (cammd) {
	case 2:
		offset_v.x = 0;
		offset_v.y = 0;
		offset_v.z = 0x4000;
		car_matrix = matrotzxy(-my_car_bank, -car_angle_y, -rot_x, 0);
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		si = polang(rot_ofs.x, rot_ofs.z);
		offset_v.x = 0;
		offset_v.y = 0;
		offset_v.z = custom_dist;
		car_matrix = matrotzxy(0, -custom_elev_angle, si - custom_azim_angle, 0);
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		viewpoint.x = car_coord.x + rot_ofs.x;
		viewpoint.y = car_coord.y + rot_ofs.y;
		viewpoint.z = car_coord.z + rot_ofs.z;
		break;
	case 1:
		viewpoint.x = state.game_vec1[followflg].x;
		viewpoint.z = state.game_vec1[followflg].z;
		viewpoint.y = state.game_vec1[followflg].y;
		break;
	case 0:
		view_rot_x = rot_x & 0x3ff;
		cam_elev = car_angle_y & 0x3ff;
		cam_bank = my_car_bank & 0x3ff;
		car_matrix = matrotzxy(-my_car_bank, -car_angle_y, -rot_x, 0);
		offset_v.x = 0;
		offset_v.z = 0;
		offset_v.y = simdpl.car_height - 6;
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		viewpoint.x = car_coord.x + rot_ofs.x;
		viewpoint.y = car_coord.y + rot_ofs.y;
		viewpoint.z = car_coord.z + rot_ofs.z;
		break;
	case 3:
		viewpoint.x = trackdata9[state.field_3F7[followflg]].x;
		viewpoint.y = trackdata9[state.field_3F7[followflg]].y + word_44D20 + 0x5A;
		viewpoint.z = trackdata9[state.field_3F7[followflg]].z;
		break;
	}

	// Unknown part; seems to be performing some initialization
	if (view_rot_x == -1) {
		build_obj(&viewpoint, &viewpoint);
		if (viewpoint.y < terrhgt) {
			viewpoint.y = terrhgt;
		}

		if (byte_4392C != 0) {		
			si = plnoriginop(planindex, viewpoint.x, viewpoint.y, viewpoint.z);
			if (si < 0xC) {			
				vunk2.x = 0;
				vunk2.y = 0xC - si;
				vunk2.z = 0;
				planindexcpy = planindex;
				pstf36mminf40sar2 = 0;
				pstateminusx2 = 0;
				pstateminusz2 = 0;
				pstateminusy2 = 0;
				plnrotop();
				viewpoint.x += vplanerotopresult.x;
				viewpoint.y += vplanerotopresult.y;
				viewpoint.z += vplanerotopresult.z;
			}
		}

		view_rot_x = (-polang(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z)) & 0x3FF;
		ground_dist = polradius2d(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z);
		cam_elev = polang(car_coord.y - viewpoint.y + 0x32, ground_dist) & 0x3FF;
	}

	if (cam_bank > 1 && cam_bank < 0x3FF) {
		bank = cam_bank;
	} else {
		bank = 0;
	}

	paint = byte_3C0C6[(state.game_frame == 0 ? word_46468 : state.game_frame) & 0xF];

	// Select the vector specifying the 23 tiles to draw. The vector contains
	// 24 elements, each 3 bytes long, in format (east_offset, south_offset,
	// detail threshold). A tile is drawn only if its detail threshold is lower
	// enough (0 = draw always, 1 = only if graphic detail is MEDIUM or FULL,
	// 2 = only if graphic detail is FULL).
	// There are 8 possible vectors, but they are all rotations/reflections of a
	// basic schema. Which is chosen depends on the cur_view_heading_val of the car. For a
	// car cur_view_heading_val north ($), the schema is the following:
	//
	// OOOOO
	// OOOOO
	// OOOOO
	// OOOOO
	//  O$O
	//
	// Also, note that the tiles appear in the vector in drawing order
	// (farthest tiles first). If a car is cur_view_heading_val north but slightly west, the
	// algo will draw the NW tile before the NE, and vice-versa

	cur_view_heading_val = select_rot(bank, cam_elev, view_rot_x, clip, 0);
	si = (cur_view_heading_val & 0x3FF) >> 7;
	tile_table = ahead_tables[si];

	view_mat = *matrotzxy(bank, cam_elev, 0, 1);
	offset_v.x = 0;
	offset_v.y = 0;
	offset_v.z = 0x3E8;
	mat_vec(&offset_v, &view_mat, &elem_pos);
	if (elem_pos.z > 0) {
		cur_sky_sign = 1;
	} else {
		cur_sky_sign = -1;
	}

	// Draw 8 shapes (still TBD what they are), but only if the detail
	// level is the max one
	if (detail_lvl == 0) {
		curtshape->rectptr = &rcunk[7];
		curtshape->ts_flags = flagbits | 7;
		curtshape->rotvec.x = 0;
		curtshape->rotvec.y = 0;
		curtshape->unk = 0x400;
		curtshape->material = 0;

		for (nfences = 0; nfences < 8; nfences++) {
			si = (word_3BE34[nfences] + view_rot_x + runrandom) & 0x3ff;
			if (si < 0x87 || si > 0x379) {
				matroty(&sky_mat, si);
				offset_v.x = 0;
				offset_v.y = 0xAE6 - viewpoint.y;
				offset_v.z = 0x3A98; //15000
				mat_vec(&offset_v, &sky_mat, &rot_ofs);
				rot_ofs.z = 0x3A98; //15000
				mat_vec(&rot_ofs, &view_mat, &curtshape->pos);
				if (curtshape->pos.z > 0xC8) {
					curtshape->shapeptr = off_3BE44[nfences];
					curtshape->rotvec.z = -view_rot_x;
					result = trans_op(&curtshape[0]);
					(void) result; // we cannot be out of memory as we are just starting to process
				}
			}
		}
	}

/*
; -----------------------------------------------------------------------------------------------
*/

	cam_tx = viewpoint.x >> 0xA;
	cam_tz = -((viewpoint.z >> 0xA) - 0x1D);
	if (detail_lvl != 0) {
		car_x_tile = state.playerstate.car_posWorld1.lx >> 16;
		car_z_tile = 0x1D - (state.playerstate.car_posWorld1.lz >> 16);
	}

	for (si = 0; si < 0x17; si++) {
		skipped[si] = 0;
	}

	// Select the detail level (FULL if 1st or 2nd option in the graphics menu
	// were chosen, MEDIUM if the 3rd, FASTEST if 4th or 5th)
	thresh = detthrlevel[detail_lvl];
	
	// Cycle on the 23 tiles to draw, determine if they really need to be drawn
	for (si = 0x16; si >= 0; si--) {
	if (skipped[si] != 0) continue;
	if (tile_table[si * 3 + 2] > thresh) {
	skipped[si] = 2;
	} else {
	tx = tile_table[si * 3] + cam_tx;
	trow = tile_table[si * 3 + 1] + cam_tz;
	if (tx < 0 || tx > 0x1D || trow < 0 || trow > 0x1D) {
	skipped[si] = 2;
	} else {
					track_elem = td14map[trackrows[trow] + tx];
					tile_ground = td15map[terrrows[trow] + tx];
	
					if (track_elem != 0) {
						if (tile_ground >= 7 && tile_ground < 0xB) {
							track_elem = subst_hillroad(tile_ground, track_elem);
							tile_ground = 0;
						}
	
						switch (track_elem) {
						case 0xFD:
							tx--;
							trow--;
							track_elem = td14map[trackrows[trow] + tx];
							tile_ground = td15map[terrrows[trow] + tx];
							break;
						case 0xFE:
							trow--;
							track_elem = td14map[trackrows[trow] + tx];
							tile_ground = td15map[terrrows[trow] + tx];
							break;
						case 0xFF:
							tx--;
							track_elem = td14map[trackrows[trow] + tx];
							tile_ground = td15map[terrrows[trow] + tx];
							break;
						}
					}
	
					terrain_list[si] = tile_ground;
					lod_levels[si] = tile_table[si * 3 + 2];
	
					if (track_elem != 0 && detail_lvl != 0 &&
						trklst[track_elem].ss_physicalModel >= 0x40 &&
						(tx != car_x_tile || trow != car_z_tile))
					{
						track_elem = 0;
					}
	
					east_list[si] = tx;
					south_tab[si] = trow;
					elem_list[si] = track_elem;
	
					if (track_elem != 0) {
						j = trklst[track_elem].ss_multiTileFlag;
						if (j != 0) {
							tile_east2 = tx - cam_tx;
							drow = trow - cam_tz;
							switch (j) {
							case 1:
								for (j2 = 0; j2 < si; j2++) {
									if (tile_table[j2 * 3] == tile_east2 &&
										(tile_table[j2 * 3 + 1] == drow ||
										 tile_table[j2 * 3 + 1] == drow + 1))
										skipped[j2] = 1;
								}
								break;
							case 2:
								for (j2 = 0; j2 < si; j2++) {
									if (tile_table[j2 * 3 + 1] == drow &&
										(tile_table[j2 * 3] == tile_east2 ||
										 tile_table[j2 * 3] == tile_east2 + 1))
										skipped[j2] = 1;
								}
								break;
							case 3:
								for (j2 = 0; j2 < si; j2++) {
									if ((tile_table[j2 * 3] == tile_east2 ||
										 tile_table[j2 * 3] == tile_east2 + 1) &&
										(tile_table[j2 * 3 + 1] == drow ||
										 tile_table[j2 * 3 + 1] == drow + 1))
										skipped[j2] = 1;
								}
						}
					}
						
					}
	}
	}
	}

	// Draw own wheels
	car_east1 = -1;
	car_zofs = 0;
	if (cammd != 0 || followflg != 0) {

		if (state.playerstate.car_crashBmpFlag != 2) {

			car_matrix = matrotzxy(-state.playerstate.car_rotate.z, -state.playerstate.car_rotate.y, -state.playerstate.car_rotate.x, 0);
			j = -1;
			j2 = -1;
			for (wheel_idx = 0; wheel_idx < 4; wheel_idx++) {
				offset_v = simdpl.wheel_coords[wheel_idx];
				mat_vec(&offset_v, car_matrix, &elem_pos); //; rotating car wheels, maybe?
				// Tile where the wheel is standing
				tx = (elem_pos.x + state.playerstate.car_posWorld1.lx) >> 16; // bits 16-24
				trow = -(((elem_pos.z + state.playerstate.car_posWorld1.lz) >> 16) - 0x1D);

				for (si = 0x16; si > j; si--) {
					if (skipped[si] != 2 && tile_table[si * 3] + cam_tx == tx && tile_table[si * 3 + 1] + cam_tz == trow) {
						car_east1 = tx;
						wheel_south = trow;
						j = si;
						j2 = wheel_idx;
					}
				}
			}

			if (j2 != -1) {
				if (state.playerstate.car_surfaceWhl[0] != 4 || state.playerstate.car_surfaceWhl[1] != 4 || state.playerstate.car_surfaceWhl[2] != 4 || state.playerstate.car_surfaceWhl[3] != 4) {
					offset_v.x = 0;
					offset_v.z = 0;
					offset_v.y = 0x7530;
					mat_vec(&offset_v, car_matrix, &elem_pos);
					mat_vec(&elem_pos, &mattmp, &offset_v);
					if (offset_v.z <= 0) {
						car_zofs = -0x800 ;
					} else {
						car_zofs = 0x800;
					}
				}
			}
		}
	}

	// Draw opponent's wheels
	cur_opp_east = -1;
	opp_zbias_val = 0;
	if (gmcfg.game_opponenttype != 0) {

		if (cammd != 0 || followflg == 0) {
			if (state.opponentstate.car_crashBmpFlag != 2) {
				car_matrix = matrotzxy(-state.opponentstate.car_rotate.z, -state.opponentstate.car_rotate.y, -state.opponentstate.car_rotate.x, 0);
				j = -1;
				j2 = -1;

				for (wheel_idx = 0; wheel_idx < 4; wheel_idx++) {
					offset_v = simdopp.wheel_coords[wheel_idx];
					mat_vec(&offset_v, car_matrix, &elem_pos); //; rotating car wheels, maybe?
					tx = (elem_pos.x + state.opponentstate.car_posWorld1.lx) >> 16; // bits 16-24
					trow = -(((elem_pos.z + state.opponentstate.car_posWorld1.lz) >> 16) - 0x1D);

					for (si = 0x16; si > j; si--) {
						if (skipped[si] != 2 && tile_table[si * 3] + cam_tx == tx && tile_table[si * 3 + 1] + cam_tz == trow) {
							cur_opp_east = tx;
							opp_wheel_south_val = trow;
							j = si;
							j2 = wheel_idx;
						}
					}
				}

				if (j2 != -1) {
						
					if (state.opponentstate.car_surfaceWhl[0] != 4 || state.opponentstate.car_surfaceWhl[1] != 4 || state.opponentstate.car_surfaceWhl[2] != 4 || state.opponentstate.car_surfaceWhl[3] != 4) {
						offset_v.x = 0;
						offset_v.z = 0;
						offset_v.y = 0x7530;
						mat_vec(&offset_v, car_matrix, &elem_pos);
						mat_vec(&elem_pos, &mattmp, &offset_v);
						if (offset_v.z <= 0) {
							opp_zbias_val = -0x800; //0xF800; // signed number!
						} else {
							opp_zbias_val = 0x800;
						}
					}
				}
			}
		}
	}
//; -----------------------------------------------------------------------------


	ovl_pending = 0;
	si = 0;
	
	// With the information collected by the previus tile-scan algorithm,
	// proceed to draw the shapes in each tile. Start from the farthest
	// (painter's algorithm)
	for (si = 0; si < 0x17; si++) {
		if (skipped[si] != 0) {
			continue;
		}
		tx = east_list[si];
		trow = south_tab[si];
		track_elem = elem_list[si];
		tile_ground = terrain_list[si];
		tile_lod = lod_levels[si];
		start_bias = 0;
		if (track_elem != 0) {
			trkobj = &trklst[track_elem];
			switch (trkobj->ss_multiTileFlag) {
			case 0:
				nfences = 1;
				fence_list = unk_3C0EE;
				break;
			case 1:
				nfences = 2;
				fence_list = unk_3C0F0;
				break;
			case 2:
				nfences = 3;
				fence_list = unk_3C0F4;
				break;
			case 3:
				nfences = 4;
				fence_list = unk_3C0F8;
				break;
			}
		} else {
			nfences = 1;
			fence_list = unk_3C0F4;
		}

		// Draw the fence
		for (j = 0; j < nfences; j++) {
			tile_east2 = fence_list[j * 2] + tx;
			drow = fence_list[j * 2 + 1] + trow;

			if (detail_lvl != 0 && (tile_east2 != car_x_tile || drow != car_z_tile))
				continue;

			switch (tile_east2) {
			case 0:
				switch (drow) {
				case 0:
					j2 = 7;
					break;
				case 0x1D:
					j2 = 5;
					break;
				default:
					j2 = 6;
				}
				break;
			case 0x1D:
				switch (drow) {
				case 0:
					j2 = 1;
					break;
				case 0x1D:
					j2 = 3;
					break;
				default:
					j2 = 2;
				}
				break;
			default:
				switch (drow) {
				case 0:
					j2 = 0;
					break;
				case 0x1D:
					j2 = 4;
					break;
				default:
					j2 = -1;
				}
			}

			if (j2 != -1) {
				ovl = &trklst[fence_codes[j2]];
				curtshape->shapeptr = (tile_lod == 0) ? ovl->ss_shapePtr : ovl->ss_loShapePtr;
				curtshape->pos.x = trackctrpos2[tile_east2] - viewpoint.x;
				curtshape->pos.y = -viewpoint.y;
				curtshape->pos.z = trackctrpos[drow] - viewpoint.z;
				curtshape->rectptr = &rcunk[1];
				curtshape->ts_flags = flagbits | 5;
				curtshape->rotvec.x = 0;
				curtshape->rotvec.y = 0;
				curtshape->rotvec.z = word_3C0D6[j2];
				curtshape->unk = 0x400;
				curtshape->material = 0;
				result = trans_op(&curtshape[0]);
				if (result > 0)
					goto draw_done;
			}
		
		}

		// terrain type 0x06: a flat piece of land at an elevated level
		if (tile_ground == 6) {
			hill_elev = hillconsts[1];
			if (track_elem != 0)
				tile_ground = 0;
		} else {
			hill_elev = 0;

			// Special treatment of elevated corners
			switch (track_elem) {
			case 0x69:
			case 0x6A:
			case 0x6B:
			case 0x6C:
				for (j = 0; j < 4; j++) {
					switch (j) {
					case 0:
						tile_east2 = tx;
						drow = trow;
						break;
					case 1:
						tile_east2 = tx + 1;
						drow = trow;
						break;
					case 2:
						tile_east2 = tx;
						drow = trow + 1;
						break;
					case 3:
						tile_east2 = tx + 1;
						drow = trow + 1;
						break;
					}
					tile_ground = td15map[terrrows[drow] + tile_east2];
					if (tile_ground != 0) {
						trkobj = &scene2[tile_ground];
						curtshape->shapeptr = trkobj->ss_shapePtr;
						curtshape->pos.x = trackctrpos2[tile_east2] - viewpoint.x;
						curtshape->pos.y = -viewpoint.y;
						curtshape->pos.z = trackctrpos[drow] - viewpoint.z;
						curtshape->rectptr = &rcunk[1];
						curtshape->ts_flags = flagbits | 5;
						curtshape->rotvec.x = 0;
						curtshape->rotvec.y = 0;
						curtshape->rotvec.z = trkobj->ss_rotY;
						curtshape->unk = 0x400;
						curtshape->material = 0;
						result = trans_op(&curtshape[0]);
						if (result > 0)
							goto draw_done;
					}
				}
				tile_ground = 0;
			}
		}

		// The rest of the rendering loop still needs to be analyzed in detail.
		// Anyway, the gist is that every tile is associated with various shape,
		// each of which is rendered via a call to `transformed_shape_op`. The
		// result of such fn is checked each time, since a return value of 1
		// means we ran out of memory

		if (tile_ground != 0) {
			trkobj = &scene2[tile_ground];
			curtshape->shapeptr = trkobj->ss_shapePtr;
			curtshape->pos.x = trackctrpos2[tx] - viewpoint.x;
			curtshape->pos.y = hill_elev - viewpoint.y;
			curtshape->pos.z = trackctrpos[trow] - viewpoint.z;
			if (hill_elev == 0) {
				curtshape->rectptr = &rcunk[1];
			} else {
				curtshape->rectptr = &rcunk[2];
			}

			curtshape->ts_flags = flagbits | 5;
			curtshape->rotvec.x = 0;
			curtshape->rotvec.y = 0;
			curtshape->rotvec.z = trkobj->ss_rotY;
			curtshape->unk = 0x400;
			curtshape->material = 0;
			result = trans_op(&curtshape[0]);
			if (result > 0)
				goto draw_done;
		}

		tshapecnt = 0;
		curtshapep = curtshape;
		if (track_elem != 0) {
			trkobj = &trklst[track_elem];
			if ((trkobj->ss_multiTileFlag & 1) != 0) {
				obj_zpos = trkp[trow];
				drow = trow + 1;
			} else {
				obj_zpos = trackctrpos[trow];
				drow = trow;
			}

			if ((trkobj->ss_multiTileFlag & 2) != 0) {
				objx = trkp2[1 + tx];
				tile_east2 = tx + 1;
			} else {
				objx = trackctrpos2[tx];
				tile_east2 = tx;
			}

			elem_pos.x = objx - viewpoint.x;
			elem_pos.y = hill_elev - viewpoint.y;
			elem_pos.z = obj_zpos - viewpoint.z;
			if (hill_elev != 0) {
				switch (trkobj->ss_multiTileFlag) {
				case 0:
					j2 = 1;
					offsets = unk_3C0A2[0];
					break;
				case 1:
					j2 = 2;
					offsets = unk_3C0A2[1];
					break;
				case 2:
					j2 = 2;
					offsets = unk_3C0A2[3];
					break;
				case 3:
					j2 = 4;
					offsets = unk_3C0A2[5];
					break;
				}

				for (j = 0; j < j2; j++) {
					curtshape->pos.x = *offsets++ + elem_pos.x;
					curtshape->pos.y = elem_pos.y;
					curtshape->pos.z = *offsets++ + elem_pos.z;
					curtshape->shapeptr = &game3dshapes[0x3B2 / sizeof(struct SHAPE3D)];
					curtshape->rectptr = &rcunk[2];
					curtshape->ts_flags = flagbits | 5;
					curtshape->rotvec.x = 0;
					curtshape->rotvec.y = 0;
					curtshape->rotvec.z = 0;
					curtshape->unk = 0x800;
					curtshape->material = 0;
					result = trans_op(&curtshape[0]);
					if (result > 0)
						goto draw_done;
				}
			}

			if (trkobj->ss_ssOvelay != 0) {
				ovl = &trklst[trkobj->ss_ssOvelay];
				if (tile_lod != 0) {
					curtshape[1].shapeptr = ovl->ss_loShapePtr;
				} else {
					curtshape[1].shapeptr = ovl->ss_shapePtr;
				}

				if (curtshape[1].shapeptr != 0) {
					curtshape[1].pos = elem_pos;
					curtshape[1].rotvec.x = 0;
					curtshape[1].rotvec.y = 0;
					curtshape[1].rotvec.z = ovl->ss_rotY;
					if (ovl->ss_multiTileFlag != 0) {
						curtshape[1].unk = 0x400;
					} else {
						curtshape[1].unk = 0x800;
					}

					if (ovl->ss_surfaceType >= 0) {
						curtshape[1].material = ovl->ss_surfaceType;
					} else {
						curtshape[1].material = paint;
					}

					curtshape[1].ts_flags = ovl->ss_ignoreZBias | flagbits | 4;
					if ((curtshape[1].ts_flags & 1) != 0) {
						curtshape[1].rectptr = &rcunk[1];
						result = trans_op(&curtshape[1]);
						if (result > 0)
							goto draw_done;
					} else {
						curtshape[1].rectptr = &rcunk[2];
						ovl_pending = 1;
					}
				}
			}

			if (tile_lod != 0) {
				curtshape->shapeptr = trkobj->ss_loShapePtr;
			} else {
				curtshape->shapeptr = trkobj->ss_shapePtr;
			}

			curtshape->pos = elem_pos; // whatever
			curtshape->rotvec.x = 0;
			curtshape->rotvec.y = 0;
			curtshape->rotvec.z = trkobj->ss_rotY;
			if (trkobj->ss_multiTileFlag != 0) {
				curtshape->unk = 0x400;
			} else {
				curtshape->unk = 0x800;
			}

			curtshape->ts_flags = trkobj->ss_ignoreZBias | flagbits | 4;
			if (trkobj->ss_surfaceType >= 0) {
				curtshape->material = trkobj->ss_surfaceType;
			} else {
				curtshape->material = paint;
			}

			if ((trkobj->ss_ignoreZBias & 1) != 0) {
				curtshape->rectptr = &rcunk[1];
				result = trans_op(&curtshape[0]);
				if (result > 0)
					goto draw_done;
			} else {
				curtshape->rectptr = &rcunk[2];
				transformed_shape_add_for_sort(0, 0);
				if (ovl_pending != 0) {
					ovl_pending = 0;
					transformed_shape_add_for_sort(-0x800 /*0xF800*/, 0);
					if (car_zofs != 0) {
						car_zofs = -0x400;//0xFC00;
					}

					if (opp_zbias_val != 0) {
						opp_zbias_val -= 0x400;
					}
				}

				if (tx == stcol2 && trow == strow2) {
					start_bias = 0;
				} else {
					start_bias = -1;
				}
			}

			checkpoint_no = trkdata19[trackrows[trow] + tx];
			if (checkpoint_no != -1) {
				if (state.field_3FA[checkpoint_no] != 0) {
					if (state.field_42A != 0) {
						for (j2 = 0; j2 < 0x18; j2++) {
							if (state.field_38E[j2] != 0) if (checkpoint_no + 2 == state.field_443[j2]) {
								trkobj = &scene3[state.field_42B[j2]];
								curtshapep->pos.x = (state.game_longs1[j2] >> 6) + td10checkrel[checkpoint_no * 3 + 0] - viewpoint.x;
								curtshapep->pos.y = (state.game_longs2[j2] >> 6) + td10checkrel[checkpoint_no * 3 + 1] - viewpoint.y;
								curtshapep->pos.z = (state.game_longs3[j2] >> 6) + td10checkrel[checkpoint_no * 3 + 2] - viewpoint.z;
								curtshapep->shapeptr = trkobj->ss_shapePtr;
								curtshapep->rectptr = &rcunk[2];
								curtshapep->ts_flags = flagbits | 5;
								curtshapep->rotvec.x = -state.field_2FE[j2];
								curtshapep->rotvec.y = -state.field_32E[j2];
								curtshapep->rotvec.z = -state.field_35E[j2];
								curtshapep->unk = 0x400;
								curtshapep->material = 0;
								transformed_shape_add_for_sort(0, 0);
							}
						}
					}
				} else {
					trkobj = &trklst[212 + trkdata23[checkpoint_no]];
					curtshapep->pos.x = td10checkrel[checkpoint_no * 3 + 0] - viewpoint.x;
					curtshapep->pos.y = td10checkrel[checkpoint_no * 3 + 1] - viewpoint.y;
					curtshapep->pos.z = td10checkrel[checkpoint_no * 3 + 2] - viewpoint.z;
					curtshapep->shapeptr = trkobj->ss_shapePtr;
					curtshapep->rectptr = &rcunk[2];
					curtshapep->ts_flags = flagbits | 4;
					curtshapep->rotvec.x = 0;
					curtshapep->rotvec.y = 0;
					curtshapep->rotvec.z = td08dir[checkpoint_no];
					curtshapep->unk = 0x64;
					curtshapep->material = 0;
					transformed_shape_add_for_sort(0, 0);
				}
			}
		} else {
			tile_east2 = tx;
			drow = trow;
		}

		if ((car_east1 == tx || car_east1 == tile_east2) && (wheel_south == trow || wheel_south == drow)) {
			if (state.field_42A != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (state.field_38E[j2] != 0) if (state.field_443[j2] == 0) {
						trkobj = &scene3[state.field_42B[j2]];
						curtshapep->pos.x = (state.game_longs1[j2] + state.playerstate.car_posWorld1.lx >> 6) - viewpoint.x;
						curtshapep->pos.y = (state.game_longs2[j2] + state.playerstate.car_posWorld1.ly >> 6) - viewpoint.y;
						curtshapep->pos.z = (state.game_longs3[j2] + state.playerstate.car_posWorld1.lz >> 6) - viewpoint.z;
						curtshapep->shapeptr = trkobj->ss_shapePtr;
						curtshapep->rectptr = &rcunk[2];
						curtshapep->ts_flags = flagbits | 5;
						curtshapep->rotvec.x = -state.field_2FE[j2];
						curtshapep->rotvec.y = -state.field_32E[j2];
						curtshapep->rotvec.z = -state.field_35E[j2];
						curtshapep->unk = 0x400;
						curtshapep->material = gmcfg.game_playermaterial;
						transformed_shape_add_for_sort(car_zofs & start_bias, 0);
					}
				}
			}

			trkobj = &trklst[2];//0x1C / sizeof(struct TRACKOBJECT)];
			curtshapep->pos.x = (state.playerstate.car_posWorld1.lx >> 6) - viewpoint.x;
			curtshapep->pos.y = (state.playerstate.car_posWorld1.ly >> 6) - viewpoint.y;
			curtshapep->pos.z = (state.playerstate.car_posWorld1.lz >> 6) - viewpoint.z;
			
			if (tile_lod != 0 || detail_lvl > 2) {
				curtshapep->shapeptr = trkobj->ss_loShapePtr;
			} else {
				curtshapep->shapeptr = trkobj->ss_shapePtr;
				sub_204AE(&game3dshapes[0x0AD4 / sizeof(struct SHAPE3D)].shape3d_verts[8], state.playerstate.car_steeringAngle, &state.playerstate.car_rc2, word_443E8, carvecs, &carvec);
			}

			if (slowmgmtcopy != 0) {
				curtshapep->rectptr = &rcunk[3];
				curtshapep->ts_flags = 0xC;
			} else if (state.playerstate.car_crashBmpFlag == 1) {
				cliprect1 = clipunk;
				curtshapep->rectptr = &cliprect1;
				curtshapep->ts_flags = 0xC;
			} else {
				curtshapep->ts_flags = 4;
			}

			curtshapep->rotvec.x = -state.playerstate.car_rotate.z;
			curtshapep->rotvec.y = -state.playerstate.car_rotate.y;
			curtshapep->rotvec.z = -state.playerstate.car_rotate.x;
			curtshapep->unk = 0x12C;
			curtshapep->material = gmcfg.game_playermaterial;
			transformed_shape_add_for_sort(car_zofs & start_bias, 2);
		}
		
		if ((cur_opp_east == tx || cur_opp_east == tile_east2) && (opp_wheel_south_val == trow || opp_wheel_south_val == drow)) {
			if (state.field_42A != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (state.field_38E[j2] != 0) {
						if (state.field_443[j2] == 1) {
							trkobj = &scene3[state.field_42B[j2]];
							curtshapep->pos.x = (state.game_longs1[j2] + state.opponentstate.car_posWorld1.lx >> 6) - viewpoint.x;
							curtshapep->pos.y = (state.game_longs2[j2] + state.opponentstate.car_posWorld1.ly >> 6) - viewpoint.y;
							curtshapep->pos.z = (state.game_longs3[j2] + state.opponentstate.car_posWorld1.lz >> 6) - viewpoint.z;
							curtshapep->shapeptr = trkobj->ss_shapePtr;
							curtshapep->rectptr = &rcunk[2];
							curtshapep->ts_flags = flagbits | 5;
							curtshapep->rotvec.x = -state.field_2FE[j2];
							curtshapep->rotvec.y = -state.field_32E[j2];
							curtshapep->rotvec.z = -state.field_35E[j2];
							curtshapep->unk = 0x400;
							curtshapep->material = gmcfg.game_opponentmaterial;
							transformed_shape_add_for_sort(opp_zbias_val & start_bias, 0);
						}
					}
				}
			}
			trkobj = &trklst[3];//0x2A / sizeof(struct TRACKOBJECT)];
			curtshapep->pos.x = (state.opponentstate.car_posWorld1.lx >> 6) - viewpoint.x;
			curtshapep->pos.y = (state.opponentstate.car_posWorld1.ly >> 6) - viewpoint.y;
			curtshapep->pos.z = (state.opponentstate.car_posWorld1.lz >> 6) - viewpoint.z;

			if (tile_lod != 0 || detail_lvl > 2) {
				curtshapep->shapeptr = trkobj->ss_loShapePtr;
			} else {
				curtshapep->shapeptr = trkobj->ss_shapePtr;
				sub_204AE(&game3dshapes[0x0AEA / sizeof(struct SHAPE3D)].shape3d_verts[8], state.opponentstate.car_steeringAngle, &state.opponentstate.car_rc2, word_4448A, ocarvecs, &ocarvec);
			}

			if (slowmgmtcopy != 0) {
				curtshapep->rectptr = &rcunk[4];
				curtshapep->ts_flags = 0xC;
			} else if (state.opponentstate.car_crashBmpFlag == 1) {
				cliprect2 = clipunk;
				curtshapep->rectptr = &cliprect2;
				curtshapep->ts_flags = 0xC;
			} else {
				curtshapep->ts_flags = 4;
			}

			curtshapep->rotvec.x = -state.opponentstate.car_rotate.z;
			curtshapep->rotvec.y = -state.opponentstate.car_rotate.y;
			curtshapep->rotvec.z = -state.opponentstate.car_rotate.x;
			curtshapep->unk = 0x12C;
			curtshapep->material = gmcfg.game_opponentmaterial;
			transformed_shape_add_for_sort(opp_zbias_val & start_bias, 3);
		}

		if (state.game_inputmode == 0) {
			if ((tx == stcol2 || tile_east2 == stcol2) && (trow == strow2 || drow == strow2)) {

				j = mulscl(cosfast(word_44DCA), 0x24);
				nfences = mulscl(sinfast(word_44DCA), 0x24) + 0x38;

				vptr = &game3dshapes[0x98A / sizeof(struct SHAPE3D)].shape3d_verts[8];
				vptr[0].x = j - 0x24;
				vptr[1].x = j - 0x24;
				vptr[2].x = 0x24 - j;
				vptr[3].x = 0x24 - j;

				vptr[0].z = nfences;
				vptr[1].z = nfences;
				vptr[2].z = nfences;
				vptr[3].z = nfences;
				 
				curtshapep->pos.x =
					mulscl(sinfast(trkang + 0x200), 0x1B6) +
					mulscl(sinfast(trkang + 0x100), 0x24) + 
					trackctrpos2[stcol2] - viewpoint.x;
				curtshapep->pos.y = hillconsts[hillFlag] - viewpoint.y;
				curtshapep->pos.z =
					mulscl(cosfast(trkang + 0x200), 0x1B6) +
					mulscl(cosfast(trkang + 0x100), 0x24) + 
					trackctrpos[strow2] - viewpoint.z;

				curtshapep->shapeptr = &game3dshapes[0x98A / sizeof(struct SHAPE3D)];
				curtshapep->rectptr = &rcunk[2];
				curtshapep->ts_flags = flagbits | 4;
				curtshapep->rotvec.x = 0;
				curtshapep->rotvec.y = 0;
				curtshapep->rotvec.z = trkang;
				curtshapep->unk = 0x400;
				j = word_44DCA >> 6;
				if (j > 3) {
					j = 3;
				}

				curtshapep->material = j;
				transformed_shape_add_for_sort(start_bias & -0x800 /*0xF800*/, 0);
			}
		}

		if (tshapecnt != 0) {
			if (tshapecnt > 1) {
				heapsortorder(tshapecnt, tshapezs, tshapeixs);
			}

			// Draw red overlights on the brake lights on own and opponent's car
			for (j = 0; j < tshapecnt; j++) {
				// j2 is used for index into currenttransshape elsewhere
				j2 = tshapeixs[j];
				switch (tshapearg2arr[j2]) {
				case 2:
					if (state.playerstate.car_is_braking != 0) {
						backlights_ovr = 0x2F;
					} else {
						backlights_ovr = 0x2E;
					}
					break;
				case 3:
					if (state.opponentstate.car_is_braking != 0) {
						backlights_ovr = 0x2F;
					} else {
						backlights_ovr = 0x2E;
					}
					break;
				}

				result = trans_op(&curtshape[j2]);
				if (result > 0)
					goto draw_done;

				if (result == 0) {
					switch (tshapearg2arr[j2]) {
					case 2:
						if (state.playerstate.car_crashBmpFlag == 1)
							crashed[0] = 1;
						break;
					case 3:
						if (state.opponentstate.car_crashBmpFlag == 1)
							crashed[1] = 1;
						break;
					}
				}
			}
		}
	}

draw_done:
	// Draw the skybox
	skybox_result = skybox_op(page, clip, cur_sky_sign, &view_mat, bank, view_rot_x, viewpoint.y);
	sprset1size(0, 0x140, clip->top, clip->bottom);
	polyinfo();

	// This supposedly draws the explosion. The fact that it cycles three
	// different patterns, each 4 frames long, seems to corroborate the
	// hypothesis
	for (si = 0; si < 2; si++) {
		if (crashed[si] == 0) {
			continue;
		}
		if (slowmgmtcopy != 0) {
			if (si == 0)
				rect_ptr = &rcunk[3];
			else
				rect_ptr = &rcunk[4];
		} else {
			rect_ptr = (si == 0) ? &cliprect1 : &cliprect2;
		}

		if (rcintersect(rect_ptr, clip) == 0) {
			sprset1size(rect_ptr->left, rect_ptr->right, rect_ptr->top, rect_ptr->bottom);
			offset_v.x = (rect_ptr->right + rect_ptr->left) >> 1;
			offset_v.y = (rect_ptr->top + rect_ptr->bottom) >> 1;
			j = rect_ptr->right - rect_ptr->left;
			nfences = rect_ptr->bottom - rect_ptr->top;
			if (nfences > j) {
				j = nfences;
			}

			j2 = (state.game_frame >> 2) % 3 ;
			nfences = ((long)j << 8) / (long)sdg2widths[j2];
			shapeexpl(nfences, sdg2shps[j2], offset_v.x, offset_v.y);
		}
	}

/*
; --------------------------------------------------------
*/

	// Depict windscreen cracking after a crash
	sprset1size(0, 0x140, clip->top, clip->bottom);
	if (cammd == 0) {

		if (followflg != 0) {
			followed = &state.opponentstate;
			si = state.game_oEndFrame;
		} else {
			followed = &state.playerstate;
			si = state.game_pEndFrame;
		}

		if (followed->car_crashBmpFlag == 1) {
			if (slowmgmtcopy != 0) {
				rcunion(init_crak(state.game_frame - si, clip->top, clip->bottom - clip->top), rcunk, rcunk);
			} else {
				init_crak(state.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		} else if (followed->car_crashBmpFlag == 2) {
			if (slowmgmtcopy != 0) {
				rcunion(do_sinking(state.game_frame - si, clip->top, clip->bottom - clip->top), rcunk, rcunk);
			} else {
				do_sinking(state.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		}
	}

	// Show elapsed time
	if (replay_mode == 0) {
		if (state.game_inputmode != 0) {
			fmtframestr(&residbyte1, elaptm1 + elaptm2, 0);
			fontsetfontdef2(fontledresptr);
			if (slowmgmtcopy != 0) {
				rcunion(introtext(&residbyte1, 0x8C, roofhgt + 2, dlg_colour, 0), &rcunk[6], &rcunk[6]);
			} else {
				introtext(&residbyte1, 0x8C, roofhgt + 2, dlg_colour, 0);
			}

			fontsetfontdef();
		}
	}

	if (slowmgmtcopy != 0) {
		rcunion(draw_ingame_text(), rcunk, rcunk);
		if (skybox_result != 0) {
			rcunk[0] = *clip;
			for (si = 1; si < 15; si++) {
				rcunk[si] = clipunk;
			}
		}

		for (si = 0; si < 15; si++) {
			rectp[si] = rcunk[si];
		}
		word_449FC[page] = view_rot_x;
		word_463D6 = view_rot_x;

	} else {
		draw_ingame_text();
	}

}

extern char byte_454A4;
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far rectsorttop(char, struct RECTANGLE *, int *);
extern struct RECTANGLE rectarr3[15];
extern int word_355D4[15];
extern char rectarr3len;
extern char rcarrayixs[15];
extern struct RECTANGLE rcunk5;
extern void far rectlist_add(char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);
extern void far sprcopy2to12(void);
extern void far sprputimage(void far *);
extern void sprset1size(unsigned short left, unsigned short right, unsigned short top, unsigned short height);
extern struct SPRITE far *wndsprite;
extern int word_449FC[];
extern int word_463D6;

void far skybox_op_helper2(struct RECTANGLE *rectptr, int x, int horizon)
{
    {
        register int upper_height;
        if (detail_lvl != 4)
            upper_height = horizon - rectptr->top - skybox_current;
        else
            upper_height = horizon - rectptr->top;

        if (rectptr->bottom - rectptr->top < upper_height)
            upper_height = rectptr->bottom - rectptr->top;

        if (upper_height > 0) {
            sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->top + upper_height);
            sprite_clear_1_color(skybox_sky_color);
        }
    }

    {
        register int sky_x;
        if (detail_lvl != 4) {
            sky_x = ((x + 0x200) & 0x3ff) - 0x400;
            if (rectptr->top < horizon && horizon - word_454CE <= rectptr->bottom) {
                sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                sprite_putimage_and_alt(skyboxes[0], sky_x, horizon - skybox_ptr1);
                sprite_putimage_and_alt(skyboxes[1], sky_x + 0x140, horizon - skybox_ptr2);
                sprite_putimage_and_alt(skyboxes[2], sky_x + 0x200, horizon - skybox_ptr3);
                sprite_putimage_and_alt(skyboxes[3], sky_x + 0x340, horizon - skybox_ptr4);
                sprite_putimage_and_alt(skyboxes[0], sky_x + 0x400, horizon - skybox_ptr1);
            }
        }
    }

    {
        register int lower_height;
        register int ground_top;
        if (rectptr->top > horizon)
            ground_top = rectptr->top;
        else
            ground_top = horizon;

        lower_height = rectptr->bottom - ground_top;
        if (lower_height > 0) {
            sprset1size(rectptr->left, rectptr->right, ground_top, ground_top + lower_height);
            sprite_clear_1_color(skybox_ground_color);
        }
    }
}

int skybox_op(int preview_index, struct RECTANGLE *clip, int latitude,
              struct MATRIX *rotation, int projection_mode, int detail, int camera_y)
{
    register int i;
    register int temp;
    int xstart;
    int start_y;
    int slope;
    int draw_result;
    int found;
    int horizon;
    int clip_line[14];
    struct VECTOR transform_input;
    struct VECTOR vecs[6];
    struct POINT2D pts[6];
    struct RECTANGLE rc;
    struct RECTANGLE *rectptr;

    rectarr3len = 0;
    draw_result = 0;
    sprset1size(0, 0x140, clip->top, clip->bottom);

    if (projection_mode != 0) {
        transform_input.x = 0x4650 * latitude;
        transform_input.y = -camera_y;
        transform_input.z = 0x3a98 * latitude;
        mat_vec(&transform_input, rotation, &vecs[0]);
        transform_input.x = -0x4650 * latitude;
        mat_vec(&transform_input, rotation, &vecs[1]);

        if (vecs[0].z < 0 || vecs[1].z < 0) {
            temp = skybox_sky_color;
fill:
            sprset1size(0, 0x140, clip->top, clip->bottom);
            sprite_clear_1_color(temp);
            draw_result = 1;
            goto done;
        }

        vector_to_point(&vecs[0], &pts[0]);
        vector_to_point(&vecs[1], &pts[1]);

        if (pts[0].px > 0x140 && pts[1].px > 0x140) {
            if (pts[0].py < pts[1].py) {
                temp = skybox_sky_color;
                goto fill;
            }
            temp = skybox_ground_color;
            goto fill;
        }
        if (pts[0].px < 0 && pts[1].px < 0) {
            if (pts[0].py <= pts[1].py) {
                temp = skybox_ground_color;
                goto fill;
            }
            temp = skybox_sky_color;
            goto fill;
        }
        if (pts[0].py > clip->bottom && pts[1].py > clip->bottom) {
            if (pts[0].px <= pts[1].px) {
                temp = skybox_ground_color;
                goto fill;
            }
            temp = skybox_sky_color;
            goto fill;
        }
        if (pts[0].py < clip->top && pts[1].py < clip->top) {
            if (pts[0].px >= pts[1].px) {
                temp = skybox_ground_color;
                goto fill;
            }
            temp = skybox_sky_color;
            goto fill;
        }

        found = 0;
        if (detail_lvl != 4 && pts[1].px < 0 && pts[0].px > 0x140 &&
            draw_line_related(pts[1].px, pts[1].py,
                              pts[0].px, pts[0].py, clip_line) == 0) {
            temp = clip_line[3] - clip_line[5];
            if ((temp < 0 ? -temp : temp) < 0x60) {
                if (clip_line[1] == 0) {
                    start_y = clip_line[3];
                    slope = clip_line[5] - start_y;
                    found = 1;
                } else if (clip_line[1] == 0x13f) {
                    start_y = clip_line[5];
                    slope = clip_line[3] - start_y;
                    found = 1;
                }
            }
        }

        if (found != 0) {
            if (slowmgmtcopy != 0) {
                rc.left = 0;
                rcunk[5].left = 0;
                rc.right = 0x140;
                rcunk[5].right = 0x140;
                if (byte_454A4 != 0) {
                    rcunk[5].top = clip->top;
                    rcunk[5].bottom = clip->bottom;
                } else {
                    rcunk[5].top = (start_y < start_y + slope ? start_y : start_y + slope) - word_454CE;
                    if (clip->top > rcunk[5].top)
                        rcunk[5].top = clip->top;
                    rcunk[5].bottom = start_y > start_y + slope ? start_y : start_y + slope;

                    for (i = 0; i < 15; i++)
                        rcarrayixs[i] = 1;
                    rcarrayixs[5] = 3;

                    rc.top = 0;
                    rc.bottom = rcunk[5].top;
                    if (!rcintersect(&rc, clip)) {
                        rectarr3len = 0;
                        rectlist_add(15, rcarrayixs, rectp, rcunk,
                                           &rc, &rectarr3len, rectarr3);
                        for (temp = 0; temp < rectarr3len; temp++) {
                            rectptr = &rectarr3[temp];
                            sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                            sprite_clear_1_color(skybox_sky_color);
                        }
                    }

                    rc.top = rcunk[5].bottom;
                    rc.bottom = 0xc8;
                    if (!rcintersect(&rc, clip)) {
                        rectarr3len = 0;
                        rectlist_add(15, rcarrayixs, rectp, rcunk,
                                           &rc, &rectarr3len, rectarr3);
                        for (temp = 0; temp < rectarr3len; temp++) {
                            rectptr = &rectarr3[temp];
                            sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                            sprite_clear_1_color(skybox_ground_color);
                        }
                    }
                }
                rc.top = rcunk[5].top;
                rc.bottom = rcunk[5].bottom;
            } else {
                rc.top = clip->top;
                rc.bottom = clip->bottom;
            }
            rc.left = 0;
            rc.right = 0x140;
            if (!rcintersect(&rc, clip)) {
                xstart = 0;
                temp = (slope < 0 ? -slope : slope) + 1;
                if (temp > 0x20)
                    temp = 0x20;
                for (i = 0; i < temp; i++) {
                    rc.left = xstart;
                    rc.right = ((0x140 * i + 0x140) / temp) & video_flag3_isFFFF;
                    if (rc.left != rc.right) {
                        horizon = start_y + (slope * i) / temp;
                        skybox_op_helper2(&rc, detail, horizon);
                        xstart = rc.right;
                    }
                }
            }
        } else {
            i = polang(pts[0].px - pts[1].px, pts[0].py - pts[1].py) & 0x3ff;
            for (slope = 2; slope < 6; slope++) {
                if (slope < 4)
                    temp = 0;
                else
                    temp = 1;
                pts[slope].px = pts[temp].px + mulscl(0x3e80, sinfast(word_2C0FC[slope] + i));
                pts[slope].py = pts[temp].py + mulscl(0x3e80, cosfast(word_2C0FC[slope] + i));
            }
            skybox_op_helper(skybox_sky_color, 4, pts[0], pts[1], pts[3], pts[2]);
            skybox_op_helper(skybox_ground_color, 4, pts[0], pts[1], pts[4], pts[5]);
            draw_result = 1;
        }
    } else {
        transform_input.x = 0;
        transform_input.y = -camera_y;
        transform_input.z = 0x3a98 * latitude;
        mat_vec(&transform_input, rotation, &vecs[0]);
        if (vecs[0].z < 0) {
            sprite_clear_1_color(skybox_sky_color);
            if (slowmgmtcopy != 0) {
                draw_result = 1;
                rcunk[5].left = 0;
                rcunk[5].right = 0x140;
                rcunk[5].top = clip->top;
                rcunk[5].bottom = clip->bottom;
            }
            goto done;
        }

        vector_to_point(&vecs[0], &pts[0]);
        horizon = pts[0].py;
        if (clip->top > horizon)
            horizon = clip->top;

        if (latitude == 1) {
            if (slowmgmtcopy == 0)
                goto simple;
            rcunk[5].top = (detail_lvl == 4) ? horizon - 1 : horizon - word_454CE;
            rcunk[5].left = 0;
            rcunk[5].right = 0x140;
            rcunk[5].bottom = horizon;
            if (byte_454A4 != 0) {
simple:
                rc.left = 0;
                rc.right = 0x140;
                rc.top = clip->top;
                rc.bottom = clip->bottom;
                skybox_op_helper2(&rc, detail, horizon);
            } else {
                for (i = 0; i < 15; i++)
                    rcarrayixs[i] = 1;
                if (detail_lvl == 4)
                    word_449FC[preview_index] = word_463D6;
                if (word_449FC[preview_index] == detail &&
                    rectp[5].left == rcunk[5].left &&
                    rectp[5].right == rcunk[5].right &&
                    rectp[5].top == rcunk[5].top &&
                    rectp[5].bottom == rcunk[5].bottom)
                    rcarrayixs[5] = 0;
                else
                    rcarrayixs[5] = 3;
                rectarr3len = 0;
                rectlist_add(15, rcarrayixs, rectp, rcunk,
                                   clip, &rectarr3len, rectarr3);
                for (temp = 0; temp < rectarr3len; temp++)
                    skybox_op_helper2(&rectarr3[temp], detail, horizon);
            }
        } else {
            i = horizon - clip->top;
            if (clip->bottom - clip->top < i)
                i = clip->bottom - clip->top;
            if (i > 0) {
                sprset1size(0, 0x140, clip->top, clip->top + i);
                sprite_clear_1_color(skybox_ground_color);
            }
            i = clip->bottom - horizon;
            if (i > 0) {
                sprset1size(0, 0x140, horizon, horizon + i);
                sprite_clear_1_color(skybox_sky_color);
            }
            draw_result = 1;
        }
    }
done:
    return draw_result;
}

void transformed_shape_add_for_sort(int zadjust, char arg_2) {
	struct VECTOR transformedpos;
	struct VECTOR shapepos;

	shapepos = curtshapep->pos;
	mat_vec(&shapepos, &mattmp, &transformedpos);
	tshapezs[(int)tshapecnt] = transformedpos.z + zadjust;
	tshapearg2arr[(int)tshapecnt] = arg_2;
	tshapeixs[(int)tshapecnt] = (int)tshapecnt;
	tshapecnt++;
	curtshapep++;
}

void draw_track_preview(void)
{
    struct TRACKOBJECT *overlay;
    unsigned char elem;
    register int obj_x_pos;
    struct VECTOR vector;
    register int obj_height;
    int camera_distance;
    int obj_z;
    struct MATRIX *cam_matrix;
    struct TRANSFORMEDSHAPE3D transformed;
    int horizon;
    struct POINT2D screen_point;
    int corner;
    signed char cx;
    int rot_angle;
    signed char tile_col;
    signed char row_idx;
    signed char cz;
    unsigned char terr;
    struct TRACKOBJECT *track_obj;

    camera_distance = polradius2d(word_3C10E - word_3C108,
                                    word_3C112 - word_3C10C);
    rot_angle = polang(word_3C110 - word_3C10A, camera_distance);
    cam_matrix = matrotzxy(0, rot_angle, 0, 1);
    mat_vec(&unk_3C114, cam_matrix, &vector);
    vector_to_point(&vector, &screen_point);

    horizon = screen_point.py;
    if (horizon < 0)
        horizon = 0;
    sprset1size(0, 0x140, 0, horizon - skybox_current);
    sprite_clear_1_color(skybox_sky_color);
    sprset1size(0, 0x140, 0, 0x64);
    sprite_putimage_and_alt(skyboxes[2], 0, horizon - skybox_ptr3);
    sprite_putimage_and_alt(skyboxes[3], 0x140, horizon - skybox_ptr4);
    sprset1size(0, 0x140, horizon, 0xc8);
    sprite_clear_1_color(skybox_ground_color);
    sprset1size(0, 0x140, 0, 0xc8);
    select_rot(0, rot_angle, 0, &trackpreview_cliprect, 1);

    transformed.rotvec.x = 0;
    transformed.rotvec.y = 0;
    transformed.unk = 0x400;

    for (row_idx = 0; row_idx < 30; row_idx++) {
        for (tile_col = 0; tile_col < 30; tile_col++) {
            elem = td14map[trackrows[row_idx] + tile_col];
            terr = td15map[terrrows[row_idx] + tile_col];
            if (elem != 0) {
                if (terr >= 7 && terr < 11) {
                    elem = subst_hillroad(terr, elem);
                    terr = 0;
                }
                switch (elem) {
                case 0xfd:
                case 0xfe:
                case 0xff:
                    terr = 0;
                    elem = 0;
                    break;
                }
            }

            if (terr == 6) {
                obj_height = hillconsts[1];
                if (elem != 0)
                    terr = 0;
            } else {
                obj_height = 0;
                switch (elem) {
                case 0x69:
                case 0x6a:
                case 0x6b:
                case 0x6c:
                    for (corner = 0; corner < 4; corner++) {
                        switch (corner) {
                        case 0:
                            cx = tile_col;
                            cz = row_idx;
                            break;
                        case 1:
                            cx = tile_col + 1;
                            cz = row_idx;
                            break;
                        case 2:
                            cx = tile_col;
                            cz = row_idx + 1;
                            break;
                        case 3:
                            cx = tile_col + 1;
                            cz = row_idx + 1;
                            break;
                        }
                        terr = td15map[terrrows[cz] + cx];
                        if (terr != 0) {
                            track_obj = &scene2[terr];
                            transformed.shapeptr = track_obj->ss_shapePtr;
                            transformed.pos.x = (trackctrpos2[cx] - word_3C108) >> 1;
                            transformed.pos.y = (-word_3C10A) >> 1;
                            transformed.pos.z = (trackctrpos[cz] - word_3C10C) >> 1;
                            transformed.ts_flags = 5;
                            transformed.rotvec.x = 0;
                            transformed.rotvec.y = 0;
                            transformed.rotvec.z = track_obj->ss_rotY;
                            transformed.unk = 0x400;
                            transformed.material = 0;
                            trans_op(&transformed);
                        }
                    }
                    terr = 0;
                    break;
                }
            }

            if (terr != 0) {
                track_obj = &scene2[terr];
                transformed.shapeptr = track_obj->ss_loShapePtr;
                transformed.pos.x = (trackctrpos2[tile_col] - word_3C108) >> 1;
                transformed.pos.y = (obj_height - word_3C10A) >> 1;
                transformed.pos.z = (trackctrpos[row_idx] - word_3C10C) >> 1;
                transformed.rotvec.z = track_obj->ss_rotY;
                transformed.ts_flags = 5;
                transformed.material = 0;
                trans_op(&transformed);
            }

            if (elem != 0) {
                track_obj = &trklst[elem];
                obj_z = (track_obj->ss_multiTileFlag & 1) ? trkp[row_idx] : trackctrpos[row_idx];
                if (track_obj->ss_multiTileFlag & 2)
                    obj_x_pos = trkp2[tile_col + 1];
                else
                    obj_x_pos = trackctrpos2[tile_col];
                vector.x = (obj_x_pos - word_3C108) >> 1;
                vector.y = (obj_height - word_3C10A) >> 1;
                vector.z = (obj_z - word_3C10C) >> 1;

                if (obj_height != 0) {
                    switch (track_obj->ss_multiTileFlag) {
                    case 0:
                        transformed.shapeptr = &game3dshapes[43];
                        break;
                    case 1:
                        transformed.shapeptr = &game3dshapes[91];
                        break;
                    case 2:
                        transformed.shapeptr = &game3dshapes[92];
                        break;
                    case 3:
                        transformed.shapeptr = &game3dshapes[93];
                        break;
                    }
                    transformed.pos = vector;
                    transformed.rotvec.z = 0;
                    transformed.ts_flags = 5;
                    transformed.material = 0;
                    trans_op(&transformed);
                }

                if (track_obj->ss_ssOvelay != 0) {
                    overlay = &trklst[track_obj->ss_ssOvelay];
                    if (overlay->ss_loShapePtr != 0) {
                        transformed.shapeptr = overlay->ss_loShapePtr;
                        transformed.pos = vector;
                        transformed.rotvec.z = track_obj->ss_rotY;
                        transformed.ts_flags = 5;
                        if (overlay->ss_surfaceType >= 0)
                            transformed.material = overlay->ss_surfaceType;
                        else
                            transformed.material = 0;
                        trans_op(&transformed);
                    }
                }

                transformed.shapeptr = track_obj->ss_loShapePtr;
                transformed.pos = vector;
                transformed.rotvec.z = track_obj->ss_rotY;
                transformed.ts_flags = track_obj->ss_ignoreZBias | 4;
                if (track_obj->ss_surfaceType >= 0)
                    transformed.material = track_obj->ss_surfaceType;
                else
                    transformed.material = 0;
                trans_op(&transformed);
            }
            polyinfo();
        }
    }
}

struct RECTANGLE *draw_ingame_text(void)
{
    register int remainder;

    rect_ingame_text = clipunk;

    if (idle_expired != 0) {
        copy_string(&residbyte1, locate_text_resource(gameresptr, aDm1));
        rcunion(&rect_ingame_text,
            introtext(&residbyte1, font_op2_alt(&residbyte1), 0xAA, dlg_colour, 0),
            &rect_ingame_text);

        copy_string(&residbyte1, locate_text_resource(gameresptr, aDm2));
        rcunion(&rect_ingame_text,
            introtext(&residbyte1, font_op2_alt(&residbyte1), 0xB6, dlg_colour, 0),
            &rect_ingame_text);
    } else if (replay_mode == 0) {
        if (state.game_inputmode == 0) {
            copy_string(&residbyte1, locate_text_resource(gameresptr, aPre));
            rcunion(&rect_ingame_text,
                introtext(&residbyte1, font_op2_alt(&residbyte1), 0x5A, dlg_colour, 0),
                &rect_ingame_text);
        } else if (passed_security == 0) {
            copy_string(&residbyte1, locate_text_resource(gameresptr, aSe1));
            rcunion(&rect_ingame_text,
                introtext(&residbyte1, font_op2_alt(&residbyte1), 0x5D, dlg_colour, 0),
                &rect_ingame_text);
            copy_string(&residbyte1, locate_text_resource(gameresptr, aSe2));
            rcunion(&rect_ingame_text,
                introtext(&residbyte1, font_op2_alt(&residbyte1), 0x69, dlg_colour, 0),
                &rect_ingame_text);
        } else if (followflg == 0 && cammd == 0 && state.playerstate.car_crashBmpFlag == 0) {
            switch ((int)state.field_45D) {
            case 1:
                sprite_putimage_transparent(sdg2shps[3], 0x94, 0x5D);
                rcunion(&rect_ingame_text, &rect_ingame_text2, &rect_ingame_text);
                break;
            case 2:
                sprite_putimage_transparent(sdg2shps[4], 0x94, 0x5D);
                rcunion(&rect_ingame_text, &rect_ingame_text2, &rect_ingame_text);
                break;
            case 3:
                copy_string(&residbyte1, locate_text_resource(gameresptr, aWww));
                rcunion(&rect_ingame_text,
                    introtext(&residbyte1, font_op2_alt(&residbyte1), 0x5D, dlg_colour, 0),
                    &rect_ingame_text);
                break;
            }

            residbyte1 = 0;
            switch ((int)state.field_45E) {
            case 1:
                sprite_putimage_transparent(sdg2shps[3], 0x44, 0x71);
                rcunion(&rect_ingame_text, &rect_ingame_text3, &rect_ingame_text);
                copy_string(&residbyte1, locate_text_resource(gameresptr, aOpp));
                break;
            case 2:
                sprite_putimage_transparent(sdg2shps[4], 0xE4, 0x71);
                rcunion(&rect_ingame_text, &rect_ingame_text4, &rect_ingame_text);
                copy_string(&residbyte1, locate_text_resource(gameresptr, aOpp_0));
                break;
            }

            if (residbyte1 != 0) {
                rcunion(&rect_ingame_text,
                    introtext(&residbyte1, font_op2_alt(&residbyte1), 0x74, dlg_colour, 0),
                    &rect_ingame_text);
            }

            if (show_penalty_counter != 0) {
                copy_string(&residbyte1, locate_text_resource(gameresptr, aPen));
                fmtframestr(&residbyte1 + strlen(&residbyte1), penalty_time, 0);
                rcunion(&rect_ingame_text,
                    introtext(&residbyte1, font_op2_alt(&residbyte1), 0x66, dlg_colour, 0),
                    &rect_ingame_text);
            }
        }
    } else if (replay_mode == 2) {
        remainder = state.game_frame % framespersec;
        if (remainder < ((short)framespersec >> 1)) {
            copy_string(&residbyte1, locate_text_resource(gameresptr, aRpl_0));
            rcunion(&rect_ingame_text,
                introtext(&residbyte1, 0x138 - (strlen(&residbyte1) << 3), 0x0F, dlg_colour, 0),
                &rect_ingame_text);
        }
    }

    return &rect_ingame_text;
}

struct RECTANGLE *do_sinking(int frame, int top, int height)
{
    register int offset;

    if (frame > (int)framespersec * 4)
        frame = (int)framespersec * 4;

    offset = (int)(((long)height * frame) / (long)((int)framespersec * 4));

    rect_ingame_text.left = 0;
    rect_ingame_text.right = 0x140;
    rect_ingame_text.top = top + height - offset;
    rect_ingame_text.bottom = top + height;

    sprset1size(0, 0x140, rect_ingame_text.top, rect_ingame_text.bottom);
    sprite_clear_1_color(skybox_wat_color);
    return &rect_ingame_text;
}

struct RECTANGLE *init_crak(int frame, int top, int height)
{
    struct CRACK_LINE { struct POINT2D start, end; };
    struct CRACK_LINE far *crak_shape;
    int far *cinf_shape;
    int animation_counter;
    int total_lines;
    struct POINT2D start_point, crack_end, adjust_point;
    register int crackIndex;

    crak_shape = (struct CRACK_LINE far *)locate_shape_alt(gameresptr, "crak");
    cinf_shape = (int far *)locate_shape_alt(gameresptr, "cinf");

    animation_counter = frame / ((int)framespersec / 7);
    if (animation_counter >= cinf_shape[0])
        animation_counter = cinf_shape[0] - 1;
    total_lines = cinf_shape[animation_counter + 1];

    rect_ingame_text = clipunk;
    for (crackIndex = 0; crackIndex < total_lines; ++crackIndex) {
            start_point = crak_shape[crackIndex].start;
            crack_end = crak_shape[crackIndex].end;

        start_point.py = (int)(((long)start_point.py * height) / 200);
        crack_end.py = (int)(((long)crack_end.py * height) / 200);

        preRender_line(start_point.px, start_point.py + top - 1,
                       crack_end.px, crack_end.py + top - 1, 0);
        preRender_line(start_point.px, start_point.py + top + 1,
                       crack_end.px, crack_end.py + top + 1, 0);
        preRender_line(start_point.px, start_point.py + top,
                       crack_end.px, crack_end.py + top, dlg_colour);

        if (slowmgmtcopy != 0) {
            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top - 1;
            rect_adjust_from_point(&adjust_point, &rect_ingame_text);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top + 1;
            rect_adjust_from_point(&adjust_point, &rect_ingame_text);

            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top + 1;
            rect_adjust_from_point(&adjust_point, &rect_ingame_text);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top - 1;
            rect_adjust_from_point(&adjust_point, &rect_ingame_text);
        }
    }

    return &rect_ingame_text;
}



void far load_skybox(char mode)
{
    register int skyHeight;
    struct MATERIALCLRLIST *materials;

    if (mode & 8)
        mode &= 7;
    else if (byte_3B8F6 != 0 && mode == byte_46167)
        return;
    else {
        unload_skybox();
        byte_46167 = mode;
        byte_3B8F6 = 1;
        skybox_res_ofs.pointer = file_load_shape2d_fatal_thunk(aDesert + 9 * mode);
        locate_many_resources(skybox_res_ofs.pointer, "scensce2sce3sce4",
                              (char far **)skyboxes);
        skybox_ptr1 = skyboxes[0]->height;
        skybox_ptr2 = skyboxes[1]->height;
        skybox_ptr3 = skyboxes[2]->height;
        skybox_ptr4 = skyboxes[3]->height;
        skyHeight = skybox_ptr1;
        if (skyHeight > skybox_ptr2) skyHeight = skybox_ptr2;
        if (skyHeight > skybox_ptr3) skyHeight = skybox_ptr3;
        if (skyHeight > skybox_ptr4) skyHeight = skybox_ptr4;
        skybox_current = skyHeight;
        skyHeight = skybox_ptr1;
        if (skyHeight < skybox_ptr2) skyHeight = skybox_ptr2;
        if (skyHeight < skybox_ptr3) skyHeight = skybox_ptr3;
        if (skyHeight < skybox_ptr4) skyHeight = skybox_ptr4;
        word_454CE = skyHeight;
    }

    materials = material_clrlist_ptr;
    skybox_sky_color = materials->sky;
    skybox_ground_color = materials->ground;
    skybox_wat_color = materials->water;
    meter_needle_color = dlg_colour;
}

void far unload_skybox(void)
{
    if (byte_3B8F6 != 0)
        mmgr_free(skybox_res_ofs.pointer);
    byte_3B8F6 = 0;
}

void far load_sdgame2_shapes(void)
{
    register int i;
    sdgame2ptr = file_load_resource(8, "sdgame2");
    locate_many_resources(sdgame2ptr, "ex01ex02ex03leftrigh",
                          (char far **)sdg2shps);
    for (i = 0; i < 3; ++i)
        sdg2widths[i] = sdg2shps[i]->width;
}

void far free_sdgame2(void) { mmgr_free(sdgame2ptr); }

char aCarcoun_0[] = "carcoun";

char far setup_intro(void)
{
    int lastElapsedFrames;
    int *drawSelectedCount;
    struct POINT2D basePointBufferA[100];
    int activePointCountA;
    struct POINT2D gamePointBufferB[100];
    char far *title3dresValue;
    int pointTotalB;
    struct POINT2D *currentPoints;
    int oldDrawOpponent;
    int introCloudTilt;
    char frameReady;
    int opponentXPosition;
    int lastOpponentY;
    int savedOpponentZ;
    struct VECTOR cloudPointsList[100];
    int targetApproachDifference;
    int baseFrameDelta;
    int oldCameraX;
    int cameraYIdle;
    int cameraZPrevious;
    char operationResult;
    int savedRectangleIndex;
    struct RECTANGLE lastDrawRect;
    int carHeadingData;
    struct RECTANGLE oldSavedRect;
    char far *lastShapeResources[3];
    struct RECTANGLE gameRestoredRect;
    int oldPhase;
    int oldCarDistance;
    char far *otherCarResource;
    int goalXPosition;
    int lastGoalY;
    int savedGoalZ;

    operationResult = 0;
    title3dresValue = file_load_3dres("title");
    locate_many_resources(title3dresValue, "logolog2brav", lastShapeResources);
    shape3d_init_shape(lastShapeResources[0], &logoshape);
    shape3d_init_shape(lastShapeResources[1], &logo2shape);
    shape3d_init_shape(lastShapeResources[2], &bravshape);
    if (vidflag5is0 == 0)
        wndsprite = sprite_make_window(0x140, 0xc8, 0x0f);

    targetApproachDifference = 0;
    do {
        cloudPointsList[targetApproachDifference].x = (get_kevinrandom() << 7) - 0x4000;
        cloudPointsList[targetApproachDifference].y = -((get_kevinrandom() << 7) - 0x1388);
        cloudPointsList[targetApproachDifference].z = (get_kevinrandom() << 7) - 0x4000;
        ++targetApproachDifference;
    } while (targetApproachDifference < 100);

    set_projection(0x28, 0x28, 0x140, 0xc8);
    oldCameraX = 0x400;
    cameraZPrevious = 0x400;
    cameraYIdle = 0x12c;
    oldPhase = 0;
    lastElapsedFrames = 0;
    otherCarResource = file_load_resource_file(aCarcoun_0);
    setup_aero_trackdata(otherCarResource, 1);
    unload_resource(otherCarResource);
    init_plantrak();
    timer_get_delta();
    pointTotalB = 0;
    activePointCountA = 0;
    ((int)slowmgmtcopy) = ((int)slow_video_management);
    rcunk[0].left = 0;
    rcunk[0].right = 0x140;
    rcunk[0].top = 0;
    rcunk[0].bottom = 0xc8;
    rcunk[1] = rcunk[0];
    rect_unk3 = rcunk[0];
    savedRectangleIndex = 0;
    frameReady = 1;

    do {
        baseFrameDelta = timer_get_delta();
        word_44DCC += baseFrameDelta;
        while (word_44DCC > word_4499C) {
            word_44DCC -= word_4499C;
            do_opponent_op();
            frameReady = 1;
            if (11 * ((int)framespersec) < ++lastElapsedFrames) {
                oldPhase = 1;
                cameraYIdle += 20;
                cameraZPrevious -= 5;
                targetApproachDifference = oldCameraX - 0x400;
                if ((targetApproachDifference < 0 ? -targetApproachDifference : targetApproachDifference) < 10) {
                    oldCameraX = 0x400;
                } else if (targetApproachDifference > 0) {
                    oldCameraX -= 10;
                } else if (targetApproachDifference < 0) {
                    oldCameraX += 10;
                }
                if (goalXPosition > 0x400)
                    --goalXPosition;
                else if (goalXPosition < 0x400)
                    ++goalXPosition;
                if (savedGoalZ > 0x400)
                    --savedGoalZ;
                else if (savedGoalZ < 0x400)
                    ++savedGoalZ;
            }
        }
        if (!frameReady)
            goto check_input;
        frameReady = 0;
        if (vidflag5is0 != 0)
            setup_mcgawnd2();
        else
            sprite_copy_wnd_to_1();

        carHeadingData = 0xffff;
        oldDrawOpponent = 1;
        opponentXPosition = (int)(state.opponentstate.car_posWorld1.lx >> 6);
        lastOpponentY = (int)(state.opponentstate.car_posWorld1.ly >> 6);
        savedOpponentZ = (int)(state.opponentstate.car_posWorld1.lz >> 6);
        if (6 * ((int)framespersec) > lastElapsedFrames) {
            oldDrawOpponent = 0;
            carHeadingData = state.opponentstate.car_rotate.x & 0x03ff;
            introCloudTilt = 0;
            oldCameraX = opponentXPosition;
            cameraYIdle = lastOpponentY + 20;
            cameraZPrevious = savedOpponentZ;
        } else if (11 * ((int)framespersec) > lastElapsedFrames) {
            oldCameraX = 0x400;
            cameraZPrevious = 0x400;
            cameraYIdle = 0x5a;
            goalXPosition = opponentXPosition;
            lastGoalY = lastOpponentY;
            savedGoalZ = savedOpponentZ;
        }
        if (carHeadingData == 0xffff) {
            carHeadingData = (-polang(goalXPosition - oldCameraX, savedGoalZ - cameraZPrevious)) & 0x03ff;
            oldCarDistance = polradius2d(goalXPosition - oldCameraX, savedGoalZ - cameraZPrevious);
            introCloudTilt = polang(lastGoalY - cameraYIdle, oldCarDistance) & 0x03ff;
        }

        if (((int)slowmgmtcopy) != 0) {
            if (savedRectangleIndex == 0) {
                currentPoints = (int *)gamePointBufferB;
                drawSelectedCount = &pointTotalB;
            } else {
                currentPoints = (int *)basePointBufferA;
                drawSelectedCount = &activePointCountA;
            }
        }
        intro_op(oldCameraX, cameraYIdle, cameraZPrevious, carHeadingData, introCloudTilt,
                 oldDrawOpponent, oldPhase, cloudPointsList,
                 (struct POINT2D *)currentPoints, drawSelectedCount,
                 rcunk[savedRectangleIndex], &gameRestoredRect, &lastDrawRect);

        if (vidflag5is0 != 0) {
            msdrawopaquechk();
            setup_mcgawnd1();
            msdrawtransparentchk();
            if (((int)slowmgmtcopy) != 0)
                rcunk[savedRectangleIndex] = gameRestoredRect;
            savedRectangleIndex ^= 1;
        } else {
            sprcopy2to12();
            if (((int)slowmgmtcopy) != 0) {
                rcunion(&lastDrawRect, &rcunk[2], &oldSavedRect);
                if (rcintersect(&oldSavedRect, &rect_unk3) == 0) {
                    sprset1size(oldSavedRect.left, oldSavedRect.right,
                                      oldSavedRect.top, oldSavedRect.bottom);
                    msdrawopaquechk();
                    sprputimage(wndsprite->image);
                    msdrawtransparentchk();
                    rcunk[0] = gameRestoredRect;
                    rcunk[2] = lastDrawRect;
                }
            } else {
                msdrawopaquechk();
                sprputimage(wndsprite->image);
                msdrawtransparentchk();
            }
        }

check_input:
        if (input_do_checking(baseFrameDelta)) {
            operationResult = 1;
            break;
        }
    } while (23 * ((int)framespersec) > lastElapsedFrames);

    if (vidflag5is0 != 0) {
        if (get_0() != 0) {
            setup_mcgawnd2();
            sub_35C4E(0, 0, 0x140, 0xc8, 0);
            msdrawopaquechk();
            setup_mcgawnd1();
            msdrawtransparentchk();
        }
    } else {
        sprite_free_window(wndsprite);
    }
    mmgr_free(title3dresValue);
    return (char)operationResult;
}

void far intro_op(int camX, int camY, int camZ, int logoRotation,
                  int cloudRotation, int showOpponent, int useLogo,
                  struct VECTOR *cloudPoints, struct POINT2D *oldPoints,
                  int *oldPointCount, struct RECTANGLE inputClip,
                  struct RECTANGLE *oldClip, struct RECTANGLE *oldUnion)
{
    struct TRANSFORMEDSHAPE modelShape;
    struct RECTANGLE initialClip;
    struct RECTANGLE previousClip;
    struct RECTANGLE unionRect;
    register int index;
    register int count;
    struct POINT2D projectedPoint;
    struct VECTOR inputPoint;
    struct VECTOR relativeVector;

    initialClip = clipunk;
    select_rot(0, cloudRotation, logoRotation, &intro_cliprect, 0);
    if (useLogo)
        modelShape.shape = &logoshape;
    else
        modelShape.shape = &logo2shape;
    modelShape.pos.x = 0x400 - camX;
    modelShape.pos.y = -camY;
    modelShape.pos.z = 0x400 - camZ;
    if (((int)slowmgmtcopy) != 0) {
        modelShape.rect = &initialClip;
        modelShape.flags = 0x0c;
    } else {
        modelShape.flags = 4;
    }
    modelShape.rotation.x = 0;
    modelShape.rotation.y = 0;
    modelShape.rotation.z = 0;
    modelShape.scale = 0x400;
    modelShape.material = 0;
    trans_op(&modelShape);
    if (!showOpponent)
        goto logo_complete;
    modelShape.pos.x = (int)(state.opponentstate.car_posWorld1.lx >> 6) - camX;
    modelShape.pos.y = (int)(state.opponentstate.car_posWorld1.ly >> 6) - camY;
    modelShape.pos.z = (int)(state.opponentstate.car_posWorld1.lz >> 6) - camZ;
    modelShape.shape = &bravshape;
    if (((int)slowmgmtcopy) != 0) {
        modelShape.rect = &initialClip;
        modelShape.flags = 0x0c;
    } else {
        modelShape.flags = 4;
    }
    modelShape.rotation.x = 0;
    modelShape.rotation.y = 0;
    modelShape.rotation.z = -state.opponentstate.car_rotate.x;
    modelShape.scale = 0x400;
    modelShape.material = 0;
    trans_op(&modelShape);

logo_complete:
    if (((int)slowmgmtcopy) == 0)
        goto no_incremental;
    if (*oldPointCount != 0) {
        index = 0;
        while (index < *oldPointCount) {
            projectedPoint = oldPoints[index];
            putpixel_single_maybe(projectedPoint.px, projectedPoint.py, 0);
            ++index;
        }
    }
    rcunion(oldClip, &inputClip, &unionRect);
    if (rcintersect(&unionRect, &rect_unk3) == 0) {
        sprset1size(unionRect.left, unionRect.right,
                          unionRect.top, unionRect.bottom);
        sprite_clear_1_color(0);
    }
    previousClip = initialClip;
    goto prepare_draw;

no_incremental:
    sprset1size(intro_cliprect.left, intro_cliprect.right,
                      intro_cliprect.top, intro_cliprect.bottom);
    sprite_clear_1_color(0);
prepare_draw:
    sprset1size(intro_cliprect.left, intro_cliprect.right,
                      intro_cliprect.top, intro_cliprect.bottom);
    count = 0;
    index = 0;
    do {
        inputPoint.x = cloudPoints[index].x - camX;
        inputPoint.y = cloudPoints[index].y - camY;
        inputPoint.z = cloudPoints[index].z - camZ;
        mat_vec(&inputPoint, &mattmp, &relativeVector);
        if (relativeVector.z > 0xc8) {
            vector_to_point(&relativeVector, &projectedPoint);
            putpixel_single_maybe(projectedPoint.px, projectedPoint.py,
                                  intro_colorvalue);
            if (((int)slowmgmtcopy) != 0) {
                oldPoints[count++] = projectedPoint;
                rect_adjust_from_point(&projectedPoint, &previousClip);
            }
            ++intro_colorvalue;
            if (intro_colorvalue == word_407CC)
                intro_colorvalue = 1;
        }
        ++index;
    } while (index < 100);
    if (((int)slowmgmtcopy) != 0)
        *oldPointCount = count;
    polyinfo();
    if (((int)slowmgmtcopy) != 0) {
        *oldClip = initialClip;
        *oldUnion = previousClip;
    }
}
