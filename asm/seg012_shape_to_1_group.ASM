.8086
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33D0C
public _sprite_shape_to_1
public _sprite_shape_to_1_alt
_nopsub_33D0C proc far
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
    jmp     short loc_33D69
    ; align 2
    db 144
_nopsub_33D0C endp
_sprite_shape_to_1 proc far
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
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short loc_33D69
    ; align 2
    db 144
_sprite_shape_to_1 endp
_sprite_shape_to_1_alt proc far
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
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_33D69:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    cld
    mov     dx, [si+2]
    mov     bx, word ptr cs:[_sprite1+20]
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+var_6], ax
    jnb     short loc_33DA9
    jz      short loc_33DB5
loc_33D97:
    mov     cx, [bp+var_6]
    rep movsw
    movsb
    add     di, bx
    dec     dx
    jg      short loc_33D97
loc_33DA2:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33DA9:
    mov     cx, [bp+var_6]
    rep movsw
    add     di, bx
    dec     dx
    jg      short loc_33DA9
    jmp     short loc_33DA2
loc_33DB5:
    movsb
    add     di, bx
    dec     dx
    jg      short loc_33DB5
    jmp     short loc_33DA2
    ; align 2
    db 0
_sprite_shape_to_1_alt endp
S012_TEXT ends
end
