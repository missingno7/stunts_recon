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
extrn _sinetable:word
_TEXT segment word public 'CODE'
public _sin_fast
public _cos_fast
_sin_fast proc far
     sin_fast_s = byte ptr 0
     sin_fast_r = byte ptr 2
    sin_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+sin_fast_arg_0]
code_sin_fast_main:
    mov     bl, ah
    xor     ah, ah
    and     bx, 3
    shl     bx, 1
    jmp     cs:off_326F2[bx]
off_326F2     dw offset loc_326FA
    dw offset loc_32704
    dw offset loc_32711
    dw offset loc_3271D
loc_326FA:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
loc_32704:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
loc_32711:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
loc_3271D:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
_sin_fast endp
_cos_fast proc far
     cos_fast_s = byte ptr 0
     cos_fast_r = byte ptr 2
    cos_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+cos_fast_arg_0]
    add     ax, 100h
    jmp     short code_sin_fast_main

    db 0
_cos_fast endp
_TEXT ends
end
