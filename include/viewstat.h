/* View and clip state. */
#ifndef STUNTS_VIEWSTAT_H
#define STUNTS_VIEWSTAT_H
extern struct SPRITE far *g_wndspr;
extern void far *fntndat;
extern short cliprects_spare4[120];
extern struct RECTANGLE rectclip[15];
extern int rotpr[2];
extern int prevcamrot;
extern char g_simprect;
extern int rcmapix[45];
extern char rect_num3;
extern char rects_updt[15];
extern struct RECTANGLE full_screen_rectangle;
#endif
