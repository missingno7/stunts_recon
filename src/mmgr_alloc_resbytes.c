extern void far *mmgr_alloc_pages(const char *name, unsigned short paras);

void far *mmgr_alloc_resbytes(const char *name, long int size)
{
    return mmgr_alloc_pages(name, size / 16 + 1);
}
