#define FAR far
#define NEAR near
#define HUGE huge
#define WORLD_COORDINATE_SHIFT 6
#define CAR_SPEED_Q8_SHIFT 8
#define SPEEDOMETER_HUNDREDS_BASE 100
#define SPEEDOMETER_TWO_HUNDREDS_BASE 200
#define PLATFORM_SCREEN_WIDTH_PIXELS 320
#define PLATFORM_SCREEN_HEIGHT_PIXELS 200
#define BIOS_KEY_F1 0x3B00
#define BIOS_KEY_F2 0x3C00
#define BIOS_KEY_F3 0x3D00
#define BIOS_KEY_F4 0x3E00
#define KEY_SCAN_UP 0x4800
#define KEY_SCAN_LEFT 0x4B00
#define KEY_SCAN_RIGHT 0x4D00
#define KEY_SCAN_DOWN 0x5000
#define KEY_ASCII_ESCAPE 27
#define KEY_ASCII_UPPER_C 0x43
#define KEY_ASCII_UPPER_D 0x44
#define KEY_ASCII_UPPER_H 0x48
#define KEY_ASCII_UPPER_M 0x4D
#define KEY_ASCII_UPPER_R 0x52
#define KEY_ASCII_LOWER_C 0x63
#define KEY_ASCII_LOWER_D 0x64
#define KEY_ASCII_LOWER_H 0x68
#define KEY_ASCII_LOWER_M 0x6D
#define KEY_ASCII_LOWER_R 0x72
#define KEY_ASCII_LOWER_T 0x74
#include "types.h"
/* Scratch TU. Header declarations are semantic leads from Restunts; no preprocessor directives. */ /* PORT: plain char signedness follows the pinned MSC target. */

#include "math3d.h"

#include "matrix.h"
;

/* Local signed radius view; the shared math API returns an unsigned word. */
#include "radius.h"

#include "car.h"

#include "gamestat.h"

/* Local view: SIMD; struct[22].type.of is unsigned byte; another unit uses signed byte. */
struct SIMD {
	I8 num_gears;
	I8 simd_unk;
	I16S car_mass;
	I16S braking_eff;
	I16S idle_rpm;
	I16S downshift_rpm;
	I16S upshift_rpm;
	I16S max_rpm;
	U16S  gear_ratios[7];
	struct POINT2D knob_points[7];
	I16S aero_resistance;
	I8 idle_torque;
	I8 torque_curve[104];
	I8 field_A3;
	I16S grip;
	I16S field_A6[7];
	I16S sliding;
	I16S surface_grip[4];
	I8 simd_unk3[10];
	struct POINT2D collide_points[2];
	I16S car_height;
	struct VECTOR wheel_coords[4];
	U8  steeringdots[62];
	struct POINT2D spdcenter;
	I16S spdnumpoints;
	U8  spdpoints[208];
	struct POINT2D revcenter;
	I16S revnumpoints;
	U8  revpoints[256];
	I16S far* aerorestable;
};

/* Local view: TRKOBJINFO; struct.count is 12; another unit uses 11. */
struct TRKOBJINFO {
	I8  si_noOfBlocks;      // How many shapeInfo pieces compose the element. Arbitrary for the first piece, 0 for the following ones.
	I8  si_entryPoint;      // Connectivity of the track element regarding tiles.
	I8  si_exitPoint;
	I8  si_entryType;        // Connectivity of the track element regarding element types.
	I8  si_exitType;
	I8  si_arrowType;        // Type of the element for determining penalty-arrow behaviour.
	I16S si_arrowOrient;      // Orientation angle for penalty-arrow purposes
	I16S* si_cameraDataOffset; // offset (0003B770)
	I8  si_opp1;             //Appears to affect how the opponent AI approaches an element.
	I8  si_opp2;
	I8  si_opp3;
	I8  si_oppSpedCode;
};

#include "trkobj.h"
#include "video.h"

short vidflg4_is1;
U8  g_videoflg5;

unsigned char timeraud;
unsigned char slomodiv8;
#include "timer.h"
U16S  elaptm1;
unsigned short tmr2;
unsigned char sigframe;
unsigned char g_rpl_init;
#include "game.h"
char gm_playmode; // 0 = playing, 1 = paused, 2 = replay

 // current frame?
 // fps * 30
 // 100 / fps
#include "ui.h"

struct RECTANGLE boundglassrect;
short bitmapdash;

char replaybar_toggle;
char inrepflg;

char g_rplmodui;
char gm_saved_rpl_mode;
char numid;
char g_rplbfask;
char on_off_dash;
char cam_idg;

int replayrst;
char popupact;
#include "mousebuf.h"

void far* dasm_shp_7;

char dashbtogglesaved;
char g_replaybarcpytgl;
char is_in_rplcopy;

char opp_follow_flag_backup;
int roofbmphgt_saved;
char mode_flag;
char g_rplybarenable;
int dashbmpy_copy;
int rplbarabovehgt;
char g_simprect;
char g_viewinx[2];

int dasty;
struct SHAPE2D far *g_dastbmpbuf;
int dashbmy9;
int rfy5;
#include "input.h"

/* Local view: resbuftext; type is signed byte; another unit uses array[80]. */
extern I8 resbuftext;

 //trackdata1;
 //trackdata2;
#include "track.h"

 //trackdata4;
 //trackdata5;
 //trackdata8;

/* Local view: td10checkptr; pointer is signed word; another unit uses struct VECTOR. */
extern I16 far* trksign;// trackdata10;
 //trackdata11;

char far* td13_replay_hdr; //trackdata13;
 //trackdata14;
 //trackdata15;
char far* g_tdreplay16buf; //trackdata16;
 //trackdata17;

 //trackdata20;
 //trackdata21;
 //trackdata22;
 // indexes into trkObjectList
I8 didrace;

unsigned short frm_rate2;
#include "shapes3d.h"
#include "audio.h"

 // = 'e'
/* Local view: material_clrlist_ptr; renderer retains its excluded MATERIALCLRLIST pointer. */
extern I16* material_clrlist_ptr;
/* Local view: messagebox; renderer retains its excluded unsigned return prototype. */
extern I16 far messagebox();
#include "files.h"

void far *eng1resourceptr;
void far *engdata;

int g_audio_frms_ix;

int snd_tick_clock;
void far *fntled_res;
void far *sdgresourcehandle;
#include "memory.h"

#include "viewstat.h"
#include "dlgcolor.h"
#include "trig.h"

struct ENGINESOUND player_engine_profile = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
struct ENGINESOUND eng2 = { 500, 10000, 9000, 0,
	{ "ENGI", "ENGI", "STAR", "STOP", "BLOW", "CRAS", "SKID", "SKI2", "BUMP", "SCRA" } };
I8 replay_axis_magnitude[34] = { 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
	60, 64, 68, 72, 76, 84, 90, 98, 106, 114, 121, 127, 127, 127 };

/* run_game: reconstructed from the target listing (s005c) */

/* Runs the game and replay interface. Params: none. Returns: none. State: reads and updates game, menu, replay and camera state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
void run_game(void) {
	I16 dialog_values[2];
	I16 replay_key_code, replaybar_height, last_game_frame;
	struct RECTANGLE temp_rect;
	I16 previous_roof;
	register I16 dialog_result;

	last_game_frame = -1;
	boundglassrect.left = 0;
	boundglassrect.right = PLATFORM_SCREEN_WIDTH_PIXELS;
	previous_roof = -1;
	bitmapdash = -1;
	runrndx = myrandom() << 3;
	replaybar_toggle = 1;
	inrepflg = 0;
	if (menutimeout != 0) {
		cammd++;
		if (cammd == CAMERA_MODE_COUNT) {
			cammd = CAMERA_MODE_COCKPIT;
		}
		gm_playmode = 2;
		/* PLATFORM(file): load replay data from a file. */ if (loadreplay(0, "default") != 0) {
			return;
		}
		compiletrack();
	} else if (globalgamesettings.game_recordedframes == 0) {
		cammd = CAMERA_MODE_COCKPIT;
		gm_playmode = 1;
	} else {
		cammd = CAMERA_MODE_COCKPIT;
		gm_playmode = 2;
		inrepflg = 1;
	}

	if (loadgamefiles() != 0) {
		removegamefiles();
		/* PLATFORM(input_joy): collect menu-text joystick input. */
		/* PLATFORM(input_kb): collect menu-text key input. */
		/* PLATFORM(input_mouse): collect menu-text pointer input. */
		/* PLATFORM(timer): wait for menu text input using timer ticks. */
		/* PLATFORM(video): draw the menu resource text. */
		memerr();
	} else {
		kbormouse = 0;
		g_rplmodui = 0;
		sigframe = 1;
		gameintinit();
		gm_saved_rpl_mode = -1;
		numid = 0;
		cam_idg = 0;
		g_rplbfask = 0;
		on_off_dash = 0;

		if (menutimeout != 0) {
			rate_frame = globalgamesettings.game_framespersec;
			initsim(-1);
		} else if (inrepflg == 0) {
			cammd = CAMERA_MODE_COCKPIT;
			on_off_dash = 1;
			pen_flag_count = 0;
			rate_frame = frm_rate2;
			globalgamesettings.game_framespersec = frm_rate2;
			initsim(-1);
			replayrst = 0;
			popupact = 0;
			g_rpl_init = 1;
			/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ initmousedrive(mousedrive);
			gm_playmode = 1;
			core.playerstate.car_posWorld1.lx += (I32)fixedmult(intsin(st_hdg), -240) << 6;
			core.playerstate.car_posWorld1.lz += (I32)fixedmult(intcos(st_hdg), -240) << 6;
			core.playerstate.car_posWorld1.ly += 0x580;
			didrace = 1;
		} else {
			cammd = CAMERA_MODE_COCKPIT;
			gm_playmode = 2;
			g_sgateopn = 500;
			rate_frame = globalgamesettings.game_framespersec;
			gotosim(0);
			gotosim(globalgamesettings.game_recordedframes);
			while (globalgamesettings.game_recordedframes != core.game_frame) {
				/* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ if (hurrykey(1) != 27)
					movesim();
				else
					break;
			}
			tmr2 = globalgamesettings.game_recordedframes;
		}

		for (;;) {
			while (core.game_frame != tmr2) {
				if ((mousedrive != 0 || joystick_enabled != 0) && gm_playmode == 0) {
					mousesteer();
				}
				movesim();
			}

			if (gm_playmode == 0 && sigframe == 0 && core.game_inputmode != 0) {
				if (last_game_frame == core.game_frame)
					continue;
				last_game_frame = core.game_frame;
			}

			if (core.game_inputmode == 0 && gm_playmode == 0) {
				tmr2 = 0;
				globalgamesettings.game_recordedframes = 0;
				core.game_frame = 0;
			}

			if (statemgmtcpy != slow_video_mode_state) {
				statemgmtcpy = slow_video_mode_state;
				initdrawworld();
			}

			if (g_rplbfask != 0) {
				/* PLATFORM(input_kb): save the active input source. */ /* PLATFORM(input_mouse): save mouse input mode. */ pushmouse();
				/* PLATFORM(audio): update the active audio mode. */ audio_pause_playback();
				/* PLATFORM(input_joy): collect dialog joystick input. */
				/* PLATFORM(timer): wait for dialog input using timer ticks. */
				/* PLATFORM(video): render a modal dialog. */
				dialog_result = messagebox(2, 1, getresl(gamerptrs, "rbf"), -1, -1, dialog_frame_color, 0, 0);
				if (dialog_result == -1)
					dialog_result = 0;
				/* PLATFORM(audio): restore audio playback volume. */ restore_audio_volume();
				input_pushed = 0;
				/* PLATFORM(input_kb): restore the active input source. */ /* PLATFORM(input_mouse): restore mouse input mode. */ /* PLATFORM(video): restore the opaque draw mode. */ pullmouse();
				if (dialog_result != 0) {
					finishgame(4, 0);
					sigframe = 1;
				}
				g_rplbfask = 0;
			}

			if (g_videoflg5 != 0) {
				setup_mcgawnd2();
				cam_idg = numid;
			} else {
				/* PLATFORM(video): copy the window sprite to the display page. */ usebuffer();
			}

			if (gm_playmode != gm_saved_rpl_mode || on_off_dash != dashbtogglesaved || replaybar_toggle != g_replaybarcpytgl || inrepflg != is_in_rplcopy || intraffic != opp_follow_flag_backup) {
				gm_saved_rpl_mode = gm_playmode;
				dashbtogglesaved = on_off_dash;
				g_replaybarcpytgl = replaybar_toggle;
				is_in_rplcopy = inrepflg;
				opp_follow_flag_backup = intraffic;
				roofbmphgt_saved = 0;
				mode_flag = 0;

				if (gm_playmode != 2 || menutimeout != 0 || (replaybar_toggle == 0 && inrepflg == 0)) {
						g_rplybarenable = 0;
				} else {
					g_rplybarenable = 1;
					goto replaybar_done;
				}
replaybar_done:

				if (menutimeout != 0) {
					dashbmpy_copy = PLATFORM_SCREEN_HEIGHT_PIXELS;
				} else if (on_off_dash != 0 && intraffic == 0) {
					if (gm_playmode == 2 && g_rplybarenable != 0) {
						rplbarabovehgt = 151;
					} else {
						rplbarabovehgt = PLATFORM_SCREEN_HEIGHT_PIXELS;
					}
					mode_flag = 1;
					roofbmphgt_saved = rfy5;
					dashbmpy_copy = dashbmy9;
				} else if (gm_playmode == 2 && g_rplybarenable != 0) {
					dashbmpy_copy = 151;
				} else {
					dashbmpy_copy = PLATFORM_SCREEN_HEIGHT_PIXELS;
				}

				if (previous_roof != roofbmphgt_saved || dashbmpy_copy != bitmapdash || replaybar_height != rplbarabovehgt) {
					g_simprect = g_vid_flag6;
					setprojectp(35, dashbmpy_copy / 6, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy);
					boundglassrect.top = roofbmphgt_saved;
					boundglassrect.bottom = dashbmpy_copy;
					previous_roof = roofbmphgt_saved;
					bitmapdash = dashbmpy_copy;
					replaybar_height = rplbarabovehgt;
				}
			}

			if (g_simprect != 0) {
				g_viewinx[cam_idg] = 0;
				if (mode_flag != 0) {
					/* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy, rplbarabovehgt);
					dodash(1);
				}
				if (g_rplybarenable != 0) {
					/* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS);
					doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
				}
			} else if (g_rplybarenable == 0) {
				g_viewinx[cam_idg] = 0;
			}

			drawworld(numid, &boundglassrect);
			if (dasty != 0 && mode_flag != 0) {
				if (statemgmtcpy != 0) {
					temp_rect.left = 0;
					temp_rect.right = PLATFORM_SCREEN_WIDTH_PIXELS;
					temp_rect.top = dasty;
					temp_rect.bottom = dashbmpy_copy;
					if (rectp != 0) {
						mergezones(rectp, &temp_rect, rectp);
					}
				}
				/* PLATFORM(video): draw a bitmap mask. */ andpackf(dasm_shp_7);
				/* PLATFORM(video): draw a shape layer. */ orpackf(g_dastbmpbuf);
			}

			drawworldflipzones(&boundglassrect);
			if (mode_flag != 0) {
				/* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, dashbmpy_copy, rplbarabovehgt);
				dodash(2);
				/* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS);
			}

			if (g_simprect != 0) {
				g_simprect--;
			}

			if (g_videoflg5 != 0) {
				/* PLATFORM(video): select the opaque drawing path. */ mouseoff();
				setup_mcgawnd1();
				numid ^= 1;
				cam_idg = numid;
				/* PLATFORM(video): select the transparent drawing path. */ mouseon();
			}

			if (gm_playmode == 1 && g_rpl_init == 0) {
				gm_playmode = 0;
				rate_frame = frm_rate2;
				globalgamesettings.game_framespersec = frm_rate2;
				initsim(-1);
			}

			if (menutimeout != 0) {
				/* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ if (normkeyi() != 0 || sigframe != 0 || joystate() != 0)
					break;
				continue;
			}

			if (sigframe != 0) {
				if (gm_playmode == 0 && core.game_3F6autoLoadEvalFlag != 4)
					break;
				if (sigframe == 2)
					break;
				sigframe = 0;
				gm_playmode = 2;
				/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ initmousedrive(0);
				doreplay(UI_REPLAY_OPERATION_INITIALIZE, 0, 0);
				doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 4, 0);
				inrepflg = 1;
				/* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
			}

			if (gm_playmode == 2) {
				doreplay(UI_REPLAY_OPERATION_HANDLE_INPUT, 0, 0);
				continue;
			}

			for (;;) {
				/* PLATFORM(input_kb): read the next key code. */ replay_key_code = normkeyi();
				if (replay_key_code != 0) {
					/* PLATFORM(input_kb): dispatch a normalized game key. */ mainkey(replay_key_code);
				}
				switch (replay_key_code) {
				case KEY_SCAN_UP:
				case KEY_SCAN_LEFT:
				case KEY_SCAN_RIGHT:
				case KEY_SCAN_DOWN:
					continue;
				}
				break;
			}

			if (gm_playmode == 1) {
				/* PLATFORM(input_mouse): read mouse buttons and coordinates. */ getmousepos(&flagsdown, &msecoordx, &pos_y_ms);
				/* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ if ((flagsdown & 3) != 0 || (joystate() & 0x30) != 0) {
					gm_playmode = 0;
					g_rpl_init = 0;
					rate_frame = frm_rate2;
					globalgamesettings.game_framespersec = frm_rate2;
					initsim(-1);
				}
			}
		}

		if (g_videoflg5 != 0 && get_0() != 0) {
			/* PLATFORM(video): select the opaque drawing path. */ mouseoff();
			setup_mcgawnd2();
			/* PLATFORM(video): clear a display rectangle. */ clear_rect(0, 0, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0);
			setup_mcgawnd1();
			/* PLATFORM(video): select the transparent drawing path. */ mouseon();
		}

		/* PLATFORM(video): copy the working sprite pages. */ usescreen();
		inrepflg = 1;
		/* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
		/* PLATFORM(audio): remove the audio timer hook. */ audio_remove_driver_timer();
		if (gm_playmode == 0 && globalgamesettings.game_opponenttype != 0 && core.opponentstate.car_crashBmpFlag == 0) {
			/* PLATFORM(input_joy): collect dialog joystick input. */
			/* PLATFORM(timer): wait for dialog input using timer ticks. */
			/* PLATFORM(video): render a modal dialog. */
			messagebox(3, 0, getresl(gamerptrs, "cop"), -1, 80, performGraphColor, dialog_values, 0);
			popupact = 1;
			dialog_result = rate_frame;
			dialog_result--;
			do {
				moveeverything(1);
				movesim();
				if (++dialog_result == rate_frame) {
					dialog_result = 0;
					maketime(&resbuftext, core.game_frame + elaptm1, 1);
					/* PLATFORM(video): select the opaque drawing path. */ mouseoff();
					/* PLATFORM(video): use the display text or sprite interface. */ movtxy(&resbuftext, mycenter(&resbuftext), dialog_values[1]);
					/* PLATFORM(video): select the transparent drawing path. */ mouseon();
				}
			} /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ while (hurrykey(1) != 27 && core.opponentstate.car_crashBmpFlag == 0 &&
				 1500 * rate_frame != core.game_frame + elaptm1);
		}

		popupact = 0;
		/* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ initmousedrive(0);
		removegameint();
		removegamefiles();
	}

	waitm_ms = 100;
	/* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ clear_keyboardbuffer();
	/* PLATFORM(input_joy): collect waiting-dialog joystick input. */ /* PLATFORM(timer): wait for dialog input using timer ticks. */ /* PLATFORM(video): render a waiting dialog. */ loading();
}

/* Maps a normalized key code to an in-game action. Params: key code. Returns: action/status byte. State: updates game and menu state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
I8 mainkey(unsigned key)
{
    switch (key) {
    case KEY_ASCII_ESCAPE:
        if (gm_playmode == 0) {
            finishgame(4, 0);
        }
        sigframe = 1;
        break;
    case BIOS_KEY_F2:
        cammd = CAMERA_MODE_F2;
        break;
    case BIOS_KEY_F3:
        cammd = CAMERA_MODE_F3;
        break;
    case BIOS_KEY_F4:
        cammd = CAMERA_MODE_F4;
        break;
    case KEY_ASCII_UPPER_H:
    case KEY_ASCII_LOWER_H:
        HKeyFlag ^= 1;
        break;
    /* PLATFORM(audio): pause and restore audio for mouse help. */
    /* PLATFORM(input_joy): collect mouse-help joystick input. */
    /* PLATFORM(input_kb): collect mouse-help key input. */
    /* PLATFORM(input_mouse): collect mouse-help pointer input. */
    /* PLATFORM(timer): wait for mouse help input using timer ticks. */
    /* PLATFORM(video): draw the mouse help interface. */
    case KEY_ASCII_UPPER_M:
    case KEY_ASCII_LOWER_M:
        mmouseon();
        /* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ initmousedrive(mousedrive);
        break;
    case KEY_ASCII_UPPER_D:
    case KEY_ASCII_LOWER_D:
        on_off_dash ^= 1;
        break;
    case KEY_ASCII_UPPER_R:
    case KEY_ASCII_LOWER_R:
        replaybar_toggle ^= 1;
        break;
    case KEY_ASCII_UPPER_C:
    case KEY_ASCII_LOWER_C:
        if (gm_playmode != 1) {
            cammd++;
            if (cammd == CAMERA_MODE_COUNT) {
                cammd = CAMERA_MODE_COCKPIT;
            }
        }
        break;
    case BIOS_KEY_F1:
        cammd = CAMERA_MODE_COCKPIT;
        break;
    case KEY_ASCII_LOWER_T:
        if (globalgamesettings.game_opponenttype != 0) {
            intraffic ^= 1;
        }
        break;
    default:
        if (gm_playmode == 1) {
            gm_playmode = 0;
            g_rpl_init = 0;
            rate_frame = frm_rate2;
            globalgamesettings.game_framespersec = frm_rate2;
            initsim(-1);
            return 1;
        }
        return 0;
    }
    return 1;
}

/* semantic lead: init_unknown from restunts.c */
/* Resets frame-timer and replay state. Params: none. Returns: none. State: initializes timer and replay globals. */
void initworld(void)
{
	register I16 zero;
	timeraud = 1;
	slomodiv8 = 2;
	zero = 0;
	tmr2 = zero;
	g_rpl_init = sigframe = 0;
	g_sgateopn = zero;
}

unsigned g_clocks;
unsigned char call_proc_flag;
/* Starts per-frame callback processing. Params: none. Returns: none. State: resets the callback counter and registration flag. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far gameintinit(void) {
    g_clocks = 0;
    /* PLATFORM(timer): register the timer callback. */ addtimer(gameint);
    call_proc_flag = 0;
}

/* Stops per-frame callback processing. Params: none. Returns: none. State: removes the registered timer callback. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far removegameint(void)
{
    /* PLATFORM(timer): read the timer counter. */ timedwait(10L);
    /* PLATFORM(timer): remove the timer callback. */ deltimer(gameint);
}

/* Processes one timer frame. Params: none. Returns: none. State: reads frame timing and updates audio-frame state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(timer): reads timer state or registers a callback. */
void far gameint(void)
{
    if (sseqds() == 0) {
        return;
    }
    if (call_proc_flag != 0) {
        return;
    }

    call_proc_flag++;
    if (call_proc_flag == 1) {
        snd_tick_clock++;
        if (snd_tick_clock >= frmcs_time && g_audio_frms_ix != sndposrecord) {
            /* PLATFORM(audio): apply recorded engine audio state. */ myupdatesfx1(&audio_frmarr[g_audio_frms_ix * AUDIO_FRAME_BYTES], snd_tick_clock);
            snd_tick_clock = 0;
            g_audio_frms_ix++;
            if (g_audio_frms_ix == AUDIO_FRAME_CAPACITY) {
                g_audio_frms_ix = 0;
            }
        }

        if (sigframe != 0 || g_rplbfask != 0) {
            goto frame_callback_done;
        }
        if (inrepflg != 0 && gm_playmode == 2) {
            goto frame_callback_done;
        }
        if (gm_playmode == 0 && core.game_end_delay_elapsed >= core.game_end_delay_limit) {
            inrepflg = 1;
            /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
            goto frame_callback_done;
        }

        timeraud--;
        if (timeraud == 0) {
            timeraud = frmcs_time;
            g_clocks++;
            if (gm_playmode == 2) {
                switch (g_rplmodui) {
                case 2:
                    slomodiv8--;
                    if (slomodiv8 == 0) {
                        moveeverything(0);
                        slomodiv8 = 2;
                    }
                    goto frame_callback_done;
                case 3:
                    moveeverything(0);
                    break;
                default:
                    break;
                }
            }
            moveeverything(0);
        }
    }
frame_callback_done:
    call_proc_flag--;
}

signed char array_rpl[64];
unsigned char replay_steer_flag[64];
static I8 replay_control;

/* Processes replay input for the selected mode. Params: mode. Returns: none. State: reads and updates replay, camera and input state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
void far moveeverything(I16 mode)
{
    register I16 input;
    register I16 i;

    if (mode != 0) {
        input = 0;
        goto record_input;
    }
    if (gm_playmode == 2) {
        if (tmr2 < globalgamesettings.game_recordedframes) {
            tmr2++;
            return;
        }
        if (sigframe != 0)
            return;
        inrepflg = 1;
        /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
replay_finished:
        sigframe = 1;
        return;
    }
    if (sigframe != 0 || core.game_3F6autoLoadEvalFlag != 0 || gm_playmode == 1) {
        input = 0;
        goto record_input;
    }
    if (pass_check_flag == 0 && g_rpl_init == 0 && rate_frame * 4 < core.game_frame)
        finishgame(1, 0);
    if (mousedrive != 0 || joystick_enabled != 0) {
        if (mousedrive != 0) {
            /* PLATFORM(input_mouse): read mouse buttons and coordinates. */ getmousepos(&flagsdown, &msecoordx, &pos_y_ms);
            i = msecoordx - 0xA0;
            if (abs(i) < 0x12)
                i = 0;
            else if (i > 0)
                i -= 0x12;
            else
                i += 0x12;
            replay_control = i;
            if (flagsdown & 1)
                input = 2;
            else if (flagsdown & 2)
                input = 1;
            else
                input = 0;
        } else {
            replay_control = replay_axis_value();
            if (replay_control > 0)
                replay_control = replay_axis_magnitude[replay_control];
            else if (replay_control < 0)
                replay_control = -replay_axis_magnitude[-replay_control];
            /* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ input = joystate() & 0x33;
        }
        i = tmr2 & 0x3F;
        array_rpl[i] = replay_control;
        replay_steer_flag[i] = 1;
    } else {
        /* PLATFORM(input_joy): read combined joystick flags. */ /* PLATFORM(input_kb): read combined keyboard flags. */ input = joystate();
    }
    /* PLATFORM(input_kb): read a key state. */ if (keystate(0x1E))
        input |= 0x10;
    /* PLATFORM(input_kb): read a key state. */ if (keystate(0x2C))
        input |= 0x20;
record_input:
    if (1500 * rate_frame <= tmr2 + elaptm1) {
        finishgame(4, 0);
        goto replay_finished;
    }
    if (tmr2 == 12000) {
        if (elaptm1 == 0 && popupact == 0) {
            popupact = 1;
            g_rplbfask = 1;
            return;
        }
        for (i = 0; i < 12000 / (30 * rate_frame) - 1; i++) {
            cvxs_a[i + 1].game_frame -= 30 * rate_frame;
            cvxs_a[i] = cvxs_a[i + 1];
        }
        for (i = 0; i < 12000 - 30 * rate_frame; i++)
            g_tdreplay16buf[i] = g_tdreplay16buf[i + 30 * rate_frame];
        tmr2 -= 30 * rate_frame;
        globalgamesettings.game_recordedframes -= 30 * rate_frame;
        elaptm1 += 30 * rate_frame;
        core.game_frame -= 30 * rate_frame;
    }
    g_tdreplay16buf[tmr2++] = input;
    globalgamesettings.game_recordedframes++;
}

/* Selects the car used as the camera target and refreshes its coordinates. Params: none. Returns: none. State: reads car states and updates camera-target globals. */
void far movehelicopters(void)
{
    I16 nearest_range;
    I16 car_count;
    I32 dz;
    struct CARSTATE *selected_car_state;
    struct VECTOR car_position;
    register I16 index;
    register I16 delta;
    I16S bearing;
    I16S facing;
    I16 cam_y;
    I16 climb;
    I8 track_segment;
    struct VECTOR focus;
    I16 point_range;
    I32 dx;
    I16 threshold;

    car_count = 1;
    if (globalgamesettings.game_opponenttype != 0)
        car_count = 2;
    for (index = 0; index < car_count; ++index) {
        core.game_chase_camera_prev[index] = core.game_chase_camera_pos[index];
        if (index == 0)
            selected_car_state = &core.playerstate;
        else
            selected_car_state = &core.opponentstate;

        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.y = selected_car_state->car_posWorld1.ly >> WORLD_COORDINATE_SHIFT;
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.x = selected_car_state->car_posWorld1.lx >> WORLD_COORDINATE_SHIFT;
        /* PORT: negative signed 32-bit world coordinates rely on arithmetic right shift. */
        car_position.z = selected_car_state->car_posWorld1.lz >> WORLD_COORDINATE_SHIFT;
        focus = selected_car_state->car_route_center;
        facing = selected_car_state->car_route_bearing;

        if (index == 0 && (core.game_route_recovery_mode != 0 || core.game_route_confirm_count != 0))
            focus = car_position;
        else if (selected_car_state->car_route_has_link != 0 || selected_car_state->car_crashBmpFlag != 0 ||
                 selected_car_state->car_trackdata3_index == -1 ||
                 (facing > 0x80 && facing < 0x380))
            focus = car_position;

        threshold = CAMERA_FOLLOW_DISTANCE;
        cam_y = car_position.y + CAMERA_FOLLOW_HEIGHT;
        climb = core.game_chase_camera_pos[index].y - cam_y;
        if (climb != 0) {
            delta = climb;
            if (delta > CAMERA_FOLLOW_VERTICAL_STEP)
                delta = CAMERA_FOLLOW_VERTICAL_STEP;
            else if (delta < -0x1E)
                delta = -0x1E;
            core.game_chase_camera_pos[index].y -= delta;
        }

        bearing = intatan(focus.x - core.game_chase_camera_pos[index].x,
                                  focus.z - core.game_chase_camera_pos[index].z);
        delta = inthypot(car_position.x - core.game_chase_camera_pos[index].x,
                              car_position.z - core.game_chase_camera_pos[index].z);
        if (delta > threshold) {
            delta -= threshold;
            if (rate_frame == 0x14) {
                if (delta > CAMERA_FOLLOW_MAX_STEP_20FPS) delta = CAMERA_FOLLOW_MAX_STEP_20FPS;
            } else if (delta > CAMERA_FOLLOW_MAX_STEP_10FPS) {
                delta = CAMERA_FOLLOW_MAX_STEP_10FPS;
            }
            core.game_chase_camera_pos[index].x += fixedmult(delta, intsin(bearing));
            core.game_chase_camera_pos[index].z += fixedmult(delta, intcos(bearing));
        }

        if (core.game_frame % (rate_frame >> 1) == 0) {
            nearest_range = 0x2710;
            for (track_segment = 0; track_segment < trk_sample_count; ++track_segment) {
                dx = (I32)trkptrpath[track_segment].x - car_position.x;
                dz = (I32)trkptrpath[track_segment].z - car_position.z;
                if ((dx < 0 ? -dx : dx) < nearest_range &&
                    (dz < 0 ? -dz : dz) < nearest_range) {
                    point_range = inthypot(dx, dz);
                    if (point_range < nearest_range) {
                        core.game_track_camera_index[index] = track_segment;
                        nearest_range = point_range;
                    }
                }
            }
        }
    }
}

/* semantic lead: file_load_replay from fileio.c */
/* Builds and reads the replay file header. Params: directory and replay name. Returns: load status. State: writes the replay header buffer. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
I8 loadreplay(const I8* dir, const I8* name)
{
	/* PLATFORM(file): build a file path. */ makename(dir, name, ".rpl", buf_g_path);

	g_is_busy = 1;
	/* PLATFORM(file): read replay data from a file. */ loadfileat(buf_g_path, td13_replay_hdr);
	globalgamesettings = *(struct GAMEINFO far*)td13_replay_hdr;
	g_is_busy = 0;
	return 0;
}

/* semantic lead: file_write_replay from fileio.c */
/* Writes the replay header and data. Params: filename. Returns: write status. State: reads replay buffers. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
I16S savereplay(const I8* filename)
{
	register I16 ret;
	I32 write_length;

	*(struct GAMEINFO far*)td13_replay_hdr = globalgamesettings;
	write_length = globalgamesettings.game_recordedframes + REPLAY_FILE_PREFIX_BYTES;
	g_is_busy = 1;
	/* PLATFORM(file): write replay data to a file. */ ret = savefilez(filename, td13_replay_hdr, write_length);
	g_is_busy = 0;
	return (I8)ret;
}

#include "shp2rec.h"

#include "sprite.h"
/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);

I8 aStdaxxxx[] = "stdaxxxx";
I8 aStdbxxxx[] = "stdbxxxx";
/* Prepares car dashboard and shape sprites. Params: mode. Returns: none. State: reads car/camera state and updates shape/resource handles. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(video): draws to or configures the display surface. */
void far dodash(I16 mode)
{

    static I16S last_steering_step[2];
    static I8 steering_zone[2];
    static struct SPRITE far * meters_sprite;
    static struct SPRITE far * gnob_sprite;
    static struct SPRITE far * gear_base_sprite;
    static void far * stdares;
    static void far * stdbres;
    static struct SHAPE2D far * wheel_shapes[9];
    static struct SHAPE2D far * gnobshapes[6];
    static struct SHAPE2D far * digshapes[10];
    
    static I8 gear_knob_visible_view[2];
    static I16S steering_dot_x[2];
    static I16S steering_dot_y[2];
    static I16S last_speedo[2];
    static I16S gear_knob_x_last[2];
    static I16S gear_knob_y_last[2];
    static I16S last_tacho[2];
    struct SHAPE2D far *shape;
    I16 digit;
    I8 speedo_type;
    I16 steering;
    I8 wheel_zone;
    U8  *dot;
    register I16 x;
    U8  x_pos;
    register I16 y;
    I8 changed;
    U8  y_pos;
    I8 knob_removed;
    I8 has_digit;

    switch (mode) {
    case UI_DASHBOARD_OPERATION_LOAD:
        aStdaxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdaxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdaxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdaxxxx[7] = globalgamesettings.game_playercarid[3];
        aStdbxxxx[4] = globalgamesettings.game_playercarid[0];
        aStdbxxxx[5] = globalgamesettings.game_playercarid[1];
        aStdbxxxx[6] = globalgamesettings.game_playercarid[2];
        aStdbxxxx[7] = globalgamesettings.game_playercarid[3];
        /* PLATFORM(file): load a named resource. */ stdares = myload(3, aStdaxxxx);
        /* PLATFORM(file): load a named resource. */ stdbres = myload(2, aStdbxxxx);
        locateshapes(stdares, "whl1whl2whl3ins2gboxins1ins3inm1inm3", (I8 far **)wheel_shapes);
        locateshapes(stdbres, "gnobgnabdot dotadot1dot2", (I8 far **)gnobshapes);
        if (simdp7.spdcenter.y == 0)
            locateshapes(stdbres, "dig0dig1dig2dig3dig4dig5dig6dig7dig8dig9", (I8 far **)digshapes);
        /* PLATFORM(video): create an offscreen sprite surface. */ meters_sprite = sprite_make_window(wheel_shapes[3]->s2d_width * pixel_scales, wheel_shapes[3]->s2d_height, 15);
        /* PLATFORM(video): create an offscreen sprite surface. */ gnob_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        /* PLATFORM(video): create an offscreen sprite surface. */ gear_base_sprite = sprite_make_window(wheel_shapes[4]->s2d_width * pixel_scales, wheel_shapes[4]->s2d_height, 15);
        shape = (struct SHAPE2D far *)locate_shape_fatal(stdares, "dash");
        /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(gear_base_sprite);
        /* PLATFORM(video): draw a shape layer. */ movpackcfxya(shape, shape->s2d_pos_x - wheel_shapes[4]->s2d_pos_x,
                        shape->s2d_pos_y - wheel_shapes[4]->s2d_pos_y);
        /* PLATFORM(video): copy the working sprite page. */ displaywindow();
        dashbmy9 = shape->s2d_pos_y;
        if (locateshapez(stdares, "roof") != 0)
            rfy5 = ((struct SHAPE2D far *)locate_shape_fatal(stdares, "roof"))->s2d_height;
        else
            rfy5 = 0;
        shape = (struct SHAPE2D far *)locateshapez(stdares, "dast");
        if (shape != 0) {
            dasty = shape->s2d_pos_y;
            g_dastbmpbuf = shape;
            dasm_shp_7 = locate_shape_fatal(stdares, "dasm");
            return;
        }
        dasty = 0;
        return;
    /* PLATFORM(video): select the opaque drawing path. */ case UI_DASHBOARD_OPERATION_DRAW_BASE:
        mouseoff();
        /* PLATFORM(video): draw a shape layer. */ if (locateshapez(stdares, "roof") != 0)
            movpackf((struct SHAPE2D far *)locate_shape_fatal(stdares, "roof"));
        /* PLATFORM(video): draw a shape layer. */ movpackcf((struct SHAPE2D far *)locate_shape_fatal(stdares, "dash"));
        /* PLATFORM(video): draw a shape layer. */ movpackcf(wheel_shapes[1]);
        /* PLATFORM(video): select the transparent drawing path. */ mouseon();
        x = 0;
        gear_knob_visible_view[cam_idg] = g_viewinx[cam_idg] = 0;
        steering_dot_y[cam_idg] = x;
        steering_zone[cam_idg] = (I8)x;
        x--;
        last_steering_step[cam_idg] = x;
        last_speedo[cam_idg] = x;
        last_tacho[cam_idg] = x;
        return;
    case UI_DASHBOARD_OPERATION_UPDATE:
        if ((core.playerstate.car_changing_gear | core.playerstate.car_fpsmul2) == 0 &&
            gear_knob_visible_view[cam_idg] != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                mouseoff();
            /* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ movcfxya(gear_base_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
            gear_knob_visible_view[cam_idg] = 0;
        } else if (gear_knob_visible_view[cam_idg] != core.playerstate.car_changing_gear ||
                   gear_knob_x_last[cam_idg] != core.playerstate.car_knob_x ||
                   gear_knob_y_last[cam_idg] != core.playerstate.car_knob_y ||
                   (core.playerstate.car_fpsmul2 != 0 && gear_knob_visible_view[cam_idg] == 0)) {
            /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(gnob_sprite);
            gear_knob_visible_view[cam_idg] = 1;
            /* PLATFORM(video): draw a shape layer. */ movpackcfxya(wheel_shapes[4], 0, 0);
            x = core.playerstate.car_knob_x;
            y = core.playerstate.car_knob_y;
            gear_knob_x_last[cam_idg] = x;
            gear_knob_y_last[cam_idg] = y;
            /* PLATFORM(video): blit a shape to the display. */ andcfxy(gnobshapes[1], x, y);
            /* PLATFORM(video): blit a shape with the OR raster operation. */ orcfxy(gnobshapes[0], x, y);
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            else {
                /* PLATFORM(video): copy the working sprite pages. */ usescreen();
                /* PLATFORM(video): select the opaque drawing path. */ mouseoff();
            }
            /* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ movcfxya(gnob_sprite->sprite_bitmapptr, wheel_shapes[4]->s2d_pos_x, wheel_shapes[4]->s2d_pos_y);
        }

        knob_removed = 0;
        steering = core.playerstate.car_steeringAngle / 8;
        wheel_zone = 1;
        if (steering < -10)
            wheel_zone = 0;
        else if (steering > 10)
            wheel_zone = 2;
        if (steering_zone[cam_idg] != wheel_zone || g_simprect != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                mouseoff();
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ movcfxya(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            switch (wheel_zone) {
            /* PLATFORM(video): draw a shape layer. */ case 0:
                movpackcf(wheel_shapes[0]);
                break;
            /* PLATFORM(video): draw a shape layer. */ case 1:
                movpackcf(wheel_shapes[1]);
                break;
            /* PLATFORM(video): draw a shape layer. */ case 2:
                movpackcf(wheel_shapes[2]);
                break;
            }
            steering_zone[cam_idg] = wheel_zone;
            changed = 1;
        } else
            changed = 0;

        switch (simdp7.spdcenter.y) {
        case -1:
            x = 0;
            speedo_type = 2;
            break;
        default:
            speedo_type = 0;
            x = core.playerstate.car_speed / 0x280;
            if (x >= simdp7.spdnumpoints)
                x = simdp7.spdnumpoints - 1;
            break;
        case 0:
            speedo_type = 1;
            /* PORT: car_speed is unsigned 16-bit Q8 mph; this shift is a zero-fill conversion. */
            x = core.playerstate.car_speed >> CAR_SPEED_Q8_SHIFT;
        }
        y = (U16S )core.playerstate.car_currpm >> 7;
        if (y >= simdp7.revnumpoints)
            y = simdp7.revnumpoints - 1;

        if (changed != 0 || g_simprect != 0 ||
            last_speedo[cam_idg] != x || last_tacho[cam_idg] != y) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                mouseoff();
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ movcfxya(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
                knob_removed = 1;
            }
            /* PLATFORM(video): select the sprite surface for drawing. */ sprite_setup1_from_arg_pointer(meters_sprite);
            /* PLATFORM(video): draw a shape layer. */ movpackfxya(wheel_shapes[3], 0, 0);
            last_speedo[cam_idg] = x;
            last_tacho[cam_idg] = y;
            if (speedo_type == 1) {
                digit = 0;
                if (x >= SPEEDOMETER_TWO_HUNDREDS_BASE) {
                    digit = 2;
                    x -= SPEEDOMETER_TWO_HUNDREDS_BASE;
                } else if (x >= SPEEDOMETER_HUNDREDS_BASE) {
                    digit = 1;
                    x -= SPEEDOMETER_HUNDREDS_BASE;
                }
                if (digit != 0) {
                    /* PLATFORM(video): blit a shape with the OR raster operation. */ orcfxya(digshapes[digit], simdp7.spdpoints[0], simdp7.spdpoints[1]);
                    has_digit = 1;
                }
                digit = x / 10;
                if (digit != 0 || has_digit != 0) {
                    /* PLATFORM(video): blit a shape with the OR raster operation. */ orcfxya(digshapes[digit], simdp7.spdpoints[2], simdp7.spdpoints[3]);
                    x -= digit * 10;
                    has_digit = 1;
                }
                /* PLATFORM(video): blit a shape with the OR raster operation. */ orcfxya(digshapes[x], simdp7.spdpoints[4], simdp7.spdpoints[5]);
            } else if (speedo_type == 0) {
                /* PLATFORM(video): draw a preview line. */ preRender_line(simdp7.spdcenter.x, simdp7.spdcenter.y,
                               simdp7.spdpoints[x * 2], simdp7.spdpoints[x * 2 + 1],
                               spdneedlegaugeclr);
            }
            /* PLATFORM(video): draw a preview line. */ preRender_line(simdp7.revcenter.x, simdp7.revcenter.y,
                           simdp7.revpoints[y * 2], simdp7.revpoints[y * 2 + 1],
                           spdneedlegaugeclr);
            switch (wheel_zone) {
            /* PLATFORM(video): draw a bitmap mask. */ case 0:
                andpackf(wheel_shapes[7]);
                /* PLATFORM(video): draw a shape layer. */ orpackf(wheel_shapes[5]);
                break;
            /* PLATFORM(video): draw a bitmap mask. */ case 2:
                andpackf(wheel_shapes[8]);
                /* PLATFORM(video): draw a shape layer. */ orpackf(wheel_shapes[6]);
                break;
            }
            if (g_videoflg5 != 0)
                setup_mcgawnd2();
            /* PLATFORM(video): copy the working sprite pages. */ else
                usescreen();
            /* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            /* PLATFORM(video): blit a shape to the display. */ movcfxya(meters_sprite->sprite_bitmapptr, wheel_shapes[3]->s2d_pos_x, wheel_shapes[3]->s2d_pos_y);
        }

        if (last_steering_step[cam_idg] != steering || g_simprect != 0 || knob_removed != 0) {
            /* PLATFORM(video): select the opaque drawing path. */ if (g_videoflg5 == 0)
                mouseoff();
            /* PLATFORM(video): select the sprite clip rectangle. */ clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, rplbarabovehgt);
            if (steering_dot_y[cam_idg] != 0) {
                /* PLATFORM(video): blit a shape to the display. */ movcfxya(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
                steering_dot_y[cam_idg] = 0;
            }
            dot = &simdp7.steeringdots[(steering < 0 ? -steering : steering) * 2];
            y_pos = dot[1];
            x_pos = dot[0];
            if (steering < 0)
                x_pos -= (x_pos - simdp7.steeringdots[0]) * 2;
            steering_dot_x[cam_idg] = (x_pos - gnobshapes[2]->s2d_hotspot_x) & vidflg3is_minus1;
            steering_dot_y[cam_idg] = y_pos - gnobshapes[2]->s2d_hotspot_y;
            /* PLATFORM(video): use the display text or sprite interface. */ readfxya(gnobshapes[numid + 4], steering_dot_x[cam_idg], steering_dot_y[cam_idg]);
            /* PLATFORM(video): blit a shape to the display. */ andcfxy(gnobshapes[3], x_pos, y_pos);
            /* PLATFORM(video): blit a shape with the OR raster operation. */ orcfxy(gnobshapes[2], x_pos, y_pos);
            last_steering_step[cam_idg] = steering;
        }
        /* PLATFORM(video): select the transparent drawing path. */ mouseon();
        return;
    /* PLATFORM(video): release an offscreen sprite surface. */ case UI_DASHBOARD_OPERATION_UNLOAD:
        removewindow(gear_base_sprite);
        /* PLATFORM(video): release an offscreen sprite surface. */ removewindow(gnob_sprite);
        /* PLATFORM(video): release an offscreen sprite surface. */ removewindow(meters_sprite);
        /* PLATFORM(memory): release resource memory. */ freememory(stdbres);
        /* PLATFORM(memory): release resource memory. */ freememory(stdares);
        return;
    }
}

/* semantic lead: setup_player_cars from restunts.c */
/* Local view: openresource; renderer retains its byte-pointer resource prototype. */
extern void far *far openresource(I8 *);

/* Local view: audio_init_engine; parameters[1].pointer is void; another unit uses unsigned byte. */
extern I16 audio_init_engine(I16 id, void far *data, void far *eng1, void far *eng);

/* Loads player/opponent car and engine resources. Params: none. Returns: setup status. State: updates resource handles and engine IDs. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
I16 loadgamefiles(void) {
	void far* carresptr;
	I32 mem_limit;

	g_wndspr = 0;
	/* PLATFORM(file): check required files and request missing files. */
	/* PLATFORM(input_joy): collect file-confirmation joystick input. */
	/* PLATFORM(input_kb): collect file-confirmation key input. */
	/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
	/* PLATFORM(timer): wait for file confirmation using timer ticks. */
	/* PLATFORM(video): show missing-file guidance. */
	ensure_file_exists(2);
	init_3dcars(globalgamesettings.game_playercarid, globalgamesettings.game_opponentcarid);
	aCarcoun[3] = globalgamesettings.game_playercarid[0];
	aCarcoun[4] = globalgamesettings.game_playercarid[1];
	aCarcoun[5] = globalgamesettings.game_playercarid[2];
	aCarcoun[6] = globalgamesettings.game_playercarid[3];
	/* PLATFORM(file): load a resource file. */ carresptr = openresource(aCarcoun);
	getsimdata(carresptr, 0);
	/* PLATFORM(memory): release a loaded resource. */ closeresource(carresptr);

	if (globalgamesettings.game_opponenttype != 0) {
		aCarcoun[3] = globalgamesettings.game_opponentcarid[0];
		aCarcoun[4] = globalgamesettings.game_opponentcarid[1];
		aCarcoun[5] = globalgamesettings.game_opponentcarid[2];
		aCarcoun[6] = globalgamesettings.game_opponentcarid[3];
		/* PLATFORM(file): load a resource file. */ carresptr = openresource(aCarcoun);
		getsimdata(carresptr, 1);
		/* PLATFORM(memory): release a loaded resource. */ closeresource(carresptr);
		
		/* PLATFORM(file): check required files and request missing files. */
		/* PLATFORM(input_joy): collect file-confirmation joystick input. */
		/* PLATFORM(input_kb): collect file-confirmation key input. */
		/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
		/* PLATFORM(timer): wait for file confirmation using timer ticks. */
		/* PLATFORM(video): show missing-file guidance. */
		ensure_file_exists(4);
		opponentcompile();
	}

	/* PLATFORM(file): check required files and request missing files. */
	/* PLATFORM(input_joy): collect file-confirmation joystick input. */
	/* PLATFORM(input_kb): collect file-confirmation key input. */
	/* PLATFORM(input_mouse): collect file-confirmation pointer input. */
	/* PLATFORM(timer): wait for file confirmation using timer ticks. */
	/* PLATFORM(video): show missing-file guidance. */
	ensure_file_exists(3);
	/* PLATFORM(file): load a named resource. */ eng1resourceptr = myload(5, "eng1");//aEng1); // "eng1"
	/* PLATFORM(file): load a named resource. */ engdata = myload(6, "eng");//aEng); // "eng"
	/* PLATFORM(audio): install the audio timer hook. */ audio_add_driver_timer();
	/* PLATFORM(audio): start an engine audio stream. */ g_player_sound_id = audio_init_engine(0x21, &player_engine_profile, eng1resourceptr, engdata);

	sndpendingstate = 0;
	g_plyr_snd_state = 0;
	audiooppflag = 0;
	if (globalgamesettings.game_opponenttype != 0) {
		/* PLATFORM(audio): start an engine audio stream. */ op_eng_sound_id = audio_init_engine(0x20, &eng2, eng1resourceptr, engdata);
	}

	g_audio_frms_ix = 0;
	sndposrecord = 0;
	snd_tick_clock = 0;
	/* PLATFORM(file): load a named resource. */ fntled_res = myload(0, "fontled.fnt");//aFontled_fnt); // "fontled.fnt"
	statemgmtcpy = slow_video_mode_state;
	initdrawworld();
	if (menutimeout == 0) {
		dodash(0);
	}

	if (menutimeout == 0) {
		/* PLATFORM(file): load a named resource. */ sdgresourcehandle = myload(3, "sdgame");//aSdgame); // "sdgame"
		doreplay(UI_REPLAY_OPERATION_INITIALIZE, 0, 0);
	}

	/* PLATFORM(file): load a resource file. */ gamerptrs = openresource("game");
	g_planlist = getres(gamerptrs, "plan");//aPlan); // "plan"
	wallrecrecord = getres(gamerptrs, "wall");//aWall); // "wall"
	loaddrawbits();
	initscenery(td14tb[0x384]);
	if (init_3dpieces() != 0) {
		return 1;
	}

	if (g_videoflg5 == 0) {
		
		mem_limit = 0xFA00L / (pixel_scales * vidflg4_is1) + 0x12;
		/* PLATFORM(memory): query available resource memory. */ if ((I32)availmem() <= mem_limit) {
			return 1;
		}
		/* PLATFORM(video): create an offscreen sprite surface. */ g_wndspr = sprite_make_window(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS, 0x0F);
	}

	intraffic = 0;
	is_in_rplcopy = -1;
	return 0;
}

/* semantic lead: free_player_cars from restunts.c */
/* Releases player/opponent resources and audio hooks. Params: none. Returns: none. State: reads and clears resource handles. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(memory): queries, allocates or releases resource memory. */
/* PLATFORM(video): draws to or configures the display surface. */
void removegamefiles(void) {
	if (g_videoflg5 == 0) {
		if (g_wndspr != 0) {
			/* PLATFORM(video): release an offscreen sprite surface. */ removewindow(g_wndspr);
		}
	}
	remove_3dpieces();
	removescenery();
	removedrawbits();
	/* PLATFORM(memory): release a loaded resource. */ closeresource(gamerptrs);
	if (menutimeout == 0) {
		/* PLATFORM(memory): release resource memory. */ freememory(sdgresourcehandle);
		dodash(3);
	}

	/* PLATFORM(memory): release resource memory. */ freememory(fntled_res);
	/* PLATFORM(audio): remove the audio timer hook. */ audio_remove_driver_timer();
	/* PLATFORM(memory): release resource memory. */ freememory(engdata);
	/* PLATFORM(memory): release resource memory. */ freememory(eng1resourceptr);
	remove_3dcars();
}

/* Sets the in-game mouse bounds. Params: enabled flag. Returns: none. State: maps the flag to mouse range and position. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
void initmousedrive(I16 enabled)
{
    if (enabled) {
        /* PLATFORM(input_mouse): set the mouse coordinate range. */ setmousebounds(15, 0, 0x131, PLATFORM_SCREEN_HEIGHT_PIXELS);
        /* PLATFORM(input_mouse): set the mouse position. */ setmousepos(0xA0, 0x64);
        return;
    }
    /* PLATFORM(input_mouse): set the mouse coordinate range. */ setmousebounds(0, 0, PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);
}
/* Applies the current replay steering sample. Params: none. Returns: none. State: reads replay samples and updates steering state. */
void far mousesteer(void)
{
    register I16 frame_index = core.game_frame & 0x3F;
    register I16 steering;
    I8 speed_index;
    I8 response;
    I8 angle;

    if (replay_steer_flag[frame_index] == 0)
        return;

    steering = array_rpl[frame_index];
    speed_index = (core.playerstate.car_speed2 >> 10) & 0xFC;
    response = ((I8*)table_lookup)[speed_index + 1];

    if (core.playerstate.car_steeringAngle < steering) {
        if (core.playerstate.car_steeringAngle < -1)
            response <<= 2;
    } else if (core.playerstate.car_steeringAngle > steering) {
        if (core.playerstate.car_steeringAngle > 1)
            response <<= 2;
    }

    if (core.playerstate.car_steeringAngle > steering &&
        core.playerstate.car_steeringAngle - response >= steering) {
        angle = 8;
    } else if (core.playerstate.car_steeringAngle < steering &&
               core.playerstate.car_steeringAngle + response <= steering) {
        angle = 4;
    } else {
        angle = 0;
    }

    if (angle != 0)
        g_tdreplay16buf[core.game_frame] |= angle;
    replay_steer_flag[frame_index] = 0;
}

I8 camera_button_index = 6;
I8 camera_button_count_mode[10] = { 1, 7, 3, 4, 5, 6, 7, 8, 8, 0 };
I8 camera_mode_select_a[10] = { 0, 0, 2, 2, 3, 4, 5, 1, 7, 0 };
I8 camera_mode_select_b[10] = { 2, 6, 2, 3, 4, 5, 6, 7, 8, 0 };
I8 camera_mode_select_c[10] = { 0, 1, 0, 0, 1, 1, 1, 7, 8, 0 };
I8 game_camera_buttons_count[4] = { 6, 6, 8, 7 };
I16 game_camera_buttons_x1[GAME_CAMERA_BUTTON_CAPACITY] = { 272, 109, 274, 232, 190, 151, 108, 66, 10 };
I16 game_camera_buttons_x2[GAME_CAMERA_BUTTON_CAPACITY] = { 314, 151, 314, 274, 232, 190, 151, 91, 47 };
I16 game_camera_buttons_y1[GAME_CAMERA_BUTTON_CAPACITY] = { 176, 176, 156, 156, 156, 156, 156, 156, 156 };
I16 game_camera_buttons_y2[GAME_CAMERA_BUTTON_CAPACITY] = { 193, 193, 173, 173, 173, 173, 173, 193, 193 };
I16 game_change_view_x1[1] = { 0 };
I16 game_change_view_x2[1] = { 104 };
I16 game_change_view_y1[1] = { 151 };
I16 game_change_view_y2[1] = { PLATFORM_SCREEN_HEIGHT_PIXELS };
static I8 view_camera_choice[2];
static I8 camera_buttons_pressed[GAME_CAMERA_BUTTON_CAPACITY];
/* One unreferenced target byte; semantic type/name are unrecoverable and its slot is hash-constrained. */
static I8 unused_40E73;
static I8 camera_mode_view[2];
static I8 camera_buttons_state_per_view[18];

static I16 camera_button_row[2];
static I16 camera_button_tick[2];
static I16 camera_button_col_cache[2];
int viewyshift;

static void far *replayshapes[23];

/* Local view: do_fileselect_dialog; parameters[3].pointer is void; another unit uses signed byte. */
extern I8 far floadreq(I8 *, I8 *, I8 *, void far *);
/* Local view: fsavereq; return is signed byte; another unit uses signed word. */
extern I8 far fsavereq(I8 *, I8 *, void far *);

/* Runs one game/replay loop step and updates its UI. Params: mode, frame index and frame offset. Returns: none. State: reads and updates game, replay, camera and UI state. */
/* PLATFORM(audio): updates audio playback or hooks. */
/* PLATFORM(file): loads, reads, writes or resolves game files. */
/* PLATFORM(input_joy): reads joystick state. */
/* PLATFORM(input_kb): reads keyboard state or dispatches a game key. */
/* PLATFORM(input_mouse): reads pointer state or configures pointer bounds. */
/* PLATFORM(timer): reads timer state or registers a callback. */
/* PLATFORM(video): draws to or configures the display surface. */
void doreplay(I16 mode, I16 frame_index, I16 frame_offset)
{
    I8 answer;
    unsigned key_code;
    I16 dialog_params[8];
    I16 prev_sky;
    I16 time_delta;
    register I16 i;
    register I16 j;
    I8 write_state;
    I8 button;
    I32 travel;
    I8 modifier;
    struct GAMEINFO oldcfg;

    switch (mode) {
    case UI_REPLAY_OPERATION_INITIALIZE:
        locateshapes(sdgresourcehandle,
            "rplyrpicrpacrpmcrptcbof6bof5bof4bof3bof2bof1bof0zoompannbon6bon5bon4bon3bon2bon1bof0zoompann",
            (I8 far **)replayshapes);
        frame_index = 4;
    case UI_REPLAY_OPERATION_SET_PRESSED:
        for (i = 0; i < 9; i++)
            camera_buttons_pressed[i] = 0;
        camera_buttons_pressed[frame_index] = 1;
        break;
    case UI_REPLAY_OPERATION_DRAW:
        if (g_viewinx[cam_idg] == 0) {
            g_viewinx[cam_idg] = 1;
            camera_mode_view[cam_idg] = -1;
            view_camera_choice[cam_idg] = -1;
            for (i = 0; i < 9; i++)
                camera_buttons_state_per_view[i * 2 + cam_idg] = 0;
            /* PLATFORM(video): select the opaque drawing path. */ mouseoff();
            /* PLATFORM(video): draw a shape layer. */ movpackf(replayshapes[0]);
            camera_button_tick[cam_idg] = -1;
            camera_button_col_cache[cam_idg] = -1;
            maketime(&resbuftext, globalgamesettings.game_recordedframes + elaptm1, 1);
            /* PLATFORM(video): set the font drawing state. */ textcolor(dlg_colour, 0);
            /* PLATFORM(video): select a font definition. */ mysetfont(fntled_res);
            /* PLATFORM(video): draw text at screen coordinates. */ movtxy(&resbuftext, 0xD8, 0xBB);
            /* PLATFORM(video): select the active font definition. */ fontsetfontdef();
        }
        if (camera_button_tick[cam_idg] != frame_offset + elaptm1) {
            camera_button_tick[cam_idg] = frame_offset + elaptm1;
            maketime(&resbuftext, frame_offset + elaptm1, 1);
            /* PLATFORM(video): set the font drawing state. */ textcolor(dlg_colour, 0);
            /* PLATFORM(video): select the opaque drawing path. */ mouseoff();
            /* PLATFORM(video): select a font definition. */ mysetfont(fntled_res);
            /* PLATFORM(video): draw text at screen coordinates. */ movtxy(&resbuftext, 0x98, 0xBB);
            /* PLATFORM(video): select the active font definition. */ fontsetfontdef();
        }
        if (camera_mode_view[cam_idg] != cammd) {
            camera_mode_view[cam_idg] = cammd;
            camera_button_col_cache[cam_idg] = -1;
            /* PLATFORM(video): select the opaque drawing path. */ mouseoff();
            /* PLATFORM(video): draw a shape layer. */ movpackf(replayshapes[cammd + 1]);
            if (game_camera_buttons_count[cammd] < camera_button_index)
                camera_button_index = game_camera_buttons_count[cammd];
            if (view_camera_choice[cam_idg] > 6)
                view_camera_choice[cam_idg] = -1;
        }
        if (globalgamesettings.game_recordedframes == 0) {
            i = 0;
            j = 0;
        } else {
            i = (I32)frame_index * UI_REPLAY_PROGRESS_TRAVEL / globalgamesettings.game_recordedframes;
            j = (I32)frame_offset * UI_REPLAY_PROGRESS_TRAVEL / globalgamesettings.game_recordedframes;
        }
        if (camera_button_col_cache[cam_idg] != i || camera_button_row[cam_idg] != j) {
            /* PLATFORM(video): select the opaque drawing path. */ mouseoff();
            camera_button_col_cache[cam_idg] = i;
            camera_button_row[cam_idg] = j;
            /* PLATFORM(video): draw a UI sprite. */ drawrect(0x9A, 0xB1, 0x74, 6, camera_select_fill_color);
            /* PLATFORM(video): draw a UI sprite. */ drawrect(i + 0x9A, 0xB1, 6, 6, dlg_colour);
            /* PLATFORM(video): draw a UI sprite outline. */ sprite_draw_rect_outline(j + 0x9A, 0xB1, j + 0x9F, 0xB6, camera_select_outline_color);
        }
        do {
            if (view_camera_choice[cam_idg] != camera_button_index)
                goto redraw_buttons;
            for (button = 0; button < 7; button++) {
                if (camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button])
                    goto redraw_buttons;
            }
            continue;
    /* PLATFORM(video): select the opaque drawing path. */ redraw_buttons:
            mouseoff();
            if (view_camera_choice[cam_idg] != -1) {
                /* PLATFORM(video): draw a shape layer. */ if (camera_buttons_state_per_view[view_camera_choice[cam_idg] * 2 + cam_idg] != 0)
                    movpackf(replayshapes[view_camera_choice[cam_idg] + 14]);
                /* PLATFORM(video): draw a shape layer. */ else
                    movpackf(replayshapes[view_camera_choice[cam_idg] + 5]);
                view_camera_choice[cam_idg] = -1;
            }
            for (button = 0; button < 7; button++) {
                if (camera_buttons_pressed[button] == 0 && camera_buttons_state_per_view[button * 2 + cam_idg] != camera_buttons_pressed[button]) {
                    /* PLATFORM(video): draw a shape layer. */ movpackf(replayshapes[button + 5]);
                    camera_buttons_state_per_view[button * 2 + cam_idg] = 0;
                }
            }
            for (button = 0; button < 7; button++) {
                if (camera_buttons_pressed[button] != 0) {
                    camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
                    /* PLATFORM(video): draw a shape layer. */ movpackf(replayshapes[button + 14]);
                    camera_buttons_state_per_view[button * 2 + cam_idg] = 1;
                }
            }
            view_camera_choice[cam_idg] = camera_button_index;
            /* PLATFORM(video): draw a UI sprite outline. */ if (camera_button_index != -1)
                sprite_draw_rect_outline(game_camera_buttons_x1[camera_button_index], game_camera_buttons_y1[camera_button_index],
                              game_camera_buttons_x2[camera_button_index], game_camera_buttons_y2[camera_button_index], camera_select_outline_color);
        } while (0);
/* PLATFORM(video): select the transparent drawing path. */ 
        mouseon();
        break;
    case UI_REPLAY_OPERATION_HANDLE_INPUT:
        if (game_camera_buttons_count[cammd] < camera_button_index && cammd != CAMERA_MODE_F3)
            camera_button_index = game_camera_buttons_count[cammd];
        /* PLATFORM(video): copy the working sprite page. */ displaywindow();
        if (g_videoflg5 != 0)
            cam_idg = numid ^ 1;
        for (;;) {
            /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ /* PLATFORM(timer): read the elapsed timer interval. */
            key_code = mykey(elticks());
            /* PLATFORM(input_mouse): test the current pointer against UI buttons. */ button = mousecursor(game_camera_buttons_count[cammd] + 1, game_camera_buttons_x1,
                                         game_camera_buttons_x2, game_camera_buttons_y1, game_camera_buttons_y2);
            if (button != -1) {
                if (button != camera_button_index && key_code == 0)
                    key_code = 1;
                camera_button_index = button;
                if ((key_code == ' ' || key_code == '\r') && camera_button_index >= UI_REPLAY_BUTTON_ZOOM) {
                    if (camera_button_index == UI_REPLAY_BUTTON_ZOOM) {
                        if ((game_camera_buttons_y1[UI_REPLAY_BUTTON_ZOOM] + game_camera_buttons_y2[UI_REPLAY_BUTTON_ZOOM]) >> 1 < pos_y_ms)
                            key_code = KEY_SCAN_DOWN;
                        else
                            key_code = KEY_SCAN_UP;
                    } else {
                        switch (((intatan(msecoordx - ((game_camera_buttons_x1[UI_REPLAY_BUTTON_PAN] + game_camera_buttons_x2[UI_REPLAY_BUTTON_PAN]) >> 1),
                                             ((game_camera_buttons_y1[UI_REPLAY_BUTTON_PAN] + game_camera_buttons_y2[UI_REPLAY_BUTTON_PAN]) >> 1) - pos_y_ms)
                                  + 0x80) & 0x3FF) >> 8) {
                        case 0:
                            key_code = KEY_SCAN_UP;
                            break;
                        case 1:
                            key_code = KEY_SCAN_RIGHT;
                            break;
                        case 2:
                            key_code = KEY_SCAN_DOWN;
                            break;
                        case 3:
                            key_code = KEY_SCAN_LEFT;
                            break;
                        }
                    }
                }
            } else {
                /* PLATFORM(input_mouse): test the current pointer against UI buttons. */ button = mousecursor(1, game_change_view_x1, game_change_view_x2, game_change_view_y1, game_change_view_y2);
                if (button == 0 && (key_code == ' ' || key_code == '\r'))
                    key_code = 'c';
            }
            /* PLATFORM(input_kb): dispatch a normalized game key. */ if (key_code != 0 && key_code != 0x1B && mainkey(key_code) != 0)
                return;
            if (inrepflg == 0 && key_code == 0) {
                if (g_rplybarenable == 0)
                    return;
                doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                return;
            }
            if (g_rplybarenable == 0) {
                is_in_rplcopy = -1;
                bitmapdash = -1;
            }
            if (inrepflg != 0 && (camera_buttons_pressed[3] != 0 || camera_buttons_pressed[2] != 0))
                doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 4, 0);
            doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
            modifier = 0;
            /* PLATFORM(input_kb): read a key state. */ if (keystate(0x1D) != 0 || (camera_button_index == 8 && (kbjoyflags & 0x30) != 0))
                modifier = 1;
            if (modifier != 0) {
                switch (key_code) {
                case KEY_SCAN_RIGHT:
                    camheading += 0x10;
                    return;
                case KEY_SCAN_LEFT:
                    camheading -= 0x10;
                    return;
                case KEY_SCAN_UP:
                    if (campitch + 0x10 < 0x100) {
                        campitch += 0x10;
                        return;
                    }
                    break;
                case KEY_SCAN_DOWN:
                    if (campitch - 0x10 > -0x100) {
                        campitch -= 0x10;
                        return;
                    }
                    break;
                case '-':
zoom_out:
                    if (cammd == CAMERA_MODE_F4) {
                        if (viewyshift <= 0)
                            break;
                        viewyshift -= CAMERA_MANUAL_HEIGHT_STEP;
                    } else {
                        if (camlen >= 1500)
                            break;
                        camlen += CAMERA_MANUAL_DISTANCE_STEP;
                    }
                    return;
                case '+':
zoom_in:
                    if (cammd == CAMERA_MODE_F4) {
                        if (viewyshift >= 900)
                            break;
                        viewyshift += CAMERA_MANUAL_HEIGHT_STEP;
                    } else {
                        if (camlen <= CAMERA_CUSTOM_DISTANCE_MIN)
                            break;
                        camlen -= CAMERA_MANUAL_DISTANCE_STEP;
                    }
                    return;
                }
                key_code = 0;
            }
            switch (key_code) {
            case '+':
                goto zoom_in;
            case '-':
                goto zoom_out;
            case KEY_SCAN_LEFT:
                if (game_camera_buttons_count[cammd] >= camera_button_count_mode[camera_button_index])
                    camera_button_index = camera_button_count_mode[camera_button_index];
            default:
                doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                continue;
            case KEY_SCAN_RIGHT:
                camera_button_index = camera_mode_select_a[camera_button_index];
                doreplay(1, core.game_frame, core.game_frame);
                continue;
            case KEY_SCAN_UP:
                if (camera_button_index == 7)
                    goto zoom_in;
                camera_button_index = camera_mode_select_b[camera_button_index];
                doreplay(1, core.game_frame, core.game_frame);
                continue;
            case KEY_SCAN_DOWN:
                if (camera_button_index == 7)
                    goto zoom_out;
                camera_button_index = camera_mode_select_c[camera_button_index];
                doreplay(1, core.game_frame, core.game_frame);
                continue;
            case '\r':
            case ' ':
                switch (camera_button_index) {
                case UI_REPLAY_BUTTON_MENU:
pause_menu:
                    inrepflg = 1;
                    /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                    doreplay(2, 4, 0);
                    doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                    for (i = 0; i < 8; i++)
                        dialog_params[i] = 0;
                    if (core.playerstate.car_crashBmpFlag != 0)
                        dialog_params[3] = 1;
                    if (globalgamesettings.game_recordedframes == 0 || elaptm1 != 0)
                        dialog_params[5] = 1;
                    if (pass_check_flag == 0) {
                        dialog_params[2] = 1;
                        dialog_params[3] = 1;
                    }
                    if ((didrace & 4) == 0)
                        dialog_params[1] = 1;
                    g_simprect = g_vid_flag6;
                    /* PLATFORM(input_joy): collect dialog joystick input. */
                    /* PLATFORM(timer): wait for dialog input using timer ticks. */
                    /* PLATFORM(video): render a modal dialog. */
                    answer = messagebox(2, 0, getresl(gamerptrs, "men"), -1, -1, dialog_frame_color, dialog_params, 0);
                    switch (answer) {
                    /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ case UI_PAUSE_MENU_RESTART:
                        clear_keyboardbuffer();
                        rate_frame = frm_rate2;
                        globalgamesettings.game_framespersec = frm_rate2;
                        initsim(-1);
                        tmr2 = 0;
                        globalgamesettings.game_recordedframes = 0;
                        popupact = 0;
                        didrace = 1;
                        goto continue_driving;
                    case UI_PAUSE_MENU_CONTINUE_DRIVING:
                        if (didrace & 2)
                            didrace = 3;
                        else if (globalgamesettings.game_recordedframes != tmr2) {
                            /* PLATFORM(input_joy): collect dialog joystick input. */
                            /* PLATFORM(timer): wait for dialog input using timer ticks. */
                            /* PLATFORM(video): render a modal dialog. */
                            i = messagebox(2, 0, getresl(gamerptrs, "con"), -1, -1, performGraphColor, 0, 0);
                            if (i < 1)
                                break;
                            didrace = 3;
                        } else
                            didrace = 1;
                        globalgamesettings.game_recordedframes = tmr2 = core.game_frame;
    continue_driving:
                        on_off_dash = 1;
                        pen_flag_count = 0;
                        intraffic = 0;
                        gm_playmode = 0;
                        cammd = 0;
                        core.game_3F6autoLoadEvalFlag = 0;
                        core.game_end_delay_elapsed = 0;
                        g_rplmodui = 0;
                        doreplay(2, 3, 0);
                        inrepflg = 0;
                        /* PLATFORM(input_mouse): set or restore the mouse coordinate range. */ initmousedrive(mousedrive);
                        /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ clear_keyboardbuffer();
                        kbormouse = 0;
                        break;
                    case UI_PAUSE_MENU_LOAD_REPLAY:
                        didrace = 0;
                        /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                        /* PLATFORM(file): open the file selection interface. */
                        /* PLATFORM(input_joy): collect file-selection joystick input. */
                        /* PLATFORM(timer): wait for file selection using timer ticks. */
                        /* PLATFORM(video): draw the file selection interface. */
                        i = floadreq(replay_file, ReplayName, ".rpl", getresl(main_data_file_addr, "rep"));
                        if (i == 0)
                            break;
                        waitm_ms = 150;
                        /* PLATFORM(input_joy): collect waiting-dialog joystick input. */ /* PLATFORM(timer): wait for dialog input using timer ticks. */ /* PLATFORM(video): render a waiting dialog. */ loading();
                        oldcfg = globalgamesettings;
                        prev_sky = td14tb[0x384];
                        /* PLATFORM(file): load replay data from a file. */ if (loadreplay(replay_file, ReplayName) != 0)
                            globalgamesettings.game_recordedframes = 0;
                        on_off_dash = 0;
                        compiletrack();
                        i = 0;
                        if (td14tb[0x384] != prev_sky)
                            i = 1;
                        if (oldcfg.game_playercarid[0] != globalgamesettings.game_playercarid[0] ||
                            oldcfg.game_playercarid[1] != globalgamesettings.game_playercarid[1] ||
                            oldcfg.game_playercarid[2] != globalgamesettings.game_playercarid[2] ||
                            oldcfg.game_playercarid[3] != globalgamesettings.game_playercarid[3])
                            i = 1;
                        else if (oldcfg.game_opponenttype != globalgamesettings.game_opponenttype)
                            i = 1;
                        else if (globalgamesettings.game_opponenttype != 0) {
                            if (oldcfg.game_opponentcarid[0] != globalgamesettings.game_opponentcarid[0] ||
                                oldcfg.game_opponentcarid[1] != globalgamesettings.game_opponentcarid[1] ||
                                oldcfg.game_opponentcarid[2] != globalgamesettings.game_opponentcarid[2] ||
                                oldcfg.game_opponentcarid[3] != globalgamesettings.game_opponentcarid[3])
                                i = 1;
                            else {
                                /* PLATFORM(file): check required files and request missing files. */
                                /* PLATFORM(input_joy): collect file-confirmation joystick input. */
                                /* PLATFORM(input_kb): collect file-confirmation key input. */
                                /* PLATFORM(input_mouse): collect file-confirmation pointer input. */
                                /* PLATFORM(timer): wait for file confirmation using timer ticks. */
                                /* PLATFORM(video): show missing-file guidance. */
                                ensure_file_exists(2);
                                opponentcompile();
                            }
                        }
                        if (i != 0) {
                            removegamefiles();
                            loadgamefiles();
                        }
                        rate_frame = globalgamesettings.game_framespersec;
                        initsim(-1);
                        break;
                    /* PLATFORM(audio): update engine audio from car state. */ case UI_PAUSE_MENU_SAVE_REPLAY:
                        myupdatesfx();
                        write_state = UI_SAVE_STATE_RETRY;
                        while (write_state == UI_SAVE_STATE_RETRY) {
                            /* PLATFORM(file): open the save-file interface. */
                            /* PLATFORM(input_joy): collect file-entry joystick input. */
                            /* PLATFORM(timer): wait for file entry using timer ticks. */
                            /* PLATFORM(video): draw the save-file interface. */
                            if (fsavereq(replay_file, ReplayName, getresl(main_data_file_addr, "rep")) != 0) {
                                /* PLATFORM(file): build a file path. */ makename(replay_file, ReplayName, ".rpl", buf_g_path);
                                write_state = 1;
                                g_is_busy = UI_SAVE_STATE_WRITE;
                                /* PLATFORM(file): check whether a file exists. */ if (findfirst(buf_g_path) != 0) {
                                    /* PLATFORM(input_joy): collect dialog joystick input. */
                                    /* PLATFORM(timer): wait for dialog input using timer ticks. */
                                    /* PLATFORM(video): render a modal dialog. */
                                    i = messagebox(2, 0, getresl(main_data_file_addr, "fex"), -1, -1, performGraphColor, 0, 0);
                                    if (i == -1)
                                        write_state = UI_SAVE_STATE_CANCEL;
                                    else if (i == 0)
                                        write_state = UI_SAVE_STATE_RETRY;
                                }
                                g_is_busy = 0;
                            } else
                                write_state = UI_SAVE_STATE_CANCEL;
                            if (write_state == UI_SAVE_STATE_WRITE) {
                                /* PLATFORM(file): write replay data to a file. */ button = savereplay(buf_g_path);
                                if (button != 0) {
                                    /* PLATFORM(input_joy): collect dialog joystick input. */
                                    /* PLATFORM(timer): wait for dialog input using timer ticks. */
                                    /* PLATFORM(video): render a modal dialog. */
                                    messagebox(1, 0, getresl(main_data_file_addr, "ser"), -1, -1, performGraphColor, 0, 0);
                                    write_state = UI_SAVE_STATE_RETRY;
                                }
                            }
                        }
                        break;
                    case UI_PAUSE_MENU_RESULTS:
                        finishgame(4, 0);
                        sigframe = 2;
                        break;
                    case UI_PAUSE_MENU_MAIN_MENU:
                        finishgame(4, 0);
                        didrace = 0;
                        sigframe = 2;
                        break;
                    case UI_PAUSE_MENU_DISPLAY_OPTIONS:
                        for (i = 0; i < 5; i++)
                            dialog_params[i] = 0;
                        if (globalgamesettings.game_opponenttype == 0)
                            dialog_params[4] = 1;
                        /* PLATFORM(input_joy): collect dialog joystick input. */
                        /* PLATFORM(timer): wait for dialog input using timer ticks. */
                        /* PLATFORM(video): render a modal dialog. */
                        answer = messagebox(2, 0, getresl(gamerptrs, "mdo"), -1, -1, dialog_frame_color, dialog_params, 0);
                        switch (answer) {
                        case UI_DISPLAY_MENU_DASHBOARD:
                            on_off_dash ^= 1;
                            break;
                        case UI_DISPLAY_MENU_REPLAY_BAR:
                            replaybar_toggle ^= 1;
                            break;
                        case UI_DISPLAY_MENU_CAMERA:
                            cammd++;
                            if (cammd == CAMERA_MODE_COUNT)
                                cammd = CAMERA_MODE_COCKPIT;
                            break;
                        /* PLATFORM(audio): pause and restore audio around the graphics menu. */
                        /* PLATFORM(input_joy): collect graphics-menu joystick input. */
                        /* PLATFORM(input_kb): collect graphics-menu key input. */
                        /* PLATFORM(input_mouse): collect graphics-menu pointer input. */
                        /* PLATFORM(timer): wait for graphics selection using timer ticks. */
                        /* PLATFORM(video): draw the graphics-level selection interface. */
                        case UI_DISPLAY_MENU_GRAPHICS:
                            msetrender();
                            break;
                        case UI_DISPLAY_MENU_FOLLOW_OPPONENT:
                            intraffic ^= 1;
                            break;
                        }
                        break;
                    }
                    /* PLATFORM(input_joy): wait for joystick input. */ /* PLATFORM(timer): wait for input using the timer interval. */ /* PLATFORM(video): update pointer display while waiting for input. */ clear_keyboardbuffer();
                    return;
                case UI_REPLAY_BUTTON_SEEK_FORWARD:
                    inrepflg = 1;
                    /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 0, 0);
                    /* PLATFORM(timer): read the elapsed timer interval. */ elticks();
                    travel = 20;
                    while (kbjoyflags & 0x30) {
                        j = travel / 50 + 3;
                        if (j > 100)
                            j = 100;
                        /* PLATFORM(timer): read the elapsed timer interval. */ time_delta = elticks();
                        i = time_delta * j;
                        travel += i;
                        if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                            travel = (I32)(globalgamesettings.game_recordedframes - tmr2) * 20;
                        doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, travel / 20 + tmr2);
                        /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ hurrykey(time_delta);
                    }
                    if (globalgamesettings.game_recordedframes - tmr2 < (unsigned)(travel / 20))
                        travel = (I32)(globalgamesettings.game_recordedframes - tmr2) * 20;
                    i = travel / 20 + tmr2;
                    if (i > globalgamesettings.game_recordedframes)
                        i = globalgamesettings.game_recordedframes;
                    gotosim(i);
                    tmr2 = i;
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 4, 0);
                    strcpyfar(&resbuftext, getresl(gamerptrs, "wai"));
                    /* PLATFORM(video): use the display text or sprite interface. */ if (statemgmtcpy != 0)
                        mergezones(active_ui_dirty_rect, shadowtext(&resbuftext, mycenter(&resbuftext), 100, dlg_colour, 0),
                                   active_ui_dirty_rect);
                    /* PLATFORM(video): use the display text or sprite interface. */ else
                        shadowtext(&resbuftext, mycenter(&resbuftext), 100, dlg_colour, 0);
                    while (core.game_frame != tmr2) {
                        movesim();
                        doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, tmr2);
                    }
                    /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ hurrykey(1000);
                    return;
                case UI_REPLAY_BUTTON_SEEK_BACKWARD:
                    inrepflg = 1;
                    /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 1, 0);
                    /* PLATFORM(timer): read the elapsed timer interval. */ elticks();
                    travel = 20;
                    while (kbjoyflags & 0x30) {
                        j = travel / 50 + 3;
                        if (j > 100)
                            j = 100;
                        /* PLATFORM(timer): read the elapsed timer interval. */ time_delta = elticks();
                        i = time_delta * j;
                        travel += i;
                        if ((unsigned)(travel / 20) > tmr2)
                            travel = (I32)tmr2 * 20;
                        doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, tmr2 - travel / 20);
                        /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ hurrykey(time_delta);
                    }
                    if ((unsigned)(travel / 20) > tmr2)
                        travel = (I32)tmr2 * 20;
                    j = travel / 20;
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 4, 0);
                    if (j != 0) {
                        strcpyfar(&resbuftext, getresl(gamerptrs, "wai"));
                        /* PLATFORM(video): use the display text or sprite interface. */ if (statemgmtcpy != 0)
                            mergezones(active_ui_dirty_rect, shadowtext(&resbuftext, mycenter(&resbuftext), 100, dlg_colour, 0),
                                       active_ui_dirty_rect);
                        /* PLATFORM(video): use the display text or sprite interface. */ else
                            shadowtext(&resbuftext, mycenter(&resbuftext), 100, dlg_colour, 0);
                        i = tmr2 - j;
                        gotosim(i);
                        tmr2 = i;
                        prev_sky = i - core.game_frame;
                        if (prev_sky != 0) {
                            i = prev_sky;
                            while (core.game_frame != tmr2) {
                                movesim();
                                i--;
                                doreplay(UI_REPLAY_OPERATION_DRAW, (I32)i * j / prev_sky + tmr2, tmr2);
                                /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ hurrykey(1);
                            }
                        }
                    }
                    doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                    /* PLATFORM(input_joy): poll joystick input. */ /* PLATFORM(input_kb): poll keyboard input. */ /* PLATFORM(input_mouse): poll mouse input. */ hurrykey(1000);
                    return;
                case UI_REPLAY_BUTTON_PLAY:
                    g_rplmodui = 0;
                    doreplay(2, 3, 0);
                    inrepflg = 0;
                    break;
                case UI_REPLAY_BUTTON_PAUSE:
                    inrepflg = 1;
                    /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                    doreplay(2, 4, 0);
                    doreplay(1, core.game_frame, core.game_frame);
                    doreplay(1, core.game_frame, core.game_frame);
                    continue;
                case UI_REPLAY_BUTTON_REWIND_START:
                    inrepflg = 1;
                    /* PLATFORM(audio): update engine audio from car state. */ myupdatesfx();
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 5, 0);
                    doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                    gotosim(0);
                    /* PLATFORM(timer): read the timer counter. */ timedwait(50);
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 4, 0);
                    doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                    return;
                case UI_REPLAY_BUTTON_DOUBLE_SPEED:
                    doreplay(UI_REPLAY_OPERATION_SET_PRESSED, 2, 0);
                    g_rplmodui = 3;
                    inrepflg = 0;
                    break;
                }
                doreplay(UI_REPLAY_OPERATION_DRAW, core.game_frame, core.game_frame);
                continue;
            case 0x1B:
                goto pause_menu;
            }
            return;
        }
    }
}

