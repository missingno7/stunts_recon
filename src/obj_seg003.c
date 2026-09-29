#define PLATFORM_SCREEN_WIDTH_PIXELS 320
#define PLATFORM_SCREEN_HEIGHT_PIXELS 200
#define WORLD_COORDINATE_SHIFT 6
#define FAST_TRIG_ANGLE_MASK 0x3FF
#define SKYBOX_HORIZONTAL_HALF_WRAP 0x200
#define SKYBOX_HORIZONTAL_WRAP_SIZE 0x400
#define SKYBOX_PANEL3_X 0x200
#define SKYBOX_PANEL4_X 0x340
#define FAR far
#define NEAR near
#define HUGE huge
#include "stunts_types.h"












/* obj_seg003 whole-object candidate (s003d).  RECONSTRUCTION NOTE: original identifiers are not
 * recoverable.  Local/parameter names and the scoping of extern declarations (K&R-style
 * function-scope externs in draw_clip/init_rect_arrays) are pressure-constrained reconstruction
 * choices: MSC 5.10 C2's CSE capacity in update_frame depends on symbol-table memory (names of
 * referenced globals and of update_frame's locals).  They are not recovered names.
 * Data shapes: rect_unk[15] (15-iteration copy loop + rect_unk[si], labels at 8-byte stride),
 * hill_offs[9][2] (pair table, see comment); fence pair lists by their pinned labels; plane state kept separate. */ /* PORT: plain char signedness follows the pinned MSC target. */


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	//int x1, y1;
	//int x2, y2;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct VECTOR {
	I16S x, y, z;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct VECTORLONG {
	I32 lx, ly, lz;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct POINT2D {
	I16 px, py;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct MATRIX {
	I16 vals[9];
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct PLANE {
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
I8 rcintersect(struct RECTANGLE* r1, struct RECTANGLE* r2);

void plnrotop(void);
I16 plnoriginop(I16 index, I16 b, I16 c, I16 d);




/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct GAMEINFO {
	I8 game_playercarid[4];
	I8 game_playermaterial;
	I8 game_playertransmission;
	I8 game_opponenttype;
	I8 game_opponentcarid[4];
	I8 game_opponentmaterial;
	I8 game_opponenttransmission;
	I8 game_trackname[9];
	U16S  game_framespersec;
	U16S  game_recordedframes;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct CARSTATE {
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

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct GAMESTATE {
	I32 game_longs1[24]; // x
	I32 game_longs2[24]; // y
	I32 game_longs3[24]; // z
	struct VECTOR game_vec1[2]; // 0 = player, 1 = opponent
	struct VECTOR game_vec3;
	struct VECTOR game_vec4;
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
	U8  field_42B[24];
	U8  field_443[24];
	I8 field_45B;
	I8 field_45C;
	I8 field_45D;
	I8 field_45E;
	I8 field_45F;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct SIMD {
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
	I8 steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	I8 spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	I8 revpoints[256];
	I16S far* aerorestable;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct TRKOBJINFO {
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

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct TRACKOBJECT {
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
extern struct SIMD simdp7;
extern struct SIMD ophys_7;

extern I16S pixel_scales;
extern I16S g_vid_flg2_set;
extern I16S vidflg3is_minus1;
extern I16S vidflg4_is1;
extern I8 g_videoflg5;
extern I16S g_vid_flag6;

extern U8  timeraud;
extern U8  slomodiv8;
extern U16S  elaptm1;
extern U16S  tmr2;
extern U8  sigframe;
extern U8  g_rpl_init;
extern U8  gm_playmode; // 0 = playing, 1 = paused, 2 = replay
extern I16S g_sgateopn;

extern I16S elapsed_time1; // current frame?
extern I16S g_cvxintvl; // fps * 30
extern short frmcs_time; // 100 / fps
extern I16S st_hdg;
extern void* table_lookup;
extern void* steerWhlRespTable_10fps;
extern void* steerWhlRespTable_20fps;
extern I8 idxtrk, tagtrk;
extern I8 g_hillf;
extern I16S hillconsts[];

extern struct RECTANGLE boundglassrect;
extern I16S bitmapdash;
extern int runrndx;
extern I8 replaybar_toggle;
extern I8 inrepflg;
extern I8 cammd;
extern I8 g_rplmodui;
extern I8 gm_saved_rpl_mode;
extern I8 numid;
extern I8 g_rplbfask;
extern I8 on_off_dash;
extern I8 cam_idg;
extern I8 pen_flag_count;
extern I16 replayrst;
extern I16 popupact;
extern I8 mouse_enabled;
extern I8 joystick_enabled;
extern void far* gamerptrs;
extern void far* dasm_shp_7;
extern I16 input_pushed;
extern I8 dashbtogglesaved;
extern I8 g_replaybarcpytgl;
extern I8 is_in_rplcopy;
extern char follow_op;
extern I8 opp_follow_flag_backup;
extern I16 roofbmphgt_saved;
extern I8 mode_flag;
extern I8 g_rplybarenable;
extern I16 dashbmpy_copy;
extern I16 rplbarabovehgt;
extern I8 g_viewinx[];
extern I16 dastseg;
extern I16 dasty;
extern I16 g_dastbmpbuf;
extern I16 dashbmy9;
extern I16 rfy5;
extern struct RECTANGLE* rectp;

extern void player_op(I8);
extern void opponent_op(void);
extern void audio_carstate(void);
extern void setup_car_shapes(I16);
extern void update_frame(I8 a, struct RECTANGLE* rc);
extern void loop_game(I16, I16, I16);
extern void set_frame_callback(void);
extern void mouse_minmax_position(I16);
extern I16 kb_get_char(void);
extern void handle_ingame_kb_shortcuts(I16);

extern I16 flagsdown;
extern I16 msecoordx;
extern I16 pos_y_ms;
extern I16 performGraphColor;
extern I8 resbuftext;
extern I16 waitm_ms;

extern void far* fntndat;
extern void far* def_fntadr;
extern void far* main_data_file_addr;
extern struct GAMESTATE far* cvxs_a;
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
extern int far* td10checkptr;// trackdata10;
extern I8 far* hscore_trk11_ptr; //trackdata11;
extern I8 far* savedptr_ms;
extern I8 far* td13_replay_hdr; //trackdata13;
extern U8  far* td14tb; //trackdata14;
extern U8  far* td15p_9; //trackdata15;
extern I8 far* g_tdreplay16buf; //trackdata16;
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
extern I8 endhsdemo;
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

extern U16S  rate_frame;
extern U16S  frm_rate2;
extern U16S  slow_video_mode_state;
extern unsigned short statemgmtcpy;
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

extern struct MATERIALCLRLIST *material_clrlist_ptr;
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
extern void run_game(void);
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

extern void video_set_mode4(void);
extern void video_set_mode7(void);
extern void video_set_mode_13h(void);

extern void shape3d_load_car_shapes(I8* carid, I8* oppcarid);

extern void load_palandcursor(void);
extern void sprite_blit_to_video(struct SPRITE far* sprite);

extern I16S intr0_handler(void);
extern I16S (far* old_intr0_handler)(void);
extern void timer_setup_interrupt(void);
extern U32  timer_get_delta_alt(void);

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





/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct SHAPE3D {
	U16S  shape3d_numverts;
	struct VECTOR far* shape3d_verts;
	U16S  shape3d_numprimitives;
	U16S  shape3d_numpaints;
	I8 far* shape3d_primitives;
	I8 far* shape3d_cull1;
	I8 far* shape3d_cull2;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct SHAPE3DHEADER {
	U8  header_numverts;
	U8  header_numprimitives;
	U8  header_numpaints;
	U8  header_reserved;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct TRANSFORMEDSHAPE3D {
	struct VECTOR pos;
	struct SHAPE3D* shapeptr;
	struct RECTANGLE* rectptr;
	struct VECTOR rotvec;
	U16S  unk;
	U8  ts_flags;
	U8  material;
};


I16 shape3d_load_all(void);
void shape3d_free_all(void);
void shape3d_init_shape(I8 far* shapeptr, struct SHAPE3D* gameshape);
I8 trans_op(struct TRANSFORMEDSHAPE3D* arg_transshapeptr);
void set_projection(I16 i1, I16 i2, I16 i3, I16 i4);
I16 polang(I16 z, I16 y);
unsigned select_rot(I16 angZ, I16 angX, I16 angY, struct RECTANGLE* cliprect, I16 unk);
void initialize_polyinfo(void);
void polyinfo_reset(void);
void polyinfo(void);
void wheel_update(struct VECTOR far* wheel_vertices, I16 steering_angle, I16S* base_heights, I16S* cached_angles_and_heights, struct VECTOR* wheel_offsets, struct VECTOR* car_origin);


extern struct RECTANGLE* rcpunk2;
extern struct RECTANGLE rclist[];
extern struct RECTANGLE g_savrc[];
extern struct RECTANGLE tparr[];
extern struct RECTANGLE clipunk;
extern struct VECTOR tvec2;
extern struct VECTOR pln_rot_output;
extern I16 g_planidx2;
extern I16 pln_rotate_z, car_roty_pln, car_rotate_xc, frwhl_angadjusted;
extern struct MATRIX wkmatx;
extern I16 custom_dist;
extern I16 custom_elev_angle;
extern I16 custom_azim_angle;
extern I16 viewyshift;
extern I8 detthrlevel[];
extern I8 paint_cycle[];
extern unsigned g_clocks;
extern I16 fence_offsets[];
extern I8* ahead_tables[];
extern struct SHAPE3D* fence_shapes[];
extern I16 hgthgt;
extern I16 pl_i;
extern I8 test_pln;
extern struct TRANSFORMEDSHAPE3D cur_shps[29];
//extern struct TRANSFORMEDSHAPE3D transshapeunk;
extern struct TRANSFORMEDSHAPE3D* g_curr_tsp;
extern struct TRACKOBJECT trklst[215]; // 215 entries
extern U8  fence_codes[];

extern I8 fence_off_1[];         /* Fence offset pairs are grouped by the 1-, 2-, 3-, and 4-entry extents. */
extern I8 fence_off_2[];
extern I8 fence_off_4[];
extern I8 fence_off_3[];
extern I16 shape_rot[];
extern I16 hill_offs[9][2];      /* hill (x,z) offsets: 1,2,2,4 pairs at pairs 0,1,3,5; exactly fills [0x3C0A2,0x3C0C6) */
extern I16 hill_offs_b[];
extern I16 hill_offs_c[];
extern I16 hill_offs_d[];
extern struct TRACKOBJECT scene2[];
extern struct TRACKOBJECT scene3[];
extern struct SHAPE3D g_shapes3d[130];
extern struct VECTOR pos_pt;
extern struct VECTOR pts_set[6];
extern I16S ywhlang[];
extern struct VECTOR ctrmesh;
extern struct VECTOR veco[6];
extern I16S buf_obase[];
extern char backlightovr8;
extern I16 g_tdist[];
extern I16 tsix[];
extern I8 tshapearrarg2[];
extern short exwd[3];
extern struct SHAPE2D far *sdgbmp_v[5];
extern void far* fntled_res;
extern I16 dlg_colour;
extern char g_ts_num;

void build_obj(struct VECTOR* a, struct VECTOR* b);
void transformed_shape_add_for_sort(I16 zadjust, I8 sort_group_id);
U8  subst_hillroad(U8  a, U8  b);
I16 skybox_op(I16 a, struct RECTANGLE* rectptr, I16 e, struct MATRIX* matptr, I16 c, I16 f, I16 g);
struct RECTANGLE* draw_ingame_text(void);
struct RECTANGLE* init_crak(I16 frame, I16 top, I16 height);
struct RECTANGLE* do_sinking(I16 frame, I16 top, I16 height);
struct RECTANGLE* introtext(I8* str, I16 a, I16 b, I16 c, I16 d);
void fontsetfontdef2(void far* data);
void fmtframestr(I8* s, I16 time, I16 c);
void shapeexpl(I16 a, void far* shp, I16 x, I16 y);
void heapsortorder(I16 n, I16* heap, I16* data);


extern struct RECTANGLE game_rect_txt_in, rect_ingame_text2, rect_ingame_text3, rect_ingame_text4;
extern I8 aDm1[], aDm2[], aPre[], aSe1[], aSe2[], aWww[], aOpp[], aPen[], aRpl_0[], aOpp_0[];
extern I8 far * far locate_text_resource(void far *data, I8 *name);
extern void copy_string(I8 *destination, I8 far *source);
extern I16 far font_op2_alt(I8 *name);
extern void sprite_putimage_transparent(void far *shape, I16 x, I16 y);
extern unsigned strlen(I8 *s);
extern I16 g_penaltytm;
extern int g_skyboxwat_clr;
extern void preRender_line(I16 x1, I16 y1, I16 x2, I16 y2, I16 color);
extern I8 far *locate_shape_alt(I8 far *data, I8 *name);
extern I16 sky_hgt_world, maxscnh;
extern U16S  scene_1ht, scene_2ht, scene_3ht, scene_4ht;
extern I16 g_skybox_sky_clr, ground_skybox;
extern struct SHAPE2D far *skypics[4];
extern void sprite_putimage_and_alt(void far *shape, I16 x, I16 y);
extern struct RECTANGLE trackpreview_cliprect;
extern I16S horizon_angles[];
extern I16 camera_aim_z, camera_pos_z, camera_aim_x, camera_pos_x, camera_aim_y, camera_pos_y;
extern struct VECTOR track_prev_vec;
extern unsigned draw_line_related(unsigned, unsigned, unsigned, unsigned, I16 *);
void far skybox_op_helper(unsigned color, unsigned count, struct POINT2D p1, struct POINT2D p2, struct POINT2D p3, struct POINT2D p4);
void far skybox_op_helper2(struct RECTANGLE *rectptr, I16 x, I16 horizon);
void draw_track_preview(void);


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct SHAPE2D { I16S width, height, unk1, unk2, pos_x, pos_y; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct MATERIALCLRLIST { U8  pad20[0x20]; I16S ground, sky; U8  pad24[0xa4]; I16S water; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ union FARRESOURCE { void far *pointer; struct { U16S  offset, segment; } word; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct TRANSFORMEDSHAPE { struct VECTOR pos; struct SHAPE3D *shape; struct RECTANGLE *rect; struct VECTOR rotation; I16 scale; U8  flags, material; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct SPRITE { void far *image; U16S  words[13]; };

extern struct SHAPE3D intro_alt, logo_title, brav;
extern I8 far *loadedresourceptr;
extern I16 word_34A06, word_34A08, word_34A0A;
extern I16 intro_colorvalue, intro_color_max;
extern void far *file_load_3dres(I8 *);
extern void far locate_many_resources(void far *, I8 *, I8 far **);
extern void far *sprite_make_window(I16, I16, I16);
extern I16 far get_kevinrandom(void);
extern I8 far *file_load_resource_file(I8 *);
extern void far setup_aero_trackdata(I8 far *, I16);
extern void far unload_resource(I8 far *);
extern void far init_plantrak(void);
extern I16 far timer_get_delta(void);
extern void far do_opponent_op(void);
extern void far setup_mcgawnd2(void);
extern void far setup_mcgawnd1(void);
extern void far sprite_copy_wnd_to_1(void);
extern I16 far get_0(void);
extern void far clear_rect(I16, I16, I16, I16, I16);
extern void far sprite_free_window(void far *);
extern void far mmgr_free(void far *);

extern I16 word_449FE, frmexcess;
extern int spdneedlegaugeclr;
extern U8  skybox_loaded, scene_idx;
extern struct RECTANGLE rc0_cpy;
extern struct RECTANGLE intro_cliprect;
extern union FARRESOURCE skyres_handle;
extern I8 scenery_names[];
extern void far *file_load_shape2d_fatal_thunk(I8 *);
extern char far *sdgame2hdl;
extern void far *file_load_resource(I16, I8 *);
extern void far load_sdgame2_shapes(void);
extern void far free_sdgame2(void);
extern void far init_rect_arrays(void);
extern void far draw_clip(struct RECTANGLE *);
extern void far load_skybox(I8);
extern void far unload_skybox(void);
extern I8 far setup_intro(void);
extern void far intro_op(I16, I16, I16, I16, I16, I16, I16, struct VECTOR *, struct POINT2D *, I16 *, struct RECTANGLE, struct RECTANGLE *, struct RECTANGLE *);


/* Copies a clipped window region. Params: clip rectangle. Returns: none. State: reads the active window sprite and clip state. */
/* PLATFORM(video): draws to or configures the display surface. */
void far draw_clip(struct RECTANGLE *clipRect)
{
	extern I8 g_simprect;
	extern void far msdrawopaquechk(void);
	extern void far msdrawtransparentchk(void);
	extern void far rectsorttop(I8, struct RECTANGLE *, I16 *);
	extern struct RECTANGLE rectclip[15];
	extern int rcmapix[45];
	extern char rect_num3;
	extern char rects_updt[15];
	extern void far rectlist_add(I8, I8 *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, I8 *, struct RECTANGLE *);
	extern void far sprcopy2to12(void);
	extern void far sprputimage(void far *);
	extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
	extern struct SPRITE far *g_wndspr;
	extern I16 rotpr[];
	extern int prevcamrot;
    register I16 i;
    struct RECTANGLE *currentRect;
    if (g_videoflg5 != 0)
        return;
    /* PLATFORM(video): copy the working sprite pages. */ sprcopy2to12();
    if (g_simprect == 0) {
        if (statemgmtcpy != 0) {
            for (i=0; i<15; ++i)
                rects_updt[i] = 3;
            if (detail_lvl == 4)
                rotpr[1] = prevcamrot;
            if (rotpr[1] == prevcamrot &&
                rclist[5].left == g_savrc[5].left &&
                rclist[5].right == g_savrc[5].right &&
                rclist[5].top == g_savrc[5].top &&
                rclist[5].bottom == g_savrc[5].bottom)
                rects_updt[5] = 0;
            rect_num3 = 0;
            rectlist_add(15, rects_updt, rclist,
                g_savrc, clipRect, &rect_num3,
                rectclip);
            if (rect_num3 != 0) {
                rectsorttop(rect_num3,
                    rectclip, rcmapix);
                /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
                i = 0;
                goto draw_rect_check;
                do {
                    currentRect = &rectclip[rcmapix[i]];
                    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(currentRect->left, currentRect->right,
                        currentRect->top, currentRect->bottom);
                    /* PLATFORM(video): blit the saved sprite image. */ sprputimage(g_wndspr->image);
                    ++i;
draw_rect_check:
                    ;
                } while (rect_num3 > i);
                goto draw_transparent;
            }
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, 320, clipRect->top, clipRect->bottom);
        } else {
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(clipRect->left, clipRect->right,
                clipRect->top, clipRect->bottom);
        }
    }
    /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
    /* PLATFORM(video): blit the saved sprite image. */ sprputimage(g_wndspr->image);
/* PLATFORM(video): select the transparent drawing path. */ draw_transparent:
    msdrawtransparentchk();
    if (statemgmtcpy != 0) {
        rotpr[1] = prevcamrot;
        for (i=0; i<15; ++i)
            g_savrc[i] = rclist[i];
    }
}

/* Resets saved clip rectangles for the current drawing mode. Params: none. Returns: none. State: reads clip state and updates rectangle buffers. */ void far init_rect_arrays(void)
{
	extern struct RECTANGLE rcunk5;
    register I16 i;

    if (statemgmtcpy != 0) {
        rclist[0] = rcunk5;
        g_savrc[0] = rcunk5;
        for (i = 1; i < 15; ++i) {
            rclist[i] = clipunk;
            g_savrc[i] = clipunk;
        }
    }
}
/* Updates the visible game frame. Params: page and clip rectangle. Returns: none. State: reads frame, scene, camera and sprite globals; updates render lists. */
/* PLATFORM(video): draws to or configures the display surface. */
void update_frame(I8 page, struct RECTANGLE* clip) {
	extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
	extern I16 rotpr[];
	extern I16 prevcamrot;
	I8 skipped[24];
	U8  terrain_list[24];
	I16 cur_sky_sign;
	struct TRACKOBJECT *ovl;
	I16 ground_dist;
	I16 nfences;
	struct RECTANGLE *rect_ptr;
	I8 car_east1;
	I16 objx;
	I8 checkpoint_no;
	I8 cur_opp_east;
	I16 hill_elev;
	I16 car_angle_y;
	struct VECTOR elem_pos;
	struct VECTOR viewpoint;
	I16 cur_view_heading_val;
	I8 *tile_table;
	I8 ovl_pending;
	I16 obj_zpos;
	I16 view_rot_x;
	I16 my_car_bank;
	I8 opp_wheel_south_val;
	I16 car_zofs;
	struct RECTANGLE cliprect1;
	U8  track_elem;
	I8 wheel_south;
	struct MATRIX sky_mat;
	struct RECTANGLE cliprect2;
	I8 east_list[24];
	I8 south_tab[24];
	I8 lod_levels[24];
	I16 opp_zbias_val;
	I16 wheel_idx;
	I16 j;
	I8 crashed[2];
	I16 *offsets;
	I8 car_x_tile;
	struct MATRIX *car_matrix;
	struct TRACKOBJECT *trkobj;
	I8 drow;
	I16 start_bias;
	struct VECTOR rot_ofs;
	I8 flagbits;
	struct MATRIX view_mat;
	I8 *fence_list;
	U8  tile_ground;
	I8 cam_tx;
	struct VECTOR far *vptr;
	struct CARSTATE *followed;
	I8 trow;
	I8 tile_east2;
	I16 bank;
	I8 tile_lod;
	I16 cam_elev;
	register I16 si;
	I8 tx;
	I16 rot_x;
	struct VECTOR offset_v;
	struct VECTOR car_coord;
	register I16 j2;
	I8 paint;
	I8 car_z_tile;
	I16 cam_bank;
	U8  elem_list[24];
	I8 cam_tz;
	I16 skybox_result;
	I8 thresh;
	I16 result;

	crashed[0] = 0;
	crashed[1] = 0;
	if (g_videoflg5 != 0 && page != 0) {
		rcpunk2 = rclist;
		rectp = g_savrc;
	} else {
		rectp = rclist;
		rcpunk2 = g_savrc;
	}

	if (statemgmtcpy != 0) {
		flagbits = 8;
		rect_ptr = tparr;
		for (si = 0; si < 15; si++) {
			*rect_ptr++ = clipunk;
		}
	} else {
		flagbits = 0;
	}

	// Set car position (own or opponent's)
	if (follow_op == 0) {		
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.x = core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT;
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.y = core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT;
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.z = core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT;
		car_angle_y = core.playerstate.car_rotate.y;
		my_car_bank = core.playerstate.car_rotate.z;
		rot_x = core.playerstate.car_rotate.x;
	} else {
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.x = core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT;
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.y = core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT;
		/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ car_coord.z = core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT;
		car_angle_y = core.opponentstate.car_rotate.y;
		my_car_bank = core.opponentstate.car_rotate.z;
		rot_x = core.opponentstate.car_rotate.x;
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
		viewpoint.x = core.game_vec1[follow_op].x;
		viewpoint.z = core.game_vec1[follow_op].z;
		viewpoint.y = core.game_vec1[follow_op].y;
		break;
	case 0:
		view_rot_x = rot_x & FAST_TRIG_ANGLE_MASK;
		cam_elev = car_angle_y & FAST_TRIG_ANGLE_MASK;
		cam_bank = my_car_bank & FAST_TRIG_ANGLE_MASK;
		car_matrix = matrotzxy(-my_car_bank, -car_angle_y, -rot_x, 0);
		offset_v.x = 0;
		offset_v.z = 0;
		offset_v.y = simdp7.car_height - 6;
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		viewpoint.x = car_coord.x + rot_ofs.x;
		viewpoint.y = car_coord.y + rot_ofs.y;
		viewpoint.z = car_coord.z + rot_ofs.z;
		break;
	case 3:
		viewpoint.x = trkptrpath[core.field_3F7[follow_op]].x;
		viewpoint.y = trkptrpath[core.field_3F7[follow_op]].y + viewyshift + 0x5A;
		viewpoint.z = trkptrpath[core.field_3F7[follow_op]].z;
		break;
	}

	// Unknown part; seems to be performing some initialization
	if (view_rot_x == -1) {
		build_obj(&viewpoint, &viewpoint);
		if (viewpoint.y < hgthgt) {
			viewpoint.y = hgthgt;
		}

		if (test_pln != 0) {		
			si = plnoriginop(pl_i, viewpoint.x, viewpoint.y, viewpoint.z);
			if (si < 0xC) {			
				tvec2.x = 0;
				tvec2.y = 0xC - si;
				tvec2.z = 0;
				g_planidx2 = pl_i;
				frwhl_angadjusted = 0;
				car_roty_pln = 0;
				pln_rotate_z = 0;
				car_rotate_xc = 0;
				plnrotop();
				viewpoint.x += pln_rot_output.x;
				viewpoint.y += pln_rot_output.y;
				viewpoint.z += pln_rot_output.z;
			}
		}

		view_rot_x = (-polang(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z)) & FAST_TRIG_ANGLE_MASK;
		ground_dist = polradius2d(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z);
		cam_elev = polang(car_coord.y - viewpoint.y + 0x32, ground_dist) & FAST_TRIG_ANGLE_MASK;
	}

	if (cam_bank > 1 && cam_bank < FAST_TRIG_ANGLE_MASK) {
		bank = cam_bank;
	} else {
		bank = 0;
	}

	paint = paint_cycle[(core.game_frame == 0 ? g_clocks : core.game_frame) & 0xF];

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
	si = (cur_view_heading_val & FAST_TRIG_ANGLE_MASK) >> 7;
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
		cur_shps->rectptr = &tparr[7];
		cur_shps->ts_flags = flagbits | 7;
		cur_shps->rotvec.x = 0;
		cur_shps->rotvec.y = 0;
		cur_shps->unk = 0x400;
		cur_shps->material = 0;

		for (nfences = 0; nfences < 8; nfences++) {
			si = (fence_offsets[nfences] + view_rot_x + runrndx) & FAST_TRIG_ANGLE_MASK;
			if (si < 0x87 || si > 0x379) {
				matroty(&sky_mat, si);
				offset_v.x = 0;
				offset_v.y = 0xAE6 - viewpoint.y;
				offset_v.z = 0x3A98; //15000
				mat_vec(&offset_v, &sky_mat, &rot_ofs);
				rot_ofs.z = 0x3A98; //15000
				mat_vec(&rot_ofs, &view_mat, &cur_shps->pos);
				if (cur_shps->pos.z > PLATFORM_SCREEN_HEIGHT_PIXELS) {
					cur_shps->shapeptr = fence_shapes[nfences];
					cur_shps->rotvec.z = -view_rot_x;
					result = trans_op(&cur_shps[0]);
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
		car_x_tile = core.playerstate.car_posWorld1.lx >> 16;
		car_z_tile = 0x1D - (core.playerstate.car_posWorld1.lz >> 16);
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
					track_elem = td14tb[lnoffsets[trow] + tx];
					tile_ground = td15p_9[gterrtrk[trow] + tx];
	
					if (track_elem != 0) {
						if (tile_ground >= 7 && tile_ground < 0xB) {
							track_elem = subst_hillroad(tile_ground, track_elem);
							tile_ground = 0;
						}
	
						switch (track_elem) {
						case 0xFD:
							tx--;
							trow--;
							track_elem = td14tb[lnoffsets[trow] + tx];
							tile_ground = td15p_9[gterrtrk[trow] + tx];
							break;
						case 0xFE:
							trow--;
							track_elem = td14tb[lnoffsets[trow] + tx];
							tile_ground = td15p_9[gterrtrk[trow] + tx];
							break;
						case 0xFF:
							tx--;
							track_elem = td14tb[lnoffsets[trow] + tx];
							tile_ground = td15p_9[gterrtrk[trow] + tx];
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
	if (cammd != 0 || follow_op != 0) {

		if (core.playerstate.car_crashBmpFlag != 2) {

			car_matrix = matrotzxy(-core.playerstate.car_rotate.z, -core.playerstate.car_rotate.y, -core.playerstate.car_rotate.x, 0);
			j = -1;
			j2 = -1;
			for (wheel_idx = 0; wheel_idx < 4; wheel_idx++) {
				offset_v = simdp7.wheel_coords[wheel_idx];
				mat_vec(&offset_v, car_matrix, &elem_pos); //; rotating car wheels, maybe?
				// Tile where the wheel is standing
				tx = (elem_pos.x + core.playerstate.car_posWorld1.lx) >> 16; // bits 16-24
				trow = -(((elem_pos.z + core.playerstate.car_posWorld1.lz) >> 16) - 0x1D);

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
				if (core.playerstate.car_surfaceWhl[0] != 4 || core.playerstate.car_surfaceWhl[1] != 4 || core.playerstate.car_surfaceWhl[2] != 4 || core.playerstate.car_surfaceWhl[3] != 4) {
					offset_v.x = 0;
					offset_v.z = 0;
					offset_v.y = 0x7530;
					mat_vec(&offset_v, car_matrix, &elem_pos);
					mat_vec(&elem_pos, &wkmatx, &offset_v);
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
	if (globalgamesettings.game_opponenttype != 0) {

		if (cammd != 0 || follow_op == 0) {
			if (core.opponentstate.car_crashBmpFlag != 2) {
				car_matrix = matrotzxy(-core.opponentstate.car_rotate.z, -core.opponentstate.car_rotate.y, -core.opponentstate.car_rotate.x, 0);
				j = -1;
				j2 = -1;

				for (wheel_idx = 0; wheel_idx < 4; wheel_idx++) {
					offset_v = ophys_7.wheel_coords[wheel_idx];
					mat_vec(&offset_v, car_matrix, &elem_pos); //; rotating car wheels, maybe?
					tx = (elem_pos.x + core.opponentstate.car_posWorld1.lx) >> 16; // bits 16-24
					trow = -(((elem_pos.z + core.opponentstate.car_posWorld1.lz) >> 16) - 0x1D);

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
						
					if (core.opponentstate.car_surfaceWhl[0] != 4 || core.opponentstate.car_surfaceWhl[1] != 4 || core.opponentstate.car_surfaceWhl[2] != 4 || core.opponentstate.car_surfaceWhl[3] != 4) {
						offset_v.x = 0;
						offset_v.z = 0;
						offset_v.y = 0x7530;
						mat_vec(&offset_v, car_matrix, &elem_pos);
						mat_vec(&elem_pos, &wkmatx, &offset_v);
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
				fence_list = fence_off_1;
				break;
			case 1:
				nfences = 2;
				fence_list = fence_off_2;
				break;
			case 2:
				nfences = 3;
				fence_list = fence_off_3;
				break;
			case 3:
				nfences = 4;
				fence_list = fence_off_4;
				break;
			}
		} else {
			nfences = 1;
			fence_list = fence_off_3;
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
				cur_shps->shapeptr = (tile_lod == 0) ? ovl->ss_shapePtr : ovl->ss_loShapePtr;
				cur_shps->pos.x = trackctrpos2[tile_east2] - viewpoint.x;
				cur_shps->pos.y = -viewpoint.y;
				cur_shps->pos.z = row_ctr_zs[drow] - viewpoint.z;
				cur_shps->rectptr = &tparr[1];
				cur_shps->ts_flags = flagbits | 5;
				cur_shps->rotvec.x = 0;
				cur_shps->rotvec.y = 0;
				cur_shps->rotvec.z = shape_rot[j2];
				cur_shps->unk = 0x400;
				cur_shps->material = 0;
				result = trans_op(&cur_shps[0]);
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
					tile_ground = td15p_9[gterrtrk[drow] + tile_east2];
					if (tile_ground != 0) {
						trkobj = &scene2[tile_ground];
						cur_shps->shapeptr = trkobj->ss_shapePtr;
						cur_shps->pos.x = trackctrpos2[tile_east2] - viewpoint.x;
						cur_shps->pos.y = -viewpoint.y;
						cur_shps->pos.z = row_ctr_zs[drow] - viewpoint.z;
						cur_shps->rectptr = &tparr[1];
						cur_shps->ts_flags = flagbits | 5;
						cur_shps->rotvec.x = 0;
						cur_shps->rotvec.y = 0;
						cur_shps->rotvec.z = trkobj->ss_rotY;
						cur_shps->unk = 0x400;
						cur_shps->material = 0;
						result = trans_op(&cur_shps[0]);
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
			cur_shps->shapeptr = trkobj->ss_shapePtr;
			cur_shps->pos.x = trackctrpos2[tx] - viewpoint.x;
			cur_shps->pos.y = hill_elev - viewpoint.y;
			cur_shps->pos.z = row_ctr_zs[trow] - viewpoint.z;
			if (hill_elev == 0) {
				cur_shps->rectptr = &tparr[1];
			} else {
				cur_shps->rectptr = &tparr[2];
			}

			cur_shps->ts_flags = flagbits | 5;
			cur_shps->rotvec.x = 0;
			cur_shps->rotvec.y = 0;
			cur_shps->rotvec.z = trkobj->ss_rotY;
			cur_shps->unk = 0x400;
			cur_shps->material = 0;
			result = trans_op(&cur_shps[0]);
			if (result > 0)
				goto draw_done;
		}

		g_ts_num = 0;
		g_curr_tsp = cur_shps;
		if (track_elem != 0) {
			trkobj = &trklst[track_elem];
			if ((trkobj->ss_multiTileFlag & 1) != 0) {
				obj_zpos = r_zp[trow];
				drow = trow + 1;
			} else {
				obj_zpos = row_ctr_zs[trow];
				drow = trow;
			}

			if ((trkobj->ss_multiTileFlag & 2) != 0) {
				objx = xcols[1 + tx];
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
					offsets = hill_offs[0];
					break;
				case 1:
					j2 = 2;
					offsets = hill_offs[1];
					break;
				case 2:
					j2 = 2;
					offsets = hill_offs[3];
					break;
				case 3:
					j2 = 4;
					offsets = hill_offs[5];
					break;
				}

				for (j = 0; j < j2; j++) {
					cur_shps->pos.x = *offsets++ + elem_pos.x;
					cur_shps->pos.y = elem_pos.y;
					cur_shps->pos.z = *offsets++ + elem_pos.z;
					cur_shps->shapeptr = &g_shapes3d[0x3B2 / sizeof(struct SHAPE3D)];
					cur_shps->rectptr = &tparr[2];
					cur_shps->ts_flags = flagbits | 5;
					cur_shps->rotvec.x = 0;
					cur_shps->rotvec.y = 0;
					cur_shps->rotvec.z = 0;
					cur_shps->unk = 0x800;
					cur_shps->material = 0;
					result = trans_op(&cur_shps[0]);
					if (result > 0)
						goto draw_done;
				}
			}

			if (trkobj->ss_ssOvelay != 0) {
				ovl = &trklst[trkobj->ss_ssOvelay];
				if (tile_lod != 0) {
					cur_shps[1].shapeptr = ovl->ss_loShapePtr;
				} else {
					cur_shps[1].shapeptr = ovl->ss_shapePtr;
				}

				if (cur_shps[1].shapeptr != 0) {
					cur_shps[1].pos = elem_pos;
					cur_shps[1].rotvec.x = 0;
					cur_shps[1].rotvec.y = 0;
					cur_shps[1].rotvec.z = ovl->ss_rotY;
					if (ovl->ss_multiTileFlag != 0) {
						cur_shps[1].unk = 0x400;
					} else {
						cur_shps[1].unk = 0x800;
					}

					if (ovl->ss_surfaceType >= 0) {
						cur_shps[1].material = ovl->ss_surfaceType;
					} else {
						cur_shps[1].material = paint;
					}

					cur_shps[1].ts_flags = ovl->ss_ignoreZBias | flagbits | 4;
					if ((cur_shps[1].ts_flags & 1) != 0) {
						cur_shps[1].rectptr = &tparr[1];
						result = trans_op(&cur_shps[1]);
						if (result > 0)
							goto draw_done;
					} else {
						cur_shps[1].rectptr = &tparr[2];
						ovl_pending = 1;
					}
				}
			}

			if (tile_lod != 0) {
				cur_shps->shapeptr = trkobj->ss_loShapePtr;
			} else {
				cur_shps->shapeptr = trkobj->ss_shapePtr;
			}

			cur_shps->pos = elem_pos; // whatever
			cur_shps->rotvec.x = 0;
			cur_shps->rotvec.y = 0;
			cur_shps->rotvec.z = trkobj->ss_rotY;
			if (trkobj->ss_multiTileFlag != 0) {
				cur_shps->unk = 0x400;
			} else {
				cur_shps->unk = 0x800;
			}

			cur_shps->ts_flags = trkobj->ss_ignoreZBias | flagbits | 4;
			if (trkobj->ss_surfaceType >= 0) {
				cur_shps->material = trkobj->ss_surfaceType;
			} else {
				cur_shps->material = paint;
			}

			if ((trkobj->ss_ignoreZBias & 1) != 0) {
				cur_shps->rectptr = &tparr[1];
				result = trans_op(&cur_shps[0]);
				if (result > 0)
					goto draw_done;
			} else {
				cur_shps->rectptr = &tparr[2];
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

				if (tx == idxtrk && trow == tagtrk) {
					start_bias = 0;
				} else {
					start_bias = -1;
				}
			}

			checkpoint_no = td19hdl[lnoffsets[trow] + tx];
			if (checkpoint_no != -1) {
				if (core.field_3FA[checkpoint_no] != 0) {
					if (core.field_42A != 0) {
						for (j2 = 0; j2 < 0x18; j2++) {
							if (core.field_38E[j2] != 0) if (checkpoint_no + 2 == core.field_443[j2]) {
								trkobj = &scene3[core.field_42B[j2]];
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_longs1[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 0] - viewpoint.x;
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_longs2[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 1] - viewpoint.y;
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_longs3[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 2] - viewpoint.z;
								g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
								g_curr_tsp->rectptr = &tparr[2];
								g_curr_tsp->ts_flags = flagbits | 5;
								g_curr_tsp->rotvec.x = -core.field_2FE[j2];
								g_curr_tsp->rotvec.y = -core.field_32E[j2];
								g_curr_tsp->rotvec.z = -core.field_35E[j2];
								g_curr_tsp->unk = 0x400;
								g_curr_tsp->material = 0;
								transformed_shape_add_for_sort(0, 0);
							}
						}
					}
				} else {
					trkobj = &trklst[212 + trkd23adr[checkpoint_no]];
					g_curr_tsp->pos.x = td10checkptr[checkpoint_no * 3 + 0] - viewpoint.x;
					g_curr_tsp->pos.y = td10checkptr[checkpoint_no * 3 + 1] - viewpoint.y;
					g_curr_tsp->pos.z = td10checkptr[checkpoint_no * 3 + 2] - viewpoint.z;
					g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
					g_curr_tsp->rectptr = &tparr[2];
					g_curr_tsp->ts_flags = flagbits | 4;
					g_curr_tsp->rotvec.x = 0;
					g_curr_tsp->rotvec.y = 0;
					g_curr_tsp->rotvec.z = g_td08d[checkpoint_no];
					g_curr_tsp->unk = 0x64;
					g_curr_tsp->material = 0;
					transformed_shape_add_for_sort(0, 0);
				}
			}
		} else {
			tile_east2 = tx;
			drow = trow;
		}

		if ((car_east1 == tx || car_east1 == tile_east2) && (wheel_south == trow || wheel_south == drow)) {
			if (core.field_42A != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (core.field_38E[j2] != 0) if (core.field_443[j2] == 0) {
						trkobj = &scene3[core.field_42B[j2]];
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_longs1[j2] + core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_longs2[j2] + core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_longs3[j2] + core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;
						g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
						g_curr_tsp->rectptr = &tparr[2];
						g_curr_tsp->ts_flags = flagbits | 5;
						g_curr_tsp->rotvec.x = -core.field_2FE[j2];
						g_curr_tsp->rotvec.y = -core.field_32E[j2];
						g_curr_tsp->rotvec.z = -core.field_35E[j2];
						g_curr_tsp->unk = 0x400;
						g_curr_tsp->material = globalgamesettings.game_playermaterial;
						transformed_shape_add_for_sort(car_zofs & start_bias, 0);
					}
				}
			}

			trkobj = &trklst[2];//0x1C / sizeof(struct TRACKOBJECT)];
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;
			
			if (tile_lod != 0 || detail_lvl > 2) {
				g_curr_tsp->shapeptr = trkobj->ss_loShapePtr;
			} else {
				g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
				wheel_update(&g_shapes3d[0x0AD4 / sizeof(struct SHAPE3D)].shape3d_verts[8], core.playerstate.car_steeringAngle, &core.playerstate.car_rc2, ywhlang, pts_set, &pos_pt);
			}

			if (statemgmtcpy != 0) {
				g_curr_tsp->rectptr = &tparr[3];
				g_curr_tsp->ts_flags = 0xC;
			} else if (core.playerstate.car_crashBmpFlag == 1) {
				cliprect1 = clipunk;
				g_curr_tsp->rectptr = &cliprect1;
				g_curr_tsp->ts_flags = 0xC;
			} else {
				g_curr_tsp->ts_flags = 4;
			}

			g_curr_tsp->rotvec.x = -core.playerstate.car_rotate.z;
			g_curr_tsp->rotvec.y = -core.playerstate.car_rotate.y;
			g_curr_tsp->rotvec.z = -core.playerstate.car_rotate.x;
			g_curr_tsp->unk = 0x12C;
			g_curr_tsp->material = globalgamesettings.game_playermaterial;
			transformed_shape_add_for_sort(car_zofs & start_bias, 2);
		}
		
		if ((cur_opp_east == tx || cur_opp_east == tile_east2) && (opp_wheel_south_val == trow || opp_wheel_south_val == drow)) {
			if (core.field_42A != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (core.field_38E[j2] != 0) {
						if (core.field_443[j2] == 1) {
							trkobj = &scene3[core.field_42B[j2]];
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_longs1[j2] + core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_longs2[j2] + core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_longs3[j2] + core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;
							g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
							g_curr_tsp->rectptr = &tparr[2];
							g_curr_tsp->ts_flags = flagbits | 5;
							g_curr_tsp->rotvec.x = -core.field_2FE[j2];
							g_curr_tsp->rotvec.y = -core.field_32E[j2];
							g_curr_tsp->rotvec.z = -core.field_35E[j2];
							g_curr_tsp->unk = 0x400;
							g_curr_tsp->material = globalgamesettings.game_opponentmaterial;
							transformed_shape_add_for_sort(opp_zbias_val & start_bias, 0);
						}
					}
				}
			}
			trkobj = &trklst[3];//0x2A / sizeof(struct TRACKOBJECT)];
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
			/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;

			if (tile_lod != 0 || detail_lvl > 2) {
				g_curr_tsp->shapeptr = trkobj->ss_loShapePtr;
			} else {
				g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
				wheel_update(&g_shapes3d[0x0AEA / sizeof(struct SHAPE3D)].shape3d_verts[8], core.opponentstate.car_steeringAngle, &core.opponentstate.car_rc2, buf_obase, veco, &ctrmesh);
			}

			if (statemgmtcpy != 0) {
				g_curr_tsp->rectptr = &tparr[4];
				g_curr_tsp->ts_flags = 0xC;
			} else if (core.opponentstate.car_crashBmpFlag == 1) {
				cliprect2 = clipunk;
				g_curr_tsp->rectptr = &cliprect2;
				g_curr_tsp->ts_flags = 0xC;
			} else {
				g_curr_tsp->ts_flags = 4;
			}

			g_curr_tsp->rotvec.x = -core.opponentstate.car_rotate.z;
			g_curr_tsp->rotvec.y = -core.opponentstate.car_rotate.y;
			g_curr_tsp->rotvec.z = -core.opponentstate.car_rotate.x;
			g_curr_tsp->unk = 0x12C;
			g_curr_tsp->material = globalgamesettings.game_opponentmaterial;
			transformed_shape_add_for_sort(opp_zbias_val & start_bias, 3);
		}

		if (core.game_inputmode == 0) {
			if ((tx == idxtrk || tile_east2 == idxtrk) && (trow == tagtrk || drow == tagtrk)) {

				j = mulscl(cosfast(g_sgateopn), 0x24);
				nfences = mulscl(sinfast(g_sgateopn), 0x24) + 0x38;

				vptr = &g_shapes3d[0x98A / sizeof(struct SHAPE3D)].shape3d_verts[8];
				vptr[0].x = j - 0x24;
				vptr[1].x = j - 0x24;
				vptr[2].x = 0x24 - j;
				vptr[3].x = 0x24 - j;

				vptr[0].z = nfences;
				vptr[1].z = nfences;
				vptr[2].z = nfences;
				vptr[3].z = nfences;
				 
				g_curr_tsp->pos.x =
					mulscl(sinfast(st_hdg + 0x200), 0x1B6) +
					mulscl(sinfast(st_hdg + 0x100), 0x24) + 
					trackctrpos2[idxtrk] - viewpoint.x;
				g_curr_tsp->pos.y = hillconsts[g_hillf] - viewpoint.y;
				g_curr_tsp->pos.z =
					mulscl(cosfast(st_hdg + 0x200), 0x1B6) +
					mulscl(cosfast(st_hdg + 0x100), 0x24) + 
					row_ctr_zs[tagtrk] - viewpoint.z;

				g_curr_tsp->shapeptr = &g_shapes3d[0x98A / sizeof(struct SHAPE3D)];
				g_curr_tsp->rectptr = &tparr[2];
				g_curr_tsp->ts_flags = flagbits | 4;
				g_curr_tsp->rotvec.x = 0;
				g_curr_tsp->rotvec.y = 0;
				g_curr_tsp->rotvec.z = st_hdg;
				g_curr_tsp->unk = 0x400;
				j = g_sgateopn >> 6;
				if (j > 3) {
					j = 3;
				}

				g_curr_tsp->material = j;
				transformed_shape_add_for_sort(start_bias & -0x800 /*0xF800*/, 0);
			}
		}

		if (g_ts_num != 0) {
			if (g_ts_num > 1) {
				heapsortorder(g_ts_num, g_tdist, tsix);
			}

			// Draw red overlights on the brake lights on own and opponent's car
			for (j = 0; j < g_ts_num; j++) {
				// j2 is used for index into currenttransshape elsewhere
				j2 = tsix[j];
				switch (tshapearrarg2[j2]) {
				case 2:
					if (core.playerstate.car_is_braking != 0) {
						backlightovr8 = 0x2F;
					} else {
						backlightovr8 = 0x2E;
					}
					break;
				case 3:
					if (core.opponentstate.car_is_braking != 0) {
						backlightovr8 = 0x2F;
					} else {
						backlightovr8 = 0x2E;
					}
					break;
				}

				result = trans_op(&cur_shps[j2]);
				if (result > 0)
					goto draw_done;

				if (result == 0) {
					switch (tshapearrarg2[j2]) {
					case 2:
						if (core.playerstate.car_crashBmpFlag == 1)
							crashed[0] = 1;
						break;
					case 3:
						if (core.opponentstate.car_crashBmpFlag == 1)
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
	/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->bottom);
	polyinfo();

	// This supposedly draws the explosion. The fact that it cycles three
	// different patterns, each 4 frames long, seems to corroborate the
	// hypothesis
	for (si = 0; si < 2; si++) {
		if (crashed[si] == 0) {
			continue;
		}
		if (statemgmtcpy != 0) {
			if (si == 0)
				rect_ptr = &tparr[3];
			else
				rect_ptr = &tparr[4];
		} else {
			rect_ptr = (si == 0) ? &cliprect1 : &cliprect2;
		}

		if (rcintersect(rect_ptr, clip) == 0) {
			/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rect_ptr->left, rect_ptr->right, rect_ptr->top, rect_ptr->bottom);
			offset_v.x = (rect_ptr->right + rect_ptr->left) >> 1;
			offset_v.y = (rect_ptr->top + rect_ptr->bottom) >> 1;
			j = rect_ptr->right - rect_ptr->left;
			nfences = rect_ptr->bottom - rect_ptr->top;
			if (nfences > j) {
				j = nfences;
			}

			j2 = (core.game_frame >> 2) % 3 ;
			nfences = ((I32)j << 8) / (I32)exwd[j2];
			shapeexpl(nfences, sdgbmp_v[j2], offset_v.x, offset_v.y);
		}
	}

/*
; --------------------------------------------------------
*/

	// Depict windscreen cracking after a crash
	/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->bottom);
	if (cammd == 0) {

		if (follow_op != 0) {
			followed = &core.opponentstate;
			si = core.game_oEndFrame;
		} else {
			followed = &core.playerstate;
			si = core.game_pEndFrame;
		}

		if (followed->car_crashBmpFlag == 1) {
			if (statemgmtcpy != 0) {
				rcunion(init_crak(core.game_frame - si, clip->top, clip->bottom - clip->top), tparr, tparr);
			} else {
				init_crak(core.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		} else if (followed->car_crashBmpFlag == 2) {
			if (statemgmtcpy != 0) {
				rcunion(do_sinking(core.game_frame - si, clip->top, clip->bottom - clip->top), tparr, tparr);
			} else {
				do_sinking(core.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		}
	}

	// Show elapsed time
	if (gm_playmode == 0) {
		if (core.game_inputmode != 0) {
			fmtframestr(&resbuftext, elaptm1 + tmr2, 0);
			/* PLATFORM(video): select a font definition. */ fontsetfontdef2(fntled_res);
			if (statemgmtcpy != 0) {
				/* PLATFORM(video): draw the introduction text. */ rcunion(introtext(&resbuftext, 0x8C, rfy5 + 2, dlg_colour, 0), &tparr[6], &tparr[6]);
			} else {
				/* PLATFORM(video): draw the introduction text. */ introtext(&resbuftext, 0x8C, rfy5 + 2, dlg_colour, 0);
			}

			/* PLATFORM(video): select the active font definition. */ fontsetfontdef();
		}
	}

	if (statemgmtcpy != 0) {
		rcunion(draw_ingame_text(), tparr, tparr);
		if (skybox_result != 0) {
			tparr[0] = *clip;
			for (si = 1; si < 15; si++) {
				tparr[si] = clipunk;
			}
		}

		for (si = 0; si < 15; si++) {
			rectp[si] = tparr[si];
		}
		rotpr[page] = view_rot_x;
		prevcamrot = view_rot_x;

	} else {
		draw_ingame_text();
	}

}

extern I8 g_simprect;
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far rectsorttop(I8, struct RECTANGLE *, I16 *);
extern struct RECTANGLE rectclip[15];
extern I16 rcmapix[45];
extern I8 rect_num3;
extern I8 rects_updt[15];
extern struct RECTANGLE rcunk5;
extern void far rectlist_add(I8, I8 *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, I8 *, struct RECTANGLE *);
extern void far sprcopy2to12(void);
extern void far sprputimage(void far *);
extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
extern struct SPRITE far *g_wndspr;
extern I16 rotpr[];
extern I16 prevcamrot;

/* Draws the sky and ground bands. Params: clip rectangle, x offset and horizon. Returns: none. State: reads skybox images, heights and colors. */
/* PLATFORM(video): draws to or configures the display surface. */
void far skybox_op_helper2(struct RECTANGLE *rectptr, I16 x, I16 horizon)
{
    {
        register I16 upper_height;
        if (detail_lvl != 4)
            upper_height = horizon - rectptr->top - sky_hgt_world;
        else
            upper_height = horizon - rectptr->top;

        if (rectptr->bottom - rectptr->top < upper_height)
            upper_height = rectptr->bottom - rectptr->top;

        if (upper_height > 0) {
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->top + upper_height);
            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
        }
    }

    {
        register I16 sky_x;
        if (detail_lvl != 4) {
            sky_x = ((x + SKYBOX_HORIZONTAL_HALF_WRAP) & FAST_TRIG_ANGLE_MASK) - SKYBOX_HORIZONTAL_WRAP_SIZE;
            if (rectptr->top < horizon && horizon - maxscnh <= rectptr->bottom) {
                /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[0], sky_x, horizon - scene_1ht);
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[1], sky_x + PLATFORM_SCREEN_WIDTH_PIXELS, horizon - scene_2ht);
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[2], sky_x + SKYBOX_PANEL3_X, horizon - scene_3ht);
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[3], sky_x + SKYBOX_PANEL4_X, horizon - scene_4ht);
                /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[0], sky_x + 0x400, horizon - scene_1ht);
            }
        }
    }

    {
        register I16 lower_height;
        register I16 ground_top;
        if (rectptr->top > horizon)
            ground_top = rectptr->top;
        else
            ground_top = horizon;

        lower_height = rectptr->bottom - ground_top;
        if (lower_height > 0) {
            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, ground_top, ground_top + lower_height);
            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(ground_skybox);
        }
    }
}

/* Builds and draws the skybox view. Params: preview, clip, latitude, rotation, mode, detail and camera height. Returns: draw status. State: reads skybox/terrain state and updates clip rectangles. */
/* PLATFORM(video): draws to or configures the display surface. */
I16 skybox_op(I16 preview_index, struct RECTANGLE *clip, I16 latitude,
              struct MATRIX *rotation, I16 projection_mode, I16 detail, I16 camera_y)
{
    register I16 i;
    register I16 temp;
    I16 xstart;
    I16 start_y;
    I16 slope;
    I16 draw_result;
    I16 found;
    I16 horizon;
    I16 clip_line[14];
    struct VECTOR transform_input;
    struct VECTOR vecs[6];
    struct POINT2D pts[6];
    struct RECTANGLE rc;
    struct RECTANGLE *rectptr;

    rect_num3 = 0;
    draw_result = 0;
    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->bottom);

    if (projection_mode != 0) {
        transform_input.x = 0x4650 * latitude;
        transform_input.y = -camera_y;
        transform_input.z = 0x3a98 * latitude;
        mat_vec(&transform_input, rotation, &vecs[0]);
        transform_input.x = -0x4650 * latitude;
        mat_vec(&transform_input, rotation, &vecs[1]);

        if (vecs[0].z < 0 || vecs[1].z < 0) {
            temp = g_skybox_sky_clr;
/* PLATFORM(video): select the sprite clip rectangle. */ fill:
            sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->bottom);
            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(temp);
            draw_result = 1;
            goto done;
        }

        vector_to_point(&vecs[0], &pts[0]);
        vector_to_point(&vecs[1], &pts[1]);

        if (pts[0].px > PLATFORM_SCREEN_WIDTH_PIXELS && pts[1].px > PLATFORM_SCREEN_WIDTH_PIXELS) {
            if (pts[0].py < pts[1].py) {
                temp = g_skybox_sky_clr;
                goto fill;
            }
            temp = ground_skybox;
            goto fill;
        }
        if (pts[0].px < 0 && pts[1].px < 0) {
            if (pts[0].py <= pts[1].py) {
                temp = ground_skybox;
                goto fill;
            }
            temp = g_skybox_sky_clr;
            goto fill;
        }
        if (pts[0].py > clip->bottom && pts[1].py > clip->bottom) {
            if (pts[0].px <= pts[1].px) {
                temp = ground_skybox;
                goto fill;
            }
            temp = g_skybox_sky_clr;
            goto fill;
        }
        if (pts[0].py < clip->top && pts[1].py < clip->top) {
            if (pts[0].px >= pts[1].px) {
                temp = ground_skybox;
                goto fill;
            }
            temp = g_skybox_sky_clr;
            goto fill;
        }

        found = 0;
        if (detail_lvl != 4 && pts[1].px < 0 && pts[0].px > PLATFORM_SCREEN_WIDTH_PIXELS &&
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
            if (statemgmtcpy != 0) {
                rc.left = 0;
                tparr[5].left = 0;
                rc.right = PLATFORM_SCREEN_WIDTH_PIXELS;
                tparr[5].right = PLATFORM_SCREEN_WIDTH_PIXELS;
                if (g_simprect != 0) {
                    tparr[5].top = clip->top;
                    tparr[5].bottom = clip->bottom;
                } else {
                    tparr[5].top = (start_y < start_y + slope ? start_y : start_y + slope) - maxscnh;
                    if (clip->top > tparr[5].top)
                        tparr[5].top = clip->top;
                    tparr[5].bottom = start_y > start_y + slope ? start_y : start_y + slope;

                    for (i = 0; i < 15; i++)
                        rects_updt[i] = 1;
                    rects_updt[5] = 3;

                    rc.top = 0;
                    rc.bottom = tparr[5].top;
                    if (!rcintersect(&rc, clip)) {
                        rect_num3 = 0;
                        rectlist_add(15, rects_updt, rectp, tparr,
                                           &rc, &rect_num3, rectclip);
                        for (temp = 0; temp < rect_num3; temp++) {
                            rectptr = &rectclip[temp];
                            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
                        }
                    }

                    rc.top = tparr[5].bottom;
                    rc.bottom = PLATFORM_SCREEN_HEIGHT_PIXELS;
                    if (!rcintersect(&rc, clip)) {
                        rect_num3 = 0;
                        rectlist_add(15, rects_updt, rectp, tparr,
                                           &rc, &rect_num3, rectclip);
                        for (temp = 0; temp < rect_num3; temp++) {
                            rectptr = &rectclip[temp];
                            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(ground_skybox);
                        }
                    }
                }
                rc.top = tparr[5].top;
                rc.bottom = tparr[5].bottom;
            } else {
                rc.top = clip->top;
                rc.bottom = clip->bottom;
            }
            rc.left = 0;
            rc.right = PLATFORM_SCREEN_WIDTH_PIXELS;
            if (!rcintersect(&rc, clip)) {
                xstart = 0;
                temp = (slope < 0 ? -slope : slope) + 1;
                if (temp > 0x20)
                    temp = 0x20;
                for (i = 0; i < temp; i++) {
                    rc.left = xstart;
                    rc.right = ((PLATFORM_SCREEN_WIDTH_PIXELS * i + PLATFORM_SCREEN_WIDTH_PIXELS) / temp) & vidflg3is_minus1;
                    if (rc.left != rc.right) {
                        horizon = start_y + (slope * i) / temp;
                        skybox_op_helper2(&rc, detail, horizon);
                        xstart = rc.right;
                    }
                }
            }
        } else {
            i = polang(pts[0].px - pts[1].px, pts[0].py - pts[1].py) & FAST_TRIG_ANGLE_MASK;
            for (slope = 2; slope < 6; slope++) {
                if (slope < 4)
                    temp = 0;
                else
                    temp = 1;
                pts[slope].px = pts[temp].px + mulscl(0x3e80, sinfast(horizon_angles[slope - 2] + i));
                pts[slope].py = pts[temp].py + mulscl(0x3e80, cosfast(horizon_angles[slope - 2] + i));
            }
            skybox_op_helper(g_skybox_sky_clr, 4, pts[0], pts[1], pts[3], pts[2]);
            skybox_op_helper(ground_skybox, 4, pts[0], pts[1], pts[4], pts[5]);
            draw_result = 1;
        }
    } else {
        transform_input.x = 0;
        transform_input.y = -camera_y;
        transform_input.z = 0x3a98 * latitude;
        mat_vec(&transform_input, rotation, &vecs[0]);
        if (vecs[0].z < 0) {
            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
            if (statemgmtcpy != 0) {
                draw_result = 1;
                tparr[5].left = 0;
                tparr[5].right = PLATFORM_SCREEN_WIDTH_PIXELS;
                tparr[5].top = clip->top;
                tparr[5].bottom = clip->bottom;
            }
            goto done;
        }

        vector_to_point(&vecs[0], &pts[0]);
        horizon = pts[0].py;
        if (clip->top > horizon)
            horizon = clip->top;

        if (latitude == 1) {
            if (statemgmtcpy == 0)
                goto simple;
            tparr[5].top = (detail_lvl == 4) ? horizon - 1 : horizon - maxscnh;
            tparr[5].left = 0;
            tparr[5].right = PLATFORM_SCREEN_WIDTH_PIXELS;
            tparr[5].bottom = horizon;
            if (g_simprect != 0) {
simple:
                rc.left = 0;
                rc.right = PLATFORM_SCREEN_WIDTH_PIXELS;
                rc.top = clip->top;
                rc.bottom = clip->bottom;
                skybox_op_helper2(&rc, detail, horizon);
            } else {
                for (i = 0; i < 15; i++)
                    rects_updt[i] = 1;
                if (detail_lvl == 4)
                    rotpr[preview_index] = prevcamrot;
                if (rotpr[preview_index] == detail &&
                    rectp[5].left == tparr[5].left &&
                    rectp[5].right == tparr[5].right &&
                    rectp[5].top == tparr[5].top &&
                    rectp[5].bottom == tparr[5].bottom)
                    rects_updt[5] = 0;
                else
                    rects_updt[5] = 3;
                rect_num3 = 0;
                rectlist_add(15, rects_updt, rectp, tparr,
                                   clip, &rect_num3, rectclip);
                for (temp = 0; temp < rect_num3; temp++)
                    skybox_op_helper2(&rectclip[temp], detail, horizon);
            }
        } else {
            i = horizon - clip->top;
            if (clip->bottom - clip->top < i)
                i = clip->bottom - clip->top;
            if (i > 0) {
                /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->top + i);
                /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(ground_skybox);
            }
            i = clip->bottom - horizon;
            if (i > 0) {
                /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, horizon, horizon + i);
                /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
            }
            draw_result = 1;
        }
    }
done:
    return draw_result;
}

void transformed_shape_add_for_sort(I16 zadjust, I8 sort_group_id) {
	struct VECTOR transformedpos;
	struct VECTOR shapepos;

	shapepos = g_curr_tsp->pos;
	mat_vec(&shapepos, &wkmatx, &transformedpos);
	g_tdist[(I16)g_ts_num] = transformedpos.z + zadjust;
	tshapearrarg2[(I16)g_ts_num] = sort_group_id;
	tsix[(I16)g_ts_num] = (I16)g_ts_num;
	g_ts_num++;
	g_curr_tsp++;
}

/* Draws the track preview background. Params: none. Returns: none. State: reads current track, camera and skybox state. */
/* PLATFORM(video): draws to or configures the display surface. */
void draw_track_preview(void)
{
    struct TRACKOBJECT *overlay;
    U8  elem;
    register I16 obj_x_pos;
    struct VECTOR vector;
    register I16 obj_height;
    I16 camera_distance;
    I16 obj_z;
    struct MATRIX *cam_matrix;
    struct TRANSFORMEDSHAPE3D transformed;
    I16 horizon;
    struct POINT2D screen_point;
    I16 corner;
    I8S  cx;
    I16 rot_angle;
    I8S  tile_col;
    I8S  row_idx;
    I8S  cz;
    U8  terr;
    struct TRACKOBJECT *track_obj;

    camera_distance = polradius2d(camera_aim_x - camera_pos_x,
                                    camera_aim_z - camera_pos_z);
    rot_angle = polang(camera_aim_y - camera_pos_y, camera_distance);
    cam_matrix = matrotzxy(0, rot_angle, 0, 1);
    mat_vec(&track_prev_vec, cam_matrix, &vector);
    vector_to_point(&vector, &screen_point);

    horizon = screen_point.py;
    if (horizon < 0)
        horizon = 0;
    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, horizon - sky_hgt_world);
    /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, 0x64);
    /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[2], 0, horizon - scene_3ht);
    /* PLATFORM(video): blit a shape to the display. */ sprite_putimage_and_alt(skypics[3], PLATFORM_SCREEN_WIDTH_PIXELS, horizon - scene_4ht);
    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, horizon, PLATFORM_SCREEN_HEIGHT_PIXELS);
    /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(ground_skybox);
    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS);
    select_rot(0, rot_angle, 0, &trackpreview_cliprect, 1);

    transformed.rotvec.x = 0;
    transformed.rotvec.y = 0;
    transformed.unk = 0x400;

    for (row_idx = 0; row_idx < 30; row_idx++) {
        for (tile_col = 0; tile_col < 30; tile_col++) {
            elem = td14tb[lnoffsets[row_idx] + tile_col];
            terr = td15p_9[gterrtrk[row_idx] + tile_col];
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
                        terr = td15p_9[gterrtrk[cz] + cx];
                        if (terr != 0) {
                            track_obj = &scene2[terr];
                            transformed.shapeptr = track_obj->ss_shapePtr;
                            transformed.pos.x = (trackctrpos2[cx] - camera_pos_x) >> 1;
                            transformed.pos.y = (-camera_pos_y) >> 1;
                            transformed.pos.z = (row_ctr_zs[cz] - camera_pos_z) >> 1;
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
                transformed.pos.x = (trackctrpos2[tile_col] - camera_pos_x) >> 1;
                transformed.pos.y = (obj_height - camera_pos_y) >> 1;
                transformed.pos.z = (row_ctr_zs[row_idx] - camera_pos_z) >> 1;
                transformed.rotvec.z = track_obj->ss_rotY;
                transformed.ts_flags = 5;
                transformed.material = 0;
                trans_op(&transformed);
            }

            if (elem != 0) {
                track_obj = &trklst[elem];
                obj_z = (track_obj->ss_multiTileFlag & 1) ? r_zp[row_idx] : row_ctr_zs[row_idx];
                if (track_obj->ss_multiTileFlag & 2)
                    obj_x_pos = xcols[tile_col + 1];
                else
                    obj_x_pos = trackctrpos2[tile_col];
                vector.x = (obj_x_pos - camera_pos_x) >> 1;
                vector.y = (obj_height - camera_pos_y) >> 1;
                vector.z = (obj_z - camera_pos_z) >> 1;

                if (obj_height != 0) {
                    switch (track_obj->ss_multiTileFlag) {
                    case 0:
                        transformed.shapeptr = &g_shapes3d[43];
                        break;
                    case 1:
                        transformed.shapeptr = &g_shapes3d[91];
                        break;
                    case 2:
                        transformed.shapeptr = &g_shapes3d[92];
                        break;
                    case 3:
                        transformed.shapeptr = &g_shapes3d[93];
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

/* Draws the in-game text overlay. Params: none. Returns: the affected rectangle. State: reads and updates text and game-display state. */
/* PLATFORM(video): draws to or configures the display surface. */
struct RECTANGLE *draw_ingame_text(void)
{
    register I16 remainder;

    game_rect_txt_in = clipunk;

    if (menutimeout != 0) {
        copy_string(&resbuftext, locate_text_resource(gamerptrs, aDm1));
        /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
            introtext(&resbuftext, font_op2_alt(&resbuftext), 0xAA, dlg_colour, 0),
            &game_rect_txt_in);

        copy_string(&resbuftext, locate_text_resource(gamerptrs, aDm2));
        /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
            introtext(&resbuftext, font_op2_alt(&resbuftext), 0xB6, dlg_colour, 0),
            &game_rect_txt_in);
    } else if (gm_playmode == 0) {
        if (core.game_inputmode == 0) {
            copy_string(&resbuftext, locate_text_resource(gamerptrs, aPre));
            /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                introtext(&resbuftext, font_op2_alt(&resbuftext), 0x5A, dlg_colour, 0),
                &game_rect_txt_in);
        } else if (pass_check_flag == 0) {
            copy_string(&resbuftext, locate_text_resource(gamerptrs, aSe1));
            /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                introtext(&resbuftext, font_op2_alt(&resbuftext), 0x5D, dlg_colour, 0),
                &game_rect_txt_in);
            copy_string(&resbuftext, locate_text_resource(gamerptrs, aSe2));
            /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                introtext(&resbuftext, font_op2_alt(&resbuftext), 0x69, dlg_colour, 0),
                &game_rect_txt_in);
        } else if (follow_op == 0 && cammd == 0 && core.playerstate.car_crashBmpFlag == 0) {
            switch ((I16)core.field_45D) {
            /* PLATFORM(video): blit a transparent shape. */ case 1:
                sprite_putimage_transparent(sdgbmp_v[3], 0x94, 0x5D);
                rcunion(&game_rect_txt_in, &rect_ingame_text2, &game_rect_txt_in);
                break;
            /* PLATFORM(video): blit a transparent shape. */ case 2:
                sprite_putimage_transparent(sdgbmp_v[4], 0x94, 0x5D);
                rcunion(&game_rect_txt_in, &rect_ingame_text2, &game_rect_txt_in);
                break;
            case 3:
                copy_string(&resbuftext, locate_text_resource(gamerptrs, aWww));
                /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                    introtext(&resbuftext, font_op2_alt(&resbuftext), 0x5D, dlg_colour, 0),
                    &game_rect_txt_in);
                break;
            }

            resbuftext = 0;
            switch ((I16)core.field_45E) {
            /* PLATFORM(video): blit a transparent shape. */ case 1:
                sprite_putimage_transparent(sdgbmp_v[3], 0x44, 0x71);
                rcunion(&game_rect_txt_in, &rect_ingame_text3, &game_rect_txt_in);
                copy_string(&resbuftext, locate_text_resource(gamerptrs, aOpp));
                break;
            /* PLATFORM(video): blit a transparent shape. */ case 2:
                sprite_putimage_transparent(sdgbmp_v[4], 0xE4, 0x71);
                rcunion(&game_rect_txt_in, &rect_ingame_text4, &game_rect_txt_in);
                copy_string(&resbuftext, locate_text_resource(gamerptrs, aOpp_0));
                break;
            }

            if (resbuftext != 0) {
                /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                    introtext(&resbuftext, font_op2_alt(&resbuftext), 0x74, dlg_colour, 0),
                    &game_rect_txt_in);
            }

            if (pen_flag_count != 0) {
                copy_string(&resbuftext, locate_text_resource(gamerptrs, aPen));
                fmtframestr(&resbuftext + strlen(&resbuftext), g_penaltytm, 0);
                /* PLATFORM(video): use the display text or sprite interface. */ rcunion(&game_rect_txt_in,
                    introtext(&resbuftext, font_op2_alt(&resbuftext), 0x66, dlg_colour, 0),
                    &game_rect_txt_in);
            }
        }
    } else if (gm_playmode == 2) {
        remainder = core.game_frame % rate_frame;
        if (remainder < ((I16S)rate_frame >> 1)) {
            copy_string(&resbuftext, locate_text_resource(gamerptrs, aRpl_0));
            /* PLATFORM(video): draw the introduction text. */ rcunion(&game_rect_txt_in,
                introtext(&resbuftext, 0x138 - (strlen(&resbuftext) << 3), 0x0F, dlg_colour, 0),
                &game_rect_txt_in);
        }
    }

    return &game_rect_txt_in;
}

/* Updates the sinking message animation. Params: frame, top and height. Returns: affected rectangle. State: reads and updates message-rectangle state. */
/* PLATFORM(video): draws to or configures the display surface. */
struct RECTANGLE *do_sinking(I16 frame, I16 top, I16 height)
{
    register I16 offset;

    if (frame > (I16)rate_frame * 4)
        frame = (I16)rate_frame * 4;

    offset = (I16)(((I32)height * frame) / (I32)((I16)rate_frame * 4));

    game_rect_txt_in.left = 0;
    game_rect_txt_in.right = PLATFORM_SCREEN_WIDTH_PIXELS;
    game_rect_txt_in.top = top + height - offset;
    game_rect_txt_in.bottom = top + height;

    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, game_rect_txt_in.top, game_rect_txt_in.bottom);
    /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skyboxwat_clr);
    return &game_rect_txt_in;
}

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct LOOKAHEAD_TILE { I8 east_delta, south_delta, detail; };
I16 fence_offsets[8] = { 30, 200, 320, 400, 530, 700, 880, 960 };
struct SHAPE3D *fence_shapes[8] = {
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x420),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x40A),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x3F4),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x420),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x40A),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x3F4),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x420),
    (struct SHAPE3D *)((I8 *)&g_shapes3d[0].shape3d_numverts + 0x40A),
};

static struct LOOKAHEAD_TILE lookahead_heading0[23] = {
    { -2, -4, 2 },
    { -1, -4, 2 },
    { 0, -4, 2 },
    { 1, -4, 2 },
    { 2, -4, 2 },
    { -2, -3, 1 },
    { -1, -3, 1 },
    { 0, -3, 1 },
    { 1, -3, 1 },
    { 2, -3, 1 },
    { -2, -2, 1 },
    { -1, -2, 0 },
    { 0, -2, 0 },
    { 1, -2, 0 },
    { 2, -2, 1 },
    { -2, -1, 0 },
    { -1, -1, 0 },
    { 0, -1, 0 },
    { 1, -1, 0 },
    { 2, -1, 0 },
    { -1, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading1[23] = {
    { 2, -4, 2 },
    { 1, -4, 2 },
    { 0, -4, 2 },
    { -1, -4, 2 },
    { -2, -4, 2 },
    { 2, -3, 1 },
    { 1, -3, 1 },
    { 0, -3, 1 },
    { -1, -3, 1 },
    { -2, -3, 1 },
    { 2, -2, 1 },
    { 1, -2, 0 },
    { 0, -2, 0 },
    { -1, -2, 0 },
    { -2, -2, 1 },
    { 2, -1, 0 },
    { 1, -1, 0 },
    { 0, -1, 0 },
    { -1, -1, 0 },
    { -2, -1, 0 },
    { 1, 0, 0 },
    { -1, 0, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading2[23] = {
    { 4, -2, 2 },
    { 4, -1, 2 },
    { 4, 0, 2 },
    { 4, 1, 2 },
    { 4, 2, 2 },
    { 3, -2, 1 },
    { 3, -1, 1 },
    { 3, 0, 1 },
    { 3, 1, 1 },
    { 3, 2, 1 },
    { 2, -2, 1 },
    { 2, -1, 0 },
    { 2, 0, 0 },
    { 2, 1, 0 },
    { 2, 2, 1 },
    { 1, -2, 0 },
    { 1, -1, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 2, 2, 0 },
    { 0, -1, 0 },
    { 0, 1, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading3[23] = {
    { 4, 2, 2 },
    { 4, 1, 2 },
    { 4, 0, 2 },
    { 4, -1, 2 },
    { 4, -2, 2 },
    { 3, 2, 1 },
    { 3, 1, 1 },
    { 3, 0, 1 },
    { 3, -1, 1 },
    { 3, -2, 1 },
    { 2, 2, 1 },
    { 2, 1, 0 },
    { 2, 0, 0 },
    { 2, -1, 0 },
    { 2, -2, 1 },
    { 1, 2, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, -1, 0 },
    { 1, -2, 0 },
    { 0, 1, 0 },
    { 0, -1, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading4[23] = {
    { 2, 4, 2 },
    { 1, 4, 2 },
    { 0, 4, 2 },
    { -1, 4, 2 },
    { -2, 4, 2 },
    { 2, 3, 1 },
    { 1, 3, 1 },
    { 0, 3, 1 },
    { -1, 3, 1 },
    { -2, 3, 1 },
    { 2, 2, 0 },
    { 1, 2, 0 },
    { 0, 2, 0 },
    { -1, 2, 0 },
    { -2, 2, 1 },
    { 2, 1, 0 },
    { 1, 1, 0 },
    { 0, 1, 0 },
    { -1, 1, 0 },
    { -2, 1, 0 },
    { 1, 0, 0 },
    { -1, 0, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading5[23] = {
    { -2, 4, 2 },
    { -1, 4, 2 },
    { 0, 4, 2 },
    { 1, 4, 2 },
    { 2, 4, 2 },
    { -2, 3, 1 },
    { -1, 3, 1 },
    { 0, 3, 1 },
    { 1, 3, 1 },
    { 2, 3, 1 },
    { -2, 2, 1 },
    { -1, 2, 0 },
    { 0, 2, 0 },
    { 1, 2, 0 },
    { 2, 2, 1 },
    { -2, 1, 0 },
    { -1, 1, 0 },
    { 0, 1, 0 },
    { 1, 1, 0 },
    { 2, 1, 0 },
    { -1, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading6[23] = {
    { -4, 2, 2 },
    { -4, 1, 2 },
    { -4, 0, 2 },
    { -4, -1, 2 },
    { -4, -2, 2 },
    { -3, 2, 1 },
    { -3, 1, 1 },
    { -3, 0, 1 },
    { -3, -1, 1 },
    { -3, -2, 1 },
    { -2, 2, 1 },
    { -2, 1, 0 },
    { -2, 0, 0 },
    { -2, -1, 0 },
    { -2, -2, 1 },
    { -1, 2, 0 },
    { -1, 1, 0 },
    { -1, 0, 0 },
    { -1, -1, 0 },
    { -1, -2, 0 },
    { 0, 1, 0 },
    { 0, -1, 0 },
    { 0, 0, 0 },
};

static struct LOOKAHEAD_TILE lookahead_heading7[23] = {
    { -4, -2, 2 },
    { -4, -1, 2 },
    { -4, 0, 2 },
    { -4, 1, 2 },
    { -4, 2, 2 },
    { -3, -2, 1 },
    { -3, -1, 1 },
    { -3, 0, 1 },
    { -3, 1, 1 },
    { -3, 2, 1 },
    { -2, -2, 1 },
    { -2, -1, 0 },
    { -2, 0, 0 },
    { -2, 1, 0 },
    { -2, 2, 1 },
    { -1, -2, 0 },
    { -1, -1, 0 },
    { -1, 0, 0 },
    { -1, 1, 0 },
    { -1, 2, 0 },
    { 0, -1, 0 },
    { 0, 1, 0 },
    { 0, 0, 0 },
};

I8 *ahead_tables[8] = { (I8 *)lookahead_heading1, (I8 *)lookahead_heading2, (I8 *)lookahead_heading3, (I8 *)lookahead_heading4, (I8 *)lookahead_heading5, (I8 *)lookahead_heading6, (I8 *)lookahead_heading7, (I8 *)lookahead_heading0 };
struct RECTANGLE rcunk5 = { 0, 320, 0, 200 };
I8 detthrlevel[6] = { 2, 2, 1, 0, 0, 0 };
I16 hill_offs[9][2] = {
    { 0, 0 },
    { 0, 512 },
    { 0, -512 },
    { 512, 0 },
    { -512, 0 },
    { -512, 512 },
    { -512, -512 },
    { 512, 512 },
    { 512, -512 },
};
I8 paint_cycle[16] = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3 };
I16 shape_rot[8] = { 0, 0, 256, 256, 512, 512, 768, 768 };
U8  fence_codes[8] = { 214, 215, 214, 215, 214, 215, 214, 215 };
I8 fence_off_1[2] = { 0, 0 };
I8 fence_off_2[4] = { 0, 0, 0, 1 };
I8 fence_off_3[4] = { 0, 0, 1, 0 };
I8 fence_off_4[8] = { 0, 0, 1, 0, 0, 1, 1, 1 };
I16S horizon_angles[4] = { 128, 384, 640, 896 };
I16 camera_pos_x = 15360;
I16 camera_pos_y = 20200;
I16 camera_pos_z = -2800;
I16 camera_aim_x = 15360;
I16 camera_aim_y = 2800;
I16 camera_aim_z = 10960;
struct VECTOR track_prev_vec = { 0, -10100, 16760 };
struct RECTANGLE trackpreview_cliprect = { 0, 320, 0, 200 };
I8 aDm1[] = "dm1";
I8 aDm2[] = "dm2";
I8 aPre[] = "pre";
I8 aSe1[] = "se1";
I8 aSe2[] = "se2";
I8 aWww[] = "www";
I8 aOpp[] = "opp";
I8 aOpp_0[] = "opp";
I8 aPen[] = "pen";
I8 aRpl_0[] = "rpl";
struct RECTANGLE rect_ingame_text2 = { 148, 172, 93, 108 };
struct RECTANGLE rect_ingame_text3 = { 68, 92, 113, 128 };
struct RECTANGLE rect_ingame_text4 = { 228, 252, 113, 128 };




static char init_crak_resource_names[2][5] = { "crak", "cinf" };

/* Updates the crack/crash overlay. Params: frame, top and height. Returns: affected rectangle. State: reads and updates crash-overlay state. */
/* PLATFORM(video): draws to or configures the display surface. */
/* PLATFORM(file): resolves crash-shape data inside a loaded resource. */ struct RECTANGLE *init_crak(I16 frame, I16 top, I16 height)
{
    /* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct CRACK_LINE { struct POINT2D start, end; };
    struct CRACK_LINE far *crak_shape;
    I16 far *cinf_shape;
    I16 animation_counter;
    I16 total_lines;
    struct POINT2D start_point, crack_end, adjust_point;
    register I16 crackIndex;

    /* PORT: crash-shape data uses a segmented FAR resource pointer. */ /* PLATFORM(file): resolve crash-line data inside the loaded bundle. */ crak_shape = (struct CRACK_LINE far *)locate_shape_alt(gamerptrs, init_crak_resource_names[0]);
    /* PORT: animation metadata uses a segmented FAR resource pointer. */ /* PLATFORM(file): resolve crash-animation data inside the loaded bundle. */ cinf_shape = (I16 far *)locate_shape_alt(gamerptrs, init_crak_resource_names[1]);

    animation_counter = frame / ((I16)rate_frame / 7);
    if (animation_counter >= cinf_shape[0])
        animation_counter = cinf_shape[0] - 1;
    total_lines = cinf_shape[animation_counter + 1];

    game_rect_txt_in = clipunk;
    for (crackIndex = 0; crackIndex < total_lines; ++crackIndex) {
            start_point = crak_shape[crackIndex].start;
            crack_end = crak_shape[crackIndex].end;

        start_point.py = (I16)(((I32)start_point.py * height) / 200);
        crack_end.py = (I16)(((I32)crack_end.py * height) / 200);

        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top - 1,
                       crack_end.px, crack_end.py + top - 1, 0);
        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top + 1,
                       crack_end.px, crack_end.py + top + 1, 0);
        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top,
                       crack_end.px, crack_end.py + top, dlg_colour);

        if (statemgmtcpy != 0) {
            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top - 1;
            rect_adjust_from_point(&adjust_point, &game_rect_txt_in);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top + 1;
            rect_adjust_from_point(&adjust_point, &game_rect_txt_in);

            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top + 1;
            rect_adjust_from_point(&adjust_point, &game_rect_txt_in);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top - 1;
            rect_adjust_from_point(&adjust_point, &game_rect_txt_in);
        }
    }

    return &game_rect_txt_in;
}



/* Loads the selected skybox. Params: skybox mode. Returns: none. State: updates the skybox resource handle and loaded-state flag. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
void far load_skybox(I8 mode)
{
    register I16 skyHeight;
    struct MATERIALCLRLIST *materials;

    if (mode & 8)
        mode &= 7;
    else if (skybox_loaded != 0 && mode == scene_idx)
        return;
    else {
        unload_skybox();
        scene_idx = mode;
        skybox_loaded = 1;
        /* PLATFORM(file): load the selected skybox resource. */ skyres_handle.pointer = file_load_shape2d_fatal_thunk(scenery_names + 9 * mode);
        locate_many_resources(skyres_handle.pointer, "scensce2sce3sce4",
                              (I8 far **)skypics);
        scene_1ht = skypics[0]->height;
        scene_2ht = skypics[1]->height;
        scene_3ht = skypics[2]->height;
        scene_4ht = skypics[3]->height;
        skyHeight = scene_1ht;
        if (skyHeight > scene_2ht) skyHeight = scene_2ht;
        if (skyHeight > scene_3ht) skyHeight = scene_3ht;
        if (skyHeight > scene_4ht) skyHeight = scene_4ht;
        sky_hgt_world = skyHeight;
        skyHeight = scene_1ht;
        if (skyHeight < scene_2ht) skyHeight = scene_2ht;
        if (skyHeight < scene_3ht) skyHeight = scene_3ht;
        if (skyHeight < scene_4ht) skyHeight = scene_4ht;
        maxscnh = skyHeight;
    }

    materials = material_clrlist_ptr;
    g_skybox_sky_clr = materials->sky;
    ground_skybox = materials->ground;
    g_skyboxwat_clr = materials->water;
    spdneedlegaugeclr = dlg_colour;
}

/* Releases the active skybox resource. Params: none. Returns: none. State: clears the skybox resource handle and loaded-state flag. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void far unload_skybox(void)
{
    /* PLATFORM(memory): release resource memory. */ if (skybox_loaded != 0)
        mmgr_free(skyres_handle.pointer);
    skybox_loaded = 0;
}

/* Loads the SD game shape set. Params: none. Returns: none. State: stores the loaded resource handle. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
void far load_sdgame2_shapes(void)
{
    register I16 i;
    /* PLATFORM(file): load a named resource. */ sdgame2hdl = file_load_resource(8, "sdgame2");
    locate_many_resources(sdgame2hdl, "ex01ex02ex03leftrigh",
                          (I8 far **)sdgbmp_v);
    for (i = 0; i < 3; ++i)
        exwd[i] = sdgbmp_v[i]->width;
}

/* Releases the SD game shape set. Params: none. Returns: none. State: reads the stored resource handle. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void far free_sdgame2(void) { /* PLATFORM(memory): release resource memory. */ mmgr_free(sdgame2hdl); }

I8 aCarcoun_0[] = "carcoun";

/* Loads and prepares the introduction scene. Params: none. Returns: setup status. State: reads resource names and updates intro/render state. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
I8 far setup_intro(void)
{
    I16 lastElapsedFrames;
    I16 *drawSelectedCount;
    struct POINT2D basePointBufferA[100];
    I16 activePointCountA;
    struct POINT2D gamePointBufferB[100];
    I8 far *title3dresValue;
    I16 pointTotalB;
    struct POINT2D *currentPoints;
    I16 oldDrawOpponent;
    I16 introCloudTilt;
    I8 frameReady;
    I16 opponentXPosition;
    I16 lastOpponentY;
    I16 savedOpponentZ;
    struct VECTOR cloudPointsList[100];
    I16 targetApproachDifference;
    I16 baseFrameDelta;
    I16 oldCameraX;
    I16 cameraYIdle;
    I16 cameraZPrevious;
    I8 operationResult;
    I16 savedRectangleIndex;
    struct RECTANGLE lastDrawRect;
    I16 carHeadingData;
    struct RECTANGLE oldSavedRect;
    I8 far *lastShapeResources[3];
    struct RECTANGLE gameRestoredRect;
    I16 oldPhase;
    I16 oldCarDistance;
    I8 far *otherCarResource;
    I16 goalXPosition;
    I16 lastGoalY;
    I16 savedGoalZ;

    operationResult = 0;
    /* PLATFORM(file): load a 3D resource. */ title3dresValue = file_load_3dres("title");
    locate_many_resources(title3dresValue, "logolog2brav", lastShapeResources);
    shape3d_init_shape(lastShapeResources[0], &intro_alt);
    shape3d_init_shape(lastShapeResources[1], &logo_title);
    shape3d_init_shape(lastShapeResources[2], &brav);
    /* PLATFORM(video): create an offscreen sprite surface. */ if (g_videoflg5 == 0)
        g_wndspr = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0x0f);

    targetApproachDifference = 0;
    do {
        cloudPointsList[targetApproachDifference].x = (get_kevinrandom() << 7) - 0x4000;
        cloudPointsList[targetApproachDifference].y = -((get_kevinrandom() << 7) - 0x1388);
        cloudPointsList[targetApproachDifference].z = (get_kevinrandom() << 7) - 0x4000;
        ++targetApproachDifference;
    } while (targetApproachDifference < 100);

    /* PLATFORM(video): configure the intro scene projection. */ set_projection(0x28, 0x28, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);
    oldCameraX = 0x400;
    cameraZPrevious = 0x400;
    cameraYIdle = 0x12c;
    oldPhase = 0;
    lastElapsedFrames = 0;
    /* PLATFORM(file): load a resource file. */ otherCarResource = file_load_resource_file(aCarcoun_0);
    setup_aero_trackdata(otherCarResource, 1);
    /* PLATFORM(memory): release a loaded resource. */ unload_resource(otherCarResource);
    init_plantrak();
    /* PLATFORM(timer): read the elapsed timer interval. */ timer_get_delta();
    pointTotalB = 0;
    activePointCountA = 0;
    ((I16)statemgmtcpy) = ((I16)slow_video_mode_state);
    tparr[0].left = 0;
    tparr[0].right = PLATFORM_SCREEN_WIDTH_PIXELS;
    tparr[0].top = 0;
    tparr[0].bottom = PLATFORM_SCREEN_HEIGHT_PIXELS;
    tparr[1] = tparr[0];
    rc0_cpy = tparr[0];
    savedRectangleIndex = 0;
    frameReady = 1;

    do {
        /* PLATFORM(timer): read the elapsed timer interval. */ baseFrameDelta = timer_get_delta();
        frmexcess += baseFrameDelta;
        while (frmexcess > frmcs_time) {
            frmexcess -= frmcs_time;
            do_opponent_op();
            frameReady = 1;
            if (11 * ((I16)rate_frame) < ++lastElapsedFrames) {
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
        if (g_videoflg5 != 0)
            setup_mcgawnd2();
        /* PLATFORM(video): copy the window sprite to the display page. */ else
            sprite_copy_wnd_to_1();

        carHeadingData = 0xffff;
        oldDrawOpponent = 1;
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ opponentXPosition = (I16)(core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT);
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ lastOpponentY = (I16)(core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT);
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ savedOpponentZ = (I16)(core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT);
        if (6 * ((I16)rate_frame) > lastElapsedFrames) {
            oldDrawOpponent = 0;
            carHeadingData = core.opponentstate.car_rotate.x & 0x03ff;
            introCloudTilt = 0;
            oldCameraX = opponentXPosition;
            cameraYIdle = lastOpponentY + 20;
            cameraZPrevious = savedOpponentZ;
        } else if (11 * ((I16)rate_frame) > lastElapsedFrames) {
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

        if (((I16)statemgmtcpy) != 0) {
            if (savedRectangleIndex == 0) {
                currentPoints = (I16 *)gamePointBufferB;
                drawSelectedCount = &pointTotalB;
            } else {
                currentPoints = (I16 *)basePointBufferA;
                drawSelectedCount = &activePointCountA;
            }
        }
        intro_op(oldCameraX, cameraYIdle, cameraZPrevious, carHeadingData, introCloudTilt,
                 oldDrawOpponent, oldPhase, cloudPointsList,
                 (struct POINT2D *)currentPoints, drawSelectedCount,
                 tparr[savedRectangleIndex], &gameRestoredRect, &lastDrawRect);

        if (g_videoflg5 != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            setup_mcgawnd1();
            /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
            if (((I16)statemgmtcpy) != 0)
                tparr[savedRectangleIndex] = gameRestoredRect;
            savedRectangleIndex ^= 1;
        } else {
            /* PLATFORM(video): copy the working sprite pages. */ sprcopy2to12();
            if (((I16)statemgmtcpy) != 0) {
                rcunion(&lastDrawRect, &tparr[2], &oldSavedRect);
                if (rcintersect(&oldSavedRect, &rc0_cpy) == 0) {
                    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(oldSavedRect.left, oldSavedRect.right,
                                      oldSavedRect.top, oldSavedRect.bottom);
                    /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
                    /* PLATFORM(video): blit the saved sprite image. */ sprputimage(g_wndspr->image);
                    /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
                    tparr[0] = gameRestoredRect;
                    tparr[2] = lastDrawRect;
                }
            } else {
                /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
                /* PLATFORM(video): blit the saved sprite image. */ sprputimage(g_wndspr->image);
                /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
            }
        }

/* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ check_input:
        if (input_do_checking(baseFrameDelta)) {
            operationResult = 1;
            break;
        }
    } while (23 * ((I16)rate_frame) > lastElapsedFrames);

    if (g_videoflg5 != 0) {
        if (get_0() != 0) {
            setup_mcgawnd2();
            /* PLATFORM(video): clear a display rectangle. */ clear_rect(0, 0, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0);
            /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
            setup_mcgawnd1();
            /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
        }
    } else {
        /* PLATFORM(video): release an offscreen sprite surface. */ sprite_free_window(g_wndspr);
    }
    /* PLATFORM(memory): release resource memory. */ mmgr_free(title3dresValue);
    return (I8)operationResult;
}

/* Draws the introduction scene. Params: camera, projection and clip data from the signature. Returns: none. State: reads intro-scene and sprite state. */
/* PLATFORM(video): draws to or configures the display surface. */
void far intro_op(I16 camX, I16 camY, I16 camZ, I16 logoRotation,
                  I16 cloudRotation, I16 showOpponent, I16 useLogo,
                  struct VECTOR *cloudPoints, struct POINT2D *oldPoints,
                  I16 *oldPointCount, struct RECTANGLE inputClip,
                  struct RECTANGLE *oldClip, struct RECTANGLE *oldUnion)
{
    struct TRANSFORMEDSHAPE modelShape;
    struct RECTANGLE initialClip;
    struct RECTANGLE previousClip;
    struct RECTANGLE unionRect;
    register I16 index;
    register I16 count;
    struct POINT2D projectedPoint;
    struct VECTOR inputPoint;
    struct VECTOR relativeVector;

    initialClip = clipunk;
    select_rot(0, cloudRotation, logoRotation, &intro_cliprect, 0);
    if (useLogo)
        modelShape.shape = &intro_alt;
    else
        modelShape.shape = &logo_title;
    modelShape.pos.x = 0x400 - camX;
    modelShape.pos.y = -camY;
    modelShape.pos.z = 0x400 - camZ;
    if (((I16)statemgmtcpy) != 0) {
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
    /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.x = (I16)(core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - camX;
    /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.y = (I16)(core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - camY;
    /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.z = (I16)(core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - camZ;
    modelShape.shape = &brav;
    if (((I16)statemgmtcpy) != 0) {
        modelShape.rect = &initialClip;
        modelShape.flags = 0x0c;
    } else {
        modelShape.flags = 4;
    }
    modelShape.rotation.x = 0;
    modelShape.rotation.y = 0;
    modelShape.rotation.z = -core.opponentstate.car_rotate.x;
    modelShape.scale = 0x400;
    modelShape.material = 0;
    trans_op(&modelShape);

logo_complete:
    if (((I16)statemgmtcpy) == 0)
        goto no_incremental;
    if (*oldPointCount != 0) {
        index = 0;
        while (index < *oldPointCount) {
            projectedPoint = oldPoints[index];
            /* PLATFORM(video): draw a display pixel. */ putpixel_single_maybe(projectedPoint.px, projectedPoint.py, 0);
            ++index;
        }
    }
    rcunion(oldClip, &inputClip, &unionRect);
    if (rcintersect(&unionRect, &rc0_cpy) == 0) {
        /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(unionRect.left, unionRect.right,
                          unionRect.top, unionRect.bottom);
        /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(0);
    }
    previousClip = initialClip;
    goto prepare_draw;

/* PLATFORM(video): select the sprite clip rectangle. */ no_incremental:
    sprset1size(intro_cliprect.left, intro_cliprect.right,
                      intro_cliprect.top, intro_cliprect.bottom);
    /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(0);
/* PLATFORM(video): select the sprite clip rectangle. */ prepare_draw:
    sprset1size(intro_cliprect.left, intro_cliprect.right,
                      intro_cliprect.top, intro_cliprect.bottom);
    count = 0;
    index = 0;
    do {
        inputPoint.x = cloudPoints[index].x - camX;
        inputPoint.y = cloudPoints[index].y - camY;
        inputPoint.z = cloudPoints[index].z - camZ;
        mat_vec(&inputPoint, &wkmatx, &relativeVector);
        if (relativeVector.z > PLATFORM_SCREEN_HEIGHT_PIXELS) {
            vector_to_point(&relativeVector, &projectedPoint);
            /* PLATFORM(video): draw a display pixel. */ putpixel_single_maybe(projectedPoint.px, projectedPoint.py,
                                  intro_colorvalue);
            if (((I16)statemgmtcpy) != 0) {
                oldPoints[count++] = projectedPoint;
                rect_adjust_from_point(&projectedPoint, &previousClip);
            }
            ++intro_colorvalue;
            if (intro_colorvalue == intro_color_max)
                intro_colorvalue = 1;
        }
        ++index;
    } while (index < 100);
    if (((I16)statemgmtcpy) != 0)
        *oldPointCount = count;
    polyinfo();
    if (((I16)statemgmtcpy) != 0) {
        *oldClip = initialClip;
        *oldUnion = previousClip;
    }
}

struct RECTANGLE intro_cliprect = { 0, 320, 0, 200 };
I16 intro_colorvalue = 1;

/* Communals defined by this module (tentative definitions). */
I16 far* td10checkptr;
struct TRANSFORMEDSHAPE3D cur_shps[29];
I16 runrndx;
int sky_hgt_world;
struct SHAPE3D intro_alt;
struct SHAPE3D logo_title;
U16S  statemgmtcpy;
I16S frmcs_time;
void far* gamerptrs;
int rotpr[2];
struct RECTANGLE tparr[15];
int ground_skybox;
struct SHAPE3D brav;
I16 spdneedlegaugeclr;
int frmexcess;
struct RECTANGLE rectclip[15];
int maxscnh;
int g_tdist[29];
I8 backlightovr8;
unsigned short scene_1ht;
unsigned short scene_2ht;
unsigned short scene_3ht;
unsigned short scene_4ht;
I8 rects_updt[15];
struct RECTANGLE rclist[15];
int g_skybox_sky_clr;
I16 rcmapix[45];
I16S exwd[3];
struct RECTANGLE rc0_cpy;
struct RECTANGLE g_savrc[15];
char tshapearrarg2[30];
I8 g_ts_num;
struct SHAPE2D far *skypics[4];
struct SHAPE2D far *sdgbmp_v[5];
int tsix[29];
I8 rect_num3;
unsigned char scene_idx;
union FARRESOURCE skyres_handle;
struct RECTANGLE game_rect_txt_in;
I16 prevcamrot;
I8 far *sdgame2hdl;
I8 follow_op;
I16 g_skyboxwat_clr;
struct TRANSFORMEDSHAPE3D * g_curr_tsp;
struct MATRIX wkmatx;
short cliprects_spare4[120];
