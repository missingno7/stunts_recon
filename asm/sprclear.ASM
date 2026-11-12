; ASMDOC Module: load_232c0. Proposed content name: sprite_clear_1_color. Address-derived owner load_232c0 should
; ASMDOC follow its single-purpose existing source filename; no split or merge.
; ASMDOC Public routines: _sprite_clear_1_color.
; ASMDOC Static contract audit: byte/word widths and offsets describe original 8086 paths; inherited generated
; comments
; ASMDOC below are retained verbatim and may disagree. BP offsets are after PUSH BP/MOV BP,SP. Destination
; bitmap/table
; ASMDOC memory is caller-selected; no owned routine calls an application callback.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite_clear_1_color
; -----------------------------------------------------------------------------
; Purpose: Clear sprite image 1 to the requested color.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (byte ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_clear_1_color: Fill sprite1 current clip rectangle with the supplied palette index.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u8 color (low byte only); DF must be clear for REP STOSW forward writes.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP, DS, SI, DI, SS, CS; DF unchanged.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: No CLD: relies on caller direction flag. Clip right-left and bottom-top tested signed and nonpositive
; ASMDOC extents return. First row uses CS row-table; later rows advance by pitch. Odd-width and singleton paths write
; ASMDOC final byte directly. No BIOS/device service: writes whatever segment sprite1 selects, commonly A000h..
_sprite_clear_1_color proc far
    arg_color = byte ptr 6

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
    mov     al, [bp+arg_color]
    mov     ah, al
    shr     bx, 1
    jb      short clear_1_color_when_below
; ASMDOC REP STOSW inherits DF; this routine does not clear it.
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
    db 0 ; retained zero fill at this boundary
_sprite_clear_1_color endp
S012_TEXT ends
end
