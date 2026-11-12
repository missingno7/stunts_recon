; ASMDOC Module: seg012_putimage_shared_group. Proposed content name: sprite_opaque_blitters. The public group copies
; ASMDOC bitmap bytes using MOVS; old segment and AND wording obscure the operation; no split or merge.
; ASMDOC Public routines: _nopsub_33B98, _sprite_putimage_and_alt, _sprputimage.
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
public _shape2d_blit_bitmap_hotspot
public _movcfxya
public _movcf
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33B98.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_33B98: Clipped opaque bitmap copy with hotspot-adjusted destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y; header +4/+6 signed hotspot words subtracted
; ASMDOC from x/y with 16-bit wrap.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: Actual work and return are in _sprputimage shared tail. Positive source dimensions are an expected
; ASMDOC precondition (height zero can still execute one row). Clips using signed 16-bit branches and wrapped
; ASMDOC coordinates. Source and destination offsets wrap at 16 bits. Width-one, zero-source-skip path increments row
; ASMDOC advance after each row: later destination row strides grow by one. No callbacks, DOS or BIOS calls..
; ASMDOC Proposed routine name: _shape2d_blit_bitmap_hotspot.
_shape2d_blit_bitmap_hotspot proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
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
    jmp     short sprputimage_shared_exit
    even ; word-align the following entry
_shape2d_blit_bitmap_hotspot endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the alternate AND compositing path.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_and_alt: Clipped opaque bitmap copy at explicit destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: No AND operation: shared tail uses REP MOVSB/MOVSW or MOVSB. Positive source dimensions are an
; expected
; ASMDOC precondition (height zero can still execute one row). Clips using signed 16-bit branches and wrapped
; ASMDOC coordinates. Source and destination offsets wrap at 16 bits. Width-one, zero-source-skip path increments row
; ASMDOC advance after each row: later destination row strides grow by one. No callbacks, DOS or BIOS calls..
_movcfxya proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short sprputimage_shared_exit
    even ; word-align the following entry
_movcfxya endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprputimage.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprputimage: Clipped opaque bitmap copy at shape-stored position.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; header x +8, y +0Ah.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Positive source dimensions are an expected precondition (height zero can still execute one row). Clips
; ASMDOC using signed 16-bit branches and wrapped coordinates. Source and destination offsets wrap at 16 bits.
; ASMDOC Width-one, zero-source-skip path increments row advance after each row: later destination row strides grow by
; ASMDOC one. No callbacks, DOS or BIOS calls..
_movcf proc far
    var_E = word ptr -14
    var_bitmap = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    shapey = word ptr -4
    shapex = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+shapex], ax
    mov     ax, [si+0Ah]
    mov     [bp+shapey], ax
; ASMDOC Shared signed clipping and opaque MOVS rows; the AND-named wrapper performs no AND.
sprputimage_shared_exit:
    mov     bx, [bp+shapey]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+16]
    jl      short sprputimage_when_less
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short sprputimage_when_less_or_equal
    sub     cx, ax
    jg      short sprputimage_when_less_or_equal
sprputimage_draw_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
sprputimage_when_less:
    mov     bx, word ptr cs:[_sprite1+16]
    add     ax, cx
    sub     ax, bx
    jle     short sprputimage_draw_loop
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si+0]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short sprputimage_when_less_or_equal
    sub     cx, ax
    jle     short sprputimage_draw_loop
sprputimage_when_less_or_equal:
    mov     [bp+var_bitmap], dx
    mov     [bp+var_A], cx
    mov     [bp+shapey], bx
    mov     cx, [si+0]
    xor     dx, dx
    mov     bx, [bp+shapex]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+12]
    jl      short sprputimage_when_less_2
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+14]
    jl      short sprputimage_shared_path
    sub     cx, ax
    jle     short sprputimage_draw_loop
    mov     dx, ax
    jmp     short sprputimage_shared_path
    nop ; retained code pad at an even offset; EVEN would emit no byte
sprputimage_when_less_2:
    mov     bx, word ptr cs:[_sprite1+12]
    add     ax, cx
    sub     ax, bx
    jle     short sprputimage_draw_loop
    mov     si, cx
    sub     si, ax
    add     [bp+var_bitmap], si
    mov     si, word ptr cs:[_sprite1+14]
    sub     si, bx
    jle     short sprputimage_draw_loop
    cmp     ax, si
    jl      short sprputimage_when_less_3
    mov     ax, si
sprputimage_when_less_3:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
sprputimage_shared_path:
    or      cx, cx
    jle     short when_less_or_equal_2
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+shapex], bx
    cld
    mov     es, word ptr cs:[_sprite1+2]
    mov     bx, [bp+shapey]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+shapex]
    mov     si, [bp+var_bitmap]
    mov     dx, [bp+var_A]
    mov     bx, word ptr cs:[_sprite1+20]
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short sprputimage_when_zero
sprputimage_copy_loop:
    mov     cx, [bp+var_6]
    rep movsb
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short sprputimage_copy_loop
when_less_or_equal_2:
    jmp     sprputimage_draw_loop
sprputimage_when_zero:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short sprputimage_copy_loop_3
    jz      short sprputimage_copy_loop_4
sprputimage_copy_loop_2:
    mov     cx, [bp+var_E]
    rep movsw
    movsb
    add     di, bx
    dec     dx
    jg      short sprputimage_copy_loop_2
    jmp     sprputimage_draw_loop
sprputimage_copy_loop_3:
    mov     cx, [bp+var_E]
    rep movsw
    add     di, bx
    dec     dx
    jg      short sprputimage_copy_loop_3
    jmp     sprputimage_draw_loop
sprputimage_copy_loop_4:
    movsb
    add     di, bx
    inc     bx
    dec     dx
    jg      short sprputimage_copy_loop_4
    jmp     sprputimage_draw_loop
_movcf endp
S012_TEXT ends
end
