extern int far sin_fast(unsigned short);
extern int far cos_fast(unsigned short);
extern long var_6118;
extern long var_6114;
extern long var_6120;
extern long var_611c;
void calc_sincos80(void) {
    var_6118 = (long)sin_fast(0x80);
    var_6114 = (long)cos_fast(0x80);
    var_6120 = (long)sin_fast(0x80);
    var_611c = (long)cos_fast(0x80);
}
