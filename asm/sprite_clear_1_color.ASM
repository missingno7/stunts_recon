.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _sprite_clear_1_color
_sprite_clear_1_color proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = byte ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, word ptr cs:[5F22h]
    mov     es, ax
    mov     dx, cs:[5F32h]
    mov     bx, cs:[5F30h]
    sub     dx, bx
    jle     short loc_3330D
    shl     bx, 1
    add     bx, cs:[5F2Ah]
    mov     di, cs:[bx]
    add     di, cs:[5F2Ch]
    mov     bx, cs:[5F2Eh]
    sub     bx, cs:[5F2Ch]
    jle     short loc_3330D
    mov     si, cs:[5F34h]
    sub     si, bx
    mov     al, [bp+arg_0]
    mov     ah, al
    shr     bx, 1
    jb      short loc_33311
loc_33304:
    mov     cx, bx
    rep stosw
    add     di, si
    dec     dx
    jg      short loc_33304
loc_3330D:
    pop     di
    pop     si
    pop     bp
    retf
loc_33311:
    jz      short loc_33324
    jl      short loc_3330D
    inc     si
loc_33316:
    mov     cx, bx
    rep stosw
    mov     es:[di], al
    add     di, si
    dec     dx
    jg      short loc_33316
    jmp     short loc_3330D
loc_33324:
    inc     si
loc_33325:
    mov     es:[di], al
    add     di, si
    dec     dx
    jg      short loc_33325
    jmp     short loc_3330D
    ; align 2
    db 0
_sprite_clear_1_color endp
_TEXT ends
end
