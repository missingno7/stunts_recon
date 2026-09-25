struct VECTOR { short x, y, z; };
struct VECTORLONG { long lx, ly, lz; };
struct POINT2D { int px, py; };
struct CARSTATE {
    struct VECTORLONG car_posWorld1, car_posWorld2;
    struct VECTOR car_rotate;
    short car_pseudoGravity, car_steeringAngle, car_currpm, car_lastrpm, car_idlerpm2;
    short car_speeddiff;
    unsigned short car_speed, car_speed2, car_lastspeed, car_gearratio, car_gearratioshr8;
    short car_knob_x, car_36MwhlAngle, car_knob_y, car_knob_x2, car_knob_y2;
    short car_angle_z, car_40MfrontWhlAngle, field_42, car_demandedGrip;
    short car_surfacegrip_sum, field_48, car_trackdata3_index;
    short car_rc1[4], car_rc2[4], car_rc3[4], car_rc4[4], car_rc5[4];
    struct VECTOR car_whlWorldCrds1[4], car_whlWorldCrds2[4];
    struct VECTOR car_vec_unk3, car_vec_unk4, car_vec_unk5;
    short field_B6, field_B8, field_BA;
    char car_is_braking, car_is_accelerating, car_current_gear;
    char car_sumSurfFrontWheels, car_sumSurfRearWheels, car_sumSurfAllWheels;
    char car_surfaceWhl[4], car_engineLimiterTimer, car_slidingFlag, field_C8;
    char car_crashBmpFlag, car_changing_gear, car_fpsmul2, car_transmission;
    char field_CD, field_CE, field_CF;
};
struct SIMD {
    char num_gears, simd_unk;
    short car_mass, braking_eff, idle_rpm, downshift_rpm, upshift_rpm, max_rpm;
    unsigned short gear_ratios[7];
    struct POINT2D knob_points[7];
};
void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    char transmission, long posX, long posY, long posZ, short track_angle)
{
    register int zero = 0;
    register int i;
    struct VECTOR whlPos;
    playerstate->car_posWorld1.lx = posX;
    playerstate->car_posWorld2.lx = posX;
    playerstate->car_posWorld1.ly = posY + 512;
    playerstate->car_posWorld2.ly = posY;
    playerstate->car_posWorld1.lz = posZ;
    playerstate->car_posWorld2.lz = posZ;
    playerstate->car_rotate.x = track_angle;
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
    whlPos.x = posX >> 6;
    whlPos.y = posY >> 6;
    whlPos.z = posZ >> 6;
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






