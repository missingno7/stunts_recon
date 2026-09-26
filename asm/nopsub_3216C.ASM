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
extrn _byte_4032A:byte
_TEXT segment word public 'CODE'
public _nopsub_3216C
_nopsub_3216C proc far
     r = byte ptr 0

    push    si
    push    di
    push    ds
    mov     al, _byte_4032A
    mov     bl, 0A0h
    mul     bl
    mov     si, ax
    mov     di, si
    add     si, 0A0h
    mov     ax, 18h
    sub     al, _byte_4032A
    mov     bl, 50h
    mul     bl
    mov     cx, ax
    mov     ax, es
    mov     ds, ax
    cld
    rep movsw
    xor     ax, ax
    mov     cx, 50h
    rep stosw
    pop     ds
    pop     di
    pop     si
    retf
_nopsub_3216C endp
_TEXT ends
end
