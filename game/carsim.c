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
/* Exact-object requirement: write back the wheel response even when unchanged. */
#define WRITE_BACK(value) ((value) = (value))
#include "stunts_types.h"
/* Local view: rate_frame; type is signed word; another unit uses unsigned word. */
extern short rate_frame;


/* PORT: Layout uses MSC /Zp and target scalar widths. */


#include "geom2d.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "vec3.h"

/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "vec3l.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */
/* Local view: POINT2D; struct[0].name is px; another unit uses x. */
struct POINT2D {
	I16 px, py;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "matrix.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "plane.h"



/* Local math imports preserve communal LINK ordering; communal order is load-bearing. */




/* Local signed radius view; the shared math API returns an unsigned word. */
/* Local view: inthypot; return is signed word; another unit uses unsigned word. */
I16 inthypot(I16 z, I16 y);





















/* Local view: clipzone; return is signed word; another unit uses signed byte. */
I16 clipzone(struct RECTANGLE* r1, struct RECTANGLE* r2);





/* PORT: Layout uses MSC /Zp and target scalar widths. */




/* Local view: GAMEINFO; struct[8].type is unsigned word; another unit uses signed byte. */
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

#include "carstate.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */
/* Local view: GAMESTATE; struct.count is 45; another unit uses 46. */
struct GAMESTATE {
	I32 game_debris_x[24]; 
	I32 game_debris_y[24]; 
	I32 game_debris_z[24]; 
	struct VECTOR game_chase_camera_pos[2]; 
	struct VECTOR game_chase_camera_prev[2];
	I16S game_end_delay_elapsed;
	I16S game_end_delay_limit;
	I32  game_speed_frame_sum;
	U16S  game_frame;
	I16S game_total_finish; 
	I16S game_opponent_finish_time;
	I16S game_pEndFrame;
	I16S game_oEndFrame;   
	U16S  game_penalty;
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
	I16S game_debris_vertical_step[24];
	I8 kevinseed[6];
	I8 game_checkpoint_valid;
	I8 game_inputmode; 
	I8 game_3F6autoLoadEvalFlag;
	I8 game_track_camera_index[2]; 
	I8 game_opponent_speed_limit;
	I8 game_track_object_broken[48];
	I8 game_debris_active;
	I8 game_debris_shape_index[24];
	I8 game_debris_origin_kind[24];
	I8 game_route_recovery_mode;
	I8 game_route_confirm_count;
	I8 game_player_guidance;
	I8 game_opponent_pass_side;
	I8 field_45F;
};

/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "gamerec.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "simd.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "trackrec.h"

/* PORT: Layout uses MSC /Zp and target scalar widths. */
#include "trkinfo.h"


/* PORT: Layout uses MSC /Zp and target scalar widths. */
/* Local view: TRACKOBJECT; struct[2].type.pointer is void; another unit uses struct SHAPE3D. */
struct TRACKOBJECT {
    struct TRKOBJINFO *ss_trkObjInfoPtr;
    I16S ss_rotY;
    void *ss_shapePtr;
    void *ss_loShapePtr;
    U8  ss_ssOvelay;
    I8 ss_surfaceType, ss_ignoreZBias, ss_multiTileFlag, ss_physicalModel, scene_unk5;
};










/* Local view: g_vid_flg2_set; type is signed word; another unit uses unsigned word. */
extern I16S g_vid_flg2_set;
/* Local view: vidflg3is_minus1; type is signed word; another unit uses unsigned word. */
extern I16S vidflg3is_minus1;

/* Local view: g_videoflg5; type is signed word; another unit uses unsigned byte. */
extern I16S g_videoflg5;
/* Local view: g_vid_flag6; type is signed word; another unit uses unsigned byte. */
extern I16S g_vid_flag6;



/* Local view: elaptm1; type is unsigned word; another unit uses signed word. */
extern U16S  elaptm1;



/* Local view: gm_playmode; type is unsigned byte; another unit uses signed byte. */
extern U8  gm_playmode; 


 
 
 
/* Local view: st_hdg; type is signed word; another unit uses unsigned word. */
extern short st_hdg;
/* Local view: table_lookup; pointer is signed byte; another unit uses void. */
extern char *table_lookup;
/* Local view: steerWhlRespTable_10fps; type is array[None]; another unit uses near pointer. */
extern I8 steerWhlRespTable_10fps[];
/* Local view: steerWhlRespTable_20fps; type is array[None]; another unit uses near pointer. */
extern I8 steerWhlRespTable_20fps[];


















/* Local view: popupact; type is signed word; another unit uses signed byte. */
extern I16 popupact;
/* Local view: mouse_enabled; type is signed byte; another unit uses unsigned byte. */
extern I8 mousedrive;
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




/* Local view: main_data_file_addr; pointer is void; another unit uses signed byte. */
extern void far* main_data_file_addr;
/* Local view: cvxs_a; segment is huge; another unit uses far. */
extern struct GAMESTATE huge* cvxs_a;








 
 
/* Local view: td3; pointer is signed byte; another unit uses signed word. */
extern I8 far* td3;
 
 
/* Local view: td6_ptr_b; pointer is signed byte; another unit uses unsigned word. */
extern I8 far* td6_ptr_b;
/* Local view: trackdat7; pointer is signed byte; another unit uses unsigned word. */
extern I8 far* trackdat7;
/* Local view: g_td08d; pointer is signed word; another unit uses unsigned word. */
extern I16 far* g_td08d; 
/* Local view: trkptrpath; pointer is signed word; another unit uses struct VECTOR. */
extern I16 far* trkptrpath;
/* Local view: td10checkptr; pointer is struct VECTOR; another unit uses signed word. */
extern struct VECTOR far* trksign;
/* Local view: hscore_trk11_ptr; pointer is signed byte; another unit uses struct HighScoreRecord. */
extern I8 far* hscore_trk11_ptr; 
/* Local view: savedptr_ms; pointer is signed byte; another unit uses struct SPRITE. */
extern I8 far* savedptr_ms;
 
/* Local view: td14tb; pointer is unsigned byte; another unit uses signed byte. */
extern U8  far* td14tb; 
 
 
/* Local view: road_trk; pointer is signed byte; another unit uses unsigned byte. */
extern char far* road_trk; 
/* Local view: td_18_ref; pointer is signed byte; another unit uses unsigned byte. */
extern char far* td_18_ref;

 
/* Local view: g_column_of_trkdata21_pth; pointer is signed byte; another unit uses unsigned byte. */
extern char far* g_column_of_trkdata21_pth; 
/* Local view: tdfrompathrow22; pointer is signed byte; another unit uses unsigned byte. */
extern char far* tdfrompathrow22; 
 


/* Local view: g_is_busy; type is signed byte; another unit uses unsigned byte. */
extern I8 g_is_busy;


/* Local view: menutimeout; type is signed byte; another unit uses unsigned byte. */
extern I8 menutimeout;
/* Local view: dialog_frame_color; type is unsigned word; another unit uses signed word. */
extern U16S  dialog_frame_color;

/* Local view: endhsdemo; type is signed byte; another unit uses unsigned byte. */
extern I8 didrace;















/* Local view: renderlevel; type is unsigned byte; another unit uses signed byte. */
extern U8  renderlevel;































/* Local view: is_audioloaded; type is signed word; another unit uses signed byte. */
extern I16S is_audioloaded;


 



/* Local view: material_clrlist_ptr; pointer is signed word; another unit uses struct MATERIALCLRLIST. */
extern I16* material_clrlist_ptr;










/* Local view: run_intro_looped; return is unsigned word; another unit uses signed word. */
extern U16S  run_intro_looped(void);
/* Local view: messagebox; return is unsigned word; another unit uses signed word. */
extern U16S  messagebox(I16 unk1, I16 unk2, void far* textresptr, U16S  unk3, U16S  unk4, I16 arg, void* unk5, I16 unk6);





/* Local view: selectcar; parameters[0].pointer is struct GAMEINFO; another unit uses signed byte. */
extern void selectcar(struct GAMEINFO* unk, I8* unk2, I8* unk3, U16  unk4);

/* Local view: end_hiscore; return is unsigned word; another unit uses signed byte. */
extern unsigned evaluation(void);
/* Local view: run_option_menu; return is unsigned word; another unit uses signed byte. */
extern unsigned run_option_menu(void);
/* Local view: getprotection; parameters.count is 0; another unit uses 1. */
extern void getprotection(void);


































/* Local view: initmouse; parameters[0] is signed word; another unit uses unsigned word. */
extern I16S initmouse(I16S a1, I16S a2);









/* Local view: sprset1size; parameters[0] is unsigned word; another unit uses signed word. */
extern void clippix(U16S  left, U16S  right, U16S  top, U16S  height);
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
extern void clearwindow(U8 );
/* Local view: sprite_blit_to_video; return is void; another unit uses signed word. */
extern void sprite_blit_to_video(struct SPRITE far* sprite);




/* Local view: timer_get_delta_alt; return is unsigned long; another unit uses signed word. */
extern U32  elticks(void);



/* Local view: abortmessage; parameters.count is 2; another unit uses 1. */
extern void abortmessage(const I8*, ...);


/* Local view: _memcpy; return is near pointer; another unit uses void. */
extern void* _memcpy(void*, const void*, unsigned);


















/* PORT: Layout uses MSC /Zp and target scalar widths. */

#include "audrec.h"

/* PORT: Layout uses MSC /Zp and target scalar widths. */
#include "audapi.h"
#include "auddrv.h"
#include "car.h"
#include "dialog.h"
#include "dlgcolor.h"
#include "mathapi.h"
#include "game.h"
#include "gfxapi.h"
#include "input.h"
#include "mathops.h"
#include "memapi.h"
#include "prim3d.h"
#include "replay.h"
#include "resapi.h"
#include "shape2d.h"
#include "text.h"
#include "timer.h"
#include "track.h"
#include "trig.h"
















/* Local view: g_cursurfacekindvalue; type is signed byte; another unit uses unsigned byte. */
extern I8 g_cursurfacekindvalue;







/* Local view: g_planlist; pointer is struct PLANE; another unit uses unsigned byte. */
extern struct PLANE far* g_planlist;
/* Local view: plncurrptr; pointer is struct PLANE; another unit uses unsigned byte. */
extern struct PLANE far* plncurrptr;



/* Local view: test_pln; type is signed byte; another unit uses unsigned byte. */
extern I8 test_pln;








/* Local view: plan_memres; type is array[None]; another unit uses struct PLANE. */
extern struct PLANE far plan_memres[];



/* Local view: INIT_OBJECTS_BUFFER; return is unsigned word; another unit uses signed word. */
extern unsigned INIT_OBJECTS_BUFFER(I16 angX, I16 angY, I16 angZ, struct RECTANGLE* cliprect, I16 unk);
/* Local view: ADD_OBJECT_TO_BUFFER; return is void; another unit uses unsigned word. */
extern void ADD_OBJECT_TO_BUFFER(struct TRANSFORMSHAPE3D* shape);




/* Local view: polygonnumber; type is signed word; another unit uses unsigned word. */
extern I16 polygonnumber;

/* Unused OMF-order declaration: this address is private poly_link_list[0] in obj_seg006. */

/* Local view: drawpolypt; parameters[2].pointer is signed word; another unit uses struct POINT2D. */
extern void drawpolypt(I16 color, I16 vertlinecount, I16* vertlines);

/* Local view: trklst; array is None; another unit uses 215. */
extern struct TRACKOBJECT trklst[];




























static struct MATRIX fallback_rotation_cache;

static struct MATRIX plane_rotation_cache;










/* Local view: update_car_speed; parameters[0] is signed byte; another unit uses signed word. */
void doengine(I8 inputByte, I8 isOpponent, struct CARSTATE* carState, struct SIMD* carSetup);






/* Local view: myupdatesfx1; parameters[0].pointer is struct AUDIO_CAR_FRAME; another unit uses signed byte. */
void myupdatesfx1(struct AUDIO_CAR_FRAME *record, I16S value);
/* Local view: gettlistpoint; return is signed byte; another unit uses signed word. */
I8 gettlistpoint(I16 trackIndex, struct TRACKRESULT *result, I8 side,
              I8 *opponentSpeed);











I16 camerasizeinfo[4] = { 10, 50, 10, 20 };
struct POINT2D signsizeinfo[2] = { { 5, 40 }, { 5, 10 } };
struct POINT2D sfpostsizeinfo[2] = { { 6, 121 }, { 6, 9 } };
struct POINT2D pointsizeinfo[2] = { { 1, 10 }, { 1, 10 } };
I16 gravity[4] = { 21, 21, 15, 15 };
/* Local view: steerWhlRespTable_20fps; type is array[64]; another unit uses near pointer. */
I8 steerWhlRespTable_20fps[64] = {
    0, 8, -8, 0, 0, 7, -7, 0, 0, 6, -6, 0, 0, 5, -5, 0,
    0, 4, -4, 0, 0, 4, -4, 0, 0, 3, -3, 0, 0, 3, -3, 0,
    0, 2, -2, 0, 0, 2, -2, 0, 0, 2, -2, 0, 0, 1, -1, 0,
    0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0, 0, 1, -1, 0
};
/* Local view: steerWhlRespTable_10fps; type is array[62]; another unit uses near pointer. */
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

void moveopponent(void)
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
    core.opponentstate.car_audio_flags = 0;
    core.game_opponent_pass_side = 0;
    tmat = makematrix(core.opponentstate.car_rotate.z, core.opponentstate.car_rotate.y,
                           core.opponentstate.car_rotate.x, 1);
    core.opponentstate.car_audio_flags = 1;
    if (core.opponentstate.car_crashBmpFlag != 0) {
        if (core.opponentstate.car_speed2 == 0)
            core.opponentstate.car_audio_flags = 0;
    } else {
        waypoint = core.opponentstate.car_route_center;
        if (waypoint.y != -1) {
            delta.x = waypoint.x - x1;
            delta.y = waypoint.y - y1;
            delta.z = waypoint.z - z1;
            temp = int3dhypot(&delta);
        } else {
            temp = inthypot(waypoint.x - x1, waypoint.z - z1);
        }
        if (temp < 200) {
next_waypoint:
            if (gettlistpoint(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_route_center, core.opponentstate.car_route_point_index++,
                          &core.game_opponent_speed_limit) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((I16S far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
                    core.opponentstate.car_finish_route_passed++;
                    core.opponentstate.car_trackdata3_index = 0;
                }
                core.opponentstate.car_route_point_index = 0;
            }
        }
        if (core.game_inputmode == 2) {

            goal = core.opponentstate.car_route_center;
        } else {
            diff.x = x2 - x1;
            diff.y = y2 - y1;
            diff.z = z2 - z1;
            mat_vec(&diff, tmat, &pv);
            if (pv.y > 90 || (pv.x < 0 ? -pv.x : pv.x) > 180 ||
                pv.z > 600 || pv.z < -180)
                goal = core.opponentstate.car_route_center;
            else {
                diff.x = x2 - core.opponentstate.car_route_center.x;
                if (core.opponentstate.car_route_center.y == -1)
                    diff.y = 0;
                else
                    diff.y = y2 - core.opponentstate.car_route_center.y;
                diff.z = z2 - core.opponentstate.car_route_center.z;
                mat_vec(&diff, tmat, &rel);
                if (rel.x < 0) {
                    goal.x = ((I32)core.opponentstate.car_route_center.x + core.opponentstate.car_route_edge_b.x) >> 1;
                    if (core.opponentstate.car_route_center.y == -1)
                        goal.y = -1;
                    else
                        goal.y = ((I32)core.opponentstate.car_route_center.y + core.opponentstate.car_route_edge_b.y) >> 1;
                    goal.z = ((I32)core.opponentstate.car_route_center.z + core.opponentstate.car_route_edge_b.z) >> 1;
                    if (pv.z > -78 && core.playerstate.car_crashBmpFlag == 0)
                        core.game_opponent_pass_side = 2;
                } else {
                    goal.x = ((I32)core.opponentstate.car_route_center.x + core.opponentstate.car_route_edge_a.x) >> 1;
                    if (core.opponentstate.car_route_center.y == -1)
                        goal.y = -1;
                    else
                        goal.y = ((I32)core.opponentstate.car_route_center.y + core.opponentstate.car_route_edge_a.y) >> 1;
                    goal.z = ((I32)core.opponentstate.car_route_center.z + core.opponentstate.car_route_edge_a.z) >> 1;
                    if (pv.z > -78 && core.playerstate.car_crashBmpFlag == 0)
                        core.game_opponent_pass_side = 1;
                }
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
        steerAngle = intatan(waypoint.x, waypoint.z);
        if (core.opponentstate.car_slidingFlag == 0 &&
            (steerAngle < 0 ? -steerAngle : steerAngle) > ANGLE_QUARTER_TURN) {
            if (gettlistpoint(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
                          &core.opponentstate.car_route_center, core.opponentstate.car_route_point_index++,
                          &core.game_opponent_speed_limit) != 0) {
                core.opponentstate.car_trackdata3_index++;
                if (((I16S far *)td3)[core.opponentstate.car_trackdata3_index] == 0) {
                    core.opponentstate.car_finish_route_passed++;
                    core.opponentstate.car_trackdata3_index = 0;
                }
                core.opponentstate.car_route_point_index = 0;
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
                spdLimit = core.game_opponent_speed_limit << Q8_FRACTION_BITS; /* PORT: Q8 value is converted at the legacy boundary. */
            if (spdLimit - CAR_SPEED_Q8_ONE_MPH > core.opponentstate.car_speed)
                mode = 1;
            else if (spdLimit + CAR_SPEED_Q8_THREE_MPH < core.opponentstate.car_speed)
                mode = 2;
        }
    }
    doengine(mode, 1, &core.opponentstate, &ophys_7);
    dohandling(&core.opponentstate, &ophys_7, 0);
    movecar(&core.opponentstate, &ophys_7, &core.playerstate, &simdp7, 1);
    if (core.opponentstate.car_crashBmpFlag == 0) {
        diff = core.opponentstate.car_route_center;
        diff.x -= core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        diff.y -= core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        diff.z -= core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        tmat = makematrix(core.opponentstate.car_rotate.z, core.opponentstate.car_rotate.y,
                               core.opponentstate.car_rotate.x, 1);
        mat_vec(&diff, tmat, &waypoint);
        core.opponentstate.car_route_bearing = intatan(-waypoint.x, waypoint.z) & ANGLE_TURN_MASK;
    }
    if (core.opponentstate.car_finish_route_passed != 0) {
        temp = fixedmult(intcos(st_hdg),
            row_ctr_zs[tagtrk] - (core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        temp += fixedmult(intsin(st_hdg),
            trackctrpos2[idxtrk] - (core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        if (temp < 0)
            finishgame(3, 1);
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

void movecar(struct CARSTATE* activeCarState, struct SIMD* activeCarSetup, struct CARSTATE* otherCarState, struct SIMD* otherCarSetup, I8 isOpponent) {
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
	matrix_transform_view = *makematrix(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
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
		rotMatrix = *makematrix(0, 0, -activeCarState->car_angle_z, 0);
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
		wheelCrd.y = -(activeCarState->car_wheel_suspension_offset[w] + 0x180);
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
			move_on_plane();
			curWhl->lx += pln_rot_output.x;
			curWhl->ly += pln_rot_output.y;
			curWhl->lz += pln_rot_output.z;
		}
	}

	num = 0;
retry:
	if (++num == 5) {
		activeCarState->car_36MwhlAngle = ANGLE_HALF_TURN;
		finishgame(1, isOpponent);
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
			getpinfo(&wheelCrd, &activeCarState->car_whlWorldCrds1[w]);
		}
		activeCarState->car_surfaceWhl[w] = g_cursurfacekindvalue;
		wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (core.game_inputmode == 2)
			nextpos_normalip = wheelCrd.y;
		else
			nextpos_normalip = plane_distance(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
		if (road_num != -1 && nextpos_normalip > element_min_wall && nextpos_normalip < wall_wallelement) {
			oldVec.x = activeCarState->car_whlWorldCrds1[w].x - wallanchor_x;
			oldVec.y = 0;
			oldVec.z = activeCarState->car_whlWorldCrds1[w].z - wallanchor_z;
			vec.x = (curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT) - wallanchor_x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			vec.y = 0;
			vec.z = (curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT) - wallanchor_z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			xformy(&localPlane, -wall_facingang - ANGLE_QUARTER_TURN);
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
					zclipline(&p1, &p0, &res, 0);
					tmpCrds.x = (p1.x - res.x) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.y = (p1.y - res.y) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.z = (p1.z - res.z) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					distBefore = int3dhypot(&tmpCrds);
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
				rotMat = makematrix(-anglerotate_car, -yrotrotveh, angle, 0);
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
					finishgame(1, isOpponent);
				}
				activeCarState->car_audio_flags |= 0x10;
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
		do {
check_height:
			if (nextpos_normalip > 0) {
				if (liftOfs > 0 && nextpos_normalip < 24) {
					curWhl->lx += hopVec.x;
					curWhl->ly += hopVec.y;
					curWhl->lz += hopVec.z;
				} else {
					activeCarState->car_wheel_fall_step[w] += gravity[w];
					curWhl->ly -= activeCarState->car_wheel_fall_step[w];
					if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
						activeCarState->car_wheel_fall_step[w] += gravity[w];
						curWhl->ly -= activeCarState->car_wheel_fall_step[w];
					}
					wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					if (core.game_inputmode == 2)
						nextpos_normalip = wheelCrd.y;
					else
						nextpos_normalip = plane_distance(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
					if (nextpos_normalip > 12)
						activeCarState->car_surfaceWhl[w] = 0;
				}
			}
			whlHgts[w] = nextpos_normalip;
			if (nextpos_normalip == 0) {
				if (activeCarState->car_wheel_fall_step[w] > 250)
					activeCarState->car_audio_flags |= 0x20;
				if (activeCarState->car_wheel_fall_step[w] > 23275)
					finishgame(1, isOpponent);
				activeCarState->car_wheel_fall_step[w] = 0;
				break;
			}
			if (nextpos_normalip >= 0)
				break;
			plane = &g_planlist[pl_i];
			base.x = plane->plane_origin.x + x_course_part;
			base.y = plane->plane_origin.y + hgthgt;
			base.z = plane->plane_origin.z + road_elem_ctrz;
			oldVec.x = (prev->lx >> WORLD_COORDINATE_SCALE_SHIFT) - base.x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			oldVec.y = (prev->ly >> WORLD_COORDINATE_SCALE_SHIFT) - base.y; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			oldVec.z = (prev->lz >> WORLD_COORDINATE_SCALE_SHIFT) - base.z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			vec.x = (curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT) - base.x; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			vec.y = (curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT) - base.y; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			vec.z = (curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT) - base.z; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			localPlane = plane->plane_rotation;
			mat_invert(&localPlane, &rotMatrix);
			mat_vec(&oldVec, &rotMatrix, &p0);
			mat_vec(&vec, &rotMatrix, &p1);
			swap = 0;
			if (test_pln == 0 && p0.y < -12 && p1.y < -12) {
				if (p1.y > -24) {
					finishgame(5, isOpponent);
					swap = 1;
				} else {
					pl_i = 0;
					plncurrptr = g_planlist;
					test_pln = 1;
					wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					nextpos_normalip = plane_distance(0, wheelCrd.x, wheelCrd.y, wheelCrd.z);
					goto check_height;
				}
			}
			if (p1.y == 0) {
				tvec2.x = 0;
				tvec2.y = 0;
				tvec2.z = 0x40;
				g_planidx2 = pl_i;
				frwhl_angadjusted = wheelAngle[w];
				move_on_plane();
				curWhl->lx -= pln_rot_output.x;
				curWhl->ly -= pln_rot_output.y;
				curWhl->lz -= pln_rot_output.z;
			} else {
				if (p0.y <= 0 || p1.y >= 0) {
					tvec2.x = 0;
					tvec2.y = 0;
					tvec2.z = spd;
					g_planidx2 = pl_i;
					frwhl_angadjusted = wheelAngle[w];
					move_on_plane();
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
					zclipline(&p1, &p0, &res, 0);
					tmpCrds.x = (p1.x - res.x) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.y = (p1.y - res.y) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					tmpCrds.z = (p1.z - res.z) << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
					angle = int3dhypot(&tmpCrds);
					remDist = activeCarState->car_wheel_fall_step[w] + spd;
					distBefore = remDist - angle;
					p0.x = (curWhl->lx - prev->lx) * distBefore / remDist;
					p0.y = (curWhl->ly - prev->ly) * distBefore / remDist;
					p0.z = (curWhl->lz - prev->lz) * distBefore / remDist;
					tvec2.x = 0;
					tvec2.y = 0;
					tvec2.z = angle;
					g_planidx2 = pl_i;
					frwhl_angadjusted = wheelAngle[w];
					move_on_plane();
					curWhl->lx = prev->lx + p0.x + pln_rot_output.x;
					curWhl->ly = prev->ly + p0.y + pln_rot_output.y;
					curWhl->lz = prev->lz + p0.z + pln_rot_output.z;
				}
				wheelCrd.x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				wheelCrd.y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				wheelCrd.z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				if ((nextpos_normalip = plane_distance(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z)) < 0) {
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
			}
			if (activeCarState->car_wheel_fall_step[w] > 250)
				activeCarState->car_audio_flags |= 0x20;
			if (activeCarState->car_wheel_fall_step[w] > 23275)
				finishgame(1, isOpponent);
			activeCarState->car_wheel_fall_step[w] = 0;
		} while (0);
	}
wheels_done:
	if (activeCarState->car_surfaceWhl[0] == 5 && activeCarState->car_surfaceWhl[1] == 5 &&
	    activeCarState->car_surfaceWhl[2] == 5 && activeCarState->car_surfaceWhl[3] == 5)
		finishgame(2, isOpponent);
	curWhl = whlPos;
	for (w = 0; w < 4; ++curWhl, ++w) {
		activeCarState->car_whlWorldCrds1[w].x = curWhl->lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		activeCarState->car_whlWorldCrds1[w].y = curWhl->ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		activeCarState->car_whlWorldCrds1[w].z = curWhl->lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		angle = doshock(activeCarState, whlHgts[w], w);
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
	rotxvehicle = intatan(angle, -distBefore) & ANGLE_TURN_MASK;
	xformy(&rotMatrix, rotxvehicle);
	for (w = 0; w < 4; w++) {
		res = posDiffs[w];
		mat_vec(&res, &rotMatrix, &posDiffs[w]);
	}
	distBefore = posDiffs[3].z + posDiffs[2].z - posDiffs[0].z - posDiffs[1].z;
	remDist = posDiffs[3].y + posDiffs[2].y - posDiffs[0].y - posDiffs[1].y;
	if (remDist == 0 && distBefore < 0) {
		yrotrotveh = 0;
	} else {
		yrotrotveh = intatan(-distBefore, remDist) - 0x100;
		if ((yrotrotveh < 0 ? -yrotrotveh : yrotrotveh) < 2)
			yrotrotveh = 0;
	}
	if (yrotrotveh != 0) {
		xformx(&rotMatrix, yrotrotveh);
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
		anglerotate_car = intatan(distBefore, remDist) - 0x100;
		if ((anglerotate_car < 0 ? -anglerotate_car : anglerotate_car) < 2)
			anglerotate_car = 0;
	}
	activeCarState->car_sumSurfFrontWheels = activeCarState->car_surfaceWhl[0] + activeCarState->car_surfaceWhl[1];
	activeCarState->car_sumSurfRearWheels = activeCarState->car_surfaceWhl[2] + activeCarState->car_surfaceWhl[3];
	if (core.game_inputmode != 2) {
		if (inrepflg == 0) {
			if (isOpponent != 0)
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_dispatch_car_events(activeCarState->car_audio_flags, op_eng_sound_id);
			else
				/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_dispatch_car_events(activeCarState->car_audio_flags, g_player_sound_id);
		}
		rotMat = makematrix(-anglerotate_car, -yrotrotveh, -rotxvehicle, 0);
		for (w = 0; w < 4; w++) {
			wheelCrd = activeCarSetup->wheel_coords[w];
			wheelCrd.y = activeCarSetup->collide_points[0].py << WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Map-to-world scale is 64:1. */
			mat_vec(&wheelCrd, rotMat, &res);
			wheelCrd.x = (res.x + centerpos) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			wheelCrd.y = (res.y + veh_position) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			wheelCrd.z = (res.z + veh_z) >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			tmpCrds = wheelCrd;
			getpinfo(&wheelCrd, &activeCarState->car_whlWorldCrds2[w]);
			i = plane_distance(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
			if (pl_i < 4) {
				if (i <= 0)
					finishgame(5, isOpponent);
			} else {
				planIdx = pl_i;
				wheelCrd = activeCarState->car_whlWorldCrds2[w];
				getpinfo(&wheelCrd, &tmpCrds);
				if (planIdx == pl_i) {
					found = plane_distance(pl_i, wheelCrd.x, wheelCrd.y, wheelCrd.z);
					if (gm_playmode != 1 && ((i < 0 && found > 0) || (i > 0 && found < 0))) {

						finishgame(5, isOpponent);
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
			if (boxcheck(activeCarSetup->collide_points, self, otherCarSetup->collide_points, objPos)) {
				if (activeCarState->car_contact_processed != 0)
					return;
				if (calccoll(activeCarState, otherCarState) == 0)
					return;
				finishgame(1, isOpponent);
				finishgame(1, isOpponent ^ 1);
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
		tiltFlag = getwpinfo(res.x, res.z, objPts);
		if (tiltFlag != 0) {
			/* The otherwise-unused nextVec increment is required by the exact object. */
			for (i = 0; i < tiltFlag; nextVec++, i++) {
				objPos[0].x = objPts[i].x;
				objPos[0].y = objPts[i].y;
				objPos[0].z = objPts[i].z;
				if (boxcheck(activeCarSetup->collide_points, self, pointsizeinfo, objPos)) {
					activeCarState->car_36MwhlAngle -= ANGLE_HALF_TURN;

					finishgame(1, isOpponent);
					return;
				}
			}
		}
		i = (I8)td19hdl[lnoffsets[res.z] + res.x];
		if (i != -1 && core.game_track_object_broken[i] == 0) {
			objPos[0].x = trksign[i].x;
			objPos[0].y = trksign[i].y;
			objPos[0].z = trksign[i].z;
			if (boxcheck(activeCarSetup->collide_points, self, signsizeinfo, objPos)) {
				core.game_track_object_broken[i] = 1;
				init_explode(i + 2, -activeCarState->car_rotate.x, (I32)activeCarState->car_speed2 * 0x580 / 0x3C00U);
			}
		}
		if (res.x == idxtrk && res.z == tagtrk) {
			objPos[0].x = trackctrpos2[idxtrk] + fixedmult(intsin(st_hdg + ANGLE_QUARTER_TURN), 126);
			objPos[0].y = terheight[g_hillf];
			objPos[0].z = fixedmult(intcos(st_hdg + ANGLE_QUARTER_TURN), 126) + row_ctr_zs[tagtrk];
			if ((found = boxcheck(activeCarSetup->collide_points, self, sfpostsizeinfo, objPos)) == 0) {
				objPos[0].x = fixedmult(intsin(st_hdg + ANGLE_THREE_QUARTERS_TURN), 126) + trackctrpos2[idxtrk];
				objPos[0].z = fixedmult(intcos(st_hdg + ANGLE_THREE_QUARTERS_TURN), 126) + row_ctr_zs[tagtrk];
				found = boxcheck(activeCarSetup->collide_points, self, sfpostsizeinfo, objPos);
			}
			if (found != 0) {
				finishgame(1, isOpponent);
				return;
			}
		}
	}
store_state:
	activeCarState->car_posWorld1.lx = centerpos;
	activeCarState->car_posWorld1.ly = veh_position;
	activeCarState->car_posWorld1.lz = veh_z;
	activeCarState->car_rotate.z = anglerotate_car;
	activeCarState->car_rotate.y = yrotrotveh;
	activeCarState->car_rotate.x = rotxvehicle;
	activeCarState->car_contact_processed = 0;
}

void initcar(struct CARSTATE* playerstate, struct SIMD* simd,
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
    playerstate->car_grip_angle_excess = zero;
    playerstate->car_route_bearing = zero;
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
        playerstate->car_wheel_fall_step[i] = zero;
        playerstate->car_wheel_suspension_offset[i] = zero;
        playerstate->car_rc3[i] = zero;
        playerstate->car_rc4[i] = zero;
        playerstate->car_wheel_suspension_target[i] = zero;
        playerstate->car_whlWorldCrds1[i] = whlPos;
        playerstate->car_whlWorldCrds2[i] = whlPos;
    }
    playerstate->car_engineLimiterTimer = zero;
    playerstate->car_slidingFlag = zero;
    playerstate->car_contact_processed = zero;
    playerstate->car_crashBmpFlag = zero;
    playerstate->car_changing_gear = zero;
    playerstate->car_fpsmul2 = zero;
    playerstate->car_transmission = transmission;
    playerstate->car_finish_route_passed = zero;
    playerstate->car_route_point_index = zero;
    playerstate->car_audio_flags = 1;
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

void initsim(I16S arg)
{
	register I16 zeroValue = 0;
	register I16 i;
	I16 tmpcol, tmprow;

	if (arg == -1) {
		elaptm1 = zeroValue;

		for (i = 0; i < 20; ++i) {
			cvxs_a[i].game_checkpoint_valid = zeroValue;
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
		initworld();

		core.game_checkpoint_valid = 1;
		core.game_end_delay_limit = 1;
		core.game_inputmode = zeroValue;
		core.game_3F6autoLoadEvalFlag = zeroValue;
		core.game_end_delay_elapsed = zeroValue;
		core.game_player_last_route_node = zeroValue;
		core.game_track_camera_index[0] = zeroValue;
		core.game_track_camera_index[1] = zeroValue;

		for (i = zeroValue; i < 48; ++i) {
			core.game_track_object_broken[i] = zeroValue;
		}
				
		for (i = zeroValue; i < 24; ++i) {
			core.game_debris_planar_step[i] = zeroValue;
		}

		core.game_chase_camera_pos[0].x =
			  fixedmult(intsin(st_hdg + ANGLE_HALF_TURN), 4096)
			+ fixedmult(intsin(st_hdg + ANGLE_THREE_QUARTERS_TURN),  512)
			+ (idxtrk << 10);

		core.game_chase_camera_pos[0].y = terheight[g_hillf] + 960;

		core.game_chase_camera_pos[0].z =
			  fixedmult(intcos(st_hdg + ANGLE_HALF_TURN), 4096)
			+ fixedmult(intcos(st_hdg + ANGLE_THREE_QUARTERS_TURN),  512)
			+ r_zp[tagtrk];

		core.game_chase_camera_pos[1] = core.game_chase_camera_pos[0];
		core.game_chase_camera_prev[0] = core.game_chase_camera_pos[0];
		core.game_chase_camera_prev[1] = core.game_chase_camera_pos[0];
		
		core.game_speed_frame_sum = 0L;
		core.game_frame = zeroValue;
		core.game_total_finish = zeroValue;
		core.game_opponent_finish_time = zeroValue;
		core.game_pEndFrame = zeroValue;
		core.game_oEndFrame = zeroValue;
		core.game_penalty = zeroValue;
		core.game_impactSpeed = zeroValue;
		core.game_topSpeed = zeroValue;
		core.game_jumpCount = zeroValue;

		
		tmpcol =
			  fixedmult(intsin(st_hdg + ANGLE_QUARTER_TURN),  36)
			+ fixedmult(intsin(st_hdg + ANGLE_HALF_TURN), 210);
		
		tmprow =
			  fixedmult(intcos(st_hdg + ANGLE_QUARTER_TURN),  36)
			+ fixedmult(intcos(st_hdg + ANGLE_HALF_TURN), 210);

		initcar(
			&core.playerstate,
			&simdp7,
			globalgamesettings.game_playertransmission,
			(trackctrpos2[idxtrk] + tmpcol) * 64L,
			terheight[g_hillf] * 64L,
			(row_ctr_zs[tagtrk] + tmprow) * 64L,
			-st_hdg);

		core.game_player_route_anchor = zeroValue;
		core.game_player_guidance = zeroValue;
		core.game_opponent_pass_side = zeroValue;
		core.game_route_recovery_mode = zeroValue;
		core.game_route_confirm_count = zeroValue;
		
		core.game_startcol  = idxtrk;
		core.game_startcol2 = idxtrk;
		core.game_startrow  = tagtrk;
		core.game_startrow2 = tagtrk;

		if (arg != -2) {
			gettlistpoint(
				core.playerstate.car_trackdata3_index,
				&core.playerstate.car_route_center,
				core.playerstate.car_route_point_index++,
				0);
			
		}

		
		tmpcol =
			  fixedmult(intsin(st_hdg + ANGLE_THREE_QUARTERS_TURN),  36)
			+ fixedmult(intsin(st_hdg + ANGLE_HALF_TURN), 210);
		
		tmprow =
			  fixedmult(intcos(st_hdg + ANGLE_THREE_QUARTERS_TURN),  36)
			+ fixedmult(intcos(st_hdg + ANGLE_HALF_TURN), 210);

		initcar(
			&core.opponentstate,
			&ophys_7,
			1,
			(trackctrpos2[idxtrk] + tmpcol) * 64L,
			terheight[g_hillf] * 64L,
			(row_ctr_zs[tagtrk] + tmprow) * 64L,
			-st_hdg);

		if (globalgamesettings.game_opponenttype && arg != -2) {
			gettlistpoint(
				((I16S far *)td3)[core.opponentstate.car_trackdata3_index], 
				&core.opponentstate.car_route_center,
				core.opponentstate.car_route_point_index++,
				&core.game_opponent_speed_limit); 
		
		}

		core.game_debris_active = zeroValue;
	}
}

/* Purpose: Restores saved game state and timer position.
 * Parameters: frame.
 * Returns: none.
 * Globals read: core, cvxs_a, elaptm1, g_cvxintvl
 * Globals written: core, tmr2
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void gotosim(I16 frame)
{
    register I16 curframe;

    if (frame == 0 && elaptm1 == 0) {
        initsim(0);
    }

    curframe = frame / g_cvxintvl;
    if (curframe == 20) {
        --curframe;
    }

    if (frame < core.game_frame)
        goto restore;
    while (g_cvxintvl * curframe > core.game_frame) {
        if (cvxs_a[curframe].game_checkpoint_valid != 0) {
restore:
            core = cvxs_a[curframe];
            mysetseeds(core.kevinseed);
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

void movesim() {
	I8 var_carInputByte;
	register I16 tmp;

	var_carInputByte = g_tdreplay16buf[core.game_frame];
	if (var_carInputByte != 0) {
		core.game_inputmode = 1;
	}
	
	if ((core.game_frame % g_cvxintvl) == 0) {
		tmp = core.game_frame / g_cvxintvl;
		mygetseeds(core.kevinseed);

		cvxs_a[tmp] = core;
	}

	core.game_frame++;
	if (core.game_3F6autoLoadEvalFlag != 0 && core.game_end_delay_elapsed < core.game_end_delay_limit) {
		core.game_end_delay_elapsed++;
		if (core.game_end_delay_elapsed == core.game_end_delay_limit && sigframe == 0) {
			if (core.playerstate.car_crashBmpFlag == 1 && core.playerstate.car_speed2 != 0) {
				core.game_end_delay_limit++;
			} else if (gm_playmode == 0) {
				sigframe = 1;
			}
		}
	}

	if (core.game_inputmode != 0) {
		
		moveplayer(var_carInputByte);
		
		if (globalgamesettings.game_opponenttype != 0) {
			moveopponent();
		}

		movehelicopters();
		if (core.game_debris_active != 0) {
			update_crash_debris();
		}

		/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ myupdatesfx();

	} else if (gm_playmode == 1) {
		
		/* PLATFORM(audio): Legacy audio service or audio-resource loading. */ myupdatesfx();
		if (g_rpl_init != 0) {
			if (g_sgateopn < 0x1C2) {
				g_sgateopn += 8;
			}

			if (g_rpl_init == 1 && g_sgateopn > 0x180) {
				g_rpl_init++;
			}

			if (g_rpl_init == 2) {
				tmp =
					fixedmult(intcos(st_hdg), row_ctr_zs[tagtrk] - (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)) /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
					+ fixedmult(intsin(st_hdg), trackctrpos2[idxtrk] - (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
				if (tmp > 0xE4) {
					if (core.playerstate.car_speed < 0x500) {
						moveplayer(1);
					} else {
						moveplayer(0);
					}
				} else {
					if (core.playerstate.car_speed != 0) {
						moveplayer(2);
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

void moveplayer(I8 inputByte) {
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

	core.playerstate.car_audio_flags = 1;
	if (core.playerstate.car_crashBmpFlag != 0) {
		core.game_player_guidance = 0;
		inputByte = 2;
		if (core.playerstate.car_speed2 == 0) {
			core.playerstate.car_audio_flags = 0;
			if (core.playerstate.car_speed == 0 && core.playerstate.car_wheel_fall_step[0] == 0 &&
			    core.playerstate.car_wheel_fall_step[1] == 0 && core.playerstate.car_wheel_fall_step[2] == 0 &&
			    core.playerstate.car_wheel_fall_step[3] == 0)
				return;
		}
	}

	doengine(inputByte, 0, &core.playerstate, &simdp7);
	dosteering((inputByte >> 2) & 3);
	dohandling(&core.playerstate, &simdp7, 1);
	movecar(&core.playerstate, &simdp7, &core.opponentstate, &ophys_7, 0);
	core.game_speed_frame_sum += core.playerstate.car_speed2;
	crash_mode = core.game_route_recovery_mode;
	tile_index = core.game_player_route_anchor;
	height = findcar(&tile_index, &penalty_ctr);
	if (height != 0) {
		if (penalty_ctr == -2) {
			core.game_route_recovery_mode = 1;
			core.game_route_confirm_count = 0;
		} else if (core.game_route_recovery_mode == 1) {
			core.game_route_recovery_mode = 0;
			core.game_route_confirm_count = 0;
		}
		if (core.game_route_recovery_mode == 0) {
			if (tile_index == 0 && core.game_player_last_route_node != 0) {
				core.playerstate.car_finish_route_passed++;
				goto next_lap;
			}
			if (penalty_ctr >= 0 && penalty_ctr < 3) {
				core.game_route_confirm_count = 0;
				core.game_player_route_anchor = tile_index;
			} else if (penalty_ctr == -1 || penalty_ctr > 3) {
				if (g_td01_track_filecpy[core.game_player_last_route_node] == tile_index ||
				    trackdata_penalty_related[core.game_player_last_route_node] == tile_index) {
					core.game_route_confirm_count++;
				} else {
					if (g_td01_track_filecpy[tile_index] == core.game_player_last_route_node ||
					    trackdata_penalty_related[tile_index] == core.game_player_last_route_node)
						core.game_route_recovery_mode = 2;
					core.game_route_confirm_count = 1;
				}
				if (core.game_route_confirm_count >= 3) {
next_lap:
					core.game_player_route_anchor = tile_index;
					core.game_route_confirm_count = 0;
					if (penalty_ctr > 0) {
						g_penaltytm = penalty_ctr * /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame * 3;
						pen_flag_count = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
						core.game_penalty += g_penaltytm;
					}
				}
			}
		}
		core.game_player_last_route_node = tile_index;
	}

	core.game_player_guidance = 0;
	if (core.game_route_recovery_mode == 1)
		return;
	planeMatrix = makematrix(core.playerstate.car_rotate.z, core.playerstate.car_rotate.y,
	                          core.playerstate.car_rotate.x, 1);
	if (core.game_route_recovery_mode == 2) {
		if (core.playerstate.car_crashBmpFlag == 0)
			core.game_player_guidance = 3;
		tile_index = core.game_player_last_route_node;
		goto search;
	}
	if (core.playerstate.car_trackdata3_index == -1) {

		height = 0;
	} else {
		if ((crash_mode != 0 && core.game_route_recovery_mode == 0) ||
		    (core.playerstate.car_trackdata3_index != core.game_player_route_anchor &&
		     g_td01_track_filecpy[core.game_player_route_anchor] != core.playerstate.car_trackdata3_index &&
		     trackdata_penalty_related[core.game_player_route_anchor] != core.playerstate.car_trackdata3_index)) {
			core.playerstate.car_trackdata3_index = -1;
			height = 0;
		} else {
			playerRelative.x = core.playerstate.car_route_center.x - (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			if (core.playerstate.car_route_center.y != -1)
				playerRelative.y = core.playerstate.car_route_center.y - (core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			else
				playerRelative.y = 0;
			playerRelative.z = core.playerstate.car_route_center.z - (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
			mat_vec(&playerRelative, planeMatrix, &player_plane_vec);
			height = player_plane_vec.z;
		}
	}
	if (height < 0x113) {
		if (core.playerstate.car_trackdata3_index == -1) {
			tile_index = core.game_player_route_anchor;
search:
			if (trackdata_penalty_related[tile_index] != -1)
				goto check_lap;
			hasPenalty = 0;
			track_side = 0;
			do {
				hasPenalty = gettlistpoint(tile_index, &core.playerstate.car_route_center, track_side, 0);
				offset_vector = core.playerstate.car_route_center;
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
			if (core.game_route_recovery_mode == 2) {
				if (trackSide == 0) {
					gettlistpoint(tile_index, pathPoints, 0, 0);
					gettlistpoint(tile_index, playerEdges, 1, 0);
				} else {
					gettlistpoint(tile_index, pathPoints, (I8)(trackSide - 1), 0);
					gettlistpoint(tile_index, playerEdges, trackSide, 0);
				}
				height = intatan(pathPoints[0].x - playerEdges[0].x, playerEdges[0].z - pathPoints[0].z) & ANGLE_TURN_MASK;
				height = (core.playerstate.car_rotate.x - height) & ANGLE_TURN_MASK;
				if (height > ANGLE_SEVEN_EIGHTHS_TURN || height < ANGLE_EIGHTH_TURN) {
					core.game_route_recovery_mode = 0;
					core.game_route_confirm_count = 1;
					core.playerstate.car_trackdata3_index = tile_index;
					core.playerstate.car_route_point_index = trackSide;
				}
			} else {
				core.playerstate.car_trackdata3_index = core.game_player_route_anchor;
				core.playerstate.car_route_point_index = trackSide;
			}
		}

		if (gettlistpoint(core.playerstate.car_trackdata3_index, &core.playerstate.car_route_center,
		              core.playerstate.car_route_point_index++, 0) != 0) {
			if (trackdata_penalty_related[core.game_player_route_anchor] != -1)
				core.playerstate.car_trackdata3_index = -1;
			else
				core.playerstate.car_trackdata3_index = g_td01_track_filecpy[core.game_player_route_anchor];
			core.playerstate.car_route_point_index = 0;
		}
	}
	offset_vector = core.playerstate.car_route_center;
	if (core.playerstate.car_trackdata3_index != -1 && core.game_route_recovery_mode == 0) {
		offset_vector.x -= core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (offset_vector.y == -1)
			offset_vector.y = 0;
		else
			offset_vector.y -= core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		offset_vector.z -= core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT; /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		planeMatrix = makematrix(core.playerstate.car_rotate.z, core.playerstate.car_rotate.y,
		                          core.playerstate.car_rotate.x, 1);
		mat_vec(&offset_vector, planeMatrix, &player_plane_vec);
		core.playerstate.car_route_bearing = intatan(-player_plane_vec.x, player_plane_vec.z) & ANGLE_TURN_MASK;
		if (core.playerstate.car_crashBmpFlag == 0) {
			switch (((core.playerstate.car_route_bearing + ANGLE_EIGHTH_TURN) & ANGLE_TURN_MASK) >> 8) {
			case 1:
				core.game_player_guidance = 1;
				break;
			case 3:
				if (core.playerstate.car_route_has_link == 0) {
					core.game_player_guidance = 2;
					break;
				}
			default:
				core.game_player_guidance = 0;
				break;
			}
		}
	}
check_lap:
	if (core.playerstate.car_finish_route_passed != 0) {
		height = fixedmult(intcos(st_hdg),
			row_ctr_zs[tagtrk] - (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		height += fixedmult(intsin(st_hdg),
			trackctrpos2[idxtrk] - (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT)); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
		if (height < 0)
			finishgame(3, 0);
	}
}

I8 findcar(I16 *trackIndex, I16 *penaltyCounter)
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
    do {
        if (carCol < 0 || carCol > 0x1D || playerRow < 0 || playerRow > 0x1D)
            break;
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
                break;
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
    } while (0);

    *penaltyCounter = -2;
    return 1;
}

/* Purpose: Updates gear, speed, and engine state for one car.
 * Parameters: inputByte, isOpponent, carState, carSetup.
 * Returns: none.
 * Globals read: core, opponent_spd_tbl, rate_frame
 * Globals written: core
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

/* Local view: update_car_speed; parameters[0] is signed byte; another unit uses signed word. */
void doengine(I8 inputByte, I8 isOpponent, struct CARSTATE* carState, struct SIMD* carSetup) {
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
				carState->car_changing_gear = 1;
				carState->car_fpsmul2 = ((I8)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame >> 1) + /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
				carState->car_knob_x2 = carSetup->knob_points[carState->car_current_gear].px;
				carState->car_knob_y2 = carSetup->knob_points[carState->car_current_gear].py;
			}
		} else if (carState->car_currpm < carSetup->downshift_rpm) {
downshift:
			if (carState->car_current_gear > 1) {
				carState->car_current_gear--;

				carState->car_changing_gear = 1;
				carState->car_fpsmul2 = ((I8)/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame >> 1) + /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
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

		speedDelta -= carSetup->braking_eff;
	} else {
		switch (inputByte & 3) {
		case 2:
			carState->car_is_accelerating = 0;
			carState->car_engineLimiterTimer = 0;
			carState->car_is_braking = 1;
			if (isOpponent == 0)
				speedDelta -= carSetup->braking_eff;
			else
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
	} else if (-speedDelta > updatedSpeed) {
		updatedSpeed = 0;
	} else {
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

	carState->car_currpm = calcrpm(carState->car_currpm, carState->car_speed,
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

void dohandling(struct CARSTATE *car, struct SIMD *simd, I16 isOpponent)
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

    demandedGrip = (((U16 )speed * speed) >> 6) * scratch; /* PORT: The MSC unsigned-int product wraps at 16 bits before scaling. */
    totalGrip = simd->grip * 2;
    totalGrip = (((I32)(simd->sliding[car->car_surfaceWhl[0]] +
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
            limit = (((I32)totalGrip << 8) /
                                  ((I32)speed * speed));
            if (baseAngleValue < 0)
                limit = -1 * limit;
            limit = (limit * 3 + baseAngleValue) >> 2;
            car->car_grip_angle_excess = baseAngleValue - limit;
        } else {
            car->car_slidingFlag = 0;
            if (car->car_grip_angle_excess != 0) {
                car->car_grip_angle_excess -= car->car_grip_angle_excess >> 4;
                if ((car->car_grip_angle_excess < 0 ? -car->car_grip_angle_excess : car->car_grip_angle_excess) < 16)
                    car->car_grip_angle_excess >>= 1;
            }
        }

        if (car->car_angle_z == 0 && car->car_crashBmpFlag != 1)
            car->car_40MfrontWhlAngle = limit;
        else
            car->car_40MfrontWhlAngle = 0;

        if (car->car_rotate.z != 0 &&
            (car->car_rotate.z < 0 ? -car->car_rotate.z : car->car_rotate.z) > 4) {
                column = ((U32 )car->car_posWorld1.lx >> 16);
                rowCoord = ((U32 )car->car_posWorld1.lz >> 16);

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
                car->car_speed2 = fixedmult(
                    intcos(car->car_36MwhlAngle), car->car_speed2);
                if (intcos(car->car_36MwhlAngle) < 0)
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
        scratch = (car->car_grip_angle_excess < 0 ? -car->car_grip_angle_excess : car->car_grip_angle_excess) << 1;
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
                    car->car_audio_flags |= 2;
                else
                    car->car_audio_flags |= 4;
            }
        } else {
            car->car_speed = 0;
            car->car_speed2 = 0;
        }
    }

    car->car_grip_angle_excess = 0;
}

I8 calccoll(struct CARSTATE *player, struct CARSTATE *opponent)
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

    player->car_contact_processed = 1;
    opponent->car_contact_processed = 1;
    currentSpeed = player->car_speed2;
    opponentSpeed = opponent->car_speed2;
    pHeading = player->car_rotate.x;
    opponentHeading = opponent->car_rotate.x;
    playerSin = fixedmult(currentSpeed >> Q8_FRACTION_BITS, intsin(pHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    opponentSin = fixedmult(opponentSpeed >> Q8_FRACTION_BITS, intsin(opponentHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    playerCosAngle = fixedmult(currentSpeed >> Q8_FRACTION_BITS, intcos(pHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    opponentCosAng = fixedmult(opponentSpeed >> Q8_FRACTION_BITS, intcos(opponentHeading)); /* PORT: Q8 value is converted at the legacy boundary. */
    distanceToCar = inthypot(opponentSin - playerSin, opponentCosAng - playerCosAngle);
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

I16 doshock(struct CARSTATE *car, I16 value, I16 wheel)
{
    I16S oldRc;
    I16S target;
    I16S adjustment;

    oldRc = car->car_wheel_suspension_offset[wheel];
    target = 0;
    adjustment = 0;
    if (car->car_wheel_suspension_target[wheel] != 0) {
        if (car->car_wheel_suspension_target[wheel] < 0) {
            car->car_wheel_suspension_target[wheel] += 4;
            if (car->car_wheel_suspension_target[wheel] > adjustment)
                car->car_wheel_suspension_target[wheel] = adjustment;
        } else {
            car->car_wheel_suspension_target[wheel] -= 4;
            if (car->car_wheel_suspension_target[wheel] < adjustment)
                car->car_wheel_suspension_target[wheel] = adjustment;
        }
    }
    WRITE_BACK(car->car_wheel_suspension_target[wheel]);

    if (value < 0 && car->car_wheel_suspension_offset[wheel] > -value)
        value = 0;
    if (value == 0) {
        if (car->car_wheel_suspension_offset[wheel] > car->car_wheel_suspension_target[wheel]) {
            car->car_wheel_suspension_offset[wheel] -= 0x80;
            if (car->car_wheel_suspension_offset[wheel] < car->car_wheel_suspension_target[wheel])
                car->car_wheel_suspension_offset[wheel] = car->car_wheel_suspension_target[wheel];
            target = oldRc - car->car_wheel_suspension_offset[wheel];
        } else if (car->car_wheel_suspension_offset[wheel] < car->car_wheel_suspension_target[wheel]) {
            car->car_wheel_suspension_offset[wheel] += 0x80;
            if (car->car_wheel_suspension_offset[wheel] > car->car_wheel_suspension_target[wheel])
                car->car_wheel_suspension_offset[wheel] = car->car_wheel_suspension_target[wheel];
        }
    } else if (value > 0) {
        if (value > 0xC0)
            car->car_wheel_suspension_offset[wheel] += 0xC0;
        else
            car->car_wheel_suspension_offset[wheel] += value;
        if (car->car_wheel_suspension_offset[wheel] > 0x180)
            car->car_wheel_suspension_offset[wheel] = 0x180;
        car->car_rc4[wheel] = 0;
    } else {
        if (value + car->car_wheel_suspension_offset[wheel] > -0x120) {
            car->car_wheel_suspension_offset[wheel] += value;
        } else {
            car->car_wheel_suspension_offset[wheel] += (value * 3) >> 2;
            if (car->car_wheel_suspension_offset[wheel] < -0x180)
                car->car_wheel_suspension_offset[wheel] = -0x180;
        }
        target = oldRc - car->car_wheel_suspension_offset[wheel] + value;
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

void dosteering(I8 input)
{
    register I16 response;
    I8 responseIndex;
    register I16 oldAngle;

    oldAngle = core.playerstate.car_steeringAngle;
    responseIndex = ((core.playerstate.car_speed2 >> 10) & 0xFC);
    response = table_lookup[responseIndex + input];

    if (response > 0) {
        if (oldAngle < -1) response <<= 2;
    } else if (response != 0 && oldAngle > 1) {
        response <<= 2;
    }

    if (response == 0 && core.playerstate.car_speed2 != 0 && oldAngle != 0) {
        response = table_lookup[responseIndex + 1] * 2;
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

    if (table_lookup[responseIndex + input] == 0 &&
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

void myupdatesfx(void)
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
        if (inrepflg != replay_state_cache)
            reset_audio_driver_state();
        replay_state_cache = inrepflg;
        return;
    }

    playerPosOld.x = (core.playerstate.car_posWorld2.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosOld.y = (core.playerstate.car_posWorld2.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosOld.z = (core.playerstate.car_posWorld2.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.x = (core.playerstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.y = (core.playerstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    playerPosition.z = (core.playerstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    if (globalgamesettings.game_opponenttype != 0) {
        opponentPrior.x = (core.opponentstate.car_posWorld2.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentPrior.y = (core.opponentstate.car_posWorld2.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentPrior.z = (core.opponentstate.car_posWorld2.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.x = (core.opponentstate.car_posWorld1.lx >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.y = (core.opponentstate.car_posWorld1.ly >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
        opponentCurrent.z = (core.opponentstate.car_posWorld1.lz >> WORLD_COORDINATE_SCALE_SHIFT); /* PORT: Signed world-to-map shift is arithmetic; scale is 64:1. */
    }

    switch (cammd) {
    case 0:
    case 2:
        if (intraffic != 0) {
            targetNow = opponentCurrent;
            targetPast = opponentPrior;
        } else {
            targetNow = playerPosition;
            targetPast = playerPosOld;
        }
        break;
    case 1:
        targetNow = core.game_chase_camera_pos[intraffic];
        targetPast = core.game_chase_camera_prev[intraffic];
        break;
    case 3:
        targetNow.x = ((struct VECTOR far *)trkptrpath)[core.game_track_camera_index[intraffic]].x;
        targetNow.y = ((struct VECTOR far *)trkptrpath)[core.game_track_camera_index[intraffic]].y + viewyshift + 90;
        targetNow.z = ((struct VECTOR far *)trkptrpath)[core.game_track_camera_index[intraffic]].z;
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
        if (selectedCar->car_audio_flags & 1) {
            if (!(soundMode & 1)) {
                soundMode |= 1;
                /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk(audioId);
            }
        } else if (soundMode & 1) {
            soundMode--;
            /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_function2(audioId);
        }
        if (selectedCar->car_audio_flags & 6) {
            if ((soundMode & 6) != (selectedCar->car_audio_flags & 6)) {
                if (soundMode & 6)
                    goto stop_skid;
                if (selectedCar->car_audio_flags & 2) {
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


    replay_state_cache = inrepflg;
}

/* Purpose: Dispatches pending audio events selected by the state flags.
 * Parameters: flags, audioId.
 * Returns: none.
 * Globals read: sndpendingstate
 * Globals written: none detected
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

void audio_dispatch_car_events(I8 flags, I16S audioId) { if (sndpendingstate != 0) { if (flags & 0x10) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk4(audioId); if (flags & 0x20) /* PLATFORM(audio): Legacy audio service or audio-resource loading. */ audio_op_unk3(audioId); } }

/* Purpose: Applies a recorded audio frame to the player and opponent.
 * Parameters: record, value.
 * Returns: none.
 * Globals read: g_player_sound_id, globalgamesettings, op_eng_sound_id
 * Globals written: none detected
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 */

/* Local view: myupdatesfx1; parameters[0].pointer is struct AUDIO_CAR_FRAME; another unit uses signed byte. */
void myupdatesfx1(struct AUDIO_CAR_FRAME *record, I16S value)
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

/* Local view: gettlistpoint; return is signed byte; another unit uses signed word. */
I8 gettlistpoint(I16 trackIndex, struct TRACKRESULT *result, I8 side,
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

    entry = road_trk[trackIndex];
    objectIndex = td_18_ref[trackIndex] & 0x0f;
    isConnected = td_18_ref[trackIndex] & 0x10;
    trackElemObject = &trklst[entry];
    infoBase = (struct TRKOBJINFO far *)trackElemObject->ss_trkObjInfoPtr;
    objectInfo = infoBase + objectIndex;
    linkPresent = 0;
    arrow = objectInfo->si_arrowType;

    if (isConnected == 0)
        edge = (side * 2);
    else
        edge = ((arrow - side) * 2 - 2);

    if (opponentSpeed != 0) {
        scratch = objectInfo->si_oppSpedCode;
        trackRow = trackElemObject->ss_surfaceType;
        *opponentSpeed = opponent_spd_tbl[trackRow + scratch];
    }

    if (objectInfo->link.dataPointer != 0)
        linkPresent = 1;

    if (isConnected != 0) {
        if (linkPresent != 0) {
            trackData = (struct VECTOR far *)objectInfo->link.dataPointer; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */
            edgeData = trackData + edge;
            vectorA = edgeData[0];
            edgePoint = edgeData[1];
        } else {
            trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */
            edgeData = trackData + edge;
            vectorA = edgeData[1];
            edgePoint = edgeData[0];
        }
    } else {
        trackData = (struct VECTOR far *)objectInfo->si_cameraDataOffset; /* PORT: Far-pointer stepping must preserve DOS segment:offset normalization. */

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

    scratch = g_column_of_trkdata21_pth[trackIndex];
    trackRow = tdfrompathrow22[trackIndex];
    if (vectorA.y != -1 && td15p_9[gterrtrk[trackRow] + scratch] == 6) {
        vectorA.y += terheight[1];
        edgePoint.y += terheight[1];
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
    result->has_opponent_link = linkPresent;

    return (arrow - 1) == side;
}

I8 boxcheck(struct POINT2D *pCollPoints,
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
    if ((unsigned)int3dhypot(&result) > (unsigned)reach)
        return 0;

    matrix = makematrix(-pWorldCrds[1].x, -pWorldCrds[1].y,
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

    matrix = makematrix(oWorldCrds[1].x, oWorldCrds[1].y,
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

    matrix = makematrix(-oWorldCrds[1].x, -oWorldCrds[1].y,
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

    matrix = makematrix(pWorldCrds[1].x, pWorldCrds[1].y,
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

void initlogo(void)
{
    register I16S zeroValue;
    initsim(-3);
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
    initcar(&core.opponentstate, &ophys_7, 1,
        0x17700L, 0L, ((I32)(r_zp[28] + 0x12E)) << WORLD_COORDINATE_SCALE_SHIFT, 0); /* PORT: Map-to-world scale is 64:1. */
    gettlistpoint(((I16S far *)td3)[core.opponentstate.car_trackdata3_index],
        &core.opponentstate.car_route_center, core.opponentstate.car_route_point_index++,
        &core.game_opponent_speed_limit);
}

void movelogo(void) { moveopponent(); }

/* Purpose: Applies crash transitions and updates engine audio state.
 * Parameters: crashMode, isOpponent.
 * Returns: none.
 * Globals read: core, elaptm1, endhsdemo, g_player_sound_id, inrepflg, op_eng_sound_id,
 *            rate_frame, sndpendingstate
 * Globals written: core, race_stats
 * PLATFORM(audio): Legacy audio service or audio-resource loading.
 * PLATFORM(timer): Timer-selected frame rate or timer position.
 */

void finishgame(I16 crashMode, I16 isOpponent) {
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
		core.game_end_delay_elapsed = 1;
		core.game_end_delay_limit = 1;
		break;
	case 5:
		crashMode = 1;
		suppress_car_speed = 1;
	case 1:
		var_cState->car_crashBmpFlag = 1;
		init_explode(isOpponent, var_cState->car_rotate.x, 0);
		if (isOpponent == 0) {
			core.game_impactSpeed = var_cState->car_speed2;
			core.game_end_delay_limit = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
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
			core.game_end_delay_limit = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame << 2;
		}
		break;
	case 3:
		var_cState->car_crashBmpFlag = 3;
		if (isOpponent == 0) {
			core.game_total_finish = core.game_frame + core.game_penalty + elaptm1;
			core.game_end_delay_limit = /* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame;
		} else {
			core.game_opponent_finish_time = core.game_frame + elaptm1;
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
	if ((didrace & 4) == 0)
		race_stats = *(struct GAMESTATE_SNAPSHOT *)&core.game_speed_frame_sum;
}

void move_on_plane(void) {
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
			rotation = intatan(-rotatedVector.x, rotatedVector.z);
		}
		if ((rotation += frwhl_angadjusted) != 0) {
			if (last_track_rotation != rotation) {
				xformy(&plane_rotation_cache, -rotation);
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
			xformy(&fallback_rotation_cache, -frwhl_angadjusted);
			f36f40_whlData = frwhl_angadjusted;
		}
		mat_vec(&tvec2, &fallback_rotation_cache, &rotatedVector);
		mat_vec(&rotatedVector, &matrix_transform_view, &pln_rot_output);
		return;
	}
	mat_vec(&tvec2, &matrix_transform_view, &pln_rot_output);
}

I16 plane_distance(I16 planeIndex, I16 x, I16 y, I16 z) {
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

void init_explode(I16 mode, I16S angle, I16S speed)
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
    core.game_debris_active = 1;
    unusedCount = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex)
        if (core.game_debris_planar_step[debrisIndex] == 0)
            ++unusedCount;
    if (unusedCount > countLimit) unusedCount = countLimit;

    made = 0;
    for (debrisIndex = 0; debrisIndex < 24; ++debrisIndex) {
        if (core.game_debris_planar_step[debrisIndex] == 0) {
            core.game_debris_origin_kind[debrisIndex] = mode;
            core.game_debris_shape_index[debrisIndex] = ((made & 3) + phaseBase);
            core.game_debris_x[debrisIndex] = 0;
            core.game_debris_y[debrisIndex] = 0;
            core.game_debris_z[debrisIndex] = 0;
            core.game_debris_rot_x[debrisIndex] = (myrandom() << 2);
            core.game_debris_rot_y[debrisIndex] = (myrandom() << 2);
            core.game_debris_rot_z[debrisIndex] = ((((I32)speedFactor * made) / unusedCount + startAngle) & ANGLE_TURN_MASK);
            verticalValue = (((myrandom() * 6) >> 2) + speed + 0x180);
            core.game_debris_planar_step[debrisIndex] = verticalValue;
            core.game_debris_vertical_step[debrisIndex] = ((verticalStep * verticalValue) >> 2);
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
        if (core.game_debris_planar_step[i] != 0) {
            struct VECTOR inputVector;
            struct VECTOR outputVector;
            matrixPointer = makematrix(0, 0, core.game_debris_rot_z[i], 1);
            inputVector.x = 0;
            inputVector.y = 0;
            inputVector.z = core.game_debris_planar_step[i];
            mat_vec(&inputVector, matrixPointer, &outputVector);
            core.game_debris_x[i] += outputVector.x;
            core.game_debris_z[i] += outputVector.z;
            core.game_debris_vertical_step[i] -= 0x13;
            core.game_debris_y[i] += core.game_debris_vertical_step[i];
            if (/* PLATFORM(timer): Uses the timer-selected frame rate or timer position. */ rate_frame == 10) {
                core.game_debris_vertical_step[i] -= 0x13;
                core.game_debris_y[i] += core.game_debris_vertical_step[i];
            }
            if (core.playerstate.car_posWorld1.ly + core.game_debris_y[i] < 0) {
                core.game_debris_planar_step[i] = 0;
            } else {
                keepAlive = 1;
                core.game_debris_rot_x[i] += 0x10;
                core.game_debris_rot_y[i] += 0x10;
            }
        }
    }
    core.game_debris_active = keepAlive;
}
/* Local view: locate_shape_alt; return.pointer is signed byte; another unit uses void. */
extern I8 far *getres(I8 far *data, I8 *name);




/* Local view: setup_aero_trackdata; parameters[0].pointer is void; another unit uses signed byte. */
void getsimdata(void far *carresptr, I16 is_opponent)
{
    register I16 i;
    if (is_opponent == 0) {
        simdp7 = *(struct SIMD far *)getres(carresptr, "simd");
        simdp7.aerorestable = track04_plyraero;
        for (i = 0; i < 0x40; i++) {
            track04_plyraero[i] = ((I32)simdp7.aero_resistance * i * i) >> 9;
        }
        strcpyfar(textstr, getres(carresptr, "gnam"));
    } else {
        ophys_7 = *(struct SIMD far *)getres(carresptr, "simd");
        ophys_7.aerorestable = trackdata_05_opp_aerotbl;
        for (i = 0; i < 0x40; i++) {
            trackdata_05_opp_aerotbl[i] = ((I32)ophys_7.aero_resistance * i * i) >> 9;
        }
        strcpyfar(g_gsnashape_data, getres(carresptr, "gsna"));
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
/* Local view: rate_frame; type is signed word; another unit uses unsigned word. */
I16S rate_frame;
/* Local view: road_trk; pointer is signed byte; another unit uses unsigned byte. */
I8 far* road_trk;
I16S sndposrecord;
/* Local view: table_lookup; pointer is signed byte; another unit uses void. */
I8 *table_lookup;
I16S far* track04_plyraero;
I16S far* trackdata_05_opp_aerotbl;
unsigned char opponent_spd_tbl[16];
/* Local view: plncurrptr; pointer is struct PLANE; another unit uses unsigned byte. */
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
/* Local view: g_planlist; pointer is struct PLANE; another unit uses unsigned byte. */
struct PLANE far* g_planlist;
/* Local view: td_18_ref; pointer is signed byte; another unit uses unsigned byte. */
I8 far* td_18_ref;
/* Local view: st_hdg; type is signed word; another unit uses unsigned word. */
I16S st_hdg;
/* Local view: g_column_of_trkdata21_pth; pointer is signed byte; another unit uses unsigned byte. */
I8 far* g_column_of_trkdata21_pth;
struct SIMD ophys_7;
I8 sndpendingstate;
/* Local view: tdfrompathrow22; pointer is signed byte; another unit uses unsigned byte. */
I8 far* tdfrompathrow22;
I16 nextpos_normalip;
I16S g_cvxintvl;
/* Local view: cvxs_a; segment is huge; another unit uses far. */
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




