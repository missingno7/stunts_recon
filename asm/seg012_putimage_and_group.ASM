; ASMDOC Module: seg012_putimage_and_group. Proposed content name: sprite_and_blitters. Replaces segment-address
; ASMDOC placeholder with the shared clipped AND bitmap behavior; no split or merge.
; ASMDOC Public routines: _sprite_putimage_and_alt2, _sprite_putimage_and.
; ASMDOC Static contract audit: byte/word widths and offsets describe original 8086 paths; inherited generated comments
; ASMDOC below are retained verbatim and may disagree. BP offsets are after PUSH BP/MOV BP,SP. Destination bitmap/table
; ASMDOC memory is caller-selected; no owned routine calls an application callback.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite_putimage_and_alt2
public _sprite_putimage_and
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the second alternate AND path.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_and_alt2: Clipped AND bitmap blit with hotspot-adjusted destination.
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
; ASMDOC dead: False.
; ASMDOC notes: Enters putimage_and_shared_exit; shared tail restores DS/SI/DI/BP and RETF. Positive source dimensions
; ASMDOC are an expected precondition (height zero can still execute one row). Clips using signed 16-bit branches and
; ASMDOC wrapped coordinates. Source and destination offsets wrap at 16 bits. Width-one, zero-source-skip path
; ASMDOC increments row advance after each row: later destination row strides grow by one. No callbacks, DOS or BIOS
; ASMDOC calls..
_sprite_putimage_and_alt2 proc far
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
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short putimage_and_shared_exit
    ; align 2
    db 144
_sprite_putimage_and_alt2 endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the AND compositing operation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_and: Clipped AND bitmap blit at explicit destination.
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
; ASMDOC notes: Positive source dimensions are an expected precondition (height zero can still execute one row). Clips
; ASMDOC using signed 16-bit branches and wrapped coordinates. Source and destination offsets wrap at 16 bits.
; ASMDOC Width-one, zero-source-skip path increments row advance after each row: later destination row strides grow by
; ASMDOC one. No callbacks, DOS or BIOS calls..
_sprite_putimage_and proc far
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
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
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short putimage_and_shared_exit
    ; align 2
    db 144
; ASMDOC Dormant unlabeled prologue: AND at stored shape position. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
; ASMDOC Shared signed clipping and AND rows, despite the exit label name; wrappers share the RETF epilogue.
putimage_and_shared_exit:
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+16]
    jl      short putimage_and_when_less
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short and_when_less_or_equal
    sub     cx, ax
    jg      short and_when_less_or_equal
putimage_and_draw_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
putimage_and_when_less:
    mov     bx, word ptr cs:[_sprite1+16]
    add     ax, cx
    sub     ax, bx
    jle     short putimage_and_draw_loop
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short and_when_less_or_equal
    sub     cx, ax
    jle     short putimage_and_draw_loop
and_when_less_or_equal:
    mov     [bp+var_C], dx
    mov     [bp+var_A], cx
    mov     [bp+var_4], bx
    mov     cx, [si]
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+12]
    jl      short putimage_and_when_less_2
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+14]
    jl      short putimage_and_shared_path
    sub     cx, ax
    jle     short putimage_and_draw_loop
    mov     dx, ax
    jmp     short putimage_and_shared_path
    db 144
putimage_and_when_less_2:
    mov     bx, word ptr cs:[_sprite1+12]
    add     ax, cx
    sub     ax, bx
    jle     short putimage_and_draw_loop
    mov     si, cx
    sub     si, ax
    add     [bp+var_C], si
    mov     si, word ptr cs:[_sprite1+14]
    sub     si, bx
    jle     short putimage_and_draw_loop
    cmp     ax, si
    jl      short putimage_and_when_less_3
    mov     ax, si
putimage_and_when_less_3:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
putimage_and_shared_path:
    or      cx, cx
    jle     short and_when_less_or_equal_2
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+var_2], bx
    cld
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     si, [bp+var_C]
    mov     dx, [bp+var_A]
    mov     bx, word ptr cs:[_sprite1+20]
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short putimage_and_when_zero
putimage_and_read_loop:
    mov     cx, [bp+var_6]
putimage_and_read_loop_2:
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    putimage_and_read_loop_2
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short putimage_and_read_loop
and_when_less_or_equal_2:
    jmp     putimage_and_draw_loop
putimage_and_when_zero:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short putimage_and_read_loop_5
    jz      short putimage_and_when_zero_2
    inc     bx
putimage_and_read_loop_3:
    mov     cx, [bp+var_E]
putimage_and_read_loop_4:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putimage_and_read_loop_4
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short putimage_and_read_loop_3
    jmp     putimage_and_draw_loop
putimage_and_read_loop_5:
    mov     cx, [bp+var_E]
putimage_and_read_loop_6:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putimage_and_read_loop_6
    add     di, bx
    dec     dx
    jg      short putimage_and_read_loop_5
    jmp     putimage_and_draw_loop
putimage_and_when_zero_2:
    inc     bx
putimage_and_read_loop_7:
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    inc     bx
    dec     dx
    jg      short putimage_and_read_loop_7
    jmp     putimage_and_draw_loop
    ; align 2
    db 0
_sprite_putimage_and endp
S012_TEXT ends
end
