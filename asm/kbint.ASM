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
; Literal translation or hardware policy table; no analytic generator is inferred.
keymap1 db 00h, 1Bh, 31h, 32h, 33h, 34h, 35h, 36h, 37h, 38h, 39h, 30h
    db 2Dh, 3Dh, 08h, 09h, 71h, 77h, 65h, 72h, 74h, 79h, 75h, 69h
    db 6Fh, 70h, 5Bh, 5Dh, 0Dh, 00h, 61h, 73h, 64h, 66h, 67h, 68h
    db 6Ah, 6Bh, 6Ch, 3Bh, 27h, 60h, 00h, 5Ch, 7Ah, 78h, 63h, 76h
    db 62h, 6Eh, 6Dh, 2Ch, 2Eh, 2Fh, 00h, 2Ah, 00h, 20h, 00h, 0BBh
    db 0BCh, 0BDh, 0BEh, 0BFh, 0C0h, 0C1h, 0C2h, 0C3h, 0C4h, 00h, 00h, 0C7h
    db 0C8h, 0C9h, 2Dh, 0CBh, 0CCh, 0CDh, 2Bh, 0CFh, 0D0h, 0D1h, 0D2h, 0D3h
    db 00h, 00h, 00h, 00h, 00h, 00h, 00h
; Literal translation or hardware policy table; no analytic generator is inferred.
keymap2 db 00h, 1Bh, 21h, 40h, 23h, 24h, 25h, 5Eh, 26h, 2Ah, 28h, 29h
    db 5Fh, 2Bh, 08h, 8Fh, 51h, 57h, 45h, 52h, 54h, 59h, 55h, 49h
    db 4Fh, 50h, 7Bh, 7Dh, 0Dh, 00h, 41h, 53h, 44h, 46h, 47h, 48h
    db 4Ah, 4Bh, 4Ch, 3Ah, 22h, 7Eh, 00h, 7Ch, 5Ah, 58h, 43h, 56h
    db 42h, 4Eh, 4Dh, 3Ch, 3Eh, 3Fh, 00h, 00h, 00h, 20h, 00h, 0D4h
    db 0D5h, 0D6h, 0D7h, 0D8h, 0D9h, 0DAh, 0DBh, 0DCh, 0DDh, 00h, 00h, 0C7h
    db 0C8h, 0C9h, 2Dh, 0CBh, 0CCh, 0CDh, 2Bh, 0CFh, 0D0h, 0D1h, 0D2h, 0D3h
    db 00h, 00h, 00h, 00h, 00h, 00h, 00h
; Literal translation or hardware policy table; no analytic generator is inferred.
keymap3 db 00h, 1Bh, 31h, 32h, 33h, 34h, 35h, 36h, 37h, 38h, 39h, 30h
    db 2Dh, 3Dh, 08h, 8Fh, 51h, 57h, 45h, 52h, 54h, 59h, 55h, 49h
    db 4Fh, 50h, 5Bh, 5Dh, 0Dh, 00h, 41h, 53h, 44h, 46h, 47h, 48h
    db 4Ah, 4Bh, 4Ch, 3Bh, 27h, 60h, 00h, 5Ch, 5Ah, 58h, 43h, 56h
    db 42h, 4Eh, 4Dh, 2Ch, 2Eh, 2Fh, 00h, 00h, 00h, 20h, 00h, 0D4h
    db 0D5h, 0D6h, 0D7h, 0D8h, 0D9h, 0DAh, 0DBh, 0DCh, 0DDh, 00h, 00h, 0C7h
    db 0C8h, 0C9h, 2Dh, 0CBh, 0CCh, 0CDh, 2Bh, 0CFh, 0D0h, 0D1h, 0D2h, 0D3h
    db 00h, 00h, 00h, 00h, 00h, 00h, 00h
; Literal translation or hardware policy table; no analytic generator is inferred.
keymap4 db 00h, 1Bh, 21h, 00h, 23h, 24h, 25h, 1Eh, 26h, 2Ah, 28h, 29h
    db 1Fh, 2Bh, 7Fh, 09h, 11h, 17h, 05h, 12h, 14h, 19h, 15h, 09h
    db 0Fh, 10h, 1Bh, 1Dh, 0Dh, 00h, 01h, 13h, 04h, 06h, 07h, 08h
    db 0Ah, 0Bh, 0Ch, 3Bh, 2Ch, 60h, 00h, 1Ch, 1Ah, 18h, 03h, 16h
    db 02h, 0Eh, 0B2h, 3Ch, 3Eh, 3Fh, 00h, 00h, 00h, 20h, 00h, 0DEh
    db 0DFh, 0E0h, 0E1h, 0E2h, 0E3h, 0E4h, 0E5h, 0E6h, 0E7h, 00h, 00h, 0C7h
    db 0C8h, 0C9h, 2Dh, 0CBh, 0CCh, 0CDh, 2Bh, 0CFh, 0D0h, 0D1h, 0D2h, 0D3h
    db 00h, 00h, 00h, 00h, 00h, 00h, 00h
; Literal translation or hardware policy table; no analytic generator is inferred.
keymap5 db 00h, 1Bh, 21h, 40h, 23h, 24h, 25h, 5Eh, 26h, 2Ah, 28h, 29h
    db 5Fh, 2Bh, 08h, 8Fh, 90h, 91h, 92h, 93h, 94h, 95h, 96h, 97h
    db 98h, 99h, 7Bh, 7Dh, 0Dh, 00h, 9Eh, 9Fh, 0A0h, 0A1h, 0A2h, 0A3h
    db 0A4h, 0A5h, 0A6h, 3Ah, 22h, 7Eh, 00h, 7Ch, 0ACh, 0ADh, 0AEh, 0AFh
    db 0B0h, 0B1h, 0B2h, 3Ch, 3Eh, 3Fh, 00h, 00h, 00h, 20h, 00h, 0F8h
    db 0F9h, 0FAh, 0FBh, 0FCh, 0FDh, 0FEh, 0FFh, 80h, 81h, 00h, 00h, 0C7h
    db 0C8h, 0C9h, 2Dh, 0CBh, 0CCh, 0CDh, 2Bh, 0CFh, 0D0h, 0D1h, 0D2h, 0D3h
    db 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
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
extrn _addexit:far
public _kbinput
public _kb_init_interrupt
public _keyboard_exit_handler
public _kb_int9_handler
public _kb_int16_handler
public _keystate
; -----------------------------------------------------------------------------
; Purpose: Install keyboard IRQ and BIOS keyboard handlers and register their exit cleanup.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DI, SP, CS, DS, ES; balanced explicit saves/restores observed: DI;
; path-specific inputs and outputs follow the listed operands.
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
; INPUTDOC Reachability: Live: src/startup.c:154; installs two public interrupt entries and saves a public exit
; INPUTDOC callback.
_kb_init_interrupt proc far
    push di
    in al, R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ah, al
    or al, 03h
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    xor bx, bx
    mov es, bx
    mov bx, es:[24h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; INPUTDOC Installation detection compares only INT09 offset, not its segment; common setup still runs.
    cmp bx, offset _kb_int9_handler
    je keyboard_init_finish_setup
    mov ds:[old_kb_intr_ofs], bx
    mov bx, es:[26h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov ds:[old_kb_intr_seg], bx
    mov word ptr es:[24h], offset _kb_int9_handler
; /* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov word ptr es:[26h], cs ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, es:[58h] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov ds:[old_kb_intr_bios_ofs], bx
    mov bx, es:[5Ah] ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov ds:[old_kb_intr_bios_seg], bx
    mov word ptr es:[58h], offset _kb_int16_handler
; /* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
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
    call far ptr _addexit
    add sp, 4
    pop di
    retf
_kb_init_interrupt endp
; -----------------------------------------------------------------------------
; Purpose: Restore the prior keyboard interrupt vectors and keyboard controller state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches BX, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
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
; INPUTDOC Reachability: Live: registered in kb_init_interrupt:105-108 and called in src/frontend.c:664.
_keyboard_exit_handler proc far
    in al, R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    mov ah, al
    or al, 03h
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    xor bx, bx
    mov es, bx
    mov bx, ds:[old_kb_intr_ofs]
    or bx, bx
    je keyboard_exit_restore_flags
    mov es:[24h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, ds:[old_kb_intr_seg]
    mov es:[26h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, ds:[old_kb_intr_bios_ofs]
    mov es:[58h], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
    mov bx, ds:[old_kb_intr_bios_seg]
    mov es:[5Ah], bx ;/* PLATFORM(input_kb): reads or updates the keyboard interrupt vector table. */
; INPUTDOC AND F0h preserves lock-toggle high nibble and clears low held-modifier nibble.
    mov al, es:[417h]
    and al, 0F0h
    mov es:[417h], al
keyboard_exit_restore_flags:
    mov al, ah
    out R5HW_PIC_MASTER_MASK_PORT, al ;/* PLATFORM(input_kb): accesses the PIC or keyboard controller port. */
    retf
_keyboard_exit_handler endp
; -----------------------------------------------------------------------------
; Purpose: Handle keyboard scan-code IRQs and enqueue translated key events.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, DS; balanced explicit saves/restores observed: AX, BX, CX, DS, DX;
; path-specific inputs and outputs follow the listed operands.
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
    mov ds:[kblastinput], bx
    mov byte ptr _kbinput[bx], 1
    test byte ptr ds:[_kbinput+56], 01h
    jne keyboard_irq_map_altgr_state
    test byte ptr ds:[_kbinput+29], 01h
    jne keyboard_irq_map_shift_state
    test byte ptr ds:[_kbinput+42], 01h
    jne keyboard_irq_normalize_extended
    test byte ptr ds:[_kbinput+54], 01h
    jne keyboard_irq_normalize_extended
; INPUTDOC Caps selection tests held scan 3Ah, not a toggled BDA CapsLock bit.
    test byte ptr ds:[_kbinput+58], 01h
    jne keyboard_irq_map_shifted_key
    mov al, keymap1[bx]
keyboard_irq_store_make_state:
    test al, 80h
    jne keyboard_irq_commit_queue_tail
    xor ah, ah
keyboard_irq_translate_key:
; INPUTDOC Queue capacity is kb_intr_data3 bytes (initialized to 2), although backing allocation is 128 bytes.
    mov bx, ds:[kb_intr_data]
    cli ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    mov  word ptr kb_intr_data_array[bx], ax
    add bx, 2
    cmp bx, ds:[kb_intr_data3]
    jb keyboard_irq_enqueue_key_event
    xor bx, bx
keyboard_irq_enqueue_key_event:
    mov ds:[kb_intr_data], bx
    mov bx, ds:[kb_intr_data4]
    add bx, 2
    cmp bx, ds:[kb_intr_data3]
; INPUTDOC At capacity, advance read index to new write index: overwrite oldest key, saturate byte count.
    jbe keyboard_irq_advance_queue_head
    mov bx, ds:[kb_intr_data3]
    mov ax, ds:[kb_intr_data]
    mov ds:[kb_intr_data2], ax
keyboard_irq_advance_queue_head:
    mov ds:[kb_intr_data4], bx
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
    mov al, keymap2[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shift_state:
    mov al, keymap4[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_altgr_state:
    mov al, keymap5[bx]
    jmp keyboard_irq_store_make_state
keyboard_irq_map_shifted_key:
    mov al, keymap3[bx]
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
; ABI: FAR entry; source return form: RETF 2 mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DS; balanced explicit saves/restores observed: BX, DS; path-specific inputs and
; outputs follow the listed operands.
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
    mov bx, ds:[kb_intr_data2]
    mov ax, word ptr kb_intr_data_array[bx]
    add bx, 2
    cmp bx, ds:[kb_intr_data3]
    jb kb_bios_advance_read_index
    xor bx, bx
kb_bios_advance_read_index:
    mov ds:[kb_intr_data2], bx
    mov bx, ds:[kb_intr_data4]
    sub bx, 2
    mov ds:[kb_intr_data4], bx
    jmp keyboard_bios_return_common
keyboard_bios_peek_key:
    cmp word ptr ds:[kb_intr_data4], 0
    je keyboard_bios_unknown_function
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    mov bx, ds:[kb_intr_data2]
    mov ax, word ptr kb_intr_data_array[bx]
    pop ds
    pop bx
    retf 2
kb_bios_read_modifier_flags:
; INPUTDOC Only shift held bytes are ORed into AL; AH stays 02h, not a full BIOS modifier bitmap.
    mov al, ds:[_kbinput+42]
    or al, ds:[_kbinput+54]
    jmp keyboard_bios_return_common
_kb_int16_handler endp
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by kb get key state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
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
_keystate proc far
    push bp
    mov bp, sp
; INPUTDOC No bounds check protects the 90-byte table; callers must supply a valid scan index.
    mov bx, [bp + 6]
    mov al, _kbinput[bx]
    xor ah, ah
    pop bp
    retf
    db 0 ; retained zero fill at this boundary
_keystate endp
S012_TEXT ends
end
