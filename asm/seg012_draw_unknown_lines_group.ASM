.8086
INCLUDE include\platform_hw.inc
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT,ds:DGROUP
extrn _line_pattern_bits:word
extrn _prerender_auxiliary_arg:word
public _nopsub_33330
public _draw_unknown_lines
extrn _sprite1:byte
sprite_lineofs equ 0Ah
sprite_bitmapptr equ 0
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33330.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_A (word ptr 16); arg_C (word ptr 18).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _line_pattern_bits, _prerender_auxiliary_arg.
; -----------------------------------------------------------------------------
_nopsub_33330 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_A = word ptr 16
    arg_C = word ptr 18

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_C]
    mov     ds:[_line_pattern_bits], ax
    mov     ax, [bp+arg_A]
    mov     byte ptr ds:[_prerender_auxiliary_arg], al
    jmp     short unknown_lines_when_condition
    db 144
_nopsub_33330 endp
; -----------------------------------------------------------------------------
; Purpose: Draw the line set from the prepared line records.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _line_pattern_bits, _prerender_auxiliary_arg, _sprite1.
; Globals written: _line_pattern_bits.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_draw_unknown_lines proc far
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
unknown_lines_when_condition:
    cld
    mov     si, [bp+arg_4]
    test    si, 1
    jnz     short unknown_lines_when_nonzero
    mov     ax, ds:[_line_pattern_bits]
    xchg    ah, al
    mov     ds:[_line_pattern_bits], ax
unknown_lines_when_nonzero:
    mov     si, [bp+arg_4]
    shl     si, 1
    add     si, cs:[_sprite1+sprite_lineofs]
    mov     ax, word ptr cs:[_sprite1+sprite_bitmapptr+2]
    mov     es, ax
    mov     al, [bp+arg_8]
draw_unknown_lines_draw_loop:
    mov     bx, [bp+arg_2]
    mov     dx, [bx]
    mov     bx, [bp+arg_0]
    mov     bx, [bx]
    mov     ah, byte ptr ds:[_line_pattern_bits]
    mov     cl, bl
    and     cl, 7
    rol     ah, cl
    mov     cx, dx
    mov     dl, byte ptr ds:[_prerender_auxiliary_arg]
    sub     cx, bx
    inc     cx
    jle     short draw_unknown_lines_draw_loop_2
    mov     di, cs:[si]
    add     di, bx
draw_unknown_lines_fill_loop:
    rol     ah, 1
    jb      short draw_unknown_lines_when_below
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    draw_unknown_lines_fill_loop
draw_unknown_lines_draw_loop_2:
    add     [bp+arg_2], 2
    add     [bp+arg_0], 2
    add     si, 2
    mov     dx, ds:[_line_pattern_bits]
    xchg    dh, dl
    mov     ds:[_line_pattern_bits], dx
    dec     [bp+arg_6]
    jg      short draw_unknown_lines_draw_loop
    pop     di
    pop     si
    pop     bp
    retf
draw_unknown_lines_when_below:
    mov     es:[di], dl ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    draw_unknown_lines_fill_loop
    jmp     short draw_unknown_lines_draw_loop_2
_draw_unknown_lines endp
S012_TEXT ends
end

