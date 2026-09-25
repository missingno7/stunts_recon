struct chunk { char pad[12]; unsigned short ressize; unsigned short resofs; };
extern struct chunk *resendptr2;
extern struct chunk *resptr2;
unsigned short mmgr_get_ofs_diff(void) {
 return resendptr2->resofs - resptr2->resofs - resptr2->ressize;
}
