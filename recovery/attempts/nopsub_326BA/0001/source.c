struct NOPSUB_ENTRY_BLOCK {
    char header[6];
    unsigned long values[1];
};

void nopsub_326BA(struct NOPSUB_ENTRY_BLOCK far *source,
                  unsigned index,
                  unsigned long *output)
{
    *output = source->values[index];
}
