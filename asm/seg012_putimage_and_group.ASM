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
