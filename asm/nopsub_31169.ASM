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
extrn _resptr1:word
extrn _resptr2:word
S012_TEXT segment byte public 'CODE'
public _nopsub_31169
_nopsub_31169 proc far
    mov     bx, _resptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    add     ax, [bx+MEMCHUNK.ressize]
    mov     bx, _resptr1
    sub     ax, [bx+MEMCHUNK.resofs]
    retf
_nopsub_31169 endp
S012_TEXT ends
end
