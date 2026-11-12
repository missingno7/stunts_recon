/* Shared memapi declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_MEMAPI_H
#define STUNTS_MEMAPI_H
extern void far *far __fmemcpy(void far *, const void far *, unsigned);
extern void far *far adrtoseg(long);
extern void far *far checkmodule(const char *);
extern char *far mmgr_path_to_name(const char *);
extern void far purgememory(void far *);
extern void far *far reservememory(const char *, unsigned short);
extern long far segtoadr(unsigned char far *);
#endif
