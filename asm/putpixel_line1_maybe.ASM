.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _sprite1:byte
public _putpixel_line1_maybe
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
_putpixel_line1_maybe proc far
    var_E = byte ptr -14
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    si
    push    di
    mov     si, [bp+arg_0]
    mov     ax, [si]
    mov     dx, [si+2]
    add     ax, 8000h
    adc     dx, 0
    mov     [bp+var_2], ax
    mov     [bp+var_4], dx
    mov     ax, [si+4]
    mov     dx, [si+6]
    add     ax, 8000h
    adc     dx, 0
    mov     [bp+var_6], ax
    mov     [bp+var_8], dx
    mov     al, [si+10h]
    mov     [bp+var_E], al
    mov     ax, [si+0Ch]
    mov     [bp+var_A], ax
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     bx, [si+12h]
    shl     bx, 1
    jmp     cs:off_3340A[bx]
off_3340A     dw offset loc_3341E
    dw offset loc_3341E
    dw offset loc_33458
    dw offset loc_3347C
    dw offset loc_334A1
    dw offset loc_334C6
    dw offset loc_334F3
    dw offset loc_33520
    dw offset loc_3354C
    dw offset loc_3343C
loc_3341E:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     di, [bp+var_8]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+var_4]
    rep stosb
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_3343C:
    mov     al, [bp+var_E]
    mov     di, [bp+var_8]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+var_4]
    mov     es:[di], al
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_33458:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
loc_3346B:
    mov     di, cs:[si]
    mov     es:[bx+di], al
    add     si, 2
    loop    loc_3346B
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_3347C:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
loc_3348F:
    mov     di, cs:[si]
    mov     es:[bx+di], al
    dec     bx
    add     si, 2
    loop    loc_3348F
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_334A1:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
loc_334B4:
    mov     di, cs:[si]
    mov     es:[bx+di], al
    inc     bx
    add     si, 2
    loop    loc_334B4
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_334C6:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
loc_334DC:
    mov     di, cs:[si]
    mov     es:[bx+di], al
    add     si, 2
    sub     [bp+var_2], dx
    sbb     bx, 0
    loop    loc_334DC
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_334F3:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
loc_33509:
    mov     di, cs:[si]
    mov     es:[bx+di], al
    add     si, 2
    add     [bp+var_2], dx
    adc     bx, 0
    loop    loc_33509
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_33520:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
loc_3352C:
    mov     si, [bp+var_8]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al
    dec     bx
    add     [bp+var_6], dx
    adc     [bp+var_8], 0
    loop    loc_3352C
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_3354C:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
loc_33558:
    mov     si, [bp+var_8]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al
    inc     bx
    add     [bp+var_6], dx
    adc     [bp+var_8], 0
    loop    loc_33558
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_putpixel_line1_maybe endp
S012_TEXT ends
end
