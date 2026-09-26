void copy_material_list_pointers(void*, void*, void*, void*, unsigned short);
extern int* material_clrlist_ptr_cpy;
extern int* material_clrlist2_ptr_cpy;
extern int* material_patlist_ptr_cpy;
extern int* material_patlist2_ptr_cpy;
extern unsigned short someZeroVideoConst;

void copy_material_list_pointers(void* clrlist, void* clrlist2, void* patlist, void* patlist2, unsigned short videoConst)
{
    material_clrlist_ptr_cpy = clrlist;
    material_clrlist2_ptr_cpy = clrlist2;
    material_patlist_ptr_cpy = patlist;
    material_patlist2_ptr_cpy = patlist2;
    someZeroVideoConst = videoConst;
}
