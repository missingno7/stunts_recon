.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _sprite1:byte
extrn _word_2F3C6:word
extrn _word_30602:word
extrn _draw_filled_lines:far
extrn _preRender_sphere_helper:far
extrn _putpixel_single_maybe:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _nopsub_33006
public _vector_op_unk
public _preRender_sphere
public _nopsub_3320E
public _sprite_set_1_size
public _video_clear_color
_nopsub_33006 proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     ax, ds:_word_30602
    jmp     short loc_3301F
    ; align 2
    db 144
_nopsub_33006 endp
_vector_op_unk proc far
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
    sub     sp, 4
    push    si
    push    di
    mov     ax, [bp+arg_6]
loc_3301F:
    mov     si, [bp+arg_0]
    mov     bx, [bp+arg_2]
    mov     di, [bp+arg_4]
    mov     word ptr [di+4], ax
    sub     ax, word ptr [bx+4]
    mov     [bp+var_4], ax
    mov     ax, word ptr [si+4]
    sub     ax, word ptr [bx+4]
    or      ax, ax
    jge     short loc_33040
    shr     [bp+var_4], 1
    shr     ax, 1
loc_33040:
    mov     [bp+var_2], ax
    mov     ax, word ptr [si+0]
    sub     ax, word ptr [bx+0]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+0]
    mov     word ptr [di+0], ax
    mov     ax, word ptr [si+2]
    sub     ax, word ptr [bx+2]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+2]
    mov     word ptr [di+2], ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_vector_op_unk endp
unmapped_143465:
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     ds:_word_30602, ax
_preRender_sphere proc far
    push bp
    mov bp, sp
    sub sp, 79Ah
    push si
    push di
    mov dx, word ptr [bp + 0Ah]
    mov ax, dx
    shr ax, 1
    shr ax, 1
    sub dx, ax
    shr ax, 1
    shr ax, 1
    add dx, ax
    or dx, dx
    jg loc_23096
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_23096:
    mov word ptr [bp - 6], dx
    mov bx, dx
    shr dx, 1
    jne loc_230B6
    push word ptr [bp + 0Ch]
    push word ptr [bp + 8]
    push word ptr [bp + 6]
    call far ptr _putpixel_single_maybe
    add sp, 6
loc_230B0:
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_230B6:
    sub bx, dx
    mov ax, word ptr cs:[_sprite1+26]
    mov word ptr [bp - 4], ax
    mov ax, word ptr cs:[_sprite1+28]
    dec ax
    mov word ptr [bp - 2], ax
    mov cx, word ptr [bp + 8]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr cs:[_sprite1+18]
    jge loc_230B0
    mov word ptr [bp - 14h], ax
    add cx, bx
    cmp cx, word ptr cs:[_sprite1+16]
    jle loc_230B0
    mov dx, bx
    mov ax, dx
    shr ax, 1
    shr ax, 1
    add dx, ax
    mov cx, word ptr [bp + 6]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg loc_230B0
    add cx, dx
    cmp cx, word ptr [bp - 4]
    jl loc_230B0
    cmp bx, word ptr ds:[_word_2F3C6]
    jl loc_23148
    mov ax, word ptr [bp + 6]
    mov word ptr [bp - 12h], ax
    mov word ptr [bp - 0Eh], ax
    mov dx, word ptr [bp + 0Ah]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ah], ax
    mov ax, word ptr [bp + 8]
    mov word ptr [bp - 10h], ax
    mov word ptr [bp - 8], ax
    mov dx, word ptr [bp - 6]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ch], ax
    mov dx, word ptr [bp + 0Ch]
    lea ax, [bp - 12h]
    add sp, 780h
    push dx
    push ax
    call far ptr _preRender_sphere_helper
    add sp, 4
    sub sp, 780h
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_23148:
    shl bx, 1
    mov ax, word ptr [bx + 3C58h]
    mov word ptr [bp - 16h], ax
    mov ax, word ptr [bp - 6]
    mov word ptr [bp - 1Ah], ax
    dec ax
    shl ax, 1
    mov word ptr [bp - 18h], ax
    lea si, [bp - 3DAh]
    lea di, [bp - 79Ah]
loc_23165:
    mov bx, word ptr [bp - 16h]
    mov dl, byte ptr [bx]
    inc word ptr [bp - 16h]
    xor dh, dh
    mov bx, word ptr [bp - 18h]
    mov ax, word ptr [bp + 6]
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg loc_231AD
    cmp ax, word ptr [bp - 4]
    jge loc_23184
    mov ax, word ptr [bp - 4]
loc_23184:
    mov word ptr [si], ax
    mov word ptr [bx + si], ax
    mov ax, word ptr [bp + 6]
    add ax, dx
    cmp ax, word ptr [bp - 4]
    jl loc_231AD
    cmp ax, word ptr [bp - 2]
    jle loc_2319A
    mov ax, word ptr [bp - 2]
loc_2319A:
    mov word ptr [di], ax
    mov word ptr [bx + di], ax
    add si, 2
    add di, 2
    sub word ptr [bp - 18h], 4
    jge loc_23165
    jmp loc_231C0
loc_231AD:
    inc word ptr [bp - 14h]
    sub word ptr [bp - 1Ah], 2
    sub word ptr [bp - 18h], 4
    jge loc_23165
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_231C0:
    xor dx, dx
    mov ax, word ptr cs:[_sprite1+16]
    sub ax, word ptr [bp - 14h]
    jle loc_231D9
    sub word ptr [bp - 1Ah], ax
    shl ax, 1
    mov dx, ax
    mov ax, word ptr cs:[_sprite1+16]
    mov word ptr [bp - 14h], ax
loc_231D9:
    mov ax, word ptr [bp - 14h]
    add ax, word ptr [bp - 1Ah]
    sub ax, word ptr cs:[_sprite1+18]
    jle loc_231E9
    sub word ptr [bp - 1Ah], ax
loc_231E9:
    push word ptr [bp + 0Ch]
    push word ptr [bp - 1Ah]
    push word ptr [bp - 14h]
    lea ax, [bp - 79Ah]
    add ax, dx
    push ax
    lea ax, [bp - 3DAh]
    add ax, dx
    push ax
    call far ptr _draw_filled_lines
    add sp, 0Ah
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
_preRender_sphere endp
_nopsub_3320E proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = word ptr 14
    arg_A = word ptr 16

    push    bp
    mov     bp, sp
    push    ds
    push    si
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+2]
    cmp     word ptr cs:[_sprite1+2], ax
    jnz     short loc_3325D
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     cs:[_sprite1+26], ax
    mov     [si+0Ch], ax
    mov     cs:[_sprite1+12], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     cs:[_sprite1+28], ax
    mov     [si+0Eh], ax
    mov     cs:[_sprite1+14], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     cs:[_sprite1+16], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    mov     cs:[_sprite1+18], ax
    pop     si
    pop     ds
    pop     bp
    retf
loc_3325D:
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     [si+0Ch], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     [si+0Eh], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    pop     si
    pop     ds
    pop     bp
    retf
_nopsub_3320E endp
_sprite_set_1_size proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     cs:[_sprite1+26], ax
    mov     cs:[_sprite1+12], ax
    mov     ax, [bp+arg_2]
    mov     cs:[_sprite1+28], ax
    mov     cs:[_sprite1+14], ax
    mov     ax, [bp+arg_4]
    mov     cs:[_sprite1+16], ax
    mov     ax, [bp+arg_6]
    mov     cs:[_sprite1+18], ax
    pop     bp
    retf
_sprite_set_1_size endp
_video_clear_color proc far
    push bp
    mov bp,sp
    push di
    mov ax,0A000h
    mov es,ax
    mov ax,word ptr [bp+6]
    mov di,0
    mov cx,0FA00h
    rep stosw
    pop di
    pop bp
    retf
    db 0
_video_clear_color endp
S012_TEXT ends
end
