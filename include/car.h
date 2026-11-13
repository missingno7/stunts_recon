#ifndef STUNTS_CAR_H
#define STUNTS_CAR_H
#include "math3d.h"

/* Car parameters and simulation services. */
/* Shared carrec declarations; DOS medium-model types. */
struct car_exp_name_table;
#pragma pack()
struct car_exp_name_table {
    char entry[15][5];
    char trailing_zero;
};
/* Shared car declarations; unnamed prototype parameters keep C1 allocation stable. */
struct CARSTATE;
struct SIMD;
struct VECTOR;
/* State shared by the simulation and the renderer. */
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
    short car_grip_angle_excess;
    short car_demandedGrip;
    short car_surfacegrip_sum;
    short car_route_bearing;
    short car_trackdata3_index;
    short car_wheel_fall_step[4];
    short car_wheel_suspension_offset[4];
    short car_rc3[4];
    short car_rc4[4];
    short car_wheel_suspension_target[4];
    struct VECTOR car_whlWorldCrds1[4];
    struct VECTOR car_whlWorldCrds2[4];
    struct VECTOR car_route_center;
    struct VECTOR car_route_edge_a;
    struct VECTOR car_route_edge_b;
    short car_route_has_link;
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
    char car_contact_processed;
    char car_crashBmpFlag;
    char car_changing_gear;
    char car_fpsmul2;
    char car_transmission;
    char car_finish_route_passed;
    unsigned char car_route_point_index;
    unsigned char car_audio_flags;
};
extern struct VECTOR car_dvecs[6];
extern struct VECTOR car_menu_car_position;
extern int car_rotate_xc;
extern int car_roty_pln;
extern struct SIMD simdp7;
extern unsigned short speed_recovery_divisors[5];
extern short far *track04_plyraero;
extern short far *trackdata_05_opp_aerotbl;
extern unsigned int far calcrpm(unsigned int, unsigned int, unsigned int, int, unsigned int);
extern void far movecar(struct CARSTATE *, struct SIMD *, struct CARSTATE *, struct SIMD *, char);

/* Car simulation state and services. */
struct CARSTATE;
struct VECTOR;
struct VECTORLONG;

extern unsigned char car_wheel_offsets[8];
extern short st_hdg;
extern char steerWhlRespTable_10fps[];
extern char steerWhlRespTable_20fps[];
extern void getsimdata(void far *, int);
extern void settires(struct VECTOR far*, int, short*, short*, struct VECTOR*, struct VECTOR*);
#endif
