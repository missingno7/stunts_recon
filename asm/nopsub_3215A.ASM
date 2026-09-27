.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_3215A
_nopsub_3215A proc far
    push di
    mov ax, 0B000h
    mov es, ax
    xor di, di
    mov cx, 07D0h
    xor ax, ax
    cld
    rep stosw
    pop di
    retf
_nopsub_3215A endp
S012_TEXT ends
end
