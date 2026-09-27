.8086
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite_1_unk2
public _sprite_1_unk
public _sprite_1_unk3
_sprite_1_unk2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, word ptr cs:[_sprite1+12]
    mov     ax, dx
    sub     ax, [bp+arg_2]
    jle     short loc_33591
    mov     [bp+arg_2], dx
    sub     [bp+arg_6], ax
    jle     short loc_335CF
loc_33591:
    mov     ax, [bp+arg_2]
    add     ax, [bp+arg_6]
    sub     ax, word ptr cs:[_sprite1+14]
    jle     short loc_335A3
    sub     [bp+arg_6], ax
    jle     short loc_335CF
loc_335A3:
    mov     ax, word ptr cs:[_sprite1+16]
    sub     ax, [bp+arg_4]
    jle     short loc_335B8
    sub     [bp+arg_8], ax
    jle     short loc_335CF
    mov     ax, word ptr cs:[_sprite1+16]
    mov     [bp+arg_4], ax
loc_335B8:
    mov     ax, [bp+arg_4]
    add     ax, [bp+arg_8]
    mov     bx, word ptr cs:[_sprite1+18]
    sub     ax, bx
    jle     short loc_335D7
    sub     [bp+arg_8], ax
    jle     short loc_335CF
    jmp     short loc_335D7
    db 144
loc_335CF:
    jmp     short loc_33622
    ; align 2
    db 144
_sprite_1_unk2 endp
_sprite_1_unk proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
loc_335D7:
    cld
    cmp     [bp+arg_4], 0
    jle     short loc_335CF
    cmp     [bp+arg_6], 0
    jle     short loc_335CF
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     al, [bp+arg_8]
    mov     ah, al
    mov     dx, [bp+arg_0]
    mov     si, [bp+arg_6]
    mov     bx, [bp+arg_2]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+arg_0]
    mov     dx, word ptr cs:[_sprite1+20]
    sub     dx, [bp+arg_4]
    mov     cx, [bp+arg_4]
    sar     cx, 1
    mov     [bp+arg_4], cx
    jb      short loc_33626
    jle     short loc_33622
loc_33618:
    mov     cx, [bp+arg_4]
    rep stosw
    add     di, dx
    dec     si
    jg      short loc_33618
loc_33622:
    pop     di
    pop     si
    pop     bp
    retf
loc_33626:
    jz      short loc_3363A
    jl      short loc_33622
    inc     dx
loc_3362B:
    mov     cx, [bp+arg_4]
    rep stosw
    mov     es:[di], al
    add     di, dx
    dec     si
    jg      short loc_3362B
    jmp     short loc_33622
loc_3363A:
    inc     dx
loc_3363B:
    mov     es:[di], al
    add     di, dx
    dec     si
    jg      short loc_3363B
    jmp     short loc_33622
    ; align 2
    db 0
byte_33646     db 11
    db 5
    db 8
    db 2
    db 10
    db 4
    db 7
    db 1
    db 9
    db 3
    db 6
    db 0
byte_33652     db 1
    db 3
    db 0
    db 2
byte_33656     db 3
    db 1
    db 4
    db 2
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     ax, [bp+0Ch]
    mov     [bp-2], ax
    mov     ax, [bp+0Eh]
    jmp     short loc_33697
    ; align 2
    db 144
_sprite_1_unk endp
_sprite_1_unk3 proc far
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
loc_33697:
    shl     ax, 1
    add     ax, word ptr cs:[_sprite1+10]
    mov     [bp+var_4], ax
    add     ax, [si+2]
    add     ax, [si+2]
    mov     [bp+var_6], ax
    mov     ax, [si]
    mov     [bp+var_8], ax
    mov     bx, 0Ch
    mul     bx
    mov     [bp+var_A], ax
    add     si, 10h
    mov     [bp+var_C], si
    mov     [bp+var_E], 0Bh
loc_336C2:
    mov     di, [bp+var_E]
    mov     cl, cs:byte_33646[di]
    xor     ch, ch
    mov     ax, [bp+var_8]
    mul     cx
    mov     [bp+var_10], ax
    shl     cx, 1
    add     cx, [bp+var_4]
    mov     [bp+var_12], cx
    mov     si, [bp+var_C]
    add     si, [bp+var_10]
    mov     bx, [bp+arg_4]
    mov     [bp+var_16], bx
loc_336E8:
    mov     di, [bp+var_12]
    cmp     di, [bp+var_6]
    jnb     short loc_33733
    mov     di, cs:[di]
    add     di, [bp+var_2]
    mov     cx, [bp+var_8]
    mov     [bp+var_14], si
    xor     ah, ah
    mov     bx, [bp+var_16]
loc_33701:
    and     bx, 3
    mov     al, cs:byte_33652[bx]
    sub     cx, ax
    jle     short loc_33724
    add     si, ax
    add     di, ax
    mov     al, [si]
    mov     es:[di], al
    mov     al, cs:byte_33656[bx]
    add     si, ax
    add     di, ax
    sub     cx, ax
    inc     bx
    jmp     short loc_33701
loc_33724:
    inc     [bp+var_16]
    add     [bp+var_12], 18h
    mov     si, [bp+var_14]
    add     si, [bp+var_A]
    jmp     short loc_336E8
loc_33733:
    inc     [bp+arg_4]
    dec     [bp+var_E]
    jge     short loc_336C2
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_1_unk3 endp
S012_TEXT ends
end
