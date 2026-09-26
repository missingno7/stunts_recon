.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_exit_handler
_kb_exit_handler proc far
    in al, 21h
    mov ah, al
    or al, 3
    out 21h, al
    xor bx, bx
    mov es, bx
    mov bx, word ptr ds:[43D8h]
    or bx, bx
    je loc_208C1
    mov word ptr es:[24h], bx
    mov bx, word ptr ds:[43DAh]
    mov word ptr es:[26h], bx
    mov bx, word ptr ds:[43DCh]
    mov word ptr es:[58h], bx
    mov bx, word ptr ds:[43DEh]
    mov word ptr es:[5Ah], bx
    mov al, byte ptr es:[417h]
    and al, 0F0h
    mov byte ptr es:[417h], al
loc_208C1:
    mov al, ah
    out 21h, al
    retf
_kb_exit_handler endp
_TEXT ends
end
