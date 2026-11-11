; INPUTDOC Module keyboard_input_callbacks (owner keyboard_input_callbacks); comment-only static contract review.
; INPUTDOC Audited INPUTDOC contracts qualify the older retained generic comments; no code or declarations changed.
; INPUTDOC Routine names below follow rename-plan.json; definitions retain their original spelling.
; INPUTDOC dead=true means statically unreferenced; no proof against arbitrary external/numeric callers or execution.
; INPUTDOC Public data: _joystick_enabled
; INPUTDOC Routine _kb_call_readchar_callback -> _kb_call_readchar_callback: Invoke the configured no-argument far
; INPUTDOC character-reader callback.
; INPUTDOC Routine _kb_read_char -> _kb_read_char: Nonblockingly dequeue one BIOS key and discard scan byte for ASCII
; INPUTDOC characters.
; INPUTDOC Routine _kb_checking -> _kb_checking: Peek one BIOS key without dequeue and normalize its ASCII return.
; INPUTDOC Routine _nopsub_kb_set_readchar_callback -> _kb_set_readchar_callback: Replace the configured far
; INPUTDOC character-reader callback pointer.
; INPUTDOC Routine _nopsub_kb_get_readchar_callback -> _kb_get_readchar_callback: Return the configured far
; INPUTDOC character-reader callback pointer.
; INPUTDOC Routine _flush_stdin -> _flush_stdin: Wait until the configured reader returns a nonzero key, then return
; INPUTDOC that key.
; INPUTDOC Routine _kb_check -> _kb_check: Consume all BIOS keys until the queue is empty.
; INPUTDOC Routine _nopsub_30A77 -> _kb_read_until_deadline: Poll the configured reader until a key arrives or the
; INPUTDOC shared timer deadline is reached.
; INPUTDOC Routine _nopsub_30A97 -> _kb_read_with_timeout: Build a local 32-bit deadline from a duration and poll the
; INPUTDOC reader until key or timeout.
.8086
INCLUDE include\platform_hw.inc
name asm012_133660
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
_kb_readchar_callback dw offset _kb_read_char
_readchar_callback_seg dw seg _kb_read_char
_joystick_enabled db 00h
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
extrn _timer_copy_unk:word
extrn _timer_copy_target_high:word
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _joystick_enabled
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_call_readchar_callback (original _kb_call_readchar_callback).
; INPUTDOC Purpose: Invoke the configured no-argument far character-reader callback.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: Callback AX forwarded unchanged (zero means no character by reader convention).
; INPUTDOC Preserves: Only SS:SP balance follows from a normally returning callback; no additional preservation
; INPUTDOC guaranteed by the dispatcher.
; INPUTDOC Clobbers: Callback may change registers/FLAGS and globals: unrestricted transitive contract is unknown;
; INPUTDOC normal FAR return required.
; INPUTDOC Globals read: _kb_readchar_callback; _readchar_callback_seg; callback-dependent globals
; INPUTDOC Globals written: callback-dependent globals
; INPUTDOC Services: Indirect FAR callback initialized to kb_read_char
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_kb_call_readchar_callback proc far
; INPUTDOC No callback register saves or argument setup; ABI preservation is delegated to the target.
    call    dword ptr ds:[_kb_readchar_callback]
    retf
_kb_call_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Read a character and dispatch the configured read-character callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_read_char (original _kb_read_char).
; INPUTDOC Purpose: Nonblockingly dequeue one BIOS key and discard scan byte for ASCII characters.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=0 if absent; AX=zero-extended ASCII when AL!=0; otherwise unchanged extended word.
; INPUTDOC Preserves: With installed INT16 handler: BX,CX,DX,SI,DI,BP,DS,ES,SS; SP balanced. External BIOS may have a
; INPUTDOC different contract.
; INPUTDOC Clobbers: Direct AX, FLAGS; INT16 transitive contract: installed handler preserves all other
; registers, sets
; INPUTDOC IF; external BIOS contract not proven here.
; INPUTDOC Globals read: transitive INT16 queue
; INPUTDOC Globals written: transitive INT16 queue dequeue
; INPUTDOC Services: INT16 AH=01 then AH=00 when present
; INPUTDOC Reachability: Live: initial readchar callback pointer and src/obj_seg008.c:1640.
_kb_read_char proc far

    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short readchar_consume_buffered_key
    xor     ax, ax
    retf
readchar_consume_buffered_key:
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    or      al, al
    jz      short readchar_return_ascii_character
    xor     ah, ah
readchar_return_ascii_character:
    retf
_kb_read_char endp
; -----------------------------------------------------------------------------
; Purpose: Poll keyboard input and update the game input state.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_checking (original _kb_checking).
; INPUTDOC Purpose: Peek one BIOS key without dequeue and normalize its ASCII return.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=0 if absent; zero-extended ASCII or unchanged extended word if present.
; INPUTDOC Preserves: With installed INT16 handler: BX,CX,DX,SI,DI,BP,DS,ES,SS; SP balanced. External BIOS may have a
; INPUTDOC different contract.
; INPUTDOC Clobbers: Direct AX, FLAGS; INT16 transitive contract: installed handler preserves all other
; registers, sets
; INPUTDOC IF; external BIOS contract not proven here.
; INPUTDOC Globals read: transitive INT16 queue
; INPUTDOC Globals written:
; INPUTDOC Services: INT16 AH=01 only
; INPUTDOC Reachability: Live: called by both NumLock wrappers.
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _kb_readchar_callback, _readchar_callback_seg.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_set_readchar_callback (original _nopsub_kb_set_readchar_callback).
; INPUTDOC Purpose: Replace the configured far character-reader callback pointer.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): callback offset; BP+8 (entry SP+6): callback segment. DS=DGROUP.
; INPUTDOC Outputs: AX=callback segment (incidental); no validation or specified return.
; INPUTDOC Preserves: Untouched by this body: BP, BX, CX, DI, DS, DX, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX (FLAGS unchanged)
; INPUTDOC Globals read:
; INPUTDOC Globals written: _kb_readchar_callback; _readchar_callback_seg
; INPUTDOC Services:
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_nopsub_kb_set_readchar_callback proc far
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     ds:[_kb_readchar_callback], ax
    mov     ax, [bp+8]
    mov     ds:[_readchar_callback_seg], ax
    pop     bp
    retf
_nopsub_kb_set_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub kb get readchar callback.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _kb_readchar_callback, _readchar_callback_seg.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_get_readchar_callback (original _nopsub_kb_get_readchar_callback).
; INPUTDOC Purpose: Return the configured far character-reader callback pointer.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: DX:AX=segment:offset of configured callback.
; INPUTDOC Preserves: Untouched by this body: BP, BX, CX, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,DX (FLAGS unchanged)
; INPUTDOC Globals read: _kb_readchar_callback; _readchar_callback_seg
; INPUTDOC Globals written:
; INPUTDOC Services:
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_nopsub_kb_get_readchar_callback proc far
    mov     ax, ds:[_kb_readchar_callback]
    mov     dx, ds:[_readchar_callback_seg]
    retf
_nopsub_kb_get_readchar_callback endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by flush stdin.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _flush_stdin (original _flush_stdin).
; INPUTDOC Purpose: Wait until the configured reader returns a nonzero key, then return that key.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=first nonzero callback result; polling may never terminate.
; INPUTDOC Preserves: Only SS:SP balance follows from a normally returning callback; no additional preservation
; INPUTDOC guaranteed by the dispatcher.
; INPUTDOC Clobbers: Callback may change registers/FLAGS and globals: unrestricted transitive contract is unknown;
; INPUTDOC normal FAR return required.
; INPUTDOC Globals read: _kb_readchar_callback; _readchar_callback_seg; callback-dependent globals
; INPUTDOC Globals written: callback-dependent globals
; INPUTDOC Services: kb_call_readchar_callback repeatedly while AX==0
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_flush_stdin proc far
; INPUTDOC Despite the name, loops while empty and returns on first key; it does not drain input.
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jz      short _flush_stdin
    retf
_flush_stdin endp
; -----------------------------------------------------------------------------
; Purpose: Check the BIOS keyboard buffer for a pending character.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_check (original _kb_check).
; INPUTDOC Purpose: Consume all BIOS keys until the queue is empty.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=0 after draining; no ASCII/extended filtering.
; INPUTDOC Preserves: With installed INT16 handler: BX,CX,DX,SI,DI,BP,DS,ES,SS; SP balanced. External BIOS may have a
; INPUTDOC different contract.
; INPUTDOC Clobbers: Direct AX, FLAGS; INT16 transitive contract: installed handler preserves all other
; registers, sets
; INPUTDOC IF; external BIOS contract not proven here.
; INPUTDOC Globals read: transitive INT16 queue
; INPUTDOC Globals written: transitive INT16 queue dequeue
; INPUTDOC Services: INT16 AH=01/00 repeated
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_kb_check proc far

    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short kb_check_consume_pending_key
    xor     ax, ax
    retf
kb_check_consume_pending_key:
    mov     ah, 0
; INPUTDOC Unconditional retry discards every pending key, including normal ASCII.
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jmp     short near ptr _kb_check
_kb_check endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30A77. Its internal flow uses
; readchar_poll_deadline_expired, readchar_timeout_return.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _timer_copy_target_high, _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_read_until_deadline (original _nopsub_30A77).
; INPUTDOC Purpose: Poll the configured reader until a key arrives or the shared timer deadline is reached.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=nonzero key or zero on unsigned high/low-word deadline expiry; no wrap-safe difference test.
; INPUTDOC Preserves: Only SS:SP balance follows from a normally returning callback; no additional preservation
; INPUTDOC guaranteed by the dispatcher.
; INPUTDOC Clobbers: Direct AX,DX,FLAGS,IF through timer_get_counter; Callback may change registers/FLAGS and globals:
; INPUTDOC unrestricted transitive contract is unknown; normal FAR return required.
; INPUTDOC Globals read: _timer_copy_unk (deadline low); _timer_copy_target_high (deadline high);
; INPUTDOC _kb_readchar_callback; _readchar_callback_seg; transitive _timer_callback_counter; callback-dependent
; INPUTDOC globals
; INPUTDOC Globals written: callback-dependent globals
; INPUTDOC Services: kb_call_readchar_callback; timer_get_counter
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_nopsub_30A77 proc far
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
; INPUTDOC Unsigned high/low comparison against a shared deadline is not timer-wrap-safe.
    jnz     short readchar_timeout_return
    call    far ptr _timer_get_counter
    cmp     dx, ds:[_timer_copy_target_high]
    jb      short _nopsub_30A77
    ja      short readchar_poll_deadline_expired
    cmp     ax, ds:[_timer_copy_unk]
    jb      short _nopsub_30A77
readchar_poll_deadline_expired:
    xor     ax, ax
readchar_timeout_return:
    retf
_nopsub_30A77 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30A97. Its internal flow uses readchar_timeout_poll_loop,
; readchar_timeout_return_zero.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_read_with_timeout (original _nopsub_30A97).
; INPUTDOC Purpose: Build a local 32-bit deadline from a duration and poll the reader until key or timeout.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): duration low word; BP+8 (entry SP+6): duration high word; addition wraps mod
; INPUTDOC 2^32. DS=DGROUP; callback must preserve BP and DS for frame/global accesses.
; INPUTDOC Outputs: AX=nonzero key or zero on unsigned absolute deadline expiry; no wrap-safe difference test.
; INPUTDOC Preserves: BP restored from stack only if callback honors required BP preservation; other state depends on
; INPUTDOC callback. SS:SP balanced under this precondition.
; INPUTDOC Clobbers: Direct AX,DX,FLAGS,IF through timer_get_counter; Callback may change registers/FLAGS and globals:
; INPUTDOC unrestricted transitive contract is unknown; normal FAR return required.
; INPUTDOC Globals read: _kb_readchar_callback; _readchar_callback_seg; transitive _timer_callback_counter;
; INPUTDOC callback-dependent globals
; INPUTDOC Globals written: BP-4/BP-2 local deadline words; callback-dependent globals
; INPUTDOC Services: timer_get_counter; kb_call_readchar_callback
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_nopsub_30A97 proc far
    push    bp
    mov     bp, sp
    sub     sp, 4
; INPUTDOC Duration words occupy BP+6/BP+8; local deadline is BP-4/BP-2.
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
    db 0 ; retained zero fill at this boundary
_nopsub_30A97 endp
S012_TEXT ends
end
