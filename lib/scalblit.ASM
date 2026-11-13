; ASMDOC Module: sprite_rectangle_scaled_blitters. Proposed content name: sprite_rectangle_scaled_blitters. Existing
; ASMDOC module name covers rectangles, mapping-table operations and shared scaled bitmap tail; retain.
; ASMDOC Public routines: _draw_filled_rect, _clear_rect, _sub_35DC8, _sub_35DE6, _sub_35E08, _loc_35ED9.
; ASMDOC Static contract audit: byte/word widths and offsets describe original 8086 paths; inherited generated
; comments
; ASMDOC below are retained verbatim and may disagree. BP offsets are after PUSH BP/MOV BP,SP. Destination
; bitmap/table
; ASMDOC memory is caller-selected; no owned routine calls an application callback.
.8086
INCLUDE include\hw.inc
name asm012_154486
DGROUP group _DATA
assume ds:DGROUP, ss:DGROUP
_DATA segment word public 'DATA'
source_row_fraction_accumulator dw 0
_DATA ends
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
extrn _sprite2:word
public _xorrectc
public _clear_rect
public _incnums
public _mcgatranslation
public _mcgatransrange
public _scalefxy
public _shape2d_scale_blit_tail
; -----------------------------------------------------------------------------
; Purpose: Draw a filled rectangle using the scaled sprite blitter.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6);
; arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _draw_filled_rect: XOR the clipped rectangle with the low byte of the supplied color.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 i16 x; BP+8 i16 y; BP+10 i16 width; BP+12 i16 height; BP+14 u8 XOR operand (only low byte read);
; ASMDOC clipping mutates x/y/width/height argument words; width then replaced by half-width.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP, DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, flags; DF=0 on accepted dimensions, unchanged on early clipping exit.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Caller x/y/width/height argument words on clipping/draw paths; Bitmap memory selected by
; ASMDOC CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Uses XOR, not assignment fill, and no sprite scaling. Signed wrapped clip arithmetic; empty dimensions
; ASMDOC rejected. Later rows add pitch. Unreachable unclipped prologue after normal early-exit jump..
_xorrectc proc far
    arg_x = word ptr 6
    arg_y = word ptr 8
    arg_width = word ptr 10
    arg_height = word ptr 12
    arg_xor_color = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, cs:_sprite1+SPRITE.sprite_left
    mov     ax, dx
    sub     ax, [bp+arg_x]
    jle     short check_rectangle_right_edge
    mov     [bp+arg_x], dx
    sub     [bp+arg_width], ax
    jle     short return_empty_rectangle
check_rectangle_right_edge:
    mov     ax, [bp+arg_x]
    add     ax, [bp+arg_width]
    sub     ax, cs:_sprite1+SPRITE.sprite_right
    jle     short check_rectangle_top_edge
    sub     [bp+arg_width], ax
    jle     short return_empty_rectangle
check_rectangle_top_edge:
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    sub     ax, [bp+arg_y]
    jle     short check_rectangle_bottom_edge
    sub     [bp+arg_height], ax
    jle     short return_empty_rectangle
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    mov     [bp+arg_y], ax
check_rectangle_bottom_edge:
    mov     ax, [bp+arg_y]
    add     ax, [bp+arg_height]
    mov     bx, cs:_sprite1+SPRITE.sprite_height
    sub     ax, bx
    jle     short draw_clipped_rectangle
    sub     [bp+arg_height], ax
    jle     short return_empty_rectangle
    jmp     short draw_clipped_rectangle
    nop ; retained code pad at an even offset; EVEN would emit no byte
return_empty_rectangle:
    jmp     short return_after_rectangle_draw
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: unclipped XOR rectangle entry. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
    push    bp
    mov     bp, sp
    push    si
    push    di
; ASMDOC After clipping, XOR rectangle rows; not assignment fill.
draw_clipped_rectangle:
    cld
    cmp     [bp+arg_width], 0
    jle     short return_empty_rectangle
    cmp     [bp+arg_height], 0
    jle     short return_empty_rectangle
    mov     es, word ptr cs:_sprite1+SPRITE.sprite_bitmapptr+2
    mov     al, [bp+arg_xor_color]
    mov     ah, al
    mov     dx, [bp+arg_x]
    mov     si, [bp+arg_height]
    mov     bx, [bp+arg_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+arg_x]
    mov     dx, cs:_sprite1+SPRITE.sprite_pitch
    sub     dx, [bp+arg_width]
    mov     cx, [bp+arg_width]
handle_odd_rectangle_width:
    sar     cx, 1
    mov     [bp+arg_width], cx
    jb      short draw_odd_rectangle_width
    jle     short return_after_rectangle_draw
draw_rectangle_rows:
    mov     cx, [bp+arg_width]
write_rectangle_word_pixels:
    xor     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    write_rectangle_word_pixels
    add     di, dx
    dec     si
    jg      short draw_rectangle_rows
return_after_rectangle_draw:
    pop     di
    pop     si
    pop     bp
    retf
draw_odd_rectangle_width:
    jz      short draw_single_pixel_rows
    jl      short return_after_rectangle_draw
    inc     dx
draw_odd_rectangle_rows:
    mov     cx, [bp+arg_width]
write_odd_rect_word_pixels:
    xor     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    write_odd_rect_word_pixels
    xor     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short draw_odd_rectangle_rows
    jmp     short return_after_rectangle_draw
draw_single_pixel_rows:
    inc     dx
write_single_pixel_rows:
    xor     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short write_single_pixel_rows
    jmp     short return_after_rectangle_draw
    db 0 ; retained zero fill at this boundary
_xorrectc endp
; -----------------------------------------------------------------------------
; Purpose: Clear a rectangle in the active sprite or video buffer.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _clear_rect: Copy a rectangle from sprite2 to a linearly displaced position in sprite1.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 i16 source x; BP+8 i16 source y; BP+10 u16 byte width; BP+12 i16 row count; BP+14 i16 linear
; ASMDOC destination displacement; quotient/remainder from signed (x+displacement modulo 65536)/sprite1.width2 select
; ASMDOC destination y+quotient and remainder x.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, flags; DF=0.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, line-table +0Ah, signed width2 +18h; CS:_sprite2 bitmap segment
; ASMDOC +2 and line-table +0Ah; both CS row-table arrays.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Not a color clear. REP MOVSB forward, then source/destination row table cursors +2; no clipping, no
; ASMDOC overlap repair, no dimension validation. Zero/negative height still executes first row. Division can fault on
; ASMDOC zero divisor or quotient overflow..
_clear_rect proc far
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
    arg_source_x = word ptr 6
    arg_source_y = word ptr 8
    arg_byte_width = word ptr 10
    arg_row_count = word ptr 12
    arg_linear_offset = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    si
    push    di
    push    ds
    cld
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     ax, word ptr cs:_sprite2.sprite_bitmapptr+2
    mov     ds, ax
    mov     ax, [bp+arg_source_x]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_byte_width]
    mov     ax, [bp+arg_source_y]
    shl     ax, 1
    mov     cx, ax
    add     ax, cs:_sprite2.sprite_lineofs
    mov     [bp+var_4], ax
    add     cx, cs:_sprite1.sprite_lineofs
    mov     ax, [bp+arg_linear_offset]
    mov     bx, ax
    add     ax, [bp+arg_source_x]
    cwd
    idiv    cs:_sprite1.sprite_width2
    sub     dx, [bp+arg_source_x]
    shl     ax, 1
    add     cx, ax
    add     [bp+var_2], dx
    mov     [bp+var_6], cx
    mov     dx, [bp+arg_row_count]
; ASMDOC Forward sprite2-to-sprite1 row copy; row-table cursors advance independently.
copy_sprite_scanline:
    mov     cx, [bp+arg_byte_width]
    mov     bx, [bp+var_4]
    mov     si, cs:[bx]
    add     si, [bp+arg_source_x]
    mov     bx, [bp+var_6]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    rep movsb
    add     [bp+var_4], 2
    add     [bp+var_6], 2
    dec     dx
    jg      short copy_sprite_scanline
    pop     ds
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
; ASMDOC PUBLIC DATA _incnums: Identity-initialized byte-to-byte pixel remapping table; full/partial copy entries can
; ASMDOC change it; transparent bitmap paths test mapped value against FFh.
; Identity palette: entry[i] = i, i=0..255; exact. Verified 256/256, no exceptions.
_incnums label byte
palette_index = 0
    REPT 256
    db palette_index
palette_index = palette_index + 1
    ENDM

_clear_rect endp
copy_bytes equ 100h
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35DC8.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sub_35DC8: Copy exactly 256 source bytes into the code-segment remapping table.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 source offset; BP+8 source segment; at least 256 readable bytes.
; ASMDOC outputs: CS:_incnums[0..255] overwritten in forward order; AX=CS, CX=0, ES=CS..
; ASMDOC preserves: BP, SP, DS, SI, DI, BX, DX, SS, CS.
; ASMDOC clobbers: AX, CX, ES; DF=0, other arithmetic flags unchanged by body.
; ASMDOC globals_read: none.
; ASMDOC globals_written: CS:_incnums[256].
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: Data in CODE segment is writable. No span checks or overlap buffering; changes data bytes, not
; ASMDOC executable opcodes..
; ASMDOC Proposed routine name: _sprite_copy_increment_table.
_mcgatranslation proc far
    arg_source_offset = word ptr 6
    arg_source_segment = word ptr 8
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, copy_bytes
    mov ds, [bp+arg_source_segment]
    mov si, [bp+arg_source_offset]
    mov ax, cs
    mov es, ax
    mov di, offset _incnums
    cld
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_mcgatranslation endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35DE6. Its internal flow uses clear_copy_direction_flag,
; copy_bytes_to_increment_table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sub_35DE6: Copy an explicit source byte span into the code-segment remapping table at an index.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 destination table index; BP+8 u16 byte count; BP+10 source offset; BP+12 source segment.
; ASMDOC outputs: count bytes copied forward to CS:(_incnums+index modulo 65536); AX=CS, CX=0, ES=CS..
; ASMDOC preserves: BP, SP, DS, SI, DI, BX, DX, SS, CS.
; ASMDOC clobbers: AX, CX, ES; DF=0, other arithmetic flags unchanged by body.
; ASMDOC globals_read: none.
; ASMDOC globals_written: CS:_incnums+index for count bytes; no table-bound check.
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: Count zero copies nothing; caller must keep requested destination within intended storage. Normal host
; ASMDOC adapter deliberately rejects out-of-table spans..
; ASMDOC Proposed routine name: _sprite_update_increment_table.
_mcgatransrange proc far
    arg_table_index = word ptr 6
    arg_byte_count = word ptr 8
    arg_source_offset = word ptr 10
    arg_source_segment = word ptr 12
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, [bp+arg_byte_count]
    mov ds, [bp+arg_source_segment]
    mov si, [bp+arg_table_index]
    mov ax, cs
    mov es, ax
    lea di, _incnums[si]
    mov si, [bp+arg_source_offset]
clear_copy_direction_flag:
    cld
copy_bytes_to_increment_table:
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_mcgatransrange endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35E08. Its internal flow uses allocate_sprite_scale_locals,
; prepare_sprite_dimensions.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6);
; arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (byte ptr 14); arg_shapeptr (dword ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores
; observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: source_row_fraction_accumulator.
; Globals written: source_row_fraction_accumulator.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sub_35E08: Unclipped Q8 scaled transparent bitmap blit with scaled hotspot.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 Q8 scale (256=1:1; accepted 2..65535); BP+8 source shape offset; BP+10 source segment; BP+12
; ASMDOC i16 destination x; BP+14 i16 destination y; source +0/+2 dimensions, +4/+6 hotspot, bitmap +10h; SS must
; ASMDOC address DGROUP for row fraction accumulator.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 pitch +14h, bitmap segment +2, row-table +0Ah; first CS row-table word.
; ASMDOC globals_written: SS:source_row_fraction_accumulator; Bitmap memory selected by CS:_sprite1; descriptor fields
; ASMDOC unchanged.
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: No clipping in this public prologue. Dimensions use unsigned MUL(scale) with middle 16 product bits;
; ASMDOC hotspot uses signed IMUL middle bits. scale<2 rejected unsigned; zero scaled dimension rejected. Source
; ASMDOC step=floor(65536/scale), Q8 sampling; initial half-step selects sampling center; raw FFh skips (no incnums
; ASMDOC remap). Shares live tail _loc_35ED9..
; ASMDOC Proposed routine name: _shape2d_scale_blit_hotspot.
_scalefxy proc far
    local_horizontal_fraction = word ptr -20
    local_vertical_fraction = word ptr -18
    local_reciprocal_step_q8 = word ptr -16
    local_source_row_origin = word ptr -14
    local_destination_row_skip = word ptr -12
    local_source_width = word ptr -10
    local_rows_left = word ptr -8
    local_output_width = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_scale_q8 = word ptr 6
    arg_shape_offset = word ptr 8
    arg_shape_segment = word ptr 10
    arg_destination_x = word ptr 12
    arg_destination_y = word ptr 14

    push    bp
    mov     bp, sp
allocate_sprite_scale_locals:
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
prepare_sprite_dimensions:
    mov     si, [bp+arg_shape_offset]
    mov     cx, [bp+arg_destination_x]
    mov     ax, [si+4]
    imul    [bp+arg_scale_q8]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+local_destination_x], cx
    mov     cx, [bp+arg_destination_y]
    mov     ax, [si+6]
    imul    [bp+arg_scale_q8]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+local_destination_y], cx
    jmp     short validate_sprite_dimensions
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: scaled transparent blit at explicit position. Named public-entry control flow
; ASMDOC skips this stub; retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short validate_sprite_dimensions
    even ; word-align the following entry
return_invalid_sprite_size:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
; ASMDOC Dormant unlabeled prologue: scaled transparent blit at stored position. Named public-entry control flow skips
; ASMDOC this stub; retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Unclipped scale setup computes positive dimensions and reciprocal; later public tail is also used by
; ASMDOC shapeexpl.
validate_sprite_dimensions:
    cmp     [bp+arg_scale_q8], 2
    jb      short return_invalid_sprite_size
    cld
    mov     ax, [si+2]
    mul     [bp+arg_scale_q8]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short return_invalid_sprite_size
    mov     [bp+local_rows_left], ax
    mov     ax, [si]
    mov     [bp+local_source_width], ax
    mul     [bp+arg_scale_q8]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short return_invalid_sprite_size
    mov     [bp+local_output_width], ax
    mov     dx, cs:_sprite1.sprite_pitch
    sub     dx, ax
    mov     [bp+local_destination_row_skip], dx
    add     si, 10h
    mov     dx, 1
    xor     ax, ax
    mov     [bp+local_horizontal_fraction], ax
    mov     [bp+local_vertical_fraction], ax
    div     [bp+arg_scale_q8]
    mov     dx, ax
    mov     al, ah
    xor     ah, ah
    shr     ax, 1
    jz      short save_sprite_source_origin
    add     si, ax
    mov     cx, ax
step_sprite_source_rows:
    add     si, [bp+local_source_width]
    loop    step_sprite_source_rows
save_sprite_source_origin:
    mov     [bp+local_source_row_origin], si
    mov     [bp+local_reciprocal_step_q8], dx
    local_horizontal_fraction = word ptr -20
    local_vertical_fraction = word ptr -18
    local_reciprocal_step_q8 = word ptr -16
    local_source_row_origin = word ptr -14
    local_destination_row_skip = word ptr -12
    local_source_width = word ptr -10
    local_rows_left = word ptr -8
    local_output_width = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_scale_q8 = word ptr 6
    arg_shapeptr = dword ptr 8
    arg_destination_x = word ptr 12
    arg_destination_y = word ptr 14
; ASMDOC Contract _loc_35ED9: Shared scaled-transparent bitmap raster continuation, also reached from shapeexpl.
; ASMDOC convention: Public interior near-jump continuation, NOT a callable FAR/near procedure; falls through from
; ASMDOC local setup or JMP from _shapeexpl; shared unwind performs RETF to original caller.
; ASMDOC inputs: Inherited BP frame: [BP-2] x, [-4] y, [-6] positive output width, [-8] positive remaining rows, [-10]
; ASMDOC source width, [-12] destination pitch minus output width, [-14] source row origin, [-16] reciprocal Q8 step,
; ASMDOC [-18] initial vertical fraction, [-20] horizontal fraction; DS:SI current source pixel; SS=DGROUP; stack
; ASMDOC already contains saved DS, SI, DI from SUB SP,16h/PUSH DS/PUSH SI/PUSH DI; BP+2/+4 FAR return.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: Restores original caller BP, SP, DS, SI and DI from inherited frame; does not preserve
; ASMDOC continuation-entry DS/SI/DI values; SS and CS unchanged.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF inherited (normal setup supplies DF=0).
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2 and row-table +0Ah; first CS row-table word;
; ASMDOC SS:source_row_fraction_accumulator.
; ASMDOC globals_written: SS:source_row_fraction_accumulator; Inherited locals [BP-8] remaining rows and [BP-14]
; source
; ASMDOC row origin; Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Does not perform clipping itself; shapeexpl supplies clipped locals. Pixel AL=FFh leaves destination
; ASMDOC unchanged; otherwise write raw AL. Fraction high byte selects integer source skip then is cleared; source and
; ASMDOC destination arithmetic wrap at 16 bits. Final unwind restores saved DS/SI/DI/BP. Shared data state is
; ASMDOC non-reentrant..
; ASMDOC Proposed routine name: _shape2d_scale_blit_tail.
_shape2d_scale_blit_tail:
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:_sprite1.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     dx, [bp+local_reciprocal_step_q8]
    mov     ax, [bp+local_vertical_fraction]
    mov     ss:source_row_fraction_accumulator, ax
; ASMDOC Horizontal Q8 sampling restarts each row; vertical fraction lives in SS:DGROUP writable data.
begin_scaled_sprite_row:
    mov     cx, [bp+local_output_width]
    mov     bx, [bp+local_horizontal_fraction]
draw_scaled_sprite_pixels:
    mov     al, [si]
    cmp     al, 0FFh
    jz      short advance_scaled_sprite_pixel
    mov     es:[di], al
advance_scaled_sprite_pixel:
    inc     di
    add     bx, dx
    mov     al, bh
    xor     ah, ah
    add     si, ax
    xor     bh, bh
    loop    draw_scaled_sprite_pixels
    dec     [bp+local_rows_left]
    jle     short return_after_sprite_draw
    add     di, [bp+local_destination_row_skip]
    mov     si, [bp+local_source_row_origin]
    add     ss:source_row_fraction_accumulator, dx
    mov     cl, byte ptr ss:source_row_fraction_accumulator+1
    or      cl, cl
    jz      short begin_scaled_sprite_row
    xor     ch, ch
skip_source_rows_for_scale:
    add     si, [bp+local_source_width]
    loop    skip_source_rows_for_scale
    mov     byte ptr ss:source_row_fraction_accumulator+1, 0
    mov     [bp+local_source_row_origin], si
    jmp     short begin_scaled_sprite_row
return_after_sprite_draw:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    db 0 ; retained zero fill at this boundary
_scalefxy endp
S012_TEXT ends
end
