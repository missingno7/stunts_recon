.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_33DBE
public _shape2d_op_unk5
public _shape2d_op_unk
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
_shape2d_op_unk5 proc far
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
_shape2d_op_unk5 endp
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
shape_op_unk_shared_exit:
    cld
    lea     ax, [si+10h]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
shape_op_unk_load_source_value:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[5F2Ah]
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr word ptr cs:[5F22h]
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_6]
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
    stosb
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
_TEXT ends
end
