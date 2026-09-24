extern void far* mmgr_free(char far* ptr);
extern char far *sdgame2ptr;
void free_sdgame2(void) { mmgr_free(sdgame2ptr); }
