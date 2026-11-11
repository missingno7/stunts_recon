; ASMDOC(font_shapes) Module seg012_shape2d_extended_group. Replace placeholder extended operation names with the common
; ASMDOC(font_shapes) signed-packet RLE overwrite blit, with clipped and unclipped entry families. Public callable entries:
; ASMDOC(font_shapes) _nopsub_33DBE, _shape2d_op_unknown5, _shape2d_op_unk, _nopsub_33E90, _shape2d_op_unk2, _shape2d_op_unk3.
; ASMDOC(font_shapes) Contract offsets use BP after the standard prologue; FAR return IP/CS occupy BP+2/+4. Original source
; ASMDOC(font_shapes) comments remain verbatim and may contain older incomplete summaries; these added contracts describe
; ASMDOC(font_shapes) transitive shared exits.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33DBE
public _shape2d_op_unknown5
public _shape2d_op_unk
public _nopsub_33E90
public _shape2d_op_unk2
public _shape2d_op_unk3
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33DBE.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_33DBE (proposed _shape2d_blit_rle_hotspot): Decode a signed-packet RLE shape as pixel
; ASMDOC(font_shapes) overwrite, using requested coordinates minus header hotspot (+4,+6).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; signed-byte packet stream +10h; valid positive
; ASMDOC(font_shapes) dimensions and enough encoded/destination storage required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Shape height is not used by the
; ASMDOC(font_shapes) shared unclipped decoder; zero packet alone terminates it
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) No clipping, height limit, span checks or segment-wrap normalization; rows can advance past declared
; ASMDOC(font_shapes) height until a zero packet.
_nopsub_33DBE proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short shape_op_unk_shared_exit
    ; align 2
    db 144
_nopsub_33DBE endp
; -----------------------------------------------------------------------------
; Purpose: Process the fifth 2D shape operation.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _shape2d_op_unknown5 (proposed _shape2d_blit_rle_at): Decode a signed-packet RLE shape as pixel
; ASMDOC(font_shapes) overwrite, using explicit x,y.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; signed-byte packet stream +10h; valid positive
; ASMDOC(font_shapes) dimensions and enough encoded/destination storage required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Shape height is not used by the
; ASMDOC(font_shapes) shared unclipped decoder; zero packet alone terminates it
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) No clipping, height limit, span checks or segment-wrap normalization; rows can advance past declared
; ASMDOC(font_shapes) height until a zero packet.
_shape2d_op_unknown5 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short shape_op_unk_shared_exit ; goto shape2d_op_unk3
    ; align 2
    db 144
_shape2d_op_unknown5 endp
; -----------------------------------------------------------------------------
; Purpose: Process a 2D shape operation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shapeofs (word ptr 8); arg_shapeseg (word ptr 6).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _shape2d_op_unk (proposed _shape2d_blit_rle): Decode a signed-packet RLE shape as pixel
; ASMDOC(font_shapes) overwrite, using header position (+8,+0A).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; SHAPE2D +0 width word,+2 height word;
; ASMDOC(font_shapes) signed-byte packet stream +10h; valid positive dimensions and enough encoded/destination storage
; ASMDOC(font_shapes) required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Shape height is not used by the
; ASMDOC(font_shapes) shared unclipped decoder; zero packet alone terminates it
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) No clipping, height limit, span checks or segment-wrap normalization; rows can advance past declared
; ASMDOC(font_shapes) height until a zero packet.
_shape2d_op_unk proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_shapeseg = word ptr 6
    arg_shapeofs = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shapeofs]
    mov     si, [bp+arg_shapeseg]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
; ASMDOC(font_shapes) Unclipped entry family shares width and stream initialization. Height does not bound the common decoder.
shape_op_unk_shared_exit:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
shape_op_unk_draw_loop:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_6]
; ASMDOC(font_shapes) Signed packet: zero exits; positive consumes one repeated color; negative consumes literal bytes. Runs
; ASMDOC(font_shapes) may cross row boundaries.
shape_op_unk_read_loop:
    lodsb
    or      al, al
    jg      short shape_op_unk_when_greater
    jl      short shape_op_unk_when_less
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_op_unk_when_greater:
    mov     cl, al
    xor     ch, ch
    lodsb
shape_op_unk_fill_loop:
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    dec     dx
    jg      short shape_op_unk_when_greater_2
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
shape_op_unk_when_greater_2:
    loop    shape_op_unk_fill_loop
    jmp     short shape_op_unk_read_loop
shape_op_unk_when_less:
    neg     al
    mov     cl, al
    xor     ch, ch
shape_op_unk_copy_loop:
    movsb
    dec     dx
    jg      short shape_op_unk_when_greater_3
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
shape_op_unk_when_greater_3:
    loop    shape_op_unk_copy_loop
    jmp     short shape_op_unk_read_loop
    ; align 2
    db 0
_shape2d_op_unk endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33E90.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_33E90 (proposed _shape2d_blit_rle_clip_hotspot): Clip and decode a signed-packet RLE
; ASMDOC(font_shapes) shape as pixel overwrite, using requested coordinates minus header hotspot (+4,+6).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; signed-byte packet stream +10h; valid positive
; ASMDOC(font_shapes) dimensions and enough encoded/destination storage required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Writes stop after last visible
; ASMDOC(font_shapes) clipped row even if encoded stream continues
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes; CS:_sprite1+12/+14 left/right,+16/+18 top/bottom, exclusive right/bottom; signed word
; ASMDOC(font_shapes) comparisons
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) When fully inside the window, far JMP enters shape_op_unk_draw_loop in the unclipped routine, with the
; ASMDOC(font_shapes) already-built compatible frame. Otherwise source skips account for top/left/right clipping across packet
; ASMDOC(font_shapes) boundaries.
_nopsub_33E90 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short shape_op_unk3_shared_exit
    ; align 2
    db 144
_nopsub_33E90 endp
; -----------------------------------------------------------------------------
; Purpose: Process a 2D shape operation variant.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _shape2d_op_unk2 (proposed _shape2d_blit_rle_clipped_at): Clip and decode a signed-packet RLE
; ASMDOC(font_shapes) shape as pixel overwrite, using explicit x,y.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; [BP+10] s16 requested x; [BP+12] s16
; ASMDOC(font_shapes) requested y; SHAPE2D +0 width word,+2 height word; signed-byte packet stream +10h; valid positive
; ASMDOC(font_shapes) dimensions and enough encoded/destination storage required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Writes stop after last visible
; ASMDOC(font_shapes) clipped row even if encoded stream continues
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes; CS:_sprite1+12/+14 left/right,+16/+18 top/bottom, exclusive right/bottom; signed word
; ASMDOC(font_shapes) comparisons
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) When fully inside the window, far JMP enters shape_op_unk_draw_loop in the unclipped routine, with the
; ASMDOC(font_shapes) already-built compatible frame. Otherwise source skips account for top/left/right clipping across packet
; ASMDOC(font_shapes) boundaries.
_shape2d_op_unk2 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short shape_op_unk3_shared_exit ; goto somewhere inside shape2d_op_unk3
    ; align 2
    db 144
_shape2d_op_unk2 endp
; -----------------------------------------------------------------------------
; Purpose: Process a 2D shape operation variant.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shape2dofs (word ptr 6); arg_shape2dseg (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _shape2d_op_unk3 (proposed _shape2d_blit_rle_clipped): Clip and decode a signed-packet RLE
; ASMDOC(font_shapes) shape as pixel overwrite, using header position (+8,+0A).
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP / MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] SHAPE2D far offset; [BP+8] SHAPE2D far segment; SHAPE2D +0 width word,+2 height word;
; ASMDOC(font_shapes) signed-byte packet stream +10h; valid positive dimensions and enough encoded/destination storage
; ASMDOC(font_shapes) required
; ASMDOC(font_shapes) outputs: No scalar return; positive control 1..127 repeats following color; negative -1..-128 copies
; ASMDOC(font_shapes) that many literals; zero control ends stream; Runs cross row boundaries; destination row offsets
; ASMDOC(font_shapes) resolved from sprite1 line table; no transparency or Boolean combine; Writes stop after last visible
; ASMDOC(font_shapes) clipped row even if encoded stream continues
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:_sprite1+2 bitmap segment,+10 line table; CS:line-table entries; shape width/header and
; ASMDOC(font_shapes) encoded bytes; CS:_sprite1+12/+14 left/right,+16/+18 top/bottom, exclusive right/bottom; signed word
; ASMDOC(font_shapes) comparisons
; ASMDOC(font_shapes) globals_written: sprite1 bitmap pixels within computed span
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Header source offset/segment equate names are swapped in _shape2d_op_unk, but the instructions still
; ASMDOC(font_shapes) load offset from BP+6 and segment from BP+8.
; ASMDOC(font_shapes) Hotspot/explicit stubs jump into the respective shared body; all return via its DS/SI/DI/BP-restoring
; ASMDOC(font_shapes) epilogue.
; ASMDOC(font_shapes) When fully inside the window, far JMP enters shape_op_unk_draw_loop in the unclipped routine, with the
; ASMDOC(font_shapes) already-built compatible frame. Otherwise source skips account for top/left/right clipping across packet
; ASMDOC(font_shapes) boundaries.
_shape2d_op_unk3 proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_shape2dofs = word ptr 6
    arg_shape2dseg = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape2dseg]
    mov     si, [bp+arg_shape2dofs]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
; ASMDOC(font_shapes) Clipped family clips signed coordinates and extents against all four active sprite bounds before
; ASMDOC(font_shapes) decoding source skips.
shape_op_unk3_shared_exit:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
    xor     di, di
    xor     dx, dx
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+16]
    jl      short shape_op_unk3_when_less
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short op_unk3_when_less_or_equal
    inc     di
    sub     cx, ax
    jg      short op_unk3_when_less_or_equal
shape_op_unk3_draw_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_op_unk3_when_less:
    inc     di
    mov     bx, word ptr cs:[_sprite1+16]
    add     ax, cx
    sub     ax, bx
    jle     short shape_op_unk3_draw_loop
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+var_6]
    mov     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short op_unk3_when_less_or_equal
    sub     cx, ax
    jle     short shape_op_unk3_draw_loop
op_unk3_when_less_or_equal:
    mov     [bp+var_8], cx
    mov     [bp+var_4], bx
    mov     [bp+var_A], dx
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    mov     cx, [bp+var_6]
    cmp     ax, word ptr cs:[_sprite1+12]
    jl      short shape_op_unk3_when_less_2
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+14]
    jle     short shape_op_unk3_shared_path
    sub     cx, ax
    jle     short shape_op_unk3_draw_loop
    inc     di
    mov     dx, ax
    jmp     short shape_op_unk3_shared_path
    db 144
shape_op_unk3_when_less_2:
    inc     di
    mov     bx, word ptr cs:[_sprite1+12]
    add     ax, cx
    sub     ax, bx
    jle     short shape_op_unk3_draw_loop
    mov     si, cx
    sub     si, ax
    add     [bp+var_A], si
    mov     si, word ptr cs:[_sprite1+14]
    sub     si, bx
    cmp     ax, si
    jl      short shape_op_unk3_when_less_3
    mov     ax, si
shape_op_unk3_when_less_3:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
shape_op_unk3_shared_path:
    mov     [bp+var_6], cx
    mov     [bp+var_C], dx
    mov     [bp+var_2], bx
    or      di, di
    jnz     short shape_op_unk3_when_nonzero
; ASMDOC(font_shapes) Fully-contained shapes reuse the unclipped decoder by a far jump with the same live frame; no far return
; ASMDOC(font_shapes) address is added.
    jmp far ptr shape_op_unk_draw_loop
; ASMDOC(font_shapes) A clipped span must skip encoded source pixels and stop after the final visible row, even in the middle
; ASMDOC(font_shapes) of a packet.
shape_op_unk3_when_nonzero:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_A]
    or      dx, dx
    jnz     short shape_op_unk3_when_nonzero_2
shape_op_unk3_read_loop:
    mov     dx, [bp+var_6]
shape_op_unk3_read_loop_2:
    lodsb
    or      al, al
    jg      short shape_op_unk3_when_greater
    jl      short shape_op_unk3_when_less_4
shape_op_unk3_scan_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_op_unk3_when_greater:
    mov     cl, al
    xor     ch, ch
    lodsb
shape_op_unk3_fill_loop:
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    dec     dx
    jle     short op_unk3_when_less_or_equal_2
    loop    shape_op_unk3_fill_loop
    jmp     short shape_op_unk3_read_loop_2
op_unk3_when_less_or_equal_2:
    dec     [bp+var_8]
    jz      short shape_op_unk3_scan_loop
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    dec     cx
    mov     dx, [bp+var_C]
shape_op_unk3_scan_loop_2:
    sub     cx, dx
    jz      short shape_op_unk3_read_loop
    jl      short shape_op_unk3_when_less_6
    mov     dx, [bp+var_6]
    jmp     short shape_op_unk3_fill_loop
shape_op_unk3_when_less_4:
    neg     al
    mov     cl, al
    xor     ch, ch
shape_op_unk3_copy_loop:
    movsb
    dec     dx
    jle     short op_unk3_when_less_or_equal_3
    loop    shape_op_unk3_copy_loop
    jmp     short shape_op_unk3_read_loop_2
op_unk3_when_less_or_equal_3:
    dec     [bp+var_8]
    jz      short shape_op_unk3_scan_loop
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    dec     cx
    mov     dx, [bp+var_C]
shape_op_unk3_scan_loop_3:
    add     si, dx
    sub     cx, dx
    jz      short shape_op_unk3_read_loop
    jl      short shape_op_unk3_when_less_5
    mov     dx, [bp+var_6]
    jmp     short shape_op_unk3_copy_loop
shape_op_unk3_when_less_5:
    add     si, cx
shape_op_unk3_when_less_6:
    neg     cx
    mov     dx, cx
; ASMDOC(font_shapes) Consume hidden pixels as packet counts/literals, retaining any remainder for visible output.
shape_op_unk3_when_nonzero_2:
    lodsb
    or      al, al
    jg      short shape_op_unk3_when_greater_2
    jl      short shape_op_unk3_when_less_7
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_op_unk3_when_greater_2:
    mov     cl, al
    xor     ch, ch
    lodsb
    jmp     short shape_op_unk3_scan_loop_2
shape_op_unk3_when_less_7:
    neg     al
    mov     cl, al
    xor     ch, ch
    jmp     short shape_op_unk3_scan_loop_3
    ; align 2
    db 0
_shape2d_op_unk3 endp
S012_TEXT ends
end
