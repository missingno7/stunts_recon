.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33AC0
public _nopsub_33AE4
public _shape2d_render_bmp_as_mask
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33AC0.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_33AC0 proc far
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
    jmp     short render_bmp_as_mask_shared_exit
    ; align 2
    db 144
_nopsub_33AC0 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33AE4.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_33AE4 proc far
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
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short render_bmp_as_mask_shared_exit
    ; align 2
    db 144
_nopsub_33AE4 endp
; -----------------------------------------------------------------------------
; Purpose: Render a 2D bitmap shape as a mask.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shapeofs (word ptr 6); arg_shapeseg (word ptr 8).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_shape2d_render_bmp_as_mask proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
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
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
render_bmp_as_mask_shared_exit:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si+0]
    mov     [bp+var_6], ax
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
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    dec     dx
    jg      short bmp_as_mask_when_greater_2
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
bmp_as_mask_when_greater_2:
    loop    render_bmp_as_mask_scan_loop
    jmp     short render_bmp_as_mask_read_loop
render_bmp_as_mask_when_less:
    neg     al
    mov     cl, al
    xor     ch, ch
render_bmp_as_mask_read_loop_2:
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    dec     dx
    jg      short bmp_as_mask_when_greater_3
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
bmp_as_mask_when_greater_3:
    loop    render_bmp_as_mask_read_loop_2
    jmp     short render_bmp_as_mask_read_loop
_shape2d_render_bmp_as_mask endp
S012_TEXT ends
end
