.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _sprite_putimage_and_alt2
public _sprite_putimage_and
_sprite_putimage_and_alt2 proc far
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
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short loc_338C9
    ; align 2
    db 144
_sprite_putimage_and_alt2 endp
_sprite_putimage_and proc far
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
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
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short loc_338C9
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_338C9:
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, word ptr cs:[5F30h]
    jl      short loc_338EF
    add     ax, cx
    sub     ax, word ptr cs:[5F32h]
    jle     short loc_33916
    sub     cx, ax
    jg      short loc_33916
loc_338E8:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_338EF:
    mov     bx, word ptr cs:[5F30h]
    add     ax, cx
    sub     ax, bx
    jle     short loc_338E8
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[5F32h]
    jle     short loc_33916
    sub     cx, ax
    jle     short loc_338E8
loc_33916:
    mov     [bp+var_C], dx
    mov     [bp+var_A], cx
    mov     [bp+var_4], bx
    mov     cx, [si]
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    cmp     ax, word ptr cs:[5F2Ch]
    jl      short loc_33941
    add     ax, cx
    sub     ax, word ptr cs:[5F2Eh]
    jl      short loc_33968
    sub     cx, ax
    jle     short loc_338E8
    mov     dx, ax
    jmp     short loc_33968
    db 144
loc_33941:
    mov     bx, word ptr cs:[5F2Ch]
    add     ax, cx
    sub     ax, bx
    jle     short loc_338E8
    mov     si, cx
    sub     si, ax
    add     [bp+var_C], si
    mov     si, word ptr cs:[5F2Eh]
    sub     si, bx
    jle     short loc_338E8
    cmp     ax, si
    jl      short loc_33962
    mov     ax, si
loc_33962:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
loc_33968:
    or      cx, cx
    jle     short loc_339B1
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+var_2], bx
    cld
    mov     es, word ptr word ptr cs:[5F22h]
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     si, [bp+var_C]
    mov     dx, [bp+var_A]
    mov     bx, word ptr cs:[5F34h]
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short loc_339B4
loc_3399F:
    mov     cx, [bp+var_6]
loc_339A2:
    lodsb
    and     es:[di], al
    inc     di
    loop    loc_339A2
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short loc_3399F
loc_339B1:
    jmp     loc_338E8
loc_339B4:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short loc_339D8
    jz      short loc_339EB
    inc     bx
loc_339C1:
    mov     cx, [bp+var_E]
loc_339C4:
    lodsw
    and     es:[di], ax
    inc     di
    inc     di
    loop    loc_339C4
    lodsb
    and     es:[di], al
    add     di, bx
    dec     dx
    jg      short loc_339C1
    jmp     loc_338E8
loc_339D8:
    mov     cx, [bp+var_E]
loc_339DB:
    lodsw
    and     es:[di], ax
    inc     di
    inc     di
    loop    loc_339DB
    add     di, bx
    dec     dx
    jg      short loc_339D8
    jmp     loc_338E8
loc_339EB:
    inc     bx
loc_339EC:
    lodsb
    and     es:[di], al
    add     di, bx
    inc     bx
    dec     dx
    jg      short loc_339EC
    jmp     loc_338E8
    ; align 2
    db 0
_sprite_putimage_and endp
_TEXT ends
end
