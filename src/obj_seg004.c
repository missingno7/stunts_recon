#define FAR far
#define NEAR near
#define HUGE huge
#define FAST_TRIG_ANGLE_MASK 0x3FF
#include "stunts_types.h"
/* Scratch whole-object candidate in target member order. */ /* PORT: plain char signedness follows the pinned MSC target. */
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct VECTOR { I16S x; I16S y; I16S z; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct SHAPE3D { U16S  numverts; struct VECTOR far *shape3d_verts; U16S  numprimitives; U8  numpaints; U8  reserved; void far *primitives; void far *cull1; void far *cull2; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct TRKOBJINFO { U8  noOfBlocks,entry,exitPoint,entryType,exitType,arrowType; I16S arrowOrient; I16S *cameraDataOffset; union { struct { U8  opponent1,opponent2; } opponent; U16S  cameraOffsetOverride; } cameraOverlay; U8  opponent3,opponentSpeedCode; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct TRACKOBJECT { struct TRKOBJINFO *info; I16S rotation; struct SHAPE3D *shape,*lowShape; U8  overlay; I8 surface,ignoreZ,multiTile,physicalModel,unknown; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct TrackNode { U8  column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; I16S parent; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct GAMESTATE { U8  before_game_inputmode[0x3f5]; I8 game_inputmode; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct WALLREC { I16S orientation,x,z; };
/* PORT: aggregate field offsets rely on the pinned MSC default /Zp packing. */
struct GAMEINFO { I8 game_playercarid[4],game_playermaterial,game_playertransmission,game_opponenttype,game_opponentcarid[4],game_opponentmaterial,game_opponenttransmission,game_trackname[9]; U16S  game_framespersec,game_recordedframes; };
extern struct GAMESTATE core;
extern I16 lnoffsets[30];
extern I16 gterrtrk[30];
int postable[30];
int r_zp[30];
extern I16 row_ctr_zs[30];
int xcols[30];
extern I16 trackctrpos2[30];
int z_ctr_pos[30];
extern U8  far *td14tb;
extern U8  far *td15p_9;
extern struct TRACKOBJECT trklst[215];
extern I16 far sinfast(I16),far cosfast(I16),far mulscl(I16,I16);
extern I16 far polradius2d(I16,I16),far polang(I16,I16);
short pl_i;
short road_num;
short wall_wallelement;
short element_min_wall;
unsigned char g_corkscrew_type_flag;
unsigned char g_cursurfacekindvalue;
unsigned char test_pln;
short hgthgt;
short x_course_part;
short road_elem_ctrz;
extern I16S hillconsts[];
struct WALLREC far *wallrecrecord;
extern U8  far *g_planlist;
extern U8  far *plncurrptr;
extern I16S wall_facingang;
short wallanchor_x;
extern I16S wallanchor_z;
extern I16S highEntrZBounds0[],highEntrXInnBounds0[],highEntrXOutBounds0[];
extern I16S loopSurface_ZBounds0[],loopSurface_XBounds0[];
extern I16S loopBase_ZBounds0[],loopBae_InnXBounds0[],loopBase_OutXBounds0[];
extern I16S bkRdEntr_triang_zAdjust[],corkLR_negZBound[],corkLR_posZBound[];
void far *g_game13dresource;
void far *game2res_pointer;
void far *ptr_model_active;
extern I32 far mmgr_get_res_ofs_diff_scaled(void),far mmgr_get_chunk_size_bytes(I8 far *);
extern I8 far *file_load_3dres(I8 *name);
extern I8 far *locate_shape_fatal();
extern I8 far *locate_shape_nofatal();
extern void shape3d_init_shape(I8 far *,struct SHAPE3D *);
struct SHAPE3D g_shapes3d[130];
extern I8 aBarn[116][5];
extern I8 aGame1[],aGame2[];
extern I8 aCar0[][5];
extern I8 aStxxx[];
char far *pl_carres_3d;
char far *carcopyresourceptr;
struct VECTOR pos_pt;
struct VECTOR ancv2;
struct VECTOR pts_set[6];
struct VECTOR secondveccar[6];
struct VECTOR veccar[6];
struct VECTOR car_dvecs[6];
struct VECTOR ctrmesh;
struct VECTOR g_op_carvector2;
struct VECTOR veco[6];
struct VECTOR secondoveh[6];
struct VECTOR opponent_pointc[6];
struct VECTOR veh_od[6];
short ywhlang[5];
short buf_obase[5];
extern I16S car_wheel_offsets[4];
extern struct VECTOR shape_template_points_a[],shape_template_points_b[],shape_template_points_c[],shape_template_points_d[],shape_template_points_e[],shape_template_points_f[];
char g_road_piece_id;
char trk_sample_count;
extern I8 far *mmgr_alloc_resbytes(I8 *,I32);
extern void far mmgr_free(void far *),far mmgr_release(void far *);
extern I8 far *word_338A8;
extern I8 far *word_33892;
extern I16 bto_auxiliary1(I16,I16,struct VECTOR *);
extern void wheel_update(struct VECTOR far *,I16,I16S *,I16S *,struct VECTOR *,I16S *);
extern U8  subst_hillroad(U8 ,U8 );
short far *g_td01_track_filecpy;
extern I16S far *trackdata_penalty_related;
unsigned char far *td19hdl;
extern U8  far *road_trk;
extern U8  far *td_18_ref;
extern U8  far *trkd23adr;
extern U8  far *g_column_of_trkdata21_pth;
extern U8  far *tdfrompathrow22;
int g_trackpiecescounter;
extern U16S  st_hdg;
extern I8 idxtrk,tagtrk,g_hillf;
extern U8  sampled_trk_column;
short gap_trackrow_1;
unsigned char g_cur_track_row;
unsigned short far *g_td08d;
extern U16S  far *td6_ptr_b;
unsigned short far *trackdat7;
extern struct VECTOR far *td10checkptr;
struct VECTOR far *trkptrpath;
extern struct GAMEINFO globalgamesettings;
extern U8  opponent_spd_tbl[16];
extern I16S far *td3;
extern I8 aOpp1[],opptext_label[];
extern void far *file_load_resource_file(const I8 *);
extern I8 far *locate_text_resource(void far *,const I8 *);
extern I8 far *locate_shape_alt(void far *,const I8 *);
extern void copy_string(I8 *,I8 far *);
extern void far unload_resource(void far *);

/* Adds track objects and wall anchors for adjacent positions. Params: current and next track positions. Returns: none. State: reads track tables and updates object lists and anchors. */
void build_obj(struct VECTOR *position, struct VECTOR *nextPosition)
{
	struct TRACKOBJECT *o;
	I16S wallOrientationState;
	I16S angAngle;
	U8  cellTerr;
	struct VECTOR absLocalCrds;
	struct VECTOR locElemCrds;
	I16S physModel;
	I16S side;
	I8S  trkR;
	I16S localX;
	I16S baseIndex;
	I8S  trackCol;
	struct VECTOR nextPositionElemCrds;
	U8  surfType;
	I16S elementAngle;
	I16S wallIx;
	I16S wallHi;
	register I16S step;
	I16S turnRadius;
	U8  tileElem;
	struct VECTOR effElemCrds;

	pl_i = 0;
	road_num = -1;
	wall_wallelement = -12;
	element_min_wall = -1000;
	g_corkscrew_type_flag = 0;
	g_cursurfacekindvalue = 4;
	test_pln = 1;
	step = 0;
	wallOrientationState = step;
	elementAngle = step;
	hgthgt = step;
	trackCol = (I8S )(position->x >> 10);
	trkR = (I8S )(position->z >> 10);
	physModel = -1;
	if (trackCol < 0 || trackCol > 29 || trkR < 0 || trkR > 29)
		goto select_plan;

	x_course_part = trackctrpos2[trackCol];
	road_elem_ctrz = z_ctr_pos[trkR];
	cellTerr = td15p_9[lnoffsets[trkR] + trackCol];
	if (cellTerr != 0) {
		switch (cellTerr) {
		case 6:
			hgthgt = hillconsts[1];
			break;
		case 2:
			step = 0x80;
			goto slope;
		case 3:
			step = -0x280;
			goto slope;
		case 4:
			step = -0x180;
			goto slope;
		case 5:
			step = -0x80;
		slope:
			locElemCrds.x = (I16S)(position->x - x_course_part);
			locElemCrds.z = (I16S)(position->z - road_elem_ctrz);
			side = mulscl(cosfast(step), locElemCrds.x) +
				mulscl(sinfast(step), locElemCrds.z);
			if (side < 0)
				g_cursurfacekindvalue = 5;
			break;
		case 1:
			g_cursurfacekindvalue = 5;
			break;
		}
	}

	tileElem = td14tb[gterrtrk[trkR] + trackCol];
	if (tileElem == 0)
		goto finish;
	if (tileElem >= 0xfd) {
		switch (tileElem) {
		case 0xfd:
			tileElem = td14tb[gterrtrk[trkR + 1] + trackCol - 1];
			if (trklst[tileElem].multiTile & 1)
				road_elem_ctrz = postable[trkR + 1];
			if (trklst[tileElem].multiTile & 2)
				x_course_part = xcols[trackCol];
			break;
		case 0xfe:
			tileElem = td14tb[gterrtrk[trkR + 1] + trackCol];
			if (trklst[tileElem].multiTile & 1)
				road_elem_ctrz = postable[trkR + 1];
			if (trklst[tileElem].multiTile & 2)
				x_course_part = xcols[trackCol + 1];
			break;
		case 0xff:
			tileElem = td14tb[gterrtrk[trkR] + trackCol - 1];
			if (trklst[tileElem].multiTile & 1)
				road_elem_ctrz = postable[trkR];
			if (trklst[tileElem].multiTile & 2)
				x_course_part = xcols[trackCol];
			break;
		}
	} else if (trklst[tileElem].multiTile != 0) {
		if (trklst[tileElem].multiTile & 1)
			road_elem_ctrz = postable[trkR];
		if (trklst[tileElem].multiTile & 2)
			x_course_part = xcols[trackCol + 1];
	}

	locElemCrds.x = (I16S)(position->x - x_course_part);
	locElemCrds.z = (I16S)(position->z - road_elem_ctrz);
	nextPositionElemCrds.x = (I16S)(nextPosition->x - x_course_part);
	nextPositionElemCrds.z = (I16S)(nextPosition->z - road_elem_ctrz);
	if (tileElem != 0 && cellTerr >= 7 && cellTerr < 11)
		tileElem = subst_hillroad(cellTerr, tileElem);

	o = &trklst[tileElem];
	physModel = o->physicalModel;
	elementAngle = o->rotation;
	switch (elementAngle) {
	case 0:
		break;
	case 0x300:
		localX = locElemCrds.x;
		locElemCrds.x = locElemCrds.z;
		locElemCrds.z = (I16S)-localX;
		localX = nextPositionElemCrds.x;
		nextPositionElemCrds.x = nextPositionElemCrds.z;
		nextPositionElemCrds.z = (I16S)-localX;
		break;
	case 0x200:
		locElemCrds.z = (I16S)-locElemCrds.z;
		locElemCrds.x = (I16S)-locElemCrds.x;
		nextPositionElemCrds.z = (I16S)-nextPositionElemCrds.z;
		nextPositionElemCrds.x = (I16S)-nextPositionElemCrds.x;
		break;
	case 0x100:
		localX = locElemCrds.x;
		locElemCrds.x = (I16S)-locElemCrds.z;
		locElemCrds.z = localX;
		localX = nextPositionElemCrds.x;
		nextPositionElemCrds.x = (I16S)-nextPositionElemCrds.z;
		nextPositionElemCrds.z = localX;
		break;
	}
	angAngle = 0;
	surfType = (U8 )(o->surface + 1);
	if ((I8S )surfType < 1)
		surfType = 1;
	absLocalCrds.x = locElemCrds.x < 0 ? (I16S)-locElemCrds.x : locElemCrds.x;
	absLocalCrds.z = locElemCrds.z < 0 ? (I16S)-locElemCrds.z : locElemCrds.z;
	switch (physModel) {
	case 0:
		if (core.game_inputmode == 0 && locElemCrds.x > 0) {
			if (locElemCrds.z < -380) pl_i = 0x83;
			else if (locElemCrds.z < -300) pl_i = 0x84;
		}
	case 1:
		if (absLocalCrds.x < 0x78) g_cursurfacekindvalue = surfType;
		break;
	case 12:
		if (absLocalCrds.x < 0x78 || absLocalCrds.z < 0x78) g_cursurfacekindvalue = surfType;
		break;
	case 5:
		locElemCrds.x = -locElemCrds.x;
	case 4:
		g_cursurfacekindvalue = surfType;
		if (locElemCrds.x > 0) {
			locElemCrds.z = -locElemCrds.z;
			locElemCrds.x = -locElemCrds.x;
		}
	case 3:
		turnRadius = polradius2d(locElemCrds.x + 0x400, locElemCrds.z + 0x400);
		if (turnRadius > 0x588 && turnRadius < 0x678) g_cursurfacekindvalue = surfType;
		break;
	case 6:
		if (absLocalCrds.x < 0x78) {
			g_cursurfacekindvalue = surfType;
			break;
		}
	case 2:
		turnRadius = polradius2d(locElemCrds.x + 0x200, locElemCrds.z + 0x200);
		if (turnRadius > 0x188 && turnRadius < 0x278) g_cursurfacekindvalue = surfType;
		break;
	case 7:
		if (absLocalCrds.x < 0x78) g_cursurfacekindvalue = surfType;
		else {
			turnRadius = polradius2d(0x200 - locElemCrds.x, locElemCrds.z + 0x200);
			if (turnRadius > 0x188 && turnRadius < 0x278) g_cursurfacekindvalue = surfType;
		}
		break;
	case 8:
		if (locElemCrds.x >= 0x188 && locElemCrds.x <= 0x278) g_cursurfacekindvalue = surfType;
		else {
			turnRadius = polradius2d(locElemCrds.x + 0x400, locElemCrds.z + 0x400);
			if (turnRadius > 0x588 && turnRadius < 0x678) g_cursurfacekindvalue = surfType;
		}
		break;
	case 9:
		if (locElemCrds.x >= -0x278 && locElemCrds.x <= -0x188) g_cursurfacekindvalue = surfType;
		else {
			turnRadius = polradius2d(0x400 - locElemCrds.x, locElemCrds.z + 0x400);
			if (turnRadius > 0x588 && turnRadius < 0x678) g_cursurfacekindvalue = surfType;
		}
		break;
	case 10:
		baseIndex = locElemCrds.x < 0 ? -locElemCrds.x : locElemCrds.x;
		for (step = 0; highEntrZBounds0[step + 1] < locElemCrds.z; ++step) { }
		if (highEntrXInnBounds0[step + 1] == highEntrXInnBounds0[step])
			localX = highEntrXInnBounds0[step];
		else
			localX = (I16S)(highEntrXInnBounds0[step] +
				(I32)(highEntrXInnBounds0[step + 1] - highEntrXInnBounds0[step]) *
				(locElemCrds.z - highEntrZBounds0[step]) /
				(highEntrZBounds0[step + 1] - highEntrZBounds0[step]));
		if (highEntrXOutBounds0[step + 1] == highEntrXOutBounds0[step])
			side = highEntrXOutBounds0[step];
		else
			side = (I16S)(highEntrXOutBounds0[step] +
				(I32)(highEntrXOutBounds0[step + 1] - highEntrXOutBounds0[step]) *
				(locElemCrds.z - highEntrZBounds0[step]) /
				(highEntrZBounds0[step + 1] - highEntrZBounds0[step]));
		if (baseIndex > localX && baseIndex < side) {
			g_cursurfacekindvalue = surfType;
			break;
		}
		if (locElemCrds.z < 0 || baseIndex > 0x78) break;
		pl_i = 1;
		if (locElemCrds.z >= 0x14e) {
			if (nextPositionElemCrds.x <= -0x78) road_num = 0xbc;
			else if (nextPositionElemCrds.x >= 0x78) road_num = 0xba;
		} else if (nextPositionElemCrds.x >= 0) road_num = 0xbb;
		else road_num = 0xbd;
		break;
	case 11:
		if (absLocalCrds.x > 0x168) break;
		if (absLocalCrds.x > 0x78) g_cursurfacekindvalue = surfType;
		else {
			pl_i = 1;
			if (nextPositionElemCrds.x <= -0x78) road_num = 0xbc;
			else if (nextPositionElemCrds.x >= 0x78) road_num = 0xba;
		}
		break;
	case 16:
		if (locElemCrds.z > 0) test_pln = 0;
		else if (nextPositionElemCrds.z >= 0) road_num = 0x66;
		goto ramp_common;
	case 17:
		if (nextPositionElemCrds.z >= 0x1dc) road_num = 0x67;
	ramp_common:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) < 0x78) {
			pl_i = 3;
			g_cursurfacekindvalue = surfType;
			if (road_num != -1 || locElemCrds.z < 0 || absLocalCrds.x < 0x78) break;
			wall_wallelement = 0x2a;
			element_min_wall = -12;
			if (locElemCrds.x < 0) road_num = 0x64;
			else road_num = 0x65;
		} else {
			if (test_pln == 0 || absLocalCrds.x > 0x78) break;
			pl_i = 3;
			if (road_num != -1) break;
			wallOrientationState = 0x200;
			if (locElemCrds.x < 0) road_num = 0x64;
			else road_num = 0x65;
		}
		break;
	case 22:
		if (position->y - hgthgt > 0x186) {
			test_pln = 0;
			goto elevated_road;
		}
		if (absLocalCrds.z <= 0x78) g_cursurfacekindvalue = surfType;
		break;
	case 18: case 19:
		if (position->y - hgthgt > 0x186) {
			test_pln = 0;
			goto elevated_road;
		}
		break;
	case 20:
	elevated_road:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) <= 0x78) {
			pl_i = 2;
			g_cursurfacekindvalue = surfType;
			if (test_pln != 0) {
				if (nextPositionElemCrds.z >= 0x1dc) road_num = 0x67;
				else if (nextPositionElemCrds.z <= -0x1dc) road_num = 0x68;
			}
			if (absLocalCrds.x < 0x78) break;
			wall_wallelement = 0x2a;
			if (locElemCrds.x < 0) road_num = 0x64;
			else road_num = 0x65;
		} else {
			if (test_pln == 0 || absLocalCrds.x > 0x78) break;
			pl_i = 2;
			wall_wallelement = 0x2a;
			wallOrientationState = 0x200;
			if (nextPositionElemCrds.x < 0) road_num = 0x64;
			else road_num = 0x65;
		}
		break;
	case 21:
		if (position->y - hgthgt <= 0x186) goto finish;
		turnRadius = (I16S)(polradius2d((I16S)(locElemCrds.x + 0x400), (I16S)(locElemCrds.z + 0x400)) - 0x600);
		if (turnRadius <= -0x96 || turnRadius >= 0x96) goto finish;
		g_cursurfacekindvalue = surfType;
		pl_i = 2;
		test_pln = 0;
		if (turnRadius >= -0x6c && turnRadius <= 0x6c) goto finish;
		side = (polang(locElemCrds.x + 0x400, locElemCrds.z + 0x400) & 0xff) * 0x12;
		localX = (I16S)(0x11 - (side >> 8));
		wall_wallelement = 0x2a;
		element_min_wall = -12;
		if (turnRadius < 0) road_num = (I16S)(localX + 0x69);
		else road_num = (I16S)(localX + 0x7b);
		break;
	case 24:
		baseIndex = 0x23;
		localX = 0;
		step = -0x2a0;
		goto bank_entrance_common;
	case 23:
		baseIndex = 0x19;
		localX = 1;
		step = 0xa0;
	bank_entrance_common:
		if (absLocalCrds.x > 0x78) break;
		if (localX == 0 && nextPositionElemCrds.x <= -0x78) {
			wallOrientationState = 0x200;
			road_num = 0x64;
		} else if (localX != 0 && nextPositionElemCrds.x >= 0x78) {
			wallOrientationState = 0x200;
			road_num = 0x65;
		}
		g_cursurfacekindvalue = surfType;
		if (locElemCrds.z < -0x14e) pl_i = baseIndex;
		else if (locElemCrds.z >= 0x14e) pl_i = baseIndex + 9;
		else {
			if (locElemCrds.z < -0xa8) { pl_i = baseIndex + 1; localX = 0; }
			else if (locElemCrds.z < 0) { pl_i = baseIndex + 3; localX = 1; }
			else if (locElemCrds.z < 0xa8) { pl_i = baseIndex + 5; localX = 2; }
			else if (locElemCrds.z < 0x14e) { pl_i = baseIndex + 7; localX = 3; }
			side = mulscl(cosfast(step), locElemCrds.x) +
				mulscl(sinfast(step), locElemCrds.z - bkRdEntr_triang_zAdjust[localX]);
			if (side > 0) ++pl_i;
		}
		break;
	case 25:
		if (absLocalCrds.x > 0x78) goto finish;
		g_cursurfacekindvalue = surfType;
		pl_i = 6;
		if (nextPositionElemCrds.x < 0x78) break;
		wallOrientationState = 0x200;
		road_num = 0x65;
		break;
	case 26:
		turnRadius = (I16S)(polradius2d((I16S)(locElemCrds.x + 0x400), (I16S)(locElemCrds.z + 0x400)) - 0x600);
		if (turnRadius <= -0x78 || turnRadius >= 0x7e) goto finish;
		side = (polang(locElemCrds.x + 0x400, locElemCrds.z + 0x400) & 0xff) * 0x12;
		localX = (I16S)(0x11 - (side >> 8));
		pl_i = (I16S)(localX + 7);
		g_cursurfacekindvalue = surfType;
		if (turnRadius <= 0x66) goto finish;
		wallOrientationState = 0x200;
		road_num = (I16S)(localX + 0x7b);
		test_pln = 0;
		break;
	case 27:
		if (locElemCrds.z < 0) {
			baseIndex = 0x33;
			effElemCrds.x = (I16S)-locElemCrds.x;
			effElemCrds.z = (I16S)-locElemCrds.z;
		} else {
			baseIndex = 0x2d;
			effElemCrds.x = locElemCrds.x;
			effElemCrds.z = locElemCrds.z;
		}
		if (effElemCrds.z > loopSurface_ZBounds0[3] - 1) {
			if (effElemCrds.z > loopSurface_ZBounds0[3] + 0x64) goto loop_base;
			localX = loopSurface_ZBounds0[3] - 1;
		} else localX = effElemCrds.z;
		for (step = 0; loopSurface_ZBounds0[step + 1] < localX; ++step) { }
		if (position->y - hgthgt > 0x20c) {
			step = 5 - step;
			if (loopSurface_XBounds0[step] <= effElemCrds.x && loopSurface_XBounds0[step + 1] + 0x190 >= effElemCrds.x) {
				if (loopSurface_XBounds0[step + 1] >= effElemCrds.x || loopSurface_XBounds0[step] + 0x190 <= effElemCrds.x) {
					side = (I32)(loopSurface_XBounds0[step] - loopSurface_XBounds0[step + 1]) *
						(loopSurface_ZBounds0[step] - localX) /
						(loopSurface_ZBounds0[step + 1] - loopSurface_ZBounds0[step]);
					if (loopSurface_XBounds0[step] + side >= effElemCrds.x || loopSurface_XBounds0[step] + side + 0x190 <= effElemCrds.x) break;
				}
				pl_i = baseIndex + step;
				g_cursurfacekindvalue = surfType;
				test_pln = 0;
			}
			break;
		}
		if (step > 1 && position->y - hgthgt < 0x64) goto loop_base;
		if (loopSurface_XBounds0[step] <= effElemCrds.x && loopSurface_XBounds0[step + 1] + 0x190 >= effElemCrds.x) {
			if (loopSurface_XBounds0[step + 1] >= effElemCrds.x || loopSurface_XBounds0[step] + 0x190 <= effElemCrds.x) {
				if (loopSurface_XBounds0[step + 1] == loopSurface_XBounds0[step]) goto loop_base;
				side = (I32)(loopSurface_XBounds0[step] - loopSurface_XBounds0[step + 1]) *
					(loopSurface_ZBounds0[step] - localX) /
					(loopSurface_ZBounds0[step + 1] - loopSurface_ZBounds0[step]);
				if (loopSurface_XBounds0[step] + side >= effElemCrds.x || loopSurface_XBounds0[step] + side + 0x190 <= effElemCrds.x) goto loop_base;
			}
			pl_i = baseIndex + step;
			g_cursurfacekindvalue = surfType;
			test_pln = 0;
			break;
		}
loop_base:
		for (step = 0; effElemCrds.z > loopBase_ZBounds0[step + 1]; ++step) { }
		localX = (I16S)(loopBae_InnXBounds0[step] +
			(I32)(loopBae_InnXBounds0[step + 1] - loopBae_InnXBounds0[step]) *
			(effElemCrds.z - loopBase_ZBounds0[step]) /
			(loopBase_ZBounds0[step + 1] - loopBase_ZBounds0[step]));
		side = (I16S)(loopBase_OutXBounds0[step] +
			(I32)(loopBase_OutXBounds0[step + 1] - loopBase_OutXBounds0[step]) *
			(effElemCrds.z - loopBase_ZBounds0[step]) /
			(loopBase_ZBounds0[step + 1] - loopBase_ZBounds0[step]));
		if (effElemCrds.x >= localX && effElemCrds.x <= side) g_cursurfacekindvalue = surfType;
		break;
	case 28:
		if (position->y - hgthgt >= 0x90 ||
			nextPosition->y - hgthgt >= 0x90) {
			if (absLocalCrds.x >= 0x10e) goto finish;
			g_cursurfacekindvalue = surfType;
			pl_i = 0x85;
			break;
		}
		if (absLocalCrds.x < 0x78) g_cursurfacekindvalue = surfType;
		if (locElemCrds.x >= 0x78 && locElemCrds.x <= 0x10e) {
			wall_wallelement = 0x90;
			if (nextPositionElemCrds.z <= -0x200) road_num = 0x9a;
			else if (nextPositionElemCrds.z >= 0x200) road_num = 0x99;
			else if (nextPositionElemCrds.x <= 0x78) road_num = 0x98;
			else if (nextPositionElemCrds.x >= 0x10e) road_num = 0x96;
			break;
		}
		if (locElemCrds.x <= -0x78 && locElemCrds.x >= -0x10e) {
			wall_wallelement = 0x90;
			if (nextPositionElemCrds.z <= -0x200) road_num = 0x9a;
			else if (nextPositionElemCrds.z >= 0x200) road_num = 0x99;
			else if (nextPositionElemCrds.x >= -0x78) road_num = 0x97;
			else if (nextPositionElemCrds.x <= -0x10e) road_num = 0x95;
		}
		break;
	case 29:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) >= 0x73 &&
		    absLocalCrds.x <= 0xa4) {
			wall_wallelement = 0x97;
			if (nextPositionElemCrds.x > 0) road_num = 0x9f;
			else road_num = 0xa0;
			break;
		}
		if (absLocalCrds.x >= 0x73 || position->y - hgthgt >= 0xab) break;
		g_cursurfacekindvalue = surfType;
		if (absLocalCrds.x < 0x1f) {
			pl_i = 0x46;
			break;
		}
		if (locElemCrds.x < -0x54) { pl_i = 0x49; localX = -0x64; step = -5; }
		else if (locElemCrds.x < 0) { pl_i = 0x47; localX = -0x39; step = -8; }
		else if (locElemCrds.x > 0x54) { pl_i = 0x4d; localX = 0x64; step = 5; }
		else { pl_i = 0x4b; localX = 0x39; step = 8; }
		side = mulscl(cosfast(step), locElemCrds.x - localX) +
			mulscl(sinfast(step), locElemCrds.z);
		if (side < 0) ++pl_i;
		break;
	case 31:
		side = 1;
		goto pipe_body;
	case 30:
		side = 0;
	pipe_body:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) >= 0xa4 && absLocalCrds.x <= 0xa4) {
			wall_wallelement = 0x97;
			if (nextPositionElemCrds.x > 0) road_num = 0x9b;
			else road_num = 0x9c;
			break;
		}
		if (absLocalCrds.x >= 0xa4) break;
		if (position->y - hgthgt >= 0x109) break;
		if (absLocalCrds.x < 0x82) g_cursurfacekindvalue = surfType;
		if (position->y - hgthgt > 0x97) localX = 1;
		else localX = 0;
		if (side && !localX && absLocalCrds.x <= 0x54 && absLocalCrds.z <= 0x4b) {
			pl_i = 0x45;
			if (nextPositionElemCrds.z <= -0x4b) road_num = 0x9d;
			else if (nextPositionElemCrds.z >= 0x4b) road_num = 0x9e;
		} else if (position->y - hgthgt > 0x58 && !localX) {
			if (locElemCrds.x < 0) pl_i = 0x3c;
			else pl_i = 0x42;
		} else if (absLocalCrds.x < 0x1f) {
			if (localX) pl_i = 0x3f;
			else pl_i = 0x39;
		} else if (locElemCrds.x < -0x54) {
			if (localX) pl_i = 0x3d;
			else pl_i = 0x3b;
		} else if (locElemCrds.x < 0) {
			if (localX) pl_i = 0x3e;
			else pl_i = 0x3a;
		} else if (locElemCrds.x > 0x54) {
			if (localX) pl_i = 0x41;
			else pl_i = 0x43;
		} else {
			if (localX) pl_i = 0x40;
			else pl_i = 0x44;
		}
		break;
	case 35:
		if (absLocalCrds.x >= 0x96) break;
		if (position->y - hgthgt >= 0x109) break;
		g_cursurfacekindvalue = surfType;
		if (position->y - hgthgt > 0x97) localX = 1;
		else localX = 0;
		side = 0;
		if (position->y - hgthgt > 0x58 && !localX) {
			if (locElemCrds.x < 0) side = 3;
			else side = 9;
		} else if (absLocalCrds.x < 0x1f) {
			if (localX) side = 6;
		} else if (locElemCrds.x < -0x54) {
			if (localX) side = 4;
			else side = 2;
		} else if (locElemCrds.x < 0) {
			if (localX) side = 5;
			else side = 1;
		} else if (locElemCrds.x > 0x54) {
			if (localX) side = 8;
			else side = 0xa;
		} else {
			if (localX) side = 7;
			else side = 0xb;
		}
		if (side != 0 && corkLR_negZBound[side] < locElemCrds.z && corkLR_posZBound[side] > locElemCrds.z)
			pl_i = side + 0x39;
		if (pl_i == 0 && absLocalCrds.z < 0x200) {
			road_num = 0xb9;
			g_corkscrew_type_flag = 1;
			wall_wallelement = 0x75;
		}
		break;
	case 32:
		localX = (I16S)-locElemCrds.x;
		side = 0x4f;
		wallHi = 0x32;
		wallIx = 0x4b;
		goto cork_ud_common;
	case 33:
		localX = locElemCrds.x;
		side = 0x69;
		wallHi = 0;
		wallIx = 0x19;
	cork_ud_common:
		g_corkscrew_type_flag = 1;
		if (locElemCrds.z < 0 && position->y - hgthgt < 0x64 && localX > 0) {
			if (localX >= 0x278 || localX <= 0x188) break;
			g_cursurfacekindvalue = surfType;
			pl_i = side;
			break;
		}
		if (locElemCrds.z > 0 && position->y - hgthgt > 0x15e && localX < 0x2b4 && localX > 0x14c) {
			wall_wallelement = 0x2a;
			element_min_wall = -12;
			road_num = (localX > 0x200 ? wallHi : wallIx) + 0x18;
			g_cursurfacekindvalue = surfType;
			pl_i = side + 0x19;
			test_pln = 0;
			break;
		}
		turnRadius = polradius2d(localX, locElemCrds.z);
		if (turnRadius <= 0x14c || turnRadius >= 0x2b4) break;
		step = ((0x100 - polang(localX, locElemCrds.z)) & FAST_TRIG_ANGLE_MASK) * 0x18 >> 10;
		pl_i = side + step + 1;
		g_cursurfacekindvalue = surfType;
		test_pln = 0;
		wall_wallelement = 0x2a;
		element_min_wall = -12;
		if (turnRadius - 0x200 > 0x5a) road_num = wallHi + step;
		else if (turnRadius - 0x200 < -0x5a) road_num = wallIx + step;
		break;
	case 34:
		if (absLocalCrds.x < 0x78) g_cursurfacekindvalue = surfType;
		if (locElemCrds.x >= 0x17 && locElemCrds.x <= 0x61 && locElemCrds.z > -0x10f && locElemCrds.z < -0xf1) {
			wall_wallelement = 0x2a;
			if (nextPositionElemCrds.z < -0x10f) road_num = 0x91;
			else if (nextPositionElemCrds.z > -0xf1) road_num = 0x92;
			else if (nextPositionElemCrds.x < 0x17) road_num = 0x94;
			else if (nextPositionElemCrds.x > 0x61) road_num = 0x93;
		} else if (locElemCrds.x <= -0x17 && locElemCrds.x >= -0x61 && locElemCrds.z < 0x10f && locElemCrds.z > 0xf1) {
			wall_wallelement = 0x2a;
			if (nextPositionElemCrds.z > 0x10f) road_num = 0x8d;
			else if (nextPositionElemCrds.z < 0xf1) road_num = 0x8e;
			else if (nextPositionElemCrds.x > -0x17) road_num = 0x8f;
			else if (nextPositionElemCrds.x < -0x61) road_num = 0x90;
		}
		break;
	case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43:
	case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51:
	case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59:
	case 60: case 61: case 62: case 63: case 64:
		break;
	case 65:
		if (absLocalCrds.x > 0x96 || absLocalCrds.z > 0x96) goto finish;
		wall_wallelement = 0x1a9;
		if (nextPositionElemCrds.z <= -0x96) road_num = 0xa1;
		else if (nextPositionElemCrds.z >= 0x96) road_num = 0xa2;
		else if (nextPositionElemCrds.x >= 0x96) road_num = 0xa3;
		else if (nextPositionElemCrds.x <= -0x96) road_num = 0xa4;
		break;
	case 66:
		if (locElemCrds.x < -0xc8 || locElemCrds.x > 0x104 || absLocalCrds.z > 0x50) goto finish;
		wall_wallelement = 0xe6;
		if (nextPositionElemCrds.z <= -0x50) road_num = 0xa5;
		else if (nextPositionElemCrds.z >= 0x50) road_num = 0xa8;
		else if (nextPositionElemCrds.x <= -0xc8) road_num = 0xa6;
		else if (nextPositionElemCrds.x >= 0x104) road_num = 0xa7;
		break;
	case 67:
		if (absLocalCrds.x > 0xb4 || absLocalCrds.z > 0x64) goto finish;
		wall_wallelement = 0xf8;
		if (nextPositionElemCrds.z <= -0x64) road_num = 0xa9;
		else if (nextPositionElemCrds.z >= 0x64) road_num = 0xac;
		else if (nextPositionElemCrds.x <= -0xb4) road_num = 0xab;
		else if (nextPositionElemCrds.x >= 0xb4) road_num = 0xaa;
		break;
	case 68:
		if (absLocalCrds.x > 0xc8 || absLocalCrds.z > 0xc8) goto finish;
		wall_wallelement = 0x226;
		if (nextPositionElemCrds.z <= -0xc8) road_num = 0xad;
		else if (nextPositionElemCrds.z >= 0xc8) road_num = 0xae;
		else if (nextPositionElemCrds.x <= -0xc8) road_num = 0xaf;
		else if (nextPositionElemCrds.x >= 0xc8) road_num = 0xb0;
		break;
	case 69:
		if (absLocalCrds.x > 0x72 || absLocalCrds.z > 0x72) goto finish;
		wall_wallelement = 0x1ef;
		if (nextPositionElemCrds.z <= -0x72) road_num = 0xb4;
		else if (nextPositionElemCrds.z >= 0x72) road_num = 0xb2;
		else if (nextPositionElemCrds.x <= -0x72) road_num = 0xb1;
		else if (nextPositionElemCrds.x >= 0x72) road_num = 0xb3;
		break;
	case 70:
		if (locElemCrds.x < -0xaa || locElemCrds.x > 0x104 || absLocalCrds.z > 0x6e) goto finish;
		wall_wallelement = 0xe6;
		if (nextPositionElemCrds.z <= -0x6e) road_num = 0xb5;
		else if (nextPositionElemCrds.z >= 0x6e) road_num = 0xb8;
		else if (nextPositionElemCrds.x <= -0xaa) road_num = 0xb7;
		else if (nextPositionElemCrds.x >= 0x104) road_num = 0xb6;
		break;
	case 71: case 72: case 73: case 74:
		break;
	default:
		break;
	}

finish:
	if (cellTerr >= 7) {
		locElemCrds.x = (I16S)(position->x - trackctrpos2[trackCol]);
		locElemCrds.z = (I16S)(position->z - z_ctr_pos[trkR]);
		switch (cellTerr) {
		case 7: case 11: case 15:
			elementAngle = 0;
			break;
		case 8: case 12: case 16:
			elementAngle = 0x300;
			localX = locElemCrds.x;
			locElemCrds.x = locElemCrds.z;
			locElemCrds.z = (I16S)-localX;
			break;
		case 9: case 13: case 17:
			elementAngle = 0x200;
			locElemCrds.z = (I16S)-locElemCrds.z;
			locElemCrds.x = (I16S)-locElemCrds.x;
			break;
		case 10: case 14: case 18:
			elementAngle = 0x100;
			localX = locElemCrds.x;
			locElemCrds.x = (I16S)-locElemCrds.z;
			locElemCrds.z = localX;
			break;
		}
		switch (cellTerr) {
		case 7: case 8: case 9: case 10:
			if (pl_i == 0) pl_i = 3;
			break;
		case 11: case 12: case 13: case 14:
			side = mulscl(cosfast(0xff80), locElemCrds.x) +
				mulscl(sinfast(0xff80), locElemCrds.z);
			if (side < 0) pl_i = 4;
			break;
		case 15: case 16: case 17: case 18:
			side = mulscl(cosfast(0xff80), locElemCrds.x) +
				mulscl(sinfast(0xff80), locElemCrds.z);
			if (side > 0) pl_i = 5;
			else hgthgt = 0x1c2;
			break;
		}
	}
select_plan:
	if (pl_i > 0) {
		pl_i <<= 2;
		switch (elementAngle) {
		case 0x300: ++pl_i; break;
		case 0x200: pl_i += 2; break;
		case 0x100: pl_i += 3; break;
		}
	}
	plncurrptr = g_planlist + pl_i * 0x22;
	if (g_cursurfacekindvalue == 4)
		hgthgt += ((position->z ^ position->x) >> 8) & 1;
	else
		hgthgt += 2;

position_wall:
	if (road_num < 0) return;
	wall_facingang = (I16S)((-wallrecrecord[road_num].orientation + elementAngle + wallOrientationState) & FAST_TRIG_ANGLE_MASK);
	switch (elementAngle) {
	case 0:
		wallanchor_x = wallrecrecord[road_num].x;
		wallanchor_z = wallrecrecord[road_num].z;
		break;
	case 0x300:
		wallanchor_x = -wallrecrecord[road_num].z;
		wallanchor_z = wallrecrecord[road_num].x;
		break;
	case 0x200:
		wallanchor_x = -wallrecrecord[road_num].x;
		wallanchor_z = -wallrecrecord[road_num].z;
		break;
	case 0x100:
		wallanchor_x = wallrecrecord[road_num].z;
		wallanchor_z = -wallrecrecord[road_num].x;
		break;
	}
	wallanchor_x += x_course_part;
	wallanchor_z += road_elem_ctrz;
}

/* Builds auxiliary geometry for one track tile. Params: tile column, row and output vertices. Returns: generated vertex count. State: reads track templates and terrain data. */
I16 bto_auxiliary1(I16 column, I16 row, struct VECTOR *vertices) {
    U8  tileElement;
    I16 centerRow;
    I16 height;
    I16 rotationY;
    I16 centerX;
    struct VECTOR *templatePoints;
    register I16 point;
    register I16 numPoints;
    U8  terrainIndex;

    tileElement = td14tb[column + lnoffsets[row]];
    if (tileElement == 0) return 0;

    centerX = trackctrpos2[column];
    centerRow = row_ctr_zs[row];
    if (tileElement >= 0xfd) {
        switch (tileElement) {
        case 0xfd:
            tileElement = td14tb[column + lnoffsets[row - 1] - 1];
            if (trklst[tileElement].multiTile & 1) centerRow = r_zp[row + 1];
            if (trklst[tileElement].multiTile & 2) centerX = xcols[column];
            break;
        case 0xfe:
            tileElement = td14tb[column + lnoffsets[row - 1]];
            if (trklst[tileElement].multiTile & 1) centerRow = r_zp[row + 1];
            if (trklst[tileElement].multiTile & 2) centerX = xcols[column + 1];
            break;
        case 0xff:
            tileElement = td14tb[column + lnoffsets[row] - 1];
            if (trklst[tileElement].multiTile & 1) centerRow = r_zp[row];
            if (trklst[tileElement].multiTile & 2) centerX = xcols[column];
            break;
        }
    } else if (trklst[tileElement].multiTile != 0) {
        if (trklst[tileElement].multiTile & 1) centerRow = r_zp[row];
        if (trklst[tileElement].multiTile & 2) centerX = xcols[column + 1];
    }

    numPoints = 0;
    switch (trklst[tileElement].physicalModel) {
    case 0x23:
        numPoints = 2;
        templatePoints = shape_template_points_c;
        break;
    case 0x20:
        numPoints = 2;
        templatePoints = shape_template_points_d;
        break;
    case 0x21:
        numPoints = 2;
        templatePoints = shape_template_points_e;
        break;
    case 0x22:
        numPoints = 4;
        templatePoints = shape_template_points_f;
        break;
    case 0x0b:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
        numPoints = 1;
        templatePoints = shape_template_points_a;
        break;
    case 0x12:
        numPoints = 8;
        templatePoints = shape_template_points_b;
        break;
    }
    if (numPoints == 0) return 0;

    terrainIndex = td15p_9[column + gterrtrk[row]];
    if (terrainIndex == 6) height = hillconsts[1];
    else height = 0;
    rotationY = trklst[tileElement].rotation;

    for (point = 0; point < numPoints; point++) {
        switch (rotationY) {
        case 0:
            vertices[point].x = templatePoints[point].x + centerX;
            vertices[point].y = templatePoints[point].y + height;
            vertices[point].z = templatePoints[point].z + centerRow;
            break;
        case 0x300:
            vertices[point].x = -templatePoints[point].z + centerX;
            vertices[point].y = templatePoints[point].y + height;
            vertices[point].z = templatePoints[point].x + centerRow;
            break;
        case 0x200:
            vertices[point].x = -templatePoints[point].x + centerX;
            vertices[point].y = templatePoints[point].y + height;
            vertices[point].z = -templatePoints[point].z + centerRow;
            break;
        case 0x100:
            vertices[point].x = templatePoints[point].z + centerX;
            vertices[point].y = templatePoints[point].y + height;
            vertices[point].z = -templatePoints[point].x + centerRow;
            break;
        }
    }
    return numPoints;
}

/* Loads the shared 3D shape resources. Params: none. Returns: load status. State: updates shared shape-resource handles. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
I16 shape3d_load_all(void)
{
    register I16 i;

    g_game13dresource = 0;
    game2res_pointer = 0;
    /* PLATFORM(memory): query available resource memory. */ if (mmgr_get_res_ofs_diff_scaled() < 65000L) {
        return 1;
    }
    /* PLATFORM(file): load a 3D resource. */ g_game13dresource = file_load_3dres(aGame1);
    /* PLATFORM(file): load a 3D resource. */ game2res_pointer = file_load_3dres(aGame2);
    for (i = 0; i < 0x74; ++i) {
        ptr_model_active = locate_shape_nofatal(g_game13dresource, aBarn[i]);
        if (ptr_model_active == 0) {
            ptr_model_active = locate_shape_fatal(game2res_pointer, aBarn[i]);
        }
        shape3d_init_shape(ptr_model_active, &g_shapes3d[i]);
    }
    return 0;
}

/* Releases shared 3D shape resources. Params: none. Returns: none. State: reads and clears shared resource handles. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void shape3d_free_all(void)
{
    if (g_game13dresource != 0) {
        /* PLATFORM(memory): release resource memory. */ mmgr_free(g_game13dresource);
    }
    if (game2res_pointer != 0) {
        /* PLATFORM(memory): release resource memory. */ mmgr_free(game2res_pointer);
    }
}

/* Loads player and opponent car shapes. Params: player and opponent car IDs. Returns: none. State: updates car-shape resource pointers. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void shape3d_load_car_shapes(I8 arg_playercarid[], I8 arg_opponentcarid[]) {
	I32 copyOffset;
	I32 sizeBytes;
	register I16 loopIndex;
	struct VECTOR far *vertices;
	I8 firstOpponent;
	aStxxx[2] = arg_playercarid[0];
	aStxxx[3] = arg_playercarid[1];
	aStxxx[4] = arg_playercarid[2];
	aStxxx[5] = arg_playercarid[3];
	/* PLATFORM(file): load a 3D resource. */ pl_carres_3d = file_load_3dres(aStxxx);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[0]), &g_shapes3d[124]);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[1]), &g_shapes3d[126]);

	vertices = &(g_shapes3d[126].shape3d_verts[8]);
	pos_pt.z = vertices[0].z;
	pos_pt.x = (vertices[3].x + vertices[0].x)>> 1;
	ancv2.z = vertices[6].z;
	ancv2.x = (vertices[6].x + vertices[9].x) >> 1;
	
	for (loopIndex = 0; loopIndex < 6; loopIndex++) {
		pts_set[loopIndex].x = pos_pt.x - vertices[loopIndex + 0].x;
		pts_set[loopIndex].z = pos_pt.z - vertices[loopIndex + 0].z;
		pts_set[loopIndex].y = vertices[loopIndex + 0].y;
		secondveccar[loopIndex].x = ancv2.x - vertices[loopIndex + 6].x;
		secondveccar[loopIndex].z = ancv2.z - vertices[loopIndex + 6].z;
		secondveccar[loopIndex].y = vertices[loopIndex + 6].y;
		veccar[loopIndex] = vertices[loopIndex + 12];
		car_dvecs[loopIndex] = vertices[loopIndex + 18];
	}

	for (loopIndex = 0; loopIndex < 5; loopIndex++) {
		ywhlang[loopIndex] = 0;
	}

	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[2]), &g_shapes3d[128]);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[3]), &g_shapes3d[116]);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[4]), &g_shapes3d[117]);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[5]), &g_shapes3d[118]);
	shape3d_init_shape(locate_shape_fatal(pl_carres_3d, aCar0[6]), &g_shapes3d[119]);

	firstOpponent = arg_opponentcarid[0];
	if (firstOpponent != -1) {
		if (arg_playercarid[0] == firstOpponent && arg_playercarid[1] == arg_opponentcarid[1] &&
			arg_playercarid[2] == arg_opponentcarid[2] && arg_playercarid[3] == arg_opponentcarid[3])
		{
			/* PLATFORM(memory): query a resource block size. */ sizeBytes = mmgr_get_chunk_size_bytes(pl_carres_3d);
			/* PLATFORM(memory): allocate resource memory. */ carcopyresourceptr = mmgr_alloc_resbytes(aCar0[7], sizeBytes);
			
			for (copyOffset = 0; copyOffset < sizeBytes; copyOffset++) {
				carcopyresourceptr[copyOffset] = pl_carres_3d[copyOffset];
			}
		} else {
			aStxxx[2] = arg_opponentcarid[0];
			aStxxx[3] = arg_opponentcarid[1];
			aStxxx[4] = arg_opponentcarid[2];
			aStxxx[5] = arg_opponentcarid[3];
			/* PLATFORM(file): load a 3D resource. */ carcopyresourceptr = file_load_3dres(aStxxx);
		}

		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[8]), &g_shapes3d[125]);
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[9]), &g_shapes3d[127]);

		vertices = &(g_shapes3d[127].shape3d_verts[8]);
		ctrmesh.z = vertices[0].z;
		ctrmesh.x = (vertices[3].x + vertices[0].x)>> 1;
		g_op_carvector2.z = vertices[6].z;
		g_op_carvector2.x = (vertices[6].x + vertices[9].x) >> 1;

		for (loopIndex = 0; loopIndex < 6; loopIndex++) {
			veco[loopIndex].x = ctrmesh.x - vertices[loopIndex + 0].x;
			veco[loopIndex].z = ctrmesh.z - vertices[loopIndex + 0].z;
			veco[loopIndex].y = vertices[loopIndex + 0].y;
			secondoveh[loopIndex].x = g_op_carvector2.x - vertices[loopIndex + 6].x;
			secondoveh[loopIndex].z = g_op_carvector2.z - vertices[loopIndex + 6].z;
			secondoveh[loopIndex].y = vertices[loopIndex + 6].y;
			opponent_pointc[loopIndex] = vertices[loopIndex + 12];
			veh_od[loopIndex] = vertices[loopIndex + 18];
		}
		for (loopIndex = 0; loopIndex < 5; loopIndex++) {
			buf_obase[loopIndex] = 0;
		}
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[10]), &g_shapes3d[129]);
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[11]), &g_shapes3d[120]);
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[12]), &g_shapes3d[121]);
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[13]), &g_shapes3d[122]);
		shape3d_init_shape(locate_shape_fatal(carcopyresourceptr, aCar0[14]), &g_shapes3d[123]);
	} else {
		carcopyresourceptr = 0;
	}
}

/* Releases player and opponent car-shape resources. Params: none. Returns: none. State: reads and clears car resource pointers. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void shape3d_free_car_shapes(void)
{
    if (carcopyresourceptr != 0) {
        wheel_update(&g_shapes3d[127].shape3d_verts[8], 0, car_wheel_offsets, buf_obase, veco, (I16S *)&ctrmesh);
        /* PLATFORM(memory): release a resource block. */ mmgr_release(carcopyresourceptr);
    }
    wheel_update(&g_shapes3d[126].shape3d_verts[8], 0, car_wheel_offsets, ywhlang, pts_set, (I16S *)&pos_pt);
    /* PLATFORM(memory): release resource memory. */ mmgr_free(pl_carres_3d);
}

/* Rebuilds cached wheel vertices when steering or wheel height changes. Params: output vertices, steering angle, base heights, cached values, source offsets and origin. Returns: none. State: updates wheel geometry and cached values. */
void wheel_update(struct VECTOR far *out, I16 angle,
              I16S *base, I16S *last_angle_and_y,
              struct VECTOR *source, I16S *origin)
{
    I16 y;
    register I16 i;
    register I16 j;
    I16 end;
    I16 span_start;
    I16 sine;
    I16 cosine;

    if (last_angle_and_y[4] != angle) {
        sine = sinfast(angle >> 1);
        cosine = cosfast(angle >> 1);

        for (i = 0; i < 6; ++i) {
            out[i].x = mulscl(source[i].z, sine)
                     + origin[0]
                     + mulscl(source[i].x, cosine);
            out[i].z = mulscl(source[i].x, sine)
                     + origin[2]
                     + mulscl(source[i].z, cosine);
        }

        for (i = 6; i < 12; ++i) {
            out[i].x = mulscl(source[i].z, sine)
                     + origin[3]
                     + mulscl(source[i].x, cosine);
            out[i].z = mulscl(source[i].x, sine)
                     + origin[5]
                     + mulscl(source[i].z, cosine);
        }

        last_angle_and_y[4] = angle;
    }

    for (j = 0; j < 4; ++j) {
        /* PORT: short base heights are promoted to 16-bit int for signed division. */
        y = base[j] / 64;
        if (last_angle_and_y[j] != y) {
            span_start = j * 6;
            end = span_start + 6;
            i = span_start;
            while (i < end) {
                out[i].y = source[i].y - y;
                ++i;
            }
            last_angle_and_y[j] = y;
        }
    }
}

static U8  arrowConn0[6] = { 0, 0, 1, 0, 1, 0 };
static U8  arrowConn1[6] = { 0, 1, 0, 0, 1, 0 };
static U8  terrConnDataEtoW[19] = { 0, 0, 0, 0, 0, 0, 1, 2, 1, 3, 0, 2, 3, 0, 0, 1, 1, 3, 2 };
static U8  terrConnDataWtoE[19] = { 0, 0, 0, 0, 0, 0, 1, 2, 0, 3, 1, 0, 0, 3, 2, 2, 3, 1, 1 };
static U8  terrConnDataNtoS[19] = { 0, 0, 0, 0, 0, 0, 1, 1, 5, 0, 4, 5, 0, 0, 4, 1, 5, 4, 1 };
static U8  terrConnDataStoN[19] = { 0, 0, 0, 0, 0, 0, 1, 0, 5, 1, 4, 0, 5, 4, 0, 5, 1, 1, 4 };

/* Builds track and route data from the selected track resources. Params: none. Returns: setup status. State: reads track resources and updates track tables. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
I16 track_setup(void)
{
  I8 matches;
  U8  entryPt;
  U8  runway;
  U8  runLength;
  I8 startCount;
  I8S  cur_col;
  I16 xsave;
  struct TrackNode far *qnode;
  struct TRKOBJINFO *objInfo;
  I8S  oldRow;
  U8  connStatusOf[0x385];
  I8 prev_exit_type;
  I8S  oldX;
  U8  object;
  I16 elem_idx;
  I16 sampleIdx;
  I16 angle;
  U8  prevBlock;
  I8S  y;
  U8  on_path[0x385];
  register I16 i;
  I8 err;
  struct TRKOBJINFO *blk;
  I16 prev_path;
  U8  qCount;
  U8  exit;
  U8  sub;
  I8S  prev_reversed_dir;
  I8S  connStat;
  register I16 j;
  struct TrackNode far *alloc;
  struct VECTOR far *camData;
  struct VECTOR cur_pos;
  U8  part_of[0x385];
  U8  tileTerr;
  I8 arrow;
  struct TrackNode far *queuePtr;
  I8S  conn;
  I8 loop_done;
  U8  prev_elem;
  /* PLATFORM(memory): allocate resource memory. */ alloc = mmgr_alloc_resbytes("tcomp", 0x380L);
  if (alloc == 0)
    return 2;
  queuePtr = alloc;
  startCount = 0;
  runway = 0;
  g_trackpiecescounter = 0;
  for (i = 0; i < 0x385; i++)
    td19hdl[i] = 0xff;

  for (y = 0; y < 30; y++)
  {
    prev_exit_type = 'c';
    for (cur_col = 0; cur_col < 30; cur_col++)
    {
      tileTerr = td15p_9[gterrtrk[y] + cur_col];
      if (terrConnDataEtoW[tileTerr] != prev_exit_type && prev_exit_type != 'c')
      {
        err = 11;
        goto error;
      }
      prev_exit_type = terrConnDataWtoE[tileTerr];
    }

  }

  for (cur_col = 0; cur_col < 30; cur_col++)
  {
    prev_exit_type = 'c';
    for (y = 0; y < 30; y++)
    {
      tileTerr = td15p_9[gterrtrk[y] + cur_col];
      if (terrConnDataNtoS[tileTerr] != prev_exit_type && prev_exit_type != 'c')
      {
        err = 11;
        goto error;
      }
      else
      {
        prev_exit_type = terrConnDataStoN[tileTerr];
      }
    }

  }

  for (y = 0; y < 30; y++)
  {
    for (cur_col = 0; cur_col < 30; cur_col++)
    {
      object = td14tb[lnoffsets[y] + cur_col];
      if (object >= 0xfd)
        object = 0;
      if (object >= 0xb6)
      {
        object = 4;
        td14tb[lnoffsets[y] + cur_col] = 4;
      }
      switch (object)
      {
        default:
          goto next_col;

        case 1:

        case 0x86:

        case 0x93:
          st_hdg = 0;
          break;

        case 0x87:

        case 0x94:

        case 0xb3:
          st_hdg = 0x200;
          break;

        case 0x88:

        case 0x95:

        case 0xb4:
          st_hdg = 0x100;
          break;

        case 0x89:

        case 0x96:

        case 0xb5:
          st_hdg = 0x300;
          break;

      }

      if (startCount != 0)
      {
        err = 3;
        goto error;
      }
      idxtrk = cur_col;
      tagtrk = y;
      if ((tileTerr = td15p_9[gterrtrk[y] + cur_col]) == 6)
        g_hillf = 1;
      else
        g_hillf = 0;
      startCount++;
      next_col: ;
    }

  }

  if (startCount == 0)
  {
    err = 1;
    goto error;
  }
  g_trackpiecescounter = 0;
  qCount = 0;
  g_road_piece_id = 0;
  trk_sample_count = 0;
  runLength = 0;
  loop_done = 0;
  for (i = 0; i < 0x385; i++)
  {
    on_path[i] = 0;
    g_td01_track_filecpy[i] = 0xffff;
    trackdata_penalty_related[i] = 0xffff;
  }

  cur_col = idxtrk;
  y = tagtrk;
  angle = st_hdg;
  prev_exit_type = 0;
  prev_path = -1;
  route_loop:
  matches = 0;
  if (cur_col < 0 || y < 0 || cur_col > 29 || y > 29)
  {
    backtrack:
    if (qCount == 0)
      goto route_done;

    qCount--;
    qnode = queuePtr + qCount;
    cur_col = qnode->column;
    y = qnode->row;
    object = qnode->element;
    sub = qnode->block;
    connStat = qnode->connection;
    prev_exit_type = qnode->previousExitType;
    prev_path = qnode->parent;
    runLength = qnode->terrain;
    oldX = qnode->previousColumn;
    oldRow = qnode->previousRow;
    prev_elem = qnode->previousElement;
    prevBlock = qnode->previousBlock;
    prev_reversed_dir = qnode->previousReversed;
    matches = 1;
  }
  else
  {
    object = td14tb[lnoffsets[y] + cur_col];
    tileTerr = td15p_9[gterrtrk[y] + cur_col];
    if (object != 0 && tileTerr != 0 && tileTerr >= 7 && tileTerr < 11)
      object = subst_hillroad(tileTerr, object);
    if (object >= 0xfd)
    {
      switch (object)
      {
        case 0xfd:
          cur_col--;
          y--;
          switch (angle)
        {
          case 0:
            entryPt = 12;
            break;

          case 0x200:
            entryPt = 0;
            break;

          case 0x100:
            entryPt = 0;
            break;

          case 0x300:
            entryPt = 9;
            break;

        }

          break;

        case 0xfe:
          y--;
          switch (angle)
        {
          case 0:
            entryPt = 11;
            break;

          case 0x200:
            entryPt = 0;
            break;

          case 0x100:
            entryPt = 6;
            break;

          case 0x300:
            entryPt = 7;
            break;

        }

          break;

        case 0xff:
          cur_col--;
          switch (angle)
        {
          case 0:
            entryPt = 10;
            break;

          case 0x200:
            entryPt = 5;
            break;

          case 0x100:
            entryPt = 0;
            break;

          case 0x300:
            entryPt = 8;
            break;

        }

          break;

      }

      object = td14tb[lnoffsets[y] + cur_col];
    }
    else
    {
      switch (angle)
      {
        case 0:
          entryPt = 2;
          break;

        case 0x200:
          entryPt = 1;
          break;

        case 0x100:
          entryPt = 4;
          break;

        case 0x300:
          entryPt = 3;
          break;

      }

    }
    if (runway == 0 && entryPt == 0)
    {
      err = 2;
      goto error;
    }
    matches = 0;
    objInfo = trklst[object].info;
    if (objInfo != 0)
    {
      for (i = 0; i < objInfo->noOfBlocks; i++)
      {
        conn = -1;
        blk = objInfo + i;
        if (blk->entry == entryPt)
        {
          if (blk->entryType != prev_exit_type)
            goto elem_mismatch;
          conn = 0;
        }
        else
          if (blk->exitPoint == entryPt)
        {
          if (blk->exitType != prev_exit_type)
            goto elem_mismatch;
          conn = 1;
        }
        if (conn >= 0 && on_path[lnoffsets[y] + cur_col] != 0)
        {
          for (j = 0; j < g_trackpiecescounter; j++)
          {
            if (g_column_of_trkdata21_pth[j] == cur_col && tdfrompathrow22[j] == y && part_of[j] == ((U8 ) i))
            {
              if (connStatusOf[j] != conn)
              {
                err = 5;
                goto error;
              }
              conn = -1;
              if (g_td01_track_filecpy[prev_path] == 0xffff)
                g_td01_track_filecpy[prev_path] = j;
              else
                trackdata_penalty_related[prev_path] = j;
              if (j == 0)
                loop_done = 1;
            }
          }

        }
        if (conn < 0)
        {
        }
        else
        {
          if (matches == 0)
          {
            sub = i;
            connStat = conn;
          }
          else
          {
            if (qCount == 0x40)
            {
              err = 8;
              goto error;
            }
            qnode = queuePtr + qCount;
            qnode->column = cur_col;
            qnode->row = y;
            qnode->element = object;
            qnode->block = i;
            qnode->connection = conn;
            qnode->previousExitType = prev_exit_type;
            qnode->parent = prev_path;
            qnode->terrain = runLength;
            qnode->previousColumn = oldX;
            qnode->previousRow = oldRow;
            qnode->previousElement = prev_elem;
            qnode->previousBlock = prevBlock;
            qnode->previousReversed = prev_reversed_dir;
            qCount++;
          }
          matches++;
        }
      }

    }
    if (matches == 0)
    {
      if (prev_exit_type != 1 || runway >= 2)
        goto backtrack;
      if (runLength < 2)
      {
        err = 9;
        goto error;
      }
      runLength++;
      runway++;
      switch (angle)
      {
        case 0:
          cur_col = oldX;
          y = oldRow - runway - 1;
          break;

        case 0x200:
          cur_col = oldX;
          y = oldRow + runway + 1;
          break;

        case 0x100:
          y = oldRow;
          cur_col = oldX + runway + 1;
          break;

        case 0x300:
          y = oldRow;
          cur_col = oldX - runway - 1;
          break;

      }

      goto route_loop;
    }
  }
  if (matches != 0)
    goto after_route;
  else
    goto route_loop;

  route_done:
  if (loop_done == 0)
  {
    err = 7;
    goto error;
  }

  sampled_trk_column = idxtrk;
  g_cur_track_row = tagtrk;
  i = g_trackpiecescounter / 3;
  if (i > 0x40)
    i = 0x40;
  trk_sample_count = i;
  for (i = 0; i < 0x385; i++)
    part_of[i] = 0;

  goto sampling;

  after_route:
  if (runway > 1)
  {
    err = 10;
    goto error;
  }
  runway = 0;
  on_path[lnoffsets[y] + cur_col] = 1;
  part_of[g_trackpiecescounter] = sub;
  connStatusOf[g_trackpiecescounter] = connStat;
  if (prev_path != (-1))
  {
    if (g_td01_track_filecpy[prev_path] == 0xffff)
      g_td01_track_filecpy[prev_path] = g_trackpiecescounter;
    else
      trackdata_penalty_related[prev_path] = g_trackpiecescounter;
  }
  prev_path = g_trackpiecescounter;
  g_column_of_trkdata21_pth[g_trackpiecescounter] = cur_col;
  tdfrompathrow22[g_trackpiecescounter] = y;
  td_18_ref[g_trackpiecescounter] = (connStat << 4) + sub;
  road_trk[g_trackpiecescounter] = object;
  objInfo = trklst[object].info;
  blk = objInfo + sub;
  arrow = blk->opponent3;
  if (arrow == 0)
  {
    runLength++;
    goto next_piece;
  }
  if (arrow != (-1) && runLength > 3 && g_road_piece_id != 0x30)
  {
    objInfo = trklst[object].info;
    blk = objInfo + sub;
    arrow = blk->opponent3;
    objInfo = trklst[prev_elem].info;
    blk = objInfo + prevBlock;
    if (prev_reversed_dir != 0)
    {
      if (blk->cameraOverlay.cameraOffsetOverride != 0)
        camData = (struct VECTOR *) blk->cameraOverlay.cameraOffsetOverride;
      else
        camData = (struct VECTOR *) blk->cameraDataOffset;
    }
    else
      camData = (struct VECTOR *) blk->cameraDataOffset;
    if (prev_reversed_dir != 0)
      cur_pos = camData[blk->arrowType * 2 + 2];
    else
      cur_pos = camData[blk->arrowType * 2 + 1];
    if (connStat != 0)
      arrow = arrowConn1[arrow];
    else
      arrow = arrowConn0[arrow];
    angle = blk->arrowOrient;
    switch (angle)
    {
      case 0x300:
        xsave = cur_pos.x;
        cur_pos.x = -cur_pos.z;
        cur_pos.z = xsave;
        break;

      case 0x200:
        cur_pos.z = -cur_pos.z;
        cur_pos.x = -cur_pos.x;
        break;

      case 0x100:
        xsave = cur_pos.x;
        cur_pos.x = cur_pos.z;
        cur_pos.z = -xsave;
        break;

    }

    if (prev_reversed_dir != 0)
      g_td08d[g_road_piece_id] = angle ^ 0x200;
    else
      g_td08d[g_road_piece_id] = angle;
    trkd23adr[g_road_piece_id] = arrow;
    if (td15p_9[gterrtrk[oldRow] + oldX] == 6)
      cur_pos.y += 0x1c2;
    td10checkptr[g_road_piece_id].y = cur_pos.y;
    td10checkptr[g_road_piece_id].z = ((trklst[prev_elem].multiTile & 1) ? (r_zp[oldRow]) : (row_ctr_zs[oldRow])) + cur_pos.z;
    td10checkptr[g_road_piece_id].x = ((trklst[prev_elem].multiTile & 2) ? (xcols[oldX + 1]) : (trackctrpos2[oldX])) + cur_pos.x;
    td19hdl[lnoffsets[oldRow] + oldX] = g_road_piece_id;
    g_road_piece_id++;
  }
  runLength = 0;
  next_piece:
  if ((++g_trackpiecescounter) == 0x385)
  {
    err = 6;
    goto error;
  }

  objInfo = trklst[object].info;
  blk = objInfo + sub;
  if (connStat != 0)
  {
    exit = blk->entry;
    prev_exit_type = blk->entryType;
  }
  else
  {
    exit = blk->exitPoint;
    prev_exit_type = blk->exitType;
  }
  oldX = cur_col;
  oldRow = y;
  prev_reversed_dir = connStat;
  prevBlock = sub;
  prev_elem = object;
  switch (exit)
  {
    case 1:
      y--;
      angle = 0;
      goto route_loop;

    case 5:
      y--;
      cur_col++;
      angle = 0;
      goto route_loop;

    case 6:
      y++;

    case 4:
      cur_col--;
      angle = 0x300;
      goto route_loop;

    case 10:
      cur_col++;

    case 2:
      y++;
      angle = 0x200;
      goto route_loop;

    case 12:
      cur_col++;

    case 11:
      y += 2;
      angle = 0x200;
      goto route_loop;

    case 3:
      cur_col++;
      angle = 0x100;
      goto route_loop;

    case 7:
      cur_col++;
      y++;
      angle = 0x100;
      goto route_loop;

    case 8:
      cur_col += 2;
      angle = 0x100;
      goto route_loop;

    case 9:
      cur_col += 2;
      y++;
      angle = 0x100;
      goto route_loop;

  }

  goto route_loop;
  elem_mismatch:
  err = 4;

  goto error;
  sampling:
  j = 0;
  for (i = 0; i < trk_sample_count; i++)
  {
    sampleIdx = g_trackpiecescounter * i / trk_sample_count;
    cur_col = g_column_of_trkdata21_pth[sampleIdx];
    y = tdfrompathrow22[sampleIdx];
    if (part_of[gterrtrk[y] + cur_col] == 0)
    {
      part_of[gterrtrk[y] + cur_col] = 1;
      elem_idx = road_trk[sampleIdx];
      sub = td_18_ref[sampleIdx] & 0x0f;
      conn = td_18_ref[sampleIdx] & 0x10;
      blk = trklst[elem_idx].info;
      if (conn != 0 && blk[sub].cameraOverlay.cameraOffsetOverride != 0)
        camData = (struct VECTOR *) blk[sub].cameraOverlay.cameraOffsetOverride;
      else
        camData = (struct VECTOR *) blk[sub].cameraDataOffset;
      cur_pos = camData[blk[sub].arrowType * 2];
      angle = blk[sub].arrowOrient;
      switch (angle)
      {
        case 0x300:
          xsave = cur_pos.x;
          cur_pos.x = -cur_pos.z;
          cur_pos.z = xsave;
          break;

        case 0x200:
          cur_pos.z = -cur_pos.z;
          cur_pos.x = -cur_pos.x;
          break;

        case 0x100:
          xsave = cur_pos.x;
          cur_pos.x = cur_pos.z;
          cur_pos.z = -xsave;
          break;

      }

      if (td15p_9[gterrtrk[y] + cur_col] == 6)
        trackdat7[j] = 0x1c2;
      else
        trackdat7[j] = 0;
      td6_ptr_b[j] = 0;
      trkptrpath[j].y = trackdat7[j] + cur_pos.y;
      if (trklst[elem_idx].multiTile & 1)
        trkptrpath[j].z = r_zp[y] + cur_pos.z;
      else
        trkptrpath[j].z = row_ctr_zs[y] + cur_pos.z;
      trkptrpath[j].x = ((trklst[elem_idx].multiTile & 2) ? (xcols[cur_col + 1]) : (trackctrpos2[cur_col])) + cur_pos.x;
      j++;
    }
  }

  trk_sample_count = j;
  err = 0;
  goto release;
  error:
  if (cur_col == (-1))
    cur_col = 0;
  else
    if (cur_col == 30)
    cur_col = 29;

  if (y == (-1))
    y = 0;
  else
    if (y == 30)
    y = 29;
  sampled_trk_column = cur_col;
  g_cur_track_row = y;
  /* PLATFORM(memory): release a resource block. */ release:
  mmgr_release((I8 far *) alloc);

  return err;
}

/* Loads opponent-specific data. Params: none. Returns: none. State: stores the loaded opponent resource. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
void load_opponent_data(void)
{
    U32  savedSums[256];
    I16S savedIndices[256];
    void far *res;
    I16S dep;
    I16S count;
    U32  minSum;
    I8 far *pathData;
    I16S f;
    I16S rt[256];
    U32  sum;
    U8  far *tbl;
    I16S el;
    I16S fork;
    I16S valid;
    register I16S i;
    I16S sequence[901];
    register I16S visited;

    aOpp1[3] = (I8)(globalgamesettings.game_opponenttype + '0');
    /* PLATFORM(file): load a resource file. */ res = file_load_resource_file(aOpp1);
    copy_string(opptext_label, locate_text_resource(res, "nam"));
    /* PLATFORM(file): locate opponent path data in the loaded bundle. */ pathData = locate_shape_alt(res, "path");
    /* PORT: the speed table is traversed through a segmented FAR resource pointer. */ /* PLATFORM(file): locate opponent speed data in the loaded bundle. */ tbl = (U8  far *)locate_shape_alt(res, "sped");
    for (i = 0; i < 16; i++)
        opponent_spd_tbl[i] = tbl[i];

    minSum = 0x000f423fUL;
    count = 0;
    sum = 0;
    dep = 0;
    i = 0;
    for (;;) {
        f = 0;
        el = g_td01_track_filecpy[i];
        if (el == 0) {
            valid = 1;
            f = 1;
        } else if (el == -1) {
            valid = 0;
            f = 1;
        } else if (count != 0) {
            for (visited = 0; visited < count; visited++) {
                if (sequence[visited] == i) {
                    valid = 0;
                    f = 1;
                }
            }
        }
        sequence[count++] = i;
        sum += (U8 )tbl[(U8 )road_trk[i]] + 1;
        if (f != 0) {
            if (valid != 0 && sum < minSum) {
                sequence[count++] = 0;
                minSum = sum;
                for (visited = 0; visited < count; visited++)
                    td3[visited] = sequence[visited];
                td3[count] = 0;
                td3[count + 1] = 1;
            }
            if (dep == 0) {
                /* PLATFORM(memory): release a loaded resource. */ unload_resource(res);
                return;
            }
            dep--;
            i = savedIndices[dep];
            count = rt[dep];
            sum = savedSums[dep];
        } else {
            fork = trackdata_penalty_related[i];
            if (fork != -1) {
                savedIndices[dep] = fork;
                rt[dep] = count;
                savedSums[dep++] = sum;
            }
            i = el;
        }
    }
}

/* Maps a terrain and object pair to its hill-road replacement. Params: terrain and object IDs. Returns: replacement object ID. State: reads fixed substitution tables. */
U8  subst_hillroad(U8  a, U8  b)
{
    switch (a) {
    case 7:
        switch (b) {
        case 4: return 0xb6;
        case 14: return 0xba;
        case 24: return 0xbe;
        case 39: case 59: case 98: return 0xc2;
        }
        break;
    case 8:
        switch (b) {
        case 5: return 0xb7;
        case 15: return 0xbb;
        case 25: return 0xbf;
        case 36: case 56: case 95: return 0xc3;
        }
        break;
    case 9:
        switch (b) {
        case 4: return 0xb8;
        case 14: return 0xbc;
        case 24: return 0xc0;
        case 38: case 58: case 97: return 0xc4;
        }
        break;
    case 10:
        switch (b) {
        case 5: return 0xb9;
        case 15: return 0xbd;
        case 25: return 0xc1;
        case 37: case 57: case 96: return 0xc5;
        }
        break;
    }
    return 0;
}
