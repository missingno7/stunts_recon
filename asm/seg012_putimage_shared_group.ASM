.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33B98
public _sprite_putimage_and_alt
public _sprputimage
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33B98.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_33B98 proc far
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
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short sprputimage_shared_exit
    ; align 2
    db 144
_nopsub_33B98 endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the alternate AND compositing path.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_putimage_and_alt proc far
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
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short sprputimage_shared_exit
    ; align 2
    db 144
_sprite_putimage_and_alt endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprputimage.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprputimage proc far
    var_E = word ptr -14
    var_bitmap = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    shapey = word ptr -4
    shapex = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+shapex], ax
    mov     ax, [si+0Ah]
    mov     [bp+shapey], ax
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
    db 144
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
    mov     es, word ptr word ptr cs:[_sprite1+2]
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
_sprputimage endp
S012_TEXT ends
end
