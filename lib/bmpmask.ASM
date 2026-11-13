; ASMDOC Module review: Placeholder group name replaced by signed-run AND-mask decoder family; no split/merge.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _nopsub_33AC0, _nopsub_33AE4, _shape2d_render_bmp_as_mask. Stack offsets are
; post-prologue BP offsets.
.8086
INCLUDE include\hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _andpackfxy
public _andpackfxya
public _andpackf
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33AC0.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_33AC0; proposed name _shape2d_and_rle_hotspot
; ASMDOC Purpose: AND a signed packet stream into sprite1 at explicit coordinates minus shape hotspot. Positive
; control repeats
; ASMDOC next byte, negative control consumes literal bytes, zero terminates.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] source SHAPE2D offset; [BP+08] source SHAPE2D segment; Shape +00 width word; bitmap/run
; stream begins
; ASMDOC +10h; caller supplies valid memory/rows; no clipping; [BP+0A] x word; [BP+0C] y word; subtract shape
; +04/+06 hotspot.
; ASMDOC Outputs: No semantic register result; sprite1 pixels changed until zero control; Width controls row
; wrap; header height
; ASMDOC is not read and does not limit output.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX on nonzero packets; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment(+2), CS row-table pointer(+0A),row offsets; source width word and
; stream; hotspot
; ASMDOC words.
; ASMDOC Globals written: sprite1 backing pixels.
; ASMDOC Services: none.
; ASMDOC Dead: true. Reachability: No runtime symbol reference or address-taking found in tracked src/ or asm/.
; ASMDOC Shared tail: All three compatible frames save DS/SI/DI/BP and join render_bmp_as_mask_shared_exit
; (111); common return
; ASMDOC restores them.
; ASMDOC Edge cases: No clip or total pixel count. Control 80h NEG wraps to 80h and is used as 128 literal
; bytes. Runs may cross
; ASMDOC rows; each exhausted width moves to next CS row-table entry. Width zero still processes one pixel per
; row due signed
; ASMDOC DEC/JG.
_andpackfxy proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
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
    jmp     short render_bmp_as_mask_shared_exit
    even ; word-align the following entry
_andpackfxy endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33AE4.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_33AE4; proposed name _shape2d_and_rle_at
; ASMDOC Purpose: AND a signed packet stream into sprite1 at explicit coordinates. Positive control repeats next
; byte, negative
; ASMDOC control consumes literal bytes, zero terminates.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] source SHAPE2D offset; [BP+08] source SHAPE2D segment; Shape +00 width word; bitmap/run
; stream begins
; ASMDOC +10h; caller supplies valid memory/rows; no clipping; [BP+0A] x word; [BP+0C] y word.
; ASMDOC Outputs: No semantic register result; sprite1 pixels changed until zero control; Width controls row
; wrap; header height
; ASMDOC is not read and does not limit output.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX on nonzero packets; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment(+2), CS row-table pointer(+0A),row offsets; source width word and
; stream; explicit
; ASMDOC stack x/y.
; ASMDOC Globals written: sprite1 backing pixels.
; ASMDOC Services: none.
; ASMDOC Dead: true. Reachability: No runtime symbol reference or address-taking found in tracked src/ or asm/.
; ASMDOC Shared tail: All three compatible frames save DS/SI/DI/BP and join render_bmp_as_mask_shared_exit
; (111); common return
; ASMDOC restores them.
; ASMDOC Edge cases: No clip or total pixel count. Control 80h NEG wraps to 80h and is used as 128 literal
; bytes. Runs may cross
; ASMDOC rows; each exhausted width moves to next CS row-table entry. Width zero still processes one pixel per
; row due signed
; ASMDOC DEC/JG.
_andpackfxya proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short render_bmp_as_mask_shared_exit
    even ; word-align the following entry
_andpackfxya endp
; -----------------------------------------------------------------------------
; Purpose: Render a 2D bitmap shape as a mask.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shapeofs (word ptr 6);
; arg_shapeseg (word ptr 8).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI,
; DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
;                                                           
; -----------------------------------------------------------------------------
; ASMDOC Contract _shape2d_render_bmp_as_mask; proposed name _shape2d_render_bmp_as_mask
; ASMDOC Purpose: AND a signed packet stream into sprite1 at shape header origin. Positive control repeats next
; byte, negative
; ASMDOC control consumes literal bytes, zero terminates.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] source SHAPE2D offset; [BP+08] source SHAPE2D segment; Shape +00 width word; bitmap/run
; stream begins
; ASMDOC +10h; caller supplies valid memory/rows; no clipping; Shape +08/+0A supply x/y.
; ASMDOC Outputs: No semantic register result; sprite1 pixels changed until zero control; Width controls row
; wrap; header height
; ASMDOC is not read and does not limit output.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX on nonzero packets; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment(+2), CS row-table pointer(+0A),row offsets; source width word and
; stream; header
; ASMDOC x/y.
; ASMDOC Globals written: sprite1 backing pixels.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers game/race.c:854,1671,1675.
; ASMDOC Shared tail: All three compatible frames save DS/SI/DI/BP and join render_bmp_as_mask_shared_exit
; (111); common return
; ASMDOC restores them.
; ASMDOC Edge cases: No clip or total pixel count. Control 80h NEG wraps to 80h and is used as 128 literal
; bytes. Runs may cross
; ASMDOC rows; each exhausted width moves to next CS row-table entry. Width zero still processes one pixel per
; row due signed
; ASMDOC DEC/JG.
_andpackf proc far
    local_row_table_cursor = word ptr -16
    local_source_cursor = word ptr -14
    local_source_width = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shapeofs = word ptr 6
    arg_shapeseg = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shapeseg]
    mov     si, [bp+arg_shapeofs]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Shared AND decoder: height is never read. Width only selects row breaks; stream zero control is the
; sole normal
; ASMDOC terminator.
render_bmp_as_mask_shared_exit:
    cld
    lea     ax, [si+10h]
    mov     [bp+local_source_cursor], ax
    mov     ax, [si+0]
    mov     [bp+local_source_width], ax
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     [bp+local_row_table_cursor], bx
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     ax, word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     si, [bp+local_source_cursor]
    mov     dx, [bp+local_source_width]
; ASMDOC Signed control byte: 1..127 repeats next byte; 81h..FFh copies 127..1 literals; 80h copies 128 literals
; after byte NEG;
; ASMDOC 0 returns.
render_bmp_as_mask_read_loop:
    lodsb
    or      al, al
    jg      short bmp_as_mask_when_greater
    jl      short render_bmp_as_mask_when_less
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
bmp_as_mask_when_greater:
    mov     cl, al
    xor     ch, ch
    lodsb
render_bmp_as_mask_scan_loop:
    and     es:[di], al ;                                                          
    inc     di
    dec     dx
    jg      short bmp_as_mask_when_greater_2
; ASMDOC Packets can span rows. Row advance reads the next CS row-table word and restarts at saved x; no
; clipping/height test.
    add     [bp+local_row_table_cursor], 2
    mov     bx, [bp+local_row_table_cursor]
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     dx, [bp+local_source_width]
bmp_as_mask_when_greater_2:
    loop    render_bmp_as_mask_scan_loop
    jmp     short render_bmp_as_mask_read_loop
render_bmp_as_mask_when_less:
    neg     al
    mov     cl, al
    xor     ch, ch
render_bmp_as_mask_read_loop_2:
    lodsb
    and     es:[di], al ;                                                          
    inc     di
    dec     dx
    jg      short bmp_as_mask_when_greater_3
    add     [bp+local_row_table_cursor], 2
    mov     bx, [bp+local_row_table_cursor]
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     dx, [bp+local_source_width]
bmp_as_mask_when_greater_3:
    loop    render_bmp_as_mask_read_loop_2
    jmp     short render_bmp_as_mask_read_loop
_andpackf endp
S012_TEXT ends
end
