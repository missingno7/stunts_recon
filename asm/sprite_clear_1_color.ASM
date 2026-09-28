.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite_clear_1_color
; -----------------------------------------------------------------------------
; Purpose: Clear sprite image 1 to the requested color.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (byte ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_sprite_clear_1_color proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = byte ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     dx, cs:[_sprite1+18]
    mov     bx, cs:[_sprite1+16]
    sub     dx, bx
    jle     short sprite_clear_1_color_draw_loop
    shl     bx, 1
    add     bx, cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, cs:[_sprite1+12]
    mov     bx, cs:[_sprite1+14]
    sub     bx, cs:[_sprite1+12]
    jle     short sprite_clear_1_color_draw_loop
    mov     si, cs:[_sprite1+20]
    sub     si, bx
    mov     al, [bp+arg_0]
    mov     ah, al
    shr     bx, 1
    jb      short clear_1_color_when_below
sprite_clear_1_color_fill_loop:
    mov     cx, bx
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     di, si
    dec     dx
    jg      short sprite_clear_1_color_fill_loop
sprite_clear_1_color_draw_loop:
    pop     di
    pop     si
    pop     bp
    retf
clear_1_color_when_below:
    jz      short sprite_clear_1_color_when_zero
    jl      short sprite_clear_1_color_draw_loop
    inc     si
clear_1_color_fill_loop_2:
    mov     cx, bx
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, si
    dec     dx
    jg      short clear_1_color_fill_loop_2
    jmp     short sprite_clear_1_color_draw_loop
sprite_clear_1_color_when_zero:
    inc     si
clear_1_color_draw_loop_2:
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, si
    dec     dx
    jg      short clear_1_color_draw_loop_2
    jmp     short sprite_clear_1_color_draw_loop
    ; align 2
    db 0
_sprite_clear_1_color endp
S012_TEXT ends
end
