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
    jz      short loc_32914
    mov     cx, [si+0]
    or      cx, cx
    jz      short loc_32914
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_32914:
    mov     [di+0], dx
    mov     ax, [bx+6]
    or      ax, ax
    jz      short loc_32930
    mov     cx, [si+2]
    or      cx, cx
    jz      short loc_32930
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+0], dx
loc_32930:
    mov     ax, [bx+12]
    or      ax, ax
    jz      short loc_3294A
    mov     cx, [si+4]
    or      cx, cx
    jz      short loc_3294A
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+0], dx
loc_3294A:
    xor     dx, dx
    mov     ax, [bx+2]
    or      ax, ax
    jz      short loc_32963
    mov     cx, [si+0]
    or      cx, cx
    jz      short loc_32963
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_32963:
    mov     [di+2], dx
    mov     ax, [bx+8]
    or      ax, ax
    jz      short loc_32981
    mov     cx, [si+2]
    or      cx, cx
    jz      short loc_32981
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
loc_32981:
    mov     ax, [bx+14]
    or      ax, ax
    jz      short loc_3299C
    mov     cx, [si+4]
    or      cx, cx
    jz      short loc_3299C
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
loc_3299C:
    xor     dx, dx
    mov     ax, [bx+4]
    or      ax, ax
    jz      short loc_329B5
    mov     cx, [si+0]
    or      cx, cx
    jz      short loc_329B5
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_329B5:
    mov     [di+4], dx
    mov     ax, [bx+10]
    or      ax, ax
    jz      short loc_329D3
    mov     cx, [si+2]
    or      cx, cx
    jz      short loc_329D3
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+4], dx
loc_329D3:
    mov     ax, [bx+16]
    or      ax, ax
    jz      short loc_329EE
    mov     cx, [si+4]
    or      cx, cx
    jz      short loc_329EE
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+4], dx
loc_329EE:
    pop     di
    pop     si
    pop     bp
    retf
_mat_mul_vector endp
_TEXT ends
end
