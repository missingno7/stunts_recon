
struct VECTOR {
	short x, y, z;
};


struct VECTORLONG {
	long lx, ly, lz;
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
	short game_frame;
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

extern struct GAMESTATE state;

extern void *steerWhlRespTable_ptr;

extern char far *td16_rpl_buffer;

extern signed char byte_44292[64];

extern unsigned char byte_442EA[64];

void far replay_unk(void)
{
    register int frame_index = state.game_frame & 0x3F;
    register int steering;
    char speed_index;
    char response;
    char angle;

    if (byte_442EA[frame_index] == 0)
        return;

    steering = byte_44292[frame_index];
    speed_index = (char)((state.playerstate.car_speed2 >> 10) & 0xFC);
    response = ((char*)steerWhlRespTable_ptr)[(int)speed_index + 1];

    if (state.playerstate.car_steeringAngle < steering) {
        if (state.playerstate.car_steeringAngle < -1)
            response <<= 2;
    } else if (state.playerstate.car_steeringAngle > steering) {
        if (state.playerstate.car_steeringAngle > 1)
            response <<= 2;
    }

    if (state.playerstate.car_steeringAngle > steering &&
        state.playerstate.car_steeringAngle - response >= steering) {
        angle = 8;
    } else if (state.playerstate.car_steeringAngle < steering &&
               state.playerstate.car_steeringAngle + response <= steering) {
        angle = 4;
    } else {
        angle = 0;
    }

    if (angle != 0)
        td16_rpl_buffer[state.game_frame] |= angle;
    byte_442EA[frame_index] = 0;
}
