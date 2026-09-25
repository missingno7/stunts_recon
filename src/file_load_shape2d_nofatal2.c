extern void far* file_load_shape2d_nofatal_thunk(char* shapename);
void far* file_load_shape2d_nofatal2(char* shapename) { return file_load_shape2d_nofatal_thunk(shapename); }
