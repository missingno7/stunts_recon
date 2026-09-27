.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT,ds:DGROUP
extrn _fontdefseg:word
public _font_set_unk
_font_set_unk proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+arg_2]
    xor     ah, ah
    mov     es:2, ax
    mov     ax, [bp+arg_0]
    xor     ah, ah
    mov     es:0, ax
    pop     bp
    retf
    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+6]
    mov     es:8, ax
    mov     ax, [bp+8]
    mov     es:0Ah, ax
    pop     bp
    retf
    push    bp
    mov     bp, sp
    push    ds
    push    di
    push    si
    xor     si, si
    mov     di, [bp+6]
    mov     ax, ds
    mov     es, ax
    mov     ds, ds:[_fontdefseg]
    cld
    mov     word ptr es:[di+16h], ds
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     ds
    pop     bp
    retf
    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+6]
    mov     ax, [si+16h]
    mov     ds:[_fontdefseg], ax
    mov     es, ax
    xor     di, di
    cld
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     bp
    retf
_font_set_unk endp
S012_TEXT ends
end
