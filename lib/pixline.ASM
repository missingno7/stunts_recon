; ASMDOC Module review: Misleading tentative pixel name replaced by prepared line-record rasterizer; one
; complete module
; ASMDOC retained.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _putpixel_line1_maybe. Stack offsets are post-prologue BP offsets.
.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _sprite1:byte
public _drawcline
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
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _putpixel_line1_maybe; proposed name _sprite_draw_line_record
; ASMDOC Purpose: Dispatch one prepared line record into solid horizontal, vertical, diagonal, fixed-point slope
; or singleton
; ASMDOC pixel loops; no clipping in this entry.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] near DS pointer to prepared line record; Record +00 dword x fixed-point, +04 dword y
; fixed-point (low
; ASMDOC fractions; high integer words); 00008000h added to both before drawing; Record +0C word fractional
; slope step; +0E word
; ASMDOC sample count; +10 low color byte; +12 word mode in 0..9; Modes 0/1 horizontal, 2 vertical, 3
; descending-x diagonal, 4
; ASMDOC ascending-x diagonal, 5/6 y-major +/-x fraction, 7/8 x-major -/+x with +y fraction, 9 singleton; DS
; must address record;
; ASMDOC DF must be clear for mode 0/1 REP STOSB (routine has no CLD); positive count required for LOOP modes.
; ASMDOC Outputs: No semantic register result; sprite1 pixels changed; record not modified.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF unchanged.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A) and row offsets; caller DS line
; record.
; ASMDOC Globals written: sprite1 backing pixels.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct caller preRender_line, lib/gfxres.ASM:2852; caller
; rejects <=0
; ASMDOC record count.
; ASMDOC Edge cases: Indirect JMP table at 71 has ten unchecked near targets. LOOP modes with count 0 wrap to
; 65536 iterations.
; ASMDOC Vertical/diagonal/y-major modes index raw record y high word; horizontal/singleton/x-major modes use
; rounded local y.
; ASMDOC Zero/one modes share implementation. Fractional locals mutate only stack frame.
_drawcline proc far
    local_color = byte ptr -14
    local_fraction_step = word ptr -10
    local_rounded_y = word ptr -8
    local_y_fraction = word ptr -6
    local_rounded_x = word ptr -4
    local_x_fraction = word ptr -2
    arg_line_descriptor = word ptr 6

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    si
    push    di
    mov     si, [bp+arg_line_descriptor]
    mov     ax, [si]
    mov     dx, [si+2]
    add     ax, 8000h
    adc     dx, 0
    mov     [bp+local_x_fraction], ax
    mov     [bp+local_rounded_x], dx
    mov     ax, [si+4]
    mov     dx, [si+6]
    add     ax, 8000h
    adc     dx, 0
    mov     [bp+local_y_fraction], ax
    mov     [bp+local_rounded_y], dx
    mov     al, [si+10h]
    mov     [bp+local_color], al
    mov     ax, [si+0Ch]
    mov     [bp+local_fraction_step], ax
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     bx, [si+12h]
    shl     bx, 1
; ASMDOC Unchecked indirect near JMP, not a callable routine boundary. Prepared record mode must be 0..9; all
; targets return
; ASMDOC through original far frame.
    jmp     cs:sprite_line_draw_mode_dispatch[bx]
; ASMDOC Inline code-address DATA: 10 words, modes 0/1 share horizontal implementation; then vertical, -/+
; diagonal, -/+ y-major
; ASMDOC fraction, -/+ x-major fraction, singleton.
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
; ASMDOC Modes 0/1 use rounded local x/y and REP STOSB. No CLD here: incoming DF matters only to these
; string-store modes.
line_load_source_value:
    mov     cx, [si+0Eh]
    mov     al, [bp+local_color]
    mov     di, [bp+local_rounded_y]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+local_rounded_x]
    rep stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
putpixel_line_load_source_byte:
    mov     al, [bp+local_color]
    mov     di, [bp+local_rounded_y]
    shl     di, 1
    add     di, cs:_sprite1.sprite_lineofs
    mov     di, cs:[di]
    add     di, [bp+local_rounded_x]
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
; ASMDOC Mode 2 walks raw record y high word and constant rounded x; no clipping. LOOP zero count wraps.
line_load_source_value_2:
    mov     cx, [si+0Eh]
    mov     al, [bp+local_color]
    mov     bx, [bp+local_rounded_x]
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
    mov     al, [bp+local_color]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+local_rounded_x]
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
    mov     al, [bp+local_color]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+local_rounded_x]
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
; ASMDOC Modes 5/6 advance rows and subtract/add fractional x step with SBB/ADC into integer x.
line_load_source_value_5:
    mov     cx, [si+0Eh]
    mov     dx, [bp+local_fraction_step]
    mov     al, [bp+local_color]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+local_rounded_x]
putpixel_line_draw_loop_4:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     si, 2
    sub     [bp+local_x_fraction], dx
    sbb     bx, 0
    loop    putpixel_line_draw_loop_4
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_6:
    mov     cx, [si+0Eh]
    mov     dx, [bp+local_fraction_step]
    mov     al, [bp+local_color]
    mov     si, [si+6]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     bx, [bp+local_rounded_x]
putpixel_line_draw_loop_5:
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     si, 2
    add     [bp+local_x_fraction], dx
    adc     bx, 0
    loop    putpixel_line_draw_loop_5
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
; ASMDOC Modes 7/8 move x one per sample and advance rounded y on fractional carry; local fraction state is
; private to stack
; ASMDOC frame.
line_load_source_value_7:
    mov     cx, [si+0Eh]
    mov     dx, [bp+local_fraction_step]
    mov     al, [bp+local_color]
    mov     bx, [bp+local_rounded_x]
putpixel_line_draw_loop_6:
    mov     si, [bp+local_rounded_y]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    dec     bx
    add     [bp+local_y_fraction], dx
    adc     [bp+local_rounded_y], 0
    loop    putpixel_line_draw_loop_6
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_load_source_value_8:
    mov     cx, [si+0Eh]
    mov     dx, [bp+local_fraction_step]
    mov     al, [bp+local_color]
    mov     bx, [bp+local_rounded_x]
putpixel_line_draw_loop_7:
    mov     si, [bp+local_rounded_y]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     di, cs:[si]
    mov     es:[bx+di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     bx
    add     [bp+local_y_fraction], dx
    adc     [bp+local_rounded_y], 0
    loop    putpixel_line_draw_loop_7
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_drawcline endp
S012_TEXT ends
end
