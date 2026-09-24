extern void far* mmgr_free(char far* ptr);
void far unload_resource(void far* resptr) {
    mmgr_free((char far*)resptr);
}