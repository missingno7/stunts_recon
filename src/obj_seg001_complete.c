#define WORLD_COORDINATE_SCALE_SHIFT 6
#define ANGLE_UNITS_PER_TURN 0x400
#define ANGLE_TURN_MASK 0x3FF
#define ANGLE_THREE_QUARTERS_TURN 0x300
#define ANGLE_HALF_TURN 0x200
#define ANGLE_QUARTER_TURN 0x100
#define ANGLE_EIGHTH_TURN 0x80
#define ANGLE_SEVEN_EIGHTHS_TURN 0x380
#define Q8_FRACTION_BITS 8
#define CAR_SPEED_Q8_ONE_MPH 0x100
#define CAR_SPEED_Q8_THREE_MPH 0x300
#define CAR_SPEED_Q8_TWENTY_MPH 0x1400
#define CAR_SPEED_Q8_MAX_64_MPH 0x4000
#define CAR_SPEED_Q8_MAX_128_MPH 0x8000
#define CAR_SPEED_Q8_MAX_245_MPH 0xF500
#define CAR_SPEED_Q8_MAX_250_MPH 0xFA00
#define LOW_GEAR_RPM_LIMIT 0xA28
#define ENGINE_RPM_5000 5000
#define ENGINE_RPM_DELTA_LIMIT 2000
#include "stunts_types.h"
extern short rate_frame;


/* PORT: Layout uses MSC /Zp and target scalar widths. */

struct RECTANGLE {
	I16 left, right;
	I16 top, bottom;
	
	
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct VECTOR {
	I16S x, y, z;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct VECTORLONG {
	I32 lx, ly, lz;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct POINT2D {
	I16 px, py;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct MATRIX { I16 vals[9]; };

/* PORT: Layout uses MSC /Zp and target scalar widths. */
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





/* PORT: Layout uses MSC /Zp and target scalar widths. */




struct GAMEINFO {
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

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct CARSTATE {
	struct VECTORLONG car_posWorld1;
	struct VECTORLONG car_posWorld2;
	struct VECTOR car_rotate; 
                              
	I16S car_pseudoGravity;
	I16S car_steeringAngle;
	I16S car_currpm;
	I16S car_lastrpm;
	I16S car_idlerpm2;
	I16S car_speeddiff; 
	U16S  car_speed;     
                         
	U16S  car_speed2;    
                         
                         
                         
                         
	U16S  car_lastspeed; 
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
	I16S car_rc1[4]; 
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
	I8 car_sumSurfAllWheels; 
	I8 car_surfaceWhl[4];      
	I8 car_engineLimiterTimer;
	I8 car_slidingFlag;
	I8 field_C8;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 field_CD;
	U8  field_CE; 
	U8  field_CF; 
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct GAMESTATE {
	I32 game_longs1[24]; 
	I32 game_longs2[24]; 
	I32 game_longs3[24]; 
	struct VECTOR game_vec1[2]; 
	struct VECTOR game_vec3[2];
	I16S game_frame_in_sec;
	I16S game_frames_per_sec;
	I32  game_travDist;
	U16S  game_frame;
	I16S game_total_finish; 
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   
	U16S  game_penalty;
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
	I16S field_3BE[24];
	I8 kevinseed[6];
	I8 field_3F4;
	I8 game_inputmode; 
	I8 game_3F6autoLoadEvalFlag;
	I8 field_3F7[2]; 
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

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct GAMESTATE_SNAPSHOT {
	I32 game_travDist;
	U16S  game_frame;
	I16S game_total_finish;
	I16S field_144;
	I16S game_pEndFrame;
	I16S game_oEndFrame;
	U16S  game_penalty;
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	U16S  car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	U16S  downshift_rpm;
	U16S  upshift_rpm;
	U16S  max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	U8  idle_torque;
	U8  torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding[5];
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

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct TRKOBJINFO_LINK_BYTES { I8 first, second; };
/* PORT: Layout uses MSC /Zp and target scalar widths. */struct TRKOBJINFO {
    I8 si_noOfBlocks, si_entryPoint, si_exitPoint, si_entryType, si_exitType, si_arrowType;
    I16S si_arrowOrient;
    I16S *si_cameraDataOffset;
    union { I16S *dataPointer; struct TRKOBJINFO_LINK_BYTES opponentFlags; } link;
    I8 si_opp3, si_oppSpedCode;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
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
extern I16S g_videoflg5;
extern I16S g_vid_flag6;

extern U8  timeraud;
extern U8  slomodiv8;
extern U16S  elaptm1;
extern U16S  tmr2;
extern U8  sigframe;
extern U8  g_rpl_init;
extern U8  gm_playmode; 
extern short g_sgateopn;

extern I16S elapsed_time1; 
extern short g_cvxintvl; 
extern I16S frmcs_time; 
extern short st_hdg;
extern char *table_lookup;
extern I8 steerWhlRespTable_10fps[];
extern I8 steerWhlRespTable_20fps[];
extern I8 idxtrk, tagtrk;
extern char g_hillf;
extern I16S hillconsts[];

extern struct RECTANGLE boundglassrect;
extern I16S bitmapdash;
extern I16 runrndx;
extern I8 replaybar_toggle;
extern I8 inrepflg;
extern I8 cammd;
extern I8 g_rplmodui;
extern I8 gm_saved_rpl_mode;
extern I8 numid;
extern I8 g_rplbfask;
extern I8 on_off_dash;
extern I8 cam_idg;
extern char pen_flag_count;
extern I16 replayrst;
extern I16 popupact;
extern I8 byte_3B8F2;
extern I8 byte_3FE00;
extern void far* gamerptrs;
extern void far* dasm_shp_7;
extern I16 word_3F88E;
extern I8 dashbtogglesaved;
extern I8 g_replaybarcpytgl;
extern I8 is_in_rplcopy;
extern I8 follow_op;
extern I8 opp_follow_flag_backup;
extern I16 roofbmphgt_saved;
extern I8 mode_flag;
extern I8 g_rplybarenable;
extern I16 dashbmpy_copy;
extern I16 rplbarabovehgt;
extern I8 g_simprect;
extern I8 g_viewinx[];
extern I16 dastseg;
extern I16 dasty;
extern I16 g_dastbmpbuf;
extern I16 dashbmy9;
extern I16 rfy5;
extern struct RECTANGLE* rectp;
extern void setup_car_shapes(I16);
extern void update_frame(I8, struct RECTANGLE*);
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
extern struct GAMESTATE huge* cvxs_a;
extern I16 lnoffsets[];
extern I16 gterrtrk[];
extern I16 r_zp[];
extern I16 row_ctr_zs[];
extern I16 postable[];
extern I16 z_ctr_pos[];
extern I16 xcols[];
extern I16 trackctrpos2[];
extern I16S far* g_td01_track_filecpy; 
extern short far* trackdata_penalty_related; 
extern I8 far* td3;
extern short far* track04_plyraero; 
extern short far* trackdata_05_opp_aerotbl; 
extern I8 far* td6_ptr_b;
extern I8 far* trackdat7;
extern I16 far* g_td08d; 
extern I16 far* trkptrpath;
extern struct VECTOR far* td10checkptr;
extern I8 far* hscore_trk11_ptr; 
extern I8 far* savedptr_ms;
extern I8 far* td13_replay_hdr; 
extern U8  far* td14tb; 
extern U8  far* td15p_9; 
extern I8 far* g_tdreplay16buf; 
extern char far* road_trk; 
extern char far* td_18_ref;
extern U8  far* td19hdl;
extern I8 far* coursedataappend_address; 
extern char far* g_column_of_trkdata21_pth; 
extern char far* tdfrompathrow22; 
extern U8  far* trkd23adr; 
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
extern U16S  frm_rate2;
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

extern struct GAMESTATE_SNAPSHOT race_stats;
extern I16S is_audioloaded;
extern void far* musicfile;
extern void far* openvfile;
extern I8 textrespfxchr; 
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







extern int g_penaltytm;

/* PORT: Layout uses MSC /Zp and target scalar widths. */
struct AUDIO_CAR_FRAME {
    I8 reserved[6];
    I16S player_offsets[6];
    I16S opponent_offsets[6];
    I16S player_rpm;
    I16S opponent_rpm;
};
/* PORT: Layout uses MSC /Zp and target scalar widths. */struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    I16S has_opponent_link;
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
extern I16 pl_i;
extern int g_planidx2;
extern int frwhl_angadjusted;
extern struct VECTOR pln_rot_output;
extern I8 g_cursurfacekindvalue;
extern int nextpos_normalip;
extern I16 road_num;
extern I16 element_min_wall;
extern I16 wall_wallelement;
extern I16 wallanchor_x;
extern int wallanchor_z;
extern int wall_facingang;
extern struct PLANE far* g_planlist;
extern struct PLANE far* plncurrptr;
extern I16 x_course_part;
extern I16 road_elem_ctrz;
extern I16 hgthgt;
extern I8 test_pln;
extern struct POINT2D collision_point_set_b[2];
extern struct POINT2D collision_point_set_a[2];
extern struct POINT2D collision_point_set_c[2];
extern I16 collision_rotation_offsets[4];
extern int op_eng_sound_id;
extern int g_player_sound_id;
extern I16 bto_auxiliary1(I16, I16, struct VECTOR*);
extern I16S g_trackpiecescounter;
extern struct PLANE far plan_memres[];
extern void initialize_unknown(void);
extern unsigned const I8* g_ascii_props;
extern struct SHAPE3D g_shapes3d[];
extern unsigned select_rot(I16 angX, I16 angY, I16 angZ, struct RECTANGLE* cliprect, I16 unk);
extern void trans_op(struct TRANSFORMSHAPE3D* shape);
extern void reset_idle_counters(void);
extern void set_projection(I16, I16, I16, I16);
extern struct SPRITE far* g_wndspr;
extern struct RECTANGLE clipunk;
extern I16 polygonnumber;
extern U8  far* polyinfoptrs[];
extern U16  poly_linked_list_40ED6[];
extern void preRender_default(I16 color, I16 vertlinecount, I16* vertlines);
extern U8  opponent_spd_tbl[];
extern struct TRACKOBJECT trklst[];
extern U16  update_rpm_from_speed(U16 , U16 , U16 , I16, U16 );
extern I16 abs(I16);
extern U16S  speed_recovery_divisors[5];
extern I8 audio_frmarr[];
extern char sndpendingstate;
extern char g_plyr_snd_state;
extern char audiooppflag;
extern I8 replay_state_cache;
extern short sndposrecord;
extern I16S g_audio_frms_ix;
extern I16S viewyshift;
extern void audio_op_unk(I16S);
extern void audio_op_unk5(I16S);
extern void audio_op_unk6(I16S);
extern void audio_op_unk7(I16S);
extern void audio_function2(I16S);
extern void reset_audio_driver_state(void);
extern I16 collision_point_x_signs[];
extern I16 collision_point_y_signs[];
extern struct RECTANGLE select_rect_rc;
extern struct MATRIX g_rot_mat_z;
extern struct MATRIX matrix_x_rotation;
extern struct MATRIX g_matrix_yrot;
extern struct MATRIX matrotation_tmp;
extern unsigned mat_y_rot_angle;
extern I32 sin80, cos80;
extern U8  atantable[];
extern I16 projectiondata5, projectiondata8, projectiondata9, projectiondata10;
static struct MATRIX fallback_rotation_cache;
extern I16 last_track_rotation;
static struct MATRIX plane_rotation_cache;
extern I16 f36f40_whlData;
void opponent_op(void);
void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec);
void update_player_state(struct CARSTATE* activeCarState, struct SIMD* activeCarSetup, struct CARSTATE* otherCarState, struct SIMD* otherCarSetup, I8 isOpponent);
void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    I8 transmission, I32 posX, I32 posY, I32 posZ, I16S trkang);
void initialize_game_state(I16S arg);
void restore_gamestate(I16 frame);
void update_gamestate();
void player_op(I8 inputByte);
I8 detect_penalty(I16 *trackIndex, I16 *penaltyCounter);
void update_car_speed(I8 inputByte, I8 isOpponent, struct CARSTATE* carState, struct SIMD* carSetup);
void update_grip(struct CARSTATE *car, struct SIMD *simd, I16 isOpponent);
I8 car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent);
I16 carState_rc_op(struct CARSTATE *car, I16 value, I16 wheel);
void upd_statef20_from_steer_input(I8 input);
void audio_carstate(void);
void audio_unk3(I8 flags, I16S audioId);
void apply_audio_frame(struct AUDIO_CAR_FRAME *record, I16S value);
I8 track_edge_points(I16 trackIndex, struct TRACKRESULT *result, I8 side,
              I8 *opponentSpeed);
I8 car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds);
void init_plantrak(void);
void do_opponent_op(void);

void update_crash_state(I16 crashMode, I16 isOpponent);
void plnrotop(void);
I16 plnoriginop(I16 planeIndex, I16 x, I16 y, I16 z);
I16 vec_normalInnerProduct(I16 x, I16 y, I16 z, struct VECTOR far *normal);
void state_op_unk(I16 mode, I16S angle, I16S speed);
void update_crash_debris(void);

I16 collision_response_offsets[4] = { 10, 50, 10, 20 };
struct POINT2D collision_point_set_a[2] = { { 5, 40 }, { 5, 10 } };
struct POINT2D collision_point_set_b[2] = { { 6, 121 }, { 6, 9 } };
struct POINT2D collision_point_set_c[2] = { { 1, 10 }, { 1, 10 } };
I16 collision_rotation_offsets[4] = { 21, 21, 15, 15 };
I8 steerWhlRespTable_20fps[64] = {
    0, 8, -8, 0, 0, 7, -7, 0, 0, 6, -6, 0, 0, 5, -5, 0,
    0, 4, -4, 0, 0, 4, -4, 0, 0, 3, -3, 0, 0, 3, -3, 0,
    0, 2, -2, 0, 0, 2, -2, 0, 0, 2, -2, 0, 0, 1, -1, 0,
    0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0
};
I8 steerWhlRespTable_10fps[62] = {
    0, 16, -16, 0, 0, 14, -14, 0, 0, 12, -12, 0, 0, 10, -10, 0,
    0, 8, -8, 0, 0, 8, -8, 0, 0, 6, -6, 0, 0, 6, -6, 0,
    0, 4, -4, 0, 0, 4, -4, 0, 0, 4, -4, 0, 0, 2, -2, 0,
    0, 2, -2, 0, 0, 1, -1, 0, 0, 1, -1, 0, 0, 1
};
U16S  speed_recovery_divisors[5] = { 255, 256, 192, 128, 64 };
I8 replay_state_cache = -1;
I16 collision_point_x_signs[4] = { 1, 0, 0, 1 };
I16 collision_point_y_signs[4] = { 0, 0, 1, 1 };
I16 f36f40_whlData = 9999;
I16 last_track_rotation = 9999;

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
    I16 brake;
    I8 mode;
    register I16 temp;
    struct VECTOR pv;
    struct MATRIX *tmat;
    I16 max;
    I16 spdLimit;
    I8 skip;
    I16 x1;
    struct VECTOR diff;
    I16 x2;
    struct VECTOR delta;
    I16 steerAngle;
    I16 y1;
    I16 y2;
    struct VECTOR waypoint;
    I16 z1;
    I16 z2;

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
    x1 = core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    y1 = core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    z1 = core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    x2 = core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    y2 = core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    z2 = core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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
            if (track_edge_points(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
                          &core.field_3F9) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((I16S far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
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
                goal.x = ((I32)core.opponentstate.car_vec_unk3.x + core.opponentstate.car_vec_unk5.x) >> 1;
                if (core.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((I32)core.opponentstate.car_vec_unk3.y + core.opponentstate.car_vec_unk5.y) >> 1;
                goal.z = ((I32)core.opponentstate.car_vec_unk3.z + core.opponentstate.car_vec_unk5.z) >> 1;
                if (pv.z > -78 && core.playerstate.car_crashBmpFlag == 0)
                    core.field_45E = 2;
            } else {
                goal.x = ((I32)core.opponentstate.car_vec_unk3.x + core.opponentstate.car_vec_unk4.x) >> 1;
                if (core.opponentstate.car_vec_unk3.y == -1)
                    goal.y = -1;
                else
                    goal.y = ((I32)core.opponentstate.car_vec_unk3.y + core.opponentstate.car_vec_unk4.y) >> 1;
                goal.z = ((I32)core.opponentstate.car_vec_unk3.z + core.opponentstate.car_vec_unk4.z) >> 1;
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
            (steerAngle < 0 ? -steerAngle : steerAngle) > ANGLE_QUARTER_TURN) {
            if (track_edge_points(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
                          &core.field_3F9) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((I16S far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
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
                spdLimit = CAR_SPEED_Q8_MAX_64_MPH;
            else
                spdLimit = core.field_3F9 << Q8_FRACTION_BITS; /* PORT: Q8 value is converted at the legacy boundary. */
            if (spdLimit - CAR_SPEED_Q8_ONE_MPH > core.opponentstate.car_speed)
                mode = 1;
            else if (spdLimit + CAR_SPEED_Q8_THREE_MPH < core.opponentstate.car_speed)
                mode = 2;
        }
    }
    update_car_speed(mode, 1, &core.opponentstate, &ophys_7);
    update_grip(&core.opponentstate, &ophys_7, 0);
    update_player_state(&core.opponentstate, &ophys_7, &core.playerstate, &simdp7, 1);
    if (core.opponentstate.car_crashBmpFlag == 0) {
        diff = core.opponentstate.car_vec_unk3;
        diff.x -= core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        diff.y -= core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        diff.z -= core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        tmat = matrotzxy(core.opponentstate.car_rotate.z, core.opponentstate.car_rotate.y,
                               core.opponentstate.car_rotate.x, 1);
        mat_vec(&diff, tmat, &waypoint);
        core.opponentstate.field_48 = polang(-waypoint.x, waypoint.z) & ANGLE_TURN_MASK;
    }
    if (core.opponentstate.field_CD != 0) {
        temp = mulscl(cosfast(st_hdg),
            row_ctr_zs[tagtrk] - (I16)(core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        temp += mulscl(sinfast(st_hdg),
            trackctrpos2[idxtrk] - (I16)(core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        if (temp < 0)
            update_crash_state(3, 1);
    }
}

void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec) { struct MATRIX tmpmat; tmpmat=*mat; mat_vec(invec,&tmpmat,outvec); }

/* Purpose: Updates player and opponent vehicle simulation state.
 * Parameters: activeCarState, activeCarSetup, otherCarState, otherCarSetup, isOpponent.
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

void update_player_state(struct CARSTATE* activeCarState, struct SIMD* activeCarSetup, struct CARSTATE* otherCarState, struct SIMD* otherCarSetup, I8 isOpponent) {
	I16 planIdx;
	struct VECTOR p0;
	struct PLANE far * plane;
	I8 num;
	struct VECTOR p1;
	I16 whlHgts[4];
	struct VECTOR objPts[32];
	struct VECTOR hopVec;
	struct VECTORLONG * curWhl;
	I16 distBefore;
	I16 liftOfs;
	I16 angle;
	U8  tiltFlag;
	struct MATRIX * rotMat;
	I8 w;
	I16 spd;
	I16 remDist;
	register I16 i;
	struct VECTOR self[2];
	struct MATRIX rotMatrix;
	struct VECTOR res;
	I8 swap;
	struct MATRIX localPlane;
	struct VECTOR base;
	I8 groundContact;
	I16 frontSteer;
	I16 wheelAngle[4];
	I16 found;
	struct VECTORLONG whlPos[4];
	U16  threshold;
	struct VECTOR objPos[2];
	struct VECTOR oldVec;
	struct VECTOR tmpCrds;
	struct VECTORLONG oldWhls[4];
	struct VECTORLONG * prev;
	struct VECTOR * nextVec;
	struct VECTOR vec;
	struct VECTOR posDiffs[4];
	struct VECTOR wheelCrd;

	centerpos = activeCarState->car_posWorld1.lx;
	activeCarState->car_posWorld2.lx = centerpos;
	veh_position = activeCarState->car_posWorld1.ly;
	activeCarState->car_posWorld2.ly = veh_position;
	veh_z = activeCarState->car_posWorld1.lz;
	activeCarState->car_posWorld2.lz = veh_z;
	pln_rotate_z = anglerotate_car = activeCarState->car_rotate.z;
	car_roty_pln = yrotrotveh = activeCarState->car_rotate.y;
	car_rotate_xc = rotxvehicle = activeCarState->car_rotate.x;
	if (activeCarState->car_sumSurfAllWheels != 0)
		frontSteer = activeCarState->car_40MfrontWhlAngle >> 2;
	else
		frontSteer = 0;
	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
		spd = (I32)activeCarState->car_speed2 * 0x580 / 0x1E00U;
	else
		spd = (I32)activeCarState->car_speed2 * 0x580 / 0x3C00U;
	matrix_transform_view = *matrotzxy(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
	if (yrotrotveh != 0 || anglerotate_car != 0) {
		wheelCrd.x = 0;
		wheelCrd.y = 0;
		wheelCrd.z = 0x82;
		mat_vec(&wheelCrd, &matrix_transform_view, &res);
		activeCarState->car_pseudoGravity = -res.y;
	} else {
		activeCarState->car_pseudoGravity = 0;
	}
	if (activeCarState->car_angle_z & ANGLE_TURN_MASK) {
		tiltFlag = 1;
		rotMatrix = *matrotzxy(0, 0, -activeCarState->car_angle_z, 0);
	} else {
		tiltFlag = 0;
	}
	wheelCrd.x = 0;
	wheelCrd.y = 30000;
	wheelCrd.z = 0;
	mat_vec(&wheelCrd, &matrix_transform_view, &res);
	if (activeCarState->car_sumSurfAllWheels != 0 && res.y < 0) {
		if (activeCarState->car_speed2 > 0x1E00) {
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
		wheelCrd = activeCarSetup->wheel_coords[w];
		wheelCrd.y = -(activeCarState->car_rc2[w] + 0x180);
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
				frwhl_angadjusted = activeCarState->car_36MwhlAngle - frontSteer;
			else
				frwhl_angadjusted = activeCarState->car_36MwhlAngle;
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
		activeCarState->car_36MwhlAngle = ANGLE_HALF_TURN;
		update_crash_state(1, isOpponent);
		goto wheels_done;
	}
	curWhl = whlPos;
	prev = oldWhls;
	for (w = 0; w < 4; ++curWhl, ++prev, ++w) {
		wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (core.game_inputmode == 2) {
			road_num = -1;
			g_cursurfacekindvalue = 1;
			pl_i = 0;
			plncurrptr = g_planlist;
		} else {
			build_obj(&wheelCrd, &activeCarState->car_whlWorldCrds1[w]);
		}
		activeCarState->car_surfaceWhl[w] = g_cursurfacekindvalue;
		wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (core.game_inputmode == 2)
			nextpos_normalip = wheelCrd.y;
		else
			nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (road_num != -1 && nextpos_normalip > element_min_wall && nextpos_normalip < wall_wallelement) {
			oldVec.x = activeCarState->car_whlWorldCrds1[w].x - wallanchor_x;
			oldVec.y = 0;
			oldVec.z = activeCarState->car_whlWorldCrds1[w].z - wallanchor_z;
			vec.x = (I16)(curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT) - wallanchor_x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			vec.y = 0;
			vec.z = (I16)(curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT) - wallanchor_z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			matroty(&localPlane, -wall_facingang - ANGLE_QUARTER_TURN);
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
					tmpCrds.x = (p1.x - res.x) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.y = (p1.y - res.y) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.z = (p1.z - res.z) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					distBefore = polarRadius3D(&tmpCrds);
					remDist = spd - distBefore;
				}
				angle = (-rotxvehicle - wall_facingang) & ANGLE_TURN_MASK;
				res.z = distBefore;
				res.y = 0;
				if (angle < ANGLE_QUARTER_TURN || angle > ANGLE_THREE_QUARTERS_TURN) {
					angle = wall_facingang;
					res.x = 0x300;
				} else {
					angle = (wall_facingang + ANGLE_HALF_TURN) & ANGLE_TURN_MASK;
					res.x = -0x300;
				}
				if (swap != 0)
					res.x = -res.x;
				rotMat = matrotzxy(-anglerotate_car, -yrotrotveh, angle, 0);
				mat_vec(&res, rotMat, &p1);
				i = (-rotxvehicle - angle) & ANGLE_TURN_MASK;
				found = 0;
				if (i > 0x100) {
					i = 0x400 - i;
					found = 1;
				}
				threshold = (100 - ((70 * i) >> 8)) << Q8_FRACTION_BITS; /* PORT: Q8 value is converted at the legacy boundary. */
				if (activeCarState->car_speed2 > threshold) {
					activeCarState->car_36MwhlAngle = found = (found != 0 ? -i : i) << 1;
					update_crash_state(1, isOpponent);
				}
				activeCarState->field_CF |= 0x10;
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
					curWhl->lx = prev->lx + (I16S)(p0.x + p1.x);
					curWhl->ly = prev->ly + (I16S)(p0.y + p1.y);
					curWhl->lz = prev->lz + (I16S)(p0.z + p1.z);
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
				activeCarState->car_rc1[w] += collision_rotation_offsets[w];
				curWhl->ly -= activeCarState->car_rc1[w];
				if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
					activeCarState->car_rc1[w] += collision_rotation_offsets[w];
					curWhl->ly -= activeCarState->car_rc1[w];
				}
				wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				if (core.game_inputmode == 2)
					nextpos_normalip = wheelCrd.y;
				else
					nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (nextpos_normalip > 12)
					activeCarState->car_surfaceWhl[w] = 0;
			}
		}
		whlHgts[w] = nextpos_normalip;
		if (nextpos_normalip == 0) {
			if (activeCarState->car_rc1[w] > 250)
				activeCarState->field_CF |= 0x20;
			if (activeCarState->car_rc1[w] > 23275)
				update_crash_state(1, isOpponent);
			activeCarState->car_rc1[w] = 0;
			goto next_wheel;
		}
		if (nextpos_normalip >= 0)
			goto next_wheel;
		plane = &g_planlist[pl_i];
		base.x = plane->plane_origin.x + x_course_part;
		base.y = plane->plane_origin.y + hgthgt;
		base.z = plane->plane_origin.z + road_elem_ctrz;
		oldVec.x = (I16)(prev->lx >> WORLD_COORDINATE_SCALE_SHIFT) - base.x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		oldVec.y = (I16)(prev->ly >> WORLD_COORDINATE_SCALE_SHIFT) - base.y; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		oldVec.z = (I16)(prev->lz >> WORLD_COORDINATE_SCALE_SHIFT) - base.z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		vec.x = (I16)(curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT) - base.x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		vec.y = (I16)(curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT) - base.y; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		vec.z = (I16)(curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT) - base.z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		localPlane = plane->plane_rotation;
		mat_invert(&localPlane, &rotMatrix);
		mat_vec(&oldVec, &rotMatrix, &p0);
		mat_vec(&vec, &rotMatrix, &p1);
		swap = 0;
		if (test_pln == 0 && p0.y < -12 && p1.y < -12) {
			if (p1.y > -24) {
				update_crash_state(5, isOpponent);
				swap = 1;
			} else {
				pl_i = 0;
				plncurrptr = g_planlist;
				test_pln = 1;
				wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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
			tmpCrds.x = (p1.x - res.x) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
			tmpCrds.y = (p1.y - res.y) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
			tmpCrds.z = (p1.z - res.z) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
			angle = polarRadius3D(&tmpCrds);
			remDist = activeCarState->car_rc1[w] + spd;
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
		wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if ((nextpos_normalip = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z)) < 0) {
			if (swap != 0)
				nextpos_normalip = -nextpos_normalip + 6;
			wheelCrd.z = 0;
			wheelCrd.x = 0;
			wheelCrd.y = -nextpos_normalip << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
			mat_mul_vector2(&wheelCrd, &g_planlist[pl_i].plane_rotation, &res);
			curWhl->lx += res.x;
			curWhl->ly += res.y;
			curWhl->lz += res.z;
		}
rc_check:
		if (activeCarState->car_rc1[w] > 250)
			activeCarState->field_CF |= 0x20;
		if (activeCarState->car_rc1[w] > 23275)
			update_crash_state(1, isOpponent);
		activeCarState->car_rc1[w] = 0;
next_wheel:
		;
	}
wheels_done:
	if (activeCarState->car_surfaceWhl[0] == 5 && activeCarState->car_surfaceWhl[1] == 5 &&
	    activeCarState->car_surfaceWhl[2] == 5 && activeCarState->car_surfaceWhl[3] == 5)
		update_crash_state(2, isOpponent);
	curWhl = whlPos;
	for (w = 0; w < 4; ++curWhl, ++w) {
		activeCarState->car_whlWorldCrds1[w].x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		activeCarState->car_whlWorldCrds1[w].y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		activeCarState->car_whlWorldCrds1[w].z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		angle = carState_rc_op(activeCarState, whlHgts[w], w);
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
	rotxvehicle = polang(angle, -distBefore) & ANGLE_TURN_MASK;
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
	activeCarState->car_sumSurfFrontWheels = activeCarState->car_surfaceWhl[0] + activeCarState->car_surfaceWhl[1];
	activeCarState->car_sumSurfRearWheels = activeCarState->car_surfaceWhl[2] + activeCarState->car_surfaceWhl[3];
	if (core.game_inputmode == 2)
		goto store_state;
	if (inrepflg == 0) {
		if (isOpponent != 0)
			/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unk3(activeCarState->field_CF, op_eng_sound_id);
		else
			/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_unk3(activeCarState->field_CF, g_player_sound_id);
	}
	rotMat = matrotzxy(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
	for (w = 0; w < 4; w++) {
		wheelCrd = activeCarSetup->wheel_coords[w];
		wheelCrd.y = activeCarSetup->collide_points[0].py << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
		mat_vec(&wheelCrd, rotMat, &res);
		wheelCrd.x = (res.x + centerpos) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.y = (res.y + veh_position) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.z = (res.z + veh_z) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		tmpCrds = wheelCrd;
		build_obj(&wheelCrd, &activeCarState->car_whlWorldCrds2[w]);
		i = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (pl_i < 4) {
			if (i <= 0)
				goto crash_wheel;
		} else {
			planIdx = pl_i;
			wheelCrd = activeCarState->car_whlWorldCrds2[w];
			build_obj(&wheelCrd, &tmpCrds);
			if (planIdx == pl_i) {
				found = plnoriginop(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
				if (gm_playmode != 1 && ((i < 0 && found > 0) || (i > 0 && found < 0))) {
crash_wheel:
					update_crash_state(5, isOpponent);
				}
			}
		}
		activeCarState->car_whlWorldCrds2[w] = tmpCrds;
	}
	groundContact = activeCarState->car_sumSurfFrontWheels + activeCarState->car_sumSurfRearWheels;
	if (isOpponent == 0 && groundContact == 0 && activeCarState->car_sumSurfAllWheels != 0)
		core.game_jumpCount++;
	activeCarState->car_sumSurfAllWheels = groundContact;
	self[0].x = centerpos >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
	self[0].y = veh_position >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
	self[0].z = veh_z >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
	self[1].x = anglerotate_car;
	self[1].y = yrotrotveh;
	self[1].z = rotxvehicle;
	if (globalgamesettings.game_opponenttype != 0) {
		objPos[0].x = otherCarState->car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		objPos[0].y = otherCarState->car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		objPos[0].z = otherCarState->car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		objPos[1].x = otherCarState->car_rotate.z;
		objPos[1].y = otherCarState->car_rotate.y;
		objPos[1].z = otherCarState->car_rotate.x;
		if (car_car_coll_detect_maybe(activeCarSetup->collide_points, self, otherCarSetup->collide_points, objPos)) {
			if (activeCarState->field_C8 != 0)
				return;
			if (car_car_speed_adjust_maybe(activeCarState, otherCarState) == 0)
				return;
			update_crash_state(1, isOpponent);
			update_crash_state(1, isOpponent ^ 1);
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
			if (car_car_coll_detect_maybe(activeCarSetup->collide_points, self, collision_point_set_c, objPos)) {
				activeCarState->car_36MwhlAngle -= ANGLE_HALF_TURN;
crash_return:
				update_crash_state(1, isOpponent);
				return;
			}
		}
	}
	i = (I8)td19hdl[lnoffsets[res.z] + res.x];
	if (i != -1 && core.field_3FA[i] == 0) {
		objPos[0].x = td10checkptr[i].x;
		objPos[0].y = td10checkptr[i].y;
		objPos[0].z = td10checkptr[i].z;
		if (car_car_coll_detect_maybe(activeCarSetup->collide_points, self, collision_point_set_a, objPos)) {
			core.field_3FA[i] = 1;
			state_op_unk(i + 2, -activeCarState->car_rotate.x, (I32)activeCarState->car_speed2 * 0x580 / 0x3C00U);
		}
	}
	if (res.x == idxtrk && res.z == tagtrk) {
		objPos[0].x = trackctrpos2[idxtrk] + mulscl(sinfast(st_hdg + ANGLE_QUARTER_TURN), 126);
		objPos[0].y = hillconsts[g_hillf];
		objPos[0].z = mulscl(cosfast(st_hdg + ANGLE_QUARTER_TURN), 126) + row_ctr_zs[tagtrk];
		if ((found = car_car_coll_detect_maybe(activeCarSetup->collide_points, self, collision_point_set_b, objPos)) == 0) {
			objPos[0].x = mulscl(sinfast(st_hdg + ANGLE_THREE_QUARTERS_TURN), 126) + trackctrpos2[idxtrk];
			objPos[0].z = mulscl(cosfast(st_hdg + ANGLE_THREE_QUARTERS_TURN), 126) + row_ctr_zs[tagtrk];
			found = car_car_coll_detect_maybe(activeCarSetup->collide_points, self, collision_point_set_b, objPos);
		}
		if (found != 0)
			goto crash_return;
	}
store_state:
	activeCarState->car_posWorld1.lx = centerpos;
	activeCarState->car_posWorld1.ly = veh_position;
	activeCarState->car_posWorld1.lz = veh_z;
	activeCarState->car_rotate.z = anglerotate_car;
	activeCarState->car_rotate.y = yrotrotveh;
	activeCarState->car_rotate.x = rotxvehicle;
	activeCarState->field_C8 = 0;
}

void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    I8 transmission, I32 posX, I32 posY, I32 posZ, I16S trkang)
{
    register I16 zero = 0;
    register I16 i;
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
    whlPos.x = posX >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    whlPos.y = posY >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    whlPos.z = posZ >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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

void initialize_game_state(I16S arg)
{
	register I16 zeroValue = 0;
	register I16 i;
	I16 tmpcol, tmprow;

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
			  mulscl(sinfast(st_hdg + ANGLE_HALF_TURN), 4096)
			+ mulscl(sinfast(st_hdg + ANGLE_THREE_QUARTERS_TURN),  512)
			+ ((I16S)idxtrk << 10);

		core.game_vec1[0].y = hillconsts[g_hillf] + 960;

		core.game_vec1[0].z =
			  mulscl(cosfast(st_hdg + ANGLE_HALF_TURN), 4096)
			+ mulscl(cosfast(st_hdg + ANGLE_THREE_QUARTERS_TURN),  512)
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
			  mulscl(sinfast(st_hdg + ANGLE_QUARTER_TURN),  36)
			+ mulscl(sinfast(st_hdg + ANGLE_HALF_TURN), 210);
		
		tmprow =
			  mulscl(cosfast(st_hdg + ANGLE_QUARTER_TURN),  36)
			+ mulscl(cosfast(st_hdg + ANGLE_HALF_TURN), 210);

		init_carstate_from_simd(
			&core.playerstate,
			&simdp7,
			globalgamesettings.game_playertransmission,
			(I32)(trackctrpos2[idxtrk] + tmpcol) * 64L,
			(I32)hillconsts[g_hillf] * 64L,
			(I32)(row_ctr_zs[tagtrk] + tmprow) * 64L,
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
			  mulscl(sinfast(st_hdg + ANGLE_THREE_QUARTERS_TURN),  36)
			+ mulscl(sinfast(st_hdg + ANGLE_HALF_TURN), 210);
		
		tmprow =
			  mulscl(cosfast(st_hdg + ANGLE_THREE_QUARTERS_TURN),  36)
			+ mulscl(cosfast(st_hdg + ANGLE_HALF_TURN), 210);

		init_carstate_from_simd(
			&core.opponentstate,
			&ophys_7,
			1,
			(I32)(trackctrpos2[idxtrk] + tmpcol) * 64L,
			(I32)hillconsts[g_hillf] * 64L,
			(I32)(row_ctr_zs[tagtrk] + tmprow) * 64L,
			-st_hdg);

		if (globalgamesettings.game_opponenttype && arg != -2) {
			track_edge_points(
				((I16S far *)td3)[core.opponentstate.car_trackdata3_index], 
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

void restore_gamestate(I16 frame)
{
    register I16 curframe;

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
	I8 var_carInputByte;
	register I16 tmp;

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
					mulscl(cosfast(st_hdg), row_ctr_zs[tagtrk] - (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)) /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					+ mulscl(sinfast(st_hdg), trackctrpos2[idxtrk] - (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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
 * Parameters: inputByte.
 * Returns: none.
 * Globals read: core, g_penaltytm, g_td01_track_filecpy, idxtrk, ophys_7, pen_flag_count,
 *            rate_frame, row_ctr_zs, simdp7, st_hdg, tagtrk, trackctrpos2,
 *            trackdata_penalty_related
 * Globals written: core, g_penaltytm, pen_flag_count
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void player_op(I8 inputByte) {
	struct VECTOR player_plane_vec;
	struct VECTOR playerRelative;
	struct VECTOR pathPoints[4];
	struct MATRIX *planeMatrix;
	U8  trackSide;
	U8  track_side;
	I8 hasPenalty;
	struct VECTOR offset_vector;
	I16 penalty_ctr;
	I8 crash_mode;
	struct VECTOR playerEdges[4];
	I16 tile_index;
	register I16 height;

	if (pen_flag_count != 0)
		pen_flag_count--;

	core.playerstate.field_CF = 1;
	if (core.playerstate.car_crashBmpFlag != 0) {
		core.field_45D = 0;
		inputByte = 2;
		if (core.playerstate.car_speed2 == 0) {
			core.playerstate.field_CF = 0;
			if (core.playerstate.car_speed == 0 && core.playerstate.car_rc1[0] == 0 &&
			    core.playerstate.car_rc1[1] == 0 && core.playerstate.car_rc1[2] == 0 &&
			    core.playerstate.car_rc1[3] == 0)
				return;
		}
	}

	update_car_speed(inputByte, 0, &core.playerstate, &simdp7);
	upd_statef20_from_steer_input((inputByte >> 2) & 3);
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
		playerRelative.x = core.playerstate.car_vec_unk3.x - (I16)(core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (core.playerstate.car_vec_unk3.y != -1)
			playerRelative.y = core.playerstate.car_vec_unk3.y - (I16)(core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		else
			playerRelative.y = 0;
		playerRelative.z = core.playerstate.car_vec_unk3.z - (I16)(core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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
				offset_vector.x -= core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				if (offset_vector.y == -1)
					offset_vector.y = -(I16)(core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				else
					offset_vector.y -= core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				offset_vector.z -= core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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
					track_edge_points(tile_index, pathPoints, (I8)(trackSide - 1), 0);
					track_edge_points(tile_index, playerEdges, trackSide, 0);
				}
				height = polang(pathPoints[0].x - playerEdges[0].x, playerEdges[0].z - pathPoints[0].z) & ANGLE_TURN_MASK;
				height = (core.playerstate.car_rotate.x - height) & ANGLE_TURN_MASK;
				if (height <= ANGLE_SEVEN_EIGHTHS_TURN && height >= ANGLE_EIGHTH_TURN)
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
		offset_vector.x -= core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (offset_vector.y == -1)
			offset_vector.y = 0;
		else
			offset_vector.y -= core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		offset_vector.z -= core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		planeMatrix = matrotzxy(core.playerstate.car_rotate.z, core.playerstate.car_rotate.y,
		                          core.playerstate.car_rotate.x, 1);
		mat_vec(&offset_vector, planeMatrix, &player_plane_vec);
		core.playerstate.field_48 = polang(-player_plane_vec.x, player_plane_vec.z) & ANGLE_TURN_MASK;
		if (core.playerstate.car_crashBmpFlag == 0) {
			switch ((unsigned)((core.playerstate.field_48 + ANGLE_EIGHTH_TURN) & ANGLE_TURN_MASK) >> 8) {
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
			row_ctr_zs[tagtrk] - (I16)(core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		height += mulscl(sinfast(st_hdg),
			trackctrpos2[idxtrk] - (I16)(core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (height < 0)
			update_crash_state(3, 0);
	}
}

I8 detect_penalty(I16 *trackIndex, I16 *penaltyCounter)
{
    register I16 cur;
    I16 mapIdx;
    I16 leastDistance;
    I8 carCol;
    I8 playerRow;
    I16 nextPiece;
    I8 tileX;
    I16 previousDistance[128];
    I8 row;
    I16 minNode;
    U8  mark[901];
    register I16 distance;
    I16 searchDepth;
    I8 last_row;
    I16 node_stack[128];
    I8 multiCell;
    I8 right_col;

    carCol = (I8)(core.playerstate.car_posWorld1.lx >> 16);
    playerRow = (I8)(0x1D - (I8)(core.playerstate.car_posWorld1.lz >> 16));
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
        multiCell = trklst[(U8 )road_trk[mapIdx]].ss_multiTileFlag;
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
 * Parameters: inputByte, isOpponent, carState, carSetup.
 * Returns: none.
 * Globals read: core, opponent_spd_tbl, rate_frame
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_car_speed(I8 inputByte, I8 isOpponent, struct CARSTATE* carState, struct SIMD* carSetup) {
	I16 knobStep;
	I16 offset;
	U16  updatedSpeed;
	I16 speedDelta;
	U8  currTorque;

	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 20)
		knobStep = 6;
	else
		knobStep = 12;

	if (carState->car_engineLimiterTimer != 0)
		carState->car_engineLimiterTimer--;

	carState->car_speeddiff = carState->car_speed2 - carState->car_lastspeed;
	carState->car_lastspeed = carState->car_speed2;
	carState->car_lastrpm = carState->car_currpm;
	if (carState->car_transmission == 0 && carState->car_changing_gear == 0) {
		if (inputByte & 0x10)
			goto upshift;
		else if (inputByte & 0x20)
			goto downshift;
	} else if (carState->car_current_gear != 0 && carState->car_changing_gear == 0 &&
	           carState->car_sumSurfRearWheels != 0) {
		if (carState->car_currpm > carSetup->upshift_rpm) {
upshift:
			if (carState->car_current_gear != carSetup->num_gears) {
				carState->car_current_gear++;
				goto shifted;
			}
		} else if (carState->car_currpm < carSetup->downshift_rpm) {
downshift:
			if (carState->car_current_gear > 1) {
				carState->car_current_gear--;
shifted:
				carState->car_changing_gear = 1;
				carState->car_fpsmul2 = ((I8)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame >> 1) + (I8)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
				carState->car_knob_x2 = carSetup->knob_points[carState->car_current_gear].px;
				carState->car_knob_y2 = carSetup->knob_points[carState->car_current_gear].py;
			}
		}
	}

	if (carState->car_changing_gear != 0) {
		if (carState->car_knob_x == carState->car_knob_x2) {
			if ((offset = carState->car_knob_y2 - carState->car_knob_y) == 0) {
				carState->car_changing_gear = 0;
				carState->car_gearratio = carSetup->gear_ratios[carState->car_current_gear];
				carState->car_gearratioshr8 = carState->car_gearratio >> 8;
			} else if (abs(offset) <= knobStep) {
				carState->car_knob_y = carState->car_knob_y2;
			} else if (offset > 0) {
				carState->car_knob_y += knobStep;
			} else {
				carState->car_knob_y -= knobStep;
			}
		} else if (carSetup->knob_points[0].py == carState->car_knob_y) {
			offset = carState->car_knob_x2 - carState->car_knob_x;
			if (abs(offset) <= knobStep)
				carState->car_knob_x = carState->car_knob_x2;
			else if (offset > 0)
				carState->car_knob_x += knobStep;
			else
				carState->car_knob_x -= knobStep;
		} else {
			offset = carSetup->knob_points[0].py - carState->car_knob_y;
			if (abs(offset) <= knobStep)
				carState->car_knob_y = carSetup->knob_points[0].py;
			else if (offset > 0)
				carState->car_knob_y += knobStep;
			else
				carState->car_knob_y -= knobStep;
		}
	} else if (carState->car_fpsmul2 != 0) {
		carState->car_fpsmul2--;
	}

	updatedSpeed = carState->car_speed;
	speedDelta = carState->car_pseudoGravity - carSetup->aerorestable[updatedSpeed >> 10];
	if (carState->car_currpm > carSetup->max_rpm) {
		carState->car_currpm = carSetup->max_rpm - 1;
brake:
		speedDelta -= carSetup->braking_eff;
	} else {
		switch (inputByte & 3) {
		case 2:
			carState->car_is_accelerating = 0;
			carState->car_engineLimiterTimer = 0;
			carState->car_is_braking = 1;
			if (isOpponent == 0)
				goto brake;
			speedDelta -= carSetup->braking_eff << 1;
			break;
		default:
			carState->car_is_accelerating = 0;
			carState->car_is_braking = 0;
			break;
		case 1:
			carState->car_is_braking = 0;
			carState->car_is_accelerating = 1;
			if (carState->car_changing_gear != 0) {
				carState->car_engineLimiterTimer = 0;
				if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
					carState->car_currpm -= 80;
				else
					carState->car_currpm -= 40;
			} else if (carState->car_sumSurfRearWheels == 0) {
				if (carState->car_currpm < carSetup->max_rpm && updatedSpeed < CAR_SPEED_Q8_MAX_250_MPH)
					speedDelta += CAR_SPEED_Q8_THREE_MPH;
			} else {
				if (carState->car_current_gear <= 1 && carState->car_currpm < LOW_GEAR_RPM_LIMIT)
					currTorque = carSetup->idle_torque;
				else
					currTorque = carSetup->torque_curve[(unsigned)carState->car_currpm >> 7];
				if (carState->car_engineLimiterTimer != 0 && carState->car_currpm < ENGINE_RPM_5000)
					currTorque = (carSetup->idle_torque + currTorque) >> 1;
				speedDelta += (carState->car_gearratioshr8 * currTorque) >> 4;
				speedDelta = (I16)((I32)speedDelta * 25 / carSetup->car_mass) >> 1;
				if (isOpponent != 0) {
					currTorque = -(opponent_spd_tbl[0] - 200) >> 1;
					if (currTorque != 0)
						speedDelta -= (I32)currTorque * speedDelta / 200;
				}
				if (speedDelta > 0x128)
					carState->car_engineLimiterTimer = 5;
			}
			break;
		}
	}
	if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10)
		speedDelta += speedDelta;

if (speedDelta >= 0) {
if (updatedSpeed < CAR_SPEED_Q8_MAX_128_MPH) {
updatedSpeed += speedDelta;
} else {
updatedSpeed += speedDelta;
if (updatedSpeed < CAR_SPEED_Q8_MAX_128_MPH || updatedSpeed > CAR_SPEED_Q8_MAX_245_MPH)
updatedSpeed = CAR_SPEED_Q8_MAX_245_MPH;
}
} else {
if (-speedDelta > updatedSpeed)
updatedSpeed = 0;
else
updatedSpeed += speedDelta;
}
	if (carState->car_sumSurfRearWheels != 0) {
		if (((offset = carState->car_speed2 - updatedSpeed) < 0 ? -offset : offset) > CAR_SPEED_Q8_TWENTY_MPH) {
			carState->car_speed = ((I32)carState->car_speed + carState->car_speed2) >> 1;
			carState->car_speed2 = carState->car_speed;
			carState->car_engineLimiterTimer = 5;
		} else {
			carState->car_speed = updatedSpeed;
			carState->car_speed2 = updatedSpeed;
		}
	} else {
		carState->car_speed = updatedSpeed;
	}

	carState->car_currpm = update_rpm_from_speed(carState->car_currpm, carState->car_speed,
		carState->car_gearratio, carState->car_changing_gear, carSetup->idle_rpm);
	if (carState->car_sumSurfAllWheels != 0 && carState->car_lastrpm > carState->car_currpm) {
		if (carState->car_lastrpm - carState->car_currpm > ENGINE_RPM_DELTA_LIMIT) {
			if ((I16)(carSetup->idle_torque * carState->car_gearratioshr8) > 12000)
				carState->car_engineLimiterTimer = 30;
		} else if (carState->car_currpm - carState->car_lastrpm > ENGINE_RPM_DELTA_LIMIT) {
			carState->car_engineLimiterTimer = 10;
			carState->car_speed2 -= 0x500;
		}
	}
	if (carState->car_speed2 > core.game_topSpeed)
		core.game_topSpeed = carState->car_speed2;
}

void update_grip(struct CARSTATE *car, struct SIMD *simd, I16 isOpponent)
{
    I16 speed;
    U16  demandedGrip;
    I16 totalGrip;
    I16 scratch;
    U8  column;
    I16 baseAngleValue;
    U8  rowCoord;
    I16 limit;

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
    speed = car->car_speed >> Q8_FRACTION_BITS; /* PORT: Q8 value is converted at the legacy boundary. */

    scratch = (limit < 0 ? -limit : limit) >> 3;

    demandedGrip = (((U16 )speed * (U16 )speed) >> 6) * (U16 )scratch; /* PORT: The MSC unsigned-int product wraps at 16 bits before scaling. */
    totalGrip = simd->grip * 2;
    totalGrip = (I16)(((I32)(simd->sliding[car->car_surfaceWhl[0]] +
                                simd->sliding[car->car_surfaceWhl[1]] +
                                simd->sliding[car->car_surfaceWhl[2]] +
                                simd->sliding[car->car_surfaceWhl[3]]) *
                         totalGrip) >> 10);

    car->car_demandedGrip = demandedGrip;
    car->car_surfacegrip_sum = totalGrip;

    if (isOpponent != 0) {
        if (car->car_steeringAngle == 0) {
            scratch = (U8 )car->car_rotate.x;
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

        if (totalGrip < (I16)demandedGrip) {
            car->car_slidingFlag = 1;
            limit = (I16)(((I32)totalGrip << 8) /
                                  ((I32)speed * speed));
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
                column = (U8 )((U32 )car->car_posWorld1.lx >> 16);
                rowCoord = (U8 )((U32 )car->car_posWorld1.lz >> 16);

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

        if (totalGrip + 1000 < (I16)demandedGrip) {
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

I8 car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent)
{
    I16S pHeading;
    I16S opponentHeading;
    I16 speedPenalty;
    I16S playerSin;
    I16S playerCosAngle;
    I16S opponentSin;
    I16S hitForce;
    U16S  opponentSpeed;
    I16 distanceToCar;
    I16S turnDifference;
    U16S  currentSpeed;
    I16S opponentCosAng;

    player->field_C8 = 1;
    opponent->field_C8 = 1;
    currentSpeed = player->car_speed2;
    opponentSpeed = opponent->car_speed2;
    pHeading = player->car_rotate.x;
    opponentHeading = opponent->car_rotate.x;
    playerSin = mulscl(currentSpeed >> Q8_FRACTION_BITS, sinfast(pHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    opponentSin = mulscl(opponentSpeed >> Q8_FRACTION_BITS, sinfast(opponentHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    playerCosAngle = mulscl(currentSpeed >> Q8_FRACTION_BITS, cosfast(pHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    opponentCosAng = mulscl(opponentSpeed >> Q8_FRACTION_BITS, cosfast(opponentHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    distanceToCar = polradius2d(opponentSin - playerSin, opponentCosAng - playerCosAngle);
    if (distanceToCar < 10)
        distanceToCar = 10;
    turnDifference = (pHeading - opponentHeading) & ANGLE_TURN_MASK;
    hitForce = distanceToCar << 8;
    speedPenalty = (0x300 * distanceToCar) >> 2;
    if (player->car_speed2 < speedPenalty)
        player->car_speed2 = 0;
    else
        player->car_speed2 -= speedPenalty;

    player->car_36MwhlAngle = opponentHeading - pHeading;
    if (player->car_36MwhlAngle >= ANGLE_HALF_TURN)
        player->car_36MwhlAngle -= ANGLE_UNITS_PER_TURN;
    if (player->car_36MwhlAngle <= -ANGLE_HALF_TURN)
        player->car_36MwhlAngle += ANGLE_UNITS_PER_TURN;

    opponent->car_36MwhlAngle = pHeading - opponentHeading;
    if (opponent->car_36MwhlAngle >= ANGLE_HALF_TURN)
        opponent->car_36MwhlAngle -= ANGLE_UNITS_PER_TURN;
    if (opponent->car_36MwhlAngle <= -ANGLE_HALF_TURN)
        opponent->car_36MwhlAngle += ANGLE_UNITS_PER_TURN;

    player->car_speed = player->car_speed2;
    opponent->car_speed = opponent->car_speed2;
    return distanceToCar > 30;
}

I16 carState_rc_op(struct CARSTATE *car, I16 value, I16 wheel)
{
    I16S oldRc;
    I16S target;
    I16S adjustment;

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

void upd_statef20_from_steer_input(I8 input)
{
    register I16 response;
    I8 responseIndex;
    register I16 oldAngle;

    oldAngle = core.playerstate.car_steeringAngle;
    responseIndex = (I8)(((U16S )core.playerstate.car_speed2 >> 10) & 0xFC);
    response = table_lookup[(I8)responseIndex + input];

    if (response > 0) {
        if (oldAngle < -1) response <<= 2;
    } else if (response != 0 && oldAngle > 1) {
        response <<= 2;
    }

    if (response == 0 && core.playerstate.car_speed2 != 0 && oldAngle != 0) {
        response = table_lookup[(I8)responseIndex + 1] * 2;
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

    if (table_lookup[(I8)responseIndex + input] == 0 &&
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
    I16S carIndex;
    struct VECTOR playerPosOld;
    I8 soundMode;
    I16S carCount;
    I16S audioId;
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

    playerPosOld.x = (I16S)(core.playerstate.car_posWorld2.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosOld.y = (I16S)(core.playerstate.car_posWorld2.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosOld.z = (I16S)(core.playerstate.car_posWorld2.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.x = (I16S)(core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.y = (I16S)(core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.z = (I16S)(core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    if (globalgamesettings.game_opponenttype != 0) {
        opponentPrior.x = (I16S)(core.opponentstate.car_posWorld2.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentPrior.y = (I16S)(core.opponentstate.car_posWorld2.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentPrior.z = (I16S)(core.opponentstate.car_posWorld2.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.x = (I16S)(core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.y = (I16S)(core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.z = (I16S)(core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
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

void audio_unk3(I8 flags, I16S audioId) { if (sndpendingstate != 0) { if (flags & 0x10) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk4(audioId); if (flags & 0x20) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk3(audioId); } }

/* Purpose: Applies a recorded audio frame to the player and opponent.
 * Parameters: record, value.
 * Returns: none.
 * Globals read: g_player_sound_id, globalgamesettings, op_eng_sound_id
 * Globals written: none detected
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void apply_audio_frame(struct AUDIO_CAR_FRAME *record, I16S value)
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

I8 track_edge_points(I16 trackIndex, struct TRACKRESULT *result, I8 side,
              I8 *opponentSpeed)
{
    U8  entry;
    struct TRKOBJINFO far *objectInfo;
    struct VECTOR vectorA;
    U8  edge;
    I16S rotateTemp;
    struct TRKOBJINFO far *infoBase;
    U8  scratch;
    U8  arrow;
    U8  trackRow;
    struct VECTOR far *edgeData;
    struct VECTOR far *trackData;
    U8  isConnected;
    struct TRACKOBJECT *trackElemObject;
    I16 linkPresent;
    struct VECTOR edgePoint;
    U8  objectIndex;

    entry = (U8 )road_trk[trackIndex];
    objectIndex = (U8 )td_18_ref[trackIndex] & 0x0f;
    isConnected = (U8 )td_18_ref[trackIndex] & 0x10;
    trackElemObject = &trklst[entry];
    infoBase = (struct TRKOBJINFO far *)trackElemObject->ss_trkObjInfoPtr;
    objectInfo = infoBase + objectIndex;
    linkPresent = 0;
    arrow = (U8 )objectInfo->si_arrowType;

    if (isConnected == 0)
        edge = (U8 )(side * 2);
    else
        edge = (U8 )((arrow - side) * 2 - 2);

    if (opponentSpeed != 0) {
        scratch = (U8 )objectInfo->si_oppSpedCode;
        trackRow = (U8 )trackElemObject->ss_surfaceType;
        *opponentSpeed = opponent_spd_tbl[trackRow + scratch];
    }

    if (objectInfo->link.dataPointer != 0)
        linkPresent = 1;

    if (isConnected != 0) {
        if (linkPresent != 0) {
            trackData = (struct VECTOR far *)objectInfo->link.dataPointer; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */
            goto forward;
        }
        trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */
        edgeData = trackData + edge;
        vectorA = edgeData[1];
        edgePoint = edgeData[0];
    } else {
        trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */
forward:
        edgeData = trackData + edge;
        vectorA = edgeData[0];
        edgePoint = edgeData[1];
    }

    switch (objectInfo->si_arrowOrient) {
    case ANGLE_THREE_QUARTERS_TURN:
        rotateTemp = vectorA.x;
        vectorA.x = -vectorA.z;
        vectorA.z = rotateTemp;
        rotateTemp = edgePoint.x;
        edgePoint.x = -edgePoint.z;
        edgePoint.z = rotateTemp;
        break;
    case ANGLE_HALF_TURN:
        vectorA.z = -vectorA.z;
        vectorA.x = -vectorA.x;
        edgePoint.z = -edgePoint.z;
        edgePoint.x = -edgePoint.x;
        break;
    case ANGLE_QUARTER_TURN:
        rotateTemp = vectorA.x;
        vectorA.x = vectorA.z;
        vectorA.z = -rotateTemp;
        rotateTemp = edgePoint.x;
        edgePoint.x = edgePoint.z;
        edgePoint.z = -rotateTemp;
        break;
    }

    scratch = (U8 )g_column_of_trkdata21_pth[trackIndex];
    trackRow = (U8 )tdfrompathrow22[trackIndex];
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

    result->center.x = ((I32)vectorA.x + edgePoint.x) >> 1;
    if (vectorA.y == -1)
        result->center.y = -1;
    else
        result->center.y = ((I32)vectorA.y + edgePoint.y) >> 1;
    result->center.z = ((I32)vectorA.z + edgePoint.z) >> 1;
    result->edge_a = vectorA;
    result->edge_b = edgePoint;
    result->has_opponent_link = (I16S)linkPresent;

    return (I16)(arrow - 1) == side;
}

I8 car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds)
{
    register I16 reach;
    struct MATRIX *matrix;
    I8 index;
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
    register I16S zeroValue;
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
    ((I16S far *)td3)[0] = zeroValue;
    ((I16S far *)td3)[1] = 1; ((I16S far *)td3)[2] = 2; ((I16S far *)td3)[3] = 3; ((I16S far *)td3)[4] = 4;
    ((I16S far *)td3)[5] = 1; ((I16S far *)td3)[6] = 2; ((I16S far *)td3)[7] = 3; ((I16S far *)td3)[8] = 4;
    ((I16S far *)td3)[9] = 1; ((I16S far *)td3)[10] = 2; ((I16S far *)td3)[11] = 3; ((I16S far *)td3)[12] = 4;
    ((I16S far *)td3)[13] = zeroValue; ((I16S far *)td3)[14] = 1; ((I16S far *)td3)[15] = 2; ((I16S far *)td3)[16] = 3; ((I16S far *)td3)[17] = zeroValue;
    opponent_spd_tbl[0] = 0xC8;
    init_carstate_from_simd(&core.opponentstate, &ophys_7, 1,
        0x17700L, 0L, ((I32)(r_zp[28] + 0x12E)) << WORLD_COORDINATE_SCALE_SHIFT, 0); /* PORT: Map-to-world scale is 64:1. */
    track_edge_points(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
        &core.opponentstate.car_vec_unk3, core.opponentstate.field_CE++,
        &core.field_3F9);
}

void do_opponent_op(void) { opponent_op(); }

/* Purpose: Applies crash transitions and updates engine audio state.
 * Parameters: crashMode, isOpponent.
 * Returns: none.
 * Globals read: core, elaptm1, endhsdemo, g_player_sound_id, inrepflg, op_eng_sound_id,
 *            rate_frame, sndpendingstate
 * Globals written: core, race_stats
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void update_crash_state(I16 crashMode, I16 isOpponent) {
	I8 suppress_car_speed;
	struct CARSTATE* var_cState;

	switch (isOpponent) {
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
	switch (crashMode) {
	case 4:
		core.game_frame_in_sec = 1;
		core.game_frames_per_sec = 1;
		break;
	case 5:
		crashMode = 1;
		suppress_car_speed = 1;
	case 1:
		var_cState->car_crashBmpFlag = 1;
		state_op_unk(isOpponent, var_cState->car_rotate.x, 0);
		if (isOpponent == 0) {
			core.game_impactSpeed = var_cState->car_speed2;
			core.game_frames_per_sec = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
		}
		if (inrepflg == 0 && sndpendingstate != 0) {
			if (isOpponent == 0)
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(g_player_sound_id);
			else
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(op_eng_sound_id);
		}
		break;
	case 2:
		if (inrepflg == 0 && sndpendingstate != 0) {
			if (isOpponent == 0)
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(g_player_sound_id);
			else
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2_wrap(op_eng_sound_id);
		}
		var_cState->car_crashBmpFlag = 2;
		suppress_car_speed = 1;
		if (isOpponent == 0) {
			core.game_impactSpeed = var_cState->car_speed2;
			core.game_frames_per_sec = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
		}
		break;
	case 3:
		var_cState->car_crashBmpFlag = 3;
		if (isOpponent == 0) {
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
	if (isOpponent != 0)
		core.game_oEndFrame = core.game_frame;
	else
		core.game_pEndFrame = core.game_frame;
	if (core.game_3F6autoLoadEvalFlag == 0 && isOpponent == 0)
		core.game_3F6autoLoadEvalFlag = crashMode;
	if ((endhsdemo & 4) == 0)
		race_stats = *(struct GAMESTATE_SNAPSHOT *)&core.game_travDist;
}

void plnrotop(void) {
	struct VECTOR rotatedVector;
	struct MATRIX rotationMatrix;
	struct MATRIX matrix;
	struct VECTOR vector;
	register I16 rotation;

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

I16 plnoriginop(I16 planeIndex, I16 x, I16 y, I16 z) {
	struct PLANE far* pPlane;
	struct VECTOR a;
	struct VECTOR b;
	
	if (planeIndex == pl_i) {
		pPlane = plncurrptr;
	} else {
		pPlane = &g_planlist[planeIndex];
	}

	b.y = pPlane->plane_origin.y + hgthgt;
	a.y = y - b.y;
	if (planeIndex < 4) {
		
		return a.y;
	}
	b.x = pPlane->plane_origin.x + x_course_part;
	b.z = pPlane->plane_origin.z + road_elem_ctrz;
	a.x = x - b.x;
	a.z = z - b.z;
	return vec_normalInnerProduct(a.x, a.y, a.z, &pPlane->plane_normal);
}

I16 vec_normalInnerProduct(I16 x, I16 y, I16 z, struct VECTOR far *normal)
{
    return (((I32)normal->x * x) + ((I32)normal->y * y) +
            ((I32)normal->z * z)) / 0x2000;
}

void state_op_unk(I16 mode, I16S angle, I16S speed)
{
    I16 speedFactor;
    register I16 debrisIndex;
    I16 startAngle;
    I16 verticalStep;
    I16 made;
    register I16S verticalValue;
    I16 countLimit;
    I16 phaseBase;
    I16 unusedCount;

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
            core.field_443[debrisIndex] = (I8)mode;
            core.field_42B[debrisIndex] = (I8)((made & 3) + phaseBase);
            core.game_longs1[debrisIndex] = 0;
            core.game_longs2[debrisIndex] = 0;
            core.game_longs3[debrisIndex] = 0;
            core.field_2FE[debrisIndex] = (I16S)(get_kevinrandom() << 2);
            core.field_32E[debrisIndex] = (I16S)(get_kevinrandom() << 2);
            core.field_35E[debrisIndex] = (I16S)((((I32)speedFactor * made) / unusedCount + startAngle) & ANGLE_TURN_MASK);
            verticalValue = (I16S)(((get_kevinrandom() * 6) >> 2) + speed + 0x180);
            core.field_38E[debrisIndex] = verticalValue;
            core.field_3BE[debrisIndex] = (I16S)((verticalStep * verticalValue) >> 2);
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
    I8 keepAlive = 0;
    register I16 i;
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
extern I8 far *locate_shape_alt(I8 far *data, I8 *name);
extern void copy_string(I8 *destination, I8 far *source);
extern I8 textstr[];
extern I8 g_gsnashape_data[];

void setup_aero_trackdata(void far *carresptr, I16 is_opponent)
{
    register I16 i;
    if (is_opponent == 0) {
        simdp7 = *(struct SIMD far *)locate_shape_alt(carresptr, "simd");
        simdp7.aerorestable = track04_plyraero;
        for (i = 0; i < 0x40; i++) {
            track04_plyraero[i] = ((I32)simdp7.aero_resistance * (I32)i * (I32)i) >> 9;
        }
        copy_string(textstr, locate_shape_alt(carresptr, "gnam"));
    } else {
        ophys_7 = *(struct SIMD far *)locate_shape_alt(carresptr, "simd");
        ophys_7.aerorestable = trackdata_05_opp_aerotbl;
        for (i = 0; i < 0x40; i++) {
            trackdata_05_opp_aerotbl[i] = ((I32)ophys_7.aero_resistance * (I32)i * (I32)i) >> 9;
        }
        copy_string(g_gsnashape_data, locate_shape_alt(carresptr, "gsna"));
    }
}

/* Communals defined by this module (tentative definitions). */
I8 g_plyr_snd_state;
I8 audiooppflag;
I16 wallanchor_z;
I16S far* trackdata_penalty_related;
I16 g_player_sound_id;
I16 op_eng_sound_id;
struct MATRIX matrix_transform_view;
I16 rotxvehicle;
struct GAMESTATE core;
I16 yrotrotveh;
I16 anglerotate_car;
char idxtrk;
I8 g_hillf;
char tagtrk;
I8 pen_flag_count;
I32 centerpos;
I32 veh_position;
I32 veh_z;
I16S rate_frame;
I8 far* road_trk;
I16S sndposrecord;
I8 *table_lookup;
I16S far* track04_plyraero;
I16S far* trackdata_05_opp_aerotbl;
unsigned char opponent_spd_tbl[16];
struct PLANE far* plncurrptr;
I16S g_sgateopn;
struct VECTOR tvec2;
struct VECTOR pln_rot_output;
I16 pln_rotate_z;
I16 car_roty_pln;
I16 car_rotate_xc;
I16 frwhl_angadjusted;
I16 g_planidx2;
char audio_frmarr[1360];
struct PLANE far* g_planlist;
I8 far* td_18_ref;
I16S st_hdg;
I8 far* g_column_of_trkdata21_pth;
struct SIMD ophys_7;
I8 sndpendingstate;
I8 far* tdfrompathrow22;
I16 nextpos_normalip;
I16S g_cvxintvl;
struct GAMESTATE huge* cvxs_a;
I16 wall_facingang;
int trackctrpos2[30];
struct SIMD simdp7;
int row_ctr_zs[30];
I16 g_penaltytm;
short framerate_pad_0;
short extra_rclist4[2];
short spare_td22_1;
short fontled_free_4[2];




