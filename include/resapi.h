/* Shared resapi declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_RESAPI_H
#define STUNTS_RESAPI_H
struct ENGINESOUND;
extern const char aLocateshape4_4sShapeNotF[];
extern const char aSFileError[];
extern const char aSFileError_0[];
extern const char aSFileError_1[];
extern char far *carcopyresourceptr;
extern void far *eng1resourceptr;
extern char *findfilenames[4];
extern void far *g_game13dresource;
extern void far *g_miscfile_ptr;
extern void far *g_opp_resources[7];
extern struct ENGINESOUND player_engine_profile;
extern char replay_file[82];
extern void far *resource_ptropp;
extern void far *sdgresourcehandle;
extern void far ensure_file_exists(int);
extern const char *far myfindnext(void);
extern void far file_find_shapes_optional_alt(char far *, char *, char far **);
extern unsigned int far file_get_unflip_size(char far *);
extern void far *far myload(int, const char *);
extern signed char far load_intro_resources(void);
extern void far *far load_sfx_file(char *);
extern void far *far load_sfx_ge(char *, char *, char *);
extern void far loaddrawbits(void);
extern void far *far loadfilea(const char *, int);
extern void far loadfileat(char *, unsigned char far *);
extern void far *far loadfileatz(unsigned int, unsigned int, unsigned int);
extern void far *far loadfilez(char *);
extern int far loadgamefiles(void);
extern void far loading(void);
extern void far *far loadpacka(const char *, int);
extern void far *far loadpackz(char *);
extern void far locateshapesz(char far *, char *, char far **);
extern void far tmaskxf(void far *);
extern void far removegamefiles(void);
#endif
