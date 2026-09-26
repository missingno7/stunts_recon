.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_33DBE
public _shape2d_op_unk5
public _shape2d_op_unk
public _nopsub_33E90
public _shape2d_op_unk2
public _shape2d_op_unk3
_nopsub_33DBE proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short loc_33E1B
    ; align 2
    db 144
_nopsub_33DBE endp
_shape2d_op_unk5 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short loc_33E1B ; goto shape2d_op_unk3
    ; align 2
    db 144
_shape2d_op_unk5 endp
_shape2d_op_unk proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_shapeseg = word ptr 6
    arg_shapeofs = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shapeofs]
    mov     si, [bp+arg_shapeseg]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_33E1B:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
loc_33E27:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr word ptr cs:[5F22h]
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_6]
loc_33E46:
    lodsb
    or      al, al
    jg      short loc_33E54
    jl      short loc_33E71
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33E54:
    mov     cl, al
    xor     ch, ch
    lodsb
loc_33E59:
    stosb
    dec     dx
    jg      short loc_33E6D
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
loc_33E6D:
    loop    loc_33E59
    jmp     short loc_33E46
loc_33E71:
    neg     al
    mov     cl, al
    xor     ch, ch
loc_33E77:
    movsb
    dec     dx
    jg      short loc_33E8B
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
loc_33E8B:
    loop    loc_33E77
    jmp     short loc_33E46
    ; align 2
    db 0
_shape2d_op_unk endp
_nopsub_33E90 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short loc_33EED
    ; align 2
    db 144
_nopsub_33E90 endp
_shape2d_op_unk2 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short loc_33EED ; goto somewhere inside shape2d_op_unk3
    ; align 2
    db 144
_shape2d_op_unk2 endp
_shape2d_op_unk3 proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_shape2dofs = word ptr 6
    arg_shape2dseg = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape2dseg]
    mov     si, [bp+arg_shape2dofs]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_33EED:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
    xor     di, di
    xor     dx, dx
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    mov     ax, bx
    cmp     ax, word ptr cs:[5F30h]
    jl      short loc_33F21
    add     ax, cx
    sub     ax, word ptr cs:[5F32h]
    jle     short loc_33F44
    inc     di
    sub     cx, ax
    jg      short loc_33F44
loc_33F1A:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33F21:
    inc     di
    mov     bx, word ptr cs:[5F30h]
    add     ax, cx
    sub     ax, bx
    jle     short loc_33F1A
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+var_6]
    mov     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[5F32h]
    jle     short loc_33F44
    sub     cx, ax
    jle     short loc_33F1A
loc_33F44:
    mov     [bp+var_8], cx
    mov     [bp+var_4], bx
    mov     [bp+var_A], dx
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    mov     cx, [bp+var_6]
    cmp     ax, word ptr cs:[5F2Ch]
    jl      short loc_33F71
    add     ax, cx
    sub     ax, word ptr cs:[5F2Eh]
    jle     short loc_33F97
    sub     cx, ax
    jle     short loc_33F1A
    inc     di
    mov     dx, ax
    jmp     short loc_33F97
    db 144
loc_33F71:
    inc     di
    mov     bx, word ptr cs:[5F2Ch]
    add     ax, cx
    sub     ax, bx
    jle     short loc_33F1A
    mov     si, cx
    sub     si, ax
    add     [bp+var_A], si
    mov     si, word ptr cs:[5F2Eh]
    sub     si, bx
    cmp     ax, si
    jl      short loc_33F91
    mov     ax, si
loc_33F91:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
loc_33F97:
    mov     [bp+var_6], cx
    mov     [bp+var_C], dx
    mov     [bp+var_2], bx
    or      di, di
    jnz     short loc_33FA9
    jmp far ptr loc_33E27
loc_33FA9:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr word ptr cs:[5F22h]
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_A]
    or      dx, dx
    jnz     short loc_34042
loc_33FCC:
    mov     dx, [bp+var_6]
loc_33FCF:
    lodsb
    or      al, al
    jg      short loc_33FDD
    jl      short loc_3400B
loc_33FD6:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33FDD:
    mov     cl, al
    xor     ch, ch
    lodsb
loc_33FE2:
    stosb
    dec     dx
    jle     short loc_33FEA
    loop    loc_33FE2
    jmp     short loc_33FCF
loc_33FEA:
    dec     [bp+var_8]
    jz      short loc_33FD6
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    dec     cx
    mov     dx, [bp+var_C]
loc_34000:
    sub     cx, dx
    jz      short loc_33FCC
    jl      short loc_3403E
    mov     dx, [bp+var_6]
    jmp     short loc_33FE2
loc_3400B:
    neg     al
    mov     cl, al
    xor     ch, ch
loc_34011:
    movsb
    dec     dx
    jle     short loc_34019
    loop    loc_34011
    jmp     short loc_33FCF
loc_34019:
    dec     [bp+var_8]
    jz      short loc_33FD6
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    dec     cx
    mov     dx, [bp+var_C]
loc_3402F:
    add     si, dx
    sub     cx, dx
    jz      short loc_33FCC
    jl      short loc_3403C
    mov     dx, [bp+var_6]
    jmp     short loc_34011
loc_3403C:
    add     si, cx
loc_3403E:
    neg     cx
    mov     dx, cx
loc_34042:
    lodsb
    or      al, al
    jg      short loc_34050
    jl      short loc_34057
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_34050:
    mov     cl, al
    xor     ch, ch
    lodsb
    jmp     short loc_34000
loc_34057:
    neg     al
    mov     cl, al
    xor     ch, ch
    jmp     short loc_3402F
    ; align 2
    db 0
_shape2d_op_unk3 endp
_TEXT ends
end
