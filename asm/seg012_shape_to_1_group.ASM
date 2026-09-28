.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33D0C
public _sprite_shape_to_1
public _sprite_shape_to_1_alt
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33D0C.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_33D0C proc far
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
    jmp     short sprite_shape_to_1_shared_exit
    ; align 2
    db 144
_nopsub_33D0C endp
; -----------------------------------------------------------------------------
; Purpose: Copy the selected shape into sprite image 1.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_shape_to_1 proc far
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short sprite_shape_to_1_shared_exit
    ; align 2
    db 144
_sprite_shape_to_1 endp
; -----------------------------------------------------------------------------
; Purpose: Copy the selected shape into sprite image 1 through the alternate path.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_shape_to_1_alt proc far
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

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
sprite_shape_to_1_shared_exit:
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
    jnb     short sprite_shape_to_1_copy_loop_3
    jz      short sprite_shape_to_1_copy_loop_4
sprite_shape_to_1_copy_loop:
    mov     cx, [bp+var_6]
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
    mov     cx, [bp+var_6]
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
    ; align 2
    db 0
_sprite_shape_to_1_alt endp
S012_TEXT ends
end
