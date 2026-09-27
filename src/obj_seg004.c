/* Scratch whole-object candidate in target member order. */
struct VECTOR { short x; short y; short z; };
struct SHAPE3D { unsigned short numverts; struct VECTOR far *shape3d_verts; unsigned short numprimitives; unsigned char numpaints; unsigned char reserved; void far *primitives; void far *cull1; void far *cull2; };
struct TRKOBJINFO { unsigned char noOfBlocks,entry,exitPoint,entryType,exitType,arrowType; short arrowOrient; short *cameraDataOffset; union { struct { unsigned char opponent1,opponent2; } opponent; unsigned short cameraOffsetOverride; } cameraOverlay; unsigned char opponent3,opponentSpeedCode; };
struct TRACKOBJECT { struct TRKOBJINFO *info; short rotation; struct SHAPE3D *shape,*lowShape; unsigned char overlay; char surface,ignoreZ,multiTile,physicalModel,unknown; };
struct TrackNode { unsigned char column,row,element,block,connection,terrain,previousColumn,previousRow,previousElement,previousBlock,previousReversed,previousExitType; short parent; };
struct GAMESTATE { unsigned char before_game_inputmode[0x3f5]; char game_inputmode; };
struct WALLREC { short orientation,x,z; };
struct GAMEINFO { char game_playercarid[4],game_playermaterial,game_playertransmission,game_opponenttype,game_opponentcarid[4],game_opponentmaterial,game_opponenttransmission,game_trackname[9]; unsigned short game_framespersec,game_recordedframes; };
extern struct GAMESTATE state;
extern int trackrows[30],terrainrows[30],terrainpos[30],trackpos[30],trackcenterpos[30],trackpos2[30],trackcenterpos2[30],terraincenterpos[30];
extern unsigned char far *td14_elem_map_main;
extern unsigned char far *td15_terr_map_main;
extern struct TRACKOBJECT trkObjectList[215];
extern int far sin_fast(int),far cos_fast(int),far multiply_and_scale(int,int);
extern int far polarRadius2D(int,int),far polarAngle(int,int);
extern short planindex,wallindex,word_34A8C,word_34988;
extern unsigned char corkFlag,current_surf_type,byte_4392C;
extern short terrainHeight,elem_xCenter,elem_zCenter,hillHeightConsts[];
extern struct WALLREC far *wallptr;
extern unsigned char far *planptr;
extern unsigned char far *current_planptr;
extern short wallOrientation,wallStartX,wallStartZ;
extern short highEntrZBounds0[],highEntrZBounds1[],highEntrXInnBounds0[],highEntrXInnBounds1[],highEntrXOutBounds0[],highEntrXOutBounds1[];
extern short loopSurface_maxZ,loopSurface_ZBounds0[],loopSurface_ZBounds1[],loopSurface_XBounds0[],loopSurface_XBounds1[];
extern short loopBase_ZBounds0[],loopBase_ZBounds1[],loopBae_InnXBounds0[],loopBase_InnXBounds1[],loopBase_OutXBounds0[],loopBase_OutXBounds1[];
extern short bkRdEntr_triang_zAdjust[],corkLR_negZBound[],corkLR_posZBound[];
extern void far *game1ptr;
extern void far *game2ptr;
extern void far *curshapeptr;
extern long far mmgr_get_res_ofs_diff_scaled(void),far mmgr_get_chunk_size_bytes(char far *);
extern char far *file_load_3dres(char *name);
extern char far *locate_shape_fatal();
extern char far *locate_shape_nofatal();
extern void shape3d_init_shape(char far *,struct SHAPE3D *);
extern struct SHAPE3D game3dshapes[130];
extern char aBarn[116][5];
extern char aGame1[],aGame2[];
extern char aCar0[],aCar1[],aCar2[],aExp0_0[],aExp1_0[],aExp2_0[],aExp3_0[],aCar2_0[],aCar0_0[],aCar1_0[],aCar2_1[],aExp0_1[],aExp1_1[],aExp2_1[],aExp3_1[];
extern char aStxxx[];
extern char far *carresptr;
extern char far *car2resptr;
extern struct VECTOR word_32CBA,word_32CC0,carshapevecs[],word_3441E[],data_34442[],data_34466[];
extern struct VECTOR word_32D04,word_32D0A,word_348F4[],word_34918[],data_3493C[],data_34960[];
extern short word_443E8[],word_4448A[],unk_3E710[4];
extern struct VECTOR unk_3E640[],unk_3E646[],unk_3E676[],unk_3E682[],unk_3E68E[],unk_3E69A[];
extern char byte_45635,byte_4616E;
extern char far *mmgr_alloc_resbytes(char *,long);
extern void far mmgr_free(void far *),far mmgr_release(void far *);
extern char far *word_338A8;
extern char far *word_33892;
extern char far *word_354B0;
extern char far *word_354AA;
extern int bto_auxiliary1(int,int,struct VECTOR *);
extern void sub_204AE(struct VECTOR far *,int,short *,short *,struct VECTOR *,short *);
extern unsigned char subst_hillroad_track(unsigned char,unsigned char);
extern short far *td01_track_file_cpy;
extern short far *td02_penalty_related;
extern unsigned char far *trackdata19;
extern unsigned char far *td17_trk_elem_ordered;
extern unsigned char far *trackdata18;
extern unsigned char far *trackdata23;
extern unsigned char far *td21_col_from_path;
extern unsigned char far *td22_row_from_path;
extern int track_pieces_counter;
extern unsigned short track_angle;
extern char startcol2,startrow2,hillFlag;
extern unsigned char byte_45D90,byte_45E16;
extern unsigned short far *td08_direction_related;
extern unsigned short far *trackdata6;
extern unsigned short far *trackdata7;
extern struct VECTOR far *td10_track_check_rel;
extern struct VECTOR far *trackdata9;
extern struct GAMEINFO gameconfig;
extern unsigned char oppnentSped[16];
extern short far *trackdata3;
extern char aOpp1[],unk_46464[];
extern void far *file_load_resfile(const char *);
extern char far *locate_text_res(void far *,const char *);
extern char far *locate_shape_alt(void far *,const char *);
extern void copy_string(char *,char far *);
extern void far unload_resource(void far *);

void build_track_object(struct VECTOR *position, struct VECTOR *nextPosition)
{
	struct TRACKOBJECT *o;
	short wallOrientationState;
	short angAngle;
	unsigned char cellTerr;
	struct VECTOR absLocalCrds;
	struct VECTOR locElemCrds;
	short physModel;
	short side;
	signed char trkR;
	short localX;
	short baseIndex;
	signed char trackCol;
	struct VECTOR nextPositionElemCrds;
	unsigned char surfType;
	short elementAngle;
	short wallIx;
	short wallHi;
	register short step;
	short turnRadius;
	unsigned char tileElem;
	struct VECTOR effElemCrds;

	planindex = 0;
	wallindex = -1;
	word_34A8C = -12;
	word_34988 = -1000;
	corkFlag = 0;
	current_surf_type = 4;
	byte_4392C = 1;
	step = 0;
	wallOrientationState = step;
	elementAngle = step;
	terrainHeight = step;
	trackCol = (signed char)(position->x >> 10);
	trkR = (signed char)(position->z >> 10);
	physModel = -1;
	if (trackCol < 0 || trackCol > 29 || trkR < 0 || trkR > 29)
		goto select_plan;

	elem_xCenter = trackcenterpos2[trackCol];
	elem_zCenter = terraincenterpos[trkR];
	cellTerr = td15_terr_map_main[trackrows[trkR] + trackCol];
	if (cellTerr != 0) {
		switch (cellTerr) {
		case 6:
			terrainHeight = hillHeightConsts[1];
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
			locElemCrds.x = (short)(position->x - elem_xCenter);
			locElemCrds.z = (short)(position->z - elem_zCenter);
			side = multiply_and_scale(cos_fast(step), locElemCrds.x) +
				multiply_and_scale(sin_fast(step), locElemCrds.z);
			if (side < 0)
				current_surf_type = 5;
			break;
		case 1:
			current_surf_type = 5;
			break;
		}
	}

	tileElem = td14_elem_map_main[terrainrows[trkR] + trackCol];
	if (tileElem == 0)
		goto finish;
	if (tileElem >= 0xfd) {
		switch (tileElem) {
		case 0xfd:
			tileElem = td14_elem_map_main[terrainrows[trkR + 1] + trackCol - 1];
			if (trkObjectList[tileElem].multiTile & 1)
				elem_zCenter = terrainpos[trkR + 1];
			if (trkObjectList[tileElem].multiTile & 2)
				elem_xCenter = trackpos2[trackCol];
			break;
		case 0xfe:
			tileElem = td14_elem_map_main[terrainrows[trkR + 1] + trackCol];
			if (trkObjectList[tileElem].multiTile & 1)
				elem_zCenter = terrainpos[trkR + 1];
			if (trkObjectList[tileElem].multiTile & 2)
				elem_xCenter = trackpos2[trackCol + 1];
			break;
		case 0xff:
			tileElem = td14_elem_map_main[terrainrows[trkR] + trackCol - 1];
			if (trkObjectList[tileElem].multiTile & 1)
				elem_zCenter = terrainpos[trkR];
			if (trkObjectList[tileElem].multiTile & 2)
				elem_xCenter = trackpos2[trackCol];
			break;
		}
	} else if (trkObjectList[tileElem].multiTile != 0) {
		if (trkObjectList[tileElem].multiTile & 1)
			elem_zCenter = terrainpos[trkR];
		if (trkObjectList[tileElem].multiTile & 2)
			elem_xCenter = trackpos2[trackCol + 1];
	}

	locElemCrds.x = (short)(position->x - elem_xCenter);
	locElemCrds.z = (short)(position->z - elem_zCenter);
	nextPositionElemCrds.x = (short)(nextPosition->x - elem_xCenter);
	nextPositionElemCrds.z = (short)(nextPosition->z - elem_zCenter);
	if (tileElem != 0 && cellTerr >= 7 && cellTerr < 11)
		tileElem = subst_hillroad_track(cellTerr, tileElem);

	o = &trkObjectList[tileElem];
	physModel = o->physicalModel;
	elementAngle = o->rotation;
	switch (elementAngle) {
	case 0:
		break;
	case 0x300:
		localX = locElemCrds.x;
		locElemCrds.x = locElemCrds.z;
		locElemCrds.z = (short)-localX;
		localX = nextPositionElemCrds.x;
		nextPositionElemCrds.x = nextPositionElemCrds.z;
		nextPositionElemCrds.z = (short)-localX;
		break;
	case 0x200:
		locElemCrds.z = (short)-locElemCrds.z;
		locElemCrds.x = (short)-locElemCrds.x;
		nextPositionElemCrds.z = (short)-nextPositionElemCrds.z;
		nextPositionElemCrds.x = (short)-nextPositionElemCrds.x;
		break;
	case 0x100:
		localX = locElemCrds.x;
		locElemCrds.x = (short)-locElemCrds.z;
		locElemCrds.z = localX;
		localX = nextPositionElemCrds.x;
		nextPositionElemCrds.x = (short)-nextPositionElemCrds.z;
		nextPositionElemCrds.z = localX;
		break;
	}
	angAngle = 0;
	surfType = (unsigned char)(o->surface + 1);
	if ((signed char)surfType < 1)
		surfType = 1;
	absLocalCrds.x = locElemCrds.x < 0 ? (short)-locElemCrds.x : locElemCrds.x;
	absLocalCrds.z = locElemCrds.z < 0 ? (short)-locElemCrds.z : locElemCrds.z;
	switch (physModel) {
	case 0:
		if (state.game_inputmode == 0 && locElemCrds.x > 0) {
			if (locElemCrds.z < -380) planindex = 0x83;
			else if (locElemCrds.z < -300) planindex = 0x84;
		}
	case 1:
		if (absLocalCrds.x < 0x78) current_surf_type = surfType;
		break;
	case 12:
		if (absLocalCrds.x < 0x78 || absLocalCrds.z < 0x78) current_surf_type = surfType;
		break;
	case 5:
		locElemCrds.x = -locElemCrds.x;
	case 4:
		current_surf_type = surfType;
		if (locElemCrds.x > 0) {
			locElemCrds.z = -locElemCrds.z;
			locElemCrds.x = -locElemCrds.x;
		}
	case 3:
		turnRadius = polarRadius2D(locElemCrds.x + 0x400, locElemCrds.z + 0x400);
		if (turnRadius > 0x588 && turnRadius < 0x678) current_surf_type = surfType;
		break;
	case 6:
		if (absLocalCrds.x < 0x78) {
			current_surf_type = surfType;
			break;
		}
	case 2:
		turnRadius = polarRadius2D(locElemCrds.x + 0x200, locElemCrds.z + 0x200);
		if (turnRadius > 0x188 && turnRadius < 0x278) current_surf_type = surfType;
		break;
	case 7:
		if (absLocalCrds.x < 0x78) current_surf_type = surfType;
		else {
			turnRadius = polarRadius2D(0x200 - locElemCrds.x, locElemCrds.z + 0x200);
			if (turnRadius > 0x188 && turnRadius < 0x278) current_surf_type = surfType;
		}
		break;
	case 8:
		if (locElemCrds.x >= 0x188 && locElemCrds.x <= 0x278) current_surf_type = surfType;
		else {
			turnRadius = polarRadius2D(locElemCrds.x + 0x400, locElemCrds.z + 0x400);
			if (turnRadius > 0x588 && turnRadius < 0x678) current_surf_type = surfType;
		}
		break;
	case 9:
		if (locElemCrds.x >= -0x278 && locElemCrds.x <= -0x188) current_surf_type = surfType;
		else {
			turnRadius = polarRadius2D(0x400 - locElemCrds.x, locElemCrds.z + 0x400);
			if (turnRadius > 0x588 && turnRadius < 0x678) current_surf_type = surfType;
		}
		break;
	case 10:
		baseIndex = locElemCrds.x < 0 ? -locElemCrds.x : locElemCrds.x;
		for (step = 0; highEntrZBounds1[step] < locElemCrds.z; ++step) { }
		if (highEntrXInnBounds1[step] == highEntrXInnBounds0[step])
			localX = highEntrXInnBounds0[step];
		else
			localX = (short)(highEntrXInnBounds0[step] +
				(long)(highEntrXInnBounds1[step] - highEntrXInnBounds0[step]) *
				(locElemCrds.z - highEntrZBounds0[step]) /
				(highEntrZBounds1[step] - highEntrZBounds0[step]));
		if (highEntrXOutBounds1[step] == highEntrXOutBounds0[step])
			side = highEntrXOutBounds0[step];
		else
			side = (short)(highEntrXOutBounds0[step] +
				(long)(highEntrXOutBounds1[step] - highEntrXOutBounds0[step]) *
				(locElemCrds.z - highEntrZBounds0[step]) /
				(highEntrZBounds1[step] - highEntrZBounds0[step]));
		if (baseIndex > localX && baseIndex < side) {
			current_surf_type = surfType;
			break;
		}
		if (locElemCrds.z < 0 || baseIndex > 0x78) break;
		planindex = 1;
		if (locElemCrds.z >= 0x14e) {
			if (nextPositionElemCrds.x <= -0x78) wallindex = 0xbc;
			else if (nextPositionElemCrds.x >= 0x78) wallindex = 0xba;
		} else if (nextPositionElemCrds.x >= 0) wallindex = 0xbb;
		else wallindex = 0xbd;
		break;
	case 11:
		if (absLocalCrds.x > 0x168) break;
		if (absLocalCrds.x > 0x78) current_surf_type = surfType;
		else {
			planindex = 1;
			if (nextPositionElemCrds.x <= -0x78) wallindex = 0xbc;
			else if (nextPositionElemCrds.x >= 0x78) wallindex = 0xba;
		}
		break;
	case 16:
		if (locElemCrds.z > 0) byte_4392C = 0;
		else if (nextPositionElemCrds.z >= 0) wallindex = 0x66;
		goto ramp_common;
	case 17:
		if (nextPositionElemCrds.z >= 0x1dc) wallindex = 0x67;
	ramp_common:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) < 0x78) {
			planindex = 3;
			current_surf_type = surfType;
			if (wallindex != -1 || locElemCrds.z < 0 || absLocalCrds.x < 0x78) break;
			word_34A8C = 0x2a;
			word_34988 = -12;
			if (locElemCrds.x < 0) wallindex = 0x64;
			else wallindex = 0x65;
		} else {
			if (byte_4392C == 0 || absLocalCrds.x > 0x78) break;
			planindex = 3;
			if (wallindex != -1) break;
			wallOrientationState = 0x200;
			if (locElemCrds.x < 0) wallindex = 0x64;
			else wallindex = 0x65;
		}
		break;
	case 22:
		if (position->y - terrainHeight > 0x186) {
			byte_4392C = 0;
			goto elevated_road;
		}
		if (absLocalCrds.z <= 0x78) current_surf_type = surfType;
		break;
	case 18: case 19:
		if (position->y - terrainHeight > 0x186) {
			byte_4392C = 0;
			goto elevated_road;
		}
		break;
	case 20:
	elevated_road:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) <= 0x78) {
			planindex = 2;
			current_surf_type = surfType;
			if (byte_4392C != 0) {
				if (nextPositionElemCrds.z >= 0x1dc) wallindex = 0x67;
				else if (nextPositionElemCrds.z <= -0x1dc) wallindex = 0x68;
			}
			if (absLocalCrds.x < 0x78) break;
			word_34A8C = 0x2a;
			if (locElemCrds.x < 0) wallindex = 0x64;
			else wallindex = 0x65;
		} else {
			if (byte_4392C == 0 || absLocalCrds.x > 0x78) break;
			planindex = 2;
			word_34A8C = 0x2a;
			wallOrientationState = 0x200;
			if (nextPositionElemCrds.x < 0) wallindex = 0x64;
			else wallindex = 0x65;
		}
		break;
	case 21:
		if (position->y - terrainHeight <= 0x186) goto finish;
		turnRadius = (short)(polarRadius2D((short)(locElemCrds.x + 0x400), (short)(locElemCrds.z + 0x400)) - 0x600);
		if (turnRadius <= -0x96 || turnRadius >= 0x96) goto finish;
		current_surf_type = surfType;
		planindex = 2;
		byte_4392C = 0;
		if (turnRadius >= -0x6c && turnRadius <= 0x6c) goto finish;
		side = (polarAngle(locElemCrds.x + 0x400, locElemCrds.z + 0x400) & 0xff) * 0x12;
		localX = (short)(0x11 - (side >> 8));
		word_34A8C = 0x2a;
		word_34988 = -12;
		if (turnRadius < 0) wallindex = (short)(localX + 0x69);
		else wallindex = (short)(localX + 0x7b);
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
			wallindex = 0x64;
		} else if (localX != 0 && nextPositionElemCrds.x >= 0x78) {
			wallOrientationState = 0x200;
			wallindex = 0x65;
		}
		current_surf_type = surfType;
		if (locElemCrds.z < -0x14e) planindex = baseIndex;
		else if (locElemCrds.z >= 0x14e) planindex = baseIndex + 9;
		else {
			if (locElemCrds.z < -0xa8) { planindex = baseIndex + 1; localX = 0; }
			else if (locElemCrds.z < 0) { planindex = baseIndex + 3; localX = 1; }
			else if (locElemCrds.z < 0xa8) { planindex = baseIndex + 5; localX = 2; }
			else if (locElemCrds.z < 0x14e) { planindex = baseIndex + 7; localX = 3; }
			side = multiply_and_scale(cos_fast(step), locElemCrds.x) +
				multiply_and_scale(sin_fast(step), locElemCrds.z - bkRdEntr_triang_zAdjust[localX]);
			if (side > 0) ++planindex;
		}
		break;
	case 25:
		if (absLocalCrds.x > 0x78) goto finish;
		current_surf_type = surfType;
		planindex = 6;
		if (nextPositionElemCrds.x < 0x78) break;
		wallOrientationState = 0x200;
		wallindex = 0x65;
		break;
	case 26:
		turnRadius = (short)(polarRadius2D((short)(locElemCrds.x + 0x400), (short)(locElemCrds.z + 0x400)) - 0x600);
		if (turnRadius <= -0x78 || turnRadius >= 0x7e) goto finish;
		side = (polarAngle(locElemCrds.x + 0x400, locElemCrds.z + 0x400) & 0xff) * 0x12;
		localX = (short)(0x11 - (side >> 8));
		planindex = (short)(localX + 7);
		current_surf_type = surfType;
		if (turnRadius <= 0x66) goto finish;
		wallOrientationState = 0x200;
		wallindex = (short)(localX + 0x7b);
		byte_4392C = 0;
		break;
	case 27:
		if (locElemCrds.z < 0) {
			baseIndex = 0x33;
			effElemCrds.x = (short)-locElemCrds.x;
			effElemCrds.z = (short)-locElemCrds.z;
		} else {
			baseIndex = 0x2d;
			effElemCrds.x = locElemCrds.x;
			effElemCrds.z = locElemCrds.z;
		}
		if (effElemCrds.z > loopSurface_maxZ - 1) {
			if (effElemCrds.z > loopSurface_maxZ + 0x64) goto loop_base;
			localX = loopSurface_maxZ - 1;
		} else localX = effElemCrds.z;
		for (step = 0; loopSurface_ZBounds1[step] < localX; ++step) { }
		if (position->y - terrainHeight > 0x20c) {
			step = 5 - step;
			if (loopSurface_XBounds0[step] <= effElemCrds.x && loopSurface_XBounds1[step] + 0x190 >= effElemCrds.x) {
				if (loopSurface_XBounds1[step] >= effElemCrds.x || loopSurface_XBounds0[step] + 0x190 <= effElemCrds.x) {
					side = (long)(loopSurface_XBounds0[step] - loopSurface_XBounds1[step]) *
						(loopSurface_ZBounds0[step] - localX) /
						(loopSurface_ZBounds1[step] - loopSurface_ZBounds0[step]);
					if (loopSurface_XBounds0[step] + side >= effElemCrds.x || loopSurface_XBounds0[step] + side + 0x190 <= effElemCrds.x) break;
				}
				planindex = baseIndex + step;
				current_surf_type = surfType;
				byte_4392C = 0;
			}
			break;
		}
		if (step > 1 && position->y - terrainHeight < 0x64) goto loop_base;
		if (loopSurface_XBounds0[step] <= effElemCrds.x && loopSurface_XBounds1[step] + 0x190 >= effElemCrds.x) {
			if (loopSurface_XBounds1[step] >= effElemCrds.x || loopSurface_XBounds0[step] + 0x190 <= effElemCrds.x) {
				if (loopSurface_XBounds1[step] == loopSurface_XBounds0[step]) goto loop_base;
				side = (long)(loopSurface_XBounds0[step] - loopSurface_XBounds1[step]) *
					(loopSurface_ZBounds0[step] - localX) /
					(loopSurface_ZBounds1[step] - loopSurface_ZBounds0[step]);
				if (loopSurface_XBounds0[step] + side >= effElemCrds.x || loopSurface_XBounds0[step] + side + 0x190 <= effElemCrds.x) goto loop_base;
			}
			planindex = baseIndex + step;
			current_surf_type = surfType;
			byte_4392C = 0;
			break;
		}
loop_base:
		for (step = 0; effElemCrds.z > loopBase_ZBounds1[step]; ++step) { }
		localX = (short)(loopBae_InnXBounds0[step] +
			(long)(loopBase_InnXBounds1[step] - loopBae_InnXBounds0[step]) *
			(effElemCrds.z - loopBase_ZBounds0[step]) /
			(loopBase_ZBounds1[step] - loopBase_ZBounds0[step]));
		side = (short)(loopBase_OutXBounds0[step] +
			(long)(loopBase_OutXBounds1[step] - loopBase_OutXBounds0[step]) *
			(effElemCrds.z - loopBase_ZBounds0[step]) /
			(loopBase_ZBounds1[step] - loopBase_ZBounds0[step]));
		if (effElemCrds.x >= localX && effElemCrds.x <= side) current_surf_type = surfType;
		break;
	case 28:
		if (position->y - terrainHeight >= 0x90 ||
			nextPosition->y - terrainHeight >= 0x90) {
			if (absLocalCrds.x >= 0x10e) goto finish;
			current_surf_type = surfType;
			planindex = 0x85;
			break;
		}
		if (absLocalCrds.x < 0x78) current_surf_type = surfType;
		if (locElemCrds.x >= 0x78 && locElemCrds.x <= 0x10e) {
			word_34A8C = 0x90;
			if (nextPositionElemCrds.z <= -0x200) wallindex = 0x9a;
			else if (nextPositionElemCrds.z >= 0x200) wallindex = 0x99;
			else if (nextPositionElemCrds.x <= 0x78) wallindex = 0x98;
			else if (nextPositionElemCrds.x >= 0x10e) wallindex = 0x96;
			break;
		}
		if (locElemCrds.x <= -0x78 && locElemCrds.x >= -0x10e) {
			word_34A8C = 0x90;
			if (nextPositionElemCrds.z <= -0x200) wallindex = 0x9a;
			else if (nextPositionElemCrds.z >= 0x200) wallindex = 0x99;
			else if (nextPositionElemCrds.x >= -0x78) wallindex = 0x97;
			else if (nextPositionElemCrds.x <= -0x10e) wallindex = 0x95;
		}
		break;
	case 29:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) >= 0x73 &&
		    absLocalCrds.x <= 0xa4) {
			word_34A8C = 0x97;
			if (nextPositionElemCrds.x > 0) wallindex = 0x9f;
			else wallindex = 0xa0;
			break;
		}
		if (absLocalCrds.x >= 0x73 || position->y - terrainHeight >= 0xab) break;
		current_surf_type = surfType;
		if (absLocalCrds.x < 0x1f) {
			planindex = 0x46;
			break;
		}
		if (locElemCrds.x < -0x54) { planindex = 0x49; localX = -0x64; step = -5; }
		else if (locElemCrds.x < 0) { planindex = 0x47; localX = -0x39; step = -8; }
		else if (locElemCrds.x > 0x54) { planindex = 0x4d; localX = 0x64; step = 5; }
		else { planindex = 0x4b; localX = 0x39; step = 8; }
		side = multiply_and_scale(cos_fast(step), locElemCrds.x - localX) +
			multiply_and_scale(sin_fast(step), locElemCrds.z);
		if (side < 0) ++planindex;
		break;
	case 31:
		side = 1;
		goto pipe_body;
	case 30:
		side = 0;
	pipe_body:
		if ((nextPositionElemCrds.x < 0 ? -nextPositionElemCrds.x : nextPositionElemCrds.x) >= 0xa4 && absLocalCrds.x <= 0xa4) {
			word_34A8C = 0x97;
			if (nextPositionElemCrds.x > 0) wallindex = 0x9b;
			else wallindex = 0x9c;
			break;
		}
		if (absLocalCrds.x >= 0xa4) break;
		if (position->y - terrainHeight >= 0x109) break;
		if (absLocalCrds.x < 0x82) current_surf_type = surfType;
		if (position->y - terrainHeight > 0x97) localX = 1;
		else localX = 0;
		if (side && !localX && absLocalCrds.x <= 0x54 && absLocalCrds.z <= 0x4b) {
			planindex = 0x45;
			if (nextPositionElemCrds.z <= -0x4b) wallindex = 0x9d;
			else if (nextPositionElemCrds.z >= 0x4b) wallindex = 0x9e;
		} else if (position->y - terrainHeight > 0x58 && !localX) {
			if (locElemCrds.x < 0) planindex = 0x3c;
			else planindex = 0x42;
		} else if (absLocalCrds.x < 0x1f) {
			if (localX) planindex = 0x3f;
			else planindex = 0x39;
		} else if (locElemCrds.x < -0x54) {
			if (localX) planindex = 0x3d;
			else planindex = 0x3b;
		} else if (locElemCrds.x < 0) {
			if (localX) planindex = 0x3e;
			else planindex = 0x3a;
		} else if (locElemCrds.x > 0x54) {
			if (localX) planindex = 0x41;
			else planindex = 0x43;
		} else {
			if (localX) planindex = 0x40;
			else planindex = 0x44;
		}
		break;
	case 35:
		if (absLocalCrds.x >= 0x96) break;
		if (position->y - terrainHeight >= 0x109) break;
		current_surf_type = surfType;
		if (position->y - terrainHeight > 0x97) localX = 1;
		else localX = 0;
		side = 0;
		if (position->y - terrainHeight > 0x58 && !localX) {
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
			planindex = side + 0x39;
		if (planindex == 0 && absLocalCrds.z < 0x200) {
			wallindex = 0xb9;
			corkFlag = 1;
			word_34A8C = 0x75;
		}
		break;
	case 32:
		localX = (short)-locElemCrds.x;
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
		corkFlag = 1;
		if (locElemCrds.z < 0 && position->y - terrainHeight < 0x64 && localX > 0) {
			if (localX >= 0x278 || localX <= 0x188) break;
			current_surf_type = surfType;
			planindex = side;
			break;
		}
		if (locElemCrds.z > 0 && position->y - terrainHeight > 0x15e && localX < 0x2b4 && localX > 0x14c) {
			word_34A8C = 0x2a;
			word_34988 = -12;
			wallindex = (localX > 0x200 ? wallHi : wallIx) + 0x18;
			current_surf_type = surfType;
			planindex = side + 0x19;
			byte_4392C = 0;
			break;
		}
		turnRadius = polarRadius2D(localX, locElemCrds.z);
		if (turnRadius <= 0x14c || turnRadius >= 0x2b4) break;
		step = ((0x100 - polarAngle(localX, locElemCrds.z)) & 0x3ff) * 0x18 >> 10;
		planindex = side + step + 1;
		current_surf_type = surfType;
		byte_4392C = 0;
		word_34A8C = 0x2a;
		word_34988 = -12;
		if (turnRadius - 0x200 > 0x5a) wallindex = wallHi + step;
		else if (turnRadius - 0x200 < -0x5a) wallindex = wallIx + step;
		break;
	case 34:
		if (absLocalCrds.x < 0x78) current_surf_type = surfType;
		if (locElemCrds.x >= 0x17 && locElemCrds.x <= 0x61 && locElemCrds.z > -0x10f && locElemCrds.z < -0xf1) {
			word_34A8C = 0x2a;
			if (nextPositionElemCrds.z < -0x10f) wallindex = 0x91;
			else if (nextPositionElemCrds.z > -0xf1) wallindex = 0x92;
			else if (nextPositionElemCrds.x < 0x17) wallindex = 0x94;
			else if (nextPositionElemCrds.x > 0x61) wallindex = 0x93;
		} else if (locElemCrds.x <= -0x17 && locElemCrds.x >= -0x61 && locElemCrds.z < 0x10f && locElemCrds.z > 0xf1) {
			word_34A8C = 0x2a;
			if (nextPositionElemCrds.z > 0x10f) wallindex = 0x8d;
			else if (nextPositionElemCrds.z < 0xf1) wallindex = 0x8e;
			else if (nextPositionElemCrds.x > -0x17) wallindex = 0x8f;
			else if (nextPositionElemCrds.x < -0x61) wallindex = 0x90;
		}
		break;
	case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43:
	case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51:
	case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59:
	case 60: case 61: case 62: case 63: case 64:
		break;
	case 65:
		if (absLocalCrds.x > 0x96 || absLocalCrds.z > 0x96) goto finish;
		word_34A8C = 0x1a9;
		if (nextPositionElemCrds.z <= -0x96) wallindex = 0xa1;
		else if (nextPositionElemCrds.z >= 0x96) wallindex = 0xa2;
		else if (nextPositionElemCrds.x >= 0x96) wallindex = 0xa3;
		else if (nextPositionElemCrds.x <= -0x96) wallindex = 0xa4;
		break;
	case 66:
		if (locElemCrds.x < -0xc8 || locElemCrds.x > 0x104 || absLocalCrds.z > 0x50) goto finish;
		word_34A8C = 0xe6;
		if (nextPositionElemCrds.z <= -0x50) wallindex = 0xa5;
		else if (nextPositionElemCrds.z >= 0x50) wallindex = 0xa8;
		else if (nextPositionElemCrds.x <= -0xc8) wallindex = 0xa6;
		else if (nextPositionElemCrds.x >= 0x104) wallindex = 0xa7;
		break;
	case 67:
		if (absLocalCrds.x > 0xb4 || absLocalCrds.z > 0x64) goto finish;
		word_34A8C = 0xf8;
		if (nextPositionElemCrds.z <= -0x64) wallindex = 0xa9;
		else if (nextPositionElemCrds.z >= 0x64) wallindex = 0xac;
		else if (nextPositionElemCrds.x <= -0xb4) wallindex = 0xab;
		else if (nextPositionElemCrds.x >= 0xb4) wallindex = 0xaa;
		break;
	case 68:
		if (absLocalCrds.x > 0xc8 || absLocalCrds.z > 0xc8) goto finish;
		word_34A8C = 0x226;
		if (nextPositionElemCrds.z <= -0xc8) wallindex = 0xad;
		else if (nextPositionElemCrds.z >= 0xc8) wallindex = 0xae;
		else if (nextPositionElemCrds.x <= -0xc8) wallindex = 0xaf;
		else if (nextPositionElemCrds.x >= 0xc8) wallindex = 0xb0;
		break;
	case 69:
		if (absLocalCrds.x > 0x72 || absLocalCrds.z > 0x72) goto finish;
		word_34A8C = 0x1ef;
		if (nextPositionElemCrds.z <= -0x72) wallindex = 0xb4;
		else if (nextPositionElemCrds.z >= 0x72) wallindex = 0xb2;
		else if (nextPositionElemCrds.x <= -0x72) wallindex = 0xb1;
		else if (nextPositionElemCrds.x >= 0x72) wallindex = 0xb3;
		break;
	case 70:
		if (locElemCrds.x < -0xaa || locElemCrds.x > 0x104 || absLocalCrds.z > 0x6e) goto finish;
		word_34A8C = 0xe6;
		if (nextPositionElemCrds.z <= -0x6e) wallindex = 0xb5;
		else if (nextPositionElemCrds.z >= 0x6e) wallindex = 0xb8;
		else if (nextPositionElemCrds.x <= -0xaa) wallindex = 0xb7;
		else if (nextPositionElemCrds.x >= 0x104) wallindex = 0xb6;
		break;
	case 71: case 72: case 73: case 74:
		break;
	default:
		break;
	}

finish:
	if (cellTerr >= 7) {
		locElemCrds.x = (short)(position->x - trackcenterpos2[trackCol]);
		locElemCrds.z = (short)(position->z - terraincenterpos[trkR]);
		switch (cellTerr) {
		case 7: case 11: case 15:
			elementAngle = 0;
			break;
		case 8: case 12: case 16:
			elementAngle = 0x300;
			localX = locElemCrds.x;
			locElemCrds.x = locElemCrds.z;
			locElemCrds.z = (short)-localX;
			break;
		case 9: case 13: case 17:
			elementAngle = 0x200;
			locElemCrds.z = (short)-locElemCrds.z;
			locElemCrds.x = (short)-locElemCrds.x;
			break;
		case 10: case 14: case 18:
			elementAngle = 0x100;
			localX = locElemCrds.x;
			locElemCrds.x = (short)-locElemCrds.z;
			locElemCrds.z = localX;
			break;
		}
		switch (cellTerr) {
		case 7: case 8: case 9: case 10:
			if (planindex == 0) planindex = 3;
			break;
		case 11: case 12: case 13: case 14:
			side = multiply_and_scale(cos_fast(0xff80), locElemCrds.x) +
				multiply_and_scale(sin_fast(0xff80), locElemCrds.z);
			if (side < 0) planindex = 4;
			break;
		case 15: case 16: case 17: case 18:
			side = multiply_and_scale(cos_fast(0xff80), locElemCrds.x) +
				multiply_and_scale(sin_fast(0xff80), locElemCrds.z);
			if (side > 0) planindex = 5;
			else terrainHeight = 0x1c2;
			break;
		}
	}
select_plan:
	if (planindex > 0) {
		planindex <<= 2;
		switch (elementAngle) {
		case 0x300: ++planindex; break;
		case 0x200: planindex += 2; break;
		case 0x100: planindex += 3; break;
		}
	}
	current_planptr = planptr + planindex * 0x22;
	if (current_surf_type == 4)
		terrainHeight += ((position->z ^ position->x) >> 8) & 1;
	else
		terrainHeight += 2;

position_wall:
	if (wallindex < 0) return;
	wallOrientation = (short)((-wallptr[wallindex].orientation + elementAngle + wallOrientationState) & 0x3ff);
	switch (elementAngle) {
	case 0:
		wallStartX = wallptr[wallindex].x;
		wallStartZ = wallptr[wallindex].z;
		break;
	case 0x300:
		wallStartX = -wallptr[wallindex].z;
		wallStartZ = wallptr[wallindex].x;
		break;
	case 0x200:
		wallStartX = -wallptr[wallindex].x;
		wallStartZ = -wallptr[wallindex].z;
		break;
	case 0x100:
		wallStartX = wallptr[wallindex].z;
		wallStartZ = -wallptr[wallindex].x;
		break;
	}
	wallStartX += elem_xCenter;
	wallStartZ += elem_zCenter;
}

int bto_auxiliary1(int column, int row, struct VECTOR *vertices) {
    unsigned char tileElement;
    int centerRow;
    int height;
    int rotationY;
    int centerX;
    struct VECTOR *templatePoints;
    register int point;
    register int numPoints;
    unsigned char terrainIndex;

    tileElement = td14_elem_map_main[column + trackrows[row]];
    if (tileElement == 0) return 0;

    centerX = trackcenterpos2[column];
    centerRow = trackcenterpos[row];
    if (tileElement >= 0xfd) {
        switch (tileElement) {
        case 0xfd:
            tileElement = td14_elem_map_main[column + trackrows[row - 1] - 1];
            if (trkObjectList[tileElement].multiTile & 1) centerRow = trackpos[row + 1];
            if (trkObjectList[tileElement].multiTile & 2) centerX = trackpos2[column];
            break;
        case 0xfe:
            tileElement = td14_elem_map_main[column + trackrows[row - 1]];
            if (trkObjectList[tileElement].multiTile & 1) centerRow = trackpos[row + 1];
            if (trkObjectList[tileElement].multiTile & 2) centerX = trackpos2[column + 1];
            break;
        case 0xff:
            tileElement = td14_elem_map_main[column + trackrows[row] - 1];
            if (trkObjectList[tileElement].multiTile & 1) centerRow = trackpos[row];
            if (trkObjectList[tileElement].multiTile & 2) centerX = trackpos2[column];
            break;
        }
    } else if (trkObjectList[tileElement].multiTile != 0) {
        if (trkObjectList[tileElement].multiTile & 1) centerRow = trackpos[row];
        if (trkObjectList[tileElement].multiTile & 2) centerX = trackpos2[column + 1];
    }

    numPoints = 0;
    switch (trkObjectList[tileElement].physicalModel) {
    case 0x23:
        numPoints = 2;
        templatePoints = unk_3E676;
        break;
    case 0x20:
        numPoints = 2;
        templatePoints = unk_3E682;
        break;
    case 0x21:
        numPoints = 2;
        templatePoints = unk_3E68E;
        break;
    case 0x22:
        numPoints = 4;
        templatePoints = unk_3E69A;
        break;
    case 0x0b:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
        numPoints = 1;
        templatePoints = unk_3E640;
        break;
    case 0x12:
        numPoints = 8;
        templatePoints = unk_3E646;
        break;
    }
    if (numPoints == 0) return 0;

    terrainIndex = td15_terr_map_main[column + terrainrows[row]];
    if (terrainIndex == 6) height = hillHeightConsts[1];
    else height = 0;
    rotationY = trkObjectList[tileElement].rotation;

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

int shape3d_load_all(void)
{
    register int i;

    game1ptr = 0;
    game2ptr = 0;
    if (mmgr_get_res_ofs_diff_scaled() < 65000L) {
        return 1;
    }
    game1ptr = file_load_3dres(aGame1);
    game2ptr = file_load_3dres(aGame2);
    for (i = 0; i < 0x74; ++i) {
        curshapeptr = locate_shape_nofatal(game1ptr, aBarn[i]);
        if (curshapeptr == 0) {
            curshapeptr = locate_shape_fatal(game2ptr, aBarn[i]);
        }
        shape3d_init_shape(curshapeptr, &game3dshapes[i]);
    }
    return 0;
}

void shape3d_free_all(void)
{
    if (game1ptr != 0) {
        mmgr_free(game1ptr);
    }
    if (game2ptr != 0) {
        mmgr_free(game2ptr);
    }
}

void shape3d_load_car_shapes(char arg_playercarid[], char arg_opponentcarid[]) {
	long copyOffset;
	long sizeBytes;
	register int loopIndex;
	struct VECTOR far *vertices;
	char firstOpponent;
	aStxxx[2] = arg_playercarid[0];
	aStxxx[3] = arg_playercarid[1];
	aStxxx[4] = arg_playercarid[2];
	aStxxx[5] = arg_playercarid[3];
	carresptr = file_load_3dres(aStxxx);
	shape3d_init_shape(locate_shape_fatal(carresptr, aCar0), &game3dshapes[124]);
	shape3d_init_shape(locate_shape_fatal(carresptr, aCar1), &game3dshapes[126]);

	vertices = &(game3dshapes[126].shape3d_verts[8]);
	word_32CBA.z = vertices[0].z;
	word_32CBA.x = (vertices[3].x + vertices[0].x)>> 1;
	word_32CC0.z = vertices[6].z;
	word_32CC0.x = (vertices[6].x + vertices[9].x) >> 1;
	
	for (loopIndex = 0; loopIndex < 6; loopIndex++) {
		carshapevecs[loopIndex].x = word_32CBA.x - vertices[loopIndex + 0].x;
		carshapevecs[loopIndex].z = word_32CBA.z - vertices[loopIndex + 0].z;
		carshapevecs[loopIndex].y = vertices[loopIndex + 0].y;
		word_3441E[loopIndex].x = word_32CC0.x - vertices[loopIndex + 6].x;
		word_3441E[loopIndex].z = word_32CC0.z - vertices[loopIndex + 6].z;
		word_3441E[loopIndex].y = vertices[loopIndex + 6].y;
		data_34442[loopIndex] = vertices[loopIndex + 12];
		data_34466[loopIndex] = vertices[loopIndex + 18];
	}

	for (loopIndex = 0; loopIndex < 5; loopIndex++) {
		word_443E8[loopIndex] = 0;
	}

	shape3d_init_shape(locate_shape_fatal(carresptr, aCar2), &game3dshapes[128]);
	shape3d_init_shape(locate_shape_fatal(carresptr, aExp0_0), &game3dshapes[116]);
	shape3d_init_shape(locate_shape_fatal(carresptr, aExp1_0), &game3dshapes[117]);
	shape3d_init_shape(locate_shape_fatal(carresptr, aExp2_0), &game3dshapes[118]);
	shape3d_init_shape(locate_shape_fatal(carresptr, aExp3_0), &game3dshapes[119]);

	firstOpponent = arg_opponentcarid[0];
	if (firstOpponent != -1) {
		if (arg_playercarid[0] == firstOpponent && arg_playercarid[1] == arg_opponentcarid[1] &&
			arg_playercarid[2] == arg_opponentcarid[2] && arg_playercarid[3] == arg_opponentcarid[3])
		{
			sizeBytes = mmgr_get_chunk_size_bytes(carresptr);
			car2resptr = mmgr_alloc_resbytes(aCar2_0, sizeBytes);
			
			for (copyOffset = 0; copyOffset < sizeBytes; copyOffset++) {
				car2resptr[copyOffset] = carresptr[copyOffset];
			}
		} else {
			aStxxx[2] = arg_opponentcarid[0];
			aStxxx[3] = arg_opponentcarid[1];
			aStxxx[4] = arg_opponentcarid[2];
			aStxxx[5] = arg_opponentcarid[3];
			car2resptr = file_load_3dres(aStxxx);
		}

		shape3d_init_shape(locate_shape_fatal(car2resptr, aCar0_0), &game3dshapes[125]);
		shape3d_init_shape(locate_shape_fatal(car2resptr, aCar1_0), &game3dshapes[127]);

		vertices = &(game3dshapes[127].shape3d_verts[8]);
		word_32D04.z = vertices[0].z;
		word_32D04.x = (vertices[3].x + vertices[0].x)>> 1;
		word_32D0A.z = vertices[6].z;
		word_32D0A.x = (vertices[6].x + vertices[9].x) >> 1;

		for (loopIndex = 0; loopIndex < 6; loopIndex++) {
			word_348F4[loopIndex].x = word_32D04.x - vertices[loopIndex + 0].x;
			word_348F4[loopIndex].z = word_32D04.z - vertices[loopIndex + 0].z;
			word_348F4[loopIndex].y = vertices[loopIndex + 0].y;
			word_34918[loopIndex].x = word_32D0A.x - vertices[loopIndex + 6].x;
			word_34918[loopIndex].z = word_32D0A.z - vertices[loopIndex + 6].z;
			word_34918[loopIndex].y = vertices[loopIndex + 6].y;
			data_3493C[loopIndex] = vertices[loopIndex + 12];
			data_34960[loopIndex] = vertices[loopIndex + 18];
		}
		for (loopIndex = 0; loopIndex < 5; loopIndex++) {
			word_4448A[loopIndex] = 0;
		}
		shape3d_init_shape(locate_shape_fatal(car2resptr, aCar2_1), &game3dshapes[129]);
		shape3d_init_shape(locate_shape_fatal(car2resptr, aExp0_1), &game3dshapes[120]);
		shape3d_init_shape(locate_shape_fatal(car2resptr, aExp1_1), &game3dshapes[121]);
		shape3d_init_shape(locate_shape_fatal(car2resptr, aExp2_1), &game3dshapes[122]);
		shape3d_init_shape(locate_shape_fatal(car2resptr, aExp3_1), &game3dshapes[123]);
	} else {
		car2resptr = 0;
	}
}

void shape3d_free_car_shapes(void)
{
    if (car2resptr != 0) {
        sub_204AE(&game3dshapes[127].shape3d_verts[8], 0, unk_3E710, word_4448A, word_348F4, (short *)&word_32D04);
        mmgr_release(car2resptr);
    }
    sub_204AE(&game3dshapes[126].shape3d_verts[8], 0, unk_3E710, word_443E8, carshapevecs, (short *)&word_32CBA);
    mmgr_free(carresptr);
}

void sub_204AE(struct VECTOR far *out, int angle,
              short *base, short *last_angle_and_y,
              struct VECTOR *source, short *origin)
{
    int y;
    register int i;
    register int j;
    int end;
    int span_start;
    int sine;
    int cosine;

    if (last_angle_and_y[4] != angle) {
        sine = sin_fast(angle >> 1);
        cosine = cos_fast(angle >> 1);

        for (i = 0; i < 6; ++i) {
            out[i].x = multiply_and_scale(source[i].z, sine)
                     + origin[0]
                     + multiply_and_scale(source[i].x, cosine);
            out[i].z = multiply_and_scale(source[i].x, sine)
                     + origin[2]
                     + multiply_and_scale(source[i].z, cosine);
        }

        for (i = 6; i < 12; ++i) {
            out[i].x = multiply_and_scale(source[i].z, sine)
                     + origin[3]
                     + multiply_and_scale(source[i].x, cosine);
            out[i].z = multiply_and_scale(source[i].x, sine)
                     + origin[5]
                     + multiply_and_scale(source[i].z, cosine);
        }

        last_angle_and_y[4] = angle;
    }

    for (j = 0; j < 4; ++j) {
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

static unsigned char byte_3E71E[6] = { 0, 0, 1, 0, 1, 0 };
static unsigned char byte_3E724[6] = { 0, 1, 0, 0, 1, 0 };
static unsigned char terrConnDataEtoW[19] = { 0, 0, 0, 0, 0, 0, 1, 2, 1, 3, 0, 2, 3, 0, 0, 1, 1, 3, 2 };
static unsigned char terrConnDataWtoE[19] = { 0, 0, 0, 0, 0, 0, 1, 2, 0, 3, 1, 0, 0, 3, 2, 2, 3, 1, 1 };
static unsigned char terrConnDataNtoS[19] = { 0, 0, 0, 0, 0, 0, 1, 1, 5, 0, 4, 5, 0, 0, 4, 1, 5, 4, 1 };
static unsigned char terrConnDataStoN[19] = { 0, 0, 0, 0, 0, 0, 1, 0, 5, 1, 4, 0, 5, 4, 0, 5, 1, 1, 4 };

int track_setup(void)
{
  char matches;
  unsigned char entryPt;
  unsigned char runway;
  unsigned char runLength;
  char startCount;
  signed char cur_col;
  int xsave;
  struct TrackNode far *qnode;
  struct TRKOBJINFO *objInfo;
  signed char oldRow;
  unsigned char connStatusOf[0x385];
  char prev_exit_type;
  signed char oldX;
  unsigned char object;
  int elem_idx;
  int sampleIdx;
  int angle;
  unsigned char prevBlock;
  signed char y;
  unsigned char on_path[0x385];
  register int i;
  char err;
  struct TRKOBJINFO *blk;
  int prev_path;
  unsigned char qCount;
  unsigned char exit;
  unsigned char sub;
  signed char prev_reversed_dir;
  signed char connStat;
  register int j;
  struct TrackNode far *alloc;
  struct VECTOR far *camData;
  struct VECTOR cur_pos;
  unsigned char part_of[0x385];
  unsigned char tileTerr;
  char arrow;
  struct TrackNode far *queuePtr;
  signed char conn;
  char loop_done;
  unsigned char prev_elem;
  alloc = mmgr_alloc_resbytes("tcomp", 0x380L);
  if (alloc == 0)
    return 2;
  queuePtr = alloc;
  startCount = 0;
  runway = 0;
  track_pieces_counter = 0;
  for (i = 0; i < 0x385; i++)
    trackdata19[i] = 0xff;

  for (y = 0; y < 30; y++)
  {
    prev_exit_type = 'c';
    for (cur_col = 0; cur_col < 30; cur_col++)
    {
      tileTerr = td15_terr_map_main[terrainrows[y] + cur_col];
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
      tileTerr = td15_terr_map_main[terrainrows[y] + cur_col];
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
      object = td14_elem_map_main[trackrows[y] + cur_col];
      if (object >= 0xfd)
        object = 0;
      if (object >= 0xb6)
      {
        object = 4;
        td14_elem_map_main[trackrows[y] + cur_col] = 4;
      }
      switch (object)
      {
        default:
          goto next_col;

        case 1:

        case 0x86:

        case 0x93:
          track_angle = 0;
          break;

        case 0x87:

        case 0x94:

        case 0xb3:
          track_angle = 0x200;
          break;

        case 0x88:

        case 0x95:

        case 0xb4:
          track_angle = 0x100;
          break;

        case 0x89:

        case 0x96:

        case 0xb5:
          track_angle = 0x300;
          break;

      }

      if (startCount != 0)
      {
        err = 3;
        goto error;
      }
      startcol2 = cur_col;
      startrow2 = y;
      if ((tileTerr = td15_terr_map_main[terrainrows[y] + cur_col]) == 6)
        hillFlag = 1;
      else
        hillFlag = 0;
      startCount++;
      next_col: ;
    }

  }

  if (startCount == 0)
  {
    err = 1;
    goto error;
  }
  track_pieces_counter = 0;
  qCount = 0;
  byte_45635 = 0;
  byte_4616E = 0;
  runLength = 0;
  loop_done = 0;
  for (i = 0; i < 0x385; i++)
  {
    on_path[i] = 0;
    td01_track_file_cpy[i] = 0xffff;
    td02_penalty_related[i] = 0xffff;
  }

  cur_col = startcol2;
  y = startrow2;
  angle = track_angle;
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
    object = td14_elem_map_main[trackrows[y] + cur_col];
    tileTerr = td15_terr_map_main[terrainrows[y] + cur_col];
    if (object != 0 && tileTerr != 0 && tileTerr >= 7 && tileTerr < 11)
      object = subst_hillroad_track(tileTerr, object);
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

      object = td14_elem_map_main[trackrows[y] + cur_col];
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
    objInfo = trkObjectList[object].info;
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
        if (conn >= 0 && on_path[trackrows[y] + cur_col] != 0)
        {
          for (j = 0; j < track_pieces_counter; j++)
          {
            if (td21_col_from_path[j] == cur_col && td22_row_from_path[j] == y && part_of[j] == ((unsigned char) i))
            {
              if (connStatusOf[j] != conn)
              {
                err = 5;
                goto error;
              }
              conn = -1;
              if (td01_track_file_cpy[prev_path] == 0xffff)
                td01_track_file_cpy[prev_path] = j;
              else
                td02_penalty_related[prev_path] = j;
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

  byte_45D90 = startcol2;
  byte_45E16 = startrow2;
  i = track_pieces_counter / 3;
  if (i > 0x40)
    i = 0x40;
  byte_4616E = i;
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
  on_path[trackrows[y] + cur_col] = 1;
  part_of[track_pieces_counter] = sub;
  connStatusOf[track_pieces_counter] = connStat;
  if (prev_path != (-1))
  {
    if (td01_track_file_cpy[prev_path] == 0xffff)
      td01_track_file_cpy[prev_path] = track_pieces_counter;
    else
      td02_penalty_related[prev_path] = track_pieces_counter;
  }
  prev_path = track_pieces_counter;
  td21_col_from_path[track_pieces_counter] = cur_col;
  td22_row_from_path[track_pieces_counter] = y;
  trackdata18[track_pieces_counter] = (connStat << 4) + sub;
  td17_trk_elem_ordered[track_pieces_counter] = object;
  objInfo = trkObjectList[object].info;
  blk = objInfo + sub;
  arrow = blk->opponent3;
  if (arrow == 0)
  {
    runLength++;
    goto next_piece;
  }
  if (arrow != (-1) && runLength > 3 && byte_45635 != 0x30)
  {
    objInfo = trkObjectList[object].info;
    blk = objInfo + sub;
    arrow = blk->opponent3;
    objInfo = trkObjectList[prev_elem].info;
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
      arrow = byte_3E724[arrow];
    else
      arrow = byte_3E71E[arrow];
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
      td08_direction_related[byte_45635] = angle ^ 0x200;
    else
      td08_direction_related[byte_45635] = angle;
    trackdata23[byte_45635] = arrow;
    if (td15_terr_map_main[terrainrows[oldRow] + oldX] == 6)
      cur_pos.y += 0x1c2;
    td10_track_check_rel[byte_45635].y = cur_pos.y;
    td10_track_check_rel[byte_45635].z = ((trkObjectList[prev_elem].multiTile & 1) ? (trackpos[oldRow]) : (trackcenterpos[oldRow])) + cur_pos.z;
    td10_track_check_rel[byte_45635].x = ((trkObjectList[prev_elem].multiTile & 2) ? (trackpos2[oldX + 1]) : (trackcenterpos2[oldX])) + cur_pos.x;
    trackdata19[trackrows[oldRow] + oldX] = byte_45635;
    byte_45635++;
  }
  runLength = 0;
  next_piece:
  if ((++track_pieces_counter) == 0x385)
  {
    err = 6;
    goto error;
  }

  objInfo = trkObjectList[object].info;
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
  for (i = 0; i < byte_4616E; i++)
  {
    sampleIdx = track_pieces_counter * i / byte_4616E;
    cur_col = td21_col_from_path[sampleIdx];
    y = td22_row_from_path[sampleIdx];
    if (part_of[terrainrows[y] + cur_col] == 0)
    {
      part_of[terrainrows[y] + cur_col] = 1;
      elem_idx = td17_trk_elem_ordered[sampleIdx];
      sub = trackdata18[sampleIdx] & 0x0f;
      conn = trackdata18[sampleIdx] & 0x10;
      blk = trkObjectList[elem_idx].info;
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

      if (td15_terr_map_main[terrainrows[y] + cur_col] == 6)
        trackdata7[j] = 0x1c2;
      else
        trackdata7[j] = 0;
      trackdata6[j] = 0;
      trackdata9[j].y = trackdata7[j] + cur_pos.y;
      if (trkObjectList[elem_idx].multiTile & 1)
        trackdata9[j].z = trackpos[y] + cur_pos.z;
      else
        trackdata9[j].z = trackcenterpos[y] + cur_pos.z;
      trackdata9[j].x = ((trkObjectList[elem_idx].multiTile & 2) ? (trackpos2[cur_col + 1]) : (trackcenterpos2[cur_col])) + cur_pos.x;
      j++;
    }
  }

  byte_4616E = j;
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
  byte_45D90 = cur_col;
  byte_45E16 = y;
  release:
  mmgr_release((char far *) alloc);

  return err;
}

void load_opponent_data(void)
{
    unsigned long savedSums[256];
    short savedIndices[256];
    void far *res;
    short dep;
    short count;
    unsigned long minSum;
    char far *pathData;
    short f;
    short rt[256];
    unsigned long sum;
    unsigned char far *tbl;
    short el;
    short fork;
    short valid;
    register short i;
    short sequence[901];
    register short visited;

    aOpp1[3] = (char)(gameconfig.game_opponenttype + '0');
    res = file_load_resfile(aOpp1);
    copy_string(unk_46464, locate_text_res(res, "nam"));
    pathData = locate_shape_alt(res, "path");
    tbl = (unsigned char far *)locate_shape_alt(res, "sped");
    for (i = 0; i < 16; i++)
        oppnentSped[i] = tbl[i];

    minSum = 0x000f423fUL;
    count = 0;
    sum = 0;
    dep = 0;
    i = 0;
    for (;;) {
        f = 0;
        el = td01_track_file_cpy[i];
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
        sum += (unsigned char)tbl[(unsigned char)td17_trk_elem_ordered[i]] + 1;
        if (f != 0) {
            if (valid != 0 && sum < minSum) {
                sequence[count++] = 0;
                minSum = sum;
                for (visited = 0; visited < count; visited++)
                    trackdata3[visited] = sequence[visited];
                trackdata3[count] = 0;
                trackdata3[count + 1] = 1;
            }
            if (dep == 0) {
                unload_resource(res);
                return;
            }
            dep--;
            i = savedIndices[dep];
            count = rt[dep];
            sum = savedSums[dep];
        } else {
            fork = td02_penalty_related[i];
            if (fork != -1) {
                savedIndices[dep] = fork;
                rt[dep] = count;
                savedSums[dep++] = sum;
            }
            i = el;
        }
    }
}

unsigned char subst_hillroad_track(unsigned char a, unsigned char b)
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
