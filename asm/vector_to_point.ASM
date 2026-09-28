.8086
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
_TEXT segment word public 'CODE'
_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:_TEXT, ds:DGROUP
extrn _word_303BC:word
extrn _word_303B6:word
extrn _word_303B8:word
extrn _word_303BA:word
_TEXT segment word public 'CODE'
public _vector_to_point
_vector_to_point proc far
    var_8000 = byte ptr -32768
     s = byte ptr 0
     r = byte ptr 2
    arg_vec = word ptr 6
    arg_point2dout = word ptr 8

    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+arg_vec]
    mov     di, [bp+arg_point2dout]
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jle     short to_point_when_less_or_equal
    mov     ax, [si+VECTOR.vx]
    or      ax, ax
    jl      short vector_to_point_when_less
    mul     _word_303BA
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short to_point_when_greater_or_equal
    inc     bx
to_point_when_greater_or_equal:
    cmp     cx, bx
    jle     short to_point_when_less_or_equal_2
    div     cx
    add     ax, _word_303B6
    jo      short vector_to_point_when_overflow
    mov     [di], ax
    jmp     short vector_to_point_shared_exit
    db 144
vector_to_point_when_less:
    neg     ax
    mul     _word_303BA
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short point_when_greater_or_equal_2
    inc     bx
point_when_greater_or_equal_2:
    cmp     cx, bx
    jle     short to_point_when_less_or_equal_3
    div     cx
    neg     ax
    add     ax, _word_303B6
    jo      short vector_to_point_when_overflow
    mov     [di], ax
    jmp     short vector_to_point_shared_exit
    db 144
to_point_when_less_or_equal:
    mov     ax, 32768
    mov     [di], ax
    mov     [di+2], ax
    jmp     short vector_to_point_vector_loop

    db 144
vector_to_point_when_overflow:
    or      ax, ax
    jge     short to_point_when_less_or_equal_3
to_point_when_less_or_equal_2:
    mov     ax, 7D00h
    mov     [di], ax
    jmp     short vector_to_point_shared_exit

    db 144
to_point_when_less_or_equal_3:
    mov     ax, 8300h
    mov     [di], ax
vector_to_point_shared_exit:
    mov     ax, [si+VECTOR.vy]
    or      ax, ax
    jl      short vector_to_point_when_less_2
    mul     _word_303BC
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short point_when_greater_or_equal_3
    inc     bx
point_when_greater_or_equal_3:
    cmp     cx, bx
    jle     short to_point_when_less_or_equal_4
    div     cx
    neg     ax
    add     ax, _word_303B8
    jo      short to_point_when_overflow_2
    mov     [di+2], ax
vector_to_point_vector_loop:
    pop     si
    pop     di
    pop     bp
    retf
vector_to_point_when_less_2:
    neg     ax
    mul     _word_303BC
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short point_when_greater_or_equal_4
    inc     bx
point_when_greater_or_equal_4:
    cmp     cx, bx
    jle     short to_point_when_less_or_equal_5
    div     cx
    add     ax, _word_303B8
    jo      short to_point_when_overflow_2
    mov     [di+2], ax
    jmp     short vector_to_point_vector_loop
to_point_when_overflow_2:
    or      ax, ax
    jl      short to_point_when_less_or_equal_5
to_point_when_less_or_equal_4:
    mov     ax, -7D00h
    mov     [di+2], ax
    jmp     short vector_to_point_vector_loop
to_point_when_less_or_equal_5:
    mov     ax, 7D00h
    mov     [di+2], ax
    jmp     short vector_to_point_vector_loop
_vector_to_point endp
_TEXT ends
end
