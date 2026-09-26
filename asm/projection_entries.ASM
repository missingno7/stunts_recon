.8086
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
extrn _cos_fast:far
extrn _polarAngle:far
extrn _sin_fast:far
extrn _word_303AE:word
extrn _word_303B0:word
extrn _word_303B2:word
extrn _word_303B4:word
extrn _word_303B6:word
extrn _word_303B8:word
extrn _word_303BA:word
extrn _word_303BC:word
extrn _word_303BE:word
extrn _word_303C0:word
_TEXT segment word public 'CODE'
public _nopsub_322DF
public _set_projection
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
    call _cos_fast
    add     sp, 2
    mul     _word_303AE
    mov     si, ax
    mov     di, dx
    push    _word_303BE
    call _sin_fast
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
    call _cos_fast
    add     sp, 2
    mul     _word_303B0
    mov     si, ax
    mov     di, dx
    push    _word_303C0
    call _sin_fast
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
    call _polarAngle
    add     sp, 4
    mov     _word_303C0, ax
    pop     di
    pop     si
    pop     bp
    retf
_set_projection endp
_TEXT ends
end
