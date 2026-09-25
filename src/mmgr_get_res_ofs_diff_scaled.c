extern unsigned short mmgr_get_ofs_diff(void);

unsigned long mmgr_get_res_ofs_diff_scaled(void)
{
    return ((unsigned long)mmgr_get_ofs_diff()) << 4;
}
