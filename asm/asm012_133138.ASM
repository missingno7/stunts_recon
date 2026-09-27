.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _byte_2FBF7:byte
extrn _byte_2FC04:byte
extrn _byte_2FC10:byte
extrn _byte_2FC12:byte
extrn _byte_2FC14:byte
extrn _word_2FB48:word
extrn _word_2FB4A:word
extrn _word_2FB4C:word
extrn _word_2FB4E:word
extrn _word_2FBD0:word
extrn _word_2FBD2:word
extrn _word_2FBD4:word
extrn _word_2FBD6:word
extrn _word_2FBD8:word
DSEG segment word public 'DATA'
DSEG ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _add_exit_handler:far
public _kb_init_interrupt
public _kb_exit_handler
public _kb_int9_handler
public _kb_int16_handler
public _kb_get_key_state
_kb_init_interrupt proc far
    push di
    in al, 21h
    mov ah, al
    or al, 3
    out 21h, al
    xor bx, bx
    mov es, bx
    mov bx, word ptr es:[24h]
    cmp bx, offset _kb_int9_handler
    je loc_20861
    mov word ptr ds:[_word_2FB48], bx
    mov bx, word ptr es:[26h]
    mov word ptr ds:[_word_2FB4A], bx
    mov word ptr es:[24h], offset _kb_int9_handler
    mov word ptr es:[26h], cs
    mov bx, word ptr es:[58h]
    mov word ptr ds:[_word_2FB4C], bx
    mov bx, word ptr es:[5Ah]
    mov word ptr ds:[_word_2FB4E], bx
    mov word ptr es:[58h], offset _kb_int16_handler
    mov word ptr es:[5Ah], cs
loc_20861:
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
    mov ax, offset _kb_exit_handler
    push ax
    call far ptr _add_exit_handler
    add sp, 4
    pop di
    retf
_kb_init_interrupt endp
_kb_exit_handler proc far
    in al, 21h
    mov ah, al
    or al, 3
    out 21h, al
    xor bx, bx
    mov es, bx
    mov bx, word ptr ds:[_word_2FB48]
    or bx, bx
    je loc_208C1
    mov word ptr es:[24h], bx
    mov bx, word ptr ds:[_word_2FB4A]
    mov word ptr es:[26h], bx
    mov bx, word ptr ds:[_word_2FB4C]
    mov word ptr es:[58h], bx
    mov bx, word ptr ds:[_word_2FB4E]
    mov word ptr es:[5Ah], bx
    mov al, byte ptr es:[417h]
    and al, 0F0h
    mov byte ptr es:[417h], al
loc_208C1:
    mov al, ah
    out 21h, al
    retf
_kb_exit_handler endp
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
    mov word ptr ds:[_word_2FBD8], bx
    mov byte ptr [bx + 446Ah], 1
    test byte ptr ds:[_byte_2FC12], 1
    jne loc_20986
    test byte ptr ds:[_byte_2FBF7], 1
    jne loc_20980
    test byte ptr ds:[_byte_2FC04], 1
    jne loc_2097A
    test byte ptr ds:[_byte_2FC10], 1
    jne loc_2097A
    test byte ptr ds:[_byte_2FC14], 1
    jne loc_2098C
    mov al, byte ptr [bx + 44C4h]
loc_2092F:
    test al, 80h
    jne loc_2096C
    xor ah, ah
loc_20935:
    mov bx, word ptr ds:[_word_2FBD0]
    cli
    mov word ptr [bx + 43E0h], ax
    add bx, 2
    cmp bx, word ptr ds:[_word_2FBD4]
    jb loc_20949
    xor bx, bx
loc_20949:
    mov word ptr ds:[_word_2FBD0], bx
    mov bx, word ptr ds:[_word_2FBD6]
    add bx, 2
    cmp bx, word ptr ds:[_word_2FBD4]
    jbe loc_20964
    mov bx, word ptr ds:[_word_2FBD4]
    mov ax, word ptr ds:[_word_2FBD0]
    mov word ptr ds:[_word_2FBD2], ax
loc_20964:
    mov word ptr ds:[_word_2FBD6], bx
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
    cmp word ptr ds:[_word_2FBD6], 0
    je loc_209BB
    mov bx, word ptr ds:[_word_2FBD2]
    mov ax, word ptr [bx + 43E0h]
    add bx, 2
    cmp bx, word ptr ds:[_word_2FBD4]
    jb loc_209DE
    xor bx, bx
loc_209DE:
    mov word ptr ds:[_word_2FBD2], bx
    mov bx, word ptr ds:[_word_2FBD6]
    sub bx, 2
    mov word ptr ds:[_word_2FBD6], bx
    jmp loc_209BD
loc_209EF:
    cmp word ptr ds:[_word_2FBD6], 0
    je loc_209BB
    sti
    mov bx, word ptr ds:[_word_2FBD2]
    mov ax, word ptr [bx + 43E0h]
    pop ds
    pop bx
    retf 2
loc_20A04:
    mov al, byte ptr ds:[_byte_2FC04]
    or al, byte ptr ds:[_byte_2FC10]
    jmp loc_209BD
_kb_int16_handler endp
_kb_get_key_state proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov al, byte ptr [bx + 446Ah]
    xor ah, ah
    pop bp
    retf
    db 0
_kb_get_key_state endp
S012_TEXT ends
end
