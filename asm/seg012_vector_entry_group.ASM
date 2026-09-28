.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_33006
public _vector_op_unk
_nopsub_33006 proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     ax, ds:4E92h
    jmp     short vector_op_unk_when_condition
    ; align 2
    db 144
_nopsub_33006 endp
_vector_op_unk proc far
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
    sub     sp, 4
    push    si
    push    di
    mov     ax, [bp+arg_6]
vector_op_unk_when_condition:
    mov     si, [bp+arg_0]
    mov     bx, [bp+arg_2]
    mov     di, [bp+arg_4]
    mov     word ptr [di+4], ax
    sub     ax, word ptr [bx+4]
    mov     [bp+var_4], ax
    mov     ax, word ptr [si+4]
    sub     ax, word ptr [bx+4]
    or      ax, ax
    jge     short op_unk_when_greater_or_equal
    shr     [bp+var_4], 1
    shr     ax, 1
op_unk_when_greater_or_equal:
    mov     [bp+var_2], ax
    mov     ax, word ptr [si+0]
    sub     ax, word ptr [bx+0]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+0]
    mov     word ptr [di+0], ax
    mov     ax, word ptr [si+2]
    sub     ax, word ptr [bx+2]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+2]
    mov     word ptr [di+2], ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_vector_op_unk endp
_TEXT ends
end
