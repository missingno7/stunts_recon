.8086
.8086
extrn _copy_paras_reverse:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_decomp_rle, _file_decomp_rle_single, _file_decomp_rle_seq
_file_decomp_rle proc far
    var_lenlo = word ptr -26
    var_lenhi = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_esclen = word ptr -18
     s = byte ptr 0
     r = byte ptr 2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 12
    arg_dstseg = word ptr 14

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    sub     sp, 116h
    cld
    mov     ax, ds
    mov     es, ax
    mov     si, [bp+arg_srcoff]
    mov     ds, [bp+arg_srcseg]
    mov     ax, [si+1]
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenhi], ax
    add     si, 4           ; Skip header + unused byte
    mov     cx, 8
    lea     di, [bp+var_16] ; Read 8 words
    rep movsw
    mov     si, [bp+arg_srcoff]; Reset source pos
    add     si, 9
    mov     dx, [bp+var_esclen]

    and     dx, 7Fh         ; Escape codes length mask

    add     si, dx
    mov     [bp+arg_srcoff], si; Skip escape codes
    cmp     byte ptr [bp+var_esclen], 80h ; ''; Skip seq pass flag
    ja      short skip_seq_pass
    call near ptr _file_decomp_rle_seq
    mov     [bp+var_16], ax
    mov     [bp+var_14], dx
    mov     si, ax          ; paras = len / 16
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short no_remainder
    inc     ax
no_remainder:
    push    ax              ; paras
    mov     bx, [bp+arg_dstseg]
    sub     bx, ax
    add     bx, [bp+arg_dstoff]
    mov     [bp+arg_srcseg], bx
    push    bx
    push    [bp+arg_dstoff]
    call far ptr _copy_paras_reverse
    add     sp, 6
    xor     si, si
    mov     [bp+arg_srcoff], si
skip_seq_pass:
    call near ptr _file_decomp_rle_single
    mov     ax, [bp+var_lenlo]; Return length
    mov     dx, [bp+var_lenhi]
    add     sp, 116h
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf

_file_decomp_rle endp
_file_decomp_rle_single proc near
    mov     cx, 80h ; ''
    lea     di, [bp-11Ch]
    mov     ax, ss
    mov     es, ax
    mov     ds, ax
    xor     ax, ax
    rep stosw
    mov     cl, [bp-12h]

    and     cx, 7Fh

    lea     si, [bp-11h]
    lea     di, [bp-11Ch]
    xor     ax, ax
    xor     bh, bh
loc_30C1A:
    inc     ax
    mov     bl, [si]
    mov     [bx+di], al
    inc     si
    loop    loc_30C1A
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr [bp+0Ch]
    xor     di, di
    mov     dx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
loc_30C36:
    cmp     si, 8000h
    jbe     short loc_30C3F
    jmp     short loc_30CB3
    db 144
loc_30C3F:
    cmp     di, 8000h
    jbe     short loc_30C48
    jmp     short loc_30CC1
    ; align 2
    db 144
loc_30C48:
    lodsb
    xor     ah, ah
    lea     bx, [bp-11Ch]
    add     bx, ax
    mov     cl, ss:[bx]
    test    cl, 0FFh
    jnz     short loc_30C68
    stosb
    dec     dx
    jnz     short loc_30C36
    cmp     word ptr [bp-1Ch], 0
    jz      short loc_30C8A
    dec     word ptr [bp-1Ch]
    jmp     short loc_30C36
loc_30C68:
    xor     ch, ch
    cmp     cl, 1
    jz      short loc_30C9A
    cmp     cl, 3
    jz      short loc_30CA5
    dec     cx
    lodsb
    rep stosb
    sub     dx, 2
loc_30C7B:
    mov     ax, [bp-1Ch]
    sbb     ax, 0
    mov     [bp-1Ch], ax
    or      ax, dx
    jz      short loc_30C8A
    jmp     short loc_30C36
loc_30C8A:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
loc_30C9A:
    lodsb
    mov     cl, al
    lodsb
    rep stosb
    sub     dx, 3
    jmp     short loc_30C7B
loc_30CA5:
    lodsb
    mov     cl, al
    lodsb
    mov     ch, al
    lodsb
    rep stosb
    sub     dx, 4
    jmp     short loc_30C7B
loc_30CB3:
    sub     si, 8000h
    mov     ax, ds
    add     ax, 800h
    mov     ds, ax
    jmp     loc_30C3F
loc_30CC1:
    sub     di, 8000h
    mov     ax, es
    add     ax, 800h
    mov     es, ax
    jmp     loc_30C48

_file_decomp_rle_single endp
_file_decomp_rle_seq proc near
    cmp     byte ptr [bp-12h], 1; _file_decomp_rle::var_esclen
    jnz     short has_codes
    retn
has_codes:
    mov     ds, word ptr [bp+8]; _file_decomp_rle::arg_srcseg
    mov     si, [bp+6]      ; _file_decomp_rle::arg_srcoff
    mov     es, word ptr [bp+0Ch]; _file_decomp_rle::var_dstoff
    xor     di, di
    mov     bx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
    mov     ah, [bp-10h]
loc_30CED:
    cmp     si, 8000h
    ja      short loc_30D0D
loc_30CF3:
    cmp     di, 8000h
    ja      short loc_30D1B
loc_30CF9:
    lodsb
    cmp     al, ah
    jz      short loc_30D39
    stosb
    dec     bx
    jnz     short loc_30CED
    cmp     word ptr [bp-1Ch], 0
    jz      short loc_30D29
    dec     word ptr [bp-1Ch]
    jmp     short loc_30CED
loc_30D0D:
    sub     si, 8000h
    mov     dx, ds
    add     dx, 800h
    mov     ds, dx
    jmp     short loc_30CF3
loc_30D1B:
    sub     di, 8000h
    mov     dx, es
    add     dx, 800h
    mov     es, dx
    jmp     short loc_30CF9
loc_30D29:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
loc_30D39:
    mov     dx, di
loc_30D3B:
    lodsb
    cmp     al, ah
    jz      short loc_30D49
    stosb
    dec     bx
    jnz     short loc_30D3B
    dec     word ptr [bp-1Ch]
    jmp     short loc_30D3B
loc_30D49:
    lodsb
    push    si
    push    ds
    mov     cx, es
    mov     ds, cx
    mov     si, dx
    mov     cx, di
    sub     cx, dx
    mov     dl, al
    dec     dl
loc_30D5A:
    push    cx
    push    si
    push    ds
    rep movsb
    pop     ds
    pop     si
    pop     cx
    dec     dl
    jnz     short loc_30D5A
    pop     ds
    pop     si
    sub     bx, 3
    sbb     word ptr [bp-1Ch], 0
    mov     dx, [bp-1Ch]
    or      dx, bx
    jz      short loc_30D29
    jmp     loc_30CED

_file_decomp_rle_seq endp
_TEXT ends
end
