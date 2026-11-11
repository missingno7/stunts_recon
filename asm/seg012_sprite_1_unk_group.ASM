; ASMDOC(font_shapes) Module seg012_sprite_1_unk_group. Replace segment/unknown grouping label with the two
; actual operations:
; ASMDOC(font_shapes) rectangle fill and phased sparse bitmap reveal. Preserve tables and alternate prologue.
; Public callable
; ASMDOC(font_shapes) entries: _sprite1_unknown2, _sprite_1_unk, _sprite_1_unk3.
; ASMDOC(font_shapes) Contract offsets use BP after the standard prologue; FAR return IP/CS occupy BP+2/+4.
; Original source
; ASMDOC(font_shapes) comments remain verbatim and may contain older incomplete summaries; these added contracts
; describe
; ASMDOC(font_shapes) transitive shared exits.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite1_unknown2
public _sprite_1_unk
public _sprite_1_unk3
; -----------------------------------------------------------------------------
; Purpose: Apply the legacy sprite-1 operation exposed at this entry.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _sprite1_unknown2 (proposed _sprite_fill_rect_clipped): Clip against sprite1
; bounds, then fill
; ASMDOC(font_shapes) a solid indexed-color rectangle.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] s16 x; [BP+8] s16 y; [BP+10] s16 width; [BP+12] s16 height; [BP+14] low
; color byte
; ASMDOC(font_shapes) (higher argument byte ignored)
; ASMDOC(font_shapes) outputs: No scalar return; accepted rectangle pixels filled with color byte; nonpositive
; resulting
; ASMDOC(font_shapes) width/height cause no stores
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; SS; CS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared on shared fill entry (a clipped early
; ASMDOC(font_shapes) rejection before entry can preserve incoming DF)
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment, +10 line-table offset, +20 pitch;
; CS:line_table[2*y];
; ASMDOC(font_shapes) CS:_sprite1+12/+14 x bounds and +16/+18 y bounds; right/bottom exclusive
; ASMDOC(font_shapes) globals_written: sprite1 bitmap bytes; SS:[BP+10] overwritten with floor(accepted
; width/2); clipping
; ASMDOC(font_shapes) also rewrites x/y/width/height argument words as required
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No clipping in shared body. All clipping arithmetic wraps at 16 bits and signed
; comparisons choose
; ASMDOC(font_shapes) branches.
; ASMDOC(font_shapes) Segment from sprite1+2 and line table offsets establish destination; bitmap offset word is
; unused. Width
; ASMDOC(font_shapes) 1 has separate byte-only loop; odd wider rows append one byte.
_sprite1_unknown2 proc far
    arg_x = word ptr 6
    arg_y = word ptr 8
    arg_width = word ptr 10
    arg_height = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, word ptr cs:[_sprite1+12]
    mov     ax, dx
    sub     ax, [bp+arg_x]
    jle     short unknown2_when_less_or_equal
    mov     [bp+arg_x], dx
    sub     [bp+arg_width], ax
    jle     short sprite1_unknown2_draw_loop
unknown2_when_less_or_equal:
    mov     ax, [bp+arg_x]
    add     ax, [bp+arg_width]
    sub     ax, word ptr cs:[_sprite1+14]
    jle     short unknown2_when_less_or_equal_2
    sub     [bp+arg_width], ax
    jle     short sprite1_unknown2_draw_loop
unknown2_when_less_or_equal_2:
    mov     ax, word ptr cs:[_sprite1+16]
    sub     ax, [bp+arg_y]
    jle     short unknown2_when_less_or_equal_3
    sub     [bp+arg_height], ax
    jle     short sprite1_unknown2_draw_loop
    mov     ax, word ptr cs:[_sprite1+16]
    mov     [bp+arg_y], ax
unknown2_when_less_or_equal_3:
    mov     ax, [bp+arg_y]
    add     ax, [bp+arg_height]
    mov     bx, word ptr cs:[_sprite1+18]
    sub     ax, bx
    jle     short sprite_1_unk_shared_path
    sub     [bp+arg_height], ax
    jle     short sprite1_unknown2_draw_loop
    jmp     short sprite_1_unk_shared_path
    nop ; retained code pad at an even offset; EVEN would emit no byte
sprite1_unknown2_draw_loop:
    jmp     short sprite_1_unk_draw_loop
    even ; word-align the following entry
_sprite1_unknown2 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite 1 unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10); arg_8
;   (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _sprite_1_unk (proposed _sprite_fill_rect_unclipped): Fill a solid indexed-color
; rectangle.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] s16 x; [BP+8] s16 y; [BP+10] s16 width; [BP+12] s16 height; [BP+14] low
; color byte
; ASMDOC(font_shapes) (higher argument byte ignored)
; ASMDOC(font_shapes) outputs: No scalar return; accepted rectangle pixels filled with color byte; nonpositive
; resulting
; ASMDOC(font_shapes) width/height cause no stores
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; SS; CS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared on shared fill entry (a clipped early
; ASMDOC(font_shapes) rejection before entry can preserve incoming DF)
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment, +10 line-table offset, +20 pitch; CS:line_table[2*y]
; ASMDOC(font_shapes) globals_written: sprite1 bitmap bytes; SS:[BP+10] overwritten with floor(accepted
; width/2); clipping
; ASMDOC(font_shapes) also rewrites x/y/width/height argument words as required
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No clipping in shared body. All clipping arithmetic wraps at 16 bits and signed
; comparisons choose
; ASMDOC(font_shapes) branches.
; ASMDOC(font_shapes) Segment from sprite1+2 and line table offsets establish destination; bitmap offset word is
; unused. Width
; ASMDOC(font_shapes) 1 has separate byte-only loop; odd wider rows append one byte.
_sprite_1_unk proc far
    arg_x = word ptr 6
    arg_y = word ptr 8
    arg_width = word ptr 10
    arg_height = word ptr 12
    arg_color = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
; ASMDOC(font_shapes) Clipped and unclipped rectangle entries share this fill core and its common epilogue.
; Color is low byte
; ASMDOC(font_shapes) at BP+14.
sprite_1_unk_shared_path:
    cld
    cmp     [bp+arg_width], 0
    jle     short sprite1_unknown2_draw_loop
    cmp     [bp+arg_height], 0
    jle     short sprite1_unknown2_draw_loop
    mov     es, word ptr cs:[_sprite1+2]
    mov     al, [bp+arg_color]
    mov     ah, al
    mov     dx, [bp+arg_x]
    mov     si, [bp+arg_height]
    mov     bx, [bp+arg_y]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+arg_x]
    mov     dx, word ptr cs:[_sprite1+20]
    sub     dx, [bp+arg_width]
    mov     cx, [bp+arg_width]
    sar     cx, 1
    mov     [bp+arg_width], cx
    jb      short sprite_1_unk_when_below
    jle     short sprite_1_unk_draw_loop
sprite_1_unk_fill_loop:
    mov     cx, [bp+arg_width]
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_fill_loop
sprite_1_unk_draw_loop:
    pop     di
    pop     si
    pop     bp
    retf
sprite_1_unk_when_below:
    jz      short sprite_1_unk_when_zero
    jl      short sprite_1_unk_draw_loop
    inc     dx
sprite_1_unk_fill_loop_2:
    mov     cx, [bp+arg_width]
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_fill_loop_2
    jmp     short sprite_1_unk_draw_loop
sprite_1_unk_when_zero:
    inc     dx
sprite_1_unk_draw_loop_2:
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_draw_loop_2
    jmp     short sprite_1_unk_draw_loop
    db 0 ; retained zero fill at this boundary
; ASMDOC(font_shapes) Inline immutable phase tables: twelve row lanes and two four-element horizontal skip
; patterns; execution
; ASMDOC(font_shapes) returns before these bytes.
; 11-floor(i/4)-6*(i mod 2)-3*floor((i mod 4)/2), i=0..11; exact
; Verified 12/12 entries, no exceptions.
sprite_scale_pattern_by_row db 0Bh, 05h, 08h, 02h, 0Ah, 04h, 07h, 01h, 09h, 03h, 06h, 00h
; bitreverse2(p) XOR 1, p=0..3; exact
; Verified 4/4 entries, no exceptions.
sprite_run_pre_skip_by_phase     db 01h
    db 03h
    db 00h
    db 02h
; 4-(bitreverse2(p) XOR 1), p=0..3; exact
; Verified 4/4 entries, no exceptions.
sprite_run_post_skip_by_phase     db 03h
    db 01h
    db 04h
    db 02h
; ASMDOC(font_shapes) Unlabelled explicit-position FAR-style prologue: shape far pointer BP+6/+8, phase BP+10, x
; BP+12,y
; ASMDOC(font_shapes) BP+14; shares dither core/epilogue. Preceding phase tables are data, not a fallthrough path.
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr cs:[_sprite1+2]
    mov     ax, [bp+0Ch]
    mov     [bp-2], ax
    mov     ax, [bp+0Eh]
    jmp     short sprite_1_unk3_when_condition
    even ; word-align the following entry
_sprite_1_unk endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite 1 unk3.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI,
; DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _sprite_1_unk3 (proposed _sprite_blit_dither_phase): Copy a phase-selected sparse
; pattern of
; ASMDOC(font_shapes) source bitmap pixels using twelve row lanes and four skip phases.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] u16 initial
; horizontal phase; low
; ASMDOC(font_shapes) two bits select skip tables; argument is incremented in place per lane; Shape +0 u16
; width,+2 u16
; ASMDOC(font_shapes) height,+8 x,+0A y; opaque byte pixels +10h
; ASMDOC(font_shapes) outputs: No scalar return; selected source pixels overwrite active sprite1 bitmap;
; unselected pixels
; ASMDOC(font_shapes) untouched; SS:[BP+10] advanced twelve times modulo 65536
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table rows;
; ASMDOC(font_shapes) CS:sprite_scale_pattern_by_row[12], sprite_run_pre_skip_by_phase[4],
; sprite_run_post_skip_by_phase[4];
; ASMDOC(font_shapes) shape header/pixels
; ASMDOC(font_shapes) globals_written: selected destination bitmap bytes; SS:[BP+10] phase argument
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No CLD/string instructions, so incoming DF is preserved.
; ASMDOC(font_shapes) Row lanes visit table indices 11..0, each advancing 12 rows; unsigned line-table-address
; comparison
; ASMDOC(font_shapes) bounds rows. Source row stride is low16(12*width). Signed remaining-width comparisons
; bound sparse x
; ASMDOC(font_shapes) iteration.
; ASMDOC(font_shapes) No clip/window checks. Unlabelled prologue at original line 166 uses explicit x at
; [BP+12],y at [BP+14],
; ASMDOC(font_shapes) with phase still [BP+10], and joins the same core.
_sprite_1_unk3 proc far
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_phase = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     es, word ptr cs:[_sprite1+2]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
; ASMDOC(font_shapes) Both stored-position and unlabeled explicit-position prologues join here; phase stays in
; BP+10, row
; ASMDOC(font_shapes) destination comes from the CS line table.
sprite_1_unk3_when_condition:
    shl     ax, 1
    add     ax, word ptr cs:[_sprite1+10]
    mov     [bp+var_4], ax
    add     ax, [si+2]
    add     ax, [si+2]
    mov     [bp+var_6], ax
    mov     ax, [si]
    mov     [bp+var_8], ax
    mov     bx, 0Ch
    mul     bx
    mov     [bp+var_A], ax
    add     si, 10h
    mov     [bp+var_C], si
    mov     [bp+var_E], 0Bh
sprite_1_unk3_draw_loop:
    mov     di, [bp+var_E]
    mov     cl, cs:sprite_scale_pattern_by_row[di]
    xor     ch, ch
    mov     ax, [bp+var_8]
    mul     cx
    mov     [bp+var_10], ax
    shl     cx, 1
    add     cx, [bp+var_4]
    mov     [bp+var_12], cx
    mov     si, [bp+var_C]
    add     si, [bp+var_10]
    mov     bx, [bp+arg_phase]
    mov     [bp+var_16], bx
sprite_1_unk3_draw_loop_2:
    mov     di, [bp+var_12]
    cmp     di, [bp+var_6]
    jnb     short sprite_1_unk3_when_not_below
    mov     di, cs:[di]
    add     di, [bp+var_2]
    mov     cx, [bp+var_8]
    mov     [bp+var_14], si
    xor     ah, ah
    mov     bx, [bp+var_16]
; ASMDOC(font_shapes) For each phase, skip pre bytes, copy exactly one pixel without auto-increment, then skip
; post bytes;
; ASMDOC(font_shapes) remaining width uses signed comparisons.
sprite_1_unk3_draw_loop_3:
    and     bx, 03h
    mov     al, cs:sprite_run_pre_skip_by_phase[bx]
    sub     cx, ax
    jle     short unk3_when_less_or_equal
    add     si, ax
    add     di, ax
    mov     al, [si]
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    mov     al, cs:sprite_run_post_skip_by_phase[bx]
    add     si, ax
    add     di, ax
    sub     cx, ax
    inc     bx
    jmp     short sprite_1_unk3_draw_loop_3
unk3_when_less_or_equal:
    inc     [bp+var_16]
    add     [bp+var_12], 18h
    mov     si, [bp+var_14]
    add     si, [bp+var_A]
    jmp     short sprite_1_unk3_draw_loop_2
sprite_1_unk3_when_not_below:
    inc     [bp+arg_phase]
    dec     [bp+var_E]
    jge     short sprite_1_unk3_draw_loop
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_1_unk3 endp
S012_TEXT ends
end
