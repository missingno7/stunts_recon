.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _mmgr_alloc_pages:far
extrn _fatal_error:far
public _sprite_make_wnd
MCGA_WINDOW_TEXT equ 541Ch
WINDOW_SPACE_ERROR_TEXT equ 5428h
NEXT_WND_DEF equ 62C4h
WND_DEFS_END equ 70D6h
SHAPE2D struc
s2d_width dw ?
s2d_height dw ?
s2d_pos_x dw ?
s2d_pos_y dw ?
s2d_unk1 dw ?
s2d_unk2 dw ?
s2d_reserved dw ?
s2d_reserved2 dw ?
SHAPE2D ends
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
_sprite_make_wnd proc far
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push bp
    mov bp, sp
    sub sp, 8
    push ds
    push si
    push di
    mov ax, [bp+arg_0]
    mov [bp+var_8], ax
    mov bx, [bp+arg_2]
    imul bx
    add ax, 10h
    mov cx, 4
    shr ax, cl
    inc ax
    mov bx, ax
    mov [bp+var_6], ax
    push ax
    mov ax, MCGA_WINDOW_TEXT
    push ax
    call _mmgr_alloc_pages
    add sp, 4
    mov [bp+var_2], dx
    mov ds, dx
    xor bx, bx
    mov ax, [bp+var_8]
    mov [bx+SHAPE2D.s2d_width], ax
    mov ax, [bp+arg_2]
    mov [bx+SHAPE2D.s2d_height], ax
    xor ax, ax
    mov [bx+SHAPE2D.s2d_unk1], ax
    mov [bx+SHAPE2D.s2d_unk2], ax
    mov [bx+SHAPE2D.s2d_pos_x], ax
    mov [bx+SHAPE2D.s2d_pos_y], ax
    mov ax, 0Fh
    add ax, [bp+arg_2]
    shl ax, 1
    mov bx, cs:[NEXT_WND_DEF]
    mov [bp+var_4], bx
    add ax, bx
    cmp ax, WND_DEFS_END
    jnb short loc_34CD6
    mov cs:[NEXT_WND_DEF], ax
    mov word ptr cs:[bx+SPRITE.sprite_bitmapptr], 0
    mov ax, [bp+var_2]
    mov word ptr cs:[bx+SPRITE.sprite_bitmapptr+2], ax
    lea ax, [bx+size SPRITE]
    mov cs:[bx+SPRITE.sprite_lineofs], ax
    mov cs:[bx+SPRITE.sprite_left], 0
    mov cs:[bx+SPRITE.sprite_left2], 0
    mov ax, [bp+var_8]
    mov cs:[bx+SPRITE.sprite_right], ax
    mov cs:[bx+SPRITE.sprite_pitch], ax
    mov cs:[bx+SPRITE.sprite_top], 0
    mov ax, [bp+arg_2]
    mov cs:[bx+SPRITE.sprite_height], ax
    mov ax, [bp+arg_0]
    mov cs:[bx+SPRITE.sprite_width2], ax
    mov cs:[bx+SPRITE.sprite_widthsum], ax
    mov cx, [bp+arg_2]
    mov ax, size SHAPE2D
loc_34CBE:
    mov cs:[bx+size SPRITE], ax
    add bx, 2
    add ax, [bp+var_8]
    loop loc_34CBE
    mov dx, cs
    mov ax, [bp+var_4]
    pop di
    pop si
    pop ds
    mov sp, bp
    pop bp
    retf
loc_34CD6:
    mov ax, ss
    mov ds, ax
    mov ax, WINDOW_SPACE_ERROR_TEXT
    push ax
    call _fatal_error
    db 90h
_sprite_make_wnd endp
_TEXT ends
end

