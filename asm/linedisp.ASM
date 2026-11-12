; ASMDOC Module review: Placeholder group name replaced by the complete shared two-color span operation; no
; split/merge.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _nopsub_33330, _draw_unknown_lines. Stack offsets are post-prologue BP offsets.
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
assume cs:S012_TEXT, ds:DGROUP
extrn _line_pattern_bits:word
extrn _prerender_auxiliary_arg:word
public _sprite_fill_two_color_spans_at
public _draw_unknown_lines
extrn _sprite1:byte
sprite_lineofs equ 0Ah
sprite_bitmapptr equ 0
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33330.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_A (word ptr 16); arg_C (word ptr 18).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _line_pattern_bits, _prerender_auxiliary_arg.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_33330; proposed name _sprite_fill_two_color_spans_at
; ASMDOC Purpose: Set the pattern word and auxiliary low color byte then enter the common two-color span filler.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] near DS offset of signed 16-bit inclusive left endpoints, one word per row; [BP+08]
; near DS offset of
; ASMDOC signed 16-bit inclusive right endpoints, one word per row; [BP+0A] first y word; caller supplies valid
; row-table index;
; ASMDOC [BP+0C] signed row count; zero/negative behavior is entry-specific; [BP+0E] primary color (low byte
; only); DS addresses
; ASMDOC span arrays and pattern globals; no clipping in this entry; [BP+10] auxiliary color low byte (only low
; byte of global
; ASMDOC changed); [BP+12] 16-bit row pattern.
; ASMDOC Outputs: No semantic register result; destination pixels changed; Caller stack slots [BP+06]/[BP+08]
; advance by 2 each
; ASMDOC row; [BP+0C] decremented in place.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A) and addressed row offsets;
; _line_pattern_bits (DS
; ASMDOC word; alternating low/high row masks); _prerender_auxiliary_arg low byte.
; ASMDOC Globals written: _line_pattern_bits; _prerender_auxiliary_arg low byte; sprite1 backing pixels; caller
; stack argument
; ASMDOC slots +06/+08/+0C.
; ASMDOC Services: none.
; ASMDOC Dead: true. Reachability: No runtime symbol reference or address-taking found in tracked src/ or asm/.
; Shared core is
; ASMDOC live via draw_unknown_lines.
; ASMDOC Shared tail: Prologue saves SI/DI/BP then jumps to unknown_lines_when_condition (67); outputs, register
; clobbers and
; ASMDOC global reads include the entire common core.
_sprite_fill_two_color_spans_at proc far
    arg_auxiliary_color = word ptr 16
    arg_row_pattern = word ptr 18

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_row_pattern]
    mov     ds:[_line_pattern_bits], ax
    mov     ax, [bp+arg_auxiliary_color]
    mov      byte ptr ds:[_prerender_auxiliary_arg], al
    jmp     short unknown_lines_when_condition
    even ; word-align the following entry
_sprite_fill_two_color_spans_at endp
; -----------------------------------------------------------------------------
; Purpose: Draw the line set from the prepared line records.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _line_pattern_bits, _prerender_auxiliary_arg, _sprite1.
; Globals written: _line_pattern_bits.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _draw_unknown_lines; proposed name _sprite_fill_two_color_spans
; ASMDOC Purpose: Fill each inclusive span: pattern bit 0 stores primary color; bit 1 stores auxiliary color.
; Swap row-mask bytes
; ASMDOC initially when y is even and after every row.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] near DS offset of signed 16-bit inclusive left endpoints, one word per row; [BP+08]
; near DS offset of
; ASMDOC signed 16-bit inclusive right endpoints, one word per row; [BP+0A] first y word; caller supplies valid
; row-table index;
; ASMDOC [BP+0C] signed row count; zero/negative behavior is entry-specific; [BP+0E] primary color (low byte
; only); DS addresses
; ASMDOC span arrays and pattern globals; no clipping in this entry.
; ASMDOC Outputs: No semantic register result; destination pixels changed; Caller stack slots [BP+06]/[BP+08]
; advance by 2 each
; ASMDOC row; [BP+0C] decremented in place.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A) and addressed row offsets;
; _line_pattern_bits (DS
; ASMDOC word; alternating low/high row masks); _prerender_auxiliary_arg low byte.
; ASMDOC Globals written: _line_pattern_bits; sprite1 backing pixels; caller stack argument slots +06/+08/+0C.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Address taken and installed as _spritefunc by preRender_unk at
; ASMDOC asm/gfxres.ASM:1541; indirect span callback.
; ASMDOC Edge cases: No initial count check: zero/negative row count processes one row. No clipping. Auxiliary
; color copied to DL
; ASMDOC each row; primary remains AL throughout.
_draw_unknown_lines proc far
    arg_left_edges = word ptr 6
    arg_right_edges = word ptr 8
    arg_start_y = word ptr 10
    arg_row_count = word ptr 12
    arg_color = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
; ASMDOC Shared entry for explicit-state and current-state routines; all reads/writes and clobbers apply to both
; public
; ASMDOC prologues.
unknown_lines_when_condition:
    cld
    mov     si, [bp+arg_start_y]
    test    si, 01h
    jnz     short unknown_lines_when_nonzero
    mov     ax, ds:[_line_pattern_bits]
    xchg    ah, al
    mov     ds:[_line_pattern_bits], ax
unknown_lines_when_nonzero:
    mov     si, [bp+arg_start_y]
    shl     si, 1
    add     si, cs:[_sprite1+sprite_lineofs]
    mov     ax, word ptr cs:[_sprite1+sprite_bitmapptr+2]
    mov     es, ax
    mov     al, [bp+arg_color]
draw_unknown_lines_draw_loop:
    mov     bx, [bp+arg_right_edges]
    mov     dx, [bx]
    mov     bx, [bp+arg_left_edges]
    mov     bx, [bx]
    mov     ah, byte ptr ds:[_line_pattern_bits]
    mov     cl, bl
    and     cl, 07h
    rol     ah, cl
    mov     cx, dx
    mov     dl, byte ptr ds:[_prerender_auxiliary_arg]
    sub     cx, bx
    inc     cx
    jle     short draw_unknown_lines_draw_loop_2
    mov     di, cs:[si]
    add     di, bx
; ASMDOC After ROL, carry 0 stores primary AL; carry 1 branches to auxiliary DL store. This polarity is opposite
; the single-color
; ASMDOC patterned filler.
draw_unknown_lines_fill_loop:
    rol     ah, 1
    jb      short draw_unknown_lines_when_below
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    draw_unknown_lines_fill_loop
draw_unknown_lines_draw_loop_2:
    add     [bp+arg_right_edges], 2
    add     [bp+arg_left_edges], 2
    add     si, 2
    mov     dx, ds:[_line_pattern_bits]
    xchg    dh, dl
    mov     ds:[_line_pattern_bits], dx
    dec     [bp+arg_row_count]
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

