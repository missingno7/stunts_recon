.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_parse_key
_kb_parse_key proc far
    push bp
    mov bp, sp
    cli
    cmp byte ptr ds:[4270h], 0
    je acquire_key_parser_lock
    sti
    pop bp
    retf
acquire_key_parser_lock:
    mov byte ptr ds:[4270h], 1
    sti
    mov bx, word ptr [bp + 6]
    or bl, bl
    je select_keyboard_callback_table
    and bx, 7Fh
    mov word ptr [bp + 6], bx
    mov bl, byte ptr [bx + 416Ah]
dispatch_key_callback:
    dec bx
    jl return_without_callback
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
select_keyboard_callback_table:
    mov bl, bh
    xor bh, bh
    cmp bx, 84h
    jl dispatch_keyboard_callback
    mov bx, 84h
dispatch_keyboard_callback:
    mov bl, byte ptr [bx + 41EAh]
    jmp dispatch_key_callback
return_without_callback:
    mov ax, word ptr [bp + 6]
    mov byte ptr ds:[4270h], 0
    pop bp
    retf
_kb_parse_key endp
_TEXT ends
end
