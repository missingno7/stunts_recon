.8086
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
MEMCHUNK struc
    resname db 12 dup (?)
    ressize dw ?
    resofs dw ?
    resunk dw ?
MEMCHUNK ends
extrn _resendptr2:word
extrn _word_3FF82:word
extrn _mmgr_alloc_resmem:far
_TEXT segment word public 'CODE'
public _nopsub_310FE
_nopsub_310FE proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, 0A000h
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    mov     ax, [bp+arg_0]
    mov     bx, _resendptr2
    sub     [bx+MEMCHUNK.resofs], ax
    sub     _word_3FF82, ax
    pop     bp
    retf
_nopsub_310FE endp
_TEXT ends
end
