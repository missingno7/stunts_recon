; ASMDOC(font_shapes) Module seg012_shape_to_1_group. Name the common unclipped opaque bitmap copy, including
; ASMDOC(font_shapes) explicit-position, header-position and hotspot entries. Public callable entries: _nopsub_33D0C,
; ASMDOC(font_shapes) _sprite_shape_to_1, _sprite_shape_to_1_alt.
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
public _movfxy
public _movfxya
public _sprite_shape_to_1_alt
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33D0C.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_33D0C (proposed _shape2d_copy_bitmap_hotspot): Copy all bytes of each
; shape bitmap row
; ASMDOC(font_shapes) to sprite1 without clipping, using requested coordinates minus header hotspot (+4,+6).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x;
; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; pixel data starts +10h; positive
; width/height and
; ASMDOC(font_shapes) nonwrapping valid row storage expected
; ASMDOC(font_shapes) outputs: No scalar return; opaque byte rows replace destination pixels
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment, +10 line-table offset, +20 pitch;
; CS:line_table[2*y]; shape
; ASMDOC(font_shapes) header and pixels
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixel bytes
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and
; src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference
; found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) All three prologues join sprite_shape_to_1_shared_exit inside _sprite_shape_to_1_alt and
; use its common
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) First row is always copied; height termination uses signed JG after decrement. Width
; parity selects
; ASMDOC(font_shapes) word/byte copy path; destination next-row stepping uses sprite1 pitch, not another
; line-table lookup.
; ASMDOC(font_shapes) MOVS is forward copy, not overlap-safe memmove; no source/destination segment-wrap adjustment.
_movfxy proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    sub     ax, [si+4]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    sub     ax, [si+6]
    mov     [bp+local_destination_y], ax
    jmp     short sprite_shape_to_1_shared_exit
    even ; word-align the following entry
_movfxy endp
; -----------------------------------------------------------------------------
; Purpose: Copy the selected shape into sprite image 1.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _sprite_shape_to_1 (proposed _sprite_shape_to_1): Copy all bytes of each shape
; bitmap row to
; ASMDOC(font_shapes) sprite1 without clipping, using explicit x,y.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x;
; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; pixel data starts +10h; positive
; width/height and
; ASMDOC(font_shapes) nonwrapping valid row storage expected
; ASMDOC(font_shapes) outputs: No scalar return; opaque byte rows replace destination pixels
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment, +10 line-table offset, +20 pitch;
; CS:line_table[2*y]; shape
; ASMDOC(font_shapes) header and pixels
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixel bytes
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) All three prologues join sprite_shape_to_1_shared_exit inside _sprite_shape_to_1_alt and
; use its common
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) First row is always copied; height termination uses signed JG after decrement. Width
; parity selects
; ASMDOC(font_shapes) word/byte copy path; destination next-row stepping uses sprite1 pitch, not another
; line-table lookup.
; ASMDOC(font_shapes) MOVS is forward copy, not overlap-safe memmove; no source/destination segment-wrap adjustment.
_movfxya proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short sprite_shape_to_1_shared_exit
    even ; word-align the following entry
_movfxya endp
; -----------------------------------------------------------------------------
; Purpose: Copy the selected shape into sprite image 1 through the alternate path.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _sprite_shape_to_1_alt (proposed _sprite_shape_to_1_alt): Copy all bytes of each
; shape bitmap
; ASMDOC(font_shapes) row to sprite1 without clipping, using header position (+8,+0A).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; SHAPE2D +0 width word,+2
; height word;
; ASMDOC(font_shapes) pixel data starts +10h; positive width/height and nonwrapping valid row storage expected
; ASMDOC(font_shapes) outputs: No scalar return; opaque byte rows replace destination pixels
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment, +10 line-table offset, +20 pitch;
; CS:line_table[2*y]; shape
; ASMDOC(font_shapes) header and pixels
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixel bytes
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) All three prologues join sprite_shape_to_1_shared_exit inside _sprite_shape_to_1_alt and
; use its common
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) First row is always copied; height termination uses signed JG after decrement. Width
; parity selects
; ASMDOC(font_shapes) word/byte copy path; destination next-row stepping uses sprite1 pitch, not another
; line-table lookup.
; ASMDOC(font_shapes) MOVS is forward copy, not overlap-safe memmove; no source/destination segment-wrap adjustment.
_sprite_shape_to_1_alt proc far
    local_source_width = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC(font_shapes) All three coordinate-policy prologues join this opaque forward bitmap copy. Initial line
; lookup plus x
; ASMDOC(font_shapes) supplies first destination, then pitch steps rows.
sprite_shape_to_1_shared_exit:
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     es, word ptr cs:[_sprite1+2]
    cld
    mov     dx, [si+2]
    mov     bx, word ptr cs:[_sprite1+20]
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+local_source_width], ax
    jnb     short sprite_shape_to_1_copy_loop_3
    jz      short sprite_shape_to_1_copy_loop_4
; ASMDOC(font_shapes) Odd widths greater than one: words followed by one byte per row; no clipping or overlap
; buffering.
sprite_shape_to_1_copy_loop:
    mov     cx, [bp+local_source_width]
    rep movsw
    movsb
    add     di, bx
    dec     dx
    jg      short sprite_shape_to_1_copy_loop
sprite_shape_to_1_copy_loop_2:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
sprite_shape_to_1_copy_loop_3:
    mov     cx, [bp+local_source_width]
    rep movsw
    add     di, bx
    dec     dx
    jg      short sprite_shape_to_1_copy_loop_3
    jmp     short sprite_shape_to_1_copy_loop_2
sprite_shape_to_1_copy_loop_4:
    movsb
    add     di, bx
    dec     dx
    jg      short sprite_shape_to_1_copy_loop_4
    jmp     short sprite_shape_to_1_copy_loop_2
    db 0 ; retained zero fill at this boundary
_sprite_shape_to_1_alt endp
S012_TEXT ends
end
