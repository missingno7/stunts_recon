.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _mat_mul_vector
_mat_mul_vector proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_inpVector = word ptr 6
    arg_matrix = word ptr 8
    arg_outVector = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     bx, [bp+arg_matrix]
    mov     si, [bp+arg_inpVector]
    mov     di, [bp+arg_outVector]
    xor     dx, dx
    mov     ax, [bx+0]
    or      ax, ax
    jz      short vector_transform_when_zero
    mov     cx, [si+0]
    or      cx, cx
    jz      short vector_transform_when_zero
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
vector_transform_when_zero:
    mov     [di+0], dx
    mov     ax, [bx+6]
    or      ax, ax
    jz      short vector_transform_when_zero_2
    mov     cx, [si+2]
    or      cx, cx
    jz      short vector_transform_when_zero_2
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+0], dx
vector_transform_when_zero_2:
    mov     ax, [bx+12]
    or      ax, ax
    jz      short vector_transform_when_zero_3
    mov     cx, [si+4]
    or      cx, cx
    jz      short vector_transform_when_zero_3
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+0], dx
vector_transform_when_zero_3:
    xor     dx, dx
    mov     ax, [bx+2]
    or      ax, ax
    jz      short vector_transform_when_zero_4
    mov     cx, [si+0]
    or      cx, cx
    jz      short vector_transform_when_zero_4
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
vector_transform_when_zero_4:
    mov     [di+2], dx
    mov     ax, [bx+8]
    or      ax, ax
    jz      short vector_transform_when_zero_5
    mov     cx, [si+2]
    or      cx, cx
    jz      short vector_transform_when_zero_5
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
vector_transform_when_zero_5:
    mov     ax, [bx+14]
    or      ax, ax
    jz      short vector_transform_when_zero_6
    mov     cx, [si+4]
    or      cx, cx
    jz      short vector_transform_when_zero_6
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
vector_transform_when_zero_6:
    xor     dx, dx
    mov     ax, [bx+4]
    or      ax, ax
    jz      short vector_transform_when_zero_7
    mov     cx, [si+0]
    or      cx, cx
    jz      short vector_transform_when_zero_7
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
vector_transform_when_zero_7:
    mov     [di+4], dx
    mov     ax, [bx+10]
    or      ax, ax
    jz      short vector_transform_when_zero_8
    mov     cx, [si+2]
    or      cx, cx
    jz      short vector_transform_when_zero_8
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+4], dx
vector_transform_when_zero_8:
    mov     ax, [bx+16]
    or      ax, ax
    jz      short transform_return_if_zero
    mov     cx, [si+4]
    or      cx, cx
    jz      short transform_return_if_zero
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+4], dx
transform_return_if_zero:
    pop     di
    pop     si
    pop     bp
    retf
_mat_mul_vector endp
_TEXT ends
end
