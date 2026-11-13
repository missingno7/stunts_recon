#ifndef STUNTS_TRACK_H
#define STUNTS_TRACK_H

/* Track records, geometry and state. */
/* Shared trackrec declarations; DOS medium-model types. */
struct SHAPE3D;
struct TRACKRESULT;
struct TRKOBJINFO_LINK_BYTES;
struct TrackNode;
struct VECTOR;
struct WALLREC;
struct scene_shape;
struct track_object;
struct track_object_info;
#include "math3d.h"
#pragma pack()
struct scene_shape {
    unsigned short opaque_first_word;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay;
    unsigned char surface;
    unsigned char ignore_z_bias;
    unsigned char multi_tile;
    unsigned char physical_model;
    unsigned char unknown;
};
#pragma pack()
struct track_object {
    struct track_object_info *info;
    short rotation_y;
    struct SHAPE3D *shape;
    struct SHAPE3D *low_detail_shape;
    unsigned char overlay;
    unsigned char surface;
    unsigned char ignore_z_bias;
    unsigned char multi_tile;
    unsigned char physical_model;
    unsigned char unknown;
};
#pragma pack()
struct track_object_info {
    unsigned char block_count;
    unsigned char entry_point;
    unsigned char exit_point;
    unsigned char entry_type;
    unsigned char exit_type;
    unsigned char arrow_type;
    short arrow_orientation;
    unsigned char *camera_data;
    unsigned char opponent1;
    unsigned char opponent2;
    unsigned char opponent3;
    unsigned char opponent_speed_code;
};
#pragma pack()
struct TrackNode {
    unsigned char column;
    unsigned char row;
    unsigned char element;
    unsigned char block;
    unsigned char connection;
    unsigned char terrain;
    unsigned char previousColumn;
    unsigned char previousRow;
    unsigned char previousElement;
    unsigned char previousBlock;
    unsigned char previousReversed;
    unsigned char previousExitType;
    short parent;
};
#pragma pack()
struct TRACKRESULT {
    struct VECTOR center;
    struct VECTOR edge_a;
    struct VECTOR edge_b;
    short has_opponent_link;
};
#pragma pack()
struct TRKOBJINFO_LINK_BYTES {
    char first;
    char second;
};
#pragma pack()
struct WALLREC {
    short orientation;
    short x;
    short z;
};
/* Shared track declarations; unnamed prototype parameters keep C1 allocation stable. */
struct SHAPE3D;
struct VECTOR;
struct track_object_info;
extern char a_trk_5[];
extern char aBarn[116][5];
extern char aGame1[6];
extern char aGame2[6];
extern struct VECTOR ancv2;
extern char aStxxx[6];
extern short bkRdEntr_triang_zAdjust[4];
extern unsigned char BSTR_LIST[42];
extern short buf_obase[5];
extern short corkLR_negZBound[12];
extern short corkLR_posZBound[12];
extern struct VECTOR ctrmesh;
extern unsigned char FLBSTR_LIST[42];
extern unsigned char fence_codes[8];
extern char fence_off_1[2];
extern char fence_off_2[4];
extern char fence_off_3[4];
extern char fence_off_4[8];
extern char g_hillf;
extern short hgthgt;
extern short element_min_wall;
extern unsigned char FSTR_LIST[42];
extern unsigned char g_corkscrew_type_flag;
extern short gap_trackrow_1;
extern unsigned char g_cur_track_row;
extern int fence_offsets[8];
extern char g_road_piece_id;
extern struct SHAPE3D g_shapes3d[130];
extern short far *g_td01_track_filecpy;
extern int g_trackpiecescounter;
extern void far *game2res_pointer;
extern int gterrtrk[30];
extern struct SHAPE3D *fence_shapes[8];
extern short highEntrXInnBounds0[7];
extern short highEntrXOutBounds0[7];
extern short highEntrZBounds0[7];
extern int hill_offs[9][2];
extern int hill_offs_b[];
extern int hill_offs_c[];
extern int hill_offs_d[];
extern unsigned char HSTR_LIST[42];
extern char idxtrk;
extern int last_track_rotation;
extern short loopBae_InnXBounds0[7];
extern short loopBase_OutXBounds0[7];
extern short loopBase_ZBounds0[7];
extern short loopSurface_XBounds0[14];
extern short loopSurface_ZBounds0[7];
extern struct VECTOR opponent_pointc[6];
extern unsigned char PIPE_LIST[42];
extern char far *pl_carres_3d;
extern short pl_i;
extern struct VECTOR pos_pt;
extern int postable[30];
extern void far *ptr_model_active;
extern struct VECTOR pts_set[6];
extern int r_zp[30];
extern short road_elem_ctrz;
extern short road_num;
extern unsigned char sampled_trk_column;
extern struct VECTOR secondoveh[6];
extern struct VECTOR secondveccar[6];
extern unsigned char shapedata42_10[1236];
extern unsigned char shapedata42_2[72];
extern unsigned char shapedata42_4[702];
extern unsigned char shapedata42_6[672];
extern unsigned char shapedata42_7[510];
extern struct track_object_info shapeinfos[120];
extern unsigned char SPIPE_LIST[42];
#include "skybox.h"
extern char tagtrk;
extern unsigned char far *td19hdl;
extern short trackmenu_buttons_x1[3];
extern char track_file[82];
extern struct VECTOR track_prev_vec;
extern int trackmenu2_buttons_y2[5];
extern short far *trackdata_penalty_related;
extern int trackmenu2_buttons_x1[5];
extern short wall_wallelement;
extern unsigned char far *trkd23adr;
extern int trackctrpos2[30];
extern short track3_gap[2];
extern short trackmenu_buttons_x2[3];
extern short trackmenu_buttons_y1[3];
extern short trackmenu_buttons_y2[3];
extern char trk_sample_count;
extern int trackmenu2_buttons_y1[5];
extern struct VECTOR veccar[6];
extern struct VECTOR veco[6];
extern struct VECTOR veh_od[6];
extern int trackmenu2_buttons_x2[5];
extern short wallanchor_x;
extern int wallanchor_z;
extern short x_course_part;
extern int xcols[30];
extern short ywhlang[5];
extern int z_ctr_pos[30];
extern char far checktracksyntax(void);
extern void far drawblimpworld(void);
extern void far getpinfo(struct VECTOR *, struct VECTOR *);
extern int far getwpinfo(int, int, struct VECTOR *);
extern void far trackedit(void);
extern void far move_on_plane(void);
extern void far opponentcompile(void);
extern int far plane_distance(int, int, int, int);
extern void far removescenery(void);
extern char far setup_track(void);
extern void far trackoverlapcheck(void);

/* Track objects and data. */
struct TRACKRESULT;
struct VECTOR;
struct WALLREC;
struct scene_shape;
extern char far* g_column_of_trkdata21_pth;
extern int far* g_td08d;
extern unsigned char  far *road_trk;
extern struct scene_shape scene2[19];
extern struct scene_shape scene3[13];
extern unsigned char  far* td14tb;
extern short far *td3;
extern unsigned short  far *td6_ptr_b;
extern char far* td_18_ref;
extern char far* tdfrompathrow22;
extern char TerName[5][9];
extern unsigned short far *trackdat7;
extern struct VECTOR far* trkptrpath;
extern struct WALLREC far *wallrecrecord;
extern int compiletrack(void);
extern char gettlistpoint(int, struct TRACKRESULT *, char, char *);
extern unsigned char  roadupcheck(unsigned char, unsigned char);
#endif
