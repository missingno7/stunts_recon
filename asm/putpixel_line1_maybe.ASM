.8086
INCLUDE include\platform_hw.inc
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
; -----------------------------------------------------------------------------
; Purpose: Write the current line pixel pattern to video memory.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
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
    jmp     cs:sprite_line_draw_mode_dispatch[bx]
sprite_line_draw_mode_dispatch     dw offset line_load_source_value
    dw offset line_load_source_value
    dw offset line_load_source_value_2
    dw offset line_load_source_value_3
    dw offset line_load_source_value_4
    dw offset line_load_source_value_5
    dw offset line_load_source_value_6
    dw offset line_load_source_value_7
    dw offset line_load_source_value_8
    dw offset putpixel_line_load_source_byte
line_load_source_value:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     di, [bp+var_8]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+var_4]
    rep stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
putpixel_line_load_source_byte:
    mov     al, [bp+var_E]
    mov     di, [bp+var_8]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+var_4]
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_2:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
putpixel_line_draw_loop:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     si, 2
    loop    putpixel_line_draw_loop
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_3:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_2:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    dec     bx
    add     si, 2
    loop    putpixel_line_draw_loop_2
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_4:
    mov     cx, [si+0Eh]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_3:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     bx
    add     si, 2
    loop    putpixel_line_draw_loop_3
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_5:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_4:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     si, 2
    sub     [bp+var_2], dx
    sbb     bx, 0
    loop    putpixel_line_draw_loop_4
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_6:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_5:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     si, 2
    add     [bp+var_2], dx
    adc     bx, 0
    loop    putpixel_line_draw_loop_5
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_7:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_6:
    mov     si, [bp+var_8]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    dec     bx
    add     [bp+var_6], dx
    adc     [bp+var_8], 0
    loop    putpixel_line_draw_loop_6
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_8:
    mov     cx, [si+0Eh]
    mov     dx, [bp+var_A]
    mov     al, [bp+var_E]
    mov     bx, [bp+var_4]
putpixel_line_draw_loop_7:
    mov     si, [bp+var_8]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     bx
    add     [bp+var_6], dx
    adc     [bp+var_8], 0
    loop    putpixel_line_draw_loop_7
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_putpixel_line1_maybe endp
S012_TEXT ends
end
