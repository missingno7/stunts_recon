.8086
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite_clear_1_color
_sprite_clear_1_color proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = byte ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     dx, cs:[_sprite1+18]
    mov     bx, cs:[_sprite1+16]
    sub     dx, bx
    jle     short loc_3330D
    shl     bx, 1
    add     bx, cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, cs:[_sprite1+12]
    mov     bx, cs:[_sprite1+14]
    sub     bx, cs:[_sprite1+12]
    jle     short loc_3330D
    mov     si, cs:[_sprite1+20]
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
S012_TEXT ends
end
