/* Shared car declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_CAR_H
#define STUNTS_CAR_H
struct CARSTATE;
struct SIMD;
struct VECTOR;
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
#endif
