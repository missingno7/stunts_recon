/* Replay state and stream operations. */
#ifndef STUNTS_REPLAY_H
#define STUNTS_REPLAY_H
extern char cam_idg;
extern unsigned char replay_steer_flag[64];
extern char g_replaybarcpytgl;
extern char far *g_tdreplay16buf;
extern char replay_axis_magnitude[34];
extern char replay_state_cache;
extern char replaybar_toggle;
extern int replayrst;
extern char far *td13_replay_hdr;
extern void far doreplay(int, int, int);
extern char far replay_axis_value(void);
#endif
