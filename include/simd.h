/* Canonical SIMD view; incompatible operational views stay local. */
#ifndef STUNTS_SIMD_H
#define STUNTS_SIMD_H
struct POINT2D;
struct VECTOR;
#include "vec3.h"
#pragma pack()
struct SIMD {
    char num_gears;
    char simd_unk;
    unsigned short car_mass;
    short braking_eff;
    short idle_rpm;
    unsigned short downshift_rpm;
    unsigned short upshift_rpm;
    unsigned short max_rpm;
    unsigned short gear_ratios[7];
    struct POINT2D knob_points[7];
    short aero_resistance;
    unsigned char idle_torque;
    unsigned char torque_curve[104];
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
    short far *aerorestable;
};
#endif
