; INPUTDOC Module keyboard_interrupt_runtime (owner keyboard_interrupt_runtime); comment-only static contract review.
; INPUTDOC Audited INPUTDOC contracts qualify the older retained generic comments; no code or declarations changed.
; INPUTDOC Routine names below follow rename-plan.json; definitions retain their original spelling.
; INPUTDOC dead=true means statically unreferenced; no proof against arbitrary external/numeric callers or execution.
; INPUTDOC Public data: _kbinput
; INPUTDOC Routine _kb_init_interrupt -> _kb_init_interrupt: Install IRQ1 and INT16 vectors, clear 90 held-key bytes
; INPUTDOC and register exit cleanup.
; INPUTDOC Routine _keyboard_exit_handler -> _keyboard_exit_handler: Restore saved keyboard vectors and clear
; INPUTDOC held-modifier BDA low bits.
; INPUTDOC Routine _kb_int9_handler -> _kb_int9_handler: Acknowledge keyboard IRQ, update held state and overwrite the
; INPUTDOC translated key queue.
; INPUTDOC Routine _kb_int16_handler -> _kb_int16_handler: Provide nonblocking dequeue, peek and held-shift services
; INPUTDOC with live result flags.
; INPUTDOC Routine _kb_get_key_state -> _kb_get_key_state: Read and zero-extend one byte from the held-key table
; INPUTDOC without bounds checking.
.8086
INCLUDE include\platform_hw.inc
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
; -----------------------------------------------------------------------------
; Purpose: Install keyboard IRQ and BIOS keyboard handlers and register their exit cleanup.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DI, SP, CS, DS, ES; balanced explicit saves/restores observed: DI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: old_kb_intr_bios_ofs, old_kb_intr_bios_seg, old_kb_intr_ofs, old_kb_intr_seg.
; Global addresses taken: _kbinput.
; /* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
; /* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_init_interrupt (original _kb_init_interrupt).
; INPUTDOC Purpose: Install IRQ1 and INT16 vectors, clear 90 held-key bytes and register exit cleanup.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: No specified result; ES=DS and DF=0 on normal return; AX/BX/CX/DX depend on add_exit_handler slot
; INPUTDOC search.
; INPUTDOC Preserves: DI explicitly saved; SI,BP,DS,SS preserved by normal add_exit_handler. SP balanced; overflow
; INPUTDOC fatal_error is outside normal return contract.
; INPUTDOC Clobbers: Direct AX,BX,CX,ES,FLAGS,DF; normal add_exit_handler also DX; fatal_error overflow path contract
; INPUTDOC unknown.
; INPUTDOC Globals read: IVT 0000:0024..0027 and 0058..005B
; INPUTDOC Globals written: old_kb_intr_ofs/seg; old_kb_intr_bios_ofs/seg; IVT INT09/INT16; _kbinput[90]; transitive
; INPUTDOC exit-handler registry
; INPUTDOC Services: IN/OUT PIC mask 21h (temporarily masks IRQ0/1); add_exit_handler(FAR keyboard_exit_handler)
; INPUTDOC Reachability: Live: src/obj_seg031.c:154; installs two public interrupt entries and saves a public exit
; INPUTDOC callback.
_kb_init_interrupt proc far
    push di
    in al, R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ah, al
    or al, 3
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    xor bx, bx
    mov es, bx
    mov bx, word ptr es:[24h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; INPUTDOC Installation detection compares only INT09 offset, not its segment; common setup still runs.
    cmp bx, offset _kb_int9_handler
    je keyboard_init_finish_setup
    mov word ptr ds:[old_kb_intr_ofs], bx
    mov bx, word ptr es:[26h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr ds:[old_kb_intr_seg], bx
    mov word ptr es:[24h], offset _kb_int9_handler ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr es:[26h], cs ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, word ptr es:[58h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr ds:[old_kb_intr_bios_ofs], bx
    mov bx, word ptr es:[5Ah] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr ds:[old_kb_intr_bios_seg], bx
    mov word ptr es:[58h], offset _kb_int16_handler ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr es:[5Ah], cs ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
keyboard_init_finish_setup:
    mov al, ah
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ax, ds
    mov es, ax
; INPUTDOC Only held keys are cleared here; ring indices/count/capacity are not reset.
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
; -----------------------------------------------------------------------------
; Purpose: Restore the prior keyboard interrupt vectors and keyboard controller state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches BX, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: old_kb_intr_bios_ofs, old_kb_intr_bios_seg, old_kb_intr_ofs, old_kb_intr_seg.
; Globals written: none identified.
; /* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
; /* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _keyboard_exit_handler (original _keyboard_exit_handler).
; INPUTDOC Purpose: Restore saved keyboard vectors and clear held-modifier BDA low bits.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: No specified result; ES=0; restores PIC mask read on entry.
; INPUTDOC Preserves: Untouched by this body: BP, CX, DI, DS, DX, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,ES,FLAGS
; INPUTDOC Globals read: old_kb_intr_ofs/seg; old_kb_intr_bios_ofs/seg; BDA 0000:0417
; INPUTDOC Globals written: IVT INT09/INT16 if old_kb_intr_ofs !=0; BDA 0000:0417 := old & F0h
; INPUTDOC Services: IN/OUT PIC mask 21h
; INPUTDOC Reachability: Live: registered in kb_init_interrupt:105-108 and called in src/obj_seg000.c:664.
_keyboard_exit_handler proc far
    in al, R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ah, al
    or al, 3
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    xor bx, bx
    mov es, bx
    mov bx, word ptr ds:[old_kb_intr_ofs]
    or bx, bx
    je keyboard_exit_restore_flags
    mov word ptr es:[24h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, word ptr ds:[old_kb_intr_seg]
    mov word ptr es:[26h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, word ptr ds:[old_kb_intr_bios_ofs]
    mov word ptr es:[58h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, word ptr ds:[old_kb_intr_bios_seg]
    mov word ptr es:[5Ah], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; INPUTDOC AND F0h preserves lock-toggle high nibble and clears low held-modifier nibble.
    mov al, byte ptr es:[417h]
    and al, 0F0h
    mov byte ptr es:[417h], al
keyboard_exit_restore_flags:
    mov al, ah
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    retf
_keyboard_exit_handler endp
; -----------------------------------------------------------------------------
; Purpose: Handle keyboard scan-code IRQs and enqueue translated key events.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, DS; balanced explicit saves/restores observed: AX, BX, CX, DS, DX; path-specific inputs and outputs follow the listed operands.
; Globals read: _kbinput, kb_intr_data, kb_intr_data3, kb_intr_data4, keymap1, keymap2, keymap3, keymap4, keymap5.
; Globals written: _kbinput, kb_intr_data, kb_intr_data2, kb_intr_data4, kb_intr_data_array, kblastinput.
; /* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; /* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
; /* PLATFORM(input_kb): returns from a keyboard hardware interrupt. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_int9_handler (original _kb_int9_handler).
; INPUTDOC Purpose: Acknowledge keyboard IRQ, update held state and overwrite the translated key queue.
; INPUTDOC Convention: Hardware interrupt FAR entry; IRET consumes IP,CS,FLAGS; no C arguments.
; INPUTDOC Inputs: Hardware INT09 stack at entry SP+0 IP, +2 CS, +4 FLAGS; scan byte from port60h. Incoming DS
; INPUTDOC arbitrary; handler loads dseg.
; INPUTDOC Outputs: IRET to interrupted CS:IP with original FLAGS restored; PIC EOI emitted.
; INPUTDOC Preserves: AX,BX,CX,DX,DS explicitly saved; SI,DI,BP,ES,SS untouched; original FLAGS restored by IRET.
; INPUTDOC Clobbers: No register or saved-FLAGS clobber on IRET; temporary IF enabling permits nested interrupts.
; INPUTDOC Globals read: _kbinput modifier indices 56,29,42,54,58; keymap1..keymap5; kb_intr_data; kb_intr_data3;
; INPUTDOC kb_intr_data4
; INPUTDOC Globals written: _kbinput[clamped scan]; kblastinput; kb_intr_data_array; kb_intr_data; kb_intr_data2 on
; INPUTDOC overflow; kb_intr_data4
; INPUTDOC Services: IN 60h/61h; OUT 61h acknowledge pulse; OUT PIC20h EOI; CLI/STI; IRET
; INPUTDOC Reachability: Live: kb_init_interrupt:87-88 writes this address to IVT INT09.
_kb_int9_handler proc far
; INPUTDOC Nested interrupts enabled before register saves; this does not chain the old INT09 handler.
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    push ax
    push bx
    push cx
    push dx
    push ds
    mov ax, seg dseg
    mov ds, ax
    in al, R5HW_KEYBOARD_DATA_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    push ax
    in al, R5HW_KEYBOARD_CONTROL_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ah, al
    or al, 80h
    out R5HW_KEYBOARD_CONTROL_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    xchg ah, al
    out R5HW_KEYBOARD_CONTROL_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    pop ax
    test al, 80h
    je keyboard_irq_decode_make_code
    and al, 7Fh
    jmp keyboard_irq_map_control_state
keyboard_irq_send_eoi_restore:
    mov al, 20h
    out R5HW_PIC_MASTER_COMMAND_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    pop ds
    pop dx
    pop cx
    pop bx
    pop ax
    iret ;/* PLATFORM(input_kb): returns from a keyboard hardware interrupt. */
keyboard_irq_decode_make_code:
    xor ah, ah
    mov bx, ax
; INPUTDOC Out-of-range make/break scans are mapped to state index zero; no E0-prefix state machine.
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
; INPUTDOC Caps selection tests held scan 3Ah, not a toggled BDA CapsLock bit.
    test byte ptr ds:[_kbinput+58], 1
    jne keyboard_irq_map_shifted_key
    mov al, byte ptr keymap1[bx]
keyboard_irq_store_make_state:
    test al, 80h
    jne keyboard_irq_commit_queue_tail
    xor ah, ah
keyboard_irq_translate_key:
; INPUTDOC Queue capacity is kb_intr_data3 bytes (initialized to 2), although backing allocation is 128 bytes.
    mov bx, word ptr ds:[kb_intr_data]
    cli ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
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
; INPUTDOC At capacity, advance read index to new write index: overwrite oldest key, saturate byte count.
    jbe keyboard_irq_advance_queue_head
    mov bx, word ptr ds:[kb_intr_data3]
    mov ax, word ptr ds:[kb_intr_data]
    mov word ptr ds:[kb_intr_data2], ax
keyboard_irq_advance_queue_head:
    mov word ptr ds:[kb_intr_data4], bx
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
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
; -----------------------------------------------------------------------------
; Purpose: Implement the BIOS keyboard read, peek, and modifier-state services.
; ABI: FAR entry; source return form: RETF 2 mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DS; balanced explicit saves/restores observed: BX, DS; path-specific inputs and outputs follow the listed operands.
; Globals read: _kbinput, kb_intr_data2, kb_intr_data3, kb_intr_data4, kb_intr_data_array.
; Globals written: kb_intr_data2, kb_intr_data4.
; /* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_int16_handler (original _kb_int16_handler).
; INPUTDOC Purpose: Provide nonblocking dequeue, peek and held-shift services with live result flags.
; INPUTDOC Convention: Software interrupt entry, not ordinary FAR callable: RETF 2 pops IP/CS and discards FLAGS word
; INPUTDOC without POPF.
; INPUTDOC Inputs: Software INT16 stack entry SP+0 IP, +2 CS, +4 saved FLAGS; AH=00 dequeue,01 peek,02 shift query,
; INPUTDOC otherwise unsupported. Incoming DS arbitrary.
; INPUTDOC Outputs: AH00 AX=queued word or 0; AH01 AX=peek or 0, ZF=0 when queue nonempty and ZF=1 empty; AH02
; INPUTDOC AL=left|right shift (0/1), AH remains02, ZF reflects OR; unsupported AX=0,ZF=1. IF=1. AH00 success ZF
; INPUTDOC follows final queue-count SUB, not character availability.
; INPUTDOC Preserves: BX,DS explicitly restored; CX,DX,SI,DI,BP,ES,SS untouched; stack restored including discarded
; INPUTDOC saved FLAGS.
; INPUTDOC Clobbers: AX, live arithmetic FLAGS,IF; original stacked FLAGS discarded.
; INPUTDOC Globals read: _kbinput[42],_kbinput[54]; kb_intr_data2; kb_intr_data3; kb_intr_data4; kb_intr_data_array
; INPUTDOC Globals written: kb_intr_data2; kb_intr_data4 on successful AH00
; INPUTDOC Services: CLI/STI; RETF 2 discards saved interrupt FLAGS
; INPUTDOC Reachability: Live: kb_init_interrupt:93-94 writes this address to IVT INT16.
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
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    pop ds
    pop bx
; INPUTDOC Discard saved interrupt FLAGS instead of restoring them, exposing service ZF and enabled IF.
    retf 2
keyboard_bios_read_waiting_key:
    cli ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; INPUTDOC AH00 is nonblocking in this replacement: an empty queue returns AX=0 immediately.
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
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    mov bx, word ptr ds:[kb_intr_data2]
    mov ax, word ptr kb_intr_data_array[bx]
    pop ds
    pop bx
    retf 2
kb_bios_read_modifier_flags:
; INPUTDOC Only shift held bytes are ORed into AL; AH stays 02h, not a full BIOS modifier bitmap.
    mov al, byte ptr ds:[_kbinput+42]
    or al, byte ptr ds:[_kbinput+54]
    jmp keyboard_bios_return_common
_kb_int16_handler endp
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by kb get key state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _kbinput.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_get_key_state (original _kb_get_key_state).
; INPUTDOC Purpose: Read and zero-extend one byte from the held-key table without bounds checking.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): byte-table index (intended 0..89); DS=DGROUP. Out-of-range accesses adjacent
; INPUTDOC DGROUP storage.
; INPUTDOC Outputs: AX=zero-extended _kbinput[index].
; INPUTDOC Preserves: Untouched by this body: BP, CX, DI, DS, DX, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,FLAGS
; INPUTDOC Globals read: _kbinput[index]
; INPUTDOC Globals written:
; INPUTDOC Services:
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_kb_get_key_state proc far
    push bp
    mov bp, sp
; INPUTDOC No bounds check protects the 90-byte table; callers must supply a valid scan index.
    mov bx, word ptr [bp + 6]
    mov al, byte ptr _kbinput[bx]
    xor ah, ah
    pop bp
    retf
    db 0
_kb_get_key_state endp
S012_TEXT ends
end
