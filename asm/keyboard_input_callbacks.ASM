.8086
INCLUDE include\platform_hw.inc
name asm012_133660
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
_kb_readchar_callback dw offset _kb_read_char
_readchar_callback_seg dw seg _kb_read_char
_byte_3FE00 db 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
extrn _timer_copy_unk:word
extrn _timer_copy_target_high:word
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _byte_3FE00
public _kb_call_readchar_callback
public _kb_read_char
public _kb_checking
public _nopsub_kb_set_readchar_callback
public _nopsub_kb_get_readchar_callback
public _flush_stdin
public _kb_check
public _nopsub_30A77
public _nopsub_30A97
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by kb call readchar callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_kb_call_readchar_callback proc far
    call    dword ptr ds:[_kb_readchar_callback]
    retf
_kb_call_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Read a character and dispatch the configured read-character callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
_kb_read_char proc far

    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short readchar_consume_buffered_key
    xor     ax, ax
    retf
readchar_consume_buffered_key:
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    or      al, al
    jz      short readchar_return_ascii_character
    xor     ah, ah
readchar_return_ascii_character:
    retf
_kb_read_char endp
; -----------------------------------------------------------------------------
; Purpose: Poll keyboard input and update the game input state.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
_kb_checking proc far
    mov ah,1
    int R5HW_BIOS_KEYBOARD_INT ;/* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz have_key
    xor ax,ax
have_key:
    or al,al
    jz done
    xor ah,ah
done:
    ret
_kb_checking endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub kb set readchar callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _kb_readchar_callback, _readchar_callback_seg.
; -----------------------------------------------------------------------------
_nopsub_kb_set_readchar_callback proc far
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     word ptr ds:[_kb_readchar_callback], ax
    mov     ax, [bp+8]
    mov     word ptr ds:[_readchar_callback_seg], ax
    pop     bp
    retf
_nopsub_kb_set_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub kb get readchar callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _kb_readchar_callback, _readchar_callback_seg.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_kb_get_readchar_callback proc far
    mov     ax, word ptr ds:[_kb_readchar_callback]
    mov     dx, word ptr ds:[_readchar_callback_seg]
    retf
_nopsub_kb_get_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by flush stdin.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_flush_stdin proc far
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jz      short _flush_stdin
    retf
_flush_stdin endp
; -----------------------------------------------------------------------------
; Purpose: Check the BIOS keyboard buffer for a pending character.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
_kb_check proc far

    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short kb_check_consume_pending_key
    xor     ax, ax
    retf
kb_check_consume_pending_key:
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jmp     short near ptr _kb_check
_kb_check endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30A77. Its internal flow uses readchar_poll_deadline_expired, readchar_timeout_return.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_copy_target_high, _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_30A77 proc far
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jnz     short readchar_timeout_return
    call    far ptr _timer_get_counter
    cmp     dx, word ptr ds:[_timer_copy_target_high]
    jb      short _nopsub_30A77
    ja      short readchar_poll_deadline_expired
    cmp     ax, word ptr ds:[_timer_copy_unk]
    jb      short _nopsub_30A77
readchar_poll_deadline_expired:
    xor     ax, ax
readchar_timeout_return:
    retf
_nopsub_30A77 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30A97. Its internal flow uses readchar_timeout_poll_loop, readchar_timeout_return_zero.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_30A97 proc far
    push    bp
    mov     bp, sp
    sub     sp, 4
    call    far ptr _timer_get_counter
    add     ax, [bp+6]
    adc     dx, [bp+8]
    mov     [bp-4], ax
    mov     [bp-2], dx
readchar_timeout_poll_loop:
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jnz     short readchar_restore_and_return
    call    far ptr _timer_get_counter
    cmp     dx, [bp-2]
    jb      short readchar_timeout_poll_loop
    ja      short readchar_timeout_return_zero
    cmp     ax, [bp-4]
    jb      short readchar_timeout_poll_loop
readchar_timeout_return_zero:
    xor     ax, ax
readchar_restore_and_return:
    mov     sp, bp
    pop     bp
    retf
    db 0
_nopsub_30A97 endp
S012_TEXT ends
end
