.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_parse_key
_kb_parse_key proc far
    push bp
    mov bp, sp
    cli
    cmp byte ptr ds:[4270h], 0
    je loc_20412
    sti
    pop bp
    retf
loc_20412:
    mov byte ptr ds:[4270h], 1
    sti
    mov bx, word ptr [bp + 6]
    or bl, bl
    je loc_20441
    and bx, 7Fh
    mov word ptr [bp + 6], bx
    mov bl, byte ptr [bx + 416Ah]
loc_20429:
    dec bx
    jl loc_20454
    shl bx, 1
    push di
    push si
    shl bx, 1
    call dword ptr [bx + 4272h]
    pop si
    pop di
    xor ax, ax
    mov byte ptr ds:[4270h], 0
    pop bp
    retf
loc_20441:
    mov bl, bh
    xor bh, bh
    cmp bx, 84h
    jl loc_2044E
    mov bx, 84h
loc_2044E:
    mov bl, byte ptr [bx + 41EAh]
    jmp loc_20429
loc_20454:
    mov ax, word ptr [bp + 6]
    mov byte ptr ds:[4270h], 0
    pop bp
    retf
_kb_parse_key endp
_TEXT ends
end
