.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _mat_multiply
_mat_multiply proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_rightMatrix = word ptr 6
    arg_leftMatrix = word ptr 8
    arg_outputMatrix = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, [bp+arg_rightMatrix]
    mov     bx, [bp+arg_leftMatrix]
    mov     di, [bp+arg_outputMatrix]
    mov     cx, 9
matrix_multiply_scan_loop:
    xor     dx, dx
    mov     ax, word ptr [si+0]
    or      ax, ax
    jz      short matrix_multiply_when_zero
    cmp     word ptr [bx+0], 0
    jz      short matrix_multiply_when_zero
    imul    word ptr [bx+0]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_multiply_when_zero:
    mov     word ptr [di+0], dx
    mov     ax, word ptr [si+2]
    or      ax, ax
    jz      short matrix_multiply_when_zero_2
    cmp     word ptr [bx+6], 0
    jz      short matrix_multiply_when_zero_2
    imul    word ptr [bx+6]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     word ptr [di+0], dx
matrix_multiply_when_zero_2:
    mov     ax, word ptr [si+4]
    or      ax, ax
    jz      short matrix_multiply_when_zero_3
    cmp     word ptr [bx+12], 0
    jz      short matrix_multiply_when_zero_3
    imul    word ptr [bx+12]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     word ptr [di+0], dx
matrix_multiply_when_zero_3:
    add     di, 2
    cmp     cx, 7
    jz      short matrix_multiply_return_if_zero
    cmp     cx, 4
    jz      short matrix_multiply_return_if_zero
    add     bx, 2
    loop    matrix_multiply_scan_loop
    pop     di
    pop     si
    pop     bp
    retf
matrix_multiply_return_if_zero:
    sub     bx, 4
    add     si, 6
    loop    matrix_multiply_scan_loop
    pop     di
    pop     si
    pop     bp
    retf
_mat_multiply endp
_TEXT ends
end
