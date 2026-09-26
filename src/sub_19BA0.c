extern short data_349D0;



struct VECTOR {
	short x, y, z;
};

struct VECTORLONG {
	long lx, ly, lz;
};

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
	short field_42;
	short car_demandedGrip;
	short car_surfacegrip_sum;
	short field_48;
	short car_trackdata3_index;
	short car_rc1[4]; 
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
	char car_sumSurfAllWheels; 
	char car_surfaceWhl[4];      
	char car_engineLimiterTimer;
	char car_slidingFlag;
	char field_C8;
	char car_crashBmpFlag;
	char car_changing_gear;
	char car_fpsmul2;
	char car_transmission;
	char field_CD;
	unsigned char field_CE; 
	char field_CF; 
};

struct GAMESTATE {
	long game_longs1[24]; 
	long game_longs2[24]; 
	long game_longs3[24]; 
	struct VECTOR game_vec1[2]; 
	struct VECTOR game_vec3;
	struct VECTOR game_vec4;
	short game_frame_in_sec;
	short game_frames_per_sec;
	long  game_travDist;
	unsigned short game_frame;
	short game_total_finish; 
	short field_144;
	short game_pEndFrame;
	short game_oEndFrame;   
	short game_penalty; 
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
	short field_3BE[24];
	char kevinseed[6];
	char field_3F4;
	char game_inputmode; 
	char game_3F6autoLoadEvalFlag;
	char field_3F7[2]; 
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

struct MATRIX;

extern struct GAMESTATE state;

struct MATRIX *mat_rot_zxy(int z, int x, int y, int unk);

void mat_mul_vector(struct VECTOR *invec, struct MATRIX *mat, struct VECTOR *outvec);



void sub_19BA0(void)
{
    struct MATRIX *matrixPointer;
    char keepAlive = 0;
    register int i;
    for (i = 0; i < 24; ++i) {
        if (state.field_38E[i] != 0) {
            struct VECTOR inputVector;
            struct VECTOR outputVector;
            matrixPointer = mat_rot_zxy(0, 0, state.field_35E[i], 1);
            inputVector.x = 0;
            inputVector.y = 0;
            inputVector.z = state.field_38E[i];
            mat_mul_vector(&inputVector, matrixPointer, &outputVector);
            state.game_longs1[i] += outputVector.x;
            state.game_longs3[i] += outputVector.z;
            state.field_3BE[i] -= 0x13;
            state.game_longs2[i] += state.field_3BE[i];
            if (data_349D0 == 10) {
                state.field_3BE[i] -= 0x13;
                state.game_longs2[i] += state.field_3BE[i];
            }
            if (state.playerstate.car_posWorld1.ly + state.game_longs2[i] < 0) {
                state.field_38E[i] = 0;
            } else {
                keepAlive = 1;
                state.field_2FE[i] += 0x10;
                state.field_32E[i] += 0x10;
            }
        }
    }
    state.field_42A = keepAlive;
}

