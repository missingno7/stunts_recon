extern void far *game1ptr;
extern void far *game2ptr;
extern void far mmgr_free(void far *);

void shape3d_free_all(void)
{
    if (game1ptr != 0) {
        mmgr_free(game1ptr);
    }
    if (game2ptr != 0) {
        mmgr_free(game2ptr);
    }
}
