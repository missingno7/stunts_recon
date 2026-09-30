.8086
DSEG segment word public 'DATA'
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_int9_handler
_kb_int9_handler proc far
    sti
    push ax
    push bx
    push cx
    push dx
    push ds
    mov ax, seg dseg
    mov ds, ax
    in al, 60h
    push ax
    in al, 61h
    mov ah, al
    or al, 80h
    out 61h, al
    xchg ah, al
    out 61h, al
    pop ax
    test al, 80h
    je loc_208F4
    and al, 7Fh
    jmp loc_20992
loc_208EA:
    mov al, 20h
    out 20h, al
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax
    iret
loc_208F4:
    xor ah, ah
    mov bx, ax
    cmp bx, 5Ah
    jb loc_208FF
    xor bx, bx
loc_208FF:
    mov word ptr ds:[4468h], bx
    mov byte ptr [bx + 446Ah], 1
    test byte ptr ds:[44A2h], 1
    jne loc_20986
    test byte ptr ds:[4487h], 1
    jne loc_20980
    test byte ptr ds:[4494h], 1
    jne loc_2097A
    test byte ptr ds:[44A0h], 1
    jne loc_2097A
    test byte ptr ds:[44A4h], 1
    jne loc_2098C
    mov al, byte ptr [bx + 44C4h]
loc_2092F:
    test al, 80h
    jne loc_2096C
    xor ah, ah
loc_20935:
    mov bx, word ptr ds:[4460h]
    cli
    mov word ptr [bx + 43E0h], ax
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jb loc_20949
    xor bx, bx
loc_20949:
    mov word ptr ds:[4460h], bx
    mov bx, word ptr ds:[4466h]
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jbe loc_20964
    mov bx, word ptr ds:[4464h]
    mov ax, word ptr ds:[4460h]
    mov word ptr ds:[4462h], ax
loc_20964:
    mov word ptr ds:[4466h], bx
    sti
    jmp loc_208EA
loc_2096C:
    mov ah, al
    xor al, al
    cmp ah, 85h
    jb loc_20978
    and ah, 7Fh
loc_20978:
    jmp loc_20935
loc_2097A:
    mov al, byte ptr [bx + 451Fh]
    jmp loc_2092F
loc_20980:
    mov al, byte ptr [bx + 45D5h]
    jmp loc_2092F
loc_20986:
    mov al, byte ptr [bx + 4630h]
    jmp loc_2092F
loc_2098C:
    mov al, byte ptr [bx + 457Ah]
    jmp loc_2092F
loc_20992:
    xor ah, ah
    mov bx, ax
    cmp bx, 5Ah
    jb loc_2099D
    xor bx, bx
loc_2099D:
    mov byte ptr [bx + 446Ah], 0
    jmp loc_208EA
_kb_int9_handler endp
_TEXT ends
end
