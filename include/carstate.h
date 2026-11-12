/* Canonical CARSTATE view; incompatible operational views stay local. */
#ifndef STUNTS_CARSTATE_H
#define STUNTS_CARSTATE_H
struct VECTOR;
struct VECTORLONG;
#include "vec3.h"
#pragma pack()
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
#endif
