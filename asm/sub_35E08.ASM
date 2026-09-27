.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
SPRITE struc
sprite_bitmapptr dw ?
sprite_bitmapseg dw ?
sprite_reserved4 dw ?
sprite_reserved6 dw ?
sprite_reserved8 dw ?
sprite_lineofs dw ?
sprite_left dw ?
sprite_right dw ?
sprite_top dw ?
sprite_height dw ?
sprite_pitch dw ?
sprite_reserved16 dw ?
sprite_width2 dw ?
sprite_left2 dw ?
sprite_widthsum dw ?
SPRITE ends
extrn sprite1:word
public _sub_35E08
_sub_35E08 proc far
    var_14 = word ptr -20
    var_12 = word ptr -18
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
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = word ptr 14

    push    bp
    mov     bp, sp
loc_35E0B:
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
loc_35E14:
    mov     si, [bp+arg_2]
    mov     cx, [bp+arg_6]
    mov     ax, [si+4]
    imul    [bp+arg_0]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+var_2], cx
    mov     cx, [bp+arg_8]
    mov     ax, [si+6]
    imul    [bp+arg_0]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+var_4], cx
    jmp     short loc_35E7A
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short loc_35E7A
    ; align 2
    db 144
loc_35E58:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
loc_35E7A:
    cmp     [bp+arg_0], 2
    jb      short loc_35E58
    cld
    mov     ax, [si+2]
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short loc_35E58
    mov     [bp+var_8], ax
    mov     ax, [si]
    mov     [bp+var_A], ax
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short loc_35E58
    mov     [bp+var_6], ax
    mov     dx, cs:sprite1.sprite_pitch
    sub     dx, ax
    mov     [bp+var_C], dx
    add     si, 10h
    mov     dx, 1
    xor     ax, ax
    mov     [bp+var_14], ax
    mov     [bp+var_12], ax
    div     [bp+arg_0]
    mov     dx, ax
    mov     al, ah
    xor     ah, ah
    shr     ax, 1
    jz      short loc_35ED3
    add     si, ax
    mov     cx, ax
loc_35ECE:
    add     si, [bp+var_A]
    loop    loc_35ECE
loc_35ED3:
    mov     [bp+var_E], si
    mov     [bp+var_10], dx
    var_14 = word ptr -20
    var_12 = word ptr -18
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
    arg_0 = word ptr 6
    arg_shapeptr = dword ptr 8
    arg_6 = word ptr 12
    arg_8 = word ptr 14
loc_35ED9:
    mov     ax, word ptr cs:sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:sprite1.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_10]
    mov     ax, [bp+var_12]
    mov     ss:54AAh, ax
loc_35EF9:
    mov     cx, [bp+var_6]
    mov     bx, [bp+var_14]
loc_35EFF:
    mov     al, [si]
    cmp     al, 0FFh
    jz      short loc_35F08
    mov     es:[di], al
loc_35F08:
    inc     di
    add     bx, dx
    mov     al, bh
    xor     ah, ah
    add     si, ax
    xor     bh, bh
    loop    loc_35EFF
    dec     [bp+var_8]
    jle     short loc_35F40
    add     di, [bp+var_C]
    mov     si, [bp+var_E]
    add     ss:54AAh, dx
    mov     cl, ss:54ABh
    or      cl, cl
    jz      short loc_35EF9
    xor     ch, ch
loc_35F30:
    add     si, [bp+var_A]
    loop    loc_35F30
    mov     byte ptr ss:54ABh, 0
    mov     [bp+var_E], si
    jmp     short loc_35EF9
loc_35F40:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 0
_sub_35E08 endp
S012_TEXT ends
end
