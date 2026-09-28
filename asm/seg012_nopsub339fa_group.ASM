.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_339FA
public _putpixel_iconMask
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 339FA.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_339FA proc far
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
    sub     sp, 0Ah
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
    jmp     short putpixel_iconmask_shared_exit
    ; align 2
    db 144
_nopsub_339FA endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by putpixel iconMask.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_putpixel_iconMask proc far
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short putpixel_iconmask_shared_exit
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
putpixel_iconmask_shared_exit:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    cld
    mov     dx, [si+2]
    mov     bx, word ptr cs:[_sprite1+20]
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+var_6], ax
    jnb     short putpixel_iconmask_read_loop_3
    jz      short putpixel_iconmask_when_zero
    inc     bx
putpixel_iconmask_read_loop:
    mov     cx, [bp+var_6]
putpixel_iconmask_read_loop_2:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putpixel_iconmask_read_loop_2
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop
putpixel_iconmask_draw_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
putpixel_iconmask_read_loop_3:
    mov     cx, [bp+var_6]
putpixel_iconmask_read_loop_4:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putpixel_iconmask_read_loop_4
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop_3
    jmp     short putpixel_iconmask_draw_loop
putpixel_iconmask_when_zero:
    inc     bx
putpixel_iconmask_read_loop_5:
    lodsb
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop_5
    jmp     short putpixel_iconmask_draw_loop
    ; align 2
    db 0
_putpixel_iconMask endp
S012_TEXT ends
end
