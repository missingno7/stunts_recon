/* reviewed_function */
/* Documentation mentions old_function without making it a label. */
extern int reviewed_function(void);
int longer_old_function_name;
char *spelling = "old_function";
int caller(void) { return reviewed_function(); }
