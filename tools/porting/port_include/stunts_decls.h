/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
/* Generated X2 SDL port API. Evidence is in declaration-evidence.json. */
#ifndef STUNTS_DECLS_H
#define STUNTS_DECLS_H
#include "stunts_types.h"
#include "platform_hw.h"
#include "stunts_constants.h"
#include "stunts_structs.h"

/* Target model: MSC medium model, far code, near DGROUP data, cdecl stack ABI.
   Host mode erases segment qualifiers; far data requires a separate handle/pointer layer. */

/* Forward declarations for opaque/TU-specific public aggregate views. */
struct AUDIOCHUNK;
struct AUDIOVOICE;
struct AUDIO_CAR_FRAME;
struct AudioChunk;
struct AudioEvent;
struct AudioVoice;
struct ENGINESOUND;
union FARRESOURCE;
struct GAMEINFO;
struct GAMESTATE;
struct GAMESTATE_SNAPSHOT;
struct HighScoreRecord;
struct MATRIX;
struct PLANE;
struct car_exp_name_table;
struct Point;
struct WheelRect;
typedef struct Point Point;
struct POINT2D;
struct RECTANGLE;
struct SCREEN_RECT;
struct SHAPE2D;
struct SHAPE3D;
struct SIMD;
struct SPRITE;
struct TRACKRESULT;
struct TRANSFORMEDSHAPE3D;
struct VECTOR;
struct WALLREC;
struct coord_pair;
struct scene_shape;
struct track_object;
struct track_object_info;

/* Registered/shared data symbols: exact state-model type where available. */
#ifndef STUNTS_LOCAL_DATA_HKeyFlag
extern char HKeyFlag; /* 0x2B8F4; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA__ctype
extern uint8_t _ctype[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aA00
extern char aA00[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aAvs
extern char aAvs[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBarn
extern char aBarn[116][5]; /* 0x2C1C0; complete 116-entry, five-byte resource-name table (src/track_constants_module.c:4; dseg.asm:3997 onward; 580 bytes). */
#endif
#ifndef STUNTS_LOCAL_DATA_aBct
extern char aBct[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBdr
extern char aBdr[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBev
extern char aBev[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBhi
extern char aBhi[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBmm_0
extern char aBmm_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBra
extern char aBra[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aBrp
extern char aBrp[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aCar0
extern struct car_exp_name_table aCar0; /* source definition: one 76-byte aggregate, entries[15][5] plus trailing_zero; obj_seg004.c has a legacy byte-matrix view (PORT note). */
#endif
#ifndef STUNTS_LOCAL_DATA_aCarcoun
extern char aCarcoun[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aCarcoun_0
extern char aCarcoun_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aCon
extern char aCon[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aCrs0crs1crs2crs3
extern char aCrs0crs1crs2crs3[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aCvx
extern char aCvx[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aD4a
extern char aD4a[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDash
extern char aDash[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDash_0
extern char aDash_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDasm
extern char aDasm[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDast
extern char aDast[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDefault_0
extern char aDefault_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDefault_1
extern char aDefault_1[10]; /* 0x2B8B0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDig0dig1dig2dig3dig4dig5d
extern char aDig0dig1dig2dig3dig4dig5d[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDm1
extern char aDm1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDm2
extern char aDm2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDnf
extern char aDnf[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDnf_0
extern char aDnf_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aDos
extern char aDos[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aElt
extern char aElt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aEokenseieemseedewwefuenpestej
extern char aEokenseieemseedewwefuenpestej[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aFlatlakelak1lak2lak3lak4highg
extern char aFlatlakelak1lak2lak3lak4highg[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aFontdef_fnt
extern char aFontdef_fnt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aFontn_fnt
extern char aFontn_fnt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aGame1
extern char aGame1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aGame2
extern char aGame2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aGnobgnabdotDotadot1dot2
extern char aGnobgnabdotDotadot1dot2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aHna
extern char aHna[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aIhd
extern char aIhd[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aImp
extern char aImp[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aInh
extern char aInh[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aInh_0
extern char aInh_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aJum
extern char aJum[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aKevin
extern char aKevin[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aLnam
extern char aLnam[6]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aLocateshape4_4sShapeNotF
extern char aLocateshape4_4sShapeNotF[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aLocatesound4_4sSoundNotF
extern char aLocatesound4_4sSoundNotF[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aLose
extern char aLose[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMain
extern char aMain[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMemoryManagerB
extern char aMemoryManagerB[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMisc_1
extern char aMisc_1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMisc_2
extern char aMisc_2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMph
extern char aMph[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMph_0
extern char aMph_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aMph_1
extern char aMph_1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOlt
extern char aOlt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOlt_0
extern char aOlt_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOp01
extern char aOp01[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOpp
extern char aOpp[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOpp1
extern char aOpp1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOpp2lose
extern char aOpp2lose[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOpp2win
extern char aOpp2win[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOpp_0
extern char aOpp_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOver
extern char aOver[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aOwt
extern char aOwt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aPen
extern char aPen[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aPpt
extern char aPpt[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aPre
extern char aPre[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aQ00
extern char aQ00[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aReservememoryO
extern char aReservememoryO[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aReservememoryOutOfMemory
extern char aReservememoryOutOfMemory[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aResizememoryCa
extern char aResizememoryCa[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aResizememoryNo
extern char aResizememoryNo[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aRoof
extern char aRoof[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aRoof_0
extern char aRoof_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aRoof_1
extern char aRoof_1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aRoof_2
extern char aRoof_2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aRpl_0
extern char aRpl_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSFileError
extern char aSFileError[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSFileError_0
extern char aSFileError_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSFileError_1
extern char aSFileError_1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSInvalidPackTy
extern char aSInvalidPackTy[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSe1
extern char aSe1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSe2
extern char aSe2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidms_0
extern char aSkidms_0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidms_1
extern char aSkidms_1[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidms_2
extern char aSkidms_2[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidover
extern char aSkidover[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidslct
extern char aSkidslct[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSkidvict
extern char aSkidvict[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aSlct
extern char aSlct[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aStdaxxxx
extern char aStdaxxxx[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aStdbxxxx
extern char aStdbxxxx[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aStxxx
extern char aStxxx[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aTedit__0
extern char aTedit__0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aTer0
extern char aTer0[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aTop
extern char aTop[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aTrakdata
extern char aTrakdata[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aUcr0ucr1ucr2ucr3
extern char aUcr0ucr1ucr2ucr3[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aVict
extern char aVict[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aWhl1whl2whl3ins2gboxins1i
extern char aWhl1whl2whl3ins2gboxins1i[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aWinn
extern char aWinn[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_aWww
extern char aWww[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_a_trk_5
extern char a_trk_5[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_ahead_tables
extern char * ahead_tables[8]; /* 0x2C084; state-model; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_ancv2
extern struct VECTOR ancv2; /* 0x32CC0; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_angle
extern int16_t angle; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_anglerotate_car
extern short anglerotate_car; /* 0x34998; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_animation_outline_color
extern short animation_outline_color; /* 0x307D2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_array_rpl
extern signed char array_rpl[64]; /* 0x34292; state-model; size=64 */
#endif
#ifndef STUNTS_LOCAL_DATA_atantable
extern unsigned char atantable[258]; /* 0x2F0C0; typeinfer machine-access widths/extent; size=258 */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_bit_masks
extern uint16_t audio_bit_masks[17]; /* 0x3060E; 17-word table follows the 4-byte driver pointer (dseg.asm:20635-20670; src/obj_seg027.c:153-154). */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_driver_extension_mode
extern unsigned char audio_driver_extension_mode; /* 0x30635; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_driver_mode
extern unsigned char audio_driver_mode; /* 0x30634; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_driver_volume_command
extern unsigned char audio_driver_volume_command[4]; /* 0x30636; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_frmarr
extern char audio_frmarr[1360]; /* 0x34F4C; state-model; size=1360 */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_load_error_policy
extern short audio_load_error_policy; /* 0x3063C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_opp_res
extern void * audio_opp_res; /* 0x3432C; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_pause_in_progress
extern unsigned char audio_pause_in_progress; /* 0x30630; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_song_ready
extern unsigned char audio_song_ready; /* 0x30632; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_tick_divider
extern int16_t audio_tick_divider; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_timer_reentry_lock
extern unsigned short audio_timer_reentry_lock; /* 0x307AA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audio_update_lock
extern short audio_update_lock; /* 0x3063A; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_audioblock
extern unsigned char audioblock[24]; /* 0x34ACA; state-model; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_audiochnk_actflags
extern unsigned char audiochnk_actflags[24]; /* 0x35D9A; state-model; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_audiochunktable
extern struct AUDIOCHUNK audiochunktable[24]; /* 0x3396C; state-model; size=1824 */
#endif
#ifndef STUNTS_LOCAL_DATA_audiodriverbinary
extern void far *audiodriverbinary; /* 0x3060A; four-byte far pointer (dseg.asm:20635; src/obj_seg027.c:153); next 34 bytes are audio_bit_masks[17]. */
#endif
#ifndef STUNTS_LOCAL_DATA_audiodriverstring
extern char audiodriverstring[5]; /* 0x30B12; typeinfer machine-access widths/extent; size=5 */
#endif
#ifndef STUNTS_LOCAL_DATA_audiofiletmp
extern char audiofiletmp[128]; /* 0x32D2E; state-model; size=128 */
#endif
#ifndef STUNTS_LOCAL_DATA_audioflag2
extern uint8_t audioflag2; /* 0x30631; byte flag, dseg.asm:20677; active source uses U8. */
#endif
#ifndef STUNTS_LOCAL_DATA_audioflag6
extern unsigned char audioflag6; /* 0x30633; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_audiooppflag
extern char audiooppflag; /* 0x32D2A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_backlightovr8
extern char backlightovr8; /* 0x35514; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_below
extern int16_t below; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_bios_tick_countdown
extern st_near_data_offset bios_tick_countdown[1]; /* 0x2F886; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_bitmapdash
extern short bitmapdash; /* 0x349EA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_bkRdEntr_triang_zAdjust
extern int16_t bkRdEntr_triang_zAdjust[4]; /* 0x2E5DE; typeinfer machine-access widths/extent; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_block_audio_num
extern unsigned char block_audio_num; /* 0x34290; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_boundglassrect
extern struct RECTANGLE boundglassrect; /* 0x33934; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_brav
extern struct SHAPE3D brav; /* 0x34CF0; state-model; size=22 */
#endif
#ifndef STUNTS_LOCAL_DATA_buf_g_path
extern char buf_g_path[94]; /* 0x34D68; state-model; size=94 */
#endif
#ifndef STUNTS_LOCAL_DATA_buf_obase
extern short buf_obase[5]; /* 0x3448A; state-model; size=10 */
#endif
#ifndef STUNTS_LOCAL_DATA_byte_349BA
extern char byte_349BA; /* 0x349BA; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_call_proc_flag
extern unsigned char call_proc_flag; /* 0x342E4; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_cam_idg
extern char cam_idg; /* 0x3432A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_aim_x
extern short camera_aim_x; /* 0x2C10E; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_aim_y
extern short camera_aim_y; /* 0x2C110; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_aim_z
extern short camera_aim_z; /* 0x2C112; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_button_count_mode
extern char camera_button_count_mode[10]; /* 0x2E9DC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_button_index
extern char camera_button_index; /* 0x2E9DB; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_mode_select_a
extern char camera_mode_select_a[10]; /* 0x2E9E6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_mode_select_b
extern char camera_mode_select_b[10]; /* 0x2E9F0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_mode_select_c
extern char camera_mode_select_c[10]; /* 0x2E9FA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_pos_x
extern short camera_pos_x; /* 0x2C108; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_pos_y
extern short camera_pos_y; /* 0x2C10A; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_pos_z
extern short camera_pos_z; /* 0x2C10C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_select_fill_color
extern short camera_select_fill_color; /* 0x307FC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_camera_select_outline_color
extern short camera_select_outline_color; /* 0x307FE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_cammd
extern char cammd; /* 0x2B8F5; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_car_dvecs
extern struct VECTOR car_dvecs[6]; /* 0x34466; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_car_menu_car_position
extern struct VECTOR car_menu_car_position; /* 0x2BB5E; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_car_rotate_xc
extern short car_rotate_xc; /* 0x34F46; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_car_roty_pln
extern short car_roty_pln; /* 0x34F44; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_car_wheel_offsets
extern unsigned char car_wheel_offsets[8]; /* 0x2E710; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_carcopyresourceptr
extern char * carcopyresourceptr; /* 0x354B0; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_carmenu_buttons_x1
extern int16_t carmenu_buttons_x1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_carmenu_buttons_x2
extern int16_t carmenu_buttons_x2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_carmenu_buttons_y1
extern int16_t carmenu_buttons_y1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_carmenu_buttons_y2
extern int16_t carmenu_buttons_y2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_carmenu_cliprect
extern struct RECTANGLE carmenu_cliprect; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_centerpos
extern int32_t centerpos; /* 0x349BE; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_chhtsample
extern void * chhtsample; /* 0x33928; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_cliprects_spare4
extern short cliprects_spare4[120]; /* 0x34E46; state-model; size=240 */
#endif
#ifndef STUNTS_LOCAL_DATA_clipunk
extern struct RECTANGLE clipunk; /* 0x2EB04; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_point_set_a
extern struct POINT2D collision_point_set_a[2]; /* 0x2BD5A; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_point_set_b
extern struct POINT2D collision_point_set_b[2]; /* 0x2BD62; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_point_set_c
extern struct POINT2D collision_point_set_c[2]; /* 0x2BD6A; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_point_x_signs
extern short collision_point_x_signs[4]; /* 0x2BE04; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_point_y_signs
extern short collision_point_y_signs[4]; /* 0x2BE0C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_response_offsets
extern short collision_response_offsets[4]; /* 0x2BD52; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_collision_rotation_offsets
extern short collision_rotation_offsets[4]; /* 0x2BD72; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_copy_mouse_modes
extern signed char copy_mouse_modes[8]; /* 0x35D0C; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_core
extern struct GAMESTATE core; /* 0x34494; state-model; size=1120 */
#endif
#ifndef STUNTS_LOCAL_DATA_corkLR_negZBound
extern int16_t corkLR_negZBound[12]; /* 0x2E5E6; typeinfer machine-access widths/extent; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_corkLR_posZBound
extern int16_t corkLR_posZBound[12]; /* 0x2E5FE; typeinfer machine-access widths/extent; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_cos80
extern int16_t cos80; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_coursedataappend_address
extern char * coursedataappend_address; /* 0x343F6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_ctrmesh
extern struct VECTOR ctrmesh; /* 0x32D04; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_ctype
extern unsigned char ctype[257]; /* 0x2EF9E; state-model; size=257 */
#endif
#ifndef STUNTS_LOCAL_DATA_cur_shps
extern struct TRANSFORMEDSHAPE3D cur_shps[29]; /* 0x32A3A; state-model; size=580 */
#endif
#ifndef STUNTS_LOCAL_DATA_cursorxposition
extern uint16_t cursorxposition; /* 0x34D3C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_custom_azim_angle
extern short custom_azim_angle; /* 0x2B8EE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_custom_dist
extern short custom_dist; /* 0x2B8EC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_custom_elev_angle
extern short custom_elev_angle; /* 0x2B8F0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_cvxs_a
extern struct GAMESTATE * cvxs_a; /* 0x35A20; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_dashbmpy_copy
extern int16_t dashbmpy_copy; /* 0x354AE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_dashbmy9
extern int16_t dashbmy9; /* 0x35DBA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_dashbtogglesaved
extern char dashbtogglesaved; /* 0x3644A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_dasm_shp_7
extern void * dasm_shp_7; /* 0x354A0; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_dastseg
extern int16_t dastseg; /* 0x3549E; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_dasty
extern int16_t dasty; /* 0x361C2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_def_fntadr
extern void * def_fntadr; /* 0x354C6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_detail_lvl
extern char detail_lvl; /* 0x2B8FA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_detthrlevel
extern char detthrlevel[6]; /* 0x2C09C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_dialogarg2
extern int16_t dialogarg2; /* 0x30802; typeinfer machine-access widths/extent; size=776 */
#endif
#ifndef STUNTS_LOCAL_DATA_digshapes
extern struct SHAPE2D * digshapes[10]; /* 0x30D88; state-model; size=40 */
#endif
#ifndef STUNTS_LOCAL_DATA_dlg_colour
extern short dlg_colour; /* 0x307CA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_elapsed_time1
extern int16_t elapsed_time1; /* 0x35A24; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_elaptm1
extern unsigned short elaptm1; /* 0x35A24; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_element_min_wall
extern short element_min_wall; /* 0x34988; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_endhsdemo
extern char endhsdemo; /* 0x33966; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_eng1resourceptr
extern void * eng1resourceptr; /* 0x354A6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_engdata
extern void * engdata; /* 0x35E0E; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_entry_score_index
extern unsigned char entry_score_index; /* 0x349CE; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_extra_rclist4
extern short extra_rclist4[2]; /* 0x355C4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_exwd
extern short exwd[3]; /* 0x35636; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_f36f40_whlData
extern int16_t f36f40_whlData; /* 0x2BE14; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_facenodeiterator
extern int16_t facenodeiterator; /* 0x343F2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_codes
extern unsigned char fence_codes[8]; /* 0x2C0E6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_off_1
extern char fence_off_1[2]; /* 0x2C0EE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_off_2
extern char fence_off_2[4]; /* 0x2C0F0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_off_3
extern char fence_off_3[4]; /* 0x2C0F4; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_off_4
extern char fence_off_4[8]; /* 0x2C0F8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_offsets
extern short fence_offsets[8]; /* 0x2BE34; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fence_shapes
extern struct SHAPE3D * fence_shapes[8]; /* 0x2BE44; state-model; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_findfilenames
extern char *findfilenames[4]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_findfiletexts
extern char *findfiletexts[4]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_flagsdown
extern int16_t flagsdown; /* 0x342E8; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_fntled_res
extern void * fntled_res; /* 0x359F4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_fntndat
extern void * fntndat; /* 0x34D22; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_follow_op
extern char follow_op; /* 0x363E0; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_font_secondary_color
extern unsigned short font_secondary_color; /* 0x2EB90; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fontdef_default
extern uint8_t fontdef_default[1408]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_fontdefvalue
extern uint16_t fontdefvalue; /* 0x359F2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_fontled_free_4
extern short fontled_free_4[2]; /* 0x359F8; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_framerate_pad_0
extern short framerate_pad_0; /* 0x349D2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_frm_rate2
extern unsigned short frm_rate2; /* 0x34D4E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_frmcs_time
extern short frmcs_time; /* 0x3499C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_frmexcess
extern int16_t frmexcess; /* 0x34DCC; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_frwhl_angadjusted
extern short frwhl_angadjusted; /* 0x34F48; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_function_key_scan_codes
extern unsigned short function_key_scan_codes[12]; /* 0x2ECBE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_g_animphase
extern int16_t g_animphase; /* 0x35D1C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_ascii_props
extern char byte_2EF9F[256]; /* 0x2EF9F; typeinfer machine-access widths/extent; size=256 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_audchnkvalue
extern unsigned char g_audchnkvalue[24]; /* 0x34D06; state-model; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_audio_frms_ix
extern int16_t g_audio_frms_ix; /* 0x34D1E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_audiodrvvoices_count
extern unsigned char g_audiodrvvoices_count; /* 0x359D2; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_clocks
extern uint16_t g_clocks; /* 0x36468; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_column_of_trkdata21_pth
extern char * g_column_of_trkdata21_pth; /* 0x3563C; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_corkscrew_type_flag
extern unsigned char g_corkscrew_type_flag; /* 0x34D46; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_cur_track_row
extern unsigned char g_cur_track_row; /* 0x35E16; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_curr_tsp
extern struct TRANSFORMEDSHAPE3D * g_curr_tsp; /* 0x36434; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_cursurfacekindvalue
extern unsigned char g_cursurfacekindvalue; /* 0x34D47; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_cvxintvl
extern short g_cvxintvl; /* 0x35A00; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_dastbmpbuf
extern struct SHAPE2D * g_dastbmpbuf; /* 0x3549C; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_drvaudiocode
extern char g_drvaudiocode[14]; /* 0x32DAE; state-model; size=14 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_game13dresource
extern void * g_game13dresource; /* 0x361C4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_gsnashape_data
extern char g_gsnashape_data[5]; /* 0x33967; state-model; size=5 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_hillf
extern char g_hillf; /* 0x3499B; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_hovercolor_idle
extern int16_t g_hovercolor_idle; /* 0x35D06; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_is_busy
extern uint8_t g_is_busy; /* 0x2B8FB; byte flag; active source definition uses uint8_t. */
#endif
#ifndef STUNTS_LOCAL_DATA_g_mat_clrlist_copy_2_ptr
extern int16_t * g_mat_clrlist_copy_2_ptr; /* 0x359D6; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_matrix_yrot
extern struct MATRIX g_matrix_yrot; /* 0x34D2A; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_miscfile_ptr
extern void * g_miscfile_ptr; /* 0x355CC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_mousesave_x_tbl
extern int16_t g_mousesave_x_tbl[4]; /* 0x3646A; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_mouseyposstacktable
extern int16_t g_mouseyposstacktable[5]; /* 0x36486; state-model; size=10 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_musicvolumesetting
extern unsigned char g_musicvolumesetting; /* 0x35950; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_op_carvector2
extern struct VECTOR g_op_carvector2; /* 0x32D0A; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_opp_resources
extern void * g_opp_resources[7]; /* 0x35D22; state-model; size=28 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_penaltytm
extern short g_penaltytm; /* 0x361CA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_planidx2
extern short g_planidx2; /* 0x34F4A; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_planlist
extern struct PLANE * g_planlist; /* 0x354C2; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_player_sound_id
extern short g_player_sound_id; /* 0x33964; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_plyr_snd_state
extern char g_plyr_snd_state; /* 0x32D26; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_replaybarcpytgl
extern char g_replaybarcpytgl; /* 0x355D2; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_road_piece_id
extern char g_road_piece_id; /* 0x35635; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_rot_mat_z
extern struct MATRIX g_rot_mat_z; /* 0x33952; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_rpl_init
extern unsigned char g_rpl_init; /* 0x3393C; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_rplbfask
extern char g_rplbfask; /* 0x36467; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_rplmodui
extern char g_rplmodui; /* 0x349E6; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_rplybarenable
extern char g_rplybarenable; /* 0x36484; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_savrc
extern struct RECTANGLE g_savrc[15]; /* 0x3595A; state-model; size=120 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_sgateopn
extern short g_sgateopn; /* 0x34DCA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_shapes3d
extern struct SHAPE3D g_shapes3d[130]; /* 0x32DBC; state-model; size=2860 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_simprect
extern char g_simprect; /* 0x354A4; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_skybox_sky_clr
extern int16_t g_skybox_sky_clr; /* 0x355D0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_skyboxwat_clr
extern short g_skyboxwat_clr; /* 0x363E2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_snaresnd
extern void * g_snaresnd; /* 0x3393E; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_td01_track_filecpy
extern short * g_td01_track_filecpy; /* 0x32D22; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_td08d
extern unsigned short * g_td08d; /* 0x35DB4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_tdist
extern int16_t g_tdist[29]; /* 0x354DA; state-model; size=58 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_tdreplay16buf
extern char * g_tdreplay16buf; /* 0x3562E; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_trackpiecescounter
extern int16_t g_trackpiecescounter; /* 0x35DD0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_ts_num
extern char g_ts_num; /* 0x35D7E; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_vector_bitix
extern char g_vector_bitix; /* 0x3393D; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_vid_flag6
extern unsigned char g_vid_flag6; /* 0x359F1; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_vid_flg2_set
extern short g_vid_flg2_set; /* 0x34DC8; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_videoflg5
extern char g_videoflg5; /* 0x36436; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_viewinx
extern char g_viewinx[2]; /* 0x349D8; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_g_wndspr
extern struct SPRITE * g_wndspr; /* 0x34D26; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_game2res_pointer
extern void * game2res_pointer; /* 0x363D2; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_camera_buttons_count
extern char game_camera_buttons_count[4]; /* 0x2EA04; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_camera_buttons_x1
extern int16_t game_camera_buttons_x1[8]; /* 0x2EA08; typeinfer machine-access widths/extent; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_camera_buttons_x2
extern int16_t game_camera_buttons_x2[8]; /* 0x2EA1A; typeinfer machine-access widths/extent; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_camera_buttons_y1
extern int16_t game_camera_buttons_y1[7]; /* 0x2EA2C; typeinfer machine-access widths/extent; size=14 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_camera_buttons_y2
extern int16_t game_camera_buttons_y2[7]; /* 0x2EA3E; typeinfer machine-access widths/extent; size=14 */
#endif
#ifndef STUNTS_LOCAL_DATA_game_rect_txt_in
extern struct RECTANGLE game_rect_txt_in; /* 0x3617E; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_gamerptrs
extern void far * gamerptrs; /* 0x349A0; four-byte resource pointer; dseg.asm:35352 labels gameresptr dd. word_349A2 is its segment word at +2, not a separate object. */
#endif
#ifndef STUNTS_LOCAL_DATA_gameunk_button_x1
extern int16_t gameunk_button_x1[1]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gameunk_button_x2
extern int16_t gameunk_button_x2[1]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gameunk_button_y1
extern int16_t gameunk_button_y1[1]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gameunk_button_y2
extern int16_t gameunk_button_y2[1]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gap_trackrow_1
extern short gap_trackrow_1; /* 0x35E18; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_gear_base_sprite
extern struct SPRITE * gear_base_sprite; /* 0x30DFC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_gear_knob_visible_view
extern char gear_knob_visible_view[2]; /* 0x30DFA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gear_knob_x_last
extern short gear_knob_x_last[2]; /* 0x30D70; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_gear_knob_y_last
extern short gear_knob_y_last[2]; /* 0x30D74; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_globalgamesettings
extern struct GAMEINFO globalgamesettings; /* 0x349A4; state-model; size=26 */
#endif
#ifndef STUNTS_LOCAL_DATA_gm_playmode
extern char gm_playmode; /* 0x35DB2; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_gm_saved_rpl_mode
extern char gm_saved_rpl_mode; /* 0x361C8; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_gmconfigbackup
extern struct GAMEINFO gmconfigbackup; /* 0x35530; state-model; size=26 */
#endif
#ifndef STUNTS_LOCAL_DATA_gnob_sprite
extern struct SPRITE * gnob_sprite; /* 0x30DEC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_gnobshapes
extern struct SHAPE2D * gnobshapes[6]; /* 0x30DD4; state-model; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_ground_skybox
extern int16_t ground_skybox; /* 0x34A88; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_gterrtrk
extern int16_t gterrtrk[30]; /* 0x343AC; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_height
extern int16_t height; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hgthgt
extern short hgthgt; /* 0x349EC; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_highEntrXInnBounds0
extern int16_t highEntrXInnBounds0[7]; /* 0x2E624; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_highEntrXOutBounds0
extern int16_t highEntrXOutBounds0[7]; /* 0x2E632; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_highEntrZBounds0
extern int16_t highEntrZBounds0[7]; /* 0x2E616; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_hill_offs
extern short hill_offs[9][2]; /* 0x2C0A2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hill_offs_b
extern int16_t hill_offs_b[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hill_offs_c
extern int16_t hill_offs_c[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hill_offs_d
extern int16_t hill_offs_d[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hillconsts
extern short hillconsts[2]; /* 0x2B8E8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_anim_table
extern int16_t hiscore_anim_table[28]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_buttons_x1
extern int16_t hiscore_buttons_x1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_buttons_x2
extern int16_t hiscore_buttons_x2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_buttons_y1
extern int16_t hiscore_buttons_y1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_buttons_y2
extern int16_t hiscore_buttons_y2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_entry_remap
extern short hiscore_entry_remap[4]; /* 0x2BCE4; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hiscore_rank_remap
extern short hiscore_rank_remap[3]; /* 0x2BCDE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_horizon_angles
extern int16_t horizon_angles[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_hscore_trk11_ptr
extern struct HighScoreRecord * hscore_trk11_ptr; /* 0x34CE6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_idx_time_gm
extern uint16_t idx_time_gm; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_idxtrk
extern char idxtrk; /* 0x3499A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_device_modestack
extern signed char input_device_modestack[8]; /* 0x35D14; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_framecount
extern int16_t input_framecount; /* 0x2EBC4; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_framecount2
extern int16_t input_framecount2; /* 0x2EBB0; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_framecount3
extern int16_t input_framecount3; /* 0x2EBB2; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_framecounter
extern int16_t input_framecounter; /* 0x2EBBE; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_input_pushed
extern short input_pushed; /* 0x2F88E; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_input_status_stack_depth
extern signed char input_status_stack_depth; /* 0x2EBD8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_inrepflg
extern char inrepflg; /* 0x354B8; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_alt
extern struct SHAPE3D intro_alt; /* 0x34330; state-model; size=22 */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_cliprect
extern struct RECTANGLE intro_cliprect; /* 0x2C1B6; complete 8-byte rectangle view in obj_seg003.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_color_max
extern short intro_color_max; /* 0x307CC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_colorvalue
extern int16_t intro_colorvalue; /* 0x2C1BE; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_a
extern short intro_text_color_a; /* 0x307D4; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_b
extern short intro_text_color_b; /* 0x307D8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_c
extern short intro_text_color_c; /* 0x307DC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_d
extern short intro_text_color_d; /* 0x307E0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_e
extern short intro_text_color_e; /* 0x307E4; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_color_f
extern short intro_text_color_f; /* 0x307E8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_a
extern short intro_text_style_a; /* 0x307D6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_b
extern short intro_text_style_b; /* 0x307DA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_c
extern short intro_text_style_c; /* 0x307DE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_d
extern short intro_text_style_d; /* 0x307E2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_e
extern short intro_text_style_e; /* 0x307E6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_intro_text_style_f
extern short intro_text_style_f; /* 0x307EA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_inverse_power_of_two_table
extern int32_t inverse_power_of_two_table[32]; /* 0x2EA82; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_is_audioloaded
extern char is_audioloaded; /* 0x2B8F3; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_is_in_rplcopy
extern char is_in_rplcopy; /* 0x35634; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_joyflags
extern int16_t joyflags; /* 0x2EBB4; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_joyinputcode
extern int16_t joyinputcode; /* 0x2EBC0; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_joystick_enabled
extern char joystick_enabled; /* 0x2FE00; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_kbjoyflags
extern int16_t kbjoyflags; /* 0x354C0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_kbormouse
extern char kbormouse; /* 0x2B8F8; byte global defined by src/obj_seg000.c:427; byte access confirmed by input TUs. */
#endif
#ifndef STUNTS_LOCAL_DATA_kick_res
extern void * kick_res; /* 0x32A34; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_last_speedo
extern short last_speedo[2]; /* 0x30D78; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_last_steering_step
extern short last_steering_step[2]; /* 0x30E00; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_last_tacho
extern short last_tacho[2]; /* 0x30D6C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_last_timer_callback_counter
extern int16_t last_timer_callback_counter[2]; /* 0x305FA; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_last_track_rotation
extern short last_track_rotation; /* 0x2BE16; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_line_input_screen_rect
extern struct SCREEN_RECT FAR * line_input_screen_rect; /* 0x305FE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_line_pattern_bits
extern st_near_data_offset line_pattern_bits[1]; /* 0x3031E; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_lnoffsets
extern int16_t lnoffsets[30]; /* 0x35D40; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_loadedresourceptr
extern void * loadedresourceptr; /* 0x35E12; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_logo_title
extern struct SHAPE3D logo_title; /* 0x3436C; state-model; size=22 */
#endif
#ifndef STUNTS_LOCAL_DATA_loopBae_InnXBounds0
extern int16_t loopBae_InnXBounds0[7]; /* 0x2E5C2; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_loopBase_OutXBounds0
extern int16_t loopBase_OutXBounds0[7]; /* 0x2E5D0; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_loopBase_ZBounds0
extern int16_t loopBase_ZBounds0[7]; /* 0x2E5B4; full 7-word table from track_constants_module.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_loopSurface_XBounds0
extern int16_t loopSurface_XBounds0[14]; /* 0x2E598; full 14-word table from track_constants_module.c; complete declared extent is 28 bytes. */
#endif
#ifndef STUNTS_LOCAL_DATA_loopSurface_ZBounds0
extern int16_t loopSurface_ZBounds0[7]; /* 0x2E58A; full 7-word table from track_constants_module.c; dseg labels identify entries within the table. */
#endif
#ifndef STUNTS_LOCAL_DATA_main_data_file_addr
extern void * main_data_file_addr; /* 0x34CEC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_mat_copy_clr_lst_ptr
extern int16_t * mat_copy_clr_lst_ptr; /* 0x35632; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mat_y_rot_angle
extern uint16_t mat_y_rot_angle; /* source definition is unsigned target int. */
#endif
#ifndef STUNTS_LOCAL_DATA_material_clrlist2_ptr
extern int16_t *material_clrlist2_ptr; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_clrlist_ptr
extern unsigned short * material_clrlist_ptr; /* 0x30B0A; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_material_color_list
extern uint16_t material_color_list[129]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_color_table_pointer
extern unsigned short * material_color_table_pointer; /* 0x30B0C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_material_pad_7
extern short material_pad_7; /* 0x35D20; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_material_patlist2_ptr
extern int16_t *material_patlist2_ptr; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_patlist_ptr
extern int16_t *material_patlist_ptr; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_patlistptr_copy
extern int16_t * material_patlistptr_copy; /* 0x35D1E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_material_pattern2_list
extern uint16_t material_pattern2_list[129]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_pattern2_table_ptr
extern unsigned short * material_pattern2_table_ptr; /* 0x30B10; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_material_pattern_list
extern uint16_t material_pattern_list[129]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_material_pattern_table_pointer
extern unsigned short * material_pattern_table_pointer; /* 0x30B0E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_matpatlistcopypointer2
extern int16_t * matpatlistcopypointer2; /* 0x35D92; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_matrix_transform_view
extern struct MATRIX matrix_transform_view; /* 0x342D2; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_matrix_x_rotation
extern struct MATRIX matrix_x_rotation; /* 0x36472; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_matrotation_tmp
extern struct MATRIX matrotation_tmp; /* 0x36438; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_maxscnh
extern int16_t maxscnh; /* 0x354CE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mcgawnd_window_sprite
extern struct SPRITE * mcgawnd_window_sprite; /* 0x3392E; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_button_color_a
extern short menu_button_color_a; /* 0x307F4; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_button_color_b
extern short menu_button_color_b; /* 0x307F6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_button_color_c
extern short menu_button_color_c; /* 0x307F8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_buttons_x1
extern int16_t menu_buttons_x1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_buttons_x2
extern int16_t menu_buttons_x2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_buttons_y1
extern int16_t menu_buttons_y1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_buttons_y2
extern int16_t menu_buttons_y2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_clear_color
extern short menu_clear_color; /* 0x307FA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_hover_color_a
extern short menu_hover_color_a; /* 0x307CE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_hover_color_b
extern short menu_hover_color_b; /* 0x307D0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_left
extern char byte_2B9F0[6]; /* 0x2B9F0; typeinfer machine-access widths/extent; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_menu_right
extern char byte_2B9F6[6]; /* 0x2B9F6; typeinfer machine-access widths/extent; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_menutimeout
extern unsigned char menutimeout; /* 0x34AE2; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_meters_sprite
extern struct SPRITE * meters_sprite; /* 0x30D80; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_mode_flag
extern char mode_flag; /* 0x349E2; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_api_y
extern uint16_t mouse_api_y; /* 0x34D62; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_buffer_count
extern char mouse_buffer_count; /* 0x2B8FC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_button_state_cache
extern short mouse_button_state_cache; /* 0x30318; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_enabled
extern char mouse_enabled; /* 0x2B8F2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_isdirty
extern char mouse_isdirty; /* 0x2B8F9; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_oldbut
extern int16_t mouse_oldbut; /* 0x2EBBC; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_oldx
extern int16_t mouse_oldx; /* 0x2EBB8; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_oldy
extern int16_t mouse_oldy; /* 0x2EBBA; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_ptr_cursor
extern struct SPRITE far *mouse_ptr_cursor;
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_transparent_mode
extern char mouse_transparent_mode; /* 0x2B8F7; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_mouse_unk_sprite_ptr
extern struct SPRITE * mouse_unk_sprite_ptr; /* 0x355C8; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_mousebutinputcode
extern int16_t mousebutinputcode; /* 0x2EBC2; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_mousehorscale
extern uint16_t mousehorscale; /* source definition and 16-bit object extent at 0x3031A. */
#endif
#ifndef STUNTS_LOCAL_DATA_ms_buttons
extern uint16_t ms_buttons; /* 0x35D7C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_msecoordx
extern int16_t msecoordx; /* 0x3616C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_msregisterms
extern MouseRegs msregisterms; /* 0x3498A; state-model; size=14 */
#endif
#ifndef STUNTS_LOCAL_DATA_mssprite_arrays
extern struct SPRITE * mssprite_arrays[4]; /* 0x32D10; state-model; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_mus_samplelimit
extern unsigned short mus_samplelimit; /* 0x354BA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_musicfile
extern void * musicfile; /* 0x34360; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_newjoyflags
extern char data_2EBB6[2]; /* 0x2EBB6; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_nextpos_normalip
extern short nextpos_normalip; /* 0x359FE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_numid
extern char numid; /* 0x34346; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_octant
extern char octant; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_old_intr0_handler
extern uint32_t old_intr0_handler; /* 0x2BE2C; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_on_off_dash
extern char on_off_dash; /* 0x33950; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_one_through_fourteen_table
extern unsigned short one_through_fourteen_table[14]; /* 0x307AE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_op_eng_sound_id
extern short op_eng_sound_id; /* 0x3408C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_openvfile
extern void * openvfile; /* 0x34A7C; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_ophys_7
extern struct SIMD ophys_7; /* 0x35640; state-model; size=776 */
#endif
#ifndef STUNTS_LOCAL_DATA_opp_follow_flag_backup
extern char opp_follow_flag_backup; /* 0x32A38; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_opponent_engine_profile
extern struct ENGINESOUND opponent_engine_profile; /* 0x2E82C; state-model; size=48 */
#endif
#ifndef STUNTS_LOCAL_DATA_opponent_pointc
extern struct VECTOR opponent_pointc[6]; /* 0x3493C; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_opponent_spd_tbl
extern unsigned char opponent_spd_tbl[16]; /* 0x34D50; state-model; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_opponentmenu_buttons_x1
extern int16_t opponentmenu_buttons_x1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_opponentmenu_buttons_x2
extern int16_t opponentmenu_buttons_x2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_opponentmenu_buttons_y1
extern int16_t opponentmenu_buttons_y1[5]; /* 0x2BBC8; typeinfer machine-access widths/extent; size=10 */
#endif
#ifndef STUNTS_LOCAL_DATA_opponentmenu_buttons_y2
extern int16_t opponentmenu_buttons_y2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_opptext_label
extern char opptext_label[3]; /* 0x36464; state-model; size=3 */
#endif
#ifndef STUNTS_LOCAL_DATA_paint_cycle
extern char paint_cycle[16]; /* 0x2C0C6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palette_column_limits
extern unsigned char palette_column_limits[2]; /* 0x2ECFE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palette_row_limits
extern unsigned char palette_row_limits[2]; /* 0x2ED00; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palette_window_fill_color
extern short palette_window_fill_color; /* 0x307EE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palette_window_line_color
extern short palette_window_line_color; /* 0x307EC; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palette_window_line_style
extern short palette_window_line_style; /* 0x307F0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_palmap
extern uint8_t palmap[16]; /* source definition: src/seg034_shape2d_group.c */
#endif
#ifndef STUNTS_LOCAL_DATA_pass_check_flag
extern char pass_check_flag; /* 0x35E1A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_pen_flag_count
extern char pen_flag_count; /* 0x3499F; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_performGraphColor
extern short performGraphColor; /* 0x30800; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_pixel_scales
extern int16_t pixel_scales; /* 0x34AE4; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_pl_carres_3d
extern char * pl_carres_3d; /* 0x354AA; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_pl_i
extern short pl_i; /* 0x34DC6; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_plan_memres
extern struct PLANE plan_memres; /* initialized far-data object: src/fardata_11036.c:11; some callers use an array/pointer view. */
#endif
#ifndef STUNTS_LOCAL_DATA_player_engine_profile
extern struct ENGINESOUND player_engine_profile; /* 0x2E7FC; state-model; size=48 */
#endif
#ifndef STUNTS_LOCAL_DATA_pln_rot_output
extern struct VECTOR pln_rot_output; /* 0x34F3C; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_pln_rotate_z
extern short pln_rotate_z; /* 0x34F42; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_plncurrptr
extern struct PLANE * plncurrptr; /* 0x34D64; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_poly_cursor1
extern int16_t poly_cursor1; /* 0x3394E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_poly_link_listit4
extern int16_t poly_link_listit4; /* 0x35D98; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_poly_linked_list_40ED6
extern int16_t poly_linked_list_40ED6[400]; /* 0x30ED6; typeinfer machine-access widths/extent; size=800 */
#endif
#ifndef STUNTS_LOCAL_DATA_polygon_link_3_list_iter
extern int16_t polygon_link_3_list_iter; /* 0x3554A; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_polygonnumber
extern uint16_t polygonnumber; /* 0x342E6; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_polyinfoptrs
extern int16_t * polyinfoptrs[401]; /* 0x311F8; typeinfer machine-access widths/extent; size=1606 */
#endif
#ifndef STUNTS_LOCAL_DATA_popupact
extern char popupact; /* 0x35D3E; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_pos_pt
extern struct VECTOR pos_pt; /* 0x32CBA; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_pos_y_ms
extern int16_t pos_y_ms; /* 0x361CE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_postable
extern int16_t postable[30]; /* 0x34A8E; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_prerender_auxiliary_arg
extern st_near_data_offset prerender_auxiliary_arg[1]; /* 0x30320; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_prerender_max_line_width
extern st_near_data_offset prerender_max_line_width[1]; /* 0x2F3C6; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_prevcamrot
extern short prevcamrot; /* 0x363D6; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_previous_timer_irq_vector
extern int16_t previous_timer_irq_vector[2]; /* 0x2F874; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_primidxcounttab
extern unsigned char primidxcounttab[16]; /* 0x2EA62; dseg.asm label primidxcounttab spans 16 bytes; src/obj_seg006.c:1095. */
#endif
#ifndef STUNTS_LOCAL_DATA_primitive_type_table
extern unsigned char primitive_type_table[16]; /* 0x2EA72; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_projected_point_table
extern struct POINT2D * projected_point_table[11]; /* 0x31854; state-model; size=22 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_center_x
extern st_near_data_offset projection_center_x[1]; /* 0x303B6; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_center_y
extern st_near_data_offset projection_center_y[1]; /* 0x303B8; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_half_height
extern st_near_data_offset projection_half_height[1]; /* 0x303B0; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_half_width
extern st_near_data_offset projection_half_width[1]; /* 0x303AE; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_left_origin
extern st_near_data_offset projection_left_origin[1]; /* 0x303B2; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_top_origin
extern st_near_data_offset projection_top_origin[1]; /* 0x303B4; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_x_angle
extern st_near_data_offset projection_x_angle[1]; /* 0x303BE; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_x_scale
extern st_near_data_offset projection_x_scale[1]; /* 0x303BA; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_y_angle
extern st_near_data_offset projection_y_angle[1]; /* 0x303C0; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projection_y_scale
extern st_near_data_offset projection_y_scale[1]; /* 0x303BC; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_projectiondata10
extern int16_t projectiondata10; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_projectiondata5
extern int16_t projectiondata5; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_projectiondata8
extern int16_t projectiondata8; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_projectiondata9
extern int16_t projectiondata9; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_pspofs
extern int16_t pspofs; /* 0x2FF88; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_pspseg
extern int16_t pspseg; /* 0x2FF8A; typeinfer machine-access widths/extent; size=902 */
#endif
#ifndef STUNTS_LOCAL_DATA_ptr_model_active
extern void * ptr_model_active; /* 0x349F8; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_pts_set
extern struct VECTOR pts_set[6]; /* 0x343FA; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_r_zp
extern int16_t r_zp[30]; /* 0x338EC; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_race_stats
extern struct GAMESTATE_SNAPSHOT race_stats; /* 0x34348; state-model; size=22 */
#endif
#ifndef STUNTS_LOCAL_DATA_randomseeds
extern unsigned char randomseeds[]; /* 0x3594A; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_rate_frame
extern short rate_frame; /* 0x349D0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_rc0_cpy
extern struct RECTANGLE rc0_cpy; /* 0x35952; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_rclist
extern struct RECTANGLE rclist[15]; /* 0x3554C; state-model; size=120 */
#endif
#ifndef STUNTS_LOCAL_DATA_rcmapix
extern short rcmapix[45]; /* 0x355D4; state-model; size=90 */
#endif
#ifndef STUNTS_LOCAL_DATA_rcpunk2
extern struct RECTANGLE * rcpunk2; /* 0x2B808; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_rcunk5
extern struct RECTANGLE rcunk5; /* 0x2C094; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_rect_ingame_text2
extern struct RECTANGLE rect_ingame_text2; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_rect_ingame_text3
extern struct RECTANGLE rect_ingame_text3; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_rect_ingame_text4
extern struct RECTANGLE rect_ingame_text4; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_rect_num3
extern char rect_num3; /* 0x36166; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_rectangle_unknown16
extern struct RECTANGLE rectangle_unknown16; /* 0x2BB56; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_rectclip
extern struct RECTANGLE rectclip[15]; /* 0x34DCE; state-model; size=120 */
#endif
#ifndef STUNTS_LOCAL_DATA_rectp
extern struct RECTANGLE * rectp; /* 0x2B80A; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_rects_updt
extern char rects_updt[15]; /* 0x35520; state-model; size=15 */
#endif
#ifndef STUNTS_LOCAL_DATA_replay_axis_magnitude
extern char replay_axis_magnitude[34]; /* 0x2E85C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_replay_file
extern char replay_file[82]; /* 0x2B85E; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_replay_state_cache
extern char replay_state_cache; /* 0x2BE02; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_replay_steer_flag
extern unsigned char replay_steer_flag[64]; /* 0x342EA; state-model; size=64 */
#endif
#ifndef STUNTS_LOCAL_DATA_replaybar_toggle
extern char replaybar_toggle; /* 0x3616F; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_replayrst
extern int16_t replayrst; /* 0x35D94; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resbuftext
extern char resbuftext[80]; /* 0x363E4; state-model; size=80 */
#endif
#ifndef STUNTS_LOCAL_DATA_resendptr1
extern void * resendptr1; /* 0x30314; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resendptr2
extern void * resendptr2; /* 0x30316; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resmaxsize
extern uint16_t resmaxsize; /* 0x2FF86; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resmem_base_seg
extern uint16_t resmem_base_seg; /* 0x2FF84; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resmem_end_seg
extern uint16_t resmem_end_seg; /* 0x2FF82; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resource_ptropp
extern void * resource_ptropp; /* 0x34A00; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_resource_sound_hit
extern void * resource_sound_hit; /* 0x34368; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_resptr1
extern void * resptr1; /* 0x30310; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_resptr2
extern void * resptr2; /* 0x30312; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_results_entryname
extern char results_entryname[17]; /* 0x359E0; state-model; size=17 */
#endif
#ifndef STUNTS_LOCAL_DATA_rfy5
extern int16_t rfy5; /* 0x361CC; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_ride_audio_sound_res
extern void * ride_audio_sound_res; /* 0x354CA; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_road_elem_ctrz
extern short road_elem_ctrz; /* 0x349F6; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_road_num
extern short road_num; /* 0x3428E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_road_trk
extern char * road_trk; /* 0x349D4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_roofbmphgt_saved
extern int16_t roofbmphgt_saved; /* 0x35DB8; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_rotpr
extern int16_t rotpr[2]; /* 0x349FC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_rotxvehicle
extern short rotxvehicle; /* 0x3435E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_row_ctr_zs
extern int16_t row_ctr_zs[30]; /* 0x36186; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_rplbarabovehgt
extern int16_t rplbarabovehgt; /* 0x359D4; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_runrndx
extern short runrndx; /* 0x32D28; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_sampled_trk_column
extern unsigned char sampled_trk_column; /* 0x35D90; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_saved_effect_chunk_volumes
extern unsigned char saved_effect_chunk_volumes[24]; /* 0x328D6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_saved_music_chunk_volumes
extern unsigned char saved_music_chunk_volumes[24]; /* 0x328BE; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_savedptr_ms
extern struct SPRITE far *savedptr_ms; /* sprite-save table view from src/obj_seg008.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_scene2
extern struct scene_shape scene2[19]; /* 0x2E3CA; state-model; size=266 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene3
extern struct scene_shape scene3[13]; /* 0x2E4D4; state-model; size=182 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene_1ht
extern unsigned short scene_1ht; /* 0x35518; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene_2ht
extern unsigned short scene_2ht; /* 0x3551A; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene_3ht
extern unsigned short scene_3ht; /* 0x3551C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene_4ht
extern unsigned short scene_4ht; /* 0x3551E; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_scene_idx
extern unsigned char scene_idx; /* 0x36167; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_scenery_names
extern char scenery_names[5][9]; /* 0x2B8BA; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_score_entry_alphabet
extern unsigned char score_entry_alphabet[]; /* 0x2BD34; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_scrorder_idxs
extern short scrorder_idxs[7]; /* 0x36170; state-model; size=14 */
#endif
#ifndef STUNTS_LOCAL_DATA_sdgame2hdl
extern char * sdgame2hdl; /* 0x363D8; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_sdgbmp_v
extern struct SHAPE2D * sdgbmp_v[5]; /* 0x35DBC; state-model; size=20 */
#endif
#ifndef STUNTS_LOCAL_DATA_sdgresourcehandle
extern void * sdgresourcehandle; /* 0x35D08; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_secondoveh
extern struct VECTOR secondoveh[6]; /* 0x34918; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_secondveccar
extern struct VECTOR secondveccar[6]; /* 0x3441E; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_select_rect_rc
extern int16_t clip[4]; /* 0x30EB0; typeinfer machine-access widths/extent; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_sfx_audio_vol
extern unsigned char sfx_audio_vol; /* 0x35948; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape2d_extension_suffixes
extern char * shape2d_extension_suffixes[6]; /* 0x30BD6; state-model; size=12 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_extensions
extern char shape_extensions[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_rot
extern short shape_rot[8]; /* 0x2C0D6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_a
extern unsigned char shape_template_points_a[6]; /* 0x2E640; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_b
extern struct coord_pair shape_template_points_b[12]; /* 0x2E646; state-model; size=48 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_c
extern struct coord_pair shape_template_points_c[3]; /* 0x2E676; state-model; size=12 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_d
extern struct coord_pair shape_template_points_d[3]; /* 0x2E682; state-model; size=12 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_e
extern struct coord_pair shape_template_points_e[3]; /* 0x2E68E; state-model; size=12 */
#endif
#ifndef STUNTS_LOCAL_DATA_shape_template_points_f
extern struct coord_pair shape_template_points_f[6]; /* 0x2E69A; state-model; size=24 */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42
extern uint8_t shapedata42[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_10
extern uint8_t shapedata42_10[1236]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_11
extern uint8_t shapedata42_11[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_2
extern uint8_t shapedata42_2[72]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_3
extern uint8_t shapedata42_3[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_4
extern uint8_t shapedata42_4[702]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_5
extern uint8_t shapedata42_5[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_6
extern uint8_t shapedata42_6[672]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_7
extern uint8_t shapedata42_7[510]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_8
extern uint8_t shapedata42_8[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapedata42_9
extern uint8_t shapedata42_9[42]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapeexts
extern char *shapeexts[]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_shapeinfos
extern struct track_object_info shapeinfos[120]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_showmouse
extern int16_t showmouse; /* 0x3031C; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_sigframe
extern unsigned char sigframe; /* 0x349DA; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_simdp7
extern struct SIMD simdp7; /* 0x35E5A; state-model; size=776 */
#endif
#ifndef STUNTS_LOCAL_DATA_sin80
extern int16_t sin80; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_sky_hgt_world
extern int16_t sky_hgt_world; /* 0x33932; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_skybox_loaded
extern char skybox_loaded; /* 0x2B8F6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_skypics
extern struct SHAPE2D * skypics[4]; /* 0x35D80; state-model; size=16 */
#endif
#ifndef STUNTS_LOCAL_DATA_skyres_handle
extern union FARRESOURCE skyres_handle; /* 0x36168; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_slomodiv8
extern unsigned char slomodiv8; /* 0x3552F; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_slow_video_mode_state
extern unsigned short slow_video_mode_state; /* 0x34CEA; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_snd_sample_rate_phase
extern unsigned short snd_sample_rate_phase; /* 0x34D48; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_snd_tick_clock
extern int16_t snd_tick_clock; /* 0x343F4; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_snd_voices_tbl
extern struct AUDIOVOICE snd_voices_tbl[16]; /* 0x35A26; state-model; size=736 */
#endif
#ifndef STUNTS_LOCAL_DATA_sndpendingstate
extern char sndpendingstate; /* 0x359D8; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_sndposrecord
extern short sndposrecord; /* 0x349E4; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_spare_td22_1
extern short spare_td22_1; /* 0x359DE; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_spdneedlegaugeclr
extern short spdneedlegaugeclr; /* 0x34D60; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_speed_recovery_divisors
extern unsigned short speed_recovery_divisors[5]; /* 0x2BDF8; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_sphere_scanline_profiles
extern st_near_data_offset sphere_scanline_profiles[40]; /* 0x2F3C8; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=80 */
#endif
#ifndef STUNTS_LOCAL_DATA_sprite2
extern struct SPRITE sprite2; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_spritepointermini
extern struct SPRITE far *spritepointermini;
#endif
#ifndef STUNTS_LOCAL_DATA_st_hdg
extern short st_hdg; /* 0x35516; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_statemgmtcpy
extern unsigned short statemgmtcpy; /* 0x34984; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_stdares
extern void * stdares; /* 0x30D7C; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_stdbres
extern void * stdbres; /* 0x30D84; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_steerWhlRespTable_10fps
extern char steerWhlRespTable_10fps[62]; /* Complete source-defined response table in obj_seg001_complete.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_steerWhlRespTable_20fps
extern char steerWhlRespTable_20fps[64]; /* Complete source-defined response table in obj_seg001_complete.c. */
#endif
#ifndef STUNTS_LOCAL_DATA_steering_dot_x
extern short steering_dot_x[2]; /* 0x30DF2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_steering_dot_y
extern short steering_dot_y[2]; /* 0x30DF6; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_steering_zone
extern char steering_zone[2]; /* 0x30DF0; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_table_lookup
extern char * table_lookup; /* 0x349E8; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_tagtrk
extern char tagtrk; /* 0x3499E; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_td10checkptr
extern struct VECTOR far *td10checkptr; /* checkpoints are x/y/z vector records (source: obj_seg001_complete.c and obj_seg004.c). */
#endif
#ifndef STUNTS_LOCAL_DATA_td13_replay_hdr
extern char * td13_replay_hdr; /* 0x36162; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td14tb
extern unsigned char * td14tb; /* 0x34D42; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td15p_9
extern unsigned char * td15p_9; /* 0x354BC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td19hdl
extern unsigned char * td19hdl; /* 0x35E56; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td3
extern char * td3; /* 0x33942; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td6_ptr_b
extern char * td6_ptr_b; /* 0x354D0; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_td_18_ref
extern char * td_18_ref; /* 0x354D6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_tdfrompathrow22
extern char * tdfrompathrow22; /* 0x359DA; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_temp
extern int16_t temp; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_test_pln
extern unsigned char test_pln; /* 0x3392C; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_text_bounds_right
extern short text_bounds_right; /* 0x3224A; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_text_bounds_upper
extern short text_bounds_upper; /* 0x3224C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_text_cursor_outline_color
extern short text_cursor_outline_color; /* 0x307F2; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_text_outline_bottom
extern short text_outline_bottom; /* 0x32256; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_text_outline_right
extern short text_outline_right; /* 0x32252; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_textboundsleft
extern short textboundsleft; /* 0x32248; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_textoutlineleft
extern short textoutlineleft; /* 0x32250; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_textrespfxchr
extern char textrespfxchr; /* 0x3645E; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_textstr
extern char textstr[40]; /* 0x34384; state-model; size=40 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_callback_busy
extern char timer_callback_busy; /* 0x2F88C; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_callback_counter
extern int16_t timer_callback_counter[2]; /* 0x2F878; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_callback_reentry_depth
extern st_near_data_offset timer_callback_reentry_depth[1]; /* 0x2F88A; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_callback_reentry_peak
extern st_near_data_offset timer_callback_reentry_peak[1]; /* 0x2F888; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_copy_target_high
extern uint16_t timer_copy_target_high; /* 0x305F8; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_copy_unk
extern uint16_t timer_copy_unk[2]; /* 0x305F6; typeinfer machine-access widths/extent; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_elapsed_ticks_high
extern st_near_data_offset timer_elapsed_ticks_high[1]; /* 0x2F87E; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_elapsed_ticks_low
extern st_near_data_offset timer_elapsed_ticks_low[1]; /* 0x2F87C; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_interrupt_reload_value
extern st_near_data_offset timer_interrupt_reload_value[1]; /* 0x2F884; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_sound_countdown_active
extern char timer_sound_countdown_active; /* 0x2F880; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_sound_countdown_enabled
extern char timer_sound_countdown_enabled; /* 0x2F881; typeinfer machine-access widths/extent; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_timer_sound_countdown_ticks
extern st_near_data_offset timer_sound_countdown_ticks[1]; /* 0x2F882; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_timeraud
extern unsigned char timeraud; /* 0x34A8A; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_tmr2
extern unsigned short tmr2; /* 0x32D02; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_tommsampleresource
extern void * tommsampleresource; /* 0x3394A; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_tparr
extern struct RECTANGLE tparr[15]; /* 0x34A04; state-model; size=120 */
#endif
#ifndef STUNTS_LOCAL_DATA_track04_plyraero
extern short * track04_plyraero; /* 0x34D3E; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_track3_gap
extern short track3_gap[2]; /* 0x33946; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_track_file
extern char track_file[82]; /* 0x2B80C; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_track_prev_vec
extern struct VECTOR track_prev_vec; /* 0x2C114; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_trackctrpos2
extern int16_t trackctrpos2[30]; /* 0x35DD2; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_trackdat7
extern unsigned short * trackdat7; /* 0x354B4; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_trackdata_05_opp_aerotbl
extern short * trackdata_05_opp_aerotbl; /* 0x34D4A; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_trackdata_penalty_related
extern short * trackdata_penalty_related; /* 0x338E8; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu2_buttons_x1
extern int16_t trackmenu2_buttons_x1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu2_buttons_x2
extern int16_t trackmenu2_buttons_x2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu2_buttons_y1
extern int16_t trackmenu2_buttons_y1[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu2_buttons_y2
extern int16_t trackmenu2_buttons_y2[5]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu_buttons_x1
extern int16_t trackmenu_buttons_x1[3]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu_buttons_x2
extern int16_t trackmenu_buttons_x2[3]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu_buttons_y1
extern int16_t trackmenu_buttons_y1[3]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackmenu_buttons_y2
extern int16_t trackmenu_buttons_y2[3]; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_trackpreview_cliprect
extern struct RECTANGLE trackpreview_cliprect; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_transformed_primitive_cursor
extern unsigned char * transformed_primitive_cursor; /* 0x3186A; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_transformed_primitive_paint
extern char transformed_primitive_paint; /* 0x31882; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_transformed_shape_bounds
extern struct RECTANGLE * transformed_shape_bounds; /* 0x31878; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_transformed_vert_count
extern unsigned char transformed_vert_count; /* 0x349F4; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_trk_sample_count
extern char trk_sample_count; /* 0x3616E; state-model; size=1 */
#endif
#ifndef STUNTS_LOCAL_DATA_trkd23adr
extern unsigned char * trkd23adr; /* 0x363DC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_trklst
extern struct track_object trklst[215]; /* 0x2D808; state-model; size=3010 */
#endif
#ifndef STUNTS_LOCAL_DATA_trkptrpath
extern struct VECTOR * trkptrpath; /* 0x349DC; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_tshapearrarg2
extern char tshapearrarg2[30]; /* 0x35A02; state-model; size=30 */
#endif
#ifndef STUNTS_LOCAL_DATA_tsix
extern int16_t tsix[29]; /* 0x35E1C; state-model; size=58 */
#endif
#ifndef STUNTS_LOCAL_DATA_tvec2
extern struct VECTOR tvec2; /* 0x34F36; state-model; size=6 */
#endif
#ifndef STUNTS_LOCAL_DATA_txt_bounds_bottom
extern short txt_bounds_bottom; /* 0x3224E; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_txt_outline_top
extern short txt_outline_top; /* 0x32254; state-model; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_unk_3B1E2
extern int16_t far unk_3B1E2[7]; /* 7 target-int words in src/fardata_11036.c:12. */
#endif
#ifndef STUNTS_LOCAL_DATA_unused_count
extern int16_t unused_count; /* 0x349F2; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_vec
extern struct VECTOR *vec; /* ?; accepted-TU declaration consensus; target evidence incomplete; size=? */
#endif
#ifndef STUNTS_LOCAL_DATA_veccar
extern struct VECTOR veccar[6]; /* 0x34442; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_veco
extern struct VECTOR veco[6]; /* 0x348F4; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_vector_op_base_z
extern st_near_data_offset vector_op_base_z[1]; /* 0x30602; state-model extent plus accepted ASM word table; elements are 16-bit DGROUP offsets; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_veh_od
extern struct VECTOR veh_od[6]; /* 0x34960; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_veh_position
extern int32_t veh_position; /* 0x349C6; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_veh_z
extern int32_t veh_z; /* 0x349CA; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_video_cnstval
extern unsigned short video_cnstval; /* 0x359FC; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_video_mode4_active
extern char video_mode4_active; /* 0x2F85A; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_vidflg3is_minus1
extern short vidflg3is_minus1; /* 0x354D4; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_vidflg4_is1
extern short vidflg4_is1; /* 0x361D0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_viewyshift
extern int16_t viewyshift; /* 0x34D20; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_voicefile_gap_c
extern short voicefile_gap_c[4]; /* 0x34A80; state-model; size=8 */
#endif
#ifndef STUNTS_LOCAL_DATA_waitm_ms
extern int16_t waitm_ms; /* 0x34382; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_wall_facingang
extern short wall_facingang; /* 0x35D96; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_wall_wallelement
extern short wall_wallelement; /* 0x34A8C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_wallanchor_x
extern short wallanchor_x; /* 0x32D20; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_wallanchor_z
extern short wallanchor_z; /* 0x32D2C; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_wallrecrecord
extern struct WALLREC * wallrecrecord; /* 0x36460; state-model; size=4 */
#endif
#ifndef STUNTS_LOCAL_DATA_wheel_shapes
extern struct SHAPE2D * wheel_shapes[9]; /* 0x30DB0; state-model; size=36 */
#endif
#ifndef STUNTS_LOCAL_DATA_window_release_order_error_msg
extern int16_t window_release_order_error_msg; /* 0x303C2; typeinfer machine-access widths/extent; size=32 */
#endif
#ifndef STUNTS_LOCAL_DATA_wkmatx
extern struct MATRIX wkmatx; /* 0x3644C; state-model; size=18 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_33892
extern int16_t word_33892; /* 0x33892; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_338A8
extern int16_t word_338A8; /* 0x338A8; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_34A06
extern int16_t word_34A06[1]; /* 0x34A06; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_34A08
extern int16_t word_34A08[1]; /* 0x34A08; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_34A0A
extern int16_t word_34A0A[1]; /* 0x34A0A; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_word_449FE
extern int16_t word_449FE; /* 0x349FE; typeinfer machine-access widths/extent; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_x_course_part
extern short x_course_part; /* 0x349E0; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_xcols
extern int16_t xcols[30]; /* 0x32CC6; state-model; size=60 */
#endif
#ifndef STUNTS_LOCAL_DATA_yrotrotveh
extern short yrotrotveh; /* 0x34986; state-model; size=2 */
#endif
#ifndef STUNTS_LOCAL_DATA_ywhlang
extern short ywhlang[5]; /* 0x343E8; state-model; size=10 */
#endif
#ifndef STUNTS_LOCAL_DATA_z_ctr_pos
extern int16_t z_ctr_pos[30]; /* 0x32C7E; state-model; size=60 */
#endif

/* One machine-evidence prototype per mapped game/runtime function. */
#ifndef STUNTS_LOCAL_FN___FF_MSGBANNER
extern void __FF_MSGBANNER(void); /* 0x1CE8A; target evidence: void far __FF_MSGBANNER(void); */
#endif
#ifndef STUNTS_LOCAL_FN___NMSG_TEXT
extern int __NMSG_TEXT(short); /* 0x1D0FE; target evidence: long far __NMSG_TEXT(int); */
#endif
#ifndef STUNTS_LOCAL_FN___NMSG_WRITE
extern short __NMSG_WRITE(short); /* 0x1D129; target evidence: int far __NMSG_WRITE(int); */
#endif
#ifndef STUNTS_LOCAL_FN___aFFblmul
extern void __aFFblmul(void *, int); /* 0x1E93C; target evidence: void far __aFFblmul(struct ? *, long); */
#endif
#ifndef STUNTS_LOCAL_FN___aFldiv
extern int __aFldiv(unsigned int, unsigned int); /* 0x1E83C; target evidence: long far __aFldiv(unsigned long, unsigned long); */
#endif
#ifndef STUNTS_LOCAL_FN___aFlmul
extern int __aFlmul(short, short, short, short); /* 0x1E8D8; target evidence: long far __aFlmul(int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN___aFlshr
extern int __aFlshr(void); /* 0x1E90C; target evidence: long far __aFlshr(void); */
#endif
#ifndef STUNTS_LOCAL_FN___aFuldiv
extern int __aFuldiv(unsigned int, unsigned short, short); /* 0x1E9A6; target evidence: long far __aFuldiv(unsigned long, unsigned int, int); */
#endif
#ifndef STUNTS_LOCAL_FN___amalloc
extern short __amalloc(void); /* 0x1E0C1; target evidence: int near __amalloc(void); */
#endif
#ifndef STUNTS_LOCAL_FN___amallocbrk
extern void __amallocbrk(void); /* 0x1E200; target evidence: void near __amallocbrk(void); */
#endif
#ifndef STUNTS_LOCAL_FN___amexpand
extern void __amexpand(void); /* 0x1E1A4; target evidence: void near __amexpand(void); */
#endif
#ifndef STUNTS_LOCAL_FN___amlink
extern short __amlink(void); /* 0x1E1DE; target evidence: int near __amlink(void); */
#endif
#ifndef STUNTS_LOCAL_FN___chkstk
extern void __chkstk(void); /* 0x1CEB4; target evidence: void far __chkstk(void); */
#endif
#ifndef STUNTS_LOCAL_FN___flsbuf
extern short __flsbuf(unsigned char, void *); /* 0x1D260; target evidence: int far __flsbuf(unsigned char, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN___fmemcpy
extern void *__fmemcpy(void *, void *, uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN___fptrap
extern void __fptrap(void); /* 0x1CEAE; target evidence: void near __fptrap(void); */
#endif
#ifndef STUNTS_LOCAL_FN___ftbuf
extern void __ftbuf(short, void *); /* 0x1D4B0; target evidence: void far __ftbuf(int, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN___getbuf
extern void __getbuf(void *); /* 0x1D3BE; target evidence: void near __getbuf(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN___maperror
extern void __maperror(void); /* 0x1D1B6; target evidence: void far __maperror(void); */
#endif
#ifndef STUNTS_LOCAL_FN___myalloc
extern short __myalloc(void); /* 0x1D154; target evidence: int near __myalloc(void); */
#endif
#ifndef STUNTS_LOCAL_FN___nullcheck
extern short __nullcheck(void); /* 0x1CED8; target evidence: int far __nullcheck(void); */
#endif
#ifndef STUNTS_LOCAL_FN___output
extern short __output(short, short, short); /* 0x1D5BE; target evidence: int far __output(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN___setargv
extern void __setargv(void); /* 0x1CEFE; target evidence: void near __setargv(void); */
#endif
#ifndef STUNTS_LOCAL_FN___setenvp
extern void __setenvp(void); /* 0x1D090; target evidence: void far __setenvp(void); */
#endif
#ifndef STUNTS_LOCAL_FN___sigentry
extern void __sigentry(void); /* 0x1E71F; target evidence: void far __sigentry(void); */
#endif
#ifndef STUNTS_LOCAL_FN___stbuf
extern short __stbuf(short); /* 0x1D42C; target evidence: int far __stbuf(int); */
#endif
#ifndef STUNTS_LOCAL_FN__abort
extern void _abort(void); /* 0x1E3C6; target evidence: void far _abort(void); */
#endif
#ifndef STUNTS_LOCAL_FN__abs
extern short _abs(short); /* 0x1E588; target evidence: int far _abs(int); */
#endif
#ifndef STUNTS_LOCAL_FN__brkctl
extern int _brkctl(int, short, short, short); /* 0x1E222; target evidence: long far _brkctl(long, int, int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN__fflush
extern short _fflush(void *); /* 0x1D54E; target evidence: int far _fflush(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN__flushall
extern void _flushall(void); /* 0x1D1EA; target evidence: void far _flushall(void); */
#endif
#ifndef STUNTS_LOCAL_FN__int86
extern short _int86(short, void *, void *); /* 0x1E40C; target evidence: int far _int86(int, struct ? *, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN__isatty
extern short _isatty(void *); /* 0x1E3E8; target evidence: int far _isatty(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN__itoa
extern char *_itoa(int, char *, int); /* 0x1E3A0; target evidence: void far _itoa(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN__lseek
extern void _lseek(unsigned short, int, short); /* 0x1DEAE; target evidence: void far _lseek(unsigned int, long, int); */
#endif
#ifndef STUNTS_LOCAL_FN__memcpy
extern void *_memcpy(void *, void *, uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN__out
extern short _out(short); /* 0x1DCD8; target evidence: int far _out(int); */
#endif
#ifndef STUNTS_LOCAL_FN__outc
extern short _outc(unsigned short); /* 0x1DBCE; target evidence: int far _outc(unsigned int); */
#endif
#ifndef STUNTS_LOCAL_FN__printf
extern void _printf(short, short); /* 0x1D21E; target evidence: void far _printf(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN__raise
extern void _raise(short); /* 0x1E5AA; target evidence: void far _raise(int); */
#endif
#ifndef STUNTS_LOCAL_FN__rand
extern short _rand(void); /* 0x1E64E; target evidence: int far _rand(void); */
#endif
#ifndef STUNTS_LOCAL_FN__signal
extern void _signal(short, int); /* 0x1E67C; target evidence: void far _signal(int, long); */
#endif
#ifndef STUNTS_LOCAL_FN__sprintf
extern void _sprintf(short, short, short); /* 0x1E48C; target evidence: void far _sprintf(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN__srand
extern void _srand(short); /* 0x1E63C; target evidence: void far _srand(int); */
#endif
#ifndef STUNTS_LOCAL_FN__stackavail
extern short _stackavail(void); /* 0x1E052; target evidence: int far _stackavail(void); */
#endif
#ifndef STUNTS_LOCAL_FN__strcat
extern short _strcat(short, short); /* 0x1E2E6; target evidence: int far _strcat(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN__strcmp
extern short _strcmp(int); /* 0x1E358; target evidence: int far _strcmp(long);; arity mismatch; source call counts [2] */
#endif
#ifndef STUNTS_LOCAL_FN__strcpy
extern void _strcpy(short, char *); /* 0x1E326; target evidence: void far _strcpy(int, char *); */
#endif
#ifndef STUNTS_LOCAL_FN__stricmp
extern short _stricmp(int); /* 0x1E4E6; target evidence: int far _stricmp(long);; arity mismatch; source call counts [2] */
#endif
#ifndef STUNTS_LOCAL_FN__strlen
extern short _strlen(short); /* 0x1E384; target evidence: int far _strlen(int); */
#endif
#ifndef STUNTS_LOCAL_FN__strrchr
extern short _strrchr(short, char); /* 0x1E810; target evidence: int far _strrchr(int, char); */
#endif
#ifndef STUNTS_LOCAL_FN__ultoa
extern char *_ultoa(unsigned long, char *, int); /* 0x1E3BC; target evidence: void near _ultoa(int  , int  , int  , int  ); */
#endif
#ifndef STUNTS_LOCAL_FN__write
extern short _write(unsigned short, short, short); /* 0x1DF28; target evidence: int far _write(unsigned int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_abs
extern int abs(int); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_add_exit_handler
extern I16 far add_exit_handler(void (far *callback)(void));
#endif
#ifndef STUNTS_LOCAL_FN_apply_audio_frame
extern void apply_audio_frame(struct AUDIO_CAR_FRAME *, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_apply_audio_voice_event
extern void apply_audio_voice_event(int16_t, uint8_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_audio_add_driver_timer
extern void audio_add_driver_timer(void); /* 0x16BAE; target evidence: void far audio_add_driver_timer(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_carstate
extern void audio_carstate(void); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_check_flag
extern I16 FAR audio_check_flag(void FAR *res, I16 chunk, U8 priority, U16 volume); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_check_flag2
extern I16 FAR audio_check_flag2(void FAR *res, I16 chunk, U8 priority); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_chunk_is_unavailable
extern int16_t audio_chunk_is_unavailable(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_audio_disable_flag2
extern void audio_disable_flag2(void); /* 0x273B8; target evidence: void far audio_disable_flag2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_disable_flag6
extern void audio_disable_flag6(void); /* 0x276CA; target evidence: void far audio_disable_flag6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_driver_func1E
extern void FAR _loadds audio_driver_func1E(I16 first, I16 last); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_driver_func3F
extern void audio_driver_func3F(short); /* 0x2776C; target evidence: void far audio_driver_func3F(int); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_driver_timer
extern void audio_driver_timer(void); /* 0x16FA3; target evidence: void far audio_driver_timer(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_enable_flag2
extern void audio_enable_flag2(void); /* 0x273B2; target evidence: void far audio_enable_flag2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_enable_flag6
extern void audio_enable_flag6(void); /* 0x27696; target evidence: void far audio_enable_flag6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_function2
extern void far audio_function2(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_function2_wrap
extern void far audio_function2_wrap(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_init_chunk
extern void FAR audio_init_chunk(I16 first, I16 last, void FAR *res, I16 offset, U8 volume, U8 priority); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_init_chunk2
extern void FAR audio_init_chunk2(I16 chunk); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_init_engine
extern int far audio_init_engine(int unused, u8 huge *blob, void far *lookup, void far *resource); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_load_driver
extern I16 FAR audio_load_driver(I8 *filename, I16 unused, I16 signature); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_make_filename
extern I8 *audio_make_filename(I8 *filename, I8 *extension, I8 *prefix); /* SOURCE: src/audio_make_filename.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_map_song_instruments
extern void FAR audio_map_song_instruments(void FAR *song, void FAR *voice); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_map_song_tracks
extern void FAR audio_map_song_tracks(U8 FAR *song); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk
extern void far audio_op_unk(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk2
extern void far audio_op_unk2(int index, u16 sample_word, int x2, int y2, int z2, int x, int y, int z, u16 speed); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk3
extern void far audio_op_unk3(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk4
extern void far audio_op_unk4(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk5
extern void far audio_op_unk5(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk6
extern void far audio_op_unk6(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_op_unk7
extern void far audio_op_unk7(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_remove_driver_timer
extern void audio_remove_driver_timer(void); /* 0x16BD5; target evidence: void far audio_remove_driver_timer(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_stop_unk
extern void audio_stop_unk(void); /* 0x20268; target evidence: void far audio_stop_unk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_stop_unknown
extern void audio_stop_unknown(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_audio_toggle_flag2
extern short audio_toggle_flag2(void); /* 0x273E8; target evidence: int far audio_toggle_flag2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_toggle_flag6
extern short audio_toggle_flag6(void); /* 0x27708; target evidence: int far audio_toggle_flag6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_unk
extern void audio_unk(void); /* 0x27216; target evidence: void far audio_unk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_unk2
extern void FAR _loadds audio_unk2(I16 voiceIndex, U8 type); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audio_unk3
extern void audio_unk3(char, short); /* 0x08CD8; target evidence: void far audio_unk3(char, int); */
#endif
#ifndef STUNTS_LOCAL_FN_audio_unload
extern void audio_unload(void); /* 0x19858; target evidence: void far audio_unload(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audiodriver_timer
extern void audiodriver_timer(void); /* 0x2863C; target evidence: void far audiodriver_timer(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audiodrv_atexit
extern void audiodrv_atexit(void); /* 0x27A64; target evidence: void far audiodrv_atexit(void); */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_compare_chunknames
extern I16 FAR audioresource_compare_chunknames(I16 caseSensitive, U8 FAR *chunkName,
                                         U8 FAR *foundName, I16 count); /* SOURCE: src/obj_seg029.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_copy_4_bytes
extern void FAR audioresource_copy_4_bytes(U8 FAR *dst, U8 FAR *src); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_copy_n_bytes
extern void FAR audioresource_copy_n_bytes(U8 FAR *src, I8 FAR *dst, I16 size); /* SOURCE: src/obj_seg029.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_find
extern I8 FAR * FAR audioresource_find(I8 HUGE *resource, U8 *chunkName); /* SOURCE: src/obj_seg029.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_get_chunk_index
extern I16 FAR audioresource_get_chunk_index(I16 stride, I16 numChunks, U8 *chunkName,
                                      I8 FAR *chunkNames); /* SOURCE: src/obj_seg029.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_get_dword
extern U32 FAR audioresource_get_dword(U32 FAR *p); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_audioresource_get_word
extern U16 FAR audioresource_get_word(U16 FAR *p); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_bto_auxiliary1
extern I16 bto_auxiliary1(I16 column, I16 row, struct VECTOR *vertices); /* SOURCE: src/obj_seg004.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_build_obj
extern void build_obj(struct VECTOR *, struct VECTOR *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_build_track_object
extern void build_track_object(void *, void *); /* 0x0E1A0; target evidence: void far build_track_object(struct ? *, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_calc_sincos80
extern void calc_sincos80(void); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_call_exitlist
extern void call_exitlist(void); /* 0x1FE59; target evidence: void far call_exitlist(void); */
#endif
#ifndef STUNTS_LOCAL_FN_call_exitlist2
extern void call_exitlist2(void); /* 0x1FE74; target evidence: void near call_exitlist2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_call_read_line
extern I16 far call_read_line(I8 *buffer, I16 x, I16 y, I16 width, I16 limit, I16 flags); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_carState_rc_op
extern I16 carState_rc_op(struct CARSTATE *car, I16 value, I16 wheel); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_car_car_coll_detect_maybe
extern I8 car_car_coll_detect_maybe(struct POINT2D *pCollPoints,
                              struct VECTOR *pWorldCrds,
                              struct POINT2D *oCollPoints,
                              struct VECTOR *oWorldCrds); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_car_car_speed_adjust_maybe
extern I8 car_car_speed_adjust_maybe(struct CARSTATE *player, struct CARSTATE *opponent); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_check_input
extern void far check_input(void); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_clear_audio_voice
extern void clear_audio_voice(struct AudioVoice *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_clear_invalid_track_tiles
extern void clear_invalid_track_tiles(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_clear_rect
extern void clear_rect(int16_t, int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_compare_ds_ss
extern short compare_ds_ss(void); /* 0x2031D; target evidence: int far compare_ds_ss(void); */
#endif
#ifndef STUNTS_LOCAL_FN_copy_material_list_pointers
extern void copy_material_list_pointers(void* clrlist, void* clrlist2, void* patlist, void* patlist2, U16S  videoConst); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_copy_paras_reverse
extern void copy_paras_reverse(short, short, short); /* 0x211D5; target evidence: void far copy_paras_reverse(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_copy_string
extern void copy_string(I8 *destination, I8 far *source); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_cos_fast
extern short cos_fast(short); /* 0x2272C; target evidence: int far cos_fast(int); */
#endif
#ifndef STUNTS_LOCAL_FN_cosfast
extern int16_t cosfast(uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_criterr_exithandler
extern void criterr_exithandler(void); /* 0x1F3BC; target evidence: void far criterr_exithandler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_criterr_interrupt_handler
extern void criterr_interrupt_handler(void); /* 0x1F35C; target evidence: void far criterr_interrupt_handler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_debug_printf_text
extern void far debug_printf_text(const char *format, ...); /* ASM forwards caller varargs to sprintf. */
#endif
#ifndef STUNTS_LOCAL_FN_detect_penalty
extern char detect_penalty(short *, short *); /* 0x07816; target evidence: char far detect_penalty(int *, int *); */
#endif
#ifndef STUNTS_LOCAL_FN_do_dea_textres
extern short do_dea_textres(void); /* 0x1A118; target evidence: int far do_dea_textres(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_dos_resource_text
extern void do_dos_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_dos_restext
extern void do_dos_restext(void); /* 0x19F5C; target evidence: void far do_dos_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_fileselect_dialog
extern I16 far do_fileselect_dialog(I8 *path, I8 *selected_name, I16 attributes,
                             I8 far *heading); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_do_joy_restext
extern void do_joy_restext(void); /* 0x19B32; target evidence: void far do_joy_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_joystick_resource_text
extern void do_joystick_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_key_resource_text
extern void do_key_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_key_restext
extern void do_key_restext(void); /* 0x19D9A; target evidence: void far do_key_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_mer_restext
extern void do_mer_restext(void); /* 0x1A200; target evidence: void far do_mer_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_mof_resource_text
extern void do_mof_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_mof_restext
extern void do_mof_restext(void); /* 0x19E98; target evidence: void far do_mof_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_mou_resource_text
extern void do_mou_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_mou_restext
extern void do_mou_restext(void); /* 0x19DF4; target evidence: void far do_mou_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_opponent_op
extern void do_opponent_op(void); /* 0x095E0; target evidence: void far do_opponent_op(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_pau_restext
extern void do_pau_restext(void); /* 0x19E4A; target evidence: void far do_pau_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_do_savefile_dialog
extern I16 far do_savefile_dialog(I8 *name, I8 *directory, I8 far *title); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_do_sinking
extern struct RECTANGLE *do_sinking(I16 frame, I16 top, I16 height); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_do_sonsof_resource_text
extern void do_sonsof_resource_text(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_do_sonsof_restext
extern void do_sonsof_restext(void); /* 0x19EFA; target evidence: void far do_sonsof_restext(void); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_2DtrackMap
extern void draw_2DtrackMap(U8  rowBase, U8  columnBase, U8  *lastElement, U8  *lastTerrain); /* SOURCE: src/obj_seg009.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_draw_button
extern void far draw_button(I8 far *caption, I16 x, I16 y, I16 width, I16 height,
                     I16 light, I16 dark, I16 sprite_id, I16 font_style); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_draw_clip
extern void draw_clip(struct RECTANGLE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_draw_filled_lines
extern short draw_filled_lines(int, short, short, short); /* 0x246BC; target evidence: int far draw_filled_lines(long, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_filled_rect
extern void draw_filled_rect(int16_t, int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_draw_ingame_text
extern struct RECTANGLE *draw_ingame_text(void); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_draw_line_related
extern short draw_line_related(short, short, short, short, void *); /* 0x1EB56; target evidence: int far draw_line_related(int, int, int, int, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_line_related_alt
extern void draw_line_related_alt(void); /* 0x1EB48; target evidence: void far draw_line_related_alt(void); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_lines_unk
extern void draw_lines_unk(int, short, short, short, short, short); /* 0x1916E; target evidence: void far draw_lines_unk(long, int, int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_lines_unknown
extern void draw_lines_unknown(int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t); /* source declaration; arity mismatch; source call counts [0, 7] */
#endif
#ifndef STUNTS_LOCAL_FN_draw_patterned_lines
extern void draw_patterned_lines(int, short, short, char); /* 0x24B96; target evidence: void far draw_patterned_lines(long, int, int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_rect_outline
extern void draw_rect_outline(int16_t, int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_draw_text_at
extern void draw_text_at(char *, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_draw_track_preview
extern void draw_track_preview(void); /* 0x0CBDC; target evidence: void far draw_track_preview(void); */
#endif
#ifndef STUNTS_LOCAL_FN_draw_unknown_lines
extern void draw_unknown_lines(int, short, short, char); /* 0x23344; target evidence: void far draw_unknown_lines(long, int, int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_end_hiscore
extern char end_hiscore(void); /* 0x03178; target evidence: char far end_hiscore(void); */
#endif
#ifndef STUNTS_LOCAL_FN_ensure_file_exists
extern void ensure_file_exists(short); /* 0x1A1A6; target evidence: void far ensure_file_exists(int); */
#endif
#ifndef STUNTS_LOCAL_FN_enter_hiscore
extern void far enter_hiscore(U16S  score, I8 far *text, U8  carStyle); /* SOURCE: src/obj_seg000.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_exit
extern void exit(int); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_fatal_error
extern void far fatal_error(const char *format, ...);
#endif
#ifndef STUNTS_LOCAL_FN_file_build_path
extern void file_build_path(I8 *dir, I8 *name, I8 *ext, I8 *dst); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_combine_and_find
extern I8 * FAR file_combine_and_find(I8 *dir, I8 *name, I8 *ext); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp
extern int file_decomp(short, short); /* 0x20DE6; target evidence: long far file_decomp(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_fatal
extern void file_decomp_fatal(short); /* 0x20E07; target evidence: void far file_decomp_fatal(int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_nofatal
extern int file_decomp_nofatal(short); /* 0x20DF7; target evidence: long far file_decomp_nofatal(int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_paras
extern short file_decomp_paras(short, short); /* 0x1FF26; target evidence: int far file_decomp_paras(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_paras_fatal
extern void file_decomp_paras_fatal(short, short); /* 0x1FF49; target evidence: void near file_decomp_paras_fatal(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_paras_nofatal
extern void file_decomp_paras_nofatal(void); /* 0x1FF38; target evidence: void far file_decomp_paras_nofatal(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_rle
extern void file_decomp_rle(int, short, short, short); /* 0x20B62; target evidence: void far file_decomp_rle(long, int  , int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_rle_seq
extern int file_decomp_rle_seq(short, int, short, short); /* 0x20CCF; target evidence: long near file_decomp_rle_seq(int  , long, int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_rle_single
extern void file_decomp_rle_single(short, int, short, short); /* 0x20BF8; target evidence: void near file_decomp_rle_single(int  , long, int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_decomp_vle
extern void file_decomp_vle(int, int); /* 0x22D7C; target evidence: void far file_decomp_vle(long, long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_find
extern short file_find(short); /* 0x1FFD4; target evidence: int far file_find(int);; arity mismatch; source call counts [0, 1] */
#endif
#ifndef STUNTS_LOCAL_FN_file_find_next
extern short file_find_next(void); /* 0x2002E; target evidence: int far file_find_next(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_find_next_alt
extern const I8* file_find_next_alt(void); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_get_res_shape_count
extern short file_get_res_shape_count(int); /* 0x2264A; target evidence: int far file_get_res_shape_count(long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_get_shape2d
extern void *file_get_shape2d(int, short); /* 0x2265B; target evidence: void far * far file_get_shape2d(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_get_unflip_size
extern U16 FAR file_get_unflip_size(I8 FAR *memchunk); /* SOURCE: src/file_get_unflip_size.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_3dres
extern void far *file_load_3dres(I8 *filename); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_audio_resource
extern void file_load_audio_resource(const char *, const char *, const char *); /* source declaration consensus; read-only arguments. */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_audiores
extern void file_load_audiores(short, short, short); /* 0x197FC; target evidence: void far file_load_audiores(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_binary
extern int file_load_binary(short, short); /* 0x20D79; target evidence: long far file_load_binary(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_binary_nofatal
extern int file_load_binary_nofatal(short); /* 0x20D88; target evidence: long far file_load_binary_nofatal(int);; arity mismatch; source call counts [0, 1] */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_replay
extern I8 file_load_replay(const I8* dir, const I8* name); /* SOURCE: src/obj_seg005.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_resfile
extern int file_load_resfile(short); /* 0x189F2; target evidence: long far file_load_resfile(int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_resource
extern void far* file_load_resource(I16 type, const I8* filename); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_resource_file
extern void far *file_load_resource_file(I8 *filename); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d
extern void FAR* file_load_shape2d(I8* shapename, I16 fatal); /* SOURCE: src/seg034_shape2d_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_expand
extern void file_load_shape2d_expand(int, int); /* 0x25FA2; target evidence: void far file_load_shape2d_expand(long, long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_expandedsize
extern I16 FAR file_load_shape2d_expandedsize(void FAR *memchunk); /* SOURCE: src/file_load_shape2d_expandedsize.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_fatal
extern void FAR* file_load_shape2d_fatal(I8* shapename); /* SOURCE: src/seg034_shape2d_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_fatal_thunk
extern int file_load_shape2d_fatal_thunk(short); /* 0x2385C; target evidence: long far file_load_shape2d_fatal_thunk(int  );; arity mismatch; source call counts [0, 1] */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_nofatal
extern void FAR* file_load_shape2d_nofatal(I8* shapename); /* SOURCE: src/seg034_shape2d_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_nofatal2
extern void FAR* file_load_shape2d_nofatal2(I8* shapename); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_nofatal_thunk
extern short file_load_shape2d_nofatal_thunk(short); /* 0x23861; target evidence: int far file_load_shape2d_nofatal_thunk(int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_palmap_apply
extern void file_load_shape2d_palmap_apply(int, short); /* 0x25F48; target evidence: void far file_load_shape2d_palmap_apply(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_palmap_init
extern void file_load_shape2d_palmap_init(U8 FAR* pal); /* SOURCE: src/seg034_shape2d_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res
extern void FAR* file_load_shape2d_res(I8* resname, I16 fatal); /* SOURCE: src/obj_seg035_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res_fatal
extern void FAR* file_load_shape2d_res_fatal(I8* resname); /* SOURCE: src/obj_seg035_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res_fatal_thunk
extern void file_load_shape2d_res_fatal_thunk(void); /* 0x23848; target evidence: void far file_load_shape2d_res_fatal_thunk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res_nofatal
extern void FAR* file_load_shape2d_res_nofatal(I8* resname); /* SOURCE: src/obj_seg035_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res_nofatal_thunk
extern void file_load_shape2d_res_nofatal_thunk(void); /* 0x2384D; target evidence: void far file_load_shape2d_res_nofatal_thunk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_res_thunk
extern void file_load_shape2d_res_thunk(void); /* 0x23852; target evidence: void far file_load_shape2d_res_thunk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_load_shape2d_thunk
extern void file_load_shape2d_thunk(void); /* 0x23866; target evidence: void far file_load_shape2d_thunk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_paras
extern short file_paras(short, short); /* 0x1FE82; target evidence: int far file_paras(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_paras_fatal
extern void file_paras_fatal(short, short); /* 0x1FEA5; target evidence: void near file_paras_fatal(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_paras_nofatal
extern void file_paras_nofatal(void); /* 0x1FE94; target evidence: void far file_paras_nofatal(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_read
extern int file_read(short, short, short, short); /* 0x20AD0; target evidence: long far file_read(int  , int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_file_read_fatal
extern void file_read_fatal(short, int); /* 0x20AEF; target evidence: void far file_read_fatal(int, long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_read_nofatal
extern int file_read_nofatal(short, short, short); /* 0x20AE0; target evidence: long far file_read_nofatal(int  , int  , int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_file_unflip_shape2d
extern void far file_unflip_shape2d(U8 far *memchunk, I8 far *mempages);
#endif
#ifndef STUNTS_LOCAL_FN_file_unflip_shape2d_pes
extern void file_unflip_shape2d_pes(int, int); /* 0x260F6; target evidence: void far file_unflip_shape2d_pes(long, long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_write_fatal
extern short file_write_fatal(short, int, int); /* 0x2250B; target evidence: int far file_write_fatal(int, long, long); */
#endif
#ifndef STUNTS_LOCAL_FN_file_write_nofatal
extern void file_write_nofatal(void); /* 0x224FA; target evidence: void far file_write_nofatal(void); */
#endif
#ifndef STUNTS_LOCAL_FN_file_write_replay
extern int16_t file_write_replay(const char *filename); /* 0x12CE8; target evidence: char far file_write_replay(int); */
#endif
#ifndef STUNTS_LOCAL_FN_find_audio_chunk_data
extern I8 FAR * FAR _loadds find_audio_chunk_data(U8 index, struct AudioChunk *chunk); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_flagchar
extern short flagchar(char); /* 0x1DE86; target evidence: int far flagchar(char); */
#endif
#ifndef STUNTS_LOCAL_FN_flush_stdin
extern void flush_stdin(void); /* 0x20A5D; target evidence: void far flush_stdin(void); */
#endif
#ifndef STUNTS_LOCAL_FN_fmtframestr
extern void fmtframestr(char *, uint16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_font_draw_text
extern void font_draw_text(char *, short, short); /* 0x23742; target evidence: void far font_draw_text(char *, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_font_op
extern short font_op(short, short); /* 0x22832; target evidence: int far font_op(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_font_op2
extern I16 far font_op2(I8 *text);
#endif
#ifndef STUNTS_LOCAL_FN_font_op2_alt
extern I16 far font_op2_alt(I8 *name); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_font_set_fontdef
extern short font_set_fontdef(void); /* 0x198A8; target evidence: int far font_set_fontdef(void); */
#endif
#ifndef STUNTS_LOCAL_FN_font_set_fontdef2
extern short font_set_fontdef2(void *); /* 0x1988A; target evidence: int far font_set_fontdef2(struct ? far *); */
#endif
#ifndef STUNTS_LOCAL_FN_font_set_unk
extern void font_set_unk(short, short); /* 0x24B0C; target evidence: void far font_set_unk(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_font_setup_unknown
extern void font_setup_unknown(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_fontsetfontdef
extern void fontsetfontdef(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_fontsetfontdef2
extern void far fontsetfontdef2(void far *data); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_format_frame_as_string
extern void format_frame_as_string(short, unsigned short, short); /* 0x198B8; target evidence: void far format_frame_as_string(int, unsigned int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_fprint
extern void fprint(short); /* 0x1DB10; target evidence: void far fprint(int); */
#endif
#ifndef STUNTS_LOCAL_FN_frame_callback
extern void frame_callback(void); /* 0x12596; target evidence: void far frame_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_free_player_cars
extern void free_player_cars(void); /* 0x139B4; target evidence: void far free_player_cars(void); */
#endif
#ifndef STUNTS_LOCAL_FN_free_sdgame2
extern void free_sdgame2(void); /* 0x0D92A; target evidence: void far free_sdgame2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_0
extern short get_0(void); /* 0x2A45C; target evidence: int far get_0(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_a_poly_info
extern void get_a_poly_info(void); /* 0x15FF6; target evidence: void far get_a_poly_info(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_joy_flags
extern short get_joy_flags(void); /* 0x205FC; target evidence: int far get_joy_flags(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_kb_or_joy_flags
extern short get_kb_or_joy_flags(void); /* 0x20538; target evidence: int far get_kb_or_joy_flags(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_kevinrandom
extern short get_kevinrandom(void); /* 0x09E7B; target evidence: int far get_kevinrandom(void); */
#endif
#ifndef STUNTS_LOCAL_FN_get_kevinrandom_seed
extern void get_kevinrandom_seed(void *); /* 0x09E4E; target evidence: void far get_kevinrandom_seed(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_get_super_random
extern short get_super_random(void); /* 0x1998E; target evidence: int far get_super_random(void); */
#endif
#ifndef STUNTS_LOCAL_FN_getnum
extern short getnum(short *, char *); /* 0x1DE06; target evidence: int far getnum(int *, char *); */
#endif
#ifndef STUNTS_LOCAL_FN_handle_ingame_kb_shortcuts
extern I8 handle_ingame_kb_shortcuts(unsigned key); /* SOURCE: src/obj_seg005.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_heapsort_by_order
extern short heapsort_by_order(short, short, short); /* 0x26BE8; target evidence: int far heapsort_by_order(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_heapsortorder
extern void heapsortorder(int16_t, int16_t *, int16_t *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_highscore_text_unk
extern short highscore_text_unk(void); /* 0x0168E; target evidence: int far highscore_text_unk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_highscore_text_unknown
extern void highscore_text_unknown(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_highscore_write_a
extern char highscore_write_a(short); /* 0x01588; target evidence: char far highscore_write_a(int); */
#endif
#ifndef STUNTS_LOCAL_FN_highscore_write_b
extern void highscore_write_b(void); /* 0x01BB4; target evidence: void far highscore_write_b(void); */
#endif
#ifndef STUNTS_LOCAL_FN_hiscore_draw_text
extern I16 * hiscore_draw_text(I8 *str, I16 x, I16 y, I16 color, I16 shadow); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_init_audio_resources
extern void FAR * FAR init_audio_resources(void FAR *song, void FAR *voice, I8 *name); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_init_carstate_from_simd
extern void init_carstate_from_simd(struct CARSTATE* playerstate, struct SIMD* simd,
    I8 transmission, I32 posX, I32 posY, I32 posZ, I16S trkang); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_init_crak
extern struct RECTANGLE *init_crak(I16 frame, I16 top, I16 height); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_init_div0
extern void init_div0(void); /* 0x09EE8; target evidence: void far init_div0(void); */
#endif
#ifndef STUNTS_LOCAL_FN_init_game_state
extern void init_game_state(short); /* 0x06B02; target evidence: void far init_game_state(int); */
#endif
#ifndef STUNTS_LOCAL_FN_init_kevinrandom
extern void init_kevinrandom(void *); /* 0x09E21; target evidence: void far init_kevinrandom(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_init_main
extern void init_main(short, short); /* 0x29E56; target evidence: void far init_main(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_init_plantrak
extern void init_plantrak(void); /* 0x093E0; target evidence: void far init_plantrak(void); */
#endif
#ifndef STUNTS_LOCAL_FN_init_polyinfo
extern void init_polyinfo(void); /* 0x14D64; target evidence: void far init_polyinfo(void); */
#endif
#ifndef STUNTS_LOCAL_FN_init_rect_arrays
extern void init_rect_arrays(void); /* 0x0A096; target evidence: void far init_rect_arrays(void); */
#endif
#ifndef STUNTS_LOCAL_FN_init_unknown
extern void init_unknown(void); /* 0x12532; target evidence: void far init_unknown(void); */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_div0
extern void initialize_div0(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_game_state
extern void initialize_game_state(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_kevin_random
extern void initialize_kevin_random(char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_main
extern void initialize_main(int16_t, char **); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_polyinfo
extern void initialize_polyinfo(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_initialize_unknown
extern void initialize_unknown(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_input_checking
extern short input_checking(short); /* 0x187C4; target evidence: int far input_checking(int); */
#endif
#ifndef STUNTS_LOCAL_FN_input_do_checking
extern short input_do_checking(short); /* 0x189E2; target evidence: int far input_do_checking(int); */
#endif
#ifndef STUNTS_LOCAL_FN_input_pop_status
extern void far input_pop_status(void); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_input_push_status
extern void input_push_status(void); /* 0x19AEC; target evidence: void far input_push_status(void); */
#endif
#ifndef STUNTS_LOCAL_FN_input_repeat_check
extern short input_repeat_check(short); /* 0x1913A; target evidence: int far input_repeat_check(int); */
#endif
#ifndef STUNTS_LOCAL_FN_insert_newest_poly_in_poly_linked_list_40ED6
extern unsigned insert_newest_poly_in_poly_linked_list_40ED6(unsigned depth, unsigned search_sorted_position); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_int86
extern int16_t int86(uint16_t, struct MouseRegs *, struct MouseRegs *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_intr0_handler
extern void intr0_handler(void); /* 0x09EC9; target evidence: void far intr0_handler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_intro_draw_text
extern short intro_draw_text(int, short, short, short); /* 0x18F98; target evidence: int far intro_draw_text(long, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_intro_op
extern void far intro_op(I16 camX, I16 camY, I16 camZ, I16 logoRotation,
                  I16 cloudRotation, I16 showOpponent, I16 useLogo,
                  struct VECTOR *cloudPoints, struct POINT2D *oldPoints,
                  I16 *oldPointCount, struct RECTANGLE inputClip,
                  struct RECTANGLE *oldClip, struct RECTANGLE *oldUnion); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_introtext
extern int16_t *introtext(char *, int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_iprint
extern void iprint(short); /* 0x1D8EA; target evidence: void far iprint(int); */
#endif
#ifndef STUNTS_LOCAL_FN_is_facing_camera
extern I8 is_facing_camera(struct POINT2D far *pts); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_itoa
extern char *itoa(int, char *, int); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_joystick_flags_to_index
extern int16_t joystick_flags_to_index(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_kb_call_readchar_callback
extern int kb_call_readchar_callback(void); /* 0x20A1C; target evidence: long far kb_call_readchar_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_check
extern void kb_check(void); /* 0x20A68; target evidence: void far kb_check(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_checking
extern short kb_checking(void); /* 0x20A35; target evidence: int far kb_checking(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_exit_handler
extern void kb_exit_handler(void); /* 0x20883; target evidence: void far kb_exit_handler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_get_char
extern short kb_get_char(void); /* 0x20519; target evidence: int far kb_get_char(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_get_key_state
extern short kb_get_key_state(void *); /* 0x20A0D; target evidence: int far kb_get_key_state(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_init_interrupt
extern void kb_init_interrupt(void); /* 0x20812; target evidence: void far kb_init_interrupt(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_int16_handler
extern void kb_int16_handler(void); /* 0x209A5; target evidence: void far kb_int16_handler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_int9_handler
extern void kb_int9_handler(void); /* 0x208C6; target evidence: void far kb_int9_handler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_parse_key
extern short kb_parse_key(short); /* 0x20404; target evidence: int far kb_parse_key(int); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_read_char
extern short kb_read_char(void); /* 0x20A21; target evidence: int far kb_read_char(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_reg_callback
extern void far kb_reg_callback(I16 code, void (far *callback)(void));
#endif
#ifndef STUNTS_LOCAL_FN_kb_shift_checking1
extern void kb_shift_checking1(void); /* 0x26AF4; target evidence: void far kb_shift_checking1(void); */
#endif
#ifndef STUNTS_LOCAL_FN_kb_shift_checking2
extern void kb_shift_checking2(void); /* 0x26B05; target evidence: void far kb_shift_checking2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_keyboard_exit_handler
extern void keyboard_exit_handler(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_keyboard_shift_checking1
extern void keyboard_shift_checking1(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_libsub_quit_to_dos
extern short libsub_quit_to_dos(short, short); /* 0x1CE03; target evidence: int near libsub_quit_to_dos(int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_libsub_quit_to_dos_alt
extern void libsub_quit_to_dos_alt(short); /* 0x1CDEC; target evidence: void near libsub_quit_to_dos_alt(int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_link_audio_shape_resources
extern void FAR link_audio_shape_resources(U8 FAR *res, void FAR *shapes); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_audio_finalize
extern void FAR load_audio_finalize(void FAR *song); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_intro_resources
extern I8S  far load_intro_resources(void); /* SOURCE: src/obj_seg000.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_opponent_data
extern void load_opponent_data(void); /* 0x117CA; target evidence: void far load_opponent_data(void); */
#endif
#ifndef STUNTS_LOCAL_FN_load_palandcursor
extern void load_palandcursor(void); /* 0x2A2C0; target evidence: void far load_palandcursor(void); */
#endif
#ifndef STUNTS_LOCAL_FN_load_sdgame2_shapes
extern void load_sdgame2_shapes(void); /* 0x0D8D2; target evidence: void far load_sdgame2_shapes(void); */
#endif
#ifndef STUNTS_LOCAL_FN_load_sfx_file
extern void FAR * FAR load_sfx_file(I8 *filename); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_sfx_ge
extern void FAR * FAR load_sfx_ge(I8 *filename, I8 *extension, I8 *kind); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_shape2d_nofatal_thunk
extern void *load_shape2d_nofatal_thunk(char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_load_shape2d_res_nofatal_thunk
extern void *load_shape2d_res_nofatal_thunk(char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_load_skybox
extern void load_skybox(char); /* 0x0D7A2; target evidence: void far load_skybox(char); */
#endif
#ifndef STUNTS_LOCAL_FN_load_song_file
extern void FAR * FAR load_song_file(I8 *filename); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_load_tracks_menu_shapes
extern void load_tracks_menu_shapes(void); /* 0x1A2BC; target evidence: void far load_tracks_menu_shapes(void); */
#endif
#ifndef STUNTS_LOCAL_FN_load_voice_file
extern void FAR * FAR load_voice_file(I8 *filename); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_loc_390C8
extern short loc_390C8(void *, void *); /* 0x290C8; target evidence: int far loc_390C8(struct ? far *, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_locate_many_resources
extern void locate_many_resources(I8 FAR *data, I8 *names, I8 FAR **result); /* SOURCE: src/obj_seg016_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_locate_shape_alt
extern I8 far *locate_shape_alt(I8 far *data, I8 *name); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_locate_shape_fatal
extern void far * far locate_shape_fatal(void far *data, I8 *name); /* returns the located resource; entry stub selects fatal behavior. */
#endif
#ifndef STUNTS_LOCAL_FN_locate_shape_nofatal
extern void far * far locate_shape_nofatal(void far *data, I8 *name); /* returns the located resource; entry stub selects fatal behavior. */
#endif
#ifndef STUNTS_LOCAL_FN_locate_sound_fatal
extern int locate_sound_fatal(int, short); /* 0x20FA9; target evidence: long far locate_sound_fatal(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_locate_text_res
extern int locate_text_res(int, void *); /* 0x18AA2; target evidence: long far locate_text_res(long, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_locate_text_resource
extern I8 far * far locate_text_resource(I8 far *data, I8 *name); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_loop_game
extern void loop_game(I16 mode, I16 frame_index, I16 frame_offset); /* SOURCE: src/obj_seg005.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_main
extern int main(int, char **); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_mat_invert
extern void mat_invert(struct MATRIX *, struct MATRIX *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_mat_mul_vector
extern short mat_mul_vector(int, short); /* 0x228EE; target evidence: int far mat_mul_vector(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_mat_mul_vector2
extern void mat_mul_vector2(struct VECTOR *invec, struct MATRIX far *mat, struct VECTOR *outvec); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mat_multiply
extern void far mat_multiply(struct MATRIX *right, struct MATRIX *left, struct MATRIX *output);
#endif
#ifndef STUNTS_LOCAL_FN_mat_rot_x
extern void mat_rot_x(struct MATRIX *outmat, I16 angle); /* SOURCE: src/seg024_matrot.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mat_rot_y
extern void mat_rot_y(void *, short); /* 0x26F80; target evidence: void far mat_rot_y(struct ? *, int); */
#endif
#ifndef STUNTS_LOCAL_FN_mat_rot_z
extern void mat_rot_z(struct MATRIX *outmat, I16 angle); /* SOURCE: src/seg024_matrot.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mat_rot_zxy
extern void *mat_rot_zxy(short, short, short, char); /* 0x161FA; target evidence: void * far mat_rot_zxy(int, int, int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_mat_vec
extern void mat_vec(struct VECTOR *, struct MATRIX *, struct VECTOR *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_matroty
extern void matroty(struct MATRIX *, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_matrotzxy
extern struct MATRIX *matrotzxy(int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_alloc_a000
extern void mmgr_alloc_a000(void); /* 0x210F1; target evidence: void far mmgr_alloc_a000(void); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_alloc_pages
extern int mmgr_alloc_pages(short, short); /* 0x21248; target evidence: long far mmgr_alloc_pages(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_alloc_resbytes
extern void FAR *mmgr_alloc_resbytes(const I8 *name, I32 size); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_alloc_resmem
extern short mmgr_alloc_resmem(short); /* 0x2107A; target evidence: int far mmgr_alloc_resmem(int); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_copy_paras
extern void mmgr_copy_paras(int, short); /* 0x2118D; target evidence: void far mmgr_copy_paras(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_find_free
extern void mmgr_find_free(void); /* 0x212FD; target evidence: void far mmgr_find_free(void); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_free
extern short mmgr_free(int); /* 0x2147C; target evidence: int far mmgr_free(long); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_get_chunk_by_name
extern int mmgr_get_chunk_by_name(short); /* 0x2136A; target evidence: long far mmgr_get_chunk_by_name(int); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_get_chunk_size
extern U16S far mmgr_get_chunk_size(void far *resource); /* returns the 16-bit MEMCHUNK.ressize field. */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_get_chunk_size_bytes
extern U32 FAR mmgr_get_chunk_size_bytes(I8 FAR *ptr); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_get_ofs_diff
extern int mmgr_get_ofs_diff(void); /* 0x2117B; target evidence: long far mmgr_get_ofs_diff(void); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_get_res_ofs_diff_scaled
extern U32 mmgr_get_res_ofs_diff_scaled(void); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_op_unk
extern void far * far mmgr_op_unk(void far *resource);
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_path_to_name
extern short mmgr_path_to_name(short); /* 0x21228; target evidence: int far mmgr_path_to_name(int); */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_release
extern void far mmgr_release(void far *resource); /* ASM reads the resource handle; source callers pass one far pointer. */
#endif
#ifndef STUNTS_LOCAL_FN_mmgr_resize_memory
extern void far mmgr_resize_memory(void far *resource, U16S size);
#endif
#ifndef STUNTS_LOCAL_FN_mouse_draw_opaque
extern void far mouse_draw_opaque(void); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_draw_opaque_check
extern short mouse_draw_opaque_check(void); /* 0x18DB6; target evidence: int far mouse_draw_opaque_check(void); */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_draw_transparent
extern void far mouse_draw_transparent(void); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_draw_transparent_check
extern short mouse_draw_transparent_check(void); /* 0x18D9E; target evidence: int far mouse_draw_transparent_check(void); */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_get_state
extern void FAR mouse_get_state(U16 *ax, U16 *cx, U16 *dx); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_init
extern I16 FAR mouse_init(U16 width, U16 height); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_minmax_position
extern void mouse_minmax_position(short); /* 0x13A50; target evidence: void far mouse_minmax_position(int); */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_multi_hittest
extern I16 far mouse_multi_hittest(I16 count, I16 *left, I16 *right, I16 *top, I16 *bottom); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_set_minmax
extern void FAR mouse_set_minmax(U16 xmin, U16 ymin,
                          U16 xmax, U16 ymax); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_set_pixratio
extern void FAR mouse_set_pixratio(U16 xratio, U16 yratio); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_set_position
extern void FAR mouse_set_position(U16 x, U16 y); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_timer_sprite_unk
extern short mouse_timer_sprite_unk(short, short, short, short, short, short, short); /* 0x19786; target evidence: int far mouse_timer_sprite_unk(int, int, int, int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_timer_sprite_unknown
extern int16_t mouse_timer_sprite_unknown(int16_t, int16_t *, int16_t *, int16_t *, int16_t *, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_mouse_track_op
extern I16 far mouse_track_op(I16 op, I16 x, I16 width, I16 top, I16 height,
                       I16 value, I16 offset, I16 divisions); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_msdrawopaquechk
extern void msdrawopaquechk(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_msdrawtransparentchk
extern void msdrawtransparentchk(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_mulscl
extern int16_t mulscl(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_multiply_and_scale
extern short multiply_and_scale(short, short); /* 0x20044; target evidence: int far multiply_and_scale(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_19DE8
extern void nopsub_19DE8(short); /* 0x09DE8; target evidence: void far nopsub_19DE8(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_19DFF
extern void nopsub_19DFF(short); /* 0x09DFF; target evidence: void far nopsub_19DFF(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_19E09
extern void nopsub_19E09(short); /* 0x09E09; target evidence: void far nopsub_19E09(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_19E13
extern void nopsub_19E13(short); /* 0x09E13; target evidence: void far nopsub_19E13(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_26552
extern I32 nopsub_26552(I32 value); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_27220
extern void far nopsub_27220(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_2726C
extern void far nopsub_2726C(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_272B0
extern void far nopsub_272B0(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_27489
extern int far nopsub_27489(int index); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_28F26
extern void nopsub_28F26(void); /* 0x18F26; target evidence: void far nopsub_28F26(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_2F424
extern void nopsub_2F424(short, short, short, short, short); /* 0x1F424; target evidence: void far nopsub_2F424(int  , int  , int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_30180
extern void nopsub_30180(void); /* 0x20180; target evidence: void far nopsub_30180(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_304AF
extern void nopsub_304AF(void); /* 0x204AF; target evidence: void far nopsub_304AF(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_304B6
extern void nopsub_304B6(void); /* 0x204B6; target evidence: void far nopsub_304B6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_305C8
extern void nopsub_305C8(void); /* 0x205C8; target evidence: void far nopsub_305C8(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_307FA
extern void nopsub_307FA(void); /* 0x207FA; target evidence: void far nopsub_307FA(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_30A77
extern void nopsub_30A77(void); /* 0x20A77; target evidence: void far nopsub_30A77(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_30A97
extern void nopsub_30A97(short, short); /* 0x20A97; target evidence: void far nopsub_30A97(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_310FE
extern void nopsub_310FE(short); /* 0x210FE; target evidence: void far nopsub_310FE(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3111D
extern void nopsub_3111D(short); /* 0x2111D; target evidence: void far nopsub_3111D(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31157
extern void nopsub_31157(void); /* 0x21157; target evidence: void far nopsub_31157(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31169
extern void nopsub_31169(void); /* 0x21169; target evidence: void far nopsub_31169(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31429
extern void nopsub_31429(short); /* 0x21429; target evidence: void far nopsub_31429(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31525
extern void nopsub_31525(int); /* 0x21525; target evidence: void far nopsub_31525(long); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31F39
extern void nopsub_31F39(void); /* 0x21F39; target evidence: void far nopsub_31F39(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_31F55
extern void nopsub_31F55(short, short, short, short); /* 0x21F55; target evidence: void near nopsub_31F55(int  , int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3215A
extern void nopsub_3215A(void); /* 0x2215A; target evidence: void far nopsub_3215A(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3216C
extern int nopsub_3216C(void); /* 0x2216C; target evidence: long far nopsub_3216C(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3219D
extern void nopsub_3219D(short, char, char, short, short, short, short); /* 0x2219D; target evidence: void far nopsub_3219D(int  , char, char, int  , int  , int  , int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_322B4
extern void nopsub_322B4(short); /* 0x222B4; target evidence: void far nopsub_322B4(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_322C0
extern void nopsub_322C0(short, short); /* 0x222C0; target evidence: void far nopsub_322C0(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_322DF
extern void nopsub_322DF(short, short); /* 0x222DF; target evidence: void far nopsub_322DF(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_326BA
extern void nopsub_326BA(int, short, void *); /* 0x226BA; target evidence: void far nopsub_326BA(long, int, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_32738
extern void nopsub_32738(unsigned int, unsigned short); /* 0x22738; target evidence: void far nopsub_32738(unsigned long, unsigned int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_32746
extern void nopsub_32746(short); /* 0x22746; target evidence: void far nopsub_32746(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_32751
extern void nopsub_32751(short); /* 0x22751; target evidence: void far nopsub_32751(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3276A
extern void nopsub_3276A(short, unsigned short); /* 0x2276A; target evidence: void far nopsub_3276A(int, unsigned int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_328C9
extern void nopsub_328C9(short, short, short, short); /* 0x228C9; target evidence: void far nopsub_328C9(int  , int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_328DB
extern void nopsub_328DB(short, short, short, short); /* 0x228DB; target evidence: void far nopsub_328DB(int  , int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_32FEE
extern void nopsub_32FEE(void); /* 0x22FEE; target evidence: void far nopsub_32FEE(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33006
extern void nopsub_33006(void); /* 0x23006; target evidence: void far nopsub_33006(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_3320E
extern void nopsub_3320E(int, short, short, short, short); /* 0x2320E; target evidence: void far nopsub_3320E(long, int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33330
extern void nopsub_33330(short, short, short, short, short, short, short); /* 0x23330; target evidence: void far nopsub_33330(int  , int  , int  , int  , int  , int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_339FA
extern void nopsub_339FA(int, short, short); /* 0x239FA; target evidence: void far nopsub_339FA(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33AC0
extern void nopsub_33AC0(int, short, short); /* 0x23AC0; target evidence: void far nopsub_33AC0(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33AE4
extern void nopsub_33AE4(int, short, short); /* 0x23AE4; target evidence: void far nopsub_33AE4(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33B98
extern void nopsub_33B98(int, short, short); /* 0x23B98; target evidence: void far nopsub_33B98(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33D0C
extern void nopsub_33D0C(int, short, short); /* 0x23D0C; target evidence: void far nopsub_33D0C(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33DBE
extern void nopsub_33DBE(int, short, short); /* 0x23DBE; target evidence: void far nopsub_33DBE(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_33E90
extern void nopsub_33E90(int, short, short); /* 0x23E90; target evidence: void far nopsub_33E90(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_34736
extern void nopsub_34736(int, short, short); /* 0x24736; target evidence: void far nopsub_34736(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_367E4
extern void FAR nopsub_367E4(I8 FAR *data, I8 *names, I8 FAR **results); /* SOURCE: src/obj_seg016_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_36826
extern void FAR nopsub_36826(I8 FAR *data, I8 *names, I8 FAR **results); /* SOURCE: src/obj_seg016_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_36868
extern void FAR nopsub_36868(I8 FAR *data, I8 *names, I8 FAR **results); /* SOURCE: src/obj_seg016_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_36A9A
extern void nopsub_36A9A(short, short); /* 0x26A9A; target evidence: void far nopsub_36A9A(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_36ACA
extern void FAR nopsub_36ACA(U16 ymin, U16 ymax); /* SOURCE: src/seg017_mouse_whole.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_36AF2
extern void nopsub_36AF2(void); /* 0x26AF2; target evidence: void far nopsub_36AF2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_373FE
extern I16 FAR nopsub_373FE(void); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_37456
extern I16 FAR nopsub_37456(void FAR *res); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_37750
extern void FAR nopsub_37750(U16 chunk, I32 value); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_37898
extern void nopsub_37898(short); /* 0x27898; target evidence: void far nopsub_37898(int); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_378AE
extern U16 FAR nopsub_378AE(I16 index); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_378BC
extern U16 FAR nopsub_378BC(I16 index); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_37D7A
extern void FAR * FAR nopsub_37D7A(I8 *filename); /* SOURCE: src/obj_seg027.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_38570
extern void nopsub_38570(void); /* 0x28570; target evidence: void far nopsub_38570(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_kb_get_readchar_callback
extern void nopsub_kb_get_readchar_callback(void); /* 0x20A55; target evidence: void far nopsub_kb_get_readchar_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_nopsub_kb_set_readchar_callback
extern void nopsub_kb_set_readchar_callback(short, short); /* 0x20A44; target evidence: void far nopsub_kb_set_readchar_callback(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_nullsub_1
extern void nullsub_1(void); /* SOURCE: src/obj_seg031.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_nullsub_2
extern void far nullsub_2(void far *resource, int16_t selector); /* callsites push a 4-byte far pointer and a 16-bit selector; stub body ignores both. */
#endif
#ifndef STUNTS_LOCAL_FN_opponent_op
extern void opponent_op(void); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_pad_id
extern char * far pad_id(u32 far *id); /* SOURCE: src/obj_seg007.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_parse_filepath_separators
extern void parse_filepath_separators(I8 *dest, I8 *path); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_parse_shape2d
extern void parse_shape2d(void FAR *memchunk, void FAR *mempages); /* SOURCE: src/obj_seg035_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_parse_shape2d_helper
extern I32 far parse_shape2d_helper(U8 far *position);
#endif
#ifndef STUNTS_LOCAL_FN_parse_shape2d_helper2
extern int parse_shape2d_helper2(int); /* 0x1F334; target evidence: long far parse_shape2d_helper2(long); */
#endif
#ifndef STUNTS_LOCAL_FN_parse_shape2d_helper3
extern I16 parse_shape2d_helper3(I8 FAR *source); /* SOURCE: src/obj_seg035_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_parse_shape2d_thunk
extern void parse_shape2d_thunk(void); /* 0x23857; target evidence: void far parse_shape2d_thunk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_plane_origin_op
extern short plane_origin_op(short, short, short, short); /* 0x09926; target evidence: int far plane_origin_op(int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_plane_rotate_op
extern void plane_rotate_op(void); /* 0x09794; target evidence: void far plane_rotate_op(void); */
#endif
#ifndef STUNTS_LOCAL_FN_player_op
extern void player_op(I8 inputByte); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_plnoriginop
extern int16_t plnoriginop(int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_plnrotop
extern void plnrotop(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_point_in_rectangle
extern char point_in_rectangle(int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_polang
extern int16_t polang(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_polarAngle
extern short polarAngle(int); /* 0x1EA4E; target evidence: int far polarAngle(long); */
#endif
#ifndef STUNTS_LOCAL_FN_polarRadius2D
extern I16 far polarRadius2D(I16 z, I16 y);
#endif
#ifndef STUNTS_LOCAL_FN_polarRadius3D
extern I16 polarRadius3D(struct VECTOR *vec); /* SOURCE: src/polarRadius3D.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_poll_input_abort
extern int16_t poll_input_abort(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_polradius2d
extern int16_t polradius2d(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_polyinfo
extern void polyinfo(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_polyinfo_reset
extern void polyinfo_reset(void); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_ported_stuntsmain_
extern short ported_stuntsmain_(int); /* 0x00000; target evidence: int far ported_stuntsmain_(long); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_default
extern void preRender_default(short, short, short); /* 0x217B2; target evidence: void far preRender_default(int  , int  , int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_default_alt
extern short preRender_default_alt(short, short, short); /* 0x217C1; target evidence: int far preRender_default_alt(int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_helper
extern void preRender_helper(void); /* 0x219CD; target evidence: void near preRender_helper(void); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_helper2
extern void preRender_helper2(void); /* 0x21A67; target evidence: void near preRender_helper2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_helper3
extern void preRender_helper3(void); /* 0x21B5E; target evidence: void near preRender_helper3(void); */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_icons
extern void preRender_icons(U8  mode); /* SOURCE: src/obj_seg009.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_line
extern void far preRender_line(I16 start_x, I16 start_y, I16 end_x, I16 end_y, I16 color);
#endif
#ifndef STUNTS_LOCAL_FN_preRender_patterned
extern void far preRender_patterned(I16 pattern, I16 fill, I16 point_count, struct POINT2D *points);
#endif
#ifndef STUNTS_LOCAL_FN_preRender_sphere
extern void far preRender_sphere(I16 x1, I16 y1, I16 x2, I16 color);
#endif
#ifndef STUNTS_LOCAL_FN_preRender_sphere_helper
extern void preRender_sphere_helper(I16 source, I16 destination); /* SOURCE: src/preRender_sphere_helper.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_sphere_helper2
extern void FAR preRender_sphere_helper2(struct Point *source, struct Point *output); /* SOURCE: src/preRender_sphere_helper2.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_unk
extern void far preRender_unk(I16 pattern, I16 color_list, I16 fill, I16 point_count, struct POINT2D *points);
#endif
#ifndef STUNTS_LOCAL_FN_preRender_wheel
extern void FAR preRender_wheel(struct WheelRect *rect, I16 count, I16 color,
    I16 styleA, I16 styleB); /* SOURCE: src/preRender_wheel.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_wheel_helper
extern void FAR preRender_wheel_helper(struct WheelRect *rect, struct Point *coords, I16 count); /* SOURCE: src/preRender_wheel_helper.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_wheel_helper2
extern void preRender_wheel_helper2(struct WheelRect *rect, struct Point *output, I16 scale); /* SOURCE: src/preRender_wheel_helper2.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_wheel_helper3
extern void preRender_wheel_helper3(Point *source, Point *output); /* SOURCE: src/preRender_wheel_helper3.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_preRender_wheel_helper4
extern void preRender_wheel_helper4(short, short, short, short, short, short, short, short, short, short); /* 0x217EE; target evidence: void far preRender_wheel_helper4(int, int, int, int  , int  , int  , int  , int  , int  , int  );; arity mismatch; source call counts [3, 10] */
#endif
#ifndef STUNTS_LOCAL_FN_print_highscore_entry
extern void far print_highscore_entry(I16 rowIndex, I8 *stringOffsets); /* SOURCE: src/obj_seg000.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_print_int_as_string_maybe
extern void far print_int_as_string_maybe(I8 *buffer, I16 value, I16 zero_fill, I16 width); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_process_audio_chunk_event
extern void process_audio_chunk_event(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_process_audio_event
extern int16_t process_audio_event(struct AudioEvent *, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_process_effect_audio_chunks
extern void process_effect_audio_chunks(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_process_music_audio_chunks
extern void process_music_audio_chunks(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_projectiondata9_times_ratio
extern short projectiondata9_times_ratio(short, unsigned short); /* 0x2275C; target evidence: int far projectiondata9_times_ratio(int, unsigned int); */
#endif
#ifndef STUNTS_LOCAL_FN_putbuf
extern void putbuf(char *, short); /* 0x1DC6E; target evidence: void far putbuf(char far *, int); */
#endif
#ifndef STUNTS_LOCAL_FN_putpad
extern short putpad(short); /* 0x1DC0E; target evidence: int far putpad(int); */
#endif
#ifndef STUNTS_LOCAL_FN_putpixel_iconFillings
extern void putpixel_iconFillings(int, short, short); /* 0x24212; target evidence: void far putpixel_iconFillings(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_putpixel_iconMask
extern void putpixel_iconMask(int, short, short); /* 0x23A1E; target evidence: void far putpixel_iconMask(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_putpixel_line1_maybe
extern short putpixel_line1_maybe(void *); /* 0x233C0; target evidence: int far putpixel_line1_maybe(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_putpixel_single_maybe
extern void far putpixel_single_maybe(I16 x, I16 y, I16 color);
#endif
#ifndef STUNTS_LOCAL_FN_putprefix
extern void putprefix(void); /* 0x1DDDC; target evidence: void far putprefix(void); */
#endif
#ifndef STUNTS_LOCAL_FN_putsign
extern void putsign(void); /* 0x1DDC4; target evidence: void far putsign(void); */
#endif
#ifndef STUNTS_LOCAL_FN_rand
extern int rand(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_random_wait
extern void random_wait(void); /* 0x2A25C; target evidence: void far random_wait(void); */
#endif
#ifndef STUNTS_LOCAL_FN_rcintersect
extern char rcintersect(struct RECTANGLE *, struct RECTANGLE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_rcunion
extern void rcunion(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_read_audio_event
extern void FAR _loadds read_audio_event(struct AudioEvent *event, U8 FAR *stream); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_read_file_with_retry
extern void far *read_file_with_retry(I16 type, U16  first, U16  second, U16  third); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_read_line
extern I16 FAR read_line(I8 flags, I8 *buffer, I16 pendingKey, I16 bufferLimit,
                  I16 maxWidth, I16 x, I16 y,
                  I16 (FAR *readCallback)(void), I16 timerOffset, I16 timerSegment); /* SOURCE: src/obj_seg032_group.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_read_line_helper
extern void read_line_helper(void); /* 0x2A7F6; target evidence: void far read_line_helper(void); */
#endif
#ifndef STUNTS_LOCAL_FN_read_line_helper2
extern void read_line_helper2(void); /* 0x2A896; target evidence: void far read_line_helper2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_rect_adjust_from_point
extern void rect_adjust_from_point(struct POINT2D *pt, struct RECTANGLE *rc); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rect_array_sort_by_top
extern void rect_array_sort_by_top(char, short, short *); /* 0x16B4A; target evidence: void far rect_array_sort_by_top(char, int, int *); */
#endif
#ifndef STUNTS_LOCAL_FN_rect_compare_point
extern unsigned rect_compare_point(struct POINT2D *point); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rect_intersect
extern char rect_intersect(int); /* 0x165EC; target evidence: char far rect_intersect(long); */
#endif
#ifndef STUNTS_LOCAL_FN_rect_is_adjacent
extern I8 rect_is_adjacent(struct RECTANGLE* r1, struct RECTANGLE* r2); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rect_is_inside
extern I8 rect_is_inside(struct RECTANGLE *a,struct RECTANGLE *b); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rect_is_overlapping
extern I8 rect_is_overlapping(struct RECTANGLE* r1, struct RECTANGLE* r2); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rect_union
extern void rect_union(void *, void *, void *); /* 0x16572; target evidence: void far rect_union(struct ? *, struct ? *, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_rectlist_add
extern void rectlist_add(uint8_t, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_rectlist_add_rect
extern void rectlist_add_rect(I8* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr, struct RECTANGLE* rect); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_rectlist_add_rects
extern void rectlist_add_rects(unsigned char, short, short, short, short, int); /* 0x16A52; target evidence: void far rectlist_add_rects(unsigned char, int, int, int, int, long); */
#endif
#ifndef STUNTS_LOCAL_FN_rectsorttop
extern void rectsorttop(char, struct RECTANGLE *, int16_t *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_release_audio_chunk
extern void release_audio_chunk(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_release_shape_resources
extern void release_shape_resources(void *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_remove_frame_callback
extern void remove_frame_callback(void); /* 0x12576; target evidence: void far remove_frame_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_replay_axis_value
extern char replay_axis_value(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_replay_unk
extern void replay_unk(void); /* 0x13A98; target evidence: void far replay_unk(void); */
#endif
#ifndef STUNTS_LOCAL_FN_replay_unk2
extern void far replay_unk2(I16 mode); /* SOURCE: src/obj_seg005.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_reserve_audio_chunk
extern int16_t reserve_audio_chunk(int16_t, uint8_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_audio_chunks
extern void reset_audio_chunks(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_audio_driver_state
extern void reset_audio_driver_state(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_audio_event_state
extern void reset_audio_event_state(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_audio_voice_length
extern void reset_audio_voice_length(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_idle_counters
extern void reset_idle_counters(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_reset_joystick_selection
extern void reset_joystick_selection(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_restore_audio_volume
extern void restore_audio_volume(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_restore_gamestate
extern void restore_gamestate(short); /* 0x06F3A; target evidence: void far restore_gamestate(int); */
#endif
#ifndef STUNTS_LOCAL_FN_restore_mouse_sprite
extern void restore_mouse_sprite(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_run_car_menu
extern void far run_car_menu(I8 *caridptr, I8 *materialofs,
                      I8 *transmissionofs, I16 opponenttype); /* SOURCE: src/obj_seg000.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_run_game
extern void run_game(void); /* 0x11B7A; target evidence: void far run_game(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_intro
extern short run_intro(void); /* 0x0069C; target evidence: int far run_intro(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_intro_looped
extern short run_intro_looped(void); /* 0x0059A; target evidence: int far run_intro_looped(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_menu
extern char run_menu(void); /* 0x00F3C; target evidence: char far run_menu(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_opponent_menu
extern void run_opponent_menu(void); /* 0x0293C; target evidence: void far run_opponent_menu(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_option_menu
extern char run_option_menu(void); /* 0x02F4A; target evidence: char far run_option_menu(void); */
#endif
#ifndef STUNTS_LOCAL_FN_run_tracks_menu
extern void run_tracks_menu(short); /* 0x010D0; target evidence: void far run_tracks_menu(int); */
#endif
#ifndef STUNTS_LOCAL_FN_security_check
extern void security_check(int16_t selection); /* 0x044CF; target evidence: void far security_check(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_select_audio_voice_slot
extern I16 FAR _loadds select_audio_voice_slot(I8 FAR *sample, struct AudioChunk *chunk); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_select_cliprect_rotate
extern short select_cliprect_rotate(int, short, short *, short); /* 0x14E06; target evidence: int far select_cliprect_rotate(long, int, int *, int); */
#endif
#ifndef STUNTS_LOCAL_FN_select_rot
extern unsigned select_rot(I16 angZ, I16 angX, I16 angY, struct RECTANGLE* cliprect, I16 unk); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_send_audio_stop_event
extern void far send_audio_stop_event(I16 first, I16 chunk_index); /* defining TU is void; obj_seg007.c consumes a value (PORT mismatch). */
#endif
#ifndef STUNTS_LOCAL_FN_set_add_value
extern void far set_add_value(I32 ticks);
#endif
#ifndef STUNTS_LOCAL_FN_set_all_audio_chunk_volume
extern void set_all_audio_chunk_volume(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_set_audio_load_error_policy
extern void set_audio_load_error_policy(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_set_audio_voice_parameter
extern void set_audio_voice_parameter(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_set_audio_voice_value
extern void set_audio_voice_value(int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_set_bios_mode3
extern short set_bios_mode3(void); /* 0x203D8; target evidence: int far set_bios_mode3(void); */
#endif
#ifndef STUNTS_LOCAL_FN_set_criterr_handler
extern I16 far set_criterr_handler(I16 (far *callback)(void));
#endif
#ifndef STUNTS_LOCAL_FN_set_default_car
extern void set_default_car(void); /* 0x046E4; target evidence: void far set_default_car(void); */
#endif
#ifndef STUNTS_LOCAL_FN_set_fontdefseg
extern void far set_fontdefseg(void far *font_data);
#endif
#ifndef STUNTS_LOCAL_FN_set_frame_callback
extern void set_frame_callback(void); /* 0x1255A; target evidence: void far set_frame_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_set_projection
extern void set_projection(short, short, unsigned short, unsigned short); /* 0x222F3; target evidence: void far set_projection(int, int, unsigned int, unsigned int); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_aero_trackdata
extern void setup_aero_trackdata(void far *carresptr, I16 is_opponent); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_setup_car_shapes
extern void setup_car_shapes(short); /* 0x12D2E; target evidence: void far setup_car_shapes(int); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_intro
extern char setup_intro(void); /* 0x0D93C; target evidence: char far setup_intro(void); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_mcgawnd1
extern void setup_mcgawnd1(void); /* 0x2A958; target evidence: void far setup_mcgawnd1(void); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_mcgawnd2
extern void setup_mcgawnd2(void); /* 0x2A9A0; target evidence: void far setup_mcgawnd2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_player_cars
extern short setup_player_cars(void); /* 0x13702; target evidence: int far setup_player_cars(void); */
#endif
#ifndef STUNTS_LOCAL_FN_setup_track
extern char setup_track(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unk
extern void shape2d_op_unk(int); /* 0x23E00; target evidence: void far shape2d_op_unk(long); */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unk2
extern void shape2d_op_unk2(int, short, short); /* 0x23EB4; target evidence: void far shape2d_op_unk2(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unk3
extern void shape2d_op_unk3(int); /* 0x23ED2; target evidence: void far shape2d_op_unk3(long); */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unk4
extern void shape2d_op_unk4(int); /* 0x242F6; target evidence: void far shape2d_op_unk4(long); */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unk5
extern void shape2d_op_unk5(int, short, short); /* 0x23DE2; target evidence: void far shape2d_op_unk5(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_op_unknown5
extern void shape2d_op_unknown5(void *, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_shape2d_render_bmp_as_mask
extern void shape2d_render_bmp_as_mask(int); /* 0x23B02; target evidence: void far shape2d_render_bmp_as_mask(long); */
#endif
#ifndef STUNTS_LOCAL_FN_shape3d_free_all
extern void shape3d_free_all(void); /* 0x0FF5E; target evidence: void far shape3d_free_all(void); */
#endif
#ifndef STUNTS_LOCAL_FN_shape3d_free_car_shapes
extern void shape3d_free_car_shapes(void); /* SOURCE: src/obj_seg004.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_shape3d_init_shape
extern void shape3d_init_shape(I8 far *shapeptr, struct SHAPE3D *gameshape); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_shape3d_load_all
extern short shape3d_load_all(void); /* 0x0FE94; target evidence: int far shape3d_load_all(void); */
#endif
#ifndef STUNTS_LOCAL_FN_shape3d_load_car_shapes
extern void shape3d_load_car_shapes(I8 arg_playercarid[], I8 arg_opponentcarid[]); /* SOURCE: src/obj_seg004.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_shape_op_explosion
extern void shape_op_explosion(unsigned short, int, short, short); /* 0x247DC; target evidence: void far shape_op_explosion(unsigned int, long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_shapeexpl
extern void shapeexpl(int16_t, void *, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_show_dialog
extern I16 far show_dialog(I16 type, I16 check, I8 far *message, I16 x, I16 y,
                    U16  frame_arg, I16 *disabled, I8 initial); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_show_graphic_levels_menu
extern void show_graphic_levels_menu(void); /* 0x19FB6; target evidence: void far show_graphic_levels_menu(void); */
#endif
#ifndef STUNTS_LOCAL_FN_show_waiting
extern void far show_waiting(void); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_sin_fast
extern short sin_fast(short); /* 0x226DE; target evidence: int far sin_fast(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sinfast
extern int16_t sinfast(uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_skybox_op
extern I16 skybox_op(I16 preview_index, struct RECTANGLE *clip, I16 latitude,
              struct MATRIX *rotation, I16 projection_mode, I16 detail, I16 camera_y); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_skybox_op_helper
extern void far skybox_op_helper(U16S color, U16S count, struct POINT2D p1, struct POINT2D p2, struct POINT2D p3, struct POINT2D p4);
#endif
#ifndef STUNTS_LOCAL_FN_skybox_op_helper2
extern void far skybox_op_helper2(struct RECTANGLE *rectptr, I16 x, I16 horizon); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_sprcopy2to12
extern void sprcopy2to12(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprint
extern void sprint(short); /* 0x1DA24; target evidence: void far sprint(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite1_unknown2
extern void sprite1_unknown2(int16_t, int16_t, int16_t, int16_t, int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_1_unk
extern void sprite_1_unk(short, short, short, short, char); /* 0x235D2; target evidence: void far sprite_1_unk(int, int, int, int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_1_unk2
extern short sprite_1_unk2(short, short, short, short, short); /* 0x23578; target evidence: int far sprite_1_unk2(int, int, int, int, int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_1_unk3
extern void sprite_1_unk3(int, short); /* 0x2367A; target evidence: void far sprite_1_unk3(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_1_unk4
extern void FAR sprite_1_unk4(I16 x1, I16 y1, I16 x2, I16 y2, I16 color); /* SOURCE: src/sprite_1_unk4.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_blit_to_video
extern I16 far sprite_blit_to_video(struct SPRITE far *sprite, I16 mode); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_clear_1_color
extern short sprite_clear_1_color(char); /* 0x232C0; target evidence: int far sprite_clear_1_color(char); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_clear_shape
extern void sprite_clear_shape(int); /* 0x2477E; target evidence: void far sprite_clear_shape(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_clear_shape_alt
extern void sprite_clear_shape_alt(int, short, short); /* 0x2475A; target evidence: void far sprite_clear_shape_alt(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_2_to_1
extern void sprite_copy_2_to_1(void); /* 0x25B14; target evidence: void far sprite_copy_2_to_1(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_2_to_1_2
extern short sprite_copy_2_to_1_2(void); /* 0x18F3C; target evidence: int far sprite_copy_2_to_1_2(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_2_to_1_clear
extern void sprite_copy_2_to_1_clear(void); /* 0x18F4E; target evidence: void far sprite_copy_2_to_1_clear(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_arg_to_both
extern short sprite_copy_arg_to_both(short *); /* 0x2262E; target evidence: int far sprite_copy_arg_to_both(int *); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_both_to_arg
extern void sprite_copy_both_to_arg(short); /* 0x2260E; target evidence: void far sprite_copy_both_to_arg(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_wnd_to_1
extern void sprite_copy_wnd_to_1(void); /* 0x18F6A; target evidence: void far sprite_copy_wnd_to_1(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_copy_wnd_to_1_clear
extern void sprite_copy_wnd_to_1_clear(void); /* 0x18F7C; target evidence: void far sprite_copy_wnd_to_1_clear(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_free_window
extern void sprite_free_window(struct SPRITE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_free_wnd
extern short sprite_free_wnd(int); /* 0x224AA; target evidence: int far sprite_free_wnd(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_make_window
extern struct SPRITE *sprite_make_window(uint16_t, uint16_t, uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_make_wnd
extern int sprite_make_wnd(short, short, short); /* 0x24C0C; target evidence: long far sprite_make_wnd(int, int, int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage
extern short sprite_putimage(int); /* 0x23BDA; target evidence: int far sprite_putimage(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_and
extern void sprite_putimage_and(int, short, short); /* 0x23890; target evidence: void far sprite_putimage_and(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_and_alt
extern void sprite_putimage_and_alt(int, short, short); /* 0x23BBC; target evidence: void far sprite_putimage_and_alt(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_and_alt2
extern void sprite_putimage_and_alt2(int, short, short); /* 0x2386C; target evidence: void far sprite_putimage_and_alt2(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_or
extern void sprite_putimage_or(int, short, short); /* 0x24084; target evidence: void far sprite_putimage_or(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_or_alt
extern void sprite_putimage_or_alt(int, short, short); /* 0x24060; target evidence: void far sprite_putimage_or_alt(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_putimage_transparent
extern void sprite_putimage_transparent(int, short, short); /* 0x243B0; target evidence: void far sprite_putimage_transparent(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_set_1_from_argptr
extern short sprite_set_1_from_argptr(int); /* 0x25AF6; target evidence: int far sprite_set_1_from_argptr(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_set_1_size
extern void sprite_set_1_size(short, short, short, short); /* 0x2327F; target evidence: void far sprite_set_1_size(int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_setup1_from_arg_pointer
extern void sprite_setup1_from_arg_pointer(struct SPRITE *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_shape_to_1
extern void sprite_shape_to_1(int, short, short); /* 0x23D30; target evidence: void far sprite_shape_to_1(long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sprite_shape_to_1_alt
extern void sprite_shape_to_1_alt(int); /* 0x23D4E; target evidence: void far sprite_shape_to_1_alt(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sprputimage
extern void sprputimage(void *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sprset1size
extern void sprset1size(uint16_t, uint16_t, uint16_t, uint16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_start
extern void start(void); /* 0x1CC62; target evidence: void near start(void); */
#endif
#ifndef STUNTS_LOCAL_FN_start_audio_voice_sample
extern void FAR _loadds start_audio_voice_sample(I16 voiceIndex, I8 FAR *voiceData); /* SOURCE: src/obj_seg028.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_state_op_unk
extern void state_op_unk(short, short, short); /* 0x09A2C; target evidence: void far state_op_unk(int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_strcat
extern char *strcat(char *, const char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_strcmp
extern int strcmp(const char *, const char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_strcpy
extern char *strcpy(char *, const char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_stricmp
extern int16_t stricmp(char *, char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_strlen
extern size_t strlen(const char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_strrchr
extern char *strrchr(const char *, int); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_sub_18D06
extern void sub_18D06(void *, short); /* 0x08D06; target evidence: void far sub_18D06(struct ? *, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_18D60
extern char sub_18D60(short, void *, char, char *); /* 0x08D60; target evidence: char far sub_18D60(int, struct ? *, char, char *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_19BA0
extern void sub_19BA0(void); /* 0x09BA0; target evidence: void far sub_19BA0(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_19F14
extern void sub_19F14(void *); /* 0x09F14; target evidence: void far sub_19F14(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_204AE
extern void sub_204AE(void *, short, short, void *, short, void *); /* 0x104AE; target evidence: void far sub_204AE(struct ? far *, int, int, struct ? *, int, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2298C
extern void sub_2298C(void); /* 0x1298C; target evidence: void far sub_2298C(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_274B0
extern char sub_274B0(short, short, short, short); /* 0x174B0; target evidence: char far sub_274B0(int, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_275C6
extern void sub_275C6(void); /* 0x175C6; target evidence: void far sub_275C6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_29772
extern void sub_29772(void); /* 0x19772; target evidence: void far sub_29772(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_29A86
extern int sub_29A86(short, int, short); /* 0x19A86; target evidence: long far sub_29A86(int, long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2C81C
extern char sub_2C81C(void); /* 0x1C81C; target evidence: char far sub_2C81C(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2C9B4
extern void sub_2C9B4(void); /* 0x1C9B4; target evidence: void far sub_2C9B4(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2CE4A
extern void sub_2CE4A(void); /* 0x1CE4A; target evidence: void near sub_2CE4A(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2CE77
extern short sub_2CE77(void); /* 0x1CE77; target evidence: int near sub_2CE77(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2D1BC
extern short sub_2D1BC(void); /* 0x1D1BC; target evidence: int near sub_2D1BC(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2E290
extern void sub_2E290(short, short, short, short, short, short); /* 0x1E290; target evidence: void near sub_2E290(int  , int  , int  , int  , int  , int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2EAD4
extern int sub_2EAD4(void); /* 0x1EAD4; target evidence: long far sub_2EAD4(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2EB07
extern short sub_2EB07(void); /* 0x1EB07; target evidence: int far sub_2EB07(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_2EB1E
extern void sub_2EB1E(short, short); /* 0x1EB1E; target evidence: void far sub_2EB1E(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_303BA
extern void sub_303BA(void); /* 0x203BA; target evidence: void near sub_303BA(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_307B4
extern void sub_307B4(void); /* 0x207B4; target evidence: void far sub_307B4(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_307D2
extern short sub_307D2(short); /* 0x207D2; target evidence: int far sub_307D2(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_307E3
extern char sub_307E3(void); /* 0x207E3; target evidence: char far sub_307E3(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_34526
extern void sub_34526(int); /* 0x24526; target evidence: void far sub_34526(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_345BC
extern void sub_345BC(char *, short, short); /* 0x245BC; target evidence: void far sub_345BC(char *, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_35B76
extern short sub_35B76(short, short, short, short, char); /* 0x25B76; target evidence: int far sub_35B76(int, int, int, int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_35C4E
extern void sub_35C4E(short, int, short, short); /* 0x25C4E; target evidence: void far sub_35C4E(int, long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_35DC8
extern void sub_35DC8(int); /* 0x25DC8; target evidence: void far sub_35DC8(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_35DE6
extern void sub_35DE6(void *, short, char *, short); /* 0x25DE6; target evidence: void far sub_35DE6(struct ? *, int, char *, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_35E08
extern void sub_35E08(unsigned short, int, short, short); /* 0x25E08; target evidence: void far sub_35E08(unsigned int, long, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3702E
extern void sub_3702E(int, short, short, short); /* 0x2702E; target evidence: void far sub_3702E(long, int, int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_372F4
extern void sub_372F4(void); /* 0x272F4; target evidence: void far sub_372F4(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3736A
extern void sub_3736A(void); /* 0x2736A; target evidence: void far sub_3736A(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_37470
extern short sub_37470(char *, char); /* 0x27470; target evidence: int far sub_37470(char *, char); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_374DE
extern void sub_374DE(char *); /* 0x274DE; target evidence: void far sub_374DE(char *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3771E
extern short sub_3771E(short); /* 0x2771E; target evidence: int far sub_3771E(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_37868
extern short sub_37868(short); /* 0x27868; target evidence: int far sub_37868(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_37C38
extern void sub_37C38(short); /* 0x27C38; target evidence: void far sub_37C38(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3803C
extern void sub_3803C(void *, int); /* 0x2803C; target evidence: void far sub_3803C(struct ? far *, long); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38156
extern short sub_38156(short); /* 0x28156; target evidence: int far sub_38156(int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38178
extern void sub_38178(void); /* 0x28178; target evidence: void far sub_38178(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3868A
extern void sub_3868A(void); /* 0x2868A; target evidence: void far sub_3868A(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_386D6
extern short sub_386D6(void); /* 0x286D6; target evidence: int far sub_386D6(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38702
extern void sub_38702(char *); /* 0x28702; target evidence: void far sub_38702(char *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38AC4
extern int sub_38AC4(short, void *); /* 0x28AC4; target evidence: long far sub_38AC4(int, struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38AEA
extern void sub_38AEA(short, unsigned char, short); /* 0x28AEA; target evidence: void far sub_38AEA(int, unsigned char, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38BEA
extern void sub_38BEA(short, short); /* 0x28BEA; target evidence: void far sub_38BEA(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38CF8
extern void sub_38CF8(short, void *); /* 0x28CF8; target evidence: void far sub_38CF8(int, struct ? far *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_38DE6
extern short sub_38DE6(int); /* 0x28DE6; target evidence: int far sub_38DE6(long); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_39050
extern short sub_39050(short, short); /* 0x29050; target evidence: int far sub_39050(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_39088
extern void sub_39088(short, short); /* 0x29088; target evidence: void far sub_39088(int, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3945A
extern void sub_3945A(int, short); /* 0x2945A; target evidence: void far sub_3945A(long, int); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3963C
extern void sub_3963C(void); /* 0x2963C; target evidence: void far sub_3963C(void); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_3968A
extern void sub_3968A(void *); /* 0x2968A; target evidence: void far sub_3968A(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_sub_39700
extern short sub_39700(void); /* 0x29700; target evidence: int far sub_39700(void); */
#endif
#ifndef STUNTS_LOCAL_FN_subst_hillroad
extern uint8_t subst_hillroad(uint8_t, uint8_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_subst_hillroad_track
extern char subst_hillroad_track(unsigned char, unsigned char); /* 0x11A5A; target evidence: char far subst_hillroad_track(unsigned char, unsigned char); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_compare_dx
extern short timer_compare_dx(void); /* 0x227EB; target evidence: int far timer_compare_dx(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_copy_counter
extern void far timer_copy_counter(U32 ticks); /* both stack words form one 32-bit tick delta; see PORT note for split-word legacy callers. */
#endif
#ifndef STUNTS_LOCAL_FN_timer_custom_delta
extern void timer_custom_delta(int); /* 0x22782; target evidence: void far timer_custom_delta(long); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_get_counter
extern int timer_get_counter(void); /* 0x22778; target evidence: long far timer_get_counter(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_get_counter_unk
extern void far timer_get_counter_unk(U32 ticks);
#endif
#ifndef STUNTS_LOCAL_FN_timer_get_delta
extern short timer_get_delta(void); /* 0x2279A; target evidence: int far timer_get_delta(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_get_delta_alt
extern int16_t timer_get_delta_alt(void); /* 0x1A230; target evidence: int far timer_get_delta_alt(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_intr_callback
extern void timer_intr_callback(void); /* 0x20329; target evidence: void far timer_intr_callback(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_reg_callback
extern I16 far timer_reg_callback(void (far *callback)(void));
#endif
#ifndef STUNTS_LOCAL_FN_timer_remove_callback
extern I16 far timer_remove_callback(void (far *callback)(void));
#endif
#ifndef STUNTS_LOCAL_FN_timer_reset
extern void timer_reset(void); /* 0x227B7; target evidence: void far timer_reset(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_setup_interrupt
extern void timer_setup_interrupt(void); /* 0x201A0; target evidence: void far timer_setup_interrupt(void); */
#endif
#ifndef STUNTS_LOCAL_FN_timer_wait_for_dx
extern void timer_wait_for_dx(void); /* 0x227D7; target evidence: void far timer_wait_for_dx(void); */
#endif
#ifndef STUNTS_LOCAL_FN_toupper
extern int toupper(int); /* 0x270BA; target evidence: int far toupper(int); */
#endif
#ifndef STUNTS_LOCAL_FN_track_edge_points
extern char track_edge_points(int16_t, struct TRACKRESULT *, char, char *); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_track_setup
extern I16 track_setup(void); /* SOURCE: src/obj_seg004.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_trans_op
extern unsigned trans_op(struct TRANSFORMEDSHAPE3D* ts); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_transformed_shape_add_for_sort
extern void transformed_shape_add_for_sort(short, char); /* 0x0CB80; target evidence: void far transformed_shape_add_for_sort(int, char); */
#endif
#ifndef STUNTS_LOCAL_FN_transformed_shape_op
extern char transformed_shape_op(void *); /* 0x14E9E; target evidence: char far transformed_shape_op(struct ? *); */
#endif
#ifndef STUNTS_LOCAL_FN_unknown_libname_1
extern void unknown_libname_1(short); /* 0x1E066; target evidence: void far unknown_libname_1(int); */
#endif
#ifndef STUNTS_LOCAL_FN_unknown_libname_2
extern short unknown_libname_2(short); /* 0x1E078; target evidence: int far unknown_libname_2(int); */
#endif
#ifndef STUNTS_LOCAL_FN_unknown_libname_3
extern void unknown_libname_3(void *, int); /* 0x1E918; target evidence: void far unknown_libname_3(struct ? *, long); */
#endif
#ifndef STUNTS_LOCAL_FN_unknown_libname_4
extern void unknown_libname_4(void *, char); /* 0x1E960; target evidence: void far unknown_libname_4(struct ? *, char); */
#endif
#ifndef STUNTS_LOCAL_FN_unknown_libname_5
extern void unknown_libname_5(void *, int); /* 0x1E982; target evidence: void far unknown_libname_5(struct ? *, long); */
#endif
#ifndef STUNTS_LOCAL_FN_unload_resource
extern void far unload_resource(void far* resptr); /* SOURCE: src/obj_seg008.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_unload_skybox
extern void unload_skybox(void); /* 0x0D8B4; target evidence: void far unload_skybox(void); */
#endif
#ifndef STUNTS_LOCAL_FN_upd_statef20_from_steer_input
extern void upd_statef20_from_steer_input(char); /* 0x087B2; target evidence: void far upd_statef20_from_steer_input(char); */
#endif
#ifndef STUNTS_LOCAL_FN_update_audio_voice_state
extern void update_audio_voice_state(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_update_camera_target
extern void update_camera_target(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_update_car_speed
extern void update_car_speed(I8 inputByte, I8 isOpponent, struct CARSTATE* carState, struct SIMD* carSetup); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_update_crash_debris
extern void update_crash_debris(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_update_crash_state
extern void update_crash_state(I16 crashMode, I16 isOpponent); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_update_frame
extern void update_frame(I8 page, struct RECTANGLE* clip); /* SOURCE: src/obj_seg003.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_update_gamestate
extern void update_gamestate(void); /* 0x07008; target evidence: void far update_gamestate(void); */
#endif
#ifndef STUNTS_LOCAL_FN_update_grip
extern void update_grip(struct CARSTATE *car, struct SIMD *simd, I16 isOpponent); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_update_player_state
extern void update_player_state(struct CARSTATE* activeCarState, struct SIMD* activeCarSetup, struct CARSTATE* otherCarState, struct SIMD* otherCarSetup, I8 isOpponent); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_update_rpm_from_speed
extern U16S far update_rpm_from_speed(U16S current_rpm, U16S speed, U16S gear_ratio, I16 changing_gear, U16S idle_rpm);
#endif
#ifndef STUNTS_LOCAL_FN_validate_track_elements
extern char validate_track_elements(void); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_vec_normalInnerProduct
extern I16 vec_normalInnerProduct(I16 x, I16 y, I16 z, struct VECTOR far *normal); /* SOURCE: src/obj_seg001_complete.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_vector_op_unk
extern void far vector_op_unk(struct VECTOR *left, struct VECTOR *right, struct VECTOR *output, I16 scale);
#endif
#ifndef STUNTS_LOCAL_FN_vector_op_unk2
extern vector_op_unk2(struct VECTOR* vec); /* SOURCE: src/obj_seg006.c; machine ABI evidence: declaration-evidence.json */
#endif
#ifndef STUNTS_LOCAL_FN_vector_to_point
extern void far vector_to_point(struct VECTOR *vector, struct POINT2D *output);
#endif
#ifndef STUNTS_LOCAL_FN_video_add_exithandler
extern void video_add_exithandler(void); /* 0x225AE; target evidence: void far video_add_exithandler(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_clear_color
extern short video_clear_color(short, short); /* 0x232A8; target evidence: int far video_clear_color(int, int  ); */
#endif
#ifndef STUNTS_LOCAL_FN_video_get_status
extern short video_get_status(void); /* 0x22FFC; target evidence: int far video_get_status(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_on_exit
extern void video_on_exit(void); /* 0x225D6; target evidence: void far video_on_exit(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_set_mode4
extern void video_set_mode4(void); /* 0x2005E; target evidence: void far video_set_mode4(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_set_mode7
extern short video_set_mode7(void); /* 0x20120; target evidence: int far video_set_mode7(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_set_mode_13h
extern void video_set_mode_13h(void); /* 0x23816; target evidence: void far video_set_mode_13h(void); */
#endif
#ifndef STUNTS_LOCAL_FN_video_set_palette
extern void far video_set_palette(U16S first, U16S count, U8 *colors);
#endif
#ifndef STUNTS_LOCAL_FN_wait_for_input_delay
extern int16_t wait_for_input_delay(int16_t); /* source declaration */
#endif
#ifndef STUNTS_LOCAL_FN_wheel_update
extern void wheel_update(struct VECTOR far *out, I16 angle,
              I16S *base, I16S *last_angle_and_y,
              struct VECTOR *source, I16S *origin); /* SOURCE: src/obj_seg004.c; machine ABI evidence: declaration-evidence.json */
#endif

#endif /* STUNTS_DECLS_H */
