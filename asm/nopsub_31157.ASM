.8086
MEMCHUNK struc
    resname db 12 dup (?)
    ressize dw ?
    resofs dw ?
    resunk dw ?
MEMCHUNK ends
_TEXT segment word public 'CODE'
_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:_TEXT, ds:DGROUP
extrn _resendptr1:word
extrn _resptr2:word
_TEXT segment word public 'CODE'
public _nopsub_31157
_nopsub_31157 proc far
    mov     bx, _resendptr1
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_nopsub_31157 endp
_TEXT ends
end
