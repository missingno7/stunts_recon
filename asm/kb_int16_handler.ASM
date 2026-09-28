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
    je keyboard_bios_read_waiting_key
    cmp ah, 1
    je keyboard_bios_peek_key
    cmp ah, 2
    je kb_bios_read_modifier_flags
keyboard_bios_unknown_function:
    xor ax, ax
keyboard_bios_return_common:
    sti
    pop ds
    pop bx
    retf 2
keyboard_bios_read_waiting_key:
    cli
    cmp word ptr ds:[4466h], 0
    je keyboard_bios_unknown_function
    mov bx, word ptr ds:[4462h]
    mov ax, word ptr [bx + 43E0h]
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jb kb_bios_advance_read_index
    xor bx, bx
kb_bios_advance_read_index:
    mov word ptr ds:[4462h], bx
    mov bx, word ptr ds:[4466h]
    sub bx, 2
    mov word ptr ds:[4466h], bx
    jmp keyboard_bios_return_common
keyboard_bios_peek_key:
    cmp word ptr ds:[4466h], 0
    je keyboard_bios_unknown_function
    sti
    mov bx, word ptr ds:[4462h]
    mov ax, word ptr [bx + 43E0h]
    pop ds
    pop bx
    retf 2
kb_bios_read_modifier_flags:
    mov al, byte ptr ds:[4494h]
    or al, byte ptr ds:[44A0h]
    jmp keyboard_bios_return_common
_kb_int16_handler endp
_TEXT ends
end
