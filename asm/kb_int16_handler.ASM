.8086
DSEG segment word public 'DATA'
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_int16_handler
_kb_int16_handler proc far
    push bx
    push ds
    mov bx, seg dseg
    mov ds, bx
    cmp ah, 0
    je loc_209C3
    cmp ah, 1
    je loc_209EF
    cmp ah, 2
    je loc_20A04
loc_209BB:
    xor ax, ax
loc_209BD:
    sti
    pop ds
    pop bx
    retf 2
loc_209C3:
    cli
    cmp word ptr ds:[4466h], 0
    je loc_209BB
    mov bx, word ptr ds:[4462h]
    mov ax, word ptr [bx + 43E0h]
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jb loc_209DE
    xor bx, bx
loc_209DE:
    mov word ptr ds:[4462h], bx
    mov bx, word ptr ds:[4466h]
    sub bx, 2
    mov word ptr ds:[4466h], bx
    jmp loc_209BD
loc_209EF:
    cmp word ptr ds:[4466h], 0
    je loc_209BB
    sti
    mov bx, word ptr ds:[4462h]
    mov ax, word ptr [bx + 43E0h]
    pop ds
    pop bx
    retf 2
loc_20A04:
    mov al, byte ptr ds:[4494h]
    or al, byte ptr ds:[44A0h]
    jmp loc_209BD
_kb_int16_handler endp
_TEXT ends
end
