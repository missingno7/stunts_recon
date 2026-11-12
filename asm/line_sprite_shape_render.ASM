INCLUDE include/video.inc
; ASMDOC Module review: Existing name is descriptive; preserve its mixed span, capture, scaled-shape, data, and
; font extent.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _draw_filled_lines, _nopsub_34736, _sprite_clear_shape_alt, _sprite_clear_shape, _shapeexpl,
; ASMDOC _font_setup_unknown, _set_fontdefseg. Stack offsets are post-prologue BP offsets.
.8086
INCLUDE include\platform_hw.inc
name asm012_149180
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
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
extrn _fontdefseg:word
extrn _shape2d_scale_blit_tail:near
public _draw_filled_lines
public _readfxy
public _sprite_clear_shape_alt
public _sprite_clear_shape
public _scalecfxy
public _sprite1
public _sprite2
public _font_setup_unknown
public _setfont
; -----------------------------------------------------------------------------
; Purpose: Draw filled scan lines from the prepared line descriptors.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_color (word ptr
; 14); arg_numlines (word ptr 12); arg_x1arr (word ptr 6); arg_x2arr
;   (word ptr 8); arg_y (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _draw_filled_lines; proposed name _draw_filled_lines
; ASMDOC Purpose: Fill inclusive left/right spans with the low color byte, selecting byte/word stores by
; destination alignment.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] near DS offset of signed 16-bit inclusive left endpoints, one word per row; [BP+08]
; near DS offset of
; ASMDOC signed 16-bit inclusive right endpoints, one word per row; [BP+0A] first y word; caller supplies valid
; row-table index;
; ASMDOC [BP+0C] signed row count; zero/negative behavior is entry-specific; [BP+0E] primary color (low byte
; only); DS addresses
; ASMDOC endpoint arrays; caller supplies valid rows/spans; no clipping.
; ASMDOC Outputs: No semantic register result; destination pixels changed; Caller stack slots [BP+06]/[BP+08]
; advance by 2 each
; ASMDOC row; [BP+0C] decremented in place.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return; DX.
; ASMDOC Clobbers: AX; BX; CX; ES; arithmetic flags; DF=0 only if row count nonzero.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A) and addressed row offsets.
; ASMDOC Globals written: sprite1 backing pixels; caller stack slots +06/+08/+0C.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Address taken in asm/prerender_wheel_raster.ASM:101 etc.; direct call
; ASMDOC asm/vector_sphere_sprite_ops.ASM:287.
; ASMDOC Edge cases: Zero rows returns without setting DF or ES. Negative nonzero row count draws one row.
; Signed nonpositive
; ASMDOC (right-left+1) skips pixels.
_draw_filled_lines proc far
    arg_x1arr = word ptr 6
    arg_x2arr = word ptr 8
    arg_y = word ptr 10
    arg_numlines = word ptr 12
    arg_color = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 2
    push    si
    push    di
    cmp     [bp+arg_numlines], 0
    jz      short fill_lines_branch_path_01 ; if arg6 == 0, return
    cld
    mov     si, [bp+arg_y]
    shl     si, 1
    add     si, cs:[_sprite1.sprite_lineofs]
    mov     ax, cs:[_sprite1.sprite_bitmapseg]
    mov     es, ax
    mov     ax, [bp+arg_color]
    mov     ah, al
; ASMDOC Common row loop: endpoint difference is signed 16-bit plus one. Span clipping is caller-owned; stack
; pointer arguments
; ASMDOC are advanced in place.
fill_lines_loop_head_01:
    mov     bx, [bp+arg_x2arr]
    mov     cx, [bx]
    mov     bx, [bp+arg_x1arr]
    mov     bx, [bx]
    sub     cx, bx
    inc     cx
    jle     short fill_lines_loop_head_02
    mov     di, cs:[si]     ; si = offset in lineofs table, di = y offset in bitmapptr
    add     di, bx
    test    cx, 0FFF8h
    jnz     short fill_lines_branch_path_02 ; jump if (cx & 0xFFF8), ie jump if cx >= 8.
    rep stosb
; rep count = cx = (arg2->member0 - arg0->member0) + 1 /* PLATFORM(video): writes display or sprite memory
; through ES:DI. */
fill_lines_loop_head_02:
    add     [bp+arg_x2arr], 2
    add     [bp+arg_x1arr], 2
    add     si, 2
    dec     [bp+arg_numlines]
    jg      short fill_lines_loop_head_01
fill_lines_branch_path_01:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
fill_lines_branch_path_02:
    test    di, 01h           ; looks like its figuring out which alignment optimizations to use
    jnz     short fill_lines_branch_path_04
    shr     cx, 1
    jb      short fill_lines_branch_path_03
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_03:
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_04:
    shr     cx, 1
    jb      short fill_lines_branch_path_05
    dec     cx
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_05:
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
    db 0 ; retained zero fill at this boundary
_draw_filled_lines endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 34736.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_34736; proposed name _shape2d_capture_bitmap_hotspot
; ASMDOC Purpose: Capture active-sprite rectangle into shape bitmap at explicit coordinates minus shape hotspot
; words +04/+06.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] destination SHAPE2D offset; [BP+08] destination SHAPE2D segment; Shape width/height
; words +00/+02;
; ASMDOC destination bitmap starts +10h; No clip or source/destination extent checks; caller supplies positive
; height and valid
; ASMDOC spans; [BP+0A] x word; [BP+0C] y word; subtract shape signed hotspot modulo 16 bits.
; ASMDOC Outputs: Shape bitmap receives width bytes per row, consecutive destination rows; no semantic register
; result.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A), row offsets; destination shape header
; ASMDOC width/height and selected origin/hotspot fields.
; ASMDOC Globals written: destination far shape bitmap (+10h onward).
; ASMDOC Services: none.
; ASMDOC Dead: true. Reachability: No runtime symbol reference or address-taking found in tracked src/ or asm/; joins
; ASMDOC shape_clear_join_01, not a no-op.
; ASMDOC Shared tail: All three entries establish compatible BP locals and save DS/SI/DI before joining line
; 228; shared epilogue
; ASMDOC at 251 restores them. Core switches DS to sprite pixels and ES to caller shape segment. Height <=0
; still copies one row.
_readfxy proc far
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
    mov     di, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    sub     ax, [di+4]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    sub     ax, [di+6]
    mov     [bp+local_destination_y], ax
    jmp     short shape_clear_join_01
    even ; word-align the following entry
_readfxy endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite clear shape alt.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_clear_shape_alt; proposed name _sprite_clear_shape_alt
; ASMDOC Purpose: Store explicit x/y into the destination shape header then capture that active-sprite rectangle
; into the shape
; ASMDOC bitmap.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] destination SHAPE2D offset; [BP+08] destination SHAPE2D segment; Shape width/height
; words +00/+02;
; ASMDOC destination bitmap starts +10h; No clip or source/destination extent checks; caller supplies positive
; height and valid
; ASMDOC spans; [BP+0A] x word; [BP+0C] y word; copied to shape +08/+0A.
; ASMDOC Outputs: Shape bitmap receives width bytes per row, consecutive destination rows; no semantic register
; result.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A), row offsets; destination shape header
; ASMDOC width/height and selected origin/hotspot fields.
; ASMDOC Globals written: destination far shape bitmap (+10h onward); destination shape +08/+0A.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers src/obj_seg000.c:1518 and src/obj_seg008.c:170.
; ASMDOC Shared tail: All three entries establish compatible BP locals and save DS/SI/DI before joining line
; 228; shared epilogue
; ASMDOC at 251 restores them. Core switches DS to sprite pixels and ES to caller shape segment. Height <=0
; still copies one row.
_sprite_clear_shape_alt proc far
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
    mov     di, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     [di+8], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    mov     [di+0Ah], ax
    jmp     short shape_clear_join_01
    even ; word-align the following entry
_sprite_clear_shape_alt endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite clear shape.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_clear_shape; proposed name _sprite_clear_shape
; ASMDOC Purpose: Capture active-sprite rectangle using x/y already stored in shape +08/+0A; this saves pixels
; and does not clear
; ASMDOC either buffer.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] destination SHAPE2D offset; [BP+08] destination SHAPE2D segment; Shape width/height
; words +00/+02;
; ASMDOC destination bitmap starts +10h; No clip or source/destination extent checks; caller supplies positive
; height and valid
; ASMDOC spans; Shape +08/+0A supply x/y.
; ASMDOC Outputs: Shape bitmap receives width bytes per row, consecutive destination rows; no semantic register
; result.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A), row offsets; destination shape header
; ASMDOC width/height and selected origin/hotspot fields.
; ASMDOC Globals written: destination far shape bitmap (+10h onward).
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct caller src/obj_seg000.c:863.
; ASMDOC Shared tail: All three entries establish compatible BP locals and save DS/SI/DI before joining line
; 228; shared epilogue
; ASMDOC at 251 restores them. Core switches DS to sprite pixels and ES to caller shape segment. Height <=0
; still copies one row.
_sprite_clear_shape proc far
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
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
    mov     di, [bp+arg_shape_offset]
    mov     ax, [di+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [di+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Shared capture core, entered with destination shape in DS:DI and x/y locals. Reads sprite pixels
; through DS, writes
; ASMDOC shape payload through ES; does not clear pixels.
shape_clear_join_01:
    mov     ax, [di]
    mov     [bp+var_8], ax
    mov     ax, [di+2]
    mov     [bp+var_A], ax
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:[_sprite1.sprite_lineofs]
    mov     [bp+var_6], bx
    mov     ds, word ptr cs:[_sprite1.sprite_bitmapseg]
    mov     es, [bp+arg_shape_segment]
    add     di, 10h
    cld
    mov     dx, [bp+var_A]
shape_clear_loop_head_01:
    mov     bx, [bp+var_6]
    mov     si, cs:[bx]
    add     si, [bp+local_destination_x]
    mov     cx, [bp+var_8]
    rep movsb
    add     [bp+var_6], 2
    dec     dx
    jg      short shape_clear_loop_head_01
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_clear_shape endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by shapeexpl.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_6
; (word ptr 12); arg_8 (word ptr 12); arg_shapeptr (dword ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _shapeexpl; proposed name _shapeexpl
; ASMDOC Purpose: Scale a transparent bitmap by unsigned 8.8 scale, subtract signed scaled hotspot, clip to
; sprite1 secondary
; ASMDOC horizontal bounds and vertical bounds, then tail-jump into the shared transparent scaled blitter.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] scale word (unsigned checks/dimension products; signed IMUL for hotspot products);
; [BP+08] source shape
; ASMDOC offset; [BP+0A] source shape segment; [BP+0C] anchor x word; [BP+0E] anchor y word; Source header
; +00/+02 width/height,
; ASMDOC +04/+06 signed hotspot; pixels +10h; FFh transparent.
; ASMDOC Outputs: No semantic register result; clipped sprite1 backing pixels updated by _loc_35ED9; Scale <2 or
; zero scaled
; ASMDOC dimensions / fully clipped rectangle returns without drawing.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES on drawing path; arithmetic flags; DF=0 after accepted scale check.
; ASMDOC Globals read: _sprite1 pitch(+14h), left2(+1Ah), widthsum(+1Ch), top(+10h), bottom(+12h); _sprite1
; bitmap segment(+2),
; ASMDOC CS row-table pointer(+0Ah) via shared blit tail; caller source header and pixels.
; ASMDOC Globals written: sprite1 backing pixels; SS:source_row_fraction_accumulator via external tail.
; ASMDOC Services: _loc_35ED9 near continuation in asm/sprite_rectangle_scaled_blitters.ASM:713 (not CALL;
; consumes this frame).
; ASMDOC Dead: false. Reachability: Direct caller src/obj_seg003.c:1926.
; ASMDOC Shared tail: Frame reserves 16h bytes and pushes DS/SI/DI. Tail requires local
; ASMDOC
; x=-2,y=-4,width=-6,height=-8,sourcewidth=-0A,rowstride=-0C,sourcebase=-0E,step=-10,yfraction=-12,xfraction=-14
; and DS:SI
; ASMDOC source. Tail restores DS/SI/DI/BP and RETFs to original caller.
; ASMDOC Edge cases: Unlabeled alternate coordinate/header-origin prologues at lines 308 and 330 are bypassed.
; Bytes 449..650 are
; ASMDOC public descriptors and row table inside PROC extent, never executed by normal entry.
_scalecfxy proc far
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
    arg_anchor_x = word ptr 12
    arg_anchor_y = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
    mov     cx, [bp+arg_anchor_x]
    mov     ax, [si+4]
    imul    [bp+arg_scale_q8]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+local_destination_x], cx
    mov     cx, [bp+arg_anchor_y]
    mov     ax, [si+6]
    imul    [bp+arg_scale_q8]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+local_destination_y], cx
    jmp     short shape_explode_join_01
    even ; word-align the following entry
; ASMDOC Unexported explicit-coordinate alternate prologue. Public hotspot entry jumps over it; no normal
; fallthrough from public
; ASMDOC entry.
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
    mov     ax, [bp+arg_anchor_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_anchor_y]
    mov     [bp+local_destination_y], ax
    jmp     short shape_explode_join_01
    even ; word-align the following entry
shape_explode_loop_head_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
; ASMDOC Unexported shape-origin alternate prologue following a RETF. Public hotspot entry does not reach this
; prologue.
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC 8.8 scale core: scaled dimensions are unsigned product >>8, hotspot setup used signed products. Prepare
; matching frame
; ASMDOC locals for external near continuation.
shape_explode_join_01:
    cmp     [bp+arg_scale_q8], 2
    jb      short shape_explode_loop_head_01
    cld
    mov     ax, [si+2]
    mul     [bp+arg_scale_q8]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short shape_explode_loop_head_01
    mov     [bp+local_rows_left], ax
    mov     ax, [si+0]
    mov     [bp+local_source_width], ax
    mul     [bp+arg_scale_q8]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short shape_explode_loop_head_01
    mov     [bp+local_output_width], ax
    mov     dx, cs:_sprite1.sprite_pitch
    sub     dx, ax
    mov     [bp+local_destination_row_skip], dx
    add     si, 10h
    mov     dx, 1
    xor     ax, ax
    mov     [bp+local_vertical_fraction], ax
    mov     [bp+local_horizontal_fraction], ax
    div     [bp+arg_scale_q8]
    mov     [bp+local_reciprocal_step_q8], ax
    mov     al, ah
    xor     ah, ah
    shr     ax, 1
    jz      short shape_explode_branch_path_01
    add     si, ax
    mov     cx, ax
shape_explode_loop_head_02:
    add     si, [bp+local_source_width]
    loop    shape_explode_loop_head_02
shape_explode_branch_path_01:
    mov     cx, [bp+local_output_width]
    mov     ax, [bp+local_destination_x]
    mov     bx, cs:_sprite1.sprite_left2
    cmp     ax, bx
    jge     short shape_explode_branch_path_02
    add     ax, cx
    sub     ax, bx
    jle     short shape_explode_branch_path_06
    mov     [bp+local_destination_x], bx
    mov     [bp+local_output_width], ax
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+local_reciprocal_step_q8]
    xchg    al, dl
    xor     dh, dh
    mov     [bp+local_horizontal_fraction], dx
    xchg    al, ah
    add     si, ax
    mov     ax, bx
shape_explode_branch_path_02:
    add     ax, cx
    sub     ax, cs:_sprite1.sprite_widthsum
    jl      short shape_explode_branch_path_03
    sub     cx, ax
    jle     short shape_explode_branch_path_06
    mov     [bp+local_output_width], cx
shape_explode_branch_path_03:
    mov     ax, cs:_sprite1.sprite_pitch
    sub     ax, cx
    mov     [bp+local_destination_row_skip], ax
    mov     cx, [bp+local_rows_left]
    mov     ax, [bp+local_destination_y]
    mov     bx, cs:_sprite1.sprite_top
    cmp     ax, bx
    jge     short shape_explode_branch_path_04
    add     ax, cx
    sub     ax, bx
    jle     short shape_explode_branch_path_06
    mov     [bp+local_rows_left], ax
    mov     [bp+local_destination_y], bx
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+local_reciprocal_step_q8]
    xchg    al, dl
    xor     dh, dh
    mov     [bp+local_vertical_fraction], dx
    xchg    al, ah
    mul     [bp+local_source_width]
    add     si, ax
    mov     ax, bx
shape_explode_branch_path_04:
    add     ax, cx
    sub     ax, cs:_sprite1.sprite_height
    jl      short shape_explode_branch_path_05
    sub     cx, ax
    jle     short shape_explode_branch_path_06
    mov     [bp+local_rows_left], cx
shape_explode_branch_path_05:
    mov     dx, [bp+local_reciprocal_step_q8]
    mov     [bp+local_source_row_origin], si
; ASMDOC Tail jump into another module; _loc_35ED9 consumes this frame, sets ES and writes pixels (FFh
; transparent), and mutates
; ASMDOC SS:source_row_fraction_accumulator.
    jmp near ptr _shape2d_scale_blit_tail
shape_explode_branch_path_06:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
; ASMDOC PUBLIC DATA _sprite1/_sprite2 (30 bytes each), followed by 200 row offsets y*320. These words live
; inside PROC extent
; ASMDOC but are never executed by normal shapeexpl paths.
_sprite1 dw 0, 0A000h, 0, 0, 0, offset lineoffsets
    dw 0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS
    dw PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_WIDTH_PIXELS
    dw 0, PLATFORM_SCREEN_WIDTH_PIXELS
; /* PLATFORM(video): selects a video-memory segment. */
_sprite2 dw 0, 0A000h, 0, 0, 0, offset lineoffsets
    dw 0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_HEIGHT_PIXELS
    dw PLATFORM_SCREEN_WIDTH_PIXELS, 0, PLATFORM_SCREEN_WIDTH_PIXELS
    dw 0, PLATFORM_SCREEN_WIDTH_PIXELS
; /* PLATFORM(video): selects a video-memory segment. */
; Scanline offsets = 320*y, y=0..199; exact. Verified 200/200, no exceptions.
lineoffsets label word
row_number = 0
    REPT PLATFORM_SCREEN_HEIGHT_PIXELS
    dw row_number * PLATFORM_SCREEN_WIDTH_PIXELS
row_number = row_number + 1
    ENDM
_scalecfxy endp
; -----------------------------------------------------------------------------
; Purpose: Implement the graphics or geometry helper identified by font setup unknown.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _fontdefseg.
; Globals written: _fontdefseg.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _font_setup_unknown; proposed name _font_set_colors
; ASMDOC Purpose: Set foreground/background font words to zero-extended argument low bytes in current font segment.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] foreground low byte; [BP+08] background low byte; DS addresses _fontdefseg; font
; segment offset 0 is
; ASMDOC header base.
; ASMDOC Outputs: font[0]=foreground & 255; font[2]=background & 255; AX=foreground & 255; ES=current font segment.
; ASMDOC Preserves: BP; BX; CX; DX; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; ES; arithmetic flags from XOR AH,AH; DF unchanged.
; ASMDOC Globals read: _fontdefseg.
; ASMDOC Globals written: current font segment words +00/+02 (not _fontdefseg itself).
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers src/obj_seg000.c:1035 and port/font.c:72.
; ASMDOC Edge cases: Later unlabeled code at 677/686/705 is separate unreachable-from-this-entry font
; operations; do not
; ASMDOC aggregate their register/global effects into this public entry.
_font_setup_unknown proc far
    arg_foreground = word ptr 6
    arg_background = word ptr 8

    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+arg_background]
    xor     ah, ah
    mov     es:2, ax
    mov     ax, [bp+arg_foreground]
    xor     ah, ah
    mov     es:0, ax
    pop     bp
    retf
; ASMDOC Unexported font-coordinate setter after RETF: BP+6/BP+8 -> current font words +8/+0A; separate operation, not
; ASMDOC fallthrough from color setter.
    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+6]
    mov     es:8, ax
    mov     ax, [bp+8]
    mov     es:0Ah, ax
    pop     bp
    retf
; ASMDOC Unexported font-state save after RETF: near DS destination BP+6 gets 11 header words and font segment
; at +16h; restores
; ASMDOC DS/SI/DI, sets ES to caller DS, CX=0, DF=0.
    push    bp
    mov     bp, sp
    push    ds
    push    di
    push    si
    xor     si, si
    mov     di, [bp+6]
    mov     ax, ds
    mov     es, ax
    mov     ds, ds:[_fontdefseg]
    cld
    mov     word ptr es:[di+16h], ds ;/* PLATFORM(video): accesses display memory through ES. */
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     ds
    pop     bp
    retf
; ASMDOC Unexported font-state restore after RETF: near DS source BP+6 supplies font segment +16h and 11 words;
; changes
; ASMDOC _fontdefseg and selected font header; restores SI/DI, sets ES/CX/DF.
    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+6]
    mov     ax, [si+16h]
    mov     ds:[_fontdefseg], ax
    mov     es, ax
    xor     di, di
    cld
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     bp
    retf
_font_setup_unknown endp
; -----------------------------------------------------------------------------
; Purpose: Implement the video or display setup operation identified by set fontdefseg.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8).
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _fontdefseg.
; -----------------------------------------------------------------------------
; ASMDOC Contract _set_fontdefseg; proposed name _set_fontdefseg
; ASMDOC Purpose: Select the font definition by storing only the supplied far-pointer segment word; offset
; argument is ignored.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] far pointer offset, unused; [BP+08] far pointer segment; DS addresses _fontdefseg.
; ASMDOC Outputs: DS:_fontdefseg = segment; AX = segment.
; ASMDOC Preserves: BP; BX; CX; DX; SI; DI; DS; ES; SS; CS; all flags including DF; SP balanced except consumed
; far return.
; ASMDOC Clobbers: AX.
; ASMDOC Globals read: none.
; ASMDOC Globals written: _fontdefseg.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct caller src/obj_seg008.c:1454; port/font.c:53 adapts far segment to
; host pointer.
_setfont proc far
    arg_font_segment = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_font_segment]
    mov     ds:[_fontdefseg], ax
    pop     bp
    retf
    db 0 ; retained zero fill at this boundary
_setfont endp
S012_TEXT ends
end
