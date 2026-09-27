.8086
MEMCHUNK struc
    resname db 12 dup (?)
    ressize dw ?
    resofs dw ?
    resunk dw ?
MEMCHUNK ends
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _resendptr2:word
extrn _resptr2:word
S012_TEXT segment byte public 'CODE'
public _mmgr_get_ofs_diff
_mmgr_get_ofs_diff proc far
    mov     bx, _resendptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_mmgr_get_ofs_diff endp
S012_TEXT ends
end
