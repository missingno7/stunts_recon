extern int polyinfonumpolys;
extern int word_31850;
extern int word_40ECE;
extern int word_411F6;
extern int poly_linklist_40ED6_iter2;
void polyinfo_reset(void)
{
    polyinfonumpolys = 0;
    word_31850 = 0;
    word_40ECE = 0;
    word_411F6 = 0xffff;
    poly_linklist_40ED6_iter2 = 0x190;
}
