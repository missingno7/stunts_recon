.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_339FA
public _putpixel_iconMask
_nopsub_339FA proc far
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
    sub     sp, 0Ah
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
    jmp     short loc_33A57
    ; align 2
    db 144
_nopsub_339FA endp
_putpixel_iconMask proc far
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short loc_33A57
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_33A57:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     es, word ptr word ptr cs:[5F22h]
    cld
    mov     dx, [si+2]
    mov     bx, word ptr cs:[5F34h]
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+var_6], ax
    jnb     short loc_33AA1
    jz      short loc_33AB3
    inc     bx
loc_33A86:
    mov     cx, [bp+var_6]
loc_33A89:
    lodsw
    and     es:[di], ax
    inc     di
    inc     di
    loop    loc_33A89
    lodsb
    and     es:[di], al
    add     di, bx
    dec     dx
    jg      short loc_33A86
loc_33A9A:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33AA1:
    mov     cx, [bp+var_6]
loc_33AA4:
    lodsw
    and     es:[di], ax
    inc     di
    inc     di
    loop    loc_33AA4
    add     di, bx
    dec     dx
    jg      short loc_33AA1
    jmp     short loc_33A9A
loc_33AB3:
    inc     bx
loc_33AB4:
    lodsb
    and     es:[di], ax
    add     di, bx
    dec     dx
    jg      short loc_33AB4
    jmp     short loc_33A9A
    ; align 2
    db 0
_putpixel_iconMask endp
_TEXT ends
end
