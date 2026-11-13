#ifndef STUNTS_MEMORY_H
#define STUNTS_MEMORY_H

/* Segmented memory and resource heap. */
/* DOS paragraph allocation and resource heap geometry. */

#define DOS_PARAGRAPH_BYTES 16UL
#define DOS_PARAGRAPH_BYTES_I16 16
#define DOS_PARAGRAPH_OFFSET_MASK 15
#define DOS_PARAGRAPH_SHIFT 4
#define SHAPE_SHARED_MIN_FREE_BYTES 65000L
#define TRACK_ARENA_BYTES 0x6BF3
#define TRACK_ARENA_REPLAY_PATH_OFFSET 0x75B

/* Shared memapi declarations; unnamed prototype parameters keep C1 allocation stable. */
extern void far *far __fmemcpy(void far *, const void far *, unsigned);
extern void far *far adrtoseg(long);
extern void far *far checkmodule(const char *);
extern char *far mmgr_path_to_name(const char *);
extern void far purgememory(void far *);
extern void far *far reservememory(const char *, unsigned short);
extern long far segtoadr(unsigned char far *);

/* Memory services and shared state. */
extern void* _memcpy(void*, const void*, unsigned);
extern unsigned long availmem(void);
extern void far freememory(void far *);
extern char far *packmemory(char far *);
extern unsigned strlen(char *);
#endif
