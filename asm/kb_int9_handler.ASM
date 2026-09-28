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
    je keyboard_irq_decode_make_code
    and al, 7Fh
    jmp keyboard_irq_map_control_state
keyboard_irq_send_eoi_restore:
    mov al, 20h
    out 20h, al
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax
    iret
keyboard_irq_decode_make_code:
    xor ah, ah
    mov bx, ax
    cmp bx, 5Ah
    jb keyboard_irq_clamp_scan_code
    xor bx, bx
keyboard_irq_clamp_scan_code:
    mov word ptr ds:[4468h], bx
    mov byte ptr [bx + 446Ah], 1
    test byte ptr ds:[44A2h], 1
    jne keyboard_irq_map_altgr_state
    test byte ptr ds:[4487h], 1
    jne keyboard_irq_map_shift_state
    test byte ptr ds:[4494h], 1
    jne keyboard_irq_normalize_extended
    test byte ptr ds:[44A0h], 1
    jne keyboard_irq_normalize_extended
    test byte ptr ds:[44A4h], 1
    jne keyboard_irq_map_shifted_key
    mov al, byte ptr [bx + 44C4h]
keyboard_irq_store_make_state:
    test al, 80h
    jne keyboard_irq_commit_queue_tail
    xor ah, ah
keyboard_irq_translate_key:
    mov bx, word ptr ds:[4460h]
    cli
    mov word ptr [bx + 43E0h], ax
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jb keyboard_irq_enqueue_key_event
    xor bx, bx
keyboard_irq_enqueue_key_event:
    mov word ptr ds:[4460h], bx
    mov bx, word ptr ds:[4466h]
    add bx, 2
    cmp bx, word ptr ds:[4464h]
    jbe keyboard_irq_advance_queue_head
    mov bx, word ptr ds:[4464h]
    mov ax, word ptr ds:[4460h]
    mov word ptr ds:[4462h], ax
keyboard_irq_advance_queue_head:
    mov word ptr ds:[4466h], bx
    sti
    jmp keyboard_irq_send_eoi_restore
keyboard_irq_commit_queue_tail:
    mov ah, al
    xor al, al
    cmp ah, 85h
    jb keyboard_irq_extended_key
    and ah, 7Fh
keyboard_irq_extended_key:
    jmp keyboard_irq_translate_key
keyboard_irq_normalize_extended:
    mov al, byte ptr [bx + 451Fh]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shift_state:
    mov al, byte ptr [bx + 45D5h]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_altgr_state:
    mov al, byte ptr [bx + 4630h]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shifted_key:
    mov al, byte ptr [bx + 457Ah]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_control_state:
    xor ah, ah
    mov bx, ax
    cmp bx, 5Ah
    jb keyboard_irq_clear_break_state
    xor bx, bx
keyboard_irq_clear_break_state:
    mov byte ptr [bx + 446Ah], 0
    jmp keyboard_irq_send_eoi_restore
_kb_int9_handler endp
_TEXT ends
end
