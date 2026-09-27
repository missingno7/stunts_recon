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
public _sub_35B76
_sub_35B76 proc far
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
    mov     dx, cs:sprite1+SPRITE.sprite_left
    mov     ax, dx
    sub     ax, [bp+arg_0]
    jle     short loc_35B8F
    mov     [bp+arg_0], dx
    sub     [bp+arg_4], ax
    jle     short loc_35BCD
loc_35B8F:
    mov     ax, [bp+arg_0]
    add     ax, [bp+arg_4]
    sub     ax, cs:sprite1+SPRITE.sprite_right
    jle     short loc_35BA1
    sub     [bp+arg_4], ax
    jle     short loc_35BCD
loc_35BA1:
    mov     ax, cs:sprite1+SPRITE.sprite_top
    sub     ax, [bp+arg_2]
    jle     short loc_35BB6
    sub     [bp+arg_6], ax
    jle     short loc_35BCD
    mov     ax, cs:sprite1+SPRITE.sprite_top
    mov     [bp+arg_2], ax
loc_35BB6:
    mov     ax, [bp+arg_2]
    add     ax, [bp+arg_6]
    mov     bx, cs:sprite1+SPRITE.sprite_height
    sub     ax, bx
    jle     short loc_35BD5
    sub     [bp+arg_6], ax
    jle     short loc_35BCD
    jmp     short loc_35BD5
    db 144
loc_35BCD:
    jmp     short loc_35C25
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    push    si
    push    di
loc_35BD5:
    cld
    cmp     [bp+arg_4], 0
    jle     short loc_35BCD
    cmp     [bp+arg_6], 0
    jle     short loc_35BCD
    mov     es, word ptr cs:sprite1+SPRITE.sprite_bitmapptr+2
    mov     al, [bp+arg_8]
    mov     ah, al
    mov     dx, [bp+arg_0]
    mov     si, [bp+arg_6]
    mov     bx, [bp+arg_2]
    shl     bx, 1
    add     bx, cs:sprite1+SPRITE.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+arg_0]
    mov     dx, cs:sprite1+SPRITE.sprite_pitch
    sub     dx, [bp+arg_4]
    mov     cx, [bp+arg_4]
loc_35C0D:
    sar     cx, 1
    mov     [bp+arg_4], cx
    jb      short loc_35C29
    jle     short loc_35C25
loc_35C16:
    mov     cx, [bp+arg_4]
loc_35C19:
    xor     es:[di], ax
    inc     di
    inc     di
    loop    loc_35C19
    add     di, dx
    dec     si
    jg      short loc_35C16
loc_35C25:
    pop     di
    pop     si
    pop     bp
    retf
loc_35C29:
    jz      short loc_35C42
    jl      short loc_35C25
    inc     dx
loc_35C2E:
    mov     cx, [bp+arg_4]
loc_35C31:
    xor     es:[di], ax
    inc     di
    inc     di
    loop    loc_35C31
    xor     es:[di], al
    add     di, dx
    dec     si
    jg      short loc_35C2E
    jmp     short loc_35C25
loc_35C42:
    inc     dx
loc_35C43:
    xor     es:[di], al
    add     di, dx
    dec     si
    jg      short loc_35C43
    jmp     short loc_35C25
    ; align 2
    db 0
_sub_35B76 endp
S012_TEXT ends
end
