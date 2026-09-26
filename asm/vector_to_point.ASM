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
    jle     short loc_32431
    mov     ax, [si+VECTOR.vx]
    or      ax, ax
    jl      short loc_3240F
    mul     _word_303BA
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short loc_323FE
    inc     bx
loc_323FE:
    cmp     cx, bx
    jle     short loc_32440
    div     cx
    add     ax, _word_303B6
    jo      short loc_3243C
    mov     [di], ax
    jmp     short loc_3244D
    db 144
loc_3240F:
    neg     ax
    mul     _word_303BA
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short loc_3241E
    inc     bx
loc_3241E:
    cmp     cx, bx
    jle     short loc_32448
    div     cx
    neg     ax
    add     ax, _word_303B6
    jo      short loc_3243C
    mov     [di], ax
    jmp     short loc_3244D
    db 144
loc_32431:
    mov     ax, 32768
    mov     [di], ax
    mov     [di+2], ax
    jmp     short loc_32472

    db 144
loc_3243C:
    or      ax, ax
    jge     short loc_32448
loc_32440:
    mov     ax, 7D00h
    mov     [di], ax
    jmp     short loc_3244D

    db 144
loc_32448:
    mov     ax, 8300h
    mov     [di], ax
loc_3244D:
    mov     ax, [si+VECTOR.vy]
    or      ax, ax
    jl      short loc_32476
    mul     _word_303BC
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short loc_32461
    inc     bx
loc_32461:
    cmp     cx, bx
    jle     short loc_3249A
    div     cx
    neg     ax
    add     ax, _word_303B8
    jo      short loc_32496
    mov     [di+2], ax
loc_32472:
    pop     si
    pop     di
    pop     bp
    retf
loc_32476:
    neg     ax
    mul     _word_303BC
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short loc_32485
    inc     bx
loc_32485:
    cmp     cx, bx
    jle     short loc_324A2
    div     cx
    add     ax, _word_303B8
    jo      short loc_32496
    mov     [di+2], ax
    jmp     short loc_32472
loc_32496:
    or      ax, ax
    jl      short loc_324A2
loc_3249A:
    mov     ax, -7D00h
    mov     [di+2], ax
    jmp     short loc_32472
loc_324A2:
    mov     ax, 7D00h
    mov     [di+2], ax
    jmp     short loc_32472
_vector_to_point endp
_TEXT ends
end
