.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _callbacks:byte
extrn _callbackflags:byte
extrn _callbackflags2:byte
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_reg_callback, _nopsub_304AF
_kb_reg_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    mov     cx, 40h ; '@'
    mov     bx, offset _callbacks
    mov     dx, [bp+arg_4]
loc_3046D:
    cmp     [bx], ax
    jnz     short loc_30476
    cmp     [bx+2], dx
    jz      short loc_30488
loc_30476:
    cmp     word ptr [bx+2], 0
    jz      short loc_30483
    add     bx, 4
    loop    loc_3046D
    pop     bp
    retf
loc_30483:
    mov     [bx], ax
    mov     [bx+2], dx
loc_30488:
    mov     ax, 41h ; 'A'
    sub     ax, cx
loc_3048D:
    mov     bx, [bp+arg_0]
    or      bl, bl
    jz      short loc_3049F
    cmp     bx, 7Fh ; ''
    jg      short loc_3049D
    mov     _callbackflags[bx], al
loc_3049D:
    pop     bp
    retf
loc_3049F:
    mov     bl, bh
    xor     bh, bh
    cmp     bx, 84h ; ''
    jg      short loc_3049D
    mov     _callbackflags2[bx], al
    pop     bp
    retf

_kb_reg_callback endp
_nopsub_304AF proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    xor     ax, ax
    jmp     short loc_3048D

_nopsub_304AF endp
_TEXT ends
end
