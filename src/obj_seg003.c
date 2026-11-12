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


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "geom2d.h"


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "vec3.h"

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "vec3l.h"


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: POINT2D; struct[0].name is px; another unit uses x. */
struct POINT2D {
	I16 px, py;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "matrix.h"


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "plane.h"



/* Local math imports preserve the original communal LINK ordering. */




/* Local signed radius view; the shared math API returns an unsigned word. */
/* Local view: inthypot; return is signed word; another unit uses unsigned word. */
I16 inthypot(I16 z, I16 y);






















/* Local view: clipzone; return is signed byte; another unit uses signed word. */
I8 clipzone(struct RECTANGLE* r1, struct RECTANGLE* r2);







/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: GAMEINFO; struct[8].type is unsigned word; another unit uses signed byte. */
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

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: CARSTATE; struct[54].type is signed byte; another unit uses unsigned byte. */
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
	I16S car_grip_angle_excess;
	I16S car_demandedGrip;
	I16S car_surfacegrip_sum;
	I16S car_route_bearing;
	I16S car_trackdata3_index;
	I16S car_wheel_fall_step[4]; // four words, one for each wheel.
	I16S car_wheel_suspension_offset[4];
	I16S car_rc3[4];
	I16S car_rc4[4];
	I16S car_wheel_suspension_target[4];
	struct VECTOR car_whlWorldCrds1[4];
	struct VECTOR car_whlWorldCrds2[4];
	struct VECTOR car_route_center;
	struct VECTOR car_route_edge_a;
	struct VECTOR car_route_edge_b;
	I16S car_route_has_link;
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
	I8 car_contact_processed;
	I8 car_crashBmpFlag;
	I8 car_changing_gear;
	I8 car_fpsmul2;
	I8 car_transmission;
	I8 car_finish_route_passed;
	I8 car_route_point_index; // is added?
	I8 car_audio_flags; // is initialized?
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: GAMESTATE; struct[9].type is unsigned word; another unit uses signed word. */
struct GAMESTATE {
	I32 game_debris_x[24]; // x
	I32 game_debris_y[24]; // y
	I32 game_debris_z[24]; // z
	struct VECTOR game_chase_camera_pos[2]; // 0 = player, 1 = opponent
	struct VECTOR game_chase_camera_prev;
	struct VECTOR game_opponent_camera_prev;
	I16S game_end_delay_elapsed;
	I16S game_end_delay_limit;
	I32  game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish; // finish time + penalty when crossed finish line
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   // former game_frame2
	I16S game_penalty; // probably penalty counter
	U16S  game_impactSpeed;
	U16S  game_topSpeed;
	I16S game_jumpCount;
	struct CARSTATE playerstate;
	struct CARSTATE opponentstate;
	I16S game_player_route_anchor;
	I16S game_player_last_route_node;
	I16S game_startcol;
	I16S game_startcol2;
	I16S game_startrow;
	I16S game_startrow2;
	I16S game_debris_rot_x[24];
	I16S game_debris_rot_y[24];
	I16S game_debris_rot_z[24];
	I16S game_debris_planar_step[24];
	I8 game_debris_vertical_step[48];
	I8 kevinseed[6];
	I8 game_checkpoint_valid;
	I8 game_inputmode; // 0 = waiting for input, 1 = input active, 2 = no input (during the intro)
	I8 game_3F6autoLoadEvalFlag;
	I8 game_track_camera_index[2]; // 0 = player, 1 = opponent
	I8 game_opponent_speed_limit;
	I8 game_track_object_broken[48];
	I8 game_debris_active;
	U8  game_debris_shape_index[24];
	U8  game_debris_origin_kind[24];
	I8 game_route_recovery_mode;
	I8 game_route_confirm_count;
	I8 game_player_guidance;
	I8 game_opponent_pass_side;
	I8 field_45F;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: SIMD; struct.count is 30; another unit uses 29. */
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
	I8 steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	I8 spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	I8 revpoints[256];
	I16S far* aerorestable;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: TRKOBJINFO; struct.count is 12; another unit uses 11. */
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
#include "trkobj.h"











/* Local view: g_vid_flg2_set; type is signed word; another unit uses unsigned word. */
extern I16S g_vid_flg2_set;
/* Local view: vidflg3is_minus1; type is signed word; another unit uses unsigned word. */
extern I16S vidflg3is_minus1;

/* Local view: g_videoflg5; type is signed byte; another unit uses unsigned byte. */
extern I8 g_videoflg5;
/* Local view: g_vid_flag6; type is signed word; another unit uses unsigned byte. */
extern I16S g_vid_flag6;



/* Local view: elaptm1; type is unsigned word; another unit uses signed word. */
extern U16S  elaptm1;



/* Local view: gm_playmode; type is unsigned byte; another unit uses signed byte. */
extern U8  gm_playmode; // 0 = playing, 1 = paused, 2 = replay


 // current frame?
 // fps * 30
 // 100 / fps
/* Local view: st_hdg; type is signed word; another unit uses unsigned word. */
extern I16S st_hdg;
/* Local view: table_lookup; pointer is void; another unit uses signed byte. */
extern void* table_lookup;
/* Local view: steerWhlRespTable_10fps; type is near pointer; another unit uses array[None]. */
extern void* steerWhlRespTable_10fps;
/* Local view: steerWhlRespTable_20fps; type is near pointer; another unit uses array[None]. */
extern void* steerWhlRespTable_20fps;


















/* Local view: popupact; type is signed word; another unit uses signed byte. */
extern I16 popupact;
/* Local view: mouse_enabled; type is signed byte; another unit uses unsigned byte. */
extern I8 mouse_enabled;
/* Local view: joystick_enabled; type is signed byte; another unit uses unsigned byte. */
extern I8 joystick_enabled;


/* Local view: input_pushed; type is signed word; another unit uses unsigned word. */
extern I16 input_pushed;













/* Local view: g_dastbmpbuf; type is signed word; another unit uses far pointer. */
extern I16 g_dastbmpbuf;













/* Local view: mainkey; return is void; another unit uses signed byte. */
extern void mainkey(I16);

/* Local view: flagsdown; type is signed word; another unit uses unsigned byte. */
extern I16 flagsdown;



/* Local view: resbuftext; type is signed byte; another unit uses array[80]. */
extern I8 resbuftext;


/* Local view: fntndat; local font import avoids introducing view-state declarations before their original block scopes. */
extern void far* fntndat;

/* Local view: main_data_file_addr; pointer is void; another unit uses signed byte. */
extern void far* main_data_file_addr;
/* Local view: cvxs_a; segment is far; another unit uses huge. */
extern struct GAMESTATE far* cvxs_a;








 //trackdata1;
 //trackdata2;
/* Local view: td3; pointer is signed byte; another unit uses signed word. */
extern I8 far* td3;
 //trackdata4;
 //trackdata5;
/* Local view: td6_ptr_b; pointer is signed byte; another unit uses unsigned word. */
extern I8 far* td6_ptr_b;
/* Local view: trackdat7; pointer is signed byte; another unit uses unsigned word. */
extern I8 far* trackdat7;
/* Local view: g_td08d; pointer is signed word; another unit uses unsigned word. */
extern I16 far* g_td08d; //trackdata8;
/* Local view: trkptrpath; pointer is struct VECTOR; another unit uses signed word. */
extern struct VECTOR far* trkptrpath;
/* Local view: td10checkptr; pointer is signed word; another unit uses struct VECTOR. */
extern int far* td10checkptr;// trackdata10;
/* Local view: hscore_trk11_ptr; pointer is signed byte; another unit uses struct HighScoreRecord. */
extern I8 far* hscore_trk11_ptr; //trackdata11;
/* Local view: savedptr_ms; pointer is signed byte; another unit uses struct SPRITE. */
extern I8 far* savedptr_ms;
 //trackdata13;
/* Local view: td14tb; pointer is unsigned byte; another unit uses signed byte. */
extern U8  far* td14tb; //trackdata14;
 //trackdata15;
 //trackdata16;
/* Local view: road_trk; pointer is signed byte; another unit uses unsigned byte. */
extern I8 far* road_trk; //trackdata17;
/* Local view: td_18_ref; pointer is signed byte; another unit uses unsigned byte. */
extern I8 far* td_18_ref;

 //trackdata20;
/* Local view: g_column_of_trkdata21_pth; pointer is signed byte; another unit uses unsigned byte. */
extern I8 far* g_column_of_trkdata21_pth; //trackdata21;
/* Local view: tdfrompathrow22; pointer is signed byte; another unit uses unsigned byte. */
extern I8 far* tdfrompathrow22; //trackdata22;
 // indexes into trkObjectList


/* Local view: g_is_busy; type is signed byte; another unit uses unsigned byte. */
extern I8 g_is_busy;


/* Local view: menutimeout; type is signed byte; another unit uses unsigned byte. */
extern I8 menutimeout;
/* Local view: dialog_frame_color; type is unsigned word; another unit uses signed word. */
extern U16S  dialog_frame_color;

/* Local view: endhsdemo; type is signed byte; another unit uses unsigned byte. */
extern I8 endhsdemo;













/* Local view: rate_frame; type is unsigned word; another unit uses signed word. */
extern U16S  rate_frame;



/* Local view: renderlevel; type is unsigned byte; another unit uses signed byte. */
extern U8  renderlevel;































/* Local view: is_audioloaded; type is signed word; another unit uses signed byte. */
extern I16S is_audioloaded;


 // = 'e'



/* Local view: material_clrlist_ptr; pointer is struct MATERIALCLRLIST; another unit uses signed word. */
extern struct MATERIALCLRLIST *material_clrlist_ptr;









/* Local view: gettlistpoint; return is signed word; another unit uses signed byte. */
extern I16S gettlistpoint(I16S car_trackdata3_index, struct VECTOR* car_vec_unk3, I16S field_CE, I16S* unk);


/* Local view: run_intro_looped; return is unsigned word; another unit uses signed word. */
extern U16S  run_intro_looped(void);
/* Local view: messagebox; return is unsigned word; another unit uses signed word. */
extern U16S  messagebox(I16 unk1, I16 unk2, void far* textresptr, U16S  unk3, U16S  unk4, I16 arg, void* unk5, I16 unk6);





/* Local view: selectcar; parameters[0].pointer is struct GAMEINFO; another unit uses signed byte. */
extern void selectcar(struct GAMEINFO* unk, I8* unk2, I8* unk3, U16  unk4);

/* Local view: end_hiscore; return is unsigned word; another unit uses signed byte. */
extern unsigned end_hiscore(void);
/* Local view: run_option_menu; return is unsigned word; another unit uses signed byte. */
extern unsigned run_option_menu(void);
/* Local view: getprotection; parameters.count is 0; another unit uses 1. */
extern void getprotection(void);


































/* Local view: initmouse; parameters[0] is signed word; another unit uses unsigned word. */
extern I16S initmouse(I16S a1, I16S a2);








/* Local view: sprite_blit_to_video; return is void; another unit uses signed word. */
extern void sprite_blit_to_video(struct SPRITE far* sprite);




/* Local view: timer_get_delta_alt; return is unsigned long; another unit uses signed word. */
extern U32  timer_get_delta_alt(void);



/* Local view: abortmessage; parameters.count is 2; another unit uses 1. */
extern void abortmessage(const I8*, ...);


/* Local view: _memcpy; return is near pointer; another unit uses void. */
extern void* _memcpy(void*, const void*, unsigned);














/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: SHAPE3D; struct.count is 7; another unit uses 8. */
struct SHAPE3D {
	U16S  shape3d_numverts;
	struct VECTOR far* shape3d_verts;
	U16S  shape3d_numprimitives;
	U16S  shape3d_numpaints;
	I8 far* shape3d_primitives;
	I8 far* shape3d_cull1;
	I8 far* shape3d_cull2;
};

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "shapehdr.h"


/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: TRANSFORMEDSHAPE3D; struct[4].type is unsigned word; another unit uses signed word. */
struct TRANSFORMEDSHAPE3D {
	struct VECTOR pos;
	struct SHAPE3D* shapeptr;
	struct RECTANGLE* rectptr;
	struct VECTOR rotvec;
	U16S  unk;
	U8  ts_flags;
	U8  material;
};


/* Local view: shape3d_load_all; return is signed word; another unit uses void. */
I16 shape3d_load_all(void);


/* Local view: ADD_OBJECT_TO_BUFFER; return is signed byte; another unit uses unsigned word. */
I8 ADD_OBJECT_TO_BUFFER(struct TRANSFORMEDSHAPE3D* arg_transshapeptr);


/* Local view: INIT_OBJECTS_BUFFER; return is unsigned word; another unit uses signed word. */
unsigned INIT_OBJECTS_BUFFER(I16 angZ, I16 angX, I16 angY, struct RECTANGLE* cliprect, I16 unk);



/* Local view: settires; parameters[5].pointer is struct VECTOR; another unit uses signed word. */
void settires(struct VECTOR far* wheel_vertices, I16 steering_angle, I16S* base_heights, I16S* cached_angles_and_heights, struct VECTOR* wheel_offsets, struct VECTOR* car_origin);
























/* Local view: test_pln; type is signed byte; another unit uses unsigned byte. */
extern I8 test_pln;

//extern struct TRANSFORMEDSHAPE3D transshapeunk;

/* Local view: trklst; of is struct TRACKOBJECT; another unit uses struct track_object. */
extern struct TRACKOBJECT trklst[215]; // 215 entries


         /* Fence offset pairs are grouped by the 1-, 2-, 3-, and 4-entry extents. */




      /* hill (x,z) offsets: 1,2,2,4 pairs at pairs 0,1,3,5; exactly fills [0x3C0A2,0x3C0C6) */



/* Local view: scene2; array is None; another unit uses 19. */
extern struct TRACKOBJECT scene2[];
/* Local view: scene3; array is None; another unit uses 13. */
extern struct TRACKOBJECT scene3[];







/* Local view: backlightovr8; type is signed byte; another unit uses unsigned byte. */
extern char backlightovr8;






/* Local view: dlg_colour; type is signed word; another unit uses unsigned word. */
extern I16 dlg_colour;




/* Local view: roadupcheck; parameters[0] is unsigned byte; another unit uses unsigned word. */
U8  roadupcheck(U8  a, U8  b);




/* Local view: shadowtext; return is near pointer; another unit uses void. */
struct RECTANGLE* shadowtext(I8* str, I16 a, I16 b, I16 c, I16 d);

/* Local view: fmtframestr; parameters[1] is signed word; another unit uses unsigned word. */
void fmtframestr(I8* s, I16 time, I16 c);






/* Local view: locate_text_resource; parameters[0].pointer is void; another unit uses signed byte. */
extern I8 far * far locate_text_resource(void far *data, I8 *name);



/* Local view: strlen; return is unsigned word; another unit uses signed word. */
extern unsigned strlen(I8 *s);



/* Local view: locate_shape_alt; return.pointer is signed byte; another unit uses void. */
extern I8 far *locate_shape_alt(I8 far *data, I8 *name);




/* Local view: sprite_putimage_and_alt; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void sprite_putimage_and_alt(void far *shape, I16 x, I16 y);










/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: SHAPE2D; struct.count is 6; another unit uses 10. */
struct SHAPE2D { I16S width, height, s2d_hotspot_x, s2d_hotspot_y, pos_x, pos_y; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "primrec.h"

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "resrec.h"

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ /* Local view: SPRITE; struct.count is 2; another unit uses 14. */
struct SPRITE { void far *image; U16S  words[13]; };
#include "audapi.h"
#include "auddrv.h"
#include "car.h"
#include "dlgapi.h"
#include "dlgcolor.h"
#include "gameapi.h"
#include "gfxapi.h"
#include "input.h"
#include "mathapi.h"
#include "memapi.h"
#include "render.h"
#include "replay.h"
#include "resapi.h"
#include "shape2d.h"
#include "timer.h"
#include "trig.h"
/* Local view: skybox_loaded; type is unsigned byte; another unit uses signed byte. */
extern unsigned char skybox_loaded;
/* Local view: initscenery; parameters[0] is signed byte; another unit uses unsigned byte. */
extern void far initscenery(I8);
#include "skybox.h"
#include "mathops.h"
#include "text.h"
#include "track.h"



/* Local view: loadedresourceptr; pointer is signed byte; another unit uses void. */
extern I8 far *loadedresourceptr;



/* Local view: fluffyload; return.pointer is void; another unit uses signed byte. */
extern void far *fluffyload(I8 *);
/* Local view: locate_many_resources; parameters[2].pointer.pointer is signed byte; another unit uses void. */
extern void far locate_many_resources(void far *, I8 *, I8 far **);
/* Local view: sprite_make_window; return.pointer is void; another unit uses struct SPRITE. */
extern void far *sprite_make_window(I16, I16, I16);

/* Local view: file_load_resource_file; return.pointer is signed byte; another unit uses void. */
extern I8 far *file_load_resource_file(I8 *);
/* Local view: setup_aero_trackdata; parameters[0].pointer is signed byte; another unit uses void. */
extern void far setup_aero_trackdata(I8 far *, I16);
/* Local view: closeresource; parameters[0].pointer is signed byte; another unit uses void. */
extern void far closeresource(I8 far *);

/* Local view: elapsedticks; return is signed word; another unit uses unsigned long. */
extern I16 far elapsedticks(void);






/* Local view: sprite_free_window; parameters[0].pointer is void; another unit uses struct SPRITE. */
extern void far sprite_free_window(void far *);
/* Local view: freememory; return is void; another unit uses far pointer. */
extern void far freememory(void far *);









/* Local view: TerName; array is None; another unit uses 5. */
extern I8 TerName[];
/* Local view: file_load_shape2d_fatal_thunk; return.pointer is void; another unit uses unsigned byte. */
extern void far *file_load_shape2d_fatal_thunk(I8 *);












/* Copies a clipped window region. Params: clip rectangle. Returns: none. State: reads the active window sprite and clip state. */
/* PLATFORM(video): draws to or configures the display surface. */
void far drawworldflipzones(struct RECTANGLE *clipRect)
{
	/* Local view: g_simprect; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern I8 g_simprect;
	
	
	
	/* Local view: rectclip; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern struct RECTANGLE rectclip[15];
	/* Local view: rcmapix; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern int rcmapix[45];
	/* Local view: rect_num3; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern char rect_num3;
	/* Local view: rects_updt; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern char rects_updt[15];
	/* Local view: makezonelist; parameters[0] is signed byte; another unit uses unsigned byte. */
	extern void far makezonelist(I8, I8 *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, I8 *, struct RECTANGLE *);
	
	/* Local view: sprputimage; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
	extern void far sprputimage(void far *);
	/* Local view: sprset1size; parameters[0] is unsigned word; another unit uses signed word. */
	extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
	/* Local view: g_wndspr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern struct SPRITE far *g_wndspr;
	/* Local view: rotpr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern I16 rotpr[];
	/* Local view: prevcamrot; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
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
            if (renderlevel == 4)
                rotpr[1] = prevcamrot;
            if (rotpr[1] == prevcamrot &&
                rclist[5].left == g_savrc[5].left &&
                rclist[5].right == g_savrc[5].right &&
                rclist[5].top == g_savrc[5].top &&
                rclist[5].bottom == g_savrc[5].bottom)
                rects_updt[5] = 0;
            rect_num3 = 0;
            makezonelist(15, rects_updt, rclist,
                g_savrc, clipRect, &rect_num3,
                rectclip);
            if (rect_num3 != 0) {
                sortzonelist(rect_num3,
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

/* Resets saved clip rectangles for the current drawing mode. Params: none. Returns: none. State: reads clip state and updates rectangle buffers. */ void far initdrawworld(void)
{
	/* Local view: full_screen_rectangle; block externs preserve external fixups to the later initialized public; a file declaration changes them to _DATA fixups. */
	extern struct RECTANGLE full_screen_rectangle;
    register I16 i;

    if (statemgmtcpy != 0) {
        rclist[0] = full_screen_rectangle;
        g_savrc[0] = full_screen_rectangle;
        for (i = 1; i < 15; ++i) {
            rclist[i] = empty_rectangle;
            g_savrc[i] = empty_rectangle;
        }
    }
}
/* Updates the visible game frame. Params: page and clip rectangle. Returns: none. State: reads frame, scene, camera and sprite globals; updates render lists. */
/* PLATFORM(video): draws to or configures the display surface. */
void drawworld(I8 page, struct RECTANGLE* clip) {
	/* Local view: sprset1size; parameters[0] is unsigned word; another unit uses signed word. */
	extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
	/* Local view: rotpr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
	extern I16 rotpr[];
	/* Local view: prevcamrot; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
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
		active_ui_dirty_rect = rclist;
		rectp = g_savrc;
	} else {
		rectp = rclist;
		active_ui_dirty_rect = g_savrc;
	}

	if (statemgmtcpy != 0) {
		flagbits = 8;
		rect_ptr = tparr;
		for (si = 0; si < 15; si++) {
			*rect_ptr++ = empty_rectangle;
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
		car_matrix = makematrix(-my_car_bank, -car_angle_y, -rot_x, 0);
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		si = intatan(rot_ofs.x, rot_ofs.z);
		offset_v.x = 0;
		offset_v.y = 0;
		offset_v.z = camlen;
		car_matrix = makematrix(0, -custom_elev_angle, si - custom_azim_angle, 0);
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		viewpoint.x = car_coord.x + rot_ofs.x;
		viewpoint.y = car_coord.y + rot_ofs.y;
		viewpoint.z = car_coord.z + rot_ofs.z;
		break;
	case 1:
		viewpoint.x = core.game_chase_camera_pos[follow_op].x;
		viewpoint.z = core.game_chase_camera_pos[follow_op].z;
		viewpoint.y = core.game_chase_camera_pos[follow_op].y;
		break;
	case 0:
		view_rot_x = rot_x & FAST_TRIG_ANGLE_MASK;
		cam_elev = car_angle_y & FAST_TRIG_ANGLE_MASK;
		cam_bank = my_car_bank & FAST_TRIG_ANGLE_MASK;
		car_matrix = makematrix(-my_car_bank, -car_angle_y, -rot_x, 0);
		offset_v.x = 0;
		offset_v.z = 0;
		offset_v.y = simdp7.car_height - 6;
		mat_vec(&offset_v, car_matrix, &rot_ofs);
		viewpoint.x = car_coord.x + rot_ofs.x;
		viewpoint.y = car_coord.y + rot_ofs.y;
		viewpoint.z = car_coord.z + rot_ofs.z;
		break;
	case 3:
		viewpoint.x = trkptrpath[core.game_track_camera_index[follow_op]].x;
		viewpoint.y = trkptrpath[core.game_track_camera_index[follow_op]].y + viewyshift + 0x5A;
		viewpoint.z = trkptrpath[core.game_track_camera_index[follow_op]].z;
		break;
	}

	// Unknown part; seems to be performing some initialization
	if (view_rot_x == -1) {
		getpinfo(&viewpoint, &viewpoint);
		if (viewpoint.y < hgthgt) {
			viewpoint.y = hgthgt;
		}

		if (test_pln != 0) {		
			si = plane_distance(pl_i, viewpoint.x, viewpoint.y, viewpoint.z);
			if (si < 0xC) {			
				tvec2.x = 0;
				tvec2.y = 0xC - si;
				tvec2.z = 0;
				g_planidx2 = pl_i;
				frwhl_angadjusted = 0;
				car_roty_pln = 0;
				pln_rotate_z = 0;
				car_rotate_xc = 0;
				move_on_plane();
				viewpoint.x += pln_rot_output.x;
				viewpoint.y += pln_rot_output.y;
				viewpoint.z += pln_rot_output.z;
			}
		}

		view_rot_x = (-intatan(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z)) & FAST_TRIG_ANGLE_MASK;
		ground_dist = inthypot(car_coord.x - viewpoint.x, car_coord.z - viewpoint.z);
		cam_elev = intatan(car_coord.y - viewpoint.y + 0x32, ground_dist) & FAST_TRIG_ANGLE_MASK;
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

	cur_view_heading_val = INIT_OBJECTS_BUFFER(bank, cam_elev, view_rot_x, clip, 0);
	si = (cur_view_heading_val & FAST_TRIG_ANGLE_MASK) >> 7;
	tile_table = headingzones[si];

	view_mat = *makematrix(bank, cam_elev, 0, 1);
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
	if (renderlevel == 0) {
		cur_shps->rectptr = &tparr[7];
		cur_shps->ts_flags = flagbits | 7;
		cur_shps->rotvec.x = 0;
		cur_shps->rotvec.y = 0;
		cur_shps->unk = 0x400;
		cur_shps->material = 0;

		for (nfences = 0; nfences < 8; nfences++) {
			si = (fence_offsets[nfences] + view_rot_x + runrndx) & FAST_TRIG_ANGLE_MASK;
			if (si < 0x87 || si > 0x379) {
				xformy(&sky_mat, si);
				offset_v.x = 0;
				offset_v.y = 0xAE6 - viewpoint.y;
				offset_v.z = 0x3A98; //15000
				mat_vec(&offset_v, &sky_mat, &rot_ofs);
				rot_ofs.z = 0x3A98; //15000
				mat_vec(&rot_ofs, &view_mat, &cur_shps->pos);
				if (cur_shps->pos.z > PLATFORM_SCREEN_HEIGHT_PIXELS) {
					cur_shps->shapeptr = fence_shapes[nfences];
					cur_shps->rotvec.z = -view_rot_x;
					result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
					/* We cannot be out of memory as we are just starting to process. */
				}
			}
		}
	}

/*
; -----------------------------------------------------------------------------------------------
*/

	cam_tx = viewpoint.x >> 0xA;
	cam_tz = -((viewpoint.z >> 0xA) - 0x1D);
	if (renderlevel != 0) {
		car_x_tile = core.playerstate.car_posWorld1.lx >> 16;
		car_z_tile = 0x1D - (core.playerstate.car_posWorld1.lz >> 16);
	}

	for (si = 0; si < 0x17; si++) {
		skipped[si] = 0;
	}

	// Select the detail level (FULL if 1st or 2nd option in the graphics menu
	// were chosen, MEDIUM if the 3rd, FASTEST if 4th or 5th)
	thresh = detthrlevel[renderlevel];
	
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
							track_elem = roadupcheck(tile_ground, track_elem);
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
	
					if (track_elem != 0 && renderlevel != 0 &&
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

			car_matrix = makematrix(-core.playerstate.car_rotate.z, -core.playerstate.car_rotate.y, -core.playerstate.car_rotate.x, 0);
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
				car_matrix = makematrix(-core.opponentstate.car_rotate.z, -core.opponentstate.car_rotate.y, -core.opponentstate.car_rotate.x, 0);
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

			if (renderlevel != 0 && (tile_east2 != car_x_tile || drow != car_z_tile))
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
				result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
				if (result > 0)
					goto draw_done;
			}
		
		}

		// terrain type 0x06: a flat piece of land at an elevated level
		if (tile_ground == 6) {
			hill_elev = terheight[1];
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
						result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
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
			result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
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
					result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
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
						result = ADD_OBJECT_TO_BUFFER(&cur_shps[1]);
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
				result = ADD_OBJECT_TO_BUFFER(&cur_shps[0]);
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
				if (core.game_track_object_broken[checkpoint_no] != 0) {
					if (core.game_debris_active != 0) {
						for (j2 = 0; j2 < 0x18; j2++) {
							if (core.game_debris_planar_step[j2] != 0) if (checkpoint_no + 2 == core.game_debris_origin_kind[j2]) {
								trkobj = &scene3[core.game_debris_shape_index[j2]];
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_debris_x[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 0] - viewpoint.x;
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_debris_y[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 1] - viewpoint.y;
								/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_debris_z[j2] >> WORLD_COORDINATE_SHIFT) + td10checkptr[checkpoint_no * 3 + 2] - viewpoint.z;
								g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
								g_curr_tsp->rectptr = &tparr[2];
								g_curr_tsp->ts_flags = flagbits | 5;
								g_curr_tsp->rotvec.x = -core.game_debris_rot_x[j2];
								g_curr_tsp->rotvec.y = -core.game_debris_rot_y[j2];
								g_curr_tsp->rotvec.z = -core.game_debris_rot_z[j2];
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
			if (core.game_debris_active != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (core.game_debris_planar_step[j2] != 0) if (core.game_debris_origin_kind[j2] == 0) {
						trkobj = &scene3[core.game_debris_shape_index[j2]];
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_debris_x[j2] + core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_debris_y[j2] + core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
						/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_debris_z[j2] + core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;
						g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
						g_curr_tsp->rectptr = &tparr[2];
						g_curr_tsp->ts_flags = flagbits | 5;
						g_curr_tsp->rotvec.x = -core.game_debris_rot_x[j2];
						g_curr_tsp->rotvec.y = -core.game_debris_rot_y[j2];
						g_curr_tsp->rotvec.z = -core.game_debris_rot_z[j2];
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
			
			if (tile_lod != 0 || renderlevel > 2) {
				g_curr_tsp->shapeptr = trkobj->ss_loShapePtr;
			} else {
				g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
				settires(&g_shapes3d[0x0AD4 / sizeof(struct SHAPE3D)].shape3d_verts[8], core.playerstate.car_steeringAngle, &core.playerstate.car_wheel_suspension_offset, ywhlang, pts_set, &pos_pt);
			}

			if (statemgmtcpy != 0) {
				g_curr_tsp->rectptr = &tparr[3];
				g_curr_tsp->ts_flags = 0xC;
			} else if (core.playerstate.car_crashBmpFlag == 1) {
				cliprect1 = empty_rectangle;
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
			if (core.game_debris_active != 0) {
				for (j2 = 0; j2 < 0x18; j2++) {
					if (core.game_debris_planar_step[j2] != 0) {
						if (core.game_debris_origin_kind[j2] == 1) {
							trkobj = &scene3[core.game_debris_shape_index[j2]];
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.x = (core.game_debris_x[j2] + core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - viewpoint.x;
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.y = (core.game_debris_y[j2] + core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - viewpoint.y;
							/* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ g_curr_tsp->pos.z = (core.game_debris_z[j2] + core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - viewpoint.z;
							g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
							g_curr_tsp->rectptr = &tparr[2];
							g_curr_tsp->ts_flags = flagbits | 5;
							g_curr_tsp->rotvec.x = -core.game_debris_rot_x[j2];
							g_curr_tsp->rotvec.y = -core.game_debris_rot_y[j2];
							g_curr_tsp->rotvec.z = -core.game_debris_rot_z[j2];
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

			if (tile_lod != 0 || renderlevel > 2) {
				g_curr_tsp->shapeptr = trkobj->ss_loShapePtr;
			} else {
				g_curr_tsp->shapeptr = trkobj->ss_shapePtr;
				settires(&g_shapes3d[0x0AEA / sizeof(struct SHAPE3D)].shape3d_verts[8], core.opponentstate.car_steeringAngle, &core.opponentstate.car_wheel_suspension_offset, buf_obase, veco, &ctrmesh);
			}

			if (statemgmtcpy != 0) {
				g_curr_tsp->rectptr = &tparr[4];
				g_curr_tsp->ts_flags = 0xC;
			} else if (core.opponentstate.car_crashBmpFlag == 1) {
				cliprect2 = empty_rectangle;
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

				j = fixedmult(intcos(g_sgateopn), 0x24);
				nfences = fixedmult(intsin(g_sgateopn), 0x24) + 0x38;

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
					fixedmult(intsin(st_hdg + 0x200), 0x1B6) +
					fixedmult(intsin(st_hdg + 0x100), 0x24) + 
					trackctrpos2[idxtrk] - viewpoint.x;
				g_curr_tsp->pos.y = terheight[g_hillf] - viewpoint.y;
				g_curr_tsp->pos.z =
					fixedmult(intcos(st_hdg + 0x200), 0x1B6) +
					fixedmult(intcos(st_hdg + 0x100), 0x24) + 
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
				shellsort(g_ts_num, g_tdist, tsix);
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

				result = ADD_OBJECT_TO_BUFFER(&cur_shps[j2]);
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
	skybox_result = drawhorizon(page, clip, cur_sky_sign, &view_mat, bank, view_rot_x, viewpoint.y);
	/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, clip->top, clip->bottom);
	DRAW_OBJECTS();

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

		if (clipzone(rect_ptr, clip) == 0) {
			/* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rect_ptr->left, rect_ptr->right, rect_ptr->top, rect_ptr->bottom);
			offset_v.x = (rect_ptr->right + rect_ptr->left) >> 1;
			offset_v.y = (rect_ptr->top + rect_ptr->bottom) >> 1;
			j = rect_ptr->right - rect_ptr->left;
			nfences = rect_ptr->bottom - rect_ptr->top;
			if (nfences > j) {
				j = nfences;
			}

			j2 = (core.game_frame >> 2) % 3 ;
			nfences = ((I32)j << 8) / exwd[j2];
			scalecfxy(nfences, sdgbmp_v[j2], offset_v.x, offset_v.y);
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
				mergezones(crack(core.game_frame - si, clip->top, clip->bottom - clip->top), tparr, tparr);
			} else {
				crack(core.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		} else if (followed->car_crashBmpFlag == 2) {
			if (statemgmtcpy != 0) {
				mergezones(sink(core.game_frame - si, clip->top, clip->bottom - clip->top), tparr, tparr);
			} else {
				sink(core.game_frame - si, clip->top, clip->bottom - clip->top);
			}
		}
	}

	// Show elapsed time
	if (gm_playmode == 0) {
		if (core.game_inputmode != 0) {
			fmtframestr(&resbuftext, elaptm1 + tmr2, 0);
			/* PLATFORM(video): select a font definition. */ fontsetfontdef2(fntled_res);
			if (statemgmtcpy != 0) {
				/* PLATFORM(video): draw the introduction text. */ mergezones(shadowtext(&resbuftext, 0x8C, rfy5 + 2, dlg_colour, 0), &tparr[6], &tparr[6]);
			} else {
				/* PLATFORM(video): draw the introduction text. */ shadowtext(&resbuftext, 0x8C, rfy5 + 2, dlg_colour, 0);
			}

			/* PLATFORM(video): select the active font definition. */ fontsetfontdef();
		}
	}

	if (statemgmtcpy != 0) {
		mergezones(drawtext(), tparr, tparr);
		if (skybox_result != 0) {
			tparr[0] = *clip;
			for (si = 1; si < 15; si++) {
				tparr[si] = empty_rectangle;
			}
		}

		for (si = 0; si < 15; si++) {
			rectp[si] = tparr[si];
		}
		rotpr[page] = view_rot_x;
		prevcamrot = view_rot_x;

	} else {
		drawtext();
	}

}

/* Local view: g_simprect; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I8 g_simprect;



/* Local view: rectclip; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern struct RECTANGLE rectclip[15];
/* Local view: rcmapix; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I16 rcmapix[45];
/* Local view: rect_num3; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I8 rect_num3;
/* Local view: rects_updt; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I8 rects_updt[15];
/* Local view: full_screen_rectangle; block externs preserve external fixups to the later initialized public; a file declaration changes them to _DATA fixups. */
extern struct RECTANGLE full_screen_rectangle;
/* Local view: makezonelist; parameters[0] is signed byte; another unit uses unsigned byte. */
extern void far makezonelist(I8, I8 *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, I8 *, struct RECTANGLE *);

/* Local view: sprputimage; parameters[0].pointer is void; another unit uses struct SHAPE2D. */
extern void far sprputimage(void far *);
/* Local view: sprset1size; parameters[0] is unsigned word; another unit uses signed word. */
extern void sprset1size(U16S  left, U16S  right, U16S  top, U16S  height);
/* Local view: g_wndspr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern struct SPRITE far *g_wndspr;
/* Local view: rotpr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I16 rotpr[];
/* Local view: prevcamrot; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
extern I16 prevcamrot;

/* Draws the sky and ground bands. Params: clip rectangle, x offset and horizon. Returns: none. State: reads skybox images, heights and colors. */
/* PLATFORM(video): draws to or configures the display surface. */
void far skybox_op_helper2(struct RECTANGLE *rectptr, I16 x, I16 horizon)
{
    {
        register I16 upper_height;
        if (renderlevel != 4)
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
        if (renderlevel != 4) {
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
I16 drawhorizon(I16 preview_index, struct RECTANGLE *clip, I16 latitude,
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

        projectp(&vecs[0], &pts[0]);
        projectp(&vecs[1], &pts[1]);

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
        if (renderlevel != 4 && pts[1].px < 0 && pts[0].px > PLATFORM_SCREEN_WIDTH_PIXELS &&
            clipline(pts[1].px, pts[1].py,
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
                    if (!clipzone(&rc, clip)) {
                        rect_num3 = 0;
                        makezonelist(15, rects_updt, rectp, tparr,
                                           &rc, &rect_num3, rectclip);
                        for (temp = 0; temp < rect_num3; temp++) {
                            rectptr = &rectclip[temp];
                            /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(rectptr->left, rectptr->right, rectptr->top, rectptr->bottom);
                            /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skybox_sky_clr);
                        }
                    }

                    rc.top = tparr[5].bottom;
                    rc.bottom = PLATFORM_SCREEN_HEIGHT_PIXELS;
                    if (!clipzone(&rc, clip)) {
                        rect_num3 = 0;
                        makezonelist(15, rects_updt, rectp, tparr,
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
            if (!clipzone(&rc, clip)) {
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
            i = intatan(pts[0].px - pts[1].px, pts[0].py - pts[1].py) & FAST_TRIG_ANGLE_MASK;
            for (slope = 2; slope < 6; slope++) {
                if (slope < 4)
                    temp = 0;
                else
                    temp = 1;
                pts[slope].px = pts[temp].px + fixedmult(0x3e80, intsin(horizon_angles[slope - 2] + i));
                pts[slope].py = pts[temp].py + fixedmult(0x3e80, intcos(horizon_angles[slope - 2] + i));
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

        projectp(&vecs[0], &pts[0]);
        horizon = pts[0].py;
        if (clip->top > horizon)
            horizon = clip->top;

        if (latitude == 1) {
            if (statemgmtcpy == 0)
                goto simple;
            tparr[5].top = (renderlevel == 4) ? horizon - 1 : horizon - maxscnh;
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
                if (renderlevel == 4)
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
                makezonelist(15, rects_updt, rectp, tparr,
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
	g_tdist[g_ts_num] = transformedpos.z + zadjust;
	tshapearrarg2[g_ts_num] = sort_group_id;
	tsix[g_ts_num] = g_ts_num;
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

    camera_distance = inthypot(camera_aim_x - camera_pos_x,
                                    camera_aim_z - camera_pos_z);
    rot_angle = intatan(camera_aim_y - camera_pos_y, camera_distance);
    cam_matrix = makematrix(0, rot_angle, 0, 1);
    mat_vec(&track_prev_vec, cam_matrix, &vector);
    projectp(&vector, &screen_point);

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
    INIT_OBJECTS_BUFFER(0, rot_angle, 0, &trackpreview_cliprect, 1);

    transformed.rotvec.x = 0;
    transformed.rotvec.y = 0;
    transformed.unk = 0x400;

    for (row_idx = 0; row_idx < 30; row_idx++) {
        for (tile_col = 0; tile_col < 30; tile_col++) {
            elem = td14tb[lnoffsets[row_idx] + tile_col];
            terr = td15p_9[gterrtrk[row_idx] + tile_col];
            if (elem != 0) {
                if (terr >= 7 && terr < 11) {
                    elem = roadupcheck(terr, elem);
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
                obj_height = terheight[1];
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
                            ADD_OBJECT_TO_BUFFER(&transformed);
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
                ADD_OBJECT_TO_BUFFER(&transformed);
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
                    ADD_OBJECT_TO_BUFFER(&transformed);
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
                        ADD_OBJECT_TO_BUFFER(&transformed);
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
                ADD_OBJECT_TO_BUFFER(&transformed);
            }
            DRAW_OBJECTS();
        }
    }
}

/* Draws the in-game text overlay. Params: none. Returns: the affected rectangle. State: reads and updates text and game-display state. */
/* PLATFORM(video): draws to or configures the display surface. */
struct RECTANGLE *drawtext(void)
{
    register I16 remainder;

    game_rect_txt_in = empty_rectangle;

    if (menutimeout != 0) {
        strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aDm1));
        /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
            shadowtext(&resbuftext, mycenter(&resbuftext), 0xAA, dlg_colour, 0),
            &game_rect_txt_in);

        strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aDm2));
        /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
            shadowtext(&resbuftext, mycenter(&resbuftext), 0xB6, dlg_colour, 0),
            &game_rect_txt_in);
    } else if (gm_playmode == 0) {
        if (core.game_inputmode == 0) {
            strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aPre));
            /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                shadowtext(&resbuftext, mycenter(&resbuftext), 0x5A, dlg_colour, 0),
                &game_rect_txt_in);
        } else if (pass_check_flag == 0) {
            strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aSe1));
            /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                shadowtext(&resbuftext, mycenter(&resbuftext), 0x5D, dlg_colour, 0),
                &game_rect_txt_in);
            strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aSe2));
            /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                shadowtext(&resbuftext, mycenter(&resbuftext), 0x69, dlg_colour, 0),
                &game_rect_txt_in);
        } else if (follow_op == 0 && cammd == 0 && core.playerstate.car_crashBmpFlag == 0) {
            switch (core.game_player_guidance) {
            /* PLATFORM(video): blit a transparent shape. */ case 1:
                sprite_putimage_transparent(sdgbmp_v[3], 0x94, 0x5D);
                mergezones(&game_rect_txt_in, &rect_ingame_text2, &game_rect_txt_in);
                break;
            /* PLATFORM(video): blit a transparent shape. */ case 2:
                sprite_putimage_transparent(sdgbmp_v[4], 0x94, 0x5D);
                mergezones(&game_rect_txt_in, &rect_ingame_text2, &game_rect_txt_in);
                break;
            case 3:
                strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aWww));
                /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                    shadowtext(&resbuftext, mycenter(&resbuftext), 0x5D, dlg_colour, 0),
                    &game_rect_txt_in);
                break;
            }

            resbuftext = 0;
            switch (core.game_opponent_pass_side) {
            /* PLATFORM(video): blit a transparent shape. */ case 1:
                sprite_putimage_transparent(sdgbmp_v[3], 0x44, 0x71);
                mergezones(&game_rect_txt_in, &rect_ingame_text3, &game_rect_txt_in);
                strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aOpp));
                break;
            /* PLATFORM(video): blit a transparent shape. */ case 2:
                sprite_putimage_transparent(sdgbmp_v[4], 0xE4, 0x71);
                mergezones(&game_rect_txt_in, &rect_ingame_text4, &game_rect_txt_in);
                strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aOpp_0));
                break;
            }

            if (resbuftext != 0) {
                /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                    shadowtext(&resbuftext, mycenter(&resbuftext), 0x74, dlg_colour, 0),
                    &game_rect_txt_in);
            }

            if (pen_flag_count != 0) {
                strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aPen));
                fmtframestr(&resbuftext + strlen(&resbuftext), g_penaltytm, 0);
                /* PLATFORM(video): use the display text or sprite interface. */ mergezones(&game_rect_txt_in,
                    shadowtext(&resbuftext, mycenter(&resbuftext), 0x66, dlg_colour, 0),
                    &game_rect_txt_in);
            }
        }
    } else if (gm_playmode == 2) {
        remainder = core.game_frame % rate_frame;
        if (remainder < ((I16S)rate_frame >> 1)) {
            strcpyfar(&resbuftext, locate_text_resource(gamerptrs, aRpl_0));
            /* PLATFORM(video): draw the introduction text. */ mergezones(&game_rect_txt_in,
                shadowtext(&resbuftext, 0x138 - (strlen(&resbuftext) << 3), 0x0F, dlg_colour, 0),
                &game_rect_txt_in);
        }
    }

    return &game_rect_txt_in;
}

/* Updates the sinking message animation. Params: frame, top and height. Returns: affected rectangle. State: reads and updates message-rectangle state. */
/* PLATFORM(video): draws to or configures the display surface. */
struct RECTANGLE *sink(I16 frame, I16 top, I16 height)
{
    register I16 offset;

    if (frame > (I16)rate_frame * 4)
        frame = (I16)rate_frame * 4;

    offset = ((I32)height * frame) / ((I16)rate_frame * 4);

    game_rect_txt_in.left = 0;
    game_rect_txt_in.right = PLATFORM_SCREEN_WIDTH_PIXELS;
    game_rect_txt_in.top = top + height - offset;
    game_rect_txt_in.bottom = top + height;

    /* PLATFORM(video): select the sprite clip rectangle. */ sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, game_rect_txt_in.top, game_rect_txt_in.bottom);
    /* PLATFORM(video): fill the active clip with a color. */ sprite_clear_1_color(g_skyboxwat_clr);
    return &game_rect_txt_in;
}

/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ 
#include "gamerec.h"

I16 fence_offsets[8] = { 30, 200, 320, 400, 530, 700, 880, 960 };
struct SHAPE3D *fence_shapes[8] = {
    &g_shapes3d[48],
    &g_shapes3d[47],
    &g_shapes3d[46],
    &g_shapes3d[48],
    &g_shapes3d[47],
    &g_shapes3d[46],
    &g_shapes3d[48],
    &g_shapes3d[47],
};

static struct LOOKAHEAD_TILE hzoneNW[23] = {
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

static struct LOOKAHEAD_TILE hzoneNE[23] = {
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

static struct LOOKAHEAD_TILE hzoneEN[23] = {
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

static struct LOOKAHEAD_TILE hzoneES[23] = {
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

static struct LOOKAHEAD_TILE hzoneSE[23] = {
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

static struct LOOKAHEAD_TILE hzoneSW[23] = {
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

static struct LOOKAHEAD_TILE hzoneWS[23] = {
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

static struct LOOKAHEAD_TILE hzoneWN[23] = {
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

I8 *headingzones[8] = { (I8 *)hzoneNE, (I8 *)hzoneEN, (I8 *)hzoneES, (I8 *)hzoneSE, (I8 *)hzoneSW, (I8 *)hzoneWS, (I8 *)hzoneWN, (I8 *)hzoneNW };
/* Local view: full_screen_rectangle; block externs preserve external fixups to the later initialized public; a file declaration changes them to _DATA fixups. */
struct RECTANGLE full_screen_rectangle = { 0, 320, 0, 200 };
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
/* PLATFORM(file): resolves crash-shape data inside a loaded resource. */ struct RECTANGLE *crack(I16 frame, I16 top, I16 height)
{
    /* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */ struct CRACK_LINE { struct POINT2D start, end; };
    struct CRACK_LINE far *crak_shape;
    I16 far *cinf_shape;
    I16 animation_counter;
    I16 total_lines;
    struct POINT2D start_point, crack_end, adjust_point;
    register I16 crackIndex;

    /* PORT: crash-shape data uses a segmented FAR resource pointer. */ /* PLATFORM(file): resolve crash-line data inside the loaded bundle. */ crak_shape = locate_shape_alt(gamerptrs, init_crak_resource_names[0]);
    /* PORT: animation metadata uses a segmented FAR resource pointer. */ /* PLATFORM(file): resolve crash-animation data inside the loaded bundle. */ cinf_shape = locate_shape_alt(gamerptrs, init_crak_resource_names[1]);

    animation_counter = frame / ((I16)rate_frame / 7);
    if (animation_counter >= cinf_shape[0])
        animation_counter = cinf_shape[0] - 1;
    total_lines = cinf_shape[animation_counter + 1];

    game_rect_txt_in = empty_rectangle;
    for (crackIndex = 0; crackIndex < total_lines; ++crackIndex) {
            start_point = crak_shape[crackIndex].start;
            crack_end = crak_shape[crackIndex].end;

        start_point.py = ((I32)start_point.py * height) / 200;
        crack_end.py = ((I32)crack_end.py * height) / 200;

        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top - 1,
                       crack_end.px, crack_end.py + top - 1, 0);
        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top + 1,
                       crack_end.px, crack_end.py + top + 1, 0);
        /* PLATFORM(video): draw a preview line. */ preRender_line(start_point.px, start_point.py + top,
                       crack_end.px, crack_end.py + top, dlg_colour);

        if (statemgmtcpy != 0) {
            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top - 1;
            zonecheck(&adjust_point, &game_rect_txt_in);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top + 1;
            zonecheck(&adjust_point, &game_rect_txt_in);

            adjust_point.px = start_point.px;
            adjust_point.py = start_point.py + top + 1;
            zonecheck(&adjust_point, &game_rect_txt_in);

            adjust_point.px = crack_end.px;
            adjust_point.py = crack_end.py + top - 1;
            zonecheck(&adjust_point, &game_rect_txt_in);
        }
    }

    return &game_rect_txt_in;
}



/* Loads the selected skybox. Params: skybox mode. Returns: none. State: updates the skybox resource handle and loaded-state flag. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* Local view: initscenery; parameters[0] is signed byte; another unit uses unsigned byte. */
void far initscenery(I8 mode)
{
    register I16 skyHeight;
    struct MATERIALCLRLIST *materials;

    if (mode & 8)
        mode &= 7;
    else if (skybox_loaded != 0 && mode == scene_idx)
        return;
    else {
        removescenery();
        scene_idx = mode;
        skybox_loaded = 1;
        /* PLATFORM(file): load the selected skybox resource. */ skyres_handle.pointer = file_load_shape2d_fatal_thunk(TerName + 9 * mode);
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
void far removescenery(void)
{
    /* PLATFORM(memory): release resource memory. */ if (skybox_loaded != 0)
        freememory(skyres_handle.pointer);
    skybox_loaded = 0;
}

/* Loads the SD game shape set. Params: none. Returns: none. State: stores the loaded resource handle. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
void far loaddrawbits(void)
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
void far removedrawbits(void) { /* PLATFORM(memory): release resource memory. */ freememory(sdgame2hdl); }

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
    /* PLATFORM(file): load a 3D resource. */ title3dresValue = fluffyload("title");
    locate_many_resources(title3dresValue, "logolog2brav", lastShapeResources);
    makeobjdef(lastShapeResources[0], &intro_alt);
    makeobjdef(lastShapeResources[1], &logo_title);
    makeobjdef(lastShapeResources[2], &brav);
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
    /* PLATFORM(memory): release a loaded resource. */ closeresource(otherCarResource);
    initlogo();
    /* PLATFORM(timer): read the elapsed timer interval. */ elapsedticks();
    pointTotalB = 0;
    activePointCountA = 0;
    statemgmtcpy = slow_video_mode_state;
    tparr[0].left = 0;
    tparr[0].right = PLATFORM_SCREEN_WIDTH_PIXELS;
    tparr[0].top = 0;
    tparr[0].bottom = PLATFORM_SCREEN_HEIGHT_PIXELS;
    tparr[1] = tparr[0];
    rc0_cpy = tparr[0];
    savedRectangleIndex = 0;
    frameReady = 1;

    do {
        /* PLATFORM(timer): read the elapsed timer interval. */ baseFrameDelta = elapsedticks();
        frmexcess += baseFrameDelta;
        while (frmexcess > frmcs_time) {
            frmexcess -= frmcs_time;
            movelogo();
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
        if (frameReady) {
            frameReady = 0;
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            /* PLATFORM(video): copy the window sprite to the display page. */ else
                sprite_copy_wnd_to_1();

            carHeadingData = 0xffff;
            oldDrawOpponent = 1;
            /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ opponentXPosition = (core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT);
            /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ lastOpponentY = (core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT);
            /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ savedOpponentZ = (core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT);
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
                carHeadingData = (-intatan(goalXPosition - oldCameraX, savedGoalZ - cameraZPrevious)) & 0x03ff;
                oldCarDistance = inthypot(goalXPosition - oldCameraX, savedGoalZ - cameraZPrevious);
                introCloudTilt = intatan(lastGoalY - cameraYIdle, oldCarDistance) & 0x03ff;
            }

            if (statemgmtcpy != 0) {
                if (savedRectangleIndex == 0) {
                    currentPoints = (I16 *)gamePointBufferB;
                    drawSelectedCount = &pointTotalB;
                } else {
                    currentPoints = (I16 *)basePointBufferA;
                    drawSelectedCount = &activePointCountA;
                }
            }
            drawlogo(oldCameraX, cameraYIdle, cameraZPrevious, carHeadingData, introCloudTilt,
                     oldDrawOpponent, oldPhase, cloudPointsList,
                     (struct POINT2D *)currentPoints, drawSelectedCount,
                     tparr[savedRectangleIndex], &gameRestoredRect, &lastDrawRect);

            if (g_videoflg5 != 0) {
                /* PLATFORM(video): select the opaque drawing path. */ msdrawopaquechk();
                setup_mcgawnd1();
                /* PLATFORM(video): select the transparent drawing path. */ msdrawtransparentchk();
                if (statemgmtcpy != 0)
                    tparr[savedRectangleIndex] = gameRestoredRect;
                savedRectangleIndex ^= 1;
            } else {
                /* PLATFORM(video): copy the working sprite pages. */ sprcopy2to12();
                if (statemgmtcpy != 0) {
                    mergezones(&lastDrawRect, &tparr[2], &oldSavedRect);
                    if (clipzone(&oldSavedRect, &rc0_cpy) == 0) {
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

        }
/* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ clear_keyboardbuffer: /* MSC 5.10: deleting this unused label changes COMDEF order. */
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
    /* PLATFORM(memory): release resource memory. */ freememory(title3dresValue);
    return operationResult;
}

/* Draws the introduction scene. Params: camera, projection and clip data from the signature. Returns: none. State: reads intro-scene and sprite state. */
/* PLATFORM(video): draws to or configures the display surface. */
void far drawlogo(I16 camX, I16 camY, I16 camZ, I16 logoRotation,
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

    initialClip = empty_rectangle;
    INIT_OBJECTS_BUFFER(0, cloudRotation, logoRotation, &intro_cliprect, 0);
    if (useLogo)
        modelShape.shape = &intro_alt;
    else
        modelShape.shape = &logo_title;
    modelShape.pos.x = 0x400 - camX;
    modelShape.pos.y = -camY;
    modelShape.pos.z = 0x400 - camZ;
    if (statemgmtcpy != 0) {
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
    ADD_OBJECT_TO_BUFFER(&modelShape);
    if (showOpponent) {
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.x = (I16)(core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SHIFT) - camX;
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.y = (I16)(core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SHIFT) - camY;
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */ modelShape.pos.z = (I16)(core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SHIFT) - camZ;
        modelShape.shape = &brav;
        if (statemgmtcpy != 0) {
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
        ADD_OBJECT_TO_BUFFER(&modelShape);
    }
logo_complete: /* MSC 5.10: deleting this unused label changes COMDEF order. */
    if (statemgmtcpy == 0)
        goto no_incremental;
    if (*oldPointCount != 0) {
        index = 0;
        while (index < *oldPointCount) {
            projectedPoint = oldPoints[index];
            /* PLATFORM(video): draw a display pixel. */ putpixel_single_maybe(projectedPoint.px, projectedPoint.py, 0);
            ++index;
        }
    }
    mergezones(oldClip, &inputClip, &unionRect);
    if (clipzone(&unionRect, &rc0_cpy) == 0) {
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
            projectp(&relativeVector, &projectedPoint);
            /* PLATFORM(video): draw a display pixel. */ putpixel_single_maybe(projectedPoint.px, projectedPoint.py,
                                  intro_colorvalue);
            if (statemgmtcpy != 0) {
                oldPoints[count++] = projectedPoint;
                zonecheck(&projectedPoint, &previousClip);
            }
            ++intro_colorvalue;
            if (intro_colorvalue == intro_color_max)
                intro_colorvalue = 1;
        }
        ++index;
    } while (index < 100);
    if (statemgmtcpy != 0)
        *oldPointCount = count;
    DRAW_OBJECTS();
    if (statemgmtcpy != 0) {
        *oldClip = initialClip;
        *oldUnion = previousClip;
    }
}

struct RECTANGLE intro_cliprect = { 0, 320, 0, 200 };
I16 intro_colorvalue = 1;

/* Communals defined by this module (tentative definitions). */
/* Local view: td10checkptr; pointer is signed word; another unit uses struct VECTOR. */
I16 far* td10checkptr;
struct TRANSFORMEDSHAPE3D cur_shps[29];
I16 runrndx;
int sky_hgt_world;
struct SHAPE3D intro_alt;
struct SHAPE3D logo_title;
U16S  statemgmtcpy;
I16S frmcs_time;
void far* gamerptrs;
/* Local view: rotpr; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
int rotpr[2];
struct RECTANGLE tparr[15];
int ground_skybox;
struct SHAPE3D brav;
I16 spdneedlegaugeclr;
int frmexcess;
/* Local view: rectclip; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
struct RECTANGLE rectclip[15];
int maxscnh;
int g_tdist[29];
/* Local view: backlightovr8; type is signed byte; another unit uses unsigned byte. */
I8 backlightovr8;
unsigned short scene_1ht;
unsigned short scene_2ht;
unsigned short scene_3ht;
unsigned short scene_4ht;
/* Local view: rects_updt; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
I8 rects_updt[15];
struct RECTANGLE rclist[15];
int g_skybox_sky_clr;
/* Local view: rcmapix; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
I16 rcmapix[45];
I16S exwd[3];
struct RECTANGLE rc0_cpy;
struct RECTANGLE g_savrc[15];
char tshapearrarg2[30];
I8 g_ts_num;
struct SHAPE2D far *skypics[4];
struct SHAPE2D far *sdgbmp_v[5];
int tsix[29];
/* Local view: rect_num3; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
I8 rect_num3;
unsigned char scene_idx;
union FARRESOURCE skyres_handle;
struct RECTANGLE game_rect_txt_in;
/* Local view: prevcamrot; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
I16 prevcamrot;
I8 far *sdgame2hdl;
I8 follow_op;
I16 g_skyboxwat_clr;
struct TRANSFORMEDSHAPE3D * g_curr_tsp;
struct MATRIX wkmatx;
/* Local view: cliprects_spare4; MSC 5.10 keeps the original block and file extern scopes; importing the view-state header at file scope changes external indices. */
short cliprects_spare4[120];
