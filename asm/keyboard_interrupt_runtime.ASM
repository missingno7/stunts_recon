.8086
name asm012_133138
_DATA segment word public 'DATA'
old_kb_intr_ofs dw 0
old_kb_intr_seg dw 0
old_kb_intr_bios_ofs dw 0
old_kb_intr_bios_seg dw 0
kb_intr_data_array db 128 dup (0)
kb_intr_data dw 0
kb_intr_data2 dw 0
kb_intr_data3 dw 2
kb_intr_data4 dw 0
kblastinput dw 0
_kbinput db 90 dup (0)
keymap1 db 0, 01Bh, 031h, 032h, 033h, 034h, 035h, 036h, 037h, 038h, 039h, 030h, 02Dh, 03Dh, 8, 9
    db 071h, 077h, 065h, 072h, 074h, 079h, 075h, 069h, 06Fh, 070h, 05Bh, 05Dh, 00Dh, 0, 061h, 073h
    db 064h, 066h, 067h, 068h, 06Ah, 06Bh, 06Ch, 03Bh, 027h, 060h, 0, 05Ch, 07Ah, 078h, 063h, 076h
    db 062h, 06Eh, 06Dh, 02Ch, 02Eh, 02Fh, 0, 02Ah, 0, 020h, 0, 0BBh, 0BCh, 0BDh, 0BEh, 0BFh
    db 0C0h, 0C1h, 0C2h, 0C3h, 0C4h, 0, 0, 0C7h, 0C8h, 0C9h, 02Dh, 0CBh, 0CCh, 0CDh, 02Bh, 0CFh
    db 0D0h, 0D1h, 0D2h, 0D3h, 0, 0, 0, 0, 0, 0, 0
keymap2 db 0, 01Bh, 021h, 040h, 023h, 024h, 025h, 05Eh, 026h, 02Ah, 028h, 029h, 05Fh, 02Bh, 8, 08Fh
    db 051h, 057h, 045h, 052h, 054h, 059h, 055h, 049h, 04Fh, 050h, 07Bh, 07Dh, 00Dh, 0, 041h, 053h
    db 044h, 046h, 047h, 048h, 04Ah, 04Bh, 04Ch, 03Ah, 022h, 07Eh, 0, 07Ch, 05Ah, 058h, 043h, 056h
    db 042h, 04Eh, 04Dh, 03Ch, 03Eh, 03Fh, 0, 0, 0, 020h, 0, 0D4h, 0D5h, 0D6h, 0D7h, 0D8h
    db 0D9h, 0DAh, 0DBh, 0DCh, 0DDh, 0, 0, 0C7h, 0C8h, 0C9h, 02Dh, 0CBh, 0CCh, 0CDh, 02Bh, 0CFh
    db 0D0h, 0D1h, 0D2h, 0D3h, 0, 0, 0, 0, 0, 0, 0
keymap3 db 0, 01Bh, 031h, 032h, 033h, 034h, 035h, 036h, 037h, 038h, 039h, 030h, 02Dh, 03Dh, 8, 08Fh
    db 051h, 057h, 045h, 052h, 054h, 059h, 055h, 049h, 04Fh, 050h, 05Bh, 05Dh, 00Dh, 0, 041h, 053h
    db 044h, 046h, 047h, 048h, 04Ah, 04Bh, 04Ch, 03Bh, 027h, 060h, 0, 05Ch, 05Ah, 058h, 043h, 056h
    db 042h, 04Eh, 04Dh, 02Ch, 02Eh, 02Fh, 0, 0, 0, 020h, 0, 0D4h, 0D5h, 0D6h, 0D7h, 0D8h
    db 0D9h, 0DAh, 0DBh, 0DCh, 0DDh, 0, 0, 0C7h, 0C8h, 0C9h, 02Dh, 0CBh, 0CCh, 0CDh, 02Bh, 0CFh
    db 0D0h, 0D1h, 0D2h, 0D3h, 0, 0, 0, 0, 0, 0, 0
keymap4 db 0, 01Bh, 021h, 0, 023h, 024h, 025h, 01Eh, 026h, 02Ah, 028h, 029h, 01Fh, 02Bh, 07Fh, 9
    db 011h, 017h, 5, 012h, 014h, 019h, 015h, 9, 00Fh, 010h, 01Bh, 01Dh, 00Dh, 0, 1, 013h
    db 4, 6, 7, 8, 00Ah, 00Bh, 00Ch, 03Bh, 02Ch, 060h, 0, 01Ch, 01Ah, 018h, 3, 016h
    db 2, 00Eh, 0B2h, 03Ch, 03Eh, 03Fh, 0, 0, 0, 020h, 0, 0DEh, 0DFh, 0E0h, 0E1h, 0E2h
    db 0E3h, 0E4h, 0E5h, 0E6h, 0E7h, 0, 0, 0C7h, 0C8h, 0C9h, 02Dh, 0CBh, 0CCh, 0CDh, 02Bh, 0CFh
    db 0D0h, 0D1h, 0D2h, 0D3h, 0, 0, 0, 0, 0, 0, 0
keymap5 db 0, 01Bh, 021h, 040h, 023h, 024h, 025h, 05Eh, 026h, 02Ah, 028h, 029h, 05Fh, 02Bh, 8, 08Fh
    db 090h, 091h, 092h, 093h, 094h, 095h, 096h, 097h, 098h, 099h, 07Bh, 07Dh, 00Dh, 0, 09Eh, 09Fh
    db 0A0h, 0A1h, 0A2h, 0A3h, 0A4h, 0A5h, 0A6h, 03Ah, 022h, 07Eh, 0, 07Ch, 0ACh, 0ADh, 0AEh, 0AFh
    db 0B0h, 0B1h, 0B2h, 03Ch, 03Eh, 03Fh, 0, 0, 0, 020h, 0, 0F8h, 0F9h, 0FAh, 0FBh, 0FCh
    db 0FDh, 0FEh, 0FFh, 080h, 081h, 0, 0, 0C7h, 0C8h, 0C9h, 02Dh, 0CBh, 0CCh, 0CDh, 02Bh, 0CFh
    db 0D0h, 0D1h, 0D2h, 0D3h, 0, 0, 0, 0, 0, 0, 0, 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
DSEG segment word public 'DATA'
DSEG ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _add_exit_handler:far
public _kbinput
public _kb_init_interrupt
public _keyboard_exit_handler
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
    je keyboard_init_finish_setup
    mov word ptr ds:[old_kb_intr_ofs], bx
    mov bx, word ptr es:[26h]
    mov word ptr ds:[old_kb_intr_seg], bx
    mov word ptr es:[24h], offset _kb_int9_handler
    mov word ptr es:[26h], cs
    mov bx, word ptr es:[58h]
    mov word ptr ds:[old_kb_intr_bios_ofs], bx
    mov bx, word ptr es:[5Ah]
    mov word ptr ds:[old_kb_intr_bios_seg], bx
    mov word ptr es:[58h], offset _kb_int16_handler
    mov word ptr es:[5Ah], cs
keyboard_init_finish_setup:
    mov al, ah
    out 21h, al
    mov ax, ds
    mov es, ax
    mov di, offset DGROUP:_kbinput
    mov cx, 5Ah
    xor ax, ax
    cld
    rep stosb
    push cs
    mov ax, offset _keyboard_exit_handler
    push ax
    call far ptr _add_exit_handler
    add sp, 4
    pop di
    retf
_kb_init_interrupt endp
_keyboard_exit_handler proc far
    in al, 21h
    mov ah, al
    or al, 3
    out 21h, al
    xor bx, bx
    mov es, bx
    mov bx, word ptr ds:[old_kb_intr_ofs]
    or bx, bx
    je keyboard_exit_restore_flags
    mov word ptr es:[24h], bx
    mov bx, word ptr ds:[old_kb_intr_seg]
    mov word ptr es:[26h], bx
    mov bx, word ptr ds:[old_kb_intr_bios_ofs]
    mov word ptr es:[58h], bx
    mov bx, word ptr ds:[old_kb_intr_bios_seg]
    mov word ptr es:[5Ah], bx
    mov al, byte ptr es:[417h]
    and al, 0F0h
    mov byte ptr es:[417h], al
keyboard_exit_restore_flags:
    mov al, ah
    out 21h, al
    retf
_keyboard_exit_handler endp
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
    mov word ptr ds:[kblastinput], bx
    mov byte ptr _kbinput[bx], 1
    test byte ptr ds:[_kbinput+56], 1
    jne keyboard_irq_map_altgr_state
    test byte ptr ds:[_kbinput+29], 1
    jne keyboard_irq_map_shift_state
    test byte ptr ds:[_kbinput+42], 1
    jne keyboard_irq_normalize_extended
    test byte ptr ds:[_kbinput+54], 1
    jne keyboard_irq_normalize_extended
    test byte ptr ds:[_kbinput+58], 1
    jne keyboard_irq_map_shifted_key
    mov al, byte ptr keymap1[bx]
keyboard_irq_store_make_state:
    test al, 80h
    jne keyboard_irq_commit_queue_tail
    xor ah, ah
keyboard_irq_translate_key:
    mov bx, word ptr ds:[kb_intr_data]
    cli
    mov word ptr kb_intr_data_array[bx], ax
    add bx, 2
    cmp bx, word ptr ds:[kb_intr_data3]
    jb keyboard_irq_enqueue_key_event
    xor bx, bx
keyboard_irq_enqueue_key_event:
    mov word ptr ds:[kb_intr_data], bx
    mov bx, word ptr ds:[kb_intr_data4]
    add bx, 2
    cmp bx, word ptr ds:[kb_intr_data3]
    jbe keyboard_irq_advance_queue_head
    mov bx, word ptr ds:[kb_intr_data3]
    mov ax, word ptr ds:[kb_intr_data]
    mov word ptr ds:[kb_intr_data2], ax
keyboard_irq_advance_queue_head:
    mov word ptr ds:[kb_intr_data4], bx
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
    mov al, byte ptr keymap2[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shift_state:
    mov al, byte ptr keymap4[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_altgr_state:
    mov al, byte ptr keymap5[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shifted_key:
    mov al, byte ptr keymap3[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_control_state:
    xor ah, ah
    mov bx, ax
    cmp bx, 5Ah
    jb keyboard_irq_clear_break_state
    xor bx, bx
keyboard_irq_clear_break_state:
    mov byte ptr _kbinput[bx], 0
    jmp keyboard_irq_send_eoi_restore
_kb_int9_handler endp
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
    cmp word ptr ds:[kb_intr_data4], 0
    je keyboard_bios_unknown_function
    mov bx, word ptr ds:[kb_intr_data2]
    mov ax, word ptr kb_intr_data_array[bx]
    add bx, 2
    cmp bx, word ptr ds:[kb_intr_data3]
    jb kb_bios_advance_read_index
    xor bx, bx
kb_bios_advance_read_index:
    mov word ptr ds:[kb_intr_data2], bx
    mov bx, word ptr ds:[kb_intr_data4]
    sub bx, 2
    mov word ptr ds:[kb_intr_data4], bx
    jmp keyboard_bios_return_common
keyboard_bios_peek_key:
    cmp word ptr ds:[kb_intr_data4], 0
    je keyboard_bios_unknown_function
    sti
    mov bx, word ptr ds:[kb_intr_data2]
    mov ax, word ptr kb_intr_data_array[bx]
    pop ds
    pop bx
    retf 2
kb_bios_read_modifier_flags:
    mov al, byte ptr ds:[_kbinput+42]
    or al, byte ptr ds:[_kbinput+54]
    jmp keyboard_bios_return_common
_kb_int16_handler endp
_kb_get_key_state proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov al, byte ptr _kbinput[bx]
    xor ah, ah
    pop bp
    retf
    db 0
_kb_get_key_state endp
S012_TEXT ends
end
