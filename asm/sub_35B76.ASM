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
extrn _sprite1:word
public _draw_filled_rect
_draw_filled_rect proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, cs:_sprite1+SPRITE.sprite_left
    mov     ax, dx
    sub     ax, [bp+arg_0]
    jle     short filled_rect_when_less_or_equal
    mov     [bp+arg_0], dx
    sub     [bp+arg_4], ax
    jle     short draw_filled_rect_draw_loop
filled_rect_when_less_or_equal:
    mov     ax, [bp+arg_0]
    add     ax, [bp+arg_4]
    sub     ax, cs:_sprite1+SPRITE.sprite_right
    jle     short rect_when_less_or_equal_2
    sub     [bp+arg_4], ax
    jle     short draw_filled_rect_draw_loop
rect_when_less_or_equal_2:
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    sub     ax, [bp+arg_2]
    jle     short rect_when_less_or_equal_3
    sub     [bp+arg_6], ax
    jle     short draw_filled_rect_draw_loop
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    mov     [bp+arg_2], ax
rect_when_less_or_equal_3:
    mov     ax, [bp+arg_2]
    add     ax, [bp+arg_6]
    mov     bx, cs:_sprite1+SPRITE.sprite_height
    sub     ax, bx
    jle     short draw_filled_rect_shared_path
    sub     [bp+arg_6], ax
    jle     short draw_filled_rect_draw_loop
    jmp     short draw_filled_rect_shared_path
    db 144
draw_filled_rect_draw_loop:
    jmp     short draw_filled_rect_draw_loop_4
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    push    si
    push    di
draw_filled_rect_shared_path:
    cld
    cmp     [bp+arg_4], 0
    jle     short draw_filled_rect_draw_loop
    cmp     [bp+arg_6], 0
    jle     short draw_filled_rect_draw_loop
    mov     es, word ptr cs:_sprite1+SPRITE.sprite_bitmapptr+2
    mov     al, [bp+arg_8]
    mov     ah, al
    mov     dx, [bp+arg_0]
    mov     si, [bp+arg_6]
    mov     bx, [bp+arg_2]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+arg_0]
    mov     dx, cs:_sprite1+SPRITE.sprite_pitch
    sub     dx, [bp+arg_4]
    mov     cx, [bp+arg_4]
draw_filled_rect_shift_value:
    sar     cx, 1
    mov     [bp+arg_4], cx
    jb      short draw_filled_rect_when_below
    jle     short draw_filled_rect_draw_loop_4
draw_filled_rect_draw_loop_2:
    mov     cx, [bp+arg_4]
draw_filled_rect_draw_loop_3:
    xor     es:[di], ax
    inc     di
    inc     di
    loop    draw_filled_rect_draw_loop_3
    add     di, dx
    dec     si
    jg      short draw_filled_rect_draw_loop_2
draw_filled_rect_draw_loop_4:
    pop     di
    pop     si
    pop     bp
    retf
draw_filled_rect_when_below:
    jz      short draw_filled_rect_when_zero
    jl      short draw_filled_rect_draw_loop_4
    inc     dx
draw_filled_rect_draw_loop_5:
    mov     cx, [bp+arg_4]
draw_filled_rect_draw_loop_6:
    xor     es:[di], ax
    inc     di
    inc     di
    loop    draw_filled_rect_draw_loop_6
    xor     es:[di], al
    add     di, dx
    dec     si
    jg      short draw_filled_rect_draw_loop_5
    jmp     short draw_filled_rect_draw_loop_4
draw_filled_rect_when_zero:
    inc     dx
draw_filled_rect_draw_loop_7:
    xor     es:[di], al
    add     di, dx
    dec     si
    jg      short draw_filled_rect_draw_loop_7
    jmp     short draw_filled_rect_draw_loop_4
    ; align 2
    db 0
_draw_filled_rect endp
S012_TEXT ends
end
