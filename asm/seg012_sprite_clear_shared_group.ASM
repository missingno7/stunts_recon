.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_34736
public _sprite_clear_shape_alt
public _sprite_clear_shape
_nopsub_34736 proc far
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
    mov     di, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [di+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [di+6]
    mov     [bp+var_4], ax
    jmp     short loc_34799
    ; align 2
    db 144
_nopsub_34736 endp
_sprite_clear_shape_alt proc far
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     di, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     [di+8], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    mov     [di+0Ah], ax
    jmp     short loc_34799
    ; align 2
    db 144
_sprite_clear_shape_alt endp
_sprite_clear_shape proc far
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     di, [bp+arg_0]
    mov     ax, [di+8]
    mov     [bp+var_2], ax
    mov     ax, [di+0Ah]
    mov     [bp+var_4], ax
loc_34799:
    mov     ax, [di]
    mov     [bp+var_8], ax
    mov     ax, [di+2]
    mov     [bp+var_A], ax
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     [bp+var_6], bx
    mov     ds, word ptr word ptr cs:[5F22h]
    mov     es, [bp+arg_2]
    add     di, 10h
    cld
    mov     dx, [bp+var_A]
loc_347C0:
    mov     bx, [bp+var_6]
    mov     si, cs:[bx]
    add     si, [bp+var_2]
    mov     cx, [bp+var_8]
    rep movsb
    add     [bp+var_6], 2
    dec     dx
    jg      short loc_347C0
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_clear_shape endp
_TEXT ends
end
