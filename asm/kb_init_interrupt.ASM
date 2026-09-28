.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn add_exit_handler:far
public _kb_init_interrupt
_kb_init_interrupt proc far
    push di
    in al, 21h
    mov ah, al
    or al, 3
    out 21h, al
    xor bx, bx
    mov es, bx
    mov bx, word ptr es:[24h]
    cmp bx, 1EA6h
    je keyboard_init_finish_setup
    mov word ptr ds:[43D8h], bx
    mov bx, word ptr es:[26h]
    mov word ptr ds:[43DAh], bx
    mov word ptr es:[24h], 1EA6h
    mov word ptr es:[26h], cs
    mov bx, word ptr es:[58h]
    mov word ptr ds:[43DCh], bx
    mov bx, word ptr es:[5Ah]
    mov word ptr ds:[43DEh], bx
    mov word ptr es:[58h], 1F85h
    mov word ptr es:[5Ah], cs
keyboard_init_finish_setup:
    mov al, ah
    out 21h, al
    mov ax, ds
    mov es, ax
    mov di, 446Ah
    mov cx, 5Ah
    xor ax, ax
    cld
    rep stosb
    push cs
    mov ax, 1E63h
    push ax
    call far ptr add_exit_handler
    add sp, 4
    pop di
    retf
_kb_init_interrupt endp
_TEXT ends
end
