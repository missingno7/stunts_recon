/* Shared resrec declarations; DOS medium-model types. */
#ifndef STUNTS_RESREC_H
#define STUNTS_RESREC_H
union FARRESOURCE;
#pragma pack()
union FARRESOURCE {
    void far *pointer;
    struct  { unsigned short offset; unsigned short segment; } word;
};
#endif
