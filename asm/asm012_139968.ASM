.8086
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
S012_TEXT segment word public 'CODE'
S012_TEXT ends
DGROUP group _DATA, CONST, _BSS
assume ds:DGROUP
_DATA segment word public 'DATA'
public _word_303AE, _word_303B0, _word_303B2, _word_303B4, _word_303B6, _word_303B8, _word_303BA, _word_303BC, _word_303BE, _word_303C0, _data_303C2
_word_303AE     dw 160
_word_303B0     dw 100
_word_303B2     dw 0
_word_303B4     dw 0
_word_303B6     dw 160
_word_303B8     dw 100
_word_303BA     dw 0
_word_303BC     dw 0
_word_303BE     dw 0
_word_303C0     dw 0
_data_303C2     db 87
    db 105
    db 110
    db 100
    db 111
    db 119
    db 32
    db 82
    db 101
    db 108
    db 101
    db 97
    db 115
    db 101
    db 100
    db 32
    db 79
    db 117
    db 116
    db 32
    db 111
    db 102
    db 32
    db 79
    db 114
    db 100
    db 101
    db 114
    db 13
    db 10
    db 0
    db 0
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
assume cs:S012_TEXT, ds:DGROUP
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
extrn _mmgr_release:far
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_322C0
public _nopsub_322DF
public _set_projection
public _vector_to_point
public _sprite_free_window
_nopsub_322C0 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     _word_303B2, ax
    add     ax, _word_303AE
    mov     _word_303B6, ax
    mov     ax, [bp+arg_2]
    mov     _word_303B4, ax
    add     ax, _word_303B0
    mov     _word_303B8, ax
    pop     bp
    retf
_nopsub_322C0 endp
_nopsub_322DF proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_0]
    mov     _word_303BE, ax
    mov     ax, [bp+arg_2]
    mov     _word_303C0, ax
    jmp     short loc_32334
    db 144
_nopsub_322DF endp
_set_projection proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_0]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
loc_32306:
    shl     ax, 1
    rcl     dx, 1
    loop    loc_32306
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _word_303BE, ax
    mov     ax, [bp+arg_2]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
loc_32324:
    shl     ax, 1
    rcl     dx, 1
    loop    loc_32324
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _word_303C0, ax
loc_32334:
    mov     ax, [bp+arg_4]
    shr     ax, 1
    mov     _word_303AE, ax
    add     ax, _word_303B2
    mov     _word_303B6, ax
    mov     ax, [bp+arg_6]
    shr     ax, 1
    mov     _word_303B0, ax
    add     ax, _word_303B4
    mov     _word_303B8, ax
    push    _word_303BE
    call _cosfast
    add     sp, 2
    mul     _word_303AE
    mov     si, ax
    mov     di, dx
    push    _word_303BE
    call _sinfast
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _word_303BA, ax
    mov     bx, _word_303C0
    or      bx, bx
    jz      short loc_323B4
    push    _word_303C0
    call _cosfast
    add     sp, 2
    mul     _word_303B0
    mov     si, ax
    mov     di, dx
    push    _word_303C0
    call _sinfast
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _word_303BC, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_323B4:
    mov     bx, ax
    shr     bx, 1
    shr     bx, 1
    shr     bx, 1
    sub     ax, bx
    shr     bx, 1
    sub     ax, bx
    mov     _word_303BC, ax
    push    _word_303B0
    push    ax
    call _polang
    add     sp, 4
    mov     _word_303C0, ax
    pop     di
    pop     si
    pop     bp
    retf
_set_projection endp
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
_sprite_free_window proc far
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     es, word ptr [bp+8]
    mov     di, word ptr [bp+6]
    mov     ds, word ptr es:[di+2]
    mov     si, word ptr es:[di]
    mov     ax, word ptr [si+2]
    add     ax, 0Fh
    shl     ax, 1
    mov     dx, ax
    add     dx, di
    mov     bx, word ptr cs:[62C4h]
    cmp     bx, dx
    jnz     short loc_324EC
    sub     bx, ax
    mov     word ptr cs:[62C4h], bx
    push    ds
    push    si
    mov     ax, ss
    mov     ds, ax
    call    far ptr _mmgr_release
    add     sp, 4
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
loc_324EC:
    mov     ax, ss
    mov     ds, ax
    lea     ax, ds:[_data_303C2]
    push    ax
    call    far ptr _fatal_error
_sprite_free_window endp
S012_TEXT ends
end
