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
extrn _fontdefseg:word
_TEXT segment word public 'CODE'
public _font_op
public _font_op2
_font_op proc far
     font_op_s = byte ptr 0
     font_op_r = byte ptr 2
    font_op_arg_4 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, [bp+font_op_arg_4]
    or      dx, dx
    jnz     short font_op2_when_nonzero
    xor     ax, ax
    jmp     short font_op2_shared_path
    db 144
_font_op endp
_font_op2 proc far
     font_op2_s = byte ptr 0
     font_op2_r = byte ptr 2
    font_op2_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    xor     dx, dx
font_op2_when_nonzero:
    mov     es, _fontdefseg
    mov     si, [bp+font_op2_arg_0]
    xor     ax, ax
    mov     cl, es:14h
    mov     di, es:10h
font_op2_draw_loop:
    mov     bl, [si]
    or      bl, bl
    jz      short font_op2_shared_path
    inc     si
    xor     bh, bh
    shl     bx, 1
    mov     bx, es:[bx+16h]
    or      bx, bx
    jz      short font_op2_draw_loop
    or      cl, cl
    jz      short font_op2_when_zero
    mov     bl, es:[bx]
    xor     bh, bh
    mov     di, bx
font_op2_when_zero:
    add     ax, di
    dec     dx
    jz      short font_op2_shared_path
    jmp     short font_op2_draw_loop
font_op2_shared_path:
    pop     di
    pop     si
    pop     bp
    retf
_font_op2 endp
_TEXT ends
end
